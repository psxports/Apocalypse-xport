#include "game_sdk_bindings.h"
#include "player.h"
#include "game_panel.h"
#include "fire_cycle.h"
extern "C" {
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
}
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

uint32 SpawnTank(uint32 object, uint32 index)
{
    uint32 child, script;
    FUNCTION_MARKER(0x80055E90, "SLUS_003.73");
    child = sub_800625AC(576);
    if (!child)
        return 0;
    script = (sint32)index < 0 ? 0 : r_u32(object + index * 4 + 404);
    return Tank::Spawn(child, object + 4, script);
}

Tank::Tank(const uint8 *record_data, uint32 index) : Enemy()
{
    const uint32 data = GuestMemoryAddress(record_data);
    uint32 object = NativeObjectGuestAddress(this);
    uint32 cursor;
    FUNCTION_MARKER(0x80055F14, "SLUS_003.73");
    if (!data)
    {
        // TODO Support native record data in the original parser adapter
        GameDiagnostic("Tank record data is outside guest memory\n");
        abort();
    }
    w_u32(object + 68, 0x800A29B8);
    cursor = sub_80062B0C(object, data);
    cursor = sub_80062B50(object, cursor);
    w_u32(object + 400, cursor);
    w_u8(object + 380, 1);
    w_u16(object + 214, index);
    Init();
    cursor = r_u32(object + 400);
    sub_8004BE90(object, cursor);
}

Tank::Tank(const uint32 *position_vector, uint32 script) : Enemy()
{
    uint32 position = GuestMemoryAddress(position_vector);
    uint32 object = NativeObjectGuestAddress(this);
    uint32 x, y, z, cursor;
    FUNCTION_MARKER(0x80055F9Cu, "SLUS_003.73");
    w_u32(object + 68u, 0x800A29B8u);
    x = position ? r_u32(position) : position_vector[0];
    y = position ? r_u32(position + 4u) : position_vector[1];
    z = position ? r_u32(position + 8u) : position_vector[2];
    w_u32(object + 240u, x);
    w_u32(object + 244u, y);
    w_u32(object + 248u, z);
    x = r_u32(object + 240u);
    y = r_u32(object + 244u);
    z = r_u32(object + 248u);
    w_u32(object + 4u, x);
    w_u32(object + 8u, y);
    w_u32(object + 12u, z);
    w_u32(object + 400u, script);
    w_u8(object + 380u, 1u);
    Init();
    cursor = r_u32(object + 400u);
    sub_8004BE90(object, cursor);
}

uint32 Tank::Init()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 flags, child, value, bank;
    FUNCTION_MARKER(0x80056038u, "SLUS_003.73");
    w_u32(0x800FF308u, r_u32(0x800FF308u) + 1u);
    sub_80062A38(object, 0x800FF5DCu);
    w_u16(object + 58u, 205u);
    flags = r_u16(object);
    w_u8(object + 129u, 3u);
    w_u16(object, flags | 16u);
    w_u8(object + 130u, 3u);
    w_u8(object + 131u, 3u);
    sub_8004BD04(object, 3u, 9u);
    w_u8(object + 382u, 0u);
    w_u16(object + 472u, 0u);
    w_u16(object + 474u, 0u);
    w_u8(object + 381u, 0u);
    sub_8004BDCC(object, 0u, 0u, 0u);
    sub_8004BDCC(object, 0u, 1u, 30u);
    sub_8004BDCC(object, 0u, 2u, 64u);
    sub_8004BDCC(object, 0u, 3u, 64u);
    sub_8004BDCC(object, 0u, 4u, 64u);
    sub_8004BDCC(object, 0u, 5u, 0u);
    sub_8004BDCC(object, 1u, 0u, 0u);
    sub_8004BDCC(object, 1u, 1u, 1u);
    sub_8004BDCC(object, 1u, 2u, 128u);
    sub_8004BDCC(object, 1u, 3u, 12u);
    sub_8004BDCC(object, 1u, 4u, 64u);
    sub_8004BDCC(object, 1u, 5u, 0u);
    sub_8004BDCC(object, 2u, 0u, 0u);
    sub_8004BDCC(object, 2u, 1u, 20u);
    sub_8004BDCC(object, 2u, 2u, 30u);
    sub_8004BDCC(object, 2u, 3u, 512u);
    sub_8004BDCC(object, 2u, 4u, 8u);
    sub_8004BDCC(object, 2u, 5u, 300u);
    sub_8004BDCC(object, 2u, 6u, 256u);
    sub_8004BDCC(object, 2u, 7u, 128u);
    sub_8004BDCC(object, 2u, 8u, 0u);
    child = sub_8002FED8(72u);
    if (child)
        child = sub_80024428(child, 0x800FF5A0u, 0u);
    w_u32(object + 572u, child);
    value = sub_8004BDFC(object, 1u, 1u);
    bank = r_u8(0x800FF644u);
    child = r_u32(object + 572u);
    w_u32(child + 40u, value & 65535u);
    value = sub_8006E080(0xA48DD5ECu, bank);
    w_u8(object + 560u, value);
    w_u8(object + 561u, 10u);
    return 10u;
}

