#ifndef OBJECT_FRAGMENTS_H
#define OBJECT_FRAGMENTS_H

#include "model_materials.h"

struct ModelPolygon;
struct ModelInstance;
struct ModelGeometry;

extern "C"
{
#include "psx.h"
}

// TODO Recover native object and polygon pointer contracts
const SVECTOR &GetFragmentNormal();
uint32 *TransformFragmentVertex(uint32 *output, const ModelGeometry &geometry, const ModelPolygon &polygon, uint32 index);
uint32 SetFragmentColor(uint32 indexed, uint32 colors, uint32 count, const ModelPalette *palette);
uint32 SpawnTexturedFragments(const uint32 *vertex0, const uint32 *vertex1, const uint32 *vertex2, uint32 u0, uint32 v0, uint32 u1, uint32 v1, uint32 u2, uint32 v2, uint32 texture, uint32 subdivisions);
uint32 SpawnOpaqueFragments(uint32 count, const uint32 *vertex0, const uint32 *vertex1, const uint32 *vertex2, const uint32 *normal, uint32 red, uint32 green, uint32 blue);
uint32 FragmentLevelPolygon(ModelInstance *object, ModelPolygon *polygon, uint32 parameter, uint32 effect, uint32 test, uint32 mark, uint32 render);
uint32 FragmentLevelObject(ModelInstance *object, uint32 test, uint32 mark);

#endif
