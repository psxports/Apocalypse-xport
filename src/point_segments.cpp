#include <new>
#include <stdlib.h>

#include "game_diagnostics.h"
#include "point_segments.h"
#include "effect_lifecycle.h"

PointSegments::PointSegments(uint32 count) : EffectObject(), forced_depth(0), point_count(count), points(0)
{
    FUNCTION_MARKER(0x80032068, "SLUS_003.73");
    if (count > 0x7FFFFFFF || count > 0xFFFFFFFF / 16 || count > (size_t)-1 / sizeof(*points))
    {
        GameDiagnostic("Invalid point segment count %u\n", count);
        abort();
    }
    if (count)
    {
        points = (EffectPoint *)malloc((size_t)count * sizeof(*points));
        if (!points)
        {
            GameDiagnostic("Point segment allocation failed\n");
            abort();
        }
    }
    for (uint32 i = 0; i < count; ++i)
    {
        points[i].rgbc.rgbc = 0x40FFFFFF;
        for (uint32 axis = 0; axis < 3; ++axis)
            points[i].pos.words[axis] = 0;
    }
    for (uint32 axis = 0; axis < 3; ++axis)
        origin.words[axis] = 0;
    AddToList(effect_lists.points);
}

PointSegments::~PointSegments()
{
    free(points);
}

uint32 PointSegments::Destroy(uint32 reason)
{
    FUNCTION_MARKER(0x80032128, "SLUS_003.73");
    free(points);
    points = 0;
    RemoveFromList(effect_lists.points);
    uint32 result = EffectObject::Destroy(0);
    return reason & 1 ? ReleaseEffectStorage(this) : result;
}

uint32 PointSegments::EnableSegments()
{
    FUNCTION_MARKER(0x80032198, "SLUS_003.73");
    for (uint32 i = 0; i < point_count; ++i)
        points[i].rgbc.code |= 2;
    return 0;
}
