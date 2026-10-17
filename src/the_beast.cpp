#include "cd_voice_event.h"
#include "enemy_list.h"
#include "record_factory.h"
#include "screen_flash.h"
#include "visual_object.h"
#include "angle_math.h"
#include "object_damage.h"
#include "object_collision_events.h"
#include "model_animation.h"
#include "game_random.h"
#include "game_sdk_bindings.h"
extern "C"
{
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
}
#include "the_beast.h"
#include "projectile_guided.h"
#include "game_panel.h"
#include "spawn_points.h"
#include "native_object_storage.h"
#include "game_heap.h"
#include "game_object_allocation.h"
#include <new>
#include <stdio.h>
#include <stdlib.h>

// Native extraction of the original bone-vector and normalization call sequence

#include "level_session.h"
#include "fire_cycle.h"

#include "beast_final_visual.h"
#include "beast_effects.h"

// Native extractions of repeated blocks in the original behavior function

#include "object_parameters.h"
#include "record_data.h"
#include "vector_arithmetic.h"
#include <string.h>

// Native input bridge for the original three target-word loads

static void DestroyTheBeastChild(uint32 child, uint32 table_offset)
{
    uint32 table, target;
    sint32 adjustment;
    if (child)
    {
        table = r_u32(child + table_offset);
        adjustment = (sint16)r_u16(table + 8);
        target = r_u32(table + 12);
        apocalypse_object_cleanup(target, child + (uint32)adjustment, 3);
    }
}

TheBeast &TheBeast::Attach(uint32 object)
{
    return *static_cast<TheBeast *>(FindNativeObjectStorage(object));
}

static uint32 DivideBeastMovement(uint32 numerator, uint32 denominator)
{
    if (!denominator)
        return (sint32)numerator < 0 ? 1 : (uint32)-1;
    if (numerator == 0x80000000 && denominator == (uint32)-1)
        return numerator;
    return (uint32)((sint32)numerator / (sint32)denominator);
}

static uint32 ReadBeastPlanarDistance(const uint32 *left, const uint32 *right)
{
    uint32 x = left[0], other_x = right[0], z, other_z;
    x = (uint32)((sint32)(x - other_x) >> 12);
    z = left[2];
    other_z = right[2];
    z = (uint32)((sint32)(z - other_z) >> 12);
    return sub_80085B54(x * x + z * z);
}

static void InterpolateBeastAngles(uint32 current, const uint16 *desired, uint16 *delta, uint16 *acceleration, uint32 scale)
{
    SVECTOR motion_cur, motion_target;
    uint32 motion_cur_word, motion_cur_tail, motion_target_word, motion_target_tail;
    uint32 packed = r_u32(current);
    packed = r_u32(current);
    uint32 tail = r_u16(current + 4);
    uint32 desired_packed = desired[0] | ((uint32)desired[1] << 16);
    uint32 desired_tail = desired[2];
    motion_cur_word = packed;
    motion_cur_tail = tail;
    motion_target_word = desired_packed;
    motion_target_tail = desired_tail;
    motion_cur.vx = (sint16)motion_cur_word;
    motion_cur.vy = (sint16)(motion_cur_word >> 16);
    motion_cur.vz = (sint16)motion_cur_tail;
    motion_cur.pad = (sint16)(motion_cur_tail >> 16);
    motion_target.vx = (sint16)motion_target_word;
    motion_target.vy = (sint16)(motion_target_word >> 16);
    motion_target.vz = (sint16)motion_target_tail;
    motion_target.pad = (sint16)(motion_target_tail >> 16);
    UpdateAngleAcceleration(motion_cur, delta, acceleration, motion_target, scale);
}

static void FireBeastWeapon(uint32 weapon, const uint32 *position, const uint16 *angles)
{
    uint32 table = r_u32(weapon);
    sint32 adjustment = (sint16)r_u16(table + 16);
    uint32 target = r_u32(table + 20);
    uint32 receiver = weapon + (uint32)adjustment;
    if (target == 0x80027978)
        apocalypse_beam_fire_27978(receiver, position, (const sint16 *)angles);
    else if (target == 0x80024598)
        apocalypse_beam_24598_native(receiver, position, angles);
    else
    {
        GameDiagnostic("TODO TheBeast weapon fire target %08X\n", target);
        abort();
    }
}

static void DestroyBeastWeapon(uint32 weapon)
{
    uint32 table = r_u32(weapon);
    sint32 adjustment = (sint16)r_u16(table + 8);
    uint32 target = r_u32(table + 12);
    apocalypse_object_cleanup(target, weapon + (uint32)adjustment, 3);
}

static void ReadTheBeastLightDirection(uint32 object, uint32 bone, uint32 horizontal, sint16 direction[3])
{
    uint32 vector[4], normalized[4], lane;
    xport_draft_host_sub_8007CC10_p1(vector, object, bone);
    for (lane = 0; lane < 3; ++lane)
        vector[lane] -= r_u32(object + 4 + lane * 4);
    if (horizontal)
        vector[1] = 0;
    xport_draft_host_sub_800666DC_p12(normalized, vector);
    direction[0] = (sint16)normalized[0];
    direction[1] = horizontal ? 0 : (sint16)normalized[1];
    direction[2] = (sint16)normalized[2];
}

static void AdvanceBeastPhase(uint32 object)
{
    uint32 phase = r_u16(object + 474);
    w_u16(object + 474, phase + 1);
}

static void SelectBeastState(uint32 object, uint32 state)
{
    w_u16(object + 472, state);
    w_u16(object + 474, 0);
}

static void RestoreBeastState(uint32 object)
{
    uint32 state = r_u16(object + 440);
    uint32 phase = r_u16(object + 442);
    w_u16(object + 472, state);
    w_u16(object + 474, phase);
}

static void ClearBeastVelocity(uint32 object)
{
    TheBeast::Attach(object).motion.velocity[2] = 0;
    TheBeast::Attach(object).motion.velocity[1] = 0;
    TheBeast::Attach(object).motion.velocity[0] = 0;
}

static void BeginBeastAttack(TheBeast &owner)
{
    const uint32 object = NativeObjectGuestAddress(&owner);
    uint32 flags = r_u32(object + 396);
    uint32 player = visual_lists.players;
    w_u32(object + 396, flags & 0xFFFFFFFE);
    ClearBeastVelocity(object);
    owner.TrackTarget(player);
}

static void PlayBeastAnimation(uint32 object, uint32 animation)
{
    sub_80063038(object, animation, 0, (uint32)-1);
}

static void JumpBeastToPoint(TheBeast &owner, uint32 *position)
{
    const uint32 object = NativeObjectGuestAddress(&owner);
    uint32 height;
    owner.ClearFacing();
    height = (uint32)(sint32)(sint16)r_u16(object + 456);
    position[1] -= height << 12;
    owner.SetJumpDestination(position);
}

static uint32 ReadBeastPointDistance(const uint32 *position, uint32 target)
{
    uint32 x = position[0], other_x = r_u32(target), z, other_z;
    x = (uint32)((sint32)(x - other_x) >> 12);
    z = position[2];
    other_z = r_u32(target + 8);
    z = (uint32)((sint32)(z - other_z) >> 12);
    return sub_80085B54(x * x + z * z);
}

static uint32 ReadBeastBeamPoint(uint32 object)
{
    uint32 index;
    if (r_u16(object + 444))
    {
        index = (uint32)(sint32)(sint16)r_u16(object + 464);
        return r_u16(0x800A6BAC + index * 2);
    }
    return r_u16(0x800FF4BC);
}

static void PositionBeastBeamPoint(uint32 object, uint32 *position, uint16 *angles, uint32 turn)
{
    uint32 distance, yaw, offset;
    xport_draft_host_sub_80066B8C_p13(angles, object + 4, position);
    if (turn)
    {
        yaw = angles[1];
        offset = r_u16(object + 446);
        angles[1] = turn == 1 ? yaw + offset : yaw - offset;
    }
    distance = xport_draft_host_sub_8006689C_p2(object + 4, position);
    xport_draft_host_sub_800667CC_p13(position, distance, angles);
    xport_draft_host_sub_8006C0B8_p1(position, object + 4);
}

static uint32 ReadTheBeastTargetWord(const uint32 *position, uint32 axis)
{
    uint32 address = GuestMemoryAddress(position), value;
    if (address)
        return r_u32(address + axis * 4);
    memcpy(&value, (const uint8 *)position + axis * 4, 4);
    return value;
}

TheBeast::TheBeast(const uint8 *record_data, uint32 index)
    : Enemy()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 cursor, roll;
    const uint32 data = GuestMemoryAddress(record_data);
    FUNCTION_MARKER(0x800408A0, "SLUS_003.73");
    if (!data)
    {
        // TODO Support native record data in the original parser adapter
        GameDiagnostic("TheBeast record data is outside guest memory\n");
        abort();
    }
    w_u32(object + 68, 0x800A20C0);
    cursor = sub_80062B0C(object, data);
    this->model.angles[0] = r_u16(cursor);
    this->model.angles[1] = r_u16(cursor + 2);
    roll = r_u16(cursor + 4);
    w_u32(object + 400, cursor + 6);
    w_u8(object + 380, 1);
    this->record_idx = index;
    this->model.angles[2] = roll;
    Configure();
    cursor = r_u32(object + 400);
    sub_8004BE90(object, cursor);
}

