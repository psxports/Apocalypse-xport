#ifndef ANGLE_MATH_H
#define ANGLE_MATH_H

#include "vector3.h"

struct AngleMotion
{
    ShortVector3 angles;
    ShortVector3 velocity;
    ShortVector3 acceleration;

    AngleMotion() : angles(), velocity(), acceleration()
    {
    }
};

#endif
