#include "visual_object.h"
#include "object_damage.h"
#include "model_animation.h"
#include "game_sdk_bindings.h"
#include "player.h"
#include "explosion_effects.h"
#include "ring_burst_effect.h"
extern "C" {
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
}
#include "mini_tank.h"
#include "native_object_storage.h"
#include "game_object_allocation.h"
#include "audio_voice.h"
#include "effect_lifecycle.h"
#include "vector_arithmetic.h"
#include "model_attachment.h"
#include "angle_math.h"
#include "projectile_oscillating.h"
#include "game_random.h"
#include "vector_distance.h"
#include "sprite_trail.h"
#include "audio_session.h"
#include <new>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

MiniTank::MiniTank(const uint8 *record_data, uint32 index) : Enemy()
{
    const uint32 data = GuestMemoryAddress(record_data);
    uint32 object = NativeObjectGuestAddress(this);
    uint32 cursor, angle;
    FUNCTION_MARKER(0x8007488C, "SLUS_003.73");
    if (!data)
    {
        // TODO Support native record data in the original parser adapter
        GameDiagnostic("MiniTank record data is outside guest memory\n");
        abort();
    }
    w_u32(object + 68, 0x800A3C98);
    cursor = sub_80062B0C(object, data);
    angle = r_u16(cursor);
    this->model.angles[0] = angle;
    angle = r_u16(cursor + 2);
    this->model.angles[1] = angle;
    angle = r_u16(cursor + 4);
    w_u32(object + 400, cursor + 6);
    this->record_idx = index;
    this->model.angles[2] = angle;
    Init();
    cursor = r_u32(object + 400);
    if (cursor)
    {
        w_u8(object + 380, 1);
        sub_8004BE90(object, cursor);
    }
}

uint32 MiniTank::Destroy(uint32 reason)
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 count, sound;
    FUNCTION_MARKER(0x800749C4, "SLUS_003.73");
    count = r_u32(0x800FF898);
    w_u32(object + 68, 0x800A3C98);
    w_u32(0x800FF898, count - 1);
    RemoveFromList(visual_lists.actors);
    Enemy::FreeBehaviorTable();
    sound = r_u32(object + 512);
    if (sound)
        StopSoundVoices(sound);
    Enemy::Destroy(0);
    return (reason & 1) ? ReleaseObjectStorage(object) : 0;
}

uint32 MiniTank::Init()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 count, total, angles, roll, floor, sound, height;
    FUNCTION_MARKER(0x80074A58u, "SLUS_003.73");
    count = r_u32(0x800FF898u);
    total = r_u32(0x800FF308u);
    angles = ((uint32)this->model.angles[0] | ((uint32)this->model.angles[1] << 16));
    roll = this->model.angles[2];
    w_u32(object + 500u, angles);
    w_u16(object + 504u, roll);
    w_u32(0x800FF898u, count + 1u);
    w_u32(0x800FF308u, total + 1u);
    sub_800626C8(object, 0x800A3C88u);
    AddToList(visual_lists.actors);
    this->kind = 207u;
    w_u16(object + 456u, 64u);
    sub_8004BD04(object, 1u, 4u);
    w_u16(object + 436u, 50u);
    w_u16(object + 438u, 128u);
    w_u16(object + 218u, 20u);
    sub_8004BDCC(object, 0u, 1u, 10u);
    sub_8004BDCC(object, 0u, 2u, 140u);
    sub_8004BDCC(object, 0u, 3u, 50u);
    floor = sub_80067A18(object + 4u, 512u, 512u);
    if (floor != 0xFFFFFFFFu)
    {
        height = (uint32)(sint32)(sint16)r_u16(object + 456u);
        this->model.pos[1] = floor - (height << 12);
    }
    w_u32(object + 516u, 0xFFFFFFFFu);
    sound = sub_80069EF4(89u, object + 4u, 0u);
    w_u32(object + 512u, sound);
    if (sound)
        w_u32(object + 516u, 16u);
    w_u8(object + 380u, 1u);
    return 1u;
}

uint32 MiniTank::Divide(uint32 numerator, uint32 denominator)
{
    sint32 left = (sint32)numerator, right = (sint32)denominator;
    if (!right)
        return left < 0 ? 1u : 0xFFFFFFFFu;
    if (left == (-2147483647 - 1) && right == -1)
        return numerator;
    return (uint32)(left / right);
}

