#ifndef ENDPOINT_SPRITE_H
#define ENDPOINT_SPRITE_H

#include "vector3.h"
#include "effect_object.h"
#include "textured_effect.h"

class SpriteTrail;

// Sprites with two independently positioned endpoints
class EndpointSprite : public TexturedEffect
{
public:
    Vector3 endpoint[2];
    SpriteTrail *hd_trail;

    virtual ~EndpointSprite()
    {
    }

public:
    EndpointSprite();
    virtual uint32 Destroy(uint32 reason);
};

#endif
