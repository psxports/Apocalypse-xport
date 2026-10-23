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
#include "particle_beam.h"
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

ParticleBeam::ParticleBeam(ModelInstance **list, sint32 range, bool mode, uint32 effect_scale, sint32 sound) : WeaponNode(list, 0), impact_count(0), seg_len(0), trail_mode(false), ring(), ring_scale(0), sound_id(0)
{
    FUNCTION_MARKER(0x80027720, "SLUS_003.73");
    type = WPN_PARTICLE_BEAM;
    trail_mode = mode;
    sound_id = sound;
    ring_scale = effect_scale;
    seg_len = range / 9;
    SetColors(255, 128, 0, 185, 185, 185, 160, 80, 0, 255, 255, 255);
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
        result = ReleaseStorage();
    return result;
}

void ParticleBeam::SetColors(uint8 r0, uint8 g0, uint8 b0, uint8 r1, uint8 g1, uint8 b1, uint8 r2, uint8 g2, uint8 b2, uint8 r3, uint8 g3, uint8 b3)
{
    FUNCTION_MARKER(0x80027878, "SLUS_003.73");
    colors[0].r = r0;
    colors[0].g = g0;
    colors[0].b = b0;
    colors[1].r = r1;
    colors[1].g = g1;
    colors[1].b = b1;
    colors[2].r = r2;
    colors[2].g = g2;
    colors[2].b = b2;
    colors[3].r = r3;
    colors[3].g = g3;
    colors[3].b = b3;
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

uint32 ParticleBeam::Fire(const Vector3 &input_pos, const ShortVector3 &input_angles)
{
    ModelBorrow model_borrow;
    CollisionRay &shared_ray = CollisionRay::GetShared();
    FUNCTION_MARKER(0x80027978, "SLUS_003.73");
    Vector3 dir, normalized, pos, endpoint, tmp, scaled, final, intersection;
    if (!sound)
        sound = PlaySoundEffect(sound_id, 8192, 0);
    ConsumeAmmo(4);
    dir.SetDir(seg_len, input_angles);
    normalized.Divide(dir, seg_len);
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
        beam[0]->persistent = true;
        beam[0]->SetColors(colors[0].r, colors[0].g, colors[0].b);
        BeamPoint *tail = beam[0]->points + 9;
        tail->size = 0;
        BeamPoint *near_tail = beam[0]->points + 8;
        near_tail->rgbc.r >>= 1;
        near_tail->rgbc.g >>= 1;
        near_tail->rgbc.b >>= 1;
        tail->rgbc.r = 0;
        tail->rgbc.g = 0;
        tail->rgbc.b = 0;
        pos = input_pos;
        for (sint32 idx = 0; idx < 10; ++idx)
        {
            BeamPoint *point = beam[0]->points + idx;
            point->pos = pos;
            pos.Add(dir);
        }
        child = EffectObject::AllocateStorage(84);
        if (child)
            beam[1] = new (child) BeamSegments(10, 0);
        if (!beam[1])
        {
            GameDiagnostic(("Weapon beam layer allocation failed\n"));
            abort();
        }
        beam[1]->persistent = true;
        beam[1]->SetColors(colors[1].r, colors[1].g, colors[1].b);
        beam[1]->SetSize(10);
        for (sint32 idx = 0; idx < 10; ++idx)
        {
            BeamPoint *dst = beam[1]->points + idx;
            BeamPoint *src = beam[0]->points + idx;
            dst->pos = src->pos;
        }
        tail = beam[1]->points + 9;
        tail->rgbc.r = 0;
        tail->rgbc.g = 0;
        tail->rgbc.b = 0;
        if (ring_scale)
        {
            child = EffectObject::AllocateStorage(104);
            if (!child)
            {
                GameDiagnostic(("Particle beam ring allocation failed\n"));
                abort();
            }
            ring = new (child) RingEffect(7, 1);
            ring->persistent = true;
            ring->SetCenterColor(colors[3].r, colors[3].g, colors[3].b);
            ring->SetRingColors(colors[2].r, colors[2].g, colors[2].b);
            ring->SetRowColors(0, 0, 0, 0);
            ring->SetRowExtent(0, (sint32)(30 * ring_scale) >> 8);
            ring->SetRadii(0);
            ring->SetPos(&muzzle_pos);
        }
    }
    sint32 step = trail_mode ? 10 : 2;
    sint32 seg = (uint32)beam[0]->point_count - 1;
    while (step - 1 < seg)
    {
        BeamPoint *dst = beam[1]->points + seg;
        BeamPoint *src = beam[0]->points + seg;
        scaled.Multiply(dir, step);
        BeamPoint *left = beam[0]->points + seg - step;
        tmp.Add(left->pos, scaled);
        --seg;
        src->pos = tmp;
        dst->pos = src->pos;
    }
    pos = input_pos;
    for (sint32 idx = 0; idx < step; ++idx)
    {
        BeamPoint *src = beam[0]->points + idx;
        BeamPoint *dst = beam[1]->points + idx;
        src->pos = pos;
        dst->pos = src->pos;
        pos.Add(dir);
    }
    sint32 phase_bias = 0;
    for (sint32 idx = 0; idx < 9; ++idx)
    {
        uint8 random = RandomRange(256);
        BeamPoint *point = beam[1]->points + idx;
        point->rgbc.r = random;
        point->rgbc.g = random;
        point->rgbc.b = random;
        sint32 sine = rsin((900 * game_clock.tick + phase_bias) & 4095);
        point = beam[0]->points + idx;
        point->size = 6 * sine / ONE + 19 + 6 * idx;
        phase_bias -= 1150;
    }
    if (ring)
    {
        uint32 radius = (sint32)(20 * ring_scale) >> 8;
        for (uint32 idx = 0; idx < ring->point_count; ++idx)
        {
            uint32 random = RandomRange(radius);
            ring->ring_points[idx].distance = radius + random;
        }
        ring->angle = RandomRange(1024);
        ring->SetPos(&input_pos);
    }
    scaled.Scale(9, dir);
    endpoint.Add(input_pos, scaled);
    tmp = input_pos;
    OffsetByAngles(tmp, input_angles);
    shared_ray.endpoints[0] = tmp;
    shared_ray.endpoints[1] = endpoint;
    shared_ray.Initialize();
    col_query.reject_surface40 = true;
    shared_ray.Query(1);
    pos = input_pos;
    ModelInstance **filter = owner_list;
    col_query.reject_surface40 = false;
    ModelInstance *hit = CollisionRay::IntersectSpheres(pos, endpoint, &intersection, *filter, 0);
    ModelInstance *wall = shared_ray.hit.GetObject();
    bool object_hit = false, wall_hit = false;
    if (wall)
    {
        endpoint = shared_ray.hit_pos;
        if (!hit)
            wall_hit = true;
        else
        {
            uint32 distance = input_pos.Distance(hit->pos);
            uint32 wall_distance = input_pos.Distance(endpoint);
            if (distance >= wall_distance)
                wall_hit = true;
            else
                object_hit = true;
        }
    }
    else if (hit)
        object_hit = true;
    if (object_hit)
    {
        endpoint = hit->pos;
        DamageTarget(hit, normalized);
    }
    if (wall_hit)
    {
        ModelInstance *target = shared_ray.hit.GetObject();
        DamageLevelObject(target, shared_ray.hit.GetPolygon(), damage);
        target = shared_ray.hit.GetObject();
        if (target && (target->flags & 16))
            DamageTarget(target, normalized);
    }
    if (object_hit || wall_hit)
    {
        if (!beam[0] || !beam[1] || !beam[0]->points || !beam[1]->points)
        {
            GameDiagnostic(("Particle beam geometry expired during damage\n"));
            abort();
        }
        sint32 distance = input_pos.EuclideanDistance(endpoint);
        seg = 0;
        if (distance >= 101)
        {
            seg = DivideSignedWord(distance, seg_len);
            if (seg < 0)
            {
                GameDiagnostic(("Particle beam has a negative impact segment: %d\n", seg));
                abort();
            }
            if (seg >= 10)
                seg = 9;
            BeamPoint *point = beam[0]->points + seg;
            BeamPoint *left = point + (seg < 9 ? 1 : 0);
            BeamPoint *right = point - (seg < 9 ? 0 : 1);
            tmp.Subtract(left->pos, right->pos);
            uint32 scalar = distance - seg * seg_len;
            scaled.Multiply(tmp, scalar);
            final.Divide(scaled, seg_len);
            final.Add(point->pos);
        }
        else
            final = endpoint;
        for (sint32 idx = seg; idx < 10; ++idx)
        {
            BeamPoint *src = beam[0]->points + idx;
            src->pos = final;
            BeamPoint *dst = beam[1]->points + idx;
            dst->pos = final;
        }
        ++impact_count;
        if (impact_count & 1)
        {
            BeamPoint *point = beam[0]->points + seg;
            RingBurstEffect::SpawnExplosion(&final, 50, point->rgbc.r, point->rgbc.g, point->rgbc.b, 3, true, 100);
        }
    }
    return 1;
}

uint32 ParticleBeam::Update(const Vector3 *)
{
    FUNCTION_MARKER(0x800284A4, "SLUS_003.73");
    if (sound)
        StopSoundVoices(sound);
    sound = 0;
    ClearChildren();
    return 0;
}

uint32 ParticleBeam::ReadLight(uint8 *r, uint8 *g, uint8 *b)
{
    FUNCTION_MARKER(0x800284E8, "SLUS_003.73");
    if (!beam[0])
        return 0;
    if (randPSX() >= 16384)
        return 0;
    *r = colors[0].r;
    *g = colors[0].g;
    *b = colors[0].b;
    return 1;
}

void ParticleBeam::DamageTarget(ModelInstance *hit, const Vector3 &dir)
{
    static_cast<VisualObject *>(hit->owner)->Damage(damage, &dir, 10);
}

ParticleBeam::~ParticleBeam()
{
    for (uint32 i = 0; i < 2; ++i)
        if (beam[i])
            beam[i]->Destroy(3);
    if (ring)
        ring->Destroy(3);
}