uint32 MiniTank::Remainder(uint32 numerator, uint32 denominator)
{
    sint32 left = (sint32)numerator, right = (sint32)denominator;
    if (!right)
        return numerator;
    if (left == (-2147483647 - 1) && right == -1)
        return 0u;
    return (uint32)(left % right);
}

uint32 MiniTank::Update()
{
    SVECTOR motion_cur, motion_target;
    uint32 motion_cur_word, motion_cur_tail, motion_target_word, motion_target_tail;
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 value, flags, count, radius, factor, damage;
    uint32 index, points, radii, factors, tick, word, tail, scale, period, frame, sound;
    uint32 constants[3];
    sint32 distance;
    sint16 angles[3];
    FUNCTION_MARKER(0x80074BA8u, "SLUS_003.73");
    value = r_u32(object + 524u);
    w_u32(object + 524u, value + 1u);
    constants[0] = r_u32(0x800A71CCu);
    constants[1] = r_u32(0x800A71D0u);
    constants[2] = r_u32(0x800A71D4u);
    motion.acceleration[0] = constants[0];
    motion.acceleration[1] = constants[1];
    motion.acceleration[2] = constants[2];
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
    if (!r_u8(object + 381u))
    {
        SetModelBoneTable(this->model.bank, MiniTankAttachments);
        InitializeModelAnimation(MiniTankHierarchy);
        flags = this->model.flags;
        w_u8(object + 381u, 1u);
        this->model.flags = flags | 4u;
    }
    flags = this->activation_flags;
    if (flags & 1u)
    {
        tick = r_u8(object + 384u);
        flags = this->activation_flags;
        w_u8(object + 384u, tick + 1u);
        this->activation_flags = flags & 0xFFFEu;
    }
    flags = this->activation_flags;
    if (flags & 4u)
    {
        index = 0u;
        factors = 0x800A7288u;
        radii = 0x800A727Cu;
        points = 0x800A724Cu;
        for (;;)
        {
            count = r_u32(0x800FF648u);
            if ((sint32)index >= (sint32)count)
                break;
            distance = (sint32)sub_8006696C(object + 4u, points);
            radius = r_u32(radii);
            if (distance < (sint32)radius)
            {
                value = radius - (uint32)distance;
                factor = r_u32(factors);
                damage = Divide(factor * value, radius);
                ApplyObjectDamage(object, damage, (const sint32 *)psx_addr(0x800A71CCu, 12), 0u);
                if ((sint16)r_u16(object + 218u) <= 0)
                    break;
            }
            factors += 4u;
            radii += 4u;
            points += 12u;
            ++index;
        }
    }
    else if (r_u32(object + 396u) & 1u)
    {
        value = sub_8006696C(object + 4u, object + 484u);
        if (value < 129u)
        {
            flags = r_u32(object + 396u);
            w_u32(object + 396u, flags & 0xFFFFFFFEu);
        }
        else
        {
            xport_draft_host_sub_80066B8C_p1(angles, object + 4u, object + 484u);
            value = this->model.angles[1];
            value = ((value - (uint16)angles[1]) & 4095u) - 64u;
            if (value < 0xF81u)
            {
                motion.velocity[2] = 0u;
                motion.velocity[1] = 0u;
                motion.velocity[0] = 0u;
                word = apocalypse_angle_pair_word(object + 16u);
                tail = this->model.angles[2];
                value = (uint32)(uint16)angles[0] | ((uint32)(uint16)angles[1] << 16);
                motion_cur_word = word;
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
                UpdateAngleAcceleration(motion_cur, motion.angular_velocity, motion.angular_acceleration, motion_target, 32u);
            }
            else
            {
                word = apocalypse_angle_pair_word(object + 500u);
                tail = r_u16(object + 504u);
                this->model.SetAnglesXY(word);
                this->model.SetAnglesXY(word);
                this->model.angles[2] = tail;
                motion.angular_velocity[2] = 0u;
                motion.angular_velocity[1] = 0u;
                motion.angular_velocity[0] = 0u;
                motion.angular_acceleration[2] = 0u;
                motion.angular_acceleration[1] = 0u;
                scale = r_u16(object + 436u);
                motion.angular_acceleration[0] = 0u;
                BuildDirectionVector(motion.velocity, scale, (const uint16 *)angles);
            }
        }
    }
    Aim();
    UpdateModelAnimationData(object);
    sub_8007FDF0(object);
    BuildModelPose(*this);
    if (r_u8(object + 382u))
        AddVectors(motion.acceleration, (const uint32 *)psx_addr(0x800A6844u, 12));
    sub_8004B9A4(object);
    period = r_u32(object + 516u);
    if (period != 0xFFFFFFFFu)
    {
        frame = r_u32(0x800FF2F0u);
        if (!Remainder(frame, period))
        {
            sound = r_u32(object + 512u);
            sub_80069FAC(sound, object + 4u, 0u);
        }
    }
    // The legacy behavior caller discards the original return carrier
    return 0u;
}

