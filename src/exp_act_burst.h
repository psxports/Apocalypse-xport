#ifndef EXP_ACT_BURST_H
#define EXP_ACT_BURST_H

#include "object_reference.h"
#include "vector3.h"
#include "effect_object.h"
#include "effect_controller.h"
#include "animated_sprite.h"

class AnimatedSprite;

// Paired sprites using the ExpAct texture
class ExpActBurst : public EffectController
{
private:
    sint16 age;
    ObjectReference<AnimatedSprite> sprs[20];
    sint32 count;
    sint32 fade_start;

public:
    ExpActBurst(const Vector3 *pos, sint32 count, uint32 spread, uint32 jitter_y, uint32 upward_speed, uint32 gravity, uint8 drag_y, uint32 speed, uint32 acceleration, uint8 drag_xz, uint16 angle_speed, uint16 angle_acceleration, uint16 size_step, sint16 color_step, sint32 fade_start, uint16 frame_step);
    virtual uint32 Destroy(uint32 reason);
    virtual void Update();
    virtual ~ExpActBurst();
};

#endif
