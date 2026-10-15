extern "C"
{
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
}
#include <string.h>
#include <stdint.h>
#include "angle_math.h"
#include "vector_distance.h"
#include "vector_arithmetic.h"
#include "native_object_storage.h"











uint32 ShortestCameraAngleDelta(sint16 first, sint16 second)
{
    sint32 delta;
    FUNCTION_MARKER(0x80077274, "SLUS_003.73");
    delta = (second & 4095) - (first & 4095);
    if (delta > 2048)
        delta -= 4096;
    else if (delta < -2048)
        delta += 4096;
    return (uint32)(sint32)(sint16)delta;
}

extern "C" uint32 sub_80077274(sint16 first, sint16 second)
{
    FUNCTION_MARKER(0x80077274, "SLUS_003.73");
    return ShortestCameraAngleDelta(first, second);
}




uint32 SnapSmallAngles(uint16 *angles)
{
    uint32 address = GuestMemoryAddress(angles), axis, value, small = 0;
    uint16 half, zero = 0;
    FUNCTION_MARKER(0x8006C64C, "SLUS_003.73");
    for (axis = 0; axis < 3; axis += 1)
    {
        if (address)
            value = r_u16(address + axis * 2);
        else
        {
            memcpy(&half, (const uint8 *)angles + axis * 2, 2);
            value = half;
        }
        small = ((value + 1) & 65535) < 3;
        if (small)
        {
            if (address)
                w_u16(address + axis * 2, 0);
            else
                memcpy((uint8 *)angles + axis * 2, &zero, 2);
        }
    }
    // Original return describes the final angle only
    return small;
}

extern "C" uint32 sub_8006C64C(uint32 angles)
{
    FUNCTION_MARKER(0x8006C64C, "SLUS_003.73");
    return SnapSmallAngles((uint16 *)psx_addr(angles, 6));
}

static uint32 ReadAngleHalf(const uint16 *angles, uint32 address, uint32 axis)
{
    uint16 value;
    if (address)
        return r_u16(address + axis * 2);
    memcpy(&value, (const uint8 *)angles + axis * 2, 2);
    return value;
}

static void WriteAngleHalf(uint16 *angles, uint32 address, uint32 axis, uint32 value)
{
    uint16 half = (uint16)value;
    if (address)
        w_u16(address + axis * 2, half);
    else
        memcpy((uint8 *)angles + axis * 2, &half, 2);
}

uint32 NormalizeAngles(uint16 *angles)
{
    uint32 address = GuestMemoryAddress(angles), x, y, z;
    FUNCTION_MARKER(0x8006C624, "SLUS_003.73");
    x = ReadAngleHalf(angles, address, 0);
    z = ReadAngleHalf(angles, address, 2);
    WriteAngleHalf(angles, address, 0, x & 4095);
    y = ReadAngleHalf(angles, address, 1);
    WriteAngleHalf(angles, address, 2, z & 4095);
    y &= 4095;
    WriteAngleHalf(angles, address, 1, y);
    return y;
}

uint16 *AddAngleVector(uint16 *angles, const uint16 *source)
{
    uint32 destination = GuestMemoryAddress(angles), input = GuestMemoryAddress(source);
    uint32 axis, left, right;
    FUNCTION_MARKER(0x8006C730, "SLUS_003.73");
    for (axis = 0; axis < 3; axis += 1)
    {
        left = ReadAngleHalf(angles, destination, axis);
        right = ReadAngleHalf(source, input, axis);
        WriteAngleHalf(angles, destination, axis, left + right);
    }
    return angles;
}

extern "C" uint32 sub_8006C624(uint32 angles)
{
    FUNCTION_MARKER(0x8006C624, "SLUS_003.73");
    return NormalizeAngles((uint16 *)psx_addr(angles, 6));
}

extern "C" uint32 sub_8006C730(uint32 destination, uint32 source)
{
    FUNCTION_MARKER(0x8006C730, "SLUS_003.73");
    AddAngleVector((uint16 *)psx_addr(destination, 6), (const uint16 *)psx_addr(source, 6));
    return destination;
}

extern "C" uint32 xport_draft_host_sub_8006C730_p12(void *argument1, void *argument2)
{
    FUNCTION_MARKER(0x8006C730, "SLUS_003.73");
    AddAngleVector((uint16 *)argument1, (const uint16 *)argument2);
    return (uint32)(uintptr_t)argument1;
}

uint16 *SubtractAngleVector(uint16 *angles, const uint16 *source)
{
    uint32 destination = GuestMemoryAddress(angles), input = GuestMemoryAddress(source);
    uint32 axis, left, right;
    FUNCTION_MARKER(0x8006C774, "SLUS_003.73");
    for (axis = 0; axis < 3; axis += 1)
    {
        left = ReadAngleHalf(angles, destination, axis);
        right = ReadAngleHalf(source, input, axis);
        WriteAngleHalf(angles, destination, axis, left - right);
    }
    return angles;
}

