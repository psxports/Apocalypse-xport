#ifndef RING_BURST_EFFECT_H
#define RING_BURST_EFFECT_H

#include "ring_effect.h"

// TODO Recover the original name for the burst ring owner
class RingBurstEffect : public RingEffect
{
public:
    RingBurstEffect(const uint32 *position, uint32 segments, uint32 red, uint32 green, uint32 blue, uint32 center_fade, uint32 ring_red, uint32 ring_green, uint32 ring_blue, uint32 ring_fade, uint32 extent, uint32 angle_step, uint32 regenerate, uint32 odd_min, uint32 odd_range, uint32 even_min, uint32 even_range, uint32 decay, uint32 lifetime);
    virtual ~RingBurstEffect() {}
    static RingBurstEffect &Attach(uint32 object);
    virtual uint32 Destroy(uint32 reason);
    virtual void Update();
    uint32 RegenerateRadii();
};

uint32 SpawnExplosionRing(const uint32 *position, uint32 size, uint32 red, uint32 green, uint32 blue, uint32 segments, uint32 mode, uint32 extent);

#endif
