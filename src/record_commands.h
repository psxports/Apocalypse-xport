#ifndef RECORD_COMMANDS_H
#define RECORD_COMMANDS_H

extern "C"
{
#include "psx.h"
}

uint32 SkipRecordString(uint32 cursor);
uint32 SkipRecordCommand(uint32 cursor);
uint32 AllocateScriptHeap(uint32 size);
void ReleaseScriptHeap();
uint32 ExecuteRecordCommands(uint32 cursor, uint32 index, uint32 synchronous);

#endif
