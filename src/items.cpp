#include <new>
#include <stdlib.h>

#include "collision_ray.h"
#include "game_heap.h"
#include "game_tuning.h"
#include "collision.h"
#include "world_gravity.h"
#include "object_initializers.h"
#include "effect_object.h"
#include "visual_object.h"
#include "object_damage.h"
#include "game_diagnostics.h"
#include "items.h"
#include "game_object_allocation.h"
#include "model_bank.h"
#include "global.h"
#include "effect_lifecycle.h"
#include "oscillating_ring.h"
#include "angle_math.h"
#include "ring_burst_effect.h"
#include "audio_session.h"
#include "player.h"

Items::Items(const RecordCursor &data, uint16 idx) : VisualObject(), hover_y(0), fuse_timer(0), settled(false), bob_phase(0)
{
    ObjectBorrow object_borrow;
    uint16 val;
    uint16 parameter;
    uint32 ground;
    GameHeapBlock *child;
    FUNCTION_MARKER(0x8005B3E0, "SLUS_003.73");
    SetModelBank("items");
    model.model_idx = ModelBank::FindResourceIdx(0x717FB6B2, items_bank);
    AddToList(visual_lists.actors);
    col_radius = 64;
    health = 20;
    kind = 204;
    RecordCursor cursor = ReadAngles(ReadPos(data));
    cfg.damage = 20;
    cfg.arm_radius = 512;
    cfg.blast_radius = 256;
    cfg.fuse_ticks = 16;
    cfg.life_ticks = 360;
    cfg.hover_height = 128;
    rec_idx = idx;
    while (cursor.Read16() != 0x4100)
    {
        cursor.Require(6);
        parameter = (cursor + 2).Read16();
        val = (cursor + 4).Read16();
        cursor += 6;
        switch (parameter)
        {
            case DamageAmount:
                cfg.damage = val;
                break;
            case ArmRadius:
                cfg.arm_radius = val;
                break;
            case BlastRadius:
                cfg.blast_radius = val;
                break;
            case FuseTicks:
                cfg.fuse_ticks = val;
                break;
            case LifeTicks:
                cfg.life_ticks = val;
                break;
            case HoverHeight:
                cfg.hover_height = val;
                break;
            default:
                GameDiagnostic(("Items configuration index out of bounds: %d\n", parameter));
                abort();
        }
    }
    ground = CollisionRay::FindGroundHeight(model.pos, 0, 8192);
    if (ground == 0xFFFFFFFF)
        VisualObject::Finish();
    else
    {
        hover_y = ground - (cfg.hover_height << 12);
        motion.acceleration = world_gravity.vector;
        child = EffectObject::AllocateStorage(176);
        if (child)
        {
            ring = new (child) OscillatingRing(&model.pos, 64, 32, 64, 32, 96, 32, 32, 64, 0);
        }
        if (!ring)
        {
            // TODO Original constructor writes through null after failed ring allocation
            GameDiagnostic(("TODO Items ring allocation failed\n"));
            abort();
        }
        ring->persistent = true;
        bob_phase = 0;
        settled = false;
    }
}

Items::Items(const Vector3 *pos, uint16 damage, uint16 arm_radius, uint16 blast_radius, uint16 fuse_ticks, uint16 life_ticks, uint16 hover_height) : VisualObject(), hover_y(0), fuse_timer(0), settled(false), bob_phase(0)
{
    ObjectBorrow object_borrow;
    uint32 ground;
    sint32 random_x, random_y, random_z;
    FUNCTION_MARKER(0x8005B5D8, "SLUS_003.73");
    SetModelBank("items");
    model.model_idx = ModelBank::FindResourceIdx(0x717FB6B2, items_bank);
    AddToList(visual_lists.actors);
    col_radius = 64;
    health = 20;
    kind = 204;
    random_x = RandomRange(128);
    random_y = RandomRange(128);
    random_z = RandomRange(128);
    motion.angular_velocity.x = random_x - 64;
    motion.angular_velocity.y = random_y - 64;
    motion.angular_velocity.z = random_z - 64;
    pos_history.saved = *pos;
    model.pos = pos_history.saved;
    cfg.damage = damage;
    cfg.blast_radius = blast_radius;
    cfg.fuse_ticks = fuse_ticks;
    cfg.life_ticks = life_ticks;
    cfg.hover_height = hover_height;
    cfg.arm_radius = arm_radius;
    ground = CollisionRay::FindGroundHeight(model.pos, 128, 8192);
    if (ground == 0xFFFFFFFF)
        VisualObject::Finish();
    else
    {
        hover_y = ground - (cfg.hover_height << 12);
        motion.acceleration = world_gravity.vector;
        bob_phase = 0;
        settled = false;
        EnableShadow();
        shadow.max_height = GetGameTuning().pickup_floor_offset * 2;
        surface.pos = model.pos;
        surface.pos.y = ground;
    }
}

