#ifndef PROJECTILE_GUIDED_H
#define PROJECTILE_GUIDED_H

#include "projectile_base.h"

// TODO Recover the projectile resource name for hash717FB6B2
class GuidedProjectile : public ProjectileBase
{
public:
    GuidedProjectile(const uint32 *position, uint32 damage, const uint32 *owner_pointer, uint32 flags);
    static GuidedProjectile &Attach(uint32 object);
    virtual uint32 Destroy(uint32 reason);
    uint32 SetAngles(const uint16 *angles);
    uint32 SetSize(uint32 size);
    uint32 CaptureOwner();
    uint32 SpawnDebris();
    virtual uint32 Explode();
    void Launch(uint32 mode, const uint32 *target);
    virtual uint32 Update();

private:
    uint32 CallAction();
};

#endif
