#ifndef BEAM_SEGMENTS_H
#define BEAM_SEGMENTS_H

#include "vector3.h"

#include "effect_object.h"

struct BeamPoint
{
    Vector3 pos;
    RGBC rgbc;
    uint16 size;
    uint32 history[2];
};

class BeamSegments : public RenderableEffect
{
public:
    BeamSegments(uint32 count, uint32 mode);
    virtual ~BeamSegments();
    virtual uint32 Destroy(uint32 reason);
    virtual uint32 Render();
    uint32 SetColors(uint32 red, uint32 green, uint32 blue);
    uint32 SetSize(uint32 size);
    uint32 mode;
    uint32 point_count;
    BeamPoint *points;

private:
    struct Projection;
    Projection *projected;
    static void CalculateNormal(const Projection *p, sint32 *x, sint32 *y, uint32 min_len);
    BeamSegments(const BeamSegments &);
    BeamSegments &operator=(const BeamSegments &);
};

#endif
