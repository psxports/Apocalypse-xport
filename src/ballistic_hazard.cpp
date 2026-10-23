#include <new>

#include "collision_ray.h"
#include "game_heap.h"
#include "level_session.h"
#include "resource_script.h"
#include "object_initializers.h"
#include "visual_object.h"
#include "fire_cycle.h"
#include "audio_session.h"
#include "global.h"
#include "effect_lifecycle.h"
#include "effect_splash.h"
#include "impact_ring.h"
#include "model_bank.h"
#include "collision.h"
#include "object_damage.h"
#include "ring_burst_effect.h"
#include "game_object_allocation.h"
#include "debris.h"
#include "ballistic_hazard.h"
#include "textured_quad.h"
#include "record_data.h"
#include "enemy.h"
#include "effect_object.h"

BallisticHazard::BallisticHazard(const Vector3 *pos, uint32 kind, uint16 flags, uint32 radius, sint32 vertical_accel) : Enemy(), impact_flags(0), impact_radius(0)
{
    uint32 trig_angle;
    uint32 val;
    sint32 floor;
    ModelInstance *player;
    FUNCTION_MARKER(0x8005ABF8, "SLUS_003.73");
    pos_history.saved = *pos;
    model.pos = pos_history.saved;
    if (kind == 2)
    {
        SetModelBank("items");
        val = ModelBank::FindResourceIdx(0x717FB6B2, items_bank);
        model.model_idx = val;
        motion.acceleration.y = (uint32)vertical_accel << 12;
        shadow.radius = 128;
    }
    else if (kind <= 3)
    {
        SetModelBank(GetRecModelBankName());
        val = ModelBank::FindResourceIdx(0x2D990A1E, GetRecModelBankSlot());
        model.model_idx = val;
        motion.acceleration.y = (uint32)vertical_accel << 12;
        model.angles.y = RandomRange(4096);
        val = 256 - RandomRange(128);
        motion.angular_drag[0] = 16;
        shadow.radius = 128;
        motion.angular_velocity.x = val;
        ai.parameters[0] = kind == 3 ? 48 : 32;
        ai.parameters[1] = 256;
        ai.parameters[2] = kind == 3 ? 4 : 8;
        if (kind == 1)
        {
            sint32 spread = RandomRange(512);
            trig_angle = RandomRange(4096) & 4095;
            sint32 coefficient = rsin(trig_angle);
            player = visual_lists.players;
            val = player->pos.x;
            model.pos.x = val + spread * coefficient;
            coefficient = rcos(trig_angle);
            val = player->pos.z;
            model.pos.z = val + spread * coefficient;
            pos_history.saved = model.pos;
        }
    }
    model.flags = model.flags & 0xFFFD;
    AddToList(visual_lists.actors);
    motion.drag[0] = 2;
    motion.drag[1] = 5;
    motion.drag[2] = 2;
    col_radius = 0;
    this->kind = 410;
    impact_flags = flags;
    impact_radius = radius;
    lifecycle_flags = lifecycle_flags & 0xFFEF;
    if (flags & 5)
    {
        floor = CollisionRay::FindGroundHeight(model.pos, 0, 8192);
        if (floor == -1)
        {
            Enemy::Finish();
            return;
        }
        dst.y = floor;
        EnableShadow();
        sint32 height = (uint32)floor - (uint32)model.pos.y;
        surface.pos = model.pos;
        shadow.max_height = height >> 12;
        surface.pos.y = floor;
    }
    else if (flags & 2)
    {
        EnableShadow();
        floor = level_environment.water_y;
        sint32 height = (uint32)floor - (uint32)model.pos.y;
        shadow.max_height = height >> 12;
        surface.pos = model.pos;
        surface.pos.y = (uint32)floor - 65536;
    }
}

uint32 BallisticHazard::Destroy(uint32 reason)
{
    uint32 result;
    FUNCTION_MARKER(0x8005AF34, "SLUS_003.73");
    RemoveFromList(visual_lists.actors);
    result = Enemy::Destroy(0);
    return (reason & 1) ? ReleaseStorage() : result;
}

