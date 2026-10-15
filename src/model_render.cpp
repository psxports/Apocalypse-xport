#include "model_render.h"
#include "model_lighting.h"
#include "model_projection.h"
#include "model_animation.h"
#include "model_shadow.h"
#include "angle_math.h"
#include "native_object_storage.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
extern "C"
{
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
}

static uint32 ProjectAndClipModelVertices(const uint32 *vertices, uint32 count);

static void RenderClippedModel(uint32 geometry, uint32 translated)
{
    uint32 vertices, count, attributes, primitives, primitive_count, x, y, z;
    vertices = geometry + 32;
    count = r_u32(geometry + 4);
    w_u32(0x800FFA9C, translated);
    if (sub_8008131C(vertices, count) & 191)
        return;
    attributes = vertices + count * 8;
    primitives = attributes + r_u32(geometry + 8) * 8;
    primitive_count = r_u32(geometry + 12);
    sub_8008793C(0x800EE710);
    x = (uint32)(sint32)(sint16)r_u16(0x800FFA8C);
    y = (uint32)(sint32)(sint16)r_u16(0x800FFA8E);
    z = (uint32)(sint32)(sint16)r_u16(0x800FFA90);
    sub_800879AC(x, y, z);
    x = (uint32)(sint32)(sint16)r_u16(0x800FFA94);
    y = (uint32)(sint32)(sint16)r_u16(0x800FFA96);
    z = (uint32)(sint32)(sint16)r_u16(0x800FFA98);
    sub_8008798C(x, y, z);
    BuildModelPolygonPackets(primitives, primitive_count);
}

void RenderTranslatedClippedModel(uint32 geometry)
{
    FUNCTION_MARKER(0x8007FA7C, "SLUS_003.73");
    RenderClippedModel(geometry, 1);
}

void RenderScratchModel(uint32 geometry)
{
    uint32 vertices, count, attributes, result, primitives, primitive_count;
    FUNCTION_MARKER(0x8007FBD4, "SLUS_003.73");
    vertices = geometry + 32;
    count = r_u32(geometry + 4);
    w_u32(0x800FFA9C, 1);
    result = sub_800811BC(vertices, count);
    attributes = vertices + count * 8;
    if (r_u32(0x800FFB38))
        return;
    if (result & 0x60006000)
        return;
    primitives = attributes + r_u32(geometry + 8) * 8;
    primitive_count = r_u32(geometry + 12);
    sub_80083F38(primitives, attributes, primitive_count);
}

void RenderUntranslatedClippedModel(uint32 geometry)
{
    FUNCTION_MARKER(0x8007FD3C, "SLUS_003.73");
    RenderClippedModel(geometry, 0);
}

// TODO Classify original render V0 consumers beyond the current void render callers
extern "C" void sub_8007FA7C(uint32 geometry)
{
    FUNCTION_MARKER(0x8007FA7C, "SLUS_003.73");
    RenderTranslatedClippedModel(geometry);
}
extern "C" void sub_8007FBD4(uint32 geometry)
{
    FUNCTION_MARKER(0x8007FBD4, "SLUS_003.73");
    RenderScratchModel(geometry);
}
extern "C" void sub_8007FD3C(uint32 geometry)
{
    FUNCTION_MARKER(0x8007FD3C, "SLUS_003.73");
    RenderUntranslatedClippedModel(geometry);
}

extern "C" void nullsub_18(void)
{
    FUNCTION_MARKER(0x8006FAE4, "SLUS_003.73");
}

void RenderProjectedModel(uint32 model)
{
    uint32 vertices, count, flags, references, packets, packet_count, far_x, far_y, far_z;
    FUNCTION_MARKER(0x8007F904, "SLUS_003.73");
    vertices = model + 32;
    count = r_u32(model + 4);
    w_u32(0x800FFA9C, 1);
    flags = ProjectAndClipModelVertices(count ? (const uint32 *)psx_addr(vertices, 8) : 0, count);
    if (r_u32(0x800FFB3C))
        UpdateProjectedVertexDepthFlags(count);
    references = vertices + count * 8;
    if (flags & 191)
        return;
    packets = references + r_u32(model + 8) * 8;
    packet_count = r_u32(model + 12);
    SetLightMatrix((MATRIX *)psx_addr(0x800F3E10, 20));
    sub_8008793C(0x800EE710);
    far_x = (uint32)(sint32)(sint16)r_u16(0x800FFA8C);
    far_y = (uint32)(sint32)(sint16)r_u16(0x800FFA8E);
    far_z = (uint32)(sint32)(sint16)r_u16(0x800FFA90);
    sub_800879AC(far_x, far_y, far_z);
    far_x = (uint32)(sint32)(sint16)r_u16(0x800FFA94);
    far_y = (uint32)(sint32)(sint16)r_u16(0x800FFA96);
    far_z = (uint32)(sint32)(sint16)r_u16(0x800FFA98);
    sub_8008798C(far_x, far_y, far_z);
    BuildModelPolygonPackets(packets, packet_count);
}

extern "C" void sub_8007F904(uint32 model)
{
    FUNCTION_MARKER(0x8007F904, "SLUS_003.73");
    RenderProjectedModel(model);
}

void RenderOffsetScratchModel(uint32 model, const sint32 position[3])
{
    uint32 vertices, count, attributes, packets, packet_count, flags, red, green, blue;
    FUNCTION_MARKER(0x8007F9E0, "SLUS_003.73");
    vertices = model + 32;
    count = r_u32(model + 4);
    w_u32(0x800FFA9C, 1);
    flags = ProjectOffsetScratchVertices(vertices, count, position);
    attributes = vertices + count * 8;
    if (flags & 0x60006000)
        return;
    packet_count = r_u32(model + 12);
    red = (uint32)(sint32)(sint16)r_u16(0x800FFA8C);
    green = (uint32)(sint32)(sint16)r_u16(0x800FFA8E);
    packets = attributes + r_u32(model + 8) * 8;
    blue = (uint32)(sint32)(sint16)r_u16(0x800FFA90);
    sub_800879AC(red, green, blue);
    sub_80083F38(packets, attributes, packet_count);
}

// TODO Classify V0 consumers beyond the current callers that discard the render result
extern "C" void sub_8007F9E0(uint32 model, uint32 position)
{
    FUNCTION_MARKER(0x8007F9E0, "SLUS_003.73");
    RenderOffsetScratchModel(model, (const sint32 *)psx_addr(position, 12));
}
extern "C" void xport_draft_host_sub_8007F9E0_p2(uint32 model, const void *position)
{
    FUNCTION_MARKER(0x8007F9E0, "SLUS_003.73");
    RenderOffsetScratchModel(model, (const sint32 *)position);
}

static uint32 ReadModelVertexWord(const uint32 *vertices, uint32 address, uint32 offset)
{
    if (address)
        return r_u32(address + offset);
    uint32 value;
    memcpy(&value, (const uint8 *)vertices + offset, 4);
    return value;
}

static uint32 ProjectAndClipModelVertices(const uint32 *vertices, uint32 count)
{
    uint32 bounds_source, packed, bounds[6], output, transformed, aggregate = 0xFFFF;
    uint32 vertex_xy, vertex_z, next_xy, next_z, screen, clip;
    sint32 x, y, z;
    FUNCTION_MARKER(0x80080D84, "SLUS_003.73");
    if (!count)
        return 0xFF;
    uint32 input = GuestMemoryAddress(vertices), offset = 0;
    bounds_source = r_u32(0x800FFB08);
    packed = r_u32(bounds_source);
    bounds[0] = packed & 0xFFFF;
    bounds[2] = packed >> 16;
    packed = r_u32(bounds_source + 4);
    bounds[1] = packed & 0xFFFF;
    bounds[3] = packed >> 16;
    packed = r_u32(bounds_source + 8);
    bounds[4] = packed & 0xFFFF;
    bounds[5] = packed >> 16;
    output = r_u32(0x800FFAC0);
    transformed = r_u32(0x800FFAC4);
    vertex_xy = ReadModelVertexWord(vertices, input, offset);
    vertex_z = ReadModelVertexWord(vertices, input, offset + 4);
    do
    {
        xport_gte_write_data(0, vertex_xy);
        xport_gte_write_data(1, vertex_z);
        offset += 8;
        --count;
        xport_gte_execute(0x180001);
        // Original prefetch reads the following vertex even on the final iteration
        next_xy = ReadModelVertexWord(vertices, input, offset);
        next_z = ReadModelVertexWord(vertices, input, offset + 4);
        x = (sint32)xport_gte_read_data(9);
        y = (sint32)xport_gte_read_data(10);
        z = (sint32)xport_gte_read_data(11);
        w_u32(transformed, ((uint32)x & 0xFFFF) | ((uint32)y << 16));
        screen = xport_gte_read_data(14);
        w_u16(output + 4, (uint32)z);
        w_u32(output, screen);
        x = (sint16)screen;
        y = (sint32)screen >> 16;
        clip = ((sint32)bounds[0] < x) | ((uint32)(x < (sint32)bounds[1]) << 1) | ((uint32)((sint32)bounds[2] < y) << 2) | ((uint32)(y < (sint32)bounds[3]) << 3) | ((uint32)(z < (sint32)bounds[4]) << 4) | ((uint32)((sint32)bounds[5] < z) << 5);
        if (z < 0)
            clip ^= 15;
        clip |= (clip << 8) ^ 0xFF00;
        w_u16(output + 6, clip);
        aggregate &= clip;
        output += 8;
        transformed += 8;
        vertex_xy = next_xy;
        vertex_z = next_z;
    } while (count);
    w_u32(0x1F8001D4, output);
    return aggregate;
}

extern "C" uint32 sub_80080D84(uint32 vertices, uint32 count)
{
    FUNCTION_MARKER(0x80080D84, "SLUS_003.73");
    return ProjectAndClipModelVertices(count ? (const uint32 *)psx_addr(vertices, 8) : 0, count);
}

static void WriteModelMatrixWord(uint32 *matrix, uint32 address, uint32 offset, uint32 value)
{
    if (address)
        w_u32(address + offset, value);
    else
        memcpy((uint8 *)matrix + offset, &value, 4);
}

static void LoadModelRotationAndTransformIr(const uint32 *matrix)
{
    uint32 words[5], axis, address = GuestMemoryAddress(matrix);
    FUNCTION_MARKER(0x80085C94, "SLUS_003.73");
    for (axis = 0; axis < 5; ++axis)
        words[axis] = ReadModelVertexWord(matrix, address, axis * 4);
    // Original reads IR1 before loading the control bank
    (void)xport_gte_read_data(9);
    for (axis = 0; axis < 5; ++axis)
        xport_gte_write_control(axis, words[axis]);
    xport_gte_execute(0x49E012);
}

