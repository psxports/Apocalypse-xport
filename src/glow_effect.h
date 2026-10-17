#ifndef GLOW_EFFECT_H
#define GLOW_EFFECT_H

#include "ring_effect.h"

// TODO Recover the original class names from resource evidence
class GlowEffect : public RingEffect
{
public:
    GlowEffect(const uint32 *position, uint32 red, uint32 green, uint32 blue, uint32 growth, uint32 fade, uint32 size, uint32 segments);
    virtual ~GlowEffect() {}
    static GlowEffect &Attach(uint32 object);
    virtual uint32 Destroy(uint32 reason);
    virtual void Update();
};

class ExpandingGlow : public GlowEffect
{
public:
    ExpandingGlow(const uint32 *position, uint32 red, uint32 green, uint32 blue, uint32 size, uint32 duration);
    virtual ~ExpandingGlow() {}
    static ExpandingGlow &Attach(uint32 object);
    virtual uint32 Destroy(uint32 reason);
    virtual void Update();
};

#endif
