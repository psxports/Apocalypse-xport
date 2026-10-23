#ifndef LAUNCH_EXPLOSION_EFFECT_H
#define LAUNCH_EXPLOSION_EFFECT_H

#include "vector3.h"
#include "effect_object.h"
#include "effect_controller.h"

class LaunchExplosionEffect : public EffectController
{
public:
    virtual ~LaunchExplosionEffect()
    {
    }

private:
    uint16 age;
    uint16 lifetime;

public:
    explicit LaunchExplosionEffect(const Vector3 *src_pos);
    virtual uint32 Destroy(uint32 reason);
    virtual void Update();
};

#endif