uint32 Tank::Destroy(uint32 reason)
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 flags, child, table, receiver, target, sound;
    FUNCTION_MARKER(0x800562C4, "SLUS_003.73");
    w_u32(object + 68, 0x800A29B8);
    RemoveFromList((uint32 *)psx_addr(0x800FF5DC, 4));
    Enemy::FreeBehaviorTable();
    if (r_u32(object + 396) & 32)
    {
        ClearTrackedObjectHealth();
        flags = r_u32(object + 396);
        w_u32(object + 396, flags & 0xFFFFFFDF);
    }
    child = r_u32(object + 572);
    if (child)
    {
        table = r_u32(child);
        receiver = child + (uint32)(sint32)(sint16)r_u16(table + 8);
        target = r_u32(table + 12);
        (void)apocalypse_object_cleanup(target, receiver, 3);
        w_u32(object + 572, 0);
    }
    sound = r_u32(object + 564);
    if (sound)
        StopSoundVoices(sound);
    if (r_u32(0x800FF4EC) == object)
        w_u32(0x800FF4EC, 0);
    Enemy::Destroy(0);
    return (reason & 1) ? ReleaseObjectStorage(object) : 0;
}

uint32 Tank::Update()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 position[3], flags, value, count, mode, target, node, packed, tail, sound;
    uint16 desired[3];
    sint32 wanted_yaw, current_yaw;
    uint32 move;
    FUNCTION_MARKER(0x800563B4u, "SLUS_003.73");
    position[0] = r_u32(0x800A71CCu);
    position[1] = r_u32(0x800A71D0u);
    position[2] = r_u32(0x800A71D4u);
    w_u32(object + 116u, position[0]);
    w_u32(object + 120u, position[1]);
    w_u32(object + 124u, position[2]);
    flags = r_u16(object + 76u);
    w_u16(object + 388u, flags);
    if (flags & 1u)
    {
        value = r_u8(object + 384u);
        w_u16(object + 76u, flags & 0xFFFEu);
        w_u8(object + 384u, value + 1u);
    }
    if (!r_u8(object + 381u))
    {
        flags = r_u16(object);
        w_u8(object + 381u, 1u);
        w_u16(object, flags | 4u);
        SetAnimationRange(0u, 0u, 0xFFFFFFFFu);
    }
    if (r_u8(object + 380u))
    {
        count = r_u16(object + 476u);
        if (count)
            w_u16(object + 476u, count - 1u);
        else
            sub_8004BE90(object, r_u32(object + 400u));
    }
    else
        UpdateState();
    if (!(r_u16(object) & 4u))
        return 0u;
    flags = r_u32(object + 396u);
    if (flags & 16u)
    {
        target = r_u32(object + 568u);
        target += r_u16(target + 58u) == 50u ? 184u : 4u;
        position[0] = r_u32(target);
        position[1] = r_u32(target + 4u);
        position[2] = r_u32(target + 8u);
        w_u32(object + 500u, position[0]);
        w_u32(object + 504u, position[1]);
        w_u32(object + 508u, position[2]);
        mode = r_u8(object + 496u);
    }
    else if (flags & 8u)
    {
        xport_draft_host_sub_8006C34C_p1(position, object + 4u, object + 512u);
        w_u32(object + 500u, position[0]);
        w_u32(object + 504u, position[1]);
        w_u32(object + 508u, position[2]);
        mode = r_u8(object + 496u);
    }
    else
        mode = (flags & 4u) ? r_u8(object + 496u) : 0u;
    Aim(mode);
    if (r_u32(object + 396u) & 1u)
    {
        if ((sint32)sub_80066918(object + 4u, object + 484u) < 128)
        {
            flags = r_u32(object + 396u);
            w_u32(object + 396u, flags & 0xFFFFFFFEu);
        }
        else
        {
            sound = r_u32(object + 564u);
            if (sound)
            {
                if (!(r_u32(0x800FF2F0u) & 15u))
                    sub_80069FAC(sound, object + 4u, 0u);
            }
            else
                w_u32(object + 564u, sub_80069EF4(49u, object + 4u, 0u));
            xport_draft_host_sub_80066B8C_p1(desired, object + 4u, object + 484u);
            desired[0] = 0u;
            packed = apocalypse_angle_pair_word(object + 16u);
            tail = r_u16(object + 20u);
            /* Original desired roll is unused by the full 66CF0 helper */
            sub_80066CF0(packed, tail, object + 132u, object + 138u,
                (uint32)desired[1] << 16, 0u, 16u);
            w_u16(object + 16u, 0u);
            wanted_yaw = (sint16)desired[1];
            current_yaw = (sint16)r_u16(object + 18u);
            move = wanted_yaw == current_yaw;
            if (wanted_yaw < current_yaw)
                move = (uint32)(current_yaw - wanted_yaw) - 64u >= 0xF81u;
            else if (wanted_yaw > current_yaw)
                move = (uint32)(wanted_yaw - current_yaw) - 64u >= 0xF81u;
            if (move)
                xport_draft_host_sub_800667CC_p3(object + 104u, r_u16(object + 436u), desired);
            else
            {
                w_u32(object + 112u, 0u);
                w_u32(object + 108u, 0u);
                w_u32(object + 104u, 0u);
            }
        }
    }
    else
    {
        sound = r_u32(object + 564u);
        if (sound)
        {
            sub_8006A294(sound);
            w_u32(object + 564u, 0u);
        }
    }
    if (r_u8(object + 382u))
        sub_8006C0B8(object + 116u, 0x800A6844u);
    sub_8004B9A4(object);
    sub_8006C730(object + 524u, object + 530u);
    sub_8006C624(object + 524u);
    sub_8006C730(object + 530u, object + 536u);
    sub_8006C8E8(object + 530u, object + 144u);
    sub_8006C64C(object + 530u);
    node = r_u32(object + 360u);
    w_u16(node + 14u, r_u16(object + 526u));
    node = r_u32(object + 360u);
    w_u16(node + 24u, r_u16(object + 524u));
    node = r_u32(object + 360u);
    value = r_u16(node + 24u);
    if (value - 129u < 0x77Fu)
        w_u16(node + 24u, 128u);
    sub_8006C730(object + 542u, object + 548u);
    sub_8006C624(object + 542u);
    sub_8006C730(object + 548u, object + 554u);
    sub_8006C8E8(object + 548u, object + 144u);
    sub_8006C64C(object + 548u);
    node = r_u32(object + 360u);
    w_u16(node + 38u, r_u16(object + 544u));
    sub_8007FDF0(object);
    sub_8007D148(object);
    if (sub_8004BDFC(object, 0u, 0u) & 0xFFFFu)
    {
        w_u8(object + 496u, r_u8(object + 496u) | 1u);
        count = sub_8004BDFC(object, 0u, 5u);
        if (count & 0xFFFFu)
            sub_8004BDCC(object, 0u, 5u, (count - 1u) & 0xFFFFu);
        else
        {
            FireProjectile();
            value = sub_8004BDFC(object, 0u, 3u);
            sub_8004BDCC(object, 0u, 5u, value & 0xFFFFu);
        }
    }
    else
        w_u8(object + 496u, r_u8(object + 496u) & 0xFEu);
    mode = (sub_8004BDFC(object, 1u, 0u) & 0xFFFFu) != 0u;
    flags = r_u8(object + 496u);
    w_u8(object + 496u, mode ? flags | 2u : flags & 0xFDu);
    FireBeam(mode);
    value = sub_8004BDFC(object, 2u, 0u) & 0xFFFFu;
    if (!value)
        return value;
    count = sub_8004BDFC(object, 2u, 8u);
    if (count & 0xFFFFu)
        value = (count - 1u) & 0xFFFFu;
    else
    {
        FireDebris();
        value = sub_8004BDFC(object, 2u, 2u) & 0xFFFFu;
    }
    return sub_8004BDCC(object, 2u, 8u, value);
}

