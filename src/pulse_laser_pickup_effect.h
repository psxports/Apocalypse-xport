#ifndef PULSE_LASER_PICKUP_EFFECT_H
#define PULSE_LASER_PICKUP_EFFECT_H

#include "object_reference.h"
#include "vector3.h"
#include "effect_object.h"
#include "pickup_effect.h"

class RingEffect;
class BeamSegments;
class RibbonSegments;

class PulseLaserPickupEffect : public PickupEffect
{
private:
    ObjectReference<RingEffect> ring;

public:
    PulseLaserPickupEffect();
    virtual uint32 Destroy(uint32 reason);
    virtual uint32 SetPos(const Vector3 *pos);
    virtual ~PulseLaserPickupEffect();
};

#endif
