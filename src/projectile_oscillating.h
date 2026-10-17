#ifndef PROJECTILE_OSCILLATING_H
#define PROJECTILE_OSCILLATING_H

#include "effect_object.h"
#include "model_instance.h"

struct ModelPolygon;

class OscillatingProjectile : public EffectController
{
public:
    OscillatingProjectile(const uint32 *position, const sint16 *angles, uint32 damage, uint32 lifetime, uint32 speed, uint32 amplitude, uint32 red1, uint32 green1, uint32 blue1, uint32 red2, uint32 green2, uint32 blue2);
    virtual ~OscillatingProjectile() {}
    static OscillatingProjectile &Attach(uint32 object);
    virtual uint32 Destroy(uint32 reason);
    virtual uint32 Update();

private:
    ModelReference impact;
};

#endif
