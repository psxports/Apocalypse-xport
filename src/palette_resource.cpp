#include <stdlib.h>

#include "game_diagnostics.h"
#include "palette_resource.h"
#include "model_bank.h"
#include "resource_io.h"
#include "game_heap.h"

template <uint32 count>
struct PaletteSlotPool
{
    PalettePosition pos;
    bool available[count];
};

static PaletteSlotPool<148> small_pal_slots;
static PaletteSlotPool<82> large_pal_slots;
// Packed CLUT colors cross the 32-bit PsyQ upload boundary
static uint32 fire_pal[128];

const PalettePosition &PaletteResource::GetLargePos()
{
    return large_pal_slots.pos;
}

template <uint32 count>
static uint32 AllocatePalSlot(PaletteSlotPool<count> &pool)
{
    uint32 slot = 0;
    while (slot < count && !pool.available[slot])
        ++slot;
    if (slot == count)
    {
        GameDiagnostic(("CLUT palette slots exhausted\n"));
        abort();
    }
    pool.available[slot] = false;
    return slot;
}

template <uint32 count>
static void ReleasePalSlot(PaletteSlotPool<count> &pool, uint32 slot)
{
    if (slot >= count)
    {
        GameDiagnostic(("Invalid CLUT palette slot %u\n", slot));
        abort();
    }
    pool.available[slot] = true;
}

uint32 PaletteResource::Initialize()
{
    uint32 pal[512];
    uint32 src, color, r, g, b, packed;
    FUNCTION_MARKER(0x80069320, "SLUS_003.73");
    AllocateModelTexRect(-1, 16, 148, &small_pal_slots.pos.x, &small_pal_slots.pos.y, 16, 0, 0);
    AllocateModelTexRect(-1, 256, 82, &large_pal_slots.pos.x, &large_pal_slots.pos.y, 16, 0, 0);
    for (uint32 i = 0; i < 148; ++i)
        small_pal_slots.available[i] = true;
    large_pal_slots.available[0] = false;
    for (uint32 i = 1; i < 82; ++i)
        large_pal_slots.available[i] = true;
    const uint32 read_bytes = LookupResource("ct.pal");
    // The conversion reads 256 RGB words after the six-word header
    if (read_bytes > sizeof(pal) || GetResourceFileSize("ct.pal") < (6 + 256) * sizeof(uint32))
    {
        GameDiagnostic(("Invalid ct.pal resource size\n"));
        abort();
    }
    BeginResourceRead(pal);
    WaitForResourceRead();
    for (uint32 row = 0; row < 16; ++row)
    {
        src = 6 + (15 - row) * 16;
        for (uint32 col = 0; col < 16; ++col, ++src)
        {
            color = pal[src];
            r = (color & 255) >> 3;
            g = (color >> 11) & 31;
            b = (color >> 19) & 31;
            if (r == 31 && !g && b == 31)
                packed = 0;
            else
                packed = 0x8000 | r | (g << 5) | (b << 10);
            const uint32 dst = row * 8 + ((15 - col) >> 1);
            if (col & 1)
                fire_pal[dst] |= packed;
            else
                fire_pal[dst] = packed << 16;
        }
    }
    PSX_RECT img_rect;
    img_rect.x = large_pal_slots.pos.x;
    img_rect.y = large_pal_slots.pos.y;
    img_rect.w = 256;
    img_rect.h = 1;
    return LoadImagePSX(&img_rect, fire_pal);
}

static PaletteResource *pal_resources;

PaletteResource *PaletteResource::Allocate(uint32 key)
{
    FUNCTION_MARKER(0x800694EC, "SLUS_003.73");
    PaletteResource *pal = (PaletteResource *)malloc(sizeof(PaletteResource));
    if (!pal)
    {
        GameDiagnostic(("Cannot allocate palette resource\n"));
        abort();
    }
    pal->prev = 0;
    pal->next = pal_resources;
    if (pal->next)
        pal->next->prev = pal;
    pal_resources = pal;
    pal->key = key;
    pal->clut = 0;
    pal->slot = 0;
    pal->flags = 0;
    pal->refs = 0;
    return pal;
}

void PaletteResource::Release()
{
    FUNCTION_MARKER(0x80069540, "SLUS_003.73");
    if (next)
        next->prev = prev;
    if (prev)
        prev->next = next;
    if (pal_resources == this)
        pal_resources = next;
    free(this);
}

PaletteResource *PaletteResource::Find(uint32 key)
{
    FUNCTION_MARKER(0x800695B4, "SLUS_003.73");
    for (PaletteResource *pal = pal_resources; pal; pal = pal->next)
        if (pal->key == key)
            return pal;
    return 0;
}

void PaletteResource::ReleaseUnused()
{
    FUNCTION_MARKER(0x800695EC, "SLUS_003.73");
    PaletteResource *pal = pal_resources;
    while (pal)
    {
        PaletteResource *next = pal->next;
        if (!pal->refs)
        {
            if (pal->flags & 1)
                ReleasePalSlot(small_pal_slots, pal->slot);
            if (pal->flags & 2)
                ReleasePalSlot(large_pal_slots, pal->slot);
            pal->Release();
        }
        pal = next;
    }
}

void PaletteResource::ResetAll()
{
    while (pal_resources)
        pal_resources->Release();
}

uint32 PaletteResource::AllocateSmallSlot()
{
    FUNCTION_MARKER(0x800696A8, "SLUS_003.73");
    return AllocatePalSlot(small_pal_slots);
}

uint32 PaletteResource::AllocateLargeSlot()
{
    FUNCTION_MARKER(0x80069700, "SLUS_003.73");
    return AllocatePalSlot(large_pal_slots);
}

PaletteResource *PaletteResource::Load(uint32 key, uint16 *colors, uint32 flags)
{
    uint32 count;
    uint16 color;
    bool found = false;
    uint16 *cursor;
    PSX_RECT img_rect;
    FUNCTION_MARKER(0x80069758, "SLUS_003.73");
    PaletteResource *pal = PaletteResource::Allocate(key);
    pal->flags = flags;
    img_rect.h = 1;
    count = flags & 1 ? 16 : 256;
    cursor = colors;
    for (uint32 idx = 0; idx < count; ++idx, ++cursor)
    {
        color = *cursor;
        if ((color & 0x7FFF) == 0x7C1F)
        {
            found = true;
            *cursor = 0;
        }
        else if (found || !color)
            *cursor = color | 0x8000;
    }
    if (!found)
    {
        cursor = colors;
        for (uint32 idx = 0; idx < count; ++idx, ++cursor)
            *cursor |= 0x8000;
    }
    pal->slot = count == 16 ? PaletteResource::AllocateSmallSlot() : PaletteResource::AllocateLargeSlot();
    const PalettePosition &pos = count == 16 ? small_pal_slots.pos : large_pal_slots.pos;
    const uint16 y = pos.y;
    img_rect.x = pos.x;
    img_rect.y = y + pal->slot;
    pal->clut = getClut(pos.x, y + pal->slot);
    img_rect.w = count;
    LoadImagePSX(&img_rect, (uint32 *)colors);
    return pal;
}
