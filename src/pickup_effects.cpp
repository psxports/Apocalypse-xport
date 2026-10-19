#include <new>
#include <string.h>
#include <stdlib.h>

#include "game_heap.h"
#include "game_tick.h"

#include "effect_object.h"

#include "visual_object.h"
#include "game_diagnostics.h"
#include "pickup_effects.h"
#include "ribbon_segments.h"
#include "beam_segments.h"
#include "ring_effect.h"
#include "effect_lifecycle.h"
#include "angle_math.h"
#include "vector_arithmetic.h"
#include "game_random.h"

uint32 PickupEffect::SetPosition(const uint32 *)
{
    FUNCTION_MARKER(0x8003200C, "SLUS_003.73");
    return 0;
}

uint32 PickupEffect::Destroy(uint32 reason)
{
    FUNCTION_MARKER(0x80032014, "SLUS_003.73");
    uint32 result = EffectController::Destroy(0);
    return (reason & 1) ? ReleaseEffectStorage(this) : result;
}

uint32 FlamethrowerPickupEffect::SetPosition(const uint32 *position)
{
    FUNCTION_MARKER(0x80031588, "SLUS_003.73");
    ShortVector3 angles;
    angles.words[0] = 0;
    angles.words[2] = 0;
    angles.words[1] = visual_lists.cameras->angles.words[1] + 1024;
    uint32 multiplier = 2;
    Vector3 step, current, scaled;
    BuildDirectionVector(step.words, 70, angles.words);
    if (!ribbon)
    {
        GameHeapBlock *allocation = AllocateEffectStorage(108);
        if (!allocation)
        {
            GameDiagnostic("Flamethrower pickup ribbon allocation failed\n");
            abort();
        }
        ribbon = new (allocation) RibbonSegments(5, 1);
        ribbon->persistent = 1;
        ribbon->SetMaterial(0x68B7B136);
        ribbon->SetColors(255, 255, 255, 128, 128, 128);
        ribbon->SetSize(10, 40);
        ribbon->points[0].width = 10;
        ribbon->points[0].height = 30;
        ribbon->points[1].width = 10;
        ribbon->points[1].height = 35;
        ribbon->points[3].width = 10;
        ribbon->points[3].height = 35;
        ribbon->points[4].width = 10;
        ribbon->points[4].height = 30;
        ribbon->points[0].center.r = 0;
        ribbon->points[0].center.g = 0;
        ribbon->points[0].center.b = 0;
        ribbon->points[0].edge.r = 0;
        ribbon->points[0].edge.g = 0;
        ribbon->points[0].edge.b = 0;
        ribbon->points[4].center.r = 0;
        ribbon->points[4].center.g = 0;
        ribbon->points[4].center.b = 0;
        ribbon->points[4].edge.r = 0;
        ribbon->points[4].edge.g = 0;
        ribbon->points[4].edge.b = 0;
    }
    MultiplyVector(scaled.words, step.words, multiplier);
    SubtractVectors(current.words, position, scaled.words);
    for (uint32 i = 0; i < 5; ++i)
    {
        ribbon->points[i].pos.words[0] = current.words[0];
        ribbon->points[i].pos.words[1] = current.words[1];
        ribbon->points[i].pos.words[2] = current.words[2];
        AddVectors(current.words, step.words);
    }
    ribbon->v_scroll += 4;
    return ribbon->v_scroll;
}

uint32 RipLaserPickupEffect::SetPosition(const uint32 *position)
{
    FUNCTION_MARKER(0x80031C64, "SLUS_003.73");
    if (!beam)
    {
        GameHeapBlock *allocation = AllocateEffectStorage(84);
        if (!allocation)
        {
            GameDiagnostic("Rip laser pickup beam allocation failed\n");
            abort();
        }
        beam = new (allocation) BeamSegments(4, 1);
        beam->persistent = 1;
        beam->SetColors(64, 32, 128);
        beam->SetSize(25);
    }
    ShortVector3 angles;
    angles.words[0] = 0;
    angles.words[2] = 0;
    angles.words[1] = visual_lists.cameras->angles.words[1] + 1024;
    uint32 multiplier = 1, divisor = 2;
    Vector3 step, current, scaled;
    BuildDirectionVector(step.words, 56, angles.words);
    MultiplyVector(scaled.words, step.words, multiplier);
    SubtractVectors(current.words, position, scaled.words);
    DivideVector(scaled.words, step.words, divisor);
    SubtractVectors(current.words, scaled.words);
    for (uint32 i = 0; i < 4; ++i)
    {
        BeamPoint *point = beam->points + i;
        point->pos = current;
        uint32 random = RandomRange(61);
        point->pos.words[1] += (random - 30) << 12;
        AddVectors(current.words, step.words);
    }
    return 0;
}

uint32 PulseLaserPickupEffect::SetPosition(const uint32 *position)
{
    FUNCTION_MARKER(0x80031ECC, "SLUS_003.73");
    if (ring)
        ring->SetPosition(position);
    else
    {
        GameHeapBlock *allocation = AllocateEffectStorage(104);
        if (!allocation)
        {
            GameDiagnostic("Pulse laser pickup ring allocation failed\n");
            abort();
        }
        ring = new (allocation) RingEffect(4, 2);
        ring->SetCenterColor(255, 255, 255);
        ring->SetRingColors(0, 130, 0);
        ring->SetRowColors(0, 0, 40, 0);
        ring->SetRowColors(1, 0, 0, 0);
        ring->SetOddRadii(21);
        ring->SetEvenRadii(12);
        ring->SetRowExtent(0, 8);
        ring->SetRowExtent(1, 8);
        ring->SetPosition(position);
        ring->angle = RandomRange(1024);
        ring->persistent = 1;
    }
    uint32 angle = ring->angle + 150;
    ring->angle = angle;
    return angle;
}

