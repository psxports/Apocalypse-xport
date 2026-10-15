#include "game_sdk_bindings.h"
#include "player.h"
#include "audio_session.h"
#include "vector_arithmetic.h"
extern "C"
{
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
}
#include "bat.h"
#include "object_parameters.h"
#include "record_data.h"
#include "game_random.h"
#include "native_object_storage.h"
#include "game_object_allocation.h"
#include "audio_voice.h"
#include <new>
#include <stdio.h>
#include <stdlib.h>

#include <string.h>

Bat::Bat(const uint8 *record_data, uint32 index) : Enemy()
{
    const uint32 data = GuestMemoryAddress(record_data);
    uint32 object = NativeObjectGuestAddress(this);
    uint32 slot, value, flags, cursor;
    FUNCTION_MARKER(0x80049F24, "SLUS_003.73");
    if (!data)
    {
        // TODO Support native record data in the original parser adapter
        GameDiagnostic("Bat record data is outside guest memory\n");
        abort();
    }
    w_u32(object + 68, 0x800A26F0);
    w_u8(object + 496, 255);
    for (slot = 0; slot < 8; ++slot)
    {
        if (!r_u32(0x800A6C3C + slot * 4))
        {
            w_u32(0x800A6C3C + slot * 4, object);
            w_u8(object + 496, slot);
            break;
        }
    }
    value = r_u32(object + 396);
    flags = r_u16(object);
    w_u16(object + 212, 0);
    w_u32(object + 396, value | 0x40);
    value = r_u16(object + 78);
    w_u16(object, flags | 1);
    w_u16(object + 78, value & 0xFFEF);
    cursor = sub_80062B0C(object, data);
    cursor = sub_80062B50(object, cursor);
    w_u32(object + 400, cursor);
    w_u16(object + 214, index);
    Initialize();
}

Bat::Bat(const uint32 *position, uint32 slot) : Enemy()
{
    uint32 guest_position = GuestMemoryAddress(position);
    const uint32 *native_position = guest_position ? 0 : position;
    uint32 object = NativeObjectGuestAddress(this);
    uint32 x, y, z;
    w_u32(object + 68, 0x800A26F0);
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
    w_u32(object + 240, x);
    w_u32(object + 244, y);
    w_u32(object + 248, z);
    x = r_u32(object + 240);
    y = r_u32(object + 244);
    z = r_u32(object + 248);
    w_u32(object + 4, x);
    w_u32(object + 8, y);
    w_u32(object + 12, z);
    w_u32(object + 400, 0);
    w_u8(object + 496, slot);
    Initialize();
    w_u8(object + 380, 0);
}

uint32 Bat::Initialize()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 total, count, x, y, z, value;
    FUNCTION_MARKER(0x8004A090, "SLUS_003.73");
    total = r_u32(0x800FF308);
    count = r_u32(0x800FF4D0);
    x = r_u32(object + 4);
    y = r_u32(object + 8);
    z = r_u32(object + 12);
    w_u32(object + 484, x);
    w_u32(object + 488, y);
    w_u32(object + 492, z);
    w_u32(0x800FF308, total + 1);
    w_u32(0x800FF4D0, count + 1);
    sub_800626C8(object, 0x800FF4D4);
    sub_80062A38(object, 0x800FF4E8);
    w_u16(object + 58, 209);
    w_u8(object + 129, 3);
    w_u8(object + 130, 3);
    w_u8(object + 131, 3);
    sub_8004BD04(object, 1, 4);
    w_u16(object + 436, 12);
    w_u16(object + 438, 128);
    w_u16(object + 440, 12);
    sub_8004BDCC(object, 0, 1, 10);
    sub_8004BDCC(object, 0, 2, 80);
    sub_8004BDCC(object, 0, 3, 90);
    StartAnimation(0, 1);
    value = sub_80066570(16);
    w_u8(object + 24, value);
    w_u16(object + 218, 5);
    w_u8(object + 380, 1);
    return 1;
}

