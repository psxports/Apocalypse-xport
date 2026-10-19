#include <new>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "game_tuning.h"
#include "game_tick.h"

#include "object_initializers.h"

#include "effect_object.h"

#include "game_diagnostics.h"
#include "weapon_targeting.h"
#include "angle_math.h"
#include "game_object_allocation.h"
#include "projectile_tracking.h"
#include "projectile_line.h"
#include "projectile_launch.h"
#include "projectile_ballistic.h"

#include "weapon_nodes.h"
static uint32 FireInactiveWeapon();
#include "beam_segments.h"
#include "ring_burst_effect.h"
#include "vector_arithmetic.h"
#include "vector_distance.h"
#include "collision.h"
#include "object_damage.h"
#include "game_random.h"
#include "effect_lifecycle.h"
#include "game_heap.h"
#include "audio_voice.h"
#include "audio_session.h"

#include "spiral_ray_weapon.h"
#include "machine_gun.h"
#include "flamethrower.h"

static sint32 DivideBeamScalar(sint32 numerator, sint32 denominator)
{
    if (!denominator)
        return numerator < 0 ? 1 : -1;
    if ((uint32)numerator == 0x80000000 && denominator == -1)
        return numerator;
    return numerator / denominator;
}

void ParticleBeam::DamageTarget(ModelInstance *hit, const Vector3 &direction)
{
    static_cast<VisualObject *>(hit->owner)->Damage(damage, direction.components, 10);
}

void RipLaser::DamageTarget(ModelInstance *hit)
{
    static_cast<VisualObject *>(hit->owner)->Damage(damage, zero_vector.components, 11);
}

uint32 WeaponNode::Fire(const uint32 *position, const sint16 *angles)
{
    return FireInactiveWeapon();
}

sint32 ReadWeaponLightRandom()
{
    return randPSX();
}

WeaponNode::WeaponNode(ModelInstance **list, ModelInstance **secondary_list) : WeaponNodeBase(), type(0), fire_active(0), last_fire_tick(0), start_tick(0), child_tick(0), damage(0), owner_list(0), secondary_list(0), sound(0), ammo(0)
{
    FUNCTION_MARKER(0x80022A24, "SLUS_003.73");
    for (uint32 axis = 0; axis < 3; ++axis)
        muzzle_pos.words[axis] = 0;
    this->owner_list = list;
    this->secondary_list = secondary_list;
    this->ammo = 1000;
}

uint32 WeaponNode::Destroy(uint32 reason)
{
    uint32 sound, result;
    FUNCTION_MARKER(0x80022A48, "SLUS_003.73");
    ClearChild();
    sound = this->sound;
    if (sound)
        StopSoundVoices(sound);
    result = reason & 1;
    if (result)
        result = ReleaseWeaponStorage(this);
    return result;
}

void WeaponNode::ClearChild()
{
    FUNCTION_MARKER(0x80022ABC, "SLUS_003.73");
    if (child)
    {
        ObjectBorrow borrow;
        child->Destroy(3);
    }
    child = 0;
}

uint32 WeaponNode::ConsumeAmmo(uint32 amount)
{
    uint32 result;
    FUNCTION_MARKER(0x80022B34, "SLUS_003.73");
    result = this->ammo - amount;
    this->ammo = result;
    if ((sint32)result < 0)
        this->ammo = 0;
    return result;
}

HomingMissiles::HomingMissiles(ModelInstance **list) : WeaponNode(list, 0), fire_delay(0), fire_timer(0)
{
    FUNCTION_MARKER(0x80025070, "SLUS_003.73");
    this->type = WPN_HOMING_MISSILES;
    this->fire_delay = 15;
}

uint32 HomingMissiles::Destroy(uint32 reason)
{
    uint32 result;
    FUNCTION_MARKER(0x800250B8, "SLUS_003.73");
    result = WeaponNode::Destroy(0);
    if (reason & 1)
        result = ReleaseWeaponStorage(this);
    return result;
}

uint32 HomingMissiles::Fire(const uint32 *input_position, const sint16 *input_angles)
{
    uint32 timer;
    Vector3 position;
    GameHeapBlock *projectile;
    uint32 damage;
    uint32 duration;
    ModelInstance **list;
    FUNCTION_MARKER(0x8002510C, "SLUS_003.73");
    timer = this->fire_timer;
    if (timer)
    {
        this->fire_timer = timer - 1;
        return 0;
    }
    memcpy(position.words, input_position, sizeof(position.words));
    OffsetPositionByAngles(position.words, (const uint16 *)input_angles);
    projectile = AllocateZeroedObject(420);
    if (projectile)
    {
        damage = this->damage;
        list = this->owner_list;
        new (projectile) TrackingProjectile(position.words, input_angles, 1000, damage, list);
    }
    ConsumeAmmo(50);
    if ((sint32)this->ammo < 50)
        this->ammo = 0;
    duration = this->fire_delay;
    this->fire_timer = duration;
    return 1;
}

