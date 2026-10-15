extern "C" {
#include "draft_first_signatures.h"
}
#include "expanding_ring.h"
#include "game_random.h"
#include "effect_lifecycle.h"
#include "native_object_storage.h"
#include <new>
#include <string.h>

ExpandingRing::ExpandingRing(const sint32 *position, uint32 scale, uint32 red, uint32 green, uint32 blue, uint32 count, uint32 tail)
    : PointSegments(count)
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 guest_position = GuestMemoryAddress(position);
    const sint32 *native_position = guest_position ? 0 : position;
    uint32 x, y, z, index, destination, segment_count;
    FUNCTION_MARKER(0x8001ECFC, "SLUS_003.73");
    w_u32(object + 68, 0x800A110C);
    x = ReadPosition(guest_position, native_position, 0);
    y = ReadPosition(guest_position, native_position, 1);
    z = ReadPosition(guest_position, native_position, 2);
    w_u32(object + 24, x);
    w_u32(object + 28, y);
    w_u32(object + 32, z);
    w_u32(object + 100, scale);
    w_u32(object + 108, tail);
    x = ReadPosition(guest_position, native_position, 0);
    y = ReadPosition(guest_position, native_position, 1);
    z = ReadPosition(guest_position, native_position, 2);
    w_u32(object + 84, x);
    w_u32(object + 88, y);
    w_u32(object + 92, z);
    segment_count = r_u32(object + 76);
    w_u8(object + 96, red);
    w_u8(object + 97, green);
    w_u8(object + 98, blue);
    index = 0;
    if ((sint32)segment_count > 0)
    {
        do
        {
            w_u8(r_u32(object + 80) + 16 * index + 12, red);
            w_u8(r_u32(object + 80) + 16 * index + 13, green);
            w_u8(r_u32(object + 80) + 16 * index + 14, blue);
            destination = r_u32(object + 80) + 16 * index;
            x = ReadPosition(guest_position, native_position, 0);
            y = ReadPosition(guest_position, native_position, 1);
            z = ReadPosition(guest_position, native_position, 2);
            w_u32(destination, x);
            w_u32(destination + 4, y);
            w_u32(destination + 8, z);
            segment_count = r_u32(object + 76);
            ++index;
        } while ((sint32)index < (sint32)segment_count);
    }
    w_u32(object + 112, RandomRange(4096));
    EnableSegments();
}

uint32 ExpandingRing::Destroy(uint32 reason)
{
    uint32 result;
    FUNCTION_MARKER(0x8001EE48, "SLUS_003.73");
    w_u32(NativeObjectGuestAddress(this) + 68, 0x800A110C);
    result = PointSegments::Destroy(0);
    if (reason & 1)
        result = ReleaseEffectStorage(NativeObjectGuestAddress(this));
    return result;
}

