extern "C" {
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
}
#include "model_attachment.h"
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

static uint32 ComputeModelAttachment(uint32 output, void *native_output, uint32 object, uint32 requested)
{
    uint32 model, entry, descriptor, flags, index, data, animation, vertices;
    uint32 table, interval, frames, frame_table, frame, offset, x, y, z;
    model = r_u8(object + 27);
    entry = 0x800EAEF8 + model * 64;
    descriptor = r_u32(entry + 28) + requested * 8;
    flags = r_u16(object);
    index = r_u16(descriptor + 6);
    data = r_u32(entry + 24);
    animation = r_u8(object + 26);
    if (flags & 4)
        vertices = r_u32(object + 356) + index * 24;
    else
    {
        table = data + animation * 8;
        interval = r_u16(table + 10);
        if (interval)
        {
            model = r_u8(object + 27);
            frame_table = r_u32(0x800EAEF8 + model * 64 + 16);
            frames = r_u32(frame_table - 4);
            sub_8007CF38(4, interval + 1, data, object, index, frames);
            vertices = 0x800ED760 + index * 24;
        }
        else
        {
            frame_table = r_u32(entry + 16);
            frames = r_u32(frame_table - 4);
            frame = r_u8(object + 24);
            offset = r_u32(table + 4);
            vertices = data + offset + (frame * frames + index) * 24;
        }
    }
    sub_8008445C(descriptor, vertices, object + 324);
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

static void TransformAttachment(uint32 descriptor, const void *native_descriptor, uint32 bone, uint32 object_matrix)
{
    uint32 value, words[5], index, tx, ty, tz;
    value = ReadAttachmentWord(descriptor, native_descriptor, 0);
    xport_draft_gte_data_write(0, value);
    value = ReadAttachmentWord(descriptor, native_descriptor, 4);
    xport_draft_gte_data_write(1, value);
    for (index = 0; index < 5; ++index)
        words[index] = r_u32(bone + index * 4);
    for (index = 0; index < 5; ++index)
        xport_draft_gte_control_write(index, words[index]);
    tx = (uint32)(sint32)(sint16)r_u16(bone + 18);
    ty = (uint32)(sint32)(sint16)r_u16(bone + 20);
    tz = (uint32)(sint32)(sint16)r_u16(bone + 22);
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

void TransformModelAttachment(uint32 descriptor, uint32 bone, uint32 object_matrix)
{
    FUNCTION_MARKER(0x8008445C, "SLUS_003.73");
    TransformAttachment(descriptor, 0, bone, object_matrix);
}

uint32 GetModelAttachment(uint32 *output, uint32 object, uint32 requested)
{
    uint32 guest = GuestMemoryAddress(output);
    FUNCTION_MARKER(0x8007CC10, "SLUS_003.73");
    return ComputeModelAttachment(guest, guest ? 0 : output, object, requested);
}

extern "C" uint32 sub_8007CC10(uint32 output, uint32 object, uint32 requested)
{
    FUNCTION_MARKER(0x8007CC10, "SLUS_003.73");
    return GetModelAttachment((uint32 *)psx_addr(output, 12), object, requested);
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

extern "C" uint32 sub_8005CEE0(uint32 object, uint32 output, uint32 requested)
{
    FUNCTION_MARKER(0x8005CEE0, "SLUS_003.73");
    return GetCharacterAttachment((uint32 *)psx_addr(output, 12), object, requested);
}

extern "C" uint32 xport_draft_host_sub_8005CEE0_p2(uint32 object, void *output, uint32 requested)
{
    FUNCTION_MARKER(0x8005CEE0, "SLUS_003.73");
    return GetCharacterAttachment((uint32 *)output, object, requested);
}

extern "C" void sub_8008445C(uint32 descriptor, uint32 bone, uint32 object_matrix)
{
    FUNCTION_MARKER(0x8008445C, "SLUS_003.73");
    TransformModelAttachment(descriptor, bone, object_matrix);
}

extern "C" void xport_draft_host_sub_8008445C_p1(const void *descriptor, uint32 bone, uint32 object_matrix)
{
    FUNCTION_MARKER(0x8008445C, "SLUS_003.73");
    TransformAttachment(0, descriptor, bone, object_matrix);
}

uint32 GetModelBonePosition(uint32 *output, uint32 object, const sint16 *descriptor)
{
    uint32 model, flags, index, animation, entry, data, frame_table, frames;
    uint32 table, interval, vertices, frame, offset, x, y, z;
    uint32 mapped_output, mapped_descriptor;
    uint16 native_index;
    FUNCTION_MARKER(0x8007CD74, "SLUS_003.73");
    mapped_output = GuestMemoryAddress(output);
    mapped_descriptor = GuestMemoryAddress(descriptor);
    model = r_u8(object + 27);
    flags = r_u16(object);
    if (mapped_descriptor)
        index = r_u16(mapped_descriptor + 6);
    else
    {
        memcpy(&native_index, descriptor + 3, 2);
        index = native_index;
    }
    animation = r_u8(object + 26);
    entry = 0x800EAEF8 + 64 * model;
    data = r_u32(entry + 24);
    frame_table = r_u32(entry + 16);
    frames = r_u32(frame_table - 4);
    if (flags & 4)
        vertices = r_u32(object + 356) + 24 * index;
    else
    {
        table = data + 8 * animation;
        interval = r_u16(table + 10);
        if (interval)
        {
            model = r_u8(object + 27);
            frame_table = r_u32(0x800EAEF8 + 64 * model + 16);
            frames = r_u32(frame_table - 4);
            sub_8007CF38(4, interval + 1, data, object, index, frames);
            vertices = 0x800ED760 + 24 * index;
        }
        else
        {
            frame = r_u8(object + 24);
            offset = r_u32(table + 4);
            vertices = data + offset + 24 * (frame * frames + index);
        }
    }
    TransformAttachment(mapped_descriptor, mapped_descriptor ? 0 : descriptor, vertices, object + 324);
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

extern "C" uint32 sub_8007CD74(uint32 output, uint32 object, uint32 angles)
{
    FUNCTION_MARKER(0x8007CD74, "SLUS_003.73");
    return GetModelBonePosition((uint32 *)psx_addr(output, 12), object, (const sint16 *)psx_addr(angles, 8));
}

extern "C" uint32 xport_draft_host_sub_8007CD74_p13(void *output, uint32 object, void *angles)
{
    FUNCTION_MARKER(0x8007CD74, "SLUS_003.73");
    return GetModelBonePosition((uint32 *)output, object, (const sint16 *)angles);
}
