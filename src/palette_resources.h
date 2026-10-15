#ifndef PALETTE_RESOURCES_H
#define PALETTE_RESOURCES_H

extern "C"
{
#include "psx.h"
}

uint32 InitializePaletteResources();
uint32 AllocatePaletteResource(uint32 key);
uint32 ReleasePaletteResource(uint32 entry);
uint32 FindPaletteResource(uint32 key);
uint32 ReleaseUnusedPaletteResources();

uint32 AllocateSmallPaletteSlot();
uint32 AllocateLargePaletteSlot();
uint32 LoadPaletteResource(uint32 key, uint32 colors, uint32 flags);

#endif
