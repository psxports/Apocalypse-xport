#include "enemy_list.h"
#include "model_resources.h"
#include "screen_flash.h"
#include "visual_object.h"
#include "angle_math.h"
#include "object_damage.h"
#include "vector_arithmetic.h"
#include "model_animation.h"
#include "game_random.h"
#include "game_sdk_bindings.h"
#include "object_force.h"
#include "level_session.h"
#include "projectile_trail.h"
#include "game_panel.h"
extern "C" {
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
}
#include "death.h"
#include "game_panel.h"
#include "death_effects.h"
#include "spawn_points.h"
#include "native_object_storage.h"
#include "game_object_allocation.h"
#include "audio_voice.h"
#include <new>
#include <stdio.h>
#include <stdlib.h>

Death::Death(const uint8 *record_data, uint32 index) : Enemy()
{
    const uint32 data = GuestMemoryAddress(record_data);
    uint32 object = NativeObjectGuestAddress(this);
    uint32 cursor, roll;
    FUNCTION_MARKER(0x80047A9C, "SLUS_003.73");
    if (!data)
    {
        // TODO Support native record data in the original parser adapter
        GameDiagnostic("Death record data is outside guest memory\n");
        abort();
    }
    w_u32(object + 68, 0x800A2648);
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

uint32 Death::Configure()
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 flags, behavior, damage, height, product;
    FUNCTION_MARKER(0x80047B34, "SLUS_003.73");
    sub_800626C8(object, 0x800FF4C8);
    motion.drag[0] = 31;
    motion.drag[1] = 3;
    motion.drag[2] = 31;
    AddToList(visual_lists.actors);
    flags = this->model.flags;
    behavior = r_u32(object + 396);
    this->model.flags = flags | 2048;
    w_u32(object + 396, behavior | 64);
    StartAnimation(0, 1);
    sub_8004BD04(object, 2, 8);
    sub_8004BDCC(object, 0, 0, 0);
    sub_8004BDCC(object, 0, 1, 10);
    sub_8004BDCC(object, 1, 0, 0);
    damage = sub_8004B914(object, 10);
    sub_8004BDCC(object, 1, 1, damage & 65535);
    sub_8004BDCC(object, 1, 2, 96);
    sub_8004BDCC(object, 1, 3, 30);
    sub_8004BDCC(object, 1, 4, 45);
    w_u16(object + 436, 16);
    w_u16(object + 438, 32);
    // The original stores type221 although the factory branch is222
    this->kind = 221;
    w_u32(object + 208, 128);
    w_u16(object + 456, 128);
    height = (uint32)(sint32)(sint16)r_u16(object + 456);
    w_u16(object + 218, 1200);
    w_u8(object + 454, 0);
    w_u16(object + 472, 0);
    w_u16(object + 474, 0);
    w_u8(object + 381, 0);
    product = height * 6500;
    w_u16(object + 204, 1024);
    w_u32(0x800FF4EC, object);
    flags = this->model.flags;
    w_u16(object + 182, 10);
    this->model.scale[2] = 6500;
    this->model.scale[1] = 6500;
    this->model.scale[0] = 6500;
    this->model.flags = flags | 512;
    height = (uint32)((sint32)product >> 12);
    w_u16(object + 456, height);
    return height;
}

uint32 Death::Destroy(uint32 reason)
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 voices, child, index, entry;
    FUNCTION_MARKER(0x80047CE0, "SLUS_003.73");
    w_u32(object + 68, 0x800A2648);
    RemoveFromList(visual_lists.actors);
    Enemy::FreeBehaviorTable();
    ClearTrackedObjectHealth();
    voices = r_u32(object + 580);
    if (voices)
        StopSoundVoices(voices);
    for (index = 0; index < 10; ++index)
    {
        entry = object + 604 + index * 4;
        child = r_u32(entry);
        if (child)
        {
            sub_80032ED8(child);
            w_u32(entry, 0);
        }
    }
    w_u32(0x800FF4EC, 0);
    Enemy::Destroy(0);
    return (reason & 1) ? ReleaseObjectStorage(object) : 0;
}

void Death::InterpolateFacing(const uint16 desired[3])
{
    SVECTOR motion_cur, motion_target;
    uint32 motion_cur_word, motion_cur_tail, motion_target_word, motion_target_tail;
    uint32 object = NativeObjectGuestAddress(this);
    uint32 packed = r_u32(object + 16);
    uint32 tail = r_u16(object + 20);
    motion_cur_word = packed;
    motion_cur_tail = tail;
    motion_target_word = desired[0] | ((uint32)desired[1] << 16);
    motion_target_tail = desired[2];
    motion_cur.vx = (sint16)motion_cur_word;
    motion_cur.vy = (sint16)(motion_cur_word >> 16);
    motion_cur.vz = (sint16)motion_cur_tail;
    motion_cur.pad = (sint16)(motion_cur_tail >> 16);
    motion_target.vx = (sint16)motion_target_word;
    motion_target.vy = (sint16)(motion_target_word >> 16);
    motion_target.vz = (sint16)motion_target_tail;
    motion_target.pad = (sint16)(motion_target_tail >> 16);
    UpdateAngleAcceleration(motion_cur, motion.angular_velocity, motion.angular_acceleration, motion_target, 32);
}

