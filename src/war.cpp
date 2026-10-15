#include "game_sdk_bindings.h"
#include "object_force.h"
#include "level_session.h"
#include "fire_cycle.h"
extern "C" {
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
}
#include "war.h"
#include "native_object_storage.h"
#include "game_object_allocation.h"
#include "game_panel.h"
#include <new>
#include "projectile_tracking.h"
#include <stdio.h>
#include <stdlib.h>

War::War(const uint8 *record_data, uint32 index) : Enemy()
{
    const uint32 data = GuestMemoryAddress(record_data);
    uint32 object = NativeObjectGuestAddress(this);
    uint32 cursor, roll;
    FUNCTION_MARKER(0x8003D41C, "SLUS_003.73");
    if (!data)
    {
        // TODO Support native record data in the original parser adapter
        GameDiagnostic("War record data is outside guest memory\n");
        abort();
    }
    w_u32(object + 68, 0x800A1EB4);
    cursor = sub_80062B0C(object, data);
    w_u16(object + 16, r_u16(cursor));
    w_u16(object + 18, r_u16(cursor + 2));
    roll = r_u16(cursor + 4);
    w_u32(object + 400, cursor + 6);
    w_u8(object + 380, 1);
    w_u16(object + 214, index);
    w_u16(object + 212, 0);
    w_u16(object + 20, roll);
    Configure();
    cursor = r_u32(object + 400);
    sub_8004BE90(object, cursor);
}

uint32 War::Configure()
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 flags;
    FUNCTION_MARKER(0x8003D4B8, "SLUS_003.73");
    sub_800626C8(object, 0x800FF48C);
    w_u8(object + 129, 31);
    w_u8(object + 130, 3);
    w_u8(object + 131, 31);
    sub_80062A38(object, 0x800FF4E8);
    StartAnimation(0, 1);
    sub_8004BD04(object, 3, 8);
    sub_8004BDCC(object, 0, 0, 0);
    sub_8004BDCC(object, 0, 1, 10);
    sub_8004BDCC(object, 1, 0, 0);
    sub_8004BDCC(object, 2, 0, 0);
    sub_8004BDCC(object, 2, 1, 20);
    sub_8004BDCC(object, 2, 2, 30);
    sub_8004BDCC(object, 2, 3, 512);
    sub_8004BDCC(object, 2, 4, 8);
    sub_8004BDCC(object, 2, 5, 600);
    sub_8004BDCC(object, 2, 6, 256);
    sub_8004BDCC(object, 2, 7, 128);
    flags = r_u16(object + 78);
    w_u16(object + 468, 450);
    w_u16(object + 436, 12);
    w_u16(object + 438, 32);
    w_u32(object + 208, 128);
    w_u16(object + 58, 221);
    w_u16(object + 456, 216);
    w_u16(object + 218, 800);
    w_u32(object + 536, 0);
    w_u16(object + 472, 0);
    w_u16(object + 474, 0);
    w_u8(object + 381, 0);
    w_u8(object + 564, 1);
    w_u32(0x800FF4EC, object);
    w_u16(object + 78, flags & 65533);
    return 1;
}

static void ReleaseWarOwnedWeapon(uint32 child)
{
    uint32 table, target;
    sint32 adjustment;
    if (child)
    {
        table = r_u32(child);
        adjustment = (sint16)r_u16(table + 8);
        target = r_u32(table + 12);
        apocalypse_object_cleanup(target, child + (uint32)adjustment, 3);
    }
}

uint32 War::Destroy(uint32 reason)
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 child, index;
    FUNCTION_MARKER(0x8003D66C, "SLUS_003.73");
    w_u32(object + 68, 0x800A1EB4);
    RemoveFromList((uint32 *)psx_addr(0x800FF4E8, 4));
    child = r_u32(object + 536);
    ReleaseWarOwnedWeapon(child);
    child = r_u32(object + 540);
    ReleaseWarOwnedWeapon(child);
    Enemy::FreeBehaviorTable();
    ClearTrackedObjectHealth();
    w_u32(0x800FF4EC, 0);
    for (index = 0; index < 5; ++index)
    {
        child = r_u32(object + 544 + index * 4);
        ReleaseWarOwnedWeapon(child);
    }
    Enemy::Destroy(0);
    return (reason & 1) ? ReleaseObjectStorage(object) : 0;
}

static uint32 DivideWarMovement(uint32 numerator, uint32 denominator)
{
    if (!denominator)
        return (sint32)numerator < 0 ? 1 : (uint32)-1;
    return (uint32)((sint32)numerator / (sint32)denominator);
}

static void InterpolateWarAngles(uint32 current, const uint16 *desired, uint32 delta, uint32 acceleration, uint32 scale)
{
    uint32 packed = r_u32(current);
    uint32 tail = r_u16(current + 4);
    sub_80066CF0(packed, tail, delta, acceleration, desired[0] | ((uint32)desired[1] << 16), desired[2], scale);
}

static uint32 FireWarWeapon(uint32 weapon, const uint32 *position, const uint16 *angles)
{
    uint32 table = r_u32(weapon);
    sint32 adjustment = (sint16)r_u16(table + 16);
    uint32 target = r_u32(table + 20);
    if (target == 0x80027978)
        return apocalypse_beam_fire_27978(weapon + (uint32)adjustment, position, (const sint16 *)angles);
    GameDiagnostic("TODO War weapon fire target %08X\n", target);
    abort();
}

static uint32 ReleaseWarUpdateWeapon(uint32 weapon)
{
    uint32 table = r_u32(weapon);
    sint32 adjustment = (sint16)r_u16(table + 8);
    uint32 target = r_u32(table + 12);
    return apocalypse_object_cleanup(target, weapon + (uint32)adjustment, 3);
}

