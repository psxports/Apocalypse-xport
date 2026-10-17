#ifndef RECORD_GROUPS_H
#define RECORD_GROUPS_H

struct ModelInstance;

extern "C"
{
#include "psx.h"
}

void ApplyObjectListRecordAction(uint32 identifier, ModelInstance *instance, uint32 mode);
uint32 SetRecordGroupEnabled(uint32 group, uint32 enabled);
uint32 ApplyRecordGroupAction(uint32 group, uint32 mode);
void SuspendObjectsByRecord(ModelInstance *&head, uint32 record);
void ResumeObjectsByRecord(ModelInstance *instance, uint32 record);
uint32 SetObjectRecordGroupMode(uint32 group, uint32 mode);

#endif
