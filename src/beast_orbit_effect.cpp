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
#include "beast_orbit_effect.h"
#include "sprite_trail.h"
#include "ribbon_trail.h"

BeastOrbitEffect::BeastOrbitEffect(const Vector3 *src_pos) : EffectController(), age(0), trail(), angle(0)
{
    FUNCTION_MARKER(0x80040270, "SLUS_003.73");
    center = *src_pos;
    angle = RandomRange(4096);
    uint32 choice = RandomRange(2);
    sint32 angular_speed = RandomRange(64);
    angle_step = choice ? angular_speed + 64 : -64 - angular_speed;
    radius = RandomRange(256) + 380;
    pos.x = rsin(angle) * radius;
    pos.y = 0;
    pos.z = rcos(angle) * radius;
    pos.Add(center);
    GameHeapBlock *allocation = EffectObject::AllocateStorage(100);
    if (allocation)
    {
        uint32 size = RandomRange(21) + 16;
        trail = new (allocation) RibbonTrail(&pos, 6, 64, size, 16);
    }
    if (!trail)
    {
        GameDiagnostic(("Effect trail allocation failed\n"));
        abort();
    }
    trail->persistent = true;
    trail->SetWidth(512);
}

uint32 BeastOrbitEffect::Destroy(uint32 reason)
{
    FUNCTION_MARKER(0x800403E4, "SLUS_003.73");
    if (trail)
        trail->Destroy(3);
    trail = 0;
    EffectController::Destroy(0);
    return reason & 1 ? ReleaseStorage() : 0;
}

void BeastOrbitEffect::Update()
{
    FUNCTION_MARKER(0x80040460, "SLUS_003.73");
    ++age;
    if (age >= 180)
    {
        Finish();
        return;
    }
    angle = (angle + angle_step) & 4095;
    pos.x = rsin(angle) * radius;
    pos.z = rcos(angle) * radius;
    pos.x = (uint32)pos.x + center.x;
    pos.z = (uint32)pos.z + center.z;
    sint32 jitter = RandomRange(64);
    pos.y = (uint32)pos.y + ((jitter - 32) * ONE);
    if (trail)
        trail->SetPos(&pos);
}

BeastOrbitEffect::~BeastOrbitEffect()
{
    if (trail)
        trail->Destroy(3);
}
