#include <new>
#include <stdlib.h>

#include "collision_ray.h"
#include "visual_object.h"
#include "game_tuning.h"
#include "game_tick.h"
#include "object_initializers.h"
#include "effect_object.h"
#include "game_diagnostics.h"
#include "weapon_type.h"
#include "weapon_node.h"
#include "rip_laser.h"
#include "angle_math.h"
#include "game_object_allocation.h"
#include "tracking_projectile.h"
#include "projectile_line.h"
#include "launched_projectile.h"
#include "ballistic_projectile.h"
#include "beam_segments.h"
#include "ring_burst_effect.h"
#include "global.h"
#include "collision.h"
#include "object_damage.h"
#include "effect_lifecycle.h"
#include "game_heap.h"
#include "audio_voice.h"
#include "audio_session.h"
#include "spiral_ray_weapon.h"
#include "machine_gun.h"
#include "flamethrower.h"

RipLaser::RipLaser(ModelInstance **list) : WeaponNode(list, 0), retarget_timer(0), impact_active(0), impact_count(0)
{
    FUNCTION_MARKER(0x80028578, "SLUS_003.73");
    impact_pos = zero_vector;
    type = WPN_RIP_LASER;
    SetColors(64, 32, 128, 255, 255, 255);
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
        result = ReleaseStorage();
    return result;
}

