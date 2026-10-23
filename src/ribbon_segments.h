#ifndef RIBBON_SEGMENTS_H
#define RIBBON_SEGMENTS_H

#include "vector3.h"
#include "effect_object.h"
#include "renderable_effect.h"
#include "texture_resources.h"

struct RibbonPoint
{
    Vector3 pos;
    RGBC center;
    uint16 width;
    uint16 height;
    RGBC edge;
};

class RibbonSegments : public RenderableEffect
{
private:
    uint8 mode;

protected:
    bool hidden;
    sint32 u_scroll;

public:
    uint32 v_scroll;
    uint32 v_step;
    bool waving;
    sint32 point_count;
    RibbonPoint *points;

protected:
    const TextureResource *tex;

private:
    RibbonSegments &operator=(const RibbonSegments &);

private:
    static sint32 CalculateNormal(XY xy0, XY xy1, sint32 *x_ret, sint32 *y_ret, sint32 min_len);

public:
    RibbonSegments(uint32 segs, uint8 mode);
    virtual uint32 Destroy(uint32 reason);
    void SetMaterial(uint32 name);
    uint32 SetColors(uint8 r, uint8 g, uint8 b, uint8 tail_r, uint8 tail_g, uint8 tail_b);
    uint32 SetSize(uint16 width, uint16 height);
    virtual uint32 Render();

private:
    RibbonSegments(const RibbonSegments &);

public:
    virtual ~RibbonSegments();
};

#endif
