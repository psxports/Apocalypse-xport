#include <stdio.h>
#include <stdlib.h>

#include "game_image_resource.h"
#include "game_diagnostics.h"
#include "game_heap.h"

struct GameHeapBlock
{
    GameHeapBlock *prev;
    GameHeapBlock *next;
    uint32 size;
    sint32 heap;
    bool free;
};

struct HeapState
{
    GameHeapBlock roots[2];
    GameHeapBlock small_blocks[100];
    GameHeapBlock *small_free;
    uint32 used[2];
    uint32 pressure_limit;
    bool pressure;
};

static HeapState heap_state = {0};
// Original byte limits and eight-byte charges are game allocation policy
static const uint32 heap_capacity[2] = {0x43800, 0xB20C0};

uint32 GetGameHeapUsed(uint32 heap)
{
    return heap_state.used[heap];
}

uint32 GetGameHeapCapacity(uint32 heap)
{
    return heap_capacity[heap];
}

bool IsGameHeapUnderPressure()
{
    return heap_state.pressure;
}

void StopWithHeapErrorColor(uint32 red, uint32 green, uint32 blue)
{
    FUNCTION_MARKER(0x8006B484, "SLUS_003.73");
    static const PSX_RECT error_rectangle = {0, 0, 512, 480};
    ClearImageRectangle(error_rectangle, red & 255, green & 255, blue & 255);
    // The original fatal handler stops here after clearing the error rectangle
    for (;;)
    {
    }
}

static void CoalesceHeapBlock(GameHeapBlock *block)
{
    FUNCTION_MARKER(0x8006B4B4, "SLUS_003.73");
    if (block->prev && block->prev->free)
    {
        GameHeapBlock *prev = block->prev;
        prev->size += 8 + block->size;
        prev->next = block->next;
        if (block->next)
            block->next->prev = prev;
        free(block);
        block = prev;
    }
    if (block->next && block->next->free)
    {
        GameHeapBlock *next = block->next;
        block->size += 8 + next->size;
        block->next = next->next;
        if (next->next)
            next->next->prev = block;
        free(next);
    }
}

uint32 InitializeGameHeaps()
{
    FUNCTION_MARKER(0x8006B5FC, "SLUS_003.73");
    for (uint32 heap = 0; heap < 2; ++heap)
    {
        GameHeapBlock *root = heap_state.roots + heap;
        GameHeapBlock *block = root->next;
        while (block)
        {
            GameHeapBlock *next = block->next;
            free(block);
            block = next;
        }
        root->prev = 0;
        root->next = 0;
        root->size = heap_capacity[heap] - 8;
        root->heap = heap;
        root->free = true;
        heap_state.used[heap] = 0;
    }
    for (uint32 index = 0; index < 100; ++index)
    {
        GameHeapBlock *block = heap_state.small_blocks + index;
        block->prev = 0;
        block->next = index < 99 ? block + 1 : 0;
        block->size = 160;
        block->heap = -1;
        block->free = true;
    }
    heap_state.small_free = heap_state.small_blocks;
    heap_state.pressure_limit = (heap_capacity[0] * 80) / 100;
    return heap_state.pressure_limit;
}

GameHeapBlock *AllocateHeapBlock(uint32 requested, sint32 heap, sint32 fatal)
{
    FUNCTION_MARKER(0x8006B864, "SLUS_003.73");
    uint32 size = (requested + 3) & 0xFFFFFFFC;
    if (heap == -1)
    {
        heap = 0;
        if (size < 161 && heap_state.small_free)
        {
            GameHeapBlock *block = heap_state.small_free;
            heap_state.small_free = block->next;
            block->next = 0;
            block->size = size;
            block->free = false;
            return block;
        }
    }
    if (heap < 0 || heap > 1)
    {
        GameDiagnostic("Allocator heap index is outside the reviewed range\n");
        return 0;
    }
    GameHeapBlock *block = heap_state.roots + heap;
    while (block && (!block->free || block->size < size))
        block = block->next;
    if (!block)
    {
        if (fatal)
        {
            if (heap == 1)
                StopWithHeapErrorColor(128, 0, 0);
            else
                StopWithHeapErrorColor(0, 0, 128);
        }
        return 0;
    }
    if (size + 8 < block->size)
    {
        GameHeapBlock *tail = (GameHeapBlock *)malloc(sizeof(*tail));
        if (!tail)
        {
            GameDiagnostic("Cannot allocate native heap metadata\n");
            abort();
        }
        tail->prev = block;
        tail->next = block->next;
        tail->size = block->size - size - 8;
        tail->heap = heap;
        tail->free = true;
        if (block->next)
            block->next->prev = tail;
        block->next = tail;
        block->size = size;
    }
    block->free = false;
    heap_state.used[heap] += 8 + block->size;
    if (!heap)
        heap_state.pressure = heap_state.used[0] >= heap_state.pressure_limit;
    return block;
}

uint32 ReleaseHeapBlock(GameHeapBlock *block)
{
    FUNCTION_MARKER(0x8006BC20, "SLUS_003.73");
    sint32 heap = block->heap;
    block->free = true;
    if (heap == -1)
    {
        block->next = heap_state.small_free;
        heap_state.small_free = block;
        return 0;
    }
    heap_state.used[heap] -= 8 + block->size;
    CoalesceHeapBlock(block);
    if (!heap)
        heap_state.pressure = heap_state.used[0] >= heap_state.pressure_limit;
    return heap == 0;
}

GameHeapBlock *AllocateZeroedHeapBlock(uint32 requested)
{
    FUNCTION_MARKER(0x8002FED8, "SLUS_003.73");
    return AllocateHeapBlock(requested, 0, 1);
}
