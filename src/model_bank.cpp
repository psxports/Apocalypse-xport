#include <string.h>
#include <stdlib.h>
#include <new>

#include "model_bank_grid.h"
#include "model_instance.h"
#include "game_diagnostics.h"
#include "model_bank.h"
#include "hd.h"
#include "model_clipping.h"
#include "global.h"
#include "resource_io.h"
#include "game_heap.h"
#include "palette_resource.h"
#include "effect_lifecycle.h"
#include "object_damage.h"
#include "hud_text.h"
#include "game_panel.h"
#include "resource_file_layouts.h"
#include "effect_object.h"
#include "texture_resources.h"
#include "psx.h"
#include "scene_lifecycle.h"
#include "level_session.h"
#include "game_tick.h"
#include "game_lighting.h"
#include "resource_script.h"
#include "model_render_view.h"
#include "character.h"
#include "model_render.h"
#include "model_matrix.h"
#include "model_subdivision.h"
#include "model_lighting.h"
#include "model_projection.h"
#include "model_animation.h"
#include "angle_math.h"

struct ModelSceneFile
{
    uint16 flags;
    uint16 generation;
    sint32 pos[3];
    sint16 angles[3];
    uint16 model;
    uint8 frame;
    uint8 flash;
    uint8 reserved[6];
    uint32 color;
};

typedef char ModelSceneFileSize[sizeof(ModelSceneFile) == 36 ? 1 : -1];

struct ModelPalette16File
{
    uint32 key;
    uint16 colors[16];
};

typedef char ModelPalette16FileSize[sizeof(ModelPalette16File) == 36 ? 1 : -1];

struct ModelPalette256File
{
    uint32 key;
    uint16 colors[256];
};

typedef char ModelPalette256FileSize[sizeof(ModelPalette256File) == 516 ? 1 : -1];

struct ModelTextureFile
{
    uint32 flags;
    uint32 format;
    uint32 palette;
    uint32 reference;
    uint16 width;
    uint16 height;
};

typedef char ModelTextureFileSize[sizeof(ModelTextureFile) == 20 ? 1 : -1];

void InvalidateTexBindings(const TextureResource *tex)
{
    EffectObject::ForgetTex(tex);
    ForgetHudTex(tex);
}

void InvalidateTexBindings(const TextureGroup *grp)
{
    EffectObject::ForgetTex(grp);
    ForgetHudTex(grp);
    ForgetPanelTex(grp);
}

static ModelBank model_banks[40];
uint8 items_bank;

struct RetiredModelMemory
{
    void *memory;
    uint32 count;
    void (*release)(void *memory, uint32 count);
    RetiredModelMemory *next;
};

static uint32 model_borrows;
static RetiredModelMemory *retired_model_memory;
static RetiredModelMemory *retired_model_memory_tail;

void BeginModelBorrow()
{
    ++model_borrows;
}

void EndModelBorrow()
{
    --model_borrows;
    if (model_borrows)
        return;
    while (retired_model_memory)
    {
        RetiredModelMemory *entry = retired_model_memory;
        retired_model_memory = entry->next;
        entry->release(entry->memory, entry->count);
        free(entry);
    }
    retired_model_memory_tail = 0;
}

static void RetireModelMemory(void *memory, uint32 count, void (*release)(void *, uint32))
{
    if (!memory)
        return;
    if (!model_borrows)
    {
        release(memory, count);
        return;
    }
    RetiredModelMemory *entry = (RetiredModelMemory *)malloc(sizeof(RetiredModelMemory));
    if (!entry)
    {
        GameDiagnostic(("Cannot retain borrowed model storage\n"));
        abort();
    }
    entry->memory = memory;
    entry->count = count;
    entry->release = release;
    entry->next = 0;
    if (retired_model_memory_tail)
        retired_model_memory_tail->next = entry;
    else
        retired_model_memory = entry;
    retired_model_memory_tail = entry;
}

static void FreeModelMemory(void *memory, uint32)
{
    free(memory);
}

void RetireModelMemory(void *memory)
{
    RetireModelMemory(memory, 0, FreeModelMemory);
}

static void DestroyModelInstances(void *memory, uint32 count)
{
    ModelInstance *objects = (ModelInstance *)memory;
    for (uint32 i = 0; i < count; ++i)
        objects[i].~ModelInstance();
    free(objects);
}

static void ReleaseModelSceneObjects(ModelSceneObjects &scene)
{
    ModelInstance::ForgetFlashes(scene.objects, scene.count);
    for (uint32 i = 0; i < scene.count; ++i)
        scene.objects[i].InvalidateReferences();
    RetireModelMemory(scene.objects, scene.count, DestroyModelInstances);
    scene.objects = 0;
    scene.count = 0;
}

