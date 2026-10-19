#include <new>

#include "game_diagnostics.h"
#include "glow_effect.h"
#include "game_random.h"
#include "effect_lifecycle.h"

static uint32 DivideGlowScalar(uint32 value, uint32 divisor)
{
    if (!divisor)
        return (sint32)value < 0 ? 1 : (uint32)-1;
    if (value == 0x80000000 && divisor == (uint32)-1)
        return value;
    return (uint32)((sint32)value / (sint32)divisor);
}

GlowEffect::GlowEffect(const uint32 *position, uint32 red, uint32 green, uint32 blue, uint32 growth, uint32 fade, uint32 size, uint32 segments) : RingEffect(segments, 2), fade(fade), radius(0), growth(growth)
{
    FUNCTION_MARKER(0x8001E8B0, "SLUS_003.73");
    SetPosition(position);
    rgb[0] = red;
    rgb[1] = green;
    rgb[2] = blue;
    hide_center = 1;
    SetCenterColor(0, 0, 0);
    SetRingColors(0, 0, 0);
    SetRadii(0);
    SetRowExtent(0, size);
    SetRowExtent(1, size);
    SetRowColors(0, rgb[0], rgb[1], rgb[2]);
    SetRowColors(1, 0, 0, 0);
    angle = RandomRange(1024);
}

uint32 GlowEffect::Destroy(uint32 reason)
{
    FUNCTION_MARKER(0x8001EA0C, "SLUS_003.73");
    uint32 result = RingEffect::Destroy(0);
    return (reason & 1) ? ReleaseEffectStorage(this) : result;
}

void GlowEffect::Update()
{
    FUNCTION_MARKER(0x8001EA60, "SLUS_003.73");
    radius += growth;
    for (uint32 axis = 0; axis < 3; ++axis)
    {
        if ((sint32)rgb[axis] < (sint32)fade)
            rgb[axis] = 0;
        else
            rgb[axis] -= (uint8)fade;
    }
    if (!(rgb[0] | rgb[1] | rgb[2]))
        Finish();
    SetRadii(radius);
    SetRowColors(0, rgb[0], rgb[1], rgb[2]);
}

ExpandingGlow::ExpandingGlow(const uint32 *position, uint32 red, uint32 green, uint32 blue, uint32 size, uint32 duration) : GlowEffect(position, red & 255, green & 255, blue & 255, 0, 0, 0, 6)
{
    FUNCTION_MARKER(0x8001EB64, "SLUS_003.73");
    extent_step = DivideGlowScalar(size, duration);
    uint32 maximum = (red & 255) < (blue & 255) ? blue & 255 : red & 255;
    if (maximum < (green & 255))
        maximum = green & 255;
    fade = DivideGlowScalar(maximum, duration);
}

uint32 ExpandingGlow::Destroy(uint32 reason)
{
    FUNCTION_MARKER(0x8001EC58, "SLUS_003.73");
    uint32 result = GlowEffect::Destroy(0);
    return (reason & 1) ? ReleaseEffectStorage(this) : result;
}

void ExpandingGlow::Update()
{
    FUNCTION_MARKER(0x8001ECAC, "SLUS_003.73");
    GlowEffect::Update();
    SetRowExtent(0, row_points[0].distance + extent_step);
    SetRowExtent(1, 50);
}