uint32 HomingMissiles::Update(const uint32 *, const sint16 *)
{
    uint32 timer, result;
    FUNCTION_MARKER(0x800251E8, "SLUS_003.73");
    timer = this->fire_timer;
    result = timer - 1;
    if (timer)
        this->fire_timer = result;
    return result;
}

uint32 HomingMissiles::ReadLight(uint8 *red, uint8 *green, uint8 *blue)
{
    uint32 timer;
    FUNCTION_MARKER(0x80025204, "SLUS_003.73");
    timer = this->fire_timer;
    if (!timer)
        return 0;
    if ((sint32)(this->fire_delay - timer) >= 5)
        return 0;
    *red = 255;
    *green = 255;
    *blue = 0;
    return 1;
}

PulseLaser::PulseLaser(ModelInstance **list) : WeaponNode(list, 0), fire_delay(0), fire_timer(0)
{
    FUNCTION_MARKER(0x800252CC, "SLUS_003.73");
    this->type = WPN_PULSE_LASER;
    this->fire_delay = 3;
}

uint32 PulseLaser::Destroy(uint32 reason)
{
    uint32 result;
    uint32 sound;
    FUNCTION_MARKER(0x80025314, "SLUS_003.73");
    sound = this->sound;
    if (sound)
        StopSoundVoices(sound);
    this->sound = 0;
    WeaponNode::Destroy(0);
    result = reason & 1;
    if (reason & 1)
        result = ReleaseWeaponStorage(this);
    return result;
}

uint32 PulseLaser::Fire(const uint32 *input_position, const sint16 *input_angles)
{
    uint32 sound;
    uint32 timer;
    GameHeapBlock *projectile;
    uint32 damage;
    GameHeapBlock *particle;
    uint32 duration;
    ModelInstance **owner;
    FUNCTION_MARKER(0x80025380, "SLUS_003.73");
    sound = this->sound;
    if (!sound)
    {
        sound = PlaySoundEffect(30, 8192, 0);
        this->sound = sound;
    }
    timer = this->fire_timer;
    if (timer)
    {
        this->fire_timer = timer - 1;
        return 0;
    }
    projectile = AllocateEffectStorage(268);
    if (projectile)
    {
        damage = this->damage;
        owner = this->owner_list;
        new (projectile) LineProjectile(input_position, input_angles, 500, damage, owner, 0, 130, 0, 1);
    }
    particle = AllocateEffectStorage(124);
    if (particle)
    {
        new (particle) RingBurstEffect(input_position, 5, 255, 255, 255, 2, 0, 100, 0, 20, 20, 0, 0, 14, 0, 10, 20, 0, 1);
    }
    ConsumeAmmo(20);
    if ((sint32)this->ammo < 20)
        this->ammo = 0;
    duration = this->fire_delay;
    this->fire_timer = duration;
    return 1;
}

uint32 PulseLaser::Update(const uint32 *, const sint16 *)
{
    uint32 timer, sound;
    FUNCTION_MARKER(0x800254E8, "SLUS_003.73");
    timer = this->fire_timer;
    if (timer)
        this->fire_timer = timer - 1;
    sound = this->sound;
    if (sound)
        StopSoundVoices(sound);
    this->sound = 0;
    return 0;
}

uint32 PulseLaser::ReadLight(uint8 *red, uint8 *green, uint8 *blue)
{
    uint32 timer;
    FUNCTION_MARKER(0x80025538, "SLUS_003.73");
    timer = this->fire_timer;
    if (!timer)
        return 0;
    if ((sint32)(this->fire_delay - timer) >= 3)
        return 0;
    *red = 0;
    *green = 255;
    *blue = 0;
    return 1;
}

ParticleBeam::ParticleBeam(ModelInstance **list, uint32 range, uint32 mode, uint32 effect_scale, uint32 sound) : WeaponNode(list, 0), impact_count(0), segment_len(0), trail_mode(0), ring_scale(0), sound_id(0), ring()
{
    FUNCTION_MARKER(0x80027720, "SLUS_003.73");
    this->type = WPN_PARTICLE_BEAM;
    this->trail_mode = mode;
    this->sound_id = sound;
    this->ring_scale = effect_scale;
    this->segment_len = (uint32)((sint32)range / 9);
    SetColors(255, 128, 0, 185, 185, 185, 160, 80, 0, 255, 255, 255);
}

ParticleBeam::~ParticleBeam()
{
    for (uint32 i = 0; i < 2; ++i)
        if (beam[i])
            beam[i]->Destroy(3);
    if (ring)
        ring->Destroy(3);
}

