#include <new>
#include <stdlib.h>
#include <string.h>

#include "game_heap.h"
#include "effect_object.h"
#include "effect_controller.h"
#include "visual_object.h"
#include "object_damage.h"
#include "player.h"
#include "global.h"
#include "game_diagnostics.h"
#include "plague_impact_effect.h"
#include "effect_lifecycle.h"
#include "ring_burst_effect.h"
#include "mesh_ring_effect.h"

PlagueImpactEffect::PlagueImpactEffect(const Vector3 *src_pos, uint32 r, uint32 g, uint32 b) : EffectController(), age(0), ring(), radius(0), width(64)
{
    FUNCTION_MARKER(0x80044A58, "SLUS_003.73");
    kind = 3;
    pos = *src_pos;
    GameHeapBlock *allocation = EffectObject::AllocateStorage(116);
    if (!allocation)
    {
        GameDiagnostic(("Plague impact ring allocation failed\n"));
        abort();
    }
    ring = new (allocation) MeshRingEffect(16, 0xCCE0187C);
    ring->persistent = true;
    ring->SetGeometry(&pos, 0, 0);
    uint32 u = RandomRange(64);
    uint32 v = RandomRange(64);
    ring->SetTexOffsets(u, v, 2);
    ring->forced_depth = 32;
    color.r = r;
    color.g = g;
    color.b = b;
    ring->SetColors(r, g, b);
}

uint32 PlagueImpactEffect::Destroy(uint32 reason)
{
    FUNCTION_MARKER(0x80044B80, "SLUS_003.73");
    if (ring)
        ring->Destroy(3);
    ring = 0;
    EffectController::Destroy(0);
    return reason & 1 ? ReleaseStorage() : 0;
}

void PlagueImpactEffect::Update()
{
    FUNCTION_MARKER(0x80044BFC, "SLUS_003.73");
    ++age;
    if (age >= 13)
    {
        Finish();
        return;
    }
    radius += 48;
    if (ring)
        ring->SetGeometry(&pos, radius, width);
}

PlagueImpactEffect::~PlagueImpactEffect()
{
    if (ring)
        ring->Destroy(3);
}
