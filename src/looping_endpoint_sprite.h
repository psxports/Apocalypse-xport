#ifndef LOOPING_ENDPOINT_SPRITE_H
#define LOOPING_ENDPOINT_SPRITE_H

#include "vector3.h"
#include "effect_object.h"
#include "endpoint_sprite.h"

class LoopingEndpointSprite : public EndpointSprite
{
public:
    virtual ~LoopingEndpointSprite()
    {
    }

public:
    LoopingEndpointSprite();
    virtual uint32 Destroy(uint32 reason);
    virtual void Update();
};

#endif
