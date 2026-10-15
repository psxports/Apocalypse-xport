#ifndef BEAM_ENDPOINT_H
#define BEAM_ENDPOINT_H

#include "beam_segments.h"

class EndpointBeam : public BeamSegments
{
public:
    EndpointBeam(uint32 red, uint32 green, uint32 blue, uint32 count, uint32 duration, uint32 first_radius, uint32 second_radius, uint32 parameter1, uint32 parameter2, uint32 parameter3, uint32 parameter4);
    virtual ~EndpointBeam() {}
    static EndpointBeam &Attach(uint32 object);
    virtual uint32 Destroy(uint32 reason);
    virtual uint32 Update();
    uint32 SetChildColors(uint32 red, uint32 green, uint32 blue, uint32 size);
    uint32 SetEndpoints(const uint32 *origin, const uint32 *target);

};

#endif
