extern "C"
{
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
}
#include "palette_resources.h"
#include "model_resources.h"
#include "resource_io.h"
#include "game_heap.h"

uint32 InitializePaletteResources()
{
    uint32 palette[512];
    sint16 rectangle[4];
    uint32 cursor, source, target, color, red, green, blue, packed;
    FUNCTION_MARKER(0x80069320, "SLUS_003.73");
    AllocateModelTextureRectangle(0xFFFFFFFF, 16, 148, (uint32 *)psx_addr(0x800FF68C, 4), (uint32 *)psx_addr(0x800FF690, 4), 16, 0, 0);
    AllocateModelTextureRectangle(0xFFFFFFFF, 256, 82, (uint32 *)psx_addr(0x800FF694, 4), (uint32 *)psx_addr(0x800FF698, 4), 16, 0, 0);
    cursor = 0x80100590 + 147;
    for (uint32 index = 0; index < 148; ++index, --cursor)
        w_u8(cursor, 1);
    w_u8(0x80100630, 0);
    for (uint32 index = 1; index < 82; ++index)
        w_u8(0x80100630 + index, 1);
    sub_8006B04C(0x800FF684);
    xport_draft_host_sub_8006B234_p1(palette);
    WaitForResourceRead();
    for (uint32 row = 0; row < 16; ++row)
    {
        source = 6 + (15 - row) * 16;
        target = 0x801003AE + (row << 5);
        for (uint32 column = 0; column < 16; ++column, ++source, target -= 2)
        {
            color = palette[source];
            red = (color & 255) >> 3;
            green = (color >> 11) & 31;
            blue = (color >> 19) & 31;
            if (red == 31 && !green && blue == 31)
                packed = 0;
            else
                packed = 0x8000 | red | (green << 5) | (blue << 10);
            w_u16(target, packed);
        }
    }
    rectangle[0] = (sint16)r_u16(0x800FF694);
    rectangle[1] = (sint16)r_u16(0x800FF698);
    rectangle[2] = 256;
    rectangle[3] = 1;
    return xport_draft_host_sub_800885A4_p1(rectangle, 0x80100390);
}

uint32 AllocatePaletteResource(uint32 key)
{
    uint32 entry, previous;
    FUNCTION_MARKER(0x800694EC, "SLUS_003.73");
    entry = AllocateHeapBlock(24, 0, 1);
    previous = r_u32(0x800FF680);
    // TODO Establish allocator success invariant before adding a failure path
    w_u32(entry + 20, 0);
    w_u32(0x800FF680, entry);
    w_u32(entry + 16, previous);
    if (previous)
        w_u32(previous + 20, entry);
    w_u32(entry + 8, key);
    w_u8(entry + 6, 0);
    w_u16(entry + 4, 0);
    return entry;
}

// TODO Establish receiver ownership before making this an instance method
uint32 ReleasePaletteResource(uint32 entry)
{
    uint32 next, previous;
    FUNCTION_MARKER(0x80069540, "SLUS_003.73");
    next = r_u32(entry + 16);
    if (next)
        w_u32(next + 20, r_u32(entry + 20));
    previous = r_u32(entry + 20);
    if (previous)
        w_u32(previous + 16, r_u32(entry + 16));
    if (entry == r_u32(0x800FF680))
        w_u32(0x800FF680, r_u32(entry + 16));
    return ReleaseHeapBlock(entry);
}

uint32 FindPaletteResource(uint32 key)
{
    uint32 entry;
    FUNCTION_MARKER(0x800695B4, "SLUS_003.73");
    entry = r_u32(0x800FF680);
    while (entry)
    {
        if (r_u32(entry + 8) == key)
            return entry;
        entry = r_u32(entry + 16);
    }
    return 0;
}

uint32 ReleaseUnusedPaletteResources()
{
    uint32 entry, next, result = 0x80100000;
    FUNCTION_MARKER(0x800695EC, "SLUS_003.73");
    entry = r_u32(0x800FF680);
    while (entry)
    {
        result = r_u16(entry + 4);
        next = r_u32(entry + 16);
        if (!result)
        {
            if (r_u8(entry + 3) & 1)
                w_u8(0x80100590 + r_u8(entry + 2), 1);
            if (r_u8(entry + 3) & 2)
                w_u8(0x80100630 + r_u8(entry + 2), 1);
            result = ReleasePaletteResource(entry);
        }
        entry = next;
    }
    return result;
}

