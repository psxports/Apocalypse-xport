#include "projectile_weaving.h"
#include "visual_object.h"
#include "record_factory.h"
#include "angle_math.h"
#include "model_animation.h"
#include "game_random.h"
#include "game_sdk_bindings.h"
#include "player.h"
#include "game_panel.h"
extern "C" {
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
}
#include "chopper.h"
#include "projectile_base.h"
#include "tank.h"
#include "object_parameters.h"
#include "record_data.h"
#include "vector_arithmetic.h"
#include "native_object_storage.h"
#include "game_object_allocation.h"
#include "audio_voice.h"
#include <new>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Chopper::Chopper(const uint8 *record_data, uint32 index) : Enemy()
{
    const uint32 data = GuestMemoryAddress(record_data);
    uint32 object = NativeObjectGuestAddress(this);
    uint32 cursor;
    FUNCTION_MARKER(0x80053AB4, "SLUS_003.73");
    if (!data)
    {
        // TODO Support native record data in the original parser adapter
        GameDiagnostic("Chopper record data is outside guest memory\n");
        abort();
    }
    w_u32(object + 68, 0x800A2A10);
    cursor = sub_80062B0C(object, data);
    cursor = sub_80062B50(object, cursor);
    w_u32(object + 400, cursor);
    w_u8(object + 380, 1);
    this->record_idx = index;
    Init();
}

uint32 Chopper::Init()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 child, value;
    FUNCTION_MARKER(0x80053BA8u, "SLUS_003.73");
    w_u32(0x800FF308u, r_u32(0x800FF308u) + 1u);
    sub_800626C8(object, 0x800FF55Cu);
    AddToList(visual_lists.actors);
    this->kind = 202u;
    motion.drag[0] = 3u;
    motion.drag[1] = 3u;
    motion.drag[2] = 3u;
    sub_8004BD04(object, 4u, 8u);
    value = visual_lists.cameras;
    value = sub_8006696C(object + 4u, value + 4u);
    w_u32(object + 540u, value);
    w_u16(object + 472u, 2u);
    w_u8(object + 382u, 0u);
    w_u16(object + 474u, 0u);
    w_u8(object + 381u, 0u);
    w_u16(object + 218u, 100u);
    child = sub_8002FED8(88u);
    if (child)
        child = sub_80022B8C(child, 0x800FF5A0u);
    w_u32(object + 528u, child);
    if (child)
    {
        w_u32(child + 40u, 5u);
        child = r_u32(object + 528u);
        sub_80022C58(child, 2u, 2u, 10u, 8096u, 0u, 0u, 11u, 128u);
    }
    w_u16(object + 436u, 16u);
    w_u16(object + 438u, 48u);
    sub_8004BDCC(object, 0u, 0u, 0u);
    sub_8004BDCC(object, 0u, 1u, 0u);
    sub_8004BDCC(object, 0u, 2u, 0u);
    sub_8004BDCC(object, 1u, 0u, 0u);
    sub_8004BDCC(object, 1u, 1u, 20u);
    sub_8004BDCC(object, 1u, 2u, 30u);
    sub_8004BDCC(object, 1u, 3u, 30u);
    sub_8004BDCC(object, 2u, 0u, 0u);
    sub_8004BDCC(object, 2u, 1u, 20u);
    sub_8004BDCC(object, 2u, 2u, 30u);
    sub_8004BDCC(object, 2u, 3u, 512u);
    sub_8004BDCC(object, 2u, 4u, 8u);
    sub_8004BDCC(object, 2u, 5u, 300u);
    sub_8004BDCC(object, 2u, 6u, 256u);
    sub_8004BDCC(object, 2u, 7u, 128u);
    sub_8004BDCC(object, 3u, 0u, 0u);
    sub_8004BDCC(object, 3u, 1u, 90u);
    sub_8004BDCC(object, 3u, 2u, 60u);
    value = sub_80069EF4(84u, object + 4u, 0u);
    w_u32(object + 536u, value);
    value = RandomRange(8u) + 1u;
    w_u16(object + 534u, value);
    return value;
}

uint32 Chopper::Destroy(uint32 reason)
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 child, table, target, receiver, value;
    FUNCTION_MARKER(0x80053E6C, "SLUS_003.73");
    w_u32(object + 68, 0x800A2A10);
    RemoveFromList(visual_lists.actors);
    child = r_u32(object + 528);
    if (child)
    {
        table = r_u32(child);
        receiver = child + (uint32)(sint32)(sint16)r_u16(table + 8);
        target = r_u32(table + 12);
        apocalypse_object_cleanup(target, receiver, 3);
    }
    Enemy::FreeBehaviorTable();
    if (r_u32(object + 396) & 128)
    {
        ClearTrackedObjectHealth();
        value = r_u32(object + 396);
        w_u32(object + 396, value & 0xFFFFFF7F);
    }
    value = r_u32(object + 536);
    if (value)
        StopSoundVoices(value);
    Enemy::Destroy(0);
    return (reason & 1) ? ReleaseObjectStorage(object) : 0;
}

void Chopper::SetParameters(uint32 command)
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 cursor, value;
    FUNCTION_MARKER(0x80053F48, "SLUS_003.73");
    command &= 0xFFFF;
    if (command != 0x2130)
    {
        Enemy::ReadParameters(command);
        return;
    }
    cursor = r_u32(object + 400);
    w_u32(object + 400, cursor + 2);
    value = r_u16(cursor + 2);
    cursor += 4;
    w_u32(object + 400, cursor);
    w_u16(object + 468, value);
    value = r_u16(cursor);
    cursor += 2;
    w_u32(object + 400, cursor);
    w_u16(object + 470, value);
}

