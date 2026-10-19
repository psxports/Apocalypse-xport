#ifndef RECORD_GROUPS_H
#define RECORD_GROUPS_H

#include "record_data.h"

struct ModelInstance;

extern "C"
{
#include "psx.h"
}

uint32 SetRecordGroupEnabled(uint32 group, uint32 enabled);
uint32 ApplyRecordGroupAction(uint32 group, uint32 mode);
uint32 SetObjectRecordGroupMode(RecordCursor group, uint32 mode);

#endif
