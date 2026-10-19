#include <new>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "game_diagnostics.h"

#include "effect_sprite.h"
#include "effect_lifecycle.h"
#include "model_resources.h"
#include "vector_arithmetic.h"
#include "game_random.h"

#include "effect_object.h"

EffectObject::EffectObject() : prev(0), next(0), finished(0), depth_bias(0), persistent(0), kind(0), list_head(0)
{
    for (uint32 axis = 0; axis < 3; ++axis)
    {
        pos.words[axis] = 0;
        velocity.words[axis] = 0;
        acceleration.words[axis] = 0;
        drag[axis] = 0;
    }
    FUNCTION_MARKER(0x80032F7C, "SLUS_003.73");
    this->drag[0] = 1;
    this->drag[1] = 1;
    this->drag[2] = 1;
    uint32 count = effect_count;
    effect_count = count + 1;
}

uint32 EffectObject::Destroy(uint32 reason)
{
    FUNCTION_MARKER(0x80032FB8, "SLUS_003.73");
    uint32 count = effect_count;
    effect_count = count - 1;
    MarkDestroyed();
    if (reason & 1)
        return ReleaseEffectStorage(this);
    return 0x800A1DB8;
}

RenderableEffect::RenderableEffect() : EffectObject()
{
    FUNCTION_MARKER(0x8003304C, "SLUS_003.73");
    AddToList(effect_lists.renderables);
}

uint32 RenderableEffect::Destroy(uint32 reason)
{
    FUNCTION_MARKER(0x80033090, "SLUS_003.73");
    RemoveFromList(effect_lists.renderables);
    uint32 result = EffectObject::Destroy(0);
    if (reason & 1)
        return ReleaseEffectStorage(this);
    return result;
}

EffectController::EffectController() : EffectObject()
{
    FUNCTION_MARKER(0x800330F4, "SLUS_003.73");
    AddToList(effect_lists.controllers);
}

TexturedEffect::TexturedEffect() : EffectObject(), color_step(0), shrink_step(0), color(0), flags(0), frame_count(0), frame(0), frame_fraction(0), frame_step(0), size(0), tex_group(0), tex_frame(0)
{
    FUNCTION_MARKER(0x8003319C, "SLUS_003.73");
    this->frame_step = 128;
    this->size = 400;
    this->color = 0x2C808080;
}

uint32 TexturedEffect::Destroy(uint32 reason)
{
    FUNCTION_MARKER(0x800331EC, "SLUS_003.73");
    uint32 result = EffectObject::Destroy(0);
    return (reason & 1) ? ReleaseEffectStorage(this) : result;
}

void TexturedEffect::SetColorStep(uint32 step)
{
    FUNCTION_MARKER(0x8003334C, "SLUS_003.73");
    this->color_step = step;
}

uint32 EffectObject::SetPosition(const uint32 *position)
{
    FUNCTION_MARKER(0x80032EE4, "SLUS_003.73");
    const uint32 x = position[0];
    const uint32 y = position[1];
    const uint32 z = position[2];
    pos.words[0] = x;
    pos.words[1] = y;
    pos.words[2] = z;
    return x;
}

uint32 TexturedEffect::FadeColors(uint32 finish_when_empty)
{
    FUNCTION_MARKER(0x80033590, "SLUS_003.73");
    uint32 packed, red, green, blue, result;
    sint32 decrement;
    packed = this->color;
    green = packed >> 8;
    if (!(packed & 0xFFFFFF))
    {
        if (finish_when_empty)
            Finish();
        return 1;
    }
    red = (uint8)this->color;
    decrement = this->color_step;
    blue = packed >> 16;
    if ((sint32)red < decrement)
        red = 0;
    else
        red -= (uint8)this->color_step;
    decrement = this->color_step;
    if ((sint32)(green & 255) < decrement)
        green = 0;
    else
        green -= (uint8)this->color_step;
    if ((sint32)(blue & 255) < decrement)
        blue = 0;
    else
        blue -= (uint8)this->color_step;
    result = this->color & 0xFF000000;
    result |= ((blue & 255) << 16) | ((green & 255) << 8) | (red & 255);
    this->color = result;
    return 0;
}

