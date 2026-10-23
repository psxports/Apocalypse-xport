#ifndef TRACKING_PROJECTILE_H
#define TRACKING_PROJECTILE_H

#include "vector3.h"

extern "C"
{
#include "psx.h"
}
#include "projectile_base.h"

class TrackingProjectile : public ProjectileBase
{
private:
    uint32 launch_delay;
    ModelReference target_ref;
    bool fixed_target;
    uint8 speed;
    Vector3 target_pos;
    sint32 turn_limit;
    uint32 sound_mask;
    sint32 launch_range;

private:
    void Configure(const Vector3 &pos, const ShortVector3 &angles, sint32 range, uint16 damage, ModelInstance **list);

public:
    TrackingProjectile(const Vector3 &pos, const ShortVector3 &angles, sint32 range, uint16 damage, ModelInstance **list);
    TrackingProjectile(const Vector3 &pos, const ShortVector3 &angles, sint32 range, uint16 damage, ModelInstance **list, const Vector3 &target_pos);
    virtual uint32 Destroy(uint32 reason);

private:
    bool ConsiderTarget(ModelInstance *target, sint32 *closest, sint32 pass);
    void SelectTarget();

public:
    virtual uint32 Explode();
    virtual uint32 Update();
    void SetSpeed(uint8 speed);
};

#endif
