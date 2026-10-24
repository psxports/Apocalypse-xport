#include <new>
#include <stdlib.h>

#include "collision_ray.h"
#include "weapon_type.h"
#include "weapon_node.h"
#include "game_heap.h"
#include "game_tick.h"
#include "game_diagnostics.h"
#include "effect_object.h"
#include "machine_gun.h"
#include "visual_object.h"
#include "projectile_line.h"
#include "projectile_tracer.h"
#include "ring_burst_effect.h"
#include "bouncing_point_effect.h"
#include "smoke_emitter.h"
#include "audio_voice.h"
#include "angle_math.h"
#include "audio_session.h"
#include "object_damage.h"
#include "collision.h"
#include "global.h"
#include "effect_lifecycle.h"

MachineGun::MachineGun(ModelInstance **list) : WeaponNode(list, 0), pitch_spread(0), yaw_spread(0), muzzle_effect(false), fire_rate(0), range(0), sound_id(0), flash_scale(0), light_pending(false)
{
    FUNCTION_MARKER(0x80022B8C, "SLUS_003.73");
    type = WPN_MACHINE_GUN;
    Configure(10, 32, 10, 8000, 1, 11, 256);
}

uint32 MachineGun::Destroy(uint32 reason)
{
    FUNCTION_MARKER(0x80022C04, "SLUS_003.73");
    uint32 result = WeaponNode::Destroy(0);
    return (reason & 1) ? ReleaseStorage() : result;
}

void MachineGun::Configure(uint32 pitch_spread, uint32 yaw_spread, uint8 fire_rate, uint32 range, uint8 muzzle_effect, sint32 sound_id, uint32 flash_scale)
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

