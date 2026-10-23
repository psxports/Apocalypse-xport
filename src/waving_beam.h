#ifndef WAVING_BEAM_H
#define WAVING_BEAM_H

#include "ribbon_segments.h"

class WavingBeam : public RibbonSegments
{
public:
    virtual ~WavingBeam()
    {
    }

private:
    struct Width
    {
        uint8 tick;
        sint32 target;
    } width[10];

    uint32 rest_x[10];
    uint16 phase[10];
    sint32 amplitude[10];
    uint8 scroll_step;
    bool damage_actors;
    uint16 damage;
    uint8 damage_timer;
    ShortVector3 angles;
    sint32 length;
    uint16 lifetime;

public:
    WavingBeam(const Vector3 *pos, const ShortVector3 *angles, sint32 len, uint16 lifetime, uint16 damage, uint32 r, uint32 g, uint32 b, uint8 scroll_step, uint32 material);
    virtual uint32 Destroy(uint32 reason);
    uint32 SetPos(const Vector3 *pos);
    virtual void Update();
};

#endif
