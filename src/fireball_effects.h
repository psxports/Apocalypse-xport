#ifndef FIREBALL_EFFECTS_H
#define FIREBALL_EFFECTS_H

#include "effect_sprite.h"

class Fireball : public Sprite
{
public:
    Fireball(uint32 source, sint16 bone);
    virtual ~Fireball() {}
    static Fireball &Attach(uint32 object);
    virtual uint32 Destroy(uint32 reason);
    virtual uint32 Update();
};

class Smoke : public Sprite
{
public:
    Smoke(const uint32 *position);
    virtual ~Smoke() {}
    static Smoke &Attach(uint32 object);
    virtual uint32 Destroy(uint32 reason);
    virtual uint32 Update();
};

#endif
