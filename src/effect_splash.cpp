#include <new>
#include <stdlib.h>

#include "game_heap.h"
#include "game_diagnostics.h"
#include "visual_object.h"
#include "effect_splash.h"
#include "droplet.h"
#include "expanding_ring.h"
#include "global.h"
#include "glow_effect.h"
#include "expanding_glow.h"
#include "effect_lifecycle.h"
#include "ring_effect.h"
#include "mesh_ring_effect.h"
#include "effect_object.h"

uint32 CreateSplash(const Vector3 *pos)
{
    Vector3 point = *pos;
    GameHeapBlock *effect;
    FUNCTION_MARKER(0x8001F52C, "SLUS_003.73");
    ExpandingGlow *glow = 0;
    effect = EffectObject::AllocateStorage(124);
    if (effect)
        glow = new (effect) ExpandingGlow(pos, 52, 102, 82, 90, 15);
    if (!glow)
    {
        GameDiagnostic(("Splash glow allocation failed\n"));
        abort();
    }
    glow->seg_mask = -505;
    glow->angle = 0;
    for (uint32 idx = 0; idx < 30; ++idx)
    {
        uint32 angle = RandomRange(4096);
        uint32 trig_angle = angle & 4095;
        sint32 offset = rsin(trig_angle) * 100 / ONE;
        point.x = (uint32)pos->x + ((uint32)offset << 12);
        offset = rcos(trig_angle) * 100 / ONE;
        point.z = (uint32)pos->z + ((uint32)offset << 12);
        effect = EffectObject::AllocateStorage(120);
        if (effect)
            new (effect) Droplet(&point, angle, 16, 48, 16);
    }
    return 0;
}

// Shared native initialization for original inline impact-ring construction
