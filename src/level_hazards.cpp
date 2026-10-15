#include "game_sdk_bindings.h"
#include "fire_cycle.h"
#include "audio_session.h"
#include "vector_distance.h"
#include "game_random.h"
#include "effect_lifecycle.h"
#include "effect_splash.h"
#include "model_resources.h"
#include "object_collision_events.h"
#include "object_damage.h"
#include "ring_burst_effect.h"
#include "game_object_allocation.h"
#include "particle_debris.h"
extern "C"
{
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
}
#include "level_hazards.h"
#include "object_parameters.h"
#include "record_data.h"
#include "native_object_storage.h"
#include <new>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// Native input bridge for the original position-word loads
static uint32 ReadMovingRecordPosition(const uint32 *position, uint32 axis)
{
    uint32 address = GuestMemoryAddress(position), value;
    if (address)
        return r_u32(address + axis * 4);
    memcpy(&value, (const uint8 *)position + axis * 4, 4);
    return value;
}

FloatingHazard::FloatingHazard(const uint8 *record_data, uint32 index) : Enemy()
{
    const uint32 data = GuestMemoryAddress(record_data);
    if (!data)
    {
        // TODO Bind native record data to the existing parser
        GameDiagnostic("TODO Record409 native record data\n");
        abort();
    }
    uint32 object = NativeObjectGuestAddress(this);
    uint32 bank, animation, flags, cursor;
    FUNCTION_MARKER(0x8005A630, "SLUS_003.73");
    bank = r_u32(0x800FF62C);
    w_u32(object + 68, 0x800A2908);
    sub_800626C8(object, bank);
    animation = sub_8006E080(0xB9E8D489, r_u8(0x800FF644));
    flags = r_u16(object);
    w_u16(object + 22, animation);
    w_u16(object, (flags & 0xFFFD) | 8);
    AddToList((uint32 *)psx_addr(0x800FF4E8, 4));
    w_u16(object + 212, 64);
    w_u16(object + 218, 20);
    w_u16(object + 58, 9);
    const uint8 *angles = ReadPosition(record_data);
    const uint8 *script = ReadAngles(angles);
    w_u32(object + 400, GuestMemoryAddress(script));
    w_u16(object + 214, index);
    w_u8(object + 129, 2);
    w_u8(object + 130, 2);
    w_u8(object + 131, 2);
    w_u8(object + 144, 16);
    w_u8(object + 145, 16);
    w_u8(object + 146, 16);
    cursor = r_u32(object + 400);
    w_u16(object + 436, 12);
    w_u16(object + 438, 256);
    w_u16(object + 440, 20);
    w_u8(object + 380, 1);
    sub_8004BE90(object, cursor);
}

uint32 FloatingHazard::Destroy(uint32 reason)
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 result;
    FUNCTION_MARKER(0x8005A74C, "SLUS_003.73");
    w_u32(object + 68, 0x800A2908);
    RemoveFromList((uint32 *)psx_addr(0x800FF4E8, 4));
    result = Enemy::Destroy(0);
    if (reason & 1)
        result = ReleaseObjectStorage(object);
    return result;
}

void FloatingHazard::FinishFromTable()
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 table = r_u32(object + 68);
    uint32 receiver = object + (uint32)(sint32)(sint16)r_u16(table + 16);
    uint32 target = r_u32(table + 20);
    apocalypse_object_virtual20(target, receiver);
}

void FloatingHazard::Explode()
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 variant;
    SpawnExplosionRing((const uint32 *)psx_addr(object + 4, 12), 96, 96, 128, 0, 4, 1, 100);
    variant = RandomRange(2);
    PlayPositionedSound((sint32)(variant + 1), (const uint32 *)psx_addr(object + 4, 12), 0);
}

uint32 FloatingHazard::Damage(uint32 damage, const sint32 *, uint32)
{
    uint32 object = NativeObjectGuestAddress(this);
    sint32 health;
    uint32 value;
    FUNCTION_MARKER(0x8005A7B0, "SLUS_003.73");
    health = (sint16)r_u16(object + 218);
    value = r_u16(object + 218) - damage;
    if (health > 0)
    {
        w_u16(object + 218, value);
        if ((sint32)(value << 16) <= 0)
        {
            Explode();
            FinishFromTable();
        }
    }
    return 1;
}

