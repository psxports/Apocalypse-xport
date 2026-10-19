#include <new>
#include <stdlib.h>

#include "game_heap.h"
#include "sprite_trail.h"
#include "game_diagnostics.h"
#include "effect_lifecycle.h"
#include "effect_sprite.h"

static uint32 DivideTrailColor(uint32 numerator, uint32 denominator)
{
    if (!denominator)
        return (sint32)numerator < 0 ? 1 : (uint32)-1;
    if (numerator == 0x80000000 && denominator == (uint32)-1)
        return numerator;
    return (uint32)((sint32)numerator / (sint32)denominator);
}

SpriteTrail::SpriteTrail(const uint32 *position, uint32 segments, uint32 spacing, const char *body_name, const char *head_name, uint32 width, uint32 enabled) : EffectController(), segment_count(segments), sample_step(spacing - 1), sample_count(0), cur_sample(0), history(0), children(0)
{
    FUNCTION_MARKER(0x80036634, "SLUS_003.73");
    uint64 samples = (uint64)segments * sample_step + 1;
    // Native allocations must represent the original signed loop domains
    if (!segments || !spacing || segments > 0x7FFFFFFF || samples > 0x7FFFFFFF || samples > (size_t)-1 / sizeof(*history) || segments > (size_t)-1 / sizeof(*children))
    {
        GameDiagnostic("Invalid sprite trail dimensions %u/%u\n", segments, spacing);
        abort();
    }
    sample_count = (uint32)samples;
    history = (uint32(*)[3])malloc((size_t)sample_count * sizeof(*history));
    if (!history)
    {
        GameDiagnostic("Sprite trail history allocation failed\n");
        abort();
    }
    for (uint32 i = 0; i < sample_count; ++i)
    {
        const uint32 x = position[0];
        const uint32 y = position[1];
        const uint32 z = position[2];
        history[i][0] = x;
        history[i][1] = y;
        history[i][2] = z;
    }
    children = (ChildReference *)malloc((size_t)segment_count * sizeof(*children));
    if (!children)
    {
        GameDiagnostic("Sprite trail child array allocation failed\n");
        abort();
    }
    for (uint32 i = 0; i < segment_count; ++i)
        new (children + i) ChildReference();
    for (uint32 i = segment_count; i;)
    {
        --i;
        GameHeapBlock *allocation = AllocateEffectStorage(120);
        if (!allocation)
        {
            // The original null child was dereferenced during setup
            GameDiagnostic("Sprite trail child allocation failed\n");
            abort();
        }
        children[i] = new (allocation) LoopingEndpointSprite();
        children[i]->persistent = 1;
        children[i]->SelectResource(body_name);
        children[i]->SetSize(width & 0xFFFF);
        if (enabled)
            children[i]->EnableSemiTransparency();
    }
    children[0]->flags |= 16;
    children[0]->SelectResource(head_name);
}

SpriteTrail::~SpriteTrail()
{
    if (children)
    {
        for (uint32 i = 0; i < segment_count; ++i)
            if (children[i])
                children[i]->Destroy(3);
        for (uint32 i = 0; i < segment_count; ++i)
            children[i].~ChildReference();
        free(children);
    }
    free(history);
}

uint32 SpriteTrail::Destroy(uint32 flags)
{
    FUNCTION_MARKER(0x80036828, "SLUS_003.73");
    for (uint32 i = 0; i < segment_count; ++i)
    {
        if (children[i])
        {
            children[i]->Destroy(3);
            children[i] = 0;
        }
    }
    for (uint32 i = 0; i < segment_count; ++i)
        children[i].~ChildReference();
    free(children);
    children = 0;
    free(history);
    history = 0;
    EffectController::Destroy(0);
    if (flags & 1)
        return ReleaseEffectStorage(this);
    return flags & 1;
}

uint32 SpriteTrail::SetColors(uint32 red, uint32 green, uint32 blue)
{
    FUNCTION_MARKER(0x800368F8, "SLUS_003.73");
    uint32 i = 0, result = segment_count, more;
    if ((sint32)result > 0)
    {
        do
        {
            if (children[i])
                children[i]->SetColors(red & 255, green & 255, blue & 255);
            result = segment_count;
            ++i;
            more = (sint32)i < (sint32)result;
            result = i * 4;
        } while (more);
    }
    return result;
}

uint32 SpriteTrail::SetWidth(uint32 width)
{
    FUNCTION_MARKER(0x80036988, "SLUS_003.73");
    uint32 i = 0, result = segment_count;
    if ((sint32)result > 0)
    {
        do
        {
            if (children[i])
                children[i]->SetSize(width & 0xFFFF);
            result = segment_count;
            ++i;
            result = (sint32)i < (sint32)result;
        } while (result);
    }
    return result;
}

void SpriteTrail::SetPosition(const uint32 *position)
{
    FUNCTION_MARKER(0x800369F8, "SLUS_003.73");
    ++cur_sample;
    if (cur_sample == sample_count)
        cur_sample = 0;
    uint32 i = cur_sample;
    const uint32 x = position[0];
    const uint32 y = position[1];
    const uint32 z = position[2];
    history[i][0] = x;
    history[i][1] = y;
    history[i][2] = z;
    for (uint32 count = segment_count; count;)
    {
        --count;
        if (children[count])
        {
            children[count]->endpoint[1].words[0] = history[i][0];
            children[count]->endpoint[1].words[1] = history[i][1];
            children[count]->endpoint[1].words[2] = history[i][2];
        }
        i -= sample_step;
        if ((sint32)i < 0)
            i += sample_count;
        if (children[count])
        {
            children[count]->endpoint[0].words[0] = history[i][0];
            children[count]->endpoint[0].words[1] = history[i][1];
            children[count]->endpoint[0].words[2] = history[i][2];
        }
    }
}

RibbonTrail::RibbonTrail(const uint32 *position, uint32 segments, uint32 red, uint32 green, uint32 blue) : SpriteTrail(position, segments, 2, "Ribbon", "Ribbon", 400, 1), fading(0)
{
    FUNCTION_MARKER(0x80036B00, "SLUS_003.73");
    uint32 red_step = DivideTrailColor(red, segment_count);
    uint32 green_step = DivideTrailColor(green, segment_count);
    uint32 blue_step = DivideTrailColor(blue, segment_count);
    for (uint32 i = 0; (sint32)i < (sint32)segment_count; ++i)
    {
        uint32 weight = segment_count - i - 1;
        children[i]->SetColors((red - weight * red_step) & 255, (green - weight * green_step) & 255, (blue - weight * blue_step) & 255);
        children[i]->SetColorStep(8);
    }
}

uint32 RibbonTrail::Destroy(uint32 reason)
{
    FUNCTION_MARKER(0x80036C5C, "SLUS_003.73");
    uint32 result = SpriteTrail::Destroy(0);
    return (reason & 1) ? ReleaseEffectStorage(this) : result;
}

void RibbonTrail::Update()
{
    FUNCTION_MARKER(0x80036CB0, "SLUS_003.73");
    uint32 result = fading, all_empty = 1, i = 0;
    if (result)
    {
        result = segment_count;
        if ((sint32)result > 0)
        {
            do
            {
                if (children[i] && !children[i]->FadeColors(0))
                    all_empty = 0;
                ++i;
                result = (sint32)i < (sint32)segment_count;
            } while (result);
        }
        if (all_empty)
        {
            Finish();
            return;
        }
    }
    return;
}