uint32 Death::Update()
{
    SVECTOR motion_cur, motion_target;
    uint32 motion_cur_word, motion_cur_tail, motion_target_word, motion_target_tail;
    uint32 object = NativeObjectGuestAddress(this);
    uint32 x, y, z, flags, value, phase, strength, player, index, kind, packed, tail, scale, speed, distance;
    uint32 point[4], difference[4], normal[4];
    uint16 desired[4];
    sint32 signed_value, absolute;
    FUNCTION_MARKER(0x80047DBC, "SLUS_003.73");
    x = r_u32(0x800A71CC);
    y = r_u32(0x800A71D0);
    z = r_u32(0x800A71D4);
    motion.acceleration[0] = x;
    motion.acceleration[1] = y;
    motion.acceleration[2] = z;
    if (!r_u8(object + 381))
    {
        SetModelBoneTable(this->model.bank, DeathAttachments);
        w_u8(object + 381, 1);
    }
    if (r_u32(object + 396) & 64)
    {
        if (!r_u32(object + 176))
            w_u32(object + 176, 0x06402020);
        value = r_u16(object + 180);
        if (value < 9216)
            w_u16(object + 180, value + 128);
    }
    else if (r_u8(object + 383))
    {
        value = RandomRange(64);
        w_u32(object + 176, ((value + 64) << 16) | 0x06004040);
        phase = r_u16(object + 584);
        strength = r_u8(object + 383);
        signed_value = rsin(phase & 4095);
        value = strength * (uint32)signed_value;
        if ((sint32)value < 0)
            value += 15;
        signed_value = (sint32)value >> 4;
        absolute = signed_value < 0 ? -signed_value : signed_value;
        phase = r_u16(object + 584);
        w_u16(object + 180, (uint32)absolute + 5632);
        strength = r_u8(object + 383);
        w_u16(object + 584, (phase + 128) & 4095);
        w_u8(object + 383, strength - 1);
    }
    else
    {
        value = r_u16(object + 180);
        if (value < 129)
            w_u32(object + 176, 0);
        else
            w_u16(object + 180, value - 128);
    }
    value = r_u16(object + 576) + 1;
    w_u16(object + 576, value);
    if ((value & 0xFFFF) >= 451)
    {
        w_u16(object + 576, 0);
        for (index = 0; index < 8; ++index)
        {
            value = RandomRange(512);
            player = visual_lists.players;
            x = r_u32(player + 4);
            point[0] = x + ((value - 256) << 12);
            y = r_u32(player + 8);
            point[1] = y - 4194304;
            value = RandomRange(1024);
            player = visual_lists.players;
            z = r_u32(player + 12);
            point[2] = z + ((value + 1024) << 12);
            value = apocalypse_ground_native(point, 0, 4096);
            if (value == (uint32)-1)
                continue;
            value = RandomRange(6);
            switch (value)
            {
                case 0: kind = 7; break;
                case 1: kind = 4; break;
                case 2: kind = 6; break;
                case 3: kind = 3; break;
                case 4: kind = 2; break;
                default: kind = 1; break;
            }
            xport_draft_host_sub_80020EF8_p2(kind, point, 4, 240, 0x800A71CC);
            break;
        }
    }
    flags = this->activation_flags;
    w_u16(object + 388, flags);
    if (flags & 1)
    {
        value = r_u8(object + 384);
        this->activation_flags = flags & 0xFFFE;
        w_u8(object + 384, value + 1);
    }
    if (r_u32(object + 220) < 256)
    {
        player = visual_lists.players;
        if (player)
            ApplyObjectContactImpulse(player, object);
    }
    if (r_u8(object + 380))
    {
        value = r_u16(object + 476);
        if (value)
            w_u16(object + 476, value - 1);
        else
            sub_8004BE90(object, r_u32(object + 400));
    }
    else
    {
        UpdateState();
    }
    if (r_u8(object + 382))
        AddVectors(motion.acceleration, (const uint32 *)psx_addr(0x800A6844, 12));
    sub_8004B9A4(object);
    x = this->model.pos[0];
    z = this->model.pos[2];
    flags = r_u32(object + 396);
    w_u32(object + 184, x);
    w_u32(object + 192, z);
    if (flags & 16)
    {
        player = r_u32(object + 588);
        x = r_u32(player + 4);
        y = r_u32(player + 8);
        z = r_u32(player + 12);
        w_u32(object + 592, x);
        w_u32(object + 596, y);
        w_u32(object + 600, z);
        xport_draft_host_sub_80066B8C_p1(desired, object + 4, object + 592);
        desired[0] = 0;
        InterpolateFacing(desired);
    }
    else if (flags & 32)
    {
        xport_draft_host_sub_80066B8C_p1(desired, object + 4, object + 592);
        desired[0] = 0;
        InterpolateFacing(desired);
    }
    else if (flags & 1)
    {
        point[0] = this->model.pos[0];
        point[1] = r_u32(object + 488);
        point[2] = this->model.pos[2];
        distance = xport_draft_host_sub_8006689C_p12(point, psx_addr(object + 484, 12));
        scale = r_u16(object + 436);
        if (distance < scale * 2)
            w_u32(object + 396, r_u32(object + 396) & ~1);
        else
        {
            xport_draft_host_sub_80066B8C_p1(desired, object + 4, object + 484);
            desired[0] = 0;
            packed = ((uint32)this->model.angles[0] | ((uint32)this->model.angles[1] << 16));
            tail = this->model.angles[2];
            scale = r_u16(object + 438);
            motion_cur_word = packed;
            motion_cur_tail = tail;
            motion_target_word = desired[0] | ((uint32)desired[1] << 16);
            motion_target_tail = desired[2];
            motion_cur.vx = (sint16)motion_cur_word;
            motion_cur.vy = (sint16)(motion_cur_word >> 16);
            motion_cur.vz = (sint16)motion_cur_tail;
            motion_cur.pad = (sint16)(motion_cur_tail >> 16);
            motion_target.vx = (sint16)motion_target_word;
            motion_target.vy = (sint16)(motion_target_word >> 16);
            motion_target.vz = (sint16)motion_target_tail;
            motion_target.pad = (sint16)(motion_target_tail >> 16);
            UpdateAngleAcceleration(motion_cur, motion.angular_velocity, motion.angular_acceleration, motion_target, scale);
            for (index = 0; index < 3; ++index)
            {
                value = r_u32(object + 484 + index * 4);
                x = this->model.pos[index];
                difference[index] = (uint32)((sint32)(value - x) >> 12);
            }
            NormalizeShortVector(difference, normal);
            signed_value = (sint16)motion.angular_velocity[1];
            absolute = signed_value < 0 ? -signed_value : signed_value;
            if (absolute >= 65)
                absolute = 64;
            scale = r_u16(object + 436);
            value = scale * (64 - (uint32)absolute);
            if ((sint32)value < 0)
                value += 63;
            speed = (uint32)((sint32)value >> 6);
            x = normal[0] * speed;
            y = normal[1] * speed;
            z = normal[2] * speed;
            motion.velocity[0] = x;
            motion.velocity[1] = y;
            motion.velocity[2] = z;
        }
    }
    else
        return sub_8007FDF0(object);
    if ((r_u32(object + 396) & 1) && this->model.animation != 2)
        StartAnimation(2, 1);
    return sub_8007FDF0(object);
}

