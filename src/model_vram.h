#ifndef MODEL_VRAM_H
#define MODEL_VRAM_H

extern "C"
{
#include "psx.h"
}

struct VramRecord;
void InitializeModelVramRects();
void SetModelVramAllocationMode(bool mode);
VramRecord *AllocateModelTexRect(sint32 owner, uint32 width, uint32 height, uint32 *x, uint32 *y, uint32 depth, uint32 flags, uint32 resource_key);
bool ReleaseUsedVramRect(sint16 x, sint16 y);
void RebuildModelVramAllocations();

#endif
