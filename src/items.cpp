#include "game_sdk_bindings.h"
extern "C" {
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
}
#include "items.h"
#include "native_object_storage.h"
#include "game_object_allocation.h"
#include "model_resources.h"
#include "game_random.h"
#include "effect_lifecycle.h"
#include "oscillating_ring.h"
#include "angle_math.h"
#include "vector_arithmetic.h"
#include "ring_burst_effect.h"
#include "audio_session.h"
#include <new>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



static uint32 ReadItemsPositionWord(const uint32 *position, uint32 axis)
{
    uint32 address = GuestMemoryAddress(position);
    if (address)
        return r_u32(address + axis * 4);
    uint32 value;
    memcpy(&value, (const uint8 *)position + axis * 4, 4);
    return value;
}

Items::Items(const uint8 *data, uint32 index) : VisualObject()
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 value, cursor, parameter, ground, x, y, z, child;
    FUNCTION_MARKER(0x8005B3E0, "SLUS_003.73");
    w_u32(object + 68, 0x800A2870);
    SetModelBank((const char *)psx_addr(0x800FF574, 1));
    value = r_u8(0x800FF645);
    value = FindModelResourceIndex(0x717FB6B2, value);
    w_u16(object + 22, value);
    AddToList((uint32 *)psx_addr(0x800FF4E8, 4));
    w_u16(object + 212, 64);
    w_u16(object + 218, 20);
    w_u16(object + 58, 204);
    if (!GuestMemoryAddress(data))
    {
        // TODO Bind native item records before storing their persistent cursor
        GameDiagnostic("TODO Items record cursor requires mapped game memory\n");
        abort();
    }
    const uint8 *next = ReadPosition(data);
    next = ReadAngles(next);
    cursor = GuestMemoryAddress(next);
    w_u32(object + 320, cursor);
    w_u16(object + 310, 512);
    w_u16(object + 312, 256);
    w_u16(object + 314, 16);
    w_u16(object + 316, 360);
    w_u16(object + 214, index);
    w_u16(object + 308, 20);
    w_u16(object + 318, 128);
    for (;;)
    {
        cursor = r_u32(object + 320);
        if (r_u16(cursor) == 0x4100)
            break;
        w_u32(object + 320, cursor + 2);
        parameter = r_u16(cursor + 2);
        w_u32(object + 320, cursor + 4);
        value = r_u16(cursor + 4);
        w_u32(object + 320, cursor + 6);
        w_u16(object + 308 + parameter * 2, value);
    }
    ground = sub_80067A18(object + 4, 0, 8192);
    if (ground == 0xFFFFFFFF)
        VisualObject::Finish();
    else
    {
        value = r_u16(object + 318);
        w_u32(object + 296, ground - (value << 12));
        x = r_u32(0x800A6844);
        y = r_u32(0x800A6848);
        z = r_u32(0x800A684C);
        w_u32(object + 116, x);
        w_u32(object + 120, y);
        w_u32(object + 124, z);
        child = AllocateEffectStorage(176);
        if (child)
        {
            void *dram_addr = AllocateNativeObjectStorage(child, sizeof(OscillatingRing));
            new (dram_addr) OscillatingRing((const uint32 *)psx_addr(object + 4, 12), 64, 32, 64, 32, 96, 32, 32, 64, 0);
        }
        w_u32(object + 324, child);
        w_u8(child + 66, 1);
        w_u16(object + 306, 0);
        w_u8(object + 304, 0);
    }
}