extern "C" uint32 sub_80047DBC(uint32 object)
{
    FUNCTION_MARKER(0x80047DBC, "SLUS_003.73");
    return Death::Attach(object).Death::Update();
}

uint32 Death::Damage(uint32 damage, const sint32 *, uint32 kind)
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 health, result;
    sint32 reduced;
    FUNCTION_MARKER(0x80048428, "SLUS_003.73");
    if (r_u32(object + 396) & 64)
        return 0;
    w_u8(object + 383, 8);
    reduced = (sint32)damage >> 1;
    if (kind == 30)
        reduced >>= 1;
    if (kind == 5 || kind == 10)
        reduced >>= 2;
    health = r_u16(object + 218);
    result = health - (uint32)reduced;
    w_u16(object + 218, result);
    if ((sint32)(result << 16) <= 0)
        ClearTrackedObjectHealth();
    SetColor(3, 255, 255, 255);
    return 1;
}

// Native helpers retain the previously saved decomposition of the original state machine
static void AdvanceDeathPhase(uint32 object)
{
    w_u16(object + 474, r_u16(object + 474) + 1);
}

static void ClearDeathMotion(uint32 object)
{
    Death::Attach(object).motion.velocity[2] = 0;
    Death::Attach(object).motion.velocity[1] = 0;
    Death::Attach(object).motion.velocity[0] = 0;
}

static void HoldDeathTarget(uint32 object, uint32 mask)
{
    uint32 flags = r_u32(object + 396);
    uint32 player = visual_lists.players;
    w_u32(object + 396, flags & mask);
    ClearDeathMotion(object);
    sub_80049C98(object, player);
}

static void RestoreDeathState(uint32 object)
{
    uint32 state = r_u16(object + 440);
    uint32 phase = r_u16(object + 442);
    w_u16(object + 472, state);
    w_u16(object + 474, phase);
}

static void DamageDeathTarget(uint32 player, uint32 damage)
{
    uint32 receiver = player;
    ApplyObjectDamage(receiver, damage, (const sint32 *)psx_addr(0x800A71CC, 12), 0);
}

static uint32 CreateDeathBeam(uint32 red, uint32 green, uint32 blue)
{
    uint32 child = sub_80032DC0(164);
    if (child)
        child = sub_80035DC8(child, red, green, blue, 1, 5, 0, 700, 100, 100, 440, 110, 1100, 93);
    return child;
}

static void ReadDeathSpawnPoint(uint32 object, uint32 index, uint32 point[3])
{
    uint32 state = r_u16(object + 440);
    uint32 waypoint = r_u16(object + (state == 5 ? 552 : 504) + index * 2);
    xport_draft_host_sub_8006613C_p1(point, waypoint);
}

static void SelectDeathDestination(uint32 object)
{
    uint32 point[3], index, floor, height;
    do
    {
        index = RandomRange(4);
        index = r_u16(object + 496 + index * 2);
        xport_draft_host_sub_8006613C_p1(point, index);
    } while (apocalypse_position_distance_native(point, psx_addr(object + 4, 12)) < 257);
    floor = apocalypse_ground_native(point, 512, 2048);
    height = (uint32)(sint32)(sint16)r_u16(object + 456);
    point[1] = floor - (height << 12);
    apocalypse_death_destination_native(object, point);
    AdvanceDeathPhase(object);
}

