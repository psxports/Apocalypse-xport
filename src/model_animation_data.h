#ifndef MODEL_ANIMATION_DATA_H
#define MODEL_ANIMATION_DATA_H

extern "C"
{
#include "psx.h"
}

struct ModelPose
{
    uint16 rotation[3][3];
    uint16 translation[3];
};

struct ModelJointAngles
{
    uint16 rotation[3];
    uint16 translation[3];
};

struct ModelAnimation
{
    uint16 frame_count;
    uint16 interval;
    uint32 bone_count;
    uint32 pose_count;
    ModelPose *poses;
};

struct ModelAnimationSet
{
    uint32 count;
    ModelAnimation *entries;
};

void ReleaseModelAnimations(ModelAnimationSet &set);
void LoadModelAnimations(ModelAnimationSet &set, const uint8 *data, uint32 bytes, uint32 bones);
const ModelAnimation &GetModelAnimation(const ModelAnimationSet &set, uint32 index);
const ModelPose *GetModelAnimationPoses(const ModelAnimation &animation, uint32 first, uint32 count);
void ResetModelPoseWorkspace();
ModelPose *GetModelPoseWorkspace(uint32 count);

#endif