static bool DecodeModelSceneObjects(ModelSceneObjects &scene, const uint8 *data, uint32 bytes, uint32 bank, const ModelGeometrySet &geometry)
{
    ModelFileLayout layout;
    if (bank >= 40 || !DecodeModelFileLayout(layout, data, bytes) || geometry.count != layout.model_count)
        return false;
    if (layout.scene_count > 0xFFFFFFFF / sizeof(ModelInstance))
        return false;
    ModelSceneObjects decoded = {layout.scene_count, 0};
    if (decoded.count)
    {
        decoded.objects = (ModelInstance *)malloc(decoded.count * sizeof(ModelInstance));
        if (!decoded.objects)
            return false;
    }
    for (uint32 i = 0; i < decoded.count; ++i)
    {
        const ModelSceneFile &file = ((const ModelSceneFile *)(data + 12))[i];
        uint16 model_idx = file.model;
        if (!geometry.models || model_idx >= geometry.count)
        {
            for (uint32 initialized = 0; initialized < i; ++initialized)
                decoded.objects[initialized].~ModelInstance();
            free(decoded.objects);
            return false;
        }
        ModelInstance &object = *new (decoded.objects + i) ModelInstance;
        object.flags = file.flags;
        if (geometry.models[model_idx].flags & 16)
            object.flags |= 32;
        object.generation = file.generation;
        for (uint32 axis = 0; axis < 3; ++axis)
        {
            object.pos.comp[axis] = file.pos[axis];
            object.angles.comp[axis] = file.angles[axis];
        }
        object.model_idx = model_idx;
        object.frame = file.frame;
        object.flash = file.flash;
        object.animation = 0;
        object.bank = bank;
        object.color = file.color;
        // Serialized links are rebuilt within the validated scene array
        object.next = i + 1 < decoded.count ? decoded.objects + i + 1 : 0;
    }
    ReleaseModelSceneObjects(scene);
    scene = decoded;
    return true;
}

struct ActiveModelState
{
    uint32 banks[2];
    uint32 replacement_slot;
    ModelInstance *scene_objects;
    const ModelKeys *script_keys[2];
};

static ActiveModelState active_model_state = {{0xFFFFFFFF, 0xFFFFFFFF}, 0, 0, {0, 0}};

void ModelBank::ResetActive()
{
    memset(&active_model_state, 0, sizeof(active_model_state));
    active_model_state.banks[0] = 0xFFFFFFFF;
    active_model_state.banks[1] = 0xFFFFFFFF;
}

static void CheckActiveModelSlot(uint32 slot)
{
    if (slot >= 2)
    {
        GameDiagnostic(("Invalid active model slot %u\n", slot));
        abort();
    }
}

uint32 ModelBank::GetActive(uint32 slot)
{
    CheckActiveModelSlot(slot);
    return active_model_state.banks[slot];
}

const ModelKeys &ModelBank::GetActiveScriptKeys(uint32 slot)
{
    CheckActiveModelSlot(slot);
    if (!active_model_state.script_keys[slot])
    {
        GameDiagnostic(("Missing active model script keys\n"));
        abort();
    }
    return *active_model_state.script_keys[slot];
}

ModelInstance *ModelBank::GetSceneObjects()
{
    return active_model_state.scene_objects;
}

void ModelBank::SelectReplacement(uint32 slot)
{
    CheckActiveModelSlot(slot);
    active_model_state.replacement_slot = slot;
}

void ModelBank::ReleaseHdCaches()
{
    for (uint32 i = 0; i < 40; ++i)
    {
        hd_bank_free(model_banks[i].geometry.hd);
        model_banks[i].geometry.hd = 0;
    }
}

HD_BANK *ModelBank::GetHdCache(uint32 bank)
{
    ModelBank &owner = Get(bank);
    bool initial = !owner.geometry.hd;
    HD_BANK *cache = owner.geometry.GetHdCache();
    if (initial)
        owner.geometry.CaptureHdMaterials(owner.colors);
    return cache;
}

void ModelBank::CaptureHdMaterials()
{
    for (uint32 i = 0; i < 40; ++i)
    {
        ModelBank &owner = model_banks[i];
        if (owner.loaded && owner.geometry.hd)
            owner.geometry.CaptureHdMaterials(owner.colors);
    }
}

