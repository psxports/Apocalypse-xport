#ifndef TRAVERSAL_RAIL_H
#define TRAVERSAL_RAIL_H

#include "vector3.h"

#include "visual_object.h"
#include "effect_segments.h"

class TraversalRail : public VisualObject
{
public:
    TraversalRail(const uint32 *data, uint32 index);
    virtual ~TraversalRail();
    virtual uint32 Destroy(uint32 reason);
    virtual uint32 Update();
    uint32 Initialize();
    bool Advance(const Vector3 &pos, uint32 scale, Vector3 &output) const;
    bool Project(const Vector3 &pos, Vector3 &output) const;
    uint16 heading;

protected:
    Vector3 target, dir;
    uint32 bounds[4];
    uint32 length;
    ObjectReference<SegmentEffect> segment;
    void UpdateSegmentColor();
    void UpdatePlayerContact();

private:
    uint32 ForwardDot(const Vector3 &delta) const;
};

class FlexibleTraversalRail : public TraversalRail
{
public:
    FlexibleTraversalRail(const uint32 *data, uint32 index);
    virtual ~FlexibleTraversalRail();
    virtual uint32 Destroy(uint32 reason);
    virtual uint32 Update();
    void ApplyRandomOffset(uint32 range);

private:
    ShortVector3 *waves;
    uint32 wave_count;
    uint32 deform_timer;
    uint32 wave_bank;
};

#endif
