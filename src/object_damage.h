#ifndef OBJECT_DAMAGE_H
#define OBJECT_DAMAGE_H

struct ModelPolygon;
struct ModelInstance;

extern "C"
{
#include "psx.h"
}

void ResetObjectDestruction();
uint32 InitializeObjectFlashes();
void ReleaseObjectFlash(uint8 index);
void UpdateObjectFlashes();
void ForgetModelObjectFlashes(const ModelInstance *objects, uint32 count);
uint32 FadeLevelObjectToBlack(ModelInstance *object, uint32 duration);
void DamageLevelObject(ModelInstance *object, ModelPolygon *polygon, sint32 damage);

#endif
