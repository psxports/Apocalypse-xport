#ifndef OBJECT_COLLISION_EVENTS_H
#define OBJECT_COLLISION_EVENTS_H

extern "C"
{
#include "psx.h"
#include "draft_first_contexts.h"
}

uint32 ChooseCandidatePosition(uint32 object, uint32 *output, const uint32 *target, uint32 minimum, uint32 maximum, uint32 height_limit, uint32 spread, uint32 ground, uint32 check_collision);
uint32 IsSegmentVisible(const uint32 *start, const uint32 *end);
uint32 FindGroundHeight(const uint32 *position, uint32 below, uint32 above);
void LoadCollisionBounds(const sint32 bounds[6], sint32 low[3], sint32 high[3], uint32 *flip_bits);
void NormalizeCollisionBounds(sint32 low[3], sint32 high[3], uint32 *flip_bits);
void MarkLinkedCollisionCandidates(uint32 list, const sint32 bounds[6], uint32 mark);
void MarkArrayCollisionCandidates(uint32 list, const sint32 bounds[6], uint32 mark);
void PrepareCollisionBounds(uint32 geometry, uint32 mirror_flags, const xport_draft_bounds *reference, const uint32 translation[3], xport_draft_bounds *result);
uint32 IntersectCollisionBounds(const sint32 query_low[3], const sint32 query_high[3], const sint32 object_low[3], const sint32 object_high[3]);
uint32 IntersectCollisionObject(uint32 object, uint8 *record);
void VisitCollisionObjects(uint32 list, uint8 *record);
void VisitCollisionCells(uint32 list, uint8 *record);
uint32 IntersectCollisionPolygons(uint32 geometry, uint32 output, uint8 *record, uint32 object);
uint32 TransformCollisionVertices(uint32 geometry, uint32 output, uint32 depth_limit, const sint32 translation[3]);
uint32 AdvanceCollisionGeneration();
uint32 FinishCollisionRay(uint8 *record);
uint32 QueryCollisionRay(uint8 *record, uint32 include_objects);
uint32 InitializeCollisionRay(uint8 *record);
uint32 IntersectObjectSpheres(const uint32 *start, const uint32 *end, uint32 *output, uint32 list, uint32 excluded);
uint32 TraceSegmentCollision(const uint32 *start, const uint32 *end, uint32 list);

#endif
