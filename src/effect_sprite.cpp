#include <new>

#include "game_diagnostics.h"
#include "effect_sprite.h"
#include "effect_lifecycle.h"
#include "model_resources.h"
#include "vector_arithmetic.h"
#include "game_random.h"

Sprite::Sprite() : TexturedEffect(), age(0), lifetime(0), angle(0), angle_speed(0), angle_accel(0), forced_depth(0), blend_mode(32), clut(0)
{
    FUNCTION_MARKER(0x80034D88, "SLUS_003.73");
    AddToList(effect_lists.sprites);
    scale[0] = 4096;
    scale[1] = 4096;
}

uint32 Sprite::Destroy(uint32 reason)
{
    FUNCTION_MARKER(0x80034DE8, "SLUS_003.73");
    RemoveFromList(effect_lists.sprites);
    uint32 result = TexturedEffect::Destroy(0);
    return (reason & 1) ? ReleaseEffectStorage(this) : result;
}

EndpointSprite::EndpointSprite() : TexturedEffect()
{
    for (uint32 end = 0; end < 2; ++end)
        for (uint32 axis = 0; axis < 3; ++axis)
            endpoint[end].words[axis] = 0;
    FUNCTION_MARKER(0x80034E4C, "SLUS_003.73");
    AddToList(effect_lists.endpoint_sprites);
}

uint32 EndpointSprite::Destroy(uint32 reason)
{
    FUNCTION_MARKER(0x80034E90, "SLUS_003.73");
    RemoveFromList(effect_lists.endpoint_sprites);
    uint32 result = TexturedEffect::Destroy(0);
    return (reason & 1) ? ReleaseEffectStorage(this) : result;
}

AnimatedSprite::AnimatedSprite(const uint32 *position, const char *name, uint32 size, uint32 initialize, uint32 first, uint32 last) : Sprite()
{
    uint32 x, y, z;
    FUNCTION_MARKER(0x800353A4, "SLUS_003.73");
    x = position[0];
    y = position[1];
    z = position[2];
    this->pos.words[0] = x;
    this->pos.words[1] = y;
    this->pos.words[2] = z;
    SelectResource(name);
    this->size = size;
    if (initialize)
        EnableSemiTransparency();
    finish_at_end = first;
    if (last == (uint32)-1)
        last = this->frame_count - 1;
    last_frame = last;
}

AnimatedSprite::AnimatedSprite(const uint32 *position, uint32 resource, uint32 size, uint32 initialize, uint32 first, uint32 last) : Sprite()
{
    uint32 x, y, z;
    FUNCTION_MARKER(0x80035478, "SLUS_003.73");
    x = position[0];
    y = position[1];
    z = position[2];
    this->pos.words[0] = x;
    this->pos.words[1] = y;
    this->pos.words[2] = z;
    SelectResource(resource);
    this->size = size;
    if (initialize)
        EnableSemiTransparency();
    finish_at_end = first;
    if (last == (uint32)-1)
        last = this->frame_count - 1;
    last_frame = last;
}

uint32 AnimatedSprite::Destroy(uint32 reason)
{
    FUNCTION_MARKER(0x8003554C, "SLUS_003.73");
    uint32 result = Sprite::Destroy(0);
    return (reason & 1) ? ReleaseEffectStorage(this) : result;
}

void AnimatedSprite::Update()
{
    FUNCTION_MARKER(0x800355A0, "SLUS_003.73");
    AdvanceFrame();
    if (finish_at_end)
    {
        if (frame >= (sint32)last_frame)
            Finish();
        return;
    }
    if (frame >= frame_count)
        SetFrame(last_frame == (uint32)-2 ? frame_count - 1 : 0);
}