uint32 ParticleBeam::Destroy(uint32 reason)
{
    uint32 sound, result;
    FUNCTION_MARKER(0x80027804, "SLUS_003.73");
    sound = this->sound;
    if (sound)
        StopSoundVoices(sound);
    this->sound = 0;
    ClearChildren();
    WeaponNode::Destroy(0);
    result = reason & 1;
    if (result)
        result = ReleaseWeaponStorage(this);
    return result;
}

uint32 ParticleBeam::SetColors(uint32 red1, uint32 green1, uint32 blue1, uint32 red2, uint32 green2, uint32 blue2, uint32 red3, uint32 green3, uint32 blue3, uint32 red4, uint32 green4, uint32 blue4)
{
    FUNCTION_MARKER(0x80027878, "SLUS_003.73");
    this->colors[0].r = red1;
    this->colors[0].g = green1;
    this->colors[0].b = blue1;
    this->colors[1].r = red2;
    this->colors[1].g = green2;
    this->colors[1].b = blue2;
    this->colors[2].r = red3;
    this->colors[2].g = green3;
    this->colors[2].b = blue3;
    this->colors[3].r = red4;
    this->colors[3].g = green4;
    this->colors[3].b = blue4;
    return red2;
}

void ParticleBeam::ClearChildren()
{
    FUNCTION_MARKER(0x800278D0, "SLUS_003.73");
    for (uint32 i = 0; i < 2; ++i)
    {
        if (beam[i])
            beam[i]->Destroy(3);
        beam[i] = 0;
    }
    if (ring)
        ring->Destroy(3);
    ring = 0;
}

