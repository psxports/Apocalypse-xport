#include <stdlib.h>
#include <string.h>
#include <new>
#include <stdio.h>

#include "model_instance.h"
#include "psx.h"
#include "hd.h"
#include "global.h"
#include "scene_lifecycle.h"
#include "level_session.h"
#include "game_tick.h"
#include "game_lighting.h"
#include "resource_script.h"
#include "model_render_view.h"
#include "character.h"
#include "model_bank.h"
#include "effect_lifecycle.h"
#include "model_render.h"
#include "model_matrix.h"
#include "model_subdivision.h"
#include "model_lighting.h"
#include "model_projection.h"
#include "model_animation.h"
#include "model_clipping.h"
#include "angle_math.h"
#include "game_diagnostics.h"
#include "effect_object.h"
#include "texture_resources.h"
#include "model_render_view_internal.h"
#include "model_attachment.h"
#include "vector3.h"
#include "record_factory.h"
#include "enemy_list.h"
#include "record_data.h"
#include "ring_attack.h"
#include "player.h"
#include "game_tuning.h"
#include "object_damage.h"
#include "game_heap.h"
#include "game_object_allocation.h"
#include "visual_object.h"
#include "activation_record.h"
#include "textured_quad.h"
#include "activation_record_base.h"
#include "pause_menu.h"
#include "cd_voice_event.h"
#include "input_recording.h"
#include "game_pad_input.h"
#include "game_settings.h"
#include "weapon_type.h"
#include "weapon_node.h"
#include "cheat_input.h"
#include "resource_io.h"
#include "game_panel.h"
#include "menu_window.h"
#include "hud_text.h"
#include "hud_sprites.h"
#include "model_reference.h"
#include "model_geometry.h"
#include "items.h"
#include "plague.h"
#include "the_beast.h"
#include "war.h"
#include "death.h"
#include "scripted_level_object.h"
#include "enemy.h"
#include "object_fragments.h"
#include "explosion_effects.h"
#include "ring_burst_effect.h"
#include "glow_effect.h"
#include "expanding_glow.h"
#include "audio_session.h"
#include "collision.h"
#include "model_materials.h"
#include "textured_fragment.h"
#include "opaque_fragment.h"
#include "collision_ray.h"
#include "model_bank_grid.h"
#include "collision_ray_internal.h"

static const uint32 polygon_material_mask = 0xFFFF0000;
static void LoadVisibleModelMatrix(const MATRIX &matrix, uint32 bank);

static uint32 destruction_depth;

struct ObjectFlash
{
    uint32 color;
    uint16 step[3];
    uint8 fraction[3];
    uint8 duration;
    ModelInstance *object;
    ObjectFlash *next;
};

static ObjectFlash object_flashes[256];
static ObjectFlash *active_object_flashes;
static ObjectFlash *free_object_flashes;

struct FragmentState
{
    uint16 clut;
    uint16 tpage;
    RGBC color;
    ShortVector3 normal;
    Vector3 origin;
    TextureRegion texture_region;
};

static FragmentState fragment_state;

static void TransformFragmentVertex(Vector3 &output, const ModelGeometry &geometry, const ModelPolygon &polygon, uint32 idx);
static void SetFragmentColor(uint32 indexed, uint32 colors, uint32 count, const ModelPalette *pal);
static uint32 SpawnTexturedFragments(const Vector3 &vertex0, const Vector3 &vertex1, const Vector3 &vertex2, uint8 u0, uint8 v0, uint8 u1, uint8 v1, uint8 u2, uint8 v2, uint32 tex, uint32 subdivisions);
static uint32 SpawnOpaqueFragments(uint32 count, const Vector3 &vertex0, const Vector3 &vertex1, const Vector3 &vertex2, const Vector3 &normal, uint32 r, uint32 g, uint32 b);
static void TransformColMatrix(MATRIX &matrix);

uint32 ModelInstance::InitializeFlashes()
{
    FUNCTION_MARKER(0x8001BAC0, "SLUS_003.73");
    active_object_flashes = 0;
    free_object_flashes = object_flashes;
    for (uint32 i = 0; i < 256; ++i)
    {
        object_flashes[i].next = i + 1 < 256 ? object_flashes + i + 1 : 0;
        object_flashes[i].object = 0;
    }
    return 0;
}

static void SelectModelBoxCorners(uint32 bank, uint8 corners[3])
{
    uint32 words[5];
    for (uint32 idx = 0; idx < 5; ++idx)
        words[idx] = xport_gte_read_control(bank + idx);
    corners[0] = ((words[0] >> 15) & 1) | ((words[0] >> 30) & 2) | ((words[1] >> 13) & 4);
    corners[1] = ((words[1] >> 31) & 1) | ((words[2] >> 14) & 2) | ((words[2] >> 29) & 4);
    corners[2] = ((words[3] >> 15) & 1) | ((words[3] >> 30) & 2) | ((words[4] >> 13) & 4);
}

static void WriteModelBoxAxis(ShortVector3 corners[8], uint32 axis, sint32 pos, const ModelBounds &bounds)
{
    // Narrow the maximum sum before halving, as in SLL16/SRA17
    sint16 sum = pos + bounds.max;
    sint16 lower = sum >> 1;
    sint16 upper = (pos + bounds.min) >> 1;
    for (uint32 idx = 0; idx < 8; ++idx)
        corners[idx].comp[axis] = idx & (1 << axis) ? upper : lower;
}

static void LoadModelBoxCorner(uint32 slot, const ShortVector3 &corner)
{
    xport_gte_write_data(slot, (uint16)corner.x | ((uint32)(uint16)corner.y << 16));
    xport_gte_write_data(slot + 1, (uint16)corner.z);
}

static bool IsModelBoxOutside(const ShortVector3 corners[8], const uint8 light_corners[3], const uint8 color_corners[3])
{
    LoadModelBoxCorner(0, corners[light_corners[0]]);
    xport_gte_execute(0x4A2012);
    sint16 plane = xport_gte_read_data(9);
    LoadModelBoxCorner(2, corners[light_corners[1]]);
    if (plane < 0)
        return true;
    xport_gte_execute(0x4AA012);
    plane = xport_gte_read_data(10);
    LoadModelBoxCorner(4, corners[light_corners[2]]);
    if (plane < 0)
        return true;
    xport_gte_execute(0x4B2012);
    plane = xport_gte_read_data(11);
    LoadModelBoxCorner(0, corners[color_corners[0]]);
    if (plane < 0)
        return true;
    xport_gte_execute(0x4C6012);
    plane = xport_gte_read_data(9);
    LoadModelBoxCorner(2, corners[color_corners[1]]);
    if (plane < 0)
        return true;
    xport_gte_execute(0x4CE012);
    plane = xport_gte_read_data(10);
    LoadModelBoxCorner(4, corners[color_corners[2]]);
    if (plane < 0)
        return true;
    xport_gte_execute(0x4D6012);
    plane = xport_gte_read_data(11);
    return plane < 0;
}

