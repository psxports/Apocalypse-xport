#ifndef PLAGUE_GROUND_EFFECTS_H
#define PLAGUE_GROUND_EFFECTS_H

#include "effect_object.h"

// TODO Recover the original names for the two Plague ground effect owners
class PlagueImpactEffect : public EffectController
{
public:
    PlagueImpactEffect(const uint32 *position, uint32 red, uint32 green, uint32 blue);
    virtual ~PlagueImpactEffect() {}
    static PlagueImpactEffect &Attach(uint32 object);
    virtual uint32 Destroy(uint32 reason);
    virtual void Update();
};

class PlagueGroundEffect : public EffectController
{
public:
    PlagueGroundEffect(const uint32 *position, uint32 damage, uint32 red, uint32 green, uint32 blue, uint32 segments, uint32 radial_speed);
    virtual ~PlagueGroundEffect() {}
    static PlagueGroundEffect &Attach(uint32 object);
    virtual uint32 Destroy(uint32 reason);
    virtual void Update();
};

#endif
