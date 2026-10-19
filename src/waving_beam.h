#ifndef WAVING_BEAM_H
#define WAVING_BEAM_H
extern "C"
{
#include "psx.h"
}
#include "ribbon_segments.h"

class WavingBeam : public RibbonSegments
{
public:
    WavingBeam(const uint32 *position, const uint16 *angles, uint32 length, uint32 height, uint32 lifetime, uint32 red, uint32 green, uint32 blue, uint32 mode, uint32 material);

    virtual ~WavingBeam()
    {
    }

    virtual uint32 Destroy(uint32 reason);
    virtual void Update();
    uint32 SetPosition(const uint32 *position);

private:
    struct Width
    {
        uint32 tick;
        uint32 target;
    } width[10];

    uint32 rest_x[10];
    uint32 phase[10];
    uint32 phase_step[10];
    uint32 amplitude[10];
    uint8 scroll_step;
    uint8 damage_actors;
    uint16 damage;
    uint16 damage_timer;
    ShortVector3 angles;
    uint32 length;
    uint16 lifetime;
};

#endif