uint32 MiniTank::Aim()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 position[3], player, angle, yaw;
    uint16 rotation[3];
    FUNCTION_MARKER(0x80074F3C, "SLUS_003.73");
    GetModelAttachment(position, object, 1);
    player = visual_lists.players;
    CalculateDirectionAngles(rotation, position, (const uint32 *)psx_addr(player + 4, 12));
    angle = rotation[1];
    yaw = this->model.angles[1];
    angle = (angle - yaw) & 4095;
    GetModelJointAngles(2).rotation[1] = (uint16)angle;
    return angle;
}



MiniTankRing::MiniTankRing(const uint32 acceleration[3]) : MeshRingEffect(12, 0xCCE0187C)
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 x = acceleration[0], y = acceleration[1], z = acceleration[2];
    // Original constructor is inlined in MiniTank state and damage handlers
    w_u32(object + 68, 0x800A3CF0);
    w_u32(object + 36, x);
    w_u32(object + 40, y);
    w_u32(object + 44, z);
}

uint32 MiniTankRing::Destroy(uint32 reason)
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 result;
    FUNCTION_MARKER(0x80075F38, "SLUS_003.73");
    result = MeshRingEffect::Destroy(0);
    if (reason & 1)
        return ReleaseEffectStorage(object);
    return result;
}

uint32 MiniTankRing::Update()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 age, lifetime, radius_step, radius, width_step, width, value, color;
    FUNCTION_MARKER(0x80075E7C, "SLUS_003.73");
    age = r_u16(object + 8);
    lifetime = r_u16(object + 10);
    age += 1;
    w_u16(object + 8, age);
    if ((sint32)lifetime < (sint16)age)
        return Finish();
    AddVectors((uint32 *)psx_addr(object + 24, 12), (const uint32 *)psx_addr(object + 36, 12));
    radius_step = (uint32)(sint32)(sint16)r_u16(object + 116);
    radius = r_u32(object + 92);
    width_step = (uint32)(sint32)(sint16)r_u16(object + 118);
    width = r_u32(object + 96);
    SetGeometry((const uint32 *)psx_addr(object + 24, 12), radius + radius_step, width + width_step);
    lifetime = r_u16(object + 10);
    age = (uint32)(sint32)(sint16)r_u16(object + 8);
    value = lifetime - age;
    value = (value << 8) - value;
    color = lifetime ? (uint32)((sint32)value / (sint32)lifetime) : ((sint32)value < 0 ? 1 : (uint32)-1);
    return SetColors(color, color, (uint32)((sint32)color >> 1));
}

MiniTankRing &MiniTankRing::Attach(uint32 object)
{
    return *static_cast<MiniTankRing *>(FindNativeObjectStorage(object));
}

extern "C" uint32 sub_80075F38(uint32 object, uint32 reason)
{
    FUNCTION_MARKER(0x80075F38, "SLUS_003.73");
    return MiniTankRing::Attach(object).MiniTankRing::Destroy(reason);
}

extern "C" uint32 sub_80075E7C(uint32 object)
{
    FUNCTION_MARKER(0x80075E7C, "SLUS_003.73");
    return MiniTankRing::Attach(object).MiniTankRing::Update();
}