Items::Items(const uint32 *position, uint32 first, uint32 second, uint32 value312, uint32 value314, uint32 value316, uint32 value318) : VisualObject()
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 bank, value, random_x, random_y, random_z, x, y, z, ground;
    FUNCTION_MARKER(0x8005B5D8, "SLUS_003.73");
    w_u32(object + 68, 0x800A2870);
    SetModelBank((const char *)psx_addr(0x800FF574, 1));
    bank = r_u8(0x800FF645);
    value = FindModelResourceIndex(0x717FB6B2, bank);
    w_u16(object + 22, value);
    AddToList((uint32 *)psx_addr(0x800FF4E8, 4));
    w_u16(object + 212, 64);
    w_u16(object + 218, 20);
    w_u16(object + 58, 204);
    random_x = RandomRange(128) - 64;
    random_y = RandomRange(128) - 64;
    random_z = RandomRange(128) - 64;
    w_u16(object + 132, random_x);
    w_u16(object + 134, random_y);
    w_u16(object + 136, random_z);
    x = ReadItemsPositionWord(position, 0);
    y = ReadItemsPositionWord(position, 1);
    z = ReadItemsPositionWord(position, 2);
    w_u32(object + 240, x);
    w_u32(object + 244, y);
    w_u32(object + 248, z);
    x = r_u32(object + 240);
    y = r_u32(object + 244);
    z = r_u32(object + 248);
    w_u32(object + 4, x);
    w_u32(object + 8, y);
    w_u32(object + 12, z);
    w_u16(object + 308, first);
    w_u16(object + 312, value312);
    w_u16(object + 314, value314);
    w_u16(object + 316, value316);
    w_u16(object + 318, value318);
    w_u16(object + 310, second);
    ground = sub_80067A18(object + 4, 128, 8192);
    if (ground == 0xFFFFFFFF)
        VisualObject::Finish();
    else
    {
        value = r_u16(object + 318);
        w_u32(object + 296, ground - (value << 12));
        x = r_u32(0x800A6844);
        y = r_u32(0x800A6848);
        z = r_u32(0x800A684C);
        w_u32(object + 116, x);
        w_u32(object + 120, y);
        w_u32(object + 124, z);
        w_u16(object + 306, 0);
        w_u8(object + 304, 0);
        EnableShadow();
        value = r_u16(0x800EC530);
        w_u16(object + 204, value << 1);
        x = r_u32(object + 4);
        y = r_u32(object + 8);
        z = r_u32(object + 12);
        w_u32(object + 184, x);
        w_u32(object + 188, y);
        w_u32(object + 192, z);
        w_u32(object + 188, ground);
    }
}

uint32 Items::Destroy(uint32 reason)
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 child, table, receiver, target;
    FUNCTION_MARKER(0x8005B7D4, "SLUS_003.73");
    w_u32(object + 68, 0x800A2870);
    RemoveFromList((uint32 *)psx_addr(0x800FF4E8, 4));
    child = r_u32(object + 324);
    if (child)
    {
        table = r_u32(child + 68);
        receiver = child + (uint32)(sint32)(sint16)r_u16(table + 8);
        target = r_u32(table + 12);
        (void)apocalypse_object_cleanup(target, receiver, 3);
    }
    VisualObject::Destroy(0);
    return (reason & 1) ? ReleaseObjectStorage(object) : 0;
}

uint32 Items::Damage(uint32 damage)
{
    const uint32 object = NativeObjectGuestAddress(this);
    sint32 health = (sint16)r_u16(object + 218);
    uint32 remaining = r_u16(object + 218) - damage;
    FUNCTION_MARKER(0x8005B864, "SLUS_003.73");
    if (health > 0)
    {
        w_u16(object + 218, remaining);
        if ((sint32)(remaining << 16) <= 0)
            w_u32(object + 300, 1);
    }
    return 1;
}

uint32 Items::Update()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 timer, child, value, ground, phase, hero, table, target, receiver, damage;
    uint32 result, flags, scale, remaining;
    sint32 distance, height, sine;
    FUNCTION_MARKER(0x8005B890, "SLUS_003.73");
    timer = r_u16(object + 316);
    if (timer)
    {
        timer = (timer - 1) & 0xFFFF;
        w_u16(object + 316, timer);
        if (!timer)
        {
            value = r_u16(object + 314);
            child = r_u32(object + 324);
            w_u32(object + 300, value);
            if (child)
            {
                OscillatingRing::Attach(child).SetCenterColor(128, 96, 32);
                OscillatingRing::Attach(r_u32(object + 324)).SetRingColors(64, 0, 0);
            }
        }
    }
    AddAngleVector((uint16 *)psx_addr(object + 16, 6), (const uint16 *)psx_addr(object + 132, 6));
    if (!r_u8(object + 304))
    {
        if ((sint32)r_u32(object + 108) <= 0)
        {
            AddVectors((uint32 *)psx_addr(object + 4, 12), (const uint32 *)psx_addr(object + 104, 12));
            AddVectors((uint32 *)psx_addr(object + 104, 12), (const uint32 *)psx_addr(object + 116, 12));
        }
        else
        {
            value = r_u32(object + 8);
            ground = r_u32(object + 296);
            if ((sint32)value < (sint32)ground)
            {
                AddVectors((uint32 *)psx_addr(object + 4, 12), (const uint32 *)psx_addr(object + 104, 12));
                AddVectors((uint32 *)psx_addr(object + 104, 12), (const uint32 *)psx_addr(object + 116, 12));
                value = r_u32(object + 8);
                ground = r_u32(object + 296);
                if ((sint32)value >= (sint32)ground)
                {
                    w_u8(object + 304, 1);
                    w_u32(object + 112, 0);
                    w_u32(object + 108, 0);
                    w_u32(object + 104, 0);
                    w_u32(object + 124, 0);
                    w_u32(object + 120, 0);
                    w_u32(object + 116, 0);
                }
            }
        }
        flags = r_u8(object + 304);
    }
    else
        flags = 1;
    if (flags)
    {
        value = 0x800F863C + 4 * (r_u16(object + 306) & 0xFFF);
        phase = r_u16(object + 306);
        sine = (sint16)r_u16(value);
        ground = r_u32(object + 296);
        value = ground + ((uint32)sine << 6);
        w_u16(object + 306, phase + 64);
        w_u32(object + 8, value);
    }
    value = r_u32(object + 188);
    height = (sint32)(value - r_u32(object + 8)) >> 12;
    hero = r_u32(0x800FF5A0);
    w_u16(object + 202, (uint32)height);
    if (hero)
    {
        distance = (sint32)r_u32(object + 220);
        if (distance < (sint32)r_u16(object + 312))
            goto explode;
    }
    else
        distance = 65535;
    if (r_u32(object + 300))
    {
        flags = r_u16(object);
        scale = r_u16(object + 38);
        w_u16(object, flags | 512);
        value = r_u16(object + 36);
        w_u16(object + 38, scale + 380);
        remaining = r_u32(object + 300) - 1;
        w_u16(object + 36, value + 380);
        result = r_u16(object + 40) + 380;
        w_u32(object + 300, remaining);
        w_u16(object + 40, result);
        if (!remaining)
            goto explode;
    }
    else
    {
        result = distance < (sint32)r_u16(object + 310);
        if (result)
        {
            result = r_u16(object + 314);
            child = r_u32(object + 324);
            w_u32(object + 300, result);
            if (!child)
                return result;
            OscillatingRing::Attach(child).SetCenterColor(128, 96, 32);
            result = OscillatingRing::Attach(r_u32(object + 324)).SetRingColors(64, 0, 0);
        }
    }
    child = r_u32(object + 324);
    return child ? OscillatingRing::Attach(child).SetPosition((const uint32 *)psx_addr(object + 4, 12)) : result;