void ModelBank::ResetAll()
{
    ReleaseModelPolygonQueue();
    ResetModelPalSelection();
    for (uint32 i = 0; i < 40; ++i)
    {
        ReleaseModelSceneObjects(model_banks[i].scene);
        free(model_banks[i].data);
        ReleaseModelGeometrySet(model_banks[i].geometry);
        TextureGroupBlock::ReleaseAll(model_banks[i].grps);
        ReleaseTexReferences(model_banks[i].textures);
        ReleaseModelKeys(model_banks[i].keys);
        ReleaseModelAnimations(model_banks[i].animations);
        ReleaseModelPal(model_banks[i].colors);
        ReleaseModelColorAnimation(model_banks[i].color_animation);
        ReleaseModelTexAnimation(model_banks[i].tex_animation);
    }
    memset(model_banks, 0, sizeof(model_banks));
    items_bank = 0;
    TextureResource::ResetAll();
    PaletteResource::ResetAll();
}

ModelBank &ModelBank::Get(uint32 idx)
{
    if (idx >= 40)
    {
        GameDiagnostic(("Invalid model bank %u\n", idx));
        abort();
    }
    return model_banks[idx];
}

ModelGeometry &ModelBank::GetGeometry(uint32 bank, uint32 idx)
{
    ModelGeometrySet &geometry = ModelBank::Get(bank).geometry;
    if (idx >= geometry.count)
    {
        GameDiagnostic(("Invalid model index %u in bank %u\n", idx, bank));
        abort();
    }
    return geometry.models[idx];
}

static void WriteModelBankName(ModelBank &bank, const char *name, char terminator)
{
    uint32 idx = 0;
    while (name[idx] != terminator)
    {
        if (!name[idx] || idx >= sizeof(bank.name) - 1)
        {
            GameDiagnostic(("Invalid model bank name %s\n", name));
            abort();
        }
        bank.name[idx] = name[idx];
        ++idx;
    }
    bank.name[idx] = 0;
}

struct ModelLoadRequest
{
    char name[13];
    uint8 bank;
    uint8 active;
    bool pending;
};

struct ModelLoadQueue
{
    ModelLoadRequest requests[20];
    uint32 read_idx;
    uint32 write_idx;
    uint32 phase;
};

static ModelLoadQueue model_load_queue;

void ModelBank::ResetLoadQueue()
{
    memset(&model_load_queue, 0, sizeof(model_load_queue));
}

uint32 ModelBank::GetLoadPending()
{
    return model_load_queue.requests[model_load_queue.read_idx].pending;
}

uint32 ModelBank::Initialize()
{
    FUNCTION_MARKER(0x8006DFA0, "SLUS_003.73");
    for (uint32 item = 0; item < 20; ++item)
        model_load_queue.requests[item].pending = false;
    model_load_queue.read_idx = 0;
    model_load_queue.write_idx = 0;
    for (uint32 item = 0; item < 40; ++item)
    {
        ModelBank &bank = model_banks[item];
        TextureGroupBlock::ReleaseAll(bank.grps);
        ReleaseTexReferences(bank.textures);
        bank.name[0] = 0;
        bank.protected_bank = false;
        bank.loaded = false;
        ReleaseModelSceneObjects(bank.scene);
        ReleaseModelGeometrySet(bank.geometry);
        free(bank.data);
        bank.data = 0;
        bank.src_bytes = 0;
        ReleaseModelAnimations(bank.animations);
        ReleaseModelColorAnimation(bank.color_animation);
        ReleaseModelTexAnimation(bank.tex_animation);
        bank.attachments = 0;
    }
    TextureResource::ResetAll();
    PaletteResource::ResetAll();
    active_model_state.banks[0] = 0xFFFFFFFF;
    active_model_state.banks[1] = 0xFFFFFFFF;
    active_model_state.scene_objects = 0;
    active_model_state.replacement_slot = 0;
    return 0xFFFFFFFF;
}

uint32 ModelBank::FindResourceIdx(uint32 key, uint32 bank)
{
    FUNCTION_MARKER(0x8006E080, "SLUS_003.73");
    const ModelBank &src = ModelBank::Get(bank);
    if (!src.keys.count)
    {
        GameDiagnostic(("Model lookup in an empty bank %u\n", bank));
        abort();
    }
    for (uint32 idx = 0; idx < src.keys.count; ++idx)
        if (ReadModelKey(src.keys, idx) == key)
            return idx;
    return 0;
}

ModelInstance *ModelBank::FindObject(uint32 key)
{
    FUNCTION_MARKER(0x8006E0D0, "SLUS_003.73");
    ModelInstance *object = active_model_state.scene_objects;
    uint32 prev_bank = 0xFFFFFFFF;
    const ModelKeys *keys = 0;
    while (object)
    {
        if (object->bank != prev_bank)
        {
            prev_bank = object->bank;
            keys = &ModelBank::Get(prev_bank).keys;
        }
        if (ReadModelKey(*keys, object->model_idx) == key)
            return object;
        object = object->next;
    }
    return 0;
}

