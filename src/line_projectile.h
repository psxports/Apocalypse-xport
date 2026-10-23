#ifndef LINE_PROJECTILE_H
#define LINE_PROJECTILE_H

#include "object_reference.h"
#include "vector3.h"
#include "effect_object.h"
#include "renderable_effect.h"
#include "collision_ray.h"
#include "projectile_line.h"

class RingEffect;

class LineProjectile : public RenderableEffect
{
private:
    sint16 age;
    uint16 lifetime;
    ModelInstance **owner_list;
    uint16 damage;
    uint32 speed;
    Vector3 origin;
    Vector3 dir;
    Vector3 tail_pos;
    ObjectReference<RingEffect> ring;
    CollisionRay trace;
    bool mode;

public:
    RGBC rgb;

public:
    LineProjectile(const Vector3 *input_pos, const ShortVector3 *input_angles, uint32 speed, uint16 damage, ModelInstance **owner_list, uint8 r, uint8 g, uint8 b, uint8 mode);
    virtual uint32 Destroy(uint32 reason);

private:
    uint32 Finish();
    uint32 InitializeTrace();

public:
    virtual uint32 Render();
    virtual void Update();
    virtual ~LineProjectile();
};

#endif
