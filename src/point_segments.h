#ifndef POINT_SEGMENTS_H
#define POINT_SEGMENTS_H

#include "vector3.h"
#include "effect_object.h"

struct HD_LINE_CHAIN;

struct EffectPoint
{
    Vector3 pos;
    RGBC rgbc;
};

class PointSegments : public EffectObject
{
public:
    uint8 forced_depth;
    sint32 point_count;
    EffectPoint *points;
    Vector3 origin;
    mutable HD_LINE_CHAIN *hd_history;

private:
    PointSegments &operator=(const PointSegments &);

public:
    PointSegments(uint32 count);
    virtual uint32 Destroy(uint32 reason);
    uint32 EnableSegs();
    virtual ~PointSegments();

private:
    PointSegments(const PointSegments &);
};

#endif