uint32 Tank::FireProjectile()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 node, yaw, pitch, position[3], child, value;
    uint16 angles[3];
    FUNCTION_MARKER(0x80056980u, "SLUS_003.73");
    node = r_u32(object + 360u);
    yaw = r_u16(object + 18u);
    yaw = (yaw + r_u16(node + 14u)) & 0xFFFu;
    pitch = r_u16(object + 524u);
    angles[0] = (uint16)pitch;
    angles[1] = (uint16)yaw;
    angles[2] = 0u;
    xport_draft_host_sub_8007CC10_p1(position, object, 2u);
    child = sub_800625AC(440u);
    if (child)
        child = apocalypse_chopper_projectile_native(child, position, angles, 0u, 4u, 128u, 0x800FF5A0u, 0x800A71CCu);
    value = sub_8004BDFC(object, 0u, 1u);
    value = sub_8004B914(object, value & 65535u);
    w_u16(child + 308u, value);
    w_u16(child + 310u, 512u);
    return 512u;
}

void Tank::FireBeam(uint32 fire)
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 weapon, node, yaw, joint_yaw, pitch, table, receiver, target;
    uint32 position[3];
    uint16 angles[3];
    FUNCTION_MARKER(0x80056A6Cu, "SLUS_003.73");
    weapon = r_u32(object + 572u);
    if (!weapon)
        return;
    node = r_u32(object + 360u);
    yaw = r_u16(object + 18u);
    joint_yaw = r_u16(node + 38u);
    pitch = r_u16(object + 542u);
    angles[0] = (uint16)pitch;
    angles[2] = 0u;
    angles[1] = (uint16)((yaw + joint_yaw) & 0xFFFu);
    xport_draft_host_sub_8007CC10_p1(position, object, 3u);
    weapon = r_u32(object + 572u);
    table = r_u32(weapon);
    receiver = weapon + (uint32)(sint32)(sint16)r_u16(table + (fire ? 16u : 24u));
    target = r_u32(table + (fire ? 20u : 28u));
    if (fire && target == 0x80024598u)
        (void)apocalypse_beam_24598_native(receiver, position, angles);
    else if (!fire && target == 0x80024FDCu)
        sub_80024FDC(receiver);
    else
    {
        /* TODO Audit additional weapon callbacks before routing them */
        GameDiagnostic("Unsupported enemy weapon callback %08X\n", target);
        abort();
    }
}

