#include "sprite.h"
#include "animated_sprite.h"
#include "effect_lifecycle.h"
#include "global.h"

AnimatedSprite::AnimatedSprite(const Vector3 *pos, const char *name, uint16 size, bool semi_transparent, bool finish_on_end, sint32 last) : Sprite()
{
    FUNCTION_MARKER(0x800353A4, "SLUS_003.73");
    this->pos = *pos;
    SelectResource(name);
    this->size = size;
    if (semi_transparent)
        EnableSemiTransparency();
    finish_at_end = finish_on_end;
    if (last == -1)
        last = frame_count - 1;
    last_frame = last;
}

AnimatedSprite::AnimatedSprite(const Vector3 *pos, uint32 resource, uint16 size, bool semi_transparent, bool finish_on_end, sint32 last) : Sprite()
{
    FUNCTION_MARKER(0x80035478, "SLUS_003.73");
    this->pos = *pos;
    SelectResource(resource);
    this->size = size;
    if (semi_transparent)
        EnableSemiTransparency();
    finish_at_end = finish_on_end;
    if (last == -1)
        last = frame_count - 1;
    last_frame = last;
}

uint32 AnimatedSprite::Destroy(uint32 reason)
{
    FUNCTION_MARKER(0x8003554C, "SLUS_003.73");
    uint32 result = Sprite::Destroy(0);
    return (reason & 1) ? ReleaseStorage() : result;
}

void AnimatedSprite::Update()
{
    FUNCTION_MARKER(0x800355A0, "SLUS_003.73");
    AdvanceFrame();
    if (finish_at_end)
    {
        if (frame >= last_frame)
            Finish();
        return;
    }
    if (frame >= frame_count)
        SetFrame(last_frame == -2 ? frame_count - 1 : 0);
}