uint32 ParticleBeam::Fire(const uint32 *input_position, const sint16 *input_angles)
{
    ModelBorrow model_borrow;
    CollisionRay &shared_ray = GetSharedCollisionRay();
    FUNCTION_MARKER(0x80027978, "SLUS_003.73");
    Vector3 direction, normalized, position, endpoint, tmp, scaled, final, intersection;
    GameHeapBlock *child;
    uint32 count;
    uint32 index;
    BeamPoint *source, *destination, *left, *right;
    uint32 red, green, blue, value, timer, random, sine, step;
    ModelInstance *wall;
    ModelInstance *hit;
    ModelInstance **filter;
    uint32 object_hit = 0, wall_hit = 0, distance, other_distance, damage;
    ModelInstance *target;
    uint32 scalar, offset;
    sint32 segment;
    if (!this->sound)
    {
        value = PlaySoundEffect(this->sound_id, 8192, 0);
        this->sound = value;
    }
    ConsumeAmmo(4);
    value = this->segment_len;
    BuildDirectionVector(direction.words, value, (const uint16 *)input_angles);
    DivideVector(normalized.words, direction.words, this->segment_len);
    if (!beam[0] || !beam[1])
    {
        ClearChildren();
        child = AllocateEffectStorage(84);
        if (child)
        {
            beam[0] = new (child) BeamSegments(10, 1);
        }
        if (!beam[0])
        {
            GameDiagnostic("Weapon beam allocation failed\n");
            abort();
        }
        beam[0]->persistent = 1;
        red = this->colors[0].r;
        green = this->colors[0].g;
        blue = this->colors[0].b;
        beam[0]->SetColors(red, green, blue);
        destination = beam[0]->points;
        destination[9].size = 0;
        destination[8].rgbc.r >>= 1;
        destination[8].rgbc.g >>= 1;
        destination[8].rgbc.b >>= 1;
        destination[9].rgbc.r = 0;
        destination[9].rgbc.g = 0;
        destination[9].rgbc.b = 0;
        position.words[0] = input_position[0];
        position.words[1] = input_position[1];
        position.words[2] = input_position[2];
        for (index = 0; index < 10; ++index)
        {
            destination = beam[0]->points + index;
            destination->pos = position;
            AddVectors(position.words, direction.words);
        }
        child = AllocateEffectStorage(84);
        if (child)
        {
            beam[1] = new (child) BeamSegments(10, 0);
        }
        if (!beam[1])
        {
            GameDiagnostic("Weapon beam layer allocation failed\n");
            abort();
        }
        beam[1]->persistent = 1;
        red = this->colors[1].r;
        green = this->colors[1].g;
        blue = this->colors[1].b;
        beam[1]->SetColors(red, green, blue);
        beam[1]->SetSize(10);
        for (index = 0; index < 10; ++index)
        {
            destination = beam[1]->points + index;
            source = beam[0]->points + index;
            destination->pos = source->pos;
        }
        destination = beam[1]->points;
        destination[9].rgbc.r = 0;
        destination[9].rgbc.g = 0;
        destination[9].rgbc.b = 0;
        if (this->ring_scale)
        {
            child = AllocateEffectStorage(104);
            if (!child)
            {
                GameDiagnostic("Particle beam ring allocation failed\n");
                abort();
            }
            ring = new (child) RingEffect(7, 1);
            ring->persistent = 1;
            ring->SetCenterColor(colors[3].r, colors[3].g, colors[3].b);
            ring->SetRingColors(colors[2].r, colors[2].g, colors[2].b);
            ring->SetRowColors(0, 0, 0, 0);
            ring->SetRowExtent(0, (sint32)(30 * ring_scale) >> 8);
            ring->SetRadii(0);
            ring->SetPosition(muzzle_pos.words);
        }
    }
    step = this->trail_mode ? 10 : 2;
    count = beam[0]->point_count;
    segment = (sint32)(count - 1);
    while ((sint32)(step - 1) < segment)
    {
        destination = beam[1]->points + (uint32)segment;
        source = beam[0]->points + (uint32)segment;
        MultiplyVector(scaled.words, direction.words, step);
        offset = 36 * ((uint32)segment - step);
        left = beam[0]->points + offset / 36;
        AddVectors(tmp.words, left->pos.words, scaled.words);
        segment = (sint32)((uint32)segment - 1);
        source->pos = tmp;
        destination->pos = source->pos;
    }
    position.words[0] = input_position[0];
    position.words[1] = input_position[1];
    position.words[2] = input_position[2];
    for (index = 0; index < step; ++index)
    {
        source = beam[0]->points + index;
        destination = beam[1]->points + index;
        source->pos = position;
        destination->pos = source->pos;
        AddVectors(position.words, direction.words);
    }
    timer = 0;
    for (index = 0; index < 9; ++index)
    {
        random = RandomRange(256);
        destination = beam[1]->points + index;
        destination->rgbc.r = random;
        destination->rgbc.g = random;
        destination->rgbc.b = random;
        value = game_clock.tick;
        sine = (uint32)rsin((900 * value + timer) & 4095);
        destination = beam[0]->points + index;
        destination->size = (uint32)((sint32)(6 * sine) / 4096) + 19 + 6 * index;
        timer -= 1150;
    }
    if (ring)
    {
        scalar = (sint32)(20 * ring_scale) >> 8;
        for (index = 0; index < ring->point_count; ++index)
        {
            random = RandomRange(scalar);
            ring->ring_points[index].distance = scalar + random;
        }
        ring->angle = RandomRange(1024);
        ring->SetPosition(input_position);
    }
    scalar = 9;
    MultiplyScalarVector(scaled.words, scalar, direction.words);
    AddVectors(endpoint.words, input_position, scaled.words);
    tmp.words[0] = input_position[0];
    tmp.words[1] = input_position[1];
    tmp.words[2] = input_position[2];
    OffsetPositionByAngles(tmp.words, (const uint16 *)input_angles);
    shared_ray.endpoints[0] = tmp;
    shared_ray.endpoints[1] = endpoint;
    InitializeCollisionRay(shared_ray);
    collision_query.reject_surface40 = true;
    QueryCollisionRay(shared_ray, 1);
    position.words[0] = input_position[0];
    position.words[1] = input_position[1];
    position.words[2] = input_position[2];
    filter = this->owner_list;
    collision_query.reject_surface40 = false;
    hit = IntersectObjectSpheres(position.words, endpoint.words, intersection.words, *filter, 0);
    wall = shared_ray.hit.GetObject();
    if (wall)
    {
        endpoint.words[0] = shared_ray.hit_pos.words[0];
        endpoint.words[1] = shared_ray.hit_pos.words[1];
        endpoint.words[2] = shared_ray.hit_pos.words[2];
        if (!hit)
            wall_hit = 1;
        else
        {
            distance = MeasurePositionDistance(input_position, hit->pos.words);
            other_distance = MeasurePositionDistance(input_position, endpoint.words);
            if (distance >= other_distance)
                wall_hit = 1;
            else
                object_hit = 1;
        }
    }
    else if (hit)
        object_hit = 1;
    if (object_hit)
    {
        endpoint.words[0] = hit->pos.words[0];
        endpoint.words[1] = hit->pos.words[1];
        endpoint.words[2] = hit->pos.words[2];
        DamageTarget(hit, normalized);
    }
    if (wall_hit)
    {
        target = shared_ray.hit.GetObject();

        damage = this->damage;
        DamageLevelObject(target, shared_ray.hit.GetPolygon(), damage);
        target = shared_ray.hit.GetObject();
        if (target && (target->flags & 16))
            DamageTarget(target, normalized);
    }
    if (object_hit || wall_hit)
    {
        distance = MeasureEuclideanDistance(input_position, endpoint.words);
        segment = 0;
        if ((sint32)distance >= 101)
        {
            segment = DivideBeamScalar((sint32)distance, (sint32)this->segment_len);
            if (segment >= 10)
                segment = 9;
            source = beam[0]->points;
            offset = 36 * (uint32)segment;
            left = source + offset / 36 + (segment < 9 ? 1 : 0);
            right = source + offset / 36 - (segment < 9 ? 0 : 1);
            SubtractVectors(tmp.words, left->pos.words, right->pos.words);
            final = tmp;
            value = this->segment_len;
            scalar = distance - (uint32)segment * value;
            MultiplyVector(scaled.words, final.words, scalar);
            DivideVector(tmp.words, scaled.words, this->segment_len);
            final = tmp;
            source = beam[0]->points + (uint32)segment;
            AddVectors(final.words, source->pos.words);
        }
        else
        {
            final = endpoint;
        }
        for (index = (uint32)segment; (sint32)index < 10; ++index)
        {
            source = beam[0]->points + index;
            source->pos = final;
            destination = beam[1]->points + index;
            destination->pos = final;
        }
        value = this->impact_count + 1;
        this->impact_count = value;
        if (value & 1)
        {
            source = beam[0]->points + (uint32)segment;
            red = source->rgbc.r;
            green = source->rgbc.g;
            blue = source->rgbc.b;
            SpawnExplosionRing(final.words, 50, red, green, blue, 3, 1, 100);
        }
    }
    return 1;
}

