#ifndef BALLISTIC_PROJECTILE_H
#define BALLISTIC_PROJECTILE_H

#include "vector3.h"

extern "C"
{
#include "psx.h"
}
#include "projectile_base.h"

class BallisticProjectile : public ProjectileBase
{
public:
    BallisticProjectile(const Vector3 *pos, const ShortVector3 *angles, uint16 damage, uint32 speed, ModelInstance **list);
    virtual uint32 Destroy(uint32 reason);
    uint32 ResolveImpact();
    virtual uint32 Update();
};

#endif
