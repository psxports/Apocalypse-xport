#ifndef ENEMY_LIST_H
#define ENEMY_LIST_H

#include "record_data.h"

struct ModelInstance;

extern "C"
{
#include "psx.h"
}

uint32 ActivateEnemyIndices(RecordCursor indices);
uint32 SpawnEnemyIndices(RecordCursor indices);

#endif
