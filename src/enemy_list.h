#ifndef ENEMY_LIST_H
#define ENEMY_LIST_H

extern "C"
{
#include "psx.h"
}

void ClearObjectActivationFlags(uint32 object);
void RestoreNearbySuspendedObjects();
uint32 FinishSelectedEnemies();
void ActivateObjectsByIndex(uint32 object, uint32 index);
uint32 ActivateEnemyIndices(const uint16 *indices);
uint32 SpawnEnemyIndices(const uint16 *indices);

#endif
