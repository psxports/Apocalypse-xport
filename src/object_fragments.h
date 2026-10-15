#ifndef OBJECT_FRAGMENTS_H
#define OBJECT_FRAGMENTS_H

extern "C"
{
#include "psx.h"
}

// TODO Recover native object and polygon pointer contracts
uint32 *TransformFragmentVertex(uint32 *output, uint32 geometry, uint32 indices, uint32 index);
uint32 SetFragmentColor(uint32 indexed, uint32 colors, uint32 count, uint32 palette);
uint32 SpawnTexturedFragments(const uint32 *vertex0, const uint32 *vertex1, const uint32 *vertex2, uint32 u0, uint32 v0, uint32 u1, uint32 v1, uint32 u2, uint32 v2, uint32 texture, uint32 subdivisions);
uint32 SpawnOpaqueFragments(uint32 count, const uint32 *vertex0, const uint32 *vertex1, const uint32 *vertex2, const uint32 *normal, uint32 red, uint32 green, uint32 blue);
uint32 FragmentLevelPolygon(uint32 object, uint32 polygon, uint32 parameter, uint32 effect, uint32 test, uint32 mark, uint32 render);
uint32 FragmentLevelObject(uint32 object, uint32 test, uint32 mark);

#endif
