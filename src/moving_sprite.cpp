#include "sprite.h"
#include "moving_sprite.h"
#include "effect_lifecycle.h"
#include "global.h"

MovingSprite::MovingSprite(const Vector3 *pos, const Vector3 *velocity, const char *name, uint16 size, sint16 shrink_step, sint16 color_step) : Sprite()
{
    FUNCTION_MARKER(0x80035638, "SLUS_003.73");
    this->pos = *pos;
    this->velocity = *velocity;
    SelectResource(name);
    EnableSemiTransparency();
    this->size = size;
    this->shrink_step = shrink_step;
    this->color_step = color_step;
}

uint32 MovingSprite::Destroy(uint32 reason)
{
    FUNCTION_MARKER(0x80035704, "SLUS_003.73");
    uint32 result = Sprite::Destroy(0);
    return (reason & 1) ? ReleaseStorage() : result;
}

void MovingSprite::Update()
{
    FUNCTION_MARKER(0x80035758, "SLUS_003.73");
    pos.Add(velocity);
    if (!Shrink(1) && !FadeColors(1))
        AdvanceLoopingFrame();
}
