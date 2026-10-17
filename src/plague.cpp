#include "enemy_list.h"
#include "visual_object.h"
#include "angle_math.h"
#include "object_damage.h"
#include "vector_arithmetic.h"
#include "object_collision_events.h"
#include "model_animation.h"
#include "game_random.h"
#include "game_sdk_bindings.h"
extern "C"
{
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
}
#include "plague.h"
#include "game_panel.h"
#include "native_object_storage.h"
#include "game_heap.h"
#include "game_object_allocation.h"
#include "audio_voice.h"
#include <new>
#include <stdio.h>
#include <stdlib.h>

#include "object_force.h"

// Native extractions of original packed-angle loads and repeated motion blocks

#include "level_session.h"
#include "projectile_trail.h"
#include "plague_ground_effects.h"
#include "particle_settings.h"

#include "textured_quad.h"
#include "plague_area.h"

// Native extractions of repeated waypoint, velocity and recovery blocks

// Native extractions of original weapon construction and sine-table blocks

// Native callback ABI discards the original parameter-reader result

#include <string.h>

// Native pointer adapter for original coordinate loads

Plague &Plague::Attach(uint32 object)
{
    return *static_cast<Plague *>(FindNativeObjectStorage(object));
}

static uint32 ReadPlagueAnglesWord(uint32 address)
{
    uint32 left = r_u32((address + 3) & ~3);
    uint32 right = r_u32(address & ~3);
    uint32 shift = (address & 3) * 8;
    return shift ? (left << (32 - shift)) | (right >> shift) : right;
}

static uint32 DividePlagueJump(uint32 numerator, uint32 denominator)
{
    if (!denominator)
        return (sint32)numerator < 0 ? 1 : (uint32)-1;
    if (numerator == 0x80000000 && denominator == (uint32)-1)
        return numerator;
    return (uint32)((sint32)numerator / (sint32)denominator);
}

static void StopPlagueMotion(uint32 object)
{
    Plague::Attach(object).motion.velocity[2] = 0;
    Plague::Attach(object).motion.velocity[1] = 0;
    Plague::Attach(object).motion.velocity[0] = 0;
    Plague::Attach(object).motion.acceleration[2] = 0;
    Plague::Attach(object).motion.acceleration[1] = 0;
    Plague::Attach(object).motion.acceleration[0] = 0;
}

static void SelectPlagueFarTarget(uint32 object, uint32 position[4])
{
    uint32 random;
    do
    {
        random = RandomRange(8);
        apocalypse_trigger_position(position, r_u16(object + 516 + 2 * random));
    }
    while (apocalypse_position_distance_native(position, psx_addr(object + 4, 12)) < 257);
}

static void ClearPlagueVelocity(uint32 object)
{
    Plague::Attach(object).motion.velocity[2] = 0;
    Plague::Attach(object).motion.velocity[1] = 0;
    Plague::Attach(object).motion.velocity[0] = 0;
}

static void SavePlagueRecovery(uint32 object, uint32 state)
{
    uint32 previous = r_u16(object + 472);
    uint32 substate = r_u16(object + 474);
    w_u16(object + 472, state);
    w_u16(object + 474, 0);
    w_u16(object + 440, previous);
    w_u16(object + 442, substate);
}

static uint32 CreatePlagueOrbitWeapon(uint32 object, uint32 slot)
{
    uint32 child = sub_8002FED8(104);
    if (child)
        child = sub_80027720(child, 0x800FF5A0, 64, 1, 0, 118);
    w_u32(object + slot, child);
    w_u32(child + 40, 4);
    return child;
}

static uint32 ReadPlagueOrbitDelta(uint32 object)
{
    uint32 phase = r_u16(object + 638);
    uint32 sine = (uint32)rsin(phase & 4095);
    return (uint32)((sint32)(19 * sine) >> 11);
}

void Plague::SetParameters(uint32 command)
{
    ReadParameters(command);
}

static uint32 ReadPlagueTargetWord(const uint32 *position, uint32 axis)
{
    uint32 address = GuestMemoryAddress(position);
    if (address)
        return r_u32(address + 4 * axis);
    uint32 value;
    memcpy(&value, (const uint8 *)position + 4 * axis, 4);
    return value;
}

Plague::Plague(const uint8 *record_data, uint32 index) : Enemy()
{
    const uint32 object = NativeObjectGuestAddress(this);
    const uint32 data = GuestMemoryAddress(record_data);
    if (!data)
    {
        // TODO Bind native record data to the original parser
        GameDiagnostic("TODO Plague native record data\n");
        abort();
    }
    uint32 rotation, cursor, value;
    FUNCTION_MARKER(0x8004509C, "SLUS_003.73");
    w_u32(object + 68, 0x800A252C);
    rotation = GuestMemoryAddress(ReadPosition(record_data));
    value = r_u16(rotation);
    cursor = rotation + 6;
    this->model.angles[0] = value;
    value = r_u16(rotation + 2);
    this->model.angles[1] = value;
    value = r_u16(rotation + 4);
    w_u32(object + 400, cursor);
    w_u8(object + 380, 1);
    this->record_idx = index;
    this->model.angles[2] = value;
    Configure();
    ExecuteScript((const uint16 *)psx_addr(r_u32(object + 400), 2));
}

uint32 Plague::Configure()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 value, first, second, target;
    FUNCTION_MARKER(0x80045134, "SLUS_003.73");
    SetModelBank((const char *)psx_addr(0x800FF4C0, 1));
    motion.drag[0] = 31;
    motion.drag[1] = 31;
    motion.drag[2] = 31;
    AddToList(visual_lists.actors);
    StartAnimation(0, 1);
    AllocateBehaviorTable(1, 5);
    value = sub_8004B914(object, 10);
    SetBehaviorValue(0, 1, value & 0xFFFF);
    SetBehaviorValue(0, 2, 160);
    w_u16(object + 436, 16);
    w_u16(object + 438, 32);
    this->kind = 223;
    w_u32(object + 208, 128);
    w_u8(object + 454, 0);
    w_u16(object + 472, 0);
    w_u16(object + 474, 0);
    this->model.scale[0] = 6000;
    this->model.scale[1] = 6000;
    this->model.scale[2] = 6000;
    w_u16(object + 456, 292);
    value = this->model.flags;
    w_u8(object + 381, 0);
    w_u8(object + 314, 1);
    this->model.flags = value | 0x200;
    first = sub_8006B864(32, 0, 1);
    w_u32(object + 316, first);
    second = sub_8006B864(16, 0, 1);
    first = r_u32(object + 316);
    w_u32(object + 320, second);
    w_u16(first, 0);
    target = r_u32(object + 316);
    w_u16(target + 2, 0xF000);
    target = r_u32(object + 316);
    w_u16(target + 4, 0);
    target = r_u32(object + 320);
    w_u8(target, 128);
    target = r_u32(object + 320);
    w_u8(target + 1, 128);
    target = r_u32(object + 320);
    w_u8(target + 2, 128);
    w_u32(0x800FF4EC, object);
    return target;
}

