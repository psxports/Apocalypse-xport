#include <stdlib.h>

#include "textured_effect.h"
#include "global.h"
#include "game_diagnostics.h"

TexturedEffect::TexturedEffect() : EffectObject(), color_step(0), shrink_step(0), color(0), tex_grp(0), tex_frame(0), flags(0), frame_count(0), frame(0), frame_fraction(0), frame_step(0), size(0)
{
    FUNCTION_MARKER(0x8003319C, "SLUS_003.73");
    frame_step = 128;
    size = 400;
    color = 0x2C808080;
}

uint32 TexturedEffect::Destroy(uint32 reason)
{
    FUNCTION_MARKER(0x800331EC, "SLUS_003.73");
    uint32 result = EffectObject::Destroy(0);
    return (reason & 1) ? ReleaseStorage() : result;
}

uint32 TexturedEffect::AddFlags(uint32 flags)
{
    FUNCTION_MARKER(0x80033240, "SLUS_003.73");
    uint32 result = this->flags | flags;
    this->flags = result;
    return result;
}

void TexturedEffect::SetFrameStep(uint16 val)
{
    FUNCTION_MARKER(0x80033288, "SLUS_003.73");
    frame_step = val;
}

void TexturedEffect::SetSize(uint16 size)
{
    FUNCTION_MARKER(0x80033290, "SLUS_003.73");
    this->size = size;
}

uint32 TexturedEffect::EnableSemiTransparency()
{
    uint32 result;
    FUNCTION_MARKER(0x800332A4, "SLUS_003.73");
    result = color | 0x2000000;
    color = result;
    return result;
}

uint32 TexturedEffect::SetBrightness(uint8 intensity)
{
    FUNCTION_MARKER(0x800332D0, "SLUS_003.73");
    color = (color & 0xFF000000) | (intensity << 16) | (intensity << 8) | intensity;
    return color;
}

uint32 TexturedEffect::SetColors(uint8 r, uint8 g, uint8 b)
{
    FUNCTION_MARKER(0x800332FC, "SLUS_003.73");
    color = (color & 0xFF000000) | (b << 16) | (g << 8) | r;
    return color;
}

void TexturedEffect::SetColorStep(sint16 step)
{
    FUNCTION_MARKER(0x8003334C, "SLUS_003.73");
    color_step = step;
}

uint8 TexturedEffect::SelectResource(const char *name)
{
    FUNCTION_MARKER(0x80033354, "SLUS_003.73");
    tex_grp = TextureGroup::Find(name);
    if (!tex_grp)
    {
        GameDiagnostic(("Missing effect texture group\n"));
        abort();
    }
    const uint8 count = tex_grp->count;
    frame = 0;
    frame_fraction = 0;
    frame_count = count;
    tex_frame = 0;
    return count;
}

uint8 TexturedEffect::SelectResource(uint32 resource)
{
    FUNCTION_MARKER(0x80033398, "SLUS_003.73");
    tex_grp = EffectObject::GetResourceGrp(resource);
    if (!tex_grp)
    {
        GameDiagnostic(("Missing effect texture group %u\n", resource));
        abort();
    }
    const uint8 count = tex_grp->count;
    frame = 0;
    frame_fraction = 0;
    frame_count = count;
    tex_frame = 0;
    return count;
}

uint32 TexturedEffect::SetFrame(sint8 frame)
{
    FUNCTION_MARKER(0x800333D0, "SLUS_003.73");
    this->frame = frame;
    frame_fraction = 0;
    tex_frame = this->frame;
    return tex_frame * 8;
}

uint32 TexturedEffect::AdvanceFrame()
{
    FUNCTION_MARKER(0x800333F0, "SLUS_003.73");
    uint16 phase = frame * 256 + frame_fraction + frame_step;
    frame = phase >> 8;
    frame_fraction = phase;
    tex_frame = frame;
    return tex_frame * 8;
}

uint32 TexturedEffect::AdvanceLoopingFrame()
{
    FUNCTION_MARKER(0x80033430, "SLUS_003.73");
    uint16 phase = frame * 256 + frame_fraction + frame_step;
    frame = phase >> 8;
    frame_fraction = phase;
    if (frame >= frame_count)
        frame = 0;
    tex_frame = frame;
    return tex_frame * 8;
}

uint32 TexturedEffect::RandomizeFrame()
{
    FUNCTION_MARKER(0x800334EC, "SLUS_003.73");
    frame = RandomRange(frame_count);
    tex_frame = frame;
    return tex_frame * 8;
}

uint32 TexturedEffect::Shrink(bool finish_when_empty)
{
    FUNCTION_MARKER(0x80033530, "SLUS_003.73");
    if (!size)
    {
        if (finish_when_empty)
            Finish();
        return 1;
    }
    sint16 remaining = size - shrink_step;
    size = remaining < 0 ? 0 : remaining;
    return 0;
}

uint32 TexturedEffect::FadeColors(bool finish_when_empty)
{
    FUNCTION_MARKER(0x80033590, "SLUS_003.73");
    RGBC faded;
    faded.rgbc = color;
    if (!(faded.rgbc & 0xFFFFFF))
    {
        if (finish_when_empty)
            Finish();
        return 1;
    }
    sint32 decrement = color_step;
    uint8 step = color_step;
    faded.r = faded.r < decrement ? 0 : faded.r - step;
    faded.g = faded.g < decrement ? 0 : faded.g - step;
    faded.b = faded.b < decrement ? 0 : faded.b - step;
    color = faded.rgbc;
    return 0;
}

const TextureFrame &TexturedEffect::GetTexFrame() const
{
    return TextureGroup::GetFrame(tex_grp, tex_frame);
}