void ModelInstance::ReleaseFlash(ObjectFlash *flash)
{
    FUNCTION_MARKER(0x8001BB14, "SLUS_003.73");
    ObjectFlash *cur = active_object_flashes, *prev = 0;
    while (cur)
    {
        ObjectFlash *next = cur->next;
        if (cur == flash)
        {
            cur->next = free_object_flashes;
            free_object_flashes = cur;
            if (prev)
                prev->next = next;
            else
                active_object_flashes = next;
            return;
        }
        prev = cur;
        cur = next;
    }
}

static void LoadVisibleModelMatrix(const MATRIX &matrix, uint32 bank)
{
    for (uint32 i = 0; i < 5; ++i)
        xport_gte_write_control(bank + i, PackModelRotationWord(matrix, i));
}

void ModelInstance::UpdateFlashes()
{
    FUNCTION_MARKER(0x8001BB68, "SLUS_003.73");
    ObjectFlash *flash = active_object_flashes;
    while (flash)
    {
        ObjectFlash *next = flash->next;
        if (flash->duration)
        {
            uint16 color_rg = flash->color;
            uint32 r = (flash->fraction[0] | (color_rg << 8)) + flash->step[0];
            flash->fraction[0] = (uint8)r;
            r = (r >> 8) & 255;
            uint32 g = (flash->fraction[1] | (color_rg & 0xFF00)) + flash->step[1];
            flash->fraction[1] = (uint8)g;
            g &= 0xFF00;
            uint32 b = (flash->fraction[2] | ((flash->color >> 8) & 0xFF00)) + flash->step[2];
            flash->fraction[2] = (uint8)b;
            b = (uint32)(sint8)((b >> 8) & 255);
            flash->color = (b << 16) | g | r;
            flash->object->color = flash->color;
            --flash->duration;
        }
        else
        {
            flash->object->flags &= 0xFBFF;
            ModelInstance::ReleaseFlash(flash);
        }
        flash = next;
    }
}

ObjectFlash *ModelInstance::AcquireFlash(ModelInstance *object)
{
    FUNCTION_MARKER(0x8001BC58, "SLUS_003.73");
    ObjectFlash *flash = 0;
    // Original rejects even an existing flash when the free list is empty
    if (!free_object_flashes)
        return 0;
    if (object->flags & 1024)
    {
        flash = active_object_flashes;
        while (flash && flash->object != object)
            flash = flash->next;
    }
    if (!flash)
    {
        flash = free_object_flashes;
        free_object_flashes = flash->next;
        flash->next = active_object_flashes;
        active_object_flashes = flash;
    }
    return flash;
}

uint32 ModelInstance::FlashColor(uint8 r, uint8 g, uint8 b, uint32 duration)
{
    FUNCTION_MARKER(0x8001BCDC, "SLUS_003.73");
    ObjectFlash *flash = ModelInstance::AcquireFlash(this);
    if (!flash)
        return 0;
    flash->object = this;
    flash->color = (b << 16) | (g << 8) | r;
    flash->fraction[2] = flash->fraction[1] = flash->fraction[0] = 0;
    flash->duration = duration;
    flash->step[0] = DivideSignedWord(32768 - (r << 8), duration);
    flash->step[1] = DivideSignedWord(32768 - (g << 8), duration);
    flash->step[2] = DivideSignedWord(32768 - (b << 8), duration);
    flags |= 1024;
    color = flash->color;
    return flash->color;
}

void ModelInstance::InvalidateReferences()
{
    active = false;
    while (references)
        references->Reset();
}

ModelInstance::~ModelInstance()
{
    InvalidateReferences();
    hd_model_history_free(hd_history);
}

void ModelInstance::ResetHdHistory()
{
    ResetHdCamera(this);
    hd_model_history_reset(hd_history);
    if (owner && (flags & 2))
        static_cast<Character *>(owner)->ResetHdEffects();
}

void ModelInstance::ResetDestruction()
{
    destruction_depth = 0;
}

void DamageLevelObject(ModelInstance *object, ModelPolygon *polygon, sint32 damage)
{
    ModelBorrow borrow;
    ModelInstance *node;
    uint32 bank, idx, flags, health, remaining;
    FUNCTION_MARKER(0x8001E740, "SLUS_003.73");
    if (!object)
        return;
    if (object->flags & 1)
        return;
    node = ModelBank::GetSceneObjects();
    if (!node)
        return;
    while (node != object)
    {
        node = node->next;
        if (!node)
            return;
    }
    bank = object->bank;
    idx = object->model_idx;
    ModelGeometry &model = ModelBank::GetGeometry(bank, idx);
    flags = model.flags;
    health = flags >> 24;
    if (flags & 2)
    {
        remaining = health;
        if (health || damage == 65535)
        {
            object->FlashColor(255, 255, 255, 3);
            remaining = health - (uint32)damage;
            if (damage >= (sint32)health)
            {
                remaining = 0;
                object->DestroyLevelObject();
            }
            flags = model.flags;
        }
        model.flags = (flags & 0xFFFFFF) | (remaining << 24);
    }
    else if (polygon)
        object->FragmentPolygon(polygon, 1, true, true, true, true);
}

uint32 ModelInstance::FadeToBlack(uint32 duration)
{
    FUNCTION_MARKER(0x8001BDC4, "SLUS_003.73");
    ObjectFlash *flash = ModelInstance::AcquireFlash(this);
    if (!flash)
        return 32768;
    uint16 step = -DivideSignedWord(32768, duration);
    flash->object = this;
    flash->color = 0x808080;
    flash->fraction[2] = flash->fraction[1] = flash->fraction[0] = 0;
    flash->duration = duration;
    flash->step[0] = flash->step[1] = flash->step[2] = step;
    flags |= 1024;
    color = flash->color;
    return flash->color;
}

uint32 ModelInstance::DestroyLevelObject()
{
    ModelBorrow borrow;
    ResourceScriptBorrow script_borrow;
    uint32 prev, count, result, bank, idx, resource;
    bool explosive = false;
    ResourceScript *rec;
    ModelInstance *node;
    Vector3 pos;
    GameHeapBlock *child;
    uint32 radius = 512;
    uint32 flags;
    uint32 distance;
    bool sel;
    sint32 height;
    FUNCTION_MARKER(0x8001CC1C, "SLUS_003.73");
    prev = destruction_depth;
    count = prev + 1;
    destruction_depth = count;
    result = (sint32)count < 10;
    if (!result)
    {
        destruction_depth = prev;
        return result;
    }
    if (Fragment(true, true))
    {
        bank = ModelBank::FindActiveSlot(this->bank);
        idx = model_idx;
        resource = ReadModelKey(ModelBank::GetActiveScriptKeys(bank), idx);
        rec = ActivateResourceScript(resource);
        if (rec)
        {
            idx = rec->idx;
            explosive = GetLevelRec(idx).Read16() == 9;
        }
        if (!IsGameHeapUnderPressure())
        {
            pos = this->pos;
            if (explosive)
            {
                sint16 explosion_radius = GetGameTuning().object_explosion_radius;
                sint16 strength = GetGameTuning().object_explosion_strength;
                QueueExplosion(&pos, explosion_radius, strength, 1);
                sel = game_clock.tick & 1;
                PlayPositionedSound(sel ? 1 : 2, &pos, 0);
                child = EffectObject::AllocateStorage(124);
                if (child)
                {
                    new (child) ExpandingGlow(&pos, 64, 128, 0, 512, 10);
                }
                SpawnExplosionParticles(&pos, 20, 10, 750, 80);
                child = EffectObject::AllocateStorage(124);
                if (child)
                {
                    new (child) RingBurstEffect(&pos, 5, 255, 255, 255, 1, 16, 64, 0, 1, 60, 0, 1, 100, 120, 80, 100, 10, 4);
                }
            }
            else if (!RandomRange(5))
                SpawnExplosion(&pos, 1);
            else
                SpawnExplosion(&pos, 0);
        }
        if (explosive)
            radius = 1024;
        node = ModelBank::GetSceneObjects();
        while (node)
        {
            if (node != this)
            {
                bank = node->bank;
                idx = node->model_idx;
                flags = ModelBank::GetGeometry(bank, idx).flags;
                if ((flags & 2) && (flags >> 24))
                {
                    sel = false;
                    distance = this->pos.Distance(node->pos);
                    if (explosive)
                        sel = distance < radius;
                    else if (distance < radius)
                    {
                        uint32 src_y = this->pos.y;
                        uint32 neighbor_y = node->pos.y;
                        height = src_y - neighbor_y;
                        if (height > 409600 && height <= 1638399)
                            sel = true;
                    }
                    if (sel)
                        node->DestroyLevelObject();
                }
            }
            node = node->next;
        }
    }
    result = destruction_depth - 1;
    destruction_depth = result;
    return result;
}

