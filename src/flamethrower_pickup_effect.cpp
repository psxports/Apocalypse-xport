#include <new>
#include <string.h>
#include <stdlib.h>

#include "game_heap.h"
#include "game_tick.h"
#include "effect_object.h"
#include "visual_object.h"
#include "game_diagnostics.h"
#include "pickup_effect.h"
#include "flamethrower_pickup_effect.h"
#include "ribbon_segments.h"
#include "beam_segments.h"
#include "ring_effect.h"
#include "effect_lifecycle.h"
#include "angle_math.h"
#include "global.h"

FlamethrowerPickupEffect::FlamethrowerPickupEffect() : PickupEffect(), ribbon()
{
    FUNCTION_MARKER(0x800314CC, "SLUS_003.73");
}

uint32 FlamethrowerPickupEffect::Destroy(uint32 reason)
{
    FUNCTION_MARKER(0x80031500, "SLUS_003.73");
    if (ribbon)
        ribbon->Destroy(3);
    ribbon = 0;
    PickupEffect::Destroy(0);
    return reason & 1 ? ReleaseStorage() : 0;
}

uint32 FlamethrowerPickupEffect::SetPos(const Vector3 *pos)
{
    FUNCTION_MARKER(0x80031588, "SLUS_003.73");
    ShortVector3 angles;
    angles.x = 0;
    angles.z = 0;
    angles.y = (uint16)visual_lists.cameras->angles.y + 1024;
    uint32 multiplier = 2;
    Vector3 step, cur, scaled;
    step.SetDir(70, angles);
    if (!ribbon)
    {
        GameHeapBlock *allocation = EffectObject::AllocateStorage(108);
        if (!allocation)
        {
            GameDiagnostic(("Flamethrower pickup ribbon allocation failed\n"));
            abort();
        }
        ribbon = new (allocation) RibbonSegments(5, 1);
        ribbon->persistent = true;
        ribbon->SetMaterial(0x68B7B136);
        ribbon->SetColors(255, 255, 255, 128, 128, 128);
        ribbon->SetSize(10, 40);
        RibbonPoint *points = ribbon->points;
        points[0].width = 10;
        points[0].height = 30;
        points[1].width = 10;
        points[1].height = 35;
        points[3].width = 10;
        points[3].height = 35;
        points[4].width = 10;
        points[4].height = 30;
        points[0].center.r = 0;
        points[0].center.g = 0;
        points[0].center.b = 0;
        points[0].edge.r = 0;
        points[0].edge.g = 0;
        points[0].edge.b = 0;
        points[4].center.r = 0;
        points[4].center.g = 0;
        points[4].center.b = 0;
        points[4].edge.r = 0;
        points[4].edge.g = 0;
        points[4].edge.b = 0;
    }
    scaled.Multiply(step, multiplier);
    cur.Subtract(*pos, scaled);
    for (uint32 i = 0; i < 5; ++i)
    {
        ribbon->points[i].pos = cur;
        cur.Add(step);
    }
    ribbon->v_scroll += 4;
    return ribbon->v_scroll;
}

FlamethrowerPickupEffect::~FlamethrowerPickupEffect()
{
    if (ribbon)
        ribbon->Destroy(3);
}