uint32 FloatingHazard::UpdateHeight()
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 state, result, y, floor;
    FUNCTION_MARKER(0x8005AADC, "SLUS_003.73");
    if (r_u16(object + 472))
        return 1;
    state = r_u16(object + 474);
    result = state < 2;
    if (state != 1)
    {
        if (!result || state)
            return result;
        result = r_u16(object + 474) + 1;
        w_u32(object + 120, 24576);
        w_u16(object + 474, result);
    }
    y = r_u32(object + 8);
    floor = r_u32(0x800FF37C);
    result = (sint32)floor < (sint32)y;
    if (result)
    {
        result = r_u16(object + 474);
        w_u32(object + 120, 0);
        w_u32(object + 108, 0);
        w_u32(object + 8, floor);
        ++result;
        w_u16(object + 474, result);
    }
    return result;
}

uint32 FloatingHazard::Update()
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 flags, value, player, damage, distance, phase, height, y;
    sint16 angles[4];
    FUNCTION_MARKER(0x8005A860, "SLUS_003.73");
    if (!r_u32(object + 108))
    {
        y = r_u32(object + 8);
        height = r_u32(object + 500);
        w_u32(object + 8, y - height);
    }
    flags = r_u16(object + 76);
    w_u16(object + 388, flags);
    if (flags & 1)
    {
        value = r_u8(object + 384);
        w_u16(object + 76, flags & 0xFFFE);
        w_u8(object + 384, value + 1);
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
        UpdateHeight();
    if (r_u32(0x800FF5A0))
    {
        value = r_u16(object + 438);
        distance = r_u32(object + 220);
        if (distance < value)
        {
            Explode();
            damage = ScaleDamageForDifficulty(r_u16(object + 440));
            player = r_u32(0x800FF5A0);
            ApplyObjectDamage(player, damage, (const sint32 *)psx_addr(0x800A71CC, 12), 0);
            FinishFromTable();
        }
    }
    if (r_u32(object + 396) & 1)
    {
        distance = MeasurePositionDistance((const uint32 *)psx_addr(object + 4, 12), (const uint32 *)psx_addr(object + 484, 12));
        if ((sint32)distance < 129)
            w_u32(object + 396, r_u32(object + 396) & ~1);
        else
        {
            xport_draft_host_sub_80066B8C_p1(angles, object + 4, object + 484);
            xport_draft_host_sub_800667CC_p3(object + 116, r_u16(object + 436), angles);
        }
    }
    if (!RandomRange(64))
    {
        value = RandomRange(256) - 128;
        w_u16(object + 132, 0);
        w_u16(object + 134, value);
        w_u16(object + 136, 0);
    }
    if (!r_u32(object + 108))
    {
        value = RandomRange(512);
        phase = (r_u16(object + 496) + value) & 4095;
        w_u16(object + 496, phase);
        height = (uint32)(sint32)(sint16)r_u16(0x800F863C + phase * 4);
        y = r_u32(object + 8);
        height <<= 5;
        w_u32(object + 500, height);
        w_u32(object + 8, y + height);
    }
    return sub_8004B9A4(object);
}

uint32 FloatingHazard::ExecuteCommand(uint32 command)
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 cursor, value, table, receiver, target;
    FUNCTION_MARKER(0x8005AB58, "SLUS_003.73");
    command &= 65535;
    if (command != 0x4221)
        return Enemy::ExecuteCommand(command);
    cursor = r_u32(object + 400);
    value = r_u16(cursor);
    w_u32(object + 400, cursor + 2);
    if (value & 0x2000)
    {
        table = r_u32(object + 68);
        receiver = object + (uint32)(sint32)(sint16)r_u16(table + 80);
        target = r_u32(table + 84);
        value = DispatchObjectParameterGetter(target, receiver, value & 65535);
    }
    ReadRecordPosition((uint32 *)psx_addr(object + 484, 12), value & 65535);
    value = r_u32(object + 396);
    w_u32(object + 396, value | 1);
    return 1;
}

extern "C" uint32 sub_8005A630(uint32 object, uint32 data, uint32 index)
{
    FUNCTION_MARKER(0x8005A630, "SLUS_003.73");
    void *native_instance_storage = AllocateNativeObjectStorage(object, sizeof(FloatingHazard));
    new (native_instance_storage) FloatingHazard((const uint8 *)psx_addr(data, 1), index);
    return object;
}

