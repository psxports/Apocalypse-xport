#include <new>

#include "collision_ray.h"
#include "weapon_type.h"
#include "weapon_node.h"
#include "game_heap.h"
#include "game_tick.h"
#include "object_initializers.h"
#include "effect_object.h"
#include "primitive_particle.h"
#include "spiral_ray_weapon.h"
#include "visual_object.h"
#include "projectile_line.h"
#include "ring_burst_effect.h"
#include "fading_particle.h"
#include "angle_math.h"
#include "global.h"
#include "object_damage.h"
#include "collision.h"
#include "audio_session.h"
#include "effect_lifecycle.h"

SpiralRayWeapon::SpiralRayWeapon(ModelInstance **target) : WeaponNode(target, 0), pitch_spread(0), yaw_spread(0), fire_rate(0), range(0), sound_id(0), flash_scale(0), light_pending(false)
{
    FUNCTION_MARKER(0x80023B2C, "SLUS_003.73");
    type = WPN_SPIRAL_RAY;
    Configure(0, 0, 1, 6000, 20, 256);
}

uint32 SpiralRayWeapon::Destroy(uint32 reason)
{
    uint32 result;
    FUNCTION_MARKER(0x80023B98, "SLUS_003.73");
    result = WeaponNode::Destroy(0);
    if (reason & 1)
        result = ReleaseStorage();
    return result;
}

void SpiralRayWeapon::Configure(uint32 pitch_spread, uint32 yaw_spread, uint8 fire_rate, uint32 range, sint32 sound_id, uint32 flash_scale)
{
    FUNCTION_MARKER(0x80023BEC, "SLUS_003.73");
    this->pitch_spread = pitch_spread;
    this->yaw_spread = yaw_spread;
    this->fire_rate = fire_rate;
    this->range = range;
    this->sound_id = sound_id;
    this->flash_scale = flash_scale;
}

