#ifndef PALETTE_RESOURCES_H
#define PALETTE_RESOURCES_H

extern "C"
{
#include "psx.h"
}

struct PalettePosition
{
    uint32 x;
    uint32 y;
};

const PalettePosition &GetLargePalettePos();

struct PaletteResource
{
    uint16 clut;
    uint8 slot;
    uint8 flags;
    uint16 refs;
    uint8 loaded;
    uint32 key;
    PaletteResource *next;
    PaletteResource *prev;
};

uint32 InitializePaletteResources();
PaletteResource *FindPaletteResource(uint32 key);
void ReleaseUnusedPaletteResources();
void ResetPaletteResources();

PaletteResource *LoadPaletteResource(uint32 key, uint16 *colors, uint32 flags);

#endif
