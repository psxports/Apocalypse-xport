extern "C"
{
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
}
#include "vector_distance.h"
#include "native_object_storage.h"
#include <string.h>

static uint32 ReadDistanceWord(const uint32 *vector, uint32 mapped, uint32 axis)
{
    uint32 value;
    if (mapped)
        return r_u32(mapped + axis * 4);
    memcpy(&value, (const uint8 *)vector + axis * 4, 4);
    return value;
}

uint32 MeasurePositionDistance(const uint32 *origin, const uint32 *target)
{
    uint32 left_address, right_address, left, right, delta[3], axis, value;
    sint32 x, y, z;
    FUNCTION_MARKER(0x8006696C, "SLUS_003.73");
    left_address = GuestMemoryAddress(origin);
    right_address = GuestMemoryAddress(target);
    for (axis = 0; axis < 3; axis += 1)
    {
        left = ReadDistanceWord(origin, left_address, axis);
        right = ReadDistanceWord(target, right_address, axis);
        delta[axis] = left - right;
    }
    for (axis = 0; axis < 3; axis += 1)
    {
        if ((sint32)delta[axis] < 0)
            delta[axis] = 0 - delta[axis];
    }
    x = (sint32)delta[0];
    y = (sint32)delta[1];
    z = (sint32)delta[2];
    if (x < y)
    {
        if (y < z)
            value = (uint32)(x >> 2) + (uint32)(y >> 1) + (uint32)z;
        else if (z < x)
            value = (uint32)(z >> 2) + (uint32)(x >> 1) + (uint32)y;
        else
            value = (uint32)(x >> 2) + (uint32)(z >> 1) + (uint32)y;
    }
    else
    {
        if (x < z)
            value = (uint32)(y >> 2) + (uint32)(x >> 1) + (uint32)z;
        else if (z < y)
            value = (uint32)(z >> 2) + (uint32)(y >> 1) + (uint32)x;
        else
            value = (uint32)(y >> 2) + (uint32)(z >> 1) + (uint32)x;
    }
    return (uint32)((sint32)value >> 12);
}

extern "C" uint32 sub_8006696C(uint32 origin, uint32 target)
{
    FUNCTION_MARKER(0x8006696C, "SLUS_003.73");
    return MeasurePositionDistance((const uint32 *)psx_addr(origin, 12), (const uint32 *)psx_addr(target, 12));
}

extern "C" uint32 xport_draft_host_sub_8006696C_p2(uint32 origin, void *target)
{
    FUNCTION_MARKER(0x8006696C, "SLUS_003.73");
    return MeasurePositionDistance((const uint32 *)psx_addr(origin, 12), (const uint32 *)target);
}

extern "C" uint32 apocalypse_position_distance_native(const void *origin, const void *target)
{
    return MeasurePositionDistance((const uint32 *)origin, (const uint32 *)target);
}

uint32 MeasureHorizontalDistance(const uint32 *origin, const uint32 *target)
{
    uint32 first = GuestMemoryAddress(origin), second = GuestMemoryAddress(target);
    uint32 x, z, left, right;
    FUNCTION_MARKER(0x80066918, "SLUS_003.73");
    left = ReadDistanceWord(origin, first, 0);
    right = ReadDistanceWord(target, second, 0);
    x = (uint32)((sint32)(left - right) >> 12);
    x *= x;
    left = ReadDistanceWord(origin, first, 2);
    right = ReadDistanceWord(target, second, 2);
    z = (uint32)((sint32)(left - right) >> 12);
    z *= z;
    return LookupDistanceRoot(x + z);
}

extern "C" uint32 sub_80066918(uint32 origin, uint32 target)
{
    FUNCTION_MARKER(0x80066918, "SLUS_003.73");
    return MeasureHorizontalDistance((const uint32 *)psx_addr(origin, 12), (const uint32 *)psx_addr(target, 12));
}

