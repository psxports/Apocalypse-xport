#ifndef WORLD_GRAVITY_H
#define WORLD_GRAVITY_H

#include "vector3.h"

extern "C"
{
#include "psx.h"
}

struct WorldGravity
{
    Vector3 vector;
    Vector3 opposed;
    uint32 magnitude;
    uint32 tenth;
};

extern WorldGravity world_gravity;
void ResetGravityVector();
uint32 SetGravityVector(const uint32 *vector);

#endif