uint32 ParticleBeam::Update(const uint32 *, const sint16 *)
{
    uint32 sound;
    FUNCTION_MARKER(0x800284A4, "SLUS_003.73");
    sound = this->sound;
    if (sound)
        StopSoundVoices(sound);
    this->sound = 0;
    ClearChildren();
    return 0;
}

uint32 ParticleBeam::ReadLight(uint8 *red, uint8 *green, uint8 *blue)
{
    FUNCTION_MARKER(0x800284E8, "SLUS_003.73");
    if (!beam[0])
        return 0;
    if (ReadWeaponLightRandom() >= 16384)
        return 0;
    *red = this->colors[0].r;
    *green = this->colors[0].g;
    *blue = this->colors[0].b;
    return 1;
}

RipLaser::RipLaser(ModelInstance **list) : WeaponNode(list, 0), retarget_timer(0), impact_active(0), impact_count(0)
{
    FUNCTION_MARKER(0x80028578, "SLUS_003.73");
    impact_pos.words[0] = impact_pos.words[1] = impact_pos.words[2] = 0;
    this->type = WPN_RIP_LASER;
    SetColors(64, 32, 128, 255, 255, 255);
}

RipLaser::~RipLaser()
{
    for (uint32 i = 0; i < 2; ++i)
        if (beam[i])
            beam[i]->Destroy(3);
}

uint32 RipLaser::Destroy(uint32 reason)
{
    uint32 result;
    uint32 sound;
    FUNCTION_MARKER(0x800285DC, "SLUS_003.73");
    sound = this->sound;
    if (sound)
        StopSoundVoices(sound);
    this->sound = 0;
    ClearChildren();
    WeaponNode::Destroy(0);
    result = reason & 1;
    if (reason & 1)
        result = ReleaseWeaponStorage(this);
    return result;
}

uint32 RipLaser::SetColors(uint32 first_red, uint32 first_green, uint32 first_blue, uint32 second_red, uint32 second_green, uint32 second_blue)
{
    FUNCTION_MARKER(0x80028650, "SLUS_003.73");
    this->colors[0].r = first_red;
    this->colors[0].g = first_green;
    this->colors[0].b = first_blue;
    this->colors[1].r = second_red;
    this->colors[1].g = second_green;
    this->colors[1].b = second_blue;
    return second_red;
}

void RipLaser::ClearChildren()
{
    FUNCTION_MARKER(0x80028678, "SLUS_003.73");
    for (uint32 i = 0; i < 2; ++i)
    {
        if (beam[i])
            beam[i]->Destroy(3);
        beam[i] = 0;
    }
}

