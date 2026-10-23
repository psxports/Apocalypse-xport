#include <stdlib.h>

#include "model_instance.h"
#include "model_bank.h"
#include "model_attachment.h"
#include "model_animation.h"
#include "model_render_view.h"
#include "character.h"
#include "game_diagnostics.h"

static const ModelAttachmentPoint &GetAttachmentPoint(const ModelAttachmentTable *table, uint32 idx)
{
    if (!table || idx >= table->count)
    {
        GameDiagnostic(("Invalid model attachment point\n"));
        abort();
    }
    return table->points[idx];
}

static const ModelPose &GetAttachmentPose(Character &character, uint16 idx)
{
    const ModelInstance &model = character.model;
    if (model.flags & 4)
        return *character.GetModelPoses(idx, 1);
    const ModelAnimation &animation = GetModelAnimation(ModelBank::Get(model.bank).animations, model.animation);
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
        xport_gte_write_control(5 + axis, bone.translation.comp[axis]);
    for (uint32 axis = 0; axis < 3; ++axis)
        xport_gte_write_control(13 + axis, world.t[axis]);
    xport_gte_execute(0x480012);
    for (uint32 idx = 0; idx < 5; ++idx)
        words[idx] = PackModelRotationWord(world, idx);
    for (uint32 idx = 0; idx < 5; ++idx)
        xport_gte_write_control(8 + idx, words[idx]);
    xport_gte_execute(0x4BA012);
}

static uint32 StoreAttachmentPos(Vector3 &output)
{
    output.x = xport_gte_read_data(25);
    output.y = xport_gte_read_data(26);
    output.z = xport_gte_read_data(27);
    uint32 x = output.x;
    uint32 z = output.z;
    output.x = x << 12;
    uint32 y = output.y;
    output.z = z << 12;
    output.y = y << 12;
    return y << 12;
}

uint32 GetModelAttachment(Vector3 &output, Character &character, uint32 requested)
{
    FUNCTION_MARKER(0x8007CC10, "SLUS_003.73");
    const ModelAttachmentPoint &point = GetAttachmentPoint(ModelBank::Get(character.model.bank).attachments, requested);
    const ModelPose &bone = GetAttachmentPose(character, point.bone);
    TransformAttachment(point, bone, character.world_transform);
    return StoreAttachmentPos(output);
}

uint32 GetModelBonePos(Vector3 &output, Character &character, const ModelAttachmentPoint &point)
{
    FUNCTION_MARKER(0x8007CD74, "SLUS_003.73");
    const ModelPose &bone = GetAttachmentPose(character, point.bone);
    TransformAttachment(point, bone, character.world_transform);
    return StoreAttachmentPos(output);
}
