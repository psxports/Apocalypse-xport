#ifndef OPAQUE_FRAGMENT_H
#define OPAQUE_FRAGMENT_H

#include "effect_object.h"
#include "vector3.h"

// Untextured triangle fragments with independent bounce and color fade
class OpaqueFragment : public EffectObject
{
private:
    uint16 bounce_count;
    uint16 lifetime;

public:
    Vector3 vertices[3];

private:
    sint32 floor_y;
    RGBC base_color;

public:
    RGBC draw_color;

private:
    uint8 fade_step;

public:
    virtual ~OpaqueFragment()
    {
    }

public:
    OpaqueFragment(const Vector3 &pos, const Vector3 &velocity, sint32 floor, uint8 r, uint8 g, uint8 b, uint32 extent_x, uint32 extent_y, uint32 extent_z);
    virtual uint32 Destroy(uint32 reason);
    virtual void Update();
};

#endif
