#ifndef EFFECT_SEGMENTS_H
#define EFFECT_SEGMENTS_H

#include "vector3.h"

#include "effect_object.h"
#include "point_segments.h"

class SegmentEffect : public EffectObject
{
public:
    SegmentEffect(uint32 count);
    virtual ~SegmentEffect();
    virtual uint32 Destroy(uint32 reason);
    void Enable();
    void SetEndpoints(const uint32 *origin, const uint32 *target);

    uint32 point_count;
    EffectPoint *points;
    Vector3 origin;
    RGBC rgbc;
    uint8 forced_depth;

private:
    SegmentEffect(const SegmentEffect &);
    SegmentEffect &operator=(const SegmentEffect &);
};

#endif
