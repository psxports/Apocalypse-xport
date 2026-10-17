extern "C" {
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
}
#include "beam_endpoint.h"
#include "native_object_storage.h"
#include "effect_lifecycle.h"
#include "game_random.h"
#include "vector_arithmetic.h"
#include "angle_math.h"
#include <new>
#include <string.h>

EndpointBeam::EndpointBeam(uint32 red, uint32 green, uint32 blue, uint32 count, uint32 duration, uint32 first_radius, uint32 second_radius, uint32 parameter1, uint32 parameter2, uint32 parameter3, uint32 parameter4) : BeamSegments(count + 1, 0)
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 storage, segments, last, value;
    sint32 interval = (sint32)duration;
    FUNCTION_MARKER(0x80035DC8, "SLUS_003.73");
    w_u32(object + 68, 0x800A1BF8);
    w_u32(object + 136, first_radius);
    w_u32(object + 148, second_radius);
    w_u32(object + 140, parameter1);
    w_u32(object + 152, parameter3);
    w_u32(object + 144, parameter2);
    w_u32(object + 156, parameter4);
    value = RandomRange(4096);
    red &= 255;
    blue &= 255;
    green &= 255;
    w_u32(object + 160, value);
    BeamSegments::SetColors(red, green, blue);
    segments = r_u32(object + 76);
    storage = r_u32(object + 80);
    last = storage + segments * 36;
    w_u8(last - 24, red >> 1);
    w_u8(storage + 12, red >> 1);
    segments = r_u32(object + 76);
    storage = r_u32(object + 80);
    last = storage + segments * 36;
    w_u8(last - 23, green >> 1);
    w_u8(storage + 13, green >> 1);
    segments = r_u32(object + 76);
    storage = r_u32(object + 80);
    last = storage + segments * 36;
    w_u8(last - 22, blue >> 1);
    w_u8(storage + 14, blue >> 1);
    SetSize((uint32)(interval / 20) & 65535);
    segments = r_u32(object + 76);
    storage = r_u32(object + 80);
    last = storage + segments * 36;
    value = (uint32)(interval / 40);
    w_u16(last - 20, value);
    w_u16(storage + 16, value);
    red *= 2;
    green *= 2;
    blue *= 2;
    if (red >= 256)
        red = 255;
    if (green >= 256)
        green = 255;
    if (blue >= 256)
        blue = 255;
    SetChildColors(red, green, blue, (uint32)(interval / 80));
}

uint32 EndpointBeam::Destroy(uint32 reason)
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 child, table, target;
    sint32 adjustment;
    FUNCTION_MARKER(0x80035FC0, "SLUS_003.73");
    child = r_u32(object + 84);
    w_u32(object + 68, 0x800A1BF8);
    if (child)
    {
        table = r_u32(child + 68);
        adjustment = (sint16)r_u16(table + 8);
        target = r_u32(table + 12);
        apocalypse_object_cleanup(target, child + (uint32)adjustment, 3);
    }
    BeamSegments::Destroy(0);
    return (reason & 1) ? ReleaseEffectStorage(object) : 0;
}

uint32 EndpointBeam::SetChildColors(uint32 red, uint32 green, uint32 blue, uint32 size)
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 child, table, target;
    sint32 adjustment;
    FUNCTION_MARKER(0x8003603C, "SLUS_003.73");
    child = r_u32(object + 84);
    if (child)
    {
        table = r_u32(child + 68);
        adjustment = (sint16)r_u16(table + 8);
        target = r_u32(table + 12);
        apocalypse_object_cleanup(target, child + (uint32)adjustment, 3);
    }
    child = AllocateEffectStorage(84);
    if (child)
    {
        uint32 count = r_u32(object + 76);
        void *dram_addr = AllocateNativeObjectStorage(child, sizeof(BeamSegments));
        new (dram_addr) BeamSegments(count, 0);
    }
    w_u32(object + 84, child);
    // Original allocation failure still reaches the child stores
    w_u8(child + 66, 1);
    child = r_u32(object + 84);
    BeamSegments::Attach(child).SetColors(red & 255, green & 255, blue & 255);
    child = r_u32(object + 84);
    return BeamSegments::Attach(child).SetSize(size & 65535);
}

