extern "C"
{
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
}

#include "character.h"
#include "object_lifecycle.h"
#include "object_damage.h"
#include "game_heap.h"
#include "model_resources.h"
#include "game_object_allocation.h"
#include "native_object_storage.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Native lookup for the mapped single-inheritance visual hierarchy

// Native binding for the original addressable list-head word

// Native adapter for the original adjusted suspend callback

// Native input adapters for original record word and halfword loads

// Native extraction of original signed color DIV corner behavior

// Native and legacy destruction share only the original buffer field operations

uint32 AllocateZeroedObject(uint32 size)
{
    uint32 object, cursor, words, index;
    FUNCTION_MARKER(0x800625AC, "SLUS_003.73");
    object = AllocateHeapBlock(size, 0, 1);
    cursor = object;
    words = (size + 3) >> 2;
    for (index = 0; index < words; ++index)
    {
        w_u32(cursor, 0);
        cursor += 4;
    }
    return object;
}

uint32 ReleaseObjectStorage(uint32 object)
{
    FUNCTION_MARKER(0x80062608, "SLUS_003.73");
    return ReleaseHeapBlock(object);
}

extern "C" uint32 sub_800625AC(uint32 size)
{
    FUNCTION_MARKER(0x800625AC, "SLUS_003.73");
    return AllocateZeroedObject(size);
}

extern "C" uint32 sub_80062608(uint32 object)
{
    FUNCTION_MARKER(0x80062608, "SLUS_003.73");
    return ReleaseObjectStorage(object);
}
