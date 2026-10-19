#include <new>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "game_heap.h"
#include "object_initializers.h"

#include "effect_object.h"
#include "visual_object.h"
#include "object_damage.h"
#include "game_diagnostics.h"
#include "game_object_allocation.h"
#include "vector_arithmetic.h"
#include "vector_distance.h"
#include "collision.h"
#include "sprite_trail.h"
#include "game_random.h"
#include "explosion_effects.h"
#include "angle_math.h"
#include "model_resources.h"
#include "effect_lifecycle.h"

#include "projectile_guided.h"
#include "effect_sprite.h"
#include "waving_beam.h"

GuidedProjectile::GuidedProjectile(const uint32 *position, uint32 damage, ModelInstance **owner_pointer, uint32 flags) : ProjectileBase()
{
    uint32 tag;
    uint32 x;
    uint32 y;
    uint32 z;
    GameHeapBlock *child;
    FUNCTION_MARKER(0x8002D7C4, "SLUS_003.73");
    tag = items_bank;
    tag = FindModelResourceIndex(0x717FB6B2, tag);
    this->model.model_idx = tag;
    AddToList(visual_lists.projectiles);
    this->kind = 61;
    projectile.damage_kind = (uint32)(2);
    projectile.trace_movement = true;
    x = position[0];
    y = position[1];
    z = position[2];
    this->model.pos.words[0] = x;
    this->model.pos.words[1] = y;
    this->model.pos.words[2] = z;
    projectile.damage = (uint16)(damage);
    projectile.owner_list = owner_pointer;
    guided.turn_limit = (uint32)(48);
    SetAngles(this->model.angles.words);
    guided.launch_delay = (uint32)(1);
    motion.drag[0] = 3;
    motion.drag[1] = 3;
    motion.drag[2] = 3;
    motion.angular_drag[0] = 1;
    motion.angular_drag[1] = 1;
    motion.angular_drag[2] = 1;
    use_small_effect_pool = false;
    child = AllocateEffectStorage(120);
    if (child)
    {
        guided.sprite = new (child) AnimatedSprite(model.pos.words, "Fireball", 2048, 1, 0, (uint32)-1);
    }
    if (!guided.sprite)
    {
        GameDiagnostic("Guided projectile sprite allocation failed\n");
        abort();
    }
    guided.sprite->persistent = 1;
    use_small_effect_pool = true;
    CaptureOwner();
    guided.held = (uint32)(1);
    guided.flags = (uint16)(flags);
}

uint32 GuidedProjectile::Destroy(uint32 reason)
{
    FUNCTION_MARKER(0x8002D930, "SLUS_003.73");
    RemoveFromList(visual_lists.projectiles);
    if (guided.sprite)
    {
        ObjectBorrow borrow;
        guided.sprite->Destroy(3);
    }
    ProjectileBase::Destroy(0);
    return (reason & 1) ? ReleaseObjectStorage(this) : 0;
}

uint32 GuidedProjectile::SetAngles(const uint16 *angles)
{
    FUNCTION_MARKER(0x8002D9C0, "SLUS_003.73");
    CopyAngleVector(model.angles.words, angles);
    return BuildDirectionVector(motion.velocity.words, 128, model.angles.words);
}

uint32 GuidedProjectile::SetSize(uint32 size)
{
    uint32 flags;
    AnimatedSprite *sprite;
    FUNCTION_MARKER(0x8002DA04, "SLUS_003.73");
    flags = this->model.flags;
    sprite = guided.sprite.Get();
    this->model.scale.words[2] = size;
    this->model.scale.words[1] = size;
    this->model.scale.words[0] = size;
    flags |= 512;
    this->model.flags = flags;
    if (sprite)
        sprite->SetSize((size >> 1) & 65535);
    return flags;
}

ModelInstance *GuidedProjectile::CaptureOwner()
{
    FUNCTION_MARKER(0x8002DA4C, "SLUS_003.73");
    ModelInstance *owner = *projectile.owner_list;
    guided.target.SetObject(owner);
    return owner;
}

void GuidedProjectile::Launch(uint32 mode, const uint32 *target)
{
    uint32 x, y, z;
    FUNCTION_MARKER(0x8002DC0C, "SLUS_003.73");
    guided.held = (uint32)(0);
    guided.target_mode = (uint32)(mode);
    if (!mode)
    {
        x = target[0];
        y = target[1];
        z = target[2];
        guided.target_pos.words[0] = (uint32)(x);
        guided.target_pos.words[1] = (uint32)(y);
        guided.target_pos.words[2] = (uint32)(z);
    }
}

uint32 GuidedProjectile::SpawnDebris()
{
    uint32 trig_angle;
    uint32 index;
    uint32 angle;
    uint32 radius;
    Vector3 position;
    GameHeapBlock *child;
    ShortVector3 angles;
    FUNCTION_MARKER(0x8002DA60, "SLUS_003.73");
    for (index = 0; index < 6; ++index)
    {
        angle = RandomRange(4096);
        radius = RandomRange(256) + 128;
        trig_angle = angle & 4095;
        position = this->model.pos;
        position.words[0] += radius * (uint32)rsin(trig_angle);
        position.words[2] += radius * (uint32)rcos(trig_angle);
        angles.words[1] = 0;
        angles.words[2] = 0;
        angles.words[0] = (uint16)-1024;
        child = AllocateEffectStorage(364);
        if (child)
        {
            new (child) WavingBeam(position.words, angles.words, 512, 60, 2, 160, 160, 160, 5, 0x68B7B136);
        }
    }
    return 0;
}

