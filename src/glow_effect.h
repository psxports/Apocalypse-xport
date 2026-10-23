#ifndef GLOW_EFFECT_H
#define GLOW_EFFECT_H

#include "vector3.h"
#include "ring_effect.h"

// Procedural ring glow with independent radius and color fade
class GlowEffect : public RingEffect
{
public:
    virtual ~GlowEffect()
    {
    }

private:
    uint8 rgb[3];

protected:
    sint32 fade;

private:
    uint32 radius;
    uint32 growth;

public:
    GlowEffect(const Vector3 *pos, uint8 r, uint8 g, uint8 b, uint32 growth, sint32 fade, uint32 size, uint32 segs);
    virtual uint32 Destroy(uint32 reason);
    virtual void Update();
};

#endif