void ModelInstance::ReleaseFlash(uint8 idx)
{
    ModelInstance::ReleaseFlash(object_flashes + idx);
}

void ModelInstance::ForgetFlashes(const ModelInstance *objects, uint32 count)
{
    ObjectFlash *flash = active_object_flashes;
    while (flash)
    {
        ObjectFlash *next = flash->next;
        for (uint32 i = 0; i < count; ++i)
            if (flash->object == objects + i)
            {
                ModelInstance::ReleaseFlash(flash);
                break;
            }
        flash = next;
    }
}

uint32 ModelInstance::FragmentPolygon(ModelPolygon *polygon, uint32 subdiv, bool effect, bool test, bool mark, bool render)
{
    Vector3 vertices[4], offset;
    ModelBorrow borrow;
    const ModelPalette *pal;
    uint32 bank, idx, count, axis, vertex;
    uint16 flags, touched;
    uint32 result = 1, tex, color;
    uint8 r, g, b, u0, v0, u1, v1, u2, v2;
    MATRIX matrix;
    FUNCTION_MARKER(0x80021CA8, "SLUS_003.73");
    bank = this->bank;
    idx = model_idx;
    const ModelGeometry &geometry = ModelBank::GetGeometry(bank, idx);
    if (test)
    {
        touched = polygon->surface;
        if (!(touched & 8) || (touched & 1))
            return 0;
        if (mark)
        {
            flags = polygon->flags;
            polygon->surface = touched | 1;
            polygon->flags = flags & 0xFE7F;
            if (!render && ((flags & 64) || !(flags & 1)))
                polygon->flags = flags & 0xFE3F;
        }
    }
    if (!render)
        return 1;
    flags = polygon->flags;
    if (flags & 1)
    {
        fragment_state.clut = polygon->clut;
        fragment_state.tpage = polygon->tpage;
        fragment_state.texture_region = polygon->texture_region;
    }
    const SVECTOR &src_normal = geometry.normals[polygon->n];
    fragment_state.normal.x = src_normal.vx;
    fragment_state.normal.y = src_normal.vy;
    fragment_state.normal.z = src_normal.vz;
    angles.BuildRotationMatrix(matrix);
    SetRotMatrix(&matrix);
    pos.LoadGteTranslation();
    for (vertex = 0; vertex < 3; ++vertex)
        TransformFragmentVertex(vertices[vertex], geometry, *polygon, vertex);
    flags = polygon->flags;
    if (flags & 16)
    {
        count = 3;
        bank = this->bank;
        color = polygon->color;
        pal = &ModelBank::Get(bank).colors;
    }
    else
    {
        count = 4;
        TransformFragmentVertex(vertices[3], geometry, *polygon, 3);
        color = polygon->color;
        bank = this->bank;
        flags = polygon->flags;
        pal = &ModelBank::Get(bank).colors;
    }
    SetFragmentColor(flags & 2048, color, count, pal);
    for (axis = 0; axis < 3; ++axis)
    {
        sint32 sum = 0;
        sint32 center;
        for (vertex = 0; vertex < count; ++vertex)
            sum += vertices[vertex].comp[axis] >> 12;
        center = count == 3 ? sum / 3 : sum >> 2;
        fragment_state.origin.comp[axis] = (uint32)center << 12;
    }
    flags = polygon->flags;
    if (!(flags & 64) && (flags & 1))
    {
        tex = (flags & 32) ? GetModelPolygonWindow(*polygon) : 0;
        if (!IsGameHeapUnderPressure())
        {
            v0 = polygon->uv[0].v;
            u0 = polygon->uv[0].u;
            u1 = polygon->uv[1].u;
            v1 = polygon->uv[1].v;
            u2 = polygon->uv[2].u;
            v2 = polygon->uv[2].v;
            SpawnTexturedFragments(vertices[0], vertices[1], vertices[2], u0, v0, u1, v1, u2, v2, tex, subdiv);
            if (count == 4)
            {
                v0 = polygon->uv[1].v;
                u0 = polygon->uv[1].u;
                u1 = polygon->uv[2].u;
                v1 = polygon->uv[2].v;
                u2 = polygon->uv[3].u;
                v2 = polygon->uv[3].v;
                SpawnTexturedFragments(vertices[1], vertices[2], vertices[3], u0, v0, u1, v1, u2, v2, tex, subdiv);
            }
        }
    }
    else
    {
        if (!IsGameHeapUnderPressure())
        {
            const ShortVector3 &normal = GetFragmentNormal();
            if (count == 3)
                b = fragment_state.color.b;
            offset.x = normal.x;
            offset.y = normal.y;
            offset.z = normal.z;
            r = fragment_state.color.r;
            if (count == 3)
                g = fragment_state.color.g;
            else
            {
                b = fragment_state.color.b;
                g = b;
            }
            result = 2;
            SpawnOpaqueFragments(count == 3 ? 15 : 30, vertices[0], vertices[1], vertices[2], offset, r, g, b);
        }
        if (mark)
        {
            flags = polygon->flags;
            polygon->flags = flags & 0xFFBF;
        }
    }
    if (effect && !IsGameHeapUnderPressure())
    {
        r = fragment_state.color.r;
        g = fragment_state.color.g;
        b = fragment_state.color.b;
        RingBurstEffect::SpawnExplosion(&fragment_state.origin, 100, r, g, b, 4, 1, 100);
    }
    return result;
}

uint32 ModelInstance::Fragment(bool test, bool mark)
{
    ModelBorrow borrow;
    uint32 bank, idx, drawn = 0, result, random;
    bool any = false;
    FUNCTION_MARKER(0x80022318, "SLUS_003.73");
    bank = this->bank;
    idx = model_idx;
    const ModelGeometry &geometry = ModelBank::GetGeometry(bank, idx);
    ModelPolygon *polygon = geometry.polys;
    idx = 0;
    while ((sint32)idx < (sint32)geometry.poly_count)
    {
        random = RandomRange(5);
        result = FragmentPolygon(polygon, random == 0, false, test, mark, drawn < 6);
        if (result)
        {
            any = true;
            if (result == 1)
                ++drawn;
        }
        ++idx;
        polygon = polygon->next;
    }
    if (mark && any)
        flags |= 1;
    return any;
}

