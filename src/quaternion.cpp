#include "angle_math.h"
#include "global.h"
#include "quaternion.h"

Quaternion &Quaternion::Multiply(const Quaternion &left, const Quaternion &right)
{
    FUNCTION_MARKER(0x80075F80, "SLUS_003.73");
    const uint32 rx = right.x, ry = right.y, rz = right.z, rw = right.w;
    const uint32 lw = left.w, lx = left.x, ly = left.y, lz = left.z;
    // Snapshot both inputs and retain low-word products before signed Q12 shifts
    x = (sint32)(rw * lx + rx * lw + ry * lz - rz * ly) >> 12;
    y = (sint32)(rw * ly + ry * lw + rz * lx - rx * lz) >> 12;
    z = (sint32)(rw * lz + rz * lw + rx * ly - ry * lx) >> 12;
    w = (sint32)(rw * lw - rx * lx - ry * ly - rz * lz) >> 12;
    return *this;
}

Quaternion &Quaternion::SetAxisAngle(const Vector3 &rotation_vector)
{
    FUNCTION_MARKER(0x80076168, "SLUS_003.73");
    uint32 x = rotation_vector.x;
    uint32 y = rotation_vector.y;
    uint32 z = rotation_vector.z;
    uint32 len = LookupDistanceRoot(x * x + y * y + z * z);
    if (!len)
    {
        this->x = 0;
        this->y = 0;
        this->z = 0;
        w = ONE;
        return *this;
    }
    Vector3 normal;
    NormalizeShortVector(rotation_vector, normal);
    uint32 half_angle = (len >> 1) & 4095;
    sint32 sine = rsin(half_angle);
    sint32 result_x = sine * normal.x;
    sint32 result_y = sine * normal.y;
    sint32 result_z = sine * normal.z;
    w = rcos(half_angle);
    this->x = result_x >> 12;
    this->y = result_y >> 12;
    this->z = result_z >> 12;
    return *this;
}

Quaternion &Quaternion::SetPitch(uint32 angle)
{
    uint32 half_angle = (angle >> 1) & 4095;
    sint32 cosine, sine;
    FUNCTION_MARKER(0x80076274, "SLUS_003.73");
    cosine = rcos(half_angle);
    sine = rsin(half_angle);
    y = 0;
    z = 0;
    x = sine;
    w = cosine;
    return *this;
}

Quaternion &Quaternion::SetYaw(uint32 angle)
{
    uint32 half_angle = (angle >> 1) & 4095;
    sint32 cosine, sine;
    FUNCTION_MARKER(0x800762A8, "SLUS_003.73");
    cosine = rcos(half_angle);
    sine = rsin(half_angle);
    x = 0;
    z = 0;
    y = sine;
    w = cosine;
    return *this;
}

Quaternion &Quaternion::SetRoll(uint32 angle)
{
    uint32 half_angle = (angle >> 1) & 4095;
    sint32 cosine, sine;
    FUNCTION_MARKER(0x800762DC, "SLUS_003.73");
    cosine = rcos(half_angle);
    sine = rsin(half_angle);
    x = 0;
    y = 0;
    z = sine;
    w = cosine;
    return *this;
}

uint32 Quaternion::ToMatrix(MATRIX &output) const
{
    uint32 w, x, y, z;
    sint32 wx, wy, wz, xx, xy, xz, yy, yz, zz, result;
    FUNCTION_MARKER(0x80076310, "SLUS_003.73");
    w = this->w;
    x = this->x;
    y = this->y;
    z = this->z;
    wx = (sint32)(w * x) >> 11;
    wy = (sint32)(w * y) >> 11;
    wz = (sint32)(w * z) >> 11;
    xx = (sint32)(x * x) >> 11;
    xy = (sint32)(x * y) >> 11;
    yy = (sint32)(y * y) >> 11;
    zz = (sint32)(z * z) >> 11;
    output.t[0] = 0;
    output.t[1] = 0;
    output.t[2] = 0;
    xz = (sint32)(x * z) >> 11;
    yz = (sint32)(y * z) >> 11;
    output.m[0][0] = ONE - (yy + zz);
    output.m[0][1] = xy + wz;
    output.m[1][0] = xy - wz;
    output.m[1][1] = ONE - (xx + zz);
    output.m[0][2] = xz - wy;
    output.m[2][0] = xz + wy;
    output.m[2][2] = ONE - (xx + yy);
    result = yz + wx;
    output.m[1][2] = result;
    output.m[2][1] = yz - wx;
    return result;
}

