#include <new>
#include <stdlib.h>
#include <string.h>

#include "game_heap.h"
#include "effect_object.h"

#include "visual_object.h"
#include "object_damage.h"
#include "player.h"
#include "game_random.h"
#include "game_diagnostics.h"
#include "plague_ground_effects.h"
#include "effect_lifecycle.h"
#include "vector_distance.h"
#include "ring_burst_effect.h"
#include "mesh_ring_effect.h"

PlagueImpactEffect::PlagueImpactEffect(const uint32 *position, uint32 red, uint32 green, uint32 blue) : EffectController(), age(0), ring(), radius(0), width(64)
{
    FUNCTION_MARKER(0x80044A58, "SLUS_003.73");
    kind = 3;
    pos.words[0] = position[0];
    pos.words[1] = position[1];
    pos.words[2] = position[2];
    GameHeapBlock *allocation = AllocateEffectStorage(116);
    if (!allocation)
    {
        GameDiagnostic("Plague impact ring allocation failed\n");
        abort();
    }
    ring = new (allocation) MeshRingEffect(16, 0xCCE0187C);
    ring->persistent = 1;
    ring->SetGeometry(pos.words, 0, 0);
    uint32 first = RandomRange(64);
    uint32 second = RandomRange(64);
    ring->SetTextureOffsets(first, second, 2);
    ring->forced_depth = 32;
    color.r = red;
    color.g = green;
    color.b = blue;
    ring->SetColors(red, green, blue);
}

PlagueImpactEffect::~PlagueImpactEffect()
{
    if (ring)
        ring->Destroy(3);
}

uint32 PlagueImpactEffect::Destroy(uint32 reason)
{
    FUNCTION_MARKER(0x80044B80, "SLUS_003.73");
    if (ring)
        ring->Destroy(3);
    ring = 0;
    EffectController::Destroy(0);
    return reason & 1 ? ReleaseEffectStorage(this) : 0;
}

void PlagueImpactEffect::Update()
{
    FUNCTION_MARKER(0x80044BFC, "SLUS_003.73");
    ++age;
    if ((sint16)age >= 13)
    {
        Finish();
        return;
    }
    radius += 48;
    if (ring)
        ring->SetGeometry(pos.words, radius, width);
}

PlagueGroundEffect::PlagueGroundEffect(const uint32 *position, uint32 damage, uint32 red, uint32 green, uint32 blue, uint32 segments, uint32 radial_speed) : EffectController(), age(0), ring(), radius(0), segment_count(segments), damage(damage), width(128), radial_speed(radial_speed), hit_player(0)
{
    FUNCTION_MARKER(0x80044C64, "SLUS_003.73");
    kind = 3;
    pos.words[0] = position[0];
    pos.words[1] = position[1];
    pos.words[2] = position[2];
    color.r = red;
    color.g = green;
    color.b = blue;
}

PlagueGroundEffect::~PlagueGroundEffect()
{
    if (ring)
        ring->Destroy(3);
}

uint32 PlagueGroundEffect::Destroy(uint32 reason)
{
    FUNCTION_MARKER(0x80044D2C, "SLUS_003.73");
    if (ring)
        ring->Destroy(3);
    ring = 0;
    EffectController::Destroy(0);
    return reason & 1 ? ReleaseEffectStorage(this) : 0;
}

void PlagueGroundEffect::Update()
{
    FUNCTION_MARKER(0x80044DA8, "SLUS_003.73");
    ObjectBorrow object_borrow;
    ++age;
    if ((sint16)age >= 8 && !ring)
    {
        GameHeapBlock *allocation = AllocateEffectStorage(116);
        if (!allocation)
        {
            GameDiagnostic("Plague ground ring allocation failed\n");
            abort();
        }
        ring = new (allocation) MeshRingEffect(segment_count, 0xCCE0187C);
        ring->persistent = 1;
        ring->SetGeometry(pos.words, 0, 0);
        uint32 first = RandomRange(64);
        uint32 second = RandomRange(64);
        ring->SetTextureOffsets(first, second, 2);
        ring->forced_depth = 32;
        ring->SetColors(color.r, color.g, color.b);
        return;
    }
    if ((sint16)age >= 91)
    {
        Finish();
        return;
    }
    if (!ring)
        return;
    uint32 old_radius = radius;
    ModelInstance *player = visual_lists.players;
    radius += radial_speed;
    if (player && !hit_player)
    {
        uint32 distance = MeasureHorizontalDistance(pos.words, player->pos.words);
        if ((sint32)distance >= (sint32)old_radius && (sint32)(radius + width) >= (sint32)distance)
        {
            player = visual_lists.players;
            Player &hero = *static_cast<Player *>(player->owner);
            if ((sint32)pos.words[1] < (sint32)(player->pos.words[1] + ((uint32)hero.ground_offset << 12)))
            {
                const Vector3 zero = {0};
                hero.Damage(damage, zero.components, 0);
                player = visual_lists.players;
                Vector3 position = player->pos;
                position.words[1] = pos.words[1];
                GameHeapBlock *allocation = AllocateEffectStorage(96);
                if (allocation)
                {
                    new (allocation) PlagueImpactEffect(position.words, (sint32)color.r >> 1, (sint32)color.g >> 1, (sint32)color.b >> 1);
                }
                allocation = AllocateEffectStorage(124);
                if (allocation)
                {
                    new (allocation) RingBurstEffect(position.words, 5, 255, 255, 255, 1, 64, 160, 0, 1, 60, 0, 1, 100, 120, 80, 100, 10, 8);
                }
                hit_player = 1;
            }
        }
    }
    if (ring)
    {
        ring->SetGeometry(pos.words, radius, width);
        ring->SetTextureOffsets(ring->u_scroll, ring->v_scroll - 1, 2);
    }
}