uint32 ExpandingRing::Update()
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 radius, speed, maximum, fade, angle, step, table, product, count;
    uint32 result, destination, index, offset, x, y, z;
    sint16 trigonometric_value;
    FUNCTION_MARKER(0x8001EE9C, "SLUS_003.73");
    radius = r_u32(object + 104);
    speed = r_u32(object + 108);
    maximum = r_u32(object + 100);
    radius += speed;
    w_u32(object + 104, radius);
    if ((sint32)radius >= (sint32)maximum)
    {
        w_u32(object + 104, maximum);
        Finish();
    }
    maximum = r_u32(object + 100);
    radius = r_u32(object + 104);
    fade = Divide((sint32)((maximum - radius) << 8), (sint32)maximum);
    angle = r_u32(object + 112);
    table = 0x800F863C + ((angle & 4095) << 2);
    trigonometric_value = (sint16)r_u16(table + 2);
    product = radius * (uint32)(sint32)trigonometric_value;
    count = r_u32(object + 76);
    step = Divide(4096, (sint32)count);
    if ((sint32)product < 0)
        product += 4095;
    result = (uint32)((sint32)product >> 12) << 12;
    x = r_u32(object + 24);
    w_u32(object + 84, x + result);
    trigonometric_value = (sint16)r_u16(table);
    radius = r_u32(object + 104);
    result = ProjectRadius(radius, trigonometric_value);
    z = r_u32(object + 32);
    count = r_u32(object + 76);
    w_u32(object + 92, z + result);
    index = 0;
    if ((sint32)count > 0)
    {
        do
        {
            product = r_u8(object + 96) * fade;
            offset = index << 4;
            destination = r_u32(object + 80) + offset;
            w_u8(destination + 12, (uint32)((sint32)product >> 8));
            product = r_u8(object + 97) * fade;
            destination = r_u32(object + 80) + offset;
            w_u8(destination + 13, (uint32)((sint32)product >> 8));
            product = r_u8(object + 98) * fade;
            destination = r_u32(object + 80) + offset;
            w_u8(destination + 14, (uint32)((sint32)product >> 8));
            count = r_u32(object + 76);
            angle += step;
            if (index == count - 1)
            {
                destination = r_u32(object + 80) + offset;
                x = r_u32(object + 84);
                y = r_u32(object + 88);
                z = r_u32(object + 92);
                w_u32(destination, x);
                w_u32(destination + 4, y);
                w_u32(destination + 8, z);
            }
            else
            {
                table = 0x800F863C + ((angle & 4095) << 2);
                trigonometric_value = (sint16)r_u16(table + 2);
                radius = r_u32(object + 104);
                destination = r_u32(object + 80) + offset;
                product = ProjectRadius(radius, trigonometric_value);
                x = r_u32(object + 24);
                w_u32(destination, x + product);
                trigonometric_value = (sint16)r_u16(table);
                radius = r_u32(object + 104);
                destination = r_u32(object + 80) + offset;
                product = ProjectRadius(radius, trigonometric_value);
                z = r_u32(object + 32);
                w_u32(destination + 8, z + product);
            }
            count = r_u32(object + 76);
            ++index;
            result = (sint32)index < (sint32)count;
        } while (result);
    }
    return result;
}

uint32 ExpandingRing::Divide(sint32 numerator, sint32 denominator)
{
    if (!denominator)
        return numerator < 0 ? 1 : (uint32)-1;
    if ((uint32)numerator == 0x80000000 && denominator == -1)
        return 0x80000000;
    return (uint32)(numerator / denominator);
}

uint32 ExpandingRing::ProjectRadius(uint32 radius, sint16 trigonometric_value)
{
    uint32 product = radius * (uint32)(sint32)trigonometric_value;
    if ((sint32)product < 0)
        product += 4095;
    return (uint32)((sint32)product >> 12) << 12;
}

ExpandingRing &ExpandingRing::Attach(uint32 object)
{
    return *static_cast<ExpandingRing *>(FindNativeObjectStorage(object));
}

uint32 ExpandingRing::ReadPosition(uint32 guest, const sint32 *native, uint32 index)
{
    uint32 value;
    if (!native)
        return r_u32(guest + 4 * index);
    memcpy(&value, (const unsigned char *)native + 4 * index, 4);
    return value;
}

extern "C" uint32 sub_8001ECFC(uint32 object, uint32 position, uint32 scale, uint32 red, uint32 green, uint32 blue, uint32 count, uint32 tail)
{
    FUNCTION_MARKER(0x8001ECFC, "SLUS_003.73");
    void *storage = AllocateNativeObjectStorage(object, sizeof(ExpandingRing));
    new (storage) ExpandingRing((const sint32 *)psx_addr(position, 12), scale, red, green, blue, count, tail);
    return object;
}

extern "C" uint32 apocalypse_effect_1ECFC_native(uint32 object, const sint32 position[3], uint32 scale, uint32 red, uint32 green, uint32 blue, uint32 count, uint32 tail)
{
    void *storage = AllocateNativeObjectStorage(object, sizeof(ExpandingRing));
    new (storage) ExpandingRing(position, scale, red, green, blue, count, tail);
    return object;
}

extern "C" uint32 sub_8001EE48(uint32 object, uint32 reason)
{
    FUNCTION_MARKER(0x8001EE48, "SLUS_003.73");
    return ExpandingRing::Attach(object).ExpandingRing::Destroy(reason);
}

extern "C" uint32 sub_8001EE9C(uint32 object)
{
    FUNCTION_MARKER(0x8001EE9C, "SLUS_003.73");
    return ExpandingRing::Attach(object).ExpandingRing::Update();
}
