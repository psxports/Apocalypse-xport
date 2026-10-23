#ifndef PICKUP_H
#define PICKUP_H

#include "object_reference.h"
#include "vector3.h"
#include "visual_object.h"
#include "pickup_effect.h"

class Player;
class RingEffect;
class PickupEffect;

class Pickup : public VisualObject
{
private:
    friend class TheBeast;
    ObjectReference<RingEffect> ring;
    uint16 ring_radius;
    uint16 ring_wave;
    uint8 spawn_linked;
    bool has_model;
    bool visible;
    bool moving;
    bool ignore_floor;
    uint8 floor_offset;
    uint16 spawn_record;
    sint32 floor_y;
    Vector3 base_pos;
    uint16 bob_phase;
    uint16 bob_size;
    uint16 bob_amplitude;
    uint16 bob_step;
    uint16 lifetime;
    ObjectReference<PickupEffect> child;

public:
    Pickup(uint16 resource, const Vector3 *pos, const Vector3 *velocity, uint32 flags, uint16 lifetime);
    void SetMotionParameters(sint32 gravity, uint8 damping_shift);

private:
    uint32 CreateChildEffect();

public:
    virtual uint32 ReleaseResources();
    virtual uint32 Destroy(uint32 reason);
    virtual uint32 Finish();

private:
    uint32 UpdateMotion();
    void UpdateLifetime();

public:
    uint32 Collect(Player &player);
    virtual uint32 Update();
    static Pickup *Spawn(uint16 resource, const Vector3 *pos, uint32 flags, uint16 lifetime, const Vector3 *velocity);
    virtual ~Pickup();
};
#endif
