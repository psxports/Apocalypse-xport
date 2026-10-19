#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "game_diagnostics.h"
#include "record_data.h"
static void RequireRecordData(const uint8 *data, uint32 bytes);

struct RecordSource
{
    uint8 *data;
    uint32 bytes;
    uint32 storage_bytes;
    uint32 count;
    const uint8 **records;
    uint32 references;
    RecordSource *next;
};

static RecordSource *record_sources;
static uint32 record_borrows;

static void DestroyRecordSource(RecordSource *source)
{
    RecordSource **link = &record_sources;
    while (*link != source)
        link = &(*link)->next;
    *link = source->next;
    free(source->records);
    free(source->data);
    free(source);
}

static void ReleaseRecordSource(RecordSource *source)
{
    if (source && !--source->references && !record_borrows)
        DestroyRecordSource(source);
}

struct LevelRecordRoot
{
    LevelRecordRoot() : source(0)
    {
    }

    ~LevelRecordRoot()
    {
        ReleaseRecordSource(source);
    }

    RecordSource *source;
};

static LevelRecordRoot level_records;

static RecordSource *FindRecordSource(const uint8 *data)
{
    uintptr_t address = reinterpret_cast<uintptr_t>(data);
    RecordSource *end_source = 0;
    for (RecordSource *source = record_sources; source; source = source->next)
    {
        uintptr_t begin = reinterpret_cast<uintptr_t>(source->data);
        if (address >= begin && address - begin < source->storage_bytes)
            return source;
        if (address >= begin && address - begin == source->storage_bytes)
            end_source = source;
    }
    return end_source;
}

RecordDataBorrow::RecordDataBorrow()
{
    ++record_borrows;
}

RecordDataBorrow::~RecordDataBorrow()
{
    if (--record_borrows)
        return;
    RecordSource *source = record_sources;
    while (source)
    {
        RecordSource *next = source->next;
        if (!source->references)
            DestroyRecordSource(source);
        source = next;
    }
}

RecordCursor::RecordCursor() : data(0), begin(0), end(0), source(0)
{
}

RecordCursor::RecordCursor(const uint8 *data) : data(data), begin(0), end(0), source(FindRecordSource(data))
{
    if (source)
    {
        ++source->references;
        begin = source->data;
        end = source->data + source->bytes;
    }
}

RecordCursor::RecordCursor(const uint8 *data, uint32 bytes) : data(data), begin(data), end(data), source(FindRecordSource(data))
{
    if (!data && bytes)
    {
        GameDiagnostic("Missing byte span storage\n");
        abort();
    }
    if (source)
    {
        ++source->references;
        RequireRecordData(data, bytes);
    }
    if (data)
        end = data + bytes;
}

RecordCursor::RecordCursor(const RecordCursor &other) : data(other.data), begin(other.begin), end(other.end), source(other.source)
{
    if (source)
        ++source->references;
}

RecordCursor::~RecordCursor()
{
    ReleaseRecordSource(source);
}

RecordCursor &RecordCursor::operator=(const RecordCursor &other)
{
    RecordSource *next = other.source;
    if (next)
        ++next->references;
    ReleaseRecordSource(source);
    data = other.data;
    begin = other.begin;
    end = other.end;
    source = next;
    return *this;
}

RecordCursor &RecordCursor::operator=(const uint8 *data)
{
    RecordSource *next = FindRecordSource(data);
    const uint8 *next_begin = 0;
    const uint8 *next_end = 0;
    if (next)
    {
        ++next->references;
        if (next == source && Contains(data))
        {
            next_begin = begin;
            next_end = end;
        }
        else
        {
            next_begin = next->data;
            next_end = next->data + next->bytes;
        }
    }
    else if (begin && reinterpret_cast<uintptr_t>(data) >= reinterpret_cast<uintptr_t>(begin) && reinterpret_cast<uintptr_t>(data) <= reinterpret_cast<uintptr_t>(end))
    {
        next_begin = begin;
        next_end = end;
    }
    ReleaseRecordSource(source);
    this->data = data;
    begin = next_begin;
    end = next_end;
    source = next;
    return *this;
}

bool RecordCursor::Contains(const uint8 *position) const
{
    uintptr_t address = reinterpret_cast<uintptr_t>(position);
    return begin && end && address >= reinterpret_cast<uintptr_t>(begin) && address <= reinterpret_cast<uintptr_t>(end);
}

