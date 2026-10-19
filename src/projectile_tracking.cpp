#include <stdlib.h>
#include <string.h>
#include <new>

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
#include "vector_arithmetic.h"
#include "collision.h"
#include "model_resources.h"
#include "effect_lifecycle.h"
#include "angle_math.h"
#include "ring_effect.h"
#include "audio_session.h"
#include "fire_cycle.h"
#include "vector_distance.h"
#include "explosion_effects.h"
#include "game_random.h"

#include "projectile_tracking.h"

void TrackingProjectile::Configure(const uint32 *position, const sint16 *angles, uint32 range, uint32 damage, ModelInstance **list)
{
    uint32 value;
    uint32 pitch;
    uint32 numerator;
    uint32 denominator;
    uint32 quotient;
    GameHeapBlock *child;
    FUNCTION_MARKER(0x8002CC78, "SLUS_003.73");
    value = items_bank;
    value = FindModelResourceIndex(0xD40B155E, value);
    this->model.model_idx = value;
    AddToList(visual_lists.projectiles);
    projectile.damage_kind = (uint32)(2);
    projectile.trace_movement = true;
    tracking.speed = (uint8)(120);
    memcpy(model.pos.words, position, sizeof(model.pos.words));
    CopyAngleVector(model.angles.words, (const uint16 *)angles);
    pitch = this->model.angles.words[0];
    tracking.range = (uint32)(range);
    projectile.damage = (uint16)(damage);
    projectile.owner_list = list;
    this->model.angles.words[0] = pitch - 10;
    this->kind = list == &visual_lists.players ? 61 : 62;
    tracking.turn_limit = (uint32)(60);
    BuildDirectionVector(motion.velocity.words, 120, this->model.angles.words);
    denominator = tracking.speed;
    numerator = tracking.range;
    if (!denominator)
        quotient = (sint32)numerator < 0 ? 1 : (uint32)-1;
    else
        quotient = (uint32)((sint32)numerator / (sint32)denominator);
    motion.drag[0] = 3;
    tracking.launch_delay = (uint32)(quotient);
    motion.drag[1] = 3;
    motion.drag[2] = 3;
    motion.angular_drag[0] = 1;
    motion.angular_drag[1] = 1;
    motion.angular_drag[2] = 1;
    motion.angular_velocity.words[2] = 400;
    if (RandomRange(2))
    {
        value = motion.angular_velocity.words[2];
        motion.angular_velocity.words[2] = 0 - value;
    }
    use_small_effect_pool = false;
    child = AllocateEffectStorage(104);
    if (child)
    {
        projectile.glow_object = new (child) RingEffect(this->model.pos.words, 32, 32, 128, 128, 128, 64, 32, 0);
    }
    use_small_effect_pool = true;
    if (!projectile.glow_object)
    {
        GameDiagnostic("Projectile ring allocation failed\n");
        abort();
    }
    projectile.glow_object->persistent = 1;
    projectile.glow_distance = (uint32)((uint32)-30);
    SyncChild();
    value = PlayPositionedSound(15, this->model.pos.words, 0);
    tracking.sound = (uint32)(value);
    SetTrailColor(9, 80, 0, 0);
}

TrackingProjectile::TrackingProjectile(const uint32 *position, const sint16 *angles, uint32 range, uint32 damage, ModelInstance **list) : ProjectileBase()
{
    ModelInstance *target;
    FUNCTION_MARKER(0x8002CE94, "SLUS_003.73");
    Configure(position, angles, range, damage, list);
    if (list == &visual_lists.players)
    {
        target = visual_lists.players;
        tracking.target.SetObject(target);
    }
    else
        SelectTarget();
}

TrackingProjectile::TrackingProjectile(const uint32 *position, const sint16 *angles, uint32 range, uint32 damage, ModelInstance **list, const uint32 *target_position) : ProjectileBase()
{
    FUNCTION_MARKER(0x8002CF60, "SLUS_003.73");
    Configure(position, angles, range, damage, list);
    tracking.fixed_target = (uint8)(1);
    memcpy(tracking.target_pos.words, target_position, sizeof(tracking.target_pos.words));
}

