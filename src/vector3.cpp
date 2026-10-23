#include "vector3.h"
#include "angle_math.h"
#include "global.h"

Vector3 &Vector3::CombineVectors(Vector3 &output, const Vector3 &left, const Vector3 &right, bool subtract)
{
    Vector3 result;
    for (uint32 axis = 0; axis < 3; ++axis)
        result.comp[axis] = subtract ? (uint32)left.comp[axis] - right.comp[axis] : (uint32)left.comp[axis] + right.comp[axis];
    output = result;
    return output;
}

Vector3 &Vector3::ApplyVectorScalar(Vector3 &output, const Vector3 &vector, const uint32 &scalar, bool shift)
{
    Vector3 result;
    for (uint32 axis = 0; axis < 3; ++axis)
    {
        uint32 factor = scalar;
        if (shift)
            result.comp[axis] = vector.comp[axis] >> (factor & 31);
        else
            result.comp[axis] = (uint32)vector.comp[axis] * factor;
    }
    output = result;
    return output;
}

Vector3 &Vector3::ShiftVectorInPlace(Vector3 &vector, const uint32 &shift, bool right)
{
    for (uint32 axis = 0; axis < 3; ++axis)
    {
        uint32 amount = shift & 31;
        if (right)
            vector.comp[axis] >>= amount;
        else
            vector.comp[axis] = (uint32)vector.comp[axis] << amount;
    }
    return vector;
}

Vector3 &Vector3::AccumulateVector(Vector3 &vector, const Vector3 &src, bool subtract)
{
    for (uint32 axis = 0; axis < 3; ++axis)
        if (subtract)
            vector.comp[axis] = (uint32)vector.comp[axis] - src.comp[axis];
        else
            vector.comp[axis] = (uint32)vector.comp[axis] + src.comp[axis];
    return vector;
}

uint32 Vector3::SqShiftedVector(const Vector3 &vector)
{
    uint32 squares[3], axis;
    Vector3 shifted;
    for (axis = 0; axis < 3; axis += 1)
        shifted.comp[axis] = vector.comp[axis] >> 12;
    for (axis = 0; axis < 3; axis += 1)
        xport_gte_write_data(9 + axis, (uint32)shifted.comp[axis]);
    xport_gte_execute(0xA00428);
    for (axis = 0; axis < 3; axis += 1)
        squares[axis] = xport_gte_read_data(25 + axis);
    return squares[0] + squares[1] + squares[2];
}

uint32 Vector3::RotateYaw(const Vector3 &input, uint32 angle)
{
    FUNCTION_MARKER(0x800665CC, "SLUS_003.73");
    uint32 left = input.x >> 3;
    left *= rcos(angle);
    uint32 right = input.z >> 3;
    right *= rsin(angle);
    sint32 combined = left + right;
    x = combined >> 9;
    y = input.y;
    left = input.z >> 3;
    left *= rcos(angle);
    right = input.x >> 3;
    right *= rsin(angle);
    combined = left - right;
    z = combined >> 9;
    return z;
}

uint32 Vector3::Normalize(const Vector3 &input)
{
    FUNCTION_MARKER(0x800666DC, "SLUS_003.73");
    sint32 divisor = LookupDistanceRoot(Vector3::SqShiftedVector(input));
    if (!divisor)
    {
        x = 0;
        y = 0;
        z = 0;
        return 0;
    }
    sint32 result = 0;
    for (uint32 axis = 0; axis < 3; ++axis)
    {
        result = DivideSignedWord(input.comp[axis], divisor);
        comp[axis] = result;
    }
    return result;
}

uint32 Vector3::SetDir(uint32 scale, const ShortVector3 &angles)
{
    FUNCTION_MARKER(0x800667CC, "SLUS_003.73");
    sint32 product = scale * rcos((uint16)angles.x & 4095);
    uint32 horizontal = product >> 12;
    x = -horizontal * rsin((uint16)angles.y & 4095);
    y = scale * rsin((uint16)angles.x & 4095);
    product = scale * rcos((uint16)angles.x & 4095);
    horizontal = product >> 12;
    z = -horizontal * rcos((uint16)angles.y & 4095);
    return z;
}

uint32 Vector3::EuclideanDistance(const Vector3 &target) const
{
    FUNCTION_MARKER(0x8006689C, "SLUS_003.73");
    sint32 dx = (uint32)x - target.x;
    sint32 dy = (uint32)y - target.y;
    sint32 dz = (uint32)z - target.z;
    uint32 x = dx >> 12;
    uint32 y = dy >> 12;
    uint32 z = dz >> 12;
    return LookupDistanceRoot(x * x + y * y + z * z);
}

uint32 Vector3::HorizontalDistance(const Vector3 &target) const
{
    FUNCTION_MARKER(0x80066918, "SLUS_003.73");
    sint32 dx = (uint32)x - target.x;
    sint32 dz = (uint32)z - target.z;
    uint32 x = dx >> 12;
    uint32 z = dz >> 12;
    return LookupDistanceRoot(x * x + z * z);
}

