extern "C"
{
#include "draft_first_signatures.h"
}
#include "spawn_points.h"
#include "game_random.h"
#include "native_object_storage.h"
#include <string.h>

static uint32 ReadSpawnIndex(const uint16 *points, uint32 index)
{
    uint32 guest = GuestMemoryAddress(points);
    uint16 value;
    if (guest)
        return r_u16(guest + index * 2);
    memcpy(&value, (const uint8 *)points + index * 2, 2);
    return value;
}

static void WriteSpawnIndex(uint16 *points, uint32 index, uint32 value)
{
    uint32 guest = GuestMemoryAddress(points);
    uint16 half = (uint16)value;
    if (guest)
        w_u16(guest + index * 2, half);
    else
        memcpy((uint8 *)points + index * 2, &half, 2);
}

uint32 ShuffleSpawnPoints(uint16 *points, uint32 count)
{
    uint32 index = 0, value, selected, replacement;
    FUNCTION_MARKER(0x80067E18, "SLUS_003.73");
    while ((sint32)index < (sint32)count)
    {
        value = ReadSpawnIndex(points, index);
        selected = RandomRange(count);
        replacement = ReadSpawnIndex(points, selected);
        WriteSpawnIndex(points, index, replacement);
        ++index;
        WriteSpawnIndex(points, selected, value);
    }
    return 0;
}

extern "C" uint32 sub_80067E18(uint32 points, uint32 count)
{
    FUNCTION_MARKER(0x80067E18, "SLUS_003.73");
    return ShuffleSpawnPoints((uint16 *)psx_addr(points, 2), count);
}