uint32 Plague::Destroy(uint32 reason)
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 value, table, receiver, target, index;
    FUNCTION_MARKER(0x800452B4, "SLUS_003.73");
    w_u32(object + 68, 0x800A252C);
    RemoveFromList(visual_lists.actors);
    FreeBehaviorTable();
    ClearTrackedObjectHealth();
    value = r_u32(object + 316);
    if (value)
        ReleaseHeapBlock(value);
    value = r_u32(object + 320);
    if (value)
        ReleaseHeapBlock(value);
    value = r_u32(object + 648);
    if (value)
    {
        table = r_u32(value + 68);
        receiver = value + (uint32)(sint32)(sint16)r_u16(table + 8);
        target = r_u32(table + 12);
        apocalypse_object_cleanup(target, receiver, 3);
    }
    value = r_u32(object + 572);
    if (value)
        StopSoundVoices(value);
    for (index = 0; index < 3; ++index)
    {
        value = r_u32(object + 620 + 4 * index);
        if (value)
        {
            table = r_u32(value);
            receiver = value + (uint32)(sint32)(sint16)r_u16(table + 8);
            target = r_u32(table + 12);
            apocalypse_object_cleanup(target, receiver, 3);
        }
    }
    w_u32(0x800FF4EC, 0);
    Enemy::Destroy(0);
    if (reason & 1)
        return ReleaseObjectStorage(object);
    return 0;
}