uint32 Tank::FireDebris()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 child, first, second, value312, value314, value316, value318;
    FUNCTION_MARKER(0x80056B48u, "SLUS_003.73");
    child = sub_800625AC(328u);
    if (child)
    {
        first = sub_8004BDFC(object, 2u, 1u) & 65535u;
        second = sub_8004BDFC(object, 2u, 3u) & 65535u;
        value312 = sub_8004BDFC(object, 2u, 6u) & 65535u;
        value314 = sub_8004BDFC(object, 2u, 4u) & 65535u;
        value316 = sub_8004BDFC(object, 2u, 5u) & 65535u;
        value318 = sub_8004BDFC(object, 2u, 7u) & 65535u;
        child = sub_8005B5D8(child, object + 4u, first, second, value312, value314, value316, value318);
    }
    w_u32(child + 108u, 0xFFF80000u);
    return 0xFFF80000u;
}

uint32 Tank::Damage(uint32 damage, const sint32 *, uint32)
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 remaining, hero, flags, state;
    sint32 health;
    FUNCTION_MARKER(0x80056C50, "SLUS_003.73");
    w_u8(object + 383, r_u8(object + 383) + 1);
    health = (sint16)r_u16(object + 218);
    remaining = r_u16(object + 218) - damage;
    if (health <= 0)
        return 1;
    w_u16(object + 218, remaining);
    if ((sint32)(remaining << 16) > 0)
        SetColor(3, 255, 255, 255);
    else
    {
        hero = r_u32(0x800FF5A0);
        if (hero)
            TryPlayPlayerKillQuote((const uint32 *)psx_addr(hero + 4, 12));
        flags = r_u16(object + 78);
        state = r_u32(object + 396);
        w_u16(object + 78, flags & 0xFFEF);
        if (state & 32)
        {
            ClearTrackedObjectHealth();
            state = r_u32(object + 396);
            w_u32(object + 396, state & 0xFFFFFFDF);
        }
        w_u8(object + 380, 0);
        w_u16(object + 472, 10);
        w_u16(object + 474, 0);
    }
    return 1;
}

void Tank::ClearVelocity()
{
    const uint32 object = NativeObjectGuestAddress(this);
    w_u32(object + 112u, 0u);
    w_u32(object + 108u, 0u);
    w_u32(object + 104u, 0u);
}

void Tank::ClearAngularVelocity()
{
    const uint32 object = NativeObjectGuestAddress(this);
    w_u16(object + 136u, 0u);
    w_u16(object + 134u, 0u);
    w_u16(object + 132u, 0u);
    w_u16(object + 142u, 0u);
    w_u16(object + 140u, 0u);
    w_u16(object + 138u, 0u);
}

void Tank::FinishFromTable()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 table = r_u32(object + 68u);
    uint32 receiver = object + (uint32)(sint32)(sint16)r_u16(table + 16u);
    uint32 target = r_u32(table + 20u);
    if (target != 0x8004B8C8u)
    {
        /* TODO Audit additional virtual finish methods */
        GameDiagnostic("Unsupported enemy205 finish %08X\n", target);
        abort();
    }
    (void)sub_8004B8C8(receiver);
}

void Tank::SetState(uint32 state)
{
    const uint32 object = NativeObjectGuestAddress(this);
    w_u16(object + 472u, state);
    w_u16(object + 474u, 0u);
}

void Tank::InitParticle(uint32 object, const uint32 acceleration[3])
{
    apocalypse_impact_ring_construct_native(object, acceleration);
}

