#ifndef SPRITE_TRAIL_H
#define SPRITE_TRAIL_H

#include "effect_object.h"

class LoopingEndpointSprite;

// TODO Recover the original base name for the trail history owner
class SpriteTrail : public EffectController
{
public:
    SpriteTrail(const uint32 *position, uint32 segments, uint32 spacing, const char *body_name, const char *head_name, uint32 width, uint32 enabled);
    virtual ~SpriteTrail();
    virtual uint32 Destroy(uint32 reason);
    uint32 SetColors(uint32 red, uint32 green, uint32 blue);
    uint32 SetWidth(uint32 width);
    void SetPosition(const uint32 *position);

protected:
    uint32 segment_count;
    uint32 sample_step;
    uint32 sample_count;
    uint32 cur_sample;
    uint32 (*history)[3];
    typedef ObjectReference<LoopingEndpointSprite> ChildReference;
    ChildReference *children;
};

class RibbonTrail : public SpriteTrail
{
public:
    RibbonTrail(const uint32 *position, uint32 segments, uint32 red, uint32 green, uint32 blue);
    uint32 fading;

    virtual ~RibbonTrail()
    {
    }

    virtual uint32 Destroy(uint32 reason);
    virtual void Update();
};

#endif
