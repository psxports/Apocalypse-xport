#include <new>
#include <stdlib.h>

#include "collision_ray.h"
#include "game_heap.h"
#include "object_initializers.h"
#include "effect_object.h"
#include "visual_object.h"
#include "object_damage.h"
#include "game_diagnostics.h"
#include "game_object_allocation.h"
#include "global.h"
#include "collision.h"
#include "sprite_trail.h"
#include "ribbon_trail.h"
#include "explosion_effects.h"
#include "angle_math.h"
#include "model_bank.h"
#include "effect_lifecycle.h"
#include "guided_projectile.h"
#include "animated_sprite.h"
#include "waving_beam.h"
#include "projectile_base.h"

GuidedProjectile::GuidedProjectile(const Vector3 &pos, uint16 damage, ModelInstance **owner_pointer, uint16 flags) : ProjectileBase(), launch_delay(0), target_ref(), held(false), target_mode(0), arrival_count(0), turn_limit(0), launch_flags(0), sprite()
{
    GameHeapBlock *child;
    FUNCTION_MARKER(0x8002D7C4, "SLUS_003.73");
    target_pos.x = target_pos.y = target_pos.z = 0;
    model.model_idx = ModelBank::FindResourceIdx(0x717FB6B2, items_bank);
    AddToList(visual_lists.projectiles);
    kind = 61;
    damage_kind = 2;
    trace_movement = true;
    model.pos = pos;
    this->damage = damage;
    owner_list = owner_pointer;
    turn_limit = 48;
    SetAngles(model.angles);
    launch_delay = 1;
    motion.drag[0] = 3;
    motion.drag[1] = 3;
    motion.drag[2] = 3;
    motion.angular_drag[0] = 1;
    motion.angular_drag[1] = 1;
    motion.angular_drag[2] = 1;
    use_small_effect_pool = false;
    child = EffectObject::AllocateStorage(120);
    if (child)
    {
        sprite = new (child) AnimatedSprite(&model.pos, "Fireball", 2048, 1, 0, -1);
    }
    if (!sprite)
    {
        GameDiagnostic(("Guided projectile sprite allocation failed\n"));
        abort();
    }
    sprite->persistent = true;
    use_small_effect_pool = true;
    CaptureOwner();
    held = true;
    launch_flags = flags;
}

uint32 GuidedProjectile::Destroy(uint32 reason)
{
    FUNCTION_MARKER(0x8002D930, "SLUS_003.73");
    RemoveFromList(visual_lists.projectiles);
    if (sprite)
    {
        ObjectBorrow borrow;
        sprite->Destroy(3);
    }
    ProjectileBase::Destroy(0);
    return (reason & 1) ? ReleaseStorage() : 0;
}

uint32 GuidedProjectile::SetAngles(const ShortVector3 &angles)
{
    FUNCTION_MARKER(0x8002D9C0, "SLUS_003.73");
    model.angles = angles;
    return motion.velocity.SetDir(128, model.angles);
}

uint32 GuidedProjectile::SetSize(uint32 size)
{
    uint32 flags;
    AnimatedSprite *spr;
    FUNCTION_MARKER(0x8002DA04, "SLUS_003.73");
    flags = model.flags;
    spr = sprite.Get();
    model.scale.z = size;
    model.scale.y = size;
    model.scale.x = size;
    flags |= 512;
    model.flags = flags;
    if (spr)
        spr->SetSize((size >> 1) & 65535);
    return flags;
}

ModelInstance *GuidedProjectile::CaptureOwner()
{
    FUNCTION_MARKER(0x8002DA4C, "SLUS_003.73");
    ModelInstance *owner = *owner_list;
    target_ref.SetObject(owner);
    return owner;
}

uint32 GuidedProjectile::SpawnDebris()
{
    sint32 idx;
    uint16 angle;
    sint32 radius;
    Vector3 pos;
    GameHeapBlock *child;
    ShortVector3 angles;
    FUNCTION_MARKER(0x8002DA60, "SLUS_003.73");
    for (idx = 0; idx < 6; ++idx)
    {
        angle = RandomRange(4096);
        radius = RandomRange(256) + 128;
        pos = model.pos;
        pos.x = (uint32)pos.x + (radius * rsin(angle));
        pos.z = (uint32)pos.z + (radius * rcos(angle));
        angles.y = 0;
        angles.z = 0;
        angles.x = -1024;
        child = EffectObject::AllocateStorage(364);
        if (child)
        {
            new (child) WavingBeam(&pos, &angles, 512, 60, 2, 160, 160, 160, 5, 0x68B7B136);
        }
    }
    return 0;
}

