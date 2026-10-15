#ifndef RECORD_FACTORY_H
#define RECORD_FACTORY_H

extern "C"
{
#include "psx.h"
}

uint32 InitializeLevelRecords();
uint32 CreateObjectFromRecord(uint32 index);
uint32 SpawnRecord(uint32 index);

#endif
