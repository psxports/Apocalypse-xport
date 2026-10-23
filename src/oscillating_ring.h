#ifndef OSCILLATING_RING_H
#define OSCILLATING_RING_H

#include "vector3.h"
#include "ring_effect.h"

// Eight-point ring with independently oscillating radii
class OscillatingRing : public RingEffect
{
public:
    virtual ~OscillatingRing()
    {
    }

private:
    uint8 speed[8];
    uint32 phase[8];
    sint32 amplitude;
    uint32 base_radius;

public:
    OscillatingRing(const Vector3 *pos, uint32 radius, uint32 extent, uint32 amplitude, uint8 r, uint8 g, uint8 b, uint8 tail_r, uint8 tail_g, uint8 tail_b);
    virtual void Update();
    virtual uint32 Destroy(uint32 reason);
};

#endif
