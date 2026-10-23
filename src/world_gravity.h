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

public:
    static uint32 SetVector(const Vector3 *vector);
    static void Reset();
};

extern WorldGravity world_gravity;

#endif
