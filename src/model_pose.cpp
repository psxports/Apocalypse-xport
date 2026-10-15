#include "game_sdk_bindings.h"
extern "C" {
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
}
#include "angle_math.h"
#include "model_animation.h"
#include "native_object_storage.h"
#include <stdio.h>
#include <stdlib.h>

static uint32 ReadPoseWord(const uint8 *input, uint32 offset)
{
    uint32 address = GuestMemoryAddress(input);
    return address ? r_u32(address + offset) : xport_load_le32(input + offset);
}

static uint32 ReadPoseHalf(const uint8 *input, uint32 offset)
{
    uint32 address = GuestMemoryAddress(input);
    return address ? r_u16(address + offset) : xport_load_le16(input + offset);
}

static void StorePoseHalf(uint8 *output, uint32 offset, uint16 value)
{
    uint32 address = GuestMemoryAddress(output);
    if (address)
        w_u16(address + offset, value);
    else
        xport_store_le16(output + offset, value);
}

static void LoadPoseRotation(const uint8 *matrix)
{
    uint32 words[5], i;
    for (i = 0; i < 5; ++i)
        words[i] = ReadPoseWord(matrix, 4 * i);
    for (i = 0; i < 5; ++i)
        xport_gte_write_control(i, words[i]);
}

static void StorePoseColumn(uint8 *output, uint32 column)
{
    uint32 values[3], row;
    for (row = 0; row < 3; ++row)
        values[row] = xport_gte_read_data(9 + row);
    for (row = 0; row < 3; ++row)
        StorePoseHalf(output, 6 * row + 2 * column, (uint16)values[row]);
}

void ComposePoseMatrix(uint8 *output, const uint8 *left, const uint8 *right)
{
    FUNCTION_MARKER(0x80085674, "SLUS_003.73");
    uint32 words[5], translation[3], i;
    LoadPoseRotation(left);
    for (i = 0; i < 3; ++i)
        translation[i] = (uint32)(sint32)(sint16)ReadPoseHalf(left, 18 + 2 * i);
    for (i = 0; i < 3; ++i)
        xport_gte_write_control(5 + i, translation[i]);
    for (i = 0; i < 5; ++i)
        words[i] = ReadPoseWord(right, 4 * i);
    xport_gte_write_data(0, (words[0] & 0xffff) | (words[1] & 0xffff0000));
    xport_gte_write_data(1, words[3]);
    xport_gte_write_data(2, (words[0] >> 16) | (words[2] << 16));
    xport_gte_write_data(3, words[3] >> 16);
    xport_gte_write_data(4, (words[1] & 0xffff) | (words[2] & 0xffff0000));
    xport_gte_write_data(5, words[4]);
    xport_gte_execute(0x486012);
    StorePoseColumn(output, 0);
    xport_gte_execute(0x48E012);
    StorePoseColumn(output, 1);
    xport_gte_execute(0x496012);
    StorePoseColumn(output, 2);
    for (i = 0; i < 3; ++i)
        translation[i] = (uint32)(sint32)(sint16)ReadPoseHalf(right, 18 + 2 * i);
    for (i = 0; i < 3; ++i)
        xport_gte_write_data(9 + i, translation[i]);
    xport_gte_execute(0x498012);
    for (i = 0; i < 3; ++i)
        words[i] = xport_gte_read_data(9 + i);
    for (i = 0; i < 3; ++i)
        StorePoseHalf(output, 18 + 2 * i, (uint16)words[i]);
}

void RelativePoseMatrix(uint8 *output, const uint8 *left, const uint8 *right)
{
    FUNCTION_MARKER(0x800857A8, "SLUS_003.73");
    uint32 first, second, third, values[3], translation[3], i;
    LoadPoseRotation(left);
    first = ReadPoseWord(right, 0);
    second = ReadPoseWord(right, 4);
    xport_gte_write_data(0, first);
    xport_gte_write_data(1, second);
    xport_gte_execute(0x486012);
    StorePoseColumn(output, 0);
    third = ReadPoseWord(right, 8);
    xport_gte_write_data(0, (third << 16) | (second >> 16));
    xport_gte_write_data(1, third >> 16);
    xport_gte_execute(0x486012);
    StorePoseColumn(output, 1);
    first = ReadPoseWord(right, 12);
    second = ReadPoseWord(right, 16);
    xport_gte_write_data(0, first);
    xport_gte_write_data(1, second);
    xport_gte_execute(0x486012);
    StorePoseColumn(output, 2);
    LoadPoseRotation(output);
    for (i = 0; i < 3; ++i)
        translation[i] = (uint32)(sint32)(sint16)ReadPoseHalf(right, 18 + 2 * i);
    for (i = 0; i < 3; ++i)
        xport_gte_write_data(9 + i, translation[i]);
    xport_gte_execute(0x49E012);
    for (i = 0; i < 3; ++i)
        values[i] = (uint32)(sint32)(sint16)ReadPoseHalf(left, 18 + 2 * i);
    for (i = 0; i < 3; ++i)
        values[i] -= xport_gte_read_data(9 + i);
    for (i = 0; i < 3; ++i)
        StorePoseHalf(output, 18 + 2 * i, (uint16)values[i]);
}