uint32 War::Update()
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 x, y, z, flags, value, second, third, target, table, receiver, scale, speed, distance, timer, weapon, result, packed, tail;
    uint32 position[4], muzzle[4];
    uint16 desired[4], bone_angles[4], weapon_angles[4];
    sint32 signed_value, absolute, high;
    FUNCTION_MARKER(0x8003D790, "SLUS_003.73");
    x = r_u32(0x800A71CC);
    y = r_u32(0x800A71D0);
    z = r_u32(0x800A71D4);
    w_u32(object + 116, x);
    w_u32(object + 120, y);
    w_u32(object + 124, z);
    if (!r_u8(object + 381))
    {
        sub_8007CA50(object, 0x800A6868);
        sub_8007CAC8(object, 0x800A68E4);
        value = r_u16(object);
        w_u8(object + 381, 1);
        w_u16(object, value | 4);
    }
    flags = r_u16(object + 76);
    w_u16(object + 388, flags);
    if (flags & 1)
    {
        value = r_u8(object + 384);
        w_u16(object + 76, flags & 0xFFFE);
        w_u8(object + 384, value + 1);
    }
    flags = r_u16(object + 76);
    if (flags & 4)
    {
        w_u16(object + 76, flags & 0xFFFB);
        distance = sub_80066918(object + 4, 0x800A724C);
        if (r_u16(object + 472) == 5 && (sint32)distance < 4096)
        {
            value = (4096 - distance) * 320;
            if ((sint32)value < 0)
                value += 4095;
            second = r_u32(0x800FF384);
            third = (uint32)((sint32)value >> 12);
            if (!second)
                third += (uint32)((sint32)value >> 13);
            table = r_u32(object + 68);
            receiver = object + (uint32)(sint32)(sint16)r_u16(table + 48);
            target = r_u32(table + 52);
            apocalypse_object_virtual52(target, receiver, third, 0x800A71CC, 0);
            if ((sint16)r_u16(object + 218) > 0)
            {
                w_u16(object + 472, 64);
                w_u16(object + 474, 0);
            }
        }
    }
    if (r_u32(object + 220) < 256 && !(r_u16(object) & 1))
    {
        value = r_u32(0x800FF5A0);
        if (value)
            ApplyObjectContactImpulse(value, object);
    }
    if (r_u8(object + 380))
    {
        timer = r_u16(object + 476);
        if (timer)
            w_u16(object + 476, timer - 1);
        else
            sub_8004BE90(object, r_u32(object + 400));
    }
    else
    {
        /* TODO Resolve the War AI undefined destination branch */
        UpdateState();
    }
    if (r_u8(object + 382))
        sub_8006C0B8(object + 116, 0x800A6844);
    sub_8004B9A4(object);
    x = r_u32(object + 4);
    z = r_u32(object + 12);
    flags = r_u32(object + 396);
    w_u32(object + 184, x);
    w_u32(object + 192, z);
    if (flags & 16)
    {
        target = r_u32(object + 516);
        x = r_u32(target + 4);
        y = r_u32(target + 8);
        z = r_u32(target + 12);
        w_u32(object + 520, x);
        w_u32(object + 524, y);
        w_u32(object + 528, z);
        xport_draft_host_sub_80066B8C_p1(desired, object + 4, object + 520);
        desired[0] = 0;
        InterpolateWarAngles(object + 16, desired, object + 132, object + 138, 32);
        goto MovementAnimation;
    }
    if (flags & 32)
    {
        xport_draft_host_sub_80066B8C_p1(desired, object + 4, object + 520);
        desired[0] = 0;
        InterpolateWarAngles(object + 16, desired, object + 132, object + 138, 32);
        goto MovementAnimation;
    }
    if (!(flags & 1))
        goto Footsteps;
    if (flags & 4)
    {
        if ((sint8)r_u8(object + 24) < 3)
        {
            w_u32(object + 304, 65536);
            w_u8(object + 382, 0);
            w_u32(object + 112, 0);
            w_u32(object + 108, 0);
            w_u32(object + 104, 0);
            w_u32(object + 124, 0);
            w_u32(object + 120, 0);
            w_u32(object + 116, 0);
        }
        else if (r_u16(object + 532))
            w_u8(object + 382, 1);
        if ((sint8)r_u8(object + 24) >= 3 && !(r_u32(object + 396) & 8))
        {
            x = r_u32(object + 484);
            value = r_u32(object + 4);
            x -= value;
            timer = r_u16(object + 532);
            x = DivideWarMovement(x, timer);
            z = r_u32(object + 492);
            value = r_u32(object + 12);
            z -= value;
            timer = r_u16(object + 532);
            z = DivideWarMovement(z, timer);
            y = r_u32(object + 488);
            value = r_u32(object + 8);
            y -= value;
            timer = r_u16(object + 532);
            y = DivideWarMovement(y, timer);
            flags = r_u32(object + 396);
            w_u32(object + 396, flags | 8);
            w_u32(object + 104, x);
            w_u32(object + 112, z);
            w_u32(object + 108, y - (timer << 14));
        }
        else if ((sint8)r_u8(object + 24) >= 22 && r_u16(object + 532))
            w_u32(object + 304, 0);
        timer = r_u16(object + 532);
        if (timer)
        {
            w_u16(object + 532, timer - 1);
            if (!((timer - 1) & 0xFFFF))
            {
                x = r_u32(object + 484);
                y = r_u32(object + 488);
                z = r_u32(object + 492);
                w_u32(object + 4, x);
                w_u32(object + 8, y);
                w_u32(object + 12, z);
                w_u32(object + 112, 0);
                w_u32(object + 108, 0);
                w_u32(object + 104, 0);
                w_u32(object + 124, 0);
                w_u32(object + 120, 0);
                w_u32(object + 116, 0);
                w_u32(object + 304, 65536);
                w_u8(object + 382, 0);
            }
        }
        if (r_u8(object + 303))
            w_u32(object + 396, r_u32(object + 396) & 0xFFFFFFF2);
        goto MovementAnimation;
    }
    xport_draft_host_sub_80066B8C_p1(desired, object + 4, object + 484);
    value = desired[0];
    desired[0] = 0;
    packed = r_u32(object + 16);
    tail = r_u16(object + 20);
    scale = r_u16(object + 438);
    sub_80066CF0(packed, tail, object + 132, object + 138, desired[0] | ((uint32)desired[1] << 16), desired[2], scale);
    desired[0] = (uint16)value;
    signed_value = (sint16)r_u16(object + 134);
    absolute = signed_value < 0 ? -signed_value : signed_value;
    if (absolute >= 65)
        absolute = 64;
    scale = r_u16(object + 436);
    value = scale * (64 - (uint32)absolute);
    if ((sint32)value < 0)
        value += 63;
    speed = (uint32)((sint32)value >> 6);
    value = r_u8(object + 26);
    if (value < 2)
    {
        second = (uint32)(sint32)(sint8)r_u8(object + 24) << 12;
        if (!value)
        {
            high = (sint32)(((sint64)(sint32)second * (sint32)0x82082083) >> 32);
            third = (uint32)((sint32)((uint32)high + second) >> 5);
        }
        else
        {
            high = (sint32)(((sint64)(sint32)second * 0x38E38E39) >> 32);
            third = (uint32)(high >> 3);
        }
        third -= (uint32)((sint32)second >> 31);
        signed_value = (sint16)r_u16(0x800F863C + (third & 4095) * 4);
        absolute = signed_value < 0 ? -signed_value : signed_value;
        speed = (scale >> 2) + (uint32)((sint32)(speed * (uint32)absolute) >> 12);
    }
    xport_draft_host_sub_800667CC_p3(object + 104, speed, desired);
    position[0] = r_u32(object + 4);
    position[1] = r_u32(object + 488);
    position[2] = r_u32(object + 12);
    distance = xport_draft_host_sub_8006689C_p12(position, psx_addr(object + 484, 12));
    scale = r_u16(object + 436);
    if (distance < scale * 2)
        w_u32(object + 396, r_u32(object + 396) & ~1);
