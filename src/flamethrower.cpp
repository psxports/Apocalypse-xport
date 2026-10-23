#include <new>
#include <string.h>
#include <stdlib.h>

#include "collision_ray.h"
#include "weapon_type.h"
#include "weapon_node.h"
#include "game_heap.h"
#include "game_tick.h"
#include "effect_object.h"
#include "visual_object.h"
#include "game_diagnostics.h"
#include "flamethrower.h"
#include "audio_voice.h"
#include "angle_math.h"
#include "ribbon_segments.h"
#include "moving_sprite.h"
#include "audio_session.h"
#include "global.h"
#include "collision.h"
#include "object_damage.h"
#include "effect_lifecycle.h"

Flamethrower::Flamethrower(ModelInstance **list, ModelInstance **secondary_list) : WeaponNode(list, secondary_list), beam(), impact_count(0), seg_len(0)
{
    FUNCTION_MARKER(0x80024428, "SLUS_003.73");
    type = WPN_FLAMETHROWER;
    Configure(3000);
}

Flamethrower::~Flamethrower()
{
    ClearBeam();
}

uint32 Flamethrower::Destroy(uint32 reason)
{
    FUNCTION_MARKER(0x80024470, "SLUS_003.73");
    ClearBeam();
    uint32 result = WeaponNode::Destroy(0);
    return (reason & 1) ? ReleaseStorage() : result;
}

sint32 Flamethrower::Configure(sint32 range)
{
    FUNCTION_MARKER(0x800244CC, "SLUS_003.73");
    sint32 result = range / 20;
    seg_len = result;
    return result;
}

void Flamethrower::ClearBeam()
{
    FUNCTION_MARKER(0x800244F0, "SLUS_003.73");
    if (beam)
        beam->Destroy(3);
    beam = 0;
    if (sound)
    {
        StopSoundVoices(sound);
        sound = 0;
    }
}

// Original beam color attenuation helper
uint8 Flamethrower::ReadShade(sint32 val)
{
    FUNCTION_MARKER(0x8002455C, "SLUS_003.73");
    if (val < 0 || val >= 2049)
        return 0;
    return 255 - ((255 * val) >> 11);
}

