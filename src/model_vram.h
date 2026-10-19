#ifndef MODEL_VRAM_H
#define MODEL_VRAM_H

extern "C"
{
#include "psx.h"
}

struct VramRecord;
void InitializeModelVramRectangles();
void SetModelVramAllocationMode(uint32 mode);
VramRecord *AllocateModelTextureRectangle(uint32 owner, uint32 width, uint32 height, uint32 *x, uint32 *y, uint32 depth, uint32 flags, uint32 resource_key);
uint32 ReleaseUsedVramRectangle(uint32 x, uint32 y);
void RebuildModelVramAllocations();

#endif
