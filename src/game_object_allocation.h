#ifndef GAME_OBJECT_ALLOCATION_H
#define GAME_OBJECT_ALLOCATION_H

extern "C"
{
#include "psx.h"
}

class Object;
struct GameHeapBlock;

GameHeapBlock *AllocateZeroedObject(uint32 size);

#endif
