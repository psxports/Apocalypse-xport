#ifndef BEAM_BARRIER_H
#define BEAM_BARRIER_H

#include "visual_object.h"

// Damaging endpoint beam shared by record kinds 404 and 408
class BeamBarrier : public VisualObject
{
public:
    BeamBarrier(const uint32 *data, uint32 index, uint32 mode);
    BeamBarrier(const uint32 *position, const uint32 *target);
    virtual ~BeamBarrier() {}
    static BeamBarrier &Attach(uint32 object);
    virtual uint32 Destroy(uint32 reason);
    virtual uint32 Update();
    uint32 Initialize(uint32 mode);
    void SetMode(uint32 mode);

};

#endif
