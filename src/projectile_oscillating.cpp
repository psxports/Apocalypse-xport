#include <new>
#include <stdlib.h>

#include "game_heap.h"
#include "visual_object.h"
#include "model_resources.h"
#include "game_diagnostics.h"
#include "projectile_oscillating.h"
#include "ring_effect.h"
#include "object_damage.h"
#include "collision.h"
#include "vector_arithmetic.h"
#include "effect_lifecycle.h"
#include "audio_session.h"
#include "ring_burst_effect.h"
#include "angle_math.h"

static uint32 DivideOscillatingScalar(uint32 numerator, uint32 denominator)
{
    if (!denominator)
        return (sint32)numerator < 0 ? 1 : (uint32)-1;
    if (numerator == 0x80000000 && denominator == (uint32)-1)
        return numerator;
    return (uint32)((sint32)numerator / (sint32)denominator);
}

OscillatingProjectile::OscillatingProjectile(const uint32 *position, const sint16 *angles, uint32 damage, uint32 lifetime, uint32 speed, uint32 amplitude, uint32 r0, uint32 g0, uint32 b0, uint32 r1, uint32 g1, uint32 b1) : EffectController(), age(0), lifetime(0), damage(damage), ring(), fade_tick(lifetime), base_radius(amplitude), phase_step(370), phase(0)
{
    FUNCTION_MARKER(0x8001C158, "SLUS_003.73");
    kind = 1;
    r0 &= 255;
    g0 &= 255;
    b0 &= 255;
    r1 &= 255;
    g1 &= 255;
    b1 &= 255;
    rgb.code = 0;
    rgb.r = (r0 + r1) >> 1;
    rgb.g = (g0 + g1) >> 1;
    rgb.b = (b0 + b1) >> 1;
    GameHeapBlock *allocation = AllocateEffectStorage(124);
    if (allocation)
    {
        uint32 extent = amplitude << 1;
        new (allocation) RingBurstEffect(position, 4, rgb.r, rgb.g, rgb.b, 2, rgb.r, rgb.g, rgb.b, 20, extent, 0, 0, extent, 0, extent, 0, 0, 0);
    }
    uint32 x = position[0];
    uint32 y = position[1];
    uint32 z = position[2];
    pos.words[0] = x;
    pos.words[1] = y;
    pos.words[2] = z;
    BuildDirectionVector(velocity.words, speed, (const uint16 *)angles);
    allocation = AllocateEffectStorage(104);
    if (!allocation)
    {
        GameDiagnostic("Oscillating projectile ring allocation failed\n");
        abort();
    }
    ring = new (allocation) RingEffect(2, 1);
    ring->SetCenterColor(r0, g0, b0);
    ring->SetRingColors(r1, g1, b1);
    ring->SetRowColors(0, 0, 0, 0);
    ring->SetOddRadii(amplitude);
    ring->SetEvenRadii(amplitude);
    ring->SetRowExtent(0, (sint32)(42 * amplitude) / 16);
    ring->SetPosition(pos.words);
    ring->persistent = 1;
    this->lifetime = (uint16)lifetime + 26;
    CollisionRay &shared_ray = GetSharedCollisionRay();
    for (uint32 axis = 0; axis < 3; ++axis)
        shared_ray.endpoints[0].words[axis] = pos.words[axis];
    for (uint32 axis = 0; axis < 3; ++axis)
        shared_ray.endpoints[1].words[axis] = pos.words[axis] + this->lifetime * velocity.words[axis];
    InitializeCollisionRay(shared_ray);
    collision_query.reject_surface40 = true;
    QueryCollisionRay(shared_ray, 0);
    ModelInstance *hit = shared_ray.hit.GetObject();
    collision_query.reject_surface40 = false;
    if (hit)
    {
        ModelInstance *candidate = GetModelSceneObjects();
        while (candidate && candidate != hit)
            candidate = candidate->next;
        impact = shared_ray.hit;
        this->lifetime = DivideOscillatingScalar(shared_ray.distance, speed);
    }
    if (this->lifetime >= 151)
        this->lifetime = 150;
}

OscillatingProjectile::~OscillatingProjectile()
{
    if (ring)
        ring->Destroy(3);
}

uint32 OscillatingProjectile::Destroy(uint32 reason)
{
    FUNCTION_MARKER(0x8001C51C, "SLUS_003.73");
    if (ring)
        ring->Destroy(3);
    ring = 0;
    EffectController::Destroy(0);
    return reason & 1 ? ReleaseEffectStorage(this) : 0;
}

void OscillatingProjectile::Update()
{
    FUNCTION_MARKER(0x8001C598, "SLUS_003.73");
    Vector3 old_pos = pos;
    AddVectors(pos.words, velocity.words);
    if (ring)
    {
        ring->SetPosition(pos.words);
        ring->angle += 140;
    }
    phase += phase_step;
    sint32 amplitude = (sint32)(6 * base_radius) / 16;
    sint32 oscillation = (sint32)((uint32)amplitude * (uint32)rsin(phase & 4095)) / 4096;
    uint32 radius = base_radius + (uint32)oscillation;
    if (ring)
    {
        ring->SetOddRadii(radius);
        ring->SetEvenRadii(radius);
        ring->SetRowExtent(0, (sint32)(42 * radius) / 16);
    }
    Vector3 hit_rec;
    ModelInstance *hit = IntersectObjectSpheres(old_pos.words, pos.words, hit_rec.words, visual_lists.players, 0);
    if (hit)
    {
        Vector3 hit_vector;
        NormalizeFixedVector(hit_vector.words, velocity.words);
        static_cast<VisualObject *>(hit->owner)->Damage(damage, hit_vector.components, 29);
        GameHeapBlock *allocation = AllocateEffectStorage(124);
        if (allocation)
        {
            new (allocation) RingBurstEffect(hit->pos.words, 4, 255, 100, 0, 2, 255, 100, 0, 20, 80, 0, 0, 30, 30, 15, 15, 0, 0);
        }
        Finish();
        return;
    }
    ++age;
    if ((sint32)fade_tick < (sint16)age && ring)
    {
        ring->FadeCenter(10);
        ring->FadeRing(10);
    }
    if ((sint16)age >= lifetime)
    {
        GameHeapBlock *allocation = AllocateEffectStorage(124);
        if (allocation)
        {
            new (allocation) RingBurstEffect(pos.words, 3, rgb.r, rgb.g, rgb.b, 2, rgb.r, rgb.g, rgb.b, 20, 40, 0, 0, 50, 50, 25, 25, 0, 0);
        }
        Finish();
        PlayPositionedSound(24, pos.words, 0);
        ModelInstance *target = impact.GetObject();
        if (target)
            DamageLevelObject(target, impact.GetPolygon(), (sint32)damage);
    }
    if (!ring || !(ring->center_color & 0xFFFFFF))
        Finish();
}
