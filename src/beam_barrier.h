#ifndef BEAM_BARRIER_H
#define BEAM_BARRIER_H

#include "object_reference.h"
#include "vector3.h"
#include "visual_object.h"

class EndpointBeam;

// Damaging endpoint beam shared by record kinds 404 and 408
class BeamBarrier : public VisualObject
{
public:
    Vector3 target;

private:
    struct ContactBounds
    {
        sint32 min_x, max_x, min_z, max_z;
    } bounds;

    bool repulsion;
    ObjectReference<EndpointBeam> effect;

public:
    BeamBarrier(RecordCursor data, uint32 idx, bool repulsion);
    BeamBarrier(const Vector3 *pos, const Vector3 *target);

private:
    void Initialize(bool repulsion);

public:
    virtual uint32 Destroy(uint32 reason);

private:
    void SetRepulsion(bool enabled);

public:
    virtual uint32 Update();
    virtual ~BeamBarrier();
};

#endif
