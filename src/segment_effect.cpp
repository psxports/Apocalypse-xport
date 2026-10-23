#include <stdlib.h>

#include "segment_effect.h"
#include "effect_lifecycle.h"
#include "game_diagnostics.h"
#include "effect_object.h"

SegmentEffect::SegmentEffect(uint32 count) : EffectObject(), point_count(count), points(0), forced_depth(0)
{
    FUNCTION_MARKER(0x800322F4, "SLUS_003.73");
    if (count > 0x7FFFFFFF || count > 0xFFFFFFFF / 16 || count > (size_t)-1 / sizeof(*points))
    {
        GameDiagnostic(("Invalid segment count %u\n", count));
        abort();
    }
    if (count)
    {
        points = (EffectPoint *)malloc(count * sizeof(*points));
        if (!points)
        {
            GameDiagnostic(("Segment allocation failed\n"));
            abort();
        }
    }
    for (sint32 i = 0; i < point_count; ++i)
    {
        EffectPoint *point = points + i;
        point->rgbc.rgbc = 0x50FFFFFF;
        for (uint32 axis = 0; axis < 3; ++axis)
            point->pos.comp[axis] = 0;
    }
    origin.x = origin.y = origin.z = 0;
    AddToList(effect_lists.segments);
    rgbc.rgbc = 0x50FFFFFF;
}

SegmentEffect::~SegmentEffect()
{
    free(points);
}

uint32 SegmentEffect::Destroy(uint32 reason)
{
    FUNCTION_MARKER(0x800323CC, "SLUS_003.73");
    free(points);
    points = 0;
    RemoveFromList(effect_lists.segments);
    uint32 result = EffectObject::Destroy(0);
    return reason & 1 ? ReleaseStorage() : result;
}

void SegmentEffect::Enable()
{
    FUNCTION_MARKER(0x8003243C, "SLUS_003.73");
    for (sint32 i = 0; i < point_count; ++i)
        points[i].rgbc.code |= 2;
    rgbc.code |= 2;
}

void SegmentEffect::SetEndpoints(const Vector3 *src, const Vector3 *target)
{
    FUNCTION_MARKER(0x800324D4, "SLUS_003.73");
    Vector3 pos = *src;
    Vector3 step;
    step.Subtract(*target, *src);
    step.Divide(step, point_count);
    origin = *src;
    for (sint32 i = 0; i < point_count; ++i)
    {
        pos.Add(step);
        points[i].pos = pos;
    }
}
