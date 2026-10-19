#ifndef EXP_ACT_BURST_H
#define EXP_ACT_BURST_H

#include "effect_object.h"

// TODO Recover the original name for the paired sprite burst owner
// Resource ExpAct from original effect texture slot 9
class AnimatedSprite;

class ExpActBurst : public EffectController
{
public:
    ExpActBurst(const uint32 *position, uint32 count, uint32 spread, uint32 jitter_y, uint32 upward_speed, uint32 gravity, uint32 drag_y, uint32 speed, uint32 acceleration, uint32 drag_xz, uint32 angle_speed, uint32 angle_acceleration, uint32 size_step, uint32 color_step, uint32 fade_start, uint32 frame_step);
    virtual ~ExpActBurst();
    virtual uint32 Destroy(uint32 reason);
    virtual void Update();

private:
    uint16 age;
    ObjectReference<AnimatedSprite> sprites[20];
    uint32 count;
    uint32 fade_start;
};

#endif
