#ifndef MODEL_ANIMATION_H
#define MODEL_ANIMATION_H

#include "model_animation_data.h"
#include "model_attachment_data.h"

extern "C"
{
#include "psx.h"
}

void SetupModelGteMatrices(const MATRIX &rotation, const MATRIX &lighting, const ModelPose &local);
uint32 PackModelPoseRotationWord(const ModelPose &pose, uint32 index);

void SetModelBoneTable(uint32 model_index, const ModelAttachmentTable &table);

// Native animation operations use decoded pose and bone owners
void UpdateModelAnimationData(uint32 object);
void InterpolateModelAnimation(const ModelAnimation &animation, uint32 frame, uint32 loop, uint32 first, uint32 count);
class Character;
uint32 BuildModelPose(Character &character);

void UpdateModelColorAnimation(uint32 model_index);

#endif
