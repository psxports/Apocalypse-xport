#ifndef MODEL_ANIMATION_H
#define MODEL_ANIMATION_H

#include "model_animation_data.h"

extern "C"
{
#include "psx.h"
}

void SetupModelGteMatrices(const MATRIX &rotation, const MATRIX &lighting, const ModelPose &local);
// Device word indices are limited to 0..4
uint32 PackModelPoseRotationWord(const ModelPose &pose, uint32 idx);

// Native animation operations use decoded pose and bone owners
void InterpolateModelAnimation(const ModelAnimation &animation, uint8 frame, bool loop, uint32 first, uint32 count);

#endif
