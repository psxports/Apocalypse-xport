#ifndef RING_EFFECT_H
#define RING_EFFECT_H

#include "effect_object.h"

// Native views bind the original ring buffers and state in guest memory
class RingEffect : public EffectObject
{
public:
    RingEffect(uint32 segments, uint32 rows);
    RingEffect(const uint32 *position, uint32 radius, uint32 extent, uint32 red, uint32 green, uint32 blue, uint32 tail_red, uint32 tail_green, uint32 tail_blue);
    virtual ~RingEffect() {}
    static RingEffect &Attach(uint32 object);
    virtual uint32 Destroy(uint32 reason);
    virtual void Update();
    uint32 SetOddRadii(uint32 value);
    uint32 SetEvenRadii(uint32 value);
    uint32 SetRadii(uint32 value);
    uint32 SetRowExtent(uint32 row, uint32 value);
    uint32 SetCenterColor(uint32 red, uint32 green, uint32 blue);
    uint32 SetRingColors(uint32 red, uint32 green, uint32 blue);
    uint32 SetRowColors(uint32 row, uint32 red, uint32 green, uint32 blue);
    uint32 SetColumnColors(uint32 column, uint32 red, uint32 green, uint32 blue);
    uint32 FadeCenter(uint32 amount);
    uint32 FadeRing(uint32 amount);

};

uint32 SpawnProjectileRingTrail(const uint32 *position, uint32 red, uint32 green, uint32 blue);

#endif
