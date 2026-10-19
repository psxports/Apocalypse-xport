#include "model_resources.h"
#include "model_attachment.h"
#include "model_animation.h"
#include "model_render_view.h"
#include "character.h"

static const ModelPose &GetAttachmentPose(Character &character, uint32 idx)
{
    const ModelInstance &model = character.model;
    if (model.flags & 4)
        return *character.GetModelPoses(idx, 1);
    const ModelAnimation &animation = GetModelAnimation(GetModelBank(model.bank).animations, model.animation);
    if (animation.interval)
    {
        InterpolateModelAnimation(animation, model.frame, character.playback.loop_mode == 1, idx, 1);
        return GetModelPoseWorkspace(animation.bone_count)[idx];
    }
    return *GetModelAnimationPoses(animation, model.frame * animation.bone_count + idx, 1);
}

static void TransformAttachment(const ModelAttachmentPoint &point, const ModelPose &bone, const MATRIX &world)
{
    FUNCTION_MARKER(0x8008445C, "SLUS_003.73");
    uint32 words[5];
    xport_gte_write_data(0, (uint16)point.x | ((uint32)(uint16)point.y << 16));
    xport_gte_write_data(1, (uint16)point.z | ((uint32)point.bone << 16));
    for (uint32 idx = 0; idx < 5; ++idx)
        words[idx] = PackModelPoseRotationWord(bone, idx);
    for (uint32 idx = 0; idx < 5; ++idx)
        xport_gte_write_control(idx, words[idx]);
    for (uint32 axis = 0; axis < 3; ++axis)
        xport_gte_write_control(5 + axis, (uint32)(sint32)(sint16)bone.translation.words[axis]);
    for (uint32 axis = 0; axis < 3; ++axis)
        xport_gte_write_control(13 + axis, world.t[axis]);
    xport_gte_execute(0x480012);
    for (uint32 idx = 0; idx < 5; ++idx)
        words[idx] = ReadModelRenderMatrixWord(world, idx);
    for (uint32 idx = 0; idx < 5; ++idx)
        xport_gte_write_control(8 + idx, words[idx]);
    xport_gte_execute(0x4BA012);
}

static uint32 StoreAttachmentPosition(uint32 *output)
{
    output[0] = xport_gte_read_data(25);
    output[1] = xport_gte_read_data(26);
    output[2] = xport_gte_read_data(27);
    uint32 x = output[0];
    uint32 z = output[2];
    output[0] = x << 12;
    uint32 y = output[1];
    output[2] = z << 12;
    output[1] = y << 12;
    return y << 12;
}

uint32 GetModelAttachment(uint32 *output, Character &character, uint32 requested)
{
    FUNCTION_MARKER(0x8007CC10, "SLUS_003.73");
    const ModelAttachmentPoint &point = GetModelAttachmentPoint(GetModelBank(character.model.bank).attachments, requested);
    const ModelPose &bone = GetAttachmentPose(character, point.bone);
    TransformAttachment(point, bone, character.world_transform);
    return StoreAttachmentPosition(output);
}

uint32 Character::GetAttachment(uint32 *output, uint32 requested)
{
    FUNCTION_MARKER(0x8005CEE0, "SLUS_003.73");
    return GetModelAttachment(output, *this, requested & 255);
}

uint32 GetModelBonePosition(uint32 *output, Character &character, const ModelAttachmentPoint &point)
{
    FUNCTION_MARKER(0x8007CD74, "SLUS_003.73");
    const ModelPose &bone = GetAttachmentPose(character, point.bone);
    TransformAttachment(point, bone, character.world_transform);
    return StoreAttachmentPosition(output);
}
