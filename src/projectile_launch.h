#ifndef PROJECTILE_LAUNCH_H
#define PROJECTILE_LAUNCH_H

#include "projectile_base.h"

struct LaunchedState
{
    uint32 spawn_tick;
    uint8 settled;
    uint32 glow_object;
    uint8 glow_rgb[3];
    uint8 explode_on_contact;
    // TODO Recover the second configuration byte role or prove its store is dead
    uint8 config_byte;

    LaunchedState() : spawn_tick(0), settled(0), glow_object(0), explode_on_contact(0), config_byte(0)
    {
        glow_rgb[0] = glow_rgb[1] = glow_rgb[2] = 0;
    }
};

class LaunchedProjectile : public ProjectileBase
{
public:
    LaunchedProjectile(const uint32 *position, const sint16 *angles, uint32 speed, uint32 damage, uint32 radius, ModelInstance **list, uint32 model, uint32 first, uint32 second);
    LaunchedProjectile(const uint32 *position, const uint32 *target, uint32 speed, uint32 damage, uint32 radius, ModelInstance **owner, uint32 model, uint32 first, uint32 second);
    static LaunchedProjectile &Attach(uint32 object);
    uint32 Configure(ModelInstance **owner, uint32 radius, uint32 model, uint32 first, uint32 second);
    uint32 SetGlowColor(uint32 red, uint32 green, uint32 blue);
    virtual uint32 Destroy(uint32 reason);
    virtual uint32 Update();

private:
    LaunchedState launch;
};

#endif
