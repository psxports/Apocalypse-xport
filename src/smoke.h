#ifndef SMOKE_H
#define SMOKE_H

#include "vector3.h"
#include "sprite.h"

class Character;

class Smoke : public Sprite
{
public:
    virtual ~Smoke()
    {
    }

private:
    uint8 rise_speed;

public:
    Smoke(const Vector3 *pos);
    virtual uint32 Destroy(uint32 reason);
    virtual void Update();
};

#endif