extern "C" uint32 sub_8005A74C(uint32 object, uint32 reason)
{
    FUNCTION_MARKER(0x8005A74C, "SLUS_003.73");
    return FloatingHazard::Attach(object).FloatingHazard::Destroy(reason);
}

extern "C" uint32 sub_8005A7B0(uint32 object, uint32 damage)
{
    FUNCTION_MARKER(0x8005A7B0, "SLUS_003.73");
    return FloatingHazard::Attach(object).FloatingHazard::Damage(damage, 0, 0);
}

extern "C" uint32 sub_8005A860(uint32 object)
{
    FUNCTION_MARKER(0x8005A860, "SLUS_003.73");
    return FloatingHazard::Attach(object).FloatingHazard::Update();
}

extern "C" uint32 sub_8005AADC(uint32 object)
{
    FUNCTION_MARKER(0x8005AADC, "SLUS_003.73");
    return FloatingHazard::Attach(object).UpdateHeight();
}

extern "C" uint32 sub_8005AB58(uint32 object, uint32 command)
{
    FUNCTION_MARKER(0x8005AB58, "SLUS_003.73");
    return FloatingHazard::Attach(object).FloatingHazard::ExecuteCommand(command);
}

BallisticHazard::BallisticHazard(const uint32 *position, uint32 kind, uint32 flags, uint32 parameter, sint32 vertical_speed) : Enemy()
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 x, y, z, value, radius, table, player, coefficient, floor;
    FUNCTION_MARKER(0x8005ABF8, "SLUS_003.73");
    w_u32(object + 68, 0x800A28B0);
    x = ReadMovingRecordPosition(position, 0);
    y = ReadMovingRecordPosition(position, 1);
    z = ReadMovingRecordPosition(position, 2);
    w_u32(object + 240, x);
    w_u32(object + 244, y);
    w_u32(object + 248, z);
    x = r_u32(object + 240);
    y = r_u32(object + 244);
    z = r_u32(object + 248);
    w_u32(object + 4, x);
    w_u32(object + 8, y);
    w_u32(object + 12, z);
    if (kind == 2)
    {
        SetModelBank((const char *)psx_addr(0x800FF574, 1));
        value = FindModelResourceIndex(0x717FB6B2, r_u8(0x800FF645));
        w_u16(object + 22, value);
        w_u32(object + 120, (uint32)vertical_speed << 12);
        w_u16(object + 200, 128);
    }
    else if ((sint32)kind >= 0 && (sint32)kind <= 3)
    {
        SetModelBank((const char *)psx_addr(r_u32(0x800FF62C), 1));
        value = FindModelResourceIndex(0x2D990A1E, r_u8(0x800FF644));
        w_u16(object + 22, value);
        w_u32(object + 120, (uint32)vertical_speed << 12);
        w_u16(object + 18, RandomRange(4096));
        value = 256 - RandomRange(128);
        w_u8(object + 144, 16);
        w_u16(object + 200, 128);
        w_u16(object + 132, value);
        w_u16(object + 436, kind == 3 ? 48 : 32);
        w_u16(object + 438, 256);
        w_u16(object + 440, kind == 3 ? 4 : 8);
        if (kind == 1)
        {
            radius = RandomRange(512);
            table = 0x800F863C + (RandomRange(4096) & 4095) * 4;
            coefficient = (uint32)(sint32)(sint16)r_u16(table);
            player = r_u32(0x800FF5A0);
            value = r_u32(player + 4);
            w_u32(object + 4, value + radius * coefficient);
            coefficient = (uint32)(sint32)(sint16)r_u16(table + 2);
            value = r_u32(player + 12);
            w_u32(object + 12, value + radius * coefficient);
            x = r_u32(object + 4);
            y = r_u32(object + 8);
            z = r_u32(object + 12);
            w_u32(object + 240, x);
            w_u32(object + 244, y);
            w_u32(object + 248, z);
        }
    }
    w_u16(object, r_u16(object) & 0xFFFD);
    AddToList((uint32 *)psx_addr(0x800FF4E8, 4));
    w_u8(object + 129, 2);
    w_u8(object + 130, 5);
    w_u8(object + 131, 2);
    w_u16(object + 212, 0);
    w_u16(object + 58, 410);
    w_u16(object + 496, flags);
    w_u32(object + 500, parameter);
    w_u16(object + 78, r_u16(object + 78) & 0xFFEF);
    if (flags & 5)
    {
        floor = FindGroundHeight((const uint32 *)psx_addr(object + 4, 12), 0, 8192);
        if (floor == (uint32)-1)
        {
            Enemy::Finish();
            return;
        }
        w_u32(object + 488, floor);
        EnableShadow();
        value = r_u32(object + 8);
        x = r_u32(object + 4);
        value = floor - value;
        w_u32(object + 184, x);
        z = r_u32(object + 12);
        w_u16(object + 204, (uint32)((sint32)value >> 12));
        w_u32(object + 188, floor);
        w_u32(object + 192, z);
    }
    else if (flags & 2)
    {
        EnableShadow();
        floor = r_u32(0x800FF37C);
        value = r_u32(object + 8);
        x = r_u32(object + 4);
        w_u16(object + 204, (uint32)((sint32)(floor - value) >> 12));
        w_u32(object + 184, x);
        z = r_u32(object + 12);
        w_u32(object + 188, floor - 65536);
        w_u32(object + 192, z);
    }
}

