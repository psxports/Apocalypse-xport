#ifndef POINT_SEGMENTS_H
#define POINT_SEGMENTS_H

#include "effect_object.h"

class PointSegments : public EffectObject
{
public:
    PointSegments(uint32 count);
    virtual ~PointSegments() {}
    static PointSegments &Attach(uint32 object);
    virtual uint32 Destroy(uint32 reason);
    uint32 EnableSegments();

};

#endif
