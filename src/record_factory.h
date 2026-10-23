#ifndef RECORD_FACTORY_H
#define RECORD_FACTORY_H

extern "C"
{
#include "psx.h"
}

uint32 InitializeLevelRecs();
class VisualObject;

VisualObject *CreateObjectFromRec(uint32 idx);
void SpawnRec(uint32 idx);

#endif