void ModelBank::DecResourceReferences(uint32 bank)
{
    FUNCTION_MARKER(0x8006E394, "SLUS_003.73");
    ReleaseTexReferences(ModelBank::Get(bank).textures);
}

static void CheckModelSrcSpan(const ModelBank &bank, uint32 offset, uint32 count, uint32 stride)
{
    if (offset > bank.src_bytes || count > (bank.src_bytes - offset) / stride)
    {
        GameDiagnostic(("Truncated model texture/palette data\n"));
        abort();
    }
}

uint32 ModelBank::LoadResourceData(uint32 bank)
{
    uint32 model_count, reference_count;
    uint32 cursor, resource;
    TextureResource *tex;
    PaletteResource *pal;
    uint32 retained_size, count, tex_count, texs, flags, format, pal_key;
    uint16 width, height;
    uint32 key, rounded_width, depth, x, y, u, right, bottom, mask;
    uint32 tex_xy[2];
    PSX_RECT img_rect;
    uint32 block_kind, block_size, block_offset;
    FUNCTION_MARKER(0x8006E4A0, "SLUS_003.73");
    ModelBank &owner = ModelBank::Get(bank);
    uint8 *src = owner.data;
    const ModelFileLayout layout = LoadModelKeys(owner.keys, src, owner.src_bytes);
    model_count = layout.model_count;
    const uint32 refs_offset = layout.keys_offset + model_count * 4;
    CheckModelSrcSpan(owner, refs_offset, 1, 4);
    LoadTexReferences(owner.textures, src + refs_offset, owner.src_bytes - refs_offset);
    reference_count = owner.textures.count;
    TextureResource::ReleaseUnused();
    PaletteResource::ReleaseUnused();
    retained_size = layout.keys_offset + (model_count << 2) + (reference_count << 2) + 4;
    cursor = retained_size;
    CheckModelSrcSpan(owner, cursor, 1, 4);
    count = *(const uint32 *)(src + cursor);
    cursor += 4;
    CheckModelSrcSpan(owner, cursor, count, 36);
    for (uint32 idx = 0; idx < count; ++idx, cursor += 36)
    {
        ModelPalette16File &palette = *(ModelPalette16File *)(src + cursor);
        if (!PaletteResource::Find(palette.key))
            PaletteResource::Load(palette.key, palette.colors, 1);
    }
    CheckModelSrcSpan(owner, cursor, 1, 4);
    count = *(const uint32 *)(src + cursor);
    cursor += 4;
    CheckModelSrcSpan(owner, cursor, count, 516);
    for (uint32 idx = 0; idx < count; ++idx, cursor += 516)
    {
        ModelPalette256File &palette = *(ModelPalette256File *)(src + cursor);
        if (!PaletteResource::Find(palette.key))
            PaletteResource::Load(palette.key, palette.colors, 2);
    }
    SetModelVramAllocationMode(false);
    if (owner.name[0] == 'b' && owner.name[1] == 'i' && owner.name[2] == 't' && owner.name[3] == 's' && !owner.name[4])
        SetModelVramAllocationMode(true);
    CheckModelSrcSpan(owner, cursor, 1, 4);
    tex_count = *(const uint32 *)(src + cursor);
    texs = cursor + 4;
    CheckModelSrcSpan(owner, texs, tex_count, 4);
    const uint32 *tex_offsets = (const uint32 *)(src + texs);
    for (uint32 idx = 0; idx < tex_count; ++idx)
    {
        const uint32 tex_offset = tex_offsets[idx];
        CheckModelSrcSpan(owner, tex_offset, 1, 20);
        const uint8 *tex_data = src + tex_offset;
        const ModelTextureFile &file = *(const ModelTextureFile *)tex_data;
        resource = file.reference;
        flags = file.flags;
        format = file.format;
        pal_key = file.palette;
        tex = &ReadTexReference(owner.textures, resource);
        if (tex->loaded)
            continue;
        key = tex->key;
        if (key == 0xCCE0187C || key == 0x68B7B136)
            flags |= 1;
        width = file.width;
        height = file.height;
        const uint8 *pixels = tex_data + 20;
        if (format == 256 || format == 16)
        {
            if (!width || !height)
            {
                GameDiagnostic(("Empty model texture dimensions\n"));
                abort();
            }
            rounded_width = format == 256 ? (width + 1) & 0xFFFFFFFE : (width + 3) & 0xFFFFFFFC;
            depth = format == 256 ? 8 : 4;
            if (!AllocateModelTexRect(0, rounded_width, height, tex_xy, &tex_xy[1], depth, flags & 1 ? 2 : 0, tex->key))
            {
                GameDiagnostic(("Model texture VRAM allocation failed for %08X\n", key));
                abort();
            }
            x = tex_xy[0];
            y = tex_xy[1];
            tex->x = x;
            tex->y = y;
            img_rect.x = x;
            img_rect.y = y;
            img_rect.w = format == 256 ? rounded_width >> 1 : rounded_width >> 2;
            img_rect.h = height;
            CheckModelSrcSpan(owner, tex_offset + 20, height, img_rect.w * 2);
            LoadImagePSX(&img_rect, (uint32 *)pixels);
        }
        pal = PaletteResource::Find(pal_key);
        if (!pal)
        {
            GameDiagnostic(("Missing texture palette %08X\n", pal_key));
            abort();
        }
        if (format != 256 && format != 16)
        {
            // TODO Original coordinates are undefined for unsupported texture formats
            GameDiagnostic(("Unsupported model texture format %u at offset %u\n", format, tex_offset));
            abort();
        }
        x = tex_xy[0];
        y = tex_xy[1];
        ++pal->refs;
        tex->pal = pal;
        tex->clut = pal->clut;
        tex->tpage = GetTPage(format == 256, 0, x, y);
        u = tex_xy[0] & 63;
        u <<= format == 256 ? 1 : 2;
        tex->uv[0].u = u;
        right = width + u - 1;
        y = tex_xy[1] & 255;
        tex->loaded = true;
        tex->uv[1].u = right;
        tex->uv[3].u = right;
        tex->uv[0].v = y;
        tex->region.x = u;
        tex->region.y = y;
        tex->region.width = width;
        tex->region.height = height;
        bottom = height + y - 1;
        x = tex->uv[0].u;
        tex->uv[2].v = bottom;
        u = tex->uv[0].u;
        tex->uv[1].v = y;
        tex->uv[2].u = x;
        mask = ((y & 248) << 12) | ((u & 248) << 7) | 0xE2000000;
        mask |= (-height & 248) << 2;
        mask |= (-width & 248) >> 3;
        bottom = tex->uv[2].v;
        tex->window = mask;
        tex->uv[3].v = bottom;
    }
    SetModelVramAllocationMode(false);
    ModelBank::PrepareGeometry(bank);
    ReleaseModelTexAnimation(owner.tex_animation);
    ReleaseModelColorAnimation(owner.color_animation);
    block_offset = layout.blocks_offset;
    for (;;)
    {
        block_kind = *(const uint32 *)(src + block_offset);
        if (block_kind == 0xFFFFFFFF)
            break;
        block_size = ((const ResourceBlockFile *)(src + block_offset))->bytes;
        const uint8 *block_data = src + block_offset + 8;
        switch (block_kind)
        {
            case 6:
                LoadModelTexAnimation(owner.tex_animation, block_data, block_size, layout.scene_count);
                break;
            case 7:
                LoadModelColorAnimation(owner.color_animation, block_data, block_size);
                break;
            case 10:
                resource = ModelBank::FindActiveSlot(bank);
                ModelBankGrid::Initialize(resource, block_data, block_size, owner.scene.objects, owner.scene.count);
                break;
            case 42:
                LoadModelAnimations(owner.animations, block_data, block_size, model_count);
                break;
            case 69:
                TextureGroupBlock::Register(owner.grps, owner.textures, block_data, block_size);
                break;
            case 0x73424752:
                LoadModelPal(owner.colors, block_data, block_size);
                break;
            default:
                break;
        }
        block_offset += 8 + block_size;
    }
    ValidateModelColorAnimation(owner.colors, owner.color_animation);
    owner.loaded = true;
    free(owner.data);
    owner.data = 0;
    owner.src_bytes = 0;
    return 1;
}

