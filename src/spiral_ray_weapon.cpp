#include <new>
#include <string.h>

#include "game_heap.h"
#include "game_tick.h"

#include "object_initializers.h"

#include "particle_settings.h"
#include "game_diagnostics.h"
#include "spiral_ray_weapon.h"
#include "visual_object.h"
#include "projectile_line.h"
#include "ring_burst_effect.h"
#include "fading_particle.h"
#include "angle_math.h"
#include "vector_arithmetic.h"
#include "vector_distance.h"
#include "object_damage.h"
#include "collision.h"
#include "audio_session.h"
#include "game_random.h"
#include "effect_lifecycle.h"

SpiralRayWeapon::SpiralRayWeapon(ModelInstance **target) : WeaponNode(target, 0), pitch_spread(0), yaw_spread(0), fire_rate(0), range(0), sound_id(0), flash_scale(0), light_pending(0)
{
    FUNCTION_MARKER(0x80023B2C, "SLUS_003.73");
    this->type = WPN_SPIRAL_RAY;
    Configure(0, 0, 1, 6000, 20, 256);
}

uint32 SpiralRayWeapon::Destroy(uint32 reason)
{
    uint32 result;
    FUNCTION_MARKER(0x80023B98, "SLUS_003.73");
    result = WeaponNode::Destroy(0);
    if (reason & 1)
        result = ReleaseWeaponStorage(this);
    return result;
}

void SpiralRayWeapon::Configure(uint32 pitch_spread, uint32 yaw_spread, uint32 fire_rate, uint32 range, uint32 sound_id, uint32 flash_scale)
{
    FUNCTION_MARKER(0x80023BEC, "SLUS_003.73");
    this->pitch_spread = pitch_spread;
    this->yaw_spread = yaw_spread;
    this->fire_rate = fire_rate;
    this->range = range;
    this->sound_id = sound_id;
    this->flash_scale = flash_scale;
}

uint32 SpiralRayWeapon::Update(const uint32 *, const sint16 *)
{
    uint32 state;
    FUNCTION_MARKER(0x800243DC, "SLUS_003.73");
    state = this->fire_active;
    if (state)
        this->fire_active = 0;
    return state;
}

uint32 SpiralRayWeapon::ReadLight(uint8 *red, uint8 *green, uint8 *blue)
{
    FUNCTION_MARKER(0x800243F8, "SLUS_003.73");
    if (!this->light_pending)
        return 0;
    *red = 0;
    *green = 0;
    *blue = 255;
    this->light_pending = 0;
    return 1;
}

uint32 SpiralRayWeapon::DivideBy256(uint32 value)
{
    if ((sint32)value < 0)
        value += 255;
    return (uint32)((sint32)value >> 8);
}

