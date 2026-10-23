#ifndef COLLISION_RAY_H
#define COLLISION_RAY_H

extern "C"
{
#include "psx.h"
}

#include "model_instance.h"

struct ModelGridCell;
struct ModelGeometry;
struct CollisionBounds;

struct CollisionRay
{
public:
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
    bool triggers;
    bool upward;
    uint16 generation;

public:
    static ModelInstance *TraceSeg(const Vector3 &start, const Vector3 &end, ModelInstance *list);
    static bool IsSegVisible(const Vector3 &start, const Vector3 &end);
    static sint32 FindGroundHeight(const Vector3 &pos, uint32 below, uint32 above);

private:
    static void AdvanceGeneration();

public:
    uint16 Initialize();

private:
    static void VisitObjects(ModelInstance *list, CollisionRay &rec);
    static void VisitCell(const ModelGridCell &cell, CollisionRay &rec);
    bool ResolveHitNormal();

public:
    static ModelInstance *IntersectSpheres(const Vector3 &start, const Vector3 &end, Vector3 *output, ModelInstance *list, ModelInstance *excluded);
    bool Query(uint32 include_objects);

private:
    static void LoadBounds(const Vector3 bounds[2], Vector3 &low, Vector3 &high, uint32 &flip_bits);
    static void NormalizeBounds(Vector3 &low, Vector3 &high, uint32 &flip_bits);
    static void PrepareBounds(const ModelGeometry &geometry, uint32 mirror_flags, const CollisionBounds &reference, const Vector3 &translation, CollisionBounds &result);
    static bool IntersectBounds(const Vector3 &query_low, const Vector3 &query_high, const Vector3 &object_low, const Vector3 &object_high);
    static void MarkLinkedCandidates(ModelInstance *object, const Vector3 bounds[2], uint16 mark);
    static void MarkArrayCandidates(ModelInstance *const *objects, uint32 count, const Vector3 bounds[2], uint16 mark);

public:
    static void ResetSharedState();
    static CollisionRay &GetShared();
};

#endif
