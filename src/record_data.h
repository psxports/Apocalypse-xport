#ifndef RECORD_DATA_H
#define RECORD_DATA_H

#include "record_data_borrow.h"
#include "record_cursor.h"

extern "C"
{
#include "psx.h"
}

struct RecordSource;
struct Vector3;

void AdoptLevelRecData(uint8 *data, uint32 bytes);
void ReleaseLevelRecData();
uint32 GetLevelRecCount();
RecordCursor GetLevelRec(uint32 idx);
bool RecListContains(uint8 val, RecordCursor cursor);
RecordCursor SkipRecList(RecordCursor cursor);
RecordCursor GetRecPayload(uint32 idx);
RecordCursor ReadRecPos(Vector3 *output, uint32 idx);

#endif