uint16 *DivideAngleVector(uint16 *angles, const uint32 *divisor)
{
    uint32 destination = GuestMemoryAddress(angles), parameter = GuestMemoryAddress(divisor);
    uint32 axis, denominator, quotient;
    sint32 numerator;
    FUNCTION_MARKER(0x8006C808, "SLUS_003.73");
    for (axis = 0; axis < 3; axis += 1)
    {
        numerator = (sint16)ReadAngleHalf(angles, destination, axis);
        if (parameter)
            denominator = r_u32(parameter);
        else
            memcpy(&denominator, divisor, 4);
        if (!denominator)
            quotient = numerator < 0 ? 1 : 0xFFFFFFFF;
        else
            quotient = (uint32)(numerator / (sint32)denominator);
        WriteAngleHalf(angles, destination, axis, quotient);
    }
    return angles;
}

extern "C" uint32 sub_8006C774(uint32 destination, uint32 source)
{
    FUNCTION_MARKER(0x8006C774, "SLUS_003.73");
    SubtractAngleVector((uint16 *)psx_addr(destination, 6), (const uint16 *)psx_addr(source, 6));
    return destination;
}

extern "C" uint32 sub_8006C808(uint32 destination, uint32 divisor)
{
    FUNCTION_MARKER(0x8006C808, "SLUS_003.73");
    DivideAngleVector((uint16 *)psx_addr(destination, 6), (const uint32 *)psx_addr(divisor, 4));
    return destination;
}

extern "C" uint32 xport_draft_host_sub_8006C808_p2(uint32 destination, void *divisor)
{
    FUNCTION_MARKER(0x8006C808, "SLUS_003.73");
    DivideAngleVector((uint16 *)psx_addr(destination, 6), (const uint32 *)divisor);
    return destination;
}

uint16 *SubtractAngleVector(uint16 *output, const uint16 *left, const uint16 *right)
{
    uint32 destination = GuestMemoryAddress(output), first = GuestMemoryAddress(left), second = GuestMemoryAddress(right);
    uint32 axis, a, b, packed, offset, mask;
    uint16 difference[3];
    FUNCTION_MARKER(0x8006CB28, "SLUS_003.73");
    for (axis = 0; axis < 3; axis += 1)
    {
        a = ReadAngleHalf(left, first, axis);
        b = ReadAngleHalf(right, second, axis);
        difference[axis] = (uint16)(a - b);
    }
    packed = (uint32)difference[0] | ((uint32)difference[1] << 16);
    if (destination)
    {
        // Original SWL then SWR retain masked stores for unaligned packed XY
        offset = (destination + 3) & 3;
        mask = (1 << (offset + 1)) - 1;
        w_masked_u32((destination + 3) & 0xFFFFFFFC, packed >> ((3 - offset) * 8), mask);
        offset = destination & 3;
        mask = (15 << offset) & 15;
        w_masked_u32(destination & 0xFFFFFFFC, packed << (offset * 8), mask);
    }
    else
    {
        memcpy(output, &packed, 4);
        memcpy(output, &packed, 4);
    }
    WriteAngleHalf(output, destination, 2, difference[2]);
    return output;
}

extern "C" uint32 sub_8006CB28(uint32 output, uint32 left, uint32 right)
{
    FUNCTION_MARKER(0x8006CB28, "SLUS_003.73");
    SubtractAngleVector((uint16 *)psx_addr(output, 6), (const uint16 *)psx_addr(left, 6), (const uint16 *)psx_addr(right, 6));
    return output;
}

extern "C" uint32 xport_draft_host_sub_8006CB28_p1(void *argument1, uint32 argument2, uint32 argument3)
{
    FUNCTION_MARKER(0x8006CB28, "SLUS_003.73");
    SubtractAngleVector((uint16 *)argument1, (const uint16 *)psx_addr(argument2, 6), (const uint16 *)psx_addr(argument3, 6));
    return (uint32)(uintptr_t)argument1;
}

uint16 *DampAngles(uint16 *angles, const uint8 *shifts)
{
    uint32 destination = GuestMemoryAddress(angles), parameter = GuestMemoryAddress(shifts);
    uint32 axis, amount, value, delta;
    sint32 signed_value, next_signed = 0;
    FUNCTION_MARKER(0x8006C8E8, "SLUS_003.73");
    signed_value = (sint16)ReadAngleHalf(angles, destination, 0);
    for (axis = 0; axis < 3; axis += 1)
    {
        amount = parameter ? r_u8(parameter + axis) : shifts[axis];
        value = ReadAngleHalf(angles, destination, axis);
        delta = value - (uint32)(signed_value >> (amount & 31));
        if (axis < 2)
            next_signed = (sint16)ReadAngleHalf(angles, destination, axis + 1);
        WriteAngleHalf(angles, destination, axis, delta);
        signed_value = next_signed;
    }
    return angles;
}

