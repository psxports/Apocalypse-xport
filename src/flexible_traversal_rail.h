#ifndef FLEXIBLE_TRAVERSAL_RAIL_H
#define FLEXIBLE_TRAVERSAL_RAIL_H

#include "vector3.h"
#include "visual_object.h"
#include "segment_effect.h"
#include "traversal_rail.h"

class FlexibleTraversalRail : public TraversalRail
{
private:
    ShortVector3 *waves;
    uint32 wave_count;
    uint8 deform_timer;
    uint8 wave_bank;

public:
    FlexibleTraversalRail(RecordCursor data, uint32 idx);
    virtual uint32 Destroy(uint32 reason);
    void ApplyRandomOffset(uint32 range);
    virtual uint32 Update();
    virtual ~FlexibleTraversalRail();
};

#endif
