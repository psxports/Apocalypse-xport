extern "C"
{
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
}
#include "vector_arithmetic.h"
#include "vector_distance.h"
#include <string.h>
#include <stdint.h>

static uint32 ReadArithmeticWord(const uint32 *input, uint32 axis)
{
    uint32 value;
    memcpy(&value, (const uint8 *)input + axis * 4, 4);
    return value;
}

static void WriteArithmeticWord(uint32 *output, uint32 axis, uint32 value)
{
    memcpy((uint8 *)output + axis * 4, &value, 4);
}

// Native buffer adapter shared by the two recovered vector operations
static uint32 *CombineVectors(uint32 *output, const uint32 *left, const uint32 *right, bool subtract)
{

    uint32 values[3], axis, a, b;
    for (axis = 0; axis < 3; axis += 1)
    {
        a = ReadArithmeticWord(left, axis);
        b = ReadArithmeticWord(right, axis);
        values[axis] = subtract ? a - b : a + b;
    }
    for (axis = 0; axis < 3; axis += 1)
        WriteArithmeticWord(output, axis, values[axis]);
    return output;
}

uint32 *AddVectors(uint32 *output, const uint32 *left, const uint32 *right)
{
    FUNCTION_MARKER(0x8006C34C, "SLUS_003.73");
    return CombineVectors(output, left, right, false);
}

uint32 *SubtractVectors(uint32 *output, const uint32 *left, const uint32 *right)
{
    FUNCTION_MARKER(0x8006C3AC, "SLUS_003.73");
    return CombineVectors(output, left, right, true);
}

extern "C" uint32 xport_draft_host_sub_8006C3AC_p13(void *output, uint32 matrix, const void *input)
{
    FUNCTION_MARKER(0x8006C3AC, "SLUS_003.73");
    SubtractVectors((uint32 *)output, (const uint32 *)psx_addr(matrix, 12), (const uint32 *)input);
    return (uint32)(uintptr_t)output;
}

extern "C" uint32 xport_draft_host_sub_8006C34C_p13(void *output, uint32 matrix, const void *input)
{
    FUNCTION_MARKER(0x8006C34C, "SLUS_003.73");
    AddVectors((uint32 *)output, (const uint32 *)psx_addr(matrix, 12), (const uint32 *)input);
    return (uint32)(uintptr_t)output;
}

extern "C" uint32 xport_draft_host_sub_8006C3AC_p123(void *output, const void *matrix, const void *input)
{
    FUNCTION_MARKER(0x8006C3AC, "SLUS_003.73");
    SubtractVectors((uint32 *)output, (const uint32 *)matrix, (const uint32 *)input);
    return (uint32)(uintptr_t)output;
}

extern "C" uint32 xport_draft_host_sub_8006C34C_p123(void *output, const void *matrix, const void *input)
{
    FUNCTION_MARKER(0x8006C34C, "SLUS_003.73");
    AddVectors((uint32 *)output, (const uint32 *)matrix, (const uint32 *)input);
    return (uint32)(uintptr_t)output;
}

extern "C" uint32 xport_draft_host_sub_8006C3AC_p1(void *argument1, uint32 argument2, uint32 argument3)
{
    FUNCTION_MARKER(0x8006C3AC, "SLUS_003.73");
    SubtractVectors((uint32 *)argument1, (const uint32 *)psx_addr(argument2, 12), (const uint32 *)psx_addr(argument3, 12));
    return (uint32)(uintptr_t)argument1;
}

extern "C" uint32 xport_draft_host_sub_8006C34C_p12(void *argument1, void *argument2, uint32 argument3)
{
    FUNCTION_MARKER(0x8006C34C, "SLUS_003.73");
    AddVectors((uint32 *)argument1, (const uint32 *)argument2, (const uint32 *)psx_addr(argument3, 12));
    return (uint32)(uintptr_t)argument1;
}