uint32 Bat::Destroy(uint32 reason)
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 count, flags, slot, voices;
    FUNCTION_MARKER(0x8004A1AC, "SLUS_003.73");
    count = r_u32(0x800FF4D0);
    w_u32(object + 68, 0x800A26F0);
    w_u32(0x800FF4D0, count - 1);
    RemoveFromList((uint32 *)psx_addr(0x800FF4E8, 4));
    Enemy::FreeBehaviorTable();
    flags = r_u32(object + 396);
    if (flags & 0x40)
    {
        slot = r_u8(object + 496);
        w_u32(0x800A6C3C + slot * 4, 0);
    }
    voices = r_u32(object + 500);
    StopSoundVoices(voices);
    Enemy::Destroy(0);
    if (reason & 1)
        return ReleaseObjectStorage(object);
    return 0;
}

uint32 Bat::Damage(uint32 damage, const sint32 *input_vector, uint32 kind)
{
    uint32 guest_vector = GuestMemoryAddress(input_vector);
    const sint32 *native_vector = guest_vector ? 0 : input_vector;
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 tick, health, value, shifted[3], axis, player, flags, table, offset, target;
    FUNCTION_MARKER(0x8004A25C, "SLUS_003.73");
    tick = r_u8(object + 383) + 1;
    health = (uint32)(sint32)(sint16)r_u16(object + 218);
    w_u8(object + 383, tick);
    if ((sint32)health <= 0)
        return tick;
    for (axis = 0; axis < 3; ++axis)
    {
        value = native_vector ? (uint32)native_vector[axis] : r_u32(guest_vector + axis * 4);
        shifted[axis] = (uint32)((sint32)value >> 3);
    }
    AddVectors((uint32 *)psx_addr(object + 104, 12), shifted);
    value = r_u16(object + 218) - damage;
    w_u16(object + 218, value);
    if ((sint32)(value << 16) > 0)
        return SetColor(3, 255, 255, 255);
    player = r_u32(0x800FF5A0);
    if (player)
        TryPlayPlayerKillQuote((const uint32 *)psx_addr(player + 4, 12));
    PlayPositionedSound(77, (const uint32 *)psx_addr(object + 4, 12), 0);
    flags = r_u16(object + 78);
    w_u16(object + 78, flags & 0xFFEF);
    if (kind == 5)
    {
        w_u16(object + 472, 2);
        w_u16(object + 474, 0);
        w_u8(object + 380, 0);
        return 2;
    }
    table = r_u32(object + 68);
    offset = (uint32)(sint32)(sint16)r_u16(table + 16);
    target = r_u32(table + 20);
    return xport_draft_guest_call1(target, object + offset);
}

void Bat::FinishFromTable()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 table, receiver, target;
    table = r_u32(object + 68u);
    receiver = object + (uint32)(sint32)(sint16)r_u16(table + 16u);
    target = r_u32(table + 20u);
    apocalypse_object_virtual20(target, receiver);
}