uint32 TheBeast::Configure()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 buffer, colors;
    FUNCTION_MARKER(0x800409E0, "SLUS_003.73");
    sub_800626C8(object, 0x800A2040);
    motion.drag[0] = 31;
    motion.drag[1] = 31;
    motion.drag[2] = 31;
    AddToList(visual_lists.actors);
    this->model.flags = this->model.flags | 128;
    sub_8004BD04(object, 3, 5);
    sub_8004BDCC(object, 0, 0, 0);
    sub_8004BDCC(object, 1, 0, 0);
    sub_8004BDCC(object, 1, 1, 1);
    sub_8004BDCC(object, 2, 0, 0);
    sub_8004BDCC(object, 2, 1, 2);
    w_u16(object + 436, 32);
    w_u16(object + 438, 32);
    this->kind = 220;
    w_u16(object + 456, 271);
    w_u32(object + 552, 0);
    w_u32(object + 556, 0);
    w_u32(object + 560, 0);
    w_u8(object + 454, 0);
    w_u16(object + 472, 0);
    w_u16(object + 474, 0);
    w_u8(object + 381, 0);
    w_u32(0x800FF4EC, object);
    w_u8(object + 314, 1);
    buffer = sub_8006B864(32, 0, 1);
    w_u32(object + 316, buffer);
    colors = sub_8006B864(16, 0, 1);
    buffer = r_u32(object + 316);
    w_u32(object + 320, colors);
    w_u16(buffer, 0);
    buffer = r_u32(object + 316);
    w_u16(buffer + 2, (uint16)-4096);
    buffer = r_u32(object + 316);
    w_u16(buffer + 4, 0);
    colors = r_u32(object + 320);
    w_u8(colors, 128);
    colors = r_u32(object + 320);
    w_u8(colors + 1, 128);
    colors = r_u32(object + 320);
    w_u8(colors + 2, 128);
    return colors;
}

uint32 TheBeast::Destroy(uint32 reason)
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 child, buffer, index, result;
    FUNCTION_MARKER(0x80040B64, "SLUS_003.73");
    w_u32(object + 68, 0x800A20C0);
    ClearTrackedObjectHealth();
    RemoveFromList(visual_lists.actors);
    Enemy::FreeBehaviorTable();
    child = r_u32(object + 552);
    DestroyTheBeastChild(child, 0);
    child = r_u32(object + 556);
    DestroyTheBeastChild(child, 0);
    child = r_u32(object + 560);
    DestroyTheBeastChild(child, 0);
    child = r_u32(object + 572);
    DestroyTheBeastChild(child, 68);
    buffer = r_u32(object + 316);
    if (buffer)
        ReleaseHeapBlock(buffer);
    buffer = r_u32(object + 320);
    if (buffer)
        ReleaseHeapBlock(buffer);
    w_u32(0x800FF4EC, 0);
    for (index = 0; index < 2; ++index)
    {
        child = r_u32(object + 564 + index * 4);
        DestroyTheBeastChild(child, 68);
    }
    Enemy::Destroy(0);
    result = reason & 1;
    if (result)
        result = ReleaseObjectStorage(object);
    return result;
}

uint32 TheBeast::SpawnPickup()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 mode, threshold, player, kind, index, entry, candidate, x, z, child, choice;
    uint32 position[4];
    sint32 timer, health;
    FUNCTION_MARKER(0x80040D14, "SLUS_003.73");
    timer = (sint16)r_u16(object + 462);
    if (timer < 150)
    {
        w_u16(object + 462, timer + 1);
        return timer + 1;
    }
    w_u16(object + 462, 0);
    mode = r_u16(object + 440);
    if (mode < 5)
        return 0x800F0000;
    threshold = r_u16(0x800EC526);
    player = visual_lists.players;
    health = (sint16)r_u16(player + 218);
    kind = 0;
    if (health < ((sint32)(threshold << 16) >> 17) && RandomRange(3) == 0)
        kind = 14;
    if (!kind)
    {
        choice = RandomRange(6);
        switch (choice)
        {
            case 0: kind = 5; break;
            case 1: kind = 3; break;
            case 2: kind = 4; break;
            case 3: kind = 1; break;
            case 4: kind = 10; break;
            default: kind = 14; break;
        }
    }
    mode = r_u16(object + 440);
    if (mode - 5 >= 5)
        return 0;
    entry = 0x800A6BB8;
    ShuffleSpawnPoints((uint16 *)psx_addr(entry, 2), 5);
    for (index = 0; index < 5; ++index)
    {
        choice = r_u16(entry);
        xport_draft_host_sub_8006613C_p1(position, choice);
        if (apocalypse_ground_native(position, 0, 2048) != (uint32)-1)
        {
            candidate = visual_lists.pickups;
            if (candidate)
            {
                x = position[0];
                z = position[2];
                while (candidate)
                {
                    if (r_u32(candidate + 4) == x && r_u32(candidate + 12) == z)
                        break;
                    candidate = r_u32(candidate + 28);
                }
            }
            if (!candidate)
            {
                child = xport_draft_host_sub_80020EF8_p2(kind & 65535, position, 7, 600, 0x800A71CC);
                if (!child)
                    return 0;
                sub_8002005C(child, 8192, 4);
                x = position[1];
                w_u8(child + 308, 0);
                w_u32(child + 312, x + 6553600);
                return 6553600;
            }
        }
        entry += 2;
    }
    return 0;
}

