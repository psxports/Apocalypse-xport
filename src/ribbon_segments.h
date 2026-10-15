#ifndef RIBBON_SEGMENTS_H
#define RIBBON_SEGMENTS_H

#include "effect_object.h"

uint32 CalculateRibbonNormal(uint32 first, uint32 second, sint32 *output_x, sint32 *output_y, uint32 minimum);

class RibbonSegments : public RenderableEffect
{
public:
    RibbonSegments(uint32 segments, uint32 mode);
    virtual ~RibbonSegments() {}
    static RibbonSegments &Attach(uint32 object);
    virtual uint32 Destroy(uint32 reason);
    virtual uint32 Render();
    uint32 SetMaterial(uint32 name);
    uint32 SetColors(uint32 red, uint32 green, uint32 blue, uint32 tail_red, uint32 tail_green, uint32 tail_blue);
    uint32 SetSize(uint32 width, uint32 height);

};

#endif
