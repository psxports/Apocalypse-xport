#include <new>
#include <stdlib.h>
#include <string.h>

#include "game_heap.h"
#include "effect_object.h"
#include "effect_controller.h"
#include "visual_object.h"
#include "object_damage.h"
#include "player.h"
#include "mesh_ring_effect.h"
#include "effect_lifecycle.h"
#include "global.h"
#include "game_diagnostics.h"
#include "beast_ground_effect.h"
#include "sprite_trail.h"

BeastGroundEffect::BeastGroundEffect(const Vector3 *src_pos, uint32 damage, uint32 r, uint32 g, uint32 b, uint32 segs, uint32 radial_speed, sint32 lifetime) : EffectController(), age(0), ring(), radius(0), lifetime(lifetime), damage(damage), width(128), radial_speed(radial_speed), hit_player(false)
{
    FUNCTION_MARKER(0x80040568, "SLUS_003.73");
    kind = 3;
    pos = *src_pos;
    GameHeapBlock *allocation = EffectObject::AllocateStorage(116);
    if (!allocation)
    {
        GameDiagnostic(("Beast ground ring allocation failed\n"));
        abort();
    }
    ring = new (allocation) MeshRingEffect(segs, 0xCCE0187C);
    ring->persistent = true;
    ring->forced_depth = 0;
    ring->SetGeometry(&pos, 0, 0);
    uint32 u = RandomRange(64);
    uint32 v = RandomRange(64);
    ring->SetTexOffsets(u, v, 2);
    color.r = r;
    color.b = b;
    color.g = g;
    ring->SetColors(r, g, b);
}

uint32 BeastGroundEffect::Destroy(uint32 reason)
{
    FUNCTION_MARKER(0x800406B8, "SLUS_003.73");
    if (ring)
        ring->Destroy(3);
    ring = 0;
    EffectController::Destroy(0);
    return reason & 1 ? ReleaseStorage() : 0;
}

void BeastGroundEffect::Update()
{
    FUNCTION_MARKER(0x80040734, "SLUS_003.73");
    ObjectBorrow object_borrow;
    ++age;
    if (lifetime < age)
    {
        Finish();
        return;
    }
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
                const Vector3 zero = {{{0, 0, 0}}};
                hero.Damage(damage, &zero, 0);
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

BeastGroundEffect::~BeastGroundEffect()
{
    if (ring)
        ring->Destroy(3);
}
