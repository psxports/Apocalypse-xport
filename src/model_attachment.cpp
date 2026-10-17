#include "model_resources.h"
extern "C" {
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
}
#include "model_attachment.h"
#include "model_animation.h"
#include "character.h"
#include "native_object_storage.h"
#include <string.h>

static void StoreAttachmentWord(uint32 guest, void *native, uint32 offset, uint32 value)
{
    if (native)
        memcpy((uint8 *)native + offset, &value, sizeof(value));
    else
        w_u32(guest + offset, value);
}

static uint32 ReadAttachmentWord(uint32 guest, const void *native, uint32 offset)
{
    uint32 value;
    if (!native)
        return r_u32(guest + offset);
    memcpy(&value, (const uint8 *)native + offset, sizeof(value));
    return value;
}

static void TransformAttachment(const ModelAttachmentPoint &point, const uint8 *bone, uint32 object_matrix);

static const uint8 *GetAttachmentPose(uint32 object, uint32 index)
{
    if (r_u16(object) & 4)
        return (const uint8 *)static_cast<Character *>(FindNativeObjectStorage(object))->GetModelPoses(index, 1);
    const ModelAnimation &animation = GetModelAnimation(GetModelBank(r_u8(object + 27)).animations, r_u8(object + 26));
    if (animation.interval)
    {
        InterpolateModelAnimation(animation, r_u8(object + 24), r_u8(object + 296) == 1, index, 1);
        return (const uint8 *)(GetModelPoseWorkspace(animation.bone_count) + index);
    }
    return (const uint8 *)GetModelAnimationPoses(animation, r_u8(object + 24) * animation.bone_count + index, 1);
}

static uint32 ComputeModelAttachment(uint32 output, void *native_output, uint32 object, uint32 requested)
{
    uint32 model, index;
    uint32 x, y, z;
    model = r_u8(object + 27);
    const ModelAttachmentPoint &point = GetModelAttachmentPoint(GetModelBank(model).attachments, requested);
    index = point.bone;
    const uint8 *vertices = GetAttachmentPose(object, index);
    TransformAttachment(point, vertices, object + 324);
    x = xport_draft_gte_data_read(25);
    StoreAttachmentWord(output, native_output, 0, x);
    y = xport_draft_gte_data_read(26);
    StoreAttachmentWord(output, native_output, 4, y);
    z = xport_draft_gte_data_read(27);
    StoreAttachmentWord(output, native_output, 8, z);
    x = ReadAttachmentWord(output, native_output, 0);
    z = ReadAttachmentWord(output, native_output, 8);
    StoreAttachmentWord(output, native_output, 0, x << 12);
    y = ReadAttachmentWord(output, native_output, 4);
    StoreAttachmentWord(output, native_output, 8, z << 12);
    StoreAttachmentWord(output, native_output, 4, y << 12);
    return y << 12;
}

static void TransformAttachment(const ModelAttachmentPoint &point, const uint8 *bone, uint32 object_matrix)
{
    FUNCTION_MARKER(0x8008445C, "SLUS_003.73");
    uint32 value, words[5], index, tx, ty, tz;
    value = (uint16)point.x | ((uint32)(uint16)point.y << 16);
    xport_draft_gte_data_write(0, value);
    value = (uint16)point.z | ((uint32)point.bone << 16);
    xport_draft_gte_data_write(1, value);
    for (index = 0; index < 5; ++index)
        words[index] = xport_load_le32(bone + index * 4);
    for (index = 0; index < 5; ++index)
        xport_draft_gte_control_write(index, words[index]);
    tx = (uint32)(sint32)(sint16)xport_load_le16(bone + 18);
    ty = (uint32)(sint32)(sint16)xport_load_le16(bone + 20);
    tz = (uint32)(sint32)(sint16)xport_load_le16(bone + 22);
    xport_draft_gte_control_write(5, tx);
    xport_draft_gte_control_write(6, ty);
    xport_draft_gte_control_write(7, tz);
    tx = r_u32(object_matrix + 20);
    ty = r_u32(object_matrix + 24);
    tz = r_u32(object_matrix + 28);
    xport_draft_gte_control_write(13, tx);
    xport_draft_gte_control_write(14, ty);
    xport_draft_gte_control_write(15, tz);
    xport_draft_gte_execute(0x480012);
    for (index = 0; index < 5; ++index)
        words[index] = r_u32(object_matrix + index * 4);
    for (index = 0; index < 5; ++index)
        xport_draft_gte_control_write(8 + index, words[index]);
    xport_draft_gte_execute(0x4BA012);
}



