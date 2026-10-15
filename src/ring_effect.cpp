#include "game_random.h"
#include "effect_lifecycle.h"
#include "effect_color.h"
#include "game_heap.h"
extern "C" {
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
}
#include "ring_effect.h"
#include "native_object_storage.h"
#include <new>
#include <string.h>

RingEffect::RingEffect(uint32 segments, uint32 rows) : EffectObject()
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 count, buffer, total, index, step;
    FUNCTION_MARKER(0x80034598, "SLUS_003.73");
    count = segments << 1;
    step = count ? 4096 / count : 0xFFFFFFFF;
    w_u32(object + 80, count);
    w_u32(object + 68, 0x800A1D08);
    w_u32(object + 84, rows);
    w_u16(object + 92, step);
    buffer = AllocateHeapBlock((count + count * rows) << 3, 0, 1);
    count = r_u32(object + 80);
    total = count * r_u32(object + 84);
    w_u32(object + 72, buffer);
    count = r_u32(object + 80);
    w_u32(object + 76, buffer + (count << 3));
    if (total)
    {
        index = 0;
        do
        {
            buffer = r_u32(object + 76);
            w_u32(buffer + (index << 3) + 4, 0x3A000000);
            count = r_u32(object + 80);
            total = count * r_u32(object + 84);
            ++index;
        } while (index < total);
    }
    w_u32(object + 88, 0x32000000);
    w_u32(object + 100, (uint32)-1);
    AddToList((uint32 *)psx_addr(0x800FF450, 4));
}

RingEffect::RingEffect(const uint32 *position, uint32 radius, uint32 extent, uint32 red, uint32 green, uint32 blue, uint32 tail_red, uint32 tail_green, uint32 tail_blue) : EffectObject()
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 guest_position = GuestMemoryAddress(position);
    const uint32 *native_position = guest_position ? 0 : position;
    uint32 count, buffer, x, y, z, index, color;
    FUNCTION_MARKER(0x800346A8, "SLUS_003.73");
    red &= 255;
    green &= 255;
    blue &= 255;
    tail_red &= 255;
    tail_green &= 255;
    tail_blue &= 255;
    w_u32(object + 68, 0x800A1D08);
    w_u32(object + 80, 8);
    w_u16(object + 92, 512);
    count = r_u32(object + 80);
    w_u32(object + 84, 1);
    buffer = AllocateHeapBlock(count << 4, 0, 1);
    count = r_u32(object + 80);
    w_u32(object + 72, buffer);
    w_u32(object + 76, buffer + (count << 3));
    AddToList((uint32 *)psx_addr(0x800FF450, 4));
    if (native_position)
    {
        memcpy(&x, native_position, 4);
        memcpy(&y, (const uint8 *)native_position + 4, 4);
        memcpy(&z, (const uint8 *)native_position + 8, 4);
    }
    else
    {
        x = r_u32(guest_position);
        y = r_u32(guest_position + 4);
        z = r_u32(guest_position + 8);
    }
    w_u32(object + 24, x);
    w_u32(object + 28, y);
    w_u32(object + 32, z);
    count = r_u32(object + 80);
    index = 0;
    if (count)
        do
        {
            buffer = r_u32(object + 72);
            w_u32(buffer + (index << 3), radius);
            count = r_u32(object + 80);
            ++index;
        } while (index < count);
    color = 0x32000000 | (blue << 16) | (green << 8) | red;
    w_u32(object + 88, color);
    color = (tail_blue << 16) | (tail_green << 8) | tail_red;
    count = r_u32(object + 80);
    index = 0;
    if (count)
        do
        {
            buffer = r_u32(object + 72);
            w_u32(buffer + (index << 3) + 4, color);
            count = r_u32(object + 80);
            ++index;
        } while (index < count);
    count = r_u32(object + 80);
    index = 0;
    if (count)
        do
        {
            buffer = r_u32(object + 76);
            w_u32(buffer + (index << 3), extent);
            buffer = r_u32(object + 76);
            w_u32(buffer + (index << 3) + 4, 0x3A000000);
            count = r_u32(object + 80);
            ++index;
        } while (index < count);
    w_u32(object + 100, (uint32)-1);
}

