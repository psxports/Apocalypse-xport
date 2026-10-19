#include <new>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
#include "model_resources.h"
#include "game_random.h"
#include "effect_lifecycle.h"
#include "oscillating_ring.h"
#include "angle_math.h"
#include "vector_arithmetic.h"
#include "ring_burst_effect.h"
#include "audio_session.h"
#include "player.h"

Items::Items(const RecordCursor &data, uint32 index) : VisualObject(), hover_y(0), fuse_timer(0), settled(0), bob_phase(0)
{
    ObjectBorrow object_borrow;
    uint32 value;
    uint32 parameter;
    uint32 ground;
    uint32 x;
    uint32 y;
    uint32 z;
    GameHeapBlock *child;
    FUNCTION_MARKER(0x8005B3E0, "SLUS_003.73");
    SetModelBank("items");
    value = items_bank;
    value = FindModelResourceIndex(0x717FB6B2, value);
    this->model.model_idx = value;
    AddToList(visual_lists.actors);
    this->collision_radius = 64;
    this->health = 20;
    this->kind = 204;
    RecordCursor cursor = ReadAngles(ReadPosition(data));
    cfg.damage = 20;
    cfg.arm_radius = 512;
    cfg.blast_radius = 256;
    cfg.fuse_ticks = 16;
    cfg.life_ticks = 360;
    cfg.hover_height = 128;
    record_idx = index;
    while (cursor.Read16() != 0x4100)
    {
        cursor.Require(6);
        parameter = (cursor + 2).Read16();
        value = (cursor + 4).Read16();
        cursor += 6;
        switch (parameter)
        {
            case DamageAmount:
                cfg.damage = value;
                break;
            case ArmRadius:
                cfg.arm_radius = value;
                break;
            case BlastRadius:
                cfg.blast_radius = value;
                break;
            case FuseTicks:
                cfg.fuse_ticks = value;
                break;
            case LifeTicks:
                cfg.life_ticks = value;
                break;
            case HoverHeight:
                cfg.hover_height = value;
                break;
            default:
                GameDiagnostic("Items configuration index out of bounds: %u\n", parameter);
                abort();
        }
    }
    ground = FindGroundHeight(model.pos.words, 0, 8192);
    if (ground == 0xFFFFFFFF)
        VisualObject::Finish();
    else
    {
        value = cfg.hover_height;
        hover_y = ground - (value << 12);
        x = world_gravity.vector.words[0];
        y = world_gravity.vector.words[1];
        z = world_gravity.vector.words[2];
        motion.acceleration.words[0] = x;
        motion.acceleration.words[1] = y;
        motion.acceleration.words[2] = z;
        child = AllocateEffectStorage(176);
        if (child)
        {
            ring = new (child) OscillatingRing(this->model.pos.words, 64, 32, 64, 32, 96, 32, 32, 64, 0);
        }
        if (!ring)
        {
            // TODO Original constructor writes through null after failed ring allocation
            GameDiagnostic("TODO Items ring allocation failed\n");
            abort();
        }
        ring->persistent = 1;
        bob_phase = 0;
        settled = 0;
    }
}

