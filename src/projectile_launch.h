#ifndef PROJECTILE_LAUNCH_H
#define PROJECTILE_LAUNCH_H

#include "projectile_base.h"

class LaunchedProjectile : public ProjectileBase
{
public:
    LaunchedProjectile(const uint32 *position, const sint16 *angles, uint32 speed, uint32 damage, uint32 radius, uint32 list, uint32 model, uint32 first, uint32 second);
    LaunchedProjectile(const uint32 *position, const uint32 *target, uint32 speed, uint32 damage, uint32 radius, uint32 owner, uint32 model, uint32 first, uint32 second);
    static LaunchedProjectile &Attach(uint32 object);
    uint32 Configure(uint32 owner, uint32 radius, uint32 model, uint32 first, uint32 second);
    uint32 SetGlowColor(uint32 red, uint32 green, uint32 blue);
    virtual uint32 Destroy(uint32 reason);
    virtual uint32 Update();
};

#endif