extern "C" uint32 xport_draft_host_sub_8006C34C_p1(void *argument1, uint32 argument2, uint32 argument3)
{
    FUNCTION_MARKER(0x8006C34C, "SLUS_003.73");
    AddVectors((uint32 *)argument1, (const uint32 *)psx_addr(argument2, 12), (const uint32 *)psx_addr(argument3, 12));
    return (uint32)(uintptr_t)argument1;
}

// Native buffer adapter retains a fresh scalar read for each original component
static uint32 *ApplyVectorScalar(uint32 *output, const uint32 *vector, const uint32 *scalar, bool shift)
{

    uint32 values[3], axis, value, factor;
    for (axis = 0; axis < 3; axis += 1)
    {
        value = ReadArithmeticWord(vector, axis);
        factor = ReadArithmeticWord(scalar, 0);
        values[axis] = shift ? (uint32)((sint32)value >> (factor & 31)) : value * factor;
    }
    for (axis = 0; axis < 3; axis += 1)
        WriteArithmeticWord(output, axis, values[axis]);
    return output;
}

uint32 *MultiplyVector(uint32 *output, const uint32 *vector, const uint32 *scalar)
{
    FUNCTION_MARKER(0x8006C40C, "SLUS_003.73");
    return ApplyVectorScalar(output, vector, scalar, false);
}

uint32 *ShiftVectorRight(uint32 *output, const uint32 *vector, const uint32 *shift)
{
    FUNCTION_MARKER(0x8006C564, "SLUS_003.73");
    return ApplyVectorScalar(output, vector, shift, true);
}

extern "C" uint32 xport_draft_host_sub_8006C40C_p123(void *output, const void *matrix, const void *input)
{
    FUNCTION_MARKER(0x8006C40C, "SLUS_003.73");
    MultiplyVector((uint32 *)output, (const uint32 *)matrix, (const uint32 *)input);
    return (uint32)(uintptr_t)output;
}

extern "C" uint32 xport_draft_host_sub_8006C564_p123(void *argument1, const void *argument2, const void *argument3)
{
    FUNCTION_MARKER(0x8006C564, "SLUS_003.73");
    ShiftVectorRight((uint32 *)argument1, (const uint32 *)argument2, (const uint32 *)argument3);
    return (uint32)(uintptr_t)argument1;
}

extern "C" uint32 xport_draft_host_sub_8006C40C_p1(void *argument1, uint32 argument2, uint32 argument3)
{
    FUNCTION_MARKER(0x8006C40C, "SLUS_003.73");
    MultiplyVector((uint32 *)argument1, (const uint32 *)psx_addr(argument2, 12), (const uint32 *)psx_addr(argument3, 4));
    return (uint32)(uintptr_t)argument1;
}

extern "C" uint32 xport_draft_host_sub_8006C564_p13(void *argument1, uint32 argument2, void *argument3)
{
    FUNCTION_MARKER(0x8006C564, "SLUS_003.73");
    ShiftVectorRight((uint32 *)argument1, (const uint32 *)psx_addr(argument2, 12), (const uint32 *)argument3);
    return (uint32)(uintptr_t)argument1;
}

extern "C" uint32 xport_draft_host_sub_8006C40C_p12(void *argument1, void *argument2, uint32 argument3)
{
    FUNCTION_MARKER(0x8006C40C, "SLUS_003.73");
    MultiplyVector((uint32 *)argument1, (const uint32 *)argument2, (const uint32 *)psx_addr(argument3, 4));
    return (uint32)(uintptr_t)argument1;
}

extern "C" uint32 xport_draft_host_sub_8006C40C_p13(void *output, uint32 vector, const void *scale)
{
    FUNCTION_MARKER(0x8006C40C, "SLUS_003.73");
    MultiplyVector((uint32 *)output, (const uint32 *)psx_addr(vector, 12), (const uint32 *)scale);
    return (uint32)(uintptr_t)output;
}

