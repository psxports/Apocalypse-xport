#ifndef RECORD_SCRIPTS_H
#define RECORD_SCRIPTS_H

extern "C"
{
#include "psx.h"
}

uint32 RegisterResourceScript(uint32 resource, uint32 index, uint32 script);
uint32 ClearResourceScripts();
uint32 FindScriptRecord(uint32 index);
uint32 ActivateResourceScript(uint32 resource);
void UnloadLevelRecords();
uint32 LoadLevelRecords(uint32 basename);
uint32 SelectStartRecord(uint32 name);
uint32 ApplyStartRecord();
void ClearScriptTouches();
void ReleaseUntouchedScripts();

#endif
