#include "angle_math.h"
extern "C"
{
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
}
#include "record_scripts.h"
#include "record_commands.h"
#include "record_data.h"
#include "record_factory.h"
#include "game_panel.h"
#include "game_heap.h"
#include "model_resources.h"
#include "resource_io.h"

uint32 FindScriptRecord(uint32 index)
{
    uint32 table, descriptor, record;
    FUNCTION_MARKER(0x80063C54, "SLUS_003.73");
    if (index == 65535)
        return 0;
    table = r_u32(0x800FF624);
    descriptor = r_u32(table + index * 4);
    if ((sint16)r_u16(descriptor) != 6)
        return 0;
    record = r_u32(0x800FF630);
    while (record)
    {
        if (r_u16(record + 10) == index)
            return record;
        record = r_u32(record + 20);
    }
    return 0;
}

uint32 ActivateResourceScript(uint32 resource)
{
    uint32 record, active, script, index;
    FUNCTION_MARKER(0x80063D3C, "SLUS_003.73");
    record = r_u32(0x800FFF90 + (resource & 255) * 4);
    while (record)
    {
        if (r_u32(record + 12) == resource)
        {
            active = r_u8(record + 5);
            w_u8(record + 4, 1);
            if (!active)
            {
                script = r_u32(record);
                index = r_u16(record + 10);
                ExecuteRecordCommands(script, index, 0);
                w_u8(record + 5, 1);
                return record;
            }
        }
        record = r_u32(record + 16);
    }
    return 0;
}

extern "C" uint32 sub_80063C54(uint32 index)
{
    FUNCTION_MARKER(0x80063C54, "SLUS_003.73");
    return FindScriptRecord(index);
}
extern "C" uint32 sub_80063D3C(uint32 resource)
{
    FUNCTION_MARKER(0x80063D3C, "SLUS_003.73");
    return ActivateResourceScript(resource);
}

void ClearScriptTouches()
{
    uint32 record;
    FUNCTION_MARKER(0x80063CC4, "SLUS_003.73");
    record = r_u32(0x800FF630);
    while (record)
    {
        w_u8(record + 4, 0);
        record = r_u32(record + 20);
    }
}

void ReleaseUntouchedScripts()
{
    uint32 record, active, touched;
    FUNCTION_MARKER(0x80063CF0, "SLUS_003.73");
    record = r_u32(0x800FF630);
    while (record)
    {
        active = r_u8(record + 5);
        if (active)
        {
            touched = r_u8(record + 4);
            if (!touched)
                w_u8(record + 5, 0);
        }
        record = r_u32(record + 20);
    }
}

extern "C" uint32 sub_80063CC4(void)
{
    FUNCTION_MARKER(0x80063CC4, "SLUS_003.73");
    ClearScriptTouches();
    return 0;
}

// The frame caller discards the original incidental return register
extern "C" void sub_80063CF0(void)
{
    FUNCTION_MARKER(0x80063CF0, "SLUS_003.73");
    ReleaseUntouchedScripts();
}

uint32 SelectStartRecord(uint32 name)
{
    uint32 index = 0, table, descriptor, payload, position[3], matched;
    FUNCTION_MARKER(0x80063DD4, "SLUS_003.73");
    w_u32(0x800FF620, 65535);
    while ((sint32)index < (sint32)r_u32(0x800FF628))
    {
        table = r_u32(0x800FF624);
        descriptor = r_u32(table + index * 4);
        if ((sint16)r_u16(descriptor) == 8)
        {
            payload = ReadRecordPosition(position, index);
            matched = sub_80067724(payload + 6, name);
            if (matched)
            {
                w_u32(0x800FF620, index);
                return matched;
            }
        }
        index += 1;
    }
    return index << 2;
}

uint32 ApplyStartRecord()
{
    uint32 position[3], payload, player, cursor;
    FUNCTION_MARKER(0x80063E7C, "SLUS_003.73");
    payload = ReadRecordPosition(position, r_u32(0x800FF620));
    player = r_u32(0x800FF5A0);
    w_u32(player + 4, position[0]);
    w_u32(player + 8, position[1]);
    w_u32(player + 12, position[2]);
    // Preserve the original partial-word pair and trailing rotation halfword
    CopyAngleVector((uint16 *)psx_addr(player + 16, 6), (const uint16 *)psx_addr(payload, 6));
    cursor = payload + 6;
    while (r_u8(cursor))
        cursor += 1;
    cursor += 1;
    cursor += cursor & 1;
    ExecuteRecordCommands(cursor, r_u32(0x800FF620), 1);
    return ResetPanelDisplayTimer();
}

extern "C" uint32 sub_80063DD4(uint32 name)
{
    FUNCTION_MARKER(0x80063DD4, "SLUS_003.73");
    return SelectStartRecord(name);
}
extern "C" uint32 sub_80063E7C(void)
{
    FUNCTION_MARKER(0x80063E7C, "SLUS_003.73");
    return ApplyStartRecord();
}

void UnloadLevelRecords()
{
    uint32 header;
    FUNCTION_MARKER(0x80063F20, "SLUS_003.73");
    header = r_u32(0x800FF61C);
    if (header)
    {
        ReleaseHeapBlock(header);
        w_u32(0x800FF61C, 0);
    }
    w_u32(0x800FF618, 0);
}