uint32 *ShiftVectorLeft(uint32 *output, const uint32 *vector, const uint32 *shift)
{

    uint32 values[3], axis, value, amount;
    FUNCTION_MARKER(0x8006C5C4, "SLUS_003.73");
    for (axis = 0; axis < 3; axis += 1)
    {
        value = ReadArithmeticWord(vector, axis);
        amount = ReadArithmeticWord(shift, 0);
        values[axis] = value << (amount & 31);
    }
    for (axis = 0; axis < 3; axis += 1)
        WriteArithmeticWord(output, axis, values[axis]);
    return output;
}

uint32 *DampVector(uint32 *vector, const uint8 *shifts)
{

    uint32 axis, amount, value;
    FUNCTION_MARKER(0x8006C270, "SLUS_003.73");
    for (axis = 0; axis < 3; axis += 1)
    {
        amount = shifts[axis];
        value = ReadArithmeticWord(vector, axis);
        value -= (uint32)((sint32)value >> (amount & 31));
        WriteArithmeticWord(vector, axis, value);
    }
    return vector;
}

extern "C" uint32 sub_8006C270(uint32 vector, uint32 shifts)
{
    FUNCTION_MARKER(0x8006C270, "SLUS_003.73");
    DampVector((uint32 *)psx_addr(vector, 12), (const uint8 *)psx_addr(shifts, 3));
    return vector;
}

extern "C" void xport_draft_host_sub_8006C270_p1(void *vector, uint32 shifts)
{
    FUNCTION_MARKER(0x8006C270, "SLUS_003.73");
    DampVector((uint32 *)vector, (const uint8 *)psx_addr(shifts, 3));
}

extern "C" uint32 xport_draft_host_sub_8006C5C4_p13(void *argument1, uint32 argument2, void *argument3)
{
    FUNCTION_MARKER(0x8006C5C4, "SLUS_003.73");
    ShiftVectorLeft((uint32 *)argument1, (const uint32 *)psx_addr(argument2, 12), (const uint32 *)argument3);
    return (uint32)(uintptr_t)argument1;
}

extern "C" uint32 xport_draft_host_sub_8006C5C4_p123(void *argument1, void *argument2, void *argument3)
{
    FUNCTION_MARKER(0x8006C5C4, "SLUS_003.73");
    ShiftVectorLeft((uint32 *)argument1, (const uint32 *)argument2, (const uint32 *)argument3);
    return (uint32)(uintptr_t)argument1;
}

// Native buffer adapter preserves per-axis read, parameter read and immediate store
static uint32 *ShiftVectorInPlace(uint32 *vector, const uint32 *shift, bool right)
{

    uint32 axis, value, amount;
    for (axis = 0; axis < 3; axis += 1)
    {
        value = ReadArithmeticWord(vector, axis);
        amount = ReadArithmeticWord(shift, 0) & 31;
        value = right ? (uint32)((sint32)value >> amount) : value << amount;
        WriteArithmeticWord(vector, axis, value);
    }
    return vector;
}

uint32 *ShiftVectorLeft(uint32 *vector, const uint32 *shift)
{
    FUNCTION_MARKER(0x8006C22C, "SLUS_003.73");
    return ShiftVectorInPlace(vector, shift, false);
}

uint32 *ShiftVectorRight(uint32 *vector, const uint32 *shift)
{
    FUNCTION_MARKER(0x8006C1E8, "SLUS_003.73");
    return ShiftVectorInPlace(vector, shift, true);
}

extern "C" uint32 xport_draft_host_sub_8006C22C_p2(uint32 argument1, void *argument2)
{
    FUNCTION_MARKER(0x8006C22C, "SLUS_003.73");
    ShiftVectorLeft((uint32 *)psx_addr(argument1, 12), (const uint32 *)argument2);
    return argument1;
}

extern "C" uint32 xport_draft_host_sub_8006C22C_p12(void *argument1, void *argument2)
{
    FUNCTION_MARKER(0x8006C22C, "SLUS_003.73");
    ShiftVectorLeft((uint32 *)argument1, (const uint32 *)argument2);
    return (uint32)(uintptr_t)argument1;
}

