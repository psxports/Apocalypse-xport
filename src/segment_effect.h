#ifndef SEGMENT_EFFECT_H
#define SEGMENT_EFFECT_H

#include "vector3.h"
#include "effect_object.h"
#include "point_segments.h"

struct HD_LINE_CHAIN;

class SegmentEffect : public EffectObject
{
public:
    sint32 point_count;
    EffectPoint *points;
    Vector3 origin;
    RGBC rgbc;
    uint8 forced_depth;
    mutable HD_LINE_CHAIN *hd_history;

private:
    SegmentEffect &operator=(const SegmentEffect &);

public:
    SegmentEffect(uint32 count);
    virtual uint32 Destroy(uint32 reason);
    void Enable();
    void SetEndpoints(const Vector3 *src, const Vector3 *target);
    virtual ~SegmentEffect();

private:
    SegmentEffect(const SegmentEffect &);
};

#endif