static void TransformPackedModelMatrix(const uint32 *input, uint32 *output)
{
    FUNCTION_MARKER(0x800854F4, "SLUS_003.73");
    uint32 source = GuestMemoryAddress(input), destination = GuestMemoryAddress(output);
    uint32 first0 = ReadModelVertexWord(input, source, 0), first1 = ReadModelVertexWord(input, source, 4), first3 = ReadModelVertexWord(input, source, 12);
    uint32 first2, first4, column0[3], column1[3], column2[3];
    xport_gte_write_data(9, first0);
    xport_gte_write_data(10, first1 >> 16);
    xport_gte_write_data(11, first3);
    xport_gte_execute(0x49E012);
    first2 = ReadModelVertexWord(input, source, 8);
    column0[0] = xport_gte_read_data(25);
    column0[1] = xport_gte_read_data(26);
    column0[2] = xport_gte_read_data(27);
    xport_gte_write_data(9, first0 >> 16);
    xport_gte_write_data(10, first2);
    xport_gte_write_data(11, first3 >> 16);
    xport_gte_execute(0x49E012);
    first4 = ReadModelVertexWord(input, source, 16);
    column1[0] = xport_gte_read_data(25);
    column1[1] = xport_gte_read_data(26);
    column1[2] = xport_gte_read_data(27);
    xport_gte_write_data(9, first1);
    xport_gte_write_data(10, first2 >> 16);
    xport_gte_write_data(11, first4);
    xport_gte_execute(0x49E012);
    WriteModelMatrixWord(output, destination, 0, (column0[0] & 0xFFFF) | (column1[0] << 16));
    WriteModelMatrixWord(output, destination, 12, (column0[2] & 0xFFFF) | (column1[2] << 16));
    column2[0] = xport_gte_read_data(25);
    column2[1] = xport_gte_read_data(26);
    column2[2] = xport_gte_read_data(27);
    WriteModelMatrixWord(output, destination, 4, (column2[0] & 0xFFFF) | (column0[1] << 16));
    WriteModelMatrixWord(output, destination, 8, (column1[1] & 0xFFFF) | (column2[1] << 16));
    WriteModelMatrixWord(output, destination, 16, column2[2]);
}

extern "C" void sub_80085C94(uint32 matrix)
{
    FUNCTION_MARKER(0x80085C94, "SLUS_003.73");
    LoadModelRotationAndTransformIr((const uint32 *)psx_addr(matrix, 20));
}
extern "C" void sub_800854F4(uint32 input, uint32 output)
{
    FUNCTION_MARKER(0x800854F4, "SLUS_003.73");
    TransformPackedModelMatrix((const uint32 *)psx_addr(input, 20), (uint32 *)psx_addr(output, 20));
}
extern "C" uint32 xport_draft_host_sub_800854F4_p12(const void *input, void *output)
{
    FUNCTION_MARKER(0x800854F4, "SLUS_003.73");
    TransformPackedModelMatrix((const uint32 *)input, (uint32 *)output);
    return 0;
}

static uint32 ReadModelMatrixHalf(const uint32 *matrix, uint32 address, uint32 offset)
{
    if (address)
        return r_u16(address + offset);
    uint16 value;
    memcpy(&value, (const uint8 *)matrix + offset, 2);
    return value;
}

static void SetupModelGteMatrices(const uint32 *rotation, const uint32 *lighting, const uint32 *local)
{
    uint32 rotation_address = GuestMemoryAddress(rotation), lighting_address = GuestMemoryAddress(lighting), local_address = GuestMemoryAddress(local);
    uint32 words[5], input[5], composed[5], output[5], translation[3], index;
    uint32 first_x, first_y, first_z, second_y, second_z, third_x, third_y, third_z;
    FUNCTION_MARKER(0x80084578, "SLUS_003.73");
    for (index = 0; index < 5; ++index)
        words[index] = ReadModelVertexWord(rotation, rotation_address, index * 4);
    for (index = 0; index < 5; ++index)
        xport_gte_write_control(index, words[index]);
    for (index = 0; index < 3; ++index)
        translation[index] = ReadModelVertexWord(rotation, rotation_address, 20 + index * 4);
    for (index = 0; index < 3; ++index)
        xport_gte_write_control(5 + index, translation[index]);
    for (index = 0; index < 5; ++index)
        words[index] = ReadModelVertexWord(lighting, lighting_address, index * 4);
    for (index = 0; index < 5; ++index)
        xport_gte_write_control(8 + index, words[index]);
    for (index = 0; index < 5; ++index)
        input[index] = ReadModelVertexWord(local, local_address, index * 4);
    xport_gte_write_data(0, (input[0] & 0xFFFF) | (input[1] & 0xFFFF0000));
    xport_gte_write_data(1, input[3]);
    xport_gte_write_data(2, (input[0] >> 16) | (input[2] << 16));
    xport_gte_write_data(3, input[3] >> 16);
    xport_gte_write_data(4, (input[1] & 0xFFFF) | (input[2] & 0xFFFF0000));
    xport_gte_write_data(5, input[4]);
    xport_gte_execute(0x4A6012);
    first_x = xport_gte_read_data(9);
    first_y = xport_gte_read_data(10);
    first_z = xport_gte_read_data(11);
    xport_gte_execute(0x4AE012);
    composed[0] = (first_x & 0xFFFF) | (xport_gte_read_data(9) << 16);
    second_y = xport_gte_read_data(10);
    second_z = xport_gte_read_data(11);
    composed[3] = (first_z & 0xFFFF) | (second_z << 16);
    xport_gte_execute(0x4B6012);
    third_x = xport_gte_read_data(9);
    composed[1] = (first_y << 16) | (third_x & 0xFFFF);
    third_y = xport_gte_read_data(10);
    composed[2] = (second_y & 0xFFFF) | (third_y << 16);
    composed[4] = xport_gte_read_data(11);
    for (index = 0; index < 5; ++index)
        xport_gte_write_control(8 + index, composed[index]);
    xport_gte_execute(0x486012);
    first_x = xport_gte_read_data(9);
    first_y = xport_gte_read_data(10);
    first_z = xport_gte_read_data(11);
    xport_gte_execute(0x48E012);
    third_x = xport_gte_read_data(9);
    second_y = xport_gte_read_data(10);
    second_z = xport_gte_read_data(11);
    xport_gte_execute(0x496012);
    output[0] = (first_x & 0xFFFF) | (third_x << 16);
    output[3] = (first_z & 0xFFFF) | (second_z << 16);
    third_x = xport_gte_read_data(9);
    third_y = xport_gte_read_data(10);
    third_z = xport_gte_read_data(11);
    output[1] = (first_y << 16) | (third_x & 0xFFFF);
    output[2] = (second_y & 0xFFFF) | (third_y << 16);
    output[4] = third_z;
    translation[0] = ReadModelMatrixHalf(local, local_address, 18);
    translation[1] = (uint32)(sint32)(sint16)ReadModelMatrixHalf(local, local_address, 20);
    translation[2] = (uint32)(sint32)(sint16)ReadModelMatrixHalf(local, local_address, 22);
    xport_gte_write_data(0, translation[0] | (translation[1] << 16));
    xport_gte_write_data(1, translation[2]);
    xport_gte_execute(0x480012);
    for (index = 0; index < 3; ++index)
        translation[index] = xport_gte_read_data(9 + index);
    for (index = 0; index < 5; ++index)
        xport_gte_write_control(index, output[index]);
    for (index = 0; index < 3; ++index)
        xport_gte_write_control(5 + index, translation[index]);
}

extern "C" void sub_80084578(uint32 rotation, uint32 lighting, uint32 local)
{
    FUNCTION_MARKER(0x80084578, "SLUS_003.73");
    SetupModelGteMatrices((const uint32 *)psx_addr(rotation, 32), (const uint32 *)psx_addr(lighting, 20), (const uint32 *)psx_addr(local, 24));
}

extern "C" void apocalypse_model_gte_setup_native(const void *rotation, const void *lighting, uint32 local)
{
    SetupModelGteMatrices((const uint32 *)rotation, (const uint32 *)lighting, (const uint32 *)psx_addr(local, 24));
}

void InitializeModelIdentityMatrix(uint32 *matrix)
{
    FUNCTION_MARKER(0x800854D8, "SLUS_003.73");
    uint32 address = GuestMemoryAddress(matrix);
    WriteModelMatrixWord(matrix, address, 0, 4096);
    WriteModelMatrixWord(matrix, address, 4, 0);
    WriteModelMatrixWord(matrix, address, 8, 4096);
    WriteModelMatrixWord(matrix, address, 12, 0);
    WriteModelMatrixWord(matrix, address, 16, 4096);
}

void RenderModelObjectList(uint32 object)
{
    FUNCTION_MARKER(0x8007F138, "SLUS_003.73");
    uint32 entry, flags, color, render_flags, geometry, matrix, index, table, packed_angles;
    MATRIX rotation, combined;
    if (!object)
        return;
    w_u32(0x1F800164, 0x7FFFFFFF);
    do
    {
        flags = r_u16(object);
        if (!(flags & 0x8001) && r_u8(0x800EAEF8 + 64 * r_u8(object + 27) + 10))
        {
            render_flags = r_u32(0x800FFAA0);
            if (flags & 0x400)
            {
                render_flags = (render_flags & 0xFFFB0000) | 8;
                color = r_u32(object + 32);
                w_u32(0x800FFB20, color & 0xFFFFFF);
                w_u32(0x1F800154, color & 0xFFFFFF);
                w_u32(0x1F8001A4, (color & 255) << 6);
                w_u32(0x1F8001B4, (color & 0xFF00) >> 2);
                w_u32(0x1F8001C4, ((color & 0xFFFFFF) >> 10) & 0x3FC0);
            }
            if (r_u16(object) & 0x800)
                render_flags = (render_flags & 0xFFFFFE7F) | (r_u32(object + 44) & 0x180) | 0x40;
            matrix = r_u32(0x800FFB0C);
            w_u32(0x1F800144, render_flags);
            SetRotMatrix((MATRIX *)psx_addr(matrix + 116, 20));
            if (r_u32(object + 16) || r_u16(object + 20))
            {
                BuildModelRotationMatrix((const uint32 *)psx_addr(object + 16, 8), (uint16 *)&rotation);
                TransformPackedModelMatrix((const uint32 *)&rotation, (uint32 *)&combined);
                SetRotMatrix(&combined);
            }
            else
                InitializeModelIdentityMatrix((uint32 *)&rotation);
            entry = 0x800EAEF8 + 64 * r_u8(object + 27);
            w_u32(0x800FFB04, r_u32(entry + 32));
            entry = 0x800EAEF8 + 64 * r_u8(object + 27);
            index = r_u16(object + 22);
            table = r_u32(entry + 16);
            packed_angles = r_u32(object + 16);
            geometry = r_u32(table + 4 * index);
            if (packed_angles || r_u16(object + 20))
                RenderTranslatedClippedModel(geometry);
            else
                RenderUntranslatedClippedModel(geometry);
        }
        object = r_u32(object + 28);
    } while (object);
    geometry = r_u32(0x800FFACC);
    w_u32(0x1F800164, geometry);
}