MovingSprite::MovingSprite(const uint32 *position, const uint32 *velocity, const char *name, uint32 size, uint32 lifetime, uint32 mode) : Sprite()
{
    uint32 x, y, z;
    FUNCTION_MARKER(0x80035638, "SLUS_003.73");
    x = position[0];
    y = position[1];
    z = position[2];
    this->pos.words[0] = x;
    this->pos.words[1] = y;
    this->pos.words[2] = z;
    x = velocity[0];
    y = velocity[1];
    z = velocity[2];
    this->velocity.words[0] = x;
    this->velocity.words[1] = y;
    this->velocity.words[2] = z;
    SelectResource(name);
    EnableSemiTransparency();
    this->size = size;
    this->shrink_step = lifetime;
    this->color_step = mode;
}

uint32 MovingSprite::Destroy(uint32 reason)
{
    FUNCTION_MARKER(0x80035704, "SLUS_003.73");
    uint32 result = Sprite::Destroy(0);
    return (reason & 1) ? ReleaseEffectStorage(this) : result;
}

void MovingSprite::Update()
{
    FUNCTION_MARKER(0x80035758, "SLUS_003.73");
    AddVectors(pos.words, velocity.words);
    if (!Shrink(1) && !FadeColors(1))
        AdvanceLoopingFrame();
}

ParticleSprite::ParticleSprite(const uint32 *position, uint32 red, uint32 green, uint32 blue, uint32 resource, uint32 size, uint32 initialize, uint32 spread, uint32 acceleration, uint32 lifetime) : Sprite()
{
    uint32 x, y, z, random, range;
    FUNCTION_MARKER(0x800358B4, "SLUS_003.73");
    x = position[0];
    y = position[1];
    z = position[2];
    this->pos.words[0] = x;
    this->pos.words[1] = y;
    this->pos.words[2] = z;
    SelectResource(resource);
    if (initialize)
        EnableSemiTransparency();
    SetColors(red & 255, green & 255, blue & 255);
    range = (spread << 1) + 1;
    this->size = size;
    random = RandomRange(range);
    this->velocity.words[0] = (random - spread) << 12;
    random = RandomRange(spread);
    this->velocity.words[1] = 0 - (random << 12);
    random = RandomRange(range);
    this->velocity.words[2] = (random - spread) << 12;
    this->acceleration.words[1] = acceleration;
    this->drag[0] = 3;
    this->drag[1] = 3;
    this->drag[2] = 3;
    this->lifetime = RandomRange(lifetime);
}

uint32 ParticleSprite::Destroy(uint32 reason)
{
    FUNCTION_MARKER(0x80035A00, "SLUS_003.73");
    uint32 result = Sprite::Destroy(0);
    return (reason & 1) ? ReleaseEffectStorage(this) : result;
}

void ParticleSprite::Update()
{
    FUNCTION_MARKER(0x80035A54, "SLUS_003.73");
    pos.words[0] += velocity.words[0];
    pos.words[1] += velocity.words[1];
    pos.words[2] += velocity.words[2];
    velocity.words[0] += acceleration.words[0];
    velocity.words[2] += acceleration.words[2];
    velocity.words[1] += acceleration.words[1];
    velocity.words[0] -= (uint32)((sint32)velocity.words[0] >> (drag[0] & 31));
    velocity.words[2] -= (uint32)((sint32)velocity.words[2] >> (drag[2] & 31));
    velocity.words[1] -= (uint32)((sint32)velocity.words[1] >> (drag[1] & 31));
    // Frame and age comparisons use signed byte and halfword loads
    frame = (uint8)frame + 1;
    if (frame >= frame_count)
        frame = frame_count - 1;
    ++age;
    if ((sint16)age >= lifetime)
        Finish();
}

LoopingEndpointSprite::LoopingEndpointSprite() : EndpointSprite()
{
    FUNCTION_MARKER(0x8003658C, "SLUS_003.73");
}

uint32 LoopingEndpointSprite::Destroy(uint32 reason)
{
    FUNCTION_MARKER(0x800365C0, "SLUS_003.73");
    uint32 result = EndpointSprite::Destroy(0);
    return (reason & 1) ? ReleaseEffectStorage(this) : result;
}

void LoopingEndpointSprite::Update()
{
    FUNCTION_MARKER(0x80036614, "SLUS_003.73");
    AdvanceLoopingFrame();
}