MovementAnimation:
    if ((r_u32(object + 396) & 1) && r_u8(object + 26) >= 2)
        StartAnimation(0, 1);
Footsteps:
    if (r_u8(object + 26) == 1)
    {
        value = sub_8006325C(object, 1, 7);
        if (sub_80062D24(object, value, 1))
        {
            sub_80069DF0(128, 8192, 0);
            sub_800774EC(r_u32(0x800FF904), object + 4, 2);
        }
        value = sub_8006325C(object, 1, 25);
        if (sub_80062D24(object, value, 2))
        {
            sub_80069DF0(129, 8192, 0);
            sub_800774EC(r_u32(0x800FF904), object + 4, 2);
        }
    }
    bone_angles[0] = 0;
    bone_angles[1] = 0;
    bone_angles[2] = 0;
    flags = r_u32(object + 396);
    if (flags & 16)
    {
        if (r_u32(0x800FF5A0))
        {
            xport_draft_host_sub_8007CC10_p1(position, object, 2);
            target = r_u32(0x800FF5A0) + 184;
            xport_draft_host_sub_80066B8C_p12(bone_angles, position, target);
            bone_angles[1] = (uint16)((bone_angles[1] - r_u16(object + 18)) & 4095);
        }
    }
    else if (flags & 32)
    {
        xport_draft_host_sub_8007CC10_p1(position, object, 2);
        xport_draft_host_sub_80066B8C_p12(bone_angles, position, object + 520);
        bone_angles[1] = (uint16)((bone_angles[1] - r_u16(object + 18)) & 4095);
    }
    InterpolateWarAngles(object + 496, bone_angles, object + 502, object + 508, 8);
    sub_8006C730(object + 496, object + 502);
    sub_8006C624(object + 496);
    sub_8006C730(object + 502, object + 508);
    sub_8006C8E8(object + 502, object + 144);
    sub_8006C64C(object + 502);
    target = r_u32(object + 360);
    value = r_u16(object + 496);
    w_u16(target + 36, value);
    target = r_u32(object + 360);
    value = r_u16(object + 498);
    w_u16(target + 38, value);
    sub_8007CECC(object);
    sub_8007FDF0(object);
    sub_8007D148(object);
    if (r_u8(object + 26) == 4 && r_u16(object + 472) == 16)
    {
        xport_draft_host_sub_8007CC10_p1(position, object, 0);
        xport_draft_host_sub_8007CC10_p1(muzzle, object, 1);
        xport_draft_host_sub_80066B8C_p123(weapon_angles, position, muzzle);
        weapon = r_u32(object + 540);
        if (!weapon)
        {
            weapon = sub_8002FED8(104);
            if (weapon)
            {
                value = (uint32)(sint32)(sint16)r_u16(object + 36);
                value *= 75;
                weapon = sub_80027720(weapon, 0x800FF5A0, (uint32)((sint32)value >> 9), 1, 128, 127);
            }
            w_u32(object + 540, weapon);
            if (weapon)
            {
                sub_80027878(r_u32(object + 540), 0, 64, 128, 185, 185, 185, 0, 64, 128, 255, 255, 255);
                signed_value = (sint16)r_u16(0x800EC526);
                value = sub_8004B914(object, (uint32)(sint32)(sint16)(signed_value / 5));
                weapon = r_u32(object + 540);
                w_u32(weapon + 40, value);
                weapon = r_u32(object + 540);
                FireWarWeapon(weapon, position, weapon_angles);
            }
        }
        else
            FireWarWeapon(weapon, position, weapon_angles);
    }
    else
    {
        weapon = r_u32(object + 540);
        if (weapon)
        {
            ReleaseWarUpdateWeapon(weapon);
            w_u32(object + 540, 0);
        }
    }
    result = sub_8004BDFC(object, 1, 0) & 0xFFFF;
    if (result)
    {
        weapon = r_u32(object + 536);
        if (!weapon)
        {
            result = sub_8002FED8(104);
            weapon = result;
            if (weapon)
            {
                result = sub_80027720(weapon, 0x800FF5A0, 3000, 0, 256, 9);
                weapon = result;
            }
            w_u32(object + 536, weapon);
            if (!weapon)
                return result;
            w_u32(weapon + 40, 1);
            sub_80027878(r_u32(object + 536), 0, 0, 64, 185, 185, 185, 0, 0, 64, 255, 255, 255);
            result = r_u32(object + 536);
            if (!result)
                return result;
        }
        xport_draft_host_sub_8007CC10_p1(position, object, 3);
        value = r_u16(object + 18);
        second = r_u16(object + 498);
        bone_angles[1] = (uint16)((value + second) & 4095);
        return FireWarWeapon(r_u32(object + 536), position, bone_angles);
    }
    weapon = r_u32(object + 536);
    if (weapon)
    {
        result = ReleaseWarUpdateWeapon(weapon);
        w_u32(object + 536, 0);
    }
    return result;
}

extern "C" uint32 sub_8003D790(uint32 object)
{
    FUNCTION_MARKER(0x8003D790, "SLUS_003.73");
    return War::Attach(object).War::Update();
}

uint32 War::Damage(uint32 damage, const sint32 *, uint32 kind)
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 count, stored_health;
    sint32 health, reduced;
    FUNCTION_MARKER(0x8003E354, "SLUS_003.73");
    if (r_u16(object + 472) < 2)
        return 0;
    health = (sint16)r_u16(object + 218);
    if (health <= 0)
        return 1;
    count = r_u8(object + 383);
    w_u8(object + 383, count + 1);
    reduced = (sint32)damage >> 1;
    if (kind == 30)
        reduced >>= 1;
    else if (kind == 5 || kind == 10)
        reduced >>= 2;
    health = (sint16)r_u16(object + 218);
    stored_health = r_u16(object + 218);
    if (reduced < health)
    {
        w_u16(object + 218, stored_health - (uint32)reduced);
        SetColor(3, 255, 255, 255);
    }
    else
    {
        w_u16(object + 218, 0);
        w_u16(object + 472, 9);
        w_u16(object + 474, 0);
    }
    return 1;
}

uint32 War::Resize(uint32 increment)
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 flags, scale, height, displacement, y;
    FUNCTION_MARKER(0x8003E418, "SLUS_003.73");
    flags = r_u16(object);
    scale = (uint32)(sint32)(sint16)r_u16(object + 36) + increment;
    w_u16(object, flags | 512);
    height = (uint32)((sint32)(scale * 216) >> 12);
    w_u16(object + 40, scale);
    w_u16(object + 38, scale);
    w_u16(object + 36, scale);
    displacement = (uint32)(sint32)(sint16)r_u16(object + 456);
    w_u16(object + 456, height);
    displacement = ((uint32)(sint32)(sint16)height - displacement) << 12;
    y = r_u32(object + 8);
    w_u32(object + 8, y - displacement);
    return displacement;
}

