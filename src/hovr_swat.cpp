#include "visual_object.h"
#include "model_resources.h"
#include "angle_math.h"
#include "game_sdk_bindings.h"
#include "player.h"
#include "vector_arithmetic.h"
#include "game_random.h"
#include "game_object_allocation.h"
#include "explosion_effects.h"
#include "ring_burst_effect.h"
#include "particle_debris.h"
extern "C" {
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
}
#include "hovr_swat.h"
#include "object_parameters.h"
#include "record_data.h"
#include "native_object_storage.h"
#include "audio_voice.h"
#include <new>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

HovrSwat::HovrSwat(const uint8 *record_data, uint32 index) : Enemy()
{
    const uint32 data = GuestMemoryAddress(record_data);
    uint32 object = NativeObjectGuestAddress(this);
    uint32 cursor;
    FUNCTION_MARKER(0x800590DC, "SLUS_003.73");
    if (!data)
    {
        // TODO Support native record data in the original parser adapter
        GameDiagnostic("HovrSwat record data is outside guest memory\n");
        abort();
    }
    w_u32(object + 68, 0x800A2818);
    cursor = sub_80062B0C(object, data);
    w_u32(object + 400, sub_80062B50(object, cursor));
    this->record_idx = index;
    Init();
}

uint32 HovrSwat::Init()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 random, model, count, hover_count;
    FUNCTION_MARKER(0x800591D4u, "SLUS_003.73");
    count = r_u32(0x800FF308u);
    hover_count = r_u32(0x800FF4E0u);
    w_u32(0x800FF308u, count + 1u);
    w_u32(0x800FF4E0u, hover_count + 1u);
    sub_800626C8(object, 0x800A2784u);
    AddToList(visual_lists.actors);
    this->kind = 201u;
    motion.drag[0] = 3u;
    motion.drag[1] = 3u;
    motion.drag[2] = 3u;
    w_u32(object + 532u, sub_8006696C(object + 4u, visual_lists.cameras + 4u));
    sub_8004BD04(object, 1u, 4u);
    w_u16(object + 436u, 12u);
    w_u16(object + 438u, 128u);
    sub_8004BDCC(object, 0u, 1u, 10u);
    sub_8004BDCC(object, 0u, 2u, 80u);
    sub_8004BDCC(object, 0u, 3u, 90u);
    SetAnimationRange(0u, 0u, 0xFFFFFFFFu);
    InitEffects();
    w_u16(object + 218u, 20u);
    w_u32(object + 528u, sub_80069EF4(101u, object + 4u, 0u));
    random = RandomRange(8u);
    model = r_u8(0x800FF644u);
    w_u16(object + 524u, random + 1u);
    w_u8(object + 568u, ::FindModelResourceIndex(0xDB2D0C4Fu, model));
    w_u8(object + 569u, 10u);
    w_u8(object + 380u, 1u);
    return 1u;
}

uint32 HovrSwat::Destroy(uint32 reason)
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 sound, count, result;
    FUNCTION_MARKER(0x8005933C, "SLUS_003.73");
    sound = r_u32(object + 528);
    w_u32(object + 68, 0x800A2818);
    StopSoundVoices(sound);
    count = r_u32(0x800FF4E0);
    w_u32(0x800FF4E0, count - 1);
    DestroyEffects();
    RemoveFromList(visual_lists.actors);
    Enemy::FreeBehaviorTable();
    result = Enemy::Destroy(0);
    if (reason & 1)
        return ReleaseObjectStorage(object);
    return result;
}