extern "C" uint32 xport_draft_host_sub_8006C1E8_p2(uint32 argument1, void *argument2)
{
    FUNCTION_MARKER(0x8006C1E8, "SLUS_003.73");
    ShiftVectorRight((uint32 *)psx_addr(argument1, 12), (const uint32 *)argument2);
    return argument1;
}

extern "C" uint32 xport_draft_host_sub_8006C1E8_p12(void *vector, const void *scale)
{
    FUNCTION_MARKER(0x8006C1E8, "SLUS_003.73");
    ShiftVectorRight((uint32 *)vector, (const uint32 *)scale);
    return (uint32)(uintptr_t)vector;
}

// Original DIV low result without host signed division overflow
static uint32 DivideVectorWord(uint32 numerator, uint32 denominator)
{
    if (!denominator)
        return (sint32)numerator < 0 ? 1 : 0xFFFFFFFF;
    if (numerator == 0x80000000 && denominator == 0xFFFFFFFF)
        return numerator;
    return (uint32)((sint32)numerator / (sint32)denominator);
}

uint32 *DivideVector(uint32 *output, const uint32 *vector, const uint32 *scalar)
{

    uint32 values[3], axis, numerator, denominator;
    FUNCTION_MARKER(0x8006C4EC, "SLUS_003.73");
    for (axis = 0; axis < 3; axis += 1)
    {
        numerator = ReadArithmeticWord(vector, axis);
        denominator = ReadArithmeticWord(scalar, 0);
        values[axis] = DivideVectorWord(numerator, denominator);
    }
    for (axis = 0; axis < 3; axis += 1)
        WriteArithmeticWord(output, axis, values[axis]);
    return output;
}

extern "C" uint32 xport_draft_host_sub_8006C4EC_p123(void *output, const void *matrix, const void *input)
{
    FUNCTION_MARKER(0x8006C4EC, "SLUS_003.73");
    DivideVector((uint32 *)output, (const uint32 *)matrix, (const uint32 *)input);
    return (uint32)(uintptr_t)output;
}

extern "C" uint32 xport_draft_host_sub_8006C4EC_p13(void *argument1, uint32 argument2, void *argument3)
{
    FUNCTION_MARKER(0x8006C4EC, "SLUS_003.73");
    DivideVector((uint32 *)argument1, (const uint32 *)psx_addr(argument2, 12), (const uint32 *)argument3);
    return (uint32)(uintptr_t)argument1;
}

extern "C" void xport_draft_host_sub_8006C4EC_p12(void *output, void *input, uint32 divisor)
{
    FUNCTION_MARKER(0x8006C4EC, "SLUS_003.73");
    DivideVector((uint32 *)output, (const uint32 *)input, (const uint32 *)psx_addr(divisor, 4));
}

extern "C" uint32 xport_draft_host_sub_8006C4EC_p1(void *output, uint32 input, uint32 scalar)
{
    FUNCTION_MARKER(0x8006C4EC, "SLUS_003.73");
    DivideVector((uint32 *)output, (const uint32 *)psx_addr(input, 12), (const uint32 *)psx_addr(scalar, 4));
    return (uint32)(uintptr_t)output;
}

// Native buffer adapter preserves immediate stores for partially overlapping vectors
static uint32 *AccumulateVector(uint32 *vector, const uint32 *source, bool subtract)
{

    uint32 axis, left, right;
    for (axis = 0; axis < 3; axis += 1)
    {
        left = ReadArithmeticWord(vector, axis);
        right = ReadArithmeticWord(source, axis);
        WriteArithmeticWord(vector, axis, subtract ? left - right : left + right);
    }
    return vector;
}

uint32 *AddVectors(uint32 *vector, const uint32 *source)
{
    FUNCTION_MARKER(0x8006C0B8, "SLUS_003.73");
    return AccumulateVector(vector, source, false);
}

