#ifndef OBJECT_LIFECYCLE_H
#define OBJECT_LIFECYCLE_H

extern "C"
{
#include "psx.h"
}

// Native field adapters also support objects awaiting native construction
uint32 DestroyScaledObjectBase(uint32 object, uint32 reason);
uint32 DestroyVisualObjectBase(uint32 object, uint32 reason);
uint32 DestroyCharacterBase(uint32 object, uint32 reason);
uint32 DestroyEnemyBase(uint32 object, uint32 reason);
uint32 DestroyEffectObjectData(uint32 object, uint32 reason);
uint32 DestroyTexturedEffectData(uint32 object, uint32 reason);

#endif