void Chopper::UpdateRoll(uint32 difference)
{
    const uint32 object = NativeObjectGuestAddress(this);
    if ((sint16)difference < -2048)
        difference += 4096u;
    if ((sint16)difference >= 2049)
        difference -= 4096u;
    if ((sint16)difference)
        motion.angular_acceleration[2] = (uint32)((sint32)(sint16)difference >> 3);
    else
        motion.angular_velocity[2] = 0u;
}

uint32 Chopper::SquaredDistance(const uint32 vector[3])
{
    uint32 axis, sum;
    for (axis = 0u; axis < 3u; ++axis)
        xport_draft_gte_data_write(9u + axis, (uint32)((sint32)vector[axis] >> 12));
    xport_draft_gte_execute(0xA00428u);
    sum = xport_draft_gte_data_read(25u);
    sum += xport_draft_gte_data_read(26u);
    sum += xport_draft_gte_data_read(27u);
    return sum;
}

void Chopper::WeaponAction(uint32 target, uint32 receiver, uint32 position, uint32 angles)
{
    if (target == 0x80022C90u)
        sub_80022C90(receiver, position, angles);
    else if (target == 0x80023380u)
        sub_80023380(receiver, position);
    else
    {
        GameDiagnostic("TODO Chopper weapon target %08X receiver %08X\n", target, receiver);
        abort();
    }
}

uint32 Chopper::Update()
{
    SVECTOR motion_cur, motion_target;
    uint32 motion_cur_word, motion_cur_tail, motion_target_word, motion_target_tail;
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 x, y, z, value, flags, target, packed, tail, scale, pointer, table, adjustment, slot, distance;
    uint32 vector[3];
    sint16 angles[3];
    sint32 difference;
    FUNCTION_MARKER(0x80053FA8u, "SLUS_003.73");
    x = r_u32(0x800A71CCu);
    y = r_u32(0x800A71D0u);
    z = r_u32(0x800A71D4u);
    motion.acceleration[0] = x;
    motion.acceleration[1] = y;
    motion.acceleration[2] = z;
    flags = this->activation_flags;
    w_u16(object + 388u, flags);
    if (flags & 1u)
    {
        value = r_u8(object + 384u);
        this->activation_flags = flags & 0xFFFEu;
        w_u8(object + 384u, value + 1u);
    }
    if (!r_u8(object + 381u))
    {
        InitializeModelAnimation(ChopperHierarchy);
        value = this->model.flags;
        w_u8(object + 381u, 1u);
        this->model.flags = value | 4u;
        SetAnimationRange(0u, 0u, 0xFFFFFFFFu);
    }
    if (r_u8(object + 380u))
    {
        value = r_u16(object + 476u);
        if (value)
            w_u16(object + 476u, value - 1u);
        else
            sub_8004BE90(object, r_u32(object + 400u));
    }
    else
        UpdateState();
    flags = r_u32(object + 396u);
    if (flags & 16u)
    {
        pointer = r_u32(object + 520u);
        value = r_u16(pointer + 58u);
        pointer += value == 50u ? 184u : 4u;
        x = r_u32(pointer);
        y = r_u32(pointer + 4u);
        z = r_u32(pointer + 8u);
        w_u32(object + 496u, x);
        w_u32(object + 500u, y);
        w_u32(object + 504u, z);
        goto TrackTarget;
    }
    if (flags & 8u)
    {
        xport_draft_host_sub_8006C34C_p1(vector, object + 4u, object + 508u);
        w_u32(object + 496u, vector[0]);
        w_u32(object + 500u, vector[1]);
        w_u32(object + 504u, vector[2]);
        goto TrackTarget;
    }
    if (flags & 4u)
    {
TrackTarget:
        Aim();
        if (!(r_u32(object + 396u) & 1u))
            goto LevelRoll;
        xport_draft_host_sub_80066B8C_p1(angles, object + 4u, object + 484u);
        goto MoveTarget;
    }
    if (!(flags & 1u))
        goto LevelRoll;
    xport_draft_host_sub_80066B8C_p1(angles, object + 4u, object + 484u);
    xport_draft_host_sub_8006C3AC_p1(vector, object + 484u, object + 4u);
    distance = xport_draft_host_sub_8006BF04_p1(vector);
    value = r_u16(object + 438u);
    scale = TraceDivide(value * 170u, distance);
    if ((sint32)scale < -256)
        scale = (uint32)-256;
    else if ((sint32)scale >= 257)
        scale = 256u;
    else if (scale - 1u < 3u)
        scale = 4u;
    else if ((sint32)scale < 0 && (sint32)scale >= -3)
        scale = (uint32)-4;
    angles[0] = 0;
    packed = apocalypse_angle_pair_word(object + 16u);
    tail = this->model.angles[2];
    value = (uint16)angles[0] | ((uint32)(uint16)angles[1] << 16);
    motion_cur_word = packed;
    motion_cur_tail = tail;
    motion_target_word = value;
    motion_target_tail = (uint16)angles[2];
    motion_cur.vx = (sint16)motion_cur_word;
    motion_cur.vy = (sint16)(motion_cur_word >> 16);
    motion_cur.vz = (sint16)motion_cur_tail;
    motion_cur.pad = (sint16)(motion_cur_tail >> 16);
    motion_target.vx = (sint16)motion_target_word;
    motion_target.vy = (sint16)(motion_target_word >> 16);
    motion_target.vz = (sint16)motion_target_tail;
    motion_target.pad = (sint16)(motion_target_tail >> 16);
    UpdateAngleAcceleration(motion_cur, motion.angular_velocity, motion.angular_acceleration, motion_target, scale);
    this->model.angles[0] = 0u;
MoveTarget:
    if (!(r_u32(object + 396u) & 1u))
        goto LevelRoll;
    xport_draft_host_sub_8006C3AC_p1(vector, object + 4u, object + 484u);
    if ((sint32)SquaredDistance(vector) <= 65536)
    {
        flags = r_u32(object + 396u);
        w_u32(object + 396u, flags & 0xFFFFFFFEu);
        goto UpdateModel;
    }
    xport_draft_host_sub_80066B8C_p1(angles, object + 4u, object + 484u);
    value = r_u16(object + 436u);
    BuildDirectionVector(motion.acceleration, value, (const uint16 *)angles);
    flags = r_u32(object + 396u);
    if (flags & 32u)
        value = ((uint16)angles[1] - this->model.angles[1]) & 4095u;
    else
        value = 0u;
    if ((sint16)value >= 2049)
        value -= 4096u;
    difference = (sint16)value;
    if (difference > 1024)
        difference = 1024;
    else if (difference < -1024)
        difference = -1024;
    value = (uint32)((sint32)(0u - (uint32)difference) >> 1);
    tail = this->model.angles[2];
    value = (value & 4095u) - tail;
    UpdateRoll(value);
    goto UpdateModel;
LevelRoll:
    value = 0u - this->model.angles[2];
    UpdateRoll(value);
UpdateModel:
    if (r_u8(object + 382u))
        AddVectors(motion.acceleration, (const uint32 *)psx_addr(0x800A6844u, 12));
    sub_8004B9A4(object);
    value = GetModelJointAngles(1).rotation[1];
    GetModelJointAngles(1).rotation[1] = (uint16)((value + 512) & 4095);
    value = GetModelJointAngles(2).rotation[1];
    GetModelJointAngles(2).rotation[1] = (uint16)((value + 512) & 4095);
    value = GetModelJointAngles(3).rotation[1];
    GetModelJointAngles(3).rotation[1] = (uint16)((value + 512) & 4095);
    sub_8007FDF0(object);
    BuildModelPose(*this);
    if (sub_8004BDFC(object, 0u, 0u) & 65535u)
        SetFireDuration(1u);
    if (sub_8004BDFC(object, 1u, 0u) & 65535u)
    {
        value = sub_8004BDFC(object, 1u, 3u) & 65535u;
        if (!RandomRange(value))
            FireProjectile();
    }
    if (sub_8004BDFC(object, 2u, 0u) & 65535u)
    {
        value = sub_8004BDFC(object, 2u, 2u) & 65535u;
        if (!RandomRange(value))
            SpawnDebris();
    }
    if (sub_8004BDFC(object, 3u, 0u) & 65535u)
    {
        value = sub_8004BDFC(object, 3u, 2u) & 65535u;
        if (!RandomRange(value))
            SpawnEnemy(0xFFFFFFFFu);
    }
    value = r_u32(object + 524u);
    pointer = r_u32(object + 528u);
    table = r_u32(pointer);
    slot = value ? 16u : 24u;
    adjustment = (uint32)(sint32)(sint16)r_u16(table + slot);
    target = r_u32(table + slot + 4u);
    WeaponAction(target, pointer + adjustment, object + 4u, object + 16u);
    if (value)
    {
        value = r_u32(object + 524u);
        w_u32(object + 524u, value - 1u);
    }
    if (r_u32(object + 396u) & 64u)
    {
        value = r_u16(object + 532u);
        tail = r_u16(object + 532u);
        x = (uint32)rsin(value & 4095u);
        y = this->model.pos[1];
        w_u16(object + 532u, (tail + 64u) & 4095u);
        this->model.pos[1] = y + (x << 4);
    }
    if (r_u32(object + 536u))
    {
        value = r_u16(object + 534u);
        if (value)
            w_u16(object + 534u, value - 1u);
        else
        {
            pointer = visual_lists.cameras;
            distance = sub_8006696C(object + 4u, pointer + 4u);
            pointer = r_u32(object + 536u);
            value = r_u32(object + 540u);
            w_u16(object + 534u, 8u);
            sub_80069FAC(pointer, object + 4u, distance - value);
            w_u32(object + 540u, distance);
        }
    }
    else if (!(r_u32(0x800FF2F0u) & 7u))
    {
        value = sub_80069EF4(84u, object + 4u, 0u);
        w_u32(object + 536u, value);
    }
    // The legacy behavior caller discards the original return carrier
    return 0u;
}

