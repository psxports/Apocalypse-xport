#include <new>
#include <stdlib.h>

#include "game_heap.h"
#include "effect_object.h"
#include "global.h"
#include "effect_lifecycle.h"
#include "game_diagnostics.h"
#include "ring_effect.h"
#include "hd.h"

RingEffect *RingEffect::SpawnProjectileTrail(const Vector3 *pos, uint8 r, uint8 g, uint8 b)
{
    FUNCTION_MARKER(0x8002557C, "SLUS_003.73");
    GameHeapBlock *allocation = EffectObject::AllocateStorage(104);
    if (!allocation)
    {
        GameDiagnostic(("Projectile ring trail allocation failed\n"));
        abort();
    }
    RingEffect *effect = new (allocation) RingEffect(4, 2);
    effect->SetCenterColor(255, 255, 255);
    effect->SetRingColors(r, g, b);
    effect->SetRowColors(0, (39 * r) >> 7, (39 * g) >> 7, (39 * b) >> 7);
    effect->SetRowColors(1, 0, 0, 0);
    effect->SetOddRadii(21);
    effect->SetEvenRadii(12);
    effect->SetRowExtent(0, 20);
    effect->SetRowExtent(1, 20);
    effect->SetPos(pos);
    effect->angle = RandomRange(1024);
    effect->persistent = true;
    return effect;
}

RingEffect::RingEffect(uint32 segs, uint32 rows) : EffectObject(), hd_history(0), ring_points(0), row_points(0), point_count(segs << 1), row_count(rows), center_color(0x32000000), angle_step(0), hide_center(false), angle(0), seg_mask(0xFFFFFFFF)
{
    FUNCTION_MARKER(0x80034598, "SLUS_003.73");
    uint64 count = (uint64)point_count * ((uint64)rows + 1);
    // Preserve representable original allocation arithmetic and native spans
    if (!segs || segs > 0x7FFFFFFF || count > 0xFFFFFFFF / 8 || count > (size_t)-1 / sizeof(*ring_points))
    {
        GameDiagnostic(("Invalid ring dimensions %u/%u\n", segs, rows));
        abort();
    }
    uint64 native_bytes = count * sizeof(*ring_points) + 2 * ((uint64)rows + 1) * sizeof(XY);
    if (native_bytes > (size_t)-1)
    {
        GameDiagnostic(("Ring native storage span overflow\n"));
        abort();
    }
    angle_step = 4096 / point_count;
    ring_points = (RingPoint *)malloc(native_bytes);
    if (!ring_points)
    {
        GameDiagnostic(("Ring geometry allocation failed\n"));
        abort();
    }
    row_points = ring_points + point_count;
    proj_rows[0] = (XY *)(ring_points + count);
    proj_rows[1] = proj_rows[0] + row_count + 1;
    for (uint32 i = 0; i < point_count * row_count; ++i)
        row_points[i].color = 0x3A000000;
    // Callers initialize inner distances/colors and row distances before rendering
    AddToList(effect_lists.rings);
}

RingEffect::RingEffect(const Vector3 *src_pos, uint32 radius, uint32 extent, uint8 r, uint8 g, uint8 b, uint8 tail_r, uint8 tail_g, uint8 tail_b) : EffectObject(), hd_history(0), ring_points(0), row_points(0), point_count(8), row_count(1), center_color(0), angle_step(512), hide_center(false), angle(0), seg_mask(0xFFFFFFFF)
{
    FUNCTION_MARKER(0x800346A8, "SLUS_003.73");
    ring_points = (RingPoint *)malloc(16 * sizeof(*ring_points) + 4 * sizeof(XY));
    if (!ring_points)
    {
        GameDiagnostic(("Ring geometry allocation failed\n"));
        abort();
    }
    row_points = ring_points + point_count;
    proj_rows[0] = (XY *)(ring_points + 16);
    proj_rows[1] = proj_rows[0] + row_count + 1;
    AddToList(effect_lists.rings);
    pos = *src_pos;
    for (uint32 i = 0; i < point_count; ++i)
        ring_points[i].distance = radius;
    RGBC color;
    color.r = r;
    color.g = g;
    color.b = b;
    color.code = 0x32;
    center_color = color.rgbc;
    color.r = tail_r;
    color.g = tail_g;
    color.b = tail_b;
    color.code = 0;
    for (uint32 i = 0; i < point_count; ++i)
        ring_points[i].color = color.rgbc;
    for (uint32 i = 0; i < point_count; ++i)
    {
        row_points[i].distance = extent;
        row_points[i].color = 0x3A000000;
    }
}