LineEffect::LineEffect() : EffectObject()
{
    FUNCTION_MARKER(0x80032824, "SLUS_003.73");
    color[0] = 0x50808080;
    color[1] = 0x55808080;
    for (uint32 point = 0; point < 2; ++point)
        for (uint32 axis = 0; axis < 3; ++axis)
            endpoint[point].words[axis] = 0;
    AddToList(effect_lists.lines);
}

EffectObject *EffectObject::AddToList(EffectObject *&head)
{
    FUNCTION_MARKER(0x80032E50, "SLUS_003.73");
    list_head = &head;
    prev = 0;
    next = head;
    head = this;
    if (next)
        next->prev = this;
    return next;
}

EffectObject *EffectObject::RemoveFromList(EffectObject *&head)
{
    FUNCTION_MARKER(0x80032E7C, "SLUS_003.73");
    if (next)
        next->prev = prev;
    if (prev)
        prev->next = next;
    if (head == this)
        head = next;
    if (list_head == &head)
        list_head = 0;
    return head;
}

uint32 LineEffect::Destroy(uint32 reason)
{
    uint32 result;
    FUNCTION_MARKER(0x80032880, "SLUS_003.73");
    RemoveFromList(effect_lists.lines);
    result = EffectObject::Destroy(0);
    if (reason & 1)
        result = ReleaseEffectStorage(this);
    return result;
}

uint32 EffectObject::Finish()
{
    FUNCTION_MARKER(0x80032ED8, "SLUS_003.73");
    this->finished = 1;
    return 1;
}

void EffectObject::IntegrateMotion()
{
    FUNCTION_MARKER(0x80032FFC, "SLUS_003.73");
    AddVectors(this->pos.words, this->velocity.words);
    AddVectors(this->velocity.words, this->acceleration.words);
    DampVector(this->velocity.words, this->drag);
}

uint32 EffectController::Destroy(uint32 reason)
{
    uint32 result;
    FUNCTION_MARKER(0x80033138, "SLUS_003.73");
    RemoveFromList(effect_lists.controllers);
    result = EffectObject::Destroy(0);
    if (reason & 1)
        result = ReleaseEffectStorage(this);
    return result;
}

uint32 TexturedEffect::AddFlags(uint32 flags)
{
    FUNCTION_MARKER(0x80033240, "SLUS_003.73");
    uint32 result = this->flags | flags;
    this->flags = result;
    return result;
}

void TexturedEffect::SetFrameStep(uint32 value)
{
    FUNCTION_MARKER(0x80033288, "SLUS_003.73");
    this->frame_step = value;
}

void TexturedEffect::SetSize(uint32 size)
{
    FUNCTION_MARKER(0x80033290, "SLUS_003.73");
    this->size = size;
}

uint32 TexturedEffect::EnableSemiTransparency()
{
    uint32 result;
    FUNCTION_MARKER(0x800332A4, "SLUS_003.73");
    result = this->color | 0x2000000;
    this->color = result;
    return result;
}

uint32 TexturedEffect::SetBrightness(uint32 intensity)
{
    FUNCTION_MARKER(0x800332D0, "SLUS_003.73");
    uint32 color = this->color;
    intensity &= 255;
    color = (color & 0xFF000000) | (intensity << 16) | (intensity << 8) | intensity;
    this->color = color;
    return color;
}

uint32 TexturedEffect::SetColors(uint32 red, uint32 green, uint32 blue)
{
    uint32 result;
    FUNCTION_MARKER(0x800332FC, "SLUS_003.73");
    blue &= 255;
    green &= 255;
    result = this->color & 0xFF000000;
    red &= 255;
    result |= (blue << 16) | (green << 8) | red;
    this->color = result;
    return result;
}

const TextureFrame &TexturedEffect::GetTextureFrame() const
{
    return ::GetTextureFrame(tex_group, tex_frame);
}

uint32 TexturedEffect::SelectResource(const char *name)
{
    FUNCTION_MARKER(0x80033354, "SLUS_003.73");
    tex_group = FindModelTextureGroup(name);
    if (!tex_group)
    {
        GameDiagnostic("Missing effect texture group\n");
        abort();
    }
    const uint32 count = (uint8)tex_group->count;
    this->frame = 0;
    this->frame_fraction = 0;
    this->frame_count = count;
    tex_frame = 0;
    return count;
}

