#include <new>
#include <string.h>
#include <stdlib.h>

#include "game_heap.h"
#include "game_tick.h"
#include "effect_object.h"
#include "visual_object.h"
#include "game_diagnostics.h"
#include "pickup_effect.h"
#include "particle_beam_pickup_effect.h"
#include "ribbon_segments.h"
#include "beam_segments.h"
#include "ring_effect.h"
#include "effect_lifecycle.h"
#include "angle_math.h"
#include "global.h"

ParticleBeamPickupEffect::ParticleBeamPickupEffect() : PickupEffect()
{
    FUNCTION_MARKER(0x8003188C, "SLUS_003.73");
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
    return reason & 1 ? ReleaseStorage() : 0;
}

uint32 ParticleBeamPickupEffect::SetPos(const Vector3 *pos)
{
    FUNCTION_MARKER(0x80031974, "SLUS_003.73");
    if (!beam[0])
    {
        if (beam[1])
            beam[1]->Destroy(3);
        beam[1] = 0;
        GameHeapBlock *allocation = EffectObject::AllocateStorage(84);
        if (!allocation)
        {
            GameDiagnostic(("Particle beam pickup allocation failed\n"));
            abort();
        }
        beam[0] = new (allocation) BeamSegments(3, 1);
        beam[0]->persistent = true;
        beam[0]->SetColors(255, 128, 0);
        allocation = EffectObject::AllocateStorage(84);
        if (!allocation)
        {
            GameDiagnostic(("Particle beam pickup layer allocation failed\n"));
            abort();
        }
        beam[1] = new (allocation) BeamSegments(3, 0);
        beam[1]->persistent = true;
        beam[1]->SetColors(185, 185, 185);
    }
    ShortVector3 angles;
    angles.x = 0;
    angles.z = 0;
    angles.y = (uint16)visual_lists.cameras->angles.y + 1024;
    Vector3 step, left, right;
    step.SetDir(100, angles);
    left.Subtract(*pos, step);
    right.Add(*pos, step);
    for (uint32 i = 0; i < 3; ++i)
    {
        Vector3 point_pos = i == 0 ? left : i == 1 ? *pos : right;
        if (beam[1])
            beam[1]->points[i].pos = point_pos;
        beam[0]->points[i].pos = point_pos;
    }
    beam[0]->SetSize(15);
    uint32 phase = (1200 * game_clock.tick) & 0x3FF0;
    sint32 sine = rsin(phase >> 2);
    beam[0]->points[1].size = 5 * sine / ONE + 20;
    return beam[1] ? beam[1]->SetSize(10) : 0;
}

ParticleBeamPickupEffect::~ParticleBeamPickupEffect()
{
    for (uint32 i = 0; i < 2; ++i)
        if (beam[i])
            beam[i]->Destroy(3);
}
