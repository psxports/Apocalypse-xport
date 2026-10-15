#ifndef STARTUP_RESOURCES_H
#define STARTUP_RESOURCES_H

extern "C"
{
#include "psx.h"
}

uint32 InitializeGameRuntime();
uint32 InitializeMenuResources();
uint32 InitializeMemoryCardEvents(uint32 requested_mode);
uint32 LoadStartupResource(const char *name);
void LoadStartupImage(uint32 compressed, uint32 name, uint32 height);
sint32 LoadCompressedStartupResource(const char *name);
uint32 DecodeStartupResource();
uint32 DrawStartupImage(uint32 height);
uint32 ClearStartupImageArea();
void ReleaseStartupResources();

#endif
