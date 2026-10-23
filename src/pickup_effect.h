#ifndef PICKUP_EFFECT_H
#define PICKUP_EFFECT_H

#include "vector3.h"
#include "effect_object.h"
#include "effect_controller.h"

class RingEffect;
class BeamSegments;
class RibbonSegments;

// Own the child graphics for weapon pickups
class PickupEffect : public EffectController
{
public:
    virtual ~PickupEffect()
    {
    }

protected:
    // Native initializer preserves the original inlined base construction
    PickupEffect() : EffectController()
    {
    }

public:
    virtual uint32 SetPos(const Vector3 *pos);
    virtual uint32 Destroy(uint32 reason);
};

#endif
