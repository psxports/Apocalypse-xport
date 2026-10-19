#include <stdlib.h>

#include "model_resources.h"
#include "game_diagnostics.h"

#include "angle_math.h"
#include "model_matrix.h"
#include "model_animation.h"
#include "model_render_view.h"
#include "vector3.h"
#include "character.h"

uint32 PackModelPoseRotationWord(const ModelPose &pose, uint32 index)
{
    switch (index)
    {
        case 0:
            return pose.rotation[0].words[0] | ((uint32)pose.rotation[0].words[1] << 16);
        case 1:
            return pose.rotation[0].words[2] | ((uint32)pose.rotation[1].words[0] << 16);
        case 2:
            return pose.rotation[1].words[1] | ((uint32)pose.rotation[1].words[2] << 16);
        case 3:
            return pose.rotation[2].words[0] | ((uint32)pose.rotation[2].words[1] << 16);
        // The fifth device word includes translation X rather than padding
        default:
            return pose.rotation[2].words[2] | ((uint32)pose.translation.words[0] << 16);
    }
}

void SetupModelGteMatrices(const MATRIX &rotation, const MATRIX &lighting, const ModelPose &local)
{
    FUNCTION_MARKER(0x80084578, "SLUS_003.73");
    uint32 matrix_words[5], pose_words[5], light_words[5], rotation_words[5];
    Vector3 light_result[3], rotation_result[3];
    Vector3 parent_translation, local_translation, t_translation;
    uint32 index;
    for (index = 0; index < 5; ++index)
        matrix_words[index] = ReadModelRenderMatrixWord(rotation, index);
    for (index = 0; index < 5; ++index)
        xport_gte_write_control(index, matrix_words[index]);
    for (index = 0; index < 3; ++index)
        parent_translation.words[index] = (uint32)rotation.t[index];
    for (index = 0; index < 3; ++index)
        xport_gte_write_control(5 + index, parent_translation.words[index]);
    for (index = 0; index < 5; ++index)
        matrix_words[index] = ReadModelRenderMatrixWord(lighting, index);
    for (index = 0; index < 5; ++index)
        xport_gte_write_control(8 + index, matrix_words[index]);
    for (index = 0; index < 5; ++index)
        pose_words[index] = PackModelPoseRotationWord(local, index);
    xport_gte_write_data(0, (pose_words[0] & 0xFFFF) | (pose_words[1] & 0xFFFF0000));
    xport_gte_write_data(1, pose_words[3]);
    xport_gte_write_data(2, (pose_words[0] >> 16) | (pose_words[2] << 16));
    xport_gte_write_data(3, pose_words[3] >> 16);
    xport_gte_write_data(4, (pose_words[1] & 0xFFFF) | (pose_words[2] & 0xFFFF0000));
    xport_gte_write_data(5, pose_words[4]);
    xport_gte_execute(0x4A6012);
    light_result[0].words[0] = xport_gte_read_data(9);
    light_result[0].words[1] = xport_gte_read_data(10);
    light_result[0].words[2] = xport_gte_read_data(11);
    xport_gte_execute(0x4AE012);
    light_words[0] = (light_result[0].words[0] & 0xFFFF) | (xport_gte_read_data(9) << 16);
    light_result[1].words[1] = xport_gte_read_data(10);
    light_result[1].words[2] = xport_gte_read_data(11);
    light_words[3] = (light_result[0].words[2] & 0xFFFF) | (light_result[1].words[2] << 16);
    xport_gte_execute(0x4B6012);
    light_result[2].words[0] = xport_gte_read_data(9);
    light_words[1] = (light_result[0].words[1] << 16) | (light_result[2].words[0] & 0xFFFF);
    light_result[2].words[1] = xport_gte_read_data(10);
    light_words[2] = (light_result[1].words[1] & 0xFFFF) | (light_result[2].words[1] << 16);
    light_words[4] = xport_gte_read_data(11);
    for (index = 0; index < 5; ++index)
        xport_gte_write_control(8 + index, light_words[index]);
    xport_gte_execute(0x486012);
    rotation_result[0].words[0] = xport_gte_read_data(9);
    rotation_result[0].words[1] = xport_gte_read_data(10);
    rotation_result[0].words[2] = xport_gte_read_data(11);
    xport_gte_execute(0x48E012);
    rotation_result[1].words[0] = xport_gte_read_data(9);
    rotation_result[1].words[1] = xport_gte_read_data(10);
    rotation_result[1].words[2] = xport_gte_read_data(11);
    xport_gte_execute(0x496012);
    rotation_words[0] = (rotation_result[0].words[0] & 0xFFFF) | (rotation_result[1].words[0] << 16);
    rotation_words[3] = (rotation_result[0].words[2] & 0xFFFF) | (rotation_result[1].words[2] << 16);
    rotation_result[2].words[0] = xport_gte_read_data(9);
    rotation_result[2].words[1] = xport_gte_read_data(10);
    rotation_result[2].words[2] = xport_gte_read_data(11);
    rotation_words[1] = (rotation_result[0].words[1] << 16) | (rotation_result[2].words[0] & 0xFFFF);
    rotation_words[2] = (rotation_result[1].words[1] & 0xFFFF) | (rotation_result[2].words[1] << 16);
    rotation_words[4] = rotation_result[2].words[2];
    local_translation.words[0] = local.translation.words[0];
    local_translation.words[1] = (uint32)(sint32)(sint16)local.translation.words[1];
    local_translation.words[2] = (uint32)(sint32)(sint16)local.translation.words[2];
    xport_gte_write_data(0, local_translation.words[0] | (local_translation.words[1] << 16));
    xport_gte_write_data(1, local_translation.words[2]);
    xport_gte_execute(0x480012);
    for (index = 0; index < 3; ++index)
        t_translation.words[index] = xport_gte_read_data(9 + index);
    for (index = 0; index < 5; ++index)
        xport_gte_write_control(index, rotation_words[index]);
    for (index = 0; index < 3; ++index)
        xport_gte_write_control(5 + index, t_translation.words[index]);
}