uint32 ModelInstance::IntersectCol(CollisionRay &rec)
{
    ResourceScriptBorrow script_borrow;
    FUNCTION_MARKER(0x8007C06C, "SLUS_003.73");
    if (!active)
        return 0;
    uint32 result, slot, target, axis;
    bool height_mode;
    XY xy;
    Vector3 translation;
    result = flags & 0x21;
    if (result)
        return result;
    SetColorMatrix(&rec.basis);
    const ModelGeometry &geometry = ModelBank::GetGeometry(bank, model_idx);
    CollisionVertex *output = GetColVertices(geometry.vertex_count);
    if (!angles.x && !angles.y && !angles.z && !(flags & 0x200))
    {
        for (axis = 0; axis < 3; ++axis)
        {
            translation.comp[axis] = (sint32)((uint32)pos.comp[axis] - rec.endpoints[0].comp[axis]) >> 12;
        }
        xport_gte_write_control(5, 0);
        xport_gte_write_control(6, 0);
        xport_gte_write_control(7, 0);
    }
    else
    {
        height_mode = rec.upward;
        col_matrix.t[0] = (sint32)((uint32)pos.x - rec.endpoints[0].x) >> 12;
        if (height_mode)
        {
            col_matrix.t[1] = (sint32)((uint32)rec.endpoints[0].z - pos.z) >> 12;
            col_matrix.t[2] = (sint32)((uint32)pos.y - rec.endpoints[0].y) >> 12;
        }
        else
        {
            col_matrix.t[1] = (sint32)((uint32)pos.y - rec.endpoints[0].y) >> 12;
            col_matrix.t[2] = (sint32)((uint32)pos.z - rec.endpoints[0].z) >> 12;
            xy.y = col_matrix.t[1];
            xy.x = col_matrix.t[0];
            xport_gte_write_data(0, xy.xy);
            xport_gte_write_data(1, col_matrix.t[2]);
            xport_gte_execute(0x4C6012);
            col_matrix.t[0] = xport_gte_read_data(25);
            col_matrix.t[1] = xport_gte_read_data(26);
            col_matrix.t[2] = xport_gte_read_data(27);
        }
        SetTransMatrix(&col_matrix);
        translation.comp[0] = translation.comp[1] = translation.comp[2] = 0;
        if (angles.x || angles.y || angles.z)
        {
            angles.BuildRotationMatrix(col_matrix);
            if (flags & 0x200)
                ScaleRotationMatrix(col_matrix);
        }
        else
        {
            InitializeModelIdentityMatrix(col_matrix);
            ScaleRotationMatrix(col_matrix);
        }
        TransformColMatrix(col_matrix);
        SetColorMatrix(&col_matrix);
    }
    result = geometry.TransformColVertices(output, rec.len, translation);
    result &= 0x60F;
    if (result)
        return result;
    col_trigger = 0xFFFFFFFF;
    geometry.IntersectColPolygons(output, rec, this);
    result = col_trigger;
    if (result == 0xFFFFFFFF)
        return result;
    slot = ModelBank::FindActiveSlot(bank);
    target = ReadModelKey(ModelBank::GetActiveScriptKeys(slot), col_trigger);
    ModelBank::SelectReplacement(slot ^ 1);
    col_query.require_surface02 = false;
    return ActivateResourceScript(target) != 0;
}

void ModelInstance::SetAttachmentTable(const ModelAttachmentTable &table) const
{
    FUNCTION_MARKER(0x8007CA50, "SLUS_003.73");
    ModelBank::Get(bank).attachments = &table;
}

void ModelInstance::PrepareVisibleDraw(bool update_lighting)
{
    const ModelProjectionConfig &proj_cfg = GetModelProjConfig();
    uint16 flags = this->flags;
    uint32 mode;
    mode = polygon_material_mask;
    model_render_state.draw.fog_start = proj_cfg.fog_start;
    model_render_state.draw.fog_shift = proj_cfg.fog_shift;
    model_render_state.draw.clut = model_render_state.reflection_clut;
    model_render_state.draw.tpage = model_render_state.reflection_tpage;
    model_render_state.draw.near_bias = model_render_state.depth_bias[0];
    model_render_state.draw.far_bias = model_render_state.depth_bias[1];
    if (flags & 0x80)
    {
        mode = (flags & 0x1000) ? (mode & 0xFFF70000) : (mode | 12);
        if (this->flags & 0x400)
        {
            RGBC color;
            color.rgbc = this->color;
            game_lighting.background_color.g = color.g << 6;
            game_lighting.background_color.b = color.b << 6;
            game_lighting.background_color.r = color.r << 6;
        }
    }
    else if (flags & 0x400)
    {
        RGBC color;
        color.rgbc = this->color;
        mode = (mode & 0xFFFB0000) | 8;
        model_render_state.draw.light.x = color.r << 6;
        model_render_state.draw.light.y = color.g << 6;
        model_render_state.draw.light.z = color.b << 6;
    }
    if (this->flags & 0x800)
        mode = (mode & 0xFFFFFE7F) | (this->render_mode & 0x180) | 0x40;
    model_render_state.draw.mode = mode;
    if (update_lighting)
    {
        if (this->flags & 0x80)
            static_cast<VisualObject *>(this->owner)->UpdateLighting();
        hd_lighting = game_lighting;
        hd_lighting_valid = true;
    }
}