extern "C" uint32 sub_8006C8E8(uint32 angles, uint32 shifts)
{
    FUNCTION_MARKER(0x8006C8E8, "SLUS_003.73");
    DampAngles((uint16 *)psx_addr(angles, 6), (const uint8 *)psx_addr(shifts, 3));
    return angles;
}

static void WriteDirectionWord(uint32 *output, uint32 address, uint32 axis, uint32 value)
{
    if (address)
        w_u32(address + axis * 4, value);
    else
        memcpy((uint8 *)output + axis * 4, &value, 4);
}

uint32 BuildDirectionVector(uint32 *output, uint32 scale, const uint16 *angles)
{
    uint32 destination = GuestMemoryAddress(output), input = GuestMemoryAddress(angles);
    uint32 pitch, yaw, product, value, table_value;
    FUNCTION_MARKER(0x800667CC, "SLUS_003.73");
    pitch = ReadAngleHalf(angles, input, 0) & 4095;
    table_value = (uint32)(sint32)(sint16)r_u16(0x800F863E + pitch * 4);
    product = scale * table_value;
    yaw = ReadAngleHalf(angles, input, 1) & 4095;
    table_value = (uint32)(sint32)(sint16)r_u16(0x800F863C + yaw * 4);
    value = 0 - (uint32)((sint32)product >> 12) * table_value;
    WriteDirectionWord(output, destination, 0, value);
    pitch = ReadAngleHalf(angles, input, 0) & 4095;
    table_value = (uint32)(sint32)(sint16)r_u16(0x800F863C + pitch * 4);
    value = scale * table_value;
    WriteDirectionWord(output, destination, 1, value);
    pitch = ReadAngleHalf(angles, input, 0) & 4095;
    table_value = (uint32)(sint32)(sint16)r_u16(0x800F863E + pitch * 4);
    product = scale * table_value;
    yaw = ReadAngleHalf(angles, input, 1) & 4095;
    table_value = (uint32)(sint32)(sint16)r_u16(0x800F863E + yaw * 4);
    value = 0 - (uint32)((sint32)product >> 12) * table_value;
    WriteDirectionWord(output, destination, 2, value);
    return value;
}

extern "C" uint32 sub_800667CC(uint32 output, uint32 scale, uint32 angles)
{
    FUNCTION_MARKER(0x800667CC, "SLUS_003.73");
    return BuildDirectionVector((uint32 *)psx_addr(output, 12), scale, (const uint16 *)psx_addr(angles, 4));
}

extern "C" uint32 xport_draft_host_sub_800667CC_p3(uint32 argument1, uint32 argument2, void *argument3)
{
    FUNCTION_MARKER(0x800667CC, "SLUS_003.73");
    return BuildDirectionVector((uint32 *)psx_addr(argument1, 12), argument2, (const uint16 *)argument3);
}

extern "C" uint32 xport_draft_host_sub_800667CC_p1(void *output, uint32 scale, uint32 angles)
{
    FUNCTION_MARKER(0x800667CC, "SLUS_003.73");
    return BuildDirectionVector((uint32 *)output, scale, (const uint16 *)psx_addr(angles, 4));
}

extern "C" uint32 xport_draft_host_sub_800667CC_p13(void *output, uint32 scale, const void *angles)
{
    FUNCTION_MARKER(0x800667CC, "SLUS_003.73");
    return BuildDirectionVector((uint32 *)output, scale, (const uint16 *)angles);
}

uint32 *OffsetPositionByAngles(uint32 *position, const uint16 *angles)
{
    uint32 direction[3];
    FUNCTION_MARKER(0x80022B54, "SLUS_003.73");
    BuildDirectionVector(direction, 100, angles);
    return SubtractVectors(position, direction);
}

extern "C" uint32 sub_80022B54(uint32 unused_object, uint32 position, uint32 angles)
{
    FUNCTION_MARKER(0x80022B54, "SLUS_003.73");
    OffsetPositionByAngles((uint32 *)psx_addr(position, 12), (const uint16 *)psx_addr(angles, 4));
    return position;
}

extern "C" void xport_draft_host_sub_80022B54_p2(uint32 unused_object, void *position, uint32 angles)
{
    FUNCTION_MARKER(0x80022B54, "SLUS_003.73");
    OffsetPositionByAngles((uint32 *)position, (const uint16 *)psx_addr(angles, 4));
}