uint32 BallisticHazard::Damage(uint32, const Vector3 *, uint32)
{
    FUNCTION_MARKER(0x8005AF98, "SLUS_003.73");
    return 0;
}

uint32 BallisticHazard::Update()
{
    uint32 trig_angle;
    GameHeapBlock *child;
    uint16 flags;
    uint32 result;
    sint32 y;
    sint32 floor;
    uint32 distance;
    uint16 threshold;
    uint32 damage;
    uint32 val, random;
    sint32 product, idx;
    Vector3 point, velocity;
    FUNCTION_MARKER(0x8005AFA0, "SLUS_003.73");
    if (shadow.effect)
        shadow.effect->depth_bias = 128;
    Enemy::IntegrateMotion();
    flags = impact_flags;
    result = flags & 2;
    if (flags & 5)
    {
        y = model.pos.y;
        floor = dst.y;
        if (y >= floor)
        {
            if (flags == 4)
            {
                model.pos.y = floor;
                FireCycle::Spawn(&model.pos, 180, 1, 2);
                sint32 radius = impact_radius;
                RingBurstEffect::SpawnExplosion(&model.pos, radius >> 1, 255, 128, 0, 4, 1, 100);
                distance = player_distance;
                if (distance < impact_radius)
                {
                    damage = ScaleDamageForDifficulty(20);
                    VisualObject *damage_target = static_cast<VisualObject *>(visual_lists.players->owner);
                    damage_target->Damage(damage, &zero_vector, 0);
                }
                child = EffectObject::AllocateStorage(120);
                if (child)
                {
                    radius = impact_radius;
                    velocity = zero_vector;
                    ImpactRing *ring = new (child) ImpactRing(velocity);
                    ring->SetGeometry(&model.pos, 0, 0);
                    ring->lifetime = 16;
                    ring->radius_step = radius / 16;
                    ring->width_step = ring->radius_step / 2;
                }
            }
            else if (flags == 1)
            {
                if (level_environment.area_kind == 1)
                    PlayPositionedSound(59, &model.pos, 0);
                for (idx = 0; idx < ai.parameters[2]; ++idx)
                {
                    random = RandomRange(4096);
                    floor = dst.y;
                    model.pos.y = (uint32)floor - 0x20000;
                    trig_angle = random & 4095;
                    product = rsin(trig_angle) * impact_radius;
                    velocity.x = product / 8;
                    sint32 rise = RandomRange(32);
                    product = rcos(trig_angle) * impact_radius;
                    velocity.y = (-16 - rise) * ONE;
                    velocity.z = product / 8;
                    Debris *debris = 0;
                    child = AllocateZeroedObject(320);
                    if (child)
                    {
                        uint16 model_idx = ModelBank::FindResourceIdx(0x51FBD441, GetRecModelBankSlot());
                        floor = dst.y;
                        debris = new (child) Debris(&model.pos, &velocity, model_idx, floor, 193, 24, GetRecModelBankName());
                    }
                    // Skip the missing child under the approved allocation-failure contract
                    if (debris)
                        debris->damage = ai.parameters[0] >> 3;
                }
            }
            return Finish();
        }
        val = surface.pos.y;
        distance = player_distance;
        threshold = ai.parameters[1];
        sint32 height = val - y;
        result = height >> 12;
    }
    else
    {
        if (!result)
            return result;
        y = model.pos.y;
        floor = level_environment.water_y;
        if (y >= floor)
        {
            point = model.pos;
            point.y = (uint32)point.y - (286720);
            CreateSplash(&point);
            PlayPositionedSound(57, &model.pos, 0);
            return Finish();
        }
        val = surface.pos.y;
        threshold = ai.parameters[1];
        distance = player_distance;
        sint32 height = val - y;
        result = height >> 12;
    }
    shadow.height = result;
    if (distance < threshold)
    {
        ModelInstance *player = visual_lists.players;
        if (player)
        {
            damage = ScaleDamageForDifficulty(ai.parameters[0]);
            VisualObject *damage_target = static_cast<VisualObject *>(visual_lists.players->owner);
            return damage_target->Damage(damage, &zero_vector, 0);
        }
        return 0;
    }
    return result;
}
