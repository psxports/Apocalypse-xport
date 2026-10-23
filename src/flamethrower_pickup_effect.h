#ifndef FLAMETHROWER_PICKUP_EFFECT_H
#define FLAMETHROWER_PICKUP_EFFECT_H

#include "object_reference.h"
#include "vector3.h"
#include "effect_object.h"
#include "pickup_effect.h"

class RingEffect;
class BeamSegments;
class RibbonSegments;

// Pickup kinds map through Pickup::Collect to the corresponding weapon
class FlamethrowerPickupEffect : public PickupEffect
{
private:
    ObjectReference<RibbonSegments> ribbon;

public:
    FlamethrowerPickupEffect();
    virtual uint32 Destroy(uint32 reason);
    virtual uint32 SetPos(const Vector3 *pos);
    virtual ~FlamethrowerPickupEffect();
};

#endif
