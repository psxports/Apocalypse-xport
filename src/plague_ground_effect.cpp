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
#include "plague_ground_effect.h"
#include "effect_lifecycle.h"
#include "ring_burst_effect.h"
#include "mesh_ring_effect.h"

PlagueGroundEffect::PlagueGroundEffect(const Vector3 *src_pos, uint32 damage, uint32 r, uint32 g, uint32 b, uint32 segs, uint32 radial_speed) : EffectController(), age(0), ring(), radius(0), seg_count(segs), damage(damage), width(128), radial_speed(radial_speed), hit_player(false)
{
    FUNCTION_MARKER(0x80044C64, "SLUS_003.73");
    kind = 3;
    pos = *src_pos;
    color.r = r;
    color.g = g;
    color.b = b;
}

uint32 PlagueGroundEffect::Destroy(uint32 reason)
{
    FUNCTION_MARKER(0x80044D2C, "SLUS_003.73");
    if (ring)
        ring->Destroy(3);
    ring = 0;
    EffectController::Destroy(0);
    return reason & 1 ? ReleaseStorage() : 0;
}

void PlagueGroundEffect::Update()
{
    FUNCTION_MARKER(0x80044DA8, "SLUS_003.73");
    ObjectBorrow object_borrow;
    ++age;
    if (age >= 8 && !ring)
    {
        GameHeapBlock *allocation = EffectObject::AllocateStorage(116);
        if (!allocation)
        {
            GameDiagnostic(("Plague ground ring allocation failed\n"));
            abort();
        }
        ring = new (allocation) MeshRingEffect(seg_count, 0xCCE0187C);
        ring->persistent = true;
        ring->SetGeometry(&pos, 0, 0);
        uint32 u = RandomRange(64);
        uint32 v = RandomRange(64);
        ring->SetTexOffsets(u, v, 2);
        ring->forced_depth = 32;
        ring->SetColors(color.r, color.g, color.b);
        return;
    }
    if (age >= 91)
    {
        Finish();
        return;
    }
    if (!ring)
        return;
    sint32 old_radius = radius;
    ModelInstance *player = visual_lists.players;
    radius += radial_speed;
    if (player && !hit_player)
    {
        sint32 distance = pos.HorizontalDistance(player->pos);
        sint32 outer_radius = radius + width;
        if (distance >= old_radius && outer_radius >= distance)
        {
            player = visual_lists.players;
            Player &hero = *static_cast<Player *>(player->owner);
            sint32 player_floor = (uint32)player->pos.y + (hero.ground_offset << 12);
            if (pos.y < player_floor)
            {
                const Vector3 zero = {0};
                hero.Damage(damage, &zero, 0);
                player = visual_lists.players;
                Vector3 tmp_pos = player->pos;
                tmp_pos.y = pos.y;
                GameHeapBlock *allocation = EffectObject::AllocateStorage(96);
                if (allocation)
                {
                    new (allocation) PlagueImpactEffect(&tmp_pos, color.r >> 1, color.g >> 1, color.b >> 1);
                }
                allocation = EffectObject::AllocateStorage(124);
                if (allocation)
                {
                    new (allocation) RingBurstEffect(&tmp_pos, 5, 255, 255, 255, 1, 64, 160, 0, 1, 60, 0, 1, 100, 120, 80, 100, 10, 8);
                }
                hit_player = true;
            }
        }
    }
    if (ring)
    {
        ring->SetGeometry(&pos, radius, width);
        ring->SetTexOffsets(ring->u_scroll, ring->v_scroll - 1, 2);
    }
}

PlagueGroundEffect::~PlagueGroundEffect()
{
    if (ring)
        ring->Destroy(3);
}