static void LoadPoseRotation(const ModelPose &matrix)
{
    uint32 words[5], i;
    for (i = 0; i < 5; ++i)
        words[i] = PackModelPoseRotationWord(matrix, i);
    for (i = 0; i < 5; ++i)
        xport_gte_write_control(i, words[i]);
}

static void StorePoseColumn(ModelPose &output, uint32 col_idx)
{
    ShortVector3 col;
    for (uint32 row = 0; row < 3; ++row)
        col[row] = (sint16)xport_gte_read_data(9 + row);
    for (uint32 row = 0; row < 3; ++row)
        output.rotation[row].words[col_idx] = (uint16)col[row];
}

static void ComposePoseMatrix(ModelPose &output, const ModelPose &left, const ModelPose &right)
{
    FUNCTION_MARKER(0x80085674, "SLUS_003.73");
    uint32 pose_words[5], i;
    Vector3 left_translation, right_translation, t_translation;
    LoadPoseRotation(left);
    for (i = 0; i < 3; ++i)
        left_translation.words[i] = (uint32)(sint32)(sint16)left.translation.words[i];
    for (i = 0; i < 3; ++i)
        xport_gte_write_control(5 + i, left_translation.words[i]);
    for (i = 0; i < 5; ++i)
        pose_words[i] = PackModelPoseRotationWord(right, i);
    xport_gte_write_data(0, (pose_words[0] & 0xffff) | (pose_words[1] & 0xffff0000));
    xport_gte_write_data(1, pose_words[3]);
    xport_gte_write_data(2, (pose_words[0] >> 16) | (pose_words[2] << 16));
    xport_gte_write_data(3, pose_words[3] >> 16);
    xport_gte_write_data(4, (pose_words[1] & 0xffff) | (pose_words[2] & 0xffff0000));
    xport_gte_write_data(5, pose_words[4]);
    xport_gte_execute(0x486012);
    StorePoseColumn(output, 0);
    xport_gte_execute(0x48E012);
    StorePoseColumn(output, 1);
    xport_gte_execute(0x496012);
    StorePoseColumn(output, 2);
    for (i = 0; i < 3; ++i)
        right_translation.words[i] = (uint32)(sint32)(sint16)right.translation.words[i];
    for (i = 0; i < 3; ++i)
        xport_gte_write_data(9 + i, right_translation.words[i]);
    xport_gte_execute(0x498012);
    for (i = 0; i < 3; ++i)
        t_translation.words[i] = xport_gte_read_data(9 + i);
    for (i = 0; i < 3; ++i)
        output.translation.words[i] = t_translation.words[i];
}

static void RelativePoseMatrix(ModelPose &output, const ModelPose &left, const ModelPose &right)
{
    FUNCTION_MARKER(0x800857A8, "SLUS_003.73");
    uint32 right_words[5], i;
    Vector3 relative_translation, right_translation;
    LoadPoseRotation(left);
    right_words[0] = PackModelPoseRotationWord(right, 0);
    right_words[1] = PackModelPoseRotationWord(right, 1);
    xport_gte_write_data(0, right_words[0]);
    xport_gte_write_data(1, right_words[1]);
    xport_gte_execute(0x486012);
    StorePoseColumn(output, 0);
    right_words[2] = PackModelPoseRotationWord(right, 2);
    xport_gte_write_data(0, (right_words[2] << 16) | (right_words[1] >> 16));
    xport_gte_write_data(1, right_words[2] >> 16);
    xport_gte_execute(0x486012);
    StorePoseColumn(output, 1);
    right_words[3] = PackModelPoseRotationWord(right, 3);
    right_words[4] = PackModelPoseRotationWord(right, 4);
    xport_gte_write_data(0, right_words[3]);
    xport_gte_write_data(1, right_words[4]);
    xport_gte_execute(0x486012);
    StorePoseColumn(output, 2);
    LoadPoseRotation(output);
    for (i = 0; i < 3; ++i)
        right_translation.words[i] = (uint32)(sint32)(sint16)right.translation.words[i];
    for (i = 0; i < 3; ++i)
        xport_gte_write_data(9 + i, right_translation.words[i]);
    xport_gte_execute(0x49E012);
    for (i = 0; i < 3; ++i)
        relative_translation.words[i] = (uint32)(sint32)(sint16)left.translation.words[i];
    for (i = 0; i < 3; ++i)
        relative_translation.words[i] -= xport_gte_read_data(9 + i);
    for (i = 0; i < 3; ++i)
        output.translation.words[i] = relative_translation.words[i];
}

