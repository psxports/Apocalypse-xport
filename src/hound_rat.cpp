#include "game_sdk_bindings.h"
#include "player.h"
#include "audio_session.h"
extern "C" {
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
}
#include "hound_rat.h"
#include "object_parameters.h"
#include "record_data.h"
#include "game_random.h"
#include "native_object_storage.h"
#include "game_object_allocation.h"
#include <new>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

HoundRat::HoundRat(const uint8 *record_data, uint32 index, uint32 mode) : Enemy()
{
    const uint32 data = GuestMemoryAddress(record_data);
    uint32 object = NativeObjectGuestAddress(this);
    uint32 flags, cursor;
    FUNCTION_MARKER(0x800737A0, "SLUS_003.73");
    if (!data)
    {
        // TODO Support native record data in the original parser adapter
        GameDiagnostic("HoundRat record data is outside guest memory\n");
        abort();
    }
    w_u32(object + 68, 0x800A3C30);
    if (mode)
    {
        w_u16(object + 58, 208);
        sub_800626C8(object, 0x800FF88C);
        w_u16(object + 40, 6144);
        w_u16(object + 38, 6144);
        w_u16(object + 36, 6144);
        w_u16(object + 456, 52);
        w_u16(object + 458, 32);
        flags = r_u16(object);
        w_u16(object + 218, 60);
        w_u16(object, flags | 0x200);
        sub_8004BD04(object, 1, 2);
        sub_8004BDCC(object, 0, 1, 10);
    }
    else
    {
        w_u16(object + 58, 210);
        sub_800626C8(object, 0x800FF894);
        w_u16(object + 456, 64);
        w_u16(object + 458, 48);
        w_u16(object + 218, 40);
        sub_8004BD04(object, 1, 2);
        sub_8004BDCC(object, 0, 1, 5);
    }
    cursor = sub_80062B0C(object, data);
    cursor = sub_80062B50(object, cursor);
    sub_80062A38(object, 0x800FF4E8);
    w_u32(object + 400, cursor);
    w_u16(object + 214, index);
    w_u16(object + 472, 1);
    Init();
    cursor = r_u32(object + 400);
    if (cursor)
    {
        w_u8(object + 380, 1);
        sub_8004BE90(object, cursor);
    }
}

uint32 HoundRat::Init()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 total, count, ground, height;
    FUNCTION_MARKER(0x80073920u, "SLUS_003.73");
    w_u32(object + 304u, 0x18000u);
    total = r_u32(0x800FF308u);
    count = r_u32(0x800FF884u);
    w_u32(0x800FF308u, total + 1u);
    w_u32(0x800FF884u, count + 1u);
    StartAnimation(0u, 1u);
    w_u16(object + 436u, 108u);
    w_u16(object + 438u, 80u);
    ground = sub_80067A18(object + 4u, 512u, 512u);
    if (ground != 0xFFFFFFFFu)
    {
        height = (uint32)(sint32)(sint16)r_u16(object + 456u);
        w_u32(object + 8u, ground - (height << 12));
    }
    w_u8(object + 380u, 1u);
    return 1u;
}

uint32 HoundRat::Destroy(uint32 reason)
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 count, result;
    FUNCTION_MARKER(0x800739C4, "SLUS_003.73");
    count = r_u32(0x800FF884);
    w_u32(object + 68, 0x800A3C30);
    w_u32(0x800FF884, count - 1);
    RemoveFromList((uint32 *)psx_addr(0x800FF4E8, 4));
    Enemy::FreeBehaviorTable();
    result = Enemy::Destroy(0);
    if (reason & 1)
        return ReleaseObjectStorage(object);
    return result;
}