void Tank::UpdateState()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 state, step, hero, flags, value, ground, height, x, z, sound;
    uint32 child, table, target, receiver, position[3], acceleration[3], scale;
    uint16 angles[3];
    FUNCTION_MARKER(0x80056D1Cu, "SLUS_003.73");
    /* The selected parent 563B4 discards this register-carried return */
    state = r_u16(object + 472u);
    if (state > 10u || (state >= 7u && state <= 9u))
        return;
    step = r_u16(object + 474u);
    switch (state)
    {
    case 0u:
        if (step == 0u)
        {
            hero = r_u32(0x800FF5A0u);
            if (hero)
            {
                z = r_u32(hero + 12u);
                value = r_u32(object + 12u);
                x = r_u32(hero + 4u);
                height = r_u32(object + 4u);
                value = (uint32)ratan2((sint32)(z - value), (sint32)(x - height));
                w_u16(object + 18u, 3072u - value);
            }
            ground = sub_80067A18(object + 4u, 0u, 4096u);
            if (ground == 0xFFFFFFFFu)
            {
                FinishFromTable();
                return;
            }
            value = r_u32(object + 8u);
            height = (uint32)(sint32)(sint16)r_u16(object + 456u);
            if ((sint32)(ground - value) >> 12 < (sint32)(height << 1))
            {
                w_u32(object + 8u, ground - (height << 12));
                w_u16(object + 472u, 1u);
                w_u8(object + 382u, 0u);
                ClearVelocity();
            }
            else
            {
                w_u16(object + 460u, (uint32)((sint32)ground >> 13));
                flags = r_u16(object + 78u);
                w_u16(object + 474u, 1u);
                w_u8(object + 382u, 1u);
                w_u16(object + 78u, flags & 0xFFEFu);
            }
        }
        else if (step == 1u)
        {
            flags = r_u32(object + 396u);
            if (flags & 2u)
            {
                if (!(r_u16(object + 216u) & 2u))
                    return;
                sub_800774EC(r_u32(0x800FF904u), object + 4u, 2u);
                sub_80069EF4(1u, object + 4u, 0u);
                w_u8(object + 382u, 0u);
                w_u16(object + 474u, 2u);
            }
            else
            {
                ground = (uint32)(sint32)(sint16)r_u16(object + 460u) << 13;
                height = (uint32)(sint32)(sint16)r_u16(object + 456u) << 12;
                value = r_u32(object + 8u);
                if ((sint32)ground >= (sint32)(value + height))
                    return;
                sub_800774EC(r_u32(0x800FF904u), object + 4u, 2u);
                sub_80069EF4(1u, object + 4u, 0u);
                height = (uint32)(sint32)(sint16)r_u16(object + 456u) << 12;
                w_u8(object + 382u, 0u);
                w_u32(object + 8u, ground - height);
                ClearVelocity();
                w_u16(object + 474u, 2u);
            }
        }
        else if (step == 2u && r_u8(object + 303u))
        {
            flags = r_u16(object + 78u);
            SetState(1u);
            w_u16(object + 78u, flags | 16u);
        }
        return;
    case 1u:
        if (!step)
        {
            w_u8(object + 382u, 0u);
            ClearVelocity();
            ClearAngularVelocity();
            value = sub_80066570(4u);
            SetState(value + 2u);
        }
        return;
    case 2u:
        if (step)
            return;
        ClearTarget();
        value = r_u32(object + 220u);
        if (value - 1025u < 2047u)
        {
            if (r_u32(0x800FF5A0u))
            {
                sub_8004BDCC(object, 0u, 0u, 1u);
                hero = r_u32(0x800FF5A0u);
                SetTargetObject(hero);
            }
        }
        else
            sub_8004BDCC(object, 0u, 0u, 0u);
        if (r_u32(object + 220u) < 2048u)
        {
            if (r_u32(0x800FF5A0u))
            {
                sub_8004BDCC(object, 1u, 0u, 1u);
                hero = r_u32(0x800FF5A0u);
                SetTargetObject(hero);
            }
        }
        else
            sub_8004BDCC(object, 1u, 0u, 0u);
        SetState(1u);
        return;
    case 3u:
    case 5u:
        if (!step)
        {
            if (state == 3u)
            {
                height = r_u16(object + 392u);
                hero = r_u32(0x800FF5A0u);
                value = sub_8004BA64(object, object + 484u, hero + 4u,
                    128u, 192u, height, 1024u, 1u, 1u);
            }
            else
            {
                height = r_u16(object + 392u);
                value = sub_8004BA64(object, object + 484u, object + 4u,
                    1024u, 2048u, height, 4096u, 1u, 1u);
            }
            if (value)
            {
                SetDestination((const uint32 *)psx_addr(object + 484, 12));
                w_u16(object + 474u, 1u);
            }
            else
                SetState(1u);
        }
        else if (step == 1u)
        {
            if (!(r_u32(object + 396u) & 1u))
                SetState(step);
            else if (state == 3u && r_u32(0x800FF5A0u) && r_u32(object + 220u) < 1024u)
                SetState(2u);
        }
        return;
    case 4u:
        if (!step)
        {
            w_u16(object + 476u, 32u);
            SetState(1u);
        }
        return;
    case 6u:
        if (step)
            return;
        flags = r_u32(object + 396u);
        hero = r_u32(0x800FF5A0u);
        w_u32(object + 396u, flags & 0xFFFFFFFEu);
        ClearAngularVelocity();
        SetTargetObject(hero);
        w_u32(object + 112u, 0u);
        w_u32(object + 108u, 0u);
        value = r_u32(object + 220u);
        w_u32(object + 104u, 0u);
        if (value < 2049u)
        {
            sub_8004BDCC(object, 1u, 0u, 1u);
            sub_8004BDCC(object, 0u, 0u, 0u);
        }
        else
        {
            sub_8004BDCC(object, 0u, 0u, 1u);
            sub_8004BDCC(object, 1u, 0u, 0u);
        }
        return;
    case 10u:
        if (!step)
        {
            w_u16(object + 460u, 96u);
            ClearVelocity();
            w_u16(object + 136u, 0u);
            w_u16(object + 134u, 0u);
            w_u16(object + 132u, 0u);
            w_u16(object + 142u, 0u);
            w_u16(object + 140u, 0u);
            flags = r_u32(object + 396u);
            w_u16(object + 138u, 0u);
            w_u32(object + 396u, flags & 0xFFFFFFFEu);
            sub_8004BDCC(object, 0u, 0u, 0u);
            sub_8004BDCC(object, 1u, 0u, 0u);
            sub_8004BDCC(object, 2u, 0u, 0u);
            step = r_u16(object + 474u);
            w_u16(object + 474u, step + 1u);
        }
        else if (step == 1u)
        {
            if (!sub_80066570(4u) && !r_u32(0x800FF738u))
            {
                scale = (sint16)r_u16(object + 36u) == 2048 ? 370u : 768u;
                value = sub_80066570(1024u);
                angles[0] = (uint16)(0u - value);
                angles[1] = (uint16)sub_80066570(4096u);
                angles[2] = 0u;
                xport_draft_host_sub_800667CC_p13(position, scale, angles);
                xport_draft_host_sub_8006C0B8_p1(position, object + 4u);
                value = sub_80066570(60u);
                SpawnFireCycle(position, value + 60u, 0u, 2u);
                sound = sub_80066570(2u);
                sub_80069EF4(sound + 1u, object + 4u, 0u);
            }
            value = (uint32)(sint32)(sint16)r_u16(object + 460u) - 1u;
            w_u16(object + 460u, value);
            if (!(value << 16))
            {
                step = r_u16(object + 474u);
                w_u16(object + 474u, step + 1u);
            }
        }
        else if (step == 2u)
        {
            sub_8001D320(object + 4u, 800u, 240u, 160u, 40u, 5u, 0u, 100u);
            child = sub_80032DC0(120u);
            if (child)
            {
                acceleration[0] = r_u32(0x800A71CCu);
                acceleration[1] = r_u32(0x800A71D0u);
                acceleration[2] = r_u32(0x800A71D4u);
                InitParticle(child, acceleration);
                sub_80029198(child, object + 4u, 0u, 0u);
                w_u16(child + 10u, 32u);
                w_u16(child + 116u, 32u);
                w_u16(child + 118u, 16u);
            }
            position[0] = 0u;
            position[1] = 0xFFFEC000u;
            position[2] = 0u;
            child = sub_80032DC0(120u);
            if (child)
            {
                InitParticle(child, position);
                sub_80029198(child, object + 4u, 0u, 0u);
                w_u16(child + 10u, 32u);
                w_u16(child + 116u, 32u);
                w_u16(child + 118u, 16u);
            }
            if (r_u32(object + 480u))
            {
                flags = r_u16(object + 78u);
                table = r_u32(object + 68u);
                w_u16(object + 212u, 0u);
                w_u16(object + 78u, flags & 0xFFEFu);
                receiver = object + (uint32)(sint32)(sint16)r_u16(table + 56u);
                target = r_u32(table + 60u);
                if (target != 0x80063994u)
                {
                    /* TODO Audit additional virtual slot60 methods */
                    GameDiagnostic("Unsupported enemy205 slot60 %08X receiver %08X\n", target, receiver);
                    abort();
                }
                sub_8004B948(object);
                step = r_u16(object + 474u);
                value = r_u32(0x800FF62Cu);
                w_u16(object + 474u, step + 1u);
                sub_800626C8(object, value);
                value = r_u32(object + 480u);
                height = r_u8(0x800FF644u);
                value = sub_8006E080(value, height);
                flags = r_u16(object);
                w_u16(object + 22u, value);
                w_u8(object + 26u, 0u);
                w_u8(object + 24u, 0u);
                w_u16(object, flags & 0xFFF9u);
            }
            else
                FragmentAndFinish();
        }
        return;
    }
}

