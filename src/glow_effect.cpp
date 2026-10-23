#include "glow_effect.h"
#include "global.h"
#include "ring_effect.h"

GlowEffect::GlowEffect(const Vector3 *pos, uint8 r, uint8 g, uint8 b, uint32 growth, sint32 fade, uint32 size, uint32 segs) : RingEffect(segs, 2), fade(fade), radius(0), growth(growth)
{
    FUNCTION_MARKER(0x8001E8B0, "SLUS_003.73");
    SetPos(pos);
    rgb[0] = r;
    rgb[1] = g;
    rgb[2] = b;
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
    return (reason & 1) ? ReleaseStorage() : result;
}

void GlowEffect::Update()
{
    FUNCTION_MARKER(0x8001EA60, "SLUS_003.73");
    radius += growth;
    for (uint32 axis = 0; axis < 3; ++axis)
    {
        if (rgb[axis] < fade)
            rgb[axis] = 0;
        else
            rgb[axis] -= (uint8)fade;
    }
    if (!(rgb[0] | rgb[1] | rgb[2]))
        Finish();
    SetRadii(radius);
    SetRowColors(0, rgb[0], rgb[1], rgb[2]);
}