uint32 RingEffect::Destroy(uint32 reason)
{
    FUNCTION_MARKER(0x800348A8, "SLUS_003.73");
    free(ring_points);
    ring_points = 0;
    row_points = 0;
    proj_rows[0] = 0;
    proj_rows[1] = 0;
    RemoveFromList(effect_lists.rings);
    uint32 result = EffectObject::Destroy(0);
    return reason & 1 ? ReleaseStorage() : result;
}

uint32 RingEffect::SetOddRadii(uint32 val)
{
    FUNCTION_MARKER(0x80034918, "SLUS_003.73");
    for (uint32 i = 1; i < point_count; i += 2)
        ring_points[i].distance = val;
    return 0;
}

uint32 RingEffect::SetEvenRadii(uint32 val)
{
    FUNCTION_MARKER(0x80034958, "SLUS_003.73");
    for (uint32 i = 0; i < point_count; i += 2)
        ring_points[i].distance = val;
    return 0;
}

uint32 RingEffect::SetRadii(uint32 val)
{
    FUNCTION_MARKER(0x80034994, "SLUS_003.73");
    for (uint32 i = 0; i < point_count; i += 1)
        ring_points[i].distance = val;
    return 0;
}

uint32 RingEffect::SetRowExtent(uint32 row, uint32 val)
{
    FUNCTION_MARKER(0x800349D0, "SLUS_003.73");
    RingPoint *points = row_points + row * point_count;
    for (uint32 i = 0; i < point_count; ++i)
        points[i].distance = val;
    return 0;
}

uint32 RingEffect::SetCenterColor(uint8 r, uint8 g, uint8 b)
{
    FUNCTION_MARKER(0x80034A18, "SLUS_003.73");
    RGBC color;
    color.r = r;
    color.g = g;
    color.b = b;
    color.code = 0x32;
    center_color = color.rgbc;
    return 0x32000000;
}

uint32 RingEffect::SetRingColors(uint8 r, uint8 g, uint8 b)
{
    FUNCTION_MARKER(0x80034A44, "SLUS_003.73");
    RGBC color;
    color.r = r;
    color.g = g;
    color.b = b;
    color.code = 0;
    for (uint32 i = 0; i < point_count; ++i)
        ring_points[i].color = color.rgbc;
    return 0;
}

uint32 RingEffect::SetRowColors(uint32 row, uint8 r, uint8 g, uint8 b)
{
    FUNCTION_MARKER(0x80034A9C, "SLUS_003.73");
    RGBC color;
    color.r = r;
    color.g = g;
    color.b = b;
    color.code = 0x3A;
    RingPoint *points = row_points + row * point_count;
    for (uint32 i = 0; i < point_count; ++i)
        points[i].color = color.rgbc;
    return 0;
}

void RingEffect::SetColumnColors(uint32 column, uint8 r, uint8 g, uint8 b)
{
    FUNCTION_MARKER(0x80034B08, "SLUS_003.73");
    RGBC color;
    color.r = r;
    color.g = g;
    color.b = b;
    color.code = 0x3A;
    ring_points[column].color = color.rgbc;
    for (uint32 i = 0; i < row_count; ++i)
        row_points[i * point_count + column].color = color.rgbc;
}

uint32 RingEffect::FadeCenter(sint32 amount)
{
    FUNCTION_MARKER(0x80034B7C, "SLUS_003.73");
    return FadePackedColor(center_color, amount);
}

uint32 RingEffect::FadeRing(sint32 amount)
{
    FUNCTION_MARKER(0x80034B9C, "SLUS_003.73");
    for (uint32 i = 0; i < point_count; ++i)
        FadePackedColor(ring_points[i].color, amount);
    return point_count << 3;
}

void RingEffect::Update()
{
    FUNCTION_MARKER(0x8003A118, "SLUS_003.73");
}

RingEffect::~RingEffect()
{
    hd_ring_free(hd_history);
    free(ring_points);
}