uint32 HovrSwat::Damage(uint32 damage, const sint32 *input_vector, uint32)
{
    const uint32 guest_vector = GuestMemoryAddress(input_vector);
    const sint32 *native_vector = guest_vector ? 0 : input_vector;
    FUNCTION_MARKER(0x800593CC, "SLUS_003.73");
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 hits = r_u8(object + 383);
    sint32 health = (sint16)r_u16(object + 218);
    sint32 impulse[3];
    uint32 shift = 3, remaining, player, flags, table, receiver, target;
    w_u8(object + 383, hits + 1);
    if (health > 0)
    {
        ShiftVectorRight((uint32 *)impulse, native_vector ? (const uint32 *)native_vector : (const uint32 *)psx_addr(guest_vector, 12), &shift);
        AddVectors((uint32 *)motion.velocity, (const uint32 *)impulse);
        remaining = r_u16(object + 218) - damage;
        w_u16(object + 218, remaining);
        if ((sint16)remaining > 0)
            SetColor(3, 255, 255, 255);
        else
        {
            player = visual_lists.players;
            if (player)
                TryPlayPlayerKillQuote((const uint32 *)psx_addr(player + 4, 12));
            flags = this->lifecycle_flags;
            this->lifecycle_flags = flags & 0xFFEF;
            Fragment();
            table = r_u32(object + 68);
            receiver = object + (uint32)(sint32)(sint16)r_u16(table + 16);
            target = r_u32(table + 20);
            apocalypse_object_virtual20(target, receiver);
        }
    }
    return 1;
}

uint32 HovrSwat::Fragment()
{
    uint32 trig_angle;
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 velocity[3], index, angle, factor, child, kind, name;
    FUNCTION_MARKER(0x800594BC, "SLUS_003.73");
    HandleExplosionProximity((const uint32 *)this->model.pos, 0);
    SpawnExplosionRing((const uint32 *)this->model.pos, 200, 240, 200, 0, 5, 0, 100);
    for (index = 0; index < 6; ++index)
    {
        velocity[0] = motion.velocity[0];
        velocity[1] = motion.velocity[1];
        velocity[2] = motion.velocity[2];
        angle = RandomRange(4096);
        factor = RandomRange(8);
        trig_angle = angle & 0xFFF;
        velocity[0] += (uint32)rsin(trig_angle) * (factor + 24);
        factor = RandomRange(16);
        velocity[1] -= (factor + 32) << 12;
        factor = RandomRange(8);
        velocity[2] += (uint32)rcos(trig_angle) * (factor + 24);
        child = AllocateZeroedObject(320);
        if (child)
        {
            factor = RandomRange(r_u8(object + 569));
            kind = (r_u8(object + 568) + factor) & 0xFFFF;
            name = r_u32(0x800FF62C);
            void *dram_addr = AllocateNativeObjectStorage(child, sizeof(Debris));
            new (dram_addr) Debris((const uint32 *)this->model.pos, velocity, kind, 0, 2, 32, psx_addr(name, 1));
        }
    }
    return 0;
}

uint32 HovrSwat::InitEffects()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 position[3], index, size = 1024u, offset_y = 0u;
    uint32 red = 64u, green = 64u, blue = 128u;
    FUNCTION_MARKER(0x80059644u, "SLUS_003.73");
    position[0] = this->model.pos[0];
    position[1] = this->model.pos[1];
    position[2] = this->model.pos[2];
    w_u32(0x800FF3ACu, 0u);
    for (index = 0u; index < 8u; ++index)
    {
        uint32 effect = sub_80032DC0(112u);
        if (effect)
        {
            effect = sub_80034D88(effect);
            if (effect)
            {
                w_u8(effect + 66u, 1u);
                position[1] = this->model.pos[1] + offset_y;
                xport_draft_host_sub_80032EE4_p2(effect, position);
                sub_80033354(effect, 0x800FF4F0u);
                sub_80033290(effect, size & 0xFFFFu);
                sub_800332A4(effect);
                sub_800332FC(effect, red & 0xFFu, green & 0xFFu, blue & 0xFFu);
                red -= 8u;
                green -= 8u;
                blue -= 12u;
                w_u32(object + 536u + index * 4u, effect);
            }
        }
        size += 256u;
        offset_y += 0x18000u;
    }
    w_u32(0x800FF3ACu, 1u);
    return 1u;
}

