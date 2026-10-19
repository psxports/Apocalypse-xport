#ifndef GLOW_EFFECT_H
#define GLOW_EFFECT_H

#include "ring_effect.h"

// TODO Recover the original class names from resource evidence
class GlowEffect : public RingEffect
{
public:
    GlowEffect(const uint32 *position, uint32 red, uint32 green, uint32 blue, uint32 growth, uint32 fade, uint32 size, uint32 segments);

    virtual ~GlowEffect()
    {
    }

    virtual uint32 Destroy(uint32 reason);
    virtual void Update();

protected:
    uint8 rgb[3];
    uint32 fade;
    uint32 radius;
    uint32 growth;
};

class ExpandingGlow : public GlowEffect
{
public:
    ExpandingGlow(const uint32 *position, uint32 red, uint32 green, uint32 blue, uint32 size, uint32 duration);

    virtual ~ExpandingGlow()
    {
    }

    virtual uint32 Destroy(uint32 reason);
    virtual void Update();

private:
    uint32 extent_step;
};

#endif
