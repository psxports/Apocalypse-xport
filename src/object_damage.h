#ifndef OBJECT_DAMAGE_H
#define OBJECT_DAMAGE_H

struct ModelPolygon;
struct ModelInstance;

extern "C"
{
#include "psx.h"
}

// Native virtual damage through the current object identity
uint32 ApplyObjectDamage(uint32 object, uint32 damage, const sint32 *vector, uint32 reason);

// TODO Recover native object and polygon pointer contracts
uint32 InitializeObjectFlashes();
void ReleaseObjectFlash(uint8 index);
void UpdateObjectFlashes();
void ForgetModelObjectFlashes(const ModelInstance *objects, uint32 count);
uint32 FlashLevelObject(ModelInstance *object, uint32 red, uint32 green, uint32 blue, uint32 duration);
uint32 FadeLevelObjectToBlack(ModelInstance *object, uint32 duration);
uint32 DestroyLevelObject(ModelInstance *object);
void DamageLevelObject(ModelInstance *object, ModelPolygon *polygon, sint32 damage);

#endif
