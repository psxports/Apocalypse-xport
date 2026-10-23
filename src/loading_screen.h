#ifndef LOADING_SCREEN_H
#define LOADING_SCREEN_H

#include "psx.h"

void InitializeLevelLoadEstimates();
void ResetLevelLoadDispState();
uint32 DrawLevelLoadProgress();
uint32 PrepareLevelLoadingScr(const char *name, const char *image);

#endif