static void UpdateDeathAttack(uint32 object, uint32 phase)
{
    uint32 point[3], origin[3], child, value, index, player, state, next_phase, speed, damage;
    sint16 angles[4];
    sint32 timer;
    switch (phase)
    {
        case 0:
            w_u32(object + 396, r_u32(object + 396) | 64);
            Death::Attach(object).StartAnimation(0, 1);
            w_u16(object + 462, 190);
            HoldDeathTarget(object, 0xFFFFFFFE);
            value = visual_lists.solids;
            index = r_u16(object + 574);
            ::ActivateObjectsByIndex(value, index);
            AdvanceDeathPhase(object);
            return;
        case 1:
            value = r_u16(object + 462);
            timer = (sint16)r_u16(object + 462);
            if (value)
                w_u16(object + 462, (uint32)timer - 1);
            else
            {
                Death::Attach(object).ClearFacing();
                AdvanceDeathPhase(object);
            }
            return;
        case 2:
            timer = (sint16)r_u16(object + 462);
            value = r_u16(object + 546 + (uint32)timer * 2);
            xport_draft_host_sub_8006613C_p1(point, value);
            value = (uint32)(sint32)(sint16)r_u16(object + 456);
            point[1] -= value << 12;
            apocalypse_death_destination_native(object, point);
            AdvanceDeathPhase(object);
            return;
        case 3:
            if (!(r_u32(object + 396) & 1))
            {
                timer = (sint16)r_u16(object + 462);
                value = (uint32)timer + 1;
                if ((sint32)value < 3)
                {
                    w_u16(object + 462, value);
                    w_u16(object + 474, r_u16(object + 474) - 1);
                }
                else
                    AdvanceDeathPhase(object);
            }
            return;
        case 4:
            HoldDeathTarget(object, 0xFFFFFFBE);
            Death::Attach(object).SetAnimationRange(6, 0, (uint32)-1);
            child = CreateDeathBeam(32, 0, 96);
            value = r_u16(object + 572);
            w_u32(object + 604, child);
            xport_draft_host_sub_8006613C_p1(point, value);
            xport_draft_host_sub_8007CC10_p1(origin, object, 0);
            child = r_u32(object + 604);
            apocalypse_endpoint_beam_native(child, origin, point);
            value = r_u16(object + 474);
            w_u16(object + 460, 0);
            w_u16(object + 462, 0);
            w_u16(object + 474, value + 1);
            return;
        case 5:
            value = r_u16(object + 460);
            timer = (sint16)r_u16(object + 460);
            if (value < 15)
            {
                w_u16(object + 460, (uint32)timer + 1);
                w_u8(object + 24, 0);
            }
            else if (value == 15)
            {
                w_u16(object + 460, (uint32)timer + 1);
                sub_80069EF4(90, object + 4, 0);
            }
            value = r_u8(object + 24);
            if (value - 4 < 9)
            {
                player = visual_lists.players;
                xport_draft_host_sub_80066B8C_p1(angles, object + 4, player + 4);
                timer = (sint8)r_u8(object + 24);
                angles[1] = (sint16)(((uint32)(uint16)angles[1] - (((uint32)timer << 6) - 512)) & 4095);
                child = sub_80032DC0(268);
                if (child)
                {
                    speed = sub_8004BDFC(object, 1, 2) & 65535;
                    damage = sub_8004BDFC(object, 1, 1) & 65535;
                    apocalypse_line_projectile_native(child, psx_addr(object + 4, 12), angles, speed, damage, 0x800FF5A0, 64, 32, 128, 1);
                }
                return;
            }
            if (!r_u8(object + 303))
                return;
            value = r_u16(object + 462);
            if (value < 4 || (sint32)r_u32(0x800FF4E4) >= 4)
            {
                w_u16(object + 460, 0);
                w_u16(object + 462, value + 1);
                Death::Attach(object).SetAnimationRange(6, 0, (uint32)-1);
            }
            else
            {
                state = r_u16(object + 472);
                next_phase = r_u16(object + 474);
                w_u16(object + 472, 32);
                w_u16(object + 474, 0);
                w_u16(object + 440, state);
                w_u16(object + 442, next_phase + 1);
            }
            return;
        case 6:
            w_u16(object + 460, 0);
            w_u16(object + 462, 0);
            Death::Attach(object).SetAnimationRange(6, 0, (uint32)-1);
            w_u16(object + 474, r_u16(object + 474) - 1);
            return;
    }
}

