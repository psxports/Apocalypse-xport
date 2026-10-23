#ifndef TRAIL_PROJECTILE_H
#define TRAIL_PROJECTILE_H

#include "object_reference.h"
#include "vector3.h"
#include "effect_object.h"
#include "effect_controller.h"

class RingEffect;

// TODO Recover the original name for the 0x800A2B70 projectile owner
class TrailProjectile : public EffectController
{
private:
    uint16 age;
    uint16 lifetime;
    uint32 speed;
    Vector3 target;
    ShortVector3 angles;
    ShortVector3 angular_velocity;
    ShortVector3 angular_acceleration;
    ObjectReference<RingEffect> ring;

public:
    TrailProjectile(const Vector3 *src_pos, const Vector3 *target, const ShortVector3 *angles, uint16 lifetime, uint32 speed, uint32 scale, uint8 r, uint8 g, uint8 b, uint8 tail_r, uint8 tail_g, uint8 tail_b);
    virtual uint32 Destroy(uint32 reason);
    virtual void Update();
    virtual ~TrailProjectile();
};

#endif
