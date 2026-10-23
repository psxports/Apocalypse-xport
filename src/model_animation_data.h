#ifndef MODEL_ANIMATION_DATA_H
#define MODEL_ANIMATION_DATA_H

#include "model_pose.h"
#include "vector3.h"

extern "C"
{
#include "psx.h"
}

struct ModelJointAngles
{
    ShortVector3 rotation;
    ShortVector3 translation;
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
const ModelAnimation &GetModelAnimation(const ModelAnimationSet &set, uint32 idx);
const ModelPose *GetModelAnimationPoses(const ModelAnimation &animation, uint32 first, uint32 count);
void ResetModelPoseWorkspace();
ModelPose *GetModelPoseWorkspace(uint32 count);

#endif
