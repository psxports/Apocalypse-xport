#ifndef ENDPOINT_BEAM_H
#define ENDPOINT_BEAM_H

#include "object_reference.h"
#include "vector3.h"
#include "beam_segments.h"

class EndpointBeam : public BeamSegments
{
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

    uint16 phase;

public:
    EndpointBeam(uint8 r, uint8 g, uint8 b, uint32 count, sint32 width, uint32 radius0, uint32 radius1, uint32 point_phase0, uint32 tick_phase0, uint32 point_phase1, uint32 tick_phase1);
    virtual uint32 Destroy(uint32 reason);

private:
    uint32 RecreateChild(uint8 r, uint8 g, uint8 b, uint16 size);

public:
    uint32 SetEndpoints(const Vector3 *origin, const Vector3 *target);
    virtual void Update();
    virtual ~EndpointBeam();
};

#endif