uint32 Bat::Update()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 value, flags, node, found, sound, table, player, receiver, target;
    uint32 constants[3], word, tail, scale, end_word;
    uint16 angles[3];
    FUNCTION_MARKER(0x8004A37Cu, "SLUS_003.73");
    flags = r_u16(object + 76u);
    w_u16(object + 388u, flags);
    if (flags & 1u)
    {
        value = r_u8(object + 384u);
        w_u16(object + 76u, flags & 0xFFFEu);
        w_u8(object + 384u, value + 1u);
    }
    flags = r_u32(object + 396u);
    if (flags & 64u)
    {
        if (!(r_u32(0x800FF2F0u) & 63u))
        {
            found = 0u;
            node = r_u32(0x800FF4E8u);
            while (node)
            {
                if (node != object && r_u16(node + 58u) == 209u)
                {
                    value = r_u8(node + 496u);
                    if (value == r_u8(object + 496u))
                    {
                        found = 1u;
                        break;
                    }
                }
                node = r_u32(node + 28u);
            }
            if (!found)
            {
                FinishFromTable();
                return 0u;
            }
        }
        value = r_u32(object + 220u);
        if (value < 2049u)
        {
            sound = r_u32(object + 500u);
            if (!sound)
            {
                value = sub_80069EF4(76u, object + 4u, 0u);
                w_u32(object + 500u, value);
            }
            else if (!sub_80066570(12u))
            {
                sound = r_u32(object + 500u);
                sub_80069FAC(sound, object + 4u, 0u);
            }
        }
        else
        {
            sound = r_u32(object + 500u);
            if (sound)
            {
                sub_8006A294(sound);
                w_u32(object + 500u, 0u);
            }
        }
    }
    else if (r_u32(object + 220u) < 96u)
    {
        player = r_u32(0x800FF5A0u);
        table = r_u32(player + 68u);
        receiver = player + (uint32)(sint32)(sint16)r_u16(table + 48u);
        target = r_u32(table + 52u);
        apocalypse_object_virtual52(target, receiver, 1u, 0x800A71CCu, 0u);
    }
    constants[0] = r_u32(0x800A71CCu);
    constants[1] = r_u32(0x800A71D0u);
    constants[2] = r_u32(0x800A71D4u);
    w_u32(object + 116u, constants[0]);
    w_u32(object + 120u, constants[1]);
    w_u32(object + 124u, constants[2]);
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
    if (r_u32(object + 396u) & 1u)
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
            scale = r_u16(object + 436u);
            xport_draft_host_sub_800667CC_p3(object + 116u, scale, angles);
            word = apocalypse_angle_pair_word(object + 16u);
            tail = r_u16(object + 20u);
            scale = r_u16(object + 438u);
            end_word = (uint32)angles[0] | ((uint32)angles[1] << 16);
            sub_80066CF0(word, tail, object + 132u, object + 138u, end_word, angles[2], scale);
        }
    }
    if (r_u8(object + 382u))
        sub_8006C0B8(object + 116u, 0x800A6844u);
    sub_8004B9A4(object);
    sub_8007FDF0(object);
    // The legacy behavior caller discards the original return carrier
    return 0u;
}

uint32 Bat::ExecuteCommand(uint32 command)
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 cursor, value, count, child, slot, speed, table, offset, target;
    uint32 point[4];
    FUNCTION_MARKER(0x8004A690u, "SLUS_003.73");
    command &= 0xFFFFu;
    switch (command)
    {
        case 0x4220u:
            cursor = (r_u32(object + 400u) + 3u) & 0xFFFFFFFCu;
            point[0] = r_u32(cursor);
            point[1] = r_u32(cursor + 4u);
            point[2] = r_u32(cursor + 8u);
            point[0] <<= 12;
            point[1] <<= 12;
            point[2] <<= 12;
            w_u32(object + 400u, cursor + 12u);
            SetTarget(point);
            return 1u;
        case 0x4221u:
            cursor = r_u32(object + 400u);
            value = r_u16(cursor);
            w_u32(object + 400u, cursor + 2u);
            if (value & 0x2000u)
            {
                table = r_u32(object + 68u);
                offset = (uint32)(sint32)(sint16)r_u16(table + 80u);
                target = r_u32(table + 84u);
                value = DispatchObjectParameterGetter(target, object + offset, value);
            }
            ReadRecordPosition(point, value & 0xFFFF);
            SetTarget(point);
            return 1u;
        case 0x4600u:
            cursor = r_u32(object + 400u);
            count = r_u16(cursor);
            w_u32(object + 400u, cursor + 2u);
            w_u8(object + 497u, count);
            while (count)
            {
                point[0] = RandomRange(1024);
                point[1] = 0 - RandomRange(512);
                point[2] = RandomRange(1024) - 512;
                point[0] -= 512u;
                value = r_u32(object + 4u);
                point[0] += value;
                value = r_u32(object + 8u);
                point[1] += value;
                value = r_u32(object + 12u);
                point[2] += value;
                child = sub_800625AC(504u);
                if (child)
                {
                    slot = r_u8(object + 496u);
                    void *native_instance_storage = AllocateNativeObjectStorage(child, sizeof(Bat));
                    new (native_instance_storage) Bat(point, slot);
                }
                if (child)
                {
                    value = r_u8(object + 497);
                    w_u8(child + 497, value);
                    speed = r_u16(object + 440);
                    w_u16(child + 436, speed);
                }
                --count;
            }
            return 1u;
        default:
            return Enemy::ExecuteCommand(command);
    }
}

