#ifndef OSCILLATING_PROJECTILE_H
#define OSCILLATING_PROJECTILE_H

#include "object_reference.h"
#include "vector3.h"
#include "effect_object.h"
#include "effect_controller.h"
#include "model_instance.h"

struct ModelPolygon;
class RingEffect;

class OscillatingProjectile : public EffectController
{
private:
    sint16 age;
    uint16 lifetime;
    uint32 damage;
    ObjectReference<RingEffect> ring;
    sint32 fade_tick;
    uint32 base_radius;
    uint32 phase_step;
    uint32 phase;
    ModelReference impact;

public:
    RGBC rgb;

public:
    OscillatingProjectile(const Vector3 *src_pos, const ShortVector3 *angles, uint32 damage, uint32 lifetime, uint32 speed, uint32 amplitude, uint8 r0, uint8 g0, uint8 b0, uint8 r1, uint8 g1, uint8 b1);
    virtual uint32 Destroy(uint32 reason);
    virtual void Update();
    virtual ~OscillatingProjectile();
};

#endif
