#ifndef RIBBON_TRAIL_H
#define RIBBON_TRAIL_H

#include "vector3.h"
#include "effect_object.h"
#include "sprite_trail.h"

class LoopingEndpointSprite;

class RibbonTrail : public SpriteTrail
{
public:
    bool fading;

    virtual ~RibbonTrail()
    {
    }

public:
    RibbonTrail(const Vector3 *pos, uint32 segs, uint32 r, uint32 g, uint32 b);
    virtual uint32 Destroy(uint32 reason);
    virtual void Update();
};

#endif