uint32 HoundRat::Update()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 constants[3], difference[3], normal[3];
    uint32 value, flags, left, right, x, y, z, scale, angle, word, tail;
    FUNCTION_MARKER(0x80073A3Cu, "SLUS_003.73");
    if (r_u16(object + 58u) == 208u && !sub_80066570(240u))
        sub_80069EF4(112u, object + 4u, 0u);
    constants[0] = r_u32(0x800A71CCu);
    constants[1] = r_u32(0x800A71D0u);
    constants[2] = r_u32(0x800A71D4u);
    w_u32(object + 116u, constants[0]);
    w_u32(object + 120u, constants[1]);
    w_u32(object + 124u, constants[2]);
    sub_8007FDF0(object);
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
    if (r_u8(object + 382u))
        sub_8006C0B8(object + 116u, 0x800A6844u);
    sub_8004B9A4(object);
    if (!(r_u32(object + 396u) & 1u))
        return 0u;
    value = sub_8006696C(object + 4u, object + 484u);
    if ((sint32)value < 129)
    {
        flags = r_u32(object + 396u);
        w_u32(object + 396u, flags & 0xFFFFFFFEu);
        return 0u;
    }
    left = r_u32(object + 484u);
    right = r_u32(object + 4u);
    difference[0] = (uint32)((sint32)(left - right) >> 12);
    left = r_u32(object + 488u);
    right = r_u32(object + 8u);
    difference[1] = (uint32)((sint32)(left - right) >> 12);
    left = r_u32(object + 492u);
    right = r_u32(object + 12u);
    difference[2] = (uint32)((sint32)(left - right) >> 12);
    xport_draft_host_sub_800872BC_p12(difference, normal);
    scale = r_u16(object + 436u);
    x = normal[0] * scale;
    y = normal[1] * scale;
    z = normal[2] * scale;
    w_u32(object + 104u, x);
    w_u32(object + 108u, y);
    w_u32(object + 112u, z);
    left = r_u32(object + 492u);
    right = r_u32(object + 12u);
    z = left - right;
    left = r_u32(object + 484u);
    right = r_u32(object + 4u);
    angle = (3072u - (uint32)ratan2((sint32)z, (sint32)(left - right))) & 4095u;
    word = apocalypse_angle_pair_word(object + 16u);
    tail = r_u16(object + 20u);
    scale = r_u16(object + 438u);
    sub_80066CF0(word, tail, object + 132u, object + 138u, angle << 16, 0u, scale);
    // The legacy behavior caller discards the original return carrier
    return 0u;
}

uint32 HoundRat::ExecuteCommand(uint32 command)
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 cursor, value, table, offset, target, ground, height;
    uint32 position[4];
    FUNCTION_MARKER(0x80073C78, "SLUS_003.73");
    command &= 0xFFFF;
    if (command != 0x4221)
        return Enemy::ExecuteCommand(command);
    cursor = r_u32(object + 400);
    value = r_u16(cursor);
    w_u32(object + 400, cursor + 2);
    if (value & 0x2000)
    {
        table = r_u32(object + 68);
        offset = (uint32)(sint32)(sint16)r_u16(table + 80);
        target = r_u32(table + 84);
        value = DispatchObjectParameterGetter(target, object + offset, value) & 0xFFFF;
    }
    ReadRecordPosition(position, value);
    ground = apocalypse_ground_native(position, 0, 2048);
    if (ground != (uint32)-1)
    {
        height = (uint32)(sint32)(sint16)r_u16(object + 456);
        position[1] = ground - (height << 12);
    }
    HoundRat::SetTarget(position);
    return 1;
}

void HoundRat::FinishFromTable()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 table = r_u32(object + 68u);
    uint32 offset = (uint32)(sint32)(sint16)r_u16(table + 16u);
    uint32 target = r_u32(table + 20u);
    apocalypse_object_virtual20(target, object + offset);
}

