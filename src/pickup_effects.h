#ifndef PICKUP_EFFECTS_H
#define PICKUP_EFFECTS_H

#include "effect_object.h"

// Own the child graphics for weapon pickups
class PickupEffect : public EffectController
{
public:
    virtual ~PickupEffect() {}
    static PickupEffect &Attach(uint32 object);
    virtual uint32 Destroy(uint32 reason);

protected:
    // Native initializer preserves the original inlined base construction
    PickupEffect() : EffectController() {}
};

// Pickup kinds map through Pickup::Collect to the corresponding weapon
class FlamethrowerPickupEffect : public PickupEffect
{
public:
    FlamethrowerPickupEffect();
    virtual ~FlamethrowerPickupEffect() {}
    static FlamethrowerPickupEffect &Attach(uint32 object);
    virtual uint32 Destroy(uint32 reason);
    virtual uint32 SetPosition(const uint32 *position);

};

class RipLaserPickupEffect : public PickupEffect
{
public:
    RipLaserPickupEffect();
    virtual ~RipLaserPickupEffect() {}
    static RipLaserPickupEffect &Attach(uint32 object);
    virtual uint32 Destroy(uint32 reason);
    virtual uint32 SetPosition(const uint32 *position);

};

class PulseLaserPickupEffect : public PickupEffect
{
public:
    PulseLaserPickupEffect();
    virtual ~PulseLaserPickupEffect() {}
    static PulseLaserPickupEffect &Attach(uint32 object);
    virtual uint32 Destroy(uint32 reason);
    virtual uint32 SetPosition(const uint32 *position);

};

// Particle beam pickup owns layered beam segments
class ParticleBeamPickupEffect : public PickupEffect
{
public:
    ParticleBeamPickupEffect();
    virtual ~ParticleBeamPickupEffect() {}
    static ParticleBeamPickupEffect &Attach(uint32 object);
    virtual uint32 Destroy(uint32 reason);
    virtual uint32 SetPosition(const uint32 *position);
};

#endif
