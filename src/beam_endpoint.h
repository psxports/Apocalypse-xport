#ifndef BEAM_ENDPOINT_H
#define BEAM_ENDPOINT_H

#include "vector3.h"

#include "beam_segments.h"

class EndpointBeam : public BeamSegments
{
public:
    EndpointBeam(uint32 red, uint32 green, uint32 blue, uint32 count, uint32 duration, uint32 radius0, uint32 radius1, uint32 point_phase0, uint32 tick_phase0, uint32 point_phase1, uint32 tick_phase1);
    virtual ~EndpointBeam();
    virtual uint32 Destroy(uint32 reason);
    virtual void Update();
    uint32 SetChildColors(uint32 red, uint32 green, uint32 blue, uint32 size);
    uint32 SetEndpoints(const uint32 *origin, const uint32 *target);

private:
    ObjectReference<BeamSegments> child;
    Vector3 origin;
    Vector3 step;
    Vector3 basis[2];

    struct Wave
    {
        uint32 radius;
        uint32 point_phase;
        uint32 tick_phase;
    } wave[2];

    uint32 phase;
};

#endif
