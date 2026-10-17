#include "game_sdk_bindings.h"
extern "C" {
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
}
#include "game_heap.h"
#include "native_object_storage.h"
#include <string.h>
#include <stdio.h>

void StopWithHeapErrorColor(uint32 red, uint32 green, uint32 blue)
{
    FUNCTION_MARKER(0x8006B484, "SLUS_003.73");
    xport_draft_host_sub_8008847C_p1((const sint16 *)psx_addr(0x800FF720, 8), red & 255, green & 255, blue & 255);
    // The original fatal handler stops here after clearing the error rectangle
    for (;;)
    {
    }
}



uint32 CoalesceHeapBlock(uint32 block, uint32 heap)
{
    uint32 current, previous = 0, result, previous_flags, previous_span, block_size, next, total, flags, current_size;
    FUNCTION_MARKER(0x8006B4B4, "SLUS_003.73");
    current = r_u32(0x800FF750 + heap * 4);
    while (current < block)
    {
        if (!current)
            break;
        previous = current;
        current = r_u32(current);
    }
    if (previous)
    {
        previous_flags = r_u32(previous + 4);
        previous_span = (previous_flags >> 4) + 8;
        if (previous + previous_span == block)
        {
            block_size = r_u32(block + 4) >> 4;
            result = previous_span + block_size;
            if (block + block_size + 8 == current)
            {
                next = r_u32(current);
                total = (previous_flags >> 4) + 16;
                w_u32(previous, next);
                block_size = r_u32(block + 4) >> 4;
                result = r_u32(current + 4) >> 4;
                total += block_size + result;
                w_u32(previous + 4, (previous_flags & 15) | (total << 4));
                return result;
            }
            result <<= 4;
            w_u32(previous + 4, (previous_flags & 15) | result);
            return result;
        }
        w_u32(previous, block);
    }
    else
        w_u32(0x800FF750 + heap * 4, block);
    result = block + (r_u32(block + 4) >> 4) + 8;
    if (result == current)
    {
        flags = r_u32(block + 4);
        next = r_u32(current);
        w_u32(block, next);
        current_size = r_u32(current + 4) >> 4;
        result = ((flags >> 4) + 8 + current_size) << 4;
        w_u32(block + 4, (flags & 15) | result);
    }
    else
        w_u32(block, current);
    return result;
}

uint32 InitializeGameHeaps()
{
    uint32 bounds = 0x800E6C08, begin, end, flags, destination, next, scaled, result;
    FUNCTION_MARKER(0x8006B5FC, "SLUS_003.73");
    for (uint32 heap = 0; heap < 2; ++heap, bounds += 8)
    {
        w_u32(0x800FF750 + heap * 4, 0);
        w_u32(0x800FF748 + heap * 4, 0);
        begin = r_u32(bounds);
        end = r_u32(bounds + 4);
        flags = r_u32(begin + 4);
        w_u32(begin + 4, (flags & 15) | ((end - begin - 8) << 4));
        CoalesceHeapBlock(begin, heap);
    }
    destination = 0x800EAC68;
    next = 0x800EAD10;
    for (uint32 index = 0; index < 99; ++index)
    {
        w_u32(destination, next);
        destination -= 168;
        next -= 168;
    }
    end = r_u32(0x800E6C0C);
    begin = r_u32(0x800E6C08);
    scaled = (end - begin) * 80;
    w_u32(0x800EAD10, 0);
    w_u32(0x800FF744, 0x800E6C18);
    result = scaled / 100;
    w_u32(0x800FF73C, result);
    return result;
}

void SetTemporaryHeapBounds(uint32 begin, uint32 end)
{
    FUNCTION_MARKER(0x8006B70C, "SLUS_003.73");
    w_u32(0x800FF728, begin);
    w_u32(0x800FF72C, end);
}

static void WriteHeapWord(uint32 *bytes, uint32 size)
{
    uint32 mapped = GuestMemoryAddress(bytes);
    if (mapped)
        w_u32(mapped, size);
    else
        memcpy(bytes, &size, 4);
}