uint32 TheBeast::Update()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 x, y, z, flags, mode, frame, child, timer, value, speed, player, weapon, distance, current, target;
    uint32 position[4], muzzle[4], player_ground[3], discarded_direction[3];
    uint16 desired[4], weapon_angles[4];
    uint32 velocity_x, velocity_z, velocity_y;
    sint16 pitch;
    CollisionRay raycast;
    FUNCTION_MARKER(0x80040F34, "SLUS_003.73");
    x = r_u32(0x800A71CC);
    y = r_u32(0x800A71D0);
    z = r_u32(0x800A71D4);
    motion.acceleration[0] = x;
    motion.acceleration[1] = y;
    motion.acceleration[2] = z;
    UpdateModelAnimationData(object);
    sub_8007FDF0(object);
    BuildModelPose(*this);
    flags = this->activation_flags;
    w_u16(object + 388, flags);
    if (flags & 1)
    {
        value = r_u8(object + 384);
        this->activation_flags = flags & 65534;
        w_u8(object + 384, value + 1);
    }
    mode = this->model.animation;
    if (mode == 3)
    {
        frame = sub_8006325C(object, 3, 5);
        if (sub_80062D24(object, frame, 1))
        {
            sub_80069EF4(137, object + 4, 0);
            xport_draft_host_sub_8007CC10_p1(position, object, 6);
            position[1] += 131072;
            child = sub_80032DC0(112);
            if (child)
                apocalypse_beast_ground_effect_native(child, position, 10, 64, 96, 96, 20, 64, 64);
        }
        frame = sub_8006325C(object, 3, 31);
        if (sub_80062D24(object, frame, 2))
        {
            sub_80069EF4(137, object + 4, 0);
            xport_draft_host_sub_8007CC10_p1(position, object, 5);
            position[1] += 131072;
            child = sub_80032DC0(112);
            if (child)
                apocalypse_beast_ground_effect_native(child, position, 10, 64, 96, 96, 20, 64, 64);
        }
    }
    else if (mode == 4)
    {
        frame = sub_8006325C(object, 4, 15);
        if (sub_80062D24(object, frame, 1))
        {
            xport_draft_host_sub_8007CC10_p1(position, object, 5);
            position[1] += 131072;
            child = sub_80032DC0(112);
            if (child)
                apocalypse_beast_ground_effect_native(child, position, 10, 64, 96, 96, 20, 64, 128);
            xport_draft_host_sub_80069EF4_p2(137, position, 0);
            current = visual_lists.cameras;
            sub_800774EC(current, object + 4, 1);
        }
    }
    if (r_u8(object + 380))
    {
        timer = r_u16(object + 476);
        if (timer)
            w_u16(object + 476, timer - 1);
        else
        {
            current = r_u32(object + 400);
            sub_8004BE90(object, current);
        }
    }
    else
        UpdateBehavior();
    if (!r_u8(object + 381))
    {
        SetModelBoneTable(this->model.bank, TheBeastAttachments);
        InitializeModelAnimation(TheBeastHierarchy);
        flags = this->model.flags;
        w_u8(object + 381, 1);
        this->model.flags = flags | 4;
    }
    if (r_u8(object + 382))
        AddVectors(motion.acceleration, (const uint32 *)psx_addr(0x800A6844, 12));
    sub_8004B9A4(object);
    x = this->model.pos[0];
    raycast.pos[0][0] = x;
    y = this->model.pos[1] - 262144;
    raycast.pos[0][1] = y;
    z = this->model.pos[2];
    raycast.pos[1][0] = x;
    raycast.pos[1][1] = y + 8388608;
    raycast.pos[0][2] = z;
    raycast.pos[1][2] = z;
    InitializeCollisionRay(raycast);
    QueryCollisionRay(raycast, 1);
    if (raycast.hit.GetObject())
    {
        w_u8(object + 314, 1);
        w_u32(object + 184, raycast.hit_pos[0]);
        w_u32(object + 188, raycast.hit_pos[1]);
        w_u32(object + 192, raycast.hit_pos[2]);
        w_u16(object + 164, (uint16)raycast.n.vx);
        w_u16(object + 166, (uint16)raycast.n.vy);
        w_u16(object + 168, (uint16)raycast.n.vz);
    }
    else
        w_u8(object + 314, 0);
    flags = r_u32(object + 396);
    if (flags & 16)
    {
        target = r_u32(object + 528);
        x = r_u32(target + 4);
        y = r_u32(target + 8);
        z = r_u32(target + 12);
        w_u32(object + 532, x);
        w_u32(object + 536, y);
        w_u32(object + 540, z);
        xport_draft_host_sub_80066B8C_p1(desired, object + 4, object + 532);
        desired[0] = 0;
        InterpolateBeastAngles(object + 16, (const uint16 *)desired, motion.angular_velocity, motion.angular_acceleration, 24);
    }
    else if (flags & 32)
    {
        xport_draft_host_sub_80066B8C_p1(desired, object + 4, object + 532);
        desired[0] = 0;
        InterpolateBeastAngles(object + 16, (const uint16 *)desired, motion.angular_velocity, motion.angular_acceleration, 16);
    }
    flags = r_u32(object + 396);
    if (flags & 64)
    {
        xport_draft_host_sub_80066B8C_p1(desired, object + 4, object + 496);
        value = desired[1];
        desired[0] = 0;
        current = this->model.angles[1];
        desired[1] = (value - current) & 4095;
    }
    else
    {
        desired[0] = 0;
        desired[1] = 0;
        desired[2] = 0;
    }
    InterpolateBeastAngles(object + 508, desired, (uint16 *)psx_addr(object + 514, 4), (uint16 *)psx_addr(object + 520, 4), 16);
    sub_8006C730(object + 508, object + 514);
    sub_8006C624(object + 508);
    sub_8006C730(object + 514, object + 520);
    DampAngles((uint16 *)psx_addr(object + 514, 6), motion.angular_drag);
    sub_8006C64C(object + 514);
    value = r_u16(object + 510);
    GetModelJointAngles(17).rotation[1] = (uint16)value;
    flags = r_u32(object + 396);
    if (!(flags & 1))
        goto StoppedAnimation;
    if (flags & 4)
    {
        frame = (uint32)(sint32)(sint8)this->model.frame;
        if ((sint32)frame < 18)
        {
            w_u32(object + 304, 65536);
            w_u8(object + 382, 0);
            motion.velocity[2] = 0;
            motion.velocity[1] = 0;
            motion.velocity[0] = 0;
            motion.acceleration[2] = 0;
            motion.acceleration[1] = 0;
            motion.acceleration[0] = 0;
        }
        else if (r_u16(object + 544))
            w_u8(object + 382, 1);
        frame = (uint32)(sint32)(sint8)this->model.frame;
        if ((sint32)frame >= 18 && !(r_u32(object + 396) & 8))
        {
            x = r_u32(object + 484);
            y = this->model.pos[0];
            timer = r_u16(object + 544);
            velocity_x = DivideBeastMovement(x - y, timer);
            x = r_u32(object + 492);
            y = this->model.pos[2];
            timer = r_u16(object + 544);
            velocity_z = DivideBeastMovement(x - y, timer);
            x = r_u32(object + 488);
            y = this->model.pos[1];
            timer = r_u16(object + 544);
            velocity_y = DivideBeastMovement(x - y, timer);
            flags = r_u32(object + 396);
            w_u32(object + 396, flags | 8);
            motion.velocity[0] = velocity_x;
            motion.velocity[2] = velocity_z;
            motion.velocity[1] = velocity_y - (timer << 14);
            sub_80069EF4(139, object + 4, 0);
        }
        else
        {
            frame = (uint32)(sint32)(sint8)this->model.frame;
            if ((sint32)frame >= 38)
            {
                if (!r_u16(object + 544))
                    goto JumpFinished;
                w_u32(object + 304, 0);
            }
        }
        timer = r_u16(object + 544);
        if (timer)
        {
            timer = (timer - 1) & 65535;
            w_u16(object + 544, timer);
            if (!timer)
            {
                x = r_u32(object + 484);
                y = r_u32(object + 488);
                z = r_u32(object + 492);
                this->model.pos[0] = x;
                this->model.pos[1] = y;
                this->model.pos[2] = z;
                position[0] = this->model.pos[0];
                position[1] = this->model.pos[1];
                position[2] = this->model.pos[2];
                value = (uint32)(sint32)(sint16)r_u16(object + 456);
                position[1] += (value - 32) << 12;
                if (r_u16(object + 472) != 4)
                {
                    child = sub_80032DC0(112);
                    if (child)
                        apocalypse_beast_ground_effect_native(child, position, 10, 64, 96, 96, 20, 64, 128);
                }
                motion.velocity[2] = 0;
                motion.velocity[1] = 0;
                motion.velocity[0] = 0;
                motion.acceleration[2] = 0;
                motion.acceleration[1] = 0;
                w_u8(object + 382, 0);
                current = visual_lists.cameras;
                motion.acceleration[0] = 0;
                w_u32(object + 304, 65536);
                sub_800774EC(current, object + 4, 0);
                sub_80069EF4(138, object + 4, 0);
            }
        }
JumpFinished:
        if (r_u8(object + 303))
        {
            flags = r_u32(object + 396);
            w_u32(object + 396, flags & 0xFFFFFFF2);
        }
    }
    else
    {
        distance = sub_8006696C(object + 4, object + 484);
        speed = distance ? 32768 / distance : (uint32)-1;
        if ((sint32)speed >= 129)
            speed = 128;
        xport_draft_host_sub_80066B8C_p1(desired, object + 4, object + 484);
        desired[0] = 0;
        InterpolateBeastAngles(object + 16, (const uint16 *)desired, motion.angular_velocity, motion.angular_acceleration, speed);
        speed = r_u16(object + 436);
        BuildDirectionVector(motion.velocity, speed, (const uint16 *)this->model.angles);
        flags = r_u32(object + 396);
        if (flags & 2)
        {
            position[0] = this->model.pos[0];
            position[1] = r_u32(object + 488);
            position[2] = this->model.pos[2];
            distance = xport_draft_host_sub_8006689C_p12(position, psx_addr(object + 484, 12));
            value = distance < 64;
        }
        else
        {
            distance = sub_8006689C(object + 4, object + 484);
            value = distance < 128;
        }
        if (value)
        {
            flags = r_u32(object + 396);
            w_u32(object + 396, flags & 0xFFFFFFFE);
        }
    }
    if (r_u32(object + 396) & 1)
    {
        if (!this->model.animation && r_u8(object + 303))
            sub_80063118(object, 3, 1);
        mode = this->model.animation;
        goto AttackAnimation;
    }
StoppedAnimation:
    mode = this->model.animation;
    if (mode == 3)
    {
        frame = sub_8006325C(object, 3, 43);
        if (sub_80062D24(object, frame, 1))
            sub_80063038(object, 8, 0, (uint32)-1);
        frame = sub_8006325C(object, 3, 17);
        if (sub_80062D24(object, frame, 2))
            sub_80063038(object, 9, 0, (uint32)-1);
        mode = this->model.animation;
    }
AttackAnimation:
    if (mode == 2)
    {
        frame = this->model.frame;
        if (frame - 15 < 6)
        {
            player = visual_lists.players;
            if (player)
            {
                xport_draft_host_sub_8007CC10_p1(position, object, 0);
                player = visual_lists.players;
                xport_draft_host_sub_8006C3AC_p123(discarded_direction, position, psx_addr(player + 4, 12));
                xport_draft_host_sub_8006BF04_p1(discarded_direction);
            }
        }
    }
    if (sub_8004BDFC(object, 0, 0) & 65535)
    {
        if (!r_u32(object + 552))
        {
            child = sub_8002FED8(104);
            if (child)
                child = sub_80027720(child, 0x800FF5A0, 4096, 0, 128, 134);
            w_u32(object + 552, child);
            value = sub_8004BDFC(object, 2, 1) & 65535;
            child = r_u32(object + 552);
            w_u32(child + 40, value);
            child = r_u32(object + 552);
            sub_80027878(child, 255, 64, 0, 185, 185, 185, 160, 40, 0, 255, 255, 255);
        }
        if (!r_u32(object + 556))
        {
            child = sub_8002FED8(104);
            if (child)
                child = sub_80027720(child, 0x800FF5A0, 4096, 0, 128, 134);
            w_u32(object + 556, child);
            value = sub_8004BDFC(object, 2, 1) & 65535;
            child = r_u32(object + 556);
            w_u32(child + 40, value);
            child = r_u32(object + 556);
            sub_80027878(child, 255, 64, 0, 185, 185, 185, 160, 40, 0, 255, 255, 255);
        }
        weapon = r_u32(object + 552);
        if (weapon || r_u32(object + 556))
        {
            xport_draft_host_sub_8007CC10_p1(position, object, 3);
            player = visual_lists.players;
            xport_draft_host_sub_80066B8C_p12(weapon_angles, position, player + 4);
            player = visual_lists.players;
            player_ground[0] = r_u32(player + 184);
            player_ground[1] = r_u32(player + 188);
            player_ground[2] = r_u32(player + 192);
            distance = ReadBeastPlanarDistance(player_ground, position);
            if ((sint32)distance < 768)
                distance = 768;
            pitch = (sint16)ratan2((sint32)(player_ground[1] - position[1]) >> 12, (sint32)distance);
            xport_draft_host_sub_8007CC10_p1(muzzle, object, 1);
            xport_draft_host_sub_80066B8C_p123(weapon_angles, position, muzzle);
            weapon_angles[0] = (uint16)pitch;
            weapon = r_u32(object + 552);
            if (weapon)
                FireBeastWeapon(weapon, muzzle, weapon_angles);
            if (r_u32(object + 556))
            {
                xport_draft_host_sub_8007CC10_p1(muzzle, object, 2);
                xport_draft_host_sub_80066B8C_p123(weapon_angles, position, muzzle);
                weapon_angles[0] = (uint16)pitch;
                weapon = r_u32(object + 556);
                FireBeastWeapon(weapon, muzzle, weapon_angles);
            }
        }
    }
    else
    {
        weapon = r_u32(object + 552);
        if (weapon)
        {
            DestroyBeastWeapon(weapon);
            w_u32(object + 552, 0);
        }
        weapon = r_u32(object + 556);
        if (weapon)
        {
            DestroyBeastWeapon(weapon);
            w_u32(object + 556, 0);
        }
    }
    if (sub_8004BDFC(object, 1, 0) & 65535)
    {
        weapon = r_u32(object + 560);
        if (!weapon)
        {
            child = sub_8002FED8(72);
            if (child)
                child = sub_80024428(child, 0x800FF5A0, 0);
            w_u32(object + 560, child);
            value = sub_8004BDFC(object, 1, 1) & 65535;
            child = r_u32(object + 560);
            w_u32(child + 40, value);
            child = r_u32(object + 560);
            sub_800244CC(child, 2000);
            weapon = r_u32(object + 560);
        }
        if (weapon)
        {
            xport_draft_host_sub_8007CC10_p1(muzzle, object, 3);
            mode = this->model.animation;
            if (mode == 6)
            {
                player = visual_lists.players;
                xport_draft_host_sub_80066B8C_p12(weapon_angles, muzzle, player + 4);
                pitch = (sint16)weapon_angles[0];
                xport_draft_host_sub_8007CC10_p1(position, object, 4);
                xport_draft_host_sub_80066B8C_p123(weapon_angles, muzzle, position);
                weapon_angles[0] = (uint16)pitch;
            }
            else
            {
                xport_draft_host_sub_8007CC10_p1(position, object, 4);
                xport_draft_host_sub_80066B8C_p123(weapon_angles, muzzle, position);
            }
            weapon = r_u32(object + 560);
            FireBeastWeapon(weapon, position, weapon_angles);
        }
    }
    else
    {
        weapon = r_u32(object + 560);
        if (weapon)
        {
            DestroyBeastWeapon(weapon);
            w_u32(object + 560, 0);
        }
    }
    mode = this->model.animation;
    if (mode == 7)
    {
        frame = sub_8006325C(object, 7, 25);
        if (sub_80062D24(object, frame, 1))
        {
            xport_draft_host_sub_8007CC10_p1(position, object, 0);
            child = sub_800625AC(428);
            if (child)
            {
                void *storage = AllocateNativeObjectStorage(child, sizeof(GuidedProjectile));
                new (storage) GuidedProjectile(position, 10, &visual_lists.players, 1);
            }
            w_u32(object + 572, child);
            flags = r_u16(child + 78);
            w_u16(child + 78, flags | 32);
            child = r_u32(object + 572);
            GuidedProjectile::Attach(child).SetLaunchDelay(0);
        }
        frame = sub_8006325C(object, 7, 68);
        if (sub_80062D24(object, frame, 2))
        {
            if (r_u32(object + 572))
            {
                xport_draft_host_sub_8007CC10_p1(muzzle, object, 0);
                player = visual_lists.players;
                xport_draft_host_sub_80066B8C_p12(weapon_angles, muzzle, player + 4);
                weapon_angles[1] = this->model.angles[1];
                child = r_u32(object + 572);
                apocalypse_guided_projectile_angles_native(child, weapon_angles);
                player = visual_lists.players;
                child = r_u32(object + 572);
                sub_8002DC0C(child, 0, player + 184);
                child = r_u32(object + 572);
                flags = r_u16(child + 78);
                w_u16(child + 78, flags & 65503);
            }
            w_u32(object + 572, 0);
        }
        if (r_u32(object + 572))
        {
            xport_draft_host_sub_8007CC10_p1(position, object, 0);
            child = r_u32(object + 572);
            x = position[0];
            y = position[1];
            z = position[2];
            w_u32(child + 4, x);
            w_u32(child + 8, y);
            w_u32(child + 12, z);
        }
    }
    return SpawnPickup();
}

