#include "angle_math.h"
#include "vector_arithmetic.h"
#include "game_diagnostics.h"
#include "quaternion_math.h"
#include "vector_distance.h"

static uint32 DivideQuaternionWord(uint32 numerator, sint32 denominator)
{
    sint32 value = (sint32)numerator;
    if (!denominator)
        return value < 0 ? 1 : (uint32)-1;
    if (value == (sint32)0x80000000 && denominator == -1)
        return 0x80000000;
    return (uint32)(value / denominator);
}

uint32 InterpolateQuaternion(Quaternion &from, const Quaternion &to, uint32 amount, Quaternion &output)
{
    FUNCTION_MARKER(0x80076800, "SLUS_003.73");
    uint32 first = (uint32)from.x, sum, dot, weight1, weight2, angle, index, product, result;
    uint32 left, right;
    uint32 component1, component2, component3;
    sint32 denominator;
    right = (uint32)to.x;
    sum = first * right;
    for (index = 1; index < 4; ++index)
    {
        left = (uint32)from[index];
        right = (uint32)to[index];
        sum += left * right;
    }
    dot = (uint32)((sint32)sum >> 12);
    if ((sint32)dot < 0)
    {
        dot = 0 - dot;
        component1 = (uint32)from.y;
        from.x = 0 - first;
        component3 = (uint32)from.w;
        from.y = 0 - component1;
        component2 = (uint32)from.z;
        from.w = 0 - component3;
        from.z = 0 - component2;
    }
    if ((sint32)(dot + 4096) < 129)
    {
        output.x = 0 - (uint32)from.y;
        output.y = 0 - (uint32)from.x;
        output.z = 0 - (uint32)from.w;
        output.w = (uint32)from.z;
        weight1 = (uint32)rsin((3217 * (4096 - amount)) >> 11);
        left = (uint32)from.x;
        weight2 = (uint32)rsin((3217 * amount) >> 11);
        right = (uint32)output.x;
        product = weight1 * left + weight2 * right;
        result = (uint32)((sint32)product >> 12);
        output.x = result;
        for (index = 1; index < 4; ++index)
        {
            left = (uint32)from[index];
            right = (uint32)output[index];
            product = weight1 * left + weight2 * right;
            result = (uint32)((sint32)product >> 12);
            output[index] = result;
        }
    }
    else
    {
        weight1 = 4096 - amount;
        weight2 = amount;
        if ((sint32)(4096 - dot) >= 129)
        {
            angle = InverseCosineAngle(dot);
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
            left = (uint32)from[index];
            right = (uint32)to[index];
            product = weight1 * left + weight2 * right;
            result = (uint32)((sint32)product >> 12);
            output[index] = result;
        }
    }
    return result;
}

static uint32 ScaleMatrixQuaternionValue(uint32 value, uint32 factor)
{
    return (uint32)((sint32)(value * factor) >> 12);
}

uint32 ConvertMatrixToQuaternion(const MATRIX &matrix, Quaternion &output)
{
    uint32 root, factor, result, left, right, selected, next, last;
    sint32 diagonal[3], trace, current;
    FUNCTION_MARKER(0x80076420, "SLUS_003.73");
    diagonal[0] = matrix.m[0][0];
    diagonal[1] = matrix.m[1][1];
    diagonal[2] = matrix.m[2][2];
    trace = diagonal[0] + diagonal[1] + diagonal[2];
    selected = diagonal[0] < diagonal[1];
    if (trace > 0)
    {
        root = SquareRoot0((sint32)(((uint32)trace + 4096) << 12));
        factor = DivideQuaternionWord(0x800000, (sint32)root);
        output.w = (uint32)((sint32)root >> 1);
        left = (uint32)matrix.m[1][2];
        right = (uint32)matrix.m[2][1];
        output.x = ScaleMatrixQuaternionValue(left - right, factor);
        left = (uint32)matrix.m[2][0];
        right = (uint32)matrix.m[0][2];
        output.y = ScaleMatrixQuaternionValue(left - right, factor);
        left = (uint32)matrix.m[0][1];
        right = (uint32)matrix.m[1][0];
        result = ScaleMatrixQuaternionValue(left - right, factor);
        output.z = result;
        return result;
    }
    current = matrix.m[selected][selected];
    if (current < diagonal[2])
        selected = 2;
    static const uint32 next_axis[3] = {1, 2, 0};
    next = next_axis[selected];
    last = next_axis[next];
    left = (uint32)matrix.m[next][next];
    right = (uint32)matrix.m[last][last];
    current = matrix.m[selected][selected];
    root = SquareRoot0((sint32)(((uint32)current - (left + right) + 4096) << 12));
    output[selected] = (sint32)root >> 1;
    factor = DivideQuaternionWord(0x800000, (sint32)root);
    left = (uint32)matrix.m[next][last];
    right = (uint32)matrix.m[last][next];
    output.w = ScaleMatrixQuaternionValue(left - right, factor);
    left = (uint32)matrix.m[selected][next];
    right = (uint32)matrix.m[next][selected];
    output[next] = ScaleMatrixQuaternionValue(left + right, factor);
    left = (uint32)matrix.m[selected][last];
    right = (uint32)matrix.m[last][selected];
    result = ScaleMatrixQuaternionValue(left + right, factor);
    output[last] = result;
    return result;
}

Quaternion &BuildRollQuaternion(Quaternion &output, uint32 angle)
{
    uint32 half_angle = (angle >> 1) & 4095;
    uint32 cosine, sine;
    FUNCTION_MARKER(0x800762DC, "SLUS_003.73");
    cosine = (uint32)rcos(half_angle);
    sine = (uint32)rsin(half_angle);
    output.x = 0;
    output.y = 0;
    output.z = sine;
    output.w = cosine;
    return output;
}

