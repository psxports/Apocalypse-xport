#ifndef PROJECTILE_TRAIL_H
#define PROJECTILE_TRAIL_H

#include "effect_object.h"

// TODO Recover the original name for the 0x800A2B70 projectile owner
class TrailProjectile : public EffectController
{
public:
    TrailProjectile(const uint32 *position, const uint32 *target, const uint16 *angles, uint32 lifetime, uint32 speed, uint32 scale, uint32 red, uint32 green, uint32 blue, uint32 tail_red, uint32 tail_green, uint32 tail_blue);
    virtual ~TrailProjectile() {}
    static TrailProjectile &Attach(uint32 object);
    virtual uint32 Destroy(uint32 reason);
    virtual void Update();
};

#endif