uint32 TrackingProjectile::Destroy(uint32 reason)
{
    uint32 age, sound, result;
    ModelInstance *target;
    FUNCTION_MARKER(0x8002D018, "SLUS_003.73");
    age = projectile.age;
    if ((sint32)age < 15)
    {
        sound = tracking.sound;
        StopSoundVoices(sound);
    }
    RemoveFromList(visual_lists.projectiles);
    target = tracking.target.GetObject();
    if (target)
        static_cast<VisualObject *>(target->owner)->target_mark = 0;
    ProjectileBase::Destroy(0);
    result = reason & 1;
    if (result)
        result = ReleaseObjectStorage(this);
    return result;
}

uint32 TrackingProjectile::ConsiderTarget(ModelInstance *target, sint32 *closest, uint32 pass)
{
    CollisionRay &shared_ray = GetSharedCollisionRay();
    uint32 value;
    ModelInstance *blocked;
    sint32 distance;
    FUNCTION_MARKER(0x8002D0A8, "SLUS_003.73");
    if (static_cast<VisualObject *>(target->owner)->target_mark && !pass)
        return 0;
    if (!(static_cast<VisualObject *>(target->owner)->lifecycle_flags & 0x10))
        return 0;
    if (static_cast<VisualObject *>(target->owner)->kind == 204)
        return 0;
    distance = (sint32)MeasurePositionDistance(target->pos.words, this->model.pos.words);
    if (distance >= *closest)
        return 0;
    value = this->model.pos.words[0];
    shared_ray.endpoints[0].words[0] = (value);
    value = this->model.pos.words[1];
    shared_ray.endpoints[0].words[1] = (value);
    uint32 start_z = this->model.pos.words[2];
    shared_ray.endpoints[0].words[1] = (value - 409600);
    shared_ray.endpoints[0].words[2] = start_z;
    value = target->pos.words[0];
    shared_ray.endpoints[1].words[0] = (value);
    value = target->pos.words[1];
    shared_ray.endpoints[1].words[1] = (value);
    value = target->pos.words[2];
    shared_ray.endpoints[1].words[2] = (value);
    InitializeCollisionRay(shared_ray);
    collision_query.reject_surface40 = true;
    QueryCollisionRay(shared_ray, 0);
    blocked = shared_ray.hit.GetObject();
    collision_query.reject_surface40 = false;
    if (blocked)
        return 0;
    *closest = distance;
    return 1;
}

void TrackingProjectile::SelectTarget()
{
    ModelInstance *selected = 0;
    uint32 pass = 0;
    ModelInstance **list;
    ModelInstance *candidate;
    sint32 closest = 1000000;
    FUNCTION_MARKER(0x8002D1D4, "SLUS_003.73");
    while (pass < 2)
    {
        list = projectile.owner_list;
        candidate = *list;
        while (candidate)
        {
            if (ConsiderTarget(candidate, &closest, pass))
                selected = candidate;
            candidate = candidate->next;
        }
        candidate = visual_lists.solids;
        while (candidate)
        {
            if (static_cast<VisualObject *>(candidate->owner)->kind == 205 && ConsiderTarget(candidate, &closest, pass))
                selected = candidate;
            candidate = candidate->next;
        }
        ++pass;
        if (selected)
            break;
    }
    tracking.target.SetObject(selected);
}

uint32 TrackingProjectile::Explode()
{
    uint32 random;
    ModelInstance **list;
    FUNCTION_MARKER(0x8002D2F0, "SLUS_003.73");
    list = projectile.owner_list;
    if (list == &visual_lists.players)
    {
        SpawnFireCycle(this->model.pos.words, 180, 1, 2);
        random = RandomRange(2);
        PlayPositionedSound(random + 1, this->model.pos.words, 0);
    }
    else
        HandleExplosionProximity(this->model.pos.words, 0);
    if (level_environment.area_kind != 9)
    {
        QueueExplosion(model.pos.words, 512, 32, 256);
    }
    return Finish();
}

