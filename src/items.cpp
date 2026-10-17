#include "visual_object.h"
#include "object_damage.h"
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
    this->model.model_idx = value;
    AddToList(visual_lists.actors);
    w_u16(object + 212, 64);
    w_u16(object + 218, 20);
    this->kind = 204;
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
    this->record_idx = index;
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
        motion.acceleration[0] = x;
        motion.acceleration[1] = y;
        motion.acceleration[2] = z;
        child = AllocateEffectStorage(176);
        if (child)
        {
            void *dram_addr = AllocateNativeObjectStorage(child, sizeof(OscillatingRing));
            new (dram_addr) OscillatingRing((const uint32 *)this->model.pos, 64, 32, 64, 32, 96, 32, 32, 64, 0);
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
    this->model.model_idx = value;
    AddToList(visual_lists.actors);
    w_u16(object + 212, 64);
    w_u16(object + 218, 20);
    this->kind = 204;
    random_x = RandomRange(128) - 64;
    random_y = RandomRange(128) - 64;
    random_z = RandomRange(128) - 64;
    motion.angular_velocity[0] = random_x;
    motion.angular_velocity[1] = random_y;
    motion.angular_velocity[2] = random_z;
    x = ReadItemsPositionWord(position, 0);
    y = ReadItemsPositionWord(position, 1);
    z = ReadItemsPositionWord(position, 2);
    pos_history.saved[0] = x;
    pos_history.saved[1] = y;
    pos_history.saved[2] = z;
    x = pos_history.saved[0];
    y = pos_history.saved[1];
    z = pos_history.saved[2];
    this->model.pos[0] = x;
    this->model.pos[1] = y;
    this->model.pos[2] = z;
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
        motion.acceleration[0] = x;
        motion.acceleration[1] = y;
        motion.acceleration[2] = z;
        w_u16(object + 306, 0);
        w_u8(object + 304, 0);
        EnableShadow();
        value = r_u16(0x800EC530);
        w_u16(object + 204, value << 1);
        x = this->model.pos[0];
        y = this->model.pos[1];
        z = this->model.pos[2];
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
    RemoveFromList(visual_lists.actors);
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

uint32 Items::Damage(uint32 damage, const sint32 *, uint32)
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
    uint32 trig_angle;
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
    AddAngleVector((uint16 *)this->model.angles, (const uint16 *)motion.angular_velocity);
    if (!r_u8(object + 304))
    {
        if ((sint32)motion.velocity[1] <= 0)
        {
            AddVectors((uint32 *)this->model.pos, (const uint32 *)motion.velocity);
            AddVectors((uint32 *)motion.velocity, (const uint32 *)motion.acceleration);
        }
        else
        {
            value = this->model.pos[1];
            ground = r_u32(object + 296);
            if ((sint32)value < (sint32)ground)
            {
                AddVectors((uint32 *)this->model.pos, (const uint32 *)motion.velocity);
                AddVectors((uint32 *)motion.velocity, (const uint32 *)motion.acceleration);
                value = this->model.pos[1];
                ground = r_u32(object + 296);
                if ((sint32)value >= (sint32)ground)
                {
                    w_u8(object + 304, 1);
                    motion.velocity[2] = 0;
                    motion.velocity[1] = 0;
                    motion.velocity[0] = 0;
                    motion.acceleration[2] = 0;
                    motion.acceleration[1] = 0;
                    motion.acceleration[0] = 0;
                }
            }
        }
        flags = r_u8(object + 304);
    }
    else
        flags = 1;
    if (flags)
    {
        trig_angle = r_u16(object + 306) & 0xFFF;
        phase = r_u16(object + 306);
        sine = rsin(trig_angle);
        ground = r_u32(object + 296);
        value = ground + ((uint32)sine << 6);
        w_u16(object + 306, phase + 64);
        this->model.pos[1] = value;
    }
    value = r_u32(object + 188);
    height = (sint32)(value - this->model.pos[1]) >> 12;
    hero = visual_lists.players;
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
        flags = this->model.flags;
        scale = this->model.scale[1];
        this->model.flags = flags | 512;
        value = this->model.scale[0];
        this->model.scale[1] = scale + 380;
        remaining = r_u32(object + 300) - 1;
        this->model.scale[0] = value + 380;
        result = this->model.scale[2] + 380;
        w_u32(object + 300, remaining);
        this->model.scale[2] = result;
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
    return child ? OscillatingRing::Attach(child).SetPosition((const uint32 *)this->model.pos) : result;
explode:
    SpawnExplosionRing((const uint32 *)this->model.pos, 128, 255, 128, 0, 4, 1, 100);
    PlayPositionedSound(24, (const uint32 *)this->model.pos, 4);
    if (distance < (sint32)r_u16(object + 310))
    {
        hero = visual_lists.players;
        damage = r_u16(object + 308);
        receiver = hero;
        ApplyObjectDamage(receiver, damage, (const sint32 *)psx_addr(0x800A71CC, 12), 0);
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
    return Items::Attach(object).Items::Damage(damage, 0, 0);
}

uint32 sub_8005B890(uint32 object)
{
    FUNCTION_MARKER(0x8005B890, "SLUS_003.73");
    return Items::Attach(object).Items::Update();
}

