#ifndef RIBBON_SEGMENTS_H
#define RIBBON_SEGMENTS_H

#include "vector3.h"

#include "effect_object.h"
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
public:
    RibbonSegments(uint32 segments, uint32 mode);
    virtual ~RibbonSegments();
    virtual uint32 Destroy(uint32 reason);
    virtual uint32 Render();
    void SetMaterial(uint32 name);
    uint32 SetColors(uint32 red, uint32 green, uint32 blue, uint32 tail_red, uint32 tail_green, uint32 tail_blue);
    uint32 SetSize(uint32 width, uint32 height);

    uint8 mode;
    uint8 hidden;
    uint32 u_scroll;
    uint32 v_scroll;
    uint32 v_step;
    uint32 waving;
    uint32 point_count;
    RibbonPoint *points;

protected:
    TextureResource *tex;

private:
    RibbonSegments(const RibbonSegments &);
    RibbonSegments &operator=(const RibbonSegments &);
};

#endif
