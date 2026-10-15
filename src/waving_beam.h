#ifndef WAVING_BEAM_H
#define WAVING_BEAM_H
extern "C" {
#include "psx.h"
}
#include "ribbon_segments.h"

class WavingBeam : public RibbonSegments
{
public:
    WavingBeam(const uint32 *position, const uint16 *angles, uint32 length, uint32 height, uint32 lifetime, uint32 red, uint32 green, uint32 blue, uint32 mode, uint32 material);
    virtual ~WavingBeam() {}
    static WavingBeam &Attach(uint32 object);
    virtual uint32 Destroy(uint32 reason);
    virtual uint32 Update();
    uint32 SetPosition(const uint32 *position);

private:
    static uint32 ReadMasked(uint32 address, uint32 mask);
};

#endif
