#ifndef PROJECTILE_LINE_H
#define PROJECTILE_LINE_H

#include "vector3.h"
#include "effect_object.h"
#include "collision_ray.h"

class RingEffect;

void CreateProjectileImpact(uint32 flags, const Vector3 *pos);

#endif
