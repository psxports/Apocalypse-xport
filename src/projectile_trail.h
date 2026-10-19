#ifndef PROJECTILE_TRAIL_H
#define PROJECTILE_TRAIL_H

#include "vector3.h"

#include "effect_object.h"

class RingEffect;

// TODO Recover the original name for the 0x800A2B70 projectile owner
class TrailProjectile : public EffectController
{
public:
    TrailProjectile(const uint32 *position, const uint32 *target, const uint16 *angles, uint32 lifetime, uint32 speed, uint32 scale, uint32 red, uint32 green, uint32 blue, uint32 tail_red, uint32 tail_green, uint32 tail_blue);
    virtual ~TrailProjectile();
    virtual uint32 Destroy(uint32 reason);
    virtual void Update();

private:
    uint16 age;
    uint16 lifetime;
    uint32 speed;
    Vector3 target;
    ShortVector3 angles;
    ShortVector3 angular_velocity;
    ShortVector3 angular_acceleration;
    ObjectReference<RingEffect> ring;
};

#endif