uint32 TheBeast::Resize(uint32 increment)
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 flags, scale, product, height, old_height, displacement, y;
    FUNCTION_MARKER(0x80041FDC, "SLUS_003.73");
    flags = this->model.flags;
    scale = (uint32)(sint32)(sint16)this->model.scale[0];
    scale += increment;
    this->model.flags = flags | 512;
    product = ((scale << 4) + scale) << 4;
    product -= scale;
    this->model.scale[2] = scale;
    this->model.scale[1] = scale;
    this->model.scale[0] = scale;
    old_height = (uint32)(sint32)(sint16)r_u16(object + 456);
    height = (uint32)((sint32)product >> 12);
    w_u16(object + 456, height);
    height = (uint32)(sint32)(sint16)height;
    displacement = (height - old_height) << 12;
    y = this->model.pos[1];
    this->model.pos[1] = y - displacement;
    return displacement;
}

uint32 TheBeast::UpdateLighting()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 random, weapon, active = 0;
    uint8 red, green, blue;
    sint16 direction[3], flicker[3], color[3];
    FUNCTION_MARKER(0x80042038, "SLUS_003.73");
    w_u16(0x800FFB9E, (uint16)-4096);
    w_u16(0x800FFBA6, 4096);
    w_u16(0x800FFB9C, 0);
    w_u16(0x800FFBA0, 0);
    w_u16(0x800FFBA4, 0);
    w_u16(0x800FFBA8, 0);
    w_u16(0x800FFBAC, 2048);
    w_u16(0x800FFBAE, 2048);
    w_u16(0x800FFBB0, 2048);
    random = RandomRange(256);
    flicker[0] = (sint16)(random << 4);
    random = RandomRange((uint32)((sint32)random >> 1));
    flicker[1] = (sint16)(random << 4);
    flicker[2] = 0;
    weapon = r_u32(object + 552);
    if (weapon)
    {
        if (apocalypse_weapon_light_colors(weapon, &red, &green, &blue))
        {
            ReadTheBeastLightDirection(object, 1, 1, direction);
            color[0] = (sint16)(red << 4);
            color[1] = (sint16)(green << 4);
            color[2] = (sint16)(blue << 4);
            active = 1;
        }
    }
    else
    {
        weapon = r_u32(object + 560);
        if (weapon)
        {
            if (apocalypse_weapon_light_colors(weapon, &red, &green, &blue))
            {
                ReadTheBeastLightDirection(object, 4, 0, direction);
                color[0] = (sint16)(red << 4);
                color[1] = (sint16)(green << 4);
                color[2] = (sint16)(blue << 4);
                active = 1;
            }
        }
        else if (r_u32(object + 572))
        {
            ReadTheBeastLightDirection(object, 0, 0, direction);
            color[0] = 2560;
            color[1] = 1024;
            color[2] = 0;
            active = 1;
        }
    }
    if (!active)
    {
        direction[0] = 0;
        direction[1] = 0;
        direction[2] = 0;
        color[0] = 0;
        color[1] = 0;
        color[2] = 0;
    }
    apocalypse_lighting_directions_native(0x800FFB9C, 0x800FFBA4, direction);
    apocalypse_lighting_colors_native(0x800FFBAC, flicker, color);
    w_u16(0x800FFA98, 64);
    w_u16(0x800FFA96, 64);
    w_u16(0x800FFA94, 64);
    return 64;
}

uint32 TheBeast::Damage(uint32 damage, const sint32 *, uint32 reason)
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 pulse, health, applied;
    sint32 alive;
    FUNCTION_MARKER(0x800422C0, "SLUS_003.73");
    pulse = r_u8(object + 383);
    w_u8(object + 383, pulse + 1);
    alive = (sint16)r_u16(object + 218);
    health = r_u16(object + 218);
    if (alive <= 0)
        return 0;
    if (reason == 5)
        damage = (uint32)((sint32)damage >> 1);
    applied = (uint32)((sint32)damage >> 1);
    if (reason == 10)
    {
        damage = (uint32)((sint32)damage >> 2);
        applied = (uint32)((sint32)damage >> 1);
    }
    health -= applied;
    w_u16(object + 218, health);
    if ((sint32)(health << 16) <= 0)
        ClearTrackedObjectHealth();
    SetColor(3, 255, 255, 255);
    return 1;
}

