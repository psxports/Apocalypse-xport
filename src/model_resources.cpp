#include "game_sdk_bindings.h"
extern "C"
{
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
#include <stdlib.h>
#include <stdio.h>
}
#include "model_resources.h"
#include "model_clipping.h"
#include "game_string.h"
#include "native_object_storage.h"
#include "resource_io.h"
#include "game_heap.h"
#include "palette_resources.h"
#include "effect_lifecycle.h"
#include "object_damage.h"
#include "hud_text.h"
#include "game_panel.h"
#include <string.h>
#include <new>

void InvalidateTextureBindings(const TextureResource *tex)
{
    ForgetEffectTexture(tex);
    ForgetHudTexture(tex);
}

void InvalidateTextureBindings(const TextureGroup *grp)
{
    ForgetEffectTexture(grp);
    ForgetHudTexture(grp);
    ForgetPanelTexture(grp);
}

static ModelBank model_banks[40];

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
        GameDiagnostic("Cannot retain borrowed model storage\n");
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
    ForgetModelObjectFlashes(scene.objects, scene.count);
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
        const uint8 *src = data + 12 + i * 36;
        uint32 model_idx = xport_load_le16(src + 22);
        if (!geometry.models || model_idx >= geometry.count)
        {
            for (uint32 initialized = 0; initialized < i; ++initialized)
                decoded.objects[initialized].~ModelInstance();
            free(decoded.objects);
            return false;
        }
        ModelInstance &object = *new (decoded.objects + i) ModelInstance;
        object.flags = xport_load_le16(src);
        if (geometry.models[model_idx].flags & 16)
            object.flags |= 32;
        object.generation = xport_load_le16(src + 2);
        for (uint32 axis = 0; axis < 3; ++axis)
        {
            object.pos[axis] = xport_load_le32(src + 4 + axis * 4);
            object.angles[axis] = xport_load_le16(src + 16 + axis * 2);
        }
        object.model_idx = model_idx;
        object.frame = src[24];
        object.flash = src[25];
        object.animation = 0;
        object.bank = bank;
        object.color = xport_load_le32(src + 32);
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

void ResetActiveModelState()
{
    memset(&active_model_state, 0, sizeof(active_model_state));
    active_model_state.banks[0] = 0xFFFFFFFF;
    active_model_state.banks[1] = 0xFFFFFFFF;
}

static void CheckActiveModelSlot(uint32 slot)
{
    if (slot >= 2)
    {
        GameDiagnostic("Invalid active model slot %u\n", slot);
        abort();
    }
}

uint32 GetActiveModelBank(uint32 slot)
{
    CheckActiveModelSlot(slot);
    return active_model_state.banks[slot];
}

const ModelKeys &GetActiveModelScriptKeys(uint32 slot)
{
    CheckActiveModelSlot(slot);
    if (!active_model_state.script_keys[slot])
    {
        GameDiagnostic("Missing active model script keys\n");
        abort();
    }
    return *active_model_state.script_keys[slot];
}

ModelInstance *GetModelSceneObjects()
{
    return active_model_state.scene_objects;
}

uint32 GetModelBankReplacement()
{
    return active_model_state.replacement_slot;
}

void SelectModelBankReplacement(uint32 slot)
{
    CheckActiveModelSlot(slot);
    active_model_state.replacement_slot = slot;
}

void ResetModelBanks()
{
    ReleaseModelPolygonQueue();
    ResetModelPaletteSelection();
    for (uint32 i = 0; i < 40; ++i)
    {
        ReleaseModelSceneObjects(model_banks[i].scene);
        free(model_banks[i].data);
        ReleaseModelGeometrySet(model_banks[i].geometry);
        ReleaseTextureGroups(model_banks[i].groups);
        ReleaseTextureReferences(model_banks[i].textures);
        ReleaseModelKeys(model_banks[i].keys);
        ReleaseModelAnimations(model_banks[i].animations);
        ReleaseModelPalette(model_banks[i].colors);
        ReleaseModelColorAnimation(model_banks[i].color_animation);
        ReleaseModelTextureAnimation(model_banks[i].texture_animation);
    }
    memset(model_banks, 0, sizeof(model_banks));
    ResetTextureResources();
    ResetPaletteResources();
}