uint32 BallisticHazard::Destroy(uint32 reason)
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 result;
    FUNCTION_MARKER(0x8005AF34, "SLUS_003.73");
    w_u32(object + 68, 0x800A28B0);
    RemoveFromList((uint32 *)psx_addr(0x800FF4E8, 4));
    result = Enemy::Destroy(0);
    return (reason & 1) ? ReleaseObjectStorage(object) : result;
}

uint32 BallisticHazard::Damage(uint32, const sint32 *, uint32)
{
    FUNCTION_MARKER(0x8005AF98, "SLUS_003.73");
    return 0;
}

extern "C" uint32 sub_8005ABF8(uint32 object, uint32 position, uint32 kind, uint32 flags, uint32 parameter, sint32 vertical_speed)
{
    FUNCTION_MARKER(0x8005ABF8, "SLUS_003.73");
    void *native_instance_storage = AllocateNativeObjectStorage(object, sizeof(BallisticHazard));
    new (native_instance_storage) BallisticHazard((const uint32 *)psx_addr(position, 12), kind, flags, parameter, vertical_speed);
    return object;
}

extern "C" uint32 sub_8005AF34(uint32 object, uint32 reason)
{
    FUNCTION_MARKER(0x8005AF34, "SLUS_003.73");
    return BallisticHazard::Attach(object).BallisticHazard::Destroy(reason);
}

extern "C" uint32 sub_8005AF98(uint32 object, uint32 damage)
{
    FUNCTION_MARKER(0x8005AF98, "SLUS_003.73");
    return BallisticHazard::Attach(object).BallisticHazard::Damage(damage, 0, 0);
}

uint32 BallisticHazard::FinishFromTable()
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 table = r_u32(object + 68);
    uint32 receiver = object + (uint32)(sint32)(sint16)r_u16(table + 16);
    uint32 target = r_u32(table + 20);
    return DispatchObjectFinish(target, receiver);
}

static uint32 DamageBallisticHazardPlayer(uint32 damage)
{
    uint32 player = r_u32(0x800FF5A0);
    return ApplyObjectDamage(player, damage, (const sint32 *)psx_addr(0x800A71CC, 12), 0);
}

