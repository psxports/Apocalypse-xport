#ifndef COLLISION_RAY_INTERNAL_H
#define COLLISION_RAY_INTERNAL_H

#include "collision_ray.h"

struct CollisionBounds
{
    Vector3 minimum, maximum;
};

struct CollisionVertex
{
    XY xy;
    uint16 z;
    uint16 clip;
};

// Query reset, model traversal and polygon intersection share this state
extern CollisionVertex *col_vertices;
extern uint32 col_vertex_capacity;
extern SVECTOR col_normal;
extern MATRIX col_matrix;
extern uint32 col_trigger;
extern uint32 polygon_required_mask;
extern uint32 polygon_rejected_mask;
CollisionVertex *GetColVertices(uint32 count);

#endif