uint32 SpiralRayWeapon::Fire(const Vector3 &input_pos, const ShortVector3 &input_angles)
{
    CollisionRay &shared_ray = CollisionRay::GetShared();
    FUNCTION_MARKER(0x80023C14, "SLUS_003.73");
    ModelBorrow model_borrow;
    muzzle_pos = input_pos;
    sint32 fire_interval = DivideSignedWord(30, fire_rate);
    sint32 elapsed = game_clock.tick - last_fire_tick;
    if (elapsed < fire_interval)
    {
        fire_active = 1;
        return 0;
    }
    sound = PlaySoundEffect(sound_id, 8192, 0);
    light_pending = true;
    if (flash_scale)
    {
        GameHeapBlock *child = EffectObject::AllocateStorage(124);
        if (child)
        {
            uint32 extent = (sint32)(20 * flash_scale) / 256;
            sint16 odd_min = (sint32)(14 * flash_scale) / 256;
            sint16 even_min = (sint32)(10 * flash_scale) / 256;
            sint16 even_range = (sint32)(20 * flash_scale) / 256;
            new (child) RingBurstEffect(&muzzle_pos, 5, 255, 235, 0, 2, 90, 10, 0, 20, extent, 0, 0, odd_min, 0, even_min, even_range, 0, 2);
        }
    }
    OffsetByAngles(muzzle_pos, input_angles);
    ShortVector3 angles = input_angles;
    uint32 spread = pitch_spread;
    uint32 random = RandomRange((spread << 1) | 1);
    uint16 half_spread = pitch_spread;
    angles.x = (uint16)angles.x + (random - half_spread);
    spread = yaw_spread;
    random = RandomRange((spread << 1) | 1);
    half_spread = yaw_spread;
    angles.y = (uint16)angles.y + (random - half_spread);
    angles.NormalizeAngles();
    Vector3 dir, endpoint, tmp, side, scaled;
    endpoint.SetDir(range, angles);
    dir.Divide(endpoint, range);
    endpoint.Add(muzzle_pos);
    shared_ray.endpoints[0] = muzzle_pos;
    shared_ray.endpoints[1] = endpoint;
    shared_ray.Initialize();
    col_query.reject_surface40 = true;
    shared_ray.Query(1);
    col_query.reject_surface40 = false;
    ModelInstance *target = shared_ray.hit.GetObject();
    if (target)
    {
        endpoint = shared_ray.hit_pos;
        DamageLevelObject(target, shared_ray.hit.GetPolygon(), damage);
        target = shared_ray.hit.GetObject();
        if (target && (target->flags & 16))
            static_cast<VisualObject *>(target->owner)->Damage(damage, &dir, 28);
        target = CollisionRay::IntersectSpheres(muzzle_pos, endpoint, &endpoint, *owner_list, 0);
        if (target)
            static_cast<VisualObject *>(target->owner)->Damage(damage, &dir, 28);
        CreateProjectileImpact(4, &endpoint);
    }
    else
    {
        target = CollisionRay::IntersectSpheres(muzzle_pos, endpoint, &endpoint, *owner_list, 0);
        if (target)
        {
            static_cast<VisualObject *>(target->owner)->Damage(damage, &dir, 28);
            CreateProjectileImpact(4, &endpoint);
        }
    }
    Vector3 ring[16];
    for (uint32 idx = 0; idx < 16; ++idx)
    {
        Vector3 *point = ring + idx;
        uint32 angle = idx << 8;
        point->x = 48 * rsin(angle);
        point->z = 0;
        point->y = -48 * rcos(angle);
        if (angles.x)
        {
            angle = (uint16)angles.x & 4095;
            point->y = point->y * rcos(angle) >> 12;
            point->z = -point->y * rsin(angle) >> 12;
        }
        if (angles.y)
        {
            angle = (uint16)angles.y & 4095;
            sint32 x = point->x;
            point->x = (x * rcos(angle) + point->z * rsin(angle)) >> 12;
            point->z = (x * rsin(angle) - point->z * rcos(angle)) >> 12;
        }
    }
    ShortVector3 effect_angles;
    effect_angles.x = -1024;
    effect_angles.y = effect_angles.z = 0;
    PrimitiveParticle::SetAngles(&effect_angles);
    PrimitiveParticle::SetSpread(&zero_angles);
    PrimitiveParticle::SetSize(2);
    PrimitiveParticle::SetColor(32, 32, 128);
    PrimitiveParticle::SetColorStep(2, 2, 4);
    use_small_effect_pool = false;
    tmp.Subtract(endpoint, muzzle_pos);
    sint32 distance = tmp.Len();
    uint32 ring_idx = 0;
    for (sint32 offset = 0; offset < distance; offset += 64)
    {
        side.Add(muzzle_pos, ring[ring_idx]);
        scaled.Multiply(dir, offset);
        tmp.Add(side, scaled);
        GameHeapBlock *child = EffectObject::AllocateStorage(88);
        if (child)
        {
            sint32 accel = RandomRange(1024) + 2048;
            new (child) FadingParticle(&tmp, 0, -accel, 16);
        }
        ring_idx = (ring_idx + 2) & 15;
    }
    PrimitiveParticle::SetColor(64, 64, 64);
    PrimitiveParticle::SetColorStep(4, 4, 4);
    for (sint32 offset = 0; offset < distance; offset += 48)
    {
        side.Multiply(dir, offset);
        tmp.Add(muzzle_pos, side);
        GameHeapBlock *child = EffectObject::AllocateStorage(88);
        if (child)
            new (child) FadingParticle(&tmp, 0, 0, 12);
    }
    uint32 frame = game_clock.tick;
    use_small_effect_pool = true;
    fire_active = 1;
    last_fire_tick = frame;
    return 1;
}

uint32 SpiralRayWeapon::Update(const Vector3 *)
{
    uint32 state;
    FUNCTION_MARKER(0x800243DC, "SLUS_003.73");
    state = fire_active;
    if (state)
        fire_active = 0;
    return state;
}

uint32 SpiralRayWeapon::ReadLight(uint8 *r, uint8 *g, uint8 *b)
{
    FUNCTION_MARKER(0x800243F8, "SLUS_003.73");
    if (!light_pending)
        return 0;
    *r = 0;
    *g = 0;
    *b = 255;
    light_pending = false;
    return 1;
}
