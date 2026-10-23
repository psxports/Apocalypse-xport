#include <new>
#include <stdlib.h>

#include "collision_ray.h"
#include "game_heap.h"
#include "visual_object.h"
#include "model_bank.h"
#include "game_diagnostics.h"
#include "oscillating_projectile.h"
#include "ring_effect.h"
#include "object_damage.h"
#include "collision.h"
#include "global.h"
#include "effect_lifecycle.h"
#include "audio_session.h"
#include "ring_burst_effect.h"
#include "angle_math.h"
#include "effect_object.h"
#include "effect_controller.h"

OscillatingProjectile::OscillatingProjectile(const Vector3 *src_pos, const ShortVector3 *angles, uint32 damage, uint32 lifetime, uint32 speed, uint32 amplitude, uint8 r0, uint8 g0, uint8 b0, uint8 r1, uint8 g1, uint8 b1) : EffectController(), age(0), lifetime(0), damage(damage), ring(), fade_tick(lifetime), base_radius(amplitude), phase_step(370), phase(0)
{
    FUNCTION_MARKER(0x8001C158, "SLUS_003.73");
    kind = 1;
    rgb.code = 0;
    rgb.r = (r0 + r1) >> 1;
    rgb.g = (g0 + g1) >> 1;
    rgb.b = (b0 + b1) >> 1;
    GameHeapBlock *allocation = EffectObject::AllocateStorage(124);
    if (allocation)
    {
        uint32 extent = amplitude << 1;
        new (allocation) RingBurstEffect(src_pos, 4, rgb.r, rgb.g, rgb.b, 2, rgb.r, rgb.g, rgb.b, 20, extent, 0, 0, extent, 0, extent, 0, 0, 0);
    }
    pos = *src_pos;
    velocity.SetDir(speed, *angles);
    allocation = EffectObject::AllocateStorage(104);
    if (!allocation)
    {
        GameDiagnostic(("Oscillating projectile ring allocation failed\n"));
        abort();
    }
    ring = new (allocation) RingEffect(2, 1);
    ring->SetCenterColor(r0, g0, b0);
    ring->SetRingColors(r1, g1, b1);
    ring->SetRowColors(0, 0, 0, 0);
    ring->SetOddRadii(amplitude);
    ring->SetEvenRadii(amplitude);
    ring->SetRowExtent(0, (sint32)(42 * amplitude) / 16);
    ring->SetPos(&pos);
    ring->persistent = true;
    this->lifetime = lifetime + 26;
    CollisionRay &shared_ray = CollisionRay::GetShared();
    shared_ray.endpoints[0] = pos;
    for (uint32 axis = 0; axis < 3; ++axis)
        shared_ray.endpoints[1].comp[axis] = (uint32)pos.comp[axis] + this->lifetime * (uint32)velocity.comp[axis];
    shared_ray.Initialize();
    col_query.reject_surface40 = true;
    shared_ray.Query(0);
    ModelInstance *hit = shared_ray.hit.GetObject();
    col_query.reject_surface40 = false;
    if (hit)
    {
        ModelInstance *candidate = ModelBank::GetSceneObjects();
        while (candidate && candidate != hit)
            candidate = candidate->next;
        impact = shared_ray.hit;
        this->lifetime = DivideSignedWord(shared_ray.distance, speed);
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
    return reason & 1 ? ReleaseStorage() : 0;
}

void OscillatingProjectile::Update()
{
    FUNCTION_MARKER(0x8001C598, "SLUS_003.73");
    Vector3 old_pos = pos;
    pos.Add(velocity);
    if (ring)
    {
        ring->SetPos(&pos);
        ring->angle += 140;
    }
    phase += phase_step;
    sint32 amplitude = (sint32)(6 * base_radius) / 16;
    sint32 oscillation = (sint32)((uint32)amplitude * (uint32)rsin(phase & 4095)) / ONE;
    uint32 radius = base_radius + (uint32)oscillation;
    if (ring)
    {
        ring->SetOddRadii(radius);
        ring->SetEvenRadii(radius);
        ring->SetRowExtent(0, (sint32)(42 * radius) / 16);
    }
    Vector3 hit_rec;
    ModelInstance *hit = CollisionRay::IntersectSpheres(old_pos, pos, &hit_rec, visual_lists.players, 0);
    if (hit)
    {
        Vector3 hit_vector;
        hit_vector.Normalize(velocity);
        static_cast<VisualObject *>(hit->owner)->Damage(damage, &hit_vector, 29);
        GameHeapBlock *allocation = EffectObject::AllocateStorage(124);
        if (allocation)
        {
            new (allocation) RingBurstEffect(&hit->pos, 4, 255, 100, 0, 2, 255, 100, 0, 20, 80, 0, 0, 30, 30, 15, 15, 0, 0);
        }
        Finish();
        return;
    }
    ++age;
    if (fade_tick < age && ring)
    {
        ring->FadeCenter(10);
        ring->FadeRing(10);
    }
    if (age >= lifetime)
    {
        GameHeapBlock *allocation = EffectObject::AllocateStorage(124);
        if (allocation)
        {
            new (allocation) RingBurstEffect(&pos, 3, rgb.r, rgb.g, rgb.b, 2, rgb.r, rgb.g, rgb.b, 20, 40, 0, 0, 50, 50, 25, 25, 0, 0);
        }
        Finish();
        PlayPositionedSound(24, &pos, 0);
        ModelInstance *target = impact.GetObject();
        if (target)
            DamageLevelObject(target, impact.GetPolygon(), (sint32)damage);
    }
    if (!ring || !(ring->center_color & 0xFFFFFF))
        Finish();
}
