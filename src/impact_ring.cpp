#include <new>
#include <stdlib.h>

#include "game_heap.h"
#include "game_diagnostics.h"
#include "visual_object.h"
#include "effect_splash.h"
#include "impact_ring.h"
#include "expanding_ring.h"
#include "global.h"
#include "glow_effect.h"
#include "effect_lifecycle.h"
#include "ring_effect.h"
#include "mesh_ring_effect.h"
#include "effect_object.h"

void ImpactRing::Update()
{
    FUNCTION_MARKER(0x8005C47C, "SLUS_003.73");
    ++age;
    if (lifetime < age)
    {
        Finish();
        return;
    }
    pos.Add(velocity);
    SetGeometry(&pos, radius + radius_step, expansion + width_step);
    sint32 color = DivideSignedWord((lifetime - age) * 255, lifetime);
    SetColors(color, color, color >> 1);
}

uint32 ImpactRing::Destroy(uint32 reason)
{
    uint32 result;
    FUNCTION_MARKER(0x8005C538, "SLUS_003.73");
    result = MeshRingEffect::Destroy(0);
    if (reason & 1)
        return ReleaseStorage();
    return result;
}

ImpactRing::ImpactRing(const Vector3 &velocity) : MeshRingEffect(12, 0xCCE0187C), age(0), lifetime(0), radius_step(0), width_step(0)
{
    this->velocity = velocity;
}
