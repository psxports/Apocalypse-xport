#ifndef POINT_SEGMENTS_H
#define POINT_SEGMENTS_H

#include "vector3.h"

#include "effect_object.h"

struct EffectPoint
{
    Vector3 pos;
    RGBC rgbc;
};

class PointSegments : public EffectObject
{
public:
    PointSegments(uint32 count);
    virtual ~PointSegments();
    virtual uint32 Destroy(uint32 reason);
    uint32 EnableSegments();
    uint8 forced_depth;
    uint32 point_count;
    EffectPoint *points;
    Vector3 origin;

private:
    PointSegments(const PointSegments &);
    PointSegments &operator=(const PointSegments &);
};

#endif
