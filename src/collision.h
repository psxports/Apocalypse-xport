#ifndef COLLISION_H
#define COLLISION_H

#include "model_bank_grid.h"
#include "model_geometry.h"
#include "collision_ray.h"

extern "C"
{
#include "psx.h"
}

struct CollisionQueryOptions
{
    // Masks refer to the original polygon surface bits
    bool require_surface10;
    bool allow_surface20;
    bool reject_surface40;
    bool require_surface02;
};

extern CollisionQueryOptions col_query;

#endif
