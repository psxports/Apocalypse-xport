#include "vector_arithmetic.h"
#include <stdint.h>
extern "C"
{
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
}
#include "quaternion_math.h"
#include "vector_distance.h"
#include "native_object_storage.h"
#include <string.h>

// Native word bridges retain input mutation and aliased output behavior
static uint32 ReadQuaternionWord(const uint32 *input, uint32 index)
{
    uint32 address = GuestMemoryAddress(input), value;
    if (address)
        return r_u32(address + index * 4);
    memcpy(&value, (const uint8 *)input + index * 4, 4);
    return value;
}

static void WriteQuaternionWord(uint32 *output, uint32 index, uint32 value)
{
    uint32 address = GuestMemoryAddress(output);
    if (address)
        w_u32(address + index * 4, value);
    else
        memcpy((uint8 *)output + index * 4, &value, 4);
}

static uint32 DivideQuaternionWord(uint32 numerator, sint32 denominator)
{
    sint32 value = (sint32)numerator;
    if (!denominator)
        return value < 0 ? 1 : (uint32)-1;
    if (value == (sint32)0x80000000 && denominator == -1)
        return 0x80000000;
    return (uint32)(value / denominator);
}

uint32 InterpolateQuaternion(uint32 *from, const uint32 *to, uint32 amount, uint32 *output)
{
    FUNCTION_MARKER(0x80076800, "SLUS_003.73");
    uint32 first = ReadQuaternionWord(from, 0), sum, dot, weight1, weight2, angle, index, product, result;
    uint32 left, right;
    uint32 component1, component2, component3;
    sint32 denominator;
    right = ReadQuaternionWord(to, 0);
    sum = first * right;
    for (index = 1; index < 4; ++index)
    {
        left = ReadQuaternionWord(from, index);
        right = ReadQuaternionWord(to, index);
        sum += left * right;
    }
    dot = (uint32)((sint32)sum >> 12);
    if ((sint32)dot < 0)
    {
        dot = 0 - dot;
        component1 = ReadQuaternionWord(from, 1);
        WriteQuaternionWord(from, 0, 0 - first);
        component3 = ReadQuaternionWord(from, 3);
        WriteQuaternionWord(from, 1, 0 - component1);
        component2 = ReadQuaternionWord(from, 2);
        WriteQuaternionWord(from, 3, 0 - component3);
        WriteQuaternionWord(from, 2, 0 - component2);
    }
    if ((sint32)(dot + 4096) < 129)
    {
        WriteQuaternionWord(output, 0, 0 - ReadQuaternionWord(from, 1));
        WriteQuaternionWord(output, 1, 0 - ReadQuaternionWord(from, 0));
        WriteQuaternionWord(output, 2, 0 - ReadQuaternionWord(from, 3));
        WriteQuaternionWord(output, 3, ReadQuaternionWord(from, 2));
        weight1 = (uint32)rsin((3217 * (4096 - amount)) >> 11);
        left = ReadQuaternionWord(from, 0);
        weight2 = (uint32)rsin((3217 * amount) >> 11);
        right = ReadQuaternionWord(output, 0);
        product = weight1 * left + weight2 * right;
        result = (uint32)((sint32)product >> 12);
        WriteQuaternionWord(output, 0, result);
        for (index = 1; index < 4; ++index)
        {
            left = ReadQuaternionWord(from, index);
            right = ReadQuaternionWord(output, index);
            product = weight1 * left + weight2 * right;
            result = (uint32)((sint32)product >> 12);
            WriteQuaternionWord(output, index, result);
        }
    }
    else
    {
        weight1 = 4096 - amount;
        weight2 = amount;
        if ((sint32)(4096 - dot) >= 129)
        {
            angle = sub_80067930(dot);
            product = (4096 - amount) * angle;
            product = (uint32)((sint32)product >> 12);
            left = (uint32)rsin(product) << 12;
            denominator = rsin(angle);
            weight1 = DivideQuaternionWord(left, denominator);
            product = amount * angle;
            product = (uint32)((sint32)product >> 12);
            weight2 = DivideQuaternionWord((uint32)rsin(product) << 12, denominator);
        }
        for (index = 0; index < 4; ++index)
        {
            left = ReadQuaternionWord(from, index);
            right = ReadQuaternionWord(to, index);
            product = weight1 * left + weight2 * right;
            result = (uint32)((sint32)product >> 12);
            WriteQuaternionWord(output, index, result);
        }
    }
    return result;
}