explode:
    SpawnExplosionRing((const uint32 *)psx_addr(object + 4, 12), 128, 255, 128, 0, 4, 1, 100);
    PlayPositionedSound(24, (const uint32 *)psx_addr(object + 4, 12), 4);
    if (distance < (sint32)r_u16(object + 310))
    {
        hero = r_u32(0x800FF5A0);
        table = r_u32(hero + 68);
        damage = r_u16(object + 308);
        receiver = hero + (uint32)(sint32)(sint16)r_u16(table + 48);
        target = r_u32(table + 52);
        apocalypse_object_virtual52(target, receiver, damage, 0x800A71CC, 0);
    }
    table = r_u32(object + 68);
    receiver = object + (uint32)(sint32)(sint16)r_u16(table + 16);
    target = r_u32(table + 20);
    if (target != 0x80062F0C)
    {
        GameDiagnostic("TODO Class204 finish callback %08X receiver %08X\n", target, receiver);
        abort();
    }
    return VisualObject::Attach(receiver).VisualObject::Finish();
}

Items &Items::Attach(uint32 object)
{
    return *static_cast<Items *>(FindNativeObjectStorage(object));
}

// Compatibility entry points retain the guest call contract
uint32 sub_8005B3E0(uint32 object, uint32 data, uint32 index)
{
    FUNCTION_MARKER(0x8005B3E0, "SLUS_003.73");
    void *native_instance_storage = AllocateNativeObjectStorage(object, sizeof(Items));
    new (native_instance_storage) Items((const uint8 *)psx_addr(data, 1), index);
    return object;
}

uint32 sub_8005B5D8(uint32 object, uint32 position, uint32 first, uint32 second, uint32 value312, uint32 value314, uint32 value316, uint32 value318)
{
    FUNCTION_MARKER(0x8005B5D8, "SLUS_003.73");
    void *native_instance_storage = AllocateNativeObjectStorage(object, sizeof(Items));
    new (native_instance_storage) Items((const uint32 *)psx_addr(position, 12), first, second, value312, value314, value316, value318);
    return object;
}

uint32 sub_8005B7D4(uint32 object, uint32 reason)
{
    FUNCTION_MARKER(0x8005B7D4, "SLUS_003.73");
    return Items::Attach(object).Items::Destroy(reason);
}

uint32 sub_8005B864(uint32 object, uint32 damage)
{
    FUNCTION_MARKER(0x8005B864, "SLUS_003.73");
    return Items::Attach(object).Items::Damage(damage);
}

uint32 sub_8005B890(uint32 object)
{
    FUNCTION_MARKER(0x8005B890, "SLUS_003.73");
    return Items::Attach(object).Items::Update();
}