uint32 RipLaser::Fire(const uint32 *input_position, const sint16 *input_angles)
{
    ModelBorrow model_borrow;
    CollisionRay &shared_ray = GetSharedCollisionRay();
    GameHeapBlock *child;
    uint32 red;
    uint32 green;
    uint32 blue;
    uint32 value;
    Vector3 position, endpoint;
    ModelInstance *hit;
    ModelInstance *head;
    ModelInstance *selected;
    uint32 offset, index, phase, sine, product;
    Vector3 step, delta, axis1, axis2;
    uint32 scalar, divisor = 9;
    Vector3 scaled1, scaled2, intermediate, final;
    BeamPoint *storage;
    ShortVector3 angles;
    FUNCTION_MARKER(0x800286F4, "SLUS_003.73");
    ConsumeAmmo(4);
    if (!this->sound)
        this->sound = PlaySoundEffect(9, 8192, 0);
    if (!beam[0] || !beam[1])
    {
        ClearChildren();
        child = AllocateEffectStorage(84);
        if (child)
        {
            beam[0] = new (child) BeamSegments(10, 1);
        }
        red = this->colors[0].r;
        green = this->colors[0].g;
        blue = this->colors[0].b;
        if (!beam[0])
        {
            GameDiagnostic("Weapon beam allocation failed\n");
            abort();
        }
        beam[0]->SetColors(red, green, blue);
        beam[0]->persistent = 1;
        child = AllocateEffectStorage(84);
        if (child)
        {
            beam[1] = new (child) BeamSegments(10, 0);
        }
        red = this->colors[1].r;
        green = this->colors[1].g;
        blue = this->colors[1].b;
        if (!beam[1])
        {
            GameDiagnostic("Weapon beam layer allocation failed\n");
            abort();
        }
        beam[1]->SetColors(red, green, blue);
        beam[1]->SetSize(10);
        beam[1]->persistent = 1;
    }
    value = this->retarget_timer;
    if (value)
        this->retarget_timer = value - 1;
    head = *this->owner_list;
    selected = SelectWeaponTarget(input_position, input_angles, head, 3000, 512, 0);
    if (selected)
    {
        position.words[0] = input_position[0];
        position.words[1] = input_position[1];
        position.words[2] = input_position[2];
        OffsetPositionByAngles(position.words, (const uint16 *)input_angles);
        shared_ray.endpoints[0].words[0] = (position.words[0]);
        shared_ray.endpoints[0].words[2] = (position.words[2]);
        shared_ray.endpoints[0].words[1] = (position.words[1]);
        shared_ray.endpoints[1].words[0] = (selected->pos.words[0]);
        shared_ray.endpoints[1].words[1] = (selected->pos.words[1]);
        shared_ray.endpoints[1].words[2] = (selected->pos.words[2]);
        InitializeCollisionRay(shared_ray);
        collision_query.reject_surface40 = true;
        QueryCollisionRay(shared_ray, 1);
        hit = shared_ray.hit.GetObject();
        collision_query.reject_surface40 = false;
        if (hit)
        {
            impact_pos.words[0] = shared_ray.hit_pos.words[0];
            impact_pos.words[1] = shared_ray.hit_pos.words[1];
            selected = 0;
            impact_pos.words[2] = shared_ray.hit_pos.words[2];
        }
        else
        {
            endpoint.words[0] = selected->pos.words[0];
            endpoint.words[1] = selected->pos.words[1];
            endpoint.words[2] = selected->pos.words[2];
            impact_pos.words[0] = endpoint.words[0];
            impact_pos.words[1] = endpoint.words[1];
            impact_pos.words[2] = endpoint.words[2];
        }
        this->impact_active = 1;
    }
    if (!selected && !this->retarget_timer)
    {
        if (RandomRange(2))
        {
            value = RandomRange(50);
            angles.components[0] = (sint16)((uint16)input_angles[0] - value);
        }
        else
        {
            value = RandomRange(40);
            angles.components[0] = (sint16)((uint16)input_angles[0] + value);
        }
        value = RandomRange(321);
        angles.components[2] = 0;
        angles.components[1] = (sint16)((uint16)input_angles[1] + value - 160);
        BuildDirectionVector(endpoint.words, 3000, angles.words);
        AddVectors(endpoint.words, input_position);
        position.words[0] = input_position[0];
        position.words[1] = input_position[1];
        position.words[2] = input_position[2];
        OffsetPositionByAngles(position.words, (const uint16 *)input_angles);
        shared_ray.endpoints[0].words[0] = (position.words[0]);
        shared_ray.endpoints[0].words[1] = (position.words[1]);
        shared_ray.endpoints[0].words[2] = (position.words[2]);
        shared_ray.endpoints[1].words[0] = (endpoint.words[0]);
        shared_ray.endpoints[1].words[1] = (endpoint.words[1]);
        shared_ray.endpoints[1].words[2] = (endpoint.words[2]);
        InitializeCollisionRay(shared_ray);
        collision_query.reject_surface40 = true;
        QueryCollisionRay(shared_ray, 1);
        hit = shared_ray.hit.GetObject();
        collision_query.reject_surface40 = false;
        if (hit)
        {
            impact_pos.words[0] = shared_ray.hit_pos.words[0];
            impact_pos.words[1] = shared_ray.hit_pos.words[1];
            value = shared_ray.hit_pos.words[2];
            this->impact_active = 1;
            impact_pos.words[2] = value;
        }
        else
        {
            impact_pos.words[0] = endpoint.words[0];
            impact_pos.words[1] = endpoint.words[1];
            impact_pos.words[2] = endpoint.words[2];
            this->impact_active = 0;
        }
        this->retarget_timer = RandomRange(3) + 2;
    }
    if (selected)
        DamageTarget(selected);
    hit = shared_ray.hit.GetObject();
    if (hit)
    {
        product = this->damage;
        DamageLevelObject(hit, shared_ray.hit.GetPolygon(), product);
        hit = shared_ray.hit.GetObject();
        if (hit && (hit->flags & 16))
            DamageTarget(hit);
    }
    phase = 0;
    offset = 0;
    for (index = 0; index < 10; ++index)
    {
        value = RandomRange(256);
        storage = beam[1]->points;
        storage[index].rgbc.r = value;
        storage = beam[1]->points;
        storage[index].rgbc.g = value;
        storage = beam[1]->points;
        storage[index].rgbc.b = value;
        value = game_clock.tick;
        sine = (uint32)rsin((900 * value + phase) & 4095);
        storage = beam[0]->points;
        product = sine * 6;
        storage[index].size = (uint32)((sint32)product / 4096) + 10 + index * 5;
        phase -= 1150;
        offset += 36;
    }
    position.words[0] = input_position[0];
    position.words[1] = input_position[1];
    position.words[2] = input_position[2];
    SubtractVectors(delta.words, impact_pos.words, input_position);
    DivideVector(step.words, delta.words, divisor);
    phase = 0;
    offset = 0;
    for (index = 0; index < 10; ++index)
    {
        angles.components[0] = (sint16)(uint16)input_angles[0];
        angles.components[1] = (sint16)((uint16)input_angles[1] + 1024);
        angles.components[2] = (sint16)(uint16)input_angles[2];
        BuildDirectionVector(axis1.words, 1, angles.words);
        angles.components[0] = (sint16)((uint16)input_angles[0] + 1024);
        angles.components[1] = (sint16)(uint16)input_angles[1];
        BuildDirectionVector(axis2.words, 1, angles.words);
        sine = (uint32)rsin(phase & 4095);
        product = (RandomRange(301) - 150) * sine;
        scalar = (uint32)((sint32)product / 4096);
        MultiplyVector(scaled1.words, axis1.words, scalar);
        AddVectors(intermediate.words, position.words, scaled1.words);
        product = (RandomRange(261) - 130) * sine;
        scalar = (uint32)((sint32)product / 4096);
        MultiplyVector(scaled2.words, axis2.words, scalar);
        AddVectors(final.words, intermediate.words, scaled2.words);
        storage = beam[0]->points + offset / 36;
        storage->pos.words[0] = final.words[0];
        storage->pos.words[1] = final.words[1];
        storage->pos.words[2] = final.words[2];
        storage = beam[1]->points + offset / 36;
        storage->pos.words[0] = final.words[0];
        storage->pos.words[1] = final.words[1];
        storage->pos.words[2] = final.words[2];
        AddVectors(position.words, step.words);
        phase += 227;
        offset += 36;
    }
    value = this->impact_count + 1;
    this->impact_count = value;
    if (value & 1)
    {
        child = AllocateEffectStorage(124);
        if (child)
        {
            red = this->colors[0].r;
            green = this->colors[0].g;
            blue = this->colors[0].b;
            new (child) RingBurstEffect(impact_pos.words, 5, 255, 255, 255, 1, red, green, blue, 1, 60, 0, 1, 50, 80, 25, 40, 10, 5);
        }
    }
    return 1;
}