uint32 Chopper::Damage(uint32 damage, const sint32 *, uint32)
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 count, remaining, hero, flags, state;
    sint32 health;
    FUNCTION_MARKER(0x80054714, "SLUS_003.73");
    count = r_u8(object + 383);
    w_u8(object + 383, count + 1);
    health = (sint16)r_u16(object + 218);
    remaining = r_u16(object + 218) - damage;
    if (health <= 0)
        return 1;
    w_u16(object + 218, remaining);
    if ((sint32)(remaining << 16) > 0)
        SetColor(3, 255, 255, 255);
    else
    {
        hero = visual_lists.players;
        if (hero)
            TryPlayPlayerKillQuote((const uint32 *)psx_addr(hero + 4, 12));
        flags = this->lifecycle_flags;
        state = r_u16(object + 472);
        this->lifecycle_flags = flags & 0xFFEF;
        if (state)
        {
            w_u16(object + 472, 2);
            w_u16(object + 474, 0);
        }
        else
            w_u16(object + 474, 5);
        w_u8(object + 380, 0);
    }
    return 1;
}

void Chopper::FinishFromTable()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 table = r_u32(object + 68u);
    uint32 adjustment = (uint32)(sint32)(sint16)r_u16(table + 16u);
    uint32 target = r_u32(table + 20u);
    apocalypse_object_virtual20(target, object + adjustment);
}

