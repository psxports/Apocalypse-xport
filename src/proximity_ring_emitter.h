#ifndef PROXIMITY_RING_EMITTER_H
#define PROXIMITY_RING_EMITTER_H

#include "effect_object.h"

// Control an oscillating ring while the player is within range
class ProximityRingEmitter : public EffectController
{
public:
    ProximityRingEmitter(const uint32 *position, uint32 index, uint32 range, uint32 field72, uint32 field76, uint32 field88, uint32 red0, uint32 green0, uint32 blue0, uint32 red1, uint32 green1, uint32 blue1);
    virtual ~ProximityRingEmitter() {}
    static ProximityRingEmitter &Attach(uint32 object);
    virtual uint32 Destroy(uint32 reason);
    virtual uint32 Update();
};

#endif
