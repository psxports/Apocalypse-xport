#ifndef GAME_OBJECT_ALLOCATION_H
#define GAME_OBJECT_ALLOCATION_H

extern "C"
{
#include "psx.h"
}

uint32 AllocateZeroedObject(uint32 size);
uint32 ReleaseObjectStorage(uint32 object);

#endif