void Chopper::SpawnRandomEffect(uint32 kind, uint32 debris)
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 position[3], x, y, z, shift = 12u;
    x = RandomRange(256u);
    y = RandomRange(256u);
    z = RandomRange(256u);
    position[0] = x - 128u;
    position[1] = y - 128u;
    position[2] = z - 128u;
    xport_draft_host_sub_8006C22C_p12(position, &shift);
    xport_draft_host_sub_8006C0B8_p1(position, object + 4u);
    apocalypse_explosion_proximity_native(position, kind);
    if (debris)
        xport_draft_host_sub_8001D320_p1(position, 400u, 240u, 160u, 40u, 5u, 0u, 100u);
}

void Chopper::SetDestinationRelative(uint32 field)
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 position[3], hero, offset;
    position[0] = (uint32)(sint32)(sint16)r_u16(object + field) << 16;
    hero = visual_lists.players;
    if (hero)
    {
        offset = (uint32)(sint32)(sint16)r_u16(object + 468u) << 12;
        position[1] = r_u32(hero + 8u) - offset;
        offset = (uint32)(sint32)(sint16)r_u16(object + 466u) << 12;
        position[2] = r_u32(hero + 12u) - offset;
    }
    else
    {
        position[1] = r_u32(object + 8u);
        position[2] = r_u32(object + 12u);
    }
    SetDestination(position);
    field = r_u16(object + 474u);
    w_u16(object + 474u, field + 1u);
}

void Chopper::TrackHero()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 hero = visual_lists.players, y, z, current;
    if (!hero)
        return;
    y = r_u32(hero + 8u);
    current = r_u32(object + 8u);
    z = r_u32(hero + 12u);
    y -= current;
    current = r_u32(object + 12u);
    z -= current;
    SetRelativeTarget(0u, (uint32)((sint32)y >> 12), (uint32)((sint32)z >> 12));
}

