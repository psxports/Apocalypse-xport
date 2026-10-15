#ifndef EFFECT_SPRITE_H
#define EFFECT_SPRITE_H

#include "effect_object.h"

class Sprite : public TexturedEffect
{
public:
    Sprite();
    virtual ~Sprite() {}
    static Sprite &Attach(uint32 object);
    virtual uint32 Destroy(uint32 reason);
};

// TODO Recover the original names for the two-endpoint sprite family
class EndpointSprite : public TexturedEffect
{
public:
    EndpointSprite();
    virtual ~EndpointSprite() {}
    static EndpointSprite &Attach(uint32 object);
    virtual uint32 Destroy(uint32 reason);
};

class LoopingEndpointSprite : public EndpointSprite
{
public:
    LoopingEndpointSprite();
    virtual ~LoopingEndpointSprite() {}
    static LoopingEndpointSprite &Attach(uint32 object);
    virtual uint32 Destroy(uint32 reason);
    virtual uint32 Update();
};

class AnimatedSprite : public Sprite
{
public:
    AnimatedSprite(const uint32 *position, const char *name, uint32 size, uint32 initialize, uint32 first, uint32 last);
    AnimatedSprite(const uint32 *position, uint32 resource, uint32 size, uint32 initialize, uint32 first, uint32 last);
    virtual ~AnimatedSprite() {}
    static AnimatedSprite &Attach(uint32 object);
    virtual uint32 Destroy(uint32 reason);
    virtual uint32 Update();
};

class MovingSprite : public Sprite
{
public:
    MovingSprite(const uint32 *position, const uint32 *velocity, const char *name, uint32 size, uint32 lifetime, uint32 mode);
    virtual ~MovingSprite() {}
    static MovingSprite &Attach(uint32 object);
    virtual uint32 Destroy(uint32 reason);
    virtual uint32 Update();
};

class ParticleSprite : public Sprite
{
public:
    ParticleSprite(const uint32 *position, uint32 red, uint32 green, uint32 blue, uint32 resource, uint32 size, uint32 initialize, uint32 spread, uint32 acceleration, uint32 lifetime);
    virtual ~ParticleSprite() {}
    static ParticleSprite &Attach(uint32 object);
    virtual uint32 Destroy(uint32 reason);
    virtual uint32 Update();
};

#endif
