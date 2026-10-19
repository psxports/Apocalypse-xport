#ifndef EFFECT_LIFECYCLE_H
#define EFFECT_LIFECYCLE_H

extern "C"
{
#include "psx.h"
}

#include "texture_resources.h"

class EffectObject;
class Object;
struct GameHeapBlock;

struct EffectLists
{
    EffectObject *controllers;
    EffectObject *sprites;
    EffectObject *endpoint_sprites;
    EffectObject *particles;
    EffectObject *quads;
    // Three- and four-vertex effects share this list
    EffectObject *polygons;
    EffectObject *textured_triangles;
    EffectObject *rings;
    EffectObject *opaque_fragments;
    EffectObject *renderables;
    EffectObject *points;
    EffectObject *segments;
    EffectObject *lines;
};

extern EffectLists effect_lists;
extern bool use_small_effect_pool;
extern uint32 effect_count;

void ResetEffectDrawMode();
const DR_TPAGE &GetEffectDrawMode();
void InitializeEffectResources();
const TextureGroup *GetEffectResourceGroup(uint32 resource);
const TextureGroup *GetButtonResourceGroup();
const TextureResource *GetShadowTexture();
void ForgetEffectTexture(const TextureGroup *grp);
void ForgetEffectTexture(const TextureResource *tex);
void ClearEffectResourceBindings();
void UpdateEffects(void);
void DestroyTransientEffects();
void DestroyFinishedEffects();
void DestroyEffectsByIdentifier(uint32 identifier);
GameHeapBlock *AllocateEffectStorage(uint32 size);
uint32 ReleaseEffectStorage(Object *object);

#endif
