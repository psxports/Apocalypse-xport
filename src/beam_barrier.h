#ifndef BEAM_BARRIER_H
#define BEAM_BARRIER_H

#include "vector3.h"

#include "visual_object.h"

class EndpointBeam;

// Damaging endpoint beam shared by record kinds 404 and 408
class BeamBarrier : public VisualObject
{
public:
    BeamBarrier(const uint32 *data, uint32 index, uint32 mode);
    BeamBarrier(const uint32 *position, const uint32 *target);
    virtual ~BeamBarrier();
    virtual uint32 Destroy(uint32 reason);
    virtual uint32 Update();
    void Initialize(uint32 mode);
    void SetMode(uint32 mode);
    Vector3 target;

private:
    uint32 bounds[4];
    uint32 mode;
    ObjectReference<EndpointBeam> effect;
};

#endif