void Chopper::UpdateState()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 state, step, flags, value, record, hero, index, best, delta, minimum;
    uint32 position[3], second[3], third[3], distances[3], shift, ground, x, y, z;
    sint32 timer;
    FUNCTION_MARKER(0x800547D0u, "SLUS_003.73");
    state = r_u16(object + 472u);
    if (state > 4u)
        return;
    if (state == 1u && (r_u16(object + 388u) & 1u))
    {
        position[0] = this->model.pos[0];
        position[1] = this->model.pos[1];
        position[2] = this->model.pos[2];
        position[1] -= 0x400000u;
        SetDestination(position);
        w_u16(object + 474u, 6u);
    }
    step = r_u16(object + 474u);
    if (state == 0u || state == 3u)
    {
        if (step == 0u)
        {
            TrackObjectHealth(object, 64, 32, 4096);
            if (state == 0u)
            {
                flags = r_u32(object + 396u);
                hero = visual_lists.players;
            }
            else
            {
                hero = visual_lists.players;
                flags = r_u32(object + 396u);
                w_u16(object + 462u, 0u);
            }
            w_u32(object + 396u, flags | 128u);
            SetTargetObject(hero);
            record = sub_80066088(this->record_idx);
            value = r_u16(record + 2u);
            w_u16(object + 460u, value);
            SetDestinationRecord(value);
            step = r_u16(object + 474u);
            w_u16(object + 466u, 4096u);
            w_u16(object + 474u, step + 1u);
            return;
        }
        if (step == 1u)
        {
            if (r_u32(object + 396u) & 1u)
                return;
            if (state == 3u)
            {
                value = (uint32)(sint32)(sint16)r_u16(object + 462u);
                w_u16(object + 462u, value + 1u);
            }
            record = sub_80066088((uint32)(sint32)(sint16)r_u16(object + 460u));
            value = r_u16(record + 2u);
            w_u16(object + 460u, value);
            if (state == 0u)
                SetDestinationRecord(value);
            else if ((sint16)r_u16(object + 462u) < 3)
            {
                SetDestinationRecord(value);
                return;
            }
            else
            {
                xport_draft_host_sub_8006613C_p1(position, (uint32)(sint32)(sint16)value);
                w_u16(object + 464u, position[0] >> 16);
                ClearTarget();
                SetTargetRecord(r_u16(object + 460u));
            }
            value = r_u16(object + 474u);
            w_u16(object + 474u, value + 1u);
            return;
        }
        if (state == 3u)
        {
            if (step == 2u)
            {
                if (!(r_u32(object + 396u) & 1u))
                {
                    position[0] = this->model.pos[0];
                    position[1] = this->model.pos[1];
                    position[2] = this->model.pos[2];
                    value = r_u16(object + 442u);
                    x = (uint32)(sint32)(sint16)r_u16(object + 464u) << 16;
                    y = RandomRange(value);
                    value = r_u16(object + 442u);
                    position[0] = x - (((value >> 1) - y) << 12);
                    position[2] -= 0x400000u;
                    SetDestination(position);
                }
                xport_draft_host_sub_8006613C_p1(position, (uint32)(sint32)(sint16)r_u16(object + 460u));
                value = xport_draft_host_sub_8006689C_p2(object + 4u, position);
                if (value >= r_u16(object + 440u))
                    return;
                FireProjectile();
                ::SpawnRecord((uint32)(sint32)(sint16)r_u16(object + 460u));
                record = sub_80066088((uint32)(sint32)(sint16)r_u16(object + 460u));
                if (!r_u16(record))
                {
                    value = r_u16(object + 474u);
                    w_u16(object + 474u, value + 1u);
                }
                else
                {
                    value = r_u16(record + 2u);
                    w_u16(object + 460u, value);
                    SetTargetRecord(value);
                }
            }
            else if (step == 3u)
            {
                ClearTarget();
                SetTargetObject(visual_lists.players);
                value = (uint16)(this->model.pos[0] >> 16);
                w_u16(object + 460u, value);
                sub_8004BDCC(object, 0u, 0u, 1u);
                sub_8004BDCC(object, 1u, 0u, 1u);
                value = r_u16(object + 474u);
                w_u16(object + 436u, 4u);
                w_u16(object + 474u, value + 1u);
            }
            else if (step == 4u && !(r_u32(object + 396u) & 1u))
            {
                position[1] = this->model.pos[1];
                x = (uint32)(sint32)(sint16)r_u16(object + 460u) << 16;
                value = RandomRange(4096u);
                position[0] = x + ((value - 2048u) << 12);
                position[2] = this->model.pos[2];
                SetDestination(position);
            }
            return;
        }
        switch (step)
        {
            case 2u:
                if (r_u32(object + 396u) & 1u)
                {
                    if ((sint32)r_u32(0x800FF4DCu) < 4)
                    {
                        value = sub_8004BDFC(object, 3u, 2u) & 65535u;
                        if (!RandomRange(value))
                        {
                            value = SpawnEnemy(0xFFFFFFFFu);
                            if (value)
                                sub_80051740(value, 0u);
                        }
                    }
                    return;
                }
                sub_8004BDCC(object, 1u, 0u, 1u);
                sub_8004BDCC(object, 2u, 0u, 1u);
                xport_draft_host_sub_8006613C_p1(position, (uint32)(sint32)(sint16)r_u16(object + 460u));
                record = sub_80066088((uint32)(sint32)(sint16)r_u16(object + 460u));
                value = r_u16(record + 2u);
                w_u16(object + 462u, value);
                xport_draft_host_sub_8006613C_p1(second, (uint32)(sint32)(sint16)value);
                value = r_u16(record + 4u);
                w_u16(object + 464u, value);
                xport_draft_host_sub_8006613C_p1(third, (uint32)(sint32)(sint16)value);
                if (!RandomRange(3u))
                {
                    value = RandomRange(4096u);
                    w_u16(object + 466u, value + 2048u);
                }
                hero = visual_lists.players;
                if (hero)
                {
                    distances[0] = apocalypse_position_distance_native(position, psx_addr(hero + 4u, 12u));
                    hero = visual_lists.players;
                    distances[1] = apocalypse_position_distance_native(second, psx_addr(hero + 4u, 12u));
                    hero = visual_lists.players;
                    distances[2] = apocalypse_position_distance_native(third, psx_addr(hero + 4u, 12u));
                    minimum = 0xFFFFFFu;
                    best = 0xFFFFFFFFu;
                    for (index = 0u; index < 3u; ++index)
                    {
                        value = (uint32)(sint32)(sint16)r_u16(object + 466u);
                        delta = distances[index] - value;
                        value = (uint32)((sint32)delta >> 31);
                        delta = (delta ^ value) - value;
                        if ((sint32)delta < (sint32)minimum)
                        {
                            minimum = delta;
                            best = index;
                        }
                    }
                    if (best + 1u >= 2u)
                    {
                        index = object + 2u * (best & 255u);
                        value = r_u32(object + 396u);
                        w_u32(object + 396u, value | 32u);
                        SetDestinationRecord(r_u16(index + 460u));
                        value = r_u16(index + 460u);
                        w_u16(object + 460u, value);
                        return;
                    }
                }
                value = RandomRange(16u);
                w_u16(object + 462u, value + 16u);
                w_u16(object + 474u, 3u);
                return;
            case 3u:
                Hover();
                value = r_u16(object + 474u);
                w_u16(object + 474u, value + 1u);
                return;
            case 4u:
                timer = (sint16)r_u16(object + 462u);
                if (timer)
                {
                    sub_8004BDCC(object, 2u, 0u, 0u);
                    w_u16(object + 462u, (uint32)timer - 1u);
                }
                else
                    w_u16(object + 474u, 2u);
                return;
            case 5u:
                sub_8004BDCC(object, 1u, 0u, 0u);
                sub_8004BDCC(object, 2u, 0u, 0u);
                ::SpawnRecord((uint32)(sint32)(sint16)r_u16(object + 470u));
                ClearTarget();
                value = r_u32(object + 396u);
                index = r_u16(object + 468u);
                w_u32(object + 396u, value | 32u);
                SetDestinationRecord(index);
                value = r_u16(object + 474u);
                w_u16(object + 474u, value + 1u);
                return;
            case 6u:
                if (!(r_u32(object + 396u) & 1u))
                    FinishFromTable();
                else
                    SpawnRandomEffect(0u, 0u);
                return;
            default:
                return;
        }
    }
    if (state == 1u)
    {
        if (step == 0u)
        {
            record = sub_80066088(this->record_idx);
            xport_draft_host_sub_8006613C_p1(position, r_u16(record + 2u));
            xport_draft_host_sub_8006613C_p1(second, r_u16(record + 4u));
            w_u16(object + 468u, 640u);
            x = position[0];
            y = second[0];
            if ((sint32)x < (sint32)y)
            {
                w_u16(object + 460u, x >> 16);
                z = x + (uint32)((sint32)(y - x) >> 1);
                w_u16(object + 462u, z >> 16);
                w_u16(object + 464u, y >> 16);
            }
            else
            {
                w_u16(object + 460u, y >> 16);
                z = y + (uint32)((sint32)(x - y) >> 1);
                w_u16(object + 462u, z >> 16);
                w_u16(object + 464u, x >> 16);
            }
            position[0] = this->model.pos[0];
            position[1] = this->model.pos[1] - 7372800u;
            position[2] = this->model.pos[2];
            SetDestination(position);
            value = r_u16(object + 474u);
            w_u16(object + 474u, value + 1u);
        }
        else if (step >= 1u && step <= 4u)
        {
            flags = r_u32(object + 396u);
            if (flags & 1u)
                TrackHero();
            else
            {
                if (step == 1u)
                {
                    w_u32(object + 396u, flags | 32u);
                    sub_8004BDCC(object, 0u, 0u, 1u);
                }
                index = step == 1u ? 464u : step == 3u ? 460u : 462u;
                SetDestinationRelative(index);
            }
        }
        else if (step == 5u)
        {
            if (r_u32(object + 396u) & 1u)
                TrackHero();
            else
                w_u16(object + 474u, 1u);
        }
        else if (step == 6u && !(r_u32(object + 396u) & 1u))
            FinishFromTable();
        return;
    }
    if (state == 2u)
    {
        if (step == 0u)
        {
            motion.velocity[2] = 0u;
            motion.velocity[1] = 0u;
            motion.velocity[0] = 0u;
            motion.acceleration[2] = 0u;
            motion.acceleration[1] = 0u;
            motion.angular_velocity[0] = 64u;
            motion.acceleration[0] = 0u;
            motion.angular_velocity[1] = 0u;
            motion.angular_velocity[2] = 0u;
            flags = r_u32(object + 396u);
            w_u32(object + 396u, flags & 0xFFFFFFE2u);
            ground = sub_80067A18(object + 4u, 0u, 4096u);
            if (ground == 0xFFFFFFFFu)
                w_u16(object + 474u, 2u);
            else
            {
                w_u16(object + 460u, (uint32)((sint32)ground >> 13));
                w_u8(object + 382u, 1u);
                w_u16(object + 474u, 1u);
            }
        }
        else if (step == 1u)
        {
            ground = (uint32)(sint32)(sint16)r_u16(object + 460u) << 13;
            y = this->model.pos[1];
            if ((sint32)ground < (sint32)y)
            {
                this->model.pos[1] = ground;
                w_u8(object + 382u, 0u);
                w_u16(object + 474u, 2u);
            }
            else if (!r_u32(0x800FF738u))
                SpawnRandomEffect(0u, 0u);
        }
        else if (step == 2u)
        {
            for (index = 0u; index < 3u; ++index)
                if (!r_u32(0x800FF738u))
                    SpawnRandomEffect(1u, 1u);
            FinishFromTable();
        }
        return;
    }
    if (step)
        return;
    motion.velocity[2] = 0u;
    motion.velocity[1] = 0u;
    motion.velocity[0] = 0u;
    motion.acceleration[2] = 0u;
    motion.acceleration[1] = 0u;
    flags = r_u32(object + 396u);
    motion.acceleration[0] = 0u;
    w_u32(object + 396u, flags & 0xFFFFFFE2u);
    for (index = 0u; index < 3u; ++index)
        SpawnRandomEffect(1u, 1u);
    FinishFromTable();
}

