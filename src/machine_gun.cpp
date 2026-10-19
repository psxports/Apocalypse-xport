#include <new>
#include <string.h>
#include <stdlib.h>

#include "game_heap.h"
#include "game_tick.h"
#include "game_diagnostics.h"

#include "effect_object.h"

#include "machine_gun.h"
#include "visual_object.h"
#include "projectile_line.h"
#include "ring_burst_effect.h"
#include "bouncing_point_effect.h"
#include "smoke_emitter.h"
#include "audio_voice.h"
#include "angle_math.h"
#include "audio_session.h"
#include "object_damage.h"
#include "collision.h"
#include "vector_arithmetic.h"
#include "vector_distance.h"
#include "game_random.h"
#include "effect_lifecycle.h"

MachineGun::MachineGun(ModelInstance **list) : WeaponNode(list, 0), pitch_spread(0), yaw_spread(0), muzzle_effect(0), fire_rate(0), range(0), sound_id(0), flash_scale(0), light_pending(0)
{
    FUNCTION_MARKER(0x80022B8C, "SLUS_003.73");
    this->type = WPN_MACHINE_GUN;
    Configure(10, 32, 10, 8000, 1, 11, 256);
}

uint32 MachineGun::Destroy(uint32 reason)
{
    FUNCTION_MARKER(0x80022C04, "SLUS_003.73");
    uint32 result = WeaponNode::Destroy(0);
    return (reason & 1) ? ReleaseWeaponStorage(this) : result;
}

void MachineGun::Configure(uint32 pitch_spread, uint32 yaw_spread, uint8 fire_rate, uint32 range, uint8 muzzle_effect, uint32 sound_id, uint32 flash_scale)
{
    FUNCTION_MARKER(0x80022C58, "SLUS_003.73");
    this->pitch_spread = pitch_spread;
    this->yaw_spread = yaw_spread;
    this->fire_rate = fire_rate;
    this->range = range;
    this->muzzle_effect = muzzle_effect;
    this->sound_id = sound_id;
    this->flash_scale = flash_scale;
}

// Native extractions retain original scalar, vector and callback operations
static sint32 DivideMachineGunFire(uint32 n, uint32 d)
{
    if (!d)
        return (sint32)n < 0 ? 1 : -1;
    if (n == 0x80000000 && d == 0xFFFFFFFF)
        return (sint32)n;
    return (sint32)n / (sint32)d;
}