extern "C" void xport_draft_host_sub_80022B54_p23(uint32 unused_object, void *position, const sint16 *angles)
{
    FUNCTION_MARKER(0x80022B54, "SLUS_003.73");
    OffsetPositionByAngles((uint32 *)position, (const uint16 *)angles);
}

// Existing native adapter for an original unaligned angle-word read pair
extern "C" uint32 apocalypse_angle_pair_word(uint32 address)
{
    uint32 left = r_u32((address + 3) & 0xFFFFFFFC);
    uint32 right = r_u32(address & 0xFFFFFFFC);
    uint32 shift = (address & 3) * 8;
    return shift ? (left << (32 - shift)) | (right >> shift) : right;
}

// Read the selected lanes of an original partial-word load
static uint32 ReadAngleWordLanes(uint32 address, uint32 mask)
{
    uint32 result = 0, lane;
    if (mask == 15)
        return r_u32(address);
    for (lane = 0; lane < 4; ++lane)
        if (mask & (1 << lane))
            result |= (uint32)r_u8(address + lane) << (lane * 8);
    return result;
}

uint32 CopyAngleVector(uint16 *output, const uint16 *source)
{
    const uint32 input = GuestMemoryAddress(source);
    const uint32 destination = GuestMemoryAddress(output);
    uint32 upper, lower, word, shift, last, lane;
    uint16 tail;
    if (input)
    {
        upper = ReadAngleWordLanes((input + 3) & ~3, (1 << (((input + 3) & 3) + 1)) - 1);
        lower = ReadAngleWordLanes(input & ~3, (15 << (input & 3)) & 15);
        shift = (input & 3) * 8;
        word = shift ? (upper << (32 - shift)) | (lower >> shift) : lower;
        tail = r_u16(input + 4);
    }
    else
    {
        memcpy(&word, source, 4);
        memcpy(&tail, (const uint8 *)source + 4, 2);
    }
    if (destination)
    {
        last = destination + 3;
        lane = last & 3;
        w_masked_u32(last & ~3, word >> (8 * (3 - lane)), (1 << (lane + 1)) - 1);
        lane = destination & 3;
        w_masked_u32(destination & ~3, word << (8 * lane), (15 << lane) & 15);
        w_u16(destination + 4, tail);
    }
    else
    {
        memcpy(output, &word, 4);
        memcpy(output, &word, 4);
        memcpy((uint8 *)output + 4, &tail, 2);
    }
    return word;
}

extern "C" uint32 apocalypse_effect_angles_copy(uint32 input, const void *native_input, uint32 output)
{
    return CopyAngleVector((uint16 *)psx_addr(output, 6), native_input ? (const uint16 *)native_input : (const uint16 *)psx_addr(input, 6));
}

extern "C" uint32 apocalypse_effect_direction_native(const void *angles)
{
    return CopyAngleVector((uint16 *)psx_addr(0x800FF414, 6), (const uint16 *)angles);
}

uint32 UpdateAngleAcceleration(SVECTOR current, uint16 *velocity, uint16 *acceleration, SVECTOR target, uint32 scale)
{
    uint32 velocity_address = GuestMemoryAddress(velocity), acceleration_address = GuestMemoryAddress(acceleration);
    sint16 dx = (sint16)((uint16)target.vx - (uint16)current.vx);
    sint16 dy = (sint16)((uint16)target.vy - (uint16)current.vy);
    uint32 result;
    FUNCTION_MARKER(0x80066CF0, "SLUS_003.73");
    // TODO Recover the original name of the eight-byte angle value type
    if (dx < -2048)
        dx = (sint16)((uint16)dx + 4096);
    if (dx > 2048)
        dx = (sint16)((uint16)dx - 4096);
    if (dy < -2048)
        dy = (sint16)((uint16)dy + 4096);
    if (dy > 2048)
        dy = (sint16)((uint16)dy - 4096);
    if (!dx && !dy)
    {
        WriteAngleHalf(velocity, velocity_address, 0, 0);
        WriteAngleHalf(velocity, velocity_address, 1, 0);
        return 0;
    }
    result = (uint32)((sint32)(scale * (uint32)(sint32)dx) >> 8);
    WriteAngleHalf(acceleration, acceleration_address, 0, result);
    result = (uint32)((sint32)(scale * (uint32)(sint32)dy) >> 8);
    WriteAngleHalf(acceleration, acceleration_address, 1, result);
    return result;
}

extern "C" uint32 sub_80066CF0(uint32 current_xy, uint32 current_z, uint32 velocity, uint32 acceleration, uint32 target_xy, uint32 target_z, uint32 scale)
{
    FUNCTION_MARKER(0x80066CF0, "SLUS_003.73");
    SVECTOR current, target;
    current.vx = (sint16)current_xy;
    current.vy = (sint16)(current_xy >> 16);
    current.vz = (sint16)current_z;
    current.pad = (sint16)(current_z >> 16);
    target.vx = (sint16)target_xy;
    target.vy = (sint16)(target_xy >> 16);
    target.vz = (sint16)target_z;
    target.pad = (sint16)(target_z >> 16);
    return UpdateAngleAcceleration(current, (uint16 *)psx_addr(velocity, 4), (uint16 *)psx_addr(acceleration, 4), target, scale);
}