uint32 Chopper::ResolveCommandValue(uint32 value)
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 table, adjustment, target;
    if (!(value & 0x2000u))
        return value;
    table = r_u32(object + 68u);
    adjustment = (uint32)(sint32)(sint16)r_u16(table + 80u);
    target = r_u32(table + 84u);
    return DispatchObjectParameterGetter(target, object + adjustment, value);
}

uint32 Chopper::ExecuteCommand(uint32 command)
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 cursor, value, position[3], shift = 12u;
    FUNCTION_MARKER(0x8005568Cu, "SLUS_003.73");
    command &= 0xFFFFu;
    switch (command)
    {
        case 0x4220u:
        case 0x4222u:
            cursor = (r_u32(object + 400u) + 3u) & ~3u;
            position[0] = r_u32(cursor);
            position[1] = r_u32(cursor + 4u);
            position[2] = r_u32(cursor + 8u);
            cursor += 12u;
            ShiftVectorLeft(position, &shift);
            if (command == 0x4222u)
                AddVectors(position, (const uint32 *)this->model.pos);
            w_u32(object + 400u, cursor);
            SetDestination(position);
            return 1u;
        case 0x4221u:
            cursor = r_u32(object + 400u);
            value = r_u16(cursor);
            w_u32(object + 400u, cursor + 2u);
            value = ResolveCommandValue(value);
            ReadRecordPosition(position, value & 0xFFFF);
            SetDestination(position);
            return 1u;
        case 0x4262u:
            cursor = r_u32(object + 400u);
            value = r_u16(cursor);
            w_u32(object + 400u, cursor + 2u);
            if (value == 0u)
                SetFireDuration(1u);
            else if (value == 1u)
                FireProjectile();
            else if (value == 2u)
                SpawnDebris();
            return 1u;
        case 0x4402u:
        case 0x4403u:
            cursor = r_u32(object + 400u);
            value = (uint32)(sint32)(sint16)r_u16(cursor);
            if (command == 0x4402u)
                SpawnTank(object, value);
            else
                SpawnEnemy(value);
            cursor = r_u32(object + 400u);
            w_u32(object + 400u, cursor + 2u);
            return 1u;
        case 0x4404u:
            value = r_u32(object + 396u);
            w_u32(object + 396u, value | 32u);
            return 1u;
        case 0x4405u:
            value = r_u32(object + 396u);
            w_u32(object + 396u, value & 0xFFFFFFDFu);
            return 1u;
        case 0x4406u:
        case 0x4407u:
            cursor = r_u32(object + 400u);
            value = r_u16(cursor);
            w_u32(object + 400u, cursor + 2u);
            if (!value)
                ClearTarget();
            else if (command == 0x4406u)
                SetTargetObject(visual_lists.players);
            else
            {
                value = ResolveCommandValue(value);
                SetTargetRecord(value & 0xFFFFu);
            }
            return 1u;
        case 0x4408u:
            Hover();
            return 1u;
        default:
            return Enemy::ExecuteCommand(command);
    }
}

uint32 Chopper::SetDestination(const uint32 *position)
{
    const uint32 object = NativeObjectGuestAddress(this);
    const uint32 guest_position = GuestMemoryAddress(position);
    uint32 x, y, z, flags;
    FUNCTION_MARKER(0x80055A1C, "SLUS_003.73");
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
    flags = r_u32(object + 396) | 1;
    w_u32(object + 396, flags);
    return flags;
}