uint32 RipLaser::Update(const uint32 *, const sint16 *)
{
    uint32 sound;
    FUNCTION_MARKER(0x80028EE4, "SLUS_003.73");
    sound = this->sound;
    if (sound)
        StopSoundVoices(sound);
    this->sound = 0;
    ClearChildren();
    return 0;
}

uint32 RipLaser::ReadLight(uint8 *red, uint8 *green, uint8 *blue)
{
    FUNCTION_MARKER(0x80028F28, "SLUS_003.73");
    if (!beam[0])
        return 0;
    if (ReadWeaponLightRandom() >= 16384)
        return 0;
    *red = this->colors[0].r;
    *green = this->colors[0].g;
    *blue = this->colors[0].b;
    return 1;
}

Grenades::Grenades(ModelInstance **list) : WeaponNode(list, 0), fire_delay(0), fire_timer(0)
{
    FUNCTION_MARKER(0x8002A5C0, "SLUS_003.73");
    this->type = WPN_GRENADES;
    this->fire_delay = 15;
}

uint32 Grenades::Destroy(uint32 reason)
{
    uint32 result;
    FUNCTION_MARKER(0x8002A608, "SLUS_003.73");
    result = WeaponNode::Destroy(0);
    if (reason & 1)
        result = ReleaseWeaponStorage(this);
    return result;
}