static uint32 ReadDirectionPosition(const uint32 *position, uint32 address, uint32 axis)
{
    uint32 value;
    if (address)
        return r_u32(address + axis * 4);
    memcpy(&value, (const uint8 *)position + axis * 4, 4);
    return value;
}

static uint32 DivideDirectionSigned(uint32 numerator, uint32 denominator)
{
    if (!denominator)
        return (sint32)numerator < 0 ? 1 : (uint32)-1;
    if (numerator == 0x80000000 && denominator == (uint32)-1)
        return numerator;
    return (uint32)((sint32)numerator / (sint32)denominator);
}

uint32 CalculateDirectionAngles(uint16 *angles, const uint32 *origin, const uint32 *target)
{
    uint32 output = GuestMemoryAddress(angles), from = GuestMemoryAddress(origin), to = GuestMemoryAddress(target);
    uint32 target_x, origin_x, target_y, origin_y, origin_z, target_z, distance, angle, pitch, yaw;
    sint32 x, y, z;
    FUNCTION_MARKER(0x80066B8C, "SLUS_003.73");
    target_x = ReadDirectionPosition(target, to, 0);
    origin_x = ReadDirectionPosition(origin, from, 0);
    target_y = ReadDirectionPosition(target, to, 1);
    x = (sint32)(target_x - origin_x) >> 12;
    origin_y = ReadDirectionPosition(origin, from, 1);
    origin_z = ReadDirectionPosition(origin, from, 2);
    target_z = ReadDirectionPosition(target, to, 2);
    z = (sint32)(target_z - origin_z) >> 12;
    y = (sint32)(target_y - origin_y) >> 12;
    if (z > 0)
        angle = 2048 - CalculateAngleFromSlope((sint32)DivideDirectionSigned((uint32)x << 12, 0 - (uint32)z));
    else if (z < 0)
        angle = CalculateAngleFromSlope((sint32)DivideDirectionSigned((uint32)x << 12, (uint32)z));
    else
        angle = x > 0 ? (uint32)-1024 : 1024;
    WriteAngleHalf(angles, output, 1, angle);
    distance = LookupDistanceRoot((uint32)x * (uint32)x + (uint32)z * (uint32)z);
    if (!distance)
        angle = y > 0 ? 1024 : (uint32)-1024;
    else if (y > 0)
        angle = CalculateAngleFromSlope((sint32)(((uint32)y << 12) / distance));
    else
        angle = 0 - CalculateAngleFromSlope((sint32)(((0 - (uint32)y) << 12) / distance));
    WriteAngleHalf(angles, output, 0, angle);
    pitch = ReadAngleHalf(angles, output, 0);
    yaw = ReadAngleHalf(angles, output, 1);
    WriteAngleHalf(angles, output, 2, 0);
    WriteAngleHalf(angles, output, 0, pitch & 4095);
    WriteAngleHalf(angles, output, 1, yaw & 4095);
    return distance;
}

extern "C" uint32 sub_80066B8C(uint32 angles, uint32 origin, uint32 target)
{
    FUNCTION_MARKER(0x80066B8C, "SLUS_003.73");
    return CalculateDirectionAngles((uint16 *)psx_addr(angles, 6), (const uint32 *)psx_addr(origin, 12), (const uint32 *)psx_addr(target, 12));
}

extern "C" uint32 apocalypse_rotation_between(sint16 angles[3], const uint32 origin[3], const uint32 target[3])
{
    return CalculateDirectionAngles((uint16 *)angles, origin, target);
}

static uint32 ReadYawVectorWord(const uint32 *input, uint32 address, uint32 axis)
{
    uint32 value;
    if (address)
        return r_u32(address + axis * 4);
    memcpy(&value, (const uint8 *)input + axis * 4, 4);
    return value;
}