uint32 LoadLevelRecords(uint32 basename)
{
    char filename[32], *destination = filename;
    uint32 suffix = 0x800FF634, value, file_size, header, cursor, index, record;
    uint32 position[3], payload, count, offset, table;
    FUNCTION_MARKER(0x80063F4C, "SLUS_003.73");
    w_u32(0x800FF308, 0);
    w_u32(0x800FF30C, 0);
    // TODO Original filename copy has no bounds check
    value = r_u8(basename);
    while (value)
    {
        basename += 1;
        *destination = (char)value;
        value = r_u8(basename);
        destination += 1;
    }
    value = r_u8(suffix);
    while (value)
    {
        suffix += 1;
        *destination = (char)value;
        value = r_u8(suffix);
        destination += 1;
    }
    *destination = 0;
    file_size = LookupResource(filename);
    header = AllocateHeapBlock(file_size, 0, 1);
    w_u32(0x800FF61C, header);
    BeginResourceRead(header);
    WaitForResourceRead();
    header = r_u32(0x800FF61C);
    count = r_u16(header + 8);
    cursor = header + 12;
    w_u32(0x800FF624, cursor);
    w_u32(0x800FF628, count);
    index = 0;
    if (count)
    {
        do
        {
            offset = r_u32(cursor);
            header = r_u32(0x800FF61C);
            index += 1;
            w_u32(cursor, offset + header);
            count = r_u32(0x800FF628);
            cursor += 4;
        } while ((sint32)index < (sint32)count);
    }
    count = r_u32(0x800FF628);
    table = r_u32(0x800FF624);
    // TODO Original zero-count path still reads the preceding table word
    record = r_u32(table + count * 4 - 4);
    if ((sint16)r_u16(record) == 255)
    {
        header = r_u32(0x800FF61C);
        ResizeHeapBlock(header, (record - header + 5) & 0xFFFFFFFC);
    }
    UnloadUnprotectedModelBanks();
    w_u32(0x800FF618, 0);
    for (index = 0; (sint32)index < (sint32)r_u32(0x800FF628); index += 1)
    {
        table = r_u32(0x800FF624);
        record = r_u32(table + index * 4);
        if ((sint16)r_u16(record) == 8)
        {
            payload = ReadRecordPosition(position, index);
            count = r_u32(0x800FF618);
            w_u32(0x800A71FC + count * 4, payload + 6);
            w_u32(0x800FF618, count + 1);
        }
    }
    w_u32(0x800FF620, 65535);
    for (index = 0; (sint32)index < (sint32)r_u32(0x800FF628); index += 1)
    {
        table = r_u32(0x800FF624);
        record = r_u32(table + index * 4);
        if ((sint16)r_u16(record) == 4)
            ExecuteRecordCommands(record + 2, index, 1);
    }
    return InitializeLevelRecords();
}

extern "C" void sub_80063F20(void)
{
    FUNCTION_MARKER(0x80063F20, "SLUS_003.73");
    UnloadLevelRecords();
}
extern "C" uint32 sub_80063F4C(uint32 basename)
{
    FUNCTION_MARKER(0x80063F4C, "SLUS_003.73");
    return LoadLevelRecords(basename);
}

uint32 RegisterResourceScript(uint32 resource, uint32 index, uint32 script)
{
    uint32 record, head, bucket, previous;
    FUNCTION_MARKER(0x80063B5C, "SLUS_003.73");
    record = AllocateHeapBlock(24, 0, 1);
    // TODO Original allocation failure still writes through the returned address
    head = r_u32(0x800FF630);
    w_u32(record + 20, head);
    bucket = 0x800FFF90 + (resource & 255) * 4;
    previous = r_u32(bucket);
    w_u32(0x800FF630, record);
    w_u32(record + 16, previous);
    w_u32(bucket, record);
    w_u8(record + 4, 0);
    w_u8(record + 5, 0);
    w_u16(record + 10, index);
    w_u32(record, script);
    w_u32(record + 12, resource);
    w_u8(record + 7, 0);
    w_u8(record + 6, 0);
    w_u16(record + 8, 0);
    return record;
}

uint32 ClearResourceScripts()
{
    uint32 cursor = 0x8010038C, record, next, result;
    FUNCTION_MARKER(0x80063BF0, "SLUS_003.73");
    for (sint32 index = 255; index >= 0; index -= 1)
    {
        w_u32(cursor, 0);
        cursor -= 4;
    }
    result = cursor;
    record = r_u32(0x800FF630);
    while (record)
    {
        next = r_u32(record + 20);
        result = ReleaseHeapBlock(record);
        record = next;
    }
    w_u32(0x800FF630, 0);
    return result;
}

extern "C" uint32 sub_80063B5C(uint32 resource, uint32 index, uint32 script)
{
    FUNCTION_MARKER(0x80063B5C, "SLUS_003.73");
    return RegisterResourceScript(resource, index, script);
}
extern "C" uint32 sub_80063BF0(void)
{
    FUNCTION_MARKER(0x80063BF0, "SLUS_003.73");
    return ClearResourceScripts();
}