// State-machine helpers are native extractions of original inline blocks
void War::AdvancePhase()
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 phase = r_u16(object + 474);
    w_u16(object + 474, phase + 1);
}

void War::AdvanceState()
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 state = r_u16(object + 472);
    w_u16(object + 474, 0);
    w_u16(object + 472, state + 1);
}

void War::RestoreState()
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 state = r_u16(object + 440);
    uint32 phase = r_u16(object + 442);
    w_u16(object + 472, state);
    w_u16(object + 474, phase);
}

void War::ClearVelocity()
{
    uint32 object = NativeObjectGuestAddress(this);
    w_u32(object + 112, 0);
    w_u32(object + 108, 0);
    w_u32(object + 104, 0);
}

void War::SelectPoint(uint32 table, uint32 minimum, uint32 attempts, uint32 point[3])
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 index, attempt = 0, distance;
    do
    {
        ++attempt;
        index = sub_80066570(4);
        index = r_u16(table + index * 2);
        xport_draft_host_sub_8006613C_p1(point, index);
        distance = xport_draft_host_sub_8006696C_p2(object + 4, point);
        if (distance >= minimum)
            break;
        if (attempts)
        {
            uint32 retry = attempt < attempts;
            // Original retry delay slot increments the counter again
            ++attempt;
            if (!retry)
                break;
        }
    } while (1);
    SetDestination(point);
    AdvancePhase();
}

void War::ActivateGroup(uint32 maximum)
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 cursor, count, index, list, entry;
    cursor = r_u8(object + 566);
    w_u8(object + 566, cursor + 1);
    count = r_u16(0x800FFDF0 + cursor * 2);
    if (count >= maximum)
        return;
    for (index = 0; index < count; ++index)
    {
        cursor = r_u8(object + 566);
        list = r_u32(0x800FF5DC);
        w_u8(object + 566, cursor + 1);
        entry = r_u16(0x800FFDF0 + cursor * 2);
        sub_80064E10(list, entry);
    }
}

static void SetWarCamera(uint32 camera, uint32 distance, uint32 pitch, uint32 yaw, uint32 x, uint32 y, uint32 time, uint32 field, uint32 set_field)
{
    sub_800785D8(camera, distance, time);
    sub_80078618(camera, pitch, time);
    sub_8007865C(camera, yaw, time);
    sub_800786A0(camera, x, time);
    sub_800786E4(camera, y, time);
    if (set_field)
        sub_8007851C(camera, field, time);
}

void War::FireDeathWeapons()
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 index, weapon, table, receiver, target;
    uint32 origin[4], endpoint[4];
    uint16 angles[4];
    for (index = 0; index < 5; ++index)
    {
        if (!r_u32(object + 544 + index * 4))
            continue;
        xport_draft_host_sub_8007CC10_p1(origin, object, 5 + index * 2);
        xport_draft_host_sub_8007CC10_p1(endpoint, object, 6 + index * 2);
        xport_draft_host_sub_80066B8C_p123(angles, origin, endpoint);
        weapon = r_u32(object + 544 + index * 4);
        table = r_u32(weapon);
        receiver = weapon + (uint32)(sint32)(sint16)r_u16(table + 16);
        target = r_u32(table + 20);
        if (target == 0x80027978)
            apocalypse_beam_fire_27978(receiver, origin, (const sint16 *)angles);
        else
        {
            GameDiagnostic("TODO War AI fire target %08X\n", target);
            abort();
        }
    }
}

void War::CreateDeathWeapon(uint32 offset)
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 weapon = sub_8002FED8(104);
    if (weapon)
        weapon = sub_80027720(weapon, 0x800FF4E8, 4096, 1, 0, 9);
    w_u32(object + offset, weapon);
    sub_80027878(r_u32(object + offset), 64, 64, 64, 185, 185, 185, 64, 64, 64, 255, 255, 255);
}