uint32 Chopper::SetDestinationRecord(uint32 index)
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 flags;
    FUNCTION_MARKER(0x80055A48u, "SLUS_003.73");
    sub_8006613C(object + 484u, index & 0xFFFFu);
    flags = (r_u32(object + 396u) | 1u) & 0xFFFFFFBFu;
    w_u32(object + 396u, flags);
    return flags;
}

uint32 Chopper::Hover()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 flags;
    FUNCTION_MARKER(0x80055A88u, "SLUS_003.73");
    motion.velocity[2] = 0u;
    motion.velocity[1] = 0u;
    motion.velocity[0] = 0u;
    motion.acceleration[2] = 0u;
    motion.acceleration[1] = 0u;
    flags = r_u32(object + 396u);
    motion.acceleration[0] = 0u;
    flags = (flags & 0xFFFFFFDFu) | 64u;
    w_u32(object + 396u, flags);
    return flags;
}

uint32 Chopper::Aim()
{
    SVECTOR motion_cur, motion_target;
    uint32 motion_cur_word, motion_cur_tail, motion_target_word, motion_target_tail;
    const uint32 object = NativeObjectGuestAddress(this);
    sint16 desired[3];
    uint32 packed, tail, speed, target;
    FUNCTION_MARKER(0x80055AC0u, "SLUS_003.73");
    xport_draft_host_sub_80066B8C_p1(desired, object + 4u, object + 496u);
    packed = apocalypse_angle_pair_word(object + 16u);
    tail = this->model.angles[2];
    speed = r_u16(object + 438u);
    target = (uint16)desired[0] | ((uint32)(uint16)desired[1] << 16);
    motion_cur_word = packed;
    motion_cur_tail = tail;
    motion_target_word = target;
    motion_target_tail = (uint16)desired[2];
    motion_cur.vx = (sint16)motion_cur_word;
    motion_cur.vy = (sint16)(motion_cur_word >> 16);
    motion_cur.vz = (sint16)motion_cur_tail;
    motion_cur.pad = (sint16)(motion_cur_tail >> 16);
    motion_target.vx = (sint16)motion_target_word;
    motion_target.vy = (sint16)(motion_target_word >> 16);
    motion_target.vz = (sint16)motion_target_tail;
    motion_target.pad = (sint16)(motion_target_tail >> 16);
    return UpdateAngleAcceleration(motion_cur, motion.angular_velocity, motion.angular_acceleration, motion_target, speed);
}

uint32 Chopper::SetTargetRecord(uint32 index)
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 flags;
    FUNCTION_MARKER(0x80055B48u, "SLUS_003.73");
    sub_8006613C(object + 496u, index & 0xFFFFu);
    flags = r_u32(object + 396u) | 4u;
    w_u32(object + 396u, flags);
    return flags;
}

uint32 Chopper::SetRelativeTarget(uint32 x, uint32 y, uint32 z)
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 flags;
    FUNCTION_MARKER(0x80055B84u, "SLUS_003.73");
    w_u32(object + 508u, x << 12);
    w_u32(object + 512u, y << 12);
    w_u32(object + 516u, z << 12);
    flags = r_u32(object + 396u) | 8u;
    w_u32(object + 396u, flags);
    return flags;
}

uint32 Chopper::SetTargetObject(uint32 target)
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 result;
    FUNCTION_MARKER(0x80055BB4u, "SLUS_003.73");
    result = sub_80062F48(target);
    if (result)
        return result;
    result = r_u32(object + 396u);
    w_u32(object + 520u, target);
    result |= 16u;
    w_u32(object + 396u, result);
    return result;
}

uint32 Chopper::ClearTarget()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 result = r_u32(object + 396u) & 0xFFFFFFE3u;
    FUNCTION_MARKER(0x80055C00u, "SLUS_003.73");
    w_u32(object + 396u, result);
    return result;
}

uint32 Chopper::FireProjectile()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 position[3], child, value;
    uint16 angles[3];
    FUNCTION_MARKER(0x80055C14u, "SLUS_003.73");
    position[0] = this->model.pos[0];
    position[1] = this->model.pos[1];
    position[2] = this->model.pos[2];
    angles[0] = (uint16)this->model.angles[0];
    angles[1] = (uint16)this->model.angles[1];
    angles[2] = 0u;
    child = sub_800625AC(440u);
    if (child)
    {
        void *storage = AllocateNativeObjectStorage(child, sizeof(WeavingProjectile));
        new (storage) WeavingProjectile(position, (const sint16 *)angles, 0, 6, 128, &visual_lists.players, (const uint32 *)psx_addr(0x800A71CC, 12));
    }
    value = sub_8004BDFC(object, 1u, 1u);
    value = sub_8004B914(object, value & 65535u);
    ProjectileBase::Attach(child).SetDamage(value);
    ProjectileBase::Attach(child).SetBlastRadius(256);
    return 256u;
}

uint32 Chopper::SetFireDuration(uint32 seconds)
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 result = seconds ? seconds * 30u : 0xFFFFFFFFu;
    FUNCTION_MARKER(0x80055CECu, "SLUS_003.73");
    w_u32(object + 524u, result);
    return result;
}

uint32 Chopper::SpawnDebris()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 child, first, second, value312, value314, value316, value318;
    FUNCTION_MARKER(0x80055D10u, "SLUS_003.73");
    child = sub_800625AC(328u);
    if (!child)
        return 0u;
    first = sub_8004BDFC(object, 2u, 1u) & 65535u;
    second = sub_8004BDFC(object, 2u, 3u) & 65535u;
    value312 = sub_8004BDFC(object, 2u, 6u) & 65535u;
    value314 = sub_8004BDFC(object, 2u, 4u) & 65535u;
    value316 = sub_8004BDFC(object, 2u, 5u) & 65535u;
    value318 = sub_8004BDFC(object, 2u, 7u) & 65535u;
    return sub_8005B5D8(child, object + 4u, first, second, value312, value314, value316, value318);
}