uint32 RotateVectorYaw(uint32 *output, const uint32 *input, uint32 angle)
{
    uint32 destination = GuestMemoryAddress(output), source = GuestMemoryAddress(input);
    uint32 table = 0x800F863C + (angle & 4095) * 4;
    uint32 component, coefficient, left, right, value;
    FUNCTION_MARKER(0x800665CC, "SLUS_003.73");
    component = ReadYawVectorWord(input, source, 0);
    coefficient = (uint32)(sint32)(sint16)r_u16(table + 2);
    left = (uint32)((sint32)component >> 3) * coefficient;
    component = ReadYawVectorWord(input, source, 2);
    coefficient = (uint32)(sint32)(sint16)r_u16(table);
    right = (uint32)((sint32)component >> 3) * coefficient;
    value = (uint32)((sint32)(left + right) >> 9);
    WriteDirectionWord(output, destination, 0, value);
    value = ReadYawVectorWord(input, source, 1);
    WriteDirectionWord(output, destination, 1, value);
    component = ReadYawVectorWord(input, source, 2);
    coefficient = (uint32)(sint32)(sint16)r_u16(table + 2);
    left = (uint32)((sint32)component >> 3) * coefficient;
    component = ReadYawVectorWord(input, source, 0);
    coefficient = (uint32)(sint32)(sint16)r_u16(table);
    right = (uint32)((sint32)component >> 3) * coefficient;
    value = (uint32)((sint32)(left - right) >> 9);
    WriteDirectionWord(output, destination, 2, value);
    return value;
}

extern "C" uint32 sub_800665CC(uint32 output, uint32 input, uint32 angle)
{
    FUNCTION_MARKER(0x800665CC, "SLUS_003.73");
    return RotateVectorYaw((uint32 *)psx_addr(output, 12), (const uint32 *)psx_addr(input, 12), angle);
}

extern "C" uint32 xport_draft_host_sub_800665CC_p1(void *output, uint32 input, uint32 angle)
{
    FUNCTION_MARKER(0x800665CC, "SLUS_003.73");
    return RotateVectorYaw((uint32 *)output, (const uint32 *)psx_addr(input, 12), angle);
}

static uint32 ReadModelMatrixWord(const void *input, uint32 offset)
{
    uint32 address = GuestMemoryAddress(input);
    return address ? r_u32(address + offset) : xport_load_le32((const uint8 *)input + offset);
}

static uint32 ReadModelMatrixHalf(const void *input, uint32 offset)
{
    uint32 address = GuestMemoryAddress(input);
    return address ? r_u16(address + offset) : xport_load_le16((const uint8 *)input + offset);
}

static void StoreModelMatrixHalf(uint16 *output, uint32 offset, uint16 value)
{
    uint32 address = GuestMemoryAddress(output);
    if (address)
        w_u16(address + offset, value);
    else
        xport_store_le16((uint8 *)output + offset, value);
}