void War::UpdateState()
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 state, phase, flags, value, second, player, camera, index, count, cursor, target, table, receiver;
    uint32 point[4];
    sint32 height, timer, distance;
    FUNCTION_MARKER(0x8003E478, "SLUS_003.73");
    state = r_u16(object + 472);
    if (state > 64)
        return;
    switch (state)
    {
        case 0:
            phase = r_u16(object + 474);
            if (phase)
                return;
            flags = r_u16(object);
            w_u16(object, flags | 1);
            value = sub_80067A18(object + 4, 0, 4096);
            if (value == (uint32)-1)
                return;
            second = r_u32(object + 8);
            height = (sint16)r_u16(object + 456);
            if (((sint32)(value - second) >> 12) < height * 2)
            {
                second = value - ((uint32)height << 12);
                w_u32(object + 188, second);
                w_u32(object + 8, second);
                w_u16(object + 202, (uint32)((sint32)(value - second) >> 12));
                w_u16(object + 472, 1);
                w_u8(object + 382, 0);
                ClearVelocity();
                w_u16(object + 462, 0);
            }
            return;
        case 1:
            phase = r_u16(object + 474);
            if (!phase)
                SelectPoint(0x800FF490, 257, 0, point);
            else if (phase == 1)
            {
                if (r_u8(object + 384))
                {
                    w_u8(object + 384, 0);
                    w_u16(object + 472, 2);
                    w_u16(object + 474, 0);
                }
                else if (!(r_u32(object + 396) & 1))
                    w_u16(object + 474, 0);
            }
            return;
        case 2:
            phase = r_u16(object + 474);
            if (!phase)
            {
                w_u16(object + 212, 128);
                flags = r_u16(object);
                w_u16(object + 436, 20);
                camera = r_u32(0x800FF904);
                w_u16(object, flags & 0xFFFE);
                w_u32(camera + 564, 18);
                w_u16(object + 218, 1800);
                sub_8006D390(object, 64, 32, 2048);
                AdvancePhase();
            }
            else if (phase == 1)
                SelectPoint(0x800FF498, 201, 17, point);
            else if (phase == 2 && !(r_u32(object + 396) & 1))
            {
                player = r_u32(0x800FF4E8);
                value = r_u16(0x800FF4AC);
                sub_80064E10(player, value);
                state = r_u16(object + 472);
                w_u16(object + 436, 14);
                w_u16(object + 474, 0);
                w_u16(object + 472, state + 1);
            }
            return;
        case 3:
            phase = r_u16(object + 474);
            if (!phase)
            {
                w_u16(object + 462, 0);
                w_u16(object + 474, phase + 1);
                phase = 1;
            }
            if (phase == 1)
                SelectPoint(0x800FF498, 201, 17, point);
            else if (phase == 2)
            {
                timer = (sint16)r_u16(object + 462);
                if (!timer)
                {
                    w_u16(object + 462, 90);
                    height = (sint16)r_u16(object + 218);
                    SpawnProjectileWave(height < 1656 ? 2 : 1);
                }
                else
                    w_u16(object + 462, (uint32)timer - 1);
                if ((sint16)r_u16(object + 218) < 1512)
                    AdvanceState();
                else if (!(r_u32(object + 396) & 1))
                {
                    phase = r_u16(object + 474);
                    w_u16(object + 474, phase - 1);
                }
                else if (r_u32(object + 220) < 512 && !sub_80066570(2))
                {
                    state = r_u16(object + 472);
                    w_u16(object + 442, 1);
                    w_u16(object + 444, 1512);
                    w_u16(object + 472, 16);
                    w_u16(object + 474, 0);
                    w_u16(object + 440, state);
                }
            }
            return;
        case 4:
            phase = r_u16(object + 474);
            switch (phase)
            {
                case 0:
                    value = r_u16(0x800FF4A0);
                    xport_draft_host_sub_8006613C_p1(point, value);
                    SetDestination(point);
                    AdvancePhase();
                    return;
                case 1:
                    if (!(r_u32(object + 396) & 1))
                    {
                        state = r_u16(object + 472);
                        w_u16(object + 442, 2);
                        w_u16(object + 472, 32);
                        w_u16(object + 474, 0);
                        w_u16(object + 440, state);
                    }
                    return;
                case 2:
                    camera = r_u32(0x800FF904);
                    player = r_u32(0x800FF5A0);
                    w_u32(camera + 564, 3);
                    w_u32(camera + 304, 1);
                    w_u32(camera + 296, player);
                    w_u32(camera + 340, 1);
                    w_u32(camera + 336, player);
                    SetWarCamera(camera, 2276, (uint32)-520, 0, (uint32)-1590, (uint32)-3280, 30, 2048, 1);
                    SetAnimationRange(2, 0, (uint32)-1);
                    AdvancePhase();
                    return;
                case 3:
                    if (r_u8(object + 303))
                    {
                        SetAnimationRange(3, 0, (uint32)-1);
                        AdvancePhase();
                    }
                    return;
                case 4:
                    if (r_u8(object + 303))
                        AdvancePhase();
                    else
                        Resize(178);
                    return;
                case 5:
                    SetAnimationRange(7, 0, (uint32)-1);
                    AdvancePhase();
                    return;
                case 6:
                    if ((sint8)r_u8(object + 24) >= 25)
                    {
                        sub_80069EF4(131, object + 4, 0);
                        for (index = 0; index < 2; ++index)
                        {
                            value = r_u16(0x800FF4A4 + index * 2);
                            player = r_u32(0x800FF5DC);
                            sub_80064E10(player, value);
                        }
                        AdvancePhase();
                    }
                    return;
                case 7:
                    if (r_u8(object + 303))
                    {
                        value = (uint32)(sint32)(sint8)r_u8(object + 24);
                        SetAnimationRange(7, value, 0);
                        AdvancePhase();
                    }
                    return;
                case 8:
                    if (r_u8(object + 303))
                    {
                        if (r_u32(0x800FF344))
                        {
                            value = r_u16(0x800FF4AA);
                            sub_80064874(value);
                            w_u32(object + 176, 0x06403030);
                            w_u16(object + 180, 8192);
                            w_u16(object + 182, 10);
                        }
                        SetAnimationRange(4, 0, (uint32)-1);
                        AdvancePhase();
                    }
                    return;
                case 9:
                    if (r_u8(object + 303))
                    {
                        phase = r_u16(object + 474);
                        w_u16(object + 462, 0);
                        w_u16(object + 474, phase + 1);
                    }
                    return;
                case 10:
                    timer = (sint16)r_u16(object + 462);
                    if (timer < 60)
                        w_u16(object + 462, (uint32)timer + 1);
                    else
                    {
                        w_u16(object + 462, 0);
                        AdvanceState();
                    }
                    return;
                default: return;
            }
        case 5:
            player = r_u32(0x800FF5A0);
            value = r_u32(object + 12);
            second = r_u32(player + 12);
            if ((sint32)value < (sint32)second)
            {
                w_u32(object + 112, 0);
                w_u32(object + 108, 0);
                w_u16(object + 472, 48);
                flags = r_u32(object + 396);
                w_u32(object + 104, 0);
                w_u16(object + 474, 0);
                w_u32(object + 396, flags & ~1);
                return;
            }
            if ((sint32)second < (sint32)0xF13AC000)
            {
                flags = r_u32(object + 396);
                if (!(flags & 64))
                {
                    w_u32(object + 396, flags | 64);
                    w_u32(object + 112, 0);
                    w_u32(object + 108, 0);
                    flags = r_u32(object + 396);
                    state = r_u16(object + 472);
                    w_u32(object + 104, 0);
                    w_u16(object + 474, 0);
                    w_u32(object + 396, flags & ~1);
                    w_u16(object + 472, state + 1);
                    return;
                }
            }
            index = r_u8(object + 567);
            value = r_u32(0x800A69D8 + index * 4);
            second = r_u32(object + 12);
            if ((sint32)second < (sint32)(value << 12))
            {
                cursor = r_u8(object + 566);
                w_u8(object + 567, index + 1);
                w_u8(object + 566, cursor + 1);
                count = r_u16(0x800FFDF0 + cursor * 2);
                for (index = 0; index < count; ++index)
                {
                    cursor = r_u8(object + 566);
                    player = r_u32(0x800FF5DC);
                    w_u8(object + 566, cursor + 1);
                    value = r_u16(0x800FFDF0 + cursor * 2);
                    sub_80064E10(player, value);
                }
            }
            if (r_u8(object + 26) != 1)
                StartAnimation(1, 1);
            phase = r_u16(object + 474);
            if (phase == 1)
            {
                SetDestination((const uint32 *)psx_addr(object + 484, 12));
                phase = r_u16(object + 474);
                w_u16(object + 474, phase - 1);
            }
            else if (!phase)
            {
                player = r_u32(0x800FF5A0);
                value = r_u32(object + 12);
                second = r_u32(player + 12);
                distance = (sint32)(value - second);
                if (distance <= 10649599)
                {
                    value = r_u32(object + 396) & 64;
                    second = r_u16(object + 436);
                    if (second > (value ? 40 : 60))
                        w_u16(object + 436, second - 2);
                }
                else if (distance > 11468800)
                {
                    value = r_u32(object + 396) & 64;
                    second = r_u16(object + 436);
                    if (second < (value ? 80 : 128))
                        w_u16(object + 436, second + 1);
                }
                if (!(r_u32(object + 396) & 1))
                {
                    index = r_u16(object + 462);
                    timer = (sint16)r_u16(object + 462);
                    if (index >= 8)
                    {
                        /* TODO Original local XYZ is uninitialized; native recovery awaits human contract */
                        GameDiagnostic("TODO War AI state5 exhausted path vector\n");
                        abort();
                    }
                    value = r_u16(0x800A6A60 + index * 2);
                    xport_draft_host_sub_8006613C_p1(point, value);
                    SetDestination(point);
                    w_u16(object + 462, (uint32)timer + 1);
                }
            }
            return;
        case 6:
            phase = r_u16(object + 474);
            if (!phase)
            {
                player = r_u32(0x800FF5A0);
                w_u32(player + 452, 240);
                SetAnimationRange(7, 0, (uint32)-1);
                AdvancePhase();
            }
            else if (phase == 1)
            {
                if ((sint8)r_u8(object + 24) >= 24)
                {
                    player = r_u32(0x800FF4E8);
                    value = r_u16(0x800FF4A8);
                    sub_80064E10(player, value);
                    phase = r_u16(object + 474);
                    w_u16(object + 460, 0);
                    w_u16(object + 474, phase + 1);
                }
            }
            else if (phase == 2 && r_u8(object + 303))
            {
                value = r_u16(object + 460);
                timer = (sint16)r_u16(object + 460);
                if (value < 150)
                    w_u16(object + 460, (uint32)timer + 1);
                else
                {
                    value = (uint32)(sint32)(sint8)r_u8(object + 24);
                    SetAnimationRange(7, value, 0);
                    AdvancePhase();
                }
            }
            else if (phase == 3 && r_u8(object + 303))
            {
                flags = r_u16(object);
                w_u16(object, flags | 8);
                SetDestination((const uint32 *)psx_addr(object + 484, 12));
                state = r_u16(object + 472);
                w_u16(object + 474, 0);
                w_u16(object + 472, state - 1);
            }
            return;
        case 9:
            phase = r_u16(object + 474);
            if (!phase)
            {
                if (r_u32(object + 396) & 64)
                    ActivateGroup(4);
                sub_8005F62C(r_u32(0x800FF5A0), 900);
                sub_8002FC64(18, 4, object + 4, 0);
                w_u32(object + 304, 32768);
                sub_8006D3B4();
                flags = r_u32(object + 396);
                w_u32(object + 396, flags & ~1);
                ClearVelocity();
                ClearFacing();
                SetAnimationRange(8, 0, (uint32)-1);
                camera = r_u32(0x800FF904);
                w_u32(camera + 340, 1);
                w_u32(camera + 336, object);
                w_u32(camera + 304, 1);
                w_u32(camera + 296, object);
                SetWarCamera(camera, 2276, (uint32)-1872, 64, (uint32)-30, (uint32)-714, 30, 2048, 1);
                phase = r_u16(object + 474);
                w_u16(object + 460, 0);
                w_u16(object + 474, phase + 1);
            }
            else if (phase == 1)
            {
                if (!sub_80066570(3))
                {
                    point[0] = r_u32(object + 4);
                    point[1] = r_u32(object + 8);
                    point[2] = r_u32(object + 12);
                    point[1] += 1638400;
                    point[2] -= 1048576;
                    value = sub_80066570(512);
                    point[0] += (value - 256) << 12;
                    value = sub_80066570(512);
                    point[1] += (value - 256) << 12;
                    value = sub_80066570(512);
                    point[2] += (value - 256) << 12;
                    SpawnFireCycle(point, 60, 1, 2);
                    value = sub_80066570(2);
                    sub_80069EF4(value + 1, object + 4, 2);
                }
                if ((sint8)r_u8(object + 24) >= 12)
                    w_u8(object + 24, 13);
                timer = (sint16)r_u16(object + 460);
                w_u16(object + 460, (uint32)timer + 1);
                if (timer == 42) CreateDeathWeapon(544);
                else if (timer == 58) CreateDeathWeapon(548);
                else if (timer == 68) CreateDeathWeapon(552);
                else if (timer == 73) CreateDeathWeapon(556);
                else if (timer == 89) CreateDeathWeapon(560);
                else if (timer == 90)
                {
                    camera = r_u32(0x800FF904);
                    SetWarCamera(camera, 2252, (uint32)-552, 60, 522, 268, 40, 0, 0);
                }
                else if (timer >= 141)
                    AdvancePhase();
                FireDeathWeapons();
            }
            else if (phase == 2)
            {
                FireDeathWeapons();
                if ((sint8)r_u8(object + 24) >= 28)
                {
                    for (index = 0; index < 5; ++index)
                    {
                        target = r_u32(object + 544 + index * 4);
                        if (target)
                        {
                            table = r_u32(target);
                            receiver = target + (uint32)(sint32)(sint16)r_u16(table + 8);
                            value = r_u32(table + 12);
                            apocalypse_object_cleanup(value, receiver, 3);
                            w_u32(object + 544 + index * 4, 0);
                        }
                    }
                }
                if ((sint8)r_u8(object + 24) >= 33)
                {
                    sub_800774EC(r_u32(0x800FF904), object + 4, 0);
                    sub_80069EF4(130, object + 4, 0);
                    phase = r_u16(object + 474);
                    w_u16(object + 460, 0);
                    w_u16(object + 474, phase + 1);
                }
            }
            else if (phase == 3 && r_u8(object + 303))
            {
                value = r_u16(object + 460);
                timer = (sint16)r_u16(object + 460);
                if (value < 17)
                    w_u16(object + 460, (uint32)timer + 1);
                else
                    CompleteLevel(0);
            }
            return;
        case 16:
            phase = r_u16(object + 474);
            if (!phase)
            {
                flags = r_u32(object + 396);
                player = r_u32(0x800FF5A0);
                w_u32(object + 396, flags & ~1);
                ClearVelocity();
                TrackTarget(player);
                SetAnimationRange(4, 0, (uint32)-1);
                AdvancePhase();
            }
            else if (phase == 1 && r_u8(object + 303))
            {
                if (r_u32(object + 220) < 512)
                {
                    height = (sint16)r_u16(object + 218);
                    value = r_u16(object + 444);
                    if ((sint32)value < height)
                    {
                        SetAnimationRange(4, 0, (uint32)-1);
                        return;
                    }
                }
                SetAnimationRange(6, 0, (uint32)-1);
                ClearFacing();
                AdvancePhase();
            }
            else if (phase == 2 && r_u8(object + 303))
                RestoreState();
            return;
        case 32:
            phase = r_u16(object + 474);
            if (!phase)
            {
                flags = r_u32(object + 396);
                player = r_u32(0x800FF5A0);
                w_u32(object + 396, flags & ~1);
                ClearVelocity();
                TrackTarget(player);
                AdvancePhase();
            }
            else if (phase == 1)
            {
                StartAnimation(0, 1);
                sub_8004BDCC(object, 1, 0, 1);
                phase = r_u16(object + 474);
                w_u16(object + 460, 180);
                w_u16(object + 474, phase + 1);
            }
            else if (phase == 2)
            {
                if (r_u32(0x800FF5A0))
                {
                    timer = (sint16)r_u16(object + 460);
                    if (timer)
                    {
                        w_u16(object + 460, (uint32)timer - 1);
                        return;
                    }
                }
                w_u16(object + 474, 3);
            }
            else if (phase == 3)
            {
                sub_8004BDCC(object, 1, 0, 0);
                ClearFacing();
                RestoreState();
            }
            return;
        case 48:
            phase = r_u16(object + 474);
            if (!phase)
            {
                player = r_u32(0x800FF5A0);
                w_u32(player + 452, 900);
                SetAnimationRange(7, 0, (uint32)-1);
                if (r_u32(object + 396) & 64)
                    ActivateGroup(4);
                camera = r_u32(0x800FF904);
                w_u32(camera + 340, 1);
                w_u32(camera + 336, object);
                w_u32(camera + 304, 1);
                w_u32(camera + 296, object);
                SetWarCamera(camera, 832, (uint32)-568, 208, (uint32)-624, (uint32)-1104, 16, 1736, 1);
                AdvancePhase();
            }
            else if (phase == 1 && (sint8)r_u8(object + 24) >= 25)
            {
                sub_80069EF4(131, object + 4, 0);
                xport_draft_host_sub_8007CC10_p1(point, object, 5);
                target = sub_80032DC0(84);
                if (target)
                    apocalypse_war_impact_native(target, point);
                player = r_u32(0x800FF5A0);
                height = (sint16)r_u16(player + 218);
                table = r_u32(player + 68);
                w_u32(player + 452, 0);
                w_u32(player + 448, 0);
                receiver = player + (uint32)(sint32)(sint16)r_u16(table + 48);
                target = r_u32(table + 52);
                apocalypse_object_virtual52(target, receiver, (uint32)height << 2, 0x800A71CC, 0);
                player = r_u32(0x800FF5A0);
                w_u32(player + 452, 900);
                AdvancePhase();
            }
            return;
        case 64:
            phase = r_u16(object + 474);
            if (!phase)
            {
                flags = r_u32(object + 396);
                w_u32(object + 112, 0);
                w_u32(object + 396, flags & ~1);
                SetAnimationRange(9, 0, (uint32)-1);
                w_u16(object + 474, 3);
            }
            else if (phase == 2 && r_u8(object + 303))
            {
                value = r_u16(object + 78);
                w_u16(object + 78, value | 16);
                StartAnimation(1, 1);
                w_u16(object + 472, 5);
                w_u16(object + 474, 1);
            }
            else if (phase == 3 && (sint8)r_u8(object + 24) >= 20)
            {
                value = r_u16(object + 78);
                w_u16(object + 78, value & 0xFFEF);
                ClearVelocity();
                sub_80069DF0(130, 8192, 0);
                phase = r_u16(object + 474);
                w_u16(object + 468, 0);
                w_u16(object + 474, phase + 1);
            }
            else if (phase == 4 && r_u8(object + 303))
            {
                timer = (sint16)r_u16(object + 468);
                value = (uint32)timer + 1;
                if ((value & 0xFFFF) < 9)
                    w_u16(object + 468, value);
                else
                {
                    do
                    {
                        index = sub_80066570(6);
                        value = r_u8(object + 565);
                    } while ((index & 255) == value);
                    value = r_u8(0x800FF484 + (index & 255));
                    sub_8002FC64(18, value, object + 4, 0);
                    w_u8(object + 565, index);
                    SetAnimationRange(10, 0, (uint32)-1);
                    w_u16(object + 474, 2);
                }
            }
            return;
        default: return;
    }
}