static sint32 ReadQuaternionMatrixHalf(const sint16 *matrix, uint32 address, uint32 index)
{
    sint16 value;
    if (address)
        return (sint16)r_u16(address + index * 2);
    memcpy(&value, (const uint8 *)matrix + index * 2, 2);
    return value;
}

static uint32 ScaleMatrixQuaternionValue(uint32 value, uint32 factor)
{
    return (uint32)((sint32)(value * factor) >> 12);
}

uint32 ConvertMatrixToQuaternion(const sint16 *matrix, uint32 *output)
{
    uint32 address = GuestMemoryAddress(matrix), root, factor, result, left, right;
    sint32 diagonal[3], trace, selected, next, last, current;
    FUNCTION_MARKER(0x80076420, "SLUS_003.73");
    diagonal[0] = ReadQuaternionMatrixHalf(matrix, address, 0);
    diagonal[1] = ReadQuaternionMatrixHalf(matrix, address, 4);
    diagonal[2] = ReadQuaternionMatrixHalf(matrix, address, 8);
    trace = diagonal[0] + diagonal[1] + diagonal[2];
    selected = diagonal[0] < diagonal[1];
    if (trace > 0)
    {
        root = SquareRoot0((sint32)(((uint32)trace + 4096) << 12));
        factor = DivideQuaternionWord(0x800000, (sint32)root);
        WriteQuaternionWord(output, 3, (uint32)((sint32)root >> 1));
        left = (uint32)ReadQuaternionMatrixHalf(matrix, address, 5);
        right = (uint32)ReadQuaternionMatrixHalf(matrix, address, 7);
        WriteQuaternionWord(output, 0, ScaleMatrixQuaternionValue(left - right, factor));
        left = (uint32)ReadQuaternionMatrixHalf(matrix, address, 6);
        right = (uint32)ReadQuaternionMatrixHalf(matrix, address, 2);
        WriteQuaternionWord(output, 1, ScaleMatrixQuaternionValue(left - right, factor));
        left = (uint32)ReadQuaternionMatrixHalf(matrix, address, 1);
        right = (uint32)ReadQuaternionMatrixHalf(matrix, address, 3);
        result = ScaleMatrixQuaternionValue(left - right, factor);
        WriteQuaternionWord(output, 2, result);
        return result;
    }
    current = ReadQuaternionMatrixHalf(matrix, address, (uint32)selected * 4);
    if (current < diagonal[2])
        selected = 2;
    next = (sint32)r_u32(0x800ED4C8 + (uint32)selected * 4);
    last = (sint32)r_u32(0x800ED4C8 + (uint32)next * 4);
    left = (uint32)ReadQuaternionMatrixHalf(matrix, address, (uint32)next * 4);
    right = (uint32)ReadQuaternionMatrixHalf(matrix, address, (uint32)last * 4);
    current = ReadQuaternionMatrixHalf(matrix, address, (uint32)selected * 4);
    root = SquareRoot0((sint32)(((uint32)current - (left + right) + 4096) << 12));
    if (selected == 0 || selected == 1 || selected == 2)
        WriteQuaternionWord(output, (uint32)selected, (uint32)((sint32)root >> 1));
    factor = DivideQuaternionWord(0x800000, (sint32)root);
    left = (uint32)ReadQuaternionMatrixHalf(matrix, address, (uint32)next * 3 + (uint32)last);
    right = (uint32)ReadQuaternionMatrixHalf(matrix, address, (uint32)last * 3 + (uint32)next);
    WriteQuaternionWord(output, 3, ScaleMatrixQuaternionValue(left - right, factor));
    if (next == 0 || next == 1 || next == 2)
    {
        left = (uint32)ReadQuaternionMatrixHalf(matrix, address, (uint32)selected * 3 + (uint32)next);
        right = (uint32)ReadQuaternionMatrixHalf(matrix, address, (uint32)next * 3 + (uint32)selected);
        WriteQuaternionWord(output, (uint32)next, ScaleMatrixQuaternionValue(left + right, factor));
    }
    result = last >= 2 ? 2 : 1;
    if (last == 0 || last == 1 || last == 2)
    {
        left = (uint32)ReadQuaternionMatrixHalf(matrix, address, (uint32)selected * 3 + (uint32)last);
        right = (uint32)ReadQuaternionMatrixHalf(matrix, address, (uint32)last * 3 + (uint32)selected);
        result = ScaleMatrixQuaternionValue(left + right, factor);
        WriteQuaternionWord(output, (uint32)last, result);
    }
    return result;
}



