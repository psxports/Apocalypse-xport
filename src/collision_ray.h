#ifndef COLLISION_RAY_H
#define COLLISION_RAY_H

extern "C"
{
#include "psx.h"
}

#include "model_instance.h"

struct CollisionRay
{
    uint32 pos[2][3];
    uint32 min[3];
    uint32 max[3];
    uint32 distance;
    uint32 len;
    MATRIX basis;
    ModelReference hit;
    uint32 hit_pos[3];
    SVECTOR n;
    uint32 endpoint;
    uint8 triggers;
    uint8 upward;
    uint16 generation;
};

CollisionRay &GetSharedCollisionRay();

#endif
