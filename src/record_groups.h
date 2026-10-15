#ifndef RECORD_GROUPS_H
#define RECORD_GROUPS_H

extern "C"
{
#include "psx.h"
}

void ApplyObjectListRecordAction(uint32 identifier, uint32 object, uint32 mode);
uint32 SetRecordGroupEnabled(uint32 group, uint32 enabled);
uint32 ApplyRecordGroupAction(uint32 group, uint32 mode);
void SuspendObjectsByRecord(uint32 list, uint32 record);
void ResumeObjectsByRecord(uint32 object, uint32 record);
uint32 SetObjectRecordGroupMode(uint32 group, uint32 mode);

#endif
