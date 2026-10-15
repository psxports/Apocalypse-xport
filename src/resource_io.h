#ifndef RESOURCE_IO_H
#define RESOURCE_IO_H

extern "C"
{
#include "psx.h"
}

void SetResourceReadOption(uint32 option);
uint32 InitializeResourceDirectory();

uint32 LookupResource(const char *name);

uint32 BeginResourceRead(uint32 destination);
uint32 ProcessResourceRead();
uint32 WaitForResourceRead();

#endif
