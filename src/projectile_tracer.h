#ifndef PROJECTILE_TRACER_H
#define PROJECTILE_TRACER_H

#include "vector3.h"
#include "effect_object.h"
#include "line_effect.h"
#include "collision_ray.h"
#include "projectile_line.h"

class RingEffect;

// TODO Recover the original resource/class name for this short-lived tracer
class ProjectileTracer : public LineEffect
{
public:
    virtual ~ProjectileTracer()
    {
    }

private:
    uint32 age;

public:
    ProjectileTracer(const Vector3 *start, const Vector3 *end);
    virtual void Update();
    virtual uint32 Destroy(uint32 reason);
};

#endif
