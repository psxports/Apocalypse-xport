#ifndef PROJECTILE_LINE_H
#define PROJECTILE_LINE_H

#include "vector3.h"

#include "effect_object.h"
#include "collision_ray.h"

class RingEffect;

void CreateProjectileImpact(uint32 flags, const uint32 *position);

// TODO: Recover the original resource/class name for this short-lived tracer
class ProjectileTracer : public LineEffect
{
public:
    ProjectileTracer(const uint32 *start, const uint32 *end);

    virtual ~ProjectileTracer()
    {
    }

    virtual uint32 Destroy(uint32 reason);
    virtual void Update();

private:
    uint32 age;
};

class LineProjectile : public RenderableEffect
{
public:
    LineProjectile(const uint32 *input_position, const sint16 *input_angles, uint32 speed, uint32 damage, ModelInstance **owner_list, uint32 red, uint32 green, uint32 blue, uint32 mode);
    virtual ~LineProjectile();
    virtual uint32 Destroy(uint32 reason);
    virtual void Update();
    virtual uint32 Render();
    uint32 Finish();
    uint32 InitializeTrace();

    RGBC rgb;

private:
    uint16 age;
    uint16 lifetime;
    ModelInstance **owner_list;
    uint16 damage;
    uint32 speed;
    Vector3 origin;
    Vector3 dir;
    Vector3 tail_pos;
    ObjectReference<RingEffect> ring;
    uint8 mode;
    CollisionRay trace;
    // Preserve original signed DIV corner cases
    static uint32 DivideLifetime(uint32 numerator, uint32 denominator);
};

#endif
