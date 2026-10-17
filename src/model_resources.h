#ifndef MODEL_RESOURCES_H
#define MODEL_RESOURCES_H

#include "model_vram.h"
#include "model_grid.h"
#include "model_animation_data.h"
#include "model_attachment_data.h"
#include "model_materials.h"
#include "model_keys.h"
#include "texture_resources.h"
#include "model_geometry.h"
#include "model_instance.h"

extern "C"
{
#include "psx.h"
}

struct ModelSceneObjects
{
    uint32 count;
    ModelInstance *objects;
};

struct ModelBank
{
    char name[9];
    uint8 has_animation;
    uint8 loaded;
    uint8 protected_bank;
    // Temporary payload references are removed with the connected binary codec migration
    ModelKeys keys;
    uint32 source_bytes;
    TextureReferences textures;
    TextureGroupBlock *groups;
    ModelGeometrySet geometry;
    uint8 *data;
    ModelSceneObjects scene;
    ModelAnimationSet animations;
    const ModelAttachmentTable *attachments;
    ModelPalette colors;
    ModelTextureAnimation texture_animation;
    ModelColorAnimation color_animation;
};

void ResetModelBanks();
ModelBank &GetModelBank(uint32 index);
ModelGeometry &GetModelGeometry(uint32 bank, uint32 idx);
void ResetActiveModelState();
uint32 GetActiveModelBank(uint32 slot);
const ModelKeys &GetActiveModelScriptKeys(uint32 slot);
ModelInstance *GetModelSceneObjects();
uint32 GetModelBankReplacement();
void SelectModelBankReplacement(uint32 slot);

uint32 PrepareModelBankGeometry(uint32 bank);
uint32 InitializeModelResources();
uint32 FindModelResourceIndex(uint32 key, uint32 bank);
ModelInstance *FindModelObject(uint32 key);
void DecrementModelResourceReferences(uint32 bank);
uint32 LoadModelResourceData(uint32 bank);

void AttachModelBankObjects(uint32 bank);
void DetachModelBankObjects(uint32 slot);
uint32 FindModelBankByName(uint32 name);
uint32 FindModelBankByName(const char *name);
void UnloadModelBankByName(uint32 name);
uint32 UnloadUnprotectedModelBanks();
void UnloadModelBank(uint32 bank, uint32 release_unused);

uint32 UnloadInactiveModelBanks();
uint32 UnloadActiveModelBanks();
uint32 FindActiveModelBankSlot(uint32 bank);

void ResetModelBankLoadQueue();
uint32 GetModelBankLoadPending();
uint32 QueueModelBankLoad(const char *name, uint32 active);
uint32 QueueModelBankLoad(uint32 name, uint32 active);
uint32 ProcessModelBankLoadQueue();

uint32 WaitForModelBankLoads();

#endif