extern "C" uint32 sub_800694EC(uint32 key)
{
    FUNCTION_MARKER(0x800694EC, "SLUS_003.73");
    return AllocatePaletteResource(key);
}
extern "C" uint32 sub_80069540(uint32 entry, uint32 unused)
{
    FUNCTION_MARKER(0x80069540, "SLUS_003.73");
    return ReleasePaletteResource(entry);
}
extern "C" uint32 sub_800695B4(uint32 key)
{
    FUNCTION_MARKER(0x800695B4, "SLUS_003.73");
    return FindPaletteResource(key);
}
extern "C" uint32 sub_800695EC(void)
{
    FUNCTION_MARKER(0x800695EC, "SLUS_003.73");
    return ReleaseUnusedPaletteResources();
}

uint32 AllocateSmallPaletteSlot()
{
    uint32 slot = 0;
    FUNCTION_MARKER(0x800696A8, "SLUS_003.73");
    if (!r_u8(0x80100590))
    {
        slot = 1;
        while (slot < 148)
        {
            if (r_u8(0x80100590 + slot))
                break;
            ++slot;
        }
    }
    // TODO Original exhaustion writes slot 148 beyond the searched range
    w_u8(0x80100590 + slot, 0);
    return slot;
}

uint32 AllocateLargePaletteSlot()
{
    uint32 slot = 0;
    FUNCTION_MARKER(0x80069700, "SLUS_003.73");
    if (!r_u8(0x80100630))
    {
        slot = 1;
        while (slot < 82)
        {
            if (r_u8(0x80100630 + slot))
                break;
            ++slot;
        }
    }
    // TODO Original exhaustion writes slot 82 beyond the searched range
    w_u8(0x80100630 + slot, 0);
    return slot;
}

uint32 LoadPaletteResource(uint32 key, uint32 colors, uint32 flags)
{
    uint32 entry, count, cursor, color, found = 0, x_address, y_address, y, x, slot;
    sint16 rectangle[4];
    FUNCTION_MARKER(0x80069758, "SLUS_003.73");
    entry = AllocatePaletteResource(key);
    w_u8(entry + 3, flags);
    rectangle[3] = 1;
    count = flags & 1 ? 16 : 256;
    cursor = colors;
    for (uint32 index = 0; index < count; ++index, cursor += 2)
    {
        color = r_u16(cursor);
        if ((color & 0x7FFF) == 0x7C1F)
        {
            found = 1;
            w_u16(cursor, 0);
        }
        else if (found || !color)
            w_u16(cursor, color | 0x8000);
    }
    if (!found)
    {
        cursor = colors;
        for (uint32 index = 0; index < count; ++index, cursor += 2)
            w_u16(cursor, r_u16(cursor) | 0x8000);
    }
    if (count == 16)
    {
        slot = AllocateSmallPaletteSlot();
        w_u8(entry + 2, slot);
        x_address = 0x800FF68C;
        y_address = 0x800FF690;
    }
    else
    {
        slot = AllocateLargePaletteSlot();
        w_u8(entry + 2, slot);
        x_address = 0x800FF694;
        y_address = 0x800FF698;
    }
    x = r_u16(x_address);
    y = r_u16(y_address);
    rectangle[0] = (sint16)x;
    slot = r_u8(entry + 2);
    rectangle[1] = (sint16)(y + slot);
    slot = r_u8(entry + 2);
    y += slot;
    x = r_u32(x_address);
    w_u16(entry, (y << 6) | (((uint32)((sint32)x >> 4)) & 63));
    rectangle[2] = (sint16)count;
    xport_draft_host_sub_800885A4_p1(rectangle, colors);
    w_u8(entry + 6, 1);
    return entry;
}

extern "C" uint32 sub_800696A8(void)
{
    FUNCTION_MARKER(0x800696A8, "SLUS_003.73");
    return AllocateSmallPaletteSlot();
}
extern "C" uint32 sub_80069700(void)
{
    FUNCTION_MARKER(0x80069700, "SLUS_003.73");
    return AllocateLargePaletteSlot();
}
extern "C" uint32 sub_80069758(uint32 key, uint32 colors, uint32 flags)
{
    FUNCTION_MARKER(0x80069758, "SLUS_003.73");
    return LoadPaletteResource(key, colors, flags);
}

extern "C" uint32 sub_80069320(void)
{
    FUNCTION_MARKER(0x80069320, "SLUS_003.73");
    return InitializePaletteResources();
}
