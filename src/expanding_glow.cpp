#include "glow_effect.h"
#include "expanding_glow.h"
#include "global.h"
#include "ring_effect.h"

ExpandingGlow::ExpandingGlow(const Vector3 *pos, uint8 r, uint8 g, uint8 b, sint32 size, sint32 duration) : GlowEffect(pos, r, g, b, 0, 0, 0, 6)
{
    FUNCTION_MARKER(0x8001EB64, "SLUS_003.73");
    extent_step = DivideSignedWord(size, duration);
    uint8 maximum = r < b ? b : r;
    if (maximum < g)
        maximum = g;
    fade = DivideSignedWord(maximum, duration);
}

uint32 ExpandingGlow::Destroy(uint32 reason)
{
    FUNCTION_MARKER(0x8001EC58, "SLUS_003.73");
    uint32 result = GlowEffect::Destroy(0);
    return (reason & 1) ? ReleaseStorage() : result;
}

void ExpandingGlow::Update()
{
    FUNCTION_MARKER(0x8001ECAC, "SLUS_003.73");
    GlowEffect::Update();
    SetRowExtent(0, row_points[0].distance + extent_step);
    SetRowExtent(1, 50);
}