void ModelInstance::RenderVisibleList(ModelInstance *owner)
{
    ModelBorrow borrow;
    FUNCTION_MARKER(0x8007EBDC, "SLUS_003.73");
    if (!owner)
        return;
    LoadVisibleModelMatrix(model_clip_matrix[0], 8);
    LoadVisibleModelMatrix(model_clip_matrix[1], 16);
    sint32 far_offset = model_clip_planes[0].offset >> 1;
    sint32 near_offset = model_clip_planes[1].offset >> 1;
    xport_gte_write_control(13, far_offset);
    xport_gte_write_control(14, near_offset);
    xport_gte_write_control(15, 0);
    ModelInstance::CullList(owner);
    if (owner == ModelBank::GetSceneObjects())
    {
        ModelBank::UpdateTexCoords(ModelBank::GetActive(0));
        ModelBank::UpdateTexCoords(ModelBank::GetActive(1));
    }
    while (owner)
    {
        const ModelRenderView *camera;
        uint16 flags = owner->flags;
        uint32 axis;
        const ModelGeometry *geometry;
        Vector3 pos, t;
        XY vertex;
        MATRIX matrix, combined;
        uint32 normal_count;
        bool transform_matrix;
        if (flags & 1)
            goto NextObject;
        if (!ModelBank::Get(owner->bank).loaded)
            goto NextObject;
        if (flags & 0x8000)
        {
            if (hd_scene_collecting())
            {
                GameLighting saved_lighting = game_lighting;
                ModelDrawContext saved_draw = model_render_state.draw;
                if (owner->hd_lighting_valid)
                    game_lighting = owner->hd_lighting;
                owner->PrepareVisibleDraw(false);
                bool captured = owner->CaptureHdPrepared(false);
                game_lighting = saved_lighting;
                model_render_state.draw = saved_draw;
                if (!captured)
                {
                    GameDiagnostic(("Cannot capture HD model outside the OG frustum\n"));
                    abort();
                }
            }
            goto NextObject;
        }
        owner->PrepareVisibleDraw(true);
        if (hd_scene_collecting())
        {
            if (!owner->CaptureHdPrepared(true))
            {
                GameDiagnostic(("Cannot capture HD model state\n"));
                abort();
            }
            goto NextObject;
        }
        flags = owner->flags;
        model_render_state.water_clip_enabled = (flags & 8) != 0;
        if (flags & 2)
        {
            static_cast<Character *>(owner->owner)->RenderModel();
            goto NextObject;
        }
        geometry = &ModelBank::GetGeometry(owner->bank, owner->model_idx);
        if (geometry->flags & 0x20)
            goto NextObject;
        camera = &GetConfiguredModelRenderView();
        LoadVisibleModelMatrix(camera->projection, 0);
        for (axis = 0; axis < 3; ++axis)
            xport_gte_write_control(5 + axis, 0);
        camera = &GetConfiguredModelRenderView();
        for (axis = 0; axis < 3; ++axis)
            pos.comp[axis] = (uint32)(owner->pos.comp[axis] >> 12) - (uint32)camera->pos.comp[axis];
        vertex.x = pos.x;
        vertex.y = pos.y;
        xport_gte_write_data(0, vertex.xy);
        xport_gte_write_data(1, pos.z);
        xport_gte_execute(0x486012);
        for (axis = 0; axis < 3; ++axis)
            t.comp[axis] = xport_gte_read_data(25 + axis);
        model_render_state.use_scratch = false;
        transform_matrix = owner->angles.x || owner->angles.y || owner->angles.z;
        if (transform_matrix)
            owner->angles.BuildRotationMatrix(matrix);
        else
            InitializeModelIdentityMatrix(matrix);
        if (owner->flags & 512)
        {
            owner->ScaleRotationMatrix(matrix);
            transform_matrix = true;
        }
        if (transform_matrix)
        {
            TransformModelMatrix(matrix, combined);
            SetRotMatrix(&combined);
        }
        SelectModelPal(ModelBank::Get(owner->bank).colors);
        if (geometry->flags & 1)
            GenerateModelReflectionCoords(matrix, &t, *geometry);
        else if (owner->flags & 128)
        {
            normal_count = geometry->n_count == geometry->poly_count ? geometry->n_count : geometry->vertex_count;
            GenerateModelNormalColors(geometry->normals, normal_count);
        }
        if (model_render_state.use_scratch)
            geometry->RenderOffsetScratch(&pos);
        else if (owner->angles.x || owner->angles.y || owner->angles.z || (owner->flags & 0x200))
        {
            for (axis = 0; axis < 3; ++axis)
                xport_gte_write_control(5 + axis, (uint32)t.comp[axis]);
            geometry->RenderProj();
        }
        else
            geometry->RenderOffset(&pos);
    NextObject:
        owner = owner->next;
    }
}

void ResetFragmentState()
{
    fragment_state.clut = 0;
    fragment_state.tpage = 0;
    fragment_state.color.rgbc = 0;
    fragment_state.normal.x = fragment_state.normal.y = fragment_state.normal.z = 0;
    fragment_state.origin.x = fragment_state.origin.y = fragment_state.origin.z = 0;
    fragment_state.texture_region.x = fragment_state.texture_region.y = 0;
    fragment_state.texture_region.width = fragment_state.texture_region.height = 0;
}

const Vector3 &GetFragmentOrigin()
{
    return fragment_state.origin;
}

const ShortVector3 &GetFragmentNormal()
{
    return fragment_state.normal;
}

static void TransformFragmentVertex(Vector3 &output, const ModelGeometry &geometry, const ModelPolygon &polygon, uint32 idx)
{
    const SVECTOR *input = geometry.vertices + polygon.v[idx];
    uint32 axis;
    uint32 shift = 12;
    FUNCTION_MARKER(0x80021ADC, "SLUS_003.73");
    xport_gte_write_data(0, ((uint16)input->vx | ((uint32)(uint16)input->vy << 16)));
    xport_gte_write_data(1, ((uint16)input->vz | ((uint32)(uint16)input->pad << 16)));
    xport_gte_execute(0x480012);
    for (axis = 0; axis < 3; ++axis)
        output.comp[axis] = xport_gte_read_data(25 + axis);
    output.ShiftLeft(shift);
}

static void SetFragmentColor(uint32 indexed, uint32 colors, uint32 count, const ModelPalette *pal)
{
    RGBC color[4];
    uint32 r, g, b, result;
    FUNCTION_MARKER(0x80021B3C, "SLUS_003.73");
    if (indexed)
    {
        color[0].rgbc = ReadModelPalColor(pal, colors & 255);
        color[1].rgbc = ReadModelPalColor(pal, (colors >> 8) & 255);
        color[2].rgbc = ReadModelPalColor(pal, (colors >> 16) & 255);
        r = color[0].r + color[1].r + color[2].r;
        g = color[0].g + color[1].g + color[2].g;
        b = color[0].b + color[1].b + color[2].b;
        if (count == 4)
        {
            color[3].rgbc = ReadModelPalColor(pal, colors >> 24);
            r += color[3].r;
            g += color[3].g;
            b += color[3].b;
            fragment_state.color.r = r / 4;
            fragment_state.color.g = g / 4;
            result = b / 4;
        }
        else
        {
            // Byte-channel sums are nonnegative and at most 765
            fragment_state.color.r = r / 3;
            fragment_state.color.g = g / 3;
            result = b / 3;
        }
    }
    else
    {
        fragment_state.color.g = colors >> 8;
        result = colors >> 16;
        fragment_state.color.r = colors;
    }
    fragment_state.color.b = result;
}

