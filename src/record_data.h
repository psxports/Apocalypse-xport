#ifndef RECORD_DATA_H
#define RECORD_DATA_H

extern "C"
{
#include "psx.h"
}

uint32 RecordListContains(uint32 value, uint32 cursor);
uint32 SkipRecordList(uint32 cursor);
uint32 GetRecordPayload(uint32 index);
uint32 ReadRecordPosition(uint32 *output, uint32 index);

#endif
