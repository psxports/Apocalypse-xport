#include <stdlib.h>

#include "effect_segments.h"
#include "effect_lifecycle.h"
#include "vector_arithmetic.h"
#include "game_diagnostics.h"

SegmentEffect::SegmentEffect(uint32 count) : EffectObject(), point_count(count), points(0), forced_depth(0)
{
    FUNCTION_MARKER(0x800322F4, "SLUS_003.73");
    if (count > 0x7FFFFFFF || count > 0xFFFFFFFF / 16 || count > (size_t)-1 / sizeof(*points))
    {
        GameDiagnostic("Invalid segment count %u\n", count);
        abort();
    }
    if (count)
    {
        points = (EffectPoint *)malloc((size_t)count * sizeof(*points));
        if (!points)
        {
            GameDiagnostic("Segment allocation failed\n");
            abort();
        }
    }
    for (uint32 i = 0; i < count; ++i)
    {
        points[i].rgbc.rgbc = 0x50FFFFFF;
        for (uint32 axis = 0; axis < 3; ++axis)
            points[i].pos.words[axis] = 0;
    }
    origin.words[0] = origin.words[1] = origin.words[2] = 0;
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
    return reason & 1 ? ReleaseEffectStorage(this) : result;
}

void SegmentEffect::Enable()
{
    FUNCTION_MARKER(0x8003243C, "SLUS_003.73");
    for (uint32 i = 0; i < point_count; ++i)
        points[i].rgbc.code |= 2;
    rgbc.code |= 2;
}

void SegmentEffect::SetEndpoints(const uint32 *src, const uint32 *target)
{
    FUNCTION_MARKER(0x800324D4, "SLUS_003.73");
    Vector3 pos = {{{(sint32)src[0], (sint32)src[1], (sint32)src[2]}}};
    Vector3 delta, step;
    SubtractVectors(delta.words, target, src);
    DivideVector(step.words, delta.words, point_count);
    for (uint32 axis = 0; axis < 3; ++axis)
        origin.words[axis] = src[axis];
    for (uint32 i = 0; i < point_count; ++i)
    {
        AddVectors(pos.words, step.words);
        points[i].pos = pos;
    }
}