void ModelBank::AttachObjects(uint32 bank)
{
    FUNCTION_MARKER(0x8006EBF4, "SLUS_003.73");
    ModelInstance *head = ModelBank::Get(bank).scene.objects;
    if (!head)
        return;
    ModelInstance *tail = head;
    while (tail->next)
        tail = tail->next;
    tail->next = active_model_state.scene_objects;
    active_model_state.scene_objects = head;
}

void ModelBank::DetachObjects(uint32 slot)
{
    FUNCTION_MARKER(0x8006ECAC, "SLUS_003.73");
    CheckActiveModelSlot(slot);
    ModelInstance *target = ModelBank::Get(active_model_state.banks[slot]).scene.objects;
    uint32 other_bank = active_model_state.banks[slot ^ 1];
    ModelInstance *remaining = other_bank == 0xFFFFFFFF ? 0 : ModelBank::Get(other_bank).scene.objects;
    if (!target)
        return;
    if (!remaining)
    {
        active_model_state.scene_objects = 0;
        return;
    }
    if (target == active_model_state.scene_objects)
    {
        active_model_state.scene_objects = remaining;
        return;
    }
    while (remaining->next != target)
        remaining = remaining->next;
    remaining->next = 0;
}

uint32 ModelBank::FindByName(const char *name)
{
    FUNCTION_MARKER(0x8006ED70, "SLUS_003.73");
    for (uint32 bank = 0; bank < 40; ++bank)
        if (NamesEqualIgnoringCase(name, ModelBank::Get(bank).name))
            return bank;
    return 0xFFFFFFFF;
}

