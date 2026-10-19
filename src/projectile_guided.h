#ifndef PROJECTILE_GUIDED_H
#define PROJECTILE_GUIDED_H

#include "vector3.h"

#include "projectile_base.h"

class AnimatedSprite;

// TODO Recover the projectile resource name for hash717FB6B2
struct GuidedState
{
    uint32 launch_delay;
    ModelReference target;
    uint32 held;
    uint32 target_mode;
    Vector3 target_pos;
    uint8 arrival_count;
    uint32 turn_limit;
    uint16 flags;
    ObjectReference<AnimatedSprite> sprite;

    GuidedState() : launch_delay(0), held(0), target_mode(0), arrival_count(0), turn_limit(0), flags(0)
    {
        target_pos.words[0] = target_pos.words[1] = target_pos.words[2] = 0;
    }
};

class GuidedProjectile : public ProjectileBase
{
public:
    GuidedProjectile(const uint32 *position, uint32 damage, ModelInstance **owner_pointer, uint32 flags);
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
