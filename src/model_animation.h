#ifndef MODEL_ANIMATION_H
#define MODEL_ANIMATION_H

#include "model_animation_data.h"
#include "model_attachment_data.h"

extern "C"
{
#include "psx.h"
}

void ComposePoseMatrix(uint8 *output, const uint8 *left, const uint8 *right);
void RelativePoseMatrix(uint8 *output, const uint8 *left, const uint8 *right);

void SetModelBoneTable(uint32 model_index, const ModelAttachmentTable &table);

// Native helpers operate on the original addressable model buffers
void UpdateModelAnimationData(uint32 object);
void InterpolateModelAnimation(const ModelAnimation &animation, uint32 frame, uint32 loop, uint32 first, uint32 count);
class Character;
uint32 BuildModelPose(Character &character);
uint32 UpdateModelWorldTransform(uint32 object);

void UpdateModelColorAnimation(uint32 model_index);

#endif