uint32 FindTemporaryHeapSpace(uint32 *bytes)
{
    uint32 end, begin, total, first, last, first_size, last_size;
    FUNCTION_MARKER(0x8006B71C, "SLUS_003.73");
    end = r_u32(0x800FF72C);
    begin = r_u32(0x800FF728);
    first = r_u32(0x800FF730);
    total = end - begin;
    if (first)
    {
        last = r_u32(0x800FF734);
        if (last)
        {
            first_size = r_u32(first + 4) >> 4;
            last_size = r_u32(last + 4) >> 4;
            if (first_size + last_size + 16 == total)
            {
                WriteHeapWord(bytes, 0);
                return 0;
            }
        }
        first = r_u32(0x800FF730);
        if (first)
            goto first_block;
    }
    last = r_u32(0x800FF734);
    if (!last)
    {
        WriteHeapWord(bytes, total);
        return r_u32(0x800FF728);
    }
    first = r_u32(0x800FF730);
    if (first)
        goto first_block;
    last_size = r_u32(last + 4) >> 4;
    WriteHeapWord(bytes, total - last_size - 8);
    return r_u32(0x800FF728);

first_block:
    last = r_u32(0x800FF734);
    if (last)
    {
        first = r_u32(0x800FF730);
        last_size = r_u32(last + 4) >> 4;
        first_size = r_u32(first + 4) >> 4;
        WriteHeapWord(bytes, total - (first_size + 16) - last_size);
    }
    else
    {
        first = r_u32(0x800FF730);
        first_size = r_u32(first + 4) >> 4;
        WriteHeapWord(bytes, total - (first_size + 8));
    }
    first = r_u32(0x800FF730);
    first_size = r_u32(first + 4) >> 4;
    begin = r_u32(0x800FF728);
    return begin + first_size + 8;
}

uint32 AllocateHeapBlock(uint32 requested, sint32 heap, sint32 fatal)
{
    uint32 allocation_id, size, first, last, begin, end, available, result;
    uint32 block, previous = 0, next, word, block_size, remainder, count, counter;
    sint32 selected_heap = heap;
    FUNCTION_MARKER(0x8006B864, "SLUS_003.73");
    allocation_id = (r_u32(0x800FF740) + 1) & 0x7FFFFFFF;
    size = (requested + 3) & 0xFFFFFFFC;
    w_u32(0x800FF740, allocation_id);
    if (heap == -2)
    {
        first = r_u32(0x800FF730);
        if (!first)
        {
            last = r_u32(0x800FF734);
            if (last)
            {
                begin = r_u32(0x800FF728);
                available = last - begin - 8;
            }
            else
            {
                end = r_u32(0x800FF72C);
                begin = r_u32(0x800FF728);
                available = end - begin - 16;
            }
            if (available >= size && (sint32)available >= 0)
            {
                begin = r_u32(0x800FF728);
                allocation_id = r_u32(0x800FF740);
                result = begin + 8;
                w_u32(begin, allocation_id);
                w_u32(0x800FF730, begin);
                w_u32(begin + 4, (size << 4) | 14);
                return result;
            }
        }
        else
        {
            end = r_u32(0x800FF72C);
            begin = r_u32(0x800FF728);
            word = r_u32(first + 4);
            available = end - begin - ((word >> 4) + 16);
            result = end - size;
            if (available >= size)
            {
                block = result - 8;
                w_u32(0x800FF734, block);
                w_u32(block, allocation_id);
                w_u32(block + 4, (size << 4) | 14);
                return result;
            }
        }
        if (fatal)
            StopWithHeapErrorColor(128, 128, 0);
        return 0;
    }
    if (heap == -1)
    {
        selected_heap = 0;
        if (size < 161)
        {
            block = r_u32(0x800FF744);
            if (block)
            {
                word = r_u32(block + 4);
                next = r_u32(block);
                w_u32(block, allocation_id);
                word = (word & 15) | (size << 4);
                w_u32(block + 4, word);
                result = block + 8;
                w_u32(0x800FF744, next);
                w_u32(block + 4, word | 15);
                return result;
            }
        }
    }
    // TODO Retain the prior native index guard pending the heap-selection contract
    if (selected_heap < 0 || selected_heap > 3)
    {
        GameDiagnostic("Allocator heap index is outside the reviewed range\n");
        return 0;
    }
    block = r_u32(0x800FF750 + (uint32)selected_heap * 4);
    // TODO Original probes address4 for an empty list; preserve prior native failure
    if (!block)
        goto exhausted;
    if ((r_u32(block + 4) >> 4) < size)
    {
        do
        {
            next = r_u32(block);
            if (!next)
                break;
            previous = block;
            block = next;
        } while ((r_u32(block + 4) >> 4) < size);
        if ((r_u32(block + 4) >> 4) < size)
            goto exhausted;
    }
    block_size = r_u32(block + 4) >> 4;
    counter = 0x800FF748 + (uint32)selected_heap * 4;
    if (size + 8 < block_size)
    {
        remainder = block + size + 8;
        word = r_u32(remainder + 4);
        w_u32(remainder + 4, (word & 15) | ((block_size - size - 8) << 4));
        next = r_u32(block);
        if (previous)
            w_u32(previous, next);
        else
            w_u32(0x800FF750 + (uint32)selected_heap * 4, next);
        word = ((uint32)selected_heap & 15) | (size << 4);
        allocation_id = r_u32(0x800FF740);
        w_u32(block + 4, word);
        w_u32(block, allocation_id);
        count = r_u32(counter);
        w_u32(counter, count + 8 + (word >> 4));
        CoalesceHeapBlock(remainder, (uint32)selected_heap);
    }
    else
    {
        next = r_u32(block);
        if (previous)
            w_u32(previous, next);
        else
            w_u32(0x800FF750 + (uint32)selected_heap * 4, next);
        word = r_u32(block + 4);
        allocation_id = r_u32(0x800FF740);
        word = (word & 0xFFFFFFF0) | ((uint32)selected_heap & 15);
        w_u32(block, allocation_id);
        w_u32(block + 4, word);
        count = r_u32(counter);
        w_u32(counter, count + 8 + (word >> 4));
    }
    if (!selected_heap)
    {
        count = r_u32(0x800FF748);
        available = r_u32(0x800FF73C);
        w_u32(0x800FF738, count >= available);
    }
    return block + 8;

exhausted:
    if (fatal)
    {
        if (selected_heap == 1)
            StopWithHeapErrorColor(128, 0, 0);
        else if (!selected_heap)
            StopWithHeapErrorColor(0, 0, 128);
    }
    return 0;
}