uint32 RingEffect::Destroy(uint32 reason)
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 buffer, result;
    FUNCTION_MARKER(0x800348A8, "SLUS_003.73");
    buffer = r_u32(object + 72);
    w_u32(object + 68, 0x800A1D08);
    ReleaseHeapBlock(buffer);
    RemoveFromList((uint32 *)psx_addr(0x800FF450, 4));
    result = EffectObject::Destroy(0);
    if (reason & 1)
        result = ReleaseEffectStorage(object);
    return result;
}

void RingEffect::Update()
{
    FUNCTION_MARKER(0x8003A118, "SLUS_003.73");
}

uint32 RingEffect::SetOddRadii(uint32 value)
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 index = 1, count, result, buffer;
    FUNCTION_MARKER(0x80034918, "SLUS_003.73");
    count = r_u32(object + 80);
    result = index < count;
    while (result)
    {
        buffer = r_u32(object + 72);
        w_u32(buffer + 8 * index, value);
        count = r_u32(object + 80);
        index += 2;
        result = index < count;
    }
    return result;
}

uint32 RingEffect::SetEvenRadii(uint32 value)
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 index = 0, count, result, buffer;
    FUNCTION_MARKER(0x80034958, "SLUS_003.73");
    count = r_u32(object + 80);
    result = count;
    while (result)
    {
        buffer = r_u32(object + 72);
        w_u32(buffer + 8 * index, value);
        count = r_u32(object + 80);
        index += 2;
        result = index < count;
    }
    return result;
}

uint32 RingEffect::SetRadii(uint32 value)
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 index = 0, count, result, buffer;
    FUNCTION_MARKER(0x80034994, "SLUS_003.73");
    count = r_u32(object + 80);
    result = count;
    while (result)
    {
        buffer = r_u32(object + 72);
        w_u32(buffer + 8 * index, value);
        count = r_u32(object + 80);
        ++index;
        result = index < count;
    }
    return result;
}

uint32 RingEffect::SetRowExtent(uint32 row, uint32 value)
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 count, index = 0, buffer, result;
    FUNCTION_MARKER(0x800349D0, "SLUS_003.73");
    count = r_u32(object + 80);
    result = 8 * (row * count);
    buffer = r_u32(object + 76) + result;
    if (count)
    {
        do
        {
            w_u32(buffer, value);
            count = r_u32(object + 80);
            ++index;
            result = index < count;
            buffer += 8;
        } while (result);
    }
    return result;
}

uint32 RingEffect::SetCenterColor(uint32 red, uint32 green, uint32 blue)
{
    const uint32 object = NativeObjectGuestAddress(this);
    const uint32 opcode = 0x32000000;
    FUNCTION_MARKER(0x80034A18, "SLUS_003.73");
    w_u32(object + 88, opcode | ((blue & 255) << 16) | ((green & 255) << 8) | (red & 255));
    return opcode;
}

uint32 RingEffect::SetRingColors(uint32 red, uint32 green, uint32 blue)
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 index = 0, count, result, buffer;
    uint32 value = (red & 255) | ((green & 255) << 8) | ((blue & 255) << 16);
    FUNCTION_MARKER(0x80034A44, "SLUS_003.73");
    count = r_u32(object + 80);
    result = count;
    while (result)
    {
        buffer = r_u32(object + 72);
        w_u32(buffer + 8 * index + 4, value);
        count = r_u32(object + 80);
        index += 1;
        result = index < count;
    }
    return result;
}

