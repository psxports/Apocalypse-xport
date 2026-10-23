#include <new>

#include "game_heap.h"
#include "ring_burst_effect.h"
#include "global.h"
#include "effect_lifecycle.h"
#include "ring_effect.h"
#include "effect_object.h"

RingBurstEffect::RingBurstEffect(const Vector3 *src_pos, uint32 segs, uint8 r, uint8 g, uint8 b, sint16 center_fade, uint8 ring_r, uint8 ring_g, uint8 ring_b, sint16 ring_fade, uint32 extent, uint16 angle_step, uint8 regenerate, sint16 odd_min, sint16 odd_range, sint16 even_min, sint16 even_range, sint16 decay, uint16 lifetime) : RingEffect(segs, 1), age(0), lifetime(0)
{
    FUNCTION_MARKER(0x8001CF9C, "SLUS_003.73");
    pos = *src_pos;
    SetCenterColor(r, g, b);
    this->center_fade = center_fade;
    SetRingColors(ring_r, ring_g, ring_b);
    this->ring_fade = ring_fade;
    SetRowColors(0, 0, 0, 0);
    SetRowExtent(0, extent);
    angle_inc = angle_step;
    this->regenerate = regenerate;
    radius_params[0].min = odd_min;
    radius_params[0].range = odd_range;
    radius_params[1].min = even_min;
    radius_params[1].range = even_range;
    this->decay = decay;
    this->lifetime = lifetime;
    RegenerateRadii();
}

uint32 RingBurstEffect::RegenerateRadii()
{
    FUNCTION_MARKER(0x8001D114, "SLUS_003.73");
    for (uint32 i = 0; i < point_count; ++i)
    {
        const RadiusRange &params = radius_params[i & 1 ? 0 : 1];
        uint32 random = RandomRange(params.range);
        ring_points[i].distance = params.min + random;
    }
    for (uint32 i = 0; i < 2; ++i)
    {
        RadiusRange &params = radius_params[i];
        if (params.min >= decay)
            params.min -= decay;
        if (params.range >= decay)
            params.range -= decay;
    }
    uint32 random = RandomRange(1024);
    angle = random;
    return random;
}

void RingBurstEffect::Update()
{
    FUNCTION_MARKER(0x8001D240, "SLUS_003.73");
    for (uint32 i = 0; i < point_count; ++i)
    {
        FadePackedColor(ring_points[i].color, ring_fade);
        // The original fades the center once per ring point
        FadePackedColor(center_color, center_fade);
    }
    if (regenerate)
        RegenerateRadii();
    if (lifetime)
    {
        ++age;
        if (lifetime < age)
            Finish();
    }
    else if (!(center_color & 0xFFFFFF))
        Finish();
}

void RingBurstEffect::SpawnExplosion(const Vector3 *pos, sint32 size, uint8 r, uint8 g, uint8 b, uint32 segs, bool mode, uint32 extent)
{
    FUNCTION_MARKER(0x8001D320, "SLUS_003.73");
    if (IsGameHeapUnderPressure())
        return;
    GameHeapBlock *object = EffectObject::AllocateStorage(124);
    if (!object)
        return;
    sint32 half = size / 2;
    if (mode)
        new (object) RingBurstEffect(pos, segs, r, g, b, 2, r, g, b, 20, extent, 0, 0, size, size, half, half, 0, 0);
    else
        new (object) RingBurstEffect(pos, segs, r, g, b, 2, r, g, b, 20, extent, 0, 0, size, 0, size, 0, 0, 0);
}

uint32 RingBurstEffect::Destroy(uint32 reason)
{
    FUNCTION_MARKER(0x8001FCD4, "SLUS_003.73");
    uint32 result = RingEffect::Destroy(0);
    return (reason & 1) ? ReleaseStorage() : result;
}
