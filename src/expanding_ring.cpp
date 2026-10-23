#include "expanding_ring.h"
#include "global.h"

ExpandingRing::ExpandingRing(const Vector3 *src_pos, sint32 max_radius, uint8 r, uint8 g, uint8 b, uint32 count, uint32 growth) : PointSegments(count), max_radius(max_radius), radius(0), growth(growth), phase(0)
{
    FUNCTION_MARKER(0x8001ECFC, "SLUS_003.73");
    pos = *src_pos;
    origin = *src_pos;
    rgb[0] = r;
    rgb[1] = g;
    rgb[2] = b;
    for (sint32 i = 0; i < point_count; ++i)
    {
        EffectPoint *point = points + i;
        point->rgbc.r = r;
        point->rgbc.g = g;
        point->rgbc.b = b;
        point->pos = *src_pos;
    }
    phase = RandomRange(4096);
    EnableSegs();
}

uint32 ExpandingRing::Destroy(uint32 reason)
{
    uint32 result;
    FUNCTION_MARKER(0x8001EE48, "SLUS_003.73");
    result = PointSegments::Destroy(0);
    if (reason & 1)
        result = ReleaseStorage();
    return result;
}

void ExpandingRing::Update()
{
    FUNCTION_MARKER(0x8001EE9C, "SLUS_003.73");
    radius += growth;
    if (radius >= max_radius)
    {
        radius = max_radius;
        Finish();
    }
    uint32 fade_numerator = max_radius;
    fade_numerator = (fade_numerator - radius) << 8;
    uint32 fade = DivideSignedWord(fade_numerator, max_radius);
    uint32 angle = phase;
    sint16 c = rcos(angle & 4095);
    uint32 step = DivideSignedWord(4096, point_count);
    origin[0] = (uint32)pos.x + ProjectRadius(radius, c);
    sint16 s = rsin(angle & 4095);
    origin[2] = (uint32)pos.z + ProjectRadius(radius, s);
    for (sint32 i = 0; i < point_count; ++i)
    {
        EffectPoint *point = points + i;
        sint32 product = rgb[0] * fade;
        point->rgbc.r = product >> 8;
        product = rgb[1] * fade;
        point->rgbc.g = product >> 8;
        product = rgb[2] * fade;
        point->rgbc.b = product >> 8;
        angle += step;
        if (i == point_count - 1)
        {
            point->pos = origin;
        }
        else
        {
            c = rcos(angle & 4095);
            point->pos.x = (uint32)pos.x + ProjectRadius(radius, c);
            s = rsin(angle & 4095);
            point->pos.z = (uint32)pos.z + ProjectRadius(radius, s);
        }
    }
}

uint32 ExpandingRing::ProjectRadius(sint32 radius, sint16 trigonometric_val)
{
    // The original multiplies modulo one word before truncating Q12 toward zero
    sint32 product = (uint32)radius * trigonometric_val;
    return (uint32)(product / ONE) << 12;
}