void TheBeast::UpdateBehavior()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 state, phase, value, player, camera, floor, flags, child, table, target, index, entry, best, selected, chosen, distance, scale, counter, yaw;
    uint32 position[4], destination[4], delta[4], normalized[4], scaled[4], sample[4];
    uint32 shift, scalar;
    uint16 angles[4];
    uint8 voices[3];
    sint32 health, step, old_counter, adjustment;
    bool selection_valid;
    CollisionRay raycast;
    FUNCTION_MARKER(0x80042360, "SLUS_003.73");
    state = r_u16(object + 472);
    if (state > 128)
        return;
    switch (state)
    {
        case 0:
            phase = r_u16(object + 474);
            switch (phase)
            {
                case 0:
                    floor = sub_80067A18(object + 4, 256, 1024);
                    if (floor != (uint32)-1)
                    {
                        value = (uint32)(sint32)(sint16)r_u16(object + 456);
                        this->model.pos[1] = floor - (value << 12);
                    }
                    player = visual_lists.players;
                    w_u32(player + 452, 150);
                    w_u16(object + 218, 300);
                    ::TrackObjectHealth(object, 24, 42, 4096);
                    AdvanceBeastPhase(object);
                    return;
                case 1:
                    camera = visual_lists.cameras;
                    w_u32(camera + 304, 1);
                    w_u32(camera + 296, object);
                    w_u32(camera + 340, 1);
                    w_u32(camera + 336, object);
                    sub_800785D8(camera, 1100, 0);
                    sub_80078618(camera, (uint32)-4096, 0);
                    sub_8007865C(camera, 0, 0);
                    sub_800786A0(camera, 3402, 0);
                    sub_800786E4(camera, (uint32)-916, 0);
                    phase = r_u16(object + 474);
                    w_u16(object + 460, 45);
                    w_u16(object + 474, phase + 1);
                    return;
                case 2:
                case 3:
                    counter = r_u16(object + 460);
                    old_counter = (sint16)r_u16(object + 460);
                    if (counter)
                    {
                        w_u16(object + 460, old_counter - 1);
                        return;
                    }
                    if (phase == 2)
                    {
                        camera = visual_lists.cameras;
                        sub_80078618(camera, (uint32)-206, 30);
                        sub_800786A0(camera, 90, 30);
                        sub_800786E4(camera, 604, 30);
                        phase = r_u16(object + 474);
                        w_u16(object + 460, 30);
                        w_u16(object + 474, phase + 1);
                    }
                    else
                    {
                        voices[0] = r_u8(0x800FF4B0);
                        voices[1] = r_u8(0x800FF4B1);
                        voices[2] = r_u8(0x800FF4B2);
                        index = RandomRange(3);
                        ::SelectCdVoice(19, voices[index]);
                        PlayBeastAnimation(object, 6);
                        AdvanceBeastPhase(object);
                    }
                    return;
                case 4:
                    if (!r_u8(object + 303))
                        return;
                    camera = visual_lists.cameras;
                    player = visual_lists.players;
                    w_u32(camera + 304, 1);
                    w_u32(camera + 296, player);
                    w_u32(camera + 340, 1);
                    w_u32(camera + 336, player);
                    sub_800785D8(camera, 1100, 30);
                    sub_80078618(camera, (uint32)-308, 30);
                    sub_8007865C(camera, 0, 30);
                    sub_800786A0(camera, 68, 30);
                    sub_800786E4(camera, 216, 30);
                    SelectBeastState(object, 1);
                    return;
                default:
                    return;
            }
        case 1:
            if (r_u16(object + 474))
                return;
            if ((sint16)r_u16(object + 218) <= 0)
            {
                w_u16(object + 472, 3);
                return;
            }
            if (r_u32(object + 220) < 768)
            {
                state = r_u16(object + 472);
                phase = r_u16(object + 474);
                SelectBeastState(object, 80);
                w_u16(object + 440, state);
                w_u16(object + 442, phase);
                return;
            }
            state = r_u16(object + 472);
            phase = r_u16(object + 474);
            w_u16(object + 440, state);
            w_u16(object + 442, phase);
            if (!RandomRange(2))
                SelectBeastState(object, 64);
            else
            {
                w_u16(object + 446, 768);
                w_u16(object + 444, 0);
                SelectBeastState(object, 112);
            }
            return;
        case 3:
            phase = r_u16(object + 474);
            if (!phase)
            {
                index = r_u16(0x800FF4B4);
                ::SpawnRecord(index);
                sub_80069EF4(132, object + 4, 0);
                PlayBeastAnimation(object, 5);
                AdvanceBeastPhase(object);
            }
            else if (phase == 1)
            {
                if (r_u8(object + 303))
                {
                    camera = visual_lists.cameras;
                    sub_800774EC(camera, object + 4, 1);
                    index = r_u16(0x800FF4B6);
                    ::SpawnRecord(index);
                    index = r_u16(0x800FF4B8);
                    ::SpawnRecord(index);
                    SelectBeastState(object, 4);
                }
                else
                    Resize(20);
            }
            return;
        case 4:
            phase = r_u16(object + 474);
            if (!phase)
            {
                w_u16(object + 438, 48);
                SetJumpDestination((const uint32 *)this->model.pos);
                AdvanceBeastPhase(object);
            }
            else if (phase == 1 && !(r_u32(object + 396) & 1))
            {
                ::SetScreenFlash(255, 255, 255, 8, 0, 0);
                SelectBeastState(object, 5);
                w_u16(object + 438, 48);
            }
            return;
        case 5:
            phase = r_u16(object + 474);
            switch (phase)
            {
                case 0:
                    w_u16(object + 218, 200);
                    ::TrackObjectHealth(object, 24, 42, 4096);
                    camera = visual_lists.cameras;
                    w_u16(object + 436, 48);
                    w_u32(camera + 564, 18);
                    AdvanceBeastPhase(object);
                    return;
                case 1:
                    if ((sint16)r_u16(object + 218) <= 0)
                    {
                        w_u16(object + 474, 3);
                        return;
                    }
                    distance = r_u32(object + 220);
                    if (distance < 3072)
                    {
                        state = r_u16(object + 472);
                        phase = r_u16(object + 474);
                        SelectBeastState(object, distance < 768 ? 80 : 96);
                        w_u16(object + 440, state);
                        w_u16(object + 442, phase);
                        return;
                    }
                    player = visual_lists.players;
                    if (!player)
                        return;
                    if (apocalypse_candidate_native(object, destination, player + 4, 512, 1024, 128, 1024, 1, 0))
                    {
                        ClearFacing();
                        SetDestination(destination);
                        AdvanceBeastPhase(object);
                    }
                    else
                        ClearBeastVelocity(object);
                    return;
                case 2:
                    if (!(r_u32(object + 396) & 1) || !RandomRange(32))
                        w_u16(object + 474, 1);
                    return;
                case 3:
                    index = r_u16(0x800FF4BA);
                    xport_draft_host_sub_8006613C_p1(position, index);
                    floor = apocalypse_ground_native(position, 2048, 2048);
                    position[1] = floor;
                    if (floor != (uint32)-1)
                    {
                        JumpBeastToPoint(*this, position);
                        AdvanceBeastPhase(object);
                    }
                    return;
                case 4:
                    if (!(r_u32(object + 396) & 1))
                    {
                        sub_80069EF4(132, object + 4, 0);
                        PlayBeastAnimation(object, 5);
                        AdvanceBeastPhase(object);
                    }
                    return;
                case 5:
                    if (!r_u8(object + 303))
                        Resize(20);
                    else
                    {
                        state = r_u16(object + 472);
                        w_u16(object + 474, 0);
                        w_u16(object + 472, state + 1);
                    }
                    return;
                default:
                    return;
            }
        case 6:
            phase = r_u16(object + 474);
            switch (phase)
            {
                case 0:
                    w_u16(object + 218, 600);
                    ::TrackObjectHealth(object, 24, 42, 4096);
                    phase = r_u16(object + 474);
                    w_u16(object + 460, 0);
                    w_u16(object + 474, phase + 1);
                    return;
                case 1:
                    health = (sint16)r_u16(object + 218);
                    step = (sint16)r_u16(object + 460);
                    if (health <= 0 && step >= 5)
                    {
                        index = r_u16(0x800FF4BA);
                        xport_draft_host_sub_8006613C_p1(position, index);
                        floor = apocalypse_ground_native(position, 2048, 2048);
                        position[1] = floor;
                        if (floor != (uint32)-1)
                        {
                            JumpBeastToPoint(*this, position);
                            w_u16(object + 474, 8);
                        }
                        return;
                    }
                    if (step < 5 && (sint16)r_u16(object + 218) < 600 - 90 * (step + 1))
                    {
                        AdvanceBeastPhase(object);
                        return;
                    }
                    if (r_u32(object + 220) < 3072)
                    {
                        w_u16(object + 474, 3);
                        return;
                    }
                    player = visual_lists.players;
                    if (!player)
                        return;
                    if (!apocalypse_candidate_native(object, position, player + 4, 512, 1024, 128, 1024, 1, 0))
                    {
                        ClearBeastVelocity(object);
                        w_u16(object + 474, 3);
                        return;
                    }
                    xport_draft_host_sub_8006C3AC_p123(delta, position, this->model.pos);
                    delta[1] = 0;
                    scalar = 0;
                    while (scalar < 8)
                    {
                        shift = 3;
                        xport_draft_host_sub_8006C564_p123(normalized, delta, &shift);
                        xport_draft_host_sub_8006C40C_p123(scaled, normalized, &scalar);
                        xport_draft_host_sub_8006C34C_p13(sample, object + 4, scaled);
                        raycast.pos[0][1] = sample[1];
                        raycast.pos[0][1] = sample[1] - 2097152;
                        raycast.pos[1][1] = sample[1];
                        raycast.pos[0][0] = sample[0];
                        raycast.pos[0][2] = sample[2];
                        raycast.pos[1][0] = sample[0];
                        raycast.pos[1][2] = sample[2];
                        raycast.pos[1][1] = sample[1] + 8388608;
                        InitializeCollisionRay(raycast);
                        raycast.triggers = 0;
                        QueryCollisionRay(raycast, 1);
                        if (!raycast.hit.GetObject())
                        {
                            ClearBeastVelocity(object);
                            w_u16(object + 474, 3);
                            return;
                        }
                        ++scalar;
                    }
                    ClearFacing();
                    SetDestination(position);
                    AdvanceBeastPhase(object);
                    return;
                case 2:
                case 4:
                    step = (sint16)r_u16(object + 460);
                    if (step >= 5 || (sint16)r_u16(object + 218) >= 600 - 90 * (step + 1))
                    {
                        if (phase == 4)
                            w_u16(object + 474, 1);
                        else if (!(r_u32(object + 396) & 1) || !RandomRange(32))
                        {
                            phase = r_u16(object + 474);
                            w_u16(object + 474, phase - 1);
                        }
                        return;
                    }
                    index = r_u16(0x800A6B94 + (uint32)step * 2);
                    xport_draft_host_sub_8006613C_p1(position, index);
                    floor = apocalypse_ground_native(position, 2048, 2048);
                    position[1] = floor;
                    if (floor == (uint32)-1)
                        position[1] = r_u32(object + 188);
                    ClearFacing();
                    value = (uint32)(sint32)(sint16)r_u16(object + 456);
                    position[1] -= value << 12;
                    if (phase == 2)
                        ClearBeastVelocity(object);
                    SetJumpDestination(position);
                    if (phase == 2)
                        w_u16(object + 474, 5);
                    else
                        AdvanceBeastPhase(object);
                    return;
                case 3:
                    state = r_u16(object + 472);
                    phase = r_u16(object + 474);
                    w_u16(object + 474, 0);
                    w_u16(object + 440, state);
                    distance = r_u32(object + 220);
                    w_u16(object + 442, phase - 1);
                    w_u16(object + 472, distance < 768 ? 80 : distance < 1024 ? 96 : 64);
                    return;
                case 5:
                    if (!(r_u32(object + 396) & 1))
                    {
                        PlayBeastAnimation(object, 4);
                        AdvanceBeastPhase(object);
                    }
                    return;
                case 6:
                    if (!r_u8(object + 303))
                    {
                        if ((sint8)this->model.frame == 15)
                        {
                            step = (sint16)r_u16(object + 460);
                            player = visual_lists.solids;
                            index = r_u16(0x800A6B94 + (uint32)step * 2);
                            ::ActivateObjectsByIndex(player, index);
                            step = (sint16)r_u16(object + 460);
                            w_u16(object + 460, step + 1);
                        }
                        return;
                    }
                    index = r_u16(0x800FF4BA);
                    xport_draft_host_sub_8006613C_p1(position, index);
                    floor = apocalypse_ground_native(position, 2048, 2048);
                    position[1] = floor;
                    if (floor != (uint32)-1)
                    {
                        JumpBeastToPoint(*this, position);
                        AdvanceBeastPhase(object);
                    }
                    return;
                case 7:
                    if (!(r_u32(object + 396) & 1))
                        w_u16(object + 474, 1);
                    return;
                case 8:
                    if (!(r_u32(object + 396) & 1))
                    {
                        sub_80069EF4(132, object + 4, 0);
                        PlayBeastAnimation(object, 5);
                        AdvanceBeastPhase(object);
                    }
                    return;
                case 9:
                    if (!r_u8(object + 303))
                        Resize(20);
                    else
                    {
                        state = r_u16(object + 472);
                        w_u16(object + 474, 0);
                        w_u16(object + 472, state + 1);
                    }
                    return;
                default:
                    return;
            }
        case 7:
            phase = r_u16(object + 474);
            if (!phase)
            {
                w_u16(object + 218, 600);
                ::TrackObjectHealth(object, 24, 42, 4096);
                PlayBeastAnimation(object, 4);
                AdvanceBeastPhase(object);
            }
            else if (phase == 1)
            {
                if (r_u8(object + 303))
                    w_u16(object + 474, 2);
                else if ((sint8)this->model.frame == 15)
                {
                    entry = 0x800A6BA0;
                    for (index = 0; index < 5; ++index)
                    {
                        value = r_u16(entry);
                        entry += 2;
                        player = visual_lists.solids;
                        ::ActivateObjectsByIndex(player, value);
                    }
                }
            }
            else if (phase == 2)
                SelectBeastState(object, 8);
            return;
        case 8:
            phase = r_u16(object + 474);
            if (phase == 0)
            {
                phase = r_u16(object + 474);
                w_u16(object + 460, 0);
                w_u16(object + 474, phase + 1);
                phase = 1;
            }
            switch (phase)
            {
                case 1:
                    if ((sint16)r_u16(object + 218) <= 0 && (sint16)r_u16(object + 460) == 4)
                    {
                        for (index = 0; index < 5; ++index)
                            if (!r_u8(object + 546 + index))
                            {
                                w_u16(object + 464, index);
                                break;
                            }
                        SelectBeastState(object, 9);
                        return;
                    }
                    player = visual_lists.players;
                    distance = sub_80066918(object + 4, player + 4);
                    scale = (uint32)((3 * (sint16)this->model.scale[0]) >> 4);
                    state = r_u16(object + 472);
                    phase = r_u16(object + 474);
                    SelectBeastState(object, distance < scale ? 80 : 64);
                    w_u16(object + 440, state);
                    w_u16(object + 442, phase + 1);
                    return;
                case 2:
                    step = (sint16)r_u16(object + 460);
                    if (step >= 4 || (sint16)r_u16(object + 218) >= 600 - 120 * (step + 1))
                    {
                        w_u16(object + 474, 1);
                        return;
                    }
                    best = (uint32)-1;
                    selection_valid = false;
                    selected = 0;
                    entry = 0x800A6BAC;
                    for (index = 0; index < 5; ++index)
                    {
                        if (!r_u8(object + 546 + index))
                        {
                            value = r_u16(entry);
                            xport_draft_host_sub_8006613C_p1(position, value);
                            player = visual_lists.players;
                            distance = xport_draft_host_sub_8006689C_p12(position, psx_addr(player + 4, 12));
                            if (distance < best)
                            {
                                selected = index;
                                best = distance;
                                selection_valid = true;
                            }
                        }
                        entry += 2;
                    }
                    if (!selection_valid)
                    {
                        // TODO Original reads an undefined saved register when no candidate wins
                        GameDiagnostic("TODO TheBeast AI state8 has no selected point\n");
                        abort();
                    }
                    w_u8(object + 546 + selected, 1);
                    value = r_u16(0x800A6BAC + selected * 2);
                    xport_draft_host_sub_8006613C_p1(position, value);
                    floor = apocalypse_ground_native(position, 2048, 2048);
                    position[1] = floor;
                    if (floor == (uint32)-1)
                        w_u16(object + 474, 1);
                    else
                    {
                        w_u16(object + 464, selected);
                        JumpBeastToPoint(*this, position);
                        AdvanceBeastPhase(object);
                    }
                    return;
                case 3:
                    if (!(r_u32(object + 396) & 1))
                    {
                        PlayBeastAnimation(object, 4);
                        AdvanceBeastPhase(object);
                    }
                    return;
                case 4:
                    if (!r_u8(object + 303))
                    {
                        if ((sint8)this->model.frame == 15)
                        {
                            step = (sint16)r_u16(object + 460);
                            w_u16(object + 460, step + 1);
                            chosen = (uint32)(sint32)(sint16)r_u16(object + 464);
                            player = visual_lists.solids;
                            value = r_u16(0x800A6BAC + chosen * 2);
                            ::ActivateObjectsByIndex(player, value);
                        }
                        return;
                    }
                    value = r_u16(0x800FF4BA);
                    xport_draft_host_sub_8006613C_p1(position, value);
                    floor = apocalypse_ground_native(position, 2048, 2048);
                    position[1] = floor;
                    if (floor != (uint32)-1)
                    {
                        JumpBeastToPoint(*this, position);
                        AdvanceBeastPhase(object);
                    }
                    return;
                case 5:
                    if (!(r_u32(object + 396) & 1))
                        w_u16(object + 474, 1);
                    return;
                default:
                    return;
            }
        case 9:
            phase = r_u16(object + 474);
            if (!phase)
            {
                w_u16(object + 218, 400);
                sub_8004BDCC(object, 2, 1, 4);
                ::TrackObjectHealth(object, 24, 42, 4096);
                value = r_u16(0x800FF4BA);
                xport_draft_host_sub_8006613C_p1(position, value);
                chosen = (uint32)(sint32)(sint16)r_u16(object + 464);
                value = r_u16(0x800A6BAC + chosen * 2);
                xport_draft_host_sub_8006613C_p1(delta, value);
                xport_draft_host_sub_8006C3AC_p123(delta, delta, position);
                delta[1] = 0;
                xport_draft_host_sub_800666DC_p12(normalized, delta);
                scalar = 512;
                xport_draft_host_sub_8006C40C_p123(scaled, normalized, &scalar);
                xport_draft_host_sub_8006C0B8_p12(position, scaled);
                floor = apocalypse_ground_native(position, 2048, 2048);
                position[1] = floor;
                if (floor != (uint32)-1)
                {
                    ClearFacing();
                    value = (uint32)(sint32)(sint16)r_u16(object + 456);
                    position[1] -= value << 12;
                    SetDestination(position);
                    AdvanceBeastPhase(object);
                }
            }
            else if (phase == 1)
            {
                if (!(r_u32(object + 396) & 1))
                    w_u16(object + 474, 2);
            }
            else if (phase == 2)
            {
                if ((sint16)r_u16(object + 218) <= 0)
                {
                    SelectBeastState(object, 10);
                    return;
                }
                player = visual_lists.players;
                distance = sub_80066918(object + 4, player + 4);
                scale = (uint32)((3 * (sint16)this->model.scale[0]) >> 4);
                if (distance < scale)
                {
                    state = r_u16(object + 472);
                    phase = r_u16(object + 474);
                    SelectBeastState(object, 128);
                    w_u16(object + 440, state);
                    w_u16(object + 442, phase);
                    return;
                }
                state = r_u16(object + 472);
                phase = r_u16(object + 474);
                w_u16(object + 442, phase);
                w_u16(object + 440, state);
                chosen = (uint32)(sint32)(sint16)r_u16(object + 464);
                value = r_u16(0x800A6BAC + chosen * 2);
                xport_draft_host_sub_8006613C_p1(position, value);
                player = visual_lists.players;
                chosen = 0;
                if (ReadBeastPointDistance(position, player + 4) < 1024)
                    chosen = RandomRange(3) != 0;
                if (chosen)
                {
                    w_u16(object + 446, 768);
                    w_u16(object + 444, 1);
                    SelectBeastState(object, 112);
                }
                else
                    SelectBeastState(object, 64);
            }
            return;
        case 10:
            phase = r_u16(object + 474);
            if (phase - 1 < 3)
            {
                if (!RandomRange(60))
                    sub_80069EF4(132, object + 4, 3);
                value = (uint32)(sint32)(sint16)r_u16(object + 464);
                camera = visual_lists.cameras;
                yaw = (value - 16) & 4095;
                w_u16(object + 464, yaw);
                sub_8007851C(camera, yaw, 4);
            }
            phase = r_u16(object + 474);
            switch (phase)
            {
                case 0:
                    player = visual_lists.players;
                    w_u32(player + 452, 900);
                    sub_80069EF4(132, object + 4, 0);
                    sub_80063118(object, 12, 1);
                    camera = visual_lists.cameras;
                    w_u16(object + 460, 0);
                    w_u16(object + 464, 2048);
                    if (camera)
                    {
                        w_u32(camera + 304, 1);
                        w_u32(camera + 340, 1);
                        w_u32(camera + 296, object);
                        w_u32(camera + 336, object);
                        w_u32(camera + 564, 3);
                        sub_8007851C(camera, 2048, 4);
                        camera = visual_lists.cameras;
                        sub_800785D8(camera, 1324, 16);
                        camera = visual_lists.cameras;
                        sub_80078618(camera, (uint32)-854, 16);
                        camera = visual_lists.cameras;
                        sub_8007865C(camera, 0, 16);
                        camera = visual_lists.cameras;
                        sub_800786A0(camera, (uint32)-144, 16);
                        camera = visual_lists.cameras;
                        sub_800786E4(camera, (uint32)-472, 16);
                    }
                    AdvanceBeastPhase(object);
                    return;
                case 1:
                    counter = r_u16(object + 460);
                    old_counter = (sint16)r_u16(object + 460);
                    if (counter < 180)
                    {
                        w_u16(object + 460, old_counter + 1);
                        if (!RandomRange(3))
                        {
                            w_u32(0x800FF3AC, 0);
                            child = sub_80032DC0(92);
                            if (child)
                            {
                                void *dram_addr = AllocateNativeObjectStorage(child, sizeof(BeastOrbitEffect));
                                new (dram_addr) BeastOrbitEffect((const uint32 *)this->model.pos);
                            }
                            w_u32(0x800FF3AC, 1);
                        }
                        return;
                    }
                    child = sub_80032DC0(124);
                    if (child)
                        sub_8001EB64(child, object + 4, 255, 128, 0, 512, 32);
                    sub_80069EF4(1, object + 4, 0);
                    AdvanceBeastPhase(object);
                    return;
                case 2:
                    flags = this->model.flags;
                    this->model.flags = flags & 65407;
                    sub_800626F8(object, 0, 255, 0, 0);
                    w_u32(object + 176, 0x02404040);
                    w_u16(object + 180, 10240);
                    phase = r_u16(object + 474);
                    w_u16(object + 182, 10);
                    w_u16(object + 474, phase + 1);
                    return;
                case 3:
                    if ((sint16)this->model.scale[0] >= 2049)
                    {
                        Resize((uint32)-16);
                        return;
                    }
                    camera = visual_lists.cameras;
                    old_counter = (sint16)r_u16(camera + 494) - 2048;
                    if (old_counter < 0)
                        old_counter = -old_counter;
                    if (old_counter < 128)
                    {
                        phase = r_u16(object + 474);
                        w_u16(object + 460, 0);
                        w_u16(object + 474, phase + 1);
                    }
                    return;
                case 4:
                    camera = visual_lists.cameras;
                    w_u32(camera + 564, 3);
                    sub_8007851C(camera, 2048, 4);
                    camera = visual_lists.cameras;
                    sub_800785D8(camera, 1324, 16);
                    camera = visual_lists.cameras;
                    sub_80078618(camera, (uint32)-316, 16);
                    camera = visual_lists.cameras;
                    sub_8007865C(camera, 0, 16);
                    camera = visual_lists.cameras;
                    sub_800786A0(camera, 220, 16);
                    camera = visual_lists.cameras;
                    sub_800786E4(camera, (uint32)-328, 16);
                    AdvanceBeastPhase(object);
                    return;
                case 5:
                    old_counter = (sint16)r_u16(object + 460);
                    counter = (uint16)old_counter;
                    w_u16(object + 460, old_counter + 1);
                    if (counter == 16)
                    {
                        flags = this->model.flags;
                        w_u32(object + 176, 0);
                        this->model.flags = flags | 1;
                        ::SetScreenFlash(240, 240, 240, 32, 0, 0);
                        position[0] = this->model.pos[0];
                        position[1] = this->model.pos[1];
                        position[2] = this->model.pos[2];
                        value = (uint32)(sint32)(sint16)r_u16(object + 456);
                        position[1] += value << 12;
                        child = sub_800625AC(296);
                        if (child)
                        {
                            void *dram_addr = AllocateNativeObjectStorage(child, sizeof(BeastFinalVisual));
                            new (dram_addr) BeastFinalVisual(position);
                        }
                        sub_80069EF4(1, object + 4, 0);
                        position[2] -= 524288;
                        position[0] += 737280;
                        SpawnFireCycle(position, 220, 0, 2);
                        position[0] -= 1474560;
                        SpawnFireCycle(position, 220, 0, 2);
                        return;
                    }
                    if (counter >= 220)
                    {
                        table = r_u32(object + 68);
                        adjustment = (sint16)r_u16(table + 16);
                        target = r_u32(table + 20);
                        xport_draft_guest_call1(target, object + (uint32)adjustment);
                        CompleteLevel(0);
                        return;
                    }
                    if (counter >= 17 && !(old_counter & 15))
                    {
                        child = sub_80032DC0(124);
                        if (child)
                            sub_8001EB64(child, object + 4, 255, 128, 0, 512, 32);
                    }
                    if (counter == 190)
                        ::FadeScreenToWhite(30, 1);
                    return;
                default:
                    return;
            }
        case 64:
        case 80:
        case 96:
            phase = r_u16(object + 474);
            if (!phase)
            {
                BeginBeastAttack(*this);
                if (r_u8(object + 296) == 1)
                {
                    PlayBeastAnimation(object, state == 64 ? 7 : state == 80 ? 2 : 6);
                    if (state == 80)
                        sub_80069EF4(135, object + 4, 0);
                    phase = r_u16(object + 474);
                    w_u16(object + 474, phase + 2);
                }
                else
                    AdvanceBeastPhase(object);
            }
            else if (phase == 1)
            {
                if (r_u8(object + 303))
                {
                    PlayBeastAnimation(object, state == 64 ? 7 : state == 80 ? 2 : 6);
                    if (state == 80)
                        sub_80069EF4(135, object + 4, 0);
                    AdvanceBeastPhase(object);
                }
            }
            else if (phase == 2)
            {
                if (state == 64)
                {
                    value = this->model.frame;
                    sub_8004BDCC(object, 1, 0, value - 18 < 22);
                }
                else if (state == 96 && (sint8)this->model.frame >= 28)
                    sub_8004BDCC(object, 1, 0, 1);
                player = visual_lists.players;
                if (!player || r_u8(object + 303))
                {
                    if (state == 80)
                        w_u16(object + 474, 3);
                    else
                        AdvanceBeastPhase(object);
                }
                else if (state == 80)
                {
                    value = this->model.frame;
                    if (value - 13 < 9)
                    {
                        distance = sub_80066918(object + 4, player + 4);
                        scale = (uint32)((3 * (sint16)this->model.scale[0]) >> 4);
                        if (distance < scale)
                        {
                            player = visual_lists.players;
                            ApplyObjectDamage(player, 2, (const sint32 *)psx_addr(0x800A71CC, 12), 0);
                        }
                    }
                }
            }
            else if (phase == 3)
            {
                if (state == 96)
                    sub_8004BDCC(object, 1, 0, 0);
                ClearFacing();
                RestoreBeastState(object);
            }
            return;
        case 112:
            phase = r_u16(object + 474);
            switch (phase)
            {
                case 0:
                    flags = r_u32(object + 396);
                    w_u32(object + 396, flags & 0xFFFFFFFE);
                    motion.velocity[2] = 0;
                    motion.velocity[1] = 0;
                    chosen = r_u16(object + 444);
                    motion.velocity[0] = 0;
                    if (chosen)
                    {
                        chosen = (uint32)(sint32)(sint16)r_u16(object + 464);
                        value = r_u16(0x800A6BAC + chosen * 2);
                    }
                    else
                        value = r_u16(0x800FF4BC);
                    xport_draft_host_sub_8006613C_p1(position, value);
                    PositionBeastBeamPoint(object, position, angles, 0);
                    SetFacingPoint(position);
                    phase = r_u16(object + 474);
                    w_u16(object + 466, 0);
                    w_u16(object + 474, phase + 1);
                    return;
                case 1:
                    counter = r_u16(object + 466);
                    if (counter >= 32)
                    {
                        for (index = 0; index < 2; ++index)
                        {
                            entry = object + 564 + index * 4;
                            child = r_u32(entry);
                            if (child)
                            {
                                table = r_u32(child + 68);
                                adjustment = (sint16)r_u16(table + 8);
                                target = r_u32(table + 12);
                                apocalypse_object_cleanup(target, child + (uint32)adjustment, 3);
                                w_u32(entry, 0);
                            }
                        }
                        ClearFacing();
                        sub_80063118(object, 1, 1);
                        sub_8004BDCC(object, 0, 0, 1);
                        value = ReadBeastBeamPoint(object);
                        xport_draft_host_sub_8006613C_p1(position, value);
                        PositionBeastBeamPoint(object, position, angles, 1);
                        SetBoneFacingPoint(position);
                        phase = r_u16(object + 474);
                        w_u16(object + 466, 0);
                        w_u16(object + 474, phase + 1);
                        return;
                    }
                    for (index = 0; index < 2; ++index)
                    {
                        xport_draft_host_sub_8007CC10_p1(position, object, index + 1);
                        entry = object + 564 + index * 4;
                        if (!r_u32(entry))
                        {
                            child = sub_80032DC0(104);
                            if (child)
                                child = apocalypse_ring_effect_native(child, position, 32, 32, 128, 128, 128, 64, 16, 0);
                            w_u32(entry, child);
                        }
                        child = r_u32(entry);
                        xport_draft_host_sub_80032EE4_p2(child, position);
                        child = r_u32(entry);
                        step = (sint16)r_u16(object + 466);
                        sub_80034994(child, 2 * step + 16);
                    }
                    w_u16(object + 466, counter + 1);
                    return;
                case 2:
                case 3:
                    if (phase == 3 && r_u32(object + 220) < 768)
                    {
                        sub_8004BDCC(object, 0, 0, 0);
                        ClearBoneFacing();
                        RestoreBeastState(object);
                        return;
                    }
                    step = (sint16)r_u16(object + 466);
                    if (step < (phase == 2 ? 48 : 64))
                    {
                        w_u16(object + 466, step + 1);
                        return;
                    }
                    value = ReadBeastBeamPoint(object);
                    xport_draft_host_sub_8006613C_p1(position, value);
                    PositionBeastBeamPoint(object, position, angles, phase == 2 ? 2 : 0);
                    SetBoneFacingPoint(position);
                    phase = r_u16(object + 474);
                    w_u16(object + 466, 0);
                    w_u16(object + 474, phase + 1);
                    return;
                case 4:
                    step = (sint16)r_u16(object + 466);
                    if (step < 48)
                    {
                        player = visual_lists.players;
                        distance = sub_80066918(object + 4, player + 4);
                        scale = (uint32)((3 * (sint16)this->model.scale[0]) >> 4);
                        if (distance >= scale)
                        {
                            w_u16(object + 466, step + 1);
                            return;
                        }
                    }
                    sub_8004BDCC(object, 0, 0, 0);
                    ClearBoneFacing();
                    RestoreBeastState(object);
                    return;
                default:
                    return;
            }
        case 128:
            phase = r_u16(object + 474);
            if (!phase)
            {
                BeginBeastAttack(*this);
                PlayBeastAnimation(object, 4);
                AdvanceBeastPhase(object);
            }
            else if (phase == 1)
            {
                if ((sint8)this->model.frame == 15)
                {
                    player = visual_lists.players;
                    distance = sub_80066918(object + 4, player + 4);
                    scale = (uint32)((3 * (sint16)this->model.scale[0]) >> 4);
                    if (distance < scale)
                    {
                        player = visual_lists.players;
                        health = (sint16)r_u16(0x800EC526);
                        if (health < 0)
                            health += 3;
                        health >>= 2;
                        ApplyObjectDamage(player, health, (const sint32 *)psx_addr(0x800A71CC, 12), 0);
                    }
                }
                else if (r_u8(object + 303))
                {
                    ClearFacing();
                    RestoreBeastState(object);
                }
            }
            return;
        default:
            return;
    }
}