uint32 ReleaseHeapBlock(uint32 allocation)
{
    uint32 block, header, kind, temporary, result, counter_address, available, size, limit;
    FUNCTION_MARKER(0x8006BC20, "SLUS_003.73");
    ReleaseNativeObjectStorage(allocation);
    block = allocation - 8;
    header = r_u32(block + 4);
    kind = (uint32)((sint32)(header << 28) >> 28);
    if (kind == (uint32)-2)
    {
        temporary = r_u32(0x800FF730);
        if (block == temporary)
        {
            w_u32(0x800FF730, 0);
            return temporary;
        }
        temporary = r_u32(0x800FF734);
        if (block == temporary)
        {
            w_u32(0x800FF734, 0);
            return (uint32)-1;
        }
    }
    result = kind < 2;
    if (kind == (uint32)-1)
    {
        result = r_u32(0x800FF744);
        w_u32(0x800FF744, block);
        w_u32(block, result);
    }
    else if (kind < 2)
    {
        counter_address = 0x800FF748 + kind * 4;
        available = r_u32(counter_address);
        size = r_u32(block + 4) >> 4;
        w_u32(counter_address, available - 8 - size);
        result = CoalesceHeapBlock(block, kind);
        if (!kind)
        {
            available = r_u32(0x800FF748);
            limit = r_u32(0x800FF73C);
            w_u32(0x800FF738, available >= limit);
            result = 1;
        }
    }
    return result;
}

extern "C" uint32 sub_8006B4B4(uint32 block, uint32 heap)
{
    FUNCTION_MARKER(0x8006B4B4, "SLUS_003.73");
    return CoalesceHeapBlock(block, heap);
}

extern "C" uint32 sub_8006BC20(uint32 allocation)
{
    FUNCTION_MARKER(0x8006BC20, "SLUS_003.73");
    return ReleaseHeapBlock(allocation);
}

extern "C" uint32 sub_8006B5FC(void)
{
    FUNCTION_MARKER(0x8006B5FC, "SLUS_003.73");
    return InitializeGameHeaps();
}


extern "C" uint32 apocalypse_heap_available(uint32 *bytes) { return FindTemporaryHeapSpace(bytes); }