uint32 Plague::Update()
{
    SVECTOR motion_cur, motion_target;
    uint32 motion_cur_word, motion_cur_tail, motion_target_word, motion_target_tail;
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 hero, x, y, z, flags, value, timer, target, current_xy, current_z;
    uint32 desired_xy, scale, vx, vy, vz, distance, damage;
    CollisionRay record;
    uint32 delta[4], normal[4];
    sint16 desired[4];
    sint32 frame, magnitude;
    FUNCTION_MARKER(0x800453F4, "SLUS_003.73");
    hero = visual_lists.players;
    if (!hero)
        return 0;
    x = r_u32(0x800A71CC);
    y = r_u32(0x800A71D0);
    z = r_u32(0x800A71D4);
    motion.acceleration[0] = x;
    motion.acceleration[1] = y;
    motion.acceleration[2] = z;
    if (!r_u8(object + 381))
    {
        SetModelBoneTable(this->model.bank, PlagueAttachments);
        w_u8(object + 381, 1);
    }
    flags = this->activation_flags;
    w_u16(object + 388, flags);
    if (flags & 1)
    {
        value = r_u8(object + 384);
        w_u8(object + 384, value + 1);
        value = r_u32(object + 396);
        this->activation_flags = flags & 0xFFFE;
        w_u32(object + 396, value | 0x100);
    }
    if (r_u32(object + 220) < 256)
    {
        hero = visual_lists.players;
        if (hero)
            ApplyObjectContactImpulse(hero, object);
    }
    if (r_u8(object + 380))
    {
        timer = r_u16(object + 476);
        if (timer)
            w_u16(object + 476, timer - 1);
        else
            ExecuteScript((const uint16 *)psx_addr(r_u32(object + 400), 2));
    }
    else
        UpdateState();
    UpdateOrbitingWeapons();
    SpawnPickup();
    if (r_u8(object + 382))
        AddVectors(motion.acceleration, (const uint32 *)psx_addr(0x800A6844, 12));
    IntegrateMotion();
    record.pos[0][0] = this->model.pos[0];
    record.pos[0][1] = this->model.pos[1] - 0x40000;
    z = this->model.pos[2];
    record.pos[1][0] = record.pos[0][0];
    record.pos[1][1] = record.pos[0][1] + 0x1000000;
    record.pos[0][2] = z;
    record.pos[1][2] = z;
    InitializeCollisionRay(record);
    QueryCollisionRay(record, 1);
    if (record.hit.GetObject())
    {
        w_u8(object + 314, 1);
        w_u32(object + 184, record.hit_pos[0]);
        w_u32(object + 188, record.hit_pos[1]);
        w_u32(object + 192, record.hit_pos[2]);
        w_u16(object + 164, (uint16)record.n.vx);
        w_u16(object + 166, (uint16)record.n.vy);
        w_u16(object + 168, (uint16)record.n.vz);
    }
    else
        w_u8(object + 314, 0);
    flags = r_u32(object + 396);
    if (flags & 0x31)
    {
        if (flags & 16)
        {
            target = r_u32(object + 580);
            x = r_u32(target + 4);
            y = r_u32(target + 8);
            z = r_u32(target + 12);
            w_u32(object + 584, x);
            w_u32(object + 588, y);
            w_u32(object + 592, z);
        }
        target = flags & 0x30 ? object + 584 : object + 484;
        xport_draft_host_sub_80066B8C_p1(desired, object + 4, target);
        desired[0] = 0;
        current_xy = ReadPlagueAnglesWord(object + 16);
        current_z = (uint32)(sint32)(sint16)this->model.angles[2];
        desired_xy = (uint16)desired[0] | ((uint32)(uint16)desired[1] << 16);
        scale = flags & 0x30 ? 32 : r_u16(object + 438);
        motion_cur_word = current_xy;
        motion_cur_tail = current_z & 0xFFFF;
        motion_target_word = desired_xy;
        motion_target_tail = (uint16)desired[2];
        motion_cur.vx = (sint16)motion_cur_word;
        motion_cur.vy = (sint16)(motion_cur_word >> 16);
        motion_cur.vz = (sint16)motion_cur_tail;
        motion_cur.pad = (sint16)(motion_cur_tail >> 16);
        motion_target.vx = (sint16)motion_target_word;
        motion_target.vy = (sint16)(motion_target_word >> 16);
        motion_target.vz = (sint16)motion_target_tail;
        motion_target.pad = (sint16)(motion_target_tail >> 16);
        UpdateAngleAcceleration(motion_cur, motion.angular_velocity, motion.angular_acceleration, motion_target, scale);
        if (flags & 0x30)
            goto attack;
        if (r_u32(object + 396) & 4)
        {
            frame = (sint8)this->model.frame;
            if (frame < 13)
            {
                w_u32(object + 304, 0x10000);
                w_u8(object + 382, 0);
                StopPlagueMotion(object);
            }
            else if (r_u16(object + 502))
                w_u8(object + 382, 1);
            frame = (sint8)this->model.frame;
            if (frame >= 13 && !(r_u32(object + 396) & 8))
            {
                x = r_u32(object + 484);
                y = this->model.pos[0];
                timer = r_u16(object + 502);
                vx = DividePlagueJump(x - y, timer);
                x = r_u32(object + 492);
                y = this->model.pos[2];
                timer = r_u16(object + 502);
                vz = DividePlagueJump(x - y, timer);
                x = r_u32(object + 488);
                y = this->model.pos[1];
                timer = r_u16(object + 502);
                vy = DividePlagueJump(x - y, timer);
                value = r_u32(object + 396);
                w_u32(object + 396, value | 8);
                motion.velocity[0] = vx;
                motion.velocity[2] = vz;
                motion.velocity[1] = vy - (timer << 14);
                sub_80069EF4(114, object + 4, 0);
            }
            else
            {
                frame = (sint8)this->model.frame;
                if (frame >= 23)
                {
                    if (!r_u16(object + 502))
                        goto jump_finished;
                    vy = motion.velocity[1];
                    w_u32(object + 304, (sint32)vy >= 0 && frame < 34 ? 0x10000 : 0);
                }
            }
            timer = r_u16(object + 502);
            if (timer)
            {
                value = (timer - 1) & 0xFFFF;
                w_u16(object + 502, value);
                if (!value)
                {
                    sub_80069EF4(119, object + 4, 0);
                    x = r_u32(object + 484);
                    y = r_u32(object + 488);
                    z = r_u32(object + 492);
                    this->model.pos[0] = x;
                    this->model.pos[1] = y;
                    this->model.pos[2] = z;
                    StopPlagueMotion(object);
                    w_u32(object + 304, 0x10000);
                    w_u8(object + 382, 0);
                }
            }
        jump_finished:
            if (r_u8(object + 303))
                w_u32(object + 396, r_u32(object + 396) & 0xFFFFFFF2);
        }
        else
        {
            value = this->model.animation;
            if (value != 6 && value != 1)
                StartAnimation(1, 1);
            for (value = 0; value < 3; ++value)
            {
                x = r_u32(object + 484 + 4 * value);
                y = this->model.pos[value];
                delta[value] = (uint32)((sint32)(x - y) >> 12);
            }
            NormalizeShortVector(delta, normal);
            magnitude = (sint16)motion.angular_velocity[1];
            if (magnitude < 0)
                magnitude = -magnitude;
            if (magnitude > 128)
                magnitude = 128;
            scale = r_u16(object + 436) * (128 - (uint32)magnitude) / 128;
            vx = normal[0] * scale;
            vy = normal[1] * scale;
            vz = normal[2] * scale;
            motion.velocity[0] = vx;
            motion.velocity[1] = vy;
            motion.velocity[2] = vz;
            distance = sub_80066918(object + 4, object + 484);
            scale = r_u16(object + 436);
            if (distance < 2 * scale)
                w_u32(object + 396, r_u32(object + 396) & ~1);
        }
    }
attack:
    if (this->model.animation == 7)
    {
        hero = visual_lists.players;
        if (hero && (uint32)(this->model.frame - 15) < 5
            && sub_80066918(object + 4, hero + 4) < 280)
        {
            damage = sub_8004B914(object, 2);
            hero = visual_lists.players;
            ApplyObjectDamage(hero, damage, (const sint32 *)psx_addr(0x800A71CC, 12), 0);
        }
    }
    return sub_8007FDF0(object);
}

uint32 Plague::Damage(uint32 damage, const sint32 *, uint32 reason)
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 value, health;
    FUNCTION_MARKER(0x80045B14, "SLUS_003.73");
    value = r_u8(object + 383);
    w_u8(object + 383, value + 1);
    value = (uint32)((sint32)damage >> 1);
    if (reason == 30)
        value = (uint32)((sint32)value >> 1);
    else if (reason == 11)
        value -= (uint32)((sint32)value >> 1);
    health = r_u16(object + 218) - value;
    w_u16(object + 218, health);
    if ((sint32)(health << 16) <= 0)
        ClearTrackedObjectHealth();
    SetColor(3, 0, 255, 0);
    return 1;
}

