#include "oscillating_ring.h"
#include "global.h"
#include "ring_effect.h"

OscillatingRing::OscillatingRing(const Vector3 *pos, uint32 radius, uint32 extent, uint32 amplitude, uint8 r, uint8 g, uint8 b, uint8 tail_r, uint8 tail_g, uint8 tail_b) : RingEffect(pos, radius, extent, r, g, b, tail_r, tail_g, tail_b), base_radius(radius)
{
    FUNCTION_MARKER(0x80034C0C, "SLUS_003.73");
    sint32 product = radius * amplitude;
    this->amplitude = product / 256;
    for (uint32 i = 0; i < point_count; ++i)
    {
        phase[i] = RandomRange(4096);
        speed[i] = RandomRange(50) + 50;
    }
}

void OscillatingRing::Update()
{
    FUNCTION_MARKER(0x80034CF8, "SLUS_003.73");
    for (uint32 i = 0; i < point_count; ++i)
    {
        phase[i] += speed[i];
        sint32 sine = rsin(phase[i] & 4095);
        sint32 product = (uint32)amplitude * sine;
        ring_points[i].distance = base_radius + product / ONE;
    }
}

uint32 OscillatingRing::Destroy(uint32 reason)
{
    FUNCTION_MARKER(0x8003A0C4, "SLUS_003.73");
    uint32 result = RingEffect::Destroy(0);
    return (reason & 1) ? ReleaseStorage() : result;
}