extern "C" void sub_8003E478(uint32 object)
{
    FUNCTION_MARKER(0x8003E478, "SLUS_003.73");
    War::Attach(object).UpdateState();
}

uint32 War::ReadParameters(uint32 command)
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 cursor, value, index, group, count, destination, word_index;
    FUNCTION_MARKER(0x8003FCEC, "SLUS_003.73");
    command &= 0xFFFF;
    if (command != 0x2130)
        return Enemy::ReadParameters(command);
    cursor = r_u32(object + 400);
    w_u32(object + 400, cursor + 2);
    for (index = 0; index < 4; ++index)
    {
        cursor = r_u32(object + 400);
        value = r_u16(cursor);
        w_u16(0x800FF490 + index * 2, value);
        w_u32(object + 400, cursor + 2);
    }
    for (index = 0; index < 4; ++index)
    {
        cursor = r_u32(object + 400);
        value = r_u16(cursor);
        cursor += 2;
        w_u16(0x800FF498 + index * 2, value);
        w_u32(object + 400, cursor);
    }
    value = r_u16(cursor);
    w_u32(object + 400, cursor + 10);
    w_u16(0x800FF4A0, value);
    for (index = 0; index < 2; ++index)
    {
        cursor = r_u32(object + 400);
        value = r_u16(cursor);
        w_u16(0x800FF4A4 + index * 2, value);
        w_u32(object + 400, cursor + 2);
    }
    for (index = 0; index < 8; ++index)
    {
        cursor = r_u32(object + 400);
        value = r_u16(cursor);
        w_u16(0x800A6A60 + index * 2, value);
        w_u32(object + 400, cursor + 2);
    }
    word_index = 0;
    destination = 0x800FFDF0;
    for (group = 0; group < 9; ++group)
    {
        ++word_index;
        cursor = r_u32(object + 400);
        w_u32(object + 400, cursor + 2);
        count = r_u16(cursor + 2);
        w_u32(object + 400, cursor + 4);
        w_u16(destination, count);
        destination += 2;
        if (count)
        {
            for (index = 0; index < count; ++index)
            {
                destination += 2;
                cursor = r_u32(object + 400);
                value = r_u16(cursor);
                w_u16(0x800FFDF0 + word_index * 2, value);
                ++word_index;
                w_u32(object + 400, cursor + 2);
            }
        }
    }
    destination = 0x800FFDF0 + word_index * 2;
    cursor = r_u32(object + 400);
    value = r_u16(cursor);
    w_u32(object + 400, cursor + 2);
    w_u16(0x800FF4A8, value);
    for (group = 0; group < 24; ++group)
    {
        w_u16(destination, 1);
        destination += 2;
        ++word_index;
        destination += 2;
        cursor = r_u32(object + 400);
        value = r_u16(cursor);
        w_u16(0x800FFDF0 + word_index * 2, value);
        ++word_index;
        w_u32(object + 400, cursor + 2);
    }
    cursor = r_u32(object + 400);
    value = r_u16(cursor);
    cursor += 2;
    w_u32(object + 400, cursor);
    w_u16(0x800FF4AA, value);
    value = r_u16(cursor);
    cursor += 2;
    w_u32(object + 400, cursor);
    w_u16(0x800FF4AC, value);
    return cursor;
}

