#ifndef PROJECTILE_GUIDED_H
#define PROJECTILE_GUIDED_H

#include "projectile_base.h"

// TODO Recover the projectile resource name for hash717FB6B2
struct GuidedState
{
    uint32 launch_delay;
    ModelReference target;
    uint32 held;
    uint32 target_mode;
    uint32 target_pos[3];
    uint8 arrival_count;
    uint32 turn_limit;
    uint16 flags;
    uint32 sprite_object;

    GuidedState() : launch_delay(0), held(0), target_mode(0), arrival_count(0), turn_limit(0), flags(0), sprite_object(0)
    {
        target_pos[0] = target_pos[1] = target_pos[2] = 0;
    }
};

class GuidedProjectile : public ProjectileBase
{
public:
    GuidedProjectile(const uint32 *position, uint32 damage, ModelInstance **owner_pointer, uint32 flags);
    static GuidedProjectile &Attach(uint32 object);
    virtual uint32 Destroy(uint32 reason);
    uint32 SetAngles(const uint16 *angles);
    uint32 SetSize(uint32 size);
    ModelInstance *CaptureOwner();
    uint32 SpawnDebris();
    virtual uint32 Explode();
    void Launch(uint32 mode, const uint32 *target);
    void SetLaunchDelay(uint32 delay);
    virtual uint32 Update();

private:
    GuidedState guided;
};

#endif