uint32 TheBeast::SetScriptParameters(uint32 command)
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 cursor, base, value, group, lane;
    FUNCTION_MARKER(0x80044474, "SLUS_003.73");
    command &= 65535;
    if (command != 0x2130)
        return Enemy::ReadParameters(command);
    base = r_u32(object + 400) + 2;
    w_u32(object + 400, base);
    for (lane = 0; lane < 4; ++lane)
    {
        value = r_u16(base + lane * 2);
        w_u32(object + 400, base + lane * 2 + 2);
        w_u16(0x800FF4B4 + lane * 2, value);
    }
    cursor = base + 8;
    for (group = 0; group < 5; ++group)
    {
        for (lane = 0; lane < 5; ++lane)
        {
            cursor = r_u32(object + 400);
            value = r_u16(cursor);
            cursor += 2;
            w_u16(0x800A6B94 + group * 12 + lane * 2, value);
            w_u32(object + 400, cursor);
        }
    }
    value = r_u16(cursor);
    w_u32(object + 400, cursor + 2);
    w_u16(0x800FF4BE, value);
    value = r_u16(cursor + 2);
    w_u32(object + 400, cursor + 4);
    w_u16(0x800FF4BC, value);
    return cursor + 4;
}

uint32 TheBeast::ExecuteCommand(uint32 command)
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 cursor, position[4], shift = 12, operand, table, target, height;
    sint32 adjustment;
    FUNCTION_MARKER(0x80044608, "SLUS_003.73");
    command &= 65535;
    switch (command)
    {
        case 0x4220:
        case 0x4223:
        case 0x4222:
        case 0x4225:
            cursor = (r_u32(object + 400) + 3) & 0xFFFFFFFC;
            position[0] = r_u32(cursor);
            position[1] = r_u32(cursor + 4);
            position[2] = r_u32(cursor + 8);
            ShiftVectorLeft(position, &shift);
            if (command == 0x4222 || command == 0x4225)
                AddVectors(position, (const uint32 *)this->model.pos);
            w_u32(object + 400, cursor + 12);
            break;
        case 0x4221:
        case 0x4224:
            cursor = r_u32(object + 400);
            operand = r_u16(cursor);
            w_u32(object + 400, cursor + 2);
            if (operand & 0x2000)
            {
                table = r_u32(object + 68);
                adjustment = (sint16)r_u16(table + 80);
                target = r_u32(table + 84);
                operand = DispatchObjectParameterGetter(target, object + (uint32)adjustment, operand);
            }
            ReadRecordPosition(position, operand & 65535);
            height = apocalypse_ground_native(position, 512, 2048);
            if (height != (uint32)-1)
                position[1] = height - ((uint32)(sint32)(sint16)r_u16(object + 456) << 12);
            break;
        case 0x4262:
            w_u32(object + 400, r_u32(object + 400) + 2);
            return 1;
        default:
            return Enemy::ExecuteCommand(command);
    }
    if (command == 0x4220 || command == 0x4221 || command == 0x4222)
        TheBeast::SetDestination(position);
    else
        TheBeast::SetJumpDestination(position);
    return 1;
}

