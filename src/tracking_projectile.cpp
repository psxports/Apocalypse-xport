#include <stdlib.h>
#include <new>

#include "collision_ray.h"
#include "game_heap.h"
#include "level_session.h"
#include "game_tuning.h"
#include "object_initializers.h"
#include "game_diagnostics.h"
#include "effect_object.h"
#include "visual_object.h"
#include "object_damage.h"
#include "audio_voice.h"
#include "game_object_allocation.h"
#include "sprite_trail.h"
#include "ribbon_trail.h"
#include "global.h"
#include "collision.h"
#include "model_bank.h"
#include "effect_lifecycle.h"
#include "angle_math.h"
#include "ring_effect.h"
#include "audio_session.h"
#include "fire_cycle.h"
#include "explosion_effects.h"
#include "tracking_projectile.h"
#include "projectile_base.h"

void TrackingProjectile::Configure(const Vector3 &pos, const ShortVector3 &angles, sint32 range, uint16 damage, ModelInstance **list)
{
    GameHeapBlock *child;
    FUNCTION_MARKER(0x8002CC78, "SLUS_003.73");
    model.model_idx = ModelBank::FindResourceIdx(0xD40B155E, items_bank);
    AddToList(visual_lists.projectiles);
    damage_kind = 2;
    trace_movement = true;
    speed = 120;
    model.pos = pos;
    model.angles = angles;
    launch_range = range;
    this->damage = damage;
    owner_list = list;
    model.angles.x -= 10;
    kind = list == &visual_lists.players ? 61 : 62;
    turn_limit = 60;
    motion.velocity.SetDir(120, model.angles);
    motion.drag[0] = 3;
    launch_delay = DivideSignedWord(launch_range, speed);
    motion.drag[1] = 3;
    motion.drag[2] = 3;
    motion.angular_drag[0] = 1;
    motion.angular_drag[1] = 1;
    motion.angular_drag[2] = 1;
    motion.angular_velocity.z = 400;
    if (RandomRange(2))
        motion.angular_velocity.z = -motion.angular_velocity.z;
    use_small_effect_pool = false;
    child = EffectObject::AllocateStorage(104);
    if (child)
    {
        glow_object = new (child) RingEffect(&model.pos, 32, 32, 128, 128, 128, 64, 32, 0);
    }
    use_small_effect_pool = true;
    if (!glow_object)
    {
        GameDiagnostic(("Projectile ring allocation failed\n"));
        abort();
    }
    glow_object->persistent = true;
    glow_distance = -30;
    SyncChild();
    sound_mask = PlayPositionedSound(15, &model.pos, 0);
    SetTrailColor(9, 80, 0, 0);
}

TrackingProjectile::TrackingProjectile(const Vector3 &pos, const ShortVector3 &angles, sint32 range, uint16 damage, ModelInstance **list) : ProjectileBase(), launch_delay(0), target_ref(), fixed_target(false), speed(0), turn_limit(0), sound_mask(0), launch_range(0)
{
    ModelInstance *target;
    FUNCTION_MARKER(0x8002CE94, "SLUS_003.73");
    target_pos.x = target_pos.y = target_pos.z = 0;
    Configure(pos, angles, range, damage, list);
    if (list == &visual_lists.players)
    {
        target = visual_lists.players;
        target_ref.SetObject(target);
    }
    else
        SelectTarget();
}

TrackingProjectile::TrackingProjectile(const Vector3 &pos, const ShortVector3 &angles, sint32 range, uint16 damage, ModelInstance **list, const Vector3 &target_pos) : ProjectileBase(), launch_delay(0), target_ref(), fixed_target(false), speed(0), turn_limit(0), sound_mask(0), launch_range(0)
{
    FUNCTION_MARKER(0x8002CF60, "SLUS_003.73");
    this->target_pos.x = this->target_pos.y = this->target_pos.z = 0;
    Configure(pos, angles, range, damage, list);
    fixed_target = true;
    this->target_pos = target_pos;
}

uint32 TrackingProjectile::Destroy(uint32 reason)
{
    uint32 result;
    sint32 age;
    ModelInstance *target;
    FUNCTION_MARKER(0x8002D018, "SLUS_003.73");
    age = this->age;
    if (age < 15)
        StopSoundVoices(sound_mask);
    RemoveFromList(visual_lists.projectiles);
    target = target_ref.GetObject();
    if (target)
        static_cast<VisualObject *>(target->owner)->target_mark = 0;
    ProjectileBase::Destroy(0);
    result = reason & 1;
    if (result)
        result = ReleaseStorage();
    return result;
}

bool TrackingProjectile::ConsiderTarget(ModelInstance *target, sint32 *closest, sint32 pass)
{
    CollisionRay &shared_ray = CollisionRay::GetShared();
    ModelInstance *blocked;
    sint32 distance;
    FUNCTION_MARKER(0x8002D0A8, "SLUS_003.73");
    if (static_cast<VisualObject *>(target->owner)->target_mark && !pass)
        return false;
    if (!(static_cast<VisualObject *>(target->owner)->lifecycle_flags & 0x10))
        return false;
    if (static_cast<VisualObject *>(target->owner)->kind == 204)
        return false;
    distance = target->pos.Distance(model.pos);
    if (distance >= *closest)
        return false;
    shared_ray.endpoints[0] = model.pos;
    shared_ray.endpoints[0].y = (uint32)shared_ray.endpoints[0].y - (409600);
    shared_ray.endpoints[1] = target->pos;
    shared_ray.Initialize();
    col_query.reject_surface40 = true;
    shared_ray.Query(0);
    blocked = shared_ray.hit.GetObject();
    col_query.reject_surface40 = false;
    if (blocked)
        return false;
    *closest = distance;
    return true;
}

