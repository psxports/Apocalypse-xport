#ifndef RECORD_COMMANDS_H
#define RECORD_COMMANDS_H

#include "record_data.h"

extern "C"
{
#include "psx.h"
}

RecordCursor SkipRecordString(RecordCursor cursor);
struct ScriptWorkspace;
ScriptWorkspace *AcquireScriptWorkspace();
void ReleaseScriptWorkspaceReference(ScriptWorkspace *workspace);
uint8 *GetScriptWorkspaceData(ScriptWorkspace *workspace);
uint32 GetScriptWorkspaceSize(const ScriptWorkspace *workspace);
void AllocateScriptWorkspace(uint32 size);
void ReleaseScriptWorkspace();
uint32 ExecuteRecordCommands(RecordCursor cursor, uint32 index, uint32 synchronous);

#endif