uint32 War::ExecuteCommand(uint32 command)
{
    FUNCTION_MARKER(0x8003FF2C, "SLUS_003.73");
    return Enemy::ExecuteCommand(command & 0xFFFF);
}



uint32 War::SetDestination(const uint32 *position)
{
    const uint32 object = NativeObjectGuestAddress(this);
    const uint32 guest_position = GuestMemoryAddress(position);
    uint32 x, y, z, flags;
    FUNCTION_MARKER(0x8003FF4C, "SLUS_003.73");
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
    uint32 floor;
    floor = guest_position ? sub_80067A18(guest_position, 1024, 4096) : apocalypse_ground_native(position, 1024, 4096);
    if (floor != (uint32)-1)
        y = floor - ((uint32)(sint32)(sint16)r_u16(object + 456) << 12);
    else
        y = r_u32(object + 8);
    w_u32(object + 488, y);
    flags = (r_u32(object + 396) | 1) & 0xFFFFFFF3;
    w_u32(object + 396, flags);
    return flags;
}

uint32 War::TrackTarget(uint32 target)
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 result, flags;
    FUNCTION_MARKER(0x8003FFD8, "SLUS_003.73");
    result = sub_80062F48(target);
    if (!result)
    {
        flags = r_u32(object + 396);
        w_u32(object + 516, target);
        result = (flags & 0xFFFFFFDF) | 16;
        w_u32(object + 396, result);
    }
    return result;
}