void RipLaser::SetColors(uint8 r0, uint8 g0, uint8 b0, uint8 r1, uint8 g1, uint8 b1)
{
    FUNCTION_MARKER(0x80028650, "SLUS_003.73");
    colors[0].r = r0;
    colors[0].g = g0;
    colors[0].b = b0;
    colors[1].r = r1;
    colors[1].g = g1;
    colors[1].b = b1;
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

uint32 RipLaser::Fire(const Vector3 &input_pos, const ShortVector3 &input_angles)
{
    ModelBorrow model_borrow;
    CollisionRay &shared_ray = CollisionRay::GetShared();
    Vector3 pos, endpoint;
    ShortVector3 angles;
    FUNCTION_MARKER(0x800286F4, "SLUS_003.73");
    ConsumeAmmo(4);
    if (!sound)
        sound = PlaySoundEffect(9, 8192, 0);
    if (!beam[0] || !beam[1])
    {
        ClearChildren();
        GameHeapBlock *child = EffectObject::AllocateStorage(84);
        if (child)
            beam[0] = new (child) BeamSegments(10, 1);
        if (!beam[0])
        {
            GameDiagnostic(("Weapon beam allocation failed\n"));
            abort();
        }
        beam[0]->SetColors(colors[0].r, colors[0].g, colors[0].b);
        beam[0]->persistent = true;
        child = EffectObject::AllocateStorage(84);
        if (child)
            beam[1] = new (child) BeamSegments(10, 0);
        if (!beam[1])
        {
            GameDiagnostic(("Weapon beam layer allocation failed\n"));
            abort();
        }
        beam[1]->SetColors(colors[1].r, colors[1].g, colors[1].b);
        beam[1]->SetSize(10);
        beam[1]->persistent = true;
    }
    if (retarget_timer)
        --retarget_timer;
    ModelInstance *target = WeaponNode::SelectTarget(&input_pos, &input_angles, *owner_list, 3000, 512, 0);
    if (target)
    {
        pos = input_pos;
        OffsetByAngles(pos, input_angles);
        shared_ray.endpoints[0] = pos;
        shared_ray.endpoints[1] = target->pos;
        shared_ray.Initialize();
        col_query.reject_surface40 = true;
        shared_ray.Query(1);
        ModelInstance *hit = shared_ray.hit.GetObject();
        col_query.reject_surface40 = false;
        if (hit)
        {
            impact_pos = shared_ray.hit_pos;
            target = 0;
        }
        else
            impact_pos = target->pos;
        impact_active = 1;
    }
    if (!target && !retarget_timer)
    {
        if (RandomRange(2))
        {
            sint32 random = RandomRange(50);
            angles.x = (uint16)input_angles.x - random;
        }
        else
        {
            sint32 random = RandomRange(40);
            angles.x = (uint16)input_angles.x + random;
        }
        sint32 random = RandomRange(321);
        angles.z = 0;
        angles.y = (uint16)input_angles.y + random - 160;
        endpoint.SetDir(3000, angles);
        endpoint.Add(input_pos);
        pos = input_pos;
        OffsetByAngles(pos, input_angles);
        shared_ray.endpoints[0] = pos;
        shared_ray.endpoints[1] = endpoint;
        shared_ray.Initialize();
        col_query.reject_surface40 = true;
        shared_ray.Query(1);
        ModelInstance *hit = shared_ray.hit.GetObject();
        col_query.reject_surface40 = false;
        if (hit)
        {
            impact_pos = shared_ray.hit_pos;
            impact_active = 1;
        }
        else
        {
            impact_pos = endpoint;
            impact_active = 0;
        }
        retarget_timer = RandomRange(3) + 2;
    }
    if (target)
        DamageTarget(target);
    ModelInstance *hit = shared_ray.hit.GetObject();
    if (hit)
    {
        DamageLevelObject(hit, shared_ray.hit.GetPolygon(), damage);
        hit = shared_ray.hit.GetObject();
        if (hit && (hit->flags & 16))
            DamageTarget(hit);
    }
    if (!beam[0] || !beam[1] || !beam[0]->points || !beam[1]->points)
    {
        GameDiagnostic(("Rip laser geometry expired during damage\n"));
        abort();
    }
    sint32 phase_bias = 0;
    for (sint32 idx = 0; idx < 10; ++idx)
    {
        uint8 random = RandomRange(256);
        BeamPoint *point = beam[1]->points + idx;
        point->rgbc.r = random;
        point->rgbc.g = random;
        point->rgbc.b = random;
        sint32 sine = rsin((900 * game_clock.tick + phase_bias) & 4095);
        point = beam[0]->points + idx;
        point->size = 6 * sine / ONE + 10 + idx * 5;
        phase_bias -= 1150;
    }
    pos = input_pos;
    Vector3 delta, step, axis1, axis2, scaled1, scaled2, intermediate, final;
    delta.Subtract(impact_pos, input_pos);
    step.Divide(delta, 9);
    sint32 phase = 0;
    for (sint32 idx = 0; idx < 10; ++idx)
    {
        angles.x = input_angles.x;
        angles.y = (uint16)input_angles.y + 1024;
        angles.z = input_angles.z;
        axis1.SetDir(1, angles);
        angles.x = (uint16)input_angles.x + 1024;
        angles.y = input_angles.y;
        axis2.SetDir(1, angles);
        sint32 sine = rsin(phase & 4095);
        sint32 random = RandomRange(301);
        sint32 scalar = (random - 150) * sine / ONE;
        scaled1.Multiply(axis1, scalar);
        intermediate.Add(pos, scaled1);
        random = RandomRange(261);
        scalar = (random - 130) * sine / ONE;
        scaled2.Multiply(axis2, scalar);
        final.Add(intermediate, scaled2);
        BeamPoint *point = beam[0]->points + idx;
        point->pos = final;
        point = beam[1]->points + idx;
        point->pos = final;
        pos.Add(step);
        phase += 227;
    }
    ++impact_count;
    if (impact_count & 1)
    {
        GameHeapBlock *child = EffectObject::AllocateStorage(124);
        if (child)
            new (child) RingBurstEffect(&impact_pos, 5, 255, 255, 255, 1, colors[0].r, colors[0].g, colors[0].b, 1, 60, 0, 1, 50, 80, 25, 40, 10, 5);
    }
    return 1;
}

uint32 RipLaser::Update(const Vector3 *)
{
    FUNCTION_MARKER(0x80028EE4, "SLUS_003.73");
    if (sound)
        StopSoundVoices(sound);
    sound = 0;
    ClearChildren();
    return 0;
}

uint32 RipLaser::ReadLight(uint8 *r, uint8 *g, uint8 *b)
{
    FUNCTION_MARKER(0x80028F28, "SLUS_003.73");
    if (!beam[0])
        return 0;
    if (randPSX() >= 16384)
        return 0;
    *r = colors[0].r;
    *g = colors[0].g;
    *b = colors[0].b;
    return 1;
}

void RipLaser::DamageTarget(ModelInstance *hit)
{
    static_cast<VisualObject *>(hit->owner)->Damage(damage, &zero_vector, 11);
}

RipLaser::~RipLaser()
{
    for (uint32 i = 0; i < 2; ++i)
        if (beam[i])
            beam[i]->Destroy(3);
}