uint32 HovrSwat::DestroyEffects()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 index, effect, table, receiver, target;
    FUNCTION_MARKER(0x8005979Cu, "SLUS_003.73");
    for (index = 0u; index < 8u; ++index)
    {
        effect = r_u32(object + 536u + index * 4u);
        if (effect)
        {
            table = r_u32(effect + 68u);
            receiver = effect + (uint32)(sint32)(sint16)r_u16(table + 8u);
            target = r_u32(table + 12u);
            apocalypse_object_cleanup(target, receiver, 3u);
            w_u32(object + 536u + index * 4u, 0u);
        }
    }
    return 0u;
}

uint32 HovrSwat::UpdateEffects()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 position[3], index;
    FUNCTION_MARKER(0x80059808u, "SLUS_003.73");
    position[0] = this->model.pos[0];
    position[1] = this->model.pos[1];
    position[2] = this->model.pos[2];
    for (index = 0u; index < 8u; ++index)
    {
        uint32 effect = r_u32(object + 536u + index * 4u);
        if (effect)
        {
            uint32 random = RandomRange(32u);
            uint32 offset = ((index << 5) + random - 16u) << 12;
            position[1] = this->model.pos[1] + offset;
            xport_draft_host_sub_80032EE4_p2(effect, position);
        }
    }
    return 0u;
}

uint32 HovrSwat::Update()
{
    SVECTOR motion_cur, motion_target;
    uint32 motion_cur_word, motion_cur_tail, motion_target_word, motion_target_tail;
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 flags, hits, timer, distance, target, position[3], current_xy, current_z, desired_xy, scale, sound, old_distance;
    uint16 angles[4];
    FUNCTION_MARKER(0x800598B8u, "SLUS_003.73");
    flags = this->activation_flags;
    w_u16(object + 388u, flags);
    if (flags & 1u)
    {
        hits = r_u8(object + 384u);
        this->activation_flags = flags & 0xFFFEu;
        w_u8(object + 384u, hits + 1u);
    }
    position[0] = r_u32(0x800A71CCu);
    position[1] = r_u32(0x800A71D0u);
    position[2] = r_u32(0x800A71D4u);
    motion.acceleration[0] = position[0];
    motion.acceleration[1] = position[1];
    motion.acceleration[2] = position[2];
    if (r_u8(object + 380u))
    {
        timer = r_u16(object + 476u);
        if (timer)
            w_u16(object + 476u, timer - 1u);
        else
            sub_8004BE90(object, r_u32(object + 400u));
    }
    else
        UpdateState();
    if (r_u32(object + 396u) & 1u)
    {
        distance = sub_8006696C(object + 4u, object + 484u);
        if (distance < 129u)
            w_u32(object + 396u, r_u32(object + 396u) & ~1u);
        else
        {
            xport_draft_host_sub_80066B8C_p1(angles, object + 4u, object + 484u);
            scale = r_u16(object + 436u);
            BuildDirectionVector(motion.acceleration, scale, (const uint16 *)angles);
        }
    }
    flags = r_u32(object + 396u);
    if (flags & 0x1Du)
    {
        if (flags & 0x10u)
        {
            target = r_u32(object + 520u);
            position[0] = r_u32(target + 4u);
            position[1] = r_u32(target + 8u);
            position[2] = r_u32(target + 12u);
            w_u32(object + 496u, position[0]);
            w_u32(object + 500u, position[1]);
            w_u32(object + 504u, position[2]);
        }
        else if (flags & 8u)
        {
            xport_draft_host_sub_8006C34C_p1(position, object + 4u, object + 508u);
            w_u32(object + 496u, position[0]);
            w_u32(object + 500u, position[1]);
            w_u32(object + 504u, position[2]);
        }
        if (flags & 0x1Cu)
        {
            position[0] = this->model.pos[0];
            position[1] = this->model.pos[1];
            position[2] = this->model.pos[2];
            xport_draft_host_sub_80066B8C_p12(angles, position, object + 496u);
            if (flags & 0x10u)
                angles[0] = 0u;
        }
        else
            xport_draft_host_sub_80066B8C_p1(angles, object + 4u, object + 484u);
        current_xy = apocalypse_angle_pair_word(object + 16u);
        current_z = (uint32)(sint32)(sint16)this->model.angles[2];
        desired_xy = (uint32)angles[0] | ((uint32)angles[1] << 16);
        scale = r_u16(object + 438u);
        motion_cur_word = current_xy;
        motion_cur_tail = current_z & 0xFFFFu;
        motion_target_word = desired_xy;
        motion_target_tail = angles[2];
        motion_cur.vx = (sint16)motion_cur_word;
        motion_cur.vy = (sint16)(motion_cur_word >> 16);
        motion_cur.vz = (sint16)motion_cur_tail;
        motion_cur.pad = (sint16)(motion_cur_tail >> 16);
        motion_target.vx = (sint16)motion_target_word;
        motion_target.vy = (sint16)(motion_target_word >> 16);
        motion_target.vz = (sint16)motion_target_tail;
        motion_target.pad = (sint16)(motion_target_tail >> 16);
        UpdateAngleAcceleration(motion_cur, motion.angular_velocity, motion.angular_acceleration, motion_target, scale);
    }
    else
    {
        motion.angular_velocity[2] = 0u;
        motion.angular_velocity[1] = 0u;
        motion.angular_velocity[0] = 0u;
        motion.angular_acceleration[2] = 0u;
        motion.angular_acceleration[1] = 0u;
        motion.angular_acceleration[0] = 0u;
    }
    this->model.angles[0] = 0u;
    sub_8007FDF0(object);
    if (r_u8(object + 382u))
        AddVectors(motion.acceleration, (const uint32 *)psx_addr(0x800A6844u, 12));
    sub_8004B9A4(object);
    if (r_u32(object + 528u))
    {
        timer = r_u16(object + 524u);
        if (timer)
            w_u16(object + 524u, timer - 1u);
        else
        {
            target = visual_lists.cameras + 4u;
            distance = sub_8006696C(object + 4u, target);
            sound = r_u32(object + 528u);
            old_distance = r_u32(object + 532u);
            w_u16(object + 524u, 8u);
            sub_80069FAC(sound, object + 4u, distance - old_distance);
            w_u32(object + 532u, distance);
        }
    }
    else if (!(r_u32(0x800FF2F0u) & 7u))
        w_u32(object + 528u, sub_80069EF4(101u, object + 4u, 0u));
    return UpdateEffects();
}

