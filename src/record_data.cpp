extern "C"
{
#include "draft_first_signatures.h"
}
#include "record_data.h"
#include "native_object_storage.h"
#include <string.h>
#include <stdlib.h>

uint32 GetRecordPayload(uint32 index)
{
    FUNCTION_MARKER(0x80066088, "SLUS_003.73");
    uint32 table = r_u32(0x800FF624);
    uint32 record = r_u32(table + 4 * index);
    uint32 kind = r_u16(record);
    if (kind == 1)
        return record + 6;
    if (kind == 2 || kind == 3 || kind == 6 || (kind >= 8 && kind <= 10) || kind == 1000 || kind == 1001)
        return record + 2;
    if (kind == 5)
        return record + 4;
    if (kind == 4)
        return 5;
    return kind < 1002 ? 1 : 0;
}

static uint32 LocateRecordPosition(uint32 index)
{
    uint32 record = r_u32(r_u32(0x800FF624) + index * 4);
    uint32 cursor, type = r_u16(record);
    switch (type)
    {
        case 1:
        case 7:
            cursor = record + 8 + r_u16(record + 6) * 2;
            while (r_u8(cursor) != 255)
                ++cursor;
            return (cursor + 4) & 0xFFFFFFFC;
        case 3:
        case 8:
        case 10:
        case 1000:
        case 1001:
            return (record + r_u16(record + 2) * 2 + 7) & 0xFFFFFFFC;
        case 5:
            return (record + r_u16(record + 4) * 2 + 9) & 0xFFFFFFFC;
        case 500:
        case 501:
            return (record + 5) & 0xFFFFFFFC;
        default:
            /* TODO Unsupported record types return inherited guest V1 without output */
            abort();
    }
}

uint32 ReadRecordPosition(uint32 *output, uint32 index)
{
    uint32 cursor, destination, axis, value;
    FUNCTION_MARKER(0x8006613C, "SLUS_003.73");
    cursor = LocateRecordPosition(index);
    destination = GuestMemoryAddress(output);
    for (axis = 0; axis < 3; ++axis)
    {
        value = r_u32(cursor + axis * 4) << 12;
        if (destination)
            w_u32(destination + axis * 4, value);
        else
            memcpy((uint8 *)output + axis * 4, &value, 4);
    }
    return cursor + 12;
}

extern "C" uint32 sub_80066088(uint32 index)
{
    FUNCTION_MARKER(0x80066088, "SLUS_003.73");
    return GetRecordPayload(index);
}

extern "C" uint32 sub_8006613C(uint32 destination, uint32 index)
{
    FUNCTION_MARKER(0x8006613C, "SLUS_003.73");
    return ReadRecordPosition((uint32 *)psx_addr(destination, 12), index);
}

extern "C" uint32 apocalypse_trigger_position(uint32 output[3], uint32 index)
{
    return ReadRecordPosition(output, index);
}

uint32 RecordListContains(uint32 value, uint32 cursor)
{
    uint32 current;
    FUNCTION_MARKER(0x80064180, "SLUS_003.73");
    current = r_u8(cursor);
    value &= 255;
    while (current != 255)
    {
        cursor += 1;
        if (current == value)
            return 1;
        current = r_u8(cursor);
    }
    return 0;
}

uint32 SkipRecordList(uint32 cursor)
{
    FUNCTION_MARKER(0x800641B8, "SLUS_003.73");
    while (r_u8(cursor) != 255)
        cursor += 1;
    return cursor + 1;
}

extern "C" uint32 sub_80064180(uint32 value, uint32 cursor)
{
    FUNCTION_MARKER(0x80064180, "SLUS_003.73");
    return RecordListContains(value, cursor);
}
extern "C" uint32 sub_800641B8(uint32 cursor)
{
    FUNCTION_MARKER(0x800641B8, "SLUS_003.73");
    return SkipRecordList(cursor);
}