void HoundRat::Orbit(uint32 point[3], uint32 angle, uint32 reload_angle)
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 radius, table, sine, cosine, player;
    if (reload_angle)
        angle = r_u16(object + 464u);
    radius = (uint32)(sint32)(sint16)r_u16(object + 460u);
    table = 0x800F863Cu + 4u * (angle & 4095u);
    sine = (uint32)(sint32)(sint16)r_u16(table);
    cosine = (uint32)(sint32)(sint16)r_u16(table + 2u);
    player = r_u32(0x800FF5A0u);
    point[0] = radius * sine;
    point[1] = 0u;
    point[2] = radius * cosine;
    xport_draft_host_sub_8006C0B8_p1(point, player + 4u);
}

void HoundRat::UpdateState()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 player, distance, damage, table, offset, target, state, phase;
    uint32 flags, value, left, right, angle, word, tail, speed, direction;
    uint32 candidate[3], point[3], difference[3], collision[35], divisor;
    uint32 count, child, red, green, blue;
    sint16 angles[3];
    FUNCTION_MARKER(0x80073D3Cu, "SLUS_003.73");
    player = r_u32(0x800FF5A0u);
    if (!player)
        w_u16(object + 472u, 666u);
    distance = r_u32(object + 220u);
    if (distance < 129u)
    {
        damage = sub_8004BDFC(object, 0u, 1u) & 0xFFFFu;
        player = r_u32(0x800FF5A0u);
        table = r_u32(player + 68u);
        offset = (uint32)(sint32)(sint16)r_u16(table + 48u);
        target = r_u32(table + 52u);
        apocalypse_object_virtual52(target, player + offset, damage, 0x800A71CCu, 0u);
    }
    state = r_u16(object + 472u);
    switch (state)
    {
        case 0u:
            value = r_u8(object + 26u);
            if (value)
            {
                if (value != 1u || r_u8(object + 303u))
                {
                    StartAnimation(0u, 1u);
                    value = sub_80066570(32u);
                    w_u8(object + 24u, value);
                }
            }
            else if (!sub_80066570(512u))
                SetAnimationRange(1u, 0u, 0xFFFFFFFFu);
            flags = r_u32(object + 396u);
            player = r_u32(0x800FF5A0u);
            right = r_u32(object + 12u);
            w_u32(object + 396u, flags & 0xFFFFFFFEu);
            left = r_u32(player + 12u);
            direction = left - right;
            left = r_u32(player + 4u);
            right = r_u32(object + 4u);
            angle = (3072u - (uint32)ratan2((sint32)direction, (sint32)(left - right))) & 4095u;
            word = apocalypse_angle_pair_word(object + 16u);
            tail = r_u16(object + 20u);
            speed = r_u16(object + 438u);
            sub_80066CF0(word, tail, object + 132u, object + 138u, angle << 16, 0u, speed);
            if (!sub_80066570(16u))
                w_u16(object + 472u, 1u);
            return;
        case 1u:
            if (r_u32(object + 220u) < 513u)
            {
                player = r_u32(0x800FF5A0u);
                value = r_u16(object + 220u);
                w_u16(object + 460u, value);
                xport_draft_host_sub_80066B8C_p1(angles, player + 4u, object + 4u);
                w_u16(object + 472u, 3u);
                flags = r_u32(object + 396u);
                w_u16(object + 474u, 0u);
                w_u16(object + 462u, (uint16)angles[1]);
                w_u32(object + 396u, flags & 0xFFFFFFFEu);
                return;
            }
            if (r_u32(object + 396u) & 1u)
                return;
            player = r_u32(0x800FF5A0u);
            if (player)
            {
                point[0] = r_u32(player + 4u);
                point[1] = r_u32(player + 8u);
                point[2] = r_u32(player + 12u);
                if (apocalypse_candidate_vectors_native(object, candidate, point,
                    128u, 192u, 64u, 1024u, 1u, 1u))
                {
                    SetTarget(candidate);
                    return;
                }
            }
            w_u16(object + 472u, 0u);
            return;
        case 2u:
            if (r_u8(object + 26u) == 3u)
            {
                if (r_u8(object + 303u))
                    FinishFromTable();
                return;
            }
            flags = r_u32(object + 396u);
            w_u32(object + 396u, flags & 0xFFFFFFFEu);
            w_u32(object + 112u, 0u);
            w_u32(object + 108u, 0u);
            w_u32(object + 104u, 0u);
            w_u32(object + 304u, 0x18000u);
            SetAnimationRange(3u, 0u, 0xFFFFFFFFu);
            return;
        case 3u:
            phase = r_u16(object + 474u);
            if (!phase)
            {
                if (!sub_80066570(32u))
                    w_u16(object + 466u, sub_80066570(2u) ? 0u : 1u);
                if (!sub_80066570(16u))
                {
                    value = (uint32)(sint32)(sint16)r_u16(object + 460u);
                    value = value - 128u + sub_80066570(256u);
                    if ((sint32)value > 1024)
                        value = 1024u;
                    if ((sint32)value < 400)
                        value = 400u;
                    w_u16(object + 460u, value);
                }
                direction = (uint32)(sint32)(sint16)r_u16(object + 466u);
                value = (uint32)(sint32)(sint16)r_u16(object + 464u);
                angle = sub_80066570(128u) + 256u;
                value = (direction ? value - angle : value + angle) & 4095u;
                w_u16(object + 464u, value);
                Orbit(point, value, 1u);
                if (!apocalypse_candidate_vectors_native(object, candidate, point,
                    0u, 0u, 64u, 32u, 1u, 1u))
                {
                    w_u16(object + 472u, 0u);
                    return;
                }
                SetTarget(candidate);
                value = r_u16(object + 474u);
                w_u16(object + 474u, value + 1u);
                return;
            }
            if (phase != 1u || (r_u32(object + 396u) & 1u))
                return;
            if (!r_u32(object + 304u))
                w_u32(object + 304u, 0x18000u);
            value = (uint32)(sint32)(sint16)r_u16(object + 462u) + 1u;
            w_u16(object + 462u, value);
            if ((sint16)value < 9 || sub_80066570(8u))
            {
                w_u16(object + 474u, 0u);
                return;
            }
            value = ((uint32)(sint32)(sint16)r_u16(object + 464u) + 2048u) & 4095u;
            w_u16(object + 464u, value);
            Orbit(point, value, 0u);
            if (!apocalypse_candidate_vectors_native(object, candidate, point,
                0u, 0u, 64u, 32u, phase, phase))
            {
                w_u16(object + 474u, 0u);
                return;
            }
            value = r_u16(object + 58u);
            sub_80069EF4(value == 208u ? 111u : 54u, object + 4u, 0u);
            SetTarget(candidate);
            w_u16(object + 462u, 0u);
            w_u32(object + 304u, 0u);
            w_u8(object + 24u, 7u);
            return;
        case 10u:
            phase = r_u16(object + 474u);
            if (phase == 2u)
            {
                if (r_u8(object + 303u))
                    FinishFromTable();
                return;
            }
            if (phase > 2u)
                return;
            if (phase == 1u)
            {
                if ((sint16)r_u16(object + 460u))
                {
                    sub_80034F9C(object + 16u);
                    sub_800350E8(2u);
                    red = r_u8(0x800ECC94u);
                    green = r_u8(0x800ECC96u);
                    blue = r_u8(0x800ECC98u);
                    sub_800350FC(red, green, blue);
                    red = r_u16(0x800ECC94u);
                    green = r_u16(0x800ECC96u);
                    blue = r_u16(0x800ECC98u);
                    sub_80035110((red >> 5) & 255u, (green >> 5) & 255u, (blue >> 5) & 255u);
                    w_u32(0x800FF3ACu, 0u);
                    for (count = 0u; count < 16u; ++count)
                    {
                        child = sub_80032DC0(88u);
                        if (child)
                            sub_80035124(child, object + 4u, 16u, 0x2000u, 16u);
                    }
                    value = (uint32)(sint32)(sint16)r_u16(object + 460u);
                    w_u32(0x800FF3ACu, 1u);
                    w_u16(object + 460u, value - 1u);
                    return;
                }
                w_u32(object + 112u, 0u);
                w_u32(object + 108u, 0u);
                value = r_u16(object + 474u);
                w_u32(object + 104u, 0u);
                w_u16(object + 474u, value + 1u);
                return;
            }
            w_u16(object + 16u, 0u);
            w_u16(object + 136u, 0u);
            w_u16(object + 134u, 0u);
            w_u16(object + 132u, 0u);
            w_u16(object + 142u, 0u);
            w_u16(object + 140u, 0u);
            flags = r_u32(object + 396u);
            w_u16(object + 138u, 0u);
            w_u32(object + 396u, flags & 0xFFFFFFFEu);
            sub_80062D84(object);
            sub_800667CC(object + 484u, 1024u, object + 16u);
            xport_draft_host_sub_8006C3AC_p1(point, object + 4u, object + 484u);
            w_u32(object + 484u, point[0]);
            w_u32(object + 488u, point[1]);
            w_u32(object + 492u, point[2]);
            collision[0] = r_u32(object + 4u);
            collision[1] = r_u32(object + 8u);
            collision[2] = r_u32(object + 12u);
            collision[3] = r_u32(object + 484u);
            collision[4] = r_u32(object + 488u);
            collision[5] = r_u32(object + 492u);
            xport_draft_host_sub_8007BB24_p1(collision);
            xport_draft_host_sub_8007DD04_p1(collision, 1u);
            if (collision[26])
            {
                value = collision[16] - 256u;
                if ((sint32)value < 64)
                {
                    w_u32(object + 112u, 0u);
                    w_u32(object + 108u, 0u);
                    w_u32(object + 104u, 0u);
                    w_u16(object + 460u, 1u);
                    goto death_motion;
                }
                divisor = (uint32)((sint32)(value + ((sint32)value < 0 ? 63u : 0u)) >> 6);
                w_u16(object + 460u, divisor);
                sub_800667CC(object + 484u, value, object + 16u);
                xport_draft_host_sub_8006C3AC_p1(point, object + 4u, object + 484u);
                w_u32(object + 484u, point[0]);
                w_u32(object + 488u, point[1]);
                w_u32(object + 492u, point[2]);
                xport_draft_host_sub_8006C3AC_p1(difference, object + 484u, object + 4u);
                divisor = (uint32)(sint32)(sint16)r_u16(object + 460u);
            }
            else
            {
                w_u16(object + 460u, 16u);
                xport_draft_host_sub_8006C3AC_p1(difference, object + 484u, object + 4u);
                divisor = 16u;
            }
            xport_draft_host_sub_8006C4EC_p123(point, difference, &divisor);
            w_u32(object + 104u, point[0]);
            w_u32(object + 108u, point[1]);
            w_u32(object + 112u, point[2]);
death_motion:
            w_u32(object + 108u, 0xFFFC0000u);
            w_u8(object + 382u, 1u);
            w_u8(object + 129u, 31u);
            w_u8(object + 130u, 3u);
            w_u8(object + 131u, 31u);
            SetAnimationRange(3u, 0u, 0xFFFFFFFFu);
            w_u16(object + 212u, 0u);
            sub_8004B948(object);
            value = r_u16(object + 474u);
            w_u16(object + 474u, value + 1u);
            return;
        case 666u:
            if (r_u8(object + 26u) != 1u)
            {
                StartAnimation(1u, 1u);
                value = sub_80066570(30u);
                w_u8(object + 24u, value);
            }
            flags = r_u32(object + 396u);
            w_u16(object + 472u, 0u);
            w_u32(object + 396u, flags & 0xFFFFFFFEu);
            return;
        default:
            return;
    }
}