uint32 MachineGun::Fire(const uint32 *input_position, const sint16 *input_rotation)
{
    CollisionRay &shared_ray = GetSharedCollisionRay();
    FUNCTION_MARKER(0x80022C90, "SLUS_003.73");
    ModelBorrow model_borrow;
    sint32 sound_id;
    GameHeapBlock *flash_allocation;
    sint32 emit_muzzle_particle;
    GameHeapBlock *particle_allocation;
    bool geometry_hit;
    bool object_hit;
    ModelInstance *hit_object;
    sint32 muzzle_z;
    ModelInstance **target_list;
    sint32 impact_kind;
    sint32 normal_dot;
    sint32 scaled_dot;
    GameHeapBlock *ricochet_allocation;
    sint32 impact_sound_index;
    sint32 impact_sound;
    GameHeapBlock *trace_allocation;
    uint32 fired_at;

    ShortVector3 rotation;

    Vector3 endpoint, hit_direction, collision_position, near_ray_start, ricochet_delta, ricochet_end, bounce_step, scaled_delta;
    sint32 bounce_distance;
    sint32 delta_length;
    memcpy(muzzle_pos.words, input_position, sizeof(muzzle_pos.words));
    if (!this->fire_active)
    {
        ClearChild();
        sound_id = this->sound_id;
        this->start_tick = game_clock.tick;
        this->sound = PlaySoundEffect(sound_id, 8192, 0);
    }
    if (this->flash_scale)
    {
        flash_allocation = AllocateEffectStorage(124);
        if (flash_allocation)
        {
            uint32 extent = (sint32)(20 * this->flash_scale) / 256;
            uint32 odd_min = (sint32)(14 * this->flash_scale) / 256;
            uint32 even_min = (sint32)(10 * this->flash_scale) / 256;
            uint32 even_range = (sint32)(20 * this->flash_scale) / 256;
            new (flash_allocation) RingBurstEffect(muzzle_pos.words, 5, 255, 235, 0, 2, 90, 10, 0, 20, extent, 0, 0, odd_min, 0, even_min, even_range, 0, 1);
        }
    }
    if (this->fire_active)
    {
        sint32 fire_interval = DivideMachineGunFire(30, this->fire_rate);
        uint32 last_fire = this->last_fire_tick;
        uint32 now = game_clock.tick;
        if ((sint32)(now - last_fire) < fire_interval)
        {
            this->fire_active = 1;
            return 0;
        }
    }
    emit_muzzle_particle = this->muzzle_effect;
    this->light_pending = 1;
    if (emit_muzzle_particle)
    {
        use_small_effect_pool = false;
        particle_allocation = AllocateEffectStorage(116);
        if (particle_allocation)
        {
            new (particle_allocation) BouncingPointEffect(muzzle_pos.words, (const uint16 *)input_rotation);
        }
        use_small_effect_pool = true;
    }
    CopyAngleVector(rotation.words, (const uint16 *)input_rotation);
    {
        uint32 range = this->pitch_spread;
        uint32 random = RandomRange((range << 1) | 1);
        uint32 spread = (uint16)this->pitch_spread;
        uint32 angle = (uint16)rotation.words[0];
        rotation.words[0] = angle + random - spread;
        range = this->yaw_spread;
        random = RandomRange((range << 1) | 1);
        spread = (uint16)this->yaw_spread;
        angle = (uint16)rotation.words[1];
        rotation.words[1] = angle + random - spread;
    }
    NormalizeAngles(rotation.words);
    BuildDirectionVector(endpoint.words, this->range, rotation.words);
    DivideVector(hit_direction.words, endpoint.words, this->range);
    AddVectors(endpoint.words, muzzle_pos.words);
    rotation.words[2] = 0;
    rotation.words[0] = 0;
    BuildDirectionVector(near_ray_start.words, (uint32)-150, rotation.words);
    AddVectors(near_ray_start.words, muzzle_pos.words);
    shared_ray.endpoints[0].words[0] = (near_ray_start.components[0]);
    shared_ray.endpoints[0].words[2] = (near_ray_start.components[2]);
    shared_ray.endpoints[0].words[1] = (near_ray_start.components[1]);
    geometry_hit = false;
    shared_ray.endpoints[1].words[0] = (muzzle_pos.words[0]);
    object_hit = false;
    shared_ray.endpoints[1].words[1] = (muzzle_pos.words[1]);
    shared_ray.endpoints[1].words[2] = (muzzle_pos.words[2]);
    InitializeCollisionRay(shared_ray);
    collision_query.reject_surface40 = true;
    QueryCollisionRay(shared_ray, 1);
    collision_query.reject_surface40 = false;
    hit_object = 0;
    if (shared_ray.hit.GetObject())
        goto HitGeometry;
    shared_ray.endpoints[0].words[0] = (muzzle_pos.words[0]);
    shared_ray.endpoints[0].words[1] = (muzzle_pos.words[1]);
    muzzle_z = muzzle_pos.words[2];
    shared_ray.endpoints[1].words[0] = (endpoint.components[0]);
    shared_ray.endpoints[1].words[1] = (endpoint.components[1]);
    shared_ray.endpoints[1].words[2] = (endpoint.components[2]);
    shared_ray.endpoints[0].words[2] = (muzzle_z);
    InitializeCollisionRay(shared_ray);
    collision_query.reject_surface40 = true;
    QueryCollisionRay(shared_ray, 1);
    target_list = this->owner_list;
    collision_query.reject_surface40 = false;
    hit_object = IntersectObjectSpheres(muzzle_pos.words, endpoint.words, collision_position.words, *target_list, 0);
    if (!shared_ray.hit.GetObject())
    {
        if (!hit_object)
            goto ResolveHit;
        goto HitObject;
    }
    if (hit_object)
    {
        {
            uint32 object_distance = MeasureEuclideanDistance(hit_object->pos.words, muzzle_pos.words);
            uint32 geometry_distance = shared_ray.distance;
            if (object_distance >= geometry_distance)
            {
                geometry_hit = true;
                goto ResolveHit;
            }
        }
    HitObject:
        object_hit = true;

        goto ResolveHit;
    }
HitGeometry:
    geometry_hit = true;

ResolveHit:
    impact_kind = 0;

    if (!geometry_hit)
    {
        goto ResolveObjectHit;
    }

    // Scoped snapshots keep hit-resolution jumps outside initialized locals
    {
        ModelInstance *level_object = shared_ray.hit.GetObject();
        ModelPolygon *polygon = shared_ray.hit.GetPolygon();
        sint32 damage = (sint32)this->damage;
        DamageLevelObject(level_object, polygon, damage);
    }
    {
        ModelInstance *geometry_object = shared_ray.hit.GetObject();
        if (geometry_object && (geometry_object->flags & 16))
            static_cast<VisualObject *>(geometry_object->owner)->Damage(this->damage, hit_direction.components, 28);
    }
    impact_kind = 12;
    endpoint.components[0] = shared_ray.hit_pos.words[0];
    endpoint.components[1] = shared_ray.hit_pos.words[1];
    endpoint.components[2] = shared_ray.hit_pos.words[2];
    if (!RandomRange(4))
    {
        SubtractVectors(ricochet_delta.words, endpoint.words, muzzle_pos.words);
        sint32 normal_x = shared_ray.n.vx;
        sint32 normal_y = shared_ray.n.vy;
        sint32 normal_z = shared_ray.n.vz;
        normal_dot = (sint32)((uint32)(ricochet_delta.components[0] >> 6) * (uint32)normal_x + (uint32)(ricochet_delta.components[1] >> 6) * (uint32)normal_y + (uint32)(ricochet_delta.components[2] >> 6) * (uint32)normal_z);
        scaled_dot = normal_dot >> 12;
        if (normal_dot <= 0)
        {
            ricochet_delta.components[0] = (sint32)((uint32)ricochet_delta.components[0] - (uint32)((sint32)((uint32)scaled_dot * (uint32)normal_x) >> 6));
            ricochet_delta.components[2] = (sint32)((uint32)ricochet_delta.components[2] - (uint32)((sint32)((uint32)scaled_dot * (uint32)normal_z) >> 6));
            delta_length = MeasureVectorLength(ricochet_delta.words);
            if (delta_length >= 201)
            {
                bounce_distance = 400;
                MultiplyVector(scaled_delta.words, ricochet_delta.words, (uint32)bounce_distance);
                DivideVector(bounce_step.words, scaled_delta.words, (uint32)delta_length);
                AddVectors(ricochet_end.words, endpoint.words, bounce_step.words);
                ricochet_allocation = AllocateEffectStorage(108);
                if (ricochet_allocation)
                    new (ricochet_allocation) ProjectileTracer(endpoint.words, ricochet_end.words);
            }
        }
    }
    if (RandomRange(4))
        goto ResolveObjectHit;
    impact_sound_index = RandomRange(3);
    impact_sound = 27 + impact_sound_index;
    PlayPositionedSound(impact_sound, endpoint.words, 0);
ResolveObjectHit:
    if (object_hit)
    {
        impact_kind = 4;
        static_cast<VisualObject *>(hit_object->owner)->Damage(this->damage, hit_direction.components, 28);
        if (RandomRange(2))
            impact_kind = 20;
        endpoint = collision_position;
    }

    if (impact_kind)
        CreateProjectileImpact(impact_kind, endpoint.words);
    trace_allocation = AllocateEffectStorage(108);
    if (trace_allocation)
        new (trace_allocation) ProjectileTracer(muzzle_pos.words, endpoint.words);
    fired_at = game_clock.tick;
    this->fire_active = 1;
    this->last_fire_tick = fired_at;
    return 1;
}