void MiniTank::Burst(uint32 green)
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 child, acceleration[3];
    sub_8001E1B8(object + 4u, 0u);
    sub_8001D320(object + 4u, 200u, 240u, green, 0u, 5u, 0u, 100u);
    child = AllocateEffectStorage(120);
    if (child)
    {
        acceleration[0] = r_u32(0x800A71CCu);
        acceleration[1] = r_u32(0x800A71D0u);
        acceleration[2] = r_u32(0x800A71D4u);
        void *dram_addr = AllocateNativeObjectStorage(child, sizeof(MiniTankRing));
        new (dram_addr) MiniTankRing(acceleration);
        MiniTankRing::Attach(child).SetGeometry((const uint32 *)psx_addr(object + 4, 12), 0, 0);
        w_u16(child + 10u, 32u);
        w_u16(child + 116u, 32u);
        w_u16(child + 118u, 16u);
    }
    acceleration[0] = 0u;
    acceleration[1] = 0xFFFEC000u;
    acceleration[2] = 0u;
    child = AllocateEffectStorage(120);
    if (child)
    {
        void *dram_addr = AllocateNativeObjectStorage(child, sizeof(MiniTankRing));
        new (dram_addr) MiniTankRing(acceleration);
        MiniTankRing::Attach(child).SetGeometry((const uint32 *)psx_addr(object + 4, 12), 0, 0);
        w_u16(child + 10u, 32u);
        w_u16(child + 116u, 32u);
        w_u16(child + 118u, 16u);
    }
}

void MiniTank::FinishFromTable()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 table = r_u32(object + 68u);
    uint32 adjustment = (uint32)(sint32)(sint16)r_u16(table + 16u);
    uint32 target = r_u32(table + 20u);
    apocalypse_object_virtual20(target, object + adjustment);
}

