#ifndef OSCILLATING_RING_H
#define OSCILLATING_RING_H

#include "ring_effect.h"

// TODO Recover the original resource/class name
class OscillatingRing : public RingEffect
{
public:
    OscillatingRing(const uint32 *position, uint32 radius, uint32 extent, uint32 amplitude, uint32 red, uint32 green, uint32 blue, uint32 tail_red, uint32 tail_green, uint32 tail_blue);
    virtual ~OscillatingRing() {}
    static OscillatingRing &Attach(uint32 object);
    virtual uint32 Destroy(uint32 reason);
    virtual void Update();
};

#endif
