#ifndef STARTUP_RESOURCES_H
#define STARTUP_RESOURCES_H

extern "C"
{
#include "psx.h"
}

uint32 InitializeGameRuntime();
void InitializeMenuResources();
uint32 InitializeMemoryCardEvents(uint32 requested_mode);
void LoadStartupImage(uint32 compressed, const char *name, uint32 height);
uint32 ClearStartupImageArea();

#endif
