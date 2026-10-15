extern "C"
{
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
}
#include "fire_cycle.h"
#include "effect_lifecycle.h"
#include "game_random.h"
#include "vector_distance.h"
#include "effect_sprite.h"
#include "native_object_storage.h"
#include <new>
#include <string.h>

FireCycle::FireCycle(const uint32 *position, uint32 size, uint32 enabled, uint32 count) : EffectController()
{
    const uint32 object = NativeObjectGuestAddress(this);
    const uint32 address = GuestMemoryAddress(position);
    uint32 values[3], axis;
    FUNCTION_MARKER(0x8001DB68, "SLUS_003.73");
    w_u32(object + 68, 0x800A116C);
    for (axis = 0; axis < 3; ++axis)
    {
        if (address)
            values[axis] = r_u32(address + axis * 4);
        else
            memcpy(&values[axis], (const uint8 *)position + axis * 4, 4);
    }
    w_u32(object + 24, values[0]);
    w_u32(object + 28, values[1]);
    w_u32(object + 32, values[2]);
    w_u16(object + 10, size);
    w_u8(object + 75, enabled);
    w_u8(object + 74, count);
    SpawnSprite(0);
}

uint32 FireCycle::Destroy(uint32 reason)
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 index, child, table, target, receiver;
    FUNCTION_MARKER(0x8001DBFC, "SLUS_003.73");
    w_u32(object + 68, 0x800A116C);
    for (index = 0; index < 20; ++index)
    {
        child = r_u32(object + 76 + index * 4);
        if (child)
        {
            table = r_u32(child + 68);
            receiver = child + (uint32)(sint32)(sint16)r_u16(table + 8);
            target = r_u32(table + 12);
            apocalypse_object_cleanup(target, receiver, 3);
        }
    }
    EffectController::Destroy(0);
    return (reason & 1) ? ReleaseEffectStorage(object) : 0;
}

uint32 FireCycle::SpawnSprite(uint32 index)
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 position[3], value, child, scale, clut, texture;
    FUNCTION_MARKER(0x8001DCA8, "SLUS_003.73");
    value = r_u16(object + 8);
    w_u16(object + 72, value);
    position[0] = r_u32(object + 24);
    position[1] = r_u32(object + 28);
    position[2] = r_u32(object + 32);
    value = RandomRange(200);
    position[0] += (value - 100) << 12;
    value = RandomRange(200);
    w_u32(0x800FF3AC, 0);
    position[2] += (value - 100) << 12;
    child = AllocateEffectStorage(120);
    if (child)
    {
        void *dram_addr = AllocateNativeObjectStorage(child, sizeof(AnimatedSprite));
        new (dram_addr) AnimatedSprite(position, (const char *)psx_addr(0x800A10CC, 1), 3072, 1, 0, (uint32)-1);
    }
    w_u32(0x800FF3AC, 1);
    w_u8(child + 66, 1);
    w_u16(child + 92, 0);
    value = RandomRange(2048);
    scale = value + 3072;
    value = RandomRange(1024);
    scale += (value + 1024) << 16;
    w_u32(child + 104, scale);
    AnimatedSprite::Attach(child).AddFlags(8);
    value = RandomRange(4096);
    clut = r_u16(0x800FF698);
    w_u16(child + 96, value);
    w_u16(child + 10, 4096);
    texture = r_u32(0x800FF694);
    clut = (clut << 6) | (((uint32)((sint32)texture >> 4)) & 63);
    value = r_u32(child + 76);
    w_u16(child + 110, clut);
    w_u32(child + 76, value & 0xFF000000);
    value = object + index * 4;
    w_u32(value + 76, child);
    return value;
}