extern "C" void sub_8007F138(uint32 object)
{
    FUNCTION_MARKER(0x8007F138, "SLUS_003.73");
    RenderModelObjectList(object);
}
extern "C" uint32 xport_draft_host_sub_800854D8_p1(void *output)
{
    FUNCTION_MARKER(0x800854D8, "SLUS_003.73");
    InitializeModelIdentityMatrix((uint32 *)output);
    return 0;
}

void RenderTranslatedModel(uint32 geometry)
{
    FUNCTION_MARKER(0x8007FB34, "SLUS_003.73");
    uint32 count = r_u32(geometry + 4);
    uint32 vertices = geometry + 32;
    w_u32(0x800FFA9C, 1);
    uint32 flags = ProjectModelReferenceVertices(vertices, count);
    if (r_u32(0x800FFB3C))
        UpdateProjectedVertexDepthFlags(count);
    uint32 attributes = vertices + (count << 3);
    if (!r_u32(0x800FFB38) && !(flags & 191))
    {
        uint32 attribute_count = r_u32(geometry + 8);
        uint32 polygon_count = r_u32(geometry + 12);
        BuildModelPolygonPackets(attributes + (attribute_count << 3), polygon_count);
    }
}

void TransposeModelMatrix(const uint32 *input, uint32 *output)
{
    FUNCTION_MARKER(0x80085C34, "SLUS_003.73");
    uint32 source = GuestMemoryAddress(input), destination = GuestMemoryAddress(output);
    uint32 first = ReadModelVertexWord(input, source, 0);
    uint32 second = ReadModelVertexWord(input, source, 4);
    uint32 third = ReadModelVertexWord(input, source, 8);
    uint32 fourth = ReadModelVertexWord(input, source, 12);
    uint32 tail = ReadModelMatrixHalf(input, source, 16);
    WriteModelMatrixWord(output, destination, 0, ((first ^ second) & 65535) ^ second);
    WriteModelMatrixWord(output, destination, 4, ((fourth ^ first) & 65535) ^ first);
    WriteModelMatrixWord(output, destination, 8, ((third ^ fourth) & 65535) ^ fourth);
    WriteModelMatrixWord(output, destination, 12, ((second ^ third) & 65535) ^ third);
    if (destination)
        w_u16(destination + 16, tail);
    else
    {
        uint16 last = (uint16)tail;
        memcpy((uint8 *)output + 16, &last, 2);
    }
}

extern "C" void sub_8007FB34(uint32 geometry)
{
    FUNCTION_MARKER(0x8007FB34, "SLUS_003.73");
    RenderTranslatedModel(geometry);
}
extern "C" void sub_80085C34(uint32 input, uint32 output)
{
    FUNCTION_MARKER(0x80085C34, "SLUS_003.73");
    TransposeModelMatrix((const uint32 *)psx_addr(input, 18), (uint32 *)psx_addr(output, 18));
}




/* Native helper for the post-projection portion of the original owner */
static void RenderOffsetModelPolygons(uint32 model, uint32 count, uint32 flags)
{
    uint32 references, packets, packet_count, bank[5], index;
    if (r_u32(0x800FFB3C))
        UpdateProjectedVertexDepthFlags(count);
    references = model + 32 + (count << 3);
    flags &= 0xBF;
    if (flags)
        return;
    packets = references + (r_u32(model + 8) << 3);
    packet_count = r_u32(model + 12);
    for (index = 0; index < 5; ++index)
        bank[index] = r_u32(0x800EE6F0 + index * 4);
    for (index = 0; index < 5; ++index)
        xport_gte_write_control(8 + index, bank[index]);
    for (index = 0; index < 5; ++index)
        bank[index] = r_u32(0x800EE710 + index * 4);
    for (index = 0; index < 5; ++index)
        xport_gte_write_control(16 + index, bank[index]);
    for (index = 0; index < 3; ++index)
        bank[index] = (uint32)(sint32)(sint16)r_u16(0x800FFA8C + index * 2) << 4;
    for (index = 0; index < 3; ++index)
        xport_gte_write_control(21 + index, bank[index]);
    for (index = 0; index < 3; ++index)
        bank[index] = (uint32)(sint32)(sint16)r_u16(0x800FFA94 + index * 2) << 4;
    for (index = 0; index < 3; ++index)
        xport_gte_write_control(13 + index, bank[index]);
    {
        (void)(BuildModelPolygonPackets(packets, packet_count));
        return;
    }
}

void RenderOffsetModel(uint32 model, const sint32 vector[3])
{
    uint32 count = r_u32(model + 4), flags;
    FUNCTION_MARKER(0x8007FC60, "SLUS_003.73");
    w_u32(0x800FFA9C, 0);
    flags = ProjectOffsetModelVertices(model + 32, count, vector);
    RenderOffsetModelPolygons(model, count, flags);
}

extern "C" void sub_8007FC60(uint32 model, uint32 guest_vector)
{
    uint32 count = r_u32(model + 4), flags;
    FUNCTION_MARKER(0x8007FC60, "SLUS_003.73");
    w_u32(0x800FFA9C, 0);
    flags = ProjectOffsetModelVertices(model + 32, count, count ? (const sint32 *)psx_addr(guest_vector, 12) : 0);
    RenderOffsetModelPolygons(model, count, flags);
}

extern "C" void apocalypse_render_geometry(uint32 model, const sint32 vector[3]) { RenderOffsetModel(model, vector); }


static uint32 PackModelPolygonCommand(uint32 packet, uint32 color, uint32 command, uint32 flags)
{
    FUNCTION_MARKER(0x80082188, "SLUS_003.73");
    w_u32(packet + 4, color | command | ((flags & 0x40) << 19));
    return packet;
}

static uint32 ReadModelTableColor(uint32 index, uint32 table)
{
    FUNCTION_MARKER(0x800821A4, "SLUS_003.73");
    return r_u32(table + (index & 0x3FC));
}

static void LoadModelTableColor(uint32 index, uint32 table, uint32 ir1, uint32 ir2, uint32 ir3)
{
    uint32 color;
    FUNCTION_MARKER(0x80082200, "SLUS_003.73");
    color = r_u32(table + (index & 0x3FC));
    xport_gte_write_data(9, ir1);
    xport_gte_write_data(10, ir2);
    xport_gte_write_data(11, ir3);
    xport_gte_write_data(6, color);
}

extern "C" uint32 sub_80082188(uint32 packet, uint32 command, uint32 texture, uint32 flags)
{
    FUNCTION_MARKER(0x80082188, "SLUS_003.73");
    return PackModelPolygonCommand(packet, command, texture, flags);
}
extern "C" uint32 sub_800821A4(uint32 lookup_index, uint32 lookup_base)
{
    FUNCTION_MARKER(0x800821A4, "SLUS_003.73");
    return ReadModelTableColor(lookup_index, lookup_base);
}
extern "C" void sub_80082200(uint32 lookup_index, uint32 table, uint32 ir1, uint32 ir2, uint32 ir3)
{
    FUNCTION_MARKER(0x80082200, "SLUS_003.73");
    LoadModelTableColor(lookup_index, table, ir1, ir2, ir3);
}


static uint32 ReadModelUvWave(uint32 parameter)
{
    uint32 bank = parameter >> 4;
    uint32 phase, address;
    if (!bank)
        return 0;
    phase = (r_u32(0x800FFB48) + ((parameter & 15) << 2)) & 63;
    address = 0x800F5130 + (bank << 7) + (phase << 1);
    return (uint32)(sint32)(sint16)r_u16(address);
}

uint32 ReadHorizontalModelUvOffset(uint32 coordinates)
{
    FUNCTION_MARKER(0x8007FED0, "SLUS_003.73");
    return ReadModelUvWave(r_u8(coordinates + 2));
}

uint32 ReadVerticalModelUvOffset(uint32 coordinates)
{
    FUNCTION_MARKER(0x8007FF20, "SLUS_003.73");
    return ReadModelUvWave(r_u8(coordinates + 3));
}

extern "C" uint32 sub_8007FED0(uint32 coordinates)
{
    FUNCTION_MARKER(0x8007FED0, "SLUS_003.73");
    return ReadHorizontalModelUvOffset(coordinates);
}
extern "C" uint32 sub_8007FF20(uint32 coordinates)
{
    FUNCTION_MARKER(0x8007FF20, "SLUS_003.73");
    return ReadVerticalModelUvOffset(coordinates);
}


static void WrapModelTextureCoordinates(uint32 values[4], uint32 adjustment)
{
    while (((values[0] & values[1] & values[2] & values[3]) ^ (values[0] | values[1] | values[2] | values[3])) & 0x10000)
    {
        uint32 vertex;
        for (vertex = 0; vertex < 4; ++vertex)
            values[vertex] += adjustment;
    }
}

