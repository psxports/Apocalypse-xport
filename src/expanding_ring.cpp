#include <new>
#include <string.h>

#include "game_diagnostics.h"
#include "expanding_ring.h"
#include "game_random.h"
#include "effect_lifecycle.h"

ExpandingRing::ExpandingRing(const sint32 *position, uint32 scale, uint32 red, uint32 green, uint32 blue, uint32 count, uint32 tail) : PointSegments(count), max_radius(scale), radius(0), growth(tail), phase(0)
{
    FUNCTION_MARKER(0x8001ECFC, "SLUS_003.73");
    uint32 x = position[0];
    uint32 y = position[1];
    uint32 z = position[2];
    pos.words[0] = x;
    pos.words[1] = y;
    pos.words[2] = z;
    origin[0] = position[0];
    origin[1] = position[1];
    origin[2] = position[2];
    rgb[0] = red;
    rgb[1] = green;
    rgb[2] = blue;
    for (uint32 i = 0; i < point_count; ++i)
    {
        EffectPoint *point = points + i;
        point->rgbc.r = red;
        point->rgbc.g = green;
        point->rgbc.b = blue;
        x = position[0];
        y = position[1];
        z = position[2];
        point->pos.words[0] = x;
        point->pos.words[1] = y;
        point->pos.words[2] = z;
    }
    phase = RandomRange(4096);
    EnableSegments();
}

uint32 ExpandingRing::Destroy(uint32 reason)
{
    uint32 result;
    FUNCTION_MARKER(0x8001EE48, "SLUS_003.73");
    result = PointSegments::Destroy(0);
    if (reason & 1)
        result = ReleaseEffectStorage(this);
    return result;
}

void ExpandingRing::Update()
{
    FUNCTION_MARKER(0x8001EE9C, "SLUS_003.73");
    radius += growth;
    if ((sint32)radius >= (sint32)max_radius)
    {
        radius = max_radius;
        Finish();
    }
    uint32 fade = Divide((sint32)((max_radius - radius) << 8), (sint32)max_radius);
    uint32 angle = phase;
    sint16 c = rcos(angle & 4095);
    uint32 step = Divide(4096, (sint32)point_count);
    origin[0] = pos.words[0] + ProjectRadius(radius, c);
    sint16 s = rsin(angle & 4095);
    origin[2] = pos.words[2] + ProjectRadius(radius, s);
    for (uint32 i = 0; i < point_count; ++i)
    {
        EffectPoint *point = points + i;
        uint32 product = rgb[0] * fade;
        point->rgbc.r = (sint32)product >> 8;
        product = rgb[1] * fade;
        point->rgbc.g = (sint32)product >> 8;
        product = rgb[2] * fade;
        point->rgbc.b = (sint32)product >> 8;
        angle += step;
        if (i == point_count - 1)
        {
            point->pos.words[0] = origin[0];
            point->pos.words[1] = origin[1];
            point->pos.words[2] = origin[2];
        }
        else
        {
            c = rcos(angle & 4095);
            point->pos.words[0] = pos.words[0] + ProjectRadius(radius, c);
            s = rsin(angle & 4095);
            point->pos.words[2] = pos.words[2] + ProjectRadius(radius, s);
        }
    }
}

uint32 ExpandingRing::Divide(sint32 numerator, sint32 denominator)
{
    if (!denominator)
        return numerator < 0 ? 1 : (uint32)-1;
    if ((uint32)numerator == 0x80000000 && denominator == -1)
        return 0x80000000;
    return (uint32)(numerator / denominator);
}

uint32 ExpandingRing::ProjectRadius(uint32 radius, sint16 trigonometric_value)
{
    uint32 product = radius * (uint32)(sint32)trigonometric_value;
    if ((sint32)product < 0)
        product += 4095;
    return (uint32)((sint32)product >> 12) << 12;
}
