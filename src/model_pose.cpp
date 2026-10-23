#include <stdlib.h>

#include "model_bank.h"
#include "game_diagnostics.h"
#include "angle_math.h"
#include "model_matrix.h"
#include "model_animation.h"
#include "model_render_view.h"
#include "vector3.h"
#include "character.h"
#include "model_pose.h"

uint32 PackModelPoseRotationWord(const ModelPose &pose, uint32 idx)
{
    switch (idx)
    {
        case 0:
            return (uint16)pose.rotation[0].x | ((uint32)(uint16)pose.rotation[0].y << 16);
        case 1:
            return (uint16)pose.rotation[0].z | ((uint32)(uint16)pose.rotation[1].x << 16);
        case 2:
            return (uint16)pose.rotation[1].y | ((uint32)(uint16)pose.rotation[1].z << 16);
        case 3:
            return (uint16)pose.rotation[2].x | ((uint32)(uint16)pose.rotation[2].y << 16);
        // The fifth device word includes translation X rather than padding
        default:
            return (uint16)pose.rotation[2].z | ((uint32)(uint16)pose.translation.x << 16);
    }
}

void SetupModelGteMatrices(const MATRIX &rotation, const MATRIX &lighting, const ModelPose &local)
{
    FUNCTION_MARKER(0x80084578, "SLUS_003.73");
    static const uint32 lighting_commands[3] = {0x4A6012, 0x4AE012, 0x4B6012};
    static const uint32 rotation_commands[3] = {0x486012, 0x48E012, 0x496012};
    uint32 matrix_words[5], pose_words[5];
    MATRIX light_result = {0}, rotation_result = {0};
    Vector3 parent_translation, t_translation;
    uint32 idx;
    for (idx = 0; idx < 5; ++idx)
        matrix_words[idx] = PackModelRotationWord(rotation, idx);
    for (idx = 0; idx < 5; ++idx)
        xport_gte_write_control(idx, matrix_words[idx]);
    for (idx = 0; idx < 3; ++idx)
        parent_translation.comp[idx] = rotation.t[idx];
    for (idx = 0; idx < 3; ++idx)
        xport_gte_write_control(5 + idx, (uint32)parent_translation.comp[idx]);
    for (idx = 0; idx < 5; ++idx)
        matrix_words[idx] = PackModelRotationWord(lighting, idx);
    for (idx = 0; idx < 5; ++idx)
        xport_gte_write_control(8 + idx, matrix_words[idx]);
    for (idx = 0; idx < 5; ++idx)
        pose_words[idx] = PackModelPoseRotationWord(local, idx);
    xport_gte_write_data(0, (pose_words[0] & 0xFFFF) | (pose_words[1] & 0xFFFF0000));
    xport_gte_write_data(1, pose_words[3]);
    xport_gte_write_data(2, (pose_words[0] >> 16) | (pose_words[2] << 16));
    xport_gte_write_data(3, pose_words[3] >> 16);
    xport_gte_write_data(4, (pose_words[1] & 0xFFFF) | (pose_words[2] & 0xFFFF0000));
    xport_gte_write_data(5, pose_words[4]);
    for (uint32 col = 0; col < 3; ++col)
    {
        xport_gte_execute(lighting_commands[col]);
        for (uint32 row = 0; row < 3; ++row)
            light_result.m[row][col] = xport_gte_read_data(9 + row);
    }
    for (idx = 0; idx < 4; ++idx)
        xport_gte_write_control(8 + idx, PackModelRotationWord(light_result, idx));
    // The original final control word is the sign-extended IR3 result
    xport_gte_write_control(12, light_result.m[2][2]);
    for (uint32 col = 0; col < 3; ++col)
    {
        xport_gte_execute(rotation_commands[col]);
        for (uint32 row = 0; row < 3; ++row)
            rotation_result.m[row][col] = xport_gte_read_data(9 + row);
    }
    ShortVector3 local_translation = local.translation;
    xport_gte_write_data(0, (uint16)local_translation.x | ((uint32)(uint16)local_translation.y << 16));
    xport_gte_write_data(1, local_translation.z);
    xport_gte_execute(0x480012);
    for (idx = 0; idx < 3; ++idx)
        t_translation.comp[idx] = xport_gte_read_data(9 + idx);
    for (idx = 0; idx < 4; ++idx)
        xport_gte_write_control(idx, PackModelRotationWord(rotation_result, idx));
    xport_gte_write_control(4, rotation_result.m[2][2]);
    for (idx = 0; idx < 3; ++idx)
        xport_gte_write_control(5 + idx, (uint32)t_translation.comp[idx]);
}

