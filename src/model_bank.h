#ifndef MODEL_BANK_H
#define MODEL_BANK_H

#include "model_vram.h"
#include "model_bank_grid.h"
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
public:
    char name[9];
    bool loaded;
    bool protected_bank;
    // Owned decoded resources and the pending file buffer
    ModelKeys keys;
    uint32 src_bytes;
    TextureReferences textures;
    TextureGroupBlock *grps;
    ModelGeometrySet geometry;
    uint8 *data;
    ModelSceneObjects scene;
    ModelAnimationSet animations;
    const ModelAttachmentTable *attachments;
    ModelPalette colors;
    ModelTextureAnimation tex_animation;
    ModelColorAnimation color_animation;

public:
    static uint32 Initialize();
    static uint32 FindResourceIdx(uint32 key, uint32 bank);
    static ModelInstance *FindObject(uint32 key);

private:
    static void DecResourceReferences(uint32 bank);
    static uint32 LoadResourceData(uint32 bank);
    static void AttachObjects(uint32 bank);
    static void DetachObjects(uint32 slot);

public:
    static uint32 FindByName(const char *name);
    static void UnloadByName(const char *name);
    static uint32 UnloadUnprotected();

private:
    static void Unload(uint32 bank, bool release_unused);

public:
    static uint32 UnloadInactive();
    static uint32 UnloadActive();

private:
    static uint32 WriteLoadFilename(const char *name, const char *suffix);

public:
    static uint32 QueueLoad(const char *name, bool active);
    static uint32 FindActiveSlot(uint32 bank);
    static uint32 ProcessLoadQueue();
    static uint32 WaitForLoads();

private:
    static uint32 PrepareGeometry(uint32 bank);

public:
    static void UpdateTexCoords(uint32 bank_idx);
    static void UpdateColors(uint32 bank_idx);
    static ModelInstance *GetSceneObjects();
    static const ModelKeys &GetActiveScriptKeys(uint32 slot);
    static ModelGeometry &GetGeometry(uint32 bank, uint32 idx);
    static ModelBank &Get(uint32 idx);
    static uint32 GetLoadPending();
    static void ResetLoadQueue();
    static void SelectReplacement(uint32 slot);
    static uint32 GetActive(uint32 slot);
    static void ResetActive();
    static void ResetAll();
};

extern uint8 items_bank;

#endif
