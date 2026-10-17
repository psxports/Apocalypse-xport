#include "game_sdk_bindings.h"
extern "C"
{
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
}
#include "palette_resources.h"
#include "model_resources.h"
#include "resource_io.h"
#include "game_heap.h"
#include <stdlib.h>

template <uint32 count> struct PaletteSlotPool
{
    PalettePosition pos;
    uint8 available[count];
};

static PaletteSlotPool<148> small_pal_slots;
static PaletteSlotPool<82> large_pal_slots;
// Packed CLUT colors cross the 32-bit PsyQ upload boundary
static uint32 fire_pal[128];

const PalettePosition &GetLargePalettePos()
{
    return large_pal_slots.pos;
}

template <uint32 count> static uint32 AllocatePaletteSlot(PaletteSlotPool<count> &pool)
{
    uint32 slot = 0;
    while (slot < count && !pool.available[slot])
        ++slot;
    if (slot == count)
    {
        GameDiagnostic("CLUT palette slots exhausted\n");
        abort();
    }
    pool.available[slot] = 0;
    return slot;
}

template <uint32 count> static void ReleasePaletteSlot(PaletteSlotPool<count> &pool, uint32 slot)
{
    if (slot >= count)
    {
        GameDiagnostic("Invalid CLUT palette slot %u\n", slot);
        abort();
    }
    pool.available[slot] = 1;
}

uint32 InitializePaletteResources()
{
    uint32 pal[512];
    uint32 src, color, r, g, b, packed;
    FUNCTION_MARKER(0x80069320, "SLUS_003.73");
    AllocateModelTextureRectangle(0xFFFFFFFF, 16, 148, &small_pal_slots.pos.x, &small_pal_slots.pos.y, 16, 0, 0);
    AllocateModelTextureRectangle(0xFFFFFFFF, 256, 82, &large_pal_slots.pos.x, &large_pal_slots.pos.y, 16, 0, 0);
    for (uint32 i = 0; i < 148; ++i)
        small_pal_slots.available[i] = 1;
    large_pal_slots.available[0] = 0;
    for (uint32 i = 1; i < 82; ++i)
        large_pal_slots.available[i] = 1;
    const uint32 read_bytes = LookupResource("ct.pal");
    // The conversion reads 256 RGB words after the six-word header
    if (read_bytes > sizeof(pal) || GetResourceFileSize("ct.pal") < (6 + 256) * sizeof(uint32))
    {
        GameDiagnostic("Invalid ct.pal resource size\n");
        abort();
    }
    BeginResourceRead((void *)pal);
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
    img_rect.x = (sint16)large_pal_slots.pos.x;
    img_rect.y = (sint16)large_pal_slots.pos.y;
    img_rect.w = 256;
    img_rect.h = 1;
    return LoadImagePSX(&img_rect, fire_pal);
}

static PaletteResource *pal_resources;

static PaletteResource *AllocatePaletteResource(uint32 key)
{
    FUNCTION_MARKER(0x800694EC, "SLUS_003.73");
    PaletteResource *pal = (PaletteResource *)malloc(sizeof(PaletteResource));
    if (!pal)
    {
        GameDiagnostic("Cannot allocate palette resource\n");
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
    pal->loaded = 0;
    pal->refs = 0;
    return pal;
}

static void ReleasePaletteResource(PaletteResource *pal)
{
    FUNCTION_MARKER(0x80069540, "SLUS_003.73");
    if (pal->next)
        pal->next->prev = pal->prev;
    if (pal->prev)
        pal->prev->next = pal->next;
    if (pal_resources == pal)
        pal_resources = pal->next;
    free(pal);
}

PaletteResource *FindPaletteResource(uint32 key)
{
    FUNCTION_MARKER(0x800695B4, "SLUS_003.73");
    for (PaletteResource *pal = pal_resources; pal; pal = pal->next)
        if (pal->key == key)
            return pal;
    return 0;
}

void ReleaseUnusedPaletteResources()
{
    FUNCTION_MARKER(0x800695EC, "SLUS_003.73");
    PaletteResource *pal = pal_resources;
    while (pal)
    {
        PaletteResource *next = pal->next;
        if (!pal->refs)
        {
            if (pal->flags & 1)
                ReleasePaletteSlot(small_pal_slots, pal->slot);
            if (pal->flags & 2)
                ReleasePaletteSlot(large_pal_slots, pal->slot);
            ReleasePaletteResource(pal);
        }
        pal = next;
    }
}

void ResetPaletteResources()
{
    while (pal_resources)
        ReleasePaletteResource(pal_resources);
}

uint32 AllocateSmallPaletteSlot()
{
    FUNCTION_MARKER(0x800696A8, "SLUS_003.73");
    return AllocatePaletteSlot(small_pal_slots);
}

uint32 AllocateLargePaletteSlot()
{
    FUNCTION_MARKER(0x80069700, "SLUS_003.73");
    return AllocatePaletteSlot(large_pal_slots);
}

PaletteResource *LoadPaletteResource(uint32 key, uint16 *colors, uint32 flags)
{
    uint32 count, color, found = 0, y, x, slot;
    uint16 *cursor;
    PSX_RECT img_rect;
    FUNCTION_MARKER(0x80069758, "SLUS_003.73");
    PaletteResource *pal = AllocatePaletteResource(key);
    pal->flags = (uint8)flags;
    img_rect.h = 1;
    count = flags & 1 ? 16 : 256;
    cursor = colors;
    for (uint32 index = 0; index < count; ++index, ++cursor)
    {
        color = xport_load_le16(cursor);
        if ((color & 0x7FFF) == 0x7C1F)
        {
            found = 1;
            xport_store_le16(cursor, 0);
        }
        else if (found || !color)
            xport_store_le16(cursor, color | 0x8000);
    }
    if (!found)
    {
        cursor = colors;
        for (uint32 index = 0; index < count; ++index, ++cursor)
            xport_store_le16(cursor, xport_load_le16(cursor) | 0x8000);
    }
    if (count == 16)
    {
        slot = AllocateSmallPaletteSlot();
        pal->slot = (uint8)slot;
    }
    else
    {
        slot = AllocateLargePaletteSlot();
        pal->slot = (uint8)slot;
    }
    const PalettePosition &pos = count == 16 ? small_pal_slots.pos : large_pal_slots.pos;
    x = (uint16)pos.x;
    y = (uint16)pos.y;
    img_rect.x = (sint16)x;
    slot = pal->slot;
    img_rect.y = (sint16)(y + slot);
    slot = pal->slot;
    y += slot;
    x = pos.x;
    pal->clut = (uint16)((y << 6) | (((uint32)((sint32)x >> 4)) & 63));
    img_rect.w = (sint16)count;
    LoadImagePSX(&img_rect, (uint32 *)colors);
    pal->loaded = 1;
    return pal;
}

extern "C" uint32 sub_800696A8(void)
{
    FUNCTION_MARKER(0x800696A8, "SLUS_003.73");
    return AllocateSmallPaletteSlot();
}