void BuildModelRotationMatrix(const uint32 *input, uint16 *output)
{
    FUNCTION_MARKER(0x800858FC, "SLUS_003.73");
    uint32 angle_xy = ReadModelMatrixWord(input, 0);
    uint32 angle_z, x, y, z, x_cos, packed, ir_x, ir_z, yz, saved0, saved3;
    MATRIX saved_rotation;
    ReadRotMatrix(&saved_rotation);
    saved0 = xport_load_le32((const uint8 *)&saved_rotation);
    saved3 = xport_load_le32((const uint8 *)&saved_rotation + 12);
    xport_gte_write_data(1, 0);
    angle_z = ReadModelMatrixWord(input, 4);
    x = r_u32(0x800F863C + 4 * (angle_xy & 0xFFF));
    y = r_u32(0x800F863C + 4 * (angle_z & 0xFFF));
    xport_gte_write_control(0, x);
    x_cos = x >> 16;
    xport_gte_write_control(3, x_cos);
    xport_gte_write_data(0, y & 0xFFFF);
    xport_gte_execute(0x486012);
    z = r_u32(0x800F863C + ((angle_xy >> 14) & 0x3FFC));
    xport_gte_write_data(0, y >> 16);
    packed = (z >> 16) - (z << 16);
    ir_x = xport_gte_read_data(9);
    yz = xport_gte_read_data(11);
    xport_gte_execute(0x486012);
    xport_gte_write_control(0, z);
    xport_gte_write_control(3, packed);
    ir_x = ((ir_x ^ y) & 0xFFFF) ^ y;
    xport_gte_write_data(0, ir_x);
    ir_z = xport_gte_read_data(11);
    ir_x = xport_gte_read_data(9);
    xport_gte_execute(0x486012);
    ir_x = (ir_x & 0xFFFF) - (y << 16);
    xport_gte_write_data(0, ir_x);
    ir_x = xport_gte_read_data(9);
    packed = xport_gte_read_data(11);
    xport_gte_execute(0x486012);
    StoreModelMatrixHalf(output, 0, (uint16)ir_x);
    StoreModelMatrixHalf(output, 12, (uint16)packed);
    StoreModelMatrixHalf(output, 6, (uint16)yz);
    xport_gte_write_data(0, x_cos);
    ir_x = xport_gte_read_data(9);
    packed = xport_gte_read_data(11);
    xport_gte_execute(0x486012);
    StoreModelMatrixHalf(output, 2, (uint16)ir_x);
    StoreModelMatrixHalf(output, 14, (uint16)packed);
    StoreModelMatrixHalf(output, 10, (uint16)(0 - x));
    StoreModelMatrixHalf(output, 8, (uint16)ir_z);
    xport_gte_write_control(0, saved0);
    xport_gte_write_control(3, saved3);
    ir_x = xport_gte_read_data(9);
    packed = xport_gte_read_data(11);
    StoreModelMatrixHalf(output, 4, (uint16)ir_x);
    StoreModelMatrixHalf(output, 16, (uint16)packed);
}
void ScaleModelRotationMatrix(const uint8 *input, uint16 *output)
{
    FUNCTION_MARKER(0x80085A08, "SLUS_003.73");
    uint32 scale, x, y, z, previous_x, previous_y, previous_z;

    scale = (uint32)(sint32)(sint16)ReadModelMatrixHalf(input, 36);
    x = (uint32)(sint32)(sint16)ReadModelMatrixHalf(output, 0);
    y = (uint32)(sint32)(sint16)ReadModelMatrixHalf(output, 6);
    z = (uint32)(sint32)(sint16)ReadModelMatrixHalf(output, 12);
    xport_gte_write_data(8, scale);
    xport_gte_write_data(9, x);
    xport_gte_write_data(10, y);
    xport_gte_write_data(11, z);
    xport_draft_gte_execute(0x198003D);

    scale = (uint32)(sint32)(sint16)ReadModelMatrixHalf(input, 38);
    x = (uint32)(sint32)(sint16)ReadModelMatrixHalf(output, 2);
    y = (uint32)(sint32)(sint16)ReadModelMatrixHalf(output, 8);
    z = (uint32)(sint32)(sint16)ReadModelMatrixHalf(output, 14);
    previous_x = xport_gte_read_data(9);
    previous_y = xport_gte_read_data(10);
    previous_z = xport_gte_read_data(11);
    xport_gte_write_data(8, scale);
    xport_gte_write_data(9, x);
    xport_gte_write_data(10, y);
    xport_gte_write_data(11, z);
    StoreModelMatrixHalf(output, 0, (uint16)previous_x);
    StoreModelMatrixHalf(output, 6, (uint16)previous_y);
    xport_draft_gte_execute(0x198003D);
    StoreModelMatrixHalf(output, 12, (uint16)previous_z);

    scale = (uint32)(sint32)(sint16)ReadModelMatrixHalf(input, 40);
    x = (uint32)(sint32)(sint16)ReadModelMatrixHalf(output, 4);
    y = (uint32)(sint32)(sint16)ReadModelMatrixHalf(output, 10);
    z = (uint32)(sint32)(sint16)ReadModelMatrixHalf(output, 16);
    previous_x = xport_gte_read_data(9);
    previous_y = xport_gte_read_data(10);
    previous_z = xport_gte_read_data(11);
    xport_gte_write_data(8, scale);
    xport_gte_write_data(9, x);
    xport_gte_write_data(10, y);
    xport_gte_write_data(11, z);
    StoreModelMatrixHalf(output, 2, (uint16)previous_x);
    StoreModelMatrixHalf(output, 8, (uint16)previous_y);
    xport_draft_gte_execute(0x198003D);
    StoreModelMatrixHalf(output, 14, (uint16)previous_z);

    previous_x = xport_gte_read_data(9);
    previous_y = xport_gte_read_data(10);
    previous_z = xport_gte_read_data(11);
    StoreModelMatrixHalf(output, 4, (uint16)previous_x);
    StoreModelMatrixHalf(output, 10, (uint16)previous_y);
    StoreModelMatrixHalf(output, 16, (uint16)previous_z);
}

extern "C" void sub_800858FC(uint32 input, uint32 output)
{
    FUNCTION_MARKER(0x800858FC, "SLUS_003.73");
    BuildModelRotationMatrix((const uint32 *)psx_addr(input, 8), (uint16 *)psx_addr(output, 18));
}

extern "C" void apocalypse_rotation_matrix(uint32 input, void *output)
{
    BuildModelRotationMatrix((const uint32 *)psx_addr(input, 8), (uint16 *)output);
}

extern "C" uint32 xport_draft_host_sub_800858FC_p2(uint32 input, void *output)
{
    FUNCTION_MARKER(0x800858FC, "SLUS_003.73");
    apocalypse_rotation_matrix(input, output);
    return 0;
}

extern "C" void sub_80085A08(uint32 input, uint32 output)
{
    FUNCTION_MARKER(0x80085A08, "SLUS_003.73");
    ScaleModelRotationMatrix((const uint8 *)psx_addr(input, 42), (uint16 *)psx_addr(output, 18));
}