uint32 ConvertQuaternionToMatrix(const Quaternion &quaternion, MATRIX &output)
{
    uint32 w, x, y, z, wx, wy, wz, xx, xy, xz, yy, yz, zz, result;
    FUNCTION_MARKER(0x80076310, "SLUS_003.73");
    w = (uint32)quaternion.w;
    x = (uint32)quaternion.x;
    y = (uint32)quaternion.y;
    z = (uint32)quaternion.z;
    wx = (uint32)((sint32)(w * x) >> 11);
    wy = (uint32)((sint32)(w * y) >> 11);
    wz = (uint32)((sint32)(w * z) >> 11);
    xx = (uint32)((sint32)(x * x) >> 11);
    xy = (uint32)((sint32)(x * y) >> 11);
    yy = (uint32)((sint32)(y * y) >> 11);
    zz = (uint32)((sint32)(z * z) >> 11);
    output.t[0] = 0;
    output.t[1] = 0;
    output.t[2] = 0;
    xz = (uint32)((sint32)(x * z) >> 11);
    yz = (uint32)((sint32)(y * z) >> 11);
    output.m[0][0] = (sint16)(4096 - (yy + zz));
    output.m[0][1] = (sint16)(xy + wz);
    output.m[1][0] = (sint16)(xy - wz);
    output.m[1][1] = (sint16)(4096 - (xx + zz));
    output.m[0][2] = (sint16)(xz - wy);
    output.m[2][0] = (sint16)(xz + wy);
    output.m[2][2] = (sint16)(4096 - (xx + yy));
    result = yz + wx;
    output.m[1][2] = (sint16)(result);
    output.m[2][1] = (sint16)(yz - wx);
    return result;
}

Quaternion &BuildPitchQuaternion(Quaternion &output, uint32 angle)
{
    uint32 half_angle = (angle >> 1) & 4095;
    uint32 cosine, sine;
    FUNCTION_MARKER(0x80076274, "SLUS_003.73");
    cosine = (uint32)rcos(half_angle);
    sine = (uint32)rsin(half_angle);
    output.y = 0;
    output.z = 0;
    output.x = sine;
    output.w = cosine;
    return output;
}

Quaternion &BuildYawQuaternion(Quaternion &output, uint32 angle)
{
    uint32 half_angle = (angle >> 1) & 4095;
    uint32 cosine, sine;
    FUNCTION_MARKER(0x800762A8, "SLUS_003.73");
    cosine = (uint32)rcos(half_angle);
    sine = (uint32)rsin(half_angle);
    output.x = 0;
    output.z = 0;
    output.y = sine;
    output.w = cosine;
    return output;
}

Quaternion &MultiplyQuaternions(Quaternion &output, const Quaternion &first, const Quaternion &second)
{
    Quaternion left, right, product;
    FUNCTION_MARKER(0x80075F80, "SLUS_003.73");
    right.x = second.x;
    right.y = second.y;
    right.z = second.z;
    right.w = second.w;
    left.w = first.w;
    left.x = first.x;
    left.y = first.y;
    left.z = first.z;
    // Unsigned products retain the original low-word arithmetic
    product.x = (sint32)((uint32)right.w * (uint32)left.x + (uint32)right.x * (uint32)left.w + (uint32)right.y * (uint32)left.z - (uint32)right.z * (uint32)left.y) >> 12;
    product.y = (sint32)((uint32)right.w * (uint32)left.y + (uint32)right.y * (uint32)left.w + (uint32)right.z * (uint32)left.x - (uint32)right.x * (uint32)left.z) >> 12;
    product.z = (sint32)((uint32)right.w * (uint32)left.z + (uint32)right.z * (uint32)left.w + (uint32)right.x * (uint32)left.y - (uint32)right.y * (uint32)left.x) >> 12;
    product.w = (sint32)((uint32)right.w * (uint32)left.w - (uint32)right.x * (uint32)left.x - (uint32)right.y * (uint32)left.y - (uint32)right.z * (uint32)left.z) >> 12;
    output.x = product.x;
    output.y = product.y;
    output.z = product.z;
    output.w = product.w;
    return output;
}

Quaternion &BuildAxisAngleQuaternion(Quaternion &output, const Vector3 &rotation_vector)
{
    Vector3 normal;
    uint32 x, y, z, length, half_angle, sine, cosine, result_x, result_y, result_z;
    FUNCTION_MARKER(0x80076168, "SLUS_003.73");
    x = (uint32)rotation_vector.x;
    y = (uint32)rotation_vector.y;
    z = (uint32)rotation_vector.z;
    length = LookupDistanceRoot(x * x + y * y + z * z);
    if (!length)
    {
        output.x = 0;
        output.y = 0;
        output.z = 0;
        output.w = 4096;
        return output;
    }
    // Retain the reviewed native PsyQ normalization boundary
    Vector3 vector = rotation_vector;
    NormalizeShortVector(vector.words, normal.words);
    half_angle = (length >> 1) & 4095;
    sine = (uint32)rsin(half_angle);
    result_x = sine * normal.words[0];
    result_y = sine * normal.words[1];
    result_z = sine * normal.words[2];
    cosine = (uint32)rcos(half_angle);
    output.w = cosine;
    output.x = (uint32)((sint32)result_x >> 12);
    output.y = (uint32)((sint32)result_y >> 12);
    output.z = (uint32)((sint32)result_z >> 12);
    return output;
}