uint32 Bat::SetTarget(const uint32 *target)
{
    const uint32 guest_target = GuestMemoryAddress(target);
    const uint32 *native_target = guest_target ? 0 : target;
    FUNCTION_MARKER(0x8004A874, "SLUS_003.73");
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 x, y, z, flags;
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
    flags = r_u32(object + 396) | 1;
    w_u32(object + 396, flags);
    return flags;
}

void Bat::UpdateState()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 state, phase, player, flags, value, left, right, parent, radius, amplitude;
    uint32 yaw, signed_yaw, point[3], normal[3], offset[3], axis, ground, height;
    uint16 angles[3];
    FUNCTION_MARKER(0x8004A8E4u, "SLUS_003.73");
    state = r_u16(object + 472u);
    if (state == 0u)
    {
        phase = r_u16(object + 474u);
        if (phase == 0u)
        {
            player = r_u32(0x800FF5A0u);
            if (!player)
                return;
            if (r_u32(object + 396u) & 64u)
            {
                left = r_u32(object + 4u);
                right = r_u32(player + 4u);
                point[0] = (uint32)((sint32)(left - right) >> 12);
                point[1] = 0u;
                left = r_u32(object + 12u);
                right = r_u32(player + 12u);
                point[2] = (uint32)((sint32)(left - right) >> 12);
                xport_draft_host_sub_800872BC_p12(point, normal);
                w_u16(object + 460u, normal[0]);
                value = normal[2] & 0xFFFFu;
                w_u16(object + 464u, 1024u);
                w_u16(object + 466u, 1024u);
                w_u16(object + 468u, 0u);
                w_u16(object + 462u, value);
                value = sub_80066570(64u);
                flags = r_u32(object + 396u);
                w_u32(object + 396u, (value & 1u) ? flags | 32u : flags & 0xFFFFFFDFu);
            }
            else
            {
                value = sub_80066570(256u);
                amplitude = r_u8(object + 497u) >> 3;
                w_u16(object + 466u, (value + 128u) * amplitude * amplitude);
                value = sub_80066570(4096u);
                w_u16(object + 468u, value);
            }
            phase = r_u16(object + 474u);
            w_u16(object + 474u, phase + 1u);
            return;
        }
        if (phase == 2u)
        {
            if (!(r_u32(object + 396u) & 1u))
                FinishFromTable();
            return;
        }
        if (phase != 1u)
            return;
        if (r_u16(object + 388u) & 1u)
        {
            point[0] = r_u32(object + 4u);
            point[1] = r_u32(object + 8u);
            point[2] = r_u32(object + 12u);
            point[1] -= 0x800000u;
            SetTarget(point);
            w_u16(object + 474u, 2u);
            return;
        }
        if (r_u32(object + 396u) & 1u)
            return;
        if (!sub_80066570(32u))
        {
            value = sub_80066570(4096u);
            value = 0x800F863Cu + 4u * (value & 4095u);
            left = r_u16(value);
            w_u16(object + 460u, left);
            right = r_u16(value + 2u);
            w_u16(object + 462u, right);
            return;
        }
        flags = r_u32(object + 396u);
        if (!(flags & 64u))
        {
            if (!sub_80066570(16u) && r_u32(object + 220u) < 1024u)
            {
                w_u16(object + 472u, 1u);
                w_u16(object + 474u, 0u);
                return;
            }
            flags = r_u32(object + 396u);
        }
        if (flags & 64u)
        {
            player = r_u32(0x800FF5A0u);
            if (!player)
                return;
            radius = r_u16(object + 464u);
            amplitude = r_u16(object + 466u);
            yaw = r_u16(object + 468u);
            signed_yaw = (uint32)(sint32)(sint16)r_u16(object + 468u);
            point[0] = r_u32(player + 4u);
            point[1] = r_u32(player + 8u);
            point[2] = r_u32(player + 12u);
            value = (uint32)(sint32)(sint16)r_u16(object + 460u);
            point[0] += value * radius;
            value = (uint32)(sint32)(sint16)r_u16(object + 462u);
            point[2] += value * radius;
            value = sub_80066570(128u);
            point[1] -= (value + 256u) << 12;
            value = sub_80066570(4096u);
            value = r_u16(0x800F863Cu + 4u * (value & 4095u));
            angles[0] = (uint16)((sint32)(sint16)value >> 6);
            angles[1] = (uint16)yaw;
            angles[2] = 0u;
            xport_draft_host_sub_800667CC_p13(offset, amplitude, angles);
            for (axis = 0u; axis < 3u; ++axis)
                point[axis] += offset[axis];
            SetTarget(point);
            flags = r_u32(object + 396u);
            value = sub_80066570(64u);
            value = (flags & 32u) ? signed_yaw + 256u + value : signed_yaw - 256u - value;
            w_u16(object + 468u, value & 4095u);
            if (!sub_80066570(4u))
            {
                if (sub_80066570(2u))
                {
                    if (radius < 2048u)
                        w_u16(object + 464u, radius + 32u);
                }
                else if (radius >= 513u)
                    w_u16(object + 464u, radius - 32u);
            }
            if (!sub_80066570(4u))
            {
                if (sub_80066570(2u))
                {
                    if (amplitude < 1600u)
                        w_u16(object + 466u, amplitude + 16u);
                }
                else if (amplitude >= 513u)
                    w_u16(object + 466u, amplitude - 16u);
            }
            return;
        }
        value = r_u8(object + 496u);
        parent = r_u32(0x800A6C3Cu + 4u * value);
        if (!parent)
        {
            FinishFromTable();
            return;
        }
        amplitude = r_u16(object + 466u);
        yaw = r_u16(object + 468u);
        point[0] = r_u32(parent + 484u);
        point[1] = r_u32(parent + 488u);
        point[2] = r_u32(parent + 492u);
        value = sub_80066570(4096u);
        value = r_u16(0x800F863Cu + 4u * (value & 4095u));
        angles[0] = (uint16)((sint32)(sint16)value >> 6);
        angles[1] = (uint16)yaw;
        angles[2] = 0u;
        xport_draft_host_sub_800667CC_p13(offset, amplitude, angles);
        for (axis = 0u; axis < 3u; ++axis)
            point[axis] += offset[axis];
        value = r_u32(parent + 488u);
        if ((sint32)value < (sint32)point[1])
            point[1] = value;
        SetTarget(point);
        return;
    }
    if (state == 1u)
    {
        phase = r_u16(object + 474u);
        if (phase == 2u)
        {
            if (!(r_u32(object + 396u) & 1u))
            {
                w_u16(object + 472u, 0u);
                w_u16(object + 474u, 0u);
            }
            return;
        }
        if (phase > 1u || (phase == 1u && (r_u32(object + 396u) & 1u)))
            return;
        player = r_u32(0x800FF5A0u);
        if (!player)
            return;
        point[0] = r_u32(player + 4u);
        point[1] = r_u32(player + 8u);
        point[2] = r_u32(player + 12u);
        if (phase == 0u)
            point[1] = r_u32(object + 8u) - 0x100000u;
        else
            point[1] -= 0x80000u;
        SetTarget(point);
        phase = r_u16(object + 474u);
        w_u16(object + 474u, phase + 1u);
        return;
    }
    if (state != 2u)
        return;
    phase = r_u16(object + 474u);
    if (phase > 1u)
        return;
    if (!phase)
    {
        flags = r_u32(object + 396u);
        w_u32(object + 112u, 0u);
        w_u32(object + 104u, 0u);
        w_u8(object + 382u, 1u);
        w_u32(object + 396u, flags & 0xFFFFFFFEu);
        ground = sub_80067A18(object + 4u, 256u, 4096u);
        if (ground == 0xFFFFFFFFu)
            FinishFromTable();
        else
        {
            value = (uint32)((sint32)ground >> 16);
            phase = r_u16(object + 474u);
            w_u16(object + 460u, value);
            w_u16(object + 474u, phase + 1u);
        }
        /* Failed ground lookup still falls through to the height check */
    }
    height = r_u32(object + 8u);
    value = r_u32(0x800FF37Cu);
    if ((sint32)value < (sint32)height)
    {
        w_u32(object + 8u, value);
        angles[0] = (uint16)-1024;
        angles[1] = 0u;
        angles[2] = 0u;
        value = sub_80032DC0(364u);
        if (value)
            apocalypse_effect10d_native(value, object + 4u, angles, 512u, 32u, 0u, 128u, 128u, 128u, 5u, 0xCCE0187Cu);
        FinishFromTable();
    }
    else
    {
        value = (uint32)(sint32)(sint16)r_u16(object + 460u);
        if ((sint32)value < ((sint32)height >> 16))
            FinishFromTable();
    }
}

