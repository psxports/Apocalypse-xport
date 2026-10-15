#ifndef PROJECTILE_TRACKING_H
#define PROJECTILE_TRACKING_H
extern "C" {
#include "psx.h"
}
#include "projectile_base.h"

class TrackingProjectile : public ProjectileBase
{
public:
    TrackingProjectile(const uint32 *position, const sint16 *angles, uint32 range, uint32 damage, uint32 list);
    TrackingProjectile(const uint32 *position, const sint16 *angles, uint32 range, uint32 damage, uint32 list, const uint32 *target_position);
    static TrackingProjectile &Attach(uint32 object);
    uint32 Configure(const uint32 *position, const sint16 *angles, uint32 range, uint32 damage, uint32 list);
    virtual uint32 Destroy(uint32 reason);
    uint32 ConsiderTarget(uint32 target, sint32 *closest, uint32 pass);
    void SelectTarget();
    virtual uint32 Explode();
    virtual uint32 Update();

private:
    uint32 CallAction(uint32 slot);
};

uint32 SetProjectileTrailColor(uint32 object, uint32 kind, uint32 red, uint32 green, uint32 blue);
void MakeObjectReference(uint32 output[2], uint32 object);

#endif
