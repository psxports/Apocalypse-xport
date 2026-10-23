#ifndef MOVING_SPRITE_H
#define MOVING_SPRITE_H

#include "vector3.h"
#include "effect_object.h"
#include "sprite.h"

class MovingSprite : public Sprite
{
public:
    virtual ~MovingSprite()
    {
    }

public:
    MovingSprite(const Vector3 *pos, const Vector3 *velocity, const char *name, uint16 size, sint16 shrink_step, sint16 color_step);
    virtual uint32 Destroy(uint32 reason);
    virtual void Update();
};

#endif
