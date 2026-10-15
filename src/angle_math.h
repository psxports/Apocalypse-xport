#ifndef ANGLE_MATH_H
#define ANGLE_MATH_H

extern "C"
{
#include "psx.h"
}

void LoadFixedGteTranslation(const uint32 *position);
void BuildModelRotationMatrix(const uint32 *input, uint16 *output);
void ScaleModelRotationMatrix(const uint8 *input, uint16 *output);
void TransposeRotationMatrix(uint16 *matrix);
void TransposeRotationMatrix(const uint16 *input, uint16 *output);

uint32 InverseCosineAngle(uint32 value);
sint32 CalculateAngleFromSlope(sint32 slope);

uint32 CopyAngleVector(uint16 *output, const uint16 *source);

uint16 *SubtractAngleVector(uint16 *output, const uint16 *left, const uint16 *right);
uint16 *SubtractAngleVector(uint16 *angles, const uint16 *source);
uint16 *DivideAngleVector(uint16 *angles, const uint32 *divisor);
uint32 *OffsetPositionByAngles(uint32 *position, const uint16 *angles);
uint32 BuildDirectionVector(uint32 *output, uint32 scale, const uint16 *angles);
uint16 *DampAngles(uint16 *angles, const uint8 *shifts);
uint32 NormalizeAngles(uint16 *angles);
uint16 *AddAngleVector(uint16 *angles, const uint16 *source);
uint32 SnapSmallAngles(uint16 *angles);

uint32 UpdateAngleAcceleration(SVECTOR current, uint16 *velocity, uint16 *acceleration, SVECTOR target, uint32 scale);

uint32 CalculateDirectionAngles(uint16 *angles, const uint32 *origin, const uint32 *target);
uint32 RotateVectorYaw(uint32 *output, const uint32 *input, uint32 angle);

#endif