void TrackingProjectile::SelectTarget()
{
    ModelInstance *sel = 0;
    sint32 pass = 0;
    ModelInstance *candidate;
    sint32 closest = 1000000;
    FUNCTION_MARKER(0x8002D1D4, "SLUS_003.73");
    while (pass < 2)
    {
        candidate = *owner_list;
        while (candidate)
        {
            if (ConsiderTarget(candidate, &closest, pass))
                sel = candidate;
            candidate = candidate->next;
        }
        candidate = visual_lists.solids;
        while (candidate)
        {
            if (static_cast<VisualObject *>(candidate->owner)->kind == 205 && ConsiderTarget(candidate, &closest, pass))
                sel = candidate;
            candidate = candidate->next;
        }
        ++pass;
        if (sel)
            break;
    }
    target_ref.SetObject(sel);
}

uint32 TrackingProjectile::Explode()
{
    sint32 random;
    ModelInstance **list;
    FUNCTION_MARKER(0x8002D2F0, "SLUS_003.73");
    list = owner_list;
    if (list == &visual_lists.players)
    {
        FireCycle::Spawn(&model.pos, 180, 1, 2);
        random = RandomRange(2);
        PlayPositionedSound(random + 1, &model.pos, 0);
    }
    else
        HandleExplosionProximity(&model.pos, 0);
    if (level_environment.area_kind != 9)
    {
        QueueExplosion(&model.pos, 512, 32, 256);
    }
    return Finish();
}

uint32 TrackingProjectile::Update()
{
    GameHeapBlock *trail;
    uint32 counter;
    uint32 x;
    uint32 y;
    uint32 z;
    ModelInstance *target;
    Vector3 pos, vector;
    ShortVector3 desired;
    uint32 distance, age;
    ModelInstance *hit;
    ModelInstance *head;
    ModelInstance **list;
    uint32 result;
    ModelInstance *player;
    sint32 limit;
    FUNCTION_MARKER(0x8002D3B4, "SLUS_003.73");
    ModelBorrow model_borrow;
    AdvanceMotion();
    SyncChild();
    if (!this->age)
    {
        use_small_effect_pool = false;
        trail_object = 0;
        trail = EffectObject::AllocateStorage(100);
        if (trail)
        {
            trail_object = new (trail) RibbonTrail(&glow_pos, trail_kind, trail_r, trail_g, trail_b);
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
        trail_object->SetPos(&glow_pos);
    target = target_ref.GetObject();
    if (target && (static_cast<VisualObject *>(target->owner)->IsFinished() || !(static_cast<VisualObject *>(target->owner)->lifecycle_flags & 0x10)))
        target = 0;
    counter = launch_delay;
    if (counter)
    {
        --counter;
        launch_delay = counter;
        if (!counter)
        {
            model.angles.x = 0;
            motion.velocity.y = 0;
        }
        goto CheckCollision;
    }
    if (!target && !fixed_target)
    {
        motion.angular_velocity.x = 0;
        motion.angular_velocity.y = 0;
        motion.angular_acceleration = zero_angles;
        goto CheckCollision;
    }
    if (!fixed_target && level_environment.area_kind == 9 && (sint32)this->age >= 60 && owner_list == &visual_lists.players)
    {
        x = model.pos.x;
        fixed_target = true;
        y = model.pos.y;
        z = model.pos.z;
        target_ref.Reset();
        target_pos.x = x;
        target_pos.y = y + 0x1000000;
        target_pos.z = z;
    }
    if (target)
    {
        pos = target->pos;
        static_cast<VisualObject *>(target->owner)->target_mark = 1;
    }
    else
    {
        pos = target_pos;
    }
    desired.SetDir(model.pos, pos);
    if (turn_limit >= 200 || model.pos.Distance(pos) < 101)
    {
        model.angles = desired;
    }
    else
    {
        UpdateAngularAcceleration(model.angles, motion.angular_velocity, motion.angular_acceleration, desired, turn_limit);
        turn_limit += 3;
    }
    motion.velocity.SetDir(speed, model.angles);
CheckCollision:
    list = owner_list;
    head = *list;
    hit = CollisionRay::TraceSeg(pos_history.prev, model.pos, head);
    if (hit)
    {
        vector.Normalize(motion.velocity);
        static_cast<VisualObject *>(hit->owner)->Damage(damage, &vector, damage_kind);
        model.pos = hit->pos;
        Explode();
    }
    age = this->age + 1;
    this->age = age;
    limit = GetGameTuning().tracking_projectile_ticks;
    if (limit < (sint32)age)
        goto Expired;
    result = fixed_target;
    if (!result)
        return result;
    result = launch_delay;
    if (result)
        return result;
    distance = model.pos.Distance(target_pos);
    result = distance < 64;
    if (!result)
        return result;
Expired:
    player = visual_lists.players;
    distance = model.pos.Distance(player->pos);
    if (distance < 8000)
        return Explode();
    return Finish();
}

void TrackingProjectile::SetSpeed(uint8 speed)
{
    this->speed = speed;
}
