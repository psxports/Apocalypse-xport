#include "vector_arithmetic.h"
#include "vector_distance.h"

// Snapshot operands before writing aliased outputs
static uint32 *CombineVectors(uint32 *output, const uint32 *left, const uint32 *right, bool subtract)
{
    uint32 axis, a, b;
    Vector3 values;
    for (axis = 0; axis < 3; axis += 1)
    {
        a = left[axis];
        b = right[axis];
        values.words[axis] = subtract ? a - b : a + b;
    }
    for (axis = 0; axis < 3; axis += 1)
        output[axis] = values.words[axis];
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

// Read the scalar again for each original component
static uint32 *ApplyVectorScalar(uint32 *output, const uint32 *vector, const uint32 &scalar, bool shift)
{
    uint32 axis, value, factor;
    Vector3 values;
    for (axis = 0; axis < 3; axis += 1)
    {
        value = vector[axis];
        factor = scalar;
        values.words[axis] = shift ? (uint32)((sint32)value >> (factor & 31)) : value * factor;
    }
    for (axis = 0; axis < 3; axis += 1)
        output[axis] = values.words[axis];
    return output;
}

uint32 *MultiplyVector(uint32 *output, const uint32 *vector, const uint32 &scalar)
{
    FUNCTION_MARKER(0x8006C40C, "SLUS_003.73");
    return ApplyVectorScalar(output, vector, scalar, false);
}

uint32 *ShiftVectorRight(uint32 *output, const uint32 *vector, const uint32 &shift)
{
    FUNCTION_MARKER(0x8006C564, "SLUS_003.73");
    return ApplyVectorScalar(output, vector, shift, true);
}

uint32 *ShiftVectorLeft(uint32 *output, const uint32 *vector, const uint32 &shift)
{
    uint32 axis, value, amount;
    Vector3 values;
    FUNCTION_MARKER(0x8006C5C4, "SLUS_003.73");
    for (axis = 0; axis < 3; axis += 1)
    {
        value = vector[axis];
        amount = shift;
        values.words[axis] = value << (amount & 31);
    }
    for (axis = 0; axis < 3; axis += 1)
        output[axis] = values.words[axis];
    return output;
}

uint32 *DampVector(uint32 *vector, const uint8 *shifts)
{
    uint32 axis, amount, value;
    FUNCTION_MARKER(0x8006C270, "SLUS_003.73");
    for (axis = 0; axis < 3; axis += 1)
    {
        amount = shifts[axis];
        value = vector[axis];
        value -= (uint32)((sint32)value >> (amount & 31));
        vector[axis] = value;
    }
    return vector;
}

// Preserve per-axis reads and immediate stores
static uint32 *ShiftVectorInPlace(uint32 *vector, const uint32 &shift, bool right)
{
    uint32 axis, value, amount;
    for (axis = 0; axis < 3; axis += 1)
    {
        value = vector[axis];
        amount = shift & 31;
        value = right ? (uint32)((sint32)value >> amount) : value << amount;
        vector[axis] = value;
    }
    return vector;
}

uint32 *ShiftVectorLeft(uint32 *vector, const uint32 &shift)
{
    FUNCTION_MARKER(0x8006C22C, "SLUS_003.73");
    return ShiftVectorInPlace(vector, shift, false);
}

uint32 *ShiftVectorRight(uint32 *vector, const uint32 &shift)
{
    FUNCTION_MARKER(0x8006C1E8, "SLUS_003.73");
    return ShiftVectorInPlace(vector, shift, true);
}

// Original DIV low result without host signed division overflow
static uint32 DivideVectorWord(uint32 numerator, uint32 denominator)
{
    if (!denominator)
        return (sint32)numerator < 0 ? 1 : 0xFFFFFFFF;
    if (numerator == 0x80000000 && denominator == 0xFFFFFFFF)
        return numerator;
    return (sint32)numerator / (sint32)denominator;
}

uint32 *DivideVector(uint32 *output, const uint32 *vector, const uint32 &scalar)
{
    uint32 axis, numerator, denominator;
    Vector3 values;
    FUNCTION_MARKER(0x8006C4EC, "SLUS_003.73");
    for (axis = 0; axis < 3; axis += 1)
    {
        numerator = vector[axis];
        denominator = scalar;
        values.words[axis] = DivideVectorWord(numerator, denominator);
    }
    for (axis = 0; axis < 3; axis += 1)
        output[axis] = values.words[axis];
    return output;
}

// Preserve immediate stores for partially overlapping vectors
static uint32 *AccumulateVector(uint32 *vector, const uint32 *source, bool subtract)
{
    uint32 axis, left, right;
    for (axis = 0; axis < 3; axis += 1)
    {
        left = vector[axis];
        right = source[axis];
        vector[axis] = subtract ? left - right : left + right;
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

uint32 *DivideVector(uint32 *vector, const uint32 &scalar)
{
    uint32 axis, numerator, denominator;
    FUNCTION_MARKER(0x8006C190, "SLUS_003.73");
    for (axis = 0; axis < 3; axis += 1)
    {
        numerator = vector[axis];
        denominator = scalar;
        vector[axis] = DivideVectorWord(numerator, denominator);
    }
    return vector;
}

uint32 SnapSmallVector(uint32 *vector)
{
    uint32 axis, value, small = 0;
    FUNCTION_MARKER(0x8006C05C, "SLUS_003.73");
    for (axis = 0; axis < 3; axis += 1)
    {
        value = vector[axis];
        small = value + 2048 < 4097;
        if (small)
            vector[axis] = 0;
    }
    // Original return describes the final component only
    return small;
}

uint32 VectorsDiffer(const uint32 *left, const uint32 *right)
{
    uint32 axis, a, b;
    FUNCTION_MARKER(0x8006C304, "SLUS_003.73");
    for (axis = 0; axis < 3; axis += 1)
    {
        a = left[axis];
        b = right[axis];
        if (a != b)
            return 1;
    }
    return 0;
}

uint32 *MultiplyScalarVector(uint32 *output, const uint32 &scalar, const uint32 *vector)
{
    FUNCTION_MARKER(0x8006C47C, "SLUS_003.73");
    return ApplyVectorScalar(output, vector, scalar, false);
}

ShortVector3 &MultiplyScalarShortVector(ShortVector3 &output, const uint32 &scalar, const ShortVector3 &vector)
{
    ShortVector3 result;
    FUNCTION_MARKER(0x8006CBF8, "SLUS_003.73");
    for (uint32 axis = 0; axis < 3; ++axis)
    {
        uint32 value = vector[axis];
        uint32 factor = scalar;
        result[axis] = value * factor;
    }
    output.x = result.x;
    output.y = result.y;
    output.z = result.z;
    return output;
}

ShortVector3 &ShiftShortVectorRight(ShortVector3 &output, const ShortVector3 &vector, const uint32 &shift)
{
    ShortVector3 result;
    FUNCTION_MARKER(0x8006CCE0, "SLUS_003.73");
    for (uint32 axis = 0; axis < 3; ++axis)
    {
        sint32 value = vector[axis];
        uint32 amount = shift & 31;
        result[axis] = value >> amount;
    }
    output.x = result.x;
    output.y = result.y;
    output.z = result.z;
    return output;
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
    normal[0] = output.vx;
    normal[1] = output.vy;
    normal[2] = output.vz;
    return sum;
}

uint32 NormalizeFixedVector(uint32 *output, const uint32 *input)
{
    uint32 x, y, z, divisor, axis, component, result = 0;
    FUNCTION_MARKER(0x800666DC, "SLUS_003.73");
    x = input[0];
    y = input[1];
    z = input[2];
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
        output[0] = 0;
        output[1] = 0;
        output[2] = 0;
        return 0;
    }
    for (axis = 0; axis < 3; ++axis)
    {
        component = input[axis];
        result = DivideVectorWord(component, divisor);
        output[axis] = result;
    }
    return result;
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
