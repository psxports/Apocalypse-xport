#ifndef BEAM_SEGMENTS_H
#define BEAM_SEGMENTS_H

#include "vector3.h"
#include "effect_object.h"
#include "renderable_effect.h"

struct BeamPoint
{
    Vector3 pos;
    RGBC rgbc;
    uint16 size;
    XY history[2];
};

struct HD_BEAM_POINT;
struct HD_BEAM_HISTORY;

class BeamSegments : public RenderableEffect
{
public:
    sint32 point_count;
    BeamPoint *points;

private:
    uint32 mode;
    struct Projection;
    Projection *proj;
    HD_BEAM_POINT *hd_points;
    bool hd_history_valid;
    HD_BEAM_HISTORY *hd_history;
    BeamSegments &operator=(const BeamSegments &);

public:
    BeamSegments(uint32 count, uint32 mode);
    virtual uint32 Destroy(uint32 reason);
    uint32 SetColors(uint8 r, uint8 g, uint8 b);
    uint32 SetSize(uint16 size);

private:
    static void CalculateNormal(const Projection *p, sint32 *x, sint32 *y, sint32 min_len);
    bool CaptureHd(bool logic);

public:
    virtual uint32 Render();
    virtual bool CaptureHdTick();
    virtual ~BeamSegments();

private:
    BeamSegments(const BeamSegments &);
};

#endif