uint32 MachineGun::Update(const uint32 *position, const sint16 *)
{
    FUNCTION_MARKER(0x80023380, "SLUS_003.73");
    EffectObject *effect = child.Get();
    uint32 result, sound;
    if (effect)
    {
        effect->SetPosition(position);
        uint32 now = game_clock.tick;
        uint32 created_at = this->child_tick;
        if (now - created_at >= 31)
            ClearChild();
    }
    result = this->fire_active;
    if (result)
    {
        sound = this->sound;
        this->fire_active = 0;
        if (sound)
            StopSoundVoices(sound);
        uint32 now = game_clock.tick;
        uint32 started_at = this->start_tick;
        result = now - started_at < 46;
        if (!result)
        {
            GameHeapBlock *allocation = AllocateEffectStorage(96);
            SmokeEmitter *smoke = 0;
            if (allocation)
            {
                smoke = new (allocation) SmokeEmitter(position, (uint32)-1, 1, 100, 100, 100, 3, 8, 188, 188);
            }
            child = smoke;
            if (!child)
            {
                GameDiagnostic("Machine gun smoke allocation failed\n");
                abort();
            }
            child->persistent = 1;
            result = game_clock.tick;
            this->child_tick = result;
        }
    }
    return result;
}

uint32 MachineGun::ReadLight(uint8 *red, uint8 *green, uint8 *blue)
{
    FUNCTION_MARKER(0x8002349C, "SLUS_003.73");
    if (!this->light_pending)
        return 0;
    *red = 255;
    *green = 0;
    *blue = 0;
    this->light_pending = 0;
    return 1;
}
