#ifndef PICKUP_H
#define PICKUP_H
#include "visual_object.h"

class Player;

class Pickup : public VisualObject
{
public:
    Pickup(uint32 resource, const uint32 *position, const uint32 *velocity, uint32 flags, uint32 yaw);
    virtual ~Pickup() {}
    virtual uint32 Destroy(uint32 reason);
    virtual uint32 Finish();
    virtual uint32 ReleaseEffects();
    virtual uint32 AdvanceEffects();
    uint32 CreateChildEffect();
    uint32 UpdateMotion();
    void UpdateLifetime();
    uint32 Collect(Player &player);
    void SetMotionParameters(uint32 gravity, uint32 damping_shift);
    static Pickup &Attach(uint32 object);
};
#endif