uint32 HovrSwat::ExecuteCommand(uint32 command)
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 cursor, value, table, receiver, target, position[3], shift = 12u;
    FUNCTION_MARKER(0x80059D1Cu, "SLUS_003.73");
    command &= 0xFFFFu;
    switch (command)
    {
        case 0x4220u:
        case 0x4222u:
            cursor = (r_u32(object + 400u) + 3u) & 0xFFFFFFFCu;
            position[0] = r_u32(cursor);
            position[1] = r_u32(cursor + 4u);
            position[2] = r_u32(cursor + 8u);
            ShiftVectorLeft(position, &shift);
            if (command == 0x4222u)
                AddVectors(position, (const uint32 *)this->model.pos);
            w_u32(object + 400u, cursor + 12u);
            SetDestination(position);
            return 1u;
        case 0x4221u:
            cursor = r_u32(object + 400u);
            value = r_u16(cursor);
            w_u32(object + 400u, cursor + 2u);
            if (value & 0x2000u)
            {
                table = r_u32(object + 68u);
                receiver = object + (uint32)(sint32)(sint16)r_u16(table + 80u);
                target = r_u32(table + 84u);
                value = DispatchObjectParameterGetter(target, receiver, value);
            }
            ReadRecordPosition(position, value & 0xFFFF);
            SetDestination(position);
            return 1u;
        case 0x4262u:
            cursor = r_u32(object + 400u);
            value = r_u16(cursor);
            w_u32(object + 400u, cursor + 2u);
            if (!value)
                Fire();
            return 1u;
        case 0x4406u:
            cursor = r_u32(object + 400u);
            value = r_u16(cursor);
            w_u32(object + 400u, cursor + 2u);
            if (value)
                SetTargetObject(visual_lists.players);
            else
                ClearTarget();
            return 1u;
        default:
            return Enemy::ExecuteCommand(command);
    }
}