void ModelBank::UnloadByName(const char *name)
{
    uint32 bank;
    FUNCTION_MARKER(0x8006EDD4, "SLUS_003.73");
    bank = ModelBank::FindByName(name);
    if (bank != 0xFFFFFFFF && ModelBank::Get(bank).loaded)
    {
        bank = ModelBank::FindByName(name);
        ModelBank::Unload(bank, true);
    }
}

uint32 ModelBank::UnloadUnprotected()
{
    FUNCTION_MARKER(0x8006EE2C, "SLUS_003.73");
    ModelBank::WaitForLoads();
    ModelBank::UnloadActive();
    for (uint32 bank = 0; bank < 40; ++bank)
        if (!ModelBank::Get(bank).protected_bank)
            ModelBank::Unload(bank, true);
    TextureResource::ReleaseUnused();
    return 0;
}

void ModelBank::Unload(uint32 bank, bool release_unused)
{
    uint32 slot;
    FUNCTION_MARKER(0x8006EE94, "SLUS_003.73");
    if (bank == 0xFFFFFFFF)
        return;
    if (!ModelBank::Get(bank).name[0])
        return;
    TextureGroupBlock::ReleaseAll(ModelBank::Get(bank).grps);
    ModelBank::DecResourceReferences(bank);
    ReleaseModelAnimations(ModelBank::Get(bank).animations);
    ReleaseModelColorAnimation(ModelBank::Get(bank).color_animation);
    ReleaseModelTexAnimation(ModelBank::Get(bank).tex_animation);
    ModelBank::Get(bank).protected_bank = false;
    ModelBank::Get(bank).loaded = false;
    ModelBank::Get(bank).attachments = 0;
    ReleaseModelGeometrySet(ModelBank::Get(bank).geometry);
    ModelBank::Get(bank).name[0] = 0;
    slot = ModelBank::FindActiveSlot(bank);
    if (slot != 0xFFFFFFFF)
    {
        ModelBankGrid::Clear(slot);
        ModelBank::DetachObjects(slot);
        active_model_state.banks[slot] = 0xFFFFFFFF;
    }
    ReleaseModelSceneObjects(ModelBank::Get(bank).scene);
    free(ModelBank::Get(bank).data);
    ModelBank::Get(bank).data = 0;
    ModelBank::Get(bank).src_bytes = 0;
    if (release_unused)
        TextureResource::ReleaseUnused();
}

uint32 ModelBank::UnloadInactive()
{
    FUNCTION_MARKER(0x8006EF74, "SLUS_003.73");
    for (uint32 bank = 0; bank < 40; ++bank)
        if ((ModelBank::Get(bank).data || ModelBank::Get(bank).loaded) && bank != active_model_state.banks[0] && bank != active_model_state.banks[1] && !ModelBank::Get(bank).protected_bank)
            ModelBank::Unload(bank, false);
    TextureResource::ReleaseUnused();
    return 0;
}

uint32 ModelBank::UnloadActive()
{
    FUNCTION_MARKER(0x8006F004, "SLUS_003.73");
    ModelBank::Unload(active_model_state.banks[0], false);
    ModelBank::Unload(active_model_state.banks[1], false);
    active_model_state.banks[0] = 0xFFFFFFFF;
    active_model_state.banks[1] = 0xFFFFFFFF;
    TextureResource::ReleaseUnused();
    active_model_state.replacement_slot = 0;
    return 0;
}

uint32 ModelBank::WriteLoadFilename(const char *name, const char *suffix)
{
    FUNCTION_MARKER(0x8006F238, "SLUS_003.73");
    ModelLoadRequest *request = model_load_queue.requests + model_load_queue.write_idx;
    uint32 len = 0;
    while (*name)
    {
        if (len >= sizeof(request->name) - 1)
            abort();
        request->name[len++] = *name++;
    }
    while (*suffix)
    {
        if (len >= sizeof(request->name) - 1)
            abort();
        request->name[len++] = *suffix++;
    }
    request->name[len] = 0;
    return 0;
}