// Native input bridge for the original coordinate word reads
static uint32 ReadEndpointCoordinate(const uint32 *input, uint32 axis)
{
    uint32 guest = GuestMemoryAddress(input), value;
    if (guest)
        return r_u32(guest + axis * 4);
    memcpy(&value, (const uint8 *)input + axis * 4, 4);
    return value;
}

uint32 EndpointBeam::SetEndpoints(const uint32 *origin, const uint32 *target)
{
    uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x800360FC, "SLUS_003.73");
    uint32 point[3], delta[3], step[3], divisor, value, axis;
    sint16 angles[4];
    for (axis = 0; axis < 3; ++axis)
        point[axis] = ReadEndpointCoordinate(origin, axis);
    w_u32(object + 88, point[0]);
    w_u32(object + 92, point[1]);
    w_u32(object + 96, point[2]);
    SubtractVectors(delta, target, origin);
    divisor = r_u32(object + 76) - 1;
    DivideVector(step, delta, &divisor);
    w_u32(object + 100, step[0]);
    w_u32(object + 104, step[1]);
    w_u32(object + 108, step[2]);
    CalculateDirectionAngles((uint16 *)angles, origin, target);
    angles[1] = (sint16)(((uint32)(uint16)angles[1] + 1024) & 4095);
    value = r_u32(object + 136);
    BuildDirectionVector((uint32 *)psx_addr(object + 112, 12), value, (const uint16 *)angles);
    angles[1] = (sint16)(((uint32)(uint16)angles[1] - 1024) & 4095);
    angles[0] = (sint16)(((uint32)(uint16)angles[0] + 1024) & 4095);
    value = r_u32(object + 148);
    return BuildDirectionVector((uint32 *)psx_addr(object + 124, 12), value, (const uint16 *)angles);
}

static uint32 ReadEndpointSine(uint32 angle)
{
    return (uint32)rsin(angle & 4095);
}

static void StoreEndpointPoint(uint32 destination, const uint32 point[3])
{
    w_u32(destination, point[0]);
    w_u32(destination + 4, point[1]);
    w_u32(destination + 8, point[2]);
}