uint32 RingEffect::SetRowColors(uint32 row, uint32 red, uint32 green, uint32 blue)
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 count, index = 0, buffer, result;
    uint32 value = 0x3A000000 | (red & 255) | ((green & 255) << 8) | ((blue & 255) << 16);
    FUNCTION_MARKER(0x80034A9C, "SLUS_003.73");
    count = r_u32(object + 80);
    result = 8 * (row * count);
    buffer = r_u32(object + 76) + result;
    if (count)
    {
        do
        {
            w_u32(buffer + 4, value);
            count = r_u32(object + 80);
            ++index;
            result = index < count;
            buffer += 8;
        } while (result);
    }
    return result;
}

uint32 RingEffect::SetColumnColors(uint32 column, uint32 red, uint32 green, uint32 blue)
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 index = 0, offset = column << 3;
    uint32 color = 0x3A000000 | ((blue & 255) << 16) | ((green & 255) << 8) | (red & 255);
    uint32 result, buffer, rows, stride;
    FUNCTION_MARKER(0x80034B08, "SLUS_003.73");
    w_u32(r_u32(object + 72) + offset + 4, color);
    result = r_u32(object + 76);
    buffer = result + offset;
    rows = r_u32(object + 84);
    if (rows)
    {
        do
        {
            ++index;
            w_u32(buffer + 4, color);
            stride = r_u32(object + 80) << 3;
            rows = r_u32(object + 84);
            result = index < rows;
            buffer += stride;
        } while (result);
    }
    return result;
}

uint32 RingEffect::FadeCenter(uint32 amount)
{
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x80034B7C, "SLUS_003.73");
    return FadePackedColor((uint32 *)psx_addr(object + 88, 4), (sint32)amount);
}

uint32 RingEffect::FadeRing(uint32 amount)
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 index = 0, count, offset, buffer;
    FUNCTION_MARKER(0x80034B9C, "SLUS_003.73");
    count = r_u32(object + 80);
    offset = count;
    if (count)
    {
        offset = 0;
        do
        {
            buffer = r_u32(object + 72);
            FadePackedColor((uint32 *)psx_addr(buffer + offset + 4, 4), (sint32)amount);
            count = r_u32(object + 80);
            ++index;
            offset = index << 3;
        } while (index < count);
    }
    return offset;
}

RingEffect &RingEffect::Attach(uint32 object)
{
    return *static_cast<RingEffect *>(FindNativeObjectStorage(object));
}

extern "C" uint32 sub_80034598(uint32 object, uint32 segments, uint32 rows)
{
    FUNCTION_MARKER(0x80034598, "SLUS_003.73");
    void *dram_addr = AllocateNativeObjectStorage(object, sizeof(RingEffect));
    new (dram_addr) RingEffect(segments, rows);
    return object;
}

extern "C" uint32 sub_80034918(uint32 object, uint32 value)
{
    FUNCTION_MARKER(0x80034918, "SLUS_003.73");
    return RingEffect::Attach(object).SetOddRadii(value);
}
extern "C" uint32 sub_80034958(uint32 object, uint32 value)
{
    FUNCTION_MARKER(0x80034958, "SLUS_003.73");
    return RingEffect::Attach(object).SetEvenRadii(value);
}
extern "C" uint32 sub_80034994(uint32 object, uint32 value)
{
    FUNCTION_MARKER(0x80034994, "SLUS_003.73");
    return RingEffect::Attach(object).SetRadii(value);
}
extern "C" uint32 sub_800349D0(uint32 object, uint32 row, uint32 value)
{
    FUNCTION_MARKER(0x800349D0, "SLUS_003.73");
    return RingEffect::Attach(object).SetRowExtent(row, value);
}
extern "C" uint32 sub_80034A18(uint32 a1, uint32 a2, uint32 a3, uint32 a4)
{
    FUNCTION_MARKER(0x80034A18, "SLUS_003.73");
    return RingEffect::Attach(a1).SetCenterColor(a2, a3, a4);
}
extern "C" uint32 sub_80034A44(uint32 object, uint32 red, uint32 green, uint32 blue)
{
    FUNCTION_MARKER(0x80034A44, "SLUS_003.73");
    return RingEffect::Attach(object).SetRingColors(red, green, blue);
}
extern "C" uint32 sub_80034A9C(uint32 object, uint32 row, uint32 red, uint32 green, uint32 blue)
{
    FUNCTION_MARKER(0x80034A9C, "SLUS_003.73");
    return RingEffect::Attach(object).SetRowColors(row, red, green, blue);
}
extern "C" uint32 sub_80034B08(uint32 a1, uint32 a2, uint32 a3, uint32 a4, uint32 a5)
{
    FUNCTION_MARKER(0x80034B08, "SLUS_003.73");
    return RingEffect::Attach(a1).SetColumnColors(a2, a3, a4, a5);
}
extern "C" uint32 sub_80034B7C(uint32 a1, uint32 a2)
{
    FUNCTION_MARKER(0x80034B7C, "SLUS_003.73");
    return RingEffect::Attach(a1).FadeCenter(a2);
}
extern "C" uint32 sub_80034B9C(uint32 a1, uint32 a2)
{
    FUNCTION_MARKER(0x80034B9C, "SLUS_003.73");
    return RingEffect::Attach(a1).FadeRing(a2);
}
extern "C" uint32 sub_800348A8(uint32 a1, uint32 a2)
{
    FUNCTION_MARKER(0x800348A8, "SLUS_003.73");
    return RingEffect::Attach(a1).RingEffect::Destroy(a2);
}