uint32 Grenades::Fire(const uint32 *input_position, const sint16 *input_angles)
{
    uint32 timer;
    Vector3 position;
    GameHeapBlock *projectile;
    uint32 damage;
    uint32 radius;
    uint32 duration;
    ShortVector3 angles;
    FUNCTION_MARKER(0x8002A65C, "SLUS_003.73");
    timer = this->fire_timer;
    if (timer)
    {
        this->fire_timer = timer - 1;
        return 0;
    }
    PlaySoundEffect(23, 8192, 0);
    memcpy(position.words, input_position, sizeof(position.words));
    OffsetPositionByAngles(position.words, (const uint16 *)input_angles);
    CopyAngleVector(angles.words, (const uint16 *)input_angles);
    angles.words[0] = (uint16)(angles.words[0] - 420);
    projectile = AllocateZeroedObject(400);
    if (projectile)
    {
        damage = this->damage;
        radius = (uint32)GetGameTuning().grenade_radius;
        new (projectile) LaunchedProjectile(position.words, angles.components, 100, damage, radius, &visual_lists.actors, 0, 0);
    }
    ConsumeAmmo(66);
    if ((sint32)this->ammo < 66)
        this->ammo = 0;
    duration = this->fire_delay;
    this->fire_timer = duration;
    return 1;
}

uint32 Grenades::Update(const uint32 *, const sint16 *)
{
    uint32 timer, result;
    FUNCTION_MARKER(0x8002A798, "SLUS_003.73");
    timer = this->fire_timer;
    result = timer - 1;
    if (timer)
        this->fire_timer = result;
    return result;
}

uint32 Grenades::ReadLight(uint8 *red, uint8 *green, uint8 *blue)
{
    uint32 timer;
    FUNCTION_MARKER(0x8002A7B4, "SLUS_003.73");
    timer = this->fire_timer;
    if (!timer)
        return 0;
    if ((sint32)(this->fire_delay - timer) >= 5)
        return 0;
    *red = 255;
    *green = 255;
    *blue = 0;
    return 1;
}

Rockets::Rockets(ModelInstance **list) : WeaponNode(list, 0), fire_delay(0), fire_timer(0)
{
    FUNCTION_MARKER(0x8002B4EC, "SLUS_003.73");
    this->type = WPN_ROCKETS;
    this->fire_delay = 15;
}

uint32 Rockets::Destroy(uint32 reason)
{
    uint32 result;
    FUNCTION_MARKER(0x8002B534, "SLUS_003.73");
    result = WeaponNode::Destroy(0);
    if (reason & 1)
        result = ReleaseWeaponStorage(this);
    return result;
}

uint32 Rockets::Fire(const uint32 *input_position, const sint16 *input_angles)
{
    uint32 timer;
    GameHeapBlock *projectile;
    uint32 damage;
    uint32 duration;
    FUNCTION_MARKER(0x8002B588, "SLUS_003.73");
    timer = this->fire_timer;
    if (timer)
    {
        this->fire_timer = timer - 1;
        return 0;
    }
    projectile = AllocateZeroedObject(380);
    if (projectile)
    {
        damage = this->damage;
        new (projectile) BallisticProjectile(input_position, input_angles, damage, 256, &visual_lists.actors);
    }
    ConsumeAmmo(50);
    if ((sint32)this->ammo < 50)
        this->ammo = 0;
    duration = this->fire_delay;
    this->fire_timer = duration;
    return 1;
}

uint32 Rockets::Update(const uint32 *, const sint16 *)
{
    uint32 timer, result;
    FUNCTION_MARKER(0x8002B640, "SLUS_003.73");
    timer = this->fire_timer;
    result = timer - 1;
    if (timer)
        this->fire_timer = result;
    return result;
}

uint32 Rockets::ReadLight(uint8 *red, uint8 *green, uint8 *blue)
{
    uint32 timer;
    FUNCTION_MARKER(0x8002B65C, "SLUS_003.73");
    timer = this->fire_timer;
    if (!timer)
        return 0;
    if ((sint32)(this->fire_delay - timer) >= 5)
        return 0;
    *red = 255;
    *green = 255;
    *blue = 0;
    return 1;
}

uint32 WeaponNode::ReadLight(uint8 *red, uint8 *green, uint8 *blue)
{
    FUNCTION_MARKER(0x8002BC98, "SLUS_003.73");
    return 0;
}

uint32 WeaponNode::Update(const uint32 *, const sint16 *)
{
    FUNCTION_MARKER(0x8002BCA0, "SLUS_003.73");
    return 0;
}

static uint32 FireInactiveWeapon()
{
    FUNCTION_MARKER(0x8002BCA8, "SLUS_003.73");
    return 0;
}

uint32 WeaponNodeBase::Destroy(uint32 reason)
{
    FUNCTION_MARKER(0x8002BCC4, "SLUS_003.73");
    if (reason & 1)
        return ReleaseWeaponStorage(this);
    return 0x800A15D8;
}

uint32 ReleaseWeaponStorage(Object *object)
{
    FUNCTION_MARKER(0x8002FF34, "SLUS_003.73");
    return object->ReleaseStorage();
}
