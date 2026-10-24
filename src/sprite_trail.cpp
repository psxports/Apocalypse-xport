#include <new>
#include <stdlib.h>

#include "game_heap.h"
#include "sprite_trail.h"
#include "game_diagnostics.h"
#include "effect_lifecycle.h"
#include "looping_endpoint_sprite.h"
#include "global.h"
#include "effect_object.h"
#include "effect_controller.h"
#include "hd.h"

SpriteTrail::SpriteTrail(const Vector3 *pos, uint32 segs, uint32 spacing, const char *body_name, const char *head_name, uint16 width, bool enabled) : EffectController(), segment_count(segs), sample_step(spacing - 1), sample_count(0), cur_sample(0), history(0), hd_insertions(0), hd_history(0), children(0)
{
    FUNCTION_MARKER(0x80036634, "SLUS_003.73");
    uint64 samples = (uint64)segs * (spacing - 1) + 1;
    // Native allocations must represent the original signed loop domains
    if (!segs || !spacing || segs > 0x7FFFFFFF || samples > 0x7FFFFFFF || samples > (size_t)-1 / sizeof(*history) || segs > (size_t)-1 / sizeof(*children))
    {
        GameDiagnostic(("Invalid sprite trail dimensions %u/%u\n", segs, spacing));
        abort();
    }
    sample_count = samples;
    history = (Vector3 *)malloc(sample_count * sizeof(*history));
    if (!history)
    {
        GameDiagnostic(("Sprite trail history allocation failed\n"));
        abort();
    }
    for (sint32 i = 0; i < sample_count; ++i)
    {
        history[i] = *pos;
    }
    children = (ChildReference *)malloc(segment_count * sizeof(*children));
    if (!children)
    {
        GameDiagnostic(("Sprite trail child array allocation failed\n"));
        abort();
    }
    for (sint32 i = 0; i < segment_count; ++i)
        new (children + i) ChildReference();
    for (sint32 i = segment_count; i;)
    {
        --i;
        GameHeapBlock *allocation = EffectObject::AllocateStorage(120);
        if (!allocation)
        {
            // The original null child was dereferenced during setup
            GameDiagnostic(("Sprite trail child allocation failed\n"));
            abort();
        }
        children[i] = new (allocation) LoopingEndpointSprite();
        children[i]->hd_trail = this;
        children[i]->persistent = true;
        children[i]->SelectResource(body_name);
        children[i]->SetSize(width);
        if (enabled)
            children[i]->EnableSemiTransparency();
    }
    children[0]->flags |= 16;
    children[0]->SelectResource(head_name);
}

SpriteTrail::~SpriteTrail()
{
    hd_trail_free(hd_history);
    if (children)
    {
        for (sint32 i = 0; i < segment_count; ++i)
            if (children[i])
                children[i]->Destroy(3);
        for (sint32 i = 0; i < segment_count; ++i)
            children[i].~ChildReference();
        free(children);
    }
    free(history);
}

uint32 SpriteTrail::Destroy(uint32 flags)
{
    FUNCTION_MARKER(0x80036828, "SLUS_003.73");
    for (sint32 i = 0; i < segment_count; ++i)
    {
        if (children[i])
        {
            children[i]->Destroy(3);
            children[i] = 0;
        }
    }
    for (sint32 i = 0; i < segment_count; ++i)
        children[i].~ChildReference();
    free(children);
    children = 0;
    free(history);
    history = 0;
    EffectController::Destroy(0);
    if (flags & 1)
        return ReleaseStorage();
    return flags & 1;
}

uint32 SpriteTrail::SetColors(uint8 r, uint8 g, uint8 b)
{
    FUNCTION_MARKER(0x800368F8, "SLUS_003.73");
    sint32 i = 0;
    for (; i < segment_count; ++i)
        if (children[i])
            children[i]->SetColors(r, g, b);
    // Preserve the original byte-offset result in the unsigned word domain
    return (uint32)i << 2;
}

uint32 SpriteTrail::SetWidth(uint16 width)
{
    FUNCTION_MARKER(0x80036988, "SLUS_003.73");
    for (sint32 i = 0; i < segment_count; ++i)
        if (children[i])
            children[i]->SetSize(width);
    return 0;
}

void SpriteTrail::SetPos(const Vector3 *pos)
{
    FUNCTION_MARKER(0x800369F8, "SLUS_003.73");
    ++hd_insertions;
    ++cur_sample;
    if (cur_sample == sample_count)
        cur_sample = 0;
    sint32 i = cur_sample;
    history[i] = *pos;
    for (sint32 count = segment_count; count;)
    {
        --count;
        if (children[count])
        {
            children[count]->endpoint[1] = history[i];
        }
        i -= sample_step;
        if (i < 0)
            i += sample_count;
        if (children[count])
        {
            children[count]->endpoint[0] = history[i];
        }
    }
}

bool SpriteTrail::CaptureHd(float near_depth, float far_depth, bool logic) const
{
    if (!logic && hd_trail_captured(hd_history))
        return true;
    HD_TRAIL_SEGMENT *samples = hd_trail_segments(segment_count);
    if (!samples)
        return false;
    for (sint32 index = 0; index < segment_count; ++index)
    {
        HD_TRAIL_SEGMENT &sample = samples[index];
        sample = HD_TRAIL_SEGMENT();
        if (!children[index])
            continue;
        const LoopingEndpointSprite *child = children[index].Get();
        const TextureFrame &frame = child->GetTexFrame();
        const TextureResource &tex = *frame.tex;
        sample.present = 1;
        sample.image.page = tex.tpage;
        sample.image.clut = tex.clut;
        sample.image.region = tex.region;
        sample.image.texel_flags = tex.HdTexelFlags(sample.image.clut);
        sample.image.width = frame.w;
        sample.image.height = frame.h;
        for (uint32 corner = 0; corner < 4; ++corner)
            sample.image.uv[corner] = tex.uv[corner].uv;
        for (uint32 point = 0; point < 2; ++point)
        {
            sample.points[point].vx = child->endpoint[point].x / 4096.0f;
            sample.points[point].vy = child->endpoint[point].y / 4096.0f;
            sample.points[point].vz = child->endpoint[point].z / 4096.0f;
        }
        sample.color = child->color;
        sample.flags = child->flags;
        sample.bias = child->depth_bias;
        sample.size = (sint16)child->size;
        sample.near_depth = near_depth;
        sample.far_depth = far_depth;
    }
    return (logic ? hd_trail_tick(&hd_history, samples, segment_count, sample_step, hd_insertions) : hd_trail_capture(&hd_history, samples, segment_count, sample_step, hd_insertions)) != 0;
}
