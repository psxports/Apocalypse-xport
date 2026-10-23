#include <new>
#include <string.h>
#include <stdlib.h>

#include "game_heap.h"
#include "game_tick.h"
#include "effect_object.h"
#include "visual_object.h"
#include "game_diagnostics.h"
#include "pickup_effect.h"
#include "rip_laser_pickup_effect.h"
#include "ribbon_segments.h"
#include "beam_segments.h"
#include "ring_effect.h"
#include "effect_lifecycle.h"
#include "angle_math.h"
#include "global.h"

RipLaserPickupEffect::RipLaserPickupEffect() : PickupEffect(), beam()
{
    FUNCTION_MARKER(0x80031BA8, "SLUS_003.73");
}

uint32 RipLaserPickupEffect::Destroy(uint32 reason)
{
    FUNCTION_MARKER(0x80031BDC, "SLUS_003.73");
    if (beam)
        beam->Destroy(3);
    beam = 0;
    PickupEffect::Destroy(0);
    return reason & 1 ? ReleaseStorage() : 0;
}

uint32 RipLaserPickupEffect::SetPos(const Vector3 *pos)
{
    FUNCTION_MARKER(0x80031C64, "SLUS_003.73");
    if (!beam)
    {
        GameHeapBlock *allocation = EffectObject::AllocateStorage(84);
        if (!allocation)
        {
            GameDiagnostic(("Rip laser pickup beam allocation failed\n"));
            abort();
        }
        beam = new (allocation) BeamSegments(4, 1);
        beam->persistent = true;
        beam->SetColors(64, 32, 128);
        beam->SetSize(25);
    }
    ShortVector3 angles;
    angles.x = 0;
    angles.z = 0;
    angles.y = (uint16)visual_lists.cameras->angles.y + 1024;
    uint32 divisor = 2;
    Vector3 step, cur, scaled;
    step.SetDir(56, angles);
    cur.Subtract(*pos, step);
    scaled.Divide(step, divisor);
    cur.Subtract(scaled);
    for (uint32 i = 0; i < 4; ++i)
    {
        BeamPoint *point = beam->points + i;
        point->pos = cur;
        sint32 random = RandomRange(61);
        point->pos.y = (uint32)point->pos.y + ((random - 30) * ONE);
        cur.Add(step);
    }
    return 0;
}

RipLaserPickupEffect::~RipLaserPickupEffect()
{
    if (beam)
        beam->Destroy(3);
}