uint32 War::ClearFacing()
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 flags;
    FUNCTION_MARKER(0x80040058, "SLUS_003.73");
    flags = r_u32(object + 396);
    w_u32(object + 396, flags & 0xFFFFFFCF);
    w_u16(object + 136, 0);
    w_u16(object + 134, 0);
    w_u16(object + 132, 0);
    w_u16(object + 142, 0);
    w_u16(object + 140, 0);
    w_u16(object + 138, 0);
    return object + 138;
}

uint32 War::SpawnProjectileWave(uint32 count)
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 player, projectile, damage, index, turn = 1024;
    uint32 origin[4], target[3], offset[3];
    sint16 angles[4], direction[4];
    FUNCTION_MARKER(0x80040094, "SLUS_003.73");
    player = r_u32(0x800FF5A0);
    if (!player)
        return player;
    xport_draft_host_sub_8007CC10_p1(origin, object, 4);
    xport_draft_host_sub_8001D320_p1(origin, 64, 64, 64, 160, 5, 0, 100);
    for (index = 0; (sint32)index < (sint32)count; ++index)
    {
        player = r_u32(0x800FF5A0);
        angles[0] = -1024;
        angles[1] = 0;
        angles[2] = 0;
        target[0] = r_u32(player + 184);
        target[1] = r_u32(player + 188);
        target[2] = r_u32(player + 192);
        target[1] += 262144;
        if (count == 2)
        {
            player = r_u32(0x800FF5A0);
            xport_draft_host_sub_80066B8C_p1(direction, object + 4, player + 4);
            direction[0] = 0;
            direction[1] = (sint16)(((uint32)(uint16)direction[1] + turn) & 4095);
            xport_draft_host_sub_800667CC_p13(offset, 180, direction);
            target[0] += offset[0];
            target[2] += offset[2];
            angles[1] = index ? 512 : -512;
        }
        /* Original computed target is not passed to the projectile constructor */
        (void)target;
        projectile = sub_800625AC(420);
        if (projectile)
        {
            damage = sub_8004B914(object, 10);
            projectile = apocalypse_projectile_2CE94_native(projectile, origin, 0, angles, 1024, damage, 0x800FF5A0);
            if (projectile)
            {
                SetProjectileTrailColor(projectile, 8, 0, 64, 128);
                w_u8(projectile + 393, 80);
            }
        }
        turn += 2048;
    }
    return (uint32)-1024;
}

extern "C" uint32 sub_8003D41C(uint32 object, uint32 data, uint32 index)
{
    FUNCTION_MARKER(0x8003D41C, "SLUS_003.73");
    void *native_instance_storage = AllocateNativeObjectStorage(object, sizeof(War));
    new (native_instance_storage) War((const uint8 *)psx_addr(data, 1), index);
    return object;
}

extern "C" uint32 sub_8003D4B8(uint32 object)
{
    FUNCTION_MARKER(0x8003D4B8, "SLUS_003.73");
    return War::Attach(object).Configure();
}

extern "C" uint32 sub_8003D66C(uint32 object, uint32 reason)
{
    FUNCTION_MARKER(0x8003D66C, "SLUS_003.73");
    return War::Attach(object).War::Destroy(reason);
}

extern "C" uint32 sub_8003E354(uint32 object, uint32 damage, uint32, uint32 kind)
{
    FUNCTION_MARKER(0x8003E354, "SLUS_003.73");
    return War::Attach(object).War::Damage(damage, 0, kind);
}

extern "C" uint32 sub_8003FCEC(uint32 object, uint32 command)
{
    FUNCTION_MARKER(0x8003FCEC, "SLUS_003.73");
    return War::Attach(object).ReadParameters(command);
}

extern "C" uint32 sub_8003FF2C(uint32 object, uint32 command)
{
    FUNCTION_MARKER(0x8003FF2C, "SLUS_003.73");
    return War::Attach(object).War::ExecuteCommand(command);
}

extern "C" uint32 sub_8003E418(uint32 object, uint32 increment)
{
    FUNCTION_MARKER(0x8003E418, "SLUS_003.73");
    return War::Attach(object).Resize(increment);
}

extern "C" uint32 sub_8003FF4C(uint32 object, uint32 position)
{
    FUNCTION_MARKER(0x8003FF4C, "SLUS_003.73");
    return War::Attach(object).SetDestination((const uint32 *)psx_addr(position, 12));
}

extern "C" uint32 sub_8003FFD8(uint32 object, uint32 target)
{
    FUNCTION_MARKER(0x8003FFD8, "SLUS_003.73");
    return War::Attach(object).TrackTarget(target);
}

extern "C" uint32 sub_80040058(uint32 object)
{
    FUNCTION_MARKER(0x80040058, "SLUS_003.73");
    return War::Attach(object).ClearFacing();
}

extern "C" uint32 sub_80040094(uint32 object, uint32 count)
{
    FUNCTION_MARKER(0x80040094, "SLUS_003.73");
    return War::Attach(object).SpawnProjectileWave(count);
}



War &War::Attach(uint32 object)
{
    return *static_cast<War *>(FindNativeObjectStorage(object));
}

// Native callback adapter discards the original parameter reader result
void War::SetParameters(uint32 command)
{
    ReadParameters(command);
}

extern "C" uint32 apocalypse_war_destination_native(uint32 object, const uint32 position[3])
{
    return War::Attach(object).SetDestination(position);
}