static void UpdateDeathSpawn(uint32 object, uint32 phase)
{
    uint32 point[3], origin[3], index, child, resource, kind, value, flags, state;
    sint32 timer;
    switch (phase)
    {
        case 0:
            Death::Attach(object).SetAnimationRange(3, 0, (uint32)-1);
            HoldDeathTarget(object, 0xFFFFFFFE);
            value = r_u16(object + 474);
            w_u16(object + 462, 0);
            w_u16(object + 474, value + 1);
            return;
        case 1:
            timer = (sint8)r_u8(object + 24);
            value = r_u8(object + 24);
            if (timer < 14)
                return;
            if (!(value & 1))
            {
                timer = (sint16)r_u16(object + 462);
                if (!timer)
                {
                    value = sub_80069EF4(95, object + 4, 0);
                    w_u32(object + 580, value);
                }
                if (timer < 10)
                {
                    ReadDeathSpawnPoint(object, (uint32)timer, point);
                    child = CreateDeathBeam(8, 24, 8);
                    w_u32(object + 604 + (uint32)timer * 4, child);
                    w_u16(object + 462, (uint32)timer + 1);
                }
                else if ((sint8)r_u8(object + 24) >= 58)
                    AdvanceDeathPhase(object);
            }
            xport_draft_host_sub_8007CC10_p1(origin, object, 0);
            for (index = 0; (sint32)index < (sint16)r_u16(object + 462); ++index)
            {
                ReadDeathSpawnPoint(object, index, point);
                child = r_u32(object + 604 + index * 4);
                apocalypse_endpoint_beam_native(child, origin, point);
            }
            return;
        case 2:
            ::StopSoundVoices(r_u32(object + 580));
            w_u32(object + 580, 0);
            for (index = 0; index < 10; ++index)
            {
                sub_80032ED8(r_u32(object + 604 + index * 4));
                w_u32(object + 604 + index * 4, 0);
                ReadDeathSpawnPoint(object, index, point);
                SpawnDeathFadingRing(point);
                value = point[1];
                kind = RandomRange(256) + 128;
                point[1] -= kind << 12;
                child = sub_800625AC(320);
                if (child)
                {
                    kind = r_u8(0x800FF644);
                    resource = ::FindModelResourceIndex(0x3CD11145, kind);
                    kind = (resource + RandomRange(16)) & 65535;
                    flags = r_u32(0x800FF62C);
                    child = apocalypse_debris_native(child, point, 0x800A71CC, kind, value, 9, 999, flags);
                }
                w_u32(object + 644 + index * 4, child);
                flags = r_u16(child);
                w_u16(child, flags | 512);
                child = r_u32(object + 644 + index * 4);
                w_u16(child + 36, 8192);
                w_u16(child + 38, 8192);
                w_u16(child + 40, 8192);
                child = r_u32(object + 644 + index * 4);
                w_u8(child + 296, 64);
            }
            AdvanceDeathPhase(object);
            return;
        case 3:
            if ((sint8)r_u8(object + 24) < 58)
                return;
            sub_80069DF0(94, 8192, 0);
            ReadDeathSpawnPoint(object, 0, point);
            point[1] = r_u32(object + 8);
            for (index = 0; index < 10; ++index)
            {
                child = r_u32(object + 644 + index * 4);
                w_u32(child + 308, point[0]);
                w_u32(child + 312, point[1]);
                w_u32(child + 316, point[2]);
                child = r_u32(object + 644 + index * 4);
                flags = r_u16(child + 300);
                w_u16(child + 300, flags & 0xFFFE);
                child = r_u32(object + 644 + index * 4);
                flags = r_u16(child + 300);
                w_u16(child + 300, flags | 256);
            }
            AdvanceDeathPhase(object);
            return;
        case 4:
            if ((sint8)r_u8(object + 24) < 72)
                return;
            ::SetScreenFlash(64, 255, 0, 16, 0, 0);
            for (index = 0; index < 10; ++index)
            {
                child = r_u32(object + 644 + index * 4);
                value = r_u32(child + 68);
                flags = child + (uint32)(sint32)(sint16)r_u16(value + 16);
                value = r_u32(value + 20);
                apocalypse_object_virtual20(value, flags);
                w_u32(object + 644 + index * 4, 0);
            }
            ReadDeathSpawnPoint(object, 0, point);
            point[1] = r_u32(object + 8);
            child = sub_800625AC(624);
            if (child)
                apocalypse_swat_spawn_native(child, point, 0x800A6C18);
            sub_80069DF0(93, 8192, 0);
            state = r_u16(object + 440);
            ShuffleSpawnPoints((uint16 *)psx_addr(object + (state == 5 ? 552 : 504), 2), state == 5 ? 10 : 21);
            AdvanceDeathPhase(object);
            return;
        case 5:
            if (r_u8(object + 303))
            {
                Death::Attach(object).ClearFacing();
                RestoreDeathState(object);
            }
            return;
    }
}

static void UpdateDeathSummon(uint32 object, uint32 phase)
{
    uint32 point[3], origin[3], value, index, child;
    sint32 timer;
    switch (phase)
    {
        case 0:
            HoldDeathTarget(object, 0xFFFFFFFE);
            Death::Attach(object).SetAnimationRange(3, 0, (uint32)-1);
            value = r_u16(object + 474);
            w_u16(object + 462, 0);
            w_u16(object + 474, value + 1);
            return;
        case 1:
            if ((sint8)r_u8(object + 24) < 14)
                return;
            value = sub_80069EF4(95, object + 4, 0);
            w_u32(object + 580, value);
            for (index = 0; index < 5; ++index)
            {
                value = r_u16(object + 504 + index * 2);
                xport_draft_host_sub_8006613C_p1(point, value);
                child = CreateDeathBeam(8, 8, 24);
                w_u32(object + 604 + index * 4, child);
                xport_draft_host_sub_8007CC10_p1(origin, object, 0);
                child = r_u32(object + 604 + index * 4);
                apocalypse_endpoint_beam_native(child, origin, point);
                SpawnDeathPersistentRing(point);
                child = sub_800625AC(624);
                if (child)
                    apocalypse_swat_spawn_native(child, point, 0x800A6C28);
            }
            ShuffleSpawnPoints((uint16 *)psx_addr(object + 504, 2), 21);
            AdvanceDeathPhase(object);
            return;
        case 2:
            timer = (sint16)r_u16(object + 462);
            if (timer < 30)
            {
                w_u16(object + 462, (uint32)timer + 1);
                return;
            }
            ::StopSoundVoices(r_u32(object + 580));
            w_u32(object + 580, 0);
            for (index = 0; index < 5; ++index)
            {
                sub_80032ED8(r_u32(object + 604 + index * 4));
                w_u32(object + 604 + index * 4, 0);
            }
            value = RandomRange(2);
            sub_80069DF0(value ? 91 : 92, 8192, 0);
            AdvanceDeathPhase(object);
            return;
        case 3:
            if (r_u8(object + 303))
            {
                Death::Attach(object).ClearFacing();
                RestoreDeathState(object);
            }
            return;
    }
}

