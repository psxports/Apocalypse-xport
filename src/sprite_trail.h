#ifndef SPRITE_TRAIL_H
#define SPRITE_TRAIL_H

#include "object_reference.h"
#include "vector3.h"
#include "effect_object.h"
#include "effect_controller.h"
#include "looping_endpoint_sprite.h"

class LoopingEndpointSprite;

// Owns sampled positions and their linked endpoint sprites
class SpriteTrail : public EffectController
{
protected:
    sint32 segment_count;

private:
    sint32 sample_step;
    sint32 sample_count;
    sint32 cur_sample;
    Vector3 *history;

protected:
    typedef ObjectReference<LoopingEndpointSprite> ChildReference;
    ChildReference *children;

public:
    SpriteTrail(const Vector3 *pos, uint32 segs, uint32 spacing, const char *body_name, const char *head_name, uint16 width, bool enabled);
    virtual uint32 Destroy(uint32 flags);
    uint32 SetColors(uint8 r, uint8 g, uint8 b);
    uint32 SetWidth(uint16 width);
    void SetPos(const Vector3 *pos);
    virtual ~SpriteTrail();
};

#endif