uint32 FireCycle::Update()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 player, player_y, y, distance, table, target, receiver, damage;
    uint32 index, slot, child, value, color, random, clut, texture, base, scale, width, height, age, lifetime;
    sint32 empty = -1, delta;
    FUNCTION_MARKER(0x8001DE24, "SLUS_003.73");
    if (r_u8(object + 75))
    {
        player = r_u32(0x800FF5A0);
        if (player)
        {
            player_y = r_u32(player + 8);
            y = r_u32(object + 28);
            if ((sint32)player_y < (sint32)y)
            {
                distance = MeasureHorizontalDistance((const uint32 *)psx_addr(object + 24, 12), (const uint32 *)psx_addr(player + 4, 12));
                if (distance < 256)
                {
                    player = r_u32(0x800FF5A0);
                    table = r_u32(player + 68);
                    receiver = player + (uint32)(sint32)(sint16)r_u16(table + 48);
                    damage = r_u8(object + 74);
                    target = r_u32(table + 52);
                    apocalypse_object_virtual52(target, receiver, damage, 0x800A71CC, 0);
                }
            }
        }
    }
    for (index = 0; index < 20; ++index)
    {
        slot = object + 76 + index * 4;
        child = r_u32(slot);
        if (!child)
        {
            if (empty == -1)
                empty = (sint32)index;
            continue;
        }
        value = r_u16(child + 10);
        if (value >= 97)
        {
            value = 4096 - r_u16(child + 10);
            if ((sint32)value < 1024)
            {
                value = (uint32)((sint32)value >> 3);
                color = r_u32(child + 76);
                w_u32(child + 76, (color & 0xFF000000) | (value << 16) | (value << 8) | value);
            }
            random = RandomRange(64);
            child = r_u32(slot);
            value = r_u16(child + 10);
            w_u16(child + 10, value - (random + 32));
            clut = r_u32(0x800FF698);
            texture = r_u32(0x800FF694);
            child = r_u32(slot);
            base = (clut << 6) | ((uint32)((sint32)texture >> 4) & 63);
            clut = r_u16(child + 110);
            delta = (sint32)(clut - base);
            if (delta < 2)
            {
                if (!RandomRange(9))
                {
                    child = r_u32(slot);
                    value = r_u16(child + 110);
                    w_u16(child + 110, value + 1);
                }
            }
            else if (delta < 15 && !RandomRange(3))
            {
                child = r_u32(slot);
                value = r_u16(child + 110);
                w_u16(child + 110, value + 1);
                if (delta == 12)
                    w_u8(r_u32(slot) + 109, 64);
            }
        }
        else
        {
            value = r_u8(child + 76);
            if (value < 5)
            {
                table = r_u32(child + 68);
                receiver = child + (uint32)(sint32)(sint16)r_u16(table + 8);
                target = r_u32(table + 12);
                apocalypse_object_cleanup(target, receiver, 3);
                w_u32(slot, 0);
                continue;
            }
            random = RandomRange(3);
            child = r_u32(slot);
            value -= 2 + random;
            color = r_u32(child + 76);
            w_u32(child + 76, (color & 0xFF000000) | (value << 16) | (value << 8) | value);
        }
        child = r_u32(slot);
        scale = r_u32(child + 104);
        width = scale & 65535;
        height = scale >> 16;
        if (width > 1024)
            width -= 83;
        if (height < 2048)
            height += 33;
        value = r_u32(child + 28);
        w_u32(child + 28, value - 49152);
        child = r_u32(slot);
        w_u32(child + 104, width + (height << 16));
        child = r_u32(slot);
        value = r_u16(child + 96);
        w_u16(child + 96, (index & 1) ? value + 123 : value - 123);
    }
    age = r_u16(object + 8);
    lifetime = r_u16(object + 10);
    ++age;
    w_u16(object + 8, age);
    if ((sint32)lifetime < (sint16)age)
        return Finish();
    random = RandomRange(8);
    if (random)
    {
        age = (uint32)(sint32)(sint16)r_u16(object + 8);
        value = r_u16(object + 72);
        if ((sint32)(age - value) < 9)
            return (uint32)-1;
    }
    return empty == -1 ? (uint32)-1 : SpawnSprite((uint32)empty);
}

FireCycle &FireCycle::Attach(uint32 object)
{
    return *static_cast<FireCycle *>(FindNativeObjectStorage(object));
}

extern "C" uint32 sub_8001DB68(uint32 object, uint32 position, uint32 size, uint32 enabled, uint32 count)
{
    FUNCTION_MARKER(0x8001DB68, "SLUS_003.73");
    void *dram_addr = AllocateNativeObjectStorage(object, sizeof(FireCycle));
    new (dram_addr) FireCycle((const uint32 *)psx_addr(position, 12), size, enabled, count);
    return object;
}

extern "C" uint32 apocalypse_fire_cycle_construct_native(uint32 object, const uint32 *position, uint32 size, uint32 enabled, uint32 count)
{
    void *dram_addr = AllocateNativeObjectStorage(object, sizeof(FireCycle));
    new (dram_addr) FireCycle(position, size, enabled, count);
    return object;
}

extern "C" uint32 sub_8001DCA8(uint32 object, uint32 index)
{
    FUNCTION_MARKER(0x8001DCA8, "SLUS_003.73");
    return FireCycle::Attach(object).SpawnSprite(index);
}

extern "C" uint32 sub_8001DBFC(uint32 object, uint32 reason)
{
    FUNCTION_MARKER(0x8001DBFC, "SLUS_003.73");
    return FireCycle::Attach(object).FireCycle::Destroy(reason);
}

extern "C" uint32 sub_8001DE24(uint32 object)
{
    FUNCTION_MARKER(0x8001DE24, "SLUS_003.73");
    return FireCycle::Attach(object).FireCycle::Update();
}

uint32 SpawnFireCycle(const uint32 *position, uint32 size, uint32 enabled, uint32 count)
{
    uint32 object;
    FUNCTION_MARKER(0x8001E400, "SLUS_003.73");
    object = AllocateEffectStorage(156);
    if (object)
    {
        void *dram_addr = AllocateNativeObjectStorage(object, sizeof(FireCycle));
        new (dram_addr) FireCycle(position, size, enabled, count);
    }
    return object;
}

extern "C" uint32 sub_8001E400(uint32 position, uint32 size, uint32 enabled, uint32 count)
{
    FUNCTION_MARKER(0x8001E400, "SLUS_003.73");
    return SpawnFireCycle((const uint32 *)psx_addr(position, 12), size, enabled, count);
}

extern "C" uint32 apocalypse_fire_cycle_spawn_native(const uint32 position[3], uint32 size, uint32 enabled, uint32 count)
{
    return SpawnFireCycle(position, size, enabled, count);
}
