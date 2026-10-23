#ifndef OBJECT_DAMAGE_H
#define OBJECT_DAMAGE_H

struct ModelPolygon;
struct ModelInstance;

extern "C"
{
#include "psx.h"
}

void DamageLevelObject(ModelInstance *object, ModelPolygon *polygon, sint32 damage);

#endif
