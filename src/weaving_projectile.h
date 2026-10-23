#ifndef WEAVING_PROJECTILE_H
#define WEAVING_PROJECTILE_H

#include "projectile_base.h"

// Uses model key 350E968C from items.psx
class WeavingProjectile : public ProjectileBase
{
private:
    Vector3 axis[2];
    Vector3 offset[2];
    uint16 phase[2];
    uint32 trail_segments;

public:
    WeavingProjectile(const Vector3 &pos, const ShortVector3 &angles, bool reverse, uint32 trail_segs, uint32 speed, ModelInstance **owner_pointer, const Vector3 &extra_velocity);
    virtual uint32 Destroy(uint32 reason);
    virtual uint32 Update();
};

#endif
