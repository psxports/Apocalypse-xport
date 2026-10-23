#include <new>
#include <stdlib.h>

#include "collision_ray.h"
#include "global.h"
#include "game_heap.h"
#include "traversal_rail.h"
#include "flexible_traversal_rail.h"
#include "player.h"
#include "effect_lifecycle.h"
#include "collision.h"
#include "record_data.h"
#include "game_object_allocation.h"
#include "game_diagnostics.h"
#include "visual_object.h"
#include "effect_object.h"

FlexibleTraversalRail::FlexibleTraversalRail(RecordCursor data, uint32 idx) : TraversalRail(data, idx), waves(0), wave_count(seg->point_count - 1), deform_timer(0), wave_bank(0)
{
    FUNCTION_MARKER(0x800196D4, "SLUS_003.73");
    // The original reserves two banks of twenty-four short vectors
    if (wave_count > 24)
    {
        GameDiagnostic(("Flexible rail wave count exceeds original capacity: %u\n", wave_count));
        abort();
    }
    waves = (ShortVector3 *)calloc(wave_count, 2 * sizeof(*waves));
    if (!waves)
    {
        GameDiagnostic(("Flexible rail wave allocation failed\n"));
        abort();
    }
    kind = 401;
}

uint32 FlexibleTraversalRail::Destroy(uint32 reason)
{
    FUNCTION_MARKER(0x80019788, "SLUS_003.73");
    free(waves);
    waves = 0;
    uint32 result = TraversalRail::Destroy(0);
    return reason & 1 ? ReleaseStorage() : result;
}

void FlexibleTraversalRail::ApplyRandomOffset(uint32 range)
{
    FUNCTION_MARKER(0x800197DC, "SLUS_003.73");
    uint32 idx = RandomRange(seg->point_count - 1);
    uint32 bank = wave_bank;
    uint32 half_range = (sint32)range >> 1;
    uint32 x = RandomRange(range) - half_range;
    uint32 y = RandomRange(range) - half_range;
    uint32 z = RandomRange(range) - half_range;
    ShortVector3 &offset = waves[bank * wave_count + idx];
    offset.comp[2] = z;
    offset.comp[0] = x;
    offset.comp[1] = y;
    deform_timer = 64;
}

uint32 FlexibleTraversalRail::Update()
{
    FUNCTION_MARKER(0x800198A4, "SLUS_003.73");
    UpdateSegColor();
    if (deform_timer && seg)
    {
        --deform_timer;
        sint32 count = seg->point_count;
        for (sint32 i = 0; i < count - 1; ++i)
        {
            ShortVector3 &cur = waves[wave_bank * wave_count + i];
            const ShortVector3 &prev = waves[(wave_bank ^ 1) * wave_count + (i ? i - 1 : i + 1)];
            const ShortVector3 &next = waves[(wave_bank ^ 1) * wave_count + (i == count - 2 ? i - 1 : i + 1)];
            ShortVector3 neighbor;
            if (i && i != count - 2)
                neighbor.AddAngles(prev, next);
            else
                neighbor = i == 0 ? next : prev;
            for (uint32 axis = 0; axis < 3; ++axis)
            {
                sint16 wave_delta = (uint16)neighbor.comp[axis] - (uint16)cur.comp[axis];
                cur.comp[axis] = wave_delta - (wave_delta >> 3);
            }
        }
        Vector3 delta, step, pos;
        delta.Subtract(target, model.pos);
        step.Divide(delta, count);
        pos = model.pos;
        seg->origin = pos;
        for (sint32 i = 0; i < count - 1; ++i)
        {
            pos.Add(step);
            for (uint32 axis = 0; axis < 3; ++axis)
                pos.comp[axis] = (uint32)pos.comp[axis] + ((uint32)waves[wave_bank * wave_count + i].comp[axis] << 12);
            seg->points[i].pos = pos;
        }
        seg->points[count - 1].pos = target;
        wave_bank ^= 1;
    }
    UpdatePlayerContact();
    return 0;
}

FlexibleTraversalRail::~FlexibleTraversalRail()
{
    free(waves);
}
