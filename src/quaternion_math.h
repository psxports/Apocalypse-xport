#ifndef QUATERNION_MATH_H
#define QUATERNION_MATH_H

#include "vector3.h"

struct Quaternion
{
    sint32 x;
    sint32 y;
    sint32 z;
    sint32 w;

    sint32 &operator[](uint32 index)
    {
        return index == 0 ? x : index == 1 ? y : index == 2 ? z : w;
    }

    sint32 operator[](uint32 index) const
    {
        return index == 0 ? x : index == 1 ? y : index == 2 ? z : w;
    }
};

uint32 InterpolateQuaternion(Quaternion &from, const Quaternion &to, uint32 amount, Quaternion &output);
uint32 ConvertMatrixToQuaternion(const MATRIX &matrix, Quaternion &output);
Quaternion &BuildRollQuaternion(Quaternion &output, uint32 angle);
uint32 ConvertQuaternionToMatrix(const Quaternion &quaternion, MATRIX &output);
Quaternion &BuildPitchQuaternion(Quaternion &output, uint32 angle);
Quaternion &BuildYawQuaternion(Quaternion &output, uint32 angle);
Quaternion &MultiplyQuaternions(Quaternion &output, const Quaternion &first, const Quaternion &second);
Quaternion &BuildAxisAngleQuaternion(Quaternion &output, const Vector3 &rotation_vector);

#endif
