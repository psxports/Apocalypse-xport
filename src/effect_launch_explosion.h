#ifndef EFFECT_LAUNCH_EXPLOSION_H
#define EFFECT_LAUNCH_EXPLOSION_H

#include "effect_object.h"

class LaunchExplosionEffect : public EffectController
{
public:
    explicit LaunchExplosionEffect(const uint32 *position);
    virtual ~LaunchExplosionEffect() {}
    static LaunchExplosionEffect &Attach(uint32 object);
    virtual uint32 Destroy(uint32 reason);
    virtual uint32 Update();
};

#endif