void MiniTank::UpdateState()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 state, phase, flags, player, count, parameter, random, value, child;
    uint32 damage, x, y, z, dx, dy, dz, height;
    uint32 candidate[3], position[3];
    sint32 distance, floor;
    FUNCTION_MARKER(0x80074FA8u, "SLUS_003.73");
    state = r_u16(object + 472u);
    if (!state)
    {
        phase = r_u16(object + 474u);
        switch (phase)
        {
            case 0u:
                flags = r_u32(object + 396u);
                if (!(flags & 1u))
                {
                    player = visual_lists.players;
                    value = apocalypse_candidate_native(object, candidate, player + 4u,
                        128u, 512u, 64u, 1024u, 1u, 1u);
                    if (value)
                        SetTarget(candidate);
                }
                player = visual_lists.players;
                distance = (sint32)MeasurePositionDistance((const uint32 *)this->model.pos, (const uint32 *)psx_addr(player + 4, 12));
                if (distance < 204)
                {
                    phase = r_u16(object + 474u);
                    w_u16(object + 474u, phase + 1u);
                    return;
                }
                count = r_u32(object + 508u);
                w_u32(object + 508u, count + 1u);
                parameter = GetBehaviorValue(0, 3);
                random = RandomRange(5);
                parameter = (parameter & 0xFFFFu) + random;
                count = r_u32(object + 508u);
                if (count < parameter)
                    return;
                position[0] = this->model.pos[0];
                player = visual_lists.players;
                position[1] = this->model.pos[1];
                position[2] = this->model.pos[2];
                if (xport_draft_host_sub_800679A4_p1(position, player + 4u))
                {
                    w_u16(object + 474u, 3u);
                    flags = r_u32(object + 396u);
                    w_u32(object + 508u, 0u);
                    w_u32(object + 396u, flags & 0xFFFFFFFEu);
                }
                return;
            case 1u:
                phase = r_u16(object + 474u);
                w_u16(object + 474u, phase + 1u);
                return;
            case 2u:
                value = this->model.angles[1];
                count = r_u32(object + 524u) & 3u;
                this->model.angles[1] = value + 128u;
                if (!count)
                    sub_800626F8(object, 3u, 255u, 255u, 255u);
                else if (count == 2u)
                    sub_800626F8(object, 3u, 255u, 0u, 0u);
                if (RandomRange(16))
                    return;
                player = visual_lists.players;
                distance = (sint32)MeasurePositionDistance((const uint32 *)this->model.pos, (const uint32 *)psx_addr(player + 4, 12));
                if (distance < 1024)
                {
                    damage = (uint32)((sint32)(1024u - (uint32)distance) >> 6);
                    player = visual_lists.players;
                    ApplyObjectDamage(player, damage, (const sint32 *)psx_addr(0x800A71CCu, 12), 0u);
                }
                flags = this->lifecycle_flags;
                this->lifecycle_flags = flags & 0xFFEFu;
                Burst(200u);
                FinishFromTable();
                return;
            case 3u:
                value = r_u32(object + 508u) + 1u;
                w_u32(object + 508u, value);
                if (value >= 5u)
                {
                    w_u16(object + 474u, 4u);
                    w_u32(object + 508u, 0u);
                }
                return;
            case 4u:
                if (!RandomRange(8))
                {
                    Fire();
                    w_u16(object + 474u, 0u);
                }
                return;
            default:
                return;
        }
    }
    if (state != 2u)
        return;
    phase = r_u16(object + 474u);
    if (!phase)
    {
        value = this->model.pos[1];
        w_u32(object + 396u, 0u);
        w_u8(object + 382u, 1u);
        w_u16(object + 460u, (uint32)((sint32)value >> 12));
        random = RandomRange(256);
        w_u16(object + 462u, 128u - random);
        random = RandomRange(256);
        w_u16(object + 464u, 128u - random);
        random = RandomRange(256);
        w_u16(object + 466u, 128u - random);
        random = RandomRange(120);
        w_u32(object + 520u, (random + 40u) << 12);
        motion.drag[2] = 31u;
        motion.drag[1] = 31u;
        motion.drag[0] = 31u;
        random = RandomRange(20);
        value = r_u32(object + 520u);
        motion.velocity[0] = (10u - random) << 12;
        motion.velocity[1] = 0u - value;
        random = RandomRange(20);
        phase = r_u16(object + 474u);
        motion.velocity[2] = (10u - random) << 12;
        w_u16(object + 474u, phase + 1u);
        sub_8001D320(object + 4u, 50u, 240u, 220u, 0u, 5u, 0u, 100u);
        w_u32(0x800FF3ACu, 0u);
        child = AllocateEffectStorage(100);
        if (child)
        {
            void *dram_addr = AllocateNativeObjectStorage(child, sizeof(RibbonTrail));
            new (dram_addr) RibbonTrail((const uint32 *)this->model.pos, 4, 128, 128, 128);
        }
        w_u32(object + 496u, child);
        w_u32(0x800FF3ACu, 1u);
        return;
    }
    if (phase != 1u)
        return;
    child = r_u32(object + 496u);
    SpriteTrail::Attach(child).SetPosition((const uint32 *)this->model.pos);
    dx = (uint32)(sint32)(sint16)r_u16(object + 462u);
    x = this->model.angles[0];
    x += dx;
    dz = (uint32)(sint32)(sint16)r_u16(object + 466u);
    this->model.angles[0] = x;
    z = this->model.angles[2];
    dy = (uint32)(sint32)(sint16)r_u16(object + 464u);
    z += dz;
    this->model.angles[2] = z;
    y = this->model.angles[1];
    x = this->model.angles[0];
    y += dy;
    this->model.angles[1] = y;
    z = this->model.angles[2];
    this->model.angles[0] = x & 4095u;
    y = this->model.angles[1];
    this->model.angles[2] = z & 4095u;
    height = this->model.pos[1];
    this->model.angles[1] = y & 4095u;
    floor = (sint16)r_u16(object + 460u);
    if (floor >= ((sint32)height >> 12))
        return;
    value = r_u32(object + 520u);
    if ((sint32)value > 163840)
    {
        value -= 163840u;
        w_u32(object + 520u, value);
        motion.velocity[1] = 0u - value;
        return;
    }
    Burst(220u);
    child = r_u32(object + 496u);
    if (child)
        w_u32(child + 96u, 1u);
    FinishFromTable();
}