uint32 Tank::SetMode(uint32 mode)
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 flags, result;
    FUNCTION_MARKER(0x800576B0u, "SLUS_003.73");
    mode &= 0xFFFFu;
    w_u16(object + 390u, mode);
    if (mode != 0u && mode != 1u && mode != 16u)
        return 16u;
    sub_800626C8(object, 0x800FF564u);
    w_u16(object + 456u, 0u);
    w_u16(object + 218u, mode == 0u ? 320u : (mode == 1u ? 400u : 1000u));
    sub_8007CA50(object, 0x800A6D5Cu);
    sub_8007CAC8(object, 0x800A6D28u);
    if (mode == 0u)
    {
        flags = r_u16(object);
        w_u16(object + 36u, 2048u);
        w_u16(object, flags | 0x200u);
        w_u16(object + 38u, 2048u);
        w_u16(object + 40u, 2048u);
        w_u16(object + 436u, 32u);
        result = 64u;
    }
    else
    {
        w_u16(object + 436u, 16u);
        result = 128u;
    }
    w_u16(object + 438u, result);
    w_u16(object + 440u, result);
    if (mode == 16u)
    {
        sub_8006D390(object, 64u, 32u, 2048u);
        flags = r_u32(object + 396u);
        w_u32(0x800FF4ECu, object);
        result = flags | 32u;
        w_u32(object + 396u, result);
    }
    return result;
}

uint32 Tank::ExecuteCommand(uint32 command)
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 cursor, value, table, receiver, target, position[3], shift;
    FUNCTION_MARKER(0x80057840u, "SLUS_003.73");
    command &= 0xFFFFu;
    switch (command)
    {
        case 0x4206u:
            cursor = r_u32(object + 400u);
            value = r_u16(cursor);
            w_u32(object + 400u, cursor + 2u);
            SetMode(value);
            return 1u;
        case 0x4406u:
            cursor = r_u32(object + 400u);
            value = r_u16(cursor);
            w_u32(object + 400u, cursor + 2u);
            if (value)
                SetTargetObject(r_u32(0x800FF5A0u));
            else
                ClearTarget();
            return 1u;
        case 0x4407u:
            cursor = r_u32(object + 400u);
            value = r_u16(cursor);
            w_u32(object + 400u, cursor + 2u);
            if (!value)
            {
                ClearTarget();
                return 1u;
            }
            if (value & 0x2000u)
            {
                table = r_u32(object + 68u);
                receiver = object + (uint32)(sint32)(sint16)r_u16(table + 80u);
                target = r_u32(table + 84u);
                value = DispatchObjectParameterGetter(target, receiver, value);
            }
            SetTargetRecord(value & 0xFFFFu);
            return 1u;
        case 0x4220u:
        case 0x4222u:
            cursor = (r_u32(object + 400u) + 3u) & ~3u;
            position[0] = r_u32(cursor);
            position[1] = r_u32(cursor + 4u);
            position[2] = r_u32(cursor + 8u);
            shift = 12u;
            ShiftVectorLeft(position, &shift);
            if (command == 0x4222u)
                AddVectors(position, (const uint32 *)psx_addr(object + 4, 12));
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
            w_u32(object + 400u, cursor + 2u);
            return 1u;
        default:
            return Enemy::ExecuteCommand(command);
    }
}

