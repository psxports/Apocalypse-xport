extern "C" {
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
}

#include "object_collision_events.h"

static uint32 TraceProjectileSegment(const uint32 start[3], uint32 end, uint32 list);









static uint32 TraceProjectileSegment(const uint32 start[3], uint32 end, uint32 list)
{
    return TraceSegmentCollision(start, (const uint32 *)psx_addr(end, 12), list);
}









extern "C" uint32 apocalypse_projectile_segment_native(const uint32 start[3], uint32 end, uint32 list)
{
    return TraceProjectileSegment(start, end, list);
}
