#ifndef TRAVERSAL_RAIL_H
#define TRAVERSAL_RAIL_H

#include "object_reference.h"
#include "vector3.h"
#include "visual_object.h"
#include "segment_effect.h"

class TraversalRail : public VisualObject
{
public:
    uint16 heading;

protected:
    Vector3 target, dir;

    struct ContactBounds
    {
        sint32 min_x, max_x, min_z, max_z;
    } bounds;

    sint32 length;
    ObjectReference<SegmentEffect> seg;

public:
    TraversalRail(RecordCursor data, uint32 idx);
    uint32 Initialize();
    virtual uint32 Destroy(uint32 reason);
    bool Advance(const Vector3 &pos, uint32 scale, Vector3 &output) const;
    bool Project(const Vector3 &pos, Vector3 &output) const;
    virtual uint32 Update();

private:
    static uint8 GetBrightness(sint32 distance);

public:
    virtual ~TraversalRail();

protected:
    void UpdateSegColor();
    void UpdatePlayerContact();

private:
    sint32 ForwardDot(const Vector3 &delta) const;
};

#endif
