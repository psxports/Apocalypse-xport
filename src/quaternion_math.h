#ifndef QUATERNION_MATH_H
#define QUATERNION_MATH_H

extern "C"
{
#include "psx.h"
}

uint32 InterpolateQuaternion(uint32 *from, const uint32 *to, uint32 amount, uint32 *output);

uint32 ConvertMatrixToQuaternion(const sint16 *matrix, uint32 *output);

uint32 *BuildRollQuaternion(uint32 *output, uint32 angle);
uint32 ConvertQuaternionToMatrix(const uint32 *quaternion, MATRIX *output);

uint32 *BuildPitchQuaternion(uint32 *output, uint32 angle);
uint32 *BuildYawQuaternion(uint32 *output, uint32 angle);
uint32 *MultiplyQuaternions(uint32 *output, const uint32 *first, const uint32 *second);

uint32 *BuildAxisAngleQuaternion(uint32 *output, const uint32 *rotation_vector);

#endif
