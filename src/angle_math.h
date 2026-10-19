#ifndef ANGLE_MATH_H
#define ANGLE_MATH_H

#include "vector3.h"

extern "C"
{
#include "psx.h"
}

void LoadFixedGteTranslation(const uint32 *position);

sint16 ShortestCameraAngleDelta(sint16 source_angle, sint16 target_angle);
uint32 InverseCosineAngle(uint32 value);
sint32 CalculateAngleFromSlope(sint32 slope);

uint32 CopyAngleVector(uint16 *output, const uint16 *source);

uint16 *SubtractAngleVector(uint16 *output, const uint16 *left, const uint16 *right);
uint16 *SubtractAngleVector(uint16 *angles, const uint16 *source);
uint16 *DivideAngleVector(uint16 *angles, const uint32 &divisor);
uint32 *OffsetPositionByAngles(uint32 *position, const uint16 *angles);
uint32 BuildDirectionVector(uint32 *output, uint32 scale, const uint16 *angles);
uint16 *DampAngles(uint16 *angles, const uint8 *shifts);
uint32 NormalizeAngles(uint16 *angles);
uint16 *AddAngleVector(uint16 *angles, const uint16 *source);
uint32 SnapSmallAngles(uint16 *angles);

uint32 UpdateAngleAcceleration(ShortVector3 cur, ShortVector3 &velocity, ShortVector3 &acceleration, ShortVector3 target, uint32 scale);

uint32 CalculateDirectionAngles(uint16 *angles, const uint32 *origin, const uint32 *target);
uint32 RotateVectorYaw(uint32 *output, const uint32 *input, uint32 angle);

struct AngleMotion
{
    ShortVector3 angles;
    ShortVector3 velocity;
    ShortVector3 acceleration;

    AngleMotion()
    {
        for (uint32 axis = 0; axis < 3; ++axis)
        {
            angles.words[axis] = 0;
            velocity.words[axis] = 0;
            acceleration.words[axis] = 0;
        }
    }
};

#endif