ModelBank &GetModelBank(uint32 index)
{
    if (index >= 40)
    {
        GameDiagnostic("Invalid model bank %u\n", index);
        abort();
    }
    return model_banks[index];
}

ModelGeometry &GetModelGeometry(uint32 bank, uint32 idx)
{
    ModelGeometrySet &geometry = GetModelBank(bank).geometry;
    if (idx >= geometry.count)
    {
        GameDiagnostic("Invalid model index %u in bank %u\n", idx, bank);
        abort();
    }
    return geometry.models[idx];
}

static void WriteModelBankName(ModelBank &bank, const char *name, char terminator)
{
    uint32 index = 0;
    while (name[index] != terminator)
    {
        if (!name[index] || index >= sizeof(bank.name) - 1)
        {
            GameDiagnostic("Invalid model bank name %s\n", name);
            abort();
        }
        bank.name[index] = name[index];
        ++index;
    }
    bank.name[index] = 0;
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

void ResetModelBankLoadQueue()
{
    memset(&model_load_queue, 0, sizeof(model_load_queue));
}

uint32 GetModelBankLoadPending()
{
    return model_load_queue.requests[model_load_queue.read_idx].pending;
}

uint32 InitializeModelResources()
{
    FUNCTION_MARKER(0x8006DFA0, "SLUS_003.73");
    for (uint32 item = 0; item < 20; ++item)
        model_load_queue.requests[item].pending = false;
    model_load_queue.read_idx = 0;
    model_load_queue.write_idx = 0;
    for (uint32 item = 0; item < 40; ++item)
    {
        ModelBank &bank = model_banks[item];
        ReleaseTextureGroups(bank.groups);
        ReleaseTextureReferences(bank.textures);
        bank.name[0] = 0;
        bank.protected_bank = 0;
        bank.loaded = 0;
        ReleaseModelSceneObjects(bank.scene);
        ReleaseModelGeometrySet(bank.geometry);
        free(bank.data);
        bank.data = 0;
        bank.source_bytes = 0;
        ReleaseModelAnimations(bank.animations);
        ReleaseModelColorAnimation(bank.color_animation);
        ReleaseModelTextureAnimation(bank.texture_animation);
        bank.attachments = 0;
    }
    ResetTextureResources();
    ResetPaletteResources();
    active_model_state.banks[0] = 0xFFFFFFFF;
    active_model_state.banks[1] = 0xFFFFFFFF;
    active_model_state.scene_objects = 0;
    active_model_state.replacement_slot = 0;
    return 0xFFFFFFFF;
}

uint32 FindModelResourceIndex(uint32 key, uint32 bank)
{
    FUNCTION_MARKER(0x8006E080, "SLUS_003.73");
    const ModelBank &source = GetModelBank(bank);
    if (!source.keys.count)
    {
        GameDiagnostic("Model lookup in an empty bank %u\n", bank);
        abort();
    }
    for (uint32 index = 0; index < source.keys.count; ++index)
        if (ReadModelKey(source.keys, index) == key)
            return index;
    return 0;
}

ModelInstance *FindModelObject(uint32 key)
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
            keys = &GetModelBank(prev_bank).keys;
        }
        if (ReadModelKey(*keys, object->model_idx) == key)
            return object;
        object = object->next;
    }
    return 0;
}


void DecrementModelResourceReferences(uint32 bank)
{
    FUNCTION_MARKER(0x8006E394, "SLUS_003.73");
    ReleaseTextureReferences(GetModelBank(bank).textures);
}

