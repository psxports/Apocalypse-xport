#ifndef OPAQUE_FRAGMENT_H
#define OPAQUE_FRAGMENT_H

#include "effect_object.h"

// TODO Recover the original name for the untextured fragment owner
class OpaqueFragment : public EffectObject
{
public:
    OpaqueFragment(const uint32 *position, const uint32 *velocity, uint32 floor, uint32 red, uint32 green, uint32 blue, uint32 extent_x, uint32 extent_y, uint32 extent_z);
    virtual ~OpaqueFragment() {}
    static OpaqueFragment &Attach(uint32 object);
    virtual uint32 Destroy(uint32 reason);
    virtual uint32 Update();
};

#endif
