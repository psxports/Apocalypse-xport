#ifndef MODEL_RESOURCES_H
#define MODEL_RESOURCES_H

extern "C"
{
#include "psx.h"
}

uint32 AllocateVramRecord();
uint32 ReleaseVramRecord(uint32 record);
uint32 ReleaseUsedVramRectangle(uint32 x, uint32 y);
uint32 MergeFreeVramRectangles(uint32 first, uint32 second);
uint32 CoalesceFreeVramRectangles(uint32 record);
uint32 RemoveVramRecord(uint32 *head, uint32 record);
uint32 PrependVramRecord(uint32 *head);
uint32 CreateFreeVramRectangle(uint32 x, uint32 y, uint32 width, uint32 height);
uint32 CreateUsedVramRectangle(uint32 x, uint32 y, uint32 width, uint32 height);
uint32 RebuildModelVramAllocations();
uint32 PrepareModelBankGeometry(uint32 bank);
uint32 InitializeModelResources();
uint32 InitializeModelVramRectangles();
uint32 AllocateModelTextureRectangle(uint32 owner, uint32 width, uint32 height, uint32 *x, uint32 *y, uint32 depth, uint32 flags, uint32 name);
uint32 FindModelResourceIndex(uint32 key, uint32 bank);
uint32 FindModelObject(uint32 key);
uint32 AllocateModelResourceEntry(uint32 key);
uint32 ReleaseModelResourceEntry(uint32 entry);

uint32 FindModelResourceEntry(uint32 key);
uint32 BeginModelResourceTraversal();
uint32 NextModelResourceEntry();
uint32 FindModelResourceTrailer(uint32 data);
uint32 DecrementModelResourceReferences(uint32 bank);
uint32 ReleaseUnusedModelResources();
uint32 LoadModelResourceData(uint32 bank);

uint32 AttachModelBankObjects(uint32 bank);
uint32 DetachModelBankObjects(uint32 slot);
uint32 FindModelBankByName(uint32 name);
uint32 FindModelBankByName(const char *name);
void UnloadModelBankByName(uint32 name);
uint32 UnloadUnprotectedModelBanks();
void UnloadModelBank(uint32 bank, uint32 release_unused);

uint32 UnloadInactiveModelBanks();
uint32 UnloadActiveModelBanks();
uint32 RegisterModelTextureBlock(uint32 data, uint32 block);
uint32 FindActiveModelBankSlot(uint32 bank);

uint32 FindModelTextureGroup(uint32 name);
uint32 FindModelTextureGroup(const char *name);
uint32 WriteModelLoadFilename(uint32 name, uint32 suffix);
uint32 QueueModelBankLoad(uint32 name, uint32 active);
uint32 ProcessModelBankLoadQueue();

uint32 WaitForModelBankLoads();
uint32 ClearModelBankGrid(sint32 slot);
uint32 InitializeModelBankGrid(uint32 slot, uint32 data);
uint32 ClearModelBankGrids();

#endif
