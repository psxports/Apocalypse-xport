#ifndef PROJECTILE_BALLISTIC_H
#define PROJECTILE_BALLISTIC_H

extern "C"
{
#include "psx.h"
}
#include "projectile_base.h"

class BallisticProjectile : public ProjectileBase
{
public:
    BallisticProjectile(const uint32 *position, const sint16 *angles, uint32 damage, uint32 speed, uint32 list);
    static BallisticProjectile &Attach(uint32 object);
    virtual uint32 Destroy(uint32 reason);
    uint32 ResolveImpact();
    virtual uint32 Update();

};

#endif
