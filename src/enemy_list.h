#ifndef ENEMY_LIST_H
#define ENEMY_LIST_H

struct ModelInstance;

extern "C"
{
#include "psx.h"
}

void ClearObjectActivationFlags(ModelInstance *instance);
void RestoreNearbySuspendedObjects();
uint32 FinishSelectedEnemies();
void ActivateObjectsByIndex(ModelInstance *instance, uint32 index);
uint32 ActivateEnemyIndices(const uint16 *indices);
uint32 SpawnEnemyIndices(const uint16 *indices);

#endif
