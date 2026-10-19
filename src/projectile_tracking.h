#ifndef PROJECTILE_TRACKING_H
#define PROJECTILE_TRACKING_H

#include "vector3.h"
extern "C"
{
#include "psx.h"
}
#include "projectile_base.h"

struct TrackingState
{
    uint32 launch_delay;
    ModelReference target;
    uint8 fixed_target;
    uint8 speed;
    Vector3 target_pos;
    uint32 turn_limit;
    uint32 sound;
    uint32 range;

    TrackingState() : launch_delay(0), fixed_target(0), speed(0), turn_limit(0), sound(0), range(0)
    {
        target_pos.words[0] = target_pos.words[1] = target_pos.words[2] = 0;
    }
};

class TrackingProjectile : public ProjectileBase
{
public:
    TrackingProjectile(const uint32 *position, const sint16 *angles, uint32 range, uint32 damage, ModelInstance **list);
    TrackingProjectile(const uint32 *position, const sint16 *angles, uint32 range, uint32 damage, ModelInstance **list, const uint32 *target_position);
    virtual uint32 Destroy(uint32 reason);
    void SetSpeed(uint32 speed);
    virtual uint32 Explode();
    virtual uint32 Update();

private:
    void Configure(const uint32 *position, const sint16 *angles, uint32 range, uint32 damage, ModelInstance **list);
    uint32 ConsiderTarget(ModelInstance *target, sint32 *closest, uint32 pass);
    void SelectTarget();
    TrackingState tracking;
};

#endif
