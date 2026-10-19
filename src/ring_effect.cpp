#include <new>
#include <string.h>
#include <stdlib.h>

#include "game_heap.h"
#include "effect_object.h"

#include "game_random.h"
#include "effect_lifecycle.h"
#include "effect_color.h"
#include "game_diagnostics.h"
#include "ring_effect.h"

RingEffect::RingEffect(uint32 segs, uint32 rows) : EffectObject(), ring_points(0), row_points(0), point_count(segs << 1), row_count(rows), center_color(0x32000000), angle_step(0), hide_center(0), angle(0), seg_mask(0xFFFFFFFF)
{
    FUNCTION_MARKER(0x80034598, "SLUS_003.73");
    uint64 count = (uint64)point_count * ((uint64)rows + 1);
    // Preserve representable original allocation arithmetic and native spans
    if (!segs || segs > 0x7FFFFFFF || count > 0xFFFFFFFF / 8 || count > (size_t)-1 / sizeof(*ring_points))
    {
        GameDiagnostic("Invalid ring dimensions %u/%u\n", segs, rows);
        abort();
    }
    uint64 native_bytes = count * sizeof(*ring_points) + 2 * ((uint64)rows + 1) * sizeof(XY);
    if (native_bytes > (size_t)-1)
    {
        GameDiagnostic("Ring native storage span overflow\n");
        abort();
    }
    angle_step = 4096 / point_count;
    ring_points = (RingPoint *)malloc((size_t)native_bytes);
    if (!ring_points)
    {
        GameDiagnostic("Ring geometry allocation failed\n");
        abort();
    }
    row_points = ring_points + point_count;
    proj_rows[0] = (XY *)(ring_points + (size_t)count);
    proj_rows[1] = proj_rows[0] + row_count + 1;
    for (uint32 i = 0; i < point_count * row_count; ++i)
        row_points[i].color = 0x3A000000;
    // Callers initialize inner distances/colors and row distances before rendering
    AddToList(effect_lists.rings);
}

RingEffect::RingEffect(const uint32 *position, uint32 radius, uint32 extent, uint32 red, uint32 green, uint32 blue, uint32 tail_red, uint32 tail_green, uint32 tail_blue) : EffectObject(), ring_points(0), row_points(0), point_count(8), row_count(1), center_color(0), angle_step(512), hide_center(0), angle(0), seg_mask(0xFFFFFFFF)
{
    FUNCTION_MARKER(0x800346A8, "SLUS_003.73");
    ring_points = (RingPoint *)malloc(16 * sizeof(*ring_points) + 4 * sizeof(XY));
    if (!ring_points)
    {
        GameDiagnostic("Ring geometry allocation failed\n");
        abort();
    }
    row_points = ring_points + point_count;
    proj_rows[0] = (XY *)(ring_points + 16);
    proj_rows[1] = proj_rows[0] + row_count + 1;
    AddToList(effect_lists.rings);
    const uint32 x = position[0];
    const uint32 y = position[1];
    const uint32 z = position[2];
    pos.words[0] = x;
    pos.words[1] = y;
    pos.words[2] = z;
    for (uint32 i = 0; i < point_count; ++i)
        ring_points[i].distance = radius;
    center_color = 0x32000000 | ((blue & 255) << 16) | ((green & 255) << 8) | (red & 255);
    uint32 color = ((tail_blue & 255) << 16) | ((tail_green & 255) << 8) | (tail_red & 255);
    for (uint32 i = 0; i < point_count; ++i)
        ring_points[i].color = color;
    for (uint32 i = 0; i < point_count; ++i)
    {
        row_points[i].distance = extent;
        row_points[i].color = 0x3A000000;
    }
}

RingEffect::~RingEffect()
{
    free(ring_points);
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
    return reason & 1 ? ReleaseEffectStorage(this) : result;
}

void RingEffect::Update()
{
    FUNCTION_MARKER(0x8003A118, "SLUS_003.73");
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

uint32 RingEffect::SetCenterColor(uint32 r, uint32 g, uint32 b)
{
    FUNCTION_MARKER(0x80034A18, "SLUS_003.73");
    center_color = 0x32000000 | ((b & 255) << 16) | ((g & 255) << 8) | (r & 255);
    return 0x32000000;
}

uint32 RingEffect::SetRingColors(uint32 r, uint32 g, uint32 b)
{
    FUNCTION_MARKER(0x80034A44, "SLUS_003.73");
    uint32 color = (r & 255) | ((g & 255) << 8) | ((b & 255) << 16);
    for (uint32 i = 0; i < point_count; ++i)
        ring_points[i].color = color;
    return 0;
}

uint32 RingEffect::SetRowColors(uint32 row, uint32 r, uint32 g, uint32 b)
{
    FUNCTION_MARKER(0x80034A9C, "SLUS_003.73");
    uint32 color = 0x3A000000 | (r & 255) | ((g & 255) << 8) | ((b & 255) << 16);
    RingPoint *points = row_points + row * point_count;
    for (uint32 i = 0; i < point_count; ++i)
        points[i].color = color;
    return 0;
}

void RingEffect::SetColumnColors(uint32 column, uint32 r, uint32 g, uint32 b)
{
    FUNCTION_MARKER(0x80034B08, "SLUS_003.73");
    uint32 color = 0x3A000000 | ((b & 255) << 16) | ((g & 255) << 8) | (r & 255);
    ring_points[column].color = color;
    for (uint32 i = 0; i < row_count; ++i)
        row_points[i * point_count + column].color = color;
}

uint32 RingEffect::FadeCenter(uint32 amount)
{
    FUNCTION_MARKER(0x80034B7C, "SLUS_003.73");
    return FadePackedColor(&center_color, (sint32)amount);
}

uint32 RingEffect::FadeRing(uint32 amount)
{
    FUNCTION_MARKER(0x80034B9C, "SLUS_003.73");
    for (uint32 i = 0; i < point_count; ++i)
        FadePackedColor(&ring_points[i].color, (sint32)amount);
    return point_count << 3;
}

RingEffect *SpawnProjectileRingTrail(const uint32 *position, uint32 red, uint32 green, uint32 blue)
{
    FUNCTION_MARKER(0x8002557C, "SLUS_003.73");
    GameHeapBlock *allocation = AllocateEffectStorage(104);
    if (!allocation)
    {
        GameDiagnostic("Projectile ring trail allocation failed\n");
        abort();
    }
    RingEffect *effect = new (allocation) RingEffect(4, 2);
    effect->SetCenterColor(255, 255, 255);
    red &= 255;
    green &= 255;
    blue &= 255;
    effect->SetRingColors(red, green, blue);
    effect->SetRowColors(0, ((39 * red) >> 7) & 255, ((39 * green) >> 7) & 255, ((39 * blue) >> 7) & 255);
    effect->SetRowColors(1, 0, 0, 0);
    effect->SetOddRadii(21);
    effect->SetEvenRadii(12);
    effect->SetRowExtent(0, 20);
    effect->SetRowExtent(1, 20);
    effect->SetPosition(position);
    effect->angle = RandomRange(1024);
    effect->persistent = 1;
    return effect;
}