uint32 GuidedProjectile::Explode()
{
    FUNCTION_MARKER(0x8002DB9C, "SLUS_003.73");
    HandleExplosionProximity(&model.pos, 0);
    QueueExplosion(&model.pos, 512, 32, 256);
    return Finish();
}

void GuidedProjectile::Launch(uint32 mode, const Vector3 *target)
{
    FUNCTION_MARKER(0x8002DC0C, "SLUS_003.73");
    held = false;
    target_mode = mode;
    if (!mode)
    {
        target_pos = *target;
    }
}

uint32 GuidedProjectile::Update()
{
    uint32 result;
    GameHeapBlock *child;
    uint32 delay;
    uint32 mode;
    AnimatedSprite *spr;
    ModelInstance *target;
    ModelInstance *owner;
    Vector3 vector;
    uint32 age;
    ModelInstance *hit;
    ModelInstance **list;
    ShortVector3 desired;
    FUNCTION_MARKER(0x8002DC38, "SLUS_003.73");
    owner = target_ref.GetObject();
    if (owner)
    {
        result = static_cast<VisualObject *>(owner->owner)->IsFinished();
        if (result)
            target_ref.Reset();
        else
        {
            owner = target_ref.GetObject();
            if (static_cast<VisualObject *>(owner->owner)->target_disabled)
                target_ref.Reset();
        }
    }
    result = held;
    if (result)
    {
        spr = sprite.Get();
        return spr ? spr->SetPos(&model.pos) : result;
    }
    AdvanceMotion();
    if (lifecycle_flags & 64)
    {
        result = launch_flags & 1;
        return result ? SpawnDebris() : result;
    }
    spr = sprite.Get();
    if (spr)
        spr->SetPos(&model.pos);
    if (!trail_object)
    {
        use_small_effect_pool = false;
        child = EffectObject::AllocateStorage(100);
        if (child)
        {
            trail_object = new (child) RibbonTrail(&model.pos, 9, 80, 0, 0);
        }
        if (!trail_object)
        {
            GameDiagnostic(("Projectile trail allocation failed\n"));
            abort();
        }
        trail_object->persistent = true;
        use_small_effect_pool = true;
    }
    if (trail_object)
        trail_object->SetPos(&model.pos);
    delay = launch_delay;
    if (delay)
    {
        --delay;
        launch_delay = delay;
        if (!delay)
        {
            model.angles.x = 0;
            motion.velocity.y = 0;
        }
    }
    else
    {
        target = target_ref.GetObject();
        if (target || !target_mode)
        {
            mode = target_mode;
            desired.SetDir(model.pos, mode ? target->pos : target_pos);
            if (turn_limit < 100)
            {
                UpdateAngularAcceleration(model.angles, motion.angular_velocity, motion.angular_acceleration, desired, turn_limit);
                turn_limit += 2;
            }
            else
            {
                model.angles = desired;
            }
            motion.velocity.SetDir(128, model.angles);
        }
        else
        {
            motion.angular_velocity.x = 0;
            motion.angular_velocity.y = 0;
            motion.angular_acceleration = zero_angles;
        }
    }
    list = owner_list;
    owner = *list;
    hit = CollisionRay::TraceSeg(pos_history.prev, model.pos, owner);
    if (hit)
    {
        vector.Normalize(motion.velocity);
        static_cast<VisualObject *>(hit->owner)->Damage(damage, &vector, damage_kind);
        model.pos = hit->pos;
        Explode();
    }
    if (!target_mode && (sint32)model.pos.Distance(target_pos) < 128)
    {
        ++arrival_count;
        if (arrival_count >= 3)
            Explode();
    }
    age = this->age + 1;
    this->age = age;
    result = (sint32)age < 121;
    return result ? result : Explode();
}

void GuidedProjectile::SetLaunchDelay(uint32 delay)
{
    launch_delay = delay;
}
