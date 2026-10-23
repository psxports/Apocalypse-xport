#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "game_diagnostics.h"
#include "record_data.h"
#include "vector3.h"
#include "record_cursor.h"
#include "record_source_internal.h"

struct RecordFileHeader
{
    uint32 reserved[2];
    uint16 count;
    uint16 flags;
};

typedef char RecordFileHeaderSize[sizeof(RecordFileHeader) == 12 ? 1 : -1];
static void RequireRecData(const uint8 *data, uint32 bytes);

RecordSource *rec_srcs;
uint32 rec_borrows;

void DestroyRecSrc(RecordSource *src)
{
    RecordSource **link = &rec_srcs;
    while (*link != src)
        link = &(*link)->next;
    *link = src->next;
    free(src->records);
    free(src->data);
    free(src);
}

static void ReleaseRecSrc(RecordSource *src)
{
    if (src && !--src->references && !rec_borrows)
        DestroyRecSrc(src);
}

struct LevelRecordRoot
{
    LevelRecordRoot() : src(0)
    {
    }

    ~LevelRecordRoot()
    {
        ReleaseRecSrc(src);
    }

    RecordSource *src;
};

static LevelRecordRoot level_recs;

static RecordSource *FindRecSrc(const uint8 *data)
{
    uintptr_t address = reinterpret_cast<uintptr_t>(data);
    RecordSource *end_src = 0;
    for (RecordSource *src = rec_srcs; src; src = src->next)
    {
        uintptr_t begin = reinterpret_cast<uintptr_t>(src->data);
        if (address >= begin && address - begin < src->storage_bytes)
            return src;
        if (address >= begin && address - begin == src->storage_bytes)
            end_src = src;
    }
    return end_src;
}

RecordCursor::RecordCursor() : data(0), begin(0), end(0), src(0)
{
}

RecordCursor::RecordCursor(const uint8 *data) : data(data), begin(0), end(0), src(FindRecSrc(data))
{
    if (src)
    {
        ++src->references;
        begin = src->data;
        end = src->data + src->bytes;
    }
}

RecordCursor::RecordCursor(const uint8 *data, uint32 bytes) : data(data), begin(data), end(data), src(FindRecSrc(data))
{
    if (!data && bytes)
    {
        GameDiagnostic(("Missing byte span storage\n"));
        abort();
    }
    if (src)
    {
        ++src->references;
        RequireRecData(data, bytes);
    }
    if (data)
        end = data + bytes;
}

RecordCursor::RecordCursor(const RecordCursor &other) : data(other.data), begin(other.begin), end(other.end), src(other.src)
{
    if (src)
        ++src->references;
}

RecordCursor::~RecordCursor()
{
    ReleaseRecSrc(src);
}

RecordCursor &RecordCursor::operator=(const RecordCursor &other)
{
    RecordSource *next = other.src;
    if (next)
        ++next->references;
    ReleaseRecSrc(src);
    data = other.data;
    begin = other.begin;
    end = other.end;
    src = next;
    return *this;
}

