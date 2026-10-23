#ifndef PROXIMITY_RING_EMITTER_H
#define PROXIMITY_RING_EMITTER_H

#include "object_reference.h"
#include "vector3.h"
#include "effect_object.h"
#include "effect_controller.h"

class OscillatingRing;

// Control an oscillating ring while the player is within range
class ProximityRingEmitter : public EffectController
{
public:
    uint16 rec_idx;

private:
    uint32 radius;
    uint32 extent;

    struct Color
    {
        uint8 r, g, b;
    } colors[2];

    uint32 amplitude;
    sint32 range;
    ObjectReference<OscillatingRing> child;

public:
    ProximityRingEmitter(const Vector3 *pos, uint16 idx, sint32 range, uint32 radius, uint32 extent, uint32 amplitude, uint8 r0, uint8 g0, uint8 b0, uint8 r1, uint8 g1, uint8 b1);
    virtual uint32 Destroy(uint32 reason);
    virtual void Update();
    virtual ~ProximityRingEmitter();
};

#endif