static void UpdateDeathEnding(uint32 object, uint32 phase)
{
    uint32 point[3], destination[3], value, index, child, player, camera, node;
    sint16 angles[4];
    sint32 timer, interval;
    if (phase)
    {
        value = ((uint32)(sint32)(sint16)r_u16(object + 464) - 32) & 4095;
        camera = visual_lists.cameras;
        w_u16(object + 464, value);
        sub_8007851C(camera, value, 4);
        if (!RandomRange(30))
        {
            value = RandomRange(2);
            sub_80069EF4(value + 96, object + 4, 3);
        }
        timer = (sint16)r_u16(object + 466);
        if (timer)
            w_u16(object + 466, (uint32)timer - 1);
        else
        {
            interval = (sint16)r_u16(object + 468);
            w_u16(object + 468, interval >= 4 ? (uint32)interval - 3 : 1);
            w_u16(object + 466, (uint32)interval - 1);
            xport_draft_host_sub_8007CC10_p1(point, object, 3);
            value = RandomRange(16);
            point[1] += value << 12;
            destination[0] = point[0];
            destination[1] = point[1] - 0x800000;
            destination[2] = point[2];
            angles[0] = -1024;
            angles[1] = 0;
            angles[2] = 0;
            child = sub_80032DC0(108);
            if (child)
                {
                    void *dram_addr = AllocateNativeObjectStorage(child, sizeof(TrailProjectile));
                    new (dram_addr) TrailProjectile(point, destination, (const uint16 *)angles, 16, 64, 64, 240, 200, 200, 0, 0, 128);
                }
        }
    }
    phase = r_u16(object + 474);
    switch (phase)
    {
        case 0:
            node = visual_lists.actors;
            player = visual_lists.players;
            w_u32(player + 452, 900);
            while (node)
            {
                if (r_u16(node + 58) == 200 && r_u16(node + 390) == 128)
                {
                    value = (uint32)(sint32)(sint16)r_u16(node + 218);
                    player = node;
                    ApplyObjectDamage(player, value, (const sint32 *)psx_addr(0x800A71CC, 12), 0);
                }
                node = r_u32(node + 28);
            }
            value = r_u32(object + 580);
            if (value)
            {
                ::StopSoundVoices(value);
                w_u32(object + 580, 0);
            }
            sub_80069EF4(96, object + 4, 3);
            sub_80069EF4(96, object + 4, 3);
            sub_80069EF4(96, object + 4, 3);
            for (index = 0; index < 10; ++index)
            {
                child = r_u32(object + 604 + index * 4);
                if (child)
                {
                    sub_80032ED8(child);
                    w_u32(object + 604 + index * 4, 0);
                }
            }
            Death::Attach(object).ClearFacing();
            w_u32(object + 396, r_u32(object + 396) & 0xFFFFFFFE);
            ClearDeathMotion(object);
            Death::Attach(object).motion.angular_velocity[2] = 0;
            Death::Attach(object).motion.angular_velocity[1] = 0;
            Death::Attach(object).motion.angular_velocity[0] = 0;
            Death::Attach(object).motion.angular_acceleration[2] = 0;
            Death::Attach(object).motion.angular_acceleration[1] = 0;
            Death::Attach(object).motion.angular_acceleration[0] = 0;
            Death::Attach(object).SetAnimationRange(8, 0, (uint32)-1);
            camera = visual_lists.cameras;
            w_u16(object + 464, 2048);
            w_u16(object + 466, 16);
            w_u16(object + 468, 16);
            w_u16(object + 462, 0);
            if (camera)
            {
                w_u32(camera + 304, 1);
                w_u32(camera + 296, object);
                w_u32(camera + 340, 1);
                w_u32(camera + 336, object);
                sub_8007851C(camera, 2048, 4);
                sub_800785D8(visual_lists.cameras, 1332, 16);
                sub_80078618(visual_lists.cameras, (uint32)-1608, 64);
                sub_8007865C(visual_lists.cameras, 0, 16);
                sub_800786A0(visual_lists.cameras, 460, 16);
                sub_800786E4(visual_lists.cameras, 564, 16);
                sub_80077490(visual_lists.cameras, 0);
            }
            AdvanceDeathPhase(object);
            return;
        case 1:
            if ((sint8)r_u8(object + 24) < 89)
                return;
            xport_draft_host_sub_8001D320_p1(psx_addr(object + 4, 12), 256, 32, 32, 128, 4, phase, 100);
            value = r_u16(object + 474);
            w_u16(object + 460, 0);
            w_u16(object + 474, value + 1);
            return;
        case 2:
            timer = (sint16)r_u16(object + 460);
            w_u8(object + 24, 90);
            if (timer < 180)
                w_u16(object + 460, (uint32)timer + 1);
            else
            {
                value = r_u16(object + 474);
                w_u16(object + 460, 0);
                w_u16(object + 474, value + 1);
            }
            return;
        case 3:
            timer = (sint16)r_u16(object + 460);
            w_u8(object + 24, 90);
            if (timer < 128)
                w_u16(object + 460, (uint32)timer + 1);
            else
            {
                value = r_u32(object + 68);
                player = object + (uint32)(sint32)(sint16)r_u16(value + 16);
                value = r_u32(value + 20);
                apocalypse_object_virtual20(value, player);
                CompleteLevel(0);
            }
            return;
    }
}

