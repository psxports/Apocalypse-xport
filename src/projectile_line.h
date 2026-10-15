#ifndef PROJECTILE_LINE_H
#define PROJECTILE_LINE_H

#include "effect_object.h"

uint32 CreateProjectileImpact(uint32 flags, const uint32 *position);

// TODO: Recover the original resource/class name for this short-lived tracer
class ProjectileTracer : public LineEffect
{
public:
    ProjectileTracer(const uint32 *start, const uint32 *end);
    static ProjectileTracer &Attach(uint32 object);
    virtual ~ProjectileTracer() {}
    virtual uint32 Destroy(uint32 reason);
    virtual void Update();
};

class LineProjectile : public RenderableEffect
{
public:
    LineProjectile(const uint32 *input_position, const sint16 *input_angles, uint32 speed, uint32 damage, const uint32 *owner_pointer, uint32 red, uint32 green, uint32 blue, uint32 mode);
    virtual ~LineProjectile() {}
    static LineProjectile &Attach(uint32 object);
    virtual uint32 Destroy(uint32 reason);
    virtual uint32 Update();
    virtual uint32 Render();
    uint32 Finish();
    uint32 InitializeTrace();

private:
    // Native helpers retain original unaligned loads and DIV corner cases
    static uint32 ReadAnglesWord(uint32 address, const void *native_angles);
    static uint32 ReadAnglesTail(uint32 address, const void *native_angles);
    static uint32 DivideLifetime(uint32 numerator, uint32 denominator);
};

#endif
