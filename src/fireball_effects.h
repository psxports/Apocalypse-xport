#ifndef FIREBALL_EFFECTS_H
#define FIREBALL_EFFECTS_H

#include "effect_sprite.h"

class Character;

class Fireball : public Sprite
{
public:
    Fireball(Character &source, sint16 bone);

    virtual ~Fireball()
    {
    }

    virtual uint32 Destroy(uint32 reason);
    virtual void Update();

private:
    ObjectReference<Character> source;
    uint8 rise_speed;
    uint8 intensity;
};

class Smoke : public Sprite
{
public:
    Smoke(const uint32 *position);

    virtual ~Smoke()
    {
    }

    virtual uint32 Destroy(uint32 reason);
    virtual void Update();

private:
    uint8 rise_speed;
};

#endif