Bat &Bat::Attach(uint32 object)
{
    return *static_cast<Bat *>(FindNativeObjectStorage(object));
}

// Compatibility entry points retain the guest call contract
uint32 sub_80049F24(uint32 object, uint32 data, uint32 index)
{
    FUNCTION_MARKER(0x80049F24, "SLUS_003.73");
    void *native_instance_storage = AllocateNativeObjectStorage(object, sizeof(Bat));
    new (native_instance_storage) Bat((const uint8 *)psx_addr(data, 1), index);
    return object;
}

uint32 sub_8004A000(uint32 object, uint32 position, uint32 slot)
{
    FUNCTION_MARKER(0x8004A000u, "SLUS_003.73");
    void *native_instance_storage = AllocateNativeObjectStorage(object, sizeof(Bat));
    new (native_instance_storage) Bat((const uint32 *)psx_addr(position, 12), slot);
    return object;
}

uint32 apocalypse_record209_spawn_native(uint32 object, const uint32 position[3], uint32 slot)
{
    void *native_instance_storage = AllocateNativeObjectStorage(object, sizeof(Bat));
    new (native_instance_storage) Bat(position, slot);
    return object;
}

uint32 sub_8004A1AC(uint32 object, uint32 reason)
{
    FUNCTION_MARKER(0x8004A1AC, "SLUS_003.73");
    return Bat::Attach(object).Bat::Destroy(reason);
}

