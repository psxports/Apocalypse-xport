#ifndef OBJECT_DAMAGE_H
#define OBJECT_DAMAGE_H

extern "C"
{
#include "psx.h"
}

// Native adapter for recovered adjusted damage callbacks
uint32 ApplyObjectDamage(uint32 object, uint32 damage, const sint32 *vector, uint32 reason);

// TODO Recover native object and polygon pointer contracts
uint32 InitializeObjectFlashes();
void ReleaseObjectFlash(uint32 packet);
void UpdateObjectFlashes();
uint32 AcquireObjectFlash(uint32 object);
uint32 FlashLevelObject(uint32 object, uint32 red, uint32 green, uint32 blue, uint32 duration);
uint32 FadeLevelObjectToBlack(uint32 object, uint32 duration);
uint32 DestroyLevelObject(uint32 object);
void DamageLevelObject(uint32 object, uint32 polygon, sint32 damage);

#endif