uint32 MiniTank::Damage(uint32 damage, const sint32 *input_vector, uint32)
{
    FUNCTION_MARKER(0x80075930, "SLUS_003.73");
    uint32 guest_vector = GuestMemoryAddress(input_vector);
    const sint32 *native_vector = guest_vector ? 0 : input_vector;
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 tick, health, signed_health, flags, player, child, table, receiver, target;
    uint32 impulse[3], acceleration[3], shift = 6;
    if (r_u32(object + 524) < 60)
    {
        SetColor(3, 255, 255, 255);
        return 1;
    }
    tick = r_u8(object + 383);
    w_u8(object + 383, tick + 1);
    signed_health = (uint32)(sint32)(sint16)r_u16(object + 218);
    health = r_u16(object + 218);
    if ((sint32)signed_health <= 0)
        return 1;
    w_u16(object + 218, health - damage);
    ShiftVectorLeft(impulse, native_vector ? (const uint32 *)native_vector : (const uint32 *)psx_addr(guest_vector, 12), &shift);
    AddVectors((uint32 *)motion.velocity, impulse);
    signed_health = (uint32)(sint32)(sint16)r_u16(object + 218);
    motion.velocity[1] = 0;
    if ((sint32)signed_health > 0)
    {
        SetColor(3, 255, 255, 255);
        return 1;
    }
    player = visual_lists.players;
    if (player)
        TryPlayPlayerKillQuote((const uint32 *)psx_addr(player + 4, 12));
    flags = this->lifecycle_flags;
    this->lifecycle_flags = flags & 0xFFEF;
    if (!RandomRange(2))
    {
        w_u16(object + 472, 2);
        w_u16(object + 474, 0);
        return 1;
    }
    HandleExplosionProximity((const uint32 *)this->model.pos, 0);
    SpawnExplosionRing((const uint32 *)this->model.pos, 200, 240, 200, 0, 5, 0, 100);
    child = AllocateEffectStorage(120);
    if (child)
    {
        acceleration[0] = r_u32(0x800A71CC);
        acceleration[1] = r_u32(0x800A71D0);
        acceleration[2] = r_u32(0x800A71D4);
        void *dram_addr = AllocateNativeObjectStorage(child, sizeof(MiniTankRing));
        new (dram_addr) MiniTankRing(acceleration);
        MiniTankRing::Attach(child).SetGeometry((const uint32 *)this->model.pos, 0, 0);
        w_u16(child + 10, 32);
        w_u16(child + 116, 32);
        w_u16(child + 118, 16);
    }
    acceleration[0] = 0;
    acceleration[1] = 0xFFFEC000;
    acceleration[2] = 0;
    child = AllocateEffectStorage(120);
    if (child)
    {
        void *dram_addr = AllocateNativeObjectStorage(child, sizeof(MiniTankRing));
        new (dram_addr) MiniTankRing(acceleration);
        MiniTankRing::Attach(child).SetGeometry((const uint32 *)this->model.pos, 0, 0);
        w_u16(child + 10, 32);
        w_u16(child + 116, 32);
        w_u16(child + 118, 16);
    }
    table = r_u32(object + 68);
    receiver = object + (uint32)(sint32)(sint16)r_u16(table + 16);
    target = r_u32(table + 20);
    apocalypse_object_virtual20(target, receiver);
    return 1;
}

uint32 MiniTank::SetTarget(const uint32 *target)
{
    const uint32 guest_target = GuestMemoryAddress(target);
    const uint32 *native_target = guest_target ? 0 : target;
    FUNCTION_MARKER(0x80075BE8, "SLUS_003.73");
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 x, y, z, left, right, scale, velocity_x, velocity_z, angle, flags;
    uint32 difference[3], normal[3];
    if (this->model.animation)
        StartAnimation(0, 1);
    if (native_target)
    {
        x = native_target[0];
        y = native_target[1];
        z = native_target[2];
    }
    else
    {
        x = r_u32(guest_target);
        y = r_u32(guest_target + 4);
        z = r_u32(guest_target + 8);
    }
    w_u32(object + 484, x);
    w_u32(object + 488, y);
    w_u32(object + 492, z);
    left = r_u32(object + 484);
    right = this->model.pos[0];
    difference[0] = (uint32)((sint32)(left - right) >> 12);
    left = r_u32(object + 488);
    right = this->model.pos[1];
    difference[1] = (uint32)((sint32)(left - right) >> 12);
    left = r_u32(object + 492);
    right = this->model.pos[2];
    difference[2] = (uint32)((sint32)(left - right) >> 12);
    NormalizeShortVector(difference, normal);
    scale = r_u16(object + 436);
    velocity_x = normal[0] * scale;
    velocity_z = normal[2] * scale;
    motion.velocity[0] = velocity_x;
    motion.velocity[1] = 0;
    motion.velocity[2] = velocity_z;
    left = r_u32(object + 492);
    right = this->model.pos[2];
    z = left - right;
    left = r_u32(object + 484);
    right = this->model.pos[0];
    angle = 3072 - (uint32)ratan2((sint32)z, (sint32)(left - right));
    flags = r_u32(object + 396);
    w_u16(object + 502, angle & 4095);
    w_u32(object + 396, flags | 1);
    return flags | 1;
}

