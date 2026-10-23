#ifndef FIREBALL_H
#define FIREBALL_H

#include "object_reference.h"
#include "vector3.h"
#include "sprite.h"

class Character;

class Fireball : public Sprite
{
public:
    virtual ~Fireball()
    {
    }

private:
    ObjectReference<Character> src;
    uint8 rise_speed;
    uint8 intensity;

public:
    Fireball(Character &src, sint16 bone);
    virtual uint32 Destroy(uint32 reason);
    virtual void Update();
};

#endif
