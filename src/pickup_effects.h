#ifndef PICKUP_EFFECTS_H
#define PICKUP_EFFECTS_H

#include "effect_object.h"

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

    virtual uint32 Destroy(uint32 reason);
    virtual uint32 SetPosition(const uint32 *position);

protected:
    // Native initializer preserves the original inlined base construction
    PickupEffect() : EffectController()
    {
    }
};

// Pickup kinds map through Pickup::Collect to the corresponding weapon
class FlamethrowerPickupEffect : public PickupEffect
{
public:
    FlamethrowerPickupEffect();
    virtual ~FlamethrowerPickupEffect();
    virtual uint32 Destroy(uint32 reason);
    virtual uint32 SetPosition(const uint32 *position);

private:
    ObjectReference<RibbonSegments> ribbon;
};

class RipLaserPickupEffect : public PickupEffect
{
public:
    RipLaserPickupEffect();
    virtual ~RipLaserPickupEffect();
    virtual uint32 Destroy(uint32 reason);
    virtual uint32 SetPosition(const uint32 *position);

private:
    ObjectReference<BeamSegments> beam;
};

class PulseLaserPickupEffect : public PickupEffect
{
public:
    PulseLaserPickupEffect();
    virtual ~PulseLaserPickupEffect();
    virtual uint32 Destroy(uint32 reason);
    virtual uint32 SetPosition(const uint32 *position);

private:
    ObjectReference<RingEffect> ring;
};

// Particle beam pickup owns layered beam segments
class ParticleBeamPickupEffect : public PickupEffect
{
public:
    ParticleBeamPickupEffect();
    virtual ~ParticleBeamPickupEffect();
    virtual uint32 Destroy(uint32 reason);
    virtual uint32 SetPosition(const uint32 *position);

private:
    ObjectReference<BeamSegments> beam[2];
};

#endif