extern "C" uint32 sub_8006B864(uint32 requested, sint32 heap, sint32 fatal)
{
    FUNCTION_MARKER(0x8006B864, "SLUS_003.73");
    return AllocateHeapBlock(requested, heap, fatal);
}

uint32 ResizeHeapBlock(uint32 allocation, uint32 size)
{
    uint32 block, header, result, current_size, tail, counter, used, limit;
    sint32 heap;
    FUNCTION_MARKER(0x8006BD14, "SLUS_003.73");
    block = allocation - 8;
    header = r_u32(block + 4);
    heap = (sint32)(header << 28) >> 28;
    result = 0xFFFFFFFF;
    if (heap == -1)
        return result;
    if (heap == -2)
    {
        result = r_u32(0x800FF730);
        if (block == result)
        {
            result = (header & 15) | (size << 4);
            w_u32(block + 4, result);
            return result;
        }
        result = r_u32(0x800FF734);
        if (block == result)
            return result;
    }
    current_size = r_u32(block + 4) >> 4;
    result = size < current_size - 8;
    tail = allocation + size;
    if (!result)
        return result;
    header = r_u32(tail + 4);
    counter = 0x800FF748 + ((uint32)heap << 2);
    w_u32(tail + 4, (header & 15) | ((current_size - (size + 8)) << 4));
    header = r_u32(block + 4);
    used = r_u32(counter);
    w_u32(counter, used - ((header >> 4) - size));
    header = r_u32(block + 4);
    w_u32(block + 4, (header & 15) | (size << 4));
    result = CoalesceHeapBlock(tail, (uint32)heap);
    if (!heap)
    {
        used = r_u32(0x800FF748);
        limit = r_u32(0x800FF73C);
        w_u32(0x800FF738, used >= limit);
        result = 1;
    }
    return result;
}

static uint32 ReadHeapReferenceWord(const uint32 *reference, uint32 mapped, uint32 index)
{
    uint32 value;
    if (mapped)
        return r_u32(mapped + index * 4);
    memcpy(&value, (const uint8 *)reference + index * 4, 4);
    return value;
}

bool TryCaptureHeapReference(uint32 *reference, uint32 allocation)
{
    uint32 header, generation;
    sint32 kind;
    FUNCTION_MARKER(0x8006BE38, "SLUS_003.73");
    if (!allocation || (sint32)r_u32(allocation - 8) < 0)
        return false;
    header = r_u32(allocation - 4);
    kind = (sint32)(header << 28) >> 28;
    if (kind >= 2 || kind < -2 || (header >> 4) - 4 > 0x1FFFFC)
        return false;
    generation = r_u32(allocation - 8);
    WriteHeapWord(reference, allocation);
    WriteHeapWord(reference + 1, generation);
    return true;
}

void MakeObjectReference(uint32 output[2], uint32 object)
{
    if (TryCaptureHeapReference(output, object))
        return;
    // TODO Preserve the existing cleared-reference policy for the undefined original generation
    WriteHeapWord(output, 0);
    WriteHeapWord(output + 1, 0);
}

extern "C" uint32 apocalypse_try_capture_heap_reference(uint32 *reference, uint32 allocation)
{
    return TryCaptureHeapReference(reference, allocation) ? 1 : 0;
}

uint32 ResolveHeapReference(uint32 *reference)
{
    uint32 mapped, source, generation, expected;
    FUNCTION_MARKER(0x8006BEBC, "SLUS_003.73");
    mapped = GuestMemoryAddress(reference);
    source = ReadHeapReferenceWord(reference, mapped, 0);
    if (!source)
        return 0;
    generation = r_u32(source - 8);
    expected = ReadHeapReferenceWord(reference, mapped, 1);
    if (generation == expected)
        return source;
    WriteHeapWord(reference, 0);
    return 0;
}




uint32 AllocateZeroedHeapBlock(uint32 requested)
{
    FUNCTION_MARKER(0x8002FED8, "SLUS_003.73");
    uint32 allocation = AllocateHeapBlock(requested, 0, 1);
    uint32 count = (requested + 3) >> 2;
    for (uint32 index = 0; index < count; ++index)
        w_u32(allocation + index * 4, 0);
    return allocation;
}

extern "C" uint32 sub_8002FED8(uint32 requested)
{
    FUNCTION_MARKER(0x8002FED8, "SLUS_003.73");
    return AllocateZeroedHeapBlock(requested);
}
