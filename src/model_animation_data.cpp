#include "game_sdk_bindings.h"
#include "model_animation_data.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

// Compact matrices cross the existing pose/GTE byte-buffer boundary
typedef char ModelPoseSizeCheck[sizeof(ModelPose) == 24 ? 1 : -1];
typedef char ModelPoseTranslationCheck[offsetof(ModelPose, translation) == 18 ? 1 : -1];

static ModelPose *model_pose_workspace;
static uint32 model_pose_capacity;

void ReleaseModelAnimations(ModelAnimationSet &set)
{
    for (uint32 i = 0; i < set.count; ++i)
        free(set.entries[i].poses);
    free(set.entries);
    set.entries = 0;
    set.count = 0;
}

static bool DecodeModelAnimations(ModelAnimationSet &set, const uint8 *data, uint32 bytes, uint32 bones)
{
    if (bytes < 4 || !bones)
        return false;
    uint32 count = xport_load_le32(data);
    if (count > (bytes - 4) / 8)
        return false;
    if (count > 0xFFFFFFFF / sizeof(ModelAnimation))
        return false;
    set.entries = count ? (ModelAnimation *)malloc(count * sizeof(ModelAnimation)) : 0;
    if (count && !set.entries)
        abort();
    if (count)
        memset(set.entries, 0, count * sizeof(ModelAnimation));
    set.count = count;
    for (uint32 i = 0; i < count; ++i)
    {
        uint32 entry = 4 + i * 8;
        uint32 offset = xport_load_le32(data + entry);
        ModelAnimation &animation = set.entries[i];
        animation.frame_count = xport_load_le16(data + entry + 4);
        animation.interval = xport_load_le16(data + entry + 6);
        animation.bone_count = bones;
        uint32 step = (uint32)animation.interval + 1;
        uint32 keys = ((uint32)animation.frame_count + step - 1) / step;
        if (keys && bones > 0xFFFFFFFF / keys)
            return false;
        animation.pose_count = keys * bones;
        if (offset < 4 + count * 8 || offset > bytes || animation.pose_count > (bytes - offset) / 24)
            return false;
        animation.poses = (ModelPose *)malloc(animation.pose_count * sizeof(ModelPose));
        if (animation.pose_count && !animation.poses)
            abort();
        for (uint32 pose = 0; pose < animation.pose_count; ++pose)
        {
            ModelPose &matrix = animation.poses[pose];
            const uint8 *source = data + offset + pose * 24;
            for (uint32 row = 0; row < 3; ++row)
                for (uint32 column = 0; column < 3; ++column)
                    matrix.rotation[row][column] = xport_load_le16(source + row * 6 + column * 2);
            for (uint32 axis = 0; axis < 3; ++axis)
                matrix.translation[axis] = xport_load_le16(source + 18 + axis * 2);
        }
    }
    return true;
}

void LoadModelAnimations(ModelAnimationSet &set, const uint8 *data, uint32 bytes, uint32 bones)
{
    ModelAnimationSet decoded = {0, 0};
    if (!DecodeModelAnimations(decoded, data, bytes, bones))
    {
        ReleaseModelAnimations(decoded);
        GameDiagnostic("Invalid model animation block\n");
        abort();
    }
    ReleaseModelAnimations(set);
    set = decoded;
}

const ModelAnimation &GetModelAnimation(const ModelAnimationSet &set, uint32 index)
{
    if (index >= set.count)
    {
        GameDiagnostic("Invalid model animation %u\n", index);
        abort();
    }
    return set.entries[index];
}

const ModelPose *GetModelAnimationPoses(const ModelAnimation &animation, uint32 first, uint32 count)
{
    if (first > animation.pose_count || count > animation.pose_count - first)
    {
        GameDiagnostic("Invalid model animation pose span\n");
        abort();
    }
    return first ? animation.poses + first : animation.poses;
}

void ResetModelPoseWorkspace()
{
    free(model_pose_workspace);
    model_pose_workspace = 0;
    model_pose_capacity = 0;
}

ModelPose *GetModelPoseWorkspace(uint32 count)
{
    if (count > model_pose_capacity)
    {
        if (count > 0xFFFFFFFF / sizeof(ModelPose))
            abort();
        ModelPose *poses = (ModelPose *)realloc(model_pose_workspace, count * sizeof(ModelPose));
        if (!poses)
            abort();
        // The original cold interpolation workspace contains zeroes
        memset(poses + model_pose_capacity, 0, (count - model_pose_capacity) * sizeof(ModelPose));
        model_pose_workspace = poses;
        model_pose_capacity = count;
    }
    return model_pose_workspace;
}