void Plague::UpdateState()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 state = r_u16(object + 472), substate, value, flags, hero;
    uint32 child, second, random, timer, phase, target, speed, damage;
    uint32 position[4], velocity[4], scaled[4], sum1[4], sum2[4], sum3[4], sum4[4];
    uint32 scalar, x, y, z, angle;
    sint16 direction[4], aim[4];
    sint32 health, radius, ground, increment;
    FUNCTION_MARKER(0x80045BA8, "SLUS_003.73");
    if (state >= 33)
        return;
    target = r_u32(0x800A2584 + 4 * state);
    switch (target)
    {
        case 0x80045C04:
            substate = r_u16(object + 474);
            if (!substate)
            {
                ground = (sint32)sub_80067A18(object + 4, 0, 4096);
                if (ground == -1)
                    return;
                value = this->model.pos[1];
                radius = (sint16)r_u16(object + 456);
                if (((sint32)((uint32)ground - value) >> 12) >= 2 * radius)
                    return;
                value = (uint32)ground - ((uint32)radius << 12);
                this->model.pos[1] = value;
                w_u16(object + 202, (uint32)((sint32)((uint32)ground - value) >> 12));
                w_u32(object + 188, (uint32)ground);
                w_u8(object + 382, 0);
                ClearPlagueVelocity(object);
                w_u16(object + 462, 0);
                w_u16(object + 218, 800);
                ::TrackObjectHealth(object, 24, 42, 4096);
                StartAnimation(0, 1);
                goto advance_substate;
            }
            if (substate != 1)
                return;
            value = r_u16(object + 462);
            increment = (sint16)r_u16(object + 462);
            if (value < 180)
                w_u16(object + 462, (uint32)increment + 1);
            else
            {
                w_u16(object + 462, 0);
                w_u16(object + 472, substate);
                w_u16(object + 474, 0);
            }
            return;

        case 0x80045CF4:
            w_u16(object + 504, 1);
            if (!RandomRange(64))
            {
                random = RandomRange(2);
                apocalypse_trigger_position(position, r_u16(object + 536 + 2 * random));
                child = sub_80032DC0(112);
                if (child)
                {
                    damage = sub_8004B914(object, 10);
                    void *dram_addr = AllocateNativeObjectStorage(child, sizeof(PlagueGroundEffect));
                    new (dram_addr) PlagueGroundEffect(position, damage, 64, 128, 16, 20, 48);
                }
                xport_draft_host_sub_80069EF4_p2(116, position, 0);
            }
            substate = r_u16(object + 474);
            if (!substate)
            {
                if (r_u32(object + 396) & 1)
                    return;
                if ((sint16)r_u16(object + 218) < 560)
                {
                    value = visual_lists.solids;
                    second = r_u16(object + 536);
                    ::ActivateObjectsByIndex(value, second);
                    value = visual_lists.solids;
                    second = r_u16(object + 538);
                    ::ActivateObjectsByIndex(value, second);
                    goto advance_state;
                }
                value = r_u8(object + 500) + 1;
                w_u8(object + 500, value);
                if ((value & 255) >= 2)
                    w_u8(object + 500, 0);
                value = r_u8(object + 500);
                apocalypse_trigger_position(position, r_u16(object + 532 + 2 * value));
                ground = (sint32)apocalypse_ground_native(position, 512, 2048);
                radius = (sint16)r_u16(object + 456);
                position[1] = (uint32)ground - ((uint32)radius << 12);
                SetJumpTarget(position);
                value = r_u16(object + 474);
                w_u8(object + 501, 0);
                goto store_substate;
            }
            if (substate != 1 || (r_u32(object + 396) & 1))
                return;
            phase = r_u8(object + 501);
            if (phase >= 2 || (phase == substate && RandomRange(2)))
                goto far_jump_and_repeat;
            if ((sint16)r_u16(object + 218) < 560)
                goto far_jump_and_repeat;
            phase = r_u8(object + 501);
            state = r_u16(object + 472);
            substate = r_u16(object + 474);
            w_u16(object + 472, 16);
            w_u16(object + 474, 0);
            w_u8(object + 501, phase + 1);
            w_u16(object + 440, state);
            w_u16(object + 442, substate);
            return;

        case 0x80045F30:
            flags = r_u32(object + 396);
            w_u16(object + 504, 2);
            if (!(flags & 128))
            {
                increment = (sint16)r_u16(object + 460);
                if (increment >= 101)
                {
                    value = r_u32(object + 572);
                    w_u32(object + 396, flags | 128);
                    ::StopSoundVoices(value);
                }
                else
                    w_u16(object + 460, (uint32)increment + 1);
            }
            substate = r_u16(object + 474);
            if (!substate)
            {
                value = visual_lists.solids;
                second = r_u16(object + 510);
                ::ActivateObjectsByIndex(value, second);
                value = sub_80069DF0(115, 8192, 0);
                w_u32(object + 572, value);
            }
            if (substate <= 1)
            {
                if (r_u32(object + 396) & 1)
                    return;
                random = RandomRange(2);
                apocalypse_trigger_position(position, r_u16(object + 532 + 2 * random));
                ground = (sint32)apocalypse_ground_native(position, 512, 2048);
                radius = (sint16)r_u16(object + 456);
                position[1] = (uint32)ground - ((uint32)radius << 12);
                SetJumpTarget(position);
                goto advance_substate;
            }
            if (substate != 2)
                return;
            flags = r_u32(object + 396);
            if (flags & 1)
                return;
            if ((sint16)r_u16(object + 218) < 320 && (flags & 128))
            {
                value = visual_lists.solids;
                second = r_u16(object + 510);
                ::ActivateObjectsByIndex(value, second);
                w_u32(object + 396, r_u32(object + 396) & 0xFFFFFF7F);
                SelectPlagueFarTarget(object, position);
                ground = (sint32)apocalypse_ground_native(position, 512, 2048);
                radius = (sint16)r_u16(object + 456);
                position[1] = (uint32)ground - ((uint32)radius << 12);
                SetJumpTarget(position);
                value = r_u16(object + 472);
                w_u16(object + 474, 0);
                w_u16(object + 460, 0);
                goto store_state;
            }
            goto far_jump_and_repeat;

        case 0x80046170:
            substate = r_u16(object + 474);
            w_u16(object + 504, 3);
            if (!substate)
            {
                if (r_u32(object + 396) & 1)
                    return;
                StartAnimation(0, 1);
                TrackTarget((const uint8 *)psx_addr(visual_lists.players, 80));
                goto advance_substate;
            }
            if (substate != 1)
                return;
            increment = (sint16)r_u16(object + 460);
            if (increment < 161)
                w_u16(object + 460, (uint32)increment + 1);
            else
            {
                ClearFacing();
                goto advance_state;
            }
            return;

        case 0x80046204:
            substate = r_u16(object + 474);
            w_u16(object + 504, 4);
            if (!substate)
            {
                value = r_u16(object + 474);
                w_u16(object + 436, 42);
                goto store_substate;
            }
            if (substate == 1)
            {
                if (r_u32(object + 396) & 1)
                    return;
                if (this->model.animation != 9)
                    SetAnimationRange(9, 0, 0xFFFFFFFF);
                else if (r_u8(object + 303))
                    w_u16(object + 474, 2);
                return;
            }
            if (substate != 2)
                return;
            if ((sint16)r_u16(object + 218) <= 0)
            {
                w_u32(object + 396, r_u32(object + 396) & 0xFFFFFFFE);
                ClearPlagueVelocity(object);
                w_u16(object + 472, 5);
                w_u16(object + 474, 0);
                return;
            }
            if (this->model.animation != 6)
                StartAnimation(6, 1);
            flags = r_u32(object + 396);
            if (!(flags & 1) || !RandomRange(12))
            {
                if (RandomRange(3))
                {
                    hero = visual_lists.players;
                    position[0] = r_u32(hero + 4);
                    position[1] = r_u32(hero + 8);
                    position[2] = r_u32(hero + 12);
                }
                else
                {
                    hero = visual_lists.players;
                    scalar = 8;
                    position[0] = r_u32(hero + 4);
                    position[1] = r_u32(hero + 8);
                    position[2] = r_u32(hero + 12);
                    MultiplyScalarVector(velocity, &scalar, VisualObject::Attach(hero).motion.velocity);
                    velocity[1] = 0;
                    scalar = 8;
                    xport_draft_host_sub_8006C47C_p123(scaled, &scalar, velocity);
                    xport_draft_host_sub_8006C0B8_p12(position, scaled);
                }
                ground = (sint32)apocalypse_ground_native(position, 512, 2048);
                if (ground == -1)
                    position[1] = this->model.pos[1];
                else
                {
                    radius = (sint16)r_u16(object + 456);
                    position[1] = (uint32)ground - ((uint32)radius << 12);
                }
                SetMoveTarget(position);
            }
            increment = (sint16)r_u16(object + 462);
            if (increment)
                value = (uint32)increment - 1;
            else
            {
                position[0] = this->model.pos[0];
                position[1] = this->model.pos[1];
                position[2] = this->model.pos[2];
                y = position[1];
                radius = (sint16)r_u16(object + 456);
                w_u32(0x800FF3AC, 0);
                position[1] = y + 0xFFFF0000 + ((uint32)radius << 12);
                child = sub_80032DC0(172);
                if (child)
                {
                    health = (sint16)r_u16(object + 218);
                    damage = (uint32)((sint32)(150 * (240 - (uint32)health)) / 240) + 150;
                    void *dram_addr = AllocateNativeObjectStorage(child, sizeof(PlagueArea));
                    new (dram_addr) PlagueArea();
                    sub_80033874(child);
                    TexturedQuad::Attach(child).SelectResource(0x019D23F9);
                    w_u16(child + 64, 128);
                    direction[2] = 0;
                    direction[0] = 0;
                    direction[1] = -4096;
                    random = RandomRange(48);
                    TexturedQuad::Attach(child).SetColors((random + 64) & 255, 128, 0);
                    w_u32(child + 168, 0);
                    value = r_u32(child + 168);
                    TexturedQuad::Attach(child).SetPosition(position, direction, value, 0);
                    w_u16(child + 10, damage);
                    xport_draft_host_sub_8006C34C_p1(sum1, child + 24, child + 72);
                    xport_draft_host_sub_8006C34C_p12(sum2, sum1, child + 84);
                    xport_draft_host_sub_8006C34C_p12(sum3, sum2, child + 96);
                    scalar = 2;
                    xport_draft_host_sub_8006C564_p123(sum4, sum3, &scalar);
                    x = sum4[0];
                    y = sum4[1];
                    z = sum4[2];
                    w_u32(child + 156, x);
                    w_u32(child + 160, y);
                    w_u32(child + 164, z);
                }
                w_u32(0x800FF3AC, 1);
                value = 5;
            }
            w_u16(object + 462, value);
            hero = visual_lists.players;
            if (sub_80066918(object + 4, hero + 4) < 256)
                SavePlagueRecovery(object, 32);
            return;

        case 0x80046628:
            substate = r_u16(object + 474);
            if (!substate)
            {
                hero = visual_lists.players;
                child = visual_lists.cameras;
                w_u32(hero + 452, 900);
                if (child)
                {
                    w_u32(child + 304, 1);
                    w_u32(child + 296, object);
                    w_u32(child + 340, 1);
                    w_u32(child + 336, object);
                    sub_800785D8(child, 1584, 16);
                    sub_80078618(child, (uint32)-1208, 16);
                    sub_8007865C(child, 0, 16);
                    sub_800786A0(child, (uint32)-76, 16);
                    sub_800786E4(child, 240, 16);
                    sub_8007851C(child, 2496, 16);
                }
                SetAnimationRange(8, 0, 0xFFFFFFFF);
                goto advance_substate;
            }
            if (substate == 3)
            {
                CompleteLevel(0);
                return;
            }
            if (substate == 1)
            {
                if (r_u8(object + 303))
                {
                    SetAnimationRange(4, 0, 0xFFFFFFFF);
                    value = r_u16(object + 474);
                    w_u16(object + 460, 0);
                    w_u16(object + 462, 16);
                    w_u16(object + 474, value + 1);
                }
            }
            else if (substate != 2)
                return;
            increment = (sint8)this->model.frame;
            timer = r_u16(object + 460);
            if (increment >= 45)
            {
                direction[0] = 512;
                w_u32(0x800FF3AC, 0);
                direction[1] = 4096;
                direction[2] = 0;
                SetParticleSpread((const sint16 *)direction);
                ::SetParticleSize(2);
                random = RandomRange(64);
                sub_800350FC((random + 48) & 255, 128, 0);
                sub_80035110(1, 2, 0);
                xport_draft_host_sub_8007CC10_p1(position, object, 0);
                xport_draft_host_sub_80066B8C_p13(aim, object + 4, position);
                velocity[0] = position[0];
                velocity[1] = position[1];
                velocity[2] = position[2];
                direction[0] = -1024;
                direction[1] = 0;
                direction[2] = 0;
                velocity[1] += 0xFF800000;
                child = sub_80032DC0(108);
                if (child)
                    {
                        void *dram_addr = AllocateNativeObjectStorage(child, sizeof(TrailProjectile));
                        new (dram_addr) TrailProjectile(position, velocity, (const uint16 *)direction, 16, 64, 64, 200, 240, 200, 0, 128, 0);
                    }
                xport_draft_host_sub_8007CC10_p1(position, object, 1);
                xport_draft_host_sub_80066B8C_p13(aim, object + 4, position);
                velocity[0] = position[0];
                velocity[1] = position[1];
                velocity[2] = position[2];
                velocity[1] += 0xFF800000;
                child = sub_80032DC0(108);
                if (child)
                    {
                        void *dram_addr = AllocateNativeObjectStorage(child, sizeof(TrailProjectile));
                        new (dram_addr) TrailProjectile(position, velocity, (const uint16 *)direction, 16, 64, 64, 200, 240, 200, 0, 128, 0);
                    }
                w_u32(0x800FF3AC, 1);
                if ((sint8)this->model.frame >= 68)
                    this->model.frame = 68;
            }
            if (!RandomRange(12))
            {
                random = RandomRange(2);
                sub_80069EF4(random + 1, object + 4, 0);
                child = sub_80032DC0(124);
                if (child)
                    sub_8001EB64(child, object + 4, 64, 128, 0, 512, 10);
                child = sub_80032DC0(124);
                if (child)
                    sub_8001CF9C(child, object + 4, 5, 255, 255, 255, 1, 32, 128, 0, 1, 60, 0, 1, 100, 120, 80, 100, 10, 4);
            }
            if (timer > 180)
            {
                increment = (sint16)this->model.scale[0];
                value = this->model.scale[0] - 128;
                if (increment > 512)
                {
                    second = this->model.scale[2];
                    this->model.scale[0] = value;
                    this->model.scale[2] = second - 128;
                }
                else
                {
                    increment = (sint16)this->model.scale[1];
                    value = this->model.scale[1] - 512;
                    if (increment > 512)
                        this->model.scale[1] = value;
                    else
                        w_u16(object + 474, r_u16(object + 474) + 1);
                }
            }
            w_u16(object + 460, timer + 1);
            increment = (sint16)r_u16(object + 462);
            angle = this->model.angles[1];
            this->model.angles[1] = angle + (uint32)increment;
            w_u16(object + 462, (uint32)increment + 1);
            return;

        case 0x80046ABC:
            substate = r_u16(object + 474);
            if (!substate)
            {
                flags = r_u32(object + 396);
                hero = visual_lists.players;
                w_u32(object + 396, flags & 0xFFFFFFFE);
                ClearPlagueVelocity(object);
                TrackTarget((const uint8 *)psx_addr(hero, 80));
                SetAnimationRange(2, 0, 0xFFFFFFFF);
                xport_draft_host_sub_8007CC10_p1(position, object, 0);
                child = sub_80032DC0(104);
                if (child)
                    child = apocalypse_ring_effect_native(child, position, 32, 64, 128, 128, 128, 16, 64, 0);
                w_u32(object + 648, child);
                sub_80034994(child, 16);
                goto advance_substate;
            }
            if (substate == 2)
                goto restore_recovery;
            if (substate != 1)
                return;
            xport_draft_host_sub_8007CC10_p1(position, object, 0);
            if ((sint8)this->model.frame < 37)
            {
                child = r_u32(object + 648);
                xport_draft_host_sub_80032EE4_p2(child, position);
                increment = (sint8)this->model.frame;
                child = r_u32(object + 648);
                sub_80034994(child, ((uint32)increment << 2) + 16);
                return;
            }
            child = r_u32(object + 648);
            if (child)
            {
                value = r_u32(child + 68);
                increment = (sint16)r_u16(value + 8);
                target = r_u32(value + 12);
                apocalypse_object_cleanup(target, child + (uint32)increment, 3);
                w_u32(object + 648, 0);
            }
            hero = visual_lists.players;
            xport_draft_host_sub_80066B8C_p12(aim, position, hero + 4);
            child = sub_80032DC0(268);
            if (child)
            {
                speed = GetBehaviorValue(0, 2) & 65535;
                damage = GetBehaviorValue(0, 1) & 65535;
                apocalypse_line_projectile_native(child, position, aim, speed, damage, 0x800FF5A0, 64, 96, 0, 0);
            }
            for (phase = 0; phase < 4096; phase += 256)
            {
                increment = rcos(phase & 4095);
                value = r_u8(0x800A6BE4 + (phase >> 8));
                x = (uint32)increment * value;
                increment = rsin(phase & 4095);
                y = (uint32)increment * value;
                direction[2] = 0;
                direction[0] = (sint16)(((uint32)(uint16)aim[0] - (uint32)((sint32)x >> 12)) & 4095);
                direction[1] = (sint16)(((uint32)(uint16)aim[1] + (uint32)((sint32)y >> 12)) & 4095);
                child = sub_80032DC0(268);
                if (child)
                {
                    speed = GetBehaviorValue(0, 2) & 65535;
                    damage = GetBehaviorValue(0, 1) & 65535;
                    apocalypse_line_projectile_native(child, position, direction, speed, damage, 0x800FF5A0, 64, 96, 0, 0);
                }
            }
            sub_80069EF4(117, object + 4, 0);
            goto advance_substate;

        case 0x80046DB4:
            substate = r_u16(object + 474);
            if (!substate)
            {
                flags = r_u32(object + 396);
                hero = visual_lists.players;
                w_u32(object + 396, flags & 0xFFFFFFFE);
                ClearPlagueVelocity(object);
                TrackTarget((const uint8 *)psx_addr(hero, 80));
                SetAnimationRange(7, 0, 0xFFFFFFFF);
                goto advance_substate;
            }
            if (substate == 1)
            {
                if (!r_u8(object + 303))
                    return;
                increment = (sint8)this->model.frame;
                SetAnimationRange(7, (uint32)increment, 0);
                goto advance_substate;
            }
            if (substate == 2)
                goto restore_recovery;
            return;

        case 0x80046E90:
            return;
        default:
            GameDiagnostic("TODO 80045BA8 state table target %08X\n", target);
            abort();
    }