uint32 GuidedProjectile::Explode()
{
    FUNCTION_MARKER(0x8002DB9C, "SLUS_003.73");
    HandleExplosionProximity(this->model.pos.words, 0);
    QueueExplosion(model.pos.words, 512, 32, 256);
    return Finish();
}

void GuidedProjectile::SetLaunchDelay(uint32 delay)
{
    guided.launch_delay = delay;
}

uint32 GuidedProjectile::Update()
{
    uint32 result;
    GameHeapBlock *child;
    uint32 delay;
    uint32 mode;
    uint32 packed;
    uint32 scale;
    AnimatedSprite *sprite;
    ModelInstance *target;
    ModelInstance *owner;
    Vector3 vector;
    uint32 damage, kind, x, y, z, count, age;
    ModelInstance *hit;
    ModelInstance **list;
    ShortVector3 desired;
    FUNCTION_MARKER(0x8002DC38, "SLUS_003.73");
    owner = guided.target.GetObject();
    if (owner)
    {
        result = static_cast<VisualObject *>(owner->owner)->IsFinished();
        if (result)
            guided.target.Reset();
        else
        {
            owner = guided.target.GetObject();
            if (static_cast<VisualObject *>(owner->owner)->target_disabled)
                guided.target.Reset();
        }
    }
    result = guided.held;
    if (result)
    {
        sprite = guided.sprite.Get();
        return sprite ? sprite->SetPosition(model.pos.words) : result;
    }
    AdvanceMotion();
    if (this->lifecycle_flags & 64)
    {
        result = guided.flags & 1;
        return result ? SpawnDebris() : result;
    }
    sprite = guided.sprite.Get();
    if (sprite)
        sprite->SetPosition(model.pos.words);
    if (!projectile.trail_object)
    {
        use_small_effect_pool = false;
        child = AllocateEffectStorage(100);
        if (child)
        {
            projectile.trail_object = new (child) RibbonTrail(this->model.pos.words, 9, 80, 0, 0);
        }
        if (!projectile.trail_object)
        {
            GameDiagnostic("Projectile trail allocation failed\n");
            abort();
        }
        projectile.trail_object->persistent = 1;
        use_small_effect_pool = true;
    }
    if (projectile.trail_object)
        projectile.trail_object->SetPosition(model.pos.words);
    delay = guided.launch_delay;
    if (delay)
    {
        --delay;
        guided.launch_delay = (uint32)(delay);
        if (!delay)
        {
            this->model.angles.words[0] = 0;
            motion.velocity.words[1] = 0;
        }
    }
    else
    {
        target = guided.target.GetObject();
        if (target || !guided.target_mode)
        {
            mode = guided.target_mode;
            CalculateDirectionAngles(desired.words, this->model.pos.words, (mode ? target->pos.words : guided.target_pos.words));
            scale = guided.turn_limit;
            if ((sint32)scale < 100)
            {
                scale = guided.turn_limit;
                UpdateAngleAcceleration(model.angles, motion.angular_velocity, motion.angular_acceleration, desired, scale);
                scale = guided.turn_limit;
                guided.turn_limit = (uint32)(scale + 2);
            }
            else
            {
                packed = desired.words[0] | ((uint32)desired.words[1] << 16);
                this->model.SetAnglesXY(packed);
                this->model.SetAnglesXY(packed);
                this->model.angles.words[2] = desired.words[2];
            }
            BuildDirectionVector(motion.velocity.words, 128, this->model.angles.words);
        }
        else
        {
            motion.angular_velocity.words[0] = 0;
            motion.angular_velocity.words[1] = 0;
            motion.angular_acceleration = zero_angles;
        }
    }
    list = projectile.owner_list;
    owner = *list;
    hit = TraceSegmentCollision(pos_history.prev.words, this->model.pos.words, owner);
    if (hit)
    {
        NormalizeFixedVector(vector.words, motion.velocity.words);
        damage = projectile.damage;
        kind = projectile.damage_kind;
        static_cast<VisualObject *>(hit->owner)->Damage(damage, vector.components, kind);
        x = hit->pos.words[0];
        y = hit->pos.words[1];
        z = hit->pos.words[2];
        this->model.pos.words[0] = x;
        this->model.pos.words[1] = y;
        this->model.pos.words[2] = z;
        Explode();
    }
    if (!guided.target_mode && (sint32)MeasurePositionDistance(this->model.pos.words, guided.target_pos.words) < 128)
    {
        count = (guided.arrival_count + 1) & 255;
        guided.arrival_count = (uint8)(count);
        if (count >= 3)
            Explode();
    }
    age = projectile.age + 1;
    projectile.age = (uint32)(age);
    result = (sint32)age < 121;
    return result ? result : Explode();
}
