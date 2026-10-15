#ifndef BEAST_EFFECTS_H
#define BEAST_EFFECTS_H

#include "effect_object.h"

// Native owners bind their original guest fields through ABI storage
class BeastOrbitEffect : public EffectController
{
public:
    BeastOrbitEffect(const uint32 *position);
    virtual ~BeastOrbitEffect() {}
    static BeastOrbitEffect &Attach(uint32 object);
    virtual uint32 Destroy(uint32 reason);
    virtual uint32 Update();

};

class BeastGroundEffect : public EffectController
{
public:
    BeastGroundEffect(const uint32 *position, uint32 damage, uint32 red, uint32 green, uint32 blue, uint32 segments, uint32 radial_speed, uint32 lifetime);
    virtual ~BeastGroundEffect() {}
    static BeastGroundEffect &Attach(uint32 object);
    virtual uint32 Destroy(uint32 reason);
    virtual uint32 Update();

};

#endif