uint32 UpdateModelTextureCoordinates(uint32 bank_index)
{
    uint32 model, records, object_base, result;
    FUNCTION_MARKER(0x8007FF70, "SLUS_003.73");
    if (bank_index == 0xFFFFFFFF)
        return 0xFFFFFFFF;
    result = bank_index << 6;
    model = 0x800EAEF8 + result;
    records = r_u32(model + 36);
    if (!records)
        return result;
    object_base = r_u32(model + 20);
    result = r_u32(records);
    while (result)
    {
        uint32 object = object_base + r_u32(records);
        uint32 coordinates = records + 16;
        uint32 object_flags = r_u16(object);
        uint32 count = r_u32(records + 12);
        if (object_flags & 0x8000)
            records = coordinates + (count << 4);
        else
        {
            uint32 speed_x = (uint32)(sint32)(sint16)r_u16(records + 4);
            uint32 tick = r_u32(0x800FFB40);
            uint32 scroll_x = (uint32)((sint32)(tick * speed_x) >> 4);
            uint32 phase_product = tick * r_u32(records + 8);
            uint32 bank = r_u8(object + 27);
            uint32 index = r_u16(object + 22);
            uint32 geometry_table = r_u32(0x800EAEF8 + (bank << 6) + 16);
            uint32 geometry = r_u32(geometry_table + (index << 2));
            uint32 y_product = tick * (uint32)(sint32)(sint16)r_u16(records + 6);
            uint32 packet = geometry + (r_u32(geometry + 4) << 3) + 32;
            uint32 item = 0;
            uint32 scroll_y;
            packet += r_u32(geometry + 8) << 3;
            w_u32(0x800FFB48, (uint32)((sint32)phase_product >> 10));
            scroll_y = (uint32)((sint32)y_product >> 4);
            while ((sint32)item < (sint32)count)
            {
                uint32 x[4], y[4], vertex, texture, wrap_x, wrap_y, stride;
                x[1] = (uint32)r_u8(coordinates + 4) << 8;
                x[0] = (uint32)r_u8(coordinates) << 8;
                x[2] = (uint32)r_u8(coordinates + 8) << 8;
                y[0] = (uint32)r_u8(coordinates + 1) << 8;
                y[1] = (uint32)r_u8(coordinates + 5) << 8;
                x[3] = (uint32)r_u8(coordinates + 12) << 8;
                y[2] = (uint32)r_u8(coordinates + 9) << 8;
                y[3] = (uint32)r_u8(coordinates + 13) << 8;
                texture = r_u32(packet + 32);
                wrap_x = (0 - ((texture & 0x1F) << 11)) & 0xF800;
                wrap_y = (0 - ((texture & 0x3E0) << 6)) & 0xFF00;
                for (vertex = 0; vertex < 4; ++vertex)
                    x[vertex] += scroll_x;
                for (vertex = 0; vertex < 4; ++vertex)
                    y[vertex] += scroll_y;
                for (vertex = 0; vertex < 4; ++vertex)
                {
                    x[vertex] += ReadHorizontalModelUvOffset(coordinates + vertex * 4);
                    y[vertex] += ReadVerticalModelUvOffset(coordinates + vertex * 4);
                }
                WrapModelTextureCoordinates(x, wrap_x);
                WrapModelTextureCoordinates(y, wrap_y);
                ++item;
                coordinates += 16;
                w_u16(packet + 20, ((x[0] & 0xFF00) >> 8) | (y[0] & 0xFF00));
                w_u16(packet + 24, ((x[1] & 0xFF00) >> 8) | (y[1] & 0xFF00));
                stride = r_u16(packet + 2);
                w_u32(packet + 28, ((x[2] & 0xFF00) >> 8) | (y[2] & 0xFF00) | ((x[3] & 0xFF00) << 8) | ((y[3] & 0xFF00) << 16));
                packet += stride & 0xFFFC;
            }
            records = coordinates;
        }
        result = r_u32(records);
    }
    return result;
}

extern "C" uint32 sub_8007FF70(uint32 bank_index)
{
    FUNCTION_MARKER(0x8007FF70, "SLUS_003.73");
    return UpdateModelTextureCoordinates(bank_index);
}


uint32 SetModelDrawArea(uint32 bounds, uint32 ordering_table)
{
    sint16 rectangle[4];
    uint32 frame, top, offset, right, left, bottom, packet, next_packet, tag, result;
    FUNCTION_MARKER(0x8007E57C, "SLUS_003.73");
    rectangle[0] = (sint16)r_u16(bounds + 4);
    frame = r_u32(0x800FF660);
    top = r_u16(bounds + 6);
    offset = r_u16(frame + 2);
    rectangle[1] = (sint16)(top + offset);
    right = r_u16(bounds);
    left = r_u16(bounds + 4);
    packet = r_u32(0x800FF668);
    rectangle[2] = (sint16)(right - left);
    bottom = r_u16(bounds + 2);
    top = r_u16(bounds + 6);
    rectangle[3] = (sint16)(bottom - top);
    xport_draft_host_sub_800890BC_p2(packet, rectangle);
    tag = r_u32(packet);
    result = r_u32(ordering_table + 0x3FFC);
    w_u32(packet, (tag & 0xFF000000) | (result & 0xFFFFFF));
    next_packet = packet + 12;
    result = r_u32(ordering_table + 0x3FFC);
    result = (result & 0xFF000000) | (packet & 0xFFFFFF);
    w_u32(ordering_table + 0x3FFC, result);
    w_u32(0x800FF668, next_packet);
    return result;
}

extern "C" uint32 sub_8007E57C(uint32 bounds, uint32 ordering_table)
{
    FUNCTION_MARKER(0x8007E57C, "SLUS_003.73");
    return SetModelDrawArea(bounds, ordering_table);
}


static uint32 DivideRenderViewScalar(uint32 numerator, uint32 denominator)
{
    sint32 left = (sint32)numerator, right = (sint32)denominator;
    if (!right)
        return left < 0 ? 1 : 0xFFFFFFFF;
    if (numerator == 0x80000000 && denominator == 0xFFFFFFFF)
        return numerator;
    return (uint32)(left / right);
}

static void RotateRenderViewNormal(uint32 matrix, uint32 input, uint32 output)
{
    uint32 words[5], axis, result[3];
    for (axis = 0; axis < 5; ++axis)
        words[axis] = r_u32(matrix + 4 * axis);
    for (axis = 0; axis < 5; ++axis)
        xport_gte_write_control(axis, words[axis]);
    xport_gte_write_data(0, r_u32(input));
    xport_gte_write_data(1, r_u32(input + 4));
    xport_gte_execute(0x486012);
    for (axis = 0; axis < 3; ++axis)
        result[axis] = xport_gte_read_data(9 + axis);
    for (axis = 0; axis < 3; ++axis)
        w_u16(output + 2 * axis, result[axis]);
}

static void TransposeRenderViewMatrix(uint32 input, uint32 output)
{
    uint32 first = r_u32(input), second = r_u32(input + 4), third, fourth;
    w_u32(output + 4, first);
    w_u32(output, second);
    w_u16(output, first);
    third = r_u32(input + 8);
    fourth = r_u32(input + 12);
    w_u32(output + 12, third);
    w_u32(output + 8, fourth);
    w_u16(output + 12, second);
    w_u16(output + 8, third);
    second = (uint32)(sint32)(sint16)r_u16(input + 16);
    w_u16(output + 4, fourth);
    w_u16(output + 16, second);
}

uint32 ConfigureModelRenderView(uint32 camera, uint32 bounds, uint32 ordering_table)
{
    uint32 depth, difference, length, axis, resource, owner, result;
    uint32 first, second, animation_bank, next_tick;
    uint32 first_normals = 0x800F25D0, second_normals = 0x800F25E8;
    FUNCTION_MARKER(0x8007E63C, "SLUS_003.73");
    w_u16(0x800F25D0, 0);
    w_u16(0x800F25D2, 0);
    w_u16(0x800F25D4, 0xF000);
    result = r_u16(bounds + 10);
    w_u16(0x800F25D8, 0);
    w_u16(0x800F25DA, 0);
    w_u16(0x800F25DC, 0x1000);
    w_u16(0x800F25D6, result);
    w_u16(0x800F25DE, 0 - r_u16(bounds + 8));

    first = r_u16(bounds + 14);
    second = r_u16(bounds + 6);
    difference = first - second;
    depth = r_u16(bounds + 16);
    length = sub_80085B54(difference * difference + depth * depth);
    w_u16(0x800F25E0, 0);
    w_u16(0x800F25E2, DivideRenderViewScalar(r_u16(bounds + 16) << 12, length));
    first = r_u16(bounds + 6);
    second = r_u16(bounds + 14);
    difference = first - second;
    w_u16(0x800F25E6, 0);
    w_u16(0x800F25E4, DivideRenderViewScalar(0 - (difference << 12), length));

    first = r_u16(bounds + 2);
    second = r_u16(bounds + 14);
    difference = first - second;
    depth = r_u16(bounds + 16);
    length = sub_80085B54(difference * difference + depth * depth);
    w_u16(0x800F25E8, 0);
    w_u16(0x800F25EA, DivideRenderViewScalar(0 - (r_u16(bounds + 16) << 12), length));
    first = r_u16(bounds + 14);
    second = r_u16(bounds + 2);
    difference = first - second;
    w_u16(0x800F25EE, 0);
    w_u16(0x800F25EC, DivideRenderViewScalar(0 - (difference << 12), length));

    first = r_u16(bounds + 12);
    second = r_u16(bounds + 4);
    difference = first - second;
    depth = r_u16(bounds + 16);
    length = sub_80085B54(difference * 25 * difference + depth * (depth << 6));
    w_u16(0x800F25F2, 0);
    w_u16(0x800F25F0, DivideRenderViewScalar(r_u16(bounds + 16) << 15, length));
    first = r_u16(bounds + 4);
    second = r_u16(bounds + 12);
    difference = first - second;
    w_u16(0x800F25F6, 0);
    w_u16(0x800F25F4, DivideRenderViewScalar(0 - ((difference * 5) << 12), length));

    first = r_u16(bounds);
    second = r_u16(bounds + 12);
    difference = first - second;
    depth = r_u16(bounds + 16);
    length = sub_80085B54(difference * 25 * difference + depth * (depth << 6));
    w_u16(0x800F25FA, 0);
    w_u16(0x800F25F8, DivideRenderViewScalar(0 - (r_u16(bounds + 16) << 15), length));
    first = r_u16(bounds + 12);
    second = r_u16(bounds);
    difference = first - second;
    w_u16(0x800F25FE, 0);
    w_u16(0x800F25FC, DivideRenderViewScalar(0 - ((difference * 5) << 12), length));

    first = r_u16(bounds + 12);
    second = r_u16(bounds + 4);
    difference = first - second;
    depth = r_u16(bounds + 16);
    length = sub_80085B54(difference * difference + depth * depth);
    w_u16(0x800F2602, 0);
    w_u16(0x800F2600, DivideRenderViewScalar(r_u16(bounds + 16) << 12, length));
    first = r_u16(bounds + 4);
    second = r_u16(bounds + 12);
    difference = first - second;
    w_u16(0x800F2606, 0);
    w_u16(0x800F2604, DivideRenderViewScalar(0 - (difference << 12), length));

    first = r_u16(bounds);
    second = r_u16(bounds + 12);
    difference = first - second;
    depth = r_u16(bounds + 16);
    length = sub_80085B54(difference * difference + depth * depth);
    w_u16(0x800F260A, 0);
    w_u16(0x800F2608, DivideRenderViewScalar(0 - (r_u16(bounds + 16) << 12), length));
    first = r_u16(bounds + 12);
    second = r_u16(bounds);
    difference = first - second;
    w_u16(0x800F260E, 0);
    w_u16(0x800F260C, DivideRenderViewScalar(0 - (difference << 12), length));
    for (axis = 0; axis < 3; ++axis)
    {
        RotateRenderViewNormal(camera + 52, first_normals, 0x800F3E30 + axis * 6);
        RotateRenderViewNormal(camera + 52, second_normals, 0x800F3E50 + axis * 6);
        first_normals += 8;
        second_normals += 8;
    }
    sub_800879EC(r_u16(bounds + 16));
    first = r_u16(bounds + 12);
    second = r_u16(bounds + 14);
    sub_800879CC(first, second);
    SetModelDrawArea(bounds, ordering_table);
    result = r_u16(0x800FFAE8);
    depth = r_u16(0x800FFAEC);
    w_u32(0x800FFB0C, camera);
    w_u32(0x800FFB08, bounds);
    w_u32(0x800FFB30, ordering_table);
    w_u16(bounds + 8, result);
    w_u16(bounds + 10, depth);
    TransposeRenderViewMatrix(camera + 52, camera + 84);
    for (axis = 0; axis < 3; ++axis)
    {
        sint32 scaled = 8 * (sint32)(sint16)r_u16(camera + 84 + axis * 2);
        uint32 middle = r_u16(camera + 90 + axis * 2);
        uint32 last = r_u16(camera + 96 + axis * 2);
        w_u16(camera + 122 + axis * 2, middle);
        w_u16(camera + 128 + axis * 2, last);
        w_u16(camera + 116 + axis * 2, (uint32)(scaled / 5));
    }
    resource = sub_8006E278(0xCCE0187C);
    animation_bank = r_u32(0x800FF778);
    result = r_u16(resource + 2) << 16;
    depth = r_u16(resource + 6) << 16;
    difference = r_u32(0x800FFB40);
    w_u32(0x800FFB24, result);
    w_u32(0x800FFB28, depth);
    first = r_u8(resource);
    second = r_u8(resource + 1);
    next_tick = r_u32(0x800FF650);
    result = first | (second << 8);
    w_u32(0x800FFAB4, difference);
    w_u32(0x800FFB40, next_tick);
    w_u32(0x800FFB2C, result);
    UpdateModelColorAnimation(animation_bank);
    UpdateModelColorAnimation(r_u32(0x800FF77C));
    UpdateModelColorAnimation(r_u8(0x800FF644));
    if (!r_u32(0x800FFAB0))
        InitializeModelUvWaveTable();
    UpdateModelTextureCoordinates(r_u8(0x800FF644));
    owner = r_u32(0x800FFB0C);
    w_u16(0x800FFB10, 0 - r_u16(owner + 58));
    w_u16(0x800FFB12, 0 - r_u16(owner + 60));
    w_u16(0x800FFB14, 0 - r_u16(owner + 62));
    first = r_u32(0x800FF37C);
    second = r_u16(owner + 8);
    result = (uint32)((sint32)first >> 12) - second;
    w_u16(0x800FFB16, result);
    return result;
}

