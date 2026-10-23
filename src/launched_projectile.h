#ifndef LAUNCHED_PROJECTILE_H
#define LAUNCHED_PROJECTILE_H

#include "object_reference.h"
#include "projectile_base.h"

class LaunchedProjectile : public ProjectileBase
{
private:
    uint32 spawn_tick;
    bool settled;
    ObjectReference<RingEffect> glow_ring;
    uint8 glow_r, glow_g, glow_b;
    bool explode_on_contact;

private:
    void Configure(ModelInstance **owner, uint16 radius, uint32 model, uint8 explode_on_contact);

public:
    uint32 SetGlowColor(uint8 r, uint8 g, uint8 b);
    LaunchedProjectile(const Vector3 &pos, const ShortVector3 &angles, uint32 speed, uint16 damage, uint16 radius, ModelInstance **list, uint32 model, uint8 explode_on_contact);
    LaunchedProjectile(const Vector3 &pos, const Vector3 &target, uint32 speed, uint16 damage, uint16 radius, ModelInstance **owner, uint32 model, uint8 explode_on_contact);
    virtual uint32 Destroy(uint32 reason);
    virtual uint32 Update();
    virtual ~LaunchedProjectile();
};

#endif