uint32 Tank::SetDestination(const uint32 *position)
{
    const uint32 guest_position = GuestMemoryAddress(position);
    const uint32 *native_position = guest_position ? 0 : position;
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 x, y, z, flags;
    FUNCTION_MARKER(0x80057AE0, "SLUS_003.73");
    if (native_position)
    {
        x = native_position[0];
        y = native_position[1];
        z = native_position[2];
    }
    else
    {
        x = r_u32(guest_position);
        y = r_u32(guest_position + 4);
        z = r_u32(guest_position + 8);
    }
    w_u32(object + 484, x);
    w_u32(object + 488, y);
    w_u32(object + 492, z);
    flags = r_u32(object + 396) | 1;
    w_u32(object + 396, flags);
    return flags;
}

uint32 Tank::Aim(uint32 enabled)
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 node, packed, tail, scale, desired_word, result, axis;
    sint32 origin[3];
    sint16 desired[3];
    FUNCTION_MARKER(0x80057B48u, "SLUS_003.73");
    for (axis = 0u; axis < 2u; ++axis)
    {
        xport_draft_host_sub_8007CC10_p1(origin, object, axis);
        desired[0] = 0;
        desired[1] = 0;
        /* The original stale roll carrier is unused by 66CF0 */
        desired[2] = 0;
        if (enabled & (1u << axis))
        {
            xport_draft_host_sub_80066B8C_p12(desired, origin, object + 500u);
            desired[1] = (sint16)(((uint16)desired[1] - r_u16(object + 18u)) & 0xFFFu);
        }
        node = object + (axis ? 542u : 524u);
        packed = apocalypse_angle_pair_word(node);
        tail = r_u16(node + 4u);
        scale = r_u16(object + 438u + axis * 2u);
        desired_word = (uint16)desired[0] | ((uint32)(uint16)desired[1] << 16);
        result = sub_80066CF0(packed, tail, node + 6u, node + 12u,
                             desired_word, (uint16)desired[2], scale);
    }
    return result;
}

uint32 Tank::SetTargetRecord(uint32 index)
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 flags;
    FUNCTION_MARKER(0x80057CCCu, "SLUS_003.73");
    sub_8006613C(object + 500u, index & 0xFFFFu);
    flags = r_u32(object + 396u) | 4u;
    w_u32(object + 396u, flags);
    return flags;
}

uint32 Tank::SetTargetObject(uint32 target)
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 result, flags;
    FUNCTION_MARKER(0x80057D38u, "SLUS_003.73");
    result = sub_80062F48(target);
    if (!result)
    {
        flags = r_u32(object + 396u);
        w_u32(object + 568u, target);
        result = flags | 16u;
        w_u32(object + 396u, result);
    }
    return result;
}

uint32 Tank::ClearTarget()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 flags;
    FUNCTION_MARKER(0x80057D84u, "SLUS_003.73");
    flags = r_u32(object + 396u) & 0xFFFFFFE3u;
    w_u32(object + 396u, flags);
    return flags;
}

uint32 Tank::FragmentAndFinish()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 angle, index, target, random, table, divisor, child, kind, height, name;
    uint32 position[3], velocity[3], receiver;
    FUNCTION_MARKER(0x80057D98u, "SLUS_003.73");
    angle = sub_80066570(1024u);
    for (index = 0u; index < 12u; ++index)
    {
        target = r_u32(0x800FF904u);
        if (target && !sub_80066570(8u))
        {
            target = r_u32(0x800FF904u);
            position[0] = r_u32(target + 4u);
            position[1] = r_u32(target + 8u);
            position[2] = r_u32(target + 12u);
            position[1] += 0x80000u;
            divisor = xport_draft_host_sub_8006696C_p2(object + 4u, position) >> 7;
            velocity[0] = TraceDivide(position[0] - r_u32(object + 4u), divisor);
            velocity[2] = TraceDivide(position[2] - r_u32(object + 12u), divisor);
            velocity[1] = TraceDivide(position[1] - r_u32(object + 8u), divisor) - (divisor << 13);
        }
        else
        {
            random = sub_80066570(16u);
            table = 0x800F863Cu + 4u * (angle & 0xFFFu);
            velocity[0] = (uint32)(sint32)(sint16)r_u16(table) * (random + 24u);
            random = sub_80066570(32u);
            velocity[1] = (0u - 48u - random) << 12;
            random = sub_80066570(16u);
            velocity[2] = (uint32)(sint32)(sint16)r_u16(table + 2u) * (random + 24u);
        }
        child = sub_800625AC(320u);
        if (child)
        {
            random = sub_80066570(r_u8(object + 561u));
            kind = r_u8(object + 560u);
            height = (uint32)(sint32)(sint16)r_u16(object + 456u) << 12;
            height += r_u32(object + 8u);
            name = r_u32(0x800FF62Cu);
            (void)apocalypse_debris_construct_5BBB0(child, object + 4u, velocity,
                (kind + random) & 0xFFFFu, height, 5u, 36u, name);
        }
        angle += 682u;
    }
    table = r_u32(object + 68u);
    receiver = object + (uint32)(sint32)(sint16)r_u16(table + 16u);
    target = r_u32(table + 20u);
    if (target == 0x8004B8C8u)
        return sub_8004B8C8(receiver);
    /* TODO Audit additional virtual finish targets before routing them */
    GameDiagnostic("Unsupported enemy fragment finish %08X\n", target);
    abort();
    return 0u;
}

