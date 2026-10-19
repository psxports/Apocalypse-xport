#ifndef EFFECT_SPRITE_H
#define EFFECT_SPRITE_H

#include "effect_object.h"

class Sprite : public TexturedEffect
{
public:
    uint16 age;
    uint16 lifetime;
    uint16 angle;
    uint16 angle_speed;
    uint16 angle_accel;
    uint16 scale[2];
    uint8 forced_depth;
    uint8 blend_mode;
    uint16 clut;

    Sprite();

    virtual ~Sprite()
    {
    }

    virtual uint32 Destroy(uint32 reason);
};

// TODO Recover the original names for the two-endpoint sprite family
class EndpointSprite : public TexturedEffect
{
public:
    Vector3 endpoint[2];

    EndpointSprite();

    virtual ~EndpointSprite()
    {
    }

    virtual uint32 Destroy(uint32 reason);
};

class LoopingEndpointSprite : public EndpointSprite
{
public:
    LoopingEndpointSprite();

    virtual ~LoopingEndpointSprite()
    {
    }

    virtual uint32 Destroy(uint32 reason);
    virtual void Update();
};

class AnimatedSprite : public Sprite
{
public:
    AnimatedSprite(const uint32 *position, const char *name, uint32 size, uint32 initialize, uint32 first, uint32 last);
    AnimatedSprite(const uint32 *position, uint32 resource, uint32 size, uint32 initialize, uint32 first, uint32 last);

    virtual ~AnimatedSprite()
    {
    }

    virtual uint32 Destroy(uint32 reason);
    virtual void Update();

private:
    uint32 finish_at_end;
    uint32 last_frame;
};

class MovingSprite : public Sprite
{
public:
    MovingSprite(const uint32 *position, const uint32 *velocity, const char *name, uint32 size, uint32 lifetime, uint32 mode);

    virtual ~MovingSprite()
    {
    }

    virtual uint32 Destroy(uint32 reason);
    virtual void Update();
};

class ParticleSprite : public Sprite
{
public:
    ParticleSprite(const uint32 *position, uint32 red, uint32 green, uint32 blue, uint32 resource, uint32 size, uint32 initialize, uint32 spread, uint32 acceleration, uint32 lifetime);

    virtual ~ParticleSprite()
    {
    }

    virtual uint32 Destroy(uint32 reason);
    virtual void Update();
};

#endif
