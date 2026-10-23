#ifndef RING_BURST_EFFECT_H
#define RING_BURST_EFFECT_H

#include "vector3.h"
#include "ring_effect.h"

// Alternating randomized radii with independent center and ring fades
class RingBurstEffect : public RingEffect
{
public:
    virtual ~RingBurstEffect()
    {
    }

private:
    sint16 age;
    uint16 lifetime;
    sint16 center_fade;
    sint16 ring_fade;
    uint16 angle_inc;

    struct RadiusRange
    {
        sint16 min, range;
    } radius_params[2];

    sint16 decay;
    uint8 regenerate;

public:
    RingBurstEffect(const Vector3 *src_pos, uint32 segs, uint8 r, uint8 g, uint8 b, sint16 center_fade, uint8 ring_r, uint8 ring_g, uint8 ring_b, sint16 ring_fade, uint32 extent, uint16 angle_step, uint8 regenerate, sint16 odd_min, sint16 odd_range, sint16 even_min, sint16 even_range, sint16 decay, uint16 lifetime);

private:
    uint32 RegenerateRadii();

public:
    virtual void Update();
    static void SpawnExplosion(const Vector3 *pos, sint32 size, uint8 r, uint8 g, uint8 b, uint32 segs, bool mode, uint32 extent);
    virtual uint32 Destroy(uint32 reason);
};

#endif