extern "C" uint32 sub_8007E63C(uint32 camera, uint32 bounds, uint32 ordering_table)
{
    FUNCTION_MARKER(0x8007E63C, "SLUS_003.73");
    return ConfigureModelRenderView(camera, bounds, ordering_table);
}


uint32 InitializeModelUvWaveTable()
{
    uint32 output = 0x800F5130;
    uint32 amplitude, phase;
    FUNCTION_MARKER(0x8007FE68, "SLUS_003.73");
    for (amplitude = 0; amplitude < 16; ++amplitude)
    {
        for (phase = 0; phase < 64; ++phase)
        {
            sint32 product = (sint32)(sint16)r_u16(0x800F863C + phase * 0x100) * (sint32)amplitude;
            w_u16(output, (uint32)(product >> 4));
            output += 2;
        }
    }
    w_u32(0x800FFAB0, 1);
    return 1;
}

extern "C" uint32 sub_8007FE68(void)
{
    FUNCTION_MARKER(0x8007FE68, "SLUS_003.73");
    return InitializeModelUvWaveTable();
}


uint32 AdvanceModelRenderCounter()
{
    uint32 counter;
    FUNCTION_MARKER(0x8007EBC4, "SLUS_003.73");
    counter = r_u32(0x800FFAA8) + 1;
    w_u32(0x800FFAA8, counter);
    return counter;
}

extern "C" uint32 sub_8007EBC4(void)
{
    FUNCTION_MARKER(0x8007EBC4, "SLUS_003.73");
    return AdvanceModelRenderCounter();
}

static void SelectModelBoxCorners(uint32 bank, uint32 corners[3])
{
    uint32 words[5];
    for (uint32 index = 0; index < 5; ++index)
        words[index] = xport_draft_gte_control_read(bank + index);
    corners[0] = 0x1F800000 + ((words[0] >> 12) & 8 | (words[0] >> 27) & 16 | (words[1] >> 10) & 32);
    corners[1] = 0x1F800000 + ((words[1] >> 28) & 8 | (words[2] >> 11) & 16 | (words[2] >> 26) & 32);
    corners[2] = 0x1F800000 + ((words[3] >> 12) & 8 | (words[3] >> 27) & 16 | (words[4] >> 10) & 32);
}

static void WriteModelBoxAxis(uint32 axis, sint32 position, uint32 bounds)
{
    sint32 lower = (sint16)((uint32)position + bounds) >> 1;
    sint32 upper = (sint32)((uint32)position + (uint32)((sint32)bounds >> 16)) >> 1;
    const uint32 offsets[3][8] = {
        {0, 16, 32, 48, 8, 24, 40, 56},
        {2, 10, 34, 42, 18, 26, 50, 58},
        {4, 12, 20, 28, 36, 44, 52, 60}
    };
    for (uint32 index = 0; index < 4; ++index)
        w_u16(0x1F800000 + offsets[axis][index], lower);
    for (uint32 index = 4; index < 8; ++index)
        w_u16(0x1F800000 + offsets[axis][index], upper);
}

static bool IsModelBoxOutside(const uint32 light_corners[3], const uint32 color_corners[3])
{
    xport_gte_write_data(0, r_u32(light_corners[0]));
    xport_gte_write_data(1, r_u32(light_corners[0] + 4));
    xport_gte_execute(0x4A2012);
    sint32 plane = (sint32)xport_gte_read_data(9);
    xport_gte_write_data(2, r_u32(light_corners[1]));
    xport_gte_write_data(3, r_u32(light_corners[1] + 4));
    if (plane < 0)
        return true;
    xport_gte_execute(0x4AA012);
    plane = (sint32)xport_gte_read_data(10);
    xport_gte_write_data(4, r_u32(light_corners[2]));
    xport_gte_write_data(5, r_u32(light_corners[2] + 4));
    if (plane < 0)
        return true;
    xport_gte_execute(0x4B2012);
    plane = (sint32)xport_gte_read_data(11);
    xport_gte_write_data(0, r_u32(color_corners[0]));
    xport_gte_write_data(1, r_u32(color_corners[0] + 4));
    if (plane < 0)
        return true;
    xport_gte_execute(0x4C6012);
    plane = (sint32)xport_gte_read_data(9);
    xport_gte_write_data(2, r_u32(color_corners[1]));
    xport_gte_write_data(3, r_u32(color_corners[1] + 4));
    if (plane < 0)
        return true;
    xport_gte_execute(0x4CE012);
    plane = (sint32)xport_gte_read_data(10);
    xport_gte_write_data(4, r_u32(color_corners[2]));
    xport_gte_write_data(5, r_u32(color_corners[2] + 4));
    if (plane < 0)
        return true;
    xport_gte_execute(0x4D6012);
    return (sint32)xport_gte_read_data(11) < 0;
}

uint32 CullModelObjectList(uint32 object)
{
    FUNCTION_MARKER(0x80080560, "SLUS_003.73");
    if (!object)
        return 0x1F800000;
    uint32 light_corners[3], color_corners[3];
    SelectModelBoxCorners(8, light_corners);
    SelectModelBoxCorners(16, color_corners);
    uint32 camera = r_u32(0x800FFB0C);
    uint32 camera_x = r_u32(camera + 4);
    uint32 camera_y = r_u32(camera + 8);
    uint32 camera_z = r_u32(camera + 12);
    do
    {
        uint32 flags = r_u16(object);
        uint32 position_x = r_u32(object + 4);
        uint32 position_y = r_u32(object + 8);
        uint32 position_z = r_u32(object + 12);
        bool outside = true;
        do
        {
            if (flags & 1)
                break;
            sint32 x = (sint32)((uint32)((sint32)position_x >> 12) - camera_x);
            sint32 y = (sint32)((uint32)((sint32)position_y >> 12) - camera_y);
            sint32 z = (sint32)((uint32)((sint32)position_z >> 12) - camera_z);
            if (z >= 32767 || z < -32767 || x >= 32767 || x < -32767 || y >= 32767 || y < -32767)
                break;
            xport_gte_write_data(0, ((uint32)(x >> 1) & 65535) | ((uint32)(y >> 1) << 16));
            xport_gte_write_data(1, (uint32)(z >> 1));
            uint32 geometry_index = r_u16(object + 22);
            uint32 bank = r_u8(object + 27);
            xport_gte_execute(0x4A2012);
            uint32 geometry_table = r_u32(0x800EAEF8 + bank * 64 + 16);
            uint32 geometry = r_u32(geometry_table + geometry_index * 4);
            uint32 radius = r_u32(geometry + 16) >> 13;
            if (flags & 512)
            {
                sint32 scale_x = (sint16)r_u16(object + 36);
                sint32 scale_y = (sint16)r_u16(object + 38);
                sint32 scale_z = (sint16)r_u16(object + 40);
                sint32 scale = scale_x;
                if (scale_y > scale)
                    scale = scale_y;
                if (scale_z > scale)
                    scale = scale_z;
                radius = (radius * (uint32)scale) >> 12;
            }
            sint32 first = (sint32)xport_gte_read_data(25);
            sint32 second = (sint32)xport_gte_read_data(26);
            sint32 third = (sint32)xport_gte_read_data(27);
            xport_gte_execute(0x4C6012);
            sint32 threshold = (sint32)(0 - radius);
            if (first < threshold || second < threshold || third < threshold)
                break;
            first = (sint32)xport_gte_read_data(25);
            second = (sint32)xport_gte_read_data(26);
            third = (sint32)xport_gte_read_data(27);
            if (first < threshold || second < threshold || third < threshold)
                break;
            uint32 rotation_xy = r_u32(object + 16);
            uint32 rotation_z = r_u16(object + 20);
            uint32 bounds_x = r_u32(geometry + 20);
            if (!(rotation_xy | rotation_z | (flags & 514)))
            {
                uint32 bounds_y = r_u32(geometry + 24);
                uint32 bounds_z = r_u32(geometry + 28);
                WriteModelBoxAxis(0, x, bounds_x);
                WriteModelBoxAxis(1, y, bounds_y);
                WriteModelBoxAxis(2, z, bounds_z);
                if (IsModelBoxOutside(light_corners, color_corners))
                    break;
            }
            outside = false;
        } while (false);
        uint32 next = r_u32(object + 28);
        w_u16(object, outside ? flags | 32768 : flags & 32767);
        object = next;
    } while (object);
    return 0x1F800000;
}

extern "C" uint32 sub_80080560(uint32 object)
{
    FUNCTION_MARKER(0x80080560, "SLUS_003.73");
    return CullModelObjectList(object);
}

static void LoadVisibleModelMatrix(uint32 address, uint32 bank)
{
    uint32 words[5];
    for (uint32 index = 0; index < 5; ++index)
        words[index] = r_u32(address + index * 4);
    for (uint32 index = 0; index < 5; ++index)
        xport_gte_write_control(bank + index, words[index]);
}