static uint32 SpawnTexturedFragments(const Vector3 &vertex0, const Vector3 &vertex1, const Vector3 &vertex2, uint8 u0, uint8 v0, uint8 u1, uint8 v1, uint8 u2, uint8 v2, uint32 tex, uint32 subdivisions)
{
    Vector3 vector, difference, scaled, midpoint, points[3], center;
    FUNCTION_MARKER(0x80021358, "SLUS_003.73");
    ShortVector3 vertices[3], jitter, shifted_jitter;
    sint32 len, candidate;
    sint32 factor;
    uint32 shift;
    uint32 edge = 0;
    GameHeapBlock *object;
    uint32 i;
    uint32 component;
    uint8 u, v;
    uint32 result = IsGameHeapUnderPressure();
    if (result != 0)
        return result;
    if (subdivisions != 0)
    {
        vector.Subtract(vertex1, vertex2);
        len = (sint32)vector.Len();
        vector.Subtract(vertex0, vertex2);
        candidate = (sint32)vector.Len();
        if (len < candidate)
        {
            len = candidate;
            edge = 1;
        }
        vector.Subtract(vertex0, vertex1);
        if (len < (sint32)vector.Len())
            edge = 2;
        factor = RandomRange(9);
        if (edge == 1)
            difference.Subtract(vertex2, vertex0);
        else if (edge == 2)
            difference.Subtract(vertex1, vertex0);
        else
            difference.Subtract(vertex2, vertex1);
        scaled.Multiply(difference, factor);
        shift = 3;
        midpoint.ShiftRight(scaled, shift);
        vector.Add(edge == 0 ? vertex1 : vertex0, midpoint);
        if (edge == 1)
        {
            u = u0 + (((u2 - u0) * factor) >> 3);
            v = v0 + (((v2 - v0) * factor) >> 3);
            SpawnTexturedFragments(vertex1, vertex0, vector, u1, v1, u0, v0, u, v, tex, subdivisions - 1);
            return SpawnTexturedFragments(vertex1, vertex2, vector, u1, v1, u2, v2, u, v, tex, subdivisions - 1);
        }
        if (edge == 2)
        {
            u = u0 + (((u1 - u0) * factor) >> 3);
            v = v0 + (((v1 - v0) * factor) >> 3);
            SpawnTexturedFragments(vertex2, vertex0, vector, u2, v2, u0, v0, u, v, tex, subdivisions - 1);
            return SpawnTexturedFragments(vertex2, vertex1, vector, u2, v2, u1, v1, u, v, tex, subdivisions - 1);
        }
        u = u1 + (((u2 - u1) * factor) >> 3);
        v = v1 + (((v2 - v1) * factor) >> 3);
        SpawnTexturedFragments(vertex0, vertex1, vector, u0, v0, u1, v1, u, v, tex, subdivisions - 1);
        return SpawnTexturedFragments(vertex0, vertex2, vector, u0, v0, u2, v2, u, v, tex, subdivisions - 1);
    }
    shift = 12;
    points[0].ShiftRight(vertex0, shift);
    points[1].ShiftRight(vertex1, shift);
    points[2].ShiftRight(vertex2, shift);
    for (component = 0; component < 3; ++component)
    {
        center.comp[component] = (points[0].comp[component] + points[1].comp[component] + points[2].comp[component]) / 3;
        for (i = 0; i < 3; ++i)
            vertices[i][component] = points[i].comp[component] - center.comp[component];
    }
    for (i = 0; i < 3; ++i)
    {
        factor = RandomRange(100);
        const ShortVector3 &normal = GetFragmentNormal();
        jitter.Scale(factor, normal);
        shifted_jitter.ShiftRight(jitter, shift);
        for (component = 0; component < 3; ++component)
            vertices[i][component] += shifted_jitter[component];
    }
    center.ShiftLeft(shift);
    use_small_effect_pool = false;
    object = EffectObject::AllocateStorage(204);
    TexturedFragment *fragment = 0;
    if (object != 0)
    {
        fragment = new (object) TexturedFragment(vertices[0], vertices[1], vertices[2], center);
    }
    uint8 r = fragment_state.color.r;
    uint8 g = fragment_state.color.g;
    uint8 b = fragment_state.color.b;
    use_small_effect_pool = true;
    if (!fragment)
    {
        GameDiagnostic(("Textured fragment allocation failed\n"));
        abort();
    }
    fragment->SetColors(r, g, b);
    uint16 clut = fragment_state.clut;
    uint16 tpage = fragment_state.tpage;
    result = fragment->SetTexCoords(clut, tpage, u0, v0, u1, v1, u2, v2);
    fragment->texture_window = tex;
    fragment->texture_region = fragment_state.texture_region;
    return result;
}

static uint32 SpawnOpaqueFragments(uint32 count, const Vector3 &vertex0, const Vector3 &vertex1, const Vector3 &vertex2, const Vector3 &normal, uint32 r, uint32 g, uint32 b)
{
    Vector3 midpoint, sum, edge0, edge1, tmp, pos, velocity, scaled[2], view_pos;
    uint32 axis;
    uint32 idx;
    uint32 val;
    uint32 floor = 0x7FFFFFFF;
    uint32 factor0;
    uint32 factor1;
    GameHeapBlock *object;
    uint32 distance;
    uint32 result;
    CollisionRay col;
    uint32 shift = 1;
    FUNCTION_MARKER(0x80022454, "SLUS_003.73");
    r &= 255;
    g &= 255;
    b &= 255;
    sum.Add(vertex1, vertex2);
    midpoint.ShiftRight(sum, shift);
    for (axis = 0; axis < 3; ++axis)
    {
        val = normal.comp[axis];
        col.endpoints[0].comp[axis] = (uint32)midpoint.comp[axis] + val * 500;
    }
    col.endpoints[1].x = col.endpoints[0].x;
    col.endpoints[1].y = (uint32)col.endpoints[0].y + 12288000;
    col.endpoints[1].z = col.endpoints[0].z;
    col.Initialize();
    col.Query(1);
    if (col.hit.GetObject())
        floor = col.hit_pos.y;
    shift = 12;
    tmp.Subtract(vertex2, vertex0);
    edge0.ShiftRight(tmp, shift);
    tmp.Subtract(vertex1, vertex0);
    edge1.ShiftRight(tmp, shift);
    for (idx = 0; idx < count; ++idx)
    {
        factor0 = RandomRange(4096);
        factor1 = RandomRange(4096);
        scaled[0].Scale(factor0, edge0);
        scaled[1].Scale(factor1, edge1);
        tmp.Add(scaled[0], scaled[1]);
        pos.ShiftRight(tmp, shift);
        scaled[0].ShiftLeft(pos, shift);
        pos.Add(scaled[0], vertex0);
        velocity.Subtract(pos, midpoint);
        velocity.Normalize(velocity);
        factor0 = RandomRange(40);
        scaled[0].Multiply(velocity, factor0);
        factor1 = RandomRange(40);
        scaled[1].Multiply(normal, factor1);
        velocity.Add(scaled[0], scaled[1]);
        object = EffectObject::AllocateStorage(120);
        if (object)
        {
            new (object) OpaqueFragment(pos, velocity, floor, r, g, b, 100, 100, 100);
        }
    }
    if (!positioned_sound_event.pending)
    {
        positioned_sound_event.pos = midpoint;
        positioned_sound_event.pending = true;
        return 1;
    }
    view_pos.x = (uint32)GetModelRenderView().pos.x;
    view_pos.y = (uint32)GetModelRenderView().pos.y;
    view_pos.z = (uint32)GetModelRenderView().pos.z;
    view_pos.ShiftLeft(shift);
    distance = view_pos.Distance(midpoint);
    result = view_pos.Distance(positioned_sound_event.pos);
    if (distance < result)
    {
        positioned_sound_event.pos = midpoint;
    }
    return result;
}

