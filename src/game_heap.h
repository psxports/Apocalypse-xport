#ifndef GAME_HEAP_H
#define GAME_HEAP_H

extern "C"
{
#include "psx.h"
}

struct GameHeapBlock;

void StopWithHeapErrorColor(uint32 r, uint32 g, uint32 b);
uint32 InitializeGameHeaps();
uint32 GetGameHeapUsed(uint32 heap);
uint32 GetGameHeapCapacity(uint32 heap);
bool IsGameHeapUnderPressure();
GameHeapBlock *AllocateHeapBlock(uint32 requested, sint32 heap, sint32 fatal);
GameHeapBlock *AllocateZeroedHeapBlock(uint32 requested);
uint32 ReleaseHeapBlock(GameHeapBlock *block);

#endif