uint32 HoundRat::SetTarget(const uint32 *target)
{
    const uint32 guest_target = GuestMemoryAddress(target);
    const uint32 *native_target = guest_target ? 0 : target;
    FUNCTION_MARKER(0x80074730, "SLUS_003.73");
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 x, y, z, value;
    if (r_u8(object + 26) != 2)
    {
        StartAnimation(2, 1);
        value = RandomRange(16);
        w_u8(object + 24, value);
    }
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
    value = r_u32(object + 396) | 1;
    w_u32(object + 396, value);
    return value;
}

uint32 HoundRat::Damage(uint32 damage, const sint32 *, uint32 kind)
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 tick, flags, health, value, player, type;
    FUNCTION_MARKER(0x800747A8, "SLUS_003.73");
    tick = r_u8(object + 383);
    w_u8(object + 383, tick + 1);
    if (kind == 5)
    {
        flags = r_u32(object + 396);
        w_u32(object + 396, flags | 2);
    }
    health = (uint32)(sint32)(sint16)r_u16(object + 218);
    value = r_u16(object + 218);
    if ((sint32)health <= 0)
        return 1;
    value -= damage;
    w_u16(object + 218, value);
    if ((sint32)(value << 16) > 0)
    {
        SetColor(3, 255, 255, 255);
        return 1;
    }
    player = r_u32(0x800FF5A0);
    if (player)
    {
        // The original reads the kind although the quote callee ignores it
        (void)r_u16(object + 58);
        TryPlayPlayerKillQuote((const uint32 *)psx_addr(player + 4, 12));
    }
    flags = r_u16(object + 78);
    type = r_u16(object + 58);
    w_u16(object + 78, flags & 0xFFEF);
    PlayPositionedSound(type == 208 ? 110 : 53, (const uint32 *)psx_addr(object + 4, 12), 0);
    w_u16(object + 472, 10);
    w_u16(object + 474, 0);
    return 1;
}