uint32 GetModelAttachment(uint32 *output, uint32 object, uint32 requested)
{
    uint32 guest = GuestMemoryAddress(output);
    FUNCTION_MARKER(0x8007CC10, "SLUS_003.73");
    return ComputeModelAttachment(guest, guest ? 0 : output, object, requested);
}



extern "C" uint32 xport_draft_host_sub_8007CC10_p1(void *output, uint32 object, uint32 requested)
{
    FUNCTION_MARKER(0x8007CC10, "SLUS_003.73");
    return GetModelAttachment((uint32 *)output, object, requested);
}

uint32 GetCharacterAttachment(uint32 *output, uint32 object, uint32 requested)
{
    // TODO Establish whether the original thin wrapper was an instance method
    FUNCTION_MARKER(0x8005CEE0, "SLUS_003.73");
    return GetModelAttachment(output, object, requested & 255);
}



extern "C" uint32 xport_draft_host_sub_8005CEE0_p2(uint32 object, void *output, uint32 requested)
{
    FUNCTION_MARKER(0x8005CEE0, "SLUS_003.73");
    return GetCharacterAttachment((uint32 *)output, object, requested);
}





uint32 GetModelBonePosition(uint32 *output, uint32 object, const sint16 *descriptor)
{
    uint32 index;
    uint32 x, y, z;
    uint32 mapped_output, mapped_descriptor;
    uint16 native_index;
    FUNCTION_MARKER(0x8007CD74, "SLUS_003.73");
    mapped_output = GuestMemoryAddress(output);
    mapped_descriptor = GuestMemoryAddress(descriptor);
    if (mapped_descriptor)
        index = r_u16(mapped_descriptor + 6);
    else
    {
        memcpy(&native_index, descriptor + 3, 2);
        index = native_index;
    }
    const uint8 *vertices = GetAttachmentPose(object, index);
    ModelAttachmentPoint point;
    point.x = (sint16)xport_load_le16((const uint8 *)descriptor);
    point.y = (sint16)xport_load_le16((const uint8 *)descriptor + 2);
    point.z = (sint16)xport_load_le16((const uint8 *)descriptor + 4);
    point.bone = (uint16)index;
    TransformAttachment(point, vertices, object + 324);
    x = xport_draft_gte_data_read(25);
    StoreAttachmentWord(mapped_output, mapped_output ? 0 : output, 0, x);
    y = xport_draft_gte_data_read(26);
    StoreAttachmentWord(mapped_output, mapped_output ? 0 : output, 4, y);
    z = xport_draft_gte_data_read(27);
    StoreAttachmentWord(mapped_output, mapped_output ? 0 : output, 8, z);
    x = ReadAttachmentWord(mapped_output, mapped_output ? 0 : output, 0);
    z = ReadAttachmentWord(mapped_output, mapped_output ? 0 : output, 8);
    StoreAttachmentWord(mapped_output, mapped_output ? 0 : output, 0, x << 12);
    y = ReadAttachmentWord(mapped_output, mapped_output ? 0 : output, 4);
    StoreAttachmentWord(mapped_output, mapped_output ? 0 : output, 8, z << 12);
    StoreAttachmentWord(mapped_output, mapped_output ? 0 : output, 4, y << 12);
    return y << 12;
}



extern "C" uint32 xport_draft_host_sub_8007CD74_p13(void *output, uint32 object, void *angles)
{
    FUNCTION_MARKER(0x8007CD74, "SLUS_003.73");
    return GetModelBonePosition((uint32 *)output, object, (const sint16 *)angles);
}