uint32 HovrSwat::SetDestination(const uint32 *position)
{
    const uint32 object = NativeObjectGuestAddress(this);
    const uint32 guest_position = GuestMemoryAddress(position);
    uint32 values[3], flags;
    FUNCTION_MARKER(0x80059F28, "SLUS_003.73");
    if (guest_position)
    {
        values[0] = r_u32(guest_position);
        values[1] = r_u32(guest_position + 4);
        values[2] = r_u32(guest_position + 8);
    }
    else
        memcpy(values, position, sizeof(values));
    w_u32(object + 484, values[0]);
    w_u32(object + 488, values[1]);
    w_u32(object + 492, values[2]);
    flags = r_u32(object + 396) | 1;
    w_u32(object + 396, flags);
    return flags;
}

uint32 HovrSwat::SetTargetObject(uint32 target)
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 flags = r_u32(object + 396u);
    FUNCTION_MARKER(0x8005A004u, "SLUS_003.73");
    w_u32(object + 520u, target);
    flags = (flags | 0x10u) & 0xFFFFFFF3u;
    w_u32(object + 396u, flags);
    return flags;
}

uint32 HovrSwat::ClearTarget()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 flags = r_u32(object + 396u) & 0xFFFFFFE3u;
    FUNCTION_MARKER(0x8005A020u, "SLUS_003.73");
    w_u32(object + 396u, flags);
    return flags;
}

uint32 HovrSwat::Fire()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 player = visual_lists.players;
    uint32 position[3], target, projectile, damage, speed;
    uint16 rotation[3];
    FUNCTION_MARKER(0x8005A034u, "SLUS_003.73");
    if (!player)
        return player;
    SetAnimationRange(2u, 0u, 0xFFFFFFFFu);
    target = visual_lists.players + 4u;
    position[0] = this->model.pos[0];
    position[1] = this->model.pos[1];
    position[2] = this->model.pos[2];
    xport_draft_host_sub_80066B8C_p12(rotation, position, target);
    projectile = sub_80032DC0(108u);
    if (projectile)
    {
        damage = sub_8004BDFC(object, 0u, 1u) & 0xFFFFu;
        damage = sub_8004B914(object, damage);
        speed = sub_8004BDFC(object, 0u, 2u) & 0xFFFFu;
        xport_draft_host_sub_8001C158_p23(projectile, position, rotation, damage,
            45u, speed, 16u, 240u, 240u, 240u, 64u, 32u, 128u);
    }
    return sub_80069EF4(20u, object + 4u, 0u);
}

