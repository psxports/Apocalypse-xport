#ifndef TRAVERSAL_RAIL_H
#define TRAVERSAL_RAIL_H

#include "visual_object.h"

// Segment-constrained player traversal with rigid and flexible visuals
class TraversalRail : public VisualObject
{
public:
    TraversalRail(const uint32 *data, uint32 index);
    virtual ~TraversalRail() {}
    static TraversalRail &Attach(uint32 object);
    virtual uint32 Destroy(uint32 reason);
    virtual uint32 Update();
    uint32 Initialize();
};

class FlexibleTraversalRail : public TraversalRail
{
public:
    FlexibleTraversalRail(const uint32 *data, uint32 index);
    virtual ~FlexibleTraversalRail() {}
    static FlexibleTraversalRail &Attach(uint32 object);
    virtual uint32 Destroy(uint32 reason);
    virtual uint32 Update();
};

uint32 AdvanceMovementConstraint(uint32 record, uint32 position, uint32 scale, uint32 guest_output, uint32 *native_output);
uint32 ProjectMovementConstraint(uint32 record, uint32 position, uint32 guest_output, uint32 *native_output);
#endif