static void CheckModelSourceSpan(const ModelBank &bank, uint32 offset, uint32 count, uint32 stride)
{
    if (offset > bank.source_bytes || count > (bank.source_bytes - offset) / stride)
    {
        GameDiagnostic("Truncated model texture/palette data\n");
        abort();
    }
}

uint32 LoadModelResourceData(uint32 bank)
{
    uint8 *data;
    uint32 model_count, reference_count;
    uint32 cursor, resource;
    TextureResource *tex;
    PaletteResource *pal;
    uint32 retained_size, count, texture_count, textures, flags, format, palette_key;
    uint32 width, height, key, rounded_width, depth, x, y, u, right, bottom, mask;
    uint32 texture_xy[2];
    PSX_RECT img_rect;
    uint32 block_kind, block_size, block_offset;
    FUNCTION_MARKER(0x8006E4A0, "SLUS_003.73");
    data = GetModelBank(bank).data;
    uint8 *source = data;
    const ModelFileLayout layout = LoadModelKeys(GetModelBank(bank).keys, source, GetModelBank(bank).source_bytes);
    model_count = layout.model_count;
    const uint32 refs_offset = layout.keys_offset + model_count * 4;
    CheckModelSourceSpan(GetModelBank(bank), refs_offset, 1, 4);
    LoadTextureReferences(GetModelBank(bank).textures, source + refs_offset, GetModelBank(bank).source_bytes - refs_offset);
    reference_count = GetModelBank(bank).textures.count;
    ReleaseUnusedModelResources();
    ReleaseUnusedPaletteResources();
    retained_size = layout.keys_offset + (model_count << 2) + (reference_count << 2) + 4;
    cursor = retained_size;
    CheckModelSourceSpan(GetModelBank(bank), cursor, 1, 4);
    count = xport_load_le32(source + cursor);
    cursor += 4;
    CheckModelSourceSpan(GetModelBank(bank), cursor, count, 36);
    for (uint32 index = 0; index < count; ++index, cursor += 36)
        if (!FindPaletteResource(xport_load_le32(source + cursor)))
            LoadPaletteResource(xport_load_le32(source + cursor), (uint16 *)(source + cursor + 4), 1);
    CheckModelSourceSpan(GetModelBank(bank), cursor, 1, 4);
    count = xport_load_le32(source + cursor);
    cursor += 4;
    CheckModelSourceSpan(GetModelBank(bank), cursor, count, 516);
    for (uint32 index = 0; index < count; ++index, cursor += 516)
        if (!FindPaletteResource(xport_load_le32(source + cursor)))
            LoadPaletteResource(xport_load_le32(source + cursor), (uint16 *)(source + cursor + 4), 2);
    SetModelVramAllocationMode(0);
    if ((uint8)GetModelBank(bank).name[0] == 98 && (uint8)GetModelBank(bank).name[1] == 105 && (uint8)GetModelBank(bank).name[2] == 116 && (uint8)GetModelBank(bank).name[3] == 115 && !(uint8)GetModelBank(bank).name[4])
        SetModelVramAllocationMode(1);
    CheckModelSourceSpan(GetModelBank(bank), cursor, 1, 4);
    texture_count = xport_load_le32(source + cursor);
    textures = cursor + 4;
    CheckModelSourceSpan(GetModelBank(bank), textures, texture_count, 4);
    for (uint32 index = 0; index < texture_count; ++index, textures += 4)
    {
        const uint32 tex_offset = xport_load_le32(source + textures);
        CheckModelSourceSpan(GetModelBank(bank), tex_offset, 1, 20);
        const uint8 *texture_data = source + tex_offset;
        resource = xport_load_le32(texture_data + 12);
        flags = xport_load_le32(texture_data);
        format = xport_load_le32(texture_data + 4);
        palette_key = xport_load_le32(texture_data + 8);
        tex = &ReadTextureReference(GetModelBank(bank).textures, resource);
        if (tex->loaded)
            continue;
        key = tex->key;
        if (key == 0xCCE0187C || key == 0x68B7B136)
            flags |= 1;
        width = xport_load_le16(texture_data + 16);
        height = xport_load_le16(texture_data + 18);
        const uint8 *pixels = texture_data + 20;
        if (format == 256 || format == 16)
        {
            rounded_width = format == 256 ? (width + 1) & 0xFFFFFFFE : (width + 3) & 0xFFFFFFFC;
            depth = format == 256 ? 8 : 4;
            AllocateModelTextureRectangle(0, rounded_width, height, texture_xy, &texture_xy[1], depth, flags & 1 ? 2 : 0, tex->key);
            x = texture_xy[0] & 0xFFFF;
            y = texture_xy[1] & 0xFFFF;
            tex->x = (sint16)x;
            tex->y = (sint16)y;
            img_rect.x = (sint16)x;
            img_rect.y = (sint16)y;
            img_rect.w = (sint16)(format == 256 ? rounded_width >> 1 : rounded_width >> 2);
            img_rect.h = (sint16)height;
            if (!width || !height)
            {
                GameDiagnostic("Empty model texture dimensions\n");
                abort();
            }
            CheckModelSourceSpan(GetModelBank(bank), tex_offset + 20, height, (uint16)img_rect.w * 2);
            LoadImagePSX(&img_rect, (uint32 *)pixels);
        }
        pal = FindPaletteResource(palette_key);
        if (!pal)
        {
            GameDiagnostic("Missing texture palette %08X\n", palette_key);
            abort();
        }
        if (format != 256 && format != 16)
        {
            // TODO Original coordinates are undefined for unsupported texture formats
            GameDiagnostic("Unsupported model texture format %u at offset %u\n", format, tex_offset);
            abort();
        }
        x = texture_xy[0];
        y = texture_xy[1];
        ++pal->refs;
        tex->pal = pal;
        tex->clut = pal->clut;
        tex->tpage = (uint16)GetTPage(format == 256, 0, x, y);
        u = texture_xy[0] & 63;
        u <<= format == 256 ? 1 : 2;
        tex->uv[0].u = (uint8)(u);
        right = width + u - 1;
        y = texture_xy[1] & 255;
        tex->loaded = 1;
        tex->uv[1].u = (uint8)(right);
        tex->uv[3].u = (uint8)(right);
        tex->uv[0].v = (uint8)(y);
        bottom = height + y - 1;
        x = tex->uv[0].u;
        tex->uv[2].v = (uint8)(bottom);
        u = tex->uv[0].u;
        tex->uv[1].v = (uint8)(y);
        tex->uv[2].u = (uint8)(x);
        mask = ((y & 248) << 12) | ((u & 248) << 7) | 0xE2000000;
        mask |= ((0 - height) & 248) << 2;
        mask |= ((0 - width) & 248) >> 3;
        bottom = tex->uv[2].v;
        tex->window = mask;
        tex->uv[3].v = (uint8)(bottom);
    }
    SetModelVramAllocationMode(0);
    PrepareModelBankGeometry(bank);
    GetModelBank(bank).has_animation = 0;
    ReleaseModelTextureAnimation(GetModelBank(bank).texture_animation);
    ReleaseModelColorAnimation(GetModelBank(bank).color_animation);
    block_offset = layout.blocks_offset;
    for (;;)
    {
        block_kind = xport_load_le32(source + block_offset);
        if (block_kind == 0xFFFFFFFF)
            break;
        block_size = xport_load_le32(source + block_offset + 4);
        const uint8 *block_data = source + block_offset + 8;
        switch (block_kind)
        {
            case 6:
                LoadModelTextureAnimation(GetModelBank(bank).texture_animation, block_data, block_size, layout.scene_count);
                break;
            case 7:
                LoadModelColorAnimation(GetModelBank(bank).color_animation, block_data, block_size);
                break;
            case 10:
                resource = FindActiveModelBankSlot(bank);
                InitializeModelBankGrid(resource, block_data, block_size, GetModelBank(bank).scene.objects, GetModelBank(bank).scene.count);
                break;
            case 42:
                LoadModelAnimations(GetModelBank(bank).animations, block_data, block_size, model_count);
                GetModelBank(bank).has_animation = 1;
                break;
            case 69:
                RegisterTextureGroups(GetModelBank(bank).groups, GetModelBank(bank).textures, block_data, block_size);
                break;
            case 0x73424752:
                LoadModelPalette(GetModelBank(bank).colors, block_data, block_size);
                break;
            default:
                break;
        }
        block_offset += 8 + block_size;
    }
    ValidateModelColorAnimation(GetModelBank(bank).colors, GetModelBank(bank).color_animation);
    GetModelBank(bank).loaded = 1;
    free(GetModelBank(bank).data);
    GetModelBank(bank).data = 0;
    GetModelBank(bank).source_bytes = 0;
    return 1;
}