uint32 HovrSwat::UpdateState()
{
    uint32 trig_angle;
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 state, player, value, radius, orbit_size, yaw, table, position[3], vector[3], normal[3];
    sint32 saved_yaw;
    uint16 rotation[3];
    FUNCTION_MARKER(0x8005A170u, "SLUS_003.73");
    if (r_u16(object + 472u))
        return 1u;
    state = r_u16(object + 474u);
    if (state == 0u || state == 1u)
    {
        value = r_u16(object + 388u) & 1u;
        if (value)
        {
            position[0] = this->model.pos[0];
            position[1] = this->model.pos[1];
            position[2] = this->model.pos[2];
            position[1] -= 0x800000u;
            SetDestination(position);
            w_u16(object + 474u, 2u);
            return 2u;
        }
        if (!state)
        {
            player = visual_lists.players;
            if (!player)
                return 0u;
            value = this->model.pos[0];
            vector[0] = (uint32)((sint32)(value - r_u32(player + 4u)) >> 12);
            vector[1] = 0u;
            value = this->model.pos[2];
            vector[2] = (uint32)((sint32)(value - r_u32(player + 12u)) >> 12);
            NormalizeShortVector(vector, normal);
            w_u16(object + 460u, normal[0]);
            w_u16(object + 464u, 1024u);
            w_u16(object + 466u, 512u);
            w_u16(object + 468u, 0u);
            w_u16(object + 462u, normal[2]);
            if (RandomRange(64u) & 1u)
                w_u32(object + 396u, r_u32(object + 396u) | 0x20u);
            value = r_u16(object + 474u) + 1u;
            w_u16(object + 474u, value);
            return value;
        }
        if (!(r_u32(object + 396u) & 1u))
        {
            value = RandomRange(32u);
            if (!value)
            {
                trig_angle = RandomRange(4096u) & 0xFFFu;
                w_u16(object + 460u, (uint16)rsin(trig_angle));
                value = (uint16)rcos(trig_angle);
                w_u16(object + 462u, value);
                return value;
            }
            player = visual_lists.players;
            if (!player)
                return value;
            SetTargetObject(player);
            player = visual_lists.players;
            radius = r_u16(object + 464u);
            orbit_size = r_u16(object + 466u);
            yaw = r_u16(object + 468u);
            saved_yaw = (sint16)r_u16(object + 468u);
            position[0] = r_u32(player + 4u);
            position[1] = r_u32(player + 8u);
            position[2] = r_u32(player + 12u);
            position[0] += (uint32)(sint32)(sint16)r_u16(object + 460u) * radius;
            value = (uint32)(sint32)(sint16)r_u16(object + 462u) * radius;
            position[1] -= 0x100000u;
            position[2] += value;
            trig_angle = RandomRange(4096u) & 0xFFFu;
            rotation[0] = (uint16)((sint32)rsin(trig_angle) >> 6);
            rotation[1] = (uint16)yaw;
            rotation[2] = 0u;
            xport_draft_host_sub_800667CC_p13(vector, orbit_size, rotation);
            xport_draft_host_sub_8006C0B8_p12(position, vector);
            SetDestination(position);
            value = r_u32(object + 396u) & 0x20u;
            yaw = RandomRange(64u);
            yaw = value ? (uint32)saved_yaw + 256u + yaw : (uint32)saved_yaw - 256u - yaw;
            w_u16(object + 468u, yaw & 0xFFFu);
            if (!RandomRange(4u))
            {
                if (RandomRange(2u))
                {
                    if (radius < 2048u)
                        w_u16(object + 464u, radius + 32u);
                }
                else if (radius >= 513u)
                    w_u16(object + 464u, radius - 32u);
            }
            value = RandomRange(4u);
            if (value)
                return value;
            if (RandomRange(2u))
            {
                value = orbit_size + 16u;
                if (orbit_size < 1024u)
                    w_u16(object + 466u, value);
            }
            else
            {
                value = orbit_size - 16u;
                if (orbit_size >= 257u)
                    w_u16(object + 466u, value);
            }
            return value;
        }
        value = this->model.animation;
        if (value == 1u)
        {
            value = r_u8(object + 303u);
            if (!value)
                return value;
            value = (uint32)(sint32)(signed char)this->model.frame;
            if (!value)
            {
                value = sub_8004BDFC(object, 0u, 3u) & 0xFFFFu;
                value = RandomRange(value);
                if (value)
                    return value;
                return SetAnimationRange(1u, 0u, 0xFFFFFFFFu);
            }
            w_u16(object + 470u, value);
        }
        else if (value == 2u)
        {
            value = r_u8(object + 303u);
            if (!value)
                return value;
            if (RandomRange(3u))
                return SetAnimationRange(1u, (uint32)(sint32)(sint16)r_u16(object + 470u), 0u);
        }
        else
            return SetAnimationRange(1u, 0u, 0xFFFFFFFFu);
        return Fire();
    }
    if (state == 2u)
    {
        value = r_u32(object + 396u) & 1u;
        if (value)
            return value;
        table = r_u32(object + 68u);
        player = object + (uint32)(sint32)(sint16)r_u16(table + 16u);
        value = r_u32(table + 20u);
        return xport_draft_guest_call1(value, player);
    }
    return 2u;
}