void Death::UpdateState()
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 state, phase, value, flags, floor, height, player;
    sint32 timer;
    FUNCTION_MARKER(0x800484D0, "SLUS_003.73");
    if ((sint16)r_u16(object + 218) <= 0 && r_u16(object + 472) != 96)
    {
        w_u16(object + 472, 96);
        w_u16(object + 474, 0);
    }
    state = r_u16(object + 472);
    phase = r_u16(object + 474);
    switch (state)
    {
        case 0:
            if (!phase)
            {
                floor = sub_80067A18(object + 4, 0, 4096);
                if (floor == (uint32)-1)
                    return;
                value = this->model.pos[1];
                height = (uint32)(sint32)(sint16)r_u16(object + 456);
                if (((sint32)(floor - value) >> 12) >= (sint32)(height << 1))
                    return;
                value = floor - (height << 12);
                this->model.pos[1] = value;
                height = floor - value;
                phase = r_u16(object + 474);
                w_u32(object + 188, floor);
                w_u8(object + 382, 0);
                w_u16(object + 202, (uint32)((sint32)height >> 12));
                w_u16(object + 474, phase + 1);
                ClearDeathMotion(object);
                w_u16(object + 462, 0);
                w_u16(object + 218, 1200);
                TrackObjectHealth(object, 24, 42, 4096);
            }
            else if (phase == 1)
            {
                timer = (sint16)r_u16(object + 462);
                w_u16(object + 462, (uint32)timer + 1);
                if (timer != 60 && timer >= 180)
                {
                    flags = r_u32(object + 396);
                    value = r_u16(object + 472);
                    w_u16(object + 474, 0);
                    w_u32(object + 396, flags & 0xFFFFFFBF);
                    w_u16(object + 472, value + 1);
                }
            }
            return;
        case 1:
            if (!phase)
            {
                value = r_u16(object + 474);
                w_u16(object + 462, 0);
                w_u16(object + 474, value + 1);
                SelectDeathDestination(object);
            }
            else if (phase == 1)
                SelectDeathDestination(object);
            else if (phase == 2)
            {
                if ((sint16)r_u16(object + 218) < 960)
                {
                    w_u16(object + 472, phase);
                    w_u16(object + 474, 0);
                    return;
                }
                timer = (sint16)r_u16(object + 462);
                w_u16(object + 462, (uint32)timer + 1);
                if (r_u32(object + 220) < 768)
                {
                    value = r_u16(object + 472);
                    phase = r_u16(object + 474);
                    w_u16(object + 472, 16);
                    w_u16(object + 474, 0);
                    w_u16(object + 440, value);
                    w_u16(object + 442, phase);
                }
                else if (timer == 30 && (sint32)r_u32(0x800FF4E4) < 6)
                {
                    value = r_u16(object + 472);
                    w_u16(object + 442, 0);
                    w_u16(object + 472, 32);
                    w_u16(object + 474, 0);
                    w_u16(object + 440, value);
                }
                else if (!(r_u32(object + 396) & 1))
                    w_u16(object + 474, 1);
            }
            return;
        case 2:
            if (!phase)
                SelectDeathDestination(object);
            else if (phase == 1)
            {
                if ((sint16)r_u16(object + 218) < 720)
                {
                    w_u16(object + 472, 5);
                    w_u16(object + 474, 0);
                }
                else if (r_u32(object + 220) < 768)
                {
                    w_u16(object + 440, state);
                    value = r_u16(object + 474);
                    w_u16(object + 472, 16);
                    w_u16(object + 474, 0);
                    w_u16(object + 442, value);
                }
                else if ((sint32)r_u32(0x800FF4E4) < 4)
                {
                    w_u16(object + 440, state);
                    w_u16(object + 442, 0);
                    w_u16(object + 472, 64);
                    w_u16(object + 474, 0);
                }
                else if (!(r_u32(object + 396) & 1))
                    w_u16(object + 474, 0);
            }
            return;
        case 5:
            UpdateDeathAttack(object, phase);
            return;
        case 16:
            if (!phase)
            {
                w_u32(object + 396, r_u32(object + 396) & 0xFFFFFFFE);
                ClearDeathMotion(object);
                ClearFacing();
                SetAnimationRange(5, 0, (uint32)-1);
                sub_80069EF4(90, object + 4, 0);
                AdvanceDeathPhase(object);
            }
            else if (phase == 1)
            {
                value = this->model.angles[1];
                flags = this->model.frame;
                this->model.angles[1] = (value - 113) & 4095;
                if (flags - 8 < 13 && r_u32(object + 220) < 768)
                {
                    player = visual_lists.players;
                    DamageDeathTarget(player, 2);
                }
                if (r_u8(object + 303))
                {
                    SetAnimationRange(7, 0, (uint32)-1);
                    AdvanceDeathPhase(object);
                }
            }
            else if (phase == 2 && r_u8(object + 303))
                RestoreDeathState(object);
            return;
        case 32:
            UpdateDeathSpawn(object, phase);
            return;
        case 64:
            UpdateDeathSummon(object, phase);
            return;
        case 96:
            UpdateDeathEnding(object, phase);
            return;
    }
}