uint32 ModelBank::QueueLoad(const char *name, bool active)
{
    uint8 left, right;
    uint32 idx, bank = 0xFFFFFFFF;
    const char *src;
    bool matches;
    FUNCTION_MARKER(0x8006F29C, "SLUS_003.73");
    for (idx = 0; idx < 40; ++idx)
        if (ModelBank::Get(idx).name[0] && NamesEqualIgnoringCase(name, ModelBank::Get(idx).name))
            return idx;
    for (idx = 0; idx < 20; ++idx)
    {
        const ModelLoadRequest *request = model_load_queue.requests + idx;
        if (!request->pending)
            continue;
        src = name;
        const char *queued_name = request->name;
        matches = true;
        if (*name)
        {
            for (;;)
            {
                left = *src;
                right = *queued_name;
                ++src;
                if (left != right)
                {
                    matches = false;
                    break;
                }
                left = *src;
                ++queued_name;
                if (!left)
                    break;
            }
        }
        if (matches && *queued_name == '.')
            return request->bank;
    }
    for (idx = 0; idx < 40; ++idx)
        if (!ModelBank::Get(idx).name[0])
        {
            bank = idx;
            break;
        }
    if (bank == 0xFFFFFFFF && (!active || active_model_state.banks[0] == 0xFFFFFFFF || active_model_state.banks[1] == 0xFFFFFFFF))
    {
        GameDiagnostic(("No free model bank for %s\n", name));
        abort();
    }
    if (!active)
        WriteModelBankName(ModelBank::Get(bank), name, 0);
    ModelBank::WriteLoadFilename(name, ".psx");
    ModelLoadRequest *request = model_load_queue.requests + model_load_queue.write_idx;
    request->active = active;
    request->bank = bank;
    request->pending = true;
    idx = model_load_queue.write_idx + 1;
    model_load_queue.write_idx = idx % 20;
    return bank;
}

uint32 ModelBank::FindActiveSlot(uint32 bank)
{
    FUNCTION_MARKER(0x8006F4D0, "SLUS_003.73");
    if (bank == active_model_state.banks[0])
        return 0;
    if (bank == active_model_state.banks[1])
        return 1;
    return 0xFFFFFFFF;
}

uint32 ModelBank::ProcessLoadQueue()
{
    ModelLoadRequest *queue;
    uint32 pending, state, size, mode, bank0, bank1, slot, bank;
    uint32 idx, quotient;
    uint8 *storage;
    FUNCTION_MARKER(0x8006F500, "SLUS_003.73");
    queue = model_load_queue.requests + model_load_queue.read_idx;
    pending = queue->pending;
    if (!pending)
        return pending;
    state = model_load_queue.phase;
    if (state == 1)
    {
        if (!GetResourceReadPhase())
            model_load_queue.phase = 2;
        return 2;
    }
    if ((sint32)state < 2)
    {
        if (state)
            return 1;
        size = LookupResource(queue->name);
        mode = queue->active;
        if (mode)
        {
            if (mode != 1)
            {
                model_load_queue.phase = 1;
                return 1;
            }
            bank0 = active_model_state.banks[0];
            if (bank0 != 0xFFFFFFFF)
            {
                bank1 = active_model_state.banks[1];
                if (bank1 != 0xFFFFFFFF)
                {
                    slot = active_model_state.replacement_slot;
                    bank = (uint8)active_model_state.banks[slot];
                    queue->bank = bank;
                    ModelBank::Unload(bank, true);
                    slot = active_model_state.replacement_slot;
                    bank = queue->bank;
                    active_model_state.banks[slot] = bank;
                }
                else if (bank0 != bank1)
                    active_model_state.banks[1] = queue->bank;
                else
                    active_model_state.banks[0] = queue->bank;
            }
            else
                active_model_state.banks[0] = queue->bank;
            WriteModelBankName(ModelBank::Get(queue->bank), queue->name, '.');
        }
        storage = (uint8 *)malloc(size);
        if (!storage)
        {
            GameDiagnostic(("Cannot allocate model resource buffer\n"));
            abort();
        }
        ModelBank::Get(queue->bank).data = storage;
        ModelBank::Get(queue->bank).src_bytes = GetResourceFileSize(queue->name);
        BeginResourceRead(ModelBank::Get(queue->bank).data);
        model_load_queue.phase = 1;
        return 1;
    }
    if (state != 2)
        return 2;
    mode = queue->active;
    if (!mode)
        ModelBank::LoadResourceData(queue->bank);
    else if (mode == 1)
    {
        ModelBank::LoadResourceData(queue->bank);
        ModelBank::AttachObjects(queue->bank);
        slot = ModelBank::FindActiveSlot(queue->bank);
        active_model_state.script_keys[slot] = &ModelBank::Get(queue->bank).keys;
    }
    queue->pending = false;
    idx = model_load_queue.read_idx + 1;
    model_load_queue.phase = 0;
    quotient = idx / 20;
    quotient *= 20;
    model_load_queue.read_idx = idx - quotient;
    return quotient;
}

