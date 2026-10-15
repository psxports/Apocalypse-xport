#ifndef EFFECT_SEGMENTS_H
#define EFFECT_SEGMENTS_H

#include "effect_object.h"

class SegmentEffect : public EffectObject
{
public:
    SegmentEffect(uint32 count);
    virtual ~SegmentEffect() {}
    static SegmentEffect &Attach(uint32 object);
    virtual uint32 Destroy(uint32 reason);
    uint32 Enable();
    uint32 SetEndpoints(const uint32 *origin, const uint32 *target);

};

#endif