uint32 BallisticHazard::Update()
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 child, flags, result, y, floor, parameter, distance, threshold, damage;
    uint32 value, point[3], velocity[3], table, random, product, kind, name, index;
    sint32 quotient;
    FUNCTION_MARKER(0x8005AFA0, "SLUS_003.73");
    child = r_u32(object + 196);
    if (child)
        w_u16(child + 64, 128);
    Enemy::IntegrateMotion();
    flags = r_u16(object + 496);
    result = flags & 2;
    if (flags & 5)
    {
        y = r_u32(object + 8);
        floor = r_u32(object + 488);
        if ((sint32)y >= (sint32)floor)
        {
            if (flags == 4)
            {
                w_u32(object + 8, floor);
                SpawnFireCycle((const uint32 *)psx_addr(object + 4, 12), 180, 1, 2);
                parameter = r_u32(object + 500);
                SpawnExplosionRing((const uint32 *)psx_addr(object + 4, 12), (uint32)((sint32)parameter >> 1), 255, 128, 0, 4, 1, 100);
                distance = r_u32(object + 220);
                parameter = r_u32(object + 500);
                if (distance < parameter)
                {
                    damage = ScaleDamageForDifficulty(20);
                    DamageBallisticHazardPlayer(damage);
                }
                child = AllocateEffectStorage(120);
                if (child)
                {
                    parameter = r_u32(object + 500);
                    velocity[0] = r_u32(0x800A71CC);
                    velocity[1] = r_u32(0x800A71D0);
                    velocity[2] = r_u32(0x800A71D4);
                    void *dram_addr = AllocateNativeObjectStorage(child, sizeof(ImpactRing));
                    new (dram_addr) ImpactRing(velocity);
                    ImpactRing::Attach(child).SetGeometry((const uint32 *)psx_addr(object + 4, 12), 0, 0);
                    value = (uint32)((sint32)parameter / 16);
                    w_u16(child + 10, 16);
                    w_u16(child + 116, value);
                    quotient = (sint16)value;
                    w_u16(child + 118, (uint32)(quotient / 2));
                }
            }
            else if (flags == 1)
            {
                if (r_u32(0x800FF378) == 1)
                    PlayPositionedSound(59, (const uint32 *)psx_addr(object + 4, 12), 0);
                for (index = 0; index < r_u16(object + 440); ++index)
                {
                    random = RandomRange(4096);
                    floor = r_u32(object + 488);
                    w_u32(object + 8, floor - 0x20000);
                    table = 0x800F863C + (random & 4095) * 4;
                    value = (uint32)(sint32)(sint16)r_u16(table);
                    parameter = r_u32(object + 500);
                    product = value * parameter;
                    velocity[0] = (uint32)((sint32)product / 8);
                    random = RandomRange(32);
                    value = (uint32)(sint32)(sint16)r_u16(table + 2);
                    parameter = r_u32(object + 500);
                    product = value * parameter;
                    velocity[1] = (0 - 16 - random) << 12;
                    velocity[2] = (uint32)((sint32)product / 8);
                    child = AllocateZeroedObject(320);
                    if (child)
                    {
                        kind = FindModelResourceIndex(0x51FBD441, r_u8(0x800FF644)) & 65535;
                        floor = r_u32(object + 488);
                        name = r_u32(0x800FF62C);
                        void *dram_addr = AllocateNativeObjectStorage(child, sizeof(Debris));
                        new (dram_addr) Debris((const uint32 *)psx_addr(object + 4, 12), velocity, kind, floor, 193, 24, psx_addr(name, 1));
                    }
                    // Skip the missing child under the approved allocation-failure contract
                    value = r_u16(object + 436) >> 3;
                    if (child)
                        w_u8(child + 297, value);
                }
            }
            return FinishFromTable();
        }
        value = r_u32(object + 188);
        distance = r_u32(object + 220);
        threshold = r_u16(object + 438);
        result = (uint32)((sint32)(value - y) >> 12);
    }
    else
    {
        if (!result)
            return result;
        y = r_u32(object + 8);
        floor = r_u32(0x800FF37C);
        if ((sint32)y >= (sint32)floor)
        {
            point[0] = r_u32(object + 4);
            point[1] = r_u32(object + 8);
            point[2] = r_u32(object + 12);
            point[1] -= 286720;
            CreateSplash(point);
            PlayPositionedSound(57, (const uint32 *)psx_addr(object + 4, 12), 0);
            return FinishFromTable();
        }
        value = r_u32(object + 188);
        threshold = r_u16(object + 438);
        distance = r_u32(object + 220);
        result = (uint32)((sint32)(value - y) >> 12);
    }
    w_u16(object + 202, result);
    if (distance < threshold)
    {
        result = r_u32(0x800FF5A0);
        if (result)
        {
            damage = ScaleDamageForDifficulty(r_u16(object + 436));
            return DamageBallisticHazardPlayer(damage);
        }
    }
    return result;
}

extern "C" uint32 sub_8005AFA0(uint32 object)
{
    FUNCTION_MARKER(0x8005AFA0, "SLUS_003.73");
    return BallisticHazard::Attach(object).BallisticHazard::Update();
}

FloatingHazard &FloatingHazard::Attach(uint32 object)
{
    return *static_cast<FloatingHazard *>(FindNativeObjectStorage(object));
}

BallisticHazard &BallisticHazard::Attach(uint32 object)
{
    return *static_cast<BallisticHazard *>(FindNativeObjectStorage(object));
}
