#ifndef EFFECT_LIFECYCLE_H
#define EFFECT_LIFECYCLE_H

extern "C"
{
#include "psx.h"
}

#include "texture_resources.h"

void InitializeEffectResources();
const TextureGroup *GetEffectResourceGroup(uint32 resource);
const TextureGroup *GetButtonResourceGroup();
const TextureResource *GetShadowTexture();
void ForgetEffectTexture(const TextureGroup *grp);
void ForgetEffectTexture(const TextureResource *tex);
void ClearEffectResourceBindings();
void UpdateEffectList(uint32 head);
void UpdateEffects(void);
void DestroyTransientEffectList(uint32 object);
void DestroyTransientEffects();
void DestroyFinishedEffectList(uint32 object);
void DestroyFinishedEffects();
void DestroyEffectsByIdentifier(uint32 identifier);
uint32 AllocateEffectStorage(uint32 size);
uint32 ReleaseEffectStorage(uint32 object);

#endif