HoundRat &HoundRat::Attach(uint32 object)
{
    return *static_cast<HoundRat *>(FindNativeObjectStorage(object));
}

// Compatibility entry points retain the guest call contract
uint32 sub_800737A0(uint32 object, uint32 data, uint32 index, uint32 mode)
{
    FUNCTION_MARKER(0x800737A0, "SLUS_003.73");
    void *native_instance_storage = AllocateNativeObjectStorage(object, sizeof(HoundRat));
    new (native_instance_storage) HoundRat((const uint8 *)psx_addr(data, 1), index, mode);
    return object;
}

uint32 sub_800739C4(uint32 object, uint32 reason)
{
    FUNCTION_MARKER(0x800739C4, "SLUS_003.73");
    return HoundRat::Attach(object).HoundRat::Destroy(reason);
}

void sub_80073A3C(uint32 object)
{
    FUNCTION_MARKER(0x80073A3C, "SLUS_003.73");
    HoundRat::Attach(object).HoundRat::Update();
}

uint32 sub_80073C78(uint32 object, uint32 command)
{
    FUNCTION_MARKER(0x80073C78, "SLUS_003.73");
    return HoundRat::Attach(object).HoundRat::ExecuteCommand(command);
}

void sub_80073D3C(uint32 object)
{
    FUNCTION_MARKER(0x80073D3C, "SLUS_003.73");
    HoundRat::Attach(object).UpdateState();
}

uint32 sub_80074730(uint32 object, uint32 target)
{
    FUNCTION_MARKER(0x80074730, "SLUS_003.73");
    return HoundRat::Attach(object).SetTarget((const uint32 *)psx_addr(target, 12));
}

uint32 apocalypse_record208_target_native(uint32 object, const uint32 target[3])
{
    return HoundRat::Attach(object).SetTarget(target);
}

uint32 sub_800747A8(uint32 object, uint32 damage, uint32, uint32 kind)
{
    FUNCTION_MARKER(0x800747A8, "SLUS_003.73");
    return HoundRat::Attach(object).HoundRat::Damage(damage, 0, kind);
}
