#include <new>
#include <string.h>
#include <stdlib.h>

#include "game_heap.h"
#include "effect_object.h"
#include "game_diagnostics.h"

#include "visual_object.h"

#include "proximity_ring_emitter.h"
#include "oscillating_ring.h"
#include "effect_lifecycle.h"

ProximityRingEmitter::ProximityRingEmitter(const uint32 *position, uint32 index, uint32 range, uint32 radius, uint32 extent, uint32 amplitude, uint32 red0, uint32 green0, uint32 blue0, uint32 red1, uint32 green1, uint32 blue1) : EffectController(), record_idx(0), radius(0), extent(0), amplitude(0), range(0), child(0)
{
    memset(colors, 0, sizeof(colors));
    FUNCTION_MARKER(0x8001C8E8, "SLUS_003.73");
    SetPosition(position);
    this->range = range;
    this->kind = 2;
    record_idx = (uint16)index;
    this->radius = radius;
    this->extent = extent;
    colors[0].r = red0;
    colors[0].g = green0;
    colors[0].b = blue0;
    colors[1].r = red1;
    colors[1].g = green1;
    colors[1].b = blue1;
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
    return (reason & 1) ? ReleaseEffectStorage(this) : 0;
}

void ProximityRingEmitter::Update()
{
    FUNCTION_MARKER(0x8001CA68, "SLUS_003.73");
    ModelInstance *player = visual_lists.players;
    uint32 x = (uint32)((sint32)(pos.words[0] - player->pos.words[0]) >> 12);
    if ((sint32)x < 0)
        x = 0 - x;
    uint32 z = (uint32)((sint32)(pos.words[2] - player->pos.words[2]) >> 12);
    if ((sint32)z < 0)
        z = 0 - z;
    uint32 distance = (sint32)x < (sint32)z ? z + (uint32)((sint32)x / 2) : x + (uint32)((sint32)z / 2);
    if ((sint32)distance < (sint32)range)
    {
        if (!child)
        {
            use_small_effect_pool = false;
            GameHeapBlock *allocation = AllocateEffectStorage(176);
            if (allocation)
            {
                child = new (allocation) OscillatingRing(pos.words, radius, extent, amplitude, colors[0].r, colors[0].g, colors[0].b, colors[1].r, colors[1].g, colors[1].b);
            }
            use_small_effect_pool = true;
            if (!child)
            {
                GameDiagnostic("Proximity ring allocation failed\n");
                abort();
            }
            child->persistent = 1;
        }
        uint32 angle = (distance * 4 + distance) * 2;
        angle = (uint32)((sint32)angle / 16);
        // Ring geometry still owns this original halfword pending its connected migration
        child->angle = angle;
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
