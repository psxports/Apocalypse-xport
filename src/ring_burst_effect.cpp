#include <new>
#include <string.h>

#include "game_heap.h"
#include "game_diagnostics.h"
#include "ring_burst_effect.h"
#include "game_random.h"
#include "effect_lifecycle.h"
#include "effect_color.h"

RingBurstEffect::RingBurstEffect(const uint32 *position, uint32 segments, uint32 red, uint32 green, uint32 blue, uint32 center_fade, uint32 ring_red, uint32 ring_green, uint32 ring_blue, uint32 ring_fade, uint32 extent, uint32 angle_step, uint32 regenerate, uint32 odd_min, uint32 odd_range, uint32 even_min, uint32 even_range, uint32 decay, uint32 lifetime) : RingEffect(segments, 1), age(0), lifetime(lifetime)
{
    FUNCTION_MARKER(0x8001CF9C, "SLUS_003.73");
    const uint32 x = position[0];
    const uint32 y = position[1];
    const uint32 z = position[2];
    pos.words[0] = x;
    pos.words[1] = y;
    pos.words[2] = z;
    SetCenterColor(red & 255, green & 255, blue & 255);
    this->center_fade = center_fade;
    SetRingColors(ring_red & 255, ring_green & 255, ring_blue & 255);
    this->ring_fade = ring_fade;
    SetRowColors(0, 0, 0, 0);
    SetRowExtent(0, extent);
    angle_inc = angle_step;
    this->regenerate = regenerate;
    radius_params[0] = odd_min;
    radius_params[1] = odd_range;
    radius_params[2] = even_min;
    radius_params[3] = even_range;
    this->decay = decay;
    RegenerateRadii();
}

uint32 RingBurstEffect::Destroy(uint32 reason)
{
    FUNCTION_MARKER(0x8001FCD4, "SLUS_003.73");
    uint32 result = RingEffect::Destroy(0);
    return (reason & 1) ? ReleaseEffectStorage(this) : result;
}

uint32 RingBurstEffect::RegenerateRadii()
{
    FUNCTION_MARKER(0x8001D114, "SLUS_003.73");
    for (uint32 i = 0; i < point_count; ++i)
    {
        uint32 param = i & 1 ? 0 : 2;
        uint32 random = RandomRange((sint32)radius_params[param + 1]);
        ring_points[i].distance = (uint32)(sint32)radius_params[param] + random;
    }
    for (uint32 i = 0; i < 4; ++i)
        if (radius_params[i] >= decay)
            radius_params[i] = (sint16)((uint16)radius_params[i] - (uint32)(uint16)decay);
    uint32 random = RandomRange(1024);
    angle = random;
    return random;
}

void RingBurstEffect::Update()
{
    FUNCTION_MARKER(0x8001D240, "SLUS_003.73");
    for (uint32 i = 0; i < point_count; ++i)
    {
        FadePackedColor(&ring_points[i].color, ring_fade);
        // The original fades the center once per ring point
        FadePackedColor(&center_color, center_fade);
    }
    if (regenerate)
        RegenerateRadii();
    if (lifetime)
    {
        ++age;
        if (lifetime < (sint16)age)
            Finish();
    }
    else if (!(center_color & 0xFFFFFF))
        Finish();
}

void SpawnExplosionRing(const uint32 *position, uint32 size, uint32 red, uint32 green, uint32 blue, uint32 segments, uint32 mode, uint32 extent)
{
    FUNCTION_MARKER(0x8001D320, "SLUS_003.73");
    if (IsGameHeapUnderPressure())
        return;
    GameHeapBlock *object = AllocateEffectStorage(124);
    if (!object)
        return;
    uint32 half = (uint32)((sint32)size / 2);
    if (mode)
        new (object) RingBurstEffect(position, segments, red, green, blue, 2, red, green, blue, 20, extent, 0, 0, size, size, half, half, 0, 0);
    else
        new (object) RingBurstEffect(position, segments, red, green, blue, 2, red, green, blue, 20, extent, 0, 0, size, 0, size, 0, 0, 0);
}

// Native update callbacks discard the original return register
