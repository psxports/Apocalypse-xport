#ifndef COLLISION_H
#define COLLISION_H

#include "model_grid.h"
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

extern CollisionQueryOptions collision_query;
void ResetCollisionState();

uint32 IsSegmentVisible(const uint32 *start, const uint32 *end);
uint32 FindGroundHeight(const uint32 *position, uint32 below, uint32 above);
uint32 QueryCollisionRay(CollisionRay &record, uint32 include_objects);
uint32 InitializeCollisionRay(CollisionRay &record);
ModelInstance *IntersectObjectSpheres(const uint32 *start, const uint32 *end, uint32 *output, ModelInstance *list, ModelInstance *excluded);
ModelInstance *TraceSegmentCollision(const uint32 *start, const uint32 *end, ModelInstance *list);

#endif