extern "C" uint32 xport_draft_host_sub_80076420_p1(void *matrix, uint32 output)
{
    FUNCTION_MARKER(0x80076420, "SLUS_003.73");
    return ConvertMatrixToQuaternion((const sint16 *)matrix, (uint32 *)psx_addr(output, 16));
}

uint32 *BuildRollQuaternion(uint32 *output, uint32 angle)
{
    uint32 half_angle = (angle >> 1) & 4095;
    uint32 cosine, sine;
    FUNCTION_MARKER(0x800762DC, "SLUS_003.73");
    cosine = (uint32)rcos(half_angle);
    sine = (uint32)rsin(half_angle);
    WriteQuaternionWord(output, 0, 0);
    WriteQuaternionWord(output, 1, 0);
    WriteQuaternionWord(output, 2, sine);
    WriteQuaternionWord(output, 3, cosine);
    return output;
}

static void WriteQuaternionMatrixHalf(MATRIX *output, uint32 address, uint32 index, uint32 value)
{
    if (address)
        w_u16(address + index * 2, value);
    else
        output->m[index / 3][index % 3] = (sint16)value;
}

uint32 ConvertQuaternionToMatrix(const uint32 *quaternion, MATRIX *output)
{
    uint32 address = GuestMemoryAddress(output), w, x, y, z, wx, wy, wz, xx, xy, xz, yy, yz, zz, result;
    FUNCTION_MARKER(0x80076310, "SLUS_003.73");
    w = ReadQuaternionWord(quaternion, 3);
    x = ReadQuaternionWord(quaternion, 0);
    y = ReadQuaternionWord(quaternion, 1);
    z = ReadQuaternionWord(quaternion, 2);
    wx = (uint32)((sint32)(w * x) >> 11);
    wy = (uint32)((sint32)(w * y) >> 11);
    wz = (uint32)((sint32)(w * z) >> 11);
    xx = (uint32)((sint32)(x * x) >> 11);
    xy = (uint32)((sint32)(x * y) >> 11);
    yy = (uint32)((sint32)(y * y) >> 11);
    zz = (uint32)((sint32)(z * z) >> 11);
    if (address)
    {
        w_u32(address + 20, 0);
        w_u32(address + 24, 0);
        w_u32(address + 28, 0);
    }
    else
    {
        output->t[0] = 0;
        output->t[1] = 0;
        output->t[2] = 0;
    }
    xz = (uint32)((sint32)(x * z) >> 11);
    yz = (uint32)((sint32)(y * z) >> 11);
    WriteQuaternionMatrixHalf(output, address, 0, 4096 - (yy + zz));
    WriteQuaternionMatrixHalf(output, address, 1, xy + wz);
    WriteQuaternionMatrixHalf(output, address, 3, xy - wz);
    WriteQuaternionMatrixHalf(output, address, 4, 4096 - (xx + zz));
    WriteQuaternionMatrixHalf(output, address, 2, xz - wy);
    WriteQuaternionMatrixHalf(output, address, 6, xz + wy);
    WriteQuaternionMatrixHalf(output, address, 8, 4096 - (xx + yy));
    result = yz + wx;
    WriteQuaternionMatrixHalf(output, address, 5, result);
    WriteQuaternionMatrixHalf(output, address, 7, yz - wx);
    return result;
}



extern "C" uint32 xport_draft_host_sub_800762DC_p1(void *output, uint32 angle)
{
    FUNCTION_MARKER(0x800762DC, "SLUS_003.73");
    return (uint32)(uintptr_t)BuildRollQuaternion((uint32 *)output, angle);
}



uint32 *BuildPitchQuaternion(uint32 *output, uint32 angle)
{
    uint32 half_angle = (angle >> 1) & 4095;
    uint32 cosine, sine;
    FUNCTION_MARKER(0x80076274, "SLUS_003.73");
    cosine = (uint32)rcos(half_angle);
    sine = (uint32)rsin(half_angle);
    WriteQuaternionWord(output, 1, 0);
    WriteQuaternionWord(output, 2, 0);
    WriteQuaternionWord(output, 0, sine);
    WriteQuaternionWord(output, 3, cosine);
    return output;
}

uint32 *BuildYawQuaternion(uint32 *output, uint32 angle)
{
    uint32 half_angle = (angle >> 1) & 4095;
    uint32 cosine, sine;
    FUNCTION_MARKER(0x800762A8, "SLUS_003.73");
    cosine = (uint32)rcos(half_angle);
    sine = (uint32)rsin(half_angle);
    WriteQuaternionWord(output, 0, 0);
    WriteQuaternionWord(output, 2, 0);
    WriteQuaternionWord(output, 1, sine);
    WriteQuaternionWord(output, 3, cosine);
    return output;
}