void ModelInstance::RenderList(ModelInstance *object)
{
    ModelBorrow borrow;
    const ModelProjectionConfig &proj_cfg = GetModelProjConfig();
    FUNCTION_MARKER(0x8007F138, "SLUS_003.73");
    uint16 flags;
    uint32 render_flags;
    MATRIX rotation, combined;
    if (!object)
        return;
    model_render_state.draw.fog_start = 0x7FFFFFFF;
    do
    {
        flags = object->flags;
        if (!(flags & 0x8001) && ModelBank::Get(object->bank).loaded)
        {
            render_flags = polygon_material_mask;
            if (flags & 0x400)
            {
                render_flags = (render_flags & 0xFFFB0000) | 8;
                RGBC color;
                color.rgbc = object->color;
                model_render_state.draw.light.x = color.r << 6;
                model_render_state.draw.light.y = color.g << 6;
                model_render_state.draw.light.z = color.b << 6;
            }
            if (object->flags & 0x800)
                render_flags = (render_flags & 0xFFFFFE7F) | (object->render_mode & 0x180) | 0x40;
            ModelRenderView &view = GetConfiguredModelRenderView();
            model_render_state.draw.mode = render_flags;
            if (hd_scene_collecting())
            {
                if (!object->CaptureHdRegistry())
                {
                    GameDiagnostic(("Cannot capture HD camera-relative model\n"));
                    abort();
                }
                object = object->next;
                continue;
            }
            SetRotMatrix(&view.projection);
            if (object->angles.x || object->angles.y || object->angles.z)
            {
                object->angles.BuildRotationMatrix(rotation);
                TransformModelMatrix(rotation, combined);
                SetRotMatrix(&combined);
            }
            else
                InitializeModelIdentityMatrix(rotation);
            SelectModelPal(ModelBank::Get(object->bank).colors);
            const ModelGeometry &geometry = ModelBank::GetGeometry(object->bank, object->model_idx);
            if (object->angles.x || object->angles.y || object->angles.z)
                geometry.RenderTranslatedClipped();
            else
                geometry.RenderUntranslatedClipped();
        }
        object = object->next;
    } while (object);
    model_render_state.draw.fog_start = proj_cfg.fog_start;
}

void ModelInstance::CullList(ModelInstance *object)
{
    ModelBorrow borrow;
    FUNCTION_MARKER(0x80080560, "SLUS_003.73");
    if (!object)
        return;
    uint8 light_corners[3], color_corners[3];
    ShortVector3 corners[8];
    SelectModelBoxCorners(8, light_corners);
    SelectModelBoxCorners(16, color_corners);
    const Vector3 camera_pos = GetConfiguredModelRenderView().pos;
    do
    {
        uint16 flags = object->flags;
        bool outside = true;
        do
        {
            if (flags & 1)
                break;
            sint32 x = (object->pos.x >> 12) - (uint32)camera_pos.x;
            sint32 y = (object->pos.y >> 12) - (uint32)camera_pos.y;
            sint32 z = (object->pos.z >> 12) - (uint32)camera_pos.z;
            if (z >= 32767 || z < -32767 || x >= 32767 || x < -32767 || y >= 32767 || y < -32767)
                break;
            XY half;
            half.x = x >> 1;
            half.y = y >> 1;
            xport_gte_write_data(0, half.xy);
            xport_gte_write_data(1, z >> 1);
            uint16 geometry_idx = object->model_idx;
            uint8 bank = object->bank;
            xport_gte_execute(0x4A2012);
            const ModelGeometry &geometry = ModelBank::GetGeometry(bank, geometry_idx);
            sint32 radius = geometry.radius >> 13;
            if (flags & 512)
            {
                sint16 scale_x = object->scale.x;
                sint16 scale_y = object->scale.y;
                sint16 scale_z = object->scale.z;
                sint16 scale = scale_x;
                if (scale_y > scale)
                    scale = scale_y;
                if (scale_z > scale)
                    scale = scale_z;
                radius = ((uint32)radius * scale) >> 12;
            }
            Vector3 distances;
            for (uint32 axis = 0; axis < 3; ++axis)
                distances.comp[axis] = xport_gte_read_data(25 + axis);
            xport_gte_execute(0x4C6012);
            sint32 threshold = -radius;
            if (distances.x < threshold || distances.y < threshold || distances.z < threshold)
                break;
            for (uint32 axis = 0; axis < 3; ++axis)
                distances.comp[axis] = xport_gte_read_data(25 + axis);
            if (distances.x < threshold || distances.y < threshold || distances.z < threshold)
                break;
            if (!(object->angles.x || object->angles.y || object->angles.z || (flags & 514)))
            {
                WriteModelBoxAxis(corners, 0, x, geometry.bounds[0]);
                WriteModelBoxAxis(corners, 1, y, geometry.bounds[1]);
                WriteModelBoxAxis(corners, 2, z, geometry.bounds[2]);
                if (IsModelBoxOutside(corners, light_corners, color_corners))
                    break;
            }
            outside = false;
        } while (false);
        ModelInstance *next = object->next;
        object->flags = outside ? flags | 32768 : flags & 32767;
        object = next;
    } while (object);
    return;
}

static void TransformColMatrix(MATRIX &matrix)
{
    Vector3 columns[3];
    uint32 axis;
    FUNCTION_MARKER(0x800855B4, "SLUS_003.73");
    // GTE IR inputs consume signed halfwords from each matrix column
    xport_gte_write_data(9, matrix.m[0][0]);
    xport_gte_write_data(10, matrix.m[1][0]);
    xport_gte_write_data(11, matrix.m[2][0]);
    xport_gte_execute(0x4DE012);
    for (axis = 0; axis < 3; ++axis)
        columns[0].comp[axis] = xport_gte_read_data(25 + axis);
    xport_gte_write_data(9, matrix.m[0][1]);
    xport_gte_write_data(10, matrix.m[1][1]);
    xport_gte_write_data(11, matrix.m[2][1]);
    xport_gte_execute(0x4DE012);
    for (axis = 0; axis < 3; ++axis)
        columns[1].comp[axis] = xport_gte_read_data(25 + axis);
    xport_gte_write_data(9, matrix.m[0][2]);
    xport_gte_write_data(10, matrix.m[1][2]);
    xport_gte_write_data(11, matrix.m[2][2]);
    xport_gte_execute(0x4DE012);
    matrix.m[0][0] = (uint32)columns[0].x;
    matrix.m[0][1] = (uint32)columns[1].x;
    matrix.m[2][0] = (uint32)columns[0].z;
    matrix.m[2][1] = (uint32)columns[1].z;
    for (axis = 0; axis < 3; ++axis)
        columns[2].comp[axis] = xport_gte_read_data(25 + axis);
    matrix.m[0][2] = (uint32)columns[2].x;
    matrix.m[1][0] = (uint32)columns[0].y;
    matrix.m[1][1] = (uint32)columns[1].y;
    matrix.m[1][2] = (uint32)columns[2].y;
    matrix.m[2][2] = (uint32)columns[2].z;
}