far_jump_and_repeat:
    SelectPlagueFarTarget(object, position);
    ground = (sint32)apocalypse_ground_native(position, 512, 2048);
    radius = (sint16)r_u16(object + 456);
    position[1] = (uint32)ground - ((uint32)radius << 12);
    SetJumpTarget(position);
    value = r_u16(object + 474);
    w_u16(object + 474, value - 1);
    return;

advance_state:
    value = r_u16(object + 472);
    w_u16(object + 474, 0);
store_state:
    w_u16(object + 472, value + 1);
    return;

advance_substate:
    value = r_u16(object + 474);
store_substate:
    w_u16(object + 474, value + 1);
    return;

restore_recovery:
    if (!r_u8(object + 303))
        return;
    ClearFacing();
    state = r_u16(object + 440);
    substate = r_u16(object + 442);
    w_u16(object + 472, state);
    w_u16(object + 474, substate);
}

uint32 Plague::SpawnPickup()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 state, now, elapsed, index, trigger, choice, resource;
    uint32 position[3];
    sint32 health, maximum;
    FUNCTION_MARKER(0x80046EBC, "SLUS_003.73");
    state = r_u16(object + 504);
    now = r_u32(0x800FF2F0);
    elapsed = now - r_u32(object + 496);
    if ((sint32)elapsed < 481)
        return 0xFFFFFFFF;
    if (state - 1 < 2)
    {
        if (state == 2)
        {
            maximum = (sint32)(sint16)r_u16(0x800EC526);
            now = visual_lists.players;
            health = (sint32)(sint16)r_u16(now + 218);
            maximum = (maximum + (maximum < 0 ? 1 : 0)) >> 1;
            if (health < maximum)
            {
                index = RandomRange(2);
                trigger = r_u16(object + 544 + 2 * index);
                xport_draft_host_sub_8006613C_p1(position, trigger);
                resource = 14;
                goto create;
            }
        }
        choice = RandomRange(3);
        if (!choice)
        {
            index = RandomRange(2);
            trigger = r_u16(object + 544 + 2 * index);
            xport_draft_host_sub_8006613C_p1(position, trigger);
            resource = 5;
            goto create;
        }
        index = RandomRange(4);
        trigger = r_u16(object + 548 + 2 * index);
    }
    else
    {
        choice = RandomRange(3);
        if (choice == 1)
        {
            index = RandomRange(2);
            trigger = r_u16(object + 568 + 2 * index);
            xport_draft_host_sub_8006613C_p1(position, trigger);
            resource = 14;
            goto create;
        }
        if (choice)
        {
            index = RandomRange(2);
            trigger = r_u16(object + 564 + 2 * index);
            xport_draft_host_sub_8006613C_p1(position, trigger);
            resource = 15;
            goto create;
        }
        index = RandomRange(4);
        trigger = r_u16(object + 556 + 2 * index);
    }
    xport_draft_host_sub_8006613C_p1(position, trigger);
    choice = RandomRange(2);
    resource = choice ? 3 : 4;
