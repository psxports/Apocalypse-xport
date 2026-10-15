#ifndef EFFECT_LIFECYCLE_H
#define EFFECT_LIFECYCLE_H

extern "C"
{
#include "psx.h"
}

uint32 InitializeEffectResources();
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