uint32 *SubtractVectors(uint32 *vector, const uint32 *source)
{
    FUNCTION_MARKER(0x8006C0FC, "SLUS_003.73");
    return AccumulateVector(vector, source, true);
}

extern "C" uint32 sub_8006C0B8(uint32 destination, uint32 source)
{
    FUNCTION_MARKER(0x8006C0B8, "SLUS_003.73");
    AddVectors((uint32 *)psx_addr(destination, 12), (const uint32 *)psx_addr(source, 12));
    return destination;
}

extern "C" uint32 xport_draft_host_sub_8006C0B8_p2(uint32 destination, void *source)
{
    FUNCTION_MARKER(0x8006C0B8, "SLUS_003.73");
    AddVectors((uint32 *)psx_addr(destination, 12), (const uint32 *)source);
    return destination;
}

extern "C" uint32 xport_draft_host_sub_8006C0B8_p1(void *destination, uint32 source)
{
    FUNCTION_MARKER(0x8006C0B8, "SLUS_003.73");
    AddVectors((uint32 *)destination, (const uint32 *)psx_addr(source, 12));
    return (uint32)(uintptr_t)destination;
}

extern "C" uint32 xport_draft_host_sub_8006C0B8_p12(void *destination, const void *source)
{
    FUNCTION_MARKER(0x8006C0B8, "SLUS_003.73");
    AddVectors((uint32 *)destination, (const uint32 *)source);
    return (uint32)(uintptr_t)destination;
}

extern "C" uint32 xport_draft_host_sub_8006C0FC_p2(uint32 output, void *input)
{
    FUNCTION_MARKER(0x8006C0FC, "SLUS_003.73");
    SubtractVectors((uint32 *)psx_addr(output, 12), (const uint32 *)input);
    return output;
}

extern "C" uint32 xport_draft_host_sub_8006C0FC_p12(void *output, const void *input)
{
    FUNCTION_MARKER(0x8006C0FC, "SLUS_003.73");
    SubtractVectors((uint32 *)output, (const uint32 *)input);
    return (uint32)(uintptr_t)output;
}

uint32 *DivideVector(uint32 *vector, const uint32 *scalar)
{

    uint32 axis, numerator, denominator;
    FUNCTION_MARKER(0x8006C190, "SLUS_003.73");
    for (axis = 0; axis < 3; axis += 1)
    {
        numerator = ReadArithmeticWord(vector, axis);
        denominator = ReadArithmeticWord(scalar, 0);
        WriteArithmeticWord(vector, axis, DivideVectorWord(numerator, denominator));
    }
    return vector;
}

uint32 SnapSmallVector(uint32 *vector)
{
    uint32 axis, value, small = 0;
    FUNCTION_MARKER(0x8006C05C, "SLUS_003.73");
    for (axis = 0; axis < 3; axis += 1)
    {
        value = ReadArithmeticWord(vector, axis);
        small = value + 2048 < 4097;
        if (small)
            WriteArithmeticWord(vector, axis, 0);
    }
    // Original return describes the final component only
    return small;
}

extern "C" uint32 sub_8006C05C(uint32 vector)
{
    FUNCTION_MARKER(0x8006C05C, "SLUS_003.73");
    return SnapSmallVector((uint32 *)psx_addr(vector, 12));
}

uint32 VectorsDiffer(const uint32 *left, const uint32 *right)
{

    uint32 axis, a, b;
    FUNCTION_MARKER(0x8006C304, "SLUS_003.73");
    for (axis = 0; axis < 3; axis += 1)
    {
        a = ReadArithmeticWord(left, axis);
        b = ReadArithmeticWord(right, axis);
        if (a != b)
            return 1;
    }
    return 0;
}