uint32 sub_8004A25C(uint32 object, uint32 damage, uint32 vector, uint32 kind)
{
    FUNCTION_MARKER(0x8004A25Cu, "SLUS_003.73");
    return Bat::Attach(object).Bat::Damage(damage, vector ? (const sint32 *)psx_addr(vector, 12) : 0, kind);
}

uint32 apocalypse_record209_damage_native(uint32 object, uint32 damage, const sint32 vector[3], uint32 kind)
{
    return Bat::Attach(object).Bat::Damage(damage, vector, kind);
}

void sub_8004A37C(uint32 object)
{
    FUNCTION_MARKER(0x8004A37C, "SLUS_003.73");
    Bat::Attach(object).Bat::Update();
}

uint32 sub_8004A690(uint32 object, uint32 command)
{
    FUNCTION_MARKER(0x8004A690, "SLUS_003.73");
    return Bat::Attach(object).Bat::ExecuteCommand(command);
}

uint32 sub_8004A874(uint32 object, uint32 target)
{
    FUNCTION_MARKER(0x8004A874, "SLUS_003.73");
    return Bat::Attach(object).SetTarget((const uint32 *)psx_addr(target, 12));
}

uint32 apocalypse_record209_target_native(uint32 object, const uint32 target[3])
{
    return Bat::Attach(object).SetTarget(target);
}

void sub_8004A8E4(uint32 object)
{
    FUNCTION_MARKER(0x8004A8E4, "SLUS_003.73");
    Bat::Attach(object).UpdateState();
}