uint32 BuildModelPose(uint32 object)
{
    FUNCTION_MARKER(0x8007D148, "SLUS_003.73");
    uint8 parents[144], matrix[24];
    sint32 top = -1, count, ordinal, total;
    uint32 result = r_u16(object) & 4;
    uint32 output = r_u32(object + 356), links = r_u32(object + 364);
    uint32 angles = r_u32(object + 360), table, models, poses, entry, source, joint, row, column;
    if (!result || !output)
        return result;
    if (!angles)
        return 0x800F0000;
    table = 0x800EAEF8 + 64 * r_u8(object + 27);
    models = r_u32(table + 16);
    total = (sint32)r_u32(models - 4);
    poses = r_u32(table + 24);
    for (ordinal = 0; ordinal < total; ++ordinal)
        w_u16(output + 24 * ordinal, 0x8000);
    entry = poses + 8 * r_u8(object + 26);
    count = r_u16(links - 2);
    if (r_u16(entry + 10))
        source = 0x800ED760;
    else
    {
        uint32 frame = r_u8(object + 24);
        uint32 offset = r_u32(entry + 4);
        source = poses + offset + 24 * frame * (uint32)total;
    }
    for (ordinal = 0; ordinal < count; ++ordinal, links += 12, angles += 12)
    {
        const uint8 *base;
        joint = r_u16(links);
        if (top >= 0)
        {
            uint32 parent = r_u16(links + 10);
            while (top >= 0 && parent < xport_load_le32(parents + 28 * top + 24))
                --top;
        }
        base = (const uint8 *)psx_addr(source + 24 * joint, 24);
        if (r_u16(angles) || r_u16(angles + 2) || r_u16(angles + 4))
        {
            BuildModelRotationMatrix((const uint32 *)psx_addr(angles, 8), (uint16 *)matrix);
            xport_store_le16(matrix + 18, 0);
            xport_store_le16(matrix + 20, 0);
            xport_store_le16(matrix + 22, 0);
            ComposePoseMatrix(matrix, base, matrix);
            if (top >= 0)
                ComposePoseMatrix(matrix, parents + 28 * top, matrix);
            ++top;
            if (28 * (uint32)top + 28 > sizeof(parents))
            {
                // TODO Resolve original hierarchy overflow before accepting that path
                GameDiagnostic("Pose hierarchy exceeds original local buffer\n");
                abort();
            }
            RelativePoseMatrix(parents + 28 * top, matrix, base);
            xport_store_le32(parents + 28 * top + 24, (uint32)ordinal);
            for (row = 0; row < 3; ++row)
            {
                for (column = 0; column < 3; ++column)
                    w_u16(output + 24 * joint + row * 6 + column * 2, xport_load_le16(matrix + row * 6 + column * 2));
                w_u16(output + 24 * joint + 18 + row * 2, xport_load_le16(matrix + 18 + row * 2));
            }
        }
        else if (top < 0)
        {
            for (row = 0; row < 3; ++row)
            {
                for (column = 0; column < 3; ++column)
                    w_u16(output + 24 * joint + row * 6 + column * 2, r_u16(source + 24 * joint + row * 6 + column * 2));
                w_u16(output + 24 * joint + 18 + row * 2, r_u16(source + 24 * joint + 18 + row * 2));
            }
        }
        else
            ComposePoseMatrix((uint8 *)psx_addr(output + 24 * joint, 24), parents + 28 * top, base);
    }
    result = r_u16(poses + 8 * r_u8(object + 26) + 10);
    if (!result)
        for (ordinal = 0; ordinal < total; ++ordinal)
        {
            uint32 destination = output + 24 * ordinal, from = source + 24 * ordinal, words[4];
            if ((sint16)r_u16(destination) == -32768)
            {
                for (column = 0; column < 4; ++column)
                    words[column] = r_u32(from + column * 4);
                for (column = 0; column < 4; ++column)
                    w_u32(destination + column * 4, words[column]);
                words[0] = r_u32(from + 16);
                words[1] = r_u32(from + 20);
                w_u32(destination + 16, words[0]);
                w_u32(destination + 20, words[1]);
            }
            result = ordinal + 1 < total;
        }
    return result;
}

uint32 UpdateModelWorldTransform(uint32 object)
{
    FUNCTION_MARKER(0x8007FDF0, "SLUS_003.73");
    uint32 x, z, y;
    BuildModelRotationMatrix((const uint32 *)psx_addr(object + 16, 8), (uint16 *)psx_addr(object + 324, 18));
    if (r_u16(object) & 0x200)
        ScaleModelRotationMatrix((const uint8 *)psx_addr(object, 42), (uint16 *)psx_addr(object + 324, 18));
    x = r_u32(object + 4);
    z = r_u32(object + 12);
    w_u32(object + 344, (uint32)((sint32)x >> 12));
    y = r_u32(object + 8);
    w_u32(object + 352, (uint32)((sint32)z >> 12));
    y = (uint32)((sint32)y >> 12);
    w_u32(object + 348, y);
    return y;
}

extern "C" uint32 sub_8007D148(uint32 object)
{
    FUNCTION_MARKER(0x8007D148, "SLUS_003.73");
    return BuildModelPose(object);
}

extern "C" uint32 sub_8007FDF0(uint32 object)
{
    FUNCTION_MARKER(0x8007FDF0, "SLUS_003.73");
    return UpdateModelWorldTransform(object);
}

extern "C" void sub_80085674(uint32 output, uint32 left, uint32 right)
{
    FUNCTION_MARKER(0x80085674, "SLUS_003.73");
    ComposePoseMatrix((uint8 *)psx_addr(output, 24), (const uint8 *)psx_addr(left, 24), (const uint8 *)psx_addr(right, 24));
}

extern "C" void sub_800857A8(uint32 output, uint32 left, uint32 right)
{
    FUNCTION_MARKER(0x800857A8, "SLUS_003.73");
    RelativePoseMatrix((uint8 *)psx_addr(output, 24), (const uint8 *)psx_addr(left, 24), (const uint8 *)psx_addr(right, 24));
}