static void LoadPoseRotation(const ModelPose &pose)
{
    uint32 words[5], i;
    for (i = 0; i < 5; ++i)
        words[i] = PackModelPoseRotationWord(pose, i);
    for (i = 0; i < 5; ++i)
        xport_gte_write_control(i, words[i]);
}

static void StorePoseColumn(ModelPose &output, uint32 col_idx)
{
    ShortVector3 col;
    for (uint32 row = 0; row < 3; ++row)
        col.comp[row] = xport_gte_read_data(9 + row);
    for (uint32 row = 0; row < 3; ++row)
        output.rotation[row].comp[col_idx] = col.comp[row];
}

void ModelPose::Compose(const ModelPose &left, const ModelPose &right)
{
    FUNCTION_MARKER(0x80085674, "SLUS_003.73");
    uint32 pose_words[5], i;
    LoadPoseRotation(left);
    ShortVector3 left_translation = left.translation;
    for (i = 0; i < 3; ++i)
        xport_gte_write_control(5 + i, left_translation.comp[i]);
    for (i = 0; i < 5; ++i)
        pose_words[i] = PackModelPoseRotationWord(right, i);
    xport_gte_write_data(0, (pose_words[0] & 0xffff) | (pose_words[1] & 0xffff0000));
    xport_gte_write_data(1, pose_words[3]);
    xport_gte_write_data(2, (pose_words[0] >> 16) | (pose_words[2] << 16));
    xport_gte_write_data(3, pose_words[3] >> 16);
    xport_gte_write_data(4, (pose_words[1] & 0xffff) | (pose_words[2] & 0xffff0000));
    xport_gte_write_data(5, pose_words[4]);
    xport_gte_execute(0x486012);
    StorePoseColumn(*this, 0);
    xport_gte_execute(0x48E012);
    StorePoseColumn(*this, 1);
    xport_gte_execute(0x496012);
    StorePoseColumn(*this, 2);
    ShortVector3 right_translation = right.translation;
    for (i = 0; i < 3; ++i)
        xport_gte_write_data(9 + i, right_translation.comp[i]);
    xport_gte_execute(0x498012);
    ShortVector3 transformed;
    for (i = 0; i < 3; ++i)
        transformed.comp[i] = xport_gte_read_data(9 + i);
    translation = transformed;
}

void ModelPose::SetRelative(const ModelPose &left, const ModelPose &right)
{
    FUNCTION_MARKER(0x800857A8, "SLUS_003.73");
    uint32 right_words[5], i;
    LoadPoseRotation(left);
    right_words[0] = PackModelPoseRotationWord(right, 0);
    right_words[1] = PackModelPoseRotationWord(right, 1);
    xport_gte_write_data(0, right_words[0]);
    xport_gte_write_data(1, right_words[1]);
    xport_gte_execute(0x486012);
    StorePoseColumn(*this, 0);
    right_words[2] = PackModelPoseRotationWord(right, 2);
    xport_gte_write_data(0, (right_words[2] << 16) | (right_words[1] >> 16));
    xport_gte_write_data(1, right_words[2] >> 16);
    xport_gte_execute(0x486012);
    StorePoseColumn(*this, 1);
    right_words[3] = PackModelPoseRotationWord(right, 3);
    right_words[4] = PackModelPoseRotationWord(right, 4);
    xport_gte_write_data(0, right_words[3]);
    xport_gte_write_data(1, right_words[4]);
    xport_gte_execute(0x486012);
    StorePoseColumn(*this, 2);
    LoadPoseRotation(*this);
    ShortVector3 right_translation = right.translation;
    for (i = 0; i < 3; ++i)
        xport_gte_write_data(9 + i, right_translation.comp[i]);
    xport_gte_execute(0x49E012);
    ShortVector3 relative_translation = left.translation;
    for (i = 0; i < 3; ++i)
    {
        sint16 rotated = xport_gte_read_data(9 + i);
        relative_translation.comp[i] -= rotated;
    }
    translation = relative_translation;
}
