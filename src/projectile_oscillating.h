#ifndef PROJECTILE_OSCILLATING_H
#define PROJECTILE_OSCILLATING_H

#include "effect_object.h"
#include "model_instance.h"

struct ModelPolygon;
class RingEffect;

class OscillatingProjectile : public EffectController
{
public:
    OscillatingProjectile(const uint32 *position, const sint16 *angles, uint32 damage, uint32 lifetime, uint32 speed, uint32 amplitude, uint32 r0, uint32 g0, uint32 b0, uint32 r1, uint32 g1, uint32 b1);
    virtual ~OscillatingProjectile();
    virtual uint32 Destroy(uint32 reason);
    virtual void Update();

    RGBC rgb;

private:
    uint16 age;
    uint16 lifetime;
    uint32 damage;
    ObjectReference<RingEffect> ring;
    uint32 fade_tick;
    uint32 base_radius;
    uint32 phase_step;
    uint32 phase;
    ModelReference impact;
};

#endif
