#ifndef STARTUP_RESOURCES_H
#define STARTUP_RESOURCES_H

extern "C"
{
#include "psx.h"
}

void ResetStartupDisp();
uint32 InitializeGameRuntime();
void LoadStartupImage(bool compressed, const char *name, uint32 height);
uint32 ClearStartupImageArea();

#endif