uint32 TrackingProjectile::Update()
{
    GameHeapBlock *trail;
    uint32 green;
    uint32 kind;
    uint32 red;
    uint32 blue;
    uint32 counter;
    uint32 x;
    uint32 y;
    uint32 z;
    ModelInstance *target;
    Vector3 position, vector;
    ShortVector3 desired;
    uint32 scale, distance, age;
    ModelInstance *hit;
    ModelInstance *head;
    ModelInstance **list;
    uint32 damage, result;
    ModelInstance *player;
    sint32 limit;
    FUNCTION_MARKER(0x8002D3B4, "SLUS_003.73");
    ModelBorrow model_borrow;
    AdvanceMotion();
    SyncChild();
    if (!projectile.age)
    {
        use_small_effect_pool = false;
        projectile.trail_object = 0;
        trail = AllocateEffectStorage(100);
        if (trail)
        {
            green = projectile.trail_color.g;
            kind = projectile.trail_kind;
            red = projectile.trail_color.r;
            blue = projectile.trail_color.b;
            projectile.trail_object = new (trail) RibbonTrail(projectile.glow_pos.words, kind, red, green, blue);
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
        projectile.trail_object->SetPosition(projectile.glow_pos.words);
    target = tracking.target.GetObject();
    if (target && (static_cast<VisualObject *>(target->owner)->IsFinished() || !(static_cast<VisualObject *>(target->owner)->lifecycle_flags & 0x10)))
        target = 0;
    counter = tracking.launch_delay;
    if (counter)
    {
        --counter;
        tracking.launch_delay = (uint32)(counter);
        if (!counter)
        {
            this->model.angles.words[0] = 0;
            motion.velocity.words[1] = 0;
        }
        goto CheckCollision;
    }
    if (!target && !tracking.fixed_target)
    {
        motion.angular_velocity.words[0] = 0;
        motion.angular_velocity.words[1] = 0;
        motion.angular_acceleration = zero_angles;
        goto CheckCollision;
    }
    if (!tracking.fixed_target && level_environment.area_kind == 9 && (sint32)projectile.age >= 60 && projectile.owner_list == &visual_lists.players)
    {
        x = this->model.pos.words[0];
        tracking.fixed_target = (uint8)(1);
        y = this->model.pos.words[1];
        z = this->model.pos.words[2];
        tracking.target.Reset();
        tracking.target_pos.words[0] = (uint32)(x);
        tracking.target_pos.words[1] = (uint32)(y + 0x1000000);
        tracking.target_pos.words[2] = (uint32)(z);
    }
    if (target)
    {
        position = target->pos;
        static_cast<VisualObject *>(target->owner)->target_mark = 1;
    }
    else
    {
        position = tracking.target_pos;
    }
    CalculateDirectionAngles(desired.words, this->model.pos.words, position.words);
    scale = tracking.turn_limit;
    if ((sint32)scale >= 200 || MeasurePositionDistance(this->model.pos.words, position.words) < 101)
    {
        this->model.SetAnglesXY(desired.words[0] | ((uint32)desired.words[1] << 16));
        this->model.angles.words[2] = desired.words[2];
    }
    else
    {
        scale = tracking.turn_limit;
        UpdateAngleAcceleration(model.angles, motion.angular_velocity, motion.angular_acceleration, desired, scale);
        scale = tracking.turn_limit;
        tracking.turn_limit = (uint32)(scale + 3);
    }
    scale = tracking.speed;
    BuildDirectionVector(motion.velocity.words, scale, this->model.angles.words);
CheckCollision:
    list = projectile.owner_list;
    head = *list;
    hit = TraceSegmentCollision(pos_history.prev.words, this->model.pos.words, head);
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
    age = projectile.age + 1;
    projectile.age = (uint32)(age);
    limit = GetGameTuning().tracking_projectile_ticks;
    if (limit < (sint32)age)
        goto Expired;
    result = tracking.fixed_target;
    if (!result)
        return result;
    result = tracking.launch_delay;
    if (result)
        return result;
    distance = MeasurePositionDistance(this->model.pos.words, tracking.target_pos.words);
    result = distance < 64;
    if (!result)
        return result;
Expired:
    player = visual_lists.players;
    distance = MeasurePositionDistance(model.pos.words, player->pos.words);
    if (distance < 8000)
        return Explode();
    return Finish();
}

void TrackingProjectile::SetSpeed(uint32 speed)
{
    tracking.speed = (uint8)speed;
}