void ModelInstance::ScaleRotationMatrix(MATRIX &matrix) const
{
    FUNCTION_MARKER(0x80085A08, "SLUS_003.73");
    uint32 scale, x, y, z, prev_x, prev_y, prev_z;

    scale = this->scale.comp[0];
    x = matrix.m[0][0];
    y = matrix.m[1][0];
    z = matrix.m[2][0];
    xport_gte_write_data(8, scale);
    xport_gte_write_data(9, x);
    xport_gte_write_data(10, y);
    xport_gte_write_data(11, z);
    xport_gte_execute(0x198003D);

    scale = this->scale.comp[1];
    x = matrix.m[0][1];
    y = matrix.m[1][1];
    z = matrix.m[2][1];
    prev_x = xport_gte_read_data(9);
    prev_y = xport_gte_read_data(10);
    prev_z = xport_gte_read_data(11);
    xport_gte_write_data(8, scale);
    xport_gte_write_data(9, x);
    xport_gte_write_data(10, y);
    xport_gte_write_data(11, z);
    matrix.m[0][0] = prev_x;
    matrix.m[1][0] = prev_y;
    xport_gte_execute(0x198003D);
    matrix.m[2][0] = prev_z;

    scale = this->scale.comp[2];
    x = matrix.m[0][2];
    y = matrix.m[1][2];
    z = matrix.m[2][2];
    prev_x = xport_gte_read_data(9);
    prev_y = xport_gte_read_data(10);
    prev_z = xport_gte_read_data(11);
    xport_gte_write_data(8, scale);
    xport_gte_write_data(9, x);
    xport_gte_write_data(10, y);
    xport_gte_write_data(11, z);
    matrix.m[0][1] = prev_x;
    matrix.m[1][1] = prev_y;
    xport_gte_execute(0x198003D);
    matrix.m[2][1] = prev_z;

    prev_x = xport_gte_read_data(9);
    prev_y = xport_gte_read_data(10);
    prev_z = xport_gte_read_data(11);
    matrix.m[0][2] = prev_x;
    matrix.m[1][2] = prev_y;
    matrix.m[2][2] = prev_z;
}

bool ModelInstance::CaptureHdState(HD_OBJECT_STATE &state, const GameLighting &lighting, uint32 mask, const VECTORF &constant) const
{
    state.history = 0;
    state.pose_links = 0;
    state.pose_link_count = 0;
    const ModelBank &source = ModelBank::Get(bank);
    if ((flags & 1) || !source.loaded)
        return false;
    state.bank = ModelBank::GetHdCache(bank);
    if (!state.bank)
        return false;
    state.flags = flags;
    state.near_bias = model_render_state.draw.near_bias;
    state.far_bias = model_render_state.draw.far_bias;
    state.water_y = level_environment.water_y / 4096.0f;
    state.water_clip = flags & 8;
    state.reflection = source.geometry.models[model_idx].flags & 1;
    state.reflection_page = model_render_state.reflection_tpage >> 16;
    state.reflection_clut = model_render_state.reflection_clut >> 16;
    state.reflection_uv = model_render_state.reflection_uv;
    state.reflection_region = model_render_state.reflection_region;
    state.model = model_idx;
    state.part_count = flags & 2 ? source.geometry.count : 0;
    state.hidden_parts = 0;
    if (flags & 2)
    {
        const Character &character = *static_cast<const Character *>(owner);
        state.hidden_parts = character.hidden_models;
        character.CaptureHdHierarchy(state);
    }
    angles.BuildRotationMatrixf(state.transform);
    if (flags & 512)
    {
        VECTORF factors = {scale.x / 4096.0f, scale.y / 4096.0f, scale.z / 4096.0f};
        ScaleMatrixf(&state.transform, &factors);
    }
    state.transform.t[0] = pos.x / 4096.0f;
    state.transform.t[1] = pos.y / 4096.0f;
    state.transform.t[2] = pos.z / 4096.0f;
    const ModelProjectionConfig &projection = GetModelProjConfig();
    lighting.CaptureHd(state.lighting, state.transform, mask, constant, (float)projection.fog_start, (sint32)projection.fog_shift);
    state.radius = ModelBank::GetGeometry(bank, model_idx).radius / 4096.0f;
    if (flags & 512)
    {
        float largest = scale.x < 0 ? -(float)scale.x : (float)scale.x;
        float y = scale.y < 0 ? -(float)scale.y : (float)scale.y;
        float z = scale.z < 0 ? -(float)scale.z : (float)scale.z;
        if (y > largest)
            largest = y;
        if (z > largest)
            largest = z;
        state.radius *= largest / 4096;
    }
    return true;
}

bool ModelInstance::CaptureHdTick() const
{
    if (!owner || (flags & 1) || !ModelBank::Get(bank).loaded)
        return true;
    HD_OBJECT_STATE state;
    VECTORF constant = {0, 0, 0};
    if (!CaptureHdState(state, game_lighting, model_render_state.draw.mode, constant))
        return false;
    if (!(flags & 2))
        return hd_model_history_tick(&hd_history, &state, 0, 0) != 0;
    uint32 count = ModelBank::Get(bank).geometry.count;
    MATRIXF *poses = hd_scene_pose_buffer(count);
    const Character &character = *static_cast<const Character *>(owner);
    return poses && character.CaptureHdPoses(poses, count) == count && hd_model_history_tick(&hd_history, &state, poses, count) && character.CaptureHdModel(state, poses, count, false, true);
}

bool ModelInstance::CaptureHdPrepared(bool update_effects) const
{
    HD_OBJECT_STATE state;
    VECTORF constant = {(float)model_render_state.draw.light.x, (float)model_render_state.draw.light.y, (float)model_render_state.draw.light.z};
    if (!CaptureHdState(state, game_lighting, model_render_state.draw.mode, constant))
        return false;
    const ModelBank &source = ModelBank::Get(bank);
    if (!(flags & 2))
    {
        if (source.geometry.models[model_idx].flags & 0x20)
            return true;
        if (owner && !hd_model_history_capture(&hd_history, &state, 0, 0))
            return false;
        return hd_scene_add(&state, 0, 0) != 0;
    }
    const Character &character = *static_cast<const Character *>(owner);
    uint32 count = source.geometry.count;
    MATRIXF *poses = hd_scene_pose_buffer(count);
    if (!poses || character.CaptureHdPoses(poses, count) != count)
        return false;
    if (!hd_model_history_capture(&hd_history, &state, poses, count))
        return false;
    if (!hd_model_history_copy_poses(hd_history, poses, count))
        return false;
    return character.CaptureHdModel(state, poses, count, update_effects, false);
}

bool ModelInstance::CaptureHdRegistry() const
{
    HD_OBJECT_STATE state;
    memset(&state, 0, sizeof(state));
    state.bank = ModelBank::GetHdCache(bank);
    if (!state.bank)
        return false;
    state.model = model_idx;
    state.flags = flags;
    state.near_bias = model_render_state.draw.near_bias;
    state.far_bias = model_render_state.draw.far_bias;
    angles.BuildRotationMatrixf(state.transform);
    const ModelRenderView &view = GetConfiguredModelRenderView();
    // Registry geometry rotates around the camera and ignores model position and scale
    state.transform.t[0] = (float)view.pos.x;
    state.transform.t[1] = (float)view.pos.y;
    state.transform.t[2] = (float)view.pos.z;
    VECTORF constant = {(float)model_render_state.draw.light.x, (float)model_render_state.draw.light.y, (float)model_render_state.draw.light.z};
    game_lighting.CaptureHd(state.lighting, state.transform, model_render_state.draw.mode, constant, (float)model_render_state.draw.fog_start, (sint32)model_render_state.draw.fog_shift);
    if (!hd_model_history_capture(&hd_history, &state, 0, 0))
        return false;
    return hd_scene_add(&state, 0, 0) != 0;
}