extern "C" uint32 sub_8006E4A0(uint32 bank)
{
    FUNCTION_MARKER(0x8006E4A0, "SLUS_003.73");
    return LoadModelResourceData(bank);
}

void AttachModelBankObjects(uint32 bank)
{
    FUNCTION_MARKER(0x8006EBF4, "SLUS_003.73");
    ModelInstance *head = GetModelBank(bank).scene.objects;
    if (!head)
        return;
    ModelInstance *tail = head;
    while (tail->next)
        tail = tail->next;
    tail->next = active_model_state.scene_objects;
    active_model_state.scene_objects = head;
}

void DetachModelBankObjects(uint32 slot)
{
    FUNCTION_MARKER(0x8006ECAC, "SLUS_003.73");
    CheckActiveModelSlot(slot);
    ModelInstance *target = GetModelBank(active_model_state.banks[slot]).scene.objects;
    uint32 other_bank = active_model_state.banks[slot ^ 1];
    ModelInstance *remaining = other_bank == 0xFFFFFFFF ? 0 : GetModelBank(other_bank).scene.objects;
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

uint32 FindModelBankByName(const char *name)
{
    FUNCTION_MARKER(0x8006ED70, "SLUS_003.73");
    for (uint32 bank = 0; bank < 40; ++bank)
        if (NamesEqualIgnoringCase(name, GetModelBank(bank).name))
            return bank;
    return 0xFFFFFFFF;
}

uint32 FindModelBankByName(uint32 name)
{
    return FindModelBankByName(name ? (const char *)psx_addr(name, 1) : 0);
}

void UnloadModelBankByName(uint32 name)
{
    uint32 bank;
    FUNCTION_MARKER(0x8006EDD4, "SLUS_003.73");
    bank = FindModelBankByName(name);
    if (bank != 0xFFFFFFFF && GetModelBank(bank).loaded)
    {
        bank = FindModelBankByName(name);
        UnloadModelBank(bank, 1);
    }
}

uint32 UnloadUnprotectedModelBanks()
{
    FUNCTION_MARKER(0x8006EE2C, "SLUS_003.73");
    WaitForModelBankLoads();
    UnloadActiveModelBanks();
    for (uint32 bank = 0; bank < 40; ++bank)
        if (!GetModelBank(bank).protected_bank)
            UnloadModelBank(bank, 1);
    ReleaseUnusedModelResources();
    return 0;
}

void UnloadModelBank(uint32 bank, uint32 release_unused)
{
    uint32 slot;
    FUNCTION_MARKER(0x8006EE94, "SLUS_003.73");
    if (bank == 0xFFFFFFFF)
        return;
    if (!(uint8)GetModelBank(bank).name[0])
        return;
    ReleaseTextureGroups(GetModelBank(bank).groups);
    DecrementModelResourceReferences(bank);
    ReleaseModelAnimations(GetModelBank(bank).animations);
    ReleaseModelColorAnimation(GetModelBank(bank).color_animation);
    ReleaseModelTextureAnimation(GetModelBank(bank).texture_animation);
    GetModelBank(bank).protected_bank = 0;
    GetModelBank(bank).loaded = 0;
    GetModelBank(bank).attachments = 0;
    ReleaseModelGeometrySet(GetModelBank(bank).geometry);
    GetModelBank(bank).name[0] = 0;
    slot = FindActiveModelBankSlot(bank);
    if (slot != 0xFFFFFFFF)
    {
        ClearModelBankGrid((sint32)slot);
        DetachModelBankObjects(slot & 255);
        active_model_state.banks[slot] = 0xFFFFFFFF;
    }
    ReleaseModelSceneObjects(GetModelBank(bank).scene);
    free(GetModelBank(bank).data);
    GetModelBank(bank).data = 0;
    GetModelBank(bank).source_bytes = 0;
    if (release_unused)
        ReleaseUnusedModelResources();
}







uint32 UnloadInactiveModelBanks()
{
    FUNCTION_MARKER(0x8006EF74, "SLUS_003.73");
    for (uint32 bank = 0; bank < 40; ++bank)
        if (GetModelBank(bank).loaded && bank != active_model_state.banks[0] && bank != active_model_state.banks[1] && !GetModelBank(bank).protected_bank)
            UnloadModelBank(bank, 0);
    ReleaseUnusedModelResources();
    return 0;
}

uint32 UnloadActiveModelBanks()
{
    FUNCTION_MARKER(0x8006F004, "SLUS_003.73");
    UnloadModelBank(active_model_state.banks[0], 0);
    UnloadModelBank(active_model_state.banks[1], 0);
    active_model_state.banks[0] = 0xFFFFFFFF;
    active_model_state.banks[1] = 0xFFFFFFFF;
    ReleaseUnusedModelResources();
    active_model_state.replacement_slot = 0;
    return 0;
}



uint32 FindActiveModelBankSlot(uint32 bank)
{
    FUNCTION_MARKER(0x8006F4D0, "SLUS_003.73");
    if (bank == active_model_state.banks[0])
        return 0;
    if (bank == active_model_state.banks[1])
        return 1;
    return 0xFFFFFFFF;
}










static uint32 WriteModelLoadFilename(const char *name, const char *suffix)
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

uint32 QueueModelBankLoad(const char *name, uint32 active)
{
    uint32 left, right, index, bank = 0xFFFFFFFF;
    const char *source;
    bool matches;
    FUNCTION_MARKER(0x8006F29C, "SLUS_003.73");
    for (index = 0; index < 40; ++index)
        if (GetModelBank(index).name[0] && NamesEqualIgnoringCase(name, GetModelBank(index).name))
            return index;
    for (index = 0; index < 20; ++index)
    {
        const ModelLoadRequest *request = model_load_queue.requests + index;
        if (!request->pending)
            continue;
        source = name;
        const char *queued_name = request->name;
        matches = true;
        if (*name)
        {
            for (;;)
            {
                left = (uint8)*source;
                right = (uint8)*queued_name;
                ++source;
                if (left != right)
                {
                    matches = false;
                    break;
                }
                left = (uint8)*source;
                ++queued_name;
                if (!left)
                    break;
            }
        }
        if (matches && *queued_name == '.')
            return request->bank;
    }
    for (index = 0; index < 40; ++index)
        if (!GetModelBank(index).name[0])
        {
            bank = index;
            break;
        }
    if (bank == 0xFFFFFFFF && (!active || active_model_state.banks[0] == 0xFFFFFFFF || active_model_state.banks[1] == 0xFFFFFFFF))
    {
        GameDiagnostic("No free model bank for %s\n", name);
        abort();
    }
    if (!active)
        WriteModelBankName(GetModelBank(bank), name, 0);
    WriteModelLoadFilename(name, ".psx");
    ModelLoadRequest *request = model_load_queue.requests + model_load_queue.write_idx;
    request->active = active != 0;
    request->bank = (uint8)bank;
    request->pending = true;
    index = model_load_queue.write_idx + 1;
    model_load_queue.write_idx = (uint32)((sint32)index % 20);
    return bank;
}

uint32 QueueModelBankLoad(uint32 name, uint32 active)
{
    // Incoming script/resource names remain a temporary binary boundary
    return QueueModelBankLoad((const char *)psx_addr(name, 1), active);
}

uint32 ProcessModelBankLoadQueue()
{
    ModelLoadRequest *queue;
    uint32 pending, state, size, mode, first, second, slot, bank;
    uint32 index, quotient;
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
            first = active_model_state.banks[0];
            if (first != 0xFFFFFFFF)
            {
                second = active_model_state.banks[1];
                if (second != 0xFFFFFFFF)
                {
                    slot = active_model_state.replacement_slot;
                    bank = (uint8)active_model_state.banks[slot];
                    queue->bank = (uint8)bank;
                    UnloadModelBank(bank, 1);
                    slot = active_model_state.replacement_slot;
                    bank = queue->bank;
                    active_model_state.banks[slot] = bank;
                }
                else if (first != second)
                    active_model_state.banks[1] = queue->bank;
                else
                    active_model_state.banks[0] = queue->bank;
            }
            else
                active_model_state.banks[0] = queue->bank;
            WriteModelBankName(GetModelBank(queue->bank), queue->name, '.');
        }
        storage = (uint8 *)malloc(size);
        if (!storage)
        {
            GameDiagnostic("Cannot allocate model resource buffer\n");
            abort();
        }
        GetModelBank((uint32)queue->bank).data = storage;
        GetModelBank((uint32)queue->bank).source_bytes = GetResourceFileSize(queue->name);
        BeginResourceRead(GetModelBank((uint32)queue->bank).data);
        model_load_queue.phase = 1;
        return 1;
    }
    if (state != 2)
        return 2;
    mode = queue->active;
    if (!mode)
        LoadModelResourceData(queue->bank);
    else if (mode == 1)
    {
        LoadModelResourceData(queue->bank);
        AttachModelBankObjects(queue->bank);
        slot = FindActiveModelBankSlot(queue->bank);
        active_model_state.script_keys[slot] = &GetModelBank(queue->bank).keys;
    }
    queue->pending = false;
    index = model_load_queue.read_idx + 1;
    model_load_queue.phase = 0;
    quotient = (uint32)((sint32)index / 20);
    quotient *= 20;
    model_load_queue.read_idx = index - quotient;
    return quotient;
}



extern "C" uint32 sub_8006F29C(uint32 name, uint32 active)
{
    FUNCTION_MARKER(0x8006F29C, "SLUS_003.73");
    return QueueModelBankLoad(name, active);
}


uint32 WaitForModelBankLoads()
{
    uint32 busy, pending;
    FUNCTION_MARKER(0x8006F7D8, "SLUS_003.73");
    busy = GetResourceReadPhase();
    for (;;)
    {
        if (!busy)
        {
            pending = GetModelBankLoadPending();
            if (!pending)
                return pending;
        }
        ProcessModelBankLoadQueue();
        ProcessResourceRead();
        busy = GetResourceReadPhase();
    }
}



uint32 PrepareModelBankGeometry(uint32 bank)
{
    FUNCTION_MARKER(0x8007D8E8, "SLUS_003.73");
    ModelBank &source = GetModelBank(bank);
    if (!DecodeModelGeometrySet(source.geometry, source.data, source.source_bytes, source.textures) ||
        !DecodeModelSceneObjects(source.scene, source.data, source.source_bytes, bank, source.geometry))
    {
        GameDiagnostic("Invalid model geometry or scene in bank %u\n", bank);
        abort();
    }
    return 1;
}
