#ifndef STARTUP_RESOURCES_H
#define STARTUP_RESOURCES_H

extern "C"
{
#include "psx.h"
}

void ResetStartupDisplay();
uint32 InitializeGameRuntime();
void LoadStartupImage(uint32 compressed, const char *name, uint32 height);
uint32 ClearStartupImageArea();

#endif
