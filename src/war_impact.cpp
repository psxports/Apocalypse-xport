#include <new>
#include <stdlib.h>

#include "game_heap.h"
#include "effect_object.h"
#include "effect_controller.h"
#include "game_diagnostics.h"
#include "war_impact.h"
#include "fire_cycle.h"
#include "mesh_ring_effect.h"
#include "effect_lifecycle.h"
#include "global.h"
#include "audio_session.h"

WarImpact::WarImpact(const Vector3 *src_pos) : EffectController(), age(0), ring(), radius(128), phase(0)
{
    FUNCTION_MARKER(0x8003D134, "SLUS_003.73");
    kind = 3;
    pos = *src_pos;
    GameHeapBlock *allocation = EffectObject::AllocateStorage(116);
    if (!allocation)
    {
        GameDiagnostic(("War impact ring allocation failed\n"));
        abort();
    }
    ring = new (allocation) MeshRingEffect(16, 0xCCE0187C);
    ring->persistent = true;
    ring->SetGeometry(&pos, 0, 0);
    uint32 u = RandomRange(64);
    uint32 v = RandomRange(64);
    ring->SetTexOffsets(u, v, 2);
    ring->forced_depth = 32;
    ring->SetColors(128, 128, 64);
}

WarImpact::~WarImpact()
{
    if (ring)
        ring->Destroy(3);
}

uint32 WarImpact::Destroy(uint32 reason)
{
    FUNCTION_MARKER(0x8003D22C, "SLUS_003.73");
    if (ring)
        ring->Destroy(3);
    ring = 0;
    EffectController::Destroy(0);
    return reason & 1 ? ReleaseStorage() : 0;
}

void WarImpact::Update()
{
    FUNCTION_MARKER(0x8003D2A8, "SLUS_003.73");
    if (age >= 46)
        Finish();
    else
    {
        radius += 128;
        if (ring)
            ring->SetGeometry(&pos, radius, 512);
        if (!(age & 3))
        {
            uint32 choice = RandomRange(2);
            PlaySoundEffect(choice ? 1 : 2, 8192, 0);
            for (uint32 i = 0, offset = 0; i < 6; ++i, offset += 682)
            {
                uint32 angle = (phase + offset) & 4095;
                Vector3 tmp_pos;
                tmp_pos.x = radius * rsin(angle);
                tmp_pos.y = 0;
                tmp_pos.z = radius * rcos(angle);
                tmp_pos.Add(pos);
                GameHeapBlock *allocation = EffectObject::AllocateStorage(156);
                if (allocation)
                {
                    new (allocation) FireCycle(&tmp_pos, 45, 0, 2);
                }
            }
            phase = (phase + 423) & 4095;
        }
    }
    ++age;
}