void RenderVisibleModelObjectList(uint32 owner)
{
    FUNCTION_MARKER(0x8007EBDC, "SLUS_003.73");
    if (!owner)
        return;
    LoadVisibleModelMatrix(0x800F3E30, 8);
    LoadVisibleModelMatrix(0x800F3E50, 16);
    uint32 plane_x = (uint32)((sint32)(sint16)r_u16(0x800F25D6) >> 1);
    uint32 plane_y = (uint32)((sint32)(sint16)r_u16(0x800F25DE) >> 1);
    xport_gte_write_control(13, plane_x);
    xport_gte_write_control(14, plane_y);
    xport_gte_write_control(15, 0);
    CullModelObjectList(owner);
    if (owner == r_u32(0x800FF794))
    {
        UpdateModelTextureCoordinates(r_u32(0x800FF778));
        UpdateModelTextureCoordinates(r_u32(0x800FF77C));
    }
    while (owner)
    {
        uint32 flags = r_u16(owner), model, mode, geometry, camera, axis;
        sint32 position[3];
        uint32 transformed[4], matrix[8], combined[8];
        uint32 render_limit, depth_shift, fog_scale, fog_bias, far_start, far_end;
        uint32 vertex_index, geometry_table, object_value, camera_value, geometry_flags;
        uint32 rotation_xy, rotation_z, attributes, attribute_end, vertex_count, material;
        uint32 packet_start, packet_end, secondary_flags, callback_flags;
        bool transform_matrix;
        if (flags & 0x8001)
            goto NextObject;
        model = 0x800EAEF8 + ((uint32)r_u8(owner + 27) << 6);
        if (!r_u8(model + 10))
            goto NextObject;
        render_limit = r_u32(0x800FFACC);
        mode = r_u32(0x800FFAA0);
        depth_shift = r_u32(0x800FFAD4);
        fog_scale = r_u32(0x800FFB24);
        fog_bias = r_u32(0x800FFB28);
        far_start = (uint32)(sint32)(sint16)r_u16(0x800FFAAC);
        far_end = (uint32)(sint32)(sint16)r_u16(0x800FFAAE);
        flags = r_u16(owner);
        w_u32(0x1F800164, render_limit);
        w_u32(0x1F800174, depth_shift);
        w_u32(0x1F800184, fog_scale);
        w_u32(0x1F800194, fog_bias);
        w_u32(0x1F800204, far_start);
        w_u32(0x1F800214, far_end);
        if (flags & 0x80)
        {
            mode = (flags & 0x1000) ? (mode & 0xFFF70000) : (mode | 12);
            if (r_u16(owner) & 0x400)
            {
                uint32 color = r_u32(owner + 32);
                w_u32(0x800FFB20, color & 0xFFFFFF);
                w_u16(0x800FFA96, (color & 0xFF00) >> 2);
                uint32 red = r_u8(0x800FFB20);
                w_u16(0x800FFA98, ((color & 0xFFFFFF) >> 10) & 0x3FC0);
                w_u16(0x800FFA94, red << 6);
            }
        }
        else if (flags & 0x400)
        {
            uint32 color = r_u32(owner + 32);
            mode = (mode & 0xFFFB0000) | 8;
            w_u32(0x800FFB20, color & 0xFFFFFF);
            w_u32(0x1F800154, color & 0xFFFFFF);
            w_u32(0x1F8001A4, (color & 0xFF) << 6);
            w_u32(0x1F8001B4, (color & 0xFF00) >> 2);
            w_u32(0x1F8001C4, ((color & 0xFFFFFF) >> 10) & 0x3FC0);
        }
        if (r_u16(owner) & 0x800)
            mode = (mode & 0xFFFFFE7F) | (r_u32(owner + 44) & 0x180) | 0x40;
        callback_flags = r_u16(owner);
        w_u32(0x1F800144, mode);
        if (callback_flags & 0x80)
        {
            uint32 vtable = r_u32(owner + 68);
            uint32 adjusted = owner + (uint32)(sint32)(sint16)r_u16(vtable + 40);
            /* TODO Missing native dispatch for this exact guest virtual target */
            xport_draft_guest_call1(r_u32(vtable + 44), adjusted);
        }
        flags = r_u16(owner);
        secondary_flags = r_u16(owner);
        w_u32(0x800FFB3C, (flags >> 3) & 1);
        if (secondary_flags & 2)
        {
            packet_start = r_u32(0x800FF668);
            RenderPosedModelObject(owner);
            if (r_u16(owner) & 64)
            {
                packet_end = r_u32(0x800FF668);
                // Original hook discards object and packet range arguments
                (void)packet_start;
                (void)packet_end;
                nullsub_18();
            }
            goto NextObject;
        }
        model = 0x800EAEF8 + ((uint32)r_u8(owner + 27) << 6);
        vertex_index = r_u16(owner + 22);
        geometry_table = r_u32(model + 16);
        geometry = r_u32(geometry_table + vertex_index * 4);
        if (r_u32(geometry) & 0x20)
            goto NextObject;
        camera = r_u32(0x800FFB0C);
        LoadVisibleModelMatrix(camera + 116, 0);
        for (axis = 0; axis < 3; ++axis)
            xport_gte_write_control(5 + axis, 0);
        camera = r_u32(0x800FFB0C);
        for (axis = 0; axis < 3; ++axis)
        {
            object_value = r_u32(owner + 4 + axis * 4);
            camera_value = r_u32(camera + 4 + axis * 4);
            position[axis] = (sint32)((uint32)((sint32)object_value >> 12) - camera_value);
        }
        xport_gte_write_data(0, ((uint32)position[0] & 65535) | ((uint32)position[1] << 16));
        xport_gte_write_data(1, (uint32)position[2]);
        xport_gte_execute(0x486012);
        for (axis = 0; axis < 3; ++axis)
            transformed[axis] = xport_gte_read_data(25 + axis);
        rotation_xy = r_u32(owner + 16);
        w_u32(0x800FFB44, 0);
        rotation_z = rotation_xy ? 0 : r_u16(owner + 20);
        transform_matrix = rotation_xy || rotation_z;
        if (transform_matrix)
            BuildModelRotationMatrix((const uint32 *)psx_addr(owner + 16, 8), (uint16 *)matrix);
        else
            InitializeModelIdentityMatrix(matrix);
        if (r_u16(owner) & 512)
        {
            ScaleModelRotationMatrix((const uint8 *)psx_addr(owner, 42), (uint16 *)matrix);
            transform_matrix = true;
        }
        if (transform_matrix)
        {
            TransformPackedModelMatrix(matrix, combined);
            SetRotMatrix((MATRIX *)combined);
        }
        model = 0x800EAEF8 + ((uint32)r_u8(owner + 27) << 6);
        material = r_u32(model + 32);
        geometry_flags = r_u32(geometry);
        w_u32(0x800FFB04, material);
        if (geometry_flags & 1)
            GenerateModelReflectionCoordinates(matrix, transformed, 0x800F3E70, geometry);
        else if (r_u16(owner) & 128)
        {
            attributes = r_u32(geometry + 8);
            attribute_end = r_u32(geometry + 12);
            vertex_count = r_u32(geometry + 4);
            if (attributes != attribute_end)
                attributes = vertex_count;
            GenerateModelNormalColors(geometry + 32 + vertex_count * 8, attributes);
        }
        if (r_u32(0x800FFB44))
            RenderOffsetScratchModel(geometry, position);
        else if (r_u32(owner + 16) || r_u16(owner + 20) || (r_u16(owner) & 0x200))
        {
            for (axis = 0; axis < 3; ++axis)
                xport_gte_write_control(5 + axis, transformed[axis]);
            RenderProjectedModel(geometry);
        }
        else
            RenderOffsetModel(geometry, position);
    NextObject:
        owner = r_u32(owner + 28);
    }
}

extern "C" void sub_8007EBDC(uint32 object)
{
    FUNCTION_MARKER(0x8007EBDC, "SLUS_003.73");
    RenderVisibleModelObjectList(object);
}

static void MultiplyPosedModelMatrix(uint32 left, uint32 right, void *output)
{
    MulMatrix0((MATRIX *)psx_addr(left, 20), (MATRIX *)psx_addr(right, 20), (MATRIX *)output);
}