uint32 *MultiplyQuaternions(uint32 *output, const uint32 *first, const uint32 *second)
{
    uint32 a[4], b[4], result[4];
    FUNCTION_MARKER(0x80075F80, "SLUS_003.73");
    b[0] = ReadQuaternionWord(second, 0);
    b[1] = ReadQuaternionWord(second, 1);
    b[2] = ReadQuaternionWord(second, 2);
    b[3] = ReadQuaternionWord(second, 3);
    a[3] = ReadQuaternionWord(first, 3);
    a[0] = ReadQuaternionWord(first, 0);
    a[1] = ReadQuaternionWord(first, 1);
    a[2] = ReadQuaternionWord(first, 2);
    result[0] = (uint32)((sint32)(b[3] * a[0] + b[0] * a[3] + b[1] * a[2] - b[2] * a[1]) >> 12);
    result[1] = (uint32)((sint32)(b[3] * a[1] + b[1] * a[3] + b[2] * a[0] - b[0] * a[2]) >> 12);
    result[2] = (uint32)((sint32)(b[3] * a[2] + b[2] * a[3] + b[0] * a[1] - b[1] * a[0]) >> 12);
    result[3] = (uint32)((sint32)(b[3] * a[3] - b[0] * a[0] - b[1] * a[1] - b[2] * a[2]) >> 12);
    WriteQuaternionWord(output, 0, result[0]);
    WriteQuaternionWord(output, 1, result[1]);
    WriteQuaternionWord(output, 2, result[2]);
    WriteQuaternionWord(output, 3, result[3]);
    return output;
}







extern "C" uint32 xport_draft_host_sub_80076274_p1(void *output, uint32 angle)
{
    FUNCTION_MARKER(0x80076274, "SLUS_003.73");
    return (uint32)(uintptr_t)BuildPitchQuaternion((uint32 *)output, angle);
}

extern "C" uint32 xport_draft_host_sub_800762A8_p1(void *output, uint32 angle)
{
    FUNCTION_MARKER(0x800762A8, "SLUS_003.73");
    return (uint32)(uintptr_t)BuildYawQuaternion((uint32 *)output, angle);
}

extern "C" uint32 xport_draft_host_sub_80075F80_p12(void *output, void *first, uint32 second)
{
    FUNCTION_MARKER(0x80075F80, "SLUS_003.73");
    return (uint32)(uintptr_t)MultiplyQuaternions((uint32 *)output, (const uint32 *)first, (const uint32 *)psx_addr(second, 16));
}

extern "C" uint32 xport_draft_host_sub_80075F80_p123(void *output, void *first, void *second)
{
    FUNCTION_MARKER(0x80075F80, "SLUS_003.73");
    return (uint32)(uintptr_t)MultiplyQuaternions((uint32 *)output, (const uint32 *)first, (const uint32 *)second);
}

uint32 *BuildAxisAngleQuaternion(uint32 *output, const uint32 *rotation_vector)
{
    uint32 x, y, z, length, half_angle, sine, cosine, normal[4], result_x, result_y, result_z;
    FUNCTION_MARKER(0x80076168, "SLUS_003.73");
    x = ReadQuaternionWord(rotation_vector, 0);
    y = ReadQuaternionWord(rotation_vector, 1);
    z = ReadQuaternionWord(rotation_vector, 2);
    length = LookupDistanceRoot(x * x + y * y + z * z);
    if (!length)
    {
        WriteQuaternionWord(output, 0, 0);
        WriteQuaternionWord(output, 1, 0);
        WriteQuaternionWord(output, 2, 0);
        WriteQuaternionWord(output, 3, 4096);
        return output;
    }
    // Retain the reviewed native PsyQ normalization boundary
    NormalizeShortVector(rotation_vector, normal);
    half_angle = (length >> 1) & 4095;
    sine = (uint32)rsin(half_angle);
    result_x = sine * normal[0];
    result_y = sine * normal[1];
    result_z = sine * normal[2];
    cosine = (uint32)rcos(half_angle);
    WriteQuaternionWord(output, 3, cosine);
    WriteQuaternionWord(output, 0, (uint32)((sint32)result_x >> 12));
    WriteQuaternionWord(output, 1, (uint32)((sint32)result_y >> 12));
    WriteQuaternionWord(output, 2, (uint32)((sint32)result_z >> 12));
    return output;
}