extern "C" uint32 sub_800346A8(uint32 object, uint32 position, uint32 radius, uint32 extent, uint32 red, uint32 green, uint32 blue, uint32 tail_red, uint32 tail_green, uint32 tail_blue)
{
    FUNCTION_MARKER(0x800346A8, "SLUS_003.73");
    void *dram_addr = AllocateNativeObjectStorage(object, sizeof(RingEffect));
    new (dram_addr) RingEffect((const uint32 *)psx_addr(position, 12), radius, extent, red, green, blue, tail_red, tail_green, tail_blue);
    return object;
}

extern "C" uint32 apocalypse_ring_effect_native(uint32 object, const void *position, uint32 radius, uint32 extent, uint32 red, uint32 green, uint32 blue, uint32 tail_red, uint32 tail_green, uint32 tail_blue)
{
    void *dram_addr = AllocateNativeObjectStorage(object, sizeof(RingEffect));
    new (dram_addr) RingEffect((const uint32 *)position, radius, extent, red, green, blue, tail_red, tail_green, tail_blue);
    return object;
}

uint32 SpawnProjectileRingTrail(const uint32 *position, uint32 red, uint32 green, uint32 blue)
{
    uint32 object, random;
    FUNCTION_MARKER(0x8002557C, "SLUS_003.73");
    object = AllocateEffectStorage(104);
    if (object)
    {
        void *dram_addr = AllocateNativeObjectStorage(object, sizeof(RingEffect));
        new (dram_addr) RingEffect(4, 2);
    }
    // TODO Original allocation failure falls through with a null receiver; native binding fails fast
    RingEffect &effect = RingEffect::Attach(object);
    effect.SetCenterColor(255, 255, 255);
    red &= 255;
    green &= 255;
    blue &= 255;
    effect.SetRingColors(red, green, blue);
    effect.SetRowColors(0, ((39 * red) >> 7) & 255, ((39 * green) >> 7) & 255, ((39 * blue) >> 7) & 255);
    effect.SetRowColors(1, 0, 0, 0);
    effect.SetOddRadii(21);
    effect.SetEvenRadii(12);
    effect.SetRowExtent(0, 20);
    effect.SetRowExtent(1, 20);
    effect.SetPosition(position);
    random = RandomRange(1024);
    w_u16(object + 96, random);
    w_u8(object + 66, 1);
    return object;
}

extern "C" uint32 sub_8002557C(uint32 position, uint32 red, uint32 green, uint32 blue)
{
    FUNCTION_MARKER(0x8002557C, "SLUS_003.73");
    return SpawnProjectileRingTrail((const uint32 *)psx_addr(position, 12), red, green, blue);
}
