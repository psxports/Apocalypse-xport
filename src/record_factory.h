#ifndef RECORD_FACTORY_H
#define RECORD_FACTORY_H

extern "C"
{
#include "psx.h"
}

uint32 InitializeLevelRecords();
class VisualObject;

VisualObject *CreateObjectFromRecord(uint32 index);
void SpawnRecord(uint32 index);

#endif