uint32 TheBeast::SetDestination(const uint32 *position)
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 mode, x, y, z, flags;
    FUNCTION_MARKER(0x8004481C, "SLUS_003.73");
    mode = this->model.animation;
    if (mode && mode != 3)
        sub_80063038(object, 0, 0, (uint32)-1);
    x = ReadTheBeastTargetWord(position, 0);
    y = ReadTheBeastTargetWord(position, 1);
    z = ReadTheBeastTargetWord(position, 2);
    w_u32(object + 484, x);
    w_u32(object + 488, y);
    w_u32(object + 492, z);
    flags = (r_u32(object + 396) | 1) & 0xFFFFFFF3;
    w_u32(object + 396, flags);
    return flags;
}

uint32 TheBeast::SetJumpDestination(const uint32 *position)
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 x, y, z, flags, timer;
    FUNCTION_MARKER(0x80044898, "SLUS_003.73");
    sub_80063038(object, 10, 0, (uint32)-1);
    x = ReadTheBeastTargetWord(position, 0);
    y = ReadTheBeastTargetWord(position, 1);
    z = ReadTheBeastTargetWord(position, 2);
    w_u32(object + 484, x);
    w_u32(object + 488, y);
    w_u32(object + 492, z);
    flags = r_u32(object + 396);
    timer = r_u16(object + 438);
    flags |= 5;
    w_u16(object + 544, timer);
    w_u32(object + 396, flags);
    return flags;
}

