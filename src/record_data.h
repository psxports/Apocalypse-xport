#ifndef RECORD_DATA_H
#define RECORD_DATA_H

extern "C"
{
#include "psx.h"
}

struct RecordSource;

class RecordCursor
{
public:
    RecordCursor();
    explicit RecordCursor(const uint8 *data);
    RecordCursor(const uint8 *data, uint32 bytes);
    RecordCursor(const RecordCursor &other);
    ~RecordCursor();
    RecordCursor &operator=(const RecordCursor &other);
    RecordCursor &operator=(const uint8 *data);

    operator const uint8 *() const
    {
        return data;
    }

    const uint8 *Get() const
    {
        return data;
    }

    bool Contains(const uint8 *position) const;
    RecordCursor At(const uint8 *position) const;
    void Require(uint32 bytes) const;
    uint32 Remaining() const;
    uint32 DistanceTo(const RecordCursor &other) const;
    uint8 Read8() const;
    uint16 Read16() const;
    uint32 Read32() const;
    const char *CStr() const;
    RecordCursor Aligned(uint32 alignment) const;
    RecordCursor operator+(sint32 offset) const;
    RecordCursor operator-(sint32 offset) const;
    RecordCursor &operator+=(sint32 offset);
    RecordCursor &operator-=(sint32 offset);
    uint8 operator[](sint32 offset) const;

private:
    RecordCursor Shifted(sint64 offset) const;
    const uint8 *data;
    const uint8 *begin;
    const uint8 *end;
    RecordSource *source;
};

class RecordDataBorrow
{
public:
    RecordDataBorrow();
    ~RecordDataBorrow();

private:
    RecordDataBorrow(const RecordDataBorrow &);
    RecordDataBorrow &operator=(const RecordDataBorrow &);
};

void AdoptLevelRecordData(uint8 *data, uint32 bytes);
void ReleaseLevelRecordData();
uint32 GetLevelRecordCount();
RecordCursor GetLevelRecord(uint32 index);
RecordCursor AlignRecordData(RecordCursor cursor);
uint32 RecordListContains(uint32 value, RecordCursor cursor);
RecordCursor SkipRecordList(RecordCursor cursor);
RecordCursor GetRecordPayload(uint32 index);
RecordCursor ReadRecordPosition(uint32 *output, uint32 index);

#endif
