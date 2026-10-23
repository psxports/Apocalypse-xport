#ifndef RECORD_SOURCE_INTERNAL_H
#define RECORD_SOURCE_INTERNAL_H

#include "psx.h"

struct RecordSource
{
    uint8 *data;
    uint32 bytes;
    uint32 storage_bytes;
    uint16 count;
    const uint8 **records;
    uint32 references;
    RecordSource *next;
};

// Cursor references and scoped borrows share source retirement
extern RecordSource *rec_srcs;
extern uint32 rec_borrows;
void DestroyRecSrc(RecordSource *src);

#endif