uint32 ModelBank::WaitForLoads()
{
    uint32 busy, pending;
    FUNCTION_MARKER(0x8006F7D8, "SLUS_003.73");
    busy = GetResourceReadPhase();
    for (;;)
    {
        if (!busy)
        {
            pending = ModelBank::GetLoadPending();
            if (!pending)
                return pending;
        }
        ModelBank::ProcessLoadQueue();
        ProcessResourceRead();
        busy = GetResourceReadPhase();
    }
}

uint32 ModelBank::PrepareGeometry(uint32 bank)
{
    FUNCTION_MARKER(0x8007D8E8, "SLUS_003.73");
    ModelBank &src = ModelBank::Get(bank);
    if (!DecodeModelGeometrySet(src.geometry, src.data, src.src_bytes, src.textures) || !DecodeModelSceneObjects(src.scene, src.data, src.src_bytes, bank, src.geometry))
    {
        GameDiagnostic(("Invalid model geometry or scene in bank %u\n", bank));
        abort();
    }
    return 1;
}

static void WrapTexCoords(uint32 vals[4], uint32 adjust)
{
    while (((vals[0] & vals[1] & vals[2] & vals[3]) ^ (vals[0] | vals[1] | vals[2] | vals[3])) & 0x10000)
    {
        uint32 vertex;
        for (vertex = 0; vertex < 4; ++vertex)
            vals[vertex] += adjust;
    }
}

void ModelBank::UpdateTexCoords(uint32 bank_idx)
{
    FUNCTION_MARKER(0x8007FF70, "SLUS_003.73");
    if (bank_idx == 0xFFFFFFFF)
        return;
    const ModelBank &src_bank = ModelBank::Get(bank_idx);
    const ModelTextureAnimation &animation = src_bank.tex_animation;
    for (uint32 row = 0; row < animation.count; ++row)
    {
        const ModelTextureTrack &track = animation.tracks[row];
        const ModelInstance &object = src_bank.scene.objects[track.object_idx];
        if (!(object.flags & 0x8000))
        {
            uint32 tick = model_render_state.frame;
            uint32 scroll_x = (sint32)(tick * track.scroll_x) >> 4;
            uint32 phase_product = tick * track.phase_speed;
            ModelGeometry &geometry = ModelBank::GetGeometry(object.bank, object.model_idx);
            uint32 y_product = tick * track.scroll_y;
            uint32 item = 0;
            const uint32 phase = (sint32)phase_product >> 10;
            uint32 scroll_y = (sint32)y_product >> 4;
            while (item < track.quad_count)
            {
                uint32 x[4], y[4], vertex, tex, wrap_x, wrap_y;
                if (item >= geometry.poly_count)
                {
                    GameDiagnostic(("Model UV animation exceeds polygon count\n"));
                    abort();
                }
                ModelPolygon &poly = geometry.polys[item];
                const ModelUvQuad &quad = track.quads[item];
                for (vertex = 0; vertex < 4; ++vertex)
                {
                    x[vertex] = quad.vertices[vertex].u << 8;
                    y[vertex] = quad.vertices[vertex].v << 8;
                }
                tex = GetModelPolygonWindow(poly);
                wrap_x = (-(tex & 0x1F) << 11) & 0xF800;
                wrap_y = (-(tex & 0x3E0) << 6) & 0xFF00;
                for (vertex = 0; vertex < 4; ++vertex)
                    x[vertex] += scroll_x;
                for (vertex = 0; vertex < 4; ++vertex)
                    y[vertex] += scroll_y;
                for (vertex = 0; vertex < 4; ++vertex)
                {
                    x[vertex] += quad.vertices[vertex].ReadHorizontalOffset(phase);
                    y[vertex] += quad.vertices[vertex].ReadVerticalOffset(phase);
                }
                WrapTexCoords(x, wrap_x);
                WrapTexCoords(y, wrap_y);
                ++item;
                for (vertex = 0; vertex < 4; ++vertex)
                {
                    poly.uv[vertex].u = x[vertex] >> 8;
                    poly.uv[vertex].v = y[vertex] >> 8;
                }
            }
        }
    }
    return;
}

void ModelBank::UpdateColors(uint32 bank_idx)
{
    FUNCTION_MARKER(0x80080398, "SLUS_003.73");
    if (bank_idx == 0xFFFFFFFF)
        return;
    ModelBank &bank = ModelBank::Get(bank_idx);
    if (!bank.color_animation.present)
        return;
    SelectModelPal(bank.colors);
    const uint32 tick = model_render_state.frame;
    const uint32 old_tick = model_render_state.previous_frame;
    const uint32 step = tick >= old_tick ? tick - old_tick : 1;
    AdvanceModelColors(bank.colors, bank.color_animation, step);
}
