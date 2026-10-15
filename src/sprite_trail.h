#ifndef SPRITE_TRAIL_H
#define SPRITE_TRAIL_H

#include "effect_object.h"

// TODO Recover the original base name for the trail history owner
class SpriteTrail : public EffectController
{
public:
    SpriteTrail(const uint32 *position, uint32 segments, uint32 spacing, const char *body_name, const char *head_name, uint32 width, uint32 enabled);
    virtual ~SpriteTrail() {}
    static SpriteTrail &Attach(uint32 object);
    virtual uint32 Destroy(uint32 reason);
    uint32 SetColors(uint32 red, uint32 green, uint32 blue);
    uint32 SetWidth(uint32 width);
    uint32 SetPosition(const uint32 *position);
};

class RibbonTrail : public SpriteTrail
{
public:
    RibbonTrail(const uint32 *position, uint32 segments, uint32 red, uint32 green, uint32 blue);
    virtual ~RibbonTrail() {}
    static RibbonTrail &Attach(uint32 object);
    virtual uint32 Destroy(uint32 reason);
    virtual uint32 Update();
};

#endif