create:
    xport_draft_host_sub_80020EF8_p2(resource, position, 4, 240, 0x800A71CC);
    now = r_u32(0x800FF2F0);
    w_u32(object + 496, now);
    return now;
}

uint32 Plague::UpdateOrbitingWeapons()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 node = visual_lists.solids, identity, child, second, third;
    uint32 timer, phase, angle, slot, table, receiver, target, index, random;
    uint32 position[3], direction[3];
    sint16 angles[3];
    FUNCTION_MARKER(0x800470B0, "SLUS_003.73");
    w_u32(object + 576, 0);
    if (node)
    {
        identity = r_u16(object + 510);
        while (r_u16(node + 214) != identity)
        {
            node = r_u32(node + 28);
            if (!node)
                break;
        }
        if (node)
            w_u32(object + 576, node);
    }
    if (r_u32(object + 396) & 128)
    {
        xport_draft_host_sub_8006613C_p1(position, r_u16(object + 512));
        child = r_u32(object + 620);
        if (!child)
        {
            w_u16(object + 634, 1365);
            w_u16(object + 632, 0);
            w_u16(object + 636, 2730);
            CreatePlagueOrbitWeapon(object, 620);
        }
        else
        {
            second = r_u32(object + 624);
            if (!second)
            {
                if (r_u16(object + 640) >= 151)
                    CreatePlagueOrbitWeapon(object, 624);
                else
                {
                    timer = r_u16(object + 640);
                    w_u32(child + 72, (uint32)((sint32)(timer << 12) / 150 / 9));
                }
            }
            else
            {
                third = r_u32(object + 628);
                if (!third)
                {
                    if (r_u16(object + 640) >= 301)
                        CreatePlagueOrbitWeapon(object, 628);
                    else
                    {
                        timer = r_u16(object + 640) - 150;
                        w_u32(second + 72, (uint32)((sint32)(timer << 12) / 150 / 9));
                    }
                }
                else if (r_u16(object + 640) < 450)
                {
                    timer = r_u16(object + 640) - 300;
                    w_u32(third + 72, (uint32)((sint32)(timer << 12) / 150 / 9));
                }
            }
        }
        for (index = 0; index < 3; ++index)
        {
            uint32 delta = ReadPlagueOrbitDelta(object);
            angle = (r_u16(object + 632 + 2 * index) + delta) & 4095;
            w_u16(object + 632 + 2 * index, angle);
            slot = object + 620 + 4 * index;
            if (r_u32(slot))
            {
                angles[0] = 0;
                angles[1] = (sint16)angle;
                angles[2] = 0;
                xport_draft_host_sub_800667CC_p13(direction, 145, angles);
                xport_draft_host_sub_8006C0B8_p12(direction, position);
                child = r_u32(slot);
                table = r_u32(child);
                receiver = child + (uint32)(sint32)(sint16)r_u16(table + 16);
                target = r_u32(table + 20);
                apocalypse_orbit_fire_native(target, receiver, direction, angles);
            }
        }
        node = r_u32(object + 576);
        if (node)
        {
            uint32 delta = ReadPlagueOrbitDelta(object);
            angle = (r_u16(node + 18) + delta) & 4095;
            w_u16(node + 18, angle);
        }
        random = RandomRange(4);
        phase = r_u16(object + 638);
        timer = r_u16(object + 640);
        w_u16(object + 638, phase + 4 + random);
        w_u16(object + 640, timer + 1);
        return random;
    }
    node = r_u32(object + 576);
    if (node)
        w_u16(node + 18, (r_u16(node + 18) + 32) & 4095);
    for (index = 0; index < 3; ++index)
    {
        slot = object + 620 + 4 * index;
        child = r_u32(slot);
        if (child)
        {
            table = r_u32(child);
            receiver = child + (uint32)(sint32)(sint16)r_u16(table + 8);
            target = r_u32(table + 12);
            apocalypse_object_cleanup(target, receiver, 3);
            w_u32(slot, 0);
        }
    }
    return 0;
}