HovrSwat &HovrSwat::Attach(uint32 object)
{
    return *static_cast<HovrSwat *>(FindNativeObjectStorage(object));
}

// Compatibility entry points retain the guest call contract
uint32 sub_800590DC(uint32 object, uint32 data, uint32 index)
{
    FUNCTION_MARKER(0x800590DC, "SLUS_003.73");
    void *native_instance_storage = AllocateNativeObjectStorage(object, sizeof(HovrSwat));
    new (native_instance_storage) HovrSwat((const uint8 *)psx_addr(data, 1), index);
    return object;
}

uint32 sub_800591D4(uint32 object)
{
    FUNCTION_MARKER(0x800591D4, "SLUS_003.73");
    return HovrSwat::Attach(object).Init();
}

uint32 sub_8005933C(uint32 object, uint32 reason)
{
    FUNCTION_MARKER(0x8005933C, "SLUS_003.73");
    return HovrSwat::Attach(object).HovrSwat::Destroy(reason);
}

uint32 apocalypse_hover_damage_native(uint32 object, uint32 damage, const sint32 vector[3])
{
    return HovrSwat::Attach(object).HovrSwat::Damage(damage, vector, 0);
}

uint32 sub_800593CC(uint32 object, uint32 damage, uint32 vector)
{
    FUNCTION_MARKER(0x800593CC, "SLUS_003.73");
    return HovrSwat::Attach(object).HovrSwat::Damage(damage, vector ? (const sint32 *)psx_addr(vector, 12) : 0, 0);
}

uint32 sub_800594BC(uint32 object)
{
    FUNCTION_MARKER(0x800594BC, "SLUS_003.73");
    return HovrSwat::Attach(object).Fragment();
}

uint32 sub_80059644(uint32 object)
{
    FUNCTION_MARKER(0x80059644, "SLUS_003.73");
    return HovrSwat::Attach(object).InitEffects();
}

uint32 sub_8005979C(uint32 object)
{
    FUNCTION_MARKER(0x8005979C, "SLUS_003.73");
    return HovrSwat::Attach(object).DestroyEffects();
}

uint32 sub_80059808(uint32 object)
{
    FUNCTION_MARKER(0x80059808, "SLUS_003.73");
    return HovrSwat::Attach(object).UpdateEffects();
}

uint32 sub_800598B8(uint32 object)
{
    FUNCTION_MARKER(0x800598B8, "SLUS_003.73");
    return HovrSwat::Attach(object).HovrSwat::Update();
}

uint32 sub_80059D1C(uint32 object, uint32 command)
{
    FUNCTION_MARKER(0x80059D1C, "SLUS_003.73");
    return HovrSwat::Attach(object).HovrSwat::ExecuteCommand(command);
}

uint32 apocalypse_hover_target_native(uint32 object, const void *position)
{
    return HovrSwat::Attach(object).SetDestination((const uint32 *)position);
}

uint32 sub_80059F28(uint32 object, uint32 position)
{
    FUNCTION_MARKER(0x80059F28, "SLUS_003.73");
    return HovrSwat::Attach(object).SetDestination((const uint32 *)psx_addr(position, 12));
}

uint32 sub_8005A004(uint32 object, uint32 target)
{
    FUNCTION_MARKER(0x8005A004, "SLUS_003.73");
    return HovrSwat::Attach(object).SetTargetObject(target);
}

uint32 sub_8005A020(uint32 object)
{
    FUNCTION_MARKER(0x8005A020, "SLUS_003.73");
    return HovrSwat::Attach(object).ClearTarget();
}

uint32 sub_8005A034(uint32 object)
{
    FUNCTION_MARKER(0x8005A034, "SLUS_003.73");
    return HovrSwat::Attach(object).Fire();
}

uint32 sub_8005A170(uint32 object)
{
    FUNCTION_MARKER(0x8005A170, "SLUS_003.73");
    return HovrSwat::Attach(object).UpdateState();
}