extern "C" uint32 xport_draft_host_sub_80085A08_p2(uint32 input, void *output)
{
    FUNCTION_MARKER(0x80085A08, "SLUS_003.73");
    ScaleModelRotationMatrix((const uint8 *)psx_addr(input, 42), (uint16 *)output);
    return 0;
}

void LoadFixedGteTranslation(const uint32 *position)
{
    FUNCTION_MARKER(0x80084504, "SLUS_003.73");
    uint32 x = ReadModelMatrixWord(position, 0);
    uint32 y = ReadModelMatrixWord(position, 4);
    uint32 z = ReadModelMatrixWord(position, 8);
    xport_gte_write_control(5, (uint32)((sint32)x >> 12));
    xport_gte_write_control(6, (uint32)((sint32)y >> 12));
    xport_gte_write_control(7, (uint32)((sint32)z >> 12));
}

extern "C" void sub_80084504(uint32 position)
{
    FUNCTION_MARKER(0x80084504, "SLUS_003.73");
    LoadFixedGteTranslation((const uint32 *)psx_addr(position, 12));
}

uint32 InverseCosineAngle(uint32 value)
{
    FUNCTION_MARKER(0x80067930, "SLUS_003.73");
    uint32 angle = 512, negative = (sint32)value < 0;
    sint32 step = 256;
    if (negative)
        value = 0 - value;
    do
    {
        sint32 cosine = (sint16)r_u16(0x800F863C + angle * 4 + 2);
        if ((sint32)value < cosine)
            angle += (uint32)step;
        else if ((sint32)value == cosine)
            break;
        else
            angle -= (uint32)step;
        step >>= 1;
    } while (step);
    return negative ? 2048 - angle : angle;
}

extern "C" uint32 sub_80067930(uint32 value)
{
    FUNCTION_MARKER(0x80067930, "SLUS_003.73");
    return InverseCosineAngle(value);
}

void TransposeRotationMatrix(uint16 *matrix)
{
    FUNCTION_MARKER(0x80085ACC, "SLUS_003.73");
    uint16 first = (uint16)ReadModelMatrixHalf(matrix, 2);
    uint16 second = (uint16)ReadModelMatrixHalf(matrix, 4);
    uint16 third = (uint16)ReadModelMatrixHalf(matrix, 6);
    uint16 fourth = (uint16)ReadModelMatrixHalf(matrix, 10);
    uint16 fifth = (uint16)ReadModelMatrixHalf(matrix, 12);
    uint16 sixth = (uint16)ReadModelMatrixHalf(matrix, 14);
    StoreModelMatrixHalf(matrix, 6, first);
    StoreModelMatrixHalf(matrix, 12, second);
    StoreModelMatrixHalf(matrix, 2, third);
    StoreModelMatrixHalf(matrix, 14, fourth);
    StoreModelMatrixHalf(matrix, 4, fifth);
    StoreModelMatrixHalf(matrix, 10, sixth);
}

void TransposeRotationMatrix(const uint16 *input, uint16 *output)
{
    FUNCTION_MARKER(0x80085B04, "SLUS_003.73");
    uint16 values[9];
    const uint32 offsets[9] = {0, 6, 12, 2, 8, 14, 4, 10, 16};
    for (uint32 index = 0; index < 9; ++index)
        values[index] = (uint16)ReadModelMatrixHalf(input, index * 2);
    for (uint32 index = 0; index < 9; ++index)
        StoreModelMatrixHalf(output, offsets[index], values[index]);
}

extern "C" void sub_80085ACC(uint32 matrix)
{
    FUNCTION_MARKER(0x80085ACC, "SLUS_003.73");
    TransposeRotationMatrix((uint16 *)psx_addr(matrix, 18));
}
extern "C" void sub_80085B04(uint32 input, uint32 output)
{
    FUNCTION_MARKER(0x80085B04, "SLUS_003.73");
    TransposeRotationMatrix((const uint16 *)psx_addr(input, 18), (uint16 *)psx_addr(output, 18));
}

sint32 CalculateAngleFromSlope(sint32 slope)
{
    uint32 x = 4096, y = (uint32)slope, angle = 0;
    uint32 step;
    for (step = 0; step < 12; ++step)
    {
        uint32 x_shift = (uint32)((sint32)x >> step);
        uint32 y_shift = (uint32)((sint32)y >> step);
        uint32 increment = r_u32(0x800F82DC + 4 * step);
        if ((sint32)y >= 0)
        {
            x += y_shift;
            y -= x_shift;
            angle += increment;
        }
        else
        {
            x -= y_shift;
            y += x_shift;
            angle -= increment;
        }
    }
    return (sint32)angle;
}

extern "C" sint32 apocalypse_angle_from_slope(sint32 slope)
{
    return CalculateAngleFromSlope(slope);
}