Items::~Items()
{
    if (ring)
        ring->Destroy(3);
}

uint32 Items::Destroy(uint32 reason)
{
    ObjectBorrow object_borrow;
    FUNCTION_MARKER(0x8005B7D4, "SLUS_003.73");
    RemoveFromList(visual_lists.actors);
    if (ring)
        ring->Destroy(3);
    ring = 0;
    VisualObject::Destroy(0);
    return (reason & 1) ? ReleaseStorage() : 0;
}

uint32 Items::Damage(uint32 damage, const Vector3 *, uint32)
{
    FUNCTION_MARKER(0x8005B864, "SLUS_003.73");
    if (health > 0)
    {
        health -= damage;
        if (health <= 0)
            fuse_timer = 1;
    }
    return 1;
}

uint32 Items::Update()
{
    ObjectBorrow object_borrow;
    uint32 result;
    sint32 distance;
    FUNCTION_MARKER(0x8005B890, "SLUS_003.73");
    if (cfg.life_ticks)
    {
        if (!--cfg.life_ticks)
        {
            OscillatingRing *child = ring.Get();
            fuse_timer = cfg.fuse_ticks;
            if (child)
            {
                child->SetCenterColor(128, 96, 32);
                ring->SetRingColors(64, 0, 0);
            }
        }
    }
    model.angles.AddAngles(motion.angular_velocity);
    if (!settled)
    {
        if (motion.velocity.y <= 0)
        {
            model.pos.Add(motion.velocity);
            motion.velocity.Add(motion.acceleration);
        }
        else if (model.pos.y < hover_y)
        {
            model.pos.Add(motion.velocity);
            motion.velocity.Add(motion.acceleration);
            if (model.pos.y >= hover_y)
            {
                settled = true;
                motion.velocity = zero_vector;
                motion.acceleration = zero_vector;
            }
        }
    }
    if (settled)
    {
        sint32 sine = rsin(bob_phase & 0xFFF);
        model.pos.y = (uint32)hover_y + sine * 64;
        bob_phase += 64;
    }
    shadow.height = (sint32)((uint32)surface.pos.y - model.pos.y) >> 12;
    if (visual_lists.players)
    {
        distance = player_distance;
        if (distance < cfg.blast_radius)
            goto explode;
    }
    else
        distance = 65535;
    if (fuse_timer)
    {
        model.flags |= 512;
        model.scale.y = (uint16)model.scale.y + (380);
        model.scale.x = (uint16)model.scale.x + (380);
        --fuse_timer;
        result = (uint16)model.scale.z + 380;
        model.scale.z = result;
        if (!fuse_timer)
            goto explode;
    }
    else
    {
        result = distance < cfg.arm_radius;
        if (result)
        {
            result = cfg.fuse_ticks;
            OscillatingRing *child = ring.Get();
            fuse_timer = result;
            if (!child)
                return result;
            child->SetCenterColor(128, 96, 32);
            result = ring->SetRingColors(64, 0, 0);
        }
    }
    return ring ? ring->SetPos(&model.pos) : result;
explode:
    RingBurstEffect::SpawnExplosion(&model.pos, 128, 255, 128, 0, 4, 1, 100);
    PlayPositionedSound(24, &model.pos, 4);
    if (distance < cfg.arm_radius)
    {
        // Ring creation and sound queuing do not execute gameplay callbacks
        if (!visual_lists.players)
        {
            GameDiagnostic(("Items explosion requires an active player\n"));
            abort();
        }
        Player &hero = *static_cast<Player *>(visual_lists.players->owner);
        hero.Damage(cfg.damage, &zero_vector, 0);
    }
    return VisualObject::Finish();
}