RecordCursor RecordCursor::At(const uint8 *position) const
{
    if (!Contains(position))
    {
        GameDiagnostic("Script label outside known byte span\n");
        abort();
    }
    RecordCursor result(*this);
    result.data = position;
    return result;
}

void RecordCursor::Require(uint32 bytes) const
{
    uintptr_t address = reinterpret_cast<uintptr_t>(data);
    uintptr_t first = reinterpret_cast<uintptr_t>(begin);
    uintptr_t last = reinterpret_cast<uintptr_t>(end);
    if (!begin || !end || address < first || address > last || bytes > last - address)
    {
        GameDiagnostic("Script cursor outside known byte span\n");
        abort();
    }
}

uint32 RecordCursor::Remaining() const
{
    Require(0);
    return reinterpret_cast<uintptr_t>(end) - reinterpret_cast<uintptr_t>(data);
}

uint32 RecordCursor::DistanceTo(const RecordCursor &other) const
{
    Require(0);
    other.Require(0);
    if (begin != other.begin || end != other.end || source != other.source || reinterpret_cast<uintptr_t>(other.data) < reinterpret_cast<uintptr_t>(data))
    {
        GameDiagnostic("Operand cursor changed containing byte span\n");
        abort();
    }
    return reinterpret_cast<uintptr_t>(other.data) - reinterpret_cast<uintptr_t>(data);
}

uint8 RecordCursor::Read8() const
{
    Require(1);
    return data[0];
}

uint16 RecordCursor::Read16() const
{
    Require(2);
    return xport_load_le16(data);
}

uint32 RecordCursor::Read32() const
{
    Require(4);
    return xport_load_le32(data);
}

RecordCursor RecordCursor::Shifted(sint64 offset) const
{
    Require(0);
    uintptr_t first = reinterpret_cast<uintptr_t>(begin);
    sint64 next = static_cast<sint64>(reinterpret_cast<uintptr_t>(data) - first) + offset;
    if (next < 0 || static_cast<uint64>(next) > reinterpret_cast<uintptr_t>(end) - first)
    {
        GameDiagnostic("Script cursor advance outside known byte span\n");
        abort();
    }
    RecordCursor result(*this);
    result.data = begin + static_cast<uint32>(next);
    return result;
}

RecordCursor RecordCursor::operator+(sint32 offset) const
{
    return Shifted(offset);
}

RecordCursor RecordCursor::operator-(sint32 offset) const
{
    return Shifted(-static_cast<sint64>(offset));
}

RecordCursor &RecordCursor::operator+=(sint32 offset)
{
    return *this = Shifted(offset);
}

RecordCursor &RecordCursor::operator-=(sint32 offset)
{
    return *this = Shifted(-static_cast<sint64>(offset));
}

uint8 RecordCursor::operator[](sint32 offset) const
{
    return Shifted(offset).Read8();
}

RecordCursor RecordCursor::Aligned(uint32 alignment) const
{
    if (!alignment || (alignment & (alignment - 1)))
    {
        GameDiagnostic("Invalid script cursor alignment\n");
        abort();
    }
    Require(0);
    return Shifted((0 - reinterpret_cast<uintptr_t>(data)) & (alignment - 1));
}

const char *RecordCursor::CStr() const
{
    RecordCursor cursor(*this);
    while (cursor.Read8())
        cursor += 1;
    return reinterpret_cast<const char *>(data);
}

static void RequireRecordData(const uint8 *data, uint32 bytes)
{
    RecordSource *source = FindRecordSource(data);
    if (!source || static_cast<uint32>(data - source->data) > source->bytes || bytes > source->bytes - static_cast<uint32>(data - source->data))
    {
        GameDiagnostic("Record data span outside retained resource\n");
        abort();
    }
}

void ReleaseLevelRecordData()
{
    RecordSource *source = level_records.source;
    level_records.source = 0;
    ReleaseRecordSource(source);
}