uint32 TexturedEffect::SelectResource(uint32 resource)
{
    FUNCTION_MARKER(0x80033398, "SLUS_003.73");
    tex_group = GetEffectResourceGroup(resource);
    if (!tex_group)
    {
        GameDiagnostic("Missing effect texture group %u\n", resource);
        abort();
    }
    const uint32 count = (uint8)tex_group->count;
    this->frame = 0;
    this->frame_fraction = 0;
    this->frame_count = count;
    tex_frame = 0;
    return count;
}

uint32 TexturedEffect::SetFrame(uint32 frame)
{
    FUNCTION_MARKER(0x800333D0, "SLUS_003.73");
    uint32 index, offset;
    this->frame = frame;
    index = (uint32)(sint32)(sint8)(uint8)this->frame;
    this->frame_fraction = 0;
    offset = index << 3;
    tex_frame = (sint8)(uint8)this->frame;
    return offset;
}

uint32 TexturedEffect::AdvanceFrame()
{
    FUNCTION_MARKER(0x800333F0, "SLUS_003.73");
    uint32 frame, fraction, speed, phase, offset;
    frame = (uint32)(sint32)(sint8)(uint8)this->frame;
    fraction = this->frame_fraction;
    speed = this->frame_step;
    phase = (frame << 8) | fraction;
    phase += speed;
    this->frame = phase >> 8;
    frame = (uint32)(sint32)(sint8)(uint8)this->frame;
    this->frame_fraction = phase;
    offset = frame << 3;
    tex_frame = (sint8)(uint8)this->frame;
    return offset;
}

uint32 TexturedEffect::AdvanceLoopingFrame()
{
    FUNCTION_MARKER(0x80033430, "SLUS_003.73");
    uint32 frame, fraction, speed, phase, count, offset;
    frame = (uint32)(sint32)(sint8)(uint8)this->frame;
    fraction = this->frame_fraction;
    speed = this->frame_step;
    phase = (frame << 8) | fraction;
    phase += speed;
    this->frame = phase >> 8;
    frame = (uint32)(sint32)(sint8)(uint8)this->frame;
    count = this->frame_count;
    this->frame_fraction = phase;
    if ((sint32)frame >= (sint32)count)
        this->frame = 0;
    frame = (uint32)(sint32)(sint8)(uint8)this->frame;
    offset = frame << 3;
    tex_frame = (sint8)(uint8)this->frame;
    return offset;
}

uint32 TexturedEffect::RandomizeFrame()
{
    FUNCTION_MARKER(0x800334EC, "SLUS_003.73");
    uint32 frame = RandomRange(this->frame_count);
    this->frame = frame;
    uint32 offset = (uint32)((sint32)(frame << 24) >> 21);
    tex_frame = (sint8)(uint8)this->frame;
    return offset;
}

uint32 TexturedEffect::Shrink(uint32 finish_when_empty)
{
    FUNCTION_MARKER(0x80033530, "SLUS_003.73");
    sint32 signed_size;
    uint32 size, step;
    signed_size = (sint16)this->size;
    size = this->size;
    if (!signed_size)
    {
        if (finish_when_empty)
            Finish();
        return 1;
    }
    step = this->shrink_step;
    size -= step;
    this->size = size;
    if (size & 32768)
        this->size = 0;
    return 0;
}

PrimitiveParticle::PrimitiveParticle() : EffectObject()
{
    FUNCTION_MARKER(0x80034EF4, "SLUS_003.73");
    memset(&primitive, 0, sizeof(primitive));
    AddToList(effect_lists.particles);
}

uint32 PrimitiveParticle::Destroy(uint32 reason)
{
    uint32 result;
    FUNCTION_MARKER(0x80034F38, "SLUS_003.73");
    RemoveFromList(effect_lists.particles);
    result = EffectObject::Destroy(0);
    return (reason & 1) ? ReleaseEffectStorage(this) : result;
}

void EffectObject::Update()
{
    FUNCTION_MARKER(0x8003A144, "SLUS_003.73");
}

uint32 RenderableEffect::Render()
{
    FUNCTION_MARKER(0x8003A13C, "SLUS_003.73");
    // Original no-op has no defined return carrier
    return 0;
}

EffectObject::~EffectObject()
{
    if (list_head)
        RemoveFromList(*list_head);
}
