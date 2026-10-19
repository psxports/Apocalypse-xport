#ifndef COLLISION_RAY_H
#define COLLISION_RAY_H

extern "C"
{
#include "psx.h"
}

#include "model_instance.h"

struct CollisionRay
{
    Vector3 endpoints[2];
    Vector3 min;
    Vector3 max;
    uint32 distance;
    uint32 len;
    MATRIX basis;
    ModelReference hit;
    Vector3 hit_pos;
    SVECTOR n;
    uint32 endpoint;
    uint8 triggers;
    uint8 upward;
    uint16 generation;
};

CollisionRay &GetSharedCollisionRay();

#endif