Items::Items(const uint32 *pos, uint32 damage, uint32 arm_radius, uint32 blast_radius, uint32 fuse_ticks, uint32 life_ticks, uint32 hover_height) : VisualObject(), hover_y(0), fuse_timer(0), settled(0), bob_phase(0)
{
    ObjectBorrow object_borrow;
    uint32 bank, value, random_x, random_y, random_z, x, y, z, ground;
    FUNCTION_MARKER(0x8005B5D8, "SLUS_003.73");
    SetModelBank("items");
    bank = items_bank;
    value = FindModelResourceIndex(0x717FB6B2, bank);
    this->model.model_idx = value;
    AddToList(visual_lists.actors);
    this->collision_radius = 64;
    this->health = 20;
    this->kind = 204;
    random_x = RandomRange(128) - 64;
    random_y = RandomRange(128) - 64;
    random_z = RandomRange(128) - 64;
    motion.angular_velocity.words[0] = random_x;
    motion.angular_velocity.words[1] = random_y;
    motion.angular_velocity.words[2] = random_z;
    x = pos[0];
    y = pos[1];
    z = pos[2];
    pos_history.saved.words[0] = x;
    pos_history.saved.words[1] = y;
    pos_history.saved.words[2] = z;
    this->model.pos.words[0] = x;
    this->model.pos.words[1] = y;
    this->model.pos.words[2] = z;
    cfg.damage = damage;
    cfg.blast_radius = blast_radius;
    cfg.fuse_ticks = fuse_ticks;
    cfg.life_ticks = life_ticks;
    cfg.hover_height = hover_height;
    cfg.arm_radius = arm_radius;
    ground = FindGroundHeight(model.pos.words, 128, 8192);
    if (ground == 0xFFFFFFFF)
        VisualObject::Finish();
    else
    {
        value = cfg.hover_height;
        hover_y = ground - (value << 12);
        x = world_gravity.vector.words[0];
        y = world_gravity.vector.words[1];
        z = world_gravity.vector.words[2];
        motion.acceleration.words[0] = x;
        motion.acceleration.words[1] = y;
        motion.acceleration.words[2] = z;
        bob_phase = 0;
        settled = 0;
        EnableShadow();
        value = GetGameTuning().pickup_floor_offset;
        shadow.max_height = value << 1;
        x = this->model.pos.words[0];
        y = this->model.pos.words[1];
        z = this->model.pos.words[2];
        surface.pos.words[0] = x;
        surface.pos.words[1] = y;
        surface.pos.words[2] = z;
        surface.pos.words[1] = ground;
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
    return (reason & 1) ? ReleaseObjectStorage(this) : 0;
}

uint32 Items::Damage(uint32 damage, const sint32 *, uint32)
{
    sint32 health = this->health;
    uint32 remaining = (uint16)this->health - damage;
    FUNCTION_MARKER(0x8005B864, "SLUS_003.73");
    if (health > 0)
    {
        this->health = (sint16)(uint16)(remaining);
        if ((sint32)(remaining << 16) <= 0)
            fuse_timer = 1;
    }
    return 1;
}

uint32 Items::Update()
{
    ObjectBorrow object_borrow;
    uint32 trig_angle;
    uint32 timer, value, ground, phase, damage;
    uint32 result, flags, scale, remaining;
    sint32 distance, height, sine;
    FUNCTION_MARKER(0x8005B890, "SLUS_003.73");
    timer = cfg.life_ticks;
    if (timer)
    {
        timer = (timer - 1) & 0xFFFF;
        cfg.life_ticks = timer;
        if (!timer)
        {
            value = cfg.fuse_ticks;
            OscillatingRing *child = ring.Get();
            fuse_timer = value;
            if (child)
            {
                child->SetCenterColor(128, 96, 32);
                ring->SetRingColors(64, 0, 0);
            }
        }
    }
    AddAngleVector(this->model.angles.words, motion.angular_velocity.words);
    if (!settled)
    {
        if ((sint32)motion.velocity.words[1] <= 0)
        {
            AddVectors(this->model.pos.words, motion.velocity.words);
            AddVectors(motion.velocity.words, motion.acceleration.words);
        }
        else
        {
            value = this->model.pos.words[1];
            ground = hover_y;
            if ((sint32)value < (sint32)ground)
            {
                AddVectors(this->model.pos.words, motion.velocity.words);
                AddVectors(motion.velocity.words, motion.acceleration.words);
                value = this->model.pos.words[1];
                ground = hover_y;
                if ((sint32)value >= (sint32)ground)
                {
                    settled = 1;
                    motion.velocity.words[2] = 0;
                    motion.velocity.words[1] = 0;
                    motion.velocity.words[0] = 0;
                    motion.acceleration.words[2] = 0;
                    motion.acceleration.words[1] = 0;
                    motion.acceleration.words[0] = 0;
                }
            }
        }
        flags = settled;
    }
    else
        flags = 1;
    if (flags)
    {
        trig_angle = bob_phase & 0xFFF;
        phase = bob_phase;
        sine = rsin(trig_angle);
        ground = hover_y;
        value = ground + ((uint32)sine << 6);
        bob_phase = phase + 64;
        this->model.pos.words[1] = value;
    }
    value = surface.pos.words[1];
    height = (sint32)(value - this->model.pos.words[1]) >> 12;
    shadow.height = (uint32)height;
    if (visual_lists.players)
    {
        distance = (sint32)player_distance;
        if (distance < (sint32)cfg.blast_radius)
            goto explode;
    }
    else
        distance = 65535;
    if (fuse_timer)
    {
        flags = this->model.flags;
        scale = this->model.scale.words[1];
        this->model.flags = flags | 512;
        value = this->model.scale.words[0];
        this->model.scale.words[1] = scale + 380;
        remaining = fuse_timer - 1;
        this->model.scale.words[0] = value + 380;
        result = this->model.scale.words[2] + 380;
        fuse_timer = remaining;
        this->model.scale.words[2] = result;
        if (!remaining)
            goto explode;
    }
    else
    {
        result = distance < (sint32)cfg.arm_radius;
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
    return ring ? ring->SetPosition(this->model.pos.words) : result;
explode:
    SpawnExplosionRing(this->model.pos.words, 128, 255, 128, 0, 4, 1, 100);
    PlayPositionedSound(24, this->model.pos.words, 4);
    if (distance < (sint32)cfg.arm_radius)
    {
        // Ring creation and sound queuing do not execute gameplay callbacks
        if (!visual_lists.players)
        {
            GameDiagnostic("Items explosion requires an active player\n");
            abort();
        }
        Player &hero = *static_cast<Player *>(visual_lists.players->owner);
        damage = cfg.damage;
        hero.Damage(damage, zero_vector.components, 0);
    }
    return VisualObject::Finish();
}

// Compatibility entry points retain the guest call contract
