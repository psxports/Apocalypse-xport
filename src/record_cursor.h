#ifndef RECORD_CURSOR_H
#define RECORD_CURSOR_H

#include "psx.h"

struct RecordSource;

class RecordCursor
{
public:
    operator const uint8 *() const
    {
        return data;
    }

    const uint8 *Get() const
    {
        return data;
    }

private:
    const uint8 *data;
    const uint8 *begin;
    const uint8 *end;
    RecordSource *src;

public:
    bool Contains(const uint8 *pos) const;
    RecordCursor Aligned(uint32 alignment) const;
    RecordCursor operator+(sint32 offset) const;
    RecordCursor operator-(sint32 offset) const;
    RecordCursor &operator+=(sint32 offset);
    RecordCursor &operator-=(sint32 offset);
    RecordCursor();
    explicit RecordCursor(const uint8 *data);
    RecordCursor(const uint8 *data, uint32 bytes);
    RecordCursor(const RecordCursor &other);
    ~RecordCursor();
    RecordCursor &operator=(const RecordCursor &other);
    RecordCursor &operator=(const uint8 *data);
    RecordCursor At(const uint8 *pos) const;
    void Require(uint32 bytes) const;
    uint32 Remaining() const;
    uint32 DistanceTo(const RecordCursor &other) const;
    uint8 Read8() const;
    uint16 Read16() const;
    uint32 Read32() const;
    const char *CStr() const;
    uint8 operator[](sint32 offset) const;

private:
    RecordCursor Shifted(sint64 offset) const;
};

#endif
