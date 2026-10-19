#ifndef EFFECT_LAUNCH_EXPLOSION_H
#define EFFECT_LAUNCH_EXPLOSION_H

#include "effect_object.h"

class LaunchExplosionEffect : public EffectController
{
public:
    explicit LaunchExplosionEffect(const uint32 *position);

    virtual ~LaunchExplosionEffect()
    {
    }

    virtual uint32 Destroy(uint32 reason);
    virtual void Update();

private:
    uint16 age;
    uint16 lifetime;
};

#endif
