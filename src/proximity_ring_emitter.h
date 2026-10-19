#ifndef PROXIMITY_RING_EMITTER_H
#define PROXIMITY_RING_EMITTER_H

#include "effect_object.h"

class OscillatingRing;

// Control an oscillating ring while the player is within range
class ProximityRingEmitter : public EffectController
{
public:
    ProximityRingEmitter(const uint32 *position, uint32 index, uint32 range, uint32 radius, uint32 extent, uint32 amplitude, uint32 red0, uint32 green0, uint32 blue0, uint32 red1, uint32 green1, uint32 blue1);
    virtual ~ProximityRingEmitter();
    virtual uint32 Destroy(uint32 reason);
    virtual void Update();
    uint16 record_idx;

private:
    uint32 radius;
    uint32 extent;
    CVECTOR colors[2];
    uint32 amplitude;
    uint32 range;
    ObjectReference<OscillatingRing> child;
};

#endif
