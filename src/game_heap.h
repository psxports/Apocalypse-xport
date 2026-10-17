#ifndef GAME_HEAP_H
#define GAME_HEAP_H

extern "C"
{
#include "psx.h"
}

void StopWithHeapErrorColor(uint32 red, uint32 green, uint32 blue);
uint32 CoalesceHeapBlock(uint32 block, uint32 heap);
uint32 InitializeGameHeaps();
void SetTemporaryHeapBounds(uint32 begin, uint32 end);
uint32 FindTemporaryHeapSpace(uint32 *bytes);
uint32 AllocateHeapBlock(uint32 requested, sint32 heap, sint32 fatal);
uint32 AllocateZeroedHeapBlock(uint32 requested);
uint32 ReleaseHeapBlock(uint32 allocation);
uint32 ResizeHeapBlock(uint32 allocation, uint32 size);
bool TryCaptureHeapReference(uint32 *reference, uint32 allocation);
void MakeObjectReference(uint32 output[2], uint32 object);
uint32 ResolveHeapReference(uint32 *reference);

#endif
