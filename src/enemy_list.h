#ifndef ENEMY_LIST_H
#define ENEMY_LIST_H

#include "record_data.h"

struct ModelInstance;

extern "C"
{
#include "psx.h"
}

void ClearObjectActivationFlags(ModelInstance *instance);
void RestoreNearbySuspendedObjects();
uint32 FinishSelectedEnemies();
void ActivateObjectsByIndex(ModelInstance *instance, uint32 index);
uint32 ActivateEnemyIndices(RecordCursor indices);
uint32 SpawnEnemyIndices(RecordCursor indices);

#endif