uint32 Death::ReadParameters(uint32 command)
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 cursor, value, index;
    FUNCTION_MARKER(0x80049B38, "SLUS_003.73");
    command &= 0xFFFF;
    if (command != 0x2130)
        return Enemy::ReadParameters(command);
    cursor = r_u32(object + 400);
    w_u32(object + 400, cursor + 2);
    for (index = 0; index < 4; ++index)
    {
        cursor = r_u32(object + 400);
        value = r_u16(cursor);
        w_u16(object + 496 + index * 2, value);
        w_u32(object + 400, cursor + 2);
    }
    for (index = 0; index < 3; ++index)
    {
        cursor = r_u32(object + 400);
        value = r_u16(cursor);
        cursor += 2;
        w_u16(object + 546 + index * 2, value);
        w_u32(object + 400, cursor);
    }
    value = r_u16(cursor);
    w_u32(object + 400, cursor + 2);
    w_u16(object + 572, value);
    for (index = 0; index < 21; ++index)
    {
        cursor = r_u32(object + 400);
        value = r_u16(cursor);
        cursor += 2;
        w_u16(object + 504 + index * 2, value);
        w_u32(object + 400, cursor);
    }
    value = r_u16(cursor);
    w_u32(object + 400, cursor + 2);
    w_u16(object + 574, value);
    for (index = 0; index < 10; ++index)
    {
        cursor = r_u32(object + 400);
        value = r_u16(cursor);
        w_u16(object + 552 + index * 2, value);
        w_u32(object + 400, cursor + 2);
    }
    return 0;
}

uint32 Death::ExecuteCommand(uint32 command)
{
    FUNCTION_MARKER(0x80049C48, "SLUS_003.73");
    return Enemy::ExecuteCommand(command & 0xFFFF);
}

uint32 Death::SetDestination(const uint32 *position)
{
    const uint32 object = NativeObjectGuestAddress(this);
    const uint32 guest_position = GuestMemoryAddress(position);
    uint32 x, y, z, flags;
    FUNCTION_MARKER(0x80049C68, "SLUS_003.73");
    if (guest_position)
    {
        x = r_u32(guest_position);
        y = r_u32(guest_position + 4);
        z = r_u32(guest_position + 8);
    }
    else
    {
        x = position[0];
        y = position[1];
        z = position[2];
    }
    w_u32(object + 484, x);
    w_u32(object + 488, y);
    w_u32(object + 492, z);
    flags = (r_u32(object + 396) | 1) & 0xFFFFFFF3;
    w_u32(object + 396, flags);
    return flags;
}

uint32 Death::TrackTarget(uint32 target)
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 result, flags;
    FUNCTION_MARKER(0x80049C98, "SLUS_003.73");
    result = sub_80062F48(target);
    if (!result)
    {
        flags = r_u32(object + 396);
        w_u32(object + 588, target);
        result = (flags & 0xFFFFFFDF) | 16;
        w_u32(object + 396, result);
    }
    return result;
}

uint16 *Death::ClearFacing()
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 flags;
    FUNCTION_MARKER(0x80049D18, "SLUS_003.73");
    flags = r_u32(object + 396);
    flags &= 0xFFFFFFEF;
    flags &= 0xFFFFFFDF;
    w_u32(object + 396, flags);
    motion.angular_velocity[2] = 0;
    motion.angular_velocity[1] = 0;
    motion.angular_velocity[0] = 0;
    motion.angular_acceleration[2] = 0;
    motion.angular_acceleration[1] = 0;
    motion.angular_acceleration[0] = 0;
    return motion.angular_acceleration;
}





extern "C" uint32 sub_80047CE0(uint32 object, uint32 reason)
{
    FUNCTION_MARKER(0x80047CE0, "SLUS_003.73");
    return Death::Attach(object).Death::Destroy(reason);
}

extern "C" uint32 sub_80048428(uint32 object, uint32 damage, uint32, uint32 kind)
{
    FUNCTION_MARKER(0x80048428, "SLUS_003.73");
    return Death::Attach(object).Death::Damage(damage, 0, kind);
}

extern "C" uint32 sub_80049B38(uint32 object, uint32 command)
{
    FUNCTION_MARKER(0x80049B38, "SLUS_003.73");
    return Death::Attach(object).ReadParameters(command);
}

extern "C" uint32 sub_80049C48(uint32 object, uint32 command)
{
    FUNCTION_MARKER(0x80049C48, "SLUS_003.73");
    return Death::Attach(object).Death::ExecuteCommand(command);
}



extern "C" uint32 sub_80049C98(uint32 object, uint32 target)
{
    FUNCTION_MARKER(0x80049C98, "SLUS_003.73");
    return Death::Attach(object).TrackTarget(target);
}



extern "C" uint32 apocalypse_death_destination_native(uint32 object, const uint32 position[3])
{
    return Death::Attach(object).SetDestination(position);
}



void Death::SetParameters(uint32 command)
{
    // The existing native Enemy callback discards V0; the legacy wrapper retains it
    ReadParameters(command);
}

Death &Death::Attach(uint32 object)
{
    return *static_cast<Death *>(FindNativeObjectStorage(object));
}
