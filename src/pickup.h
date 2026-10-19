#ifndef PICKUP_H
#define PICKUP_H

#include "vector3.h"
#include "visual_object.h"

class Player;
class RingEffect;
class PickupEffect;

class Pickup : public VisualObject
{
public:
    Pickup(uint32 resource, const uint32 *position, const uint32 *velocity, uint32 flags, uint32 lifetime);
    virtual ~Pickup();
    virtual uint32 Destroy(uint32 reason);
    virtual uint32 Finish();
    virtual uint32 ReleaseResources();
    virtual uint32 Update();
    uint32 CreateChildEffect();
    uint32 UpdateMotion();
    void UpdateLifetime();
    uint32 Collect(Player &player);
    void SetMotionParameters(uint32 gravity, uint32 damping_shift);

private:
    friend class TheBeast;
    ObjectReference<RingEffect> ring;
    uint16 ring_radius;
    uint16 ring_wave;
    uint8 spawn_linked;
    uint8 has_model;
    uint8 visible;
    uint8 moving;
    uint8 ignore_floor;
    uint8 floor_offset;
    uint16 spawn_record;
    uint32 floor_y;
    Vector3 base_pos;
    uint16 bob_phase;
    uint16 bob_size;
    uint16 bob_amplitude;
    uint16 bob_step;
    uint16 lifetime;
    ObjectReference<PickupEffect> child;
};
#endif
