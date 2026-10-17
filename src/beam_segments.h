#ifndef BEAM_SEGMENTS_H
#define BEAM_SEGMENTS_H

#include "effect_object.h"

class BeamSegments : public RenderableEffect
{
public:
    BeamSegments(uint32 count, uint32 mode);
    virtual ~BeamSegments() {}
    static BeamSegments &Attach(uint32 object);
    virtual uint32 Destroy(uint32 reason);
    virtual uint32 Render();
    uint32 SetColors(uint32 red, uint32 green, uint32 blue);
    uint32 SetSize(uint32 size);

};

#endif