extern "C" uint32 sub_8006C304(uint32 left, uint32 right)
{
    FUNCTION_MARKER(0x8006C304, "SLUS_003.73");
    return VectorsDiffer((const uint32 *)psx_addr(left, 12), (const uint32 *)psx_addr(right, 12));
}

extern "C" uint32 apocalypse_vector_different_native(uint32 left, const void *right)
{
    return VectorsDiffer((const uint32 *)psx_addr(left, 12), (const uint32 *)right);
}

uint32 *MultiplyScalarVector(uint32 *output, const uint32 *scalar, const uint32 *vector)
{
    FUNCTION_MARKER(0x8006C47C, "SLUS_003.73");
    return ApplyVectorScalar(output, vector, scalar, false);
}

extern "C" uint32 sub_8006C47C(uint32 output, uint32 scalar, uint32 vector)
{
    FUNCTION_MARKER(0x8006C47C, "SLUS_003.73");
    MultiplyScalarVector((uint32 *)psx_addr(output, 12), (const uint32 *)psx_addr(scalar, 4), (const uint32 *)psx_addr(vector, 12));
    return output;
}

extern "C" uint32 xport_draft_host_sub_8006C47C_p13(void *argument1, uint32 argument2, void *argument3)
{
    FUNCTION_MARKER(0x8006C47C, "SLUS_003.73");
    MultiplyScalarVector((uint32 *)argument1, (const uint32 *)psx_addr(argument2, 4), (const uint32 *)argument3);
    return (uint32)(uintptr_t)argument1;
}

extern "C" uint32 xport_draft_host_sub_8006C47C_p12(void *output, const void *scale, uint32 vector)
{
    FUNCTION_MARKER(0x8006C47C, "SLUS_003.73");
    MultiplyScalarVector((uint32 *)output, (const uint32 *)scale, (const uint32 *)psx_addr(vector, 12));
    return (uint32)(uintptr_t)output;
}

extern "C" void xport_draft_host_sub_8006C47C_p123(void *output, const void *scale, const void *input)
{
    FUNCTION_MARKER(0x8006C47C, "SLUS_003.73");
    MultiplyScalarVector((uint32 *)output, (const uint32 *)scale, (const uint32 *)input);
}

static uint32 ReadShortVectorHalf(const uint16 *input, uint32 axis)
{
    uint16 value;
    memcpy(&value, (const uint8 *)input + axis * 2, 2);
    return value;
}

// Native buffer adapter for the original packed SWL/SWR and trailing halfword
static void WriteShortVectorResult(uint16 *output, const uint16 values[3])
{
    uint32 packed = (uint32)values[0] | ((uint32)values[1] << 16);
    memcpy(output, &packed, 4);
    memcpy((uint8 *)output + 4, values + 2, 2);
}

uint16 *MultiplyScalarShortVector(uint16 *output, const uint32 *scalar, const uint16 *vector)
{

    uint32 axis, value, factor;
    uint16 result[3];
    FUNCTION_MARKER(0x8006CBF8, "SLUS_003.73");
    for (axis = 0; axis < 3; axis += 1)
    {
        value = (uint32)(sint32)(sint16)ReadShortVectorHalf(vector, axis);
        factor = ReadArithmeticWord(scalar, 0);
        result[axis] = (uint16)(value * factor);
    }
    WriteShortVectorResult(output, result);
    return output;
}

uint16 *ShiftShortVectorRight(uint16 *output, const uint16 *vector, const uint32 *shift)
{

    uint32 axis, amount;
    sint32 value;
    uint16 result[3];
    FUNCTION_MARKER(0x8006CCE0, "SLUS_003.73");
    for (axis = 0; axis < 3; axis += 1)
    {
        value = (sint16)ReadShortVectorHalf(vector, axis);
        amount = ReadArithmeticWord(shift, 0) & 31;
        result[axis] = (uint16)(value >> amount);
    }
    WriteShortVectorResult(output, result);
    return output;
}

