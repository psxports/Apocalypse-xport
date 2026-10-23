#include <new>
#include <stdlib.h>

#include "game_heap.h"
#include "effect_object.h"
#include "effect_controller.h"
#include "game_diagnostics.h"
#include "visual_object.h"
#include "proximity_ring_emitter.h"
#include "oscillating_ring.h"
#include "effect_lifecycle.h"

ProximityRingEmitter::ProximityRingEmitter(const Vector3 *pos, uint16 idx, sint32 range, uint32 radius, uint32 extent, uint32 amplitude, uint8 r0, uint8 g0, uint8 b0, uint8 r1, uint8 g1, uint8 b1) : EffectController(), rec_idx(0), radius(0), extent(0), amplitude(0), range(0), child(0)
{
    FUNCTION_MARKER(0x8001C8E8, "SLUS_003.73");
    this->pos = *pos;
    this->range = range;
    kind = 2;
    rec_idx = idx;
    this->radius = radius;
    this->extent = extent;
    colors[0].r = r0;
    colors[0].g = g0;
    colors[0].b = b0;
    colors[1].r = r1;
    colors[1].g = g1;
    colors[1].b = b1;
    this->amplitude = amplitude;
}

uint32 ProximityRingEmitter::Destroy(uint32 reason)
{
    FUNCTION_MARKER(0x8001C9EC, "SLUS_003.73");
    if (child)
    {
        child->Destroy(3);
        child = 0;
    }
    EffectController::Destroy(0);
    return (reason & 1) ? ReleaseStorage() : 0;
}

void ProximityRingEmitter::Update()
{
    FUNCTION_MARKER(0x8001CA68, "SLUS_003.73");
    ModelInstance *player = visual_lists.players;
    sint32 x = (uint32)pos.x - player->pos.x;
    x >>= 12;
    if (x < 0)
        x = -x;
    sint32 z = (uint32)pos.z - player->pos.z;
    z >>= 12;
    if (z < 0)
        z = -z;
    sint32 distance = x < z ? z + x / 2 : x + z / 2;
    if (distance < range)
    {
        if (!child)
        {
            use_small_effect_pool = false;
            GameHeapBlock *allocation = EffectObject::AllocateStorage(176);
            if (allocation)
            {
                child = new (allocation) OscillatingRing(&pos, radius, extent, amplitude, colors[0].r, colors[0].g, colors[0].b, colors[1].r, colors[1].g, colors[1].b);
            }
            use_small_effect_pool = true;
            if (!child)
            {
                GameDiagnostic(("Proximity ring allocation failed\n"));
                abort();
            }
            child->persistent = true;
        }
        child->angle = distance * 10 / 16;
    }
    else if (child)
    {
        child->Destroy(3);
        child = 0;
    }
}

ProximityRingEmitter::~ProximityRingEmitter()
{
    if (child)
        child->Destroy(3);
}
