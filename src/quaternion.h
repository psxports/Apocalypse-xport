#ifndef QUATERNION_H
#define QUATERNION_H

#include "vector3.h"

struct Quaternion
{
    sint32 x;
    sint32 y;
    sint32 z;
    sint32 w;

    // Component indices are limited to 0..3
    sint32 &operator[](uint32 idx)
    {
        return idx == 0 ? x : idx == 1 ? y : idx == 2 ? z : w;
    }

    sint32 operator[](uint32 idx) const
    {
        return idx == 0 ? x : idx == 1 ? y : idx == 2 ? z : w;
    }

public:
    Quaternion &Multiply(const Quaternion &left, const Quaternion &right);
    Quaternion &SetAxisAngle(const Vector3 &rotation_vector);
    Quaternion &SetPitch(uint32 angle);
    Quaternion &SetYaw(uint32 angle);
    Quaternion &SetRoll(uint32 angle);
    uint32 ToMatrix(MATRIX &output) const;
    static uint32 FromMatrix(const MATRIX &matrix, Quaternion &output);
    uint32 Interpolate(const Quaternion &to, uint32 amount, Quaternion &output);
};

#endif