uint32 Vector3::Distance(const Vector3 &target) const
{
    FUNCTION_MARKER(0x8006696C, "SLUS_003.73");
    uint32 val;
    Vector3 delta;
    delta.x = (uint32)x - target.x;
    delta.y = (uint32)y - target.y;
    delta.z = (uint32)z - target.z;
    if (delta.x < 0)
        delta.x = -(uint32)delta.x;
    if (delta.y < 0)
        delta.y = -(uint32)delta.y;
    if (delta.z < 0)
        delta.z = -(uint32)delta.z;
    sint32 x = delta.x;
    sint32 y = delta.y;
    sint32 z = delta.z;
    if (x < y)
    {
        if (y < z)
            val = (uint32)(x >> 2) + (y >> 1) + z;
        else if (z < x)
            val = (uint32)(z >> 2) + (x >> 1) + y;
        else
            val = (uint32)(x >> 2) + (z >> 1) + y;
    }
    else
    {
        if (x < z)
            val = (uint32)(y >> 2) + (x >> 1) + z;
        else if (z < y)
            val = (uint32)(z >> 2) + (y >> 1) + x;
        else
            val = (uint32)(y >> 2) + (z >> 1) + x;
    }
    return (sint32)val >> 12;
}

uint32 Vector3::Len() const
{
    FUNCTION_MARKER(0x8006BF04, "SLUS_003.73");
    return LookupDistanceRoot(Vector3::SqShiftedVector(*this));
}

uint32 Vector3::LenSq() const
{
    FUNCTION_MARKER(0x8006BF8C, "SLUS_003.73");
    return Vector3::SqShiftedVector(*this);
}

bool Vector3::SnapSmall()
{
    FUNCTION_MARKER(0x8006C05C, "SLUS_003.73");
    bool small = false;
    for (uint32 axis = 0; axis < 3; ++axis)
    {
        sint32 val = comp[axis];
        small = val >= -2048 && val <= 2048;
        if (small)
            comp[axis] = 0;
    }
    // Original return describes the final component only
    return small;
}

Vector3 &Vector3::Add(const Vector3 &src)
{
    FUNCTION_MARKER(0x8006C0B8, "SLUS_003.73");
    return Vector3::AccumulateVector(*this, src, false);
}

Vector3 &Vector3::Subtract(const Vector3 &src)
{
    FUNCTION_MARKER(0x8006C0FC, "SLUS_003.73");
    return Vector3::AccumulateVector(*this, src, true);
}

Vector3 &Vector3::Divide(const uint32 &scalar)
{
    FUNCTION_MARKER(0x8006C190, "SLUS_003.73");
    for (uint32 axis = 0; axis < 3; ++axis)
        comp[axis] = DivideSignedWord(comp[axis], scalar);
    return *this;
}

Vector3 &Vector3::ShiftRight(const uint32 &shift)
{
    FUNCTION_MARKER(0x8006C1E8, "SLUS_003.73");
    return Vector3::ShiftVectorInPlace(*this, shift, true);
}

Vector3 &Vector3::ShiftLeft(const uint32 &shift)
{
    FUNCTION_MARKER(0x8006C22C, "SLUS_003.73");
    return Vector3::ShiftVectorInPlace(*this, shift, false);
}

Vector3 &Vector3::Damp(const uint8 *shifts)
{
    FUNCTION_MARKER(0x8006C270, "SLUS_003.73");
    for (uint32 axis = 0; axis < 3; ++axis)
    {
        uint32 amount = shifts[axis] & 31;
        comp[axis] = (uint32)comp[axis] - (comp[axis] >> amount);
    }
    return *this;
}

bool Vector3::Differs(const Vector3 &right) const
{
    FUNCTION_MARKER(0x8006C304, "SLUS_003.73");
    return x != right.x || y != right.y || z != right.z;
}

Vector3 &Vector3::Add(const Vector3 &left, const Vector3 &right)
{
    FUNCTION_MARKER(0x8006C34C, "SLUS_003.73");
    return Vector3::CombineVectors(*this, left, right, false);
}

Vector3 &Vector3::Subtract(const Vector3 &left, const Vector3 &right)
{
    FUNCTION_MARKER(0x8006C3AC, "SLUS_003.73");
    return Vector3::CombineVectors(*this, left, right, true);
}

Vector3 &Vector3::Multiply(const Vector3 &vector, const uint32 &scalar)
{
    FUNCTION_MARKER(0x8006C40C, "SLUS_003.73");
    return Vector3::ApplyVectorScalar(*this, vector, scalar, false);
}

Vector3 &Vector3::Scale(const uint32 &scalar, const Vector3 &vector)
{
    FUNCTION_MARKER(0x8006C47C, "SLUS_003.73");
    return Vector3::ApplyVectorScalar(*this, vector, scalar, false);
}

Vector3 &Vector3::Divide(const Vector3 &vector, const uint32 &scalar)
{
    FUNCTION_MARKER(0x8006C4EC, "SLUS_003.73");
    Vector3 result;
    for (uint32 axis = 0; axis < 3; ++axis)
        result.comp[axis] = DivideSignedWord(vector.comp[axis], scalar);
    *this = result;
    return *this;
}

Vector3 &Vector3::ShiftRight(const Vector3 &vector, const uint32 &shift)
{
    FUNCTION_MARKER(0x8006C564, "SLUS_003.73");
    return Vector3::ApplyVectorScalar(*this, vector, shift, true);
}

Vector3 &Vector3::ShiftLeft(const Vector3 &vector, const uint32 &shift)
{
    FUNCTION_MARKER(0x8006C5C4, "SLUS_003.73");
    Vector3 result;
    for (uint32 axis = 0; axis < 3; ++axis)
        result.comp[axis] = (uint32)vector.comp[axis] << (shift & 31);
    *this = result;
    return *this;
}

void Vector3::LoadGteTranslation() const
{
    FUNCTION_MARKER(0x80084504, "SLUS_003.73");
    // Snapshot all Q12 components before GTE control writes
    const Vector3 pos = *this;
    xport_gte_write_control(5, pos.x >> 12);
    xport_gte_write_control(6, pos.y >> 12);
    xport_gte_write_control(7, pos.z >> 12);
}