uint32 MiniTank::Fire()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 player, position[3], target[3], projectile, damage, lifetime, speed;
    uint16 rotation[3];
    FUNCTION_MARKER(0x80075CFC, "SLUS_003.73");
    player = visual_lists.players;
    if (!player)
        return 0;
    GetModelAttachment(position, object, 1);
    player = visual_lists.players;
    GetCharacterAttachment(target, player, 9);
    target[1] += 65536;
    CalculateDirectionAngles(rotation, position, target);
    GetModelAttachment(position, object, 0);
    projectile = AllocateEffectStorage(108);
    if (projectile)
    {
        damage = GetBehaviorValue(0, 1) & 65535;
        lifetime = GetBehaviorValue(0, 2) & 65535;
        speed = GetBehaviorValue(0, 2) & 65535;
        void *dram_addr = AllocateNativeObjectStorage(projectile, sizeof(OscillatingProjectile));
        new (dram_addr) OscillatingProjectile(position, (const sint16 *)rotation, damage, lifetime, speed, 16, 240, 240, 240, 96, 128, 0);
    }
    return PlayPositionedSound(20, (const uint32 *)this->model.pos, 0);
}

MiniTank &MiniTank::Attach(uint32 object)
{
    return *static_cast<MiniTank *>(FindNativeObjectStorage(object));
}

// Compatibility entry points retain the guest call contract
uint32 sub_8007488C(uint32 object, uint32 data, uint32 index)
{
    FUNCTION_MARKER(0x8007488C, "SLUS_003.73");
    void *native_instance_storage = AllocateNativeObjectStorage(object, sizeof(MiniTank));
    new (native_instance_storage) MiniTank((const uint8 *)psx_addr(data, 1), index);
    return object;
}

uint32 sub_800749C4(uint32 object, uint32 reason)
{
    FUNCTION_MARKER(0x800749C4, "SLUS_003.73");
    return MiniTank::Attach(object).MiniTank::Destroy(reason);
}

void sub_80074BA8(uint32 object)
{
    FUNCTION_MARKER(0x80074BA8, "SLUS_003.73");
    MiniTank::Attach(object).MiniTank::Update();
}

uint32 sub_80074F3C(uint32 object)
{
    FUNCTION_MARKER(0x80074F3C, "SLUS_003.73");
    return MiniTank::Attach(object).Aim();
}

void sub_80074FA8(uint32 object)
{
    FUNCTION_MARKER(0x80074FA8, "SLUS_003.73");
    MiniTank::Attach(object).UpdateState();
}

uint32 sub_80075930(uint32 object, uint32 damage, uint32 vector)
{
    FUNCTION_MARKER(0x80075930u, "SLUS_003.73");
    return MiniTank::Attach(object).MiniTank::Damage(damage, vector ? (const sint32 *)psx_addr(vector, 12) : 0, 0);
}

uint32 apocalypse_record207_damage_native(uint32 object, uint32 damage, const sint32 vector[3])
{
    return MiniTank::Attach(object).MiniTank::Damage(damage, vector, 0);
}

uint32 sub_80075BE8(uint32 object, uint32 target)
{
    FUNCTION_MARKER(0x80075BE8, "SLUS_003.73");
    return MiniTank::Attach(object).SetTarget((const uint32 *)psx_addr(target, 12));
}

uint32 apocalypse_record207_target_native(uint32 object, const uint32 target[3])
{
    return MiniTank::Attach(object).SetTarget(target);
}

uint32 sub_80075CFC(uint32 object)
{
    FUNCTION_MARKER(0x80075CFC, "SLUS_003.73");
    return MiniTank::Attach(object).Fire();
}