extern "C" uint32 xport_draft_host_sub_8006CBF8_p12(void *argument1, void *argument2, uint32 argument3)
{
    FUNCTION_MARKER(0x8006CBF8, "SLUS_003.73");
    MultiplyScalarShortVector((uint16 *)argument1, (const uint32 *)argument2, (const uint16 *)psx_addr(argument3, 6));
    return (uint32)(uintptr_t)argument1;
}

extern "C" uint32 xport_draft_host_sub_8006CCE0_p123(void *argument1, void *argument2, void *argument3)
{
    FUNCTION_MARKER(0x8006CCE0, "SLUS_003.73");
    ShiftShortVectorRight((uint16 *)argument1, (const uint16 *)argument2, (const uint32 *)argument3);
    return (uint32)(uintptr_t)argument1;
}

uint32 NormalizeShortVector(const uint32 *vector, uint32 *normal)
{
    FUNCTION_MARKER(0x800872BC, "SLUS_003.73");
    SVECTOR input, output;
    input.vx = (sint16)vector[0];
    input.vy = (sint16)vector[1];
    input.vz = (sint16)vector[2];
    input.pad = 0;
    uint32 sum = VectorNormalSS(&input, &output);
    // Preserve the original LZCS and LZCR device outputs
    xport_gte_write_data(30, sum);
    normal[0] = (uint32)(sint32)output.vx;
    normal[1] = (uint32)(sint32)output.vy;
    normal[2] = (uint32)(sint32)output.vz;
    return sum;
}

uint32 NormalizeFixedVector(uint32 *output, const uint32 *input)
{

    uint32 x, y, z, divisor, axis, component, result = 0;
    FUNCTION_MARKER(0x800666DC, "SLUS_003.73");
    x = ReadArithmeticWord(input, 0);
    y = ReadArithmeticWord(input, 1);
    z = ReadArithmeticWord(input, 2);
    xport_gte_write_data(9, (uint32)((sint32)x >> 12));
    xport_gte_write_data(10, (uint32)((sint32)y >> 12));
    xport_gte_write_data(11, (uint32)((sint32)z >> 12));
    xport_gte_execute(0xA00428);
    x = xport_gte_read_data(25);
    y = xport_gte_read_data(26);
    z = xport_gte_read_data(27);
    divisor = LookupDistanceRoot(x + y + z);
    if (!divisor)
    {
        WriteArithmeticWord(output, 0, 0);
        WriteArithmeticWord(output, 1, 0);
        WriteArithmeticWord(output, 2, 0);
        return 0;
    }
    for (axis = 0; axis < 3; ++axis)
    {
        component = ReadArithmeticWord(input, axis);
        result = DivideVectorWord(component, divisor);
        WriteArithmeticWord(output, axis, result);
    }
    return result;
}

extern "C" uint32 xport_draft_host_sub_800666DC_p12(void *output, void *input)
{
    FUNCTION_MARKER(0x800666DC, "SLUS_003.73");
    return NormalizeFixedVector((uint32 *)output, (const uint32 *)input);
}

extern "C" uint32 xport_draft_host_sub_800666DC_p1(void *output, uint32 input)
{
    FUNCTION_MARKER(0x800666DC, "SLUS_003.73");
    return NormalizeFixedVector((uint32 *)output, (const uint32 *)psx_addr(input, 12));
}

uint32 MultiplyDivideWord(uint32 first, uint32 second, uint32 divisor)
{
    FUNCTION_MARKER(0x80085BA4, "SLUS_003.73");
    uint64 product = (uint64)first * (uint64)second;
    uint32 high = (uint32)(product >> 32);
    uint32 low = (uint32)product;
    // Thirty-two shifts discard all bits of the original incoming V0
    uint32 result = 0;
    for (uint32 bit = 0; bit < 32; ++bit)
    {
        high = (high << 1) | (low >> 31);
        result <<= 1;
        if (high >= divisor)
        {
            high -= divisor;
            result |= 1;
        }
        low <<= 1;
    }
    return result;
}

