#include <new>

#include "game_diagnostics.h"
#include "oscillating_ring.h"
#include "game_random.h"
#include "effect_lifecycle.h"

OscillatingRing::OscillatingRing(const uint32 *position, uint32 radius, uint32 extent, uint32 amplitude, uint32 red, uint32 green, uint32 blue, uint32 tail_red, uint32 tail_green, uint32 tail_blue) : RingEffect(position, radius, extent, red & 255, green & 255, blue & 255, tail_red & 255, tail_green & 255, tail_blue & 255), base_radius(radius)
{
    FUNCTION_MARKER(0x80034C0C, "SLUS_003.73");
    uint32 product = radius * amplitude;
    this->amplitude = (sint32)product / 256;
    for (uint32 i = 0; i < point_count; ++i)
    {
        phase[i] = RandomRange(4096);
        speed[i] = RandomRange(50) + 50;
    }
}

uint32 OscillatingRing::Destroy(uint32 reason)
{
    FUNCTION_MARKER(0x8003A0C4, "SLUS_003.73");
    uint32 result = RingEffect::Destroy(0);
    return (reason & 1) ? ReleaseEffectStorage(this) : result;
}

void OscillatingRing::Update()
{
    FUNCTION_MARKER(0x80034CF8, "SLUS_003.73");
    for (uint32 i = 0; i < point_count; ++i)
    {
        phase[i] += speed[i];
        uint32 sine = rsin(phase[i] & 4095);
        uint32 product = amplitude * sine;
        ring_points[i].distance = base_radius + (uint32)((sint32)product / 4096);
    }
}