uint32 EndpointBeam::Update()
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 count, storage, last, point[3], product[3], current[3];
    uint32 shifted[3], first[3], second[3], combined[3];
    uint32 axis, index = 1, angle, scalar, value, frequency, phase, child, destination;
    FUNCTION_MARKER(0x80036200, "SLUS_003.73");
    count = r_u32(object + 76) - 1;
    storage = r_u32(object + 80);
    for (axis = 0; axis < 3; ++axis)
        point[axis] = r_u32(object + 88 + axis * 4);
    StoreEndpointPoint(storage, point);
    storage = r_u32(object + 80);
    last = storage + count * 36;
    for (axis = 0; axis < 3; ++axis)
        product[axis] = r_u32(object + 100 + axis * 4) * count;
    for (axis = 0; axis < 3; ++axis)
        point[axis] = r_u32(object + 88 + axis * 4) + product[axis];
    StoreEndpointPoint(last, point);
    storage = r_u32(object + 80) + 36;
    for (axis = 0; axis < 3; ++axis)
    {
        value = r_u32(object + 88 + axis * 4);
        current[axis] = value + r_u32(object + 100 + axis * 4);
    }
    count = r_u32(object + 76) - 1;
    while ((sint32)index < (sint32)count)
    {
        if (!RandomRange(15))
        {
            value = RandomRange(4096);
            w_u32(object + 160, value);
        }
        for (axis = 0; axis < 3; ++axis)
            shifted[axis] = (uint32)((sint32)r_u32(object + 100 + axis * 4) >> 6);
        value = r_u32(0x800FF2F0);
        phase = r_u32(object + 160);
        scalar = ReadEndpointSine(phase + value * 130);
        for (axis = 0; axis < 3; ++axis)
        {
            product[axis] = shifted[axis] * scalar;
            point[axis] = current[axis] + (uint32)((sint32)product[axis] / 64);
        }
        StoreEndpointPoint(storage, point);
        value = index * r_u32(object + 140);
        frequency = r_u32(object + 144);
        angle = r_u32(0x800FF2F0) * (frequency * 2);
        phase = r_u32(object + 160);
        scalar = ReadEndpointSine(value + angle + phase);
        for (axis = 0; axis < 3; ++axis)
        {
            product[axis] = r_u32(object + 112 + axis * 4) * scalar;
            first[axis] = (uint32)((sint32)product[axis] / 4096);
        }
        value = index * r_u32(object + 152);
        frequency = r_u32(object + 156);
        angle = r_u32(0x800FF2F0) * (frequency * 2);
        phase = r_u32(object + 160);
        scalar = ReadEndpointSine(value + angle + phase);
        ++index;
        for (axis = 0; axis < 3; ++axis)
        {
            product[axis] = r_u32(object + 124 + axis * 4) * scalar;
            second[axis] = (uint32)((sint32)product[axis] / 4096);
        }
        for (axis = 0; axis < 3; ++axis)
            combined[axis] = first[axis] + second[axis];
        // Original in-place addition reads and stores each axis in sequence
        for (axis = 0; axis < 3; ++axis)
        {
            value = r_u32(storage + axis * 4);
            w_u32(storage + axis * 4, value + combined[axis]);
        }
        for (axis = 0; axis < 3; ++axis)
            current[axis] += r_u32(object + 100 + axis * 4);
        count = r_u32(object + 76) - 1;
        storage += 36;
    }
    child = r_u32(object + 84);
    if (!child)
        return child;
    count = r_u32(object + 76);
    if ((sint32)count <= 0)
        return count;
    index = 0;
    value = 0;
    do
    {
        child = r_u32(object + 84);
        ++index;
        destination = r_u32(child + 80);
        storage = r_u32(object + 80);
        destination += value;
        storage += value;
        for (axis = 0; axis < 3; ++axis)
            point[axis] = r_u32(storage + axis * 4);
        StoreEndpointPoint(destination, point);
        count = (sint32)index < (sint32)r_u32(object + 76);
        value += 36;
    } while (count);
    return count;
}

extern "C" uint32 sub_80035DC8(uint32 object, uint32 red, uint32 green, uint32 blue, uint32, uint32 count, uint32, uint32 duration, uint32 first_radius, uint32 second_radius, uint32 parameter1, uint32 parameter2, uint32 parameter3, uint32 parameter4)
{
    FUNCTION_MARKER(0x80035DC8, "SLUS_003.73");
    void *dram_addr = AllocateNativeObjectStorage(object, sizeof(EndpointBeam));
    new (dram_addr) EndpointBeam(red, green, blue, count, duration, first_radius, second_radius, parameter1, parameter2, parameter3, parameter4);
    return object;
}



extern "C" uint32 sub_80035FC0(uint32 object, uint32 reason)
{
    FUNCTION_MARKER(0x80035FC0, "SLUS_003.73");
    return EndpointBeam::Attach(object).EndpointBeam::Destroy(reason);
}



extern "C" uint32 sub_80036200(uint32 object)
{
    FUNCTION_MARKER(0x80036200, "SLUS_003.73");
    return EndpointBeam::Attach(object).EndpointBeam::Update();
}

extern "C" uint32 apocalypse_endpoint_beam_native(uint32 object, const uint32 origin[3], const uint32 target[3])
{
    return EndpointBeam::Attach(object).SetEndpoints(origin, target);
}

EndpointBeam &EndpointBeam::Attach(uint32 object)
{
    return *static_cast<EndpointBeam *>(FindNativeObjectStorage(object));
}


