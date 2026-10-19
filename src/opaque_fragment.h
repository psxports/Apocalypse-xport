#ifndef OPAQUE_FRAGMENT_H
#define OPAQUE_FRAGMENT_H

#include "effect_object.h"
#include "vector3.h"

// TODO Recover the original name for the untextured fragment owner
class OpaqueFragment : public EffectObject
{
public:
    uint16 bounce_count;
    uint16 lifetime;
    Vector3 vertices[3];
    sint32 floor_y;
    RGBC base_color;
    RGBC draw_color;
    uint8 fade_step;

    OpaqueFragment(const Vector3 &position, const Vector3 &velocity, uint32 floor, uint32 red, uint32 green, uint32 blue, uint32 extent_x, uint32 extent_y, uint32 extent_z);

    virtual ~OpaqueFragment()
    {
    }

    virtual uint32 Destroy(uint32 reason);
    virtual void Update();
};

#endif