// Native adapter for the identical original input and GTE command sequence
static uint32 SquareShiftedVector(const uint32 *vector)
{
    uint32 address = GuestMemoryAddress(vector), shifted[3], squares[3], axis;
    for (axis = 0; axis < 3; axis += 1)
        shifted[axis] = (uint32)((sint32)ReadDistanceWord(vector, address, axis) >> 12);
    for (axis = 0; axis < 3; axis += 1)
        xport_gte_write_data(9 + axis, shifted[axis]);
    xport_gte_execute(0xA00428);
    for (axis = 0; axis < 3; axis += 1)
        squares[axis] = xport_gte_read_data(25 + axis);
    return squares[0] + squares[1] + squares[2];
}

uint32 MeasureVectorLength(const uint32 *vector)
{
    FUNCTION_MARKER(0x8006BF04, "SLUS_003.73");
    return LookupDistanceRoot(SquareShiftedVector(vector));
}

uint32 MeasureSquaredVectorLength(const uint32 *vector)
{
    FUNCTION_MARKER(0x8006BF8C, "SLUS_003.73");
    return SquareShiftedVector(vector);
}

extern "C" uint32 sub_8006BF04(uint32 vector)
{
    FUNCTION_MARKER(0x8006BF04, "SLUS_003.73");
    return MeasureVectorLength((const uint32 *)psx_addr(vector, 12));
}

extern "C" uint32 xport_draft_host_sub_8006BF04_p1(const void *input)
{
    FUNCTION_MARKER(0x8006BF04, "SLUS_003.73");
    return MeasureVectorLength((const uint32 *)input);
}

extern "C" uint32 apocalypse_vector_squared_length_native(const uint32 vector[3])
{
    return MeasureSquaredVectorLength(vector);
}

uint32 MeasureEuclideanDistance(const uint32 *origin, const uint32 *target)
{
    uint32 first = GuestMemoryAddress(origin), second = GuestMemoryAddress(target);
    uint32 left, right, x, y, z;
    FUNCTION_MARKER(0x8006689C, "SLUS_003.73");
    left = ReadDistanceWord(origin, first, 0);
    right = ReadDistanceWord(target, second, 0);
    x = (uint32)((sint32)(left - right) >> 12);
    left = ReadDistanceWord(origin, first, 1);
    right = ReadDistanceWord(target, second, 1);
    y = (uint32)((sint32)(left - right) >> 12);
    right = ReadDistanceWord(target, second, 2);
    left = ReadDistanceWord(origin, first, 2);
    z = (uint32)((sint32)(left - right) >> 12);
    return LookupDistanceRoot(x * x + y * y + z * z);
}

extern "C" uint32 sub_8006689C(uint32 origin, uint32 target)
{
    FUNCTION_MARKER(0x8006689C, "SLUS_003.73");
    return MeasureEuclideanDistance((const uint32 *)psx_addr(origin, 12), (const uint32 *)psx_addr(target, 12));
}

extern "C" uint32 xport_draft_host_sub_8006689C_p2(uint32 origin, const void *target)
{
    FUNCTION_MARKER(0x8006689C, "SLUS_003.73");
    return MeasureEuclideanDistance((const uint32 *)psx_addr(origin, 12), (const uint32 *)target);
}

extern "C" uint32 xport_draft_host_sub_8006689C_p12(const void *origin, const void *target)
{
    FUNCTION_MARKER(0x8006689C, "SLUS_003.73");
    return MeasureEuclideanDistance((const uint32 *)origin, (const uint32 *)target);
}

uint32 LookupDistanceRoot(uint32 value)
{
    FUNCTION_MARKER(0x80085B54, "SLUS_003.73");
    xport_gte_write_data(30, value);
    uint32 shift = xport_gte_read_data(31) & 254;
    if ((sint32)value < 0)
        return r_u16(0x800F5130 + 2 * (value >> 20));
    uint32 index = (value << (shift & 31)) >> 20;
    return r_u16(0x800F5130 + 2 * index) >> ((shift >> 1) & 31);
}

extern "C" uint32 sub_80085B54(uint32 value)
{
    FUNCTION_MARKER(0x80085B54, "SLUS_003.73");
    return LookupDistanceRoot(value);
}
