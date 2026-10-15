#ifndef MODEL_ANIMATION_H
#define MODEL_ANIMATION_H

extern "C"
{
#include "psx.h"
}

void ComposePoseMatrix(uint8 *output, const uint8 *left, const uint8 *right);
void RelativePoseMatrix(uint8 *output, const uint8 *left, const uint8 *right);

uint32 SetModelBoneTable(const uint8 *object, const uint16 *record);

// Native helpers operate on the original addressable model buffers
uint32 InitializeModelAnimation(uint32 object, uint32 record);
uint32 UpdateModelAnimationData(uint32 object);
uint32 InterpolateModelAnimation(uint32 components, uint32 interval, uint32 table, uint32 object, uint32 index_base, uint32 vertices);
uint32 BuildModelPose(uint32 object);
uint32 UpdateModelWorldTransform(uint32 object);

uint32 UpdateModelColorAnimation(uint32 model_index);

#endif
