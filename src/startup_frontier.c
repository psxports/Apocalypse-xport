#include "xport.h"
#include "draft_first_signatures.h"
#include "game_startup_adapters.h"
#include <string.h>

#define IMAGE_NAME "SLUS_003.73"

enum
{
    RESOURCE_TABLE = 0x801007A8u,
    HEAP_USED = 0x800FF748u,
    HEAP_HEAD = 0x800FF750u,
    CD_LOCATION = 0x800FFC3Cu
};

static uint32 read32(uint32 address)
{
    return dword_(address);
}

static void write32(uint32 address, uint32 value)
{
    dword_(address) = value;
}

static uint8 read8(uint32 address)
{
    return byte_(address);
}

static uint8 fold_ascii(uint8 value)
{
    if (value >= 'A' && value <= 'Z')
        value = (uint8)(value + ('a' - 'A'));
    return value;
}

static void report_wip_gap(const char *message)
{
    xport_message_error("Apocalypse WIP", message);
}

uint32 sub_8006B04C(uint32 name_address)
{
    uint32 table = RESOURCE_TABLE, input = name_address, source, destination;
    uint8 value;
    sint32 byte_count, rounded, blocks;
    FUNCTION_MARKER(0x8006B04Cu, IMAGE_NAME);
    write32(0x800FF6FCu, 0u);
    source = name_address;
    destination = 0x800FF714u;
    do { value = read8(source++); byte_(destination++) = value; } while (value != 0u);
    while (read8(input) != 0u)
    {
        uint32 mismatch = 0u;
        do
        {
            uint8 left = fold_ascii(read8(input++));
            uint8 right = fold_ascii(read8(table++));
            if (left != right)
            {
                mismatch = 1u;
                break;
            }
        } while (read8(input) != 0u);
        if (mismatch != 0u)
        {
            input = name_address;
            do { value = read8(table++); } while (value != 0u);
        }
        else if (read8(table) == 0u)
            continue;
        else
        {
            input = name_address;
            ++table;
            do { value = read8(table++); } while (value != 0u);
        }
        table = ((table + 3u) & ~3u) + 8u;
        if (read8(table) == 255u)
        {
            PSX_RECT rectangle;
            memcpy(&rectangle, psx_addr(0x800FF718u, sizeof(rectangle)), sizeof(rectangle));
            ClearImage(&rectangle, 0u, 0u, 255u);
            rectangle.y = (sint16)((uint16)rectangle.y + 256u);
            ClearImage(&rectangle, 0u, 0u, 255u);
        }
    }
    table = (table + 4u) & ~3u;
    write32(0x800FFC48u, read32(table));
    byte_count = (sint32)read32(table + 4u);
    rounded = (sint32)((uint32)byte_count + 2047u);
    blocks = rounded >= 0 ? rounded >> 11 : (sint32)((uint32)byte_count + 4094u) >> 11;
    write32(0x800FFC44u, (uint32)blocks);
    write32(0x800FFC2Cu, (uint32)blocks << 11);
    write32(0x800FF708u, 1u);
    return (uint32)blocks << 11;
}

sint32 sub_8006B234(uint32 destination)
{
    FUNCTION_MARKER(0x8006B234u, IMAGE_NAME);
    return (sint32)game_startup_read_begin(NULL, destination);
}

sint32 sub_8006B44C(void)
{
    FUNCTION_MARKER(0x8006B44Cu, IMAGE_NAME);
    while (read32(0x800FF700u) != 0u)
        sub_8006B2A8();
    return 0;
}