void AdoptLevelRecordData(uint8 *data, uint32 bytes)
{
    if (bytes < 12)
    {
        GameDiagnostic("Truncated level record header\n");
        abort();
    }
    uint32 count = xport_load_le16(data + 8);
    if (count > (bytes - 12) / 4)
    {
        GameDiagnostic("Truncated level record table\n");
        abort();
    }
    RecordSource *source = (RecordSource *)calloc(1, sizeof(*source));
    if (!source)
    {
        GameDiagnostic("Cannot allocate level record owner\n");
        abort();
    }
    source->data = data;
    source->bytes = bytes;
    source->storage_bytes = bytes;
    source->count = count;
    source->records = count ? (const uint8 **)malloc(count * sizeof(*source->records)) : 0;
    if (count && !source->records)
    {
        GameDiagnostic("Cannot allocate level record descriptors\n");
        abort();
    }
    for (uint32 index = 0; index < count; ++index)
    {
        uint32 offset = xport_load_le32(data + 12 + index * 4);
        if (offset > bytes - 2)
        {
            GameDiagnostic("Level record offset outside file\n");
            abort();
        }
        source->records[index] = data + offset;
    }
    if (count)
    {
        const uint8 *last = source->records[count - 1];
        if ((sint16)xport_load_le16(last) == 255)
        {
            uint32 tail = static_cast<uint32>(last - data);
            if (tail > 0xFFFFFFFA)
            {
                GameDiagnostic("Record file extent alignment overflow\n");
                abort();
            }
            uint32 end = (tail + 5) & 0xFFFFFFFC;
            if (end < source->bytes)
                source->bytes = end;
            for (uint32 index = 0; index < count; ++index)
                if (static_cast<uint32>(source->records[index] - data) > source->bytes - 2)
                {
                    GameDiagnostic("Record descriptor outside logical file extent\n");
                    abort();
                }
        }
    }
    source->references = 1;
    source->next = record_sources;
    record_sources = source;
    ReleaseLevelRecordData();
    level_records.source = source;
}

uint32 GetLevelRecordCount()
{
    return level_records.source ? level_records.source->count : 0;
}

RecordCursor GetLevelRecord(uint32 index)
{
    if (!level_records.source || index >= level_records.source->count)
    {
        GameDiagnostic("Level record index out of bounds: %u\n", index);
        abort();
    }
    return RecordCursor(level_records.source->records[index]);
}

RecordCursor AlignRecordData(RecordCursor cursor)
{
    return cursor.Aligned(4);
}

RecordCursor GetRecordPayload(uint32 index)
{
    FUNCTION_MARKER(0x80066088, "SLUS_003.73");
    RecordCursor record = GetLevelRecord(index);
    uint32 kind = record.Read16();
    uint32 offset;
    if (kind == 1)
        offset = 6;
    else if (kind == 2 || kind == 3 || kind == 6 || (kind >= 8 && kind <= 10) || kind == 1000 || kind == 1001)
        offset = 2;
    else if (kind == 5)
        offset = 4;
    else
    {
        GameDiagnostic("TODO Payload requested for record type %u\n", kind);
        abort();
        return RecordCursor();
    }
    record.Require(offset + 2);
    return record + offset;
}

static RecordCursor LocateRecordPosition(uint32 index)
{
    RecordCursor record = GetLevelRecord(index);
    RecordCursor cursor;
    uint32 type = record.Read16();
    switch (type)
    {
        case 1:
        case 7:
            cursor = record + 8 + (record + 6).Read16() * 2;
            while (cursor.Read8() != 255)
                cursor += 1;
            return (cursor + 1).Aligned(4);
        case 3:
        case 8:
        case 10:
        case 1000:
        case 1001:
            return (record + (record + 2).Read16() * 2 + 4).Aligned(4);
        case 5:
            return (record + (record + 4).Read16() * 2 + 6).Aligned(4);
        case 500:
        case 501:
            return (record + 2).Aligned(4);
        default:
            GameDiagnostic("TODO Position requested for record type %u\n", type);
            abort();
            return RecordCursor();
    }
}

RecordCursor ReadRecordPosition(uint32 *output, uint32 index)
{
    FUNCTION_MARKER(0x8006613C, "SLUS_003.73");
    RecordCursor cursor = LocateRecordPosition(index);
    cursor.Require(12);
    for (uint32 axis = 0; axis < 3; ++axis)
    {
        output[axis] = (cursor + axis * 4).Read32() << 12;
    }
    return cursor + 12;
}

uint32 RecordListContains(uint32 value, RecordCursor cursor)
{
    FUNCTION_MARKER(0x80064180, "SLUS_003.73");
    value &= 255;
    for (;;)
    {
        uint32 current = cursor.Read8();
        if (current == 255)
            return 0;
        cursor += 1;
        if (current == value)
            return 1;
    }
}

RecordCursor SkipRecordList(RecordCursor cursor)
{
    FUNCTION_MARKER(0x800641B8, "SLUS_003.73");
    while (cursor.Read8() != 255)
        cursor += 1;
    return cursor + 1;
}
