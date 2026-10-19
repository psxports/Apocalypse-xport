#include <new>
#include <stdlib.h>
#include <string.h>

#include "game_heap.h"
#include "effect_object.h"

#include "visual_object.h"
#include "object_damage.h"
#include "player.h"
#include "mesh_ring_effect.h"
#include "effect_lifecycle.h"
#include "game_random.h"
#include "vector_arithmetic.h"
#include "vector_distance.h"
#include "game_diagnostics.h"
#include "beast_effects.h"
#include "sprite_trail.h"

BeastOrbitEffect::BeastOrbitEffect(const uint32 *position) : EffectController(), age(0), trail(), angle(0)
{
    FUNCTION_MARKER(0x80040270, "SLUS_003.73");
    for (uint32 axis = 0; axis < 3; ++axis)
        center.words[axis] = position[axis];
    angle = RandomRange(4096);
    uint32 choice = RandomRange(2);
    uint32 angular_speed = RandomRange(64);
    angle_step = choice ? angular_speed + 64 : (uint32)-64 - angular_speed;
    radius = RandomRange(256) + 380;
    pos.words[0] = (uint32)rsin(angle & 4095) * radius;
    pos.words[1] = 0;
    pos.words[2] = (uint32)rcos(angle & 4095) * radius;
    AddVectors(pos.words, center.words);
    GameHeapBlock *allocation = AllocateEffectStorage(100);
    if (allocation)
    {
        uint32 size = RandomRange(21) + 16;
        trail = new (allocation) RibbonTrail(pos.words, 6, 64, size, 16);
    }
    if (!trail)
    {
        GameDiagnostic("Effect trail allocation failed\n");
        abort();
    }
    trail->persistent = 1;
    trail->SetWidth(512);
}

uint32 BeastOrbitEffect::Destroy(uint32 reason)
{
    FUNCTION_MARKER(0x800403E4, "SLUS_003.73");
    if (trail)
        trail->Destroy(3);
    trail = 0;
    EffectController::Destroy(0);
    return reason & 1 ? ReleaseEffectStorage(this) : 0;
}

void BeastOrbitEffect::Update()
{
    FUNCTION_MARKER(0x80040460, "SLUS_003.73");
    ++age;
    if ((sint16)age >= 180)
    {
        Finish();
        return;
    }
    angle = (angle + (uint16)angle_step) & 4095;
    pos.words[0] = (uint32)rsin(angle) * radius;
    pos.words[2] = (uint32)rcos(angle & 4095) * radius;
    pos.words[0] += center.words[0];
    pos.words[2] += center.words[2];
    pos.words[1] += (RandomRange(64) - 32) << 12;
    if (trail)
        trail->SetPosition(pos.words);
}

BeastGroundEffect::BeastGroundEffect(const uint32 *position, uint32 damage, uint32 red, uint32 green, uint32 blue, uint32 segments, uint32 radial_speed, uint32 lifetime) : EffectController(), age(0), ring(), radius(0), lifetime(lifetime), damage(damage), width(128), radial_speed(radial_speed), hit_player(0)
{
    FUNCTION_MARKER(0x80040568, "SLUS_003.73");
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
        GameDiagnostic("Beast ground ring allocation failed\n");
        abort();
    }
    ring = new (allocation) MeshRingEffect(segments, 0xCCE0187C);
    ring->persistent = 1;
    ring->forced_depth = 0;
    ring->SetGeometry(pos.words, 0, 0);
    uint32 first = RandomRange(64);
    uint32 second = RandomRange(64);
    ring->SetTextureOffsets(first, second, 2);
    color.r = red;
    color.b = blue;
    color.g = green;
    ring->SetColors(red, green, blue);
}

BeastGroundEffect::~BeastGroundEffect()
{
    if (ring)
        ring->Destroy(3);
}

uint32 BeastGroundEffect::Destroy(uint32 reason)
{
    FUNCTION_MARKER(0x800406B8, "SLUS_003.73");
    if (ring)
        ring->Destroy(3);
    ring = 0;
    EffectController::Destroy(0);
    return reason & 1 ? ReleaseEffectStorage(this) : 0;
}

void BeastGroundEffect::Update()
{
    FUNCTION_MARKER(0x80040734, "SLUS_003.73");
    ObjectBorrow object_borrow;
    ++age;
    if ((sint32)lifetime < (sint16)age)
    {
        Finish();
        return;
    }
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
                const sint32 zero[3] = {0, 0, 0};
                hero.Damage(damage, zero, 0);
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

BeastOrbitEffect::~BeastOrbitEffect()
{
    if (trail)
        trail->Destroy(3);
}