uint32 Quaternion::FromMatrix(const MATRIX &matrix, Quaternion &output)
{
    uint32 factor, sel, next, last;
    sint32 root, result, left, right, diagonal[3], trace, cur;
    FUNCTION_MARKER(0x80076420, "SLUS_003.73");
    diagonal[0] = matrix.m[0][0];
    diagonal[1] = matrix.m[1][1];
    diagonal[2] = matrix.m[2][2];
    trace = diagonal[0] + diagonal[1] + diagonal[2];
    sel = diagonal[0] < diagonal[1];
    if (trace > 0)
    {
        root = SquareRoot0((trace + ONE) << 12);
        factor = DivideSignedWord(0x800000, root);
        output.w = root >> 1;
        left = matrix.m[1][2];
        right = matrix.m[2][1];
        output.x = ScaleFixedPoint(left - right, factor);
        left = matrix.m[2][0];
        right = matrix.m[0][2];
        output.y = ScaleFixedPoint(left - right, factor);
        left = matrix.m[0][1];
        right = matrix.m[1][0];
        result = ScaleFixedPoint(left - right, factor);
        output.z = result;
        return result;
    }
    cur = matrix.m[sel][sel];
    if (cur < diagonal[2])
        sel = 2;
    static const uint8 next_axis[3] = {1, 2, 0};
    next = next_axis[sel];
    last = next_axis[next];
    left = matrix.m[next][next];
    right = matrix.m[last][last];
    cur = matrix.m[sel][sel];
    root = SquareRoot0((cur - (left + right) + ONE) << 12);
    output[sel] = root >> 1;
    factor = DivideSignedWord(0x800000, root);
    left = matrix.m[next][last];
    right = matrix.m[last][next];
    output.w = ScaleFixedPoint(left - right, factor);
    left = matrix.m[sel][next];
    right = matrix.m[next][sel];
    output[next] = ScaleFixedPoint(left + right, factor);
    left = matrix.m[sel][last];
    right = matrix.m[last][sel];
    result = ScaleFixedPoint(left + right, factor);
    output[last] = result;
    return result;
}

uint32 Quaternion::Interpolate(const Quaternion &to, uint32 amount, Quaternion &output)
{
    FUNCTION_MARKER(0x80076800, "SLUS_003.73");
    uint32 component0 = x;
    uint32 right = to.x;
    uint32 sum = component0 * right;
    for (uint32 idx = 1; idx < 4; ++idx)
    {
        uint32 left = (*this)[idx];
        right = to[idx];
        sum += left * right;
    }
    sint32 dot = sum;
    dot >>= 12;
    if (dot < 0)
    {
        dot = -dot;
        uint32 component1 = y;
        x = -component0;
        uint32 component3 = w;
        y = -component1;
        uint32 component2 = z;
        w = -component3;
        z = -component2;
    }
    uint32 weight1 = ONE - amount;
    uint32 weight2 = amount;
    if (ONE - dot >= 129)
    {
        uint32 angle = InverseCosineAngle(dot);
        sint32 product = (ONE - amount) * angle;
        sint32 numerator = rsin(product >> 12) * ONE;
        sint32 denominator = rsin(angle);
        weight1 = DivideSignedWord(numerator, denominator);
        product = amount * angle;
        numerator = rsin(product >> 12) * ONE;
        weight2 = DivideSignedWord(numerator, denominator);
    }
    sint32 result = 0;
    for (uint32 idx = 0; idx < 4; ++idx)
    {
        uint32 left = (*this)[idx];
        right = to[idx];
        sint32 combined = weight1 * left + weight2 * right;
        result = combined >> 12;
        output[idx] = result;
    }
    return result;
}