uint32 TheBeast::TrackTarget(uint32 target)
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 result, flags;
    FUNCTION_MARKER(0x80044900, "SLUS_003.73");
    result = sub_80062F48(target);
    if (!result)
    {
        flags = r_u32(object + 396);
        w_u32(object + 528, target);
        result = flags | 16;
        w_u32(object + 396, result);
    }
    return result;
}

uint32 TheBeast::SetFacingPoint(const uint32 *position)
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 x, y, z, flags;
    FUNCTION_MARKER(0x8004494C, "SLUS_003.73");
    x = ReadTheBeastTargetWord(position, 0);
    y = ReadTheBeastTargetWord(position, 1);
    z = ReadTheBeastTargetWord(position, 2);
    w_u32(object + 532, x);
    w_u32(object + 536, y);
    w_u32(object + 540, z);
    flags = r_u32(object + 396) | 32;
    w_u32(object + 396, flags);
    return flags;
}

uint16 *TheBeast::ClearFacing()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 flags;
    FUNCTION_MARKER(0x80044978, "SLUS_003.73");
    flags = r_u32(object + 396);
    w_u32(object + 396, flags & 0xFFFFFFCF);
    motion.angular_velocity[2] = 0;
    motion.angular_velocity[1] = 0;
    motion.angular_velocity[0] = 0;
    motion.angular_acceleration[2] = 0;
    motion.angular_acceleration[1] = 0;
    motion.angular_acceleration[0] = 0;
    return motion.angular_acceleration;
}

uint32 TheBeast::SetBoneFacingPoint(const uint32 *position)
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 x, y, z, flags;
    FUNCTION_MARKER(0x800449AC, "SLUS_003.73");
    x = ReadTheBeastTargetWord(position, 0);
    y = ReadTheBeastTargetWord(position, 1);
    z = ReadTheBeastTargetWord(position, 2);
    w_u32(object + 496, x);
    w_u32(object + 500, y);
    w_u32(object + 504, z);
    flags = r_u32(object + 396) | 64;
    w_u32(object + 396, flags);
    return flags;
}

uint32 TheBeast::ClearBoneFacing()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 flags;
    FUNCTION_MARKER(0x800449D8, "SLUS_003.73");
    flags = r_u32(object + 396) & 0xFFFFFFBF;
    w_u32(object + 396, flags);
    return flags;
}





extern "C" uint32 sub_80040B64(uint32 object, uint32 reason)
{
    FUNCTION_MARKER(0x80040B64, "SLUS_003.73");
    return TheBeast::Attach(object).TheBeast::Destroy(reason);
}





extern "C" uint32 sub_800422C0(uint32 object, uint32 damage, uint32, uint32 reason)
{
    FUNCTION_MARKER(0x800422C0, "SLUS_003.73");
    return TheBeast::Attach(object).TheBeast::Damage(damage, 0, reason);
}

extern "C" uint32 sub_80044474(uint32 object, uint32 command)
{
    FUNCTION_MARKER(0x80044474, "SLUS_003.73");
    return TheBeast::Attach(object).TheBeast::SetScriptParameters(command);
}

extern "C" uint32 sub_80044608(uint32 object, uint32 command)
{
    FUNCTION_MARKER(0x80044608, "SLUS_003.73");
    return TheBeast::Attach(object).TheBeast::ExecuteCommand(command);
}















extern "C" uint32 apocalypse_beast_destination_native(uint32 object, const uint32 *position)
{
    return TheBeast::Attach(object).SetDestination(position);
}

extern "C" uint32 apocalypse_beast_jump_destination_native(uint32 object, const uint32 *position)
{
    return TheBeast::Attach(object).SetJumpDestination(position);
}

extern "C" uint32 apocalypse_beast_facing_point_native(uint32 object, const uint32 *position)
{
    return TheBeast::Attach(object).SetFacingPoint(position);
}

extern "C" uint32 apocalypse_beast_bone_facing_point_native(uint32 object, const uint32 *position)
{
    return TheBeast::Attach(object).SetBoneFacingPoint(position);
}

extern "C" uint32 sub_80040F34(uint32 object)
{
    FUNCTION_MARKER(0x80040F34, "SLUS_003.73");
    return TheBeast::Attach(object).TheBeast::Update();
}