RecordCursor &RecordCursor::operator=(const uint8 *data)
{
    RecordSource *next = FindRecSrc(data);
    const uint8 *next_begin = 0;
    const uint8 *next_end = 0;
    if (next)
    {
        ++next->references;
        if (next == src && Contains(data))
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
    ReleaseRecSrc(src);
    this->data = data;
    begin = next_begin;
    end = next_end;
    src = next;
    return *this;
}

bool RecordCursor::Contains(const uint8 *pos) const
{
    uintptr_t address = reinterpret_cast<uintptr_t>(pos);
    return begin && end && address >= reinterpret_cast<uintptr_t>(begin) && address <= reinterpret_cast<uintptr_t>(end);
}

RecordCursor RecordCursor::At(const uint8 *pos) const
{
    if (!Contains(pos))
    {
        GameDiagnostic(("Script label outside known byte span\n"));
        abort();
    }
    RecordCursor result(*this);
    result.data = pos;
    return result;
}

void RecordCursor::Require(uint32 bytes) const
{
    uintptr_t address = reinterpret_cast<uintptr_t>(data);
    uintptr_t first = reinterpret_cast<uintptr_t>(begin);
    uintptr_t last = reinterpret_cast<uintptr_t>(end);
    if (!begin || !end || address < first || address > last || bytes > last - address)
    {
        GameDiagnostic(("Script cursor outside known byte span\n"));
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
    if (begin != other.begin || end != other.end || src != other.src || reinterpret_cast<uintptr_t>(other.data) < reinterpret_cast<uintptr_t>(data))
    {
        GameDiagnostic(("Operand cursor changed containing byte span\n"));
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
    uint16 value;
    memcpy(&value, data, sizeof(value));
    return value;
}

uint32 RecordCursor::Read32() const
{
    Require(4);
    uint32 value;
    memcpy(&value, data, sizeof(value));
    return value;
}

RecordCursor RecordCursor::Shifted(sint64 offset) const
{
    Require(0);
    uintptr_t first = reinterpret_cast<uintptr_t>(begin);
    sint64 next = static_cast<sint64>(reinterpret_cast<uintptr_t>(data) - first) + offset;
    if (next < 0 || static_cast<uint64>(next) > reinterpret_cast<uintptr_t>(end) - first)
    {
        GameDiagnostic(("Script cursor advance outside known byte span\n"));
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
        GameDiagnostic(("Invalid script cursor alignment\n"));
        abort();
    }
    Require(0);
    uint32 misalignment = reinterpret_cast<uintptr_t>(data) & (alignment - 1);
    uint32 padding = misalignment ? alignment - misalignment : 0;
    return Shifted(padding);
}

const char *RecordCursor::CStr() const
{
    RecordCursor cursor(*this);
    while (cursor.Read8())
        cursor += 1;
    return reinterpret_cast<const char *>(data);
}

static void RequireRecData(const uint8 *data, uint32 bytes)
{
    RecordSource *src = FindRecSrc(data);
    if (src)
    {
        uint32 offset = data - src->data;
        if (offset <= src->bytes && bytes <= src->bytes - offset)
            return;
    }
    GameDiagnostic(("Record data span outside retained resource\n"));
    abort();
}

void ReleaseLevelRecData()
{
    RecordSource *src = level_recs.src;
    level_recs.src = 0;
    ReleaseRecSrc(src);
}

void AdoptLevelRecData(uint8 *data, uint32 bytes)
{
    if (bytes < 12)
    {
        GameDiagnostic(("Truncated level record header\n"));
        abort();
    }
    const RecordFileHeader &header = *(const RecordFileHeader *)data;
    uint16 count = header.count;
    if (count > (bytes - 12) / 4)
    {
        GameDiagnostic(("Truncated level record table\n"));
        abort();
    }
    RecordSource *src = (RecordSource *)calloc(1, sizeof(*src));
    if (!src)
    {
        GameDiagnostic(("Cannot allocate level record owner\n"));
        abort();
    }
    src->data = data;
    src->bytes = bytes;
    src->storage_bytes = bytes;
    src->count = count;
    src->records = count ? (const uint8 **)malloc(count * sizeof(*src->records)) : 0;
    if (count && !src->records)
    {
        GameDiagnostic(("Cannot allocate level record descriptors\n"));
        abort();
    }
    const uint32 *offsets = (const uint32 *)(data + 12);
    for (uint32 idx = 0; idx < count; ++idx)
    {
        uint32 offset = offsets[idx];
        if (offset > bytes - 2)
        {
            GameDiagnostic(("Level record offset outside file\n"));
            abort();
        }
        src->records[idx] = data + offset;
    }
    if (count)
    {
        const uint8 *last = src->records[count - 1];
        uint16 opcode;
        memcpy(&opcode, last, sizeof(opcode));
        if (opcode == 255)
        {
            uint32 tail = last - data;
            if (tail > 0xFFFFFFFA)
            {
                GameDiagnostic(("Record file extent alignment overflow\n"));
                abort();
            }
            uint32 end = (tail + 5) & 0xFFFFFFFC;
            if (end < src->bytes)
                src->bytes = end;
            for (uint32 idx = 0; idx < count; ++idx)
            {
                uint32 offset = src->records[idx] - data;
                if (offset > src->bytes - 2)
                {
                    GameDiagnostic(("Record descriptor outside logical file extent\n"));
                    abort();
                }
            }
        }
    }
    src->references = 1;
    src->next = rec_srcs;
    rec_srcs = src;
    ReleaseLevelRecData();
    level_recs.src = src;
}

uint32 GetLevelRecCount()
{
    return level_recs.src ? level_recs.src->count : 0;
}

RecordCursor GetLevelRec(uint32 idx)
{
    if (!level_recs.src || idx >= level_recs.src->count)
    {
        GameDiagnostic(("Level record index out of bounds: %u\n", idx));
        abort();
    }
    return RecordCursor(level_recs.src->records[idx]);
}

RecordCursor GetRecPayload(uint32 idx)
{
    FUNCTION_MARKER(0x80066088, "SLUS_003.73");
    RecordCursor rec = GetLevelRec(idx);
    uint16 kind = rec.Read16();
    uint32 offset;
    if (kind == 1)
        offset = 6;
    else if (kind == 2 || kind == 3 || kind == 6 || (kind >= 8 && kind <= 10) || kind == 1000 || kind == 1001)
        offset = 2;
    else if (kind == 5)
        offset = 4;
    else
    {
        GameDiagnostic(("TODO Payload requested for record type %d\n", kind));
        abort();
        return RecordCursor();
    }
    rec.Require(offset + 2);
    return rec + offset;
}

static RecordCursor LocateRecPos(uint32 idx)
{
    RecordCursor rec = GetLevelRec(idx);
    RecordCursor cursor;
    uint16 type = rec.Read16();
    switch (type)
    {
        case 1:
        case 7:
            cursor = rec + 8 + (rec + 6).Read16() * 2;
            while (cursor.Read8() != 255)
                cursor += 1;
            return (cursor + 1).Aligned(4);
        case 3:
        case 8:
        case 10:
        case 1000:
        case 1001:
            return (rec + (rec + 2).Read16() * 2 + 4).Aligned(4);
        case 5:
            return (rec + (rec + 4).Read16() * 2 + 6).Aligned(4);
        case 500:
        case 501:
            return (rec + 2).Aligned(4);
        default:
            GameDiagnostic(("TODO Position requested for record type %d\n", type));
            abort();
            return RecordCursor();
    }
}

RecordCursor ReadRecPos(Vector3 *output, uint32 idx)
{
    FUNCTION_MARKER(0x8006613C, "SLUS_003.73");
    RecordCursor cursor = LocateRecPos(idx);
    cursor.Require(12);
    for (uint32 axis = 0; axis < 3; ++axis)
    {
        output->comp[axis] = (cursor + axis * 4).Read32() << 12;
    }
    return cursor + 12;
}

bool RecListContains(uint8 val, RecordCursor cursor)
{
    FUNCTION_MARKER(0x80064180, "SLUS_003.73");
    for (;;)
    {
        uint8 cur = cursor.Read8();
        if (cur == 255)
            return false;
        cursor += 1;
        if (cur == val)
            return true;
    }
}

RecordCursor SkipRecList(RecordCursor cursor)
{
    FUNCTION_MARKER(0x800641B8, "SLUS_003.73");
    while (cursor.Read8() != 255)
        cursor += 1;
    return cursor + 1;
}