uint32 Plague::ReadParameters(uint32 command)
{
    const uint32 object = NativeObjectGuestAddress(this);
    static const uint32 fields[8] = {536, 538, 512, 506, 540, 542, 544, 546};
    uint32 cursor, value, index, cached;
    FUNCTION_MARKER(0x800474F0, "SLUS_003.73");
    command &= 0xFFFF;
    if (command != 0x2130)
        return Enemy::ReadParameters(command);
    cursor = r_u32(object + 400) + 2;
    w_u32(object + 400, cursor);
    value = r_u16(cursor);
    cursor += 2;
    w_u32(object + 400, cursor);
    w_u16(object + 508, value);
    for (index = 0; index < 8; ++index)
    {
        cursor = r_u32(object + 400);
        value = r_u16(cursor);
        cursor += 2;
        w_u16(object + 516 + 2 * index, value);
        w_u32(object + 400, cursor);
    }
    cached = cursor;
    value = r_u16(cached);
    w_u32(object + 400, cached + 2);
    w_u16(object + 510, value);
    value = r_u16(cached + 2);
    w_u32(object + 400, cached + 36);
    w_u16(object + 514, value);
    for (index = 0; index < 2; ++index)
    {
        cursor = r_u32(object + 400);
        value = r_u16(cursor);
        cursor += 2;
        w_u16(object + 532 + 2 * index, value);
        w_u32(object + 400, cursor);
    }
    cached = cursor;
    for (index = 0; index < 8; ++index)
    {
        value = r_u16(cached + 2 * index);
        w_u32(object + 400, cached + 2 * index + 2);
        w_u16(object + fields[index], value);
    }
    for (index = 0; index < 8; ++index)
    {
        cursor = r_u32(object + 400);
        value = r_u16(cursor);
        cursor += 2;
        w_u16(object + 548 + 2 * index, value);
        w_u32(object + 400, cursor);
    }
    cached = cursor;
    for (index = 0; index < 4; ++index)
    {
        value = r_u16(cached + 2 * index);
        w_u32(object + 400, cached + 2 * index + 2);
        w_u16(object + 564 + 2 * index, value);
    }
    return cached + 8;
}

