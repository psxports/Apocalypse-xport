#ifndef FADING_PARTICLE_H
#define FADING_PARTICLE_H

#include "effect_object.h"

// TODO Recover the original name of the untextured particle family
class FadingParticle : public PrimitiveParticle
{
public:
    FadingParticle(const uint32 *position, uint32 speed, uint32 acceleration, uint32 lifetime_range);

    virtual ~FadingParticle()
    {
    }

    virtual uint32 Destroy(uint32 reason);
    virtual void Update();

private:
    uint16 age;
    uint16 lifetime;
    RGBC fade_step;
};

#endif
