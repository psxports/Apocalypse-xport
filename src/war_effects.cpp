#include <new>
#include <stdlib.h>

#include "game_heap.h"
#include "effect_object.h"

#include "game_diagnostics.h"
#include "war_effects.h"
#include "fire_cycle.h"
#include "mesh_ring_effect.h"
#include "effect_lifecycle.h"
#include "game_random.h"
#include "audio_session.h"
#include "vector_arithmetic.h"

WarImpact::WarImpact(const uint32 *position) : EffectController(), age(0), ring(), radius(128), phase(0)
{
    FUNCTION_MARKER(0x8003D134, "SLUS_003.73");
    kind = 3;
    uint32 x = position[0];
    uint32 y = position[1];
    uint32 z = position[2];
    pos.words[0] = x;
    pos.words[1] = y;
    pos.words[2] = z;
    GameHeapBlock *allocation = AllocateEffectStorage(116);
    if (!allocation)
    {
        GameDiagnostic("War impact ring allocation failed\n");
        abort();
    }
    ring = new (allocation) MeshRingEffect(16, 0xCCE0187C);
    ring->persistent = 1;
    ring->SetGeometry(pos.words, 0, 0);
    uint32 first = RandomRange(64);
    uint32 second = RandomRange(64);
    ring->SetTextureOffsets(first, second, 2);
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
    return reason & 1 ? ReleaseEffectStorage(this) : 0;
}

void WarImpact::Update()
{
    FUNCTION_MARKER(0x8003D2A8, "SLUS_003.73");
    if ((sint16)age >= 46)
        Finish();
    else
    {
        radius += 128;
        if (ring)
            ring->SetGeometry(pos.words, radius, 512);
        if (!(age & 3))
        {
            uint32 value = RandomRange(2);
            PlaySoundEffect(value ? 1 : 2, 8192, 0);
            for (uint32 i = 0, offset = 0; i < 6; ++i, offset += 682)
            {
                uint32 angle = (phase + offset) & 4095;
                Vector3 position;
                position.words[0] = radius * (uint32)rsin(angle);
                value = rcos(angle);
                position.words[1] = 0;
                position.words[2] = radius * value;
                AddVectors(position.words, pos.words);
                GameHeapBlock *allocation = AllocateEffectStorage(156);
                if (allocation)
                {
                    new (allocation) FireCycle(position.words, 45, 0, 2);
                }
            }
            phase = (phase + 423) & 4095;
        }
    }
    ++age;
}