uint32 Plague::ExecuteCommand(uint32 command)
{
    FUNCTION_MARKER(0x800476D0, "SLUS_003.73");
    return Enemy::ExecuteCommand(command & 0xFFFF);
}

uint32 Plague::SetJumpTarget(const uint32 *position)
{
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x800476F0, "SLUS_003.73");
    SetAnimationRange(5, 0, (uint32)-1);
    uint32 x = ReadPlagueTargetWord(position, 0);
    uint32 y = ReadPlagueTargetWord(position, 1);
    uint32 z = ReadPlagueTargetWord(position, 2);
    w_u32(object + 484, x);
    w_u32(object + 488, y);
    w_u32(object + 492, z);
    uint32 flags = r_u32(object + 396);
    w_u16(object + 502, 40);
    flags |= 5;
    w_u32(object + 396, flags);
    return flags;
}

uint32 Plague::SetMoveTarget(const uint32 *position)
{
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x80047758, "SLUS_003.73");
    uint32 x = ReadPlagueTargetWord(position, 0);
    uint32 y = ReadPlagueTargetWord(position, 1);
    uint32 z = ReadPlagueTargetWord(position, 2);
    w_u32(object + 484, x);
    w_u32(object + 488, y);
    w_u32(object + 492, z);
    uint32 flags = (r_u32(object + 396) | 1) & 0xFFFFFFF3;
    w_u32(object + 396, flags);
    return flags;
}

uint32 Plague::TrackTarget(const uint8 *target)
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 address = GuestMemoryAddress(target);
    if (!address)
    {
        // TODO Bind native target data to the original object layout
        GameDiagnostic("TODO Plague native target data\n");
        abort();
    }
    FUNCTION_MARKER(0x80047788, "SLUS_003.73");
    uint32 flags = sub_80062F48(address);
    if (flags)
        return flags;
    flags = r_u32(object + 396);
    w_u32(object + 580, address);
    flags = (flags & 0xFFFFFFDF) | 16;
    w_u32(object + 396, flags);
    return flags;
}

uint16 *Plague::ClearFacing()
{
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x80047808, "SLUS_003.73");
    uint32 flags = r_u32(object + 396) & 0xFFFFFFCF;
    w_u32(object + 396, flags);
    motion.angular_velocity[2] = 0;
    motion.angular_velocity[1] = 0;
    motion.angular_velocity[0] = 0;
    motion.angular_acceleration[2] = 0;
    motion.angular_acceleration[1] = 0;
    motion.angular_acceleration[0] = 0;
    return motion.angular_acceleration;
}





extern "C" uint32 sub_800452B4(uint32 object, uint32 reason)
{
    FUNCTION_MARKER(0x800452B4, "SLUS_003.73");
    return Plague::Attach(object).Plague::Destroy(reason);
}

extern "C" uint32 sub_80045B14(uint32 object, uint32 damage, uint32, uint32 reason)
{
    FUNCTION_MARKER(0x80045B14, "SLUS_003.73");
    return Plague::Attach(object).Plague::Damage(damage, 0, reason);
}

extern "C" uint32 sub_800453F4(uint32 object)
{
    FUNCTION_MARKER(0x800453F4, "SLUS_003.73");
    return Plague::Attach(object).Plague::Update();
}







extern "C" uint32 sub_800474F0(uint32 object, uint32 command)
{
    FUNCTION_MARKER(0x800474F0, "SLUS_003.73");
    return Plague::Attach(object).Plague::ReadParameters(command);
}

extern "C" uint32 sub_800476D0(uint32 object, uint32 command)
{
    FUNCTION_MARKER(0x800476D0, "SLUS_003.73");
    return Plague::Attach(object).Plague::ExecuteCommand(command);
}



extern "C" uint32 apocalypse_enemy_jump_target_native(uint32 object, const void *position)
{
    return Plague::Attach(object).SetJumpTarget((const uint32 *)position);
}



extern "C" uint32 apocalypse_enemy_move_target_native(uint32 object, const void *position)
{
    return Plague::Attach(object).SetMoveTarget((const uint32 *)position);
}