struct PoseParent
{
    ModelPose pose;
    uint32 ordinal;
};

class PoseHierarchyWorkspace
{
public:
    PoseHierarchyWorkspace(uint32 count)
    {
        if (!count || count > 0xFFFFFFFF / sizeof(PoseParent))
        {
            GameDiagnostic("Invalid pose hierarchy workspace size\n");
            abort();
        }
        values = (PoseParent *)malloc(count * sizeof(PoseParent));
        if (!values)
        {
            GameDiagnostic("Cannot allocate pose hierarchy workspace\n");
            abort();
        }
    }

    ~PoseHierarchyWorkspace()
    {
        free(values);
    }

    PoseParent *Get()
    {
        return values;
    }

private:
    PoseHierarchyWorkspace(const PoseHierarchyWorkspace &);
    PoseHierarchyWorkspace &operator=(const PoseHierarchyWorkspace &);
    PoseParent *values;
};

uint32 BuildModelPose(Character &character)
{
    FUNCTION_MARKER(0x8007D148, "SLUS_003.73");
    sint32 top = -1;
    uint32 result = character.model.flags & 4;
    if (!result || !character.HasModelPoses())
        return result;
    if (!character.HasModelJointAngles())
        return 0x800F0000;
    const uint32 total = character.GetModelPoseCount();
    ModelPose *output = character.GetModelPoses(0, total);
    const ModelAnimation &animation = GetModelAnimation(GetModelBank(character.model.bank).animations, character.model.animation);
    if (animation.bone_count != total)
    {
        GameDiagnostic("Character pose owner no longer matches its model bank\n");
        abort();
    }
    for (uint32 bone = 0; bone < total; ++bone)
        output[bone].rotation[0].words[0] = 0x8000;
    const ModelHierarchyTable &hierarchy = character.GetModelHierarchy();
    PoseHierarchyWorkspace workspace(hierarchy.count);
    PoseParent *parents = workspace.Get();
    const ModelPose *source = animation.interval ? GetModelPoseWorkspace(total) : GetModelAnimationPoses(animation, character.model.frame * total, total);
    for (uint32 ordinal = 0; ordinal < hierarchy.count; ++ordinal)
    {
        const ModelHierarchyLink &link = hierarchy.links[ordinal];
        const uint32 joint = link.bone;
        if (top >= 0)
            while (top >= 0 && link.parent < parents[top].ordinal)
                --top;
        const ModelPose &base = source[joint];
        const ModelJointAngles &angles = character.GetModelJointAngles(ordinal);
        if (angles.rotation.words[0] || angles.rotation.words[1] || angles.rotation.words[2])
        {
            MATRIX joint_rotation;
            BuildModelRotationMatrix(angles.rotation.words, joint_rotation);
            ModelPose matrix;
            for (uint32 row = 0; row < 3; ++row)
                for (uint32 col = 0; col < 3; ++col)
                    matrix.rotation[row].words[col] = (uint16)joint_rotation.m[row][col];
            for (uint32 axis = 0; axis < 3; ++axis)
                matrix.translation.words[axis] = 0;
            ComposePoseMatrix(matrix, base, matrix);
            if (top >= 0)
                ComposePoseMatrix(matrix, parents[top].pose, matrix);
            ++top;
            RelativePoseMatrix(parents[top].pose, matrix, base);
            parents[top].ordinal = ordinal;
            output[joint] = matrix;
        }
        else if (top < 0)
            output[joint] = base;
        else
            ComposePoseMatrix(output[joint], parents[top].pose, base);
    }
    result = animation.interval;
    if (!result)
        for (uint32 bone = 0; bone < total; ++bone)
        {
            if ((sint16)output[bone].rotation[0].words[0] == -32768)
                output[bone] = source[bone];
            result = bone + 1 < total;
        }
    return result;
}

uint32 Character::UpdateWorldTransform()
{
    FUNCTION_MARKER(0x8007FDF0, "SLUS_003.73");
    BuildModelRotationMatrix(model.angles.words, world_transform);
    if (model.flags & 0x200)
        ScaleModelRotationMatrix(model.scale.words, world_transform);
    world_transform.t[0] = (sint32)model.pos.words[0] >> 12;
    world_transform.t[2] = (sint32)model.pos.words[2] >> 12;
    world_transform.t[1] = (sint32)model.pos.words[1] >> 12;
    return world_transform.t[1];
}
