#ifndef ANIMATED_SPRITE_H
#define ANIMATED_SPRITE_H

#include "vector3.h"
#include "effect_object.h"
#include "sprite.h"

class AnimatedSprite : public Sprite
{
public:
    virtual ~AnimatedSprite()
    {
    }

private:
    bool finish_at_end;
    sint32 last_frame;

public:
    AnimatedSprite(const Vector3 *pos, const char *name, uint16 size, bool semi_transparent, bool finish_on_end, sint32 last);
    AnimatedSprite(const Vector3 *pos, uint32 resource, uint16 size, bool semi_transparent, bool finish_on_end, sint32 last);
    virtual uint32 Destroy(uint32 reason);
    virtual void Update();
};

#endif