uint32 RenderPosedModelObject(uint32 object)
{
    FUNCTION_MARKER(0x8007F340, "SLUS_003.73");
    uint32 flags = r_u16(object), camera, table, models, count, pose;
    uint32 index, mode, animation = 0, rgb = 0, uv0 = 0, uv1 = 0;
    uint32 rotation[8], light[8], matrix[8], transformed[3], relative[3];
    uint32 far_x, far_y, far_z, pose_index, poses, pose_entry, object_value, camera_value;
    // The unlit branch uses the approved zero light matrix
    uint32 axis;
    w_u32(0x800FFB44, (flags & 0xC80) ? 0 : 1);
    camera = r_u32(0x800FFB0C);
    w_u32(0x800FFB44, 0);
    SetRotMatrix((MATRIX *)psx_addr(camera + 116, 20));
    TransformPackedModelMatrix((const uint32 *)psx_addr(object + 324, 20), rotation);
    camera = r_u32(0x800FFB0C);
    for (axis = 0; axis < 3; ++axis)
    {
        object_value = r_u32(object + 4 + axis * 4);
        camera_value = r_u32(camera + 4 + axis * 4);
        relative[axis] = (uint32)((sint32)object_value >> 12) - camera_value;
    }
    for (axis = 0; axis < 3; ++axis)
        xport_gte_write_data(9 + axis, relative[axis]);
    xport_gte_execute(0x49E012);
    table = 0x800EAEF8 + ((uint32)r_u8(object + 27) << 6);
    models = r_u32(table + 16);
    count = r_u32(models - 4);
    for (axis = 0; axis < 3; ++axis)
    {
        transformed[axis] = xport_gte_read_data(25 + axis);
        rotation[5 + axis] = transformed[axis];
    }
    if (flags & 4)
        pose = r_u32(object + 356);
    else
    {
        table = 0x800EAEF8 + ((uint32)r_u8(object + 27) << 6);
        pose_index = r_u8(object + 26);
        poses = r_u32(table + 24);
        pose_entry = poses + (pose_index << 3);
        if (r_u16(pose_entry + 10))
        {
            sub_8007CECC(object);
            pose = 0x800ED760;
        }
        else
        {
            uint32 frame = r_u8(object + 24);
            uint32 offset = r_u32(pose_entry + 4);
            pose = poses + offset + 24 * frame * count;
        }
    }
    if (flags & 0x80)
    {
        sub_8008793C(0x800EE710);
        far_x = (uint32)(sint32)(sint16)r_u16(0x800FFA8C);
        far_y = (uint32)(sint32)(sint16)r_u16(0x800FFA8E);
        far_z = (uint32)(sint32)(sint16)r_u16(0x800FFA90);
        sub_800879AC(far_x, far_y, far_z);
        far_x = (uint32)(sint32)(sint16)r_u16(0x800FFA94);
        far_y = (uint32)(sint32)(sint16)r_u16(0x800FFA96);
        far_z = (uint32)(sint32)(sint16)r_u16(0x800FFA98);
        sub_8008798C(far_x, far_y, far_z);
        MultiplyPosedModelMatrix(0x800EE6F0, object + 324, light);
    }
    else
    {
        for (axis = 0; axis < 8; ++axis)
            light[axis] = 0;
    }
    mode = r_u8(object + 179);
    if (mode & 0x7F)
    {
        sint32 divisor, dividend, remainder;
        uint32 animation_index = (mode - 1) & 0x7F;
        if (animation_index >= 15)
            animation_index = 14;
        animation = r_u32(0x800A6808 + animation_index * 4);
        uint32 tick = r_u32(0x800FF650);
        uint32 previous_tick = r_u32(0x800FFAB8);
        if (previous_tick != tick)
        {
            uint32 render_counter = r_u32(0x800FFAA8);
            uint32 next_tick = r_u32(0x800FF650);
            w_u32(0x800FFABC, render_counter);
            w_u32(0x800FFAB8, next_tick);
        }
        dividend = (sint32)r_u32(0x800FFABC);
        divisor = (sint32)r_u32(animation - 4);
        remainder = divisor == 0 ? dividend : (dividend == (sint32)0x80000000 && divisor == -1 ? 0 : dividend % divisor);
        uv0 = r_u16(object + 180);
        uv1 = r_u16(object + 182);
        rgb = r_u32(object + 176) & 0xFFFFFF;
        animation += (uint32)remainder << 3;
    }
    w_u32(0x800FFB18, r_u32(0x800FFB44) ? 0x1F8003F8 : r_u32(0x800FFAC0) + 7992);
    table = 0x800EAEF8 + ((uint32)r_u8(object + 27) << 6);
    w_u32(0x800FFB04, r_u32(table + 32));
    for (index = 0; index < count; ++index, pose += 24)
    {
        uint32 model, effects;
        w_u32(0x800FFB38, (r_u32(object + 372) & ((uint32)1 << (index & 31))) != 0);
        model = r_u32(models + index * 4);
        SetupModelGteMatrices(rotation, light, (const uint32 *)psx_addr(pose, 24));
        if (r_u32(model) & 1)
        {
            MultiplyPosedModelMatrix(object + 324, pose, matrix);
            GenerateModelReflectionCoordinates(matrix, transformed, 0x800F3E70, model);
        }
        else if (flags & 128)
        {
            uint32 attributes = r_u32(model + 8);
            uint32 packets = r_u32(model + 12);
            uint32 vertices = r_u32(model + 4);
            if (attributes != packets)
                attributes = vertices;
            GenerateModelNormalColors(model + 32 + vertices * 8, attributes);
        }
        if (!(mode & 0x80))
        {
            if (r_u32(0x800FFB44))
                RenderScratchModel(model);
            else
                RenderTranslatedModel(model);
        }
        if (r_u32(0x800FFB38))
            continue;
        effects = r_u8(object + 314) != 0;
        if ((mode & 0x7F) || effects)
        {
            MultiplyPosedModelMatrix(object + 324, pose, matrix);
            uint32 translation[3];
            for (axis = 0; axis < 3; ++axis)
                translation[axis] = r_u16(pose + 18 + axis * 2);
            for (axis = 0; axis < 3; ++axis)
                xport_gte_write_data(9 + axis, translation[axis]);
            LoadModelRotationAndTransformIr((const uint32 *)psx_addr(object + 324, 20));
            for (axis = 0; axis < 3; ++axis)
                translation[axis] = xport_gte_read_data(25 + axis);
            for (axis = 0; axis < 3; ++axis)
                matrix[5 + axis] = translation[axis] + (uint32)((sint32)r_u32(object + 4 + axis * 4) >> 12);
        }
        if ((mode & 0x7F) && (!r_u32(0x800FFB3C) || (sint32)matrix[6] < ((sint32)r_u32(0x800FF37C) >> 12) - 40))
            RenderModelShadow(model, matrix, rgb, uv0, (uint32)(sint32)(sint16)uv1, r_u32(animation + 4));
        if (effects)
        {
            uint32 effect_index = 0;
            while (effect_index < r_u8(object + 314))
            {
                uint32 effect = r_u32(object + 316) + 8 * effect_index;
                uint32 colors = r_u32(object + 320);
                uint32 color = r_u32(colors + effect_index * 4);
                RenderModelProjectionEffect(model, matrix, object + 164, object + 184, effect, color);
                ++effect_index;
            }
        }
    }
    return 0;
}

extern "C" uint32 sub_8007F340(uint32 object)
{
    FUNCTION_MARKER(0x8007F340, "SLUS_003.73");
    return RenderPosedModelObject(object);
}

void CopyModelRotationMatrix(const uint32 *input, uint32 *output)
{
    FUNCTION_MARKER(0x80085C04, "SLUS_003.73");
    uint32 source = GuestMemoryAddress(input), destination = GuestMemoryAddress(output);
    uint32 words[4];
    for (uint32 index = 0; index < 4; ++index)
        words[index] = ReadModelVertexWord(input, source, index * 4);
    uint32 tail = ReadModelMatrixHalf(input, source, 16);
    for (uint32 index = 0; index < 4; ++index)
        WriteModelMatrixWord(output, destination, index * 4, words[index]);
    if (destination)
        w_u16(destination + 16, tail);
    else
        xport_store_le16((uint8 *)output + 16, (uint16)tail);
}

void ClearModelRotationMatrix(uint32 *output)
{
    FUNCTION_MARKER(0x80085BE8, "SLUS_003.73");
    uint32 destination = GuestMemoryAddress(output);
    for (uint32 index = 0; index < 4; ++index)
        WriteModelMatrixWord(output, destination, index * 4, 0);
    if (destination)
        w_u16(destination + 16, 0);
    else
        xport_store_le16((uint8 *)output + 16, 0);
}

extern "C" void sub_80085C04(uint32 input, uint32 output)
{
    FUNCTION_MARKER(0x80085C04, "SLUS_003.73");
    CopyModelRotationMatrix((const uint32 *)psx_addr(input, 18), (uint32 *)psx_addr(output, 18));
}
extern "C" void sub_80085BE8(uint32 output)
{
    FUNCTION_MARKER(0x80085BE8, "SLUS_003.73");
    ClearModelRotationMatrix((uint32 *)psx_addr(output, 18));
}

static void ReadModelPolygonUvs(uint32 descriptor, uint32 flags, uint32 normals, uint32 scratch, const uint32 offsets[4], uint32 vertices, uint32 uv[4])
{
    if (flags & 1)
    {
        uv[0] = r_u32(descriptor + 20);
        uv[1] = r_u32(descriptor + 24);
        uv[2] = r_u32(descriptor + 28);
    }
    else
    {
        uint32 page = r_u32(scratch + 0x184);
        uv[0] = r_u32(normals + offsets[0] + 4);
        uv[1] = r_u32(normals + offsets[1] + 4);
        uv[0] |= page;
        page = r_u32(scratch + 0x194);
        uv[2] = r_u32(normals + offsets[2] + 4);
        if (vertices == 4)
            uv[2] |= r_u32(normals + offsets[3] + 4) << 16;
        uv[1] |= page;
    }
    uv[1] |= (flags & 0x180) << 14;
    if (vertices == 4)
        uv[3] = uv[2] >> 16;
}

/* Native helper for the original eight packet-format branches */
static uint32 StoreModelPolygonPacket(uint32 packet, uint32 descriptor, uint32 flags, uint32 normals, uint32 scratch, const uint32 offsets[4], uint32 vertices, uint32 colored, uint32 textured, uint32 first_color)
{
    uint32 uv[4], i, stride, command;
    if (!colored && textured)
        ReadModelPolygonUvs(descriptor, flags, normals, scratch, offsets, vertices, uv);
    stride = colored ? (textured ? 12 : 8) : (textured ? 8 : 4);
    for (i = 1; i < vertices; ++i)
        w_u32(packet + 8 + i * stride, xport_gte_read_data(11 + i));
    if (textured)
    {
        if (colored)
            ReadModelPolygonUvs(descriptor, flags, normals, scratch, offsets, vertices, uv);
        for (i = 0; i < vertices; ++i)
            w_u32(packet + 12 + i * stride, uv[i]);
    }
    if (!colored)
        first_color = xport_gte_read_data(22);
    else if (vertices == 3)
        first_color = xport_gte_read_data(20);
    command = 0x20000000 | (vertices == 4 ? 0x08000000 : 0) | (colored ? 0x10000000 : 0) | (textured ? 0x04000000 : 0);
    PackModelPolygonCommand(packet, first_color, command, flags);
    if (colored)
    {
        for (i = 1; i < vertices; ++i)
            w_u32(packet + 4 + i * stride, xport_gte_read_data((vertices == 3 ? 20 : 19) + i));
        return (vertices * (textured ? 3 : 2)) << 24;
    }
    return (1 + vertices * (textured ? 2 : 1)) << 24;
}

