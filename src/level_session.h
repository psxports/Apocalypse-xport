#ifndef LEVEL_SESSION_H
#define LEVEL_SESSION_H

extern "C"
{
#include "psx.h"
}

void RunGameSession();
void ResetLevelLoadDisplayState();
uint32 DrawLevelLoadProgress();
void ReleaseLevelLoadResources();
uint32 QueueSelectedLevelResources();
uint32 LoadLevelFromPlayerSave(uint32 record);

uint32 CompleteLevel(uint32 completed);

#endif