uint32 SpiralRayWeapon::Fire(const uint32 *input_position, const sint16 *input_angles)
{
    uint32 trig_angle;
    CollisionRay &shared_ray = GetSharedCollisionRay();
    FUNCTION_MARKER(0x80023C14, "SLUS_003.73");
    ModelBorrow model_borrow;
    uint32 x;
    uint32 y;
    uint32 z;
    uint32 enabled;
    uint32 previous;
    uint32 frame;
    uint32 sound;
    uint32 radius;
    GameHeapBlock *child;
    uint32 scale20, scale14, scale10, random, spread, damage;
    ModelInstance *target;
    ModelInstance **filter;
    uint32 distance, offset, ring_index, index, cosine, sine, old_x;
    Vector3 ring[16];
    Vector3 direction, endpoint, tmp, side, scaled;
    ShortVector3 angles, effect_angles;
    memcpy(muzzle_pos.words, input_position, sizeof(muzzle_pos.words));
    enabled = this->fire_rate;
    previous = this->last_fire_tick;
    frame = game_clock.tick;
    if ((sint32)(frame - previous) < (enabled ? 30 / (sint32)enabled : -1))
    {
        this->fire_active = 1;
        return 0;
    }
    sound = PlaySoundEffect((sint32)this->sound_id, 8192, 0);
    radius = this->flash_scale;
    this->sound = sound;
    this->light_pending = 1;
    if (radius)
    {
        child = AllocateEffectStorage(124);
        if (child)
        {
            scale20 = SpiralRayWeapon::DivideBy256(20 * this->flash_scale);
            scale14 = SpiralRayWeapon::DivideBy256(14 * this->flash_scale);
            scale10 = SpiralRayWeapon::DivideBy256(10 * this->flash_scale);
            radius = SpiralRayWeapon::DivideBy256(20 * this->flash_scale);
            new (child) RingBurstEffect(muzzle_pos.words, 5, 255, 235, 0, 2, 90, 10, 0, 20, scale20, 0, 0, scale14, 0, scale10, radius, 0, 2);
        }
    }
    OffsetPositionByAngles(muzzle_pos.words, (const uint16 *)input_angles);
    memcpy(angles.components, input_angles, sizeof(angles.components));
    spread = this->pitch_spread;
    random = RandomRange((spread << 1) | 1);
    spread = (uint16)this->pitch_spread;
    angles.components[0] = (sint16)((uint32)(uint16)angles.components[0] + random - spread);
    spread = this->yaw_spread;
    random = RandomRange((spread << 1) | 1);
    spread = (uint16)this->yaw_spread;
    angles.components[1] = (sint16)((uint32)(uint16)angles.components[1] + random - spread);
    NormalizeAngles(angles.words);
    BuildDirectionVector(endpoint.words, this->range, angles.words);
    DivideVector(direction.words, endpoint.words, this->range);
    AddVectors(endpoint.words, muzzle_pos.words);
    shared_ray.endpoints[0].words[0] = (muzzle_pos.words[0]);
    shared_ray.endpoints[0].words[1] = (muzzle_pos.words[1]);
    z = muzzle_pos.words[2];
    shared_ray.endpoints[1].words[0] = (endpoint.words[0]);
    shared_ray.endpoints[1].words[1] = (endpoint.words[1]);
    shared_ray.endpoints[1].words[2] = (endpoint.words[2]);
    shared_ray.endpoints[0].words[2] = (z);
    InitializeCollisionRay(shared_ray);
    collision_query.reject_surface40 = true;
    QueryCollisionRay(shared_ray, 1);
    collision_query.reject_surface40 = false;
    target = shared_ray.hit.GetObject();
    if (target)
    {
        endpoint.words[0] = shared_ray.hit_pos.words[0];

        endpoint.words[1] = shared_ray.hit_pos.words[1];
        endpoint.words[2] = shared_ray.hit_pos.words[2];
        damage = this->damage;
        DamageLevelObject(target, shared_ray.hit.GetPolygon(), (sint32)damage);
        target = shared_ray.hit.GetObject();
        if (target && (target->flags & 16))
        {
            damage = this->damage;
            static_cast<VisualObject *>(target->owner)->Damage(damage, direction.components, 28);
        }
        filter = this->owner_list;
        target = IntersectObjectSpheres(muzzle_pos.words, endpoint.words, endpoint.words, *filter, 0);
        if (target)
        {
            damage = this->damage;
            static_cast<VisualObject *>(target->owner)->Damage(damage, direction.components, 28);
        }
        CreateProjectileImpact(4, endpoint.words);
    }
    else
    {
        filter = this->owner_list;
        target = IntersectObjectSpheres(muzzle_pos.words, endpoint.words, endpoint.words, *filter, 0);
        if (target)
        {
            damage = this->damage;
            static_cast<VisualObject *>(target->owner)->Damage(damage, direction.components, 28);
            CreateProjectileImpact(4, endpoint.words);
        }
    }
    /* The original delay loop has no memory or call side effects */
    for (index = 0; index < 16; ++index)
    {
        trig_angle = (index << 8) & 0xF00;
        ring[index].words[0] = 48 * (uint32)rsin(trig_angle);
        ring[index].words[2] = 0;
        ring[index].words[1] = 0 - 48 * (uint32)rcos(trig_angle);
        if (angles.components[0])
        {
            trig_angle = (uint16)angles.components[0] & 4095;
            cosine = (uint32)rcos(trig_angle);
            ring[index].words[1] = (uint32)((sint32)(ring[index].words[1] * cosine) >> 12);
            sine = (uint32)rsin(trig_angle);
            ring[index].words[2] = (uint32)((sint32)((0 - ring[index].words[1]) * sine) >> 12);
        }
        if (angles.components[1])
        {
            trig_angle = (uint16)angles.components[1] & 4095;
            old_x = ring[index].words[0];
            cosine = (uint32)rcos(trig_angle);
            sine = (uint32)rsin(trig_angle);
            ring[index].words[0] = (uint32)((sint32)(old_x * cosine + ring[index].words[2] * sine) >> 12);
            sine = (uint32)rsin(trig_angle);
            cosine = (uint32)rcos(trig_angle);
            ring[index].words[2] = (uint32)((sint32)(old_x * sine - ring[index].words[2] * cosine) >> 12);
        }
    }
    effect_angles.components[0] = -1024;
    effect_angles.components[1] = effect_angles.components[2] = 0;
    SetParticleAngles(effect_angles.components);
    SetParticleSpread(zero_angles.components);
    SetParticleSize(2);
    SetParticleColor(32, 32, 128);
    SetParticleColorStep(2, 2, 4);
    use_small_effect_pool = false;
    SubtractVectors(tmp.words, endpoint.words, muzzle_pos.words);
    distance = MeasureVectorLength(tmp.words);
    ring_index = 0;
    for (offset = 0; (sint32)offset < (sint32)distance; offset += 64)
    {
        AddVectors(side.words, muzzle_pos.words, ring[ring_index].words);
        MultiplyVector(scaled.words, direction.words, offset);
        AddVectors(tmp.words, side.words, scaled.words);
        child = AllocateEffectStorage(88);
        if (child)
        {
            random = RandomRange(1024);
            new (child) FadingParticle(tmp.words, 0, 0 - (random + 2048), 16);
        }
        ring_index = (ring_index + 2) & 15;
    }
    SetParticleColor(64, 64, 64);
    SetParticleColorStep(4, 4, 4);
    for (offset = 0; (sint32)offset < (sint32)distance; offset += 48)
    {
        MultiplyVector(side.words, direction.words, offset);
        AddVectors(tmp.words, muzzle_pos.words, side.words);
        child = AllocateEffectStorage(88);
        if (child)
        {
            new (child) FadingParticle(tmp.words, 0, 0, 12);
        }
    }
    frame = game_clock.tick;
    use_small_effect_pool = true;
    this->fire_active = 1;
    this->last_fire_tick = frame;
    return 1;
}