void BuildModelPolygonPackets(uint32 descriptor, uint32 count)
{
    FUNCTION_MARKER(0x800817FC, "SLUS_003.73");
    /* TODO Validate near-plane and subdivision carrier composition */
    const uint32 scratch = 0x1F800000;
    const uint32 normals = 0x800F3E70;
    uint32 packet, projection, table, ordering, limit;
    if (!count)
        return;
    ordering = r_u32(0x800FFB30);
    packet = r_u32(0x800FF668) & 0xFFFFFF;
    projection = r_u32(0x800FFAC0);
    table = r_u32(0x800FFB04);
    limit = r_u32(0x800FF374) - 100;
    xport_gte_write_control(5, 0);
    xport_gte_write_control(6, 0);
    xport_gte_write_control(7, 0);
    xport_gte_write_control(28, 0x10000000);
    xport_gte_write_control(27, 0);
    w_u32(scratch + 0x1E4, 0x80082FAC);
    w_u32(scratch + 0x1F4, 0);
    for (;;)
    {
        uint32 offsets[4], points[4], depths[4], flags, clipping, area, fourth_screen;
        uint32 packed = r_u32(descriptor + 4);
        uint32 packed2 = r_u32(descriptor + 8);
        uint32 mask = r_u32(scratch + 0x144);
        uint32 depth, fog = 0, fog_shift, vertices, colored, textured, tag;
        uint32 first_color = 0, normal[3], index[4];
        uint32 i, bucket, previous, bytes, chain_head = packet;
        offsets[0] = packed & 0xFFFF;
        offsets[1] = packed >> 16;
        offsets[2] = packed2 & 0xFFFF;
        offsets[3] = packed2 >> 16;
        flags = (r_u32(descriptor) & (uint32)((sint32)mask >> 16)) | (mask & 0xFFFF);
        for (i = 0; i < 4; ++i)
            points[i] = projection + offsets[i];
        for (i = 0; i < 3; ++i)
            xport_gte_write_data(12 + i, r_u32(points[i]));
        if (!(flags & 0xC0))
            goto next_descriptor;
        xport_gte_execute(0x1400006);
        fourth_screen = r_u32(points[3]);
        for (i = 0; i < 4; ++i)
            depths[i] = r_u32(points[i] + 4);
        w_u32(packet + 8, xport_gte_read_data(12));
        clipping = ((depths[0] & depths[1] & depths[2] & depths[3]) >> 16) ^ 0xFF00;
        xport_gte_write_data(15, fourth_screen);
        area = xport_gte_read_data(24);
        xport_gte_execute(0x1400006);
        if (clipping & 0x20FF)
            goto next_descriptor;
        if (clipping & 0x5000)
        {
            uint32 geometry0 = points[0] + 0x1F40;
            uint32 geometry1 = points[1] + 0x1F40;
            uint32 geometry2 = points[2] + 0x1F40;
            uint32 cross[3], magnitude = 0, geometry_depth;
            if (!(flags & 0x1000))
                goto next_descriptor;
            packed = r_u32(geometry0);
            geometry_depth = r_u32(points[0] + 4);
            xport_gte_write_control(0, packed);
            xport_gte_write_control(2, packed >> 16);
            xport_gte_write_control(4, geometry_depth);
            packed = r_u32(geometry1);
            geometry_depth = r_u32(points[1] + 4);
            xport_gte_write_data(9, packed);
            xport_gte_write_data(10, packed >> 16);
            xport_gte_write_data(11, geometry_depth);
            xport_gte_execute(0x170000C);
            if ((sint32)xport_gte_read_flag() < 0)
            {
                uint32 shift;
                for (i = 0; i < 3; ++i)
                {
                    cross[i] = xport_gte_read_data(25 + i);
                    magnitude |= (sint32)cross[i] < 0 ? 0 - cross[i] : cross[i];
                }
                xport_gte_write_data(30, magnitude);
                shift = (17 - xport_gte_read_data(31)) & 31;
                for (i = 0; i < 3; ++i)
                    cross[i] = xport_gte_read_data(25 + i);
                for (i = 0; i < 3; ++i)
                    xport_gte_write_data(9 + i, (uint32)((sint32)cross[i] >> shift));
            }
            packed = r_u32(geometry2);
            geometry_depth = r_u32(points[2] + 4);
            xport_gte_write_control(0, packed);
            xport_gte_write_control(1, geometry_depth);
            xport_gte_execute(0x41E012);
            if ((sint32)xport_gte_read_data(25) < 0)
                goto next_descriptor;
            area = 0x7FFF0000;
        }
        else if ((sint32)area <= 0)
        {
            area = xport_gte_read_data(24);
            if ((sint32)area >= 0)
                goto next_descriptor;
            area = 0 - area;
        }
        xport_gte_write_data(24, area);
        if (packet >= limit)
            break;
        if (clipping & 0x8000)
        {
            apocalypse_model_nearplane_native(descriptor, scratch, projection, flags, offsets, depths);
            goto next_descriptor;
        }
        depth = depths[0] << 16;
        for (i = 1; i < 4; ++i)
        {
            uint32 candidate = depths[i] << 16;
            if ((sint32)(depth - candidate) < 0)
                depth = candidate;
        }
        depth = (uint32)((sint32)depth >> 16);
        packed = r_u32(scratch + 0x164);
        fog_shift = r_u32(scratch + 0x174);
        packed = depth - packed;
        if ((sint32)packed > 0)
            fog = (sint32)fog_shift >= 0 ? packed << (fog_shift & 31) : (uint32)((sint32)packed >> ((0 - fog_shift) & 31));
        xport_gte_write_data(8, fog);
        vertices = flags & 0x10 ? 3 : 4;
        colored = (flags & 0x800) || ((flags & 12) == 12);
        textured = colored ? (flags & 1) : (flags & 3);
        if (flags & 0x800)
        {
            packed = r_u32(descriptor + 12);
            index[0] = packed << 2;
            index[1] = packed >> 6;
            index[2] = packed >> 14;
            index[3] = packed >> 22;
            if ((flags & 12) == 0)
            {
                if (vertices == 4)
                {
                    xport_gte_write_data(6, r_u32(table + (index[0] & 0x3FC)));
                    xport_gte_execute(0x780010);
                    for (i = 1; i < 4; ++i)
                    {
                        uint32 table_color = ReadModelTableColor(index[i], table);
                        if (i == 3)
                            first_color = xport_gte_read_data(22);
                        xport_gte_write_data(19 + i, table_color);
                    }
                }
                else
                {
                    for (i = 0; i < 3; ++i)
                        xport_gte_write_data(20 + i, r_u32(table + (index[i] & 0x3FC)));
                }
                xport_gte_execute(0xF8002A);
            }
            else
            {
                if (!(flags & 4))
                {
                    normal[0] = r_u32(scratch + 0x1A4);
                    normal[1] = r_u32(scratch + 0x1B4);
                    normal[2] = r_u32(scratch + 0x1C4);
                }
                else if (!(flags & 8))
                    ReadRendererNormal(normals + r_u32(descriptor + 16), normal);
                for (i = 0; i < vertices; ++i)
                {
                    if ((flags & 12) == 12)
                        LoadRendererVertexLighting(index[i], table, normals + offsets[i]);
                    else
                        LoadModelTableColor(index[i], table, normal[0], normal[1], normal[2]);
                    if (i == 3)
                        first_color = xport_gte_read_data(20);
                    xport_draft_gte_execute(0x680029);
                }
            }
        }
        else
        {
            xport_gte_write_data(6, r_u32(descriptor + 12));
            if ((flags & 12) == 12)
            {
                for (i = 0; i < vertices; ++i)
                {
                    LoadPackedProjectionIr(normals + offsets[i]);
                    if (i == 3)
                        first_color = xport_gte_read_data(20);
                    xport_draft_gte_execute(0x680029);
                }
            }
            else
            {
                if (flags & 4)
                    LoadPackedProjectionIr(normals + r_u32(descriptor + 16));
                else if (flags & 8)
                    LoadRendererScratchLighting(scratch);
                xport_draft_gte_execute(flags & 12 ? 0x680029 : 0x780010);
            }
        }
        tag = StoreModelPolygonPacket(packet, descriptor, flags, normals, scratch, offsets, vertices, colored, textured, first_color);
        if ((flags & 0x1000) && (sint32)(0x800 - xport_gte_read_data(24)) < 0)
        {
            if ((flags & 1) && ((flags & 0x800) || ((flags & 12) == 12)))
            {
                uint32 last_stride, end = apocalypse_model_subdivide_textured_native(packet, limit, scratch, points, xport_gte_read_data(24), !(flags & 0x10), &last_stride);
                if (end == packet)
                    goto next_descriptor;
                packet = end - last_stride;
                tag = (last_stride - 4) << 22;
            }
            else if (textured && !(flags & 0x10))
            {
                uint32 last_stride = 40, end = apocalypse_model_subdivide_flat_textured_quad_native(packet, limit, scratch, points, xport_gte_read_data(24), &last_stride);
                if (end == packet)
                    goto next_descriptor;
                packet = end - last_stride;
                tag = (last_stride - 4) << 22;
            }
            else if (!(flags & (0x10 | 0x800)) && ((flags & 12) != 12) && !textured)
            {
                uint32 last_stride = 24, end = apocalypse_model_subdivide_flat_quad_native(packet, limit, scratch, points, xport_gte_read_data(24), &last_stride);
                if (end == packet)
                    goto next_descriptor;
                packet = end - last_stride;
                tag = (last_stride - 4) << 22;
            }
            else
            {
                uint32 last_stride, end;
                if (colored)
                    end = apocalypse_model_subdivide_colored_native(packet, limit, scratch, points, xport_gte_read_data(24), vertices == 4, &last_stride);
                else
                    end = apocalypse_model_subdivide_flat_triangle_native(packet, limit, scratch, points, xport_gte_read_data(24), textured, &last_stride);
                if (end == packet)
                    goto next_descriptor;
                packet = end - last_stride;
                tag = (last_stride - 4) << 22;
            }
        }
        if (flags & 0x6000)
        {
            uint32 bias = r_u32(scratch + 0x204);
            if (!(flags & 0x2000))
                bias = r_u32(scratch + 0x214);
            depth += bias;
        }
        bucket = ordering + (depth < 0x4000 ? depth & 0xFFFC : 0x3FFC);
        previous = r_u32(bucket);
        if (flags & 0x20)
        {
            uint32 end, window_command = r_u32(descriptor + 32);
            tag += 0x03000000;
            w_u32(packet, previous | tag);
            end = packet + (tag >> 22) + 4;
            w_u32(bucket, end);
            w_u32(end - 12, 0);
            w_u32(end - 8, 0xE2000000);
            w_u32(end - 4, 0);
            w_u32(end, chain_head | 0x02000000);
            w_u32(end + 4, window_command);
            w_u32(end + 8, 0);
            packet = end + 12;
        }
        else
        {
            w_u32(bucket, chain_head);
            w_u32(packet, previous | tag);
            bytes = (tag >> 22) + 4;
            if ((flags & 0x41) == 0x40)
            {
                uint32 end = packet + bytes;
                w_u32(end + 4, 0xE1000200 | ((flags & 0x180) >> 2));
                w_u32(end, packet | 0x01000000);
                w_u32(bucket, end);
                packet = end + 8;
            }
            else
                packet += bytes;
        }
    next_descriptor:
        descriptor += flags >> 16;
        if (--count)
            continue;
        count = r_u32(scratch + 0x1F4);
        if (!count)
            break;
        descriptor = 0x80082FAC;
        w_u32(scratch + 0x1F4, 0);
    }
    w_u32(0x800FF668, packet);
    return;
}

extern "C" void sub_800817FC(uint32 descriptor, uint32 unused, uint32 count)
{
    FUNCTION_MARKER(0x800817FC, "SLUS_003.73");
    BuildModelPolygonPackets(descriptor, count);
}

extern "C" uint32 sub_800878AC(uint32 matrix, uint32 position)
{
    FUNCTION_MARKER(0x800878AC, "SLUS_003.73");
    uint32 x = r_u32(position);
    uint32 y = r_u32(position + 4);
    uint32 z = r_u32(position + 8);
    w_u32(matrix + 20, x);
    w_u32(matrix + 24, y);
    w_u32(matrix + 28, z);
    return matrix;
}

extern "C" void sub_800854D8(uint32 matrix)
{
    FUNCTION_MARKER(0x800854D8, "SLUS_003.73");
    InitializeModelIdentityMatrix((uint32 *)psx_addr(matrix, 20));
}
