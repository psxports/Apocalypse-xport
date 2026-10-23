#ifndef PARTICLE_SPRITE_H
#define PARTICLE_SPRITE_H

#include "vector3.h"
#include "effect_object.h"
#include "sprite.h"

class ParticleSprite : public Sprite
{
public:
    virtual ~ParticleSprite()
    {
    }

public:
    ParticleSprite(const Vector3 *pos, uint8 r, uint8 g, uint8 b, uint32 resource, uint16 size, bool semi_transparent, uint32 spread, uint32 acceleration, uint32 lifetime);
    virtual uint32 Destroy(uint32 reason);
    virtual void Update();
};

#endif
