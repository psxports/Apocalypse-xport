#ifndef EXPANDING_GLOW_H
#define EXPANDING_GLOW_H

#include "vector3.h"
#include "ring_effect.h"
#include "glow_effect.h"

class ExpandingGlow : public GlowEffect
{
public:
    virtual ~ExpandingGlow()
    {
    }

private:
    sint32 extent_step;

public:
    ExpandingGlow(const Vector3 *pos, uint8 r, uint8 g, uint8 b, sint32 size, sint32 duration);
    virtual uint32 Destroy(uint32 reason);
    virtual void Update();
};

#endif
