#include "model_resources.h"
#include "game_sdk_bindings.h"
extern "C" {
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
}
#include "model_animation.h"
#include "model_materials.h"
#include "character.h"
#include "angle_math.h"
#include "native_object_storage.h"
#include <stdio.h>
#include <stdlib.h>

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
    const uint32 object = NativeObjectGuestAddress(this);
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
                angles[row].translation[axis - 4] = 0;
            else
                angles[row].rotation[axis - 1] = 0;
    const ModelAnimation &animation = GetModelAnimation(GetModelBank(model).animations, this->model.animation);
    if (animation.bone_count != vertices)
    {
        free(poses);
        free(angles);
        GameDiagnostic("Character animation bone count mismatch\n");
        abort();
    }
    // Prepare the current zero-angle pose before attachment queries can read it
    UpdateModelAnimationData(object);
    const ModelPose *source = animation.interval ? GetModelPoseWorkspace(vertices) : GetModelAnimationPoses(animation, this->model.frame * vertices, vertices);
    for (uint32 bone = 0; bone < vertices; ++bone)
        poses[bone] = source[bone];
    ReleaseModelAnimation();
    model_hierarchy = &table;
    model_angles = angles;
    model_poses = poses;
    model_pose_count = vertices;
}

void UpdateModelAnimationData(uint32 object)
{
    FUNCTION_MARKER(0x8007CECC, "SLUS_003.73");
    const ModelInstance &model = VisualObject::Attach(object).model;
    const ModelAnimation &animation = GetModelAnimation(GetModelBank(model.bank).animations, model.animation);
    if (animation.interval)
        InterpolateModelAnimation(animation, model.frame, r_u8(object + 296) == 1, 0, animation.bone_count);
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
            const uint16 *left = vector < 3 ? source0[bone].rotation[vector] : source0[bone].translation;
            const uint16 *right = vector < 3 ? source1[bone].rotation[vector] : source1[bone].translation;
            uint16 *destination = vector < 3 ? output[bone].rotation[vector] : output[bone].translation;
            uint16 difference[4];
            uint32 initial[3], interpolated[3];
            SubtractAngleVector(difference, right, left);
            for (uint32 axis = 0; axis < 3; ++axis)
                initial[axis] = (uint32)(sint32)(sint16)left[axis];
            for (uint32 axis = 0; axis < 3; ++axis)
                xport_draft_gte_data_write(25 + axis, initial[axis]);
            for (uint32 axis = 0; axis < 3; ++axis)
                xport_draft_gte_data_write(9 + axis, (uint32)(sint32)(sint16)difference[axis]);
            xport_draft_gte_data_write(8, factor);
            xport_draft_gte_execute(0x1A8003E);
            for (uint32 axis = 0; axis < 3; ++axis)
                interpolated[axis] = xport_draft_gte_data_read(25 + axis);
            for (uint32 axis = 0; axis < 3; ++axis)
                destination[axis] = (uint16)interpolated[axis];
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
    const uint32 tick = r_u32(0x800FFB40);
    const uint32 old_tick = r_u32(0x800FFAB4);
    const uint32 step = tick >= old_tick ? tick - old_tick : 1;
    AdvanceModelColors(bank.colors, bank.color_animation, step);
}