FlamethrowerPickupEffect::FlamethrowerPickupEffect() : PickupEffect(), ribbon()
{
    FUNCTION_MARKER(0x800314CC, "SLUS_003.73");
}

RipLaserPickupEffect::RipLaserPickupEffect() : PickupEffect(), beam()
{
    FUNCTION_MARKER(0x80031BA8, "SLUS_003.73");
}

PulseLaserPickupEffect::PulseLaserPickupEffect() : PickupEffect(), ring()
{
    FUNCTION_MARKER(0x80031E10, "SLUS_003.73");
}

FlamethrowerPickupEffect::~FlamethrowerPickupEffect()
{
    if (ribbon)
        ribbon->Destroy(3);
}

uint32 FlamethrowerPickupEffect::Destroy(uint32 reason)
{
    FUNCTION_MARKER(0x80031500, "SLUS_003.73");
    if (ribbon)
        ribbon->Destroy(3);
    ribbon = 0;
    PickupEffect::Destroy(0);
    return reason & 1 ? ReleaseEffectStorage(this) : 0;
}

RipLaserPickupEffect::~RipLaserPickupEffect()
{
    if (beam)
        beam->Destroy(3);
}

uint32 RipLaserPickupEffect::Destroy(uint32 reason)
{
    FUNCTION_MARKER(0x80031BDC, "SLUS_003.73");
    if (beam)
        beam->Destroy(3);
    beam = 0;
    PickupEffect::Destroy(0);
    return reason & 1 ? ReleaseEffectStorage(this) : 0;
}

PulseLaserPickupEffect::~PulseLaserPickupEffect()
{
    if (ring)
        ring->Destroy(3);
}

uint32 PulseLaserPickupEffect::Destroy(uint32 reason)
{
    FUNCTION_MARKER(0x80031E44, "SLUS_003.73");
    if (ring)
        ring->Destroy(3);
    ring = 0;
    PickupEffect::Destroy(0);
    return reason & 1 ? ReleaseEffectStorage(this) : 0;
}

ParticleBeamPickupEffect::ParticleBeamPickupEffect() : PickupEffect()
{
    FUNCTION_MARKER(0x8003188C, "SLUS_003.73");
}

ParticleBeamPickupEffect::~ParticleBeamPickupEffect()
{
    for (uint32 i = 0; i < 2; ++i)
        if (beam[i])
            beam[i]->Destroy(3);
}

uint32 ParticleBeamPickupEffect::Destroy(uint32 reason)
{
    FUNCTION_MARKER(0x800318C0, "SLUS_003.73");
    for (uint32 i = 0; i < 2; ++i)
    {
        if (beam[i])
            beam[i]->Destroy(3);
        beam[i] = 0;
    }
    PickupEffect::Destroy(0);
    return reason & 1 ? ReleaseEffectStorage(this) : 0;
}

uint32 ParticleBeamPickupEffect::SetPosition(const uint32 *position)
{
    FUNCTION_MARKER(0x80031974, "SLUS_003.73");
    if (!beam[0])
    {
        if (beam[1])
            beam[1]->Destroy(3);
        beam[1] = 0;
        GameHeapBlock *allocation = AllocateEffectStorage(84);
        if (!allocation)
        {
            GameDiagnostic("Particle beam pickup allocation failed\n");
            abort();
        }
        beam[0] = new (allocation) BeamSegments(3, 1);
        beam[0]->persistent = 1;
        beam[0]->SetColors(255, 128, 0);
        allocation = AllocateEffectStorage(84);
        if (!allocation)
        {
            GameDiagnostic("Particle beam pickup layer allocation failed\n");
            abort();
        }
        beam[1] = new (allocation) BeamSegments(3, 0);
        beam[1]->persistent = 1;
        beam[1]->SetColors(185, 185, 185);
    }
    ShortVector3 angles;
    angles.words[0] = 0;
    angles.words[2] = 0;
    angles.words[1] = visual_lists.cameras->angles.words[1] + 1024;
    Vector3 step, left, right;
    BuildDirectionVector(step.words, 100, angles.words);
    SubtractVectors(left.words, position, step.words);
    AddVectors(right.words, position, step.words);
    for (uint32 i = 0; i < 3; ++i)
    {
        const uint32 *src = i == 0 ? left.words : i == 1 ? position : right.words;
        const uint32 x = src[0];
        const uint32 y = src[1];
        const uint32 z = src[2];
        if (beam[1])
        {
            beam[1]->points[i].pos.words[0] = x;
            beam[1]->points[i].pos.words[1] = y;
            beam[1]->points[i].pos.words[2] = z;
        }
        beam[0]->points[i].pos.words[0] = x;
        beam[0]->points[i].pos.words[1] = y;
        beam[0]->points[i].pos.words[2] = z;
    }
    beam[0]->SetSize(15);
    uint32 phase = (1200 * game_clock.tick) & 0x3FF0;
    sint32 sine = rsin(phase >> 2);
    uint32 product = 5 * (uint32)sine;
    if ((sint32)product < 0)
        product += 4095;
    beam[0]->points[1].size = (uint32)((sint32)product >> 12) + 20;
    return beam[1] ? beam[1]->SetSize(10) : 0;
}
