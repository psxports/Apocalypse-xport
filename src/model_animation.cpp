#include <stdio.h>
#include <stdlib.h>

#include "geometry_commands.h"
#include "model_render.h"
#include "model_resources.h"
#include "game_diagnostics.h"

#include "model_animation.h"
#include "model_materials.h"
#include "character.h"
#include "angle_math.h"

static uint32 DivideAnimationWord(uint32 numerator, uint32 denominator)
{
    if (!denominator)
        return (sint32)numerator < 0 ? 1 : (uint32)-1;
    if (numerator == 0x80000000 && denominator == 0xFFFFFFFF)
        return numerator;
    return (uint32)((sint32)numerator / (sint32)denominator);
}

static uint32 RemainderAnimationWord(uint32 numerator, uint32 denominator)
{
    if (!denominator)
        return numerator;
    if (numerator == 0x80000000 && denominator == 0xFFFFFFFF)
        return 0;
    return (uint32)((sint32)numerator % (sint32)denominator);
}

void Character::InitializeModelAnimation(const ModelHierarchyTable &table)
{
    FUNCTION_MARKER(0x8007CAC8, "SLUS_003.73");
    const uint32 model = this->model.bank;
    const uint32 vertices = GetModelBank(model).geometry.count;
    for (uint32 row = 0; row < table.count; ++row)
        if (table.links[row].bone >= vertices || (table.links[row].parent != 65535 && table.links[row].parent >= table.count))
        {
            GameDiagnostic("Invalid character model hierarchy\n");
            abort();
        }
    if (!vertices || vertices > 0xFFFFFFFF / sizeof(ModelPose) || !table.count || table.count > 0xFFFFFFFF / sizeof(ModelJointAngles))
    {
        GameDiagnostic("Invalid character animation dimensions\n");
        abort();
    }
    ModelPose *poses = (ModelPose *)malloc(vertices * sizeof(ModelPose));
    ModelJointAngles *angles = (ModelJointAngles *)malloc(table.count * sizeof(ModelJointAngles));
    if (!poses || !angles)
    {
        free(poses);
        free(angles);
        GameDiagnostic("Cannot allocate character model animation\n");
        abort();
    }
    for (uint32 row = 0; row < table.count; ++row)
        for (uint32 axis = 6; axis > 0; --axis)
            if (axis > 3)
                angles[row].translation.words[axis - 4] = 0;
            else
                angles[row].rotation.words[axis - 1] = 0;
    const ModelAnimation &animation = GetModelAnimation(GetModelBank(model).animations, this->model.animation);
    if (animation.bone_count != vertices)
    {
        free(poses);
        free(angles);
        GameDiagnostic("Character animation bone count mismatch\n");
        abort();
    }
    // Prepare the current zero-angle pose before attachment queries can read it
    UpdateModelAnimationData();
    const ModelPose *source = animation.interval ? GetModelPoseWorkspace(vertices) : GetModelAnimationPoses(animation, this->model.frame * vertices, vertices);
    for (uint32 bone = 0; bone < vertices; ++bone)
        poses[bone] = source[bone];
    ReleaseModelAnimation();
    model_hierarchy = &table;
    model_angles = angles;
    model_poses = poses;
    model_pose_count = vertices;
}

void Character::UpdateModelAnimationData()
{
    FUNCTION_MARKER(0x8007CECC, "SLUS_003.73");
    const ModelAnimation &animation = GetModelAnimation(GetModelBank(model.bank).animations, model.animation);
    if (animation.interval)
        InterpolateModelAnimation(animation, model.frame, playback.loop_mode == 1, 0, animation.bone_count);
}

void InterpolateModelAnimation(const ModelAnimation &animation, uint32 frame, uint32 loop, uint32 first, uint32 count)
{
    FUNCTION_MARKER(0x8007CF38, "SLUS_003.73");
    uint32 interval = (uint32)animation.interval + 1;
    uint32 lower = frame - RemainderAnimationWord(frame, interval);
    uint32 upper = lower + interval;
    uint32 limit = animation.frame_count;
    uint32 factor = 0;
    if ((sint32)upper < (sint32)limit)
        factor = DivideAnimationWord((frame - lower) << 12, upper - lower);
    else if (loop)
    {
        upper = 0;
        factor = DivideAnimationWord((frame - lower) << 12, limit - lower);
    }
    else
    {
        lower -= interval;
        if ((sint32)lower < 0)
            lower = 0;
        upper -= interval;
        if (upper)
            factor = DivideAnimationWord((frame - lower) << 12, upper - lower);
    }
    if (first > animation.bone_count || count > animation.bone_count - first)
    {
        GameDiagnostic("Invalid interpolated model bone span\n");
        abort();
    }
    ModelPose *output = GetModelPoseWorkspace(animation.bone_count) + first;
    const ModelPose *source0 = GetModelAnimationPoses(animation, DivideAnimationWord(animation.bone_count * lower, interval) + first, count);
    const ModelPose *source1 = GetModelAnimationPoses(animation, DivideAnimationWord(animation.bone_count * upper, interval) + first, count);
    for (uint32 bone = 0; bone < count; ++bone)
        for (uint32 vector = 0; vector < 4; ++vector)
        {
            const ShortVector3 &left = vector < 3 ? source0[bone].rotation[vector] : source0[bone].translation;
            const ShortVector3 &right = vector < 3 ? source1[bone].rotation[vector] : source1[bone].translation;
            ShortVector3 &destination = vector < 3 ? output[bone].rotation[vector] : output[bone].translation;
            ShortVector3 difference;
            Vector3 initial, interpolated;
            SubtractAngleVector(difference.words, right.words, left.words);
            for (uint32 axis = 0; axis < 3; ++axis)
                initial.words[axis] = (uint32)(sint32)(sint16)left.words[axis];
            for (uint32 axis = 0; axis < 3; ++axis)
                xport_gte_write_data(25 + axis, initial.words[axis]);
            for (uint32 axis = 0; axis < 3; ++axis)
                xport_gte_write_data(9 + axis, (uint32)(sint32)(sint16)difference.words[axis]);
            xport_gte_write_data(8, factor);
            ExecuteGeometryCommand(0x1A8003E);
            for (uint32 axis = 0; axis < 3; ++axis)
                interpolated.words[axis] = xport_gte_read_data(25 + axis);
            for (uint32 axis = 0; axis < 3; ++axis)
                destination.words[axis] = (uint16)interpolated.words[axis];
        }
}

void SetModelBoneTable(uint32 model_index, const ModelAttachmentTable &table)
{
    FUNCTION_MARKER(0x8007CA50, "SLUS_003.73");
    GetModelBank(model_index).attachments = &table;
}

// Resource-index owner has no recovered instance receiver
void UpdateModelColorAnimation(uint32 model_index)
{
    FUNCTION_MARKER(0x80080398, "SLUS_003.73");
    if (model_index == 0xFFFFFFFF)
        return;
    ModelBank &bank = GetModelBank(model_index);
    if (!bank.color_animation.present)
        return;
    SelectModelPalette(bank.colors);
    const uint32 tick = model_render_state.frame;
    const uint32 old_tick = model_render_state.previous_frame;
    const uint32 step = tick >= old_tick ? tick - old_tick : 1;
    AdvanceModelColors(bank.colors, bank.color_animation, step);
}
