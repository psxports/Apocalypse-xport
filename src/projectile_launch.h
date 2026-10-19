#ifndef PROJECTILE_LAUNCH_H
#define PROJECTILE_LAUNCH_H

#include "projectile_base.h"

struct LaunchedState
{
    uint32 spawn_tick;
    uint8 settled;
    ObjectReference<RingEffect> glow_object;
    RGBC glow_color;
    uint8 explode_on_contact;

    LaunchedState() : spawn_tick(0), settled(0), glow_object(), explode_on_contact(0)
    {
        glow_color.rgbc = 0;
    }
};

class LaunchedProjectile : public ProjectileBase
{
public:
    LaunchedProjectile(const uint32 *position, const sint16 *angles, uint32 speed, uint32 damage, uint32 radius, ModelInstance **list, uint32 model, uint32 explode_on_contact);
    LaunchedProjectile(const uint32 *position, const uint32 *target, uint32 speed, uint32 damage, uint32 radius, ModelInstance **owner, uint32 model, uint32 explode_on_contact);
    virtual ~LaunchedProjectile();
    uint32 SetGlowColor(uint32 red, uint32 green, uint32 blue);
    virtual uint32 Destroy(uint32 reason);
    virtual uint32 Update();

private:
    void Configure(ModelInstance **owner, uint32 radius, uint32 model, uint32 explode_on_contact);
    LaunchedState launch;
};

#endif
