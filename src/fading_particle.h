#ifndef FADING_PARTICLE_H
#define FADING_PARTICLE_H

#include "vector3.h"
#include "effect_object.h"
#include "primitive_particle.h"

// Untextured particles with byte color wrap and damped motion
class FadingParticle : public PrimitiveParticle
{
public:
    virtual ~FadingParticle()
    {
    }

private:
    sint16 age;
    uint16 lifetime;
    RGBC fade_step;

public:
    FadingParticle(const Vector3 *src_pos, uint32 speed, sint32 vertical_accel, uint32 lifetime_range);
    virtual uint32 Destroy(uint32 reason);
    virtual void Update();
};

#endif
