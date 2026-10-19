#ifndef OBJECT_FRAGMENTS_H
#define OBJECT_FRAGMENTS_H

#include "vector3.h"

struct ModelPolygon;
struct ModelInstance;

extern "C"
{
#include "psx.h"
}

void ResetFragmentState();
const Vector3 &GetFragmentOrigin();
const ShortVector3 &GetFragmentNormal();
uint32 FragmentLevelPolygon(ModelInstance *object, ModelPolygon *polygon, uint32 parameter, uint32 effect, uint32 test, uint32 mark, uint32 render);
uint32 FragmentLevelObject(ModelInstance *object, uint32 test, uint32 mark);

#endif