uint32 Tank::TraceDivide(uint32 numerator, uint32 denominator)
{
    if (!denominator)
        return (sint32)numerator < 0 ? 1u : 0xFFFFFFFFu;
    if (numerator == 0x80000000u && denominator == 0xFFFFFFFFu)
        return numerator;
    return (uint32)((sint32)numerator / (sint32)denominator);
}

Tank &Tank::Attach(uint32 object)
{
    return *static_cast<Tank *>(FindNativeObjectStorage(object));
}

uint32 Tank::Spawn(uint32 object, uint32 position, uint32 script)
{
    void *native_instance_storage = AllocateNativeObjectStorage(object, sizeof(Tank));
    new (native_instance_storage) Tank((const uint32 *)psx_addr(position, 12), script);
    return object;
}

// Compatibility entry points retain the guest call contract
uint32 sub_80055E90(uint32 object, uint32 index)
{
    FUNCTION_MARKER(0x80055E90, "SLUS_003.73");
    return SpawnTank(object, index);
}

uint32 sub_80055F14(uint32 object, uint32 data, uint32 index)
{
    FUNCTION_MARKER(0x80055F14, "SLUS_003.73");
    void *native_instance_storage = AllocateNativeObjectStorage(object, sizeof(Tank));
    new (native_instance_storage) Tank((const uint8 *)psx_addr(data, 1), index);
    return object;
}

uint32 sub_80055F9C(uint32 object, uint32 position, uint32 script)
{
    FUNCTION_MARKER(0x80055F9C, "SLUS_003.73");
    return Tank::Spawn(object, position, script);
}

uint32 sub_80056038(uint32 object)
{
    FUNCTION_MARKER(0x80056038, "SLUS_003.73");
    return Tank::Attach(object).Init();
}

uint32 sub_800562C4(uint32 object, uint32 reason)
{
    FUNCTION_MARKER(0x800562C4, "SLUS_003.73");
    return Tank::Attach(object).Tank::Destroy(reason);
}

uint32 sub_800563B4(uint32 object)
{
    FUNCTION_MARKER(0x800563B4, "SLUS_003.73");
    return Tank::Attach(object).Tank::Update();
}

uint32 sub_80056980(uint32 object)
{
    FUNCTION_MARKER(0x80056980, "SLUS_003.73");
    return Tank::Attach(object).FireProjectile();
}

void sub_80056A6C(uint32 object, uint32 fire)
{
    FUNCTION_MARKER(0x80056A6C, "SLUS_003.73");
    Tank::Attach(object).FireBeam(fire);
}

uint32 sub_80056B48(uint32 object)
{
    FUNCTION_MARKER(0x80056B48, "SLUS_003.73");
    return Tank::Attach(object).FireDebris();
}

uint32 sub_80056C50(uint32 object, uint32 damage)
{
    FUNCTION_MARKER(0x80056C50, "SLUS_003.73");
    return Tank::Attach(object).Tank::Damage(damage, 0, 0);
}

void sub_80056D1C(uint32 object)
{
    FUNCTION_MARKER(0x80056D1C, "SLUS_003.73");
    Tank::Attach(object).UpdateState();
}

uint32 sub_800576B0(uint32 object, uint32 mode)
{
    FUNCTION_MARKER(0x800576B0, "SLUS_003.73");
    return Tank::Attach(object).SetMode(mode);
}

uint32 sub_80057840(uint32 object, uint32 command)
{
    FUNCTION_MARKER(0x80057840, "SLUS_003.73");
    return Tank::Attach(object).Tank::ExecuteCommand(command);
}

uint32 sub_80057AE0(uint32 object, uint32 position)
{
    FUNCTION_MARKER(0x80057AE0, "SLUS_003.73");
    return Tank::Attach(object).SetDestination((const uint32 *)psx_addr(position, 12));
}

uint32 sub_80057B48(uint32 object, uint32 enabled)
{
    FUNCTION_MARKER(0x80057B48, "SLUS_003.73");
    return Tank::Attach(object).Aim(enabled);
}

uint32 sub_80057CCC(uint32 object, uint32 index)
{
    FUNCTION_MARKER(0x80057CCC, "SLUS_003.73");
    return Tank::Attach(object).SetTargetRecord(index);
}

uint32 sub_80057D38(uint32 object, uint32 target)
{
    FUNCTION_MARKER(0x80057D38, "SLUS_003.73");
    return Tank::Attach(object).SetTargetObject(target);
}

uint32 sub_80057D84(uint32 object)
{
    FUNCTION_MARKER(0x80057D84, "SLUS_003.73");
    return Tank::Attach(object).ClearTarget();
}

uint32 sub_80057D98(uint32 object)
{
    FUNCTION_MARKER(0x80057D98, "SLUS_003.73");
    return Tank::Attach(object).FragmentAndFinish();
}