uint32 Flamethrower::Fire(const Vector3 &input_pos, const ShortVector3 &input_angles)
{
    ModelBorrow model_borrow;
    CollisionRay &shared_ray = CollisionRay::GetShared();
    FUNCTION_MARKER(0x80024598, "SLUS_003.73");
    Vector3 step, dir, point, endpoint, start, scaled, difference, impact, fireball_velocity;
    uint32 val, scale;
    sint32 idx, prev, next, count;
    uint8 shade;
    RibbonPoint *buffer;
    ModelInstance *target;
    bool object_hit = false, geometry_hit = false;
    uint32 distance = 0;
    ModelInstance **list;
    uint32 target_distance;
    uint32 endpoint_distance;
    uint32 damage;
    GameHeapBlock *fragment;
    sint32 wave;
    ConsumeAmmo(4);
    scale = seg_len;
    step.SetDir(scale, input_angles);
    dir.Divide(step, seg_len);
    if (!beam)
    {
        ClearBeam();
        GameHeapBlock *allocation = EffectObject::AllocateStorage(108);
        if (!allocation)
        {
            GameDiagnostic(("Flamethrower ribbon allocation failed\n"));
            abort();
        }
        beam = new (allocation) RibbonSegments(20, 1);
        beam->persistent = true;
        beam->SetMaterial(0x68B7B136);
        beam->waving = 1;
        // Original leaves the first point edge RGB untouched
        buffer = beam->points;
        buffer->center.r = 0;
        buffer->center.g = 0;
        buffer->center.b = 0;
        for (idx = 1; idx < 20; ++idx)
        {
            val = (idx << 10) / 10;
            shade = Flamethrower::ReadShade(val);
            RibbonPoint *entry = beam->points + idx;
            entry->center.r = shade;
            entry->center.g = shade;
            entry->center.b = shade;
            val = LookupDistanceRoot(((val * (idx << 10)) / 10) + 0x100000);
            shade = Flamethrower::ReadShade(val);
            entry->edge.r = shade;
            entry->edge.g = shade;
            entry->edge.b = shade;
        }
        beam->v_step = 8;
        point = input_pos;
        for (idx = 0; idx < 20; ++idx)
        {
            buffer = beam->points;
            buffer[idx].pos = point;
            point.Add(step);
        }
        sound = PlaySoundEffect(18, 8192, 0);
    }
    idx = beam->point_count - 1;
    if (idx > 0)
    {
        do
        {
            RibbonPoint *point_dst = beam->points + idx;
            point_dst->pos.Add(point_dst[-1].pos, step);
            --idx;
        } while (idx > 0);
    }
    point = input_pos;
    buffer = beam->points;
    buffer->pos = point;
    point.Add(step);
    val = 2100 - game_clock.tick * 800;
    for (idx = 2, count = 12; idx < 20; ++idx, count += 6, val += 1050)
    {
        wave = rsin(val & 4095);
        buffer = beam->points + idx;
        wave = (6 * wave) / ONE;
        buffer->width = wave + 12 + count;
        buffer->height = buffer->width + 50;
    }
    buffer = beam->points;
    buffer->width = 2;
    buffer->height = 15;

    val = game_clock.tick;
    buffer = beam->points;
    buffer[1].width = 15;
    wave = rsin(((960 * val) & 0x3FC0) >> 2);

    buffer[1].height = (9 * wave) / ONE + 45;

    scale = 20;
    beam->v_scroll += 8;
    scaled.Scale(scale, step);
    endpoint.Add(input_pos, scaled);
    start = input_pos;
    OffsetByAngles(start, input_angles);
    shared_ray.endpoints[0] = start;
    shared_ray.endpoints[1] = endpoint;
    shared_ray.Initialize();
    col_query.reject_surface40 = true;
    shared_ray.Query(1);
    point = input_pos;
    list = owner_list;
    col_query.reject_surface40 = false;
    target = CollisionRay::IntersectSpheres(point, endpoint, &start, *list, 0);
    if (!target)
    {
        list = secondary_list;
        if (list)
            target = CollisionRay::IntersectSpheres(point, endpoint, &start, *list, 0);
    }
    if (target && target == visual_lists.players)
    {
        target_distance = target->pos.Distance(input_pos);
        scale = seg_len;
        if (19 * scale < target_distance)
            target = 0;
    }
    if (shared_ray.hit.GetObject())
    {
        endpoint = shared_ray.hit_pos;
        distance = shared_ray.distance - 100;
        if (!target)
            geometry_hit = true;
        else
        {
            target_distance = input_pos.Distance(target->pos);
            endpoint_distance = input_pos.Distance(endpoint);
            if (target_distance >= endpoint_distance)
                geometry_hit = true;
            else
                object_hit = true;
        }
    }
    else if (target)
        object_hit = true;
    if (object_hit)
    {
        endpoint = target->pos;
        distance = input_pos.EuclideanDistance(target->pos);
        damage = this->damage;
        static_cast<VisualObject *>(target->owner)->Damage(damage, &dir, 5);
    }
    if (geometry_hit)
    {
        ModelInstance *level_target = shared_ray.hit.GetObject();
        damage = this->damage;
        DamageLevelObject(level_target, shared_ray.hit.GetPolygon(), damage);
        level_target = shared_ray.hit.GetObject();
        if (level_target && (level_target->flags & 16))
        {
            if (static_cast<VisualObject *>(level_target->owner)->kind != 205 || owner_list != &visual_lists.players)
            {
                damage = this->damage;
                static_cast<VisualObject *>(level_target->owner)->Damage(damage, &dir, 5);
            }
        }
    }
    if (object_hit || geometry_hit)
    {
        scale = seg_len;
        idx = DivideSignedWord(distance, scale);
        if (idx < 0 || idx >= 20)
        {
            GameDiagnostic(("Flamethrower impact index %d is outside the ribbon\n", idx));
            abort();
        }
        buffer = beam->points;
        prev = idx < 19 ? idx : idx - 1;
        next = idx < 19 ? idx + 1 : idx;
        difference.Subtract(buffer[next].pos, buffer[prev].pos);
        scale = seg_len;
        val = distance - idx * scale;
        scaled.Multiply(difference, val);
        impact.Divide(scaled, seg_len);
        buffer = beam->points;
        impact.Add(buffer[idx].pos);
        for (++idx; idx < 20; ++idx)
        {
            RibbonPoint *entry = beam->points + idx;
            entry->pos = impact;
            entry->width = 0;
            entry->height = 0;
        }
    }
    else
        impact = beam->points[18].pos;
    val = impact_count + 1;
    impact_count = val;
    if (val & 1)
    {
        fireball_velocity.x = 0;
        sint32 rise = RandomRange(20);
        fireball_velocity.y = -(10 + rise) * ONE;
        fireball_velocity.z = 0;
        MovingSprite *spr = 0;
        fragment = EffectObject::AllocateStorage(112);
        if (fragment)
        {
            uint16 size = RandomRange(1200) + 2900;
            spr = new (fragment) MovingSprite(&impact, &fireball_velocity, "Fireball", size, 0, 10);
        }
        if (!spr)
        {
            GameDiagnostic(("Flamethrower impact sprite allocation failed\n"));
            abort();
        }
        spr->depth_bias = 300;
        spr->AddFlags(8);
    }
    return 1;
}

uint32 Flamethrower::Update(const Vector3 *)
{
    FUNCTION_MARKER(0x80024FDC, "SLUS_003.73");
    ClearBeam();
    return 0;
}

uint32 Flamethrower::ReadLight(uint8 *r, uint8 *g, uint8 *b)
{
    FUNCTION_MARKER(0x80024FFC, "SLUS_003.73");
    if (!beam)
        return 0;
    if (randPSX() >= 16384)
        return 0;
    *r = 255;
    *g = 128;
    *b = 0;
    return 1;
}
