#ifndef PALETTE_RESOURCE_H
#define PALETTE_RESOURCE_H

extern "C"
{
#include "psx.h"
}

struct PalettePosition
{
    uint32 x;
    uint32 y;
};

struct PaletteResource
{
public:
    uint16 clut;
    uint8 slot;
    uint8 flags;
    uint16 refs;
    uint32 key;
    PaletteResource *next;
    PaletteResource *prev;

public:
    static uint32 Initialize();

private:
    static PaletteResource *Allocate(uint32 key);
    void Release();

public:
    static PaletteResource *Find(uint32 key);
    static void ReleaseUnused();

private:
    static uint32 AllocateSmallSlot();
    static uint32 AllocateLargeSlot();

public:
    static PaletteResource *Load(uint32 key, uint16 *colors, uint32 flags);
    static void ResetAll();
    static const PalettePosition &GetLargePos();
};

#endif
