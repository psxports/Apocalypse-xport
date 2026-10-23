#ifndef GUIDED_PROJECTILE_H
#define GUIDED_PROJECTILE_H

#include "object_reference.h"
#include "vector3.h"
#include "projectile_base.h"
#include "animated_sprite.h"

class AnimatedSprite;

// Uses model key 717FB6B2 from items.psx
class GuidedProjectile : public ProjectileBase
{
private:
    uint32 launch_delay;
    ModelReference target_ref;
    bool held;
    uint32 target_mode;
    Vector3 target_pos;
    uint8 arrival_count;
    sint32 turn_limit;
    uint16 launch_flags;
    ObjectReference<AnimatedSprite> sprite;

public:
    GuidedProjectile(const Vector3 &pos, uint16 damage, ModelInstance **owner_pointer, uint16 flags);
    virtual uint32 Destroy(uint32 reason);
    uint32 SetAngles(const ShortVector3 &angles);
    uint32 SetSize(uint32 size);

private:
    ModelInstance *CaptureOwner();
    uint32 SpawnDebris();

public:
    virtual uint32 Explode();
    void Launch(uint32 mode, const Vector3 *target);
    virtual uint32 Update();
    void SetLaunchDelay(uint32 delay);
};

#endif