uint32 Chopper::SpawnEnemy(uint32 index)
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 child, script, flags;
    FUNCTION_MARKER(0x80055E0Cu, "SLUS_003.73");
    child = sub_800625AC(624u);
    if (child)
    {
        script = (sint32)index < 0 ? 0u : r_u32(object + index * 4u + 404u);
        child = sub_8004E3E4(child, object + 4u, script);
    }
    /* Preserve the original physical RAM write when allocation returns zero */
    flags = r_u32(child + 396u) | 512u;
    w_u32(child + 396u, flags);
    return child;
}

uint32 Chopper::TraceDivide(uint32 numerator, uint32 denominator)
{
    if (!denominator)
        return (sint32)numerator < 0 ? 1u : 0xFFFFFFFFu;
    if (numerator == 0x80000000u && denominator == 0xFFFFFFFFu)
        return numerator;
    return (uint32)((sint32)numerator / (sint32)denominator);
}

Chopper &Chopper::Attach(uint32 object)
{
    return *static_cast<Chopper *>(FindNativeObjectStorage(object));
}

// Compatibility entry points retain the guest call contract
uint32 sub_80053AB4(uint32 object, uint32 data, uint32 index)
{
    FUNCTION_MARKER(0x80053AB4, "SLUS_003.73");
    void *native_instance_storage = AllocateNativeObjectStorage(object, sizeof(Chopper));
    new (native_instance_storage) Chopper((const uint8 *)psx_addr(data, 1), index);
    return object;
}

uint32 sub_80053BA8(uint32 object)
{
    FUNCTION_MARKER(0x80053BA8, "SLUS_003.73");
    return Chopper::Attach(object).Init();
}

uint32 sub_80053E6C(uint32 object, uint32 reason)
{
    FUNCTION_MARKER(0x80053E6C, "SLUS_003.73");
    return Chopper::Attach(object).Chopper::Destroy(reason);
}

void sub_80053F48(uint32 object, uint32 command)
{
    FUNCTION_MARKER(0x80053F48, "SLUS_003.73");
    Chopper::Attach(object).Chopper::SetParameters(command);
}

void sub_80053FA8(uint32 object)
{
    FUNCTION_MARKER(0x80053FA8, "SLUS_003.73");
    Chopper::Attach(object).Chopper::Update();
}

uint32 sub_80054714(uint32 object, uint32 damage)
{
    FUNCTION_MARKER(0x80054714, "SLUS_003.73");
    return Chopper::Attach(object).Chopper::Damage(damage, 0, 0);
}

void sub_800547D0(uint32 object)
{
    FUNCTION_MARKER(0x800547D0, "SLUS_003.73");
    Chopper::Attach(object).UpdateState();
}

uint32 sub_8005568C(uint32 object, uint32 command)
{
    FUNCTION_MARKER(0x8005568C, "SLUS_003.73");
    return Chopper::Attach(object).Chopper::ExecuteCommand(command);
}

uint32 apocalypse_chopper_target_native(uint32 object, const uint32 position[3])
{
    return Chopper::Attach(object).SetDestination(position);
}

uint32 sub_80055A1C(uint32 object, uint32 position)
{
    FUNCTION_MARKER(0x80055A1C, "SLUS_003.73");
    return Chopper::Attach(object).SetDestination((const uint32 *)psx_addr(position, 12));
}

uint32 sub_80055A48(uint32 object, uint32 index)
{
    FUNCTION_MARKER(0x80055A48, "SLUS_003.73");
    return Chopper::Attach(object).SetDestinationRecord(index);
}

uint32 sub_80055A88(uint32 object)
{
    FUNCTION_MARKER(0x80055A88, "SLUS_003.73");
    return Chopper::Attach(object).Hover();
}

uint32 sub_80055AC0(uint32 object)
{
    FUNCTION_MARKER(0x80055AC0, "SLUS_003.73");
    return Chopper::Attach(object).Aim();
}

uint32 sub_80055B48(uint32 object, uint32 index)
{
    FUNCTION_MARKER(0x80055B48, "SLUS_003.73");
    return Chopper::Attach(object).SetTargetRecord(index);
}

uint32 sub_80055B84(uint32 object, uint32 x, uint32 y, uint32 z)
{
    FUNCTION_MARKER(0x80055B84, "SLUS_003.73");
    return Chopper::Attach(object).SetRelativeTarget(x, y, z);
}

uint32 sub_80055BB4(uint32 object, uint32 target)
{
    FUNCTION_MARKER(0x80055BB4, "SLUS_003.73");
    return Chopper::Attach(object).SetTargetObject(target);
}

uint32 sub_80055C00(uint32 object)
{
    FUNCTION_MARKER(0x80055C00, "SLUS_003.73");
    return Chopper::Attach(object).ClearTarget();
}

uint32 sub_80055C14(uint32 object)
{
    FUNCTION_MARKER(0x80055C14, "SLUS_003.73");
    return Chopper::Attach(object).FireProjectile();
}

uint32 sub_80055CEC(uint32 object, uint32 seconds)
{
    FUNCTION_MARKER(0x80055CEC, "SLUS_003.73");
    return Chopper::Attach(object).SetFireDuration(seconds);
}

uint32 sub_80055D10(uint32 object)
{
    FUNCTION_MARKER(0x80055D10, "SLUS_003.73");
    return Chopper::Attach(object).SpawnDebris();
}

uint32 sub_80055E0C(uint32 object, uint32 index)
{
    FUNCTION_MARKER(0x80055E0C, "SLUS_003.73");
    return Chopper::Attach(object).SpawnEnemy(index);
}