uint32 MachineGun::Fire(const Vector3 &input_pos, const ShortVector3 &input_angles)
{
    CollisionRay &shared_ray = CollisionRay::GetShared();
    FUNCTION_MARKER(0x80022C90, "SLUS_003.73");
    ModelBorrow model_borrow;
    sint32 sound_id;
    GameHeapBlock *flash_allocation;
    bool emit_muzzle_particle;
    GameHeapBlock *particle_allocation;
    bool geometry_hit;
    bool object_hit;
    ModelInstance *hit_object;
    ModelInstance **target_list;
    uint32 impact_kind;
    sint32 normal_dot;
    sint32 scaled_dot;
    GameHeapBlock *ricochet_allocation;
    sint32 impact_sound;
    GameHeapBlock *trace_allocation;

    ShortVector3 rotation;

    Vector3 endpoint, hit_dir, col_pos, near_ray_start, ricochet_delta, ricochet_end, bounce_step, scaled_delta;
    uint32 bounce_distance;
    sint32 delta_len;
    muzzle_pos = input_pos;
    if (!fire_active)
    {
        ClearChild();
        sound_id = this->sound_id;
        start_tick = game_clock.tick;
        sound = PlaySoundEffect(sound_id, 8192, 0);
    }
    if (flash_scale)
    {
        flash_allocation = EffectObject::AllocateStorage(124);
        if (flash_allocation)
        {
            uint32 extent = (sint32)(20 * flash_scale) / 256;
            sint16 odd_min = (sint32)(14 * flash_scale) / 256;
            sint16 even_min = (sint32)(10 * flash_scale) / 256;
            sint16 even_range = (sint32)(20 * flash_scale) / 256;
            RingBurstEffect *flash = new (flash_allocation) RingBurstEffect(&muzzle_pos, 5, 255, 235, 0, 2, 90, 10, 0, 20, extent, 0, 0, odd_min, 0, even_min, even_range, 0, 1);
            flash->hd_attachment_owner = hd_attachment_owner;
            flash->hd_attachment_origin = muzzle_pos;
        }
    }
    if (fire_active)
    {
        sint32 fire_interval = DivideSignedWord(30, fire_rate);
        uint32 last_fire = last_fire_tick;
        uint32 now = game_clock.tick;
        if ((sint32)(now - last_fire) < fire_interval)
        {
            fire_active = 1;
            return 0;
        }
    }
    emit_muzzle_particle = muzzle_effect;
    light_pending = true;
    if (emit_muzzle_particle)
    {
        use_small_effect_pool = false;
        particle_allocation = EffectObject::AllocateStorage(116);
        if (particle_allocation)
        {
            new (particle_allocation) BouncingPointEffect(&muzzle_pos, &input_angles);
        }
        use_small_effect_pool = true;
    }
    rotation = input_angles;
    {
        uint32 range = pitch_spread;
        uint32 random = RandomRange((range << 1) | 1);
        uint16 spread = pitch_spread;
        uint16 angle = rotation.x;
        rotation.x = angle + random - spread;
        range = yaw_spread;
        random = RandomRange((range << 1) | 1);
        spread = yaw_spread;
        angle = rotation.y;
        rotation.y = angle + random - spread;
    }
    rotation.NormalizeAngles();
    endpoint.SetDir(this->range, rotation);
    hit_dir.Divide(endpoint, this->range);
    endpoint.Add(muzzle_pos);
    rotation.z = 0;
    rotation.x = 0;
    near_ray_start.SetDir(-150, rotation);
    near_ray_start.Add(muzzle_pos);
    shared_ray.endpoints[0] = near_ray_start;
    geometry_hit = false;
    shared_ray.endpoints[1] = muzzle_pos;
    object_hit = false;
    shared_ray.Initialize();
    col_query.reject_surface40 = true;
    shared_ray.Query(1);
    col_query.reject_surface40 = false;
    hit_object = 0;
    if (shared_ray.hit.GetObject())
        goto HitGeometry;
    shared_ray.endpoints[0] = muzzle_pos;
    shared_ray.endpoints[1] = endpoint;
    shared_ray.Initialize();
    col_query.reject_surface40 = true;
    shared_ray.Query(1);
    target_list = owner_list;
    col_query.reject_surface40 = false;
    hit_object = CollisionRay::IntersectSpheres(muzzle_pos, endpoint, &col_pos, *target_list, 0);
    if (!shared_ray.hit.GetObject())
    {
        if (!hit_object)
            goto ResolveHit;
        goto HitObject;
    }
    if (hit_object)
    {
        {
            uint32 object_distance = hit_object->pos.EuclideanDistance(muzzle_pos);
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
        sint32 damage = this->damage;
        DamageLevelObject(level_object, polygon, damage);
    }
    {
        ModelInstance *geometry_object = shared_ray.hit.GetObject();
        if (geometry_object && (geometry_object->flags & 16))
            static_cast<VisualObject *>(geometry_object->owner)->Damage(this->damage, &hit_dir, 28);
    }
    impact_kind = 12;
    endpoint = shared_ray.hit_pos;
    if (!RandomRange(4))
    {
        ricochet_delta.Subtract(endpoint, muzzle_pos);
        sint16 normal_x = shared_ray.n.vx;
        sint16 normal_y = shared_ray.n.vy;
        sint16 normal_z = shared_ray.n.vz;
        // Keep low product words before the signed Q12 dot shift
        normal_dot = (uint32)(ricochet_delta.x >> 6) * (uint32)normal_x + (uint32)(ricochet_delta.y >> 6) * (uint32)normal_y + (uint32)(ricochet_delta.z >> 6) * (uint32)normal_z;
        scaled_dot = normal_dot >> 12;
        if (normal_dot <= 0)
        {
            ricochet_delta.x = (uint32)ricochet_delta.x - ((sint32)((uint32)scaled_dot * (uint32)normal_x) >> 6);
            ricochet_delta.z = (uint32)ricochet_delta.z - ((sint32)((uint32)scaled_dot * (uint32)normal_z) >> 6);
            delta_len = ricochet_delta.Len();
            if (delta_len >= 201)
            {
                bounce_distance = 400;
                scaled_delta.Multiply(ricochet_delta, bounce_distance);
                bounce_step.Divide(scaled_delta, delta_len);
                ricochet_end.Add(endpoint, bounce_step);
                ricochet_allocation = EffectObject::AllocateStorage(108);
                if (ricochet_allocation)
                    new (ricochet_allocation) ProjectileTracer(&endpoint, &ricochet_end);
            }
        }
    }
    if (RandomRange(4))
        goto ResolveObjectHit;
    impact_sound = 27 + RandomRange(3);
    PlayPositionedSound(impact_sound, &endpoint, 0);
ResolveObjectHit:
    if (object_hit)
    {
        impact_kind = 4;
        static_cast<VisualObject *>(hit_object->owner)->Damage(this->damage, &hit_dir, 28);
        if (RandomRange(2))
            impact_kind = 20;
        endpoint = col_pos;
    }

    if (impact_kind)
        CreateProjectileImpact(impact_kind, &endpoint);
    trace_allocation = EffectObject::AllocateStorage(108);
    if (trace_allocation)
        new (trace_allocation) ProjectileTracer(&muzzle_pos, &endpoint);
    fire_active = 1;
    last_fire_tick = game_clock.tick;
    return 1;
}

uint32 MachineGun::Update(const Vector3 *pos)
{
    FUNCTION_MARKER(0x80023380, "SLUS_003.73");
    EffectObject *effect = child.Get();
    uint32 result, sound;
    if (effect)
    {
        effect->SetPos(pos);
        effect->hd_attachment_origin = *pos;
        uint32 now = game_clock.tick;
        uint32 created_at = child_tick;
        if (now - created_at >= 31)
            ClearChild();
    }
    result = fire_active;
    if (result)
    {
        sound = this->sound;
        fire_active = 0;
        if (sound)
            StopSoundVoices(sound);
        uint32 now = game_clock.tick;
        uint32 started_at = start_tick;
        result = now - started_at < 46;
        if (!result)
        {
            GameHeapBlock *allocation = EffectObject::AllocateStorage(96);
            SmokeEmitter *smoke = 0;
            if (allocation)
            {
                smoke = new (allocation) SmokeEmitter(pos, -1, 1, 100, 100, 100, 3, 8, 188, 188);
                smoke->hd_attachment_owner = hd_attachment_owner;
                smoke->hd_attachment_origin = *pos;
            }
            child = smoke;
            if (!child)
            {
                GameDiagnostic(("Machine gun smoke allocation failed\n"));
                abort();
            }
            child->persistent = true;
            result = game_clock.tick;
            child_tick = result;
        }
    }
    return result;
}

uint32 MachineGun::ReadLight(uint8 *r, uint8 *g, uint8 *b)
{
    FUNCTION_MARKER(0x8002349C, "SLUS_003.73");
    if (!light_pending)
        return 0;
    *r = 255;
    *g = 0;
    *b = 0;
    light_pending = false;
    return 1;
}
