#ifndef PARTICLE_BEAM_PICKUP_EFFECT_H
#define PARTICLE_BEAM_PICKUP_EFFECT_H

#include "object_reference.h"
#include "vector3.h"
#include "effect_object.h"
#include "pickup_effect.h"

class RingEffect;
class BeamSegments;
class RibbonSegments;

// Particle beam pickup owns layered beam segments
class ParticleBeamPickupEffect : public PickupEffect
{
private:
    ObjectReference<BeamSegments> beam[2];

public:
    ParticleBeamPickupEffect();
    virtual uint32 Destroy(uint32 reason);
    virtual uint32 SetPos(const Vector3 *pos);
    virtual ~ParticleBeamPickupEffect();
};

#endif