uint32 sub_8006B864(uint32 requested, sint32 heap, sint32 fatal)
{
    uint32 size = (requested + 3u) & ~3u;
    uint32 allocation_id = (read32(0x800FF740u) + 1u) & 0x7fffffffu;
    uint32 block;
    uint32 previous = 0;
    uint32 block_size;
    uint32 word;
    sint32 selected_heap = heap;

    FUNCTION_MARKER(0x8006B864u, IMAGE_NAME);
    write32(0x800FF740u, allocation_id);

    if (heap == -2)
    {
        uint32 last = read32(0x800FF730u);
        uint32 base = read32(0x800FF728u);
        uint32 end = read32(0x800FF72Cu);

        if (last == 0)
        {
            uint32 reserved = read32(0x800FF734u);
            sint32 available = (sint32)((reserved != 0 ? reserved : end) - base - (reserved != 0 ? 8u : 16u));
            if (available >= 0 && (uint32)available >= size)
            {
                write32(base, allocation_id);
                write32(base + 4u, size * 16u | 0x0eu);
                write32(0x800FF730u, base);
                return base + 8u;
            }
        }
        else
        {
            uint32 result = end - size;
            uint32 last_size = read32(last + 4u) >> 4;
            uint32 available = end - base - last_size - 16u;
            if (available >= size)
            {
                uint32 header = result - 8u;
                write32(0x800FF734u, header);
                write32(header, allocation_id);
                write32(header + 4u, size * 16u | 0x0eu);
                return result;
            }
        }
        if (fatal)
            report_wip_gap("Fatal tail allocator exhaustion handler at 0x8006B484 is not translated");
        return 0;
    }

    if (heap == -1)
    {
        selected_heap = 0;
        if (size < 0xa1u && read32(0x800FF744u) != 0)
        {
            block = read32(0x800FF744u);
            word = read32(block + 4u);
            write32(0x800FF744u, read32(block));
            write32(block, allocation_id);
            write32(block + 4u, (word & 0x0fu) | size * 16u | 0x0fu);
            return block + 8u;
        }
    }

    if (selected_heap < 0 || selected_heap > 3)
    {
        report_wip_gap("Allocator heap index is outside the reviewed range");
        return 0;
    }

    block = read32(HEAP_HEAD + (uint32)selected_heap * 4u);
    while (block != 0 && (read32(block + 4u) >> 4) < size)
    {
        previous = block;
        block = read32(block);
    }
    if (block == 0)
    {
        if (fatal)
            report_wip_gap("Fatal free-list exhaustion handler at 0x8006B484 is not translated");
        return 0;
    }

    block_size = read32(block + 4u) >> 4;
    if (size + 8u >= block_size)
    {
        if (previous != 0)
            write32(previous, read32(block));
        else
            write32(HEAP_HEAD + (uint32)selected_heap * 4u, read32(block));
        word = (read32(block + 4u) & 0xfffffff0u) | ((uint32)selected_heap & 0x0fu);
        write32(block, allocation_id);
        write32(block + 4u, word);
    }
    else
    {
        uint32 remainder = block + size + 8u;
        uint32 remainder_size = block_size - size - 8u;

        write32(remainder + 4u, (read32(remainder + 4u) & 0x0fu) | remainder_size * 16u);
        if (previous != 0)
            write32(previous, read32(block));
        else
            write32(HEAP_HEAD + (uint32)selected_heap * 4u, read32(block));
        word = ((uint32)selected_heap & 0x0fu) | size * 16u;
        write32(block + 4u, word);
        write32(block, allocation_id);
        sub_8006B4B4(remainder, (uint32)selected_heap);
    }

    write32(HEAP_USED + (uint32)selected_heap * 4u, read32(HEAP_USED + (uint32)selected_heap * 4u) + 8u + (read32(block + 4u) >> 4));
    if (selected_heap == 0)
        write32(0x800FF738u, read32(0x800FF748u) < read32(0x800FF73Cu) ? 0u : 1u);
    return block + 8u;
}

sint32 sub_80010028(uint32 name_address)
{
    uint32 allocation = read32(0x800FEEBCu);

    FUNCTION_MARKER(0x80010028u, IMAGE_NAME);
    if (allocation == 0)
    {
        uint32 size = sub_8006B04C(name_address);
        allocation = sub_8006B864(size, 1, 1);
        if (allocation == 0)
            return 0;
        write32(0x800FEEBCu, allocation);
        sub_8006B234(allocation);
        return sub_8006B44C();
    }
    return (sint32)allocation;
}
