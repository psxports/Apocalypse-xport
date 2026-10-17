#include "model_render_view.h"
#include "character.h"
#include "model_resources.h"
#include "effect_lifecycle.h"
#include "model_render.h"
#include "model_lighting.h"
#include "model_projection.h"
#include "model_animation.h"
#include "model_shadow.h"
#include "model_clipping.h"
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

static const uint32 polygon_material_mask = 0xFFFF0000;

struct ModelClipPlane
{
    sint16 nx, ny, nz, offset;
};

static ModelClipPlane model_clip_planes[8];
static MATRIX model_clip_matrix[2];

static uint32 PackClipPlaneHalves(sint16 low, sint16 high)
{
    return (uint16)low | ((uint32)(uint16)high << 16);
}

static uint32 ProjectAndClipModelVertices(const SVECTOR *vertices, uint32 count);

static void RenderClippedModel(const ModelGeometry &geometry, uint32 translated)
{
    uint32 count, x, y, z;
    count = geometry.vertex_count;
    w_u32(0x800FFA9C, translated);
    if (ProjectModelClippedVertices(geometry.vertices, count) & 191)
        return;
    sub_8008793C(0x800EE710);
    x = (uint32)(sint32)(sint16)r_u16(0x800FFA8C);
    y = (uint32)(sint32)(sint16)r_u16(0x800FFA8E);
    z = (uint32)(sint32)(sint16)r_u16(0x800FFA90);
    sub_800879AC(x, y, z);
    x = (uint32)(sint32)(sint16)r_u16(0x800FFA94);
    y = (uint32)(sint32)(sint16)r_u16(0x800FFA96);
    z = (uint32)(sint32)(sint16)r_u16(0x800FFA98);
    sub_8008798C(x, y, z);
    BuildModelPolygonPackets(geometry.polys, geometry.poly_count);
}

void RenderTranslatedClippedModel(const ModelGeometry &geometry)
{
    FUNCTION_MARKER(0x8007FA7C, "SLUS_003.73");
    RenderClippedModel(geometry, 1);
}

void RenderScratchModel(const ModelGeometry &geometry)
{
    uint32 count, result;
    FUNCTION_MARKER(0x8007FBD4, "SLUS_003.73");
    count = geometry.vertex_count;
    w_u32(0x800FFA9C, 1);
    result = ProjectModelScratchVertices(geometry.vertices, count);
    if (r_u32(0x800FFB38))
        return;
    if (result & 0x60006000)
        return;
    EmitScratchModelPackets(geometry.polys, geometry.poly_count);
}

void RenderUntranslatedClippedModel(const ModelGeometry &geometry)
{
    FUNCTION_MARKER(0x8007FD3C, "SLUS_003.73");
    RenderClippedModel(geometry, 0);
}

// TODO Classify original render V0 consumers beyond the current void render callers




extern "C" void nullsub_18(void)
{
    FUNCTION_MARKER(0x8006FAE4, "SLUS_003.73");
}

void RenderProjectedModel(const ModelGeometry &model)
{
    uint32 count, flags, far_x, far_y, far_z;
    FUNCTION_MARKER(0x8007F904, "SLUS_003.73");
    count = model.vertex_count;
    w_u32(0x800FFA9C, 1);
    flags = ProjectAndClipModelVertices(model.vertices, count);
    if (r_u32(0x800FFB3C))
        UpdateProjectedVertexDepthFlags(count);
    if (flags & 191)
        return;
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
    BuildModelPolygonPackets(model.polys, model.poly_count);
}



void RenderOffsetScratchModel(const ModelGeometry &model, const sint32 position[3])
{
    uint32 count, flags, red, green, blue;
    FUNCTION_MARKER(0x8007F9E0, "SLUS_003.73");
    count = model.vertex_count;
    w_u32(0x800FFA9C, 1);
    flags = ProjectOffsetScratchVertices(model.vertices, count, position);
    if (flags & 0x60006000)
        return;
    red = (uint32)(sint32)(sint16)r_u16(0x800FFA8C);
    green = (uint32)(sint32)(sint16)r_u16(0x800FFA8E);
    blue = (uint32)(sint32)(sint16)r_u16(0x800FFA90);
    sub_800879AC(red, green, blue);
    EmitScratchModelPackets(model.polys, model.poly_count);
}

// TODO Classify V0 consumers beyond the current callers that discard the render result



static uint32 ReadModelVertexWord(const uint32 *vertices, uint32 address, uint32 offset)
{
    if (address)
        return r_u32(address + offset);
    uint32 value;
    memcpy(&value, (const uint8 *)vertices + offset, 4);
    return value;
}

static uint32 ProjectAndClipModelVertices(const SVECTOR *vertices, uint32 count)
{
    uint32 bounds_source, packed, bounds[6], output, transformed, aggregate = 0xFFFF;
    uint32 vertex_xy, vertex_z, next_xy, next_z, screen, clip;
    sint32 x, y, z;
    FUNCTION_MARKER(0x80080D84, "SLUS_003.73");
    if (!count)
        return 0xFF;
    uint32 offset = 0;
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
    vertex_xy = xport_load_le32((const uint8 *)vertices + offset);
    vertex_z = xport_load_le32((const uint8 *)vertices + offset + 4);
    do
    {
        xport_gte_write_data(0, vertex_xy);
        xport_gte_write_data(1, vertex_z);
        offset += 8;
        --count;
        xport_gte_execute(0x180001);
        // Original prefetch reads the following vertex even on the final iteration
        next_xy = count ? xport_load_le32((const uint8 *)vertices + offset) : 0;
        next_z = count ? xport_load_le32((const uint8 *)vertices + offset + 4) : 0;
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

void RenderModelObjectList(ModelInstance *object)
{
    ModelBorrow borrow;
    const ModelProjectionConfig &proj_cfg = GetModelProjectionConfig();
    FUNCTION_MARKER(0x8007F138, "SLUS_003.73");
    ModelRenderView *matrix;
    uint32 flags, color, render_flags, index, packed_angles;
    MATRIX rotation, combined;
    if (!object)
        return;
    w_u32(0x1F800164, 0x7FFFFFFF);
    do
    {
        flags = object->flags;
        if (!(flags & 0x8001) && GetModelBank(object->bank).loaded)
        {
            render_flags = polygon_material_mask;
            if (flags & 0x400)
            {
                render_flags = (render_flags & 0xFFFB0000) | 8;
                color = object->color;
                w_u32(0x800FFB20, color & 0xFFFFFF);
                w_u32(0x1F800154, color & 0xFFFFFF);
                w_u32(0x1F8001A4, (color & 255) << 6);
                w_u32(0x1F8001B4, (color & 0xFF00) >> 2);
                w_u32(0x1F8001C4, ((color & 0xFFFFFF) >> 10) & 0x3FC0);
            }
            if (object->flags & 0x800)
                render_flags = (render_flags & 0xFFFFFE7F) | (object->render_mode & 0x180) | 0x40;
            matrix = &GetConfiguredModelRenderView();
            w_u32(0x1F800144, render_flags);
            SetRotMatrix((MATRIX *)&matrix->projection);
            if (((uint32)object->angles[0] | ((uint32)object->angles[1] << 16)) || object->angles[2])
            {
                BuildModelRotationMatrix(object->angles, (uint16 *)&rotation);
                TransformPackedModelMatrix((const uint32 *)&rotation, (uint32 *)&combined);
                SetRotMatrix(&combined);
            }
            else
                InitializeModelIdentityMatrix((uint32 *)&rotation);
            SelectModelPalette(GetModelBank(object->bank).colors);
            index = object->model_idx;

            packed_angles = ((uint32)object->angles[0] | ((uint32)object->angles[1] << 16));
            const ModelGeometry &geometry = GetModelGeometry(object->bank, index);
            if (packed_angles || object->angles[2])
                RenderTranslatedClippedModel(geometry);
            else
                RenderUntranslatedClippedModel(geometry);
        }
        object = object->next;
    } while (object);
    w_u32(0x1F800164, proj_cfg.fog_start);
}


extern "C" uint32 xport_draft_host_sub_800854D8_p1(void *output)
{
    FUNCTION_MARKER(0x800854D8, "SLUS_003.73");
    InitializeModelIdentityMatrix((uint32 *)output);
    return 0;
}

void RenderTranslatedModel(const ModelGeometry &geometry)
{
    FUNCTION_MARKER(0x8007FB34, "SLUS_003.73");
    uint32 count = geometry.vertex_count;
    w_u32(0x800FFA9C, 1);
    uint32 flags = ProjectModelReferenceVertices(geometry.vertices, count);
    if (r_u32(0x800FFB3C))
        UpdateProjectedVertexDepthFlags(count);
    if (!r_u32(0x800FFB38) && !(flags & 191))
    {
        BuildModelPolygonPackets(geometry.polys, geometry.poly_count);
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







/* Native helper for the post-projection portion of the original owner */
static void RenderOffsetModelPolygons(const ModelGeometry &model, uint32 count, uint32 flags)
{
    uint32 bank[5], index;
    if (r_u32(0x800FFB3C))
        UpdateProjectedVertexDepthFlags(count);
    flags &= 0xBF;
    if (flags)
        return;
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
        (void)(BuildModelPolygonPackets(model.polys, model.poly_count));
        return;
    }
}

void RenderOffsetModel(const ModelGeometry &model, const sint32 vector[3])
{
    uint32 count = model.vertex_count, flags;
    FUNCTION_MARKER(0x8007FC60, "SLUS_003.73");
    w_u32(0x800FFA9C, 0);
    flags = ProjectOffsetModelVertices(model.vertices, count, vector);
    RenderOffsetModelPolygons(model, count, flags);
}






static uint32 PackModelPolygonCommand(uint32 packet, uint32 color, uint32 command, uint32 flags)
{
    FUNCTION_MARKER(0x80082188, "SLUS_003.73");
    w_u32(packet + 4, color | command | ((flags & 0x40) << 19));
    return packet;
}

static uint32 ReadModelTableColor(uint32 index, const ModelPalette *table)
{
    FUNCTION_MARKER(0x800821A4, "SLUS_003.73");
    return ReadModelPaletteColor(table, (index & 0x3FC) >> 2);
}

static void LoadModelTableColor(uint32 index, const ModelPalette *table, uint32 ir1, uint32 ir2, uint32 ir3)
{
    uint32 color;
    FUNCTION_MARKER(0x80082200, "SLUS_003.73");
    color = ReadModelPaletteColor(table, (index & 0x3FC) >> 2);
    xport_gte_write_data(9, ir1);
    xport_gte_write_data(10, ir2);
    xport_gte_write_data(11, ir3);
    xport_gte_write_data(6, color);
}


static const sint16 model_uv_waves[16][64] =
{
#include "model_uv_waves.inc"
};

static uint32 ReadModelUvWave(uint8 parameter, uint32 phase)
{
    const uint32 amplitude = parameter >> 4;
    if (!amplitude)
        return 0;
    phase = (phase + ((parameter & 15) << 2)) & 63;
    return (uint32)(sint32)model_uv_waves[amplitude][phase];
}

static uint32 ReadHorizontalModelUvOffset(const ModelUvVertex &coordinates, uint32 phase)
{
    FUNCTION_MARKER(0x8007FED0, "SLUS_003.73");
    return ReadModelUvWave(coordinates.wave_x, phase);
}

static uint32 ReadVerticalModelUvOffset(const ModelUvVertex &coordinates, uint32 phase)
{
    FUNCTION_MARKER(0x8007FF20, "SLUS_003.73");
    return ReadModelUvWave(coordinates.wave_y, phase);
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

void UpdateModelTextureCoordinates(uint32 bank_index)
{
    FUNCTION_MARKER(0x8007FF70, "SLUS_003.73");
    if (bank_index == 0xFFFFFFFF)
        return;
    const ModelBank &source_bank = GetModelBank(bank_index);
    const ModelTextureAnimation &animation = source_bank.texture_animation;
    for (uint32 row = 0; row < animation.count; ++row)
    {
        const ModelTextureTrack &track = animation.tracks[row];
        const ModelInstance &object = source_bank.scene.objects[track.object_index];
        if (!(object.flags & 0x8000))
        {
            uint32 speed_x = (uint32)(sint32)track.scroll_x;
            uint32 tick = r_u32(0x800FFB40);
            uint32 scroll_x = (uint32)((sint32)(tick * speed_x) >> 4);
            uint32 phase_product = tick * track.phase_speed;
            uint32 bank = object.bank;
            uint32 index = object.model_idx;
            ModelGeometry &geometry = GetModelGeometry(bank, index);
            uint32 y_product = tick * (uint32)(sint32)track.scroll_y;
            uint32 item = 0;
            uint32 scroll_y;
            const uint32 phase = (uint32)((sint32)phase_product >> 10);
            scroll_y = (uint32)((sint32)y_product >> 4);
            while ((sint32)item < (sint32)track.quad_count)
            {
                uint32 x[4], y[4], vertex, texture, wrap_x, wrap_y;
                if (item >= geometry.poly_count)
                {
                    GameDiagnostic("Model UV animation exceeds polygon count\n");
                    abort();
                }
                ModelPolygon &poly = geometry.polys[item];
                const ModelUvQuad &quad = track.quads[item];
                for (vertex = 0; vertex < 4; ++vertex)
                {
                    x[vertex] = (uint32)quad.vertices[vertex].u << 8;
                    y[vertex] = (uint32)quad.vertices[vertex].v << 8;
                }
                texture = GetModelPolygonWindow(poly);
                wrap_x = (0 - ((texture & 0x1F) << 11)) & 0xF800;
                wrap_y = (0 - ((texture & 0x3E0) << 6)) & 0xFF00;
                for (vertex = 0; vertex < 4; ++vertex)
                    x[vertex] += scroll_x;
                for (vertex = 0; vertex < 4; ++vertex)
                    y[vertex] += scroll_y;
                for (vertex = 0; vertex < 4; ++vertex)
                {
                    x[vertex] += ReadHorizontalModelUvOffset(quad.vertices[vertex], phase);
                    y[vertex] += ReadVerticalModelUvOffset(quad.vertices[vertex], phase);
                }
                WrapModelTextureCoordinates(x, wrap_x);
                WrapModelTextureCoordinates(y, wrap_y);
                ++item;
                poly.uv[0].uv = (uint16)(((x[0] & 0xFF00) >> 8) | (y[0] & 0xFF00));
                poly.uv[1].uv = (uint16)(((x[1] & 0xFF00) >> 8) | (y[1] & 0xFF00));
                poly.uv[2].uv = (uint16)(((x[2] & 0xFF00) >> 8) | (y[2] & 0xFF00));
                poly.uv[3].uv = (uint16)(((x[3] & 0xFF00) >> 8) | (y[3] & 0xFF00));
            }
        }
    }
    return;
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




static uint32 DivideRenderViewScalar(uint32 numerator, uint32 denominator)
{
    sint32 left = (sint32)numerator, right = (sint32)denominator;
    if (!right)
        return left < 0 ? 1 : 0xFFFFFFFF;
    if (numerator == 0x80000000 && denominator == 0xFFFFFFFF)
        return numerator;
    return (uint32)(left / right);
}

static void RotateRenderViewNormal(const MATRIX &matrix, const ModelClipPlane &input, sint16 output[3])
{
    uint32 words[5], axis, result[3];
    for (axis = 0; axis < 5; ++axis)
        words[axis] = ReadModelRenderMatrixWord(matrix, axis);
    for (axis = 0; axis < 5; ++axis)
        xport_gte_write_control(axis, words[axis]);
    xport_gte_write_data(0, PackClipPlaneHalves(input.nx, input.ny));
    xport_gte_write_data(1, PackClipPlaneHalves(input.nz, input.offset));
    xport_gte_execute(0x486012);
    for (axis = 0; axis < 3; ++axis)
        result[axis] = xport_gte_read_data(9 + axis);
    for (axis = 0; axis < 3; ++axis)
        output[axis] = (sint16)result[axis];
}

static void TransposeRenderViewMatrix(const MATRIX &input, MATRIX &output)
{
    TransposeRotationMatrix((const uint16 *)&input, (uint16 *)&output);
}

static void ConfigureModelClipPlanes(ModelRenderView &camera, uint32 bounds)
{
    uint32 depth, difference, length, axis, result, first, second;
    model_clip_planes[0].nx = (sint16)(0);
    model_clip_planes[0].ny = (sint16)(0);
    model_clip_planes[0].nz = (sint16)(0xF000);
    result = r_u16(bounds + 10);
    model_clip_planes[1].nx = (sint16)(0);
    model_clip_planes[1].ny = (sint16)(0);
    model_clip_planes[1].nz = (sint16)(0x1000);
    model_clip_planes[0].offset = (sint16)(result);
    model_clip_planes[1].offset = (sint16)(0 - r_u16(bounds + 8));

    first = r_u16(bounds + 14);
    second = r_u16(bounds + 6);
    difference = first - second;
    depth = r_u16(bounds + 16);
    length = sub_80085B54(difference * difference + depth * depth);
    model_clip_planes[2].nx = (sint16)(0);
    model_clip_planes[2].ny = (sint16)(DivideRenderViewScalar(r_u16(bounds + 16) << 12, length));
    first = r_u16(bounds + 6);
    second = r_u16(bounds + 14);
    difference = first - second;
    model_clip_planes[2].offset = (sint16)(0);
    model_clip_planes[2].nz = (sint16)(DivideRenderViewScalar(0 - (difference << 12), length));

    first = r_u16(bounds + 2);
    second = r_u16(bounds + 14);
    difference = first - second;
    depth = r_u16(bounds + 16);
    length = sub_80085B54(difference * difference + depth * depth);
    model_clip_planes[3].nx = (sint16)(0);
    model_clip_planes[3].ny = (sint16)(DivideRenderViewScalar(0 - (r_u16(bounds + 16) << 12), length));
    first = r_u16(bounds + 14);
    second = r_u16(bounds + 2);
    difference = first - second;
    model_clip_planes[3].offset = (sint16)(0);
    model_clip_planes[3].nz = (sint16)(DivideRenderViewScalar(0 - (difference << 12), length));

    first = r_u16(bounds + 12);
    second = r_u16(bounds + 4);
    difference = first - second;
    depth = r_u16(bounds + 16);
    length = sub_80085B54(difference * 25 * difference + depth * (depth << 6));
    model_clip_planes[4].ny = (sint16)(0);
    model_clip_planes[4].nx = (sint16)(DivideRenderViewScalar(r_u16(bounds + 16) << 15, length));
    first = r_u16(bounds + 4);
    second = r_u16(bounds + 12);
    difference = first - second;
    model_clip_planes[4].offset = (sint16)(0);
    model_clip_planes[4].nz = (sint16)(DivideRenderViewScalar(0 - ((difference * 5) << 12), length));

    first = r_u16(bounds);
    second = r_u16(bounds + 12);
    difference = first - second;
    depth = r_u16(bounds + 16);
    length = sub_80085B54(difference * 25 * difference + depth * (depth << 6));
    model_clip_planes[5].ny = (sint16)(0);
    model_clip_planes[5].nx = (sint16)(DivideRenderViewScalar(0 - (r_u16(bounds + 16) << 15), length));
    first = r_u16(bounds + 12);
    second = r_u16(bounds);
    difference = first - second;
    model_clip_planes[5].offset = (sint16)(0);
    model_clip_planes[5].nz = (sint16)(DivideRenderViewScalar(0 - ((difference * 5) << 12), length));

    first = r_u16(bounds + 12);
    second = r_u16(bounds + 4);
    difference = first - second;
    depth = r_u16(bounds + 16);
    length = sub_80085B54(difference * difference + depth * depth);
    model_clip_planes[6].ny = (sint16)(0);
    model_clip_planes[6].nx = (sint16)(DivideRenderViewScalar(r_u16(bounds + 16) << 12, length));
    first = r_u16(bounds + 4);
    second = r_u16(bounds + 12);
    difference = first - second;
    model_clip_planes[6].offset = (sint16)(0);
    model_clip_planes[6].nz = (sint16)(DivideRenderViewScalar(0 - (difference << 12), length));

    first = r_u16(bounds);
    second = r_u16(bounds + 12);
    difference = first - second;
    depth = r_u16(bounds + 16);
    length = sub_80085B54(difference * difference + depth * depth);
    model_clip_planes[7].ny = (sint16)(0);
    model_clip_planes[7].nx = (sint16)(DivideRenderViewScalar(0 - (r_u16(bounds + 16) << 12), length));
    first = r_u16(bounds + 12);
    second = r_u16(bounds);
    difference = first - second;
    model_clip_planes[7].offset = (sint16)(0);
    model_clip_planes[7].nz = (sint16)(DivideRenderViewScalar(0 - (difference << 12), length));
    for (axis = 0; axis < 3; ++axis)
    {
        RotateRenderViewNormal(camera.rotation, model_clip_planes[axis], model_clip_matrix[0].m[axis]);
        RotateRenderViewNormal(camera.rotation, model_clip_planes[axis + 3], model_clip_matrix[1].m[axis]);
    }
}

uint32 ConfigureModelRenderView(ModelRenderView &camera, uint32 bounds, uint32 ordering_table)
{
    const ModelProjectionConfig &proj_cfg = GetModelProjectionConfig();
    ModelRenderView *owner;
    uint32 depth, difference, axis, result;
    uint32 first, second, animation_bank, next_tick;
    FUNCTION_MARKER(0x8007E63C, "SLUS_003.73");
    ConfigureModelClipPlanes(camera, bounds);
    sub_800879EC(r_u16(bounds + 16));
    first = r_u16(bounds + 12);
    second = r_u16(bounds + 14);
    sub_800879CC(first, second);
    SetModelDrawArea(bounds, ordering_table);
    result = (uint16)proj_cfg.near_depth;
    depth = (uint16)proj_cfg.far_depth;
    SetConfiguredModelRenderView(camera);
    w_u32(0x800FFB08, bounds);
    w_u32(0x800FFB30, ordering_table);
    w_u16(bounds + 8, result);
    w_u16(bounds + 10, depth);
    TransposeRenderViewMatrix(camera.rotation, camera.inverse);
    for (axis = 0; axis < 3; ++axis)
    {
        sint32 scaled = 8 * (sint32)(sint16)((uint32)(uint16)camera.inverse.m[0][axis]);
        uint32 middle = ((uint32)(uint16)camera.inverse.m[1][axis]);
        uint32 last = ((uint32)(uint16)camera.inverse.m[2][axis]);
        camera.projection.m[1][axis] = (sint16)(uint16)(middle);
        camera.projection.m[2][axis] = (sint16)(uint16)(last);
        camera.projection.m[0][axis] = (sint16)(uint16)((uint32)(scaled / 5));
    }
    const TextureResource &texture = GetTextureResource(0xCCE0187C);
    animation_bank = GetActiveModelBank(0);
    result = (uint32)texture.clut << 16;
    depth = (uint32)texture.tpage << 16;
    difference = r_u32(0x800FFB40);
    w_u32(0x800FFB24, result);
    w_u32(0x800FFB28, depth);
    first = texture.uv[0].u;
    second = texture.uv[0].v;
    next_tick = r_u32(0x800FF650);
    result = first | (second << 8);
    w_u32(0x800FFAB4, difference);
    w_u32(0x800FFB40, next_tick);
    w_u32(0x800FFB2C, result);
    UpdateModelColorAnimation(animation_bank);
    UpdateModelColorAnimation(GetActiveModelBank(1));
    UpdateModelColorAnimation(r_u8(0x800FF644));
    UpdateModelTextureCoordinates(r_u8(0x800FF644));
    owner = &GetConfiguredModelRenderView();
    w_u16(0x800FFB10, 0 - ((uint32)(uint16)owner->rotation.m[1][0]));
    w_u16(0x800FFB12, 0 - ((uint32)(uint16)owner->rotation.m[1][1]));
    w_u16(0x800FFB14, 0 - ((uint32)(uint16)owner->rotation.m[1][2]));
    first = r_u32(0x800FF37C);
    second = ((uint32)(uint16)owner->pos[1]);
    result = (uint32)((sint32)first >> 12) - second;
    w_u16(0x800FFB16, result);
    return result;
}




uint32 AdvanceModelRenderCounter()
{
    uint32 counter;
    FUNCTION_MARKER(0x8007EBC4, "SLUS_003.73");
    counter = r_u32(0x800FFAA8) + 1;
    w_u32(0x800FFAA8, counter);
    return counter;
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

uint32 CullModelObjectList(ModelInstance *object)
{
    ModelBorrow borrow;
    FUNCTION_MARKER(0x80080560, "SLUS_003.73");
    if (!object)
        return 0x1F800000;
    uint32 light_corners[3], color_corners[3];
    SelectModelBoxCorners(8, light_corners);
    SelectModelBoxCorners(16, color_corners);
    ModelRenderView *camera = &GetConfiguredModelRenderView();
    uint32 camera_x = camera->pos[0];
    uint32 camera_y = camera->pos[1];
    uint32 camera_z = camera->pos[2];
    do
    {
        uint32 flags = object->flags;
        uint32 position_x = object->pos[0];
        uint32 position_y = object->pos[1];
        uint32 position_z = object->pos[2];
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
            uint32 geometry_index = object->model_idx;
            uint32 bank = object->bank;
            xport_gte_execute(0x4A2012);
            const ModelGeometry &geometry = GetModelGeometry(bank, geometry_index);
            uint32 radius = geometry.radius >> 13;
            if (flags & 512)
            {
                sint32 scale_x = (sint16)object->scale[0];
                sint32 scale_y = (sint16)object->scale[1];
                sint32 scale_z = (sint16)object->scale[2];
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
            uint32 rotation_xy = ((uint32)object->angles[0] | ((uint32)object->angles[1] << 16));
            uint32 rotation_z = object->angles[2];
            uint32 bounds_x = ((uint16)geometry.bounds[0].max | ((uint32)(uint16)geometry.bounds[0].min << 16));
            if (!(rotation_xy | rotation_z | (flags & 514)))
            {
                uint32 bounds_y = ((uint16)geometry.bounds[1].max | ((uint32)(uint16)geometry.bounds[1].min << 16));
                uint32 bounds_z = ((uint16)geometry.bounds[2].max | ((uint32)(uint16)geometry.bounds[2].min << 16));
                WriteModelBoxAxis(0, x, bounds_x);
                WriteModelBoxAxis(1, y, bounds_y);
                WriteModelBoxAxis(2, z, bounds_z);
                if (IsModelBoxOutside(light_corners, color_corners))
                    break;
            }
            outside = false;
        } while (false);
        ModelInstance *next = object->next;
        object->flags = outside ? flags | 32768 : flags & 32767;
        object = next;
    } while (object);
    return 0x1F800000;
}



static void LoadVisibleModelMatrix(uint32 address, uint32 bank)
{
    uint32 words[5];
    for (uint32 index = 0; index < 5; ++index)
        words[index] = r_u32(address + index * 4);
    for (uint32 index = 0; index < 5; ++index)
        xport_gte_write_control(bank + index, words[index]);
}

static void LoadVisibleModelMatrix(const MATRIX &matrix, uint32 bank)
{
    for (uint32 i = 0; i < 4; ++i)
        xport_gte_write_control(bank + i, PackClipPlaneHalves(matrix.m[i * 2 / 3][i * 2 % 3], matrix.m[(i * 2 + 1) / 3][(i * 2 + 1) % 3]));
    xport_gte_write_control(bank + 4, (uint16)matrix.m[2][2]);
}

void LoadModelLineClipMatrices()
{
    const ModelClipPlane *p = model_clip_planes;
    xport_gte_write_control(8, PackClipPlaneHalves(p[0].nx, p[0].ny));
    xport_gte_write_control(9, PackClipPlaneHalves(p[0].nz, p[1].nx));
    xport_gte_write_control(10, PackClipPlaneHalves(p[1].ny, p[1].nz));
    xport_gte_write_control(11, PackClipPlaneHalves(p[2].nx, p[2].ny));
    xport_gte_write_control(12, (uint16)p[2].nz);
    xport_gte_write_control(13, (uint32)(sint32)p[0].offset);
    xport_gte_write_control(14, (uint32)(sint32)p[1].offset);
    xport_gte_write_control(15, 0);
    xport_gte_write_control(16, PackClipPlaneHalves(p[3].nx, p[3].ny));
    xport_gte_write_control(17, PackClipPlaneHalves(p[3].nz, p[6].nx));
    xport_gte_write_control(18, PackClipPlaneHalves(p[6].ny, p[6].nz));
    xport_gte_write_control(19, PackClipPlaneHalves(p[7].nx, p[7].ny));
    xport_gte_write_control(20, (uint16)p[7].nz);
}

void RenderVisibleModelObjectList(ModelInstance *owner)
{
    ModelBorrow borrow;
    const ModelProjectionConfig &proj_cfg = GetModelProjectionConfig();
    FUNCTION_MARKER(0x8007EBDC, "SLUS_003.73");
    if (!owner)
        return;
    LoadVisibleModelMatrix(model_clip_matrix[0], 8);
    LoadVisibleModelMatrix(model_clip_matrix[1], 16);
    uint32 plane_x = (uint32)((sint32)model_clip_planes[0].offset >> 1);
    uint32 plane_y = (uint32)((sint32)model_clip_planes[1].offset >> 1);
    xport_gte_write_control(13, plane_x);
    xport_gte_write_control(14, plane_y);
    xport_gte_write_control(15, 0);
    CullModelObjectList(owner);
    if (owner == GetModelSceneObjects())
    {
        UpdateModelTextureCoordinates(GetActiveModelBank(0));
        UpdateModelTextureCoordinates(GetActiveModelBank(1));
    }
    while (owner)
    {
    ModelRenderView *camera;
    uint32 flags = owner->flags, mode, axis;
        const ModelGeometry *geometry;
        sint32 position[3];
        uint32 transformed[4], matrix[8], combined[8];
        uint32 render_limit, depth_shift, fog_scale, fog_bias, far_start, far_end;
        uint32 vertex_index, object_value, camera_value, geometry_flags;
        uint32 rotation_xy, rotation_z, attributes, attribute_end, vertex_count;
        uint32 packet_start, packet_end, secondary_flags, callback_flags;
        bool transform_matrix;
        if (flags & 0x8001)
            goto NextObject;
        if (!GetModelBank((uint32)owner->bank).loaded)
            goto NextObject;
        render_limit = proj_cfg.fog_start;
        mode = polygon_material_mask;
        depth_shift = proj_cfg.fog_shift;
        fog_scale = r_u32(0x800FFB24);
        fog_bias = r_u32(0x800FFB28);
        far_start = (uint32)(sint32)(sint16)r_u16(0x800FFAAC);
        far_end = (uint32)(sint32)(sint16)r_u16(0x800FFAAE);
        flags = owner->flags;
        w_u32(0x1F800164, render_limit);
        w_u32(0x1F800174, depth_shift);
        w_u32(0x1F800184, fog_scale);
        w_u32(0x1F800194, fog_bias);
        w_u32(0x1F800204, far_start);
        w_u32(0x1F800214, far_end);
        if (flags & 0x80)
        {
            mode = (flags & 0x1000) ? (mode & 0xFFF70000) : (mode | 12);
            if (owner->flags & 0x400)
            {
                uint32 color = owner->color;
                w_u32(0x800FFB20, color & 0xFFFFFF);
                w_u16(0x800FFA96, (color & 0xFF00) >> 2);
                uint32 red = r_u8(0x800FFB20);
                w_u16(0x800FFA98, ((color & 0xFFFFFF) >> 10) & 0x3FC0);
                w_u16(0x800FFA94, red << 6);
            }
        }
        else if (flags & 0x400)
        {
            uint32 color = owner->color;
            mode = (mode & 0xFFFB0000) | 8;
            w_u32(0x800FFB20, color & 0xFFFFFF);
            w_u32(0x1F800154, color & 0xFFFFFF);
            w_u32(0x1F8001A4, (color & 0xFF) << 6);
            w_u32(0x1F8001B4, (color & 0xFF00) >> 2);
            w_u32(0x1F8001C4, ((color & 0xFFFFFF) >> 10) & 0x3FC0);
        }
        if (owner->flags & 0x800)
            mode = (mode & 0xFFFFFE7F) | (owner->render_mode & 0x180) | 0x40;
        callback_flags = owner->flags;
        w_u32(0x1F800144, mode);
        if (callback_flags & 0x80)
            static_cast<VisualObject *>(owner->owner)->UpdateLighting();
        flags = owner->flags;
        secondary_flags = owner->flags;
        w_u32(0x800FFB3C, (flags >> 3) & 1);
        if (secondary_flags & 2)
        {
            packet_start = r_u32(0x800FF668);
            RenderPosedModelObject(*owner);
            if (owner->flags & 64)
            {
                packet_end = r_u32(0x800FF668);
                // Original hook discards object and packet range arguments
                (void)packet_start;
                (void)packet_end;
                nullsub_18();
            }
            goto NextObject;
        }
        vertex_index = owner->model_idx;
        geometry = &GetModelGeometry(owner->bank, vertex_index);
        if (geometry->flags & 0x20)
            goto NextObject;
        camera = &GetConfiguredModelRenderView();
        LoadVisibleModelMatrix(camera->projection, 0);
        for (axis = 0; axis < 3; ++axis)
            xport_gte_write_control(5 + axis, 0);
        camera = &GetConfiguredModelRenderView();
        for (axis = 0; axis < 3; ++axis)
        {
            object_value = owner->pos[axis];
            camera_value = camera->pos[axis];
            position[axis] = (sint32)((uint32)((sint32)object_value >> 12) - camera_value);
        }
        xport_gte_write_data(0, ((uint32)position[0] & 65535) | ((uint32)position[1] << 16));
        xport_gte_write_data(1, (uint32)position[2]);
        xport_gte_execute(0x486012);
        for (axis = 0; axis < 3; ++axis)
            transformed[axis] = xport_gte_read_data(25 + axis);
        rotation_xy = ((uint32)owner->angles[0] | ((uint32)owner->angles[1] << 16));
        w_u32(0x800FFB44, 0);
        rotation_z = rotation_xy ? 0 : owner->angles[2];
        transform_matrix = rotation_xy || rotation_z;
        if (transform_matrix)
            BuildModelRotationMatrix(owner->angles, (uint16 *)matrix);
        else
            InitializeModelIdentityMatrix(matrix);
        if (owner->flags & 512)
        {
            ScaleModelRotationMatrix(owner->scale, (uint16 *)matrix);
            transform_matrix = true;
        }
        if (transform_matrix)
        {
            TransformPackedModelMatrix(matrix, combined);
            SetRotMatrix((MATRIX *)combined);
        }
        geometry_flags = geometry->flags;
        SelectModelPalette(GetModelBank((uint32)owner->bank).colors);
        if (geometry_flags & 1)
            GenerateModelReflectionCoordinates(matrix, transformed, 0x800F3E70, *geometry);
        else if (owner->flags & 128)
        {
            attributes = geometry->n_count;
            attribute_end = geometry->poly_count;
            vertex_count = geometry->vertex_count;
            if (attributes != attribute_end)
                attributes = vertex_count;
            GenerateModelNormalColors(geometry->normals, attributes);
        }
        if (r_u32(0x800FFB44))
            RenderOffsetScratchModel(*geometry, position);
        else if (((uint32)owner->angles[0] | ((uint32)owner->angles[1] << 16)) || owner->angles[2] || (owner->flags & 0x200))
        {
            for (axis = 0; axis < 3; ++axis)
                xport_gte_write_control(5 + axis, transformed[axis]);
            RenderProjectedModel(*geometry);
        }
        else
            RenderOffsetModel(*geometry, position);
    NextObject:
        owner = owner->next;
    }
}



static void MultiplyPosedModelMatrix(uint32 left, const uint8 *right, void *output)
{
    MATRIX local;
    memset(&local, 0, sizeof(local));
    memcpy(&local, right, 20);
    MulMatrix0((MATRIX *)psx_addr(left, 20), &local, (MATRIX *)output);
}

uint32 RenderPosedModelObject(ModelInstance &instance)
{
    ModelBorrow borrow;
    FUNCTION_MARKER(0x8007F340, "SLUS_003.73");
    const uint32 object = NativeObjectGuestAddress(instance.owner);
    ModelRenderView *camera;
    uint32 flags = instance.flags, count;
    const uint8 *pose;
    uint32 index, mode, rgb = 0, uv0 = 0, uv1 = 0;
    const TextureGroup *animation = 0;
    sint32 animation_frame = 0;
    uint32 rotation[8], light[8], matrix[8], transformed[3], relative[3];
    uint32 far_x, far_y, far_z, object_value, camera_value;
    // The unlit branch uses the approved zero light matrix
    uint32 axis;
    w_u32(0x800FFB44, (flags & 0xC80) ? 0 : 1);
    camera = &GetConfiguredModelRenderView();
    w_u32(0x800FFB44, 0);
    SetRotMatrix((MATRIX *)&camera->projection);
    TransformPackedModelMatrix((const uint32 *)psx_addr(object + 324, 20), rotation);
    camera = &GetConfiguredModelRenderView();
    for (axis = 0; axis < 3; ++axis)
    {
        object_value = instance.pos[axis];
        camera_value = camera->pos[axis];
        relative[axis] = (uint32)((sint32)object_value >> 12) - camera_value;
    }
    for (axis = 0; axis < 3; ++axis)
        xport_gte_write_data(9 + axis, relative[axis]);
    xport_gte_execute(0x49E012);
    const ModelGeometrySet &models = GetModelBank(instance.bank).geometry;
    count = models.count;
    for (axis = 0; axis < 3; ++axis)
    {
        transformed[axis] = xport_gte_read_data(25 + axis);
        rotation[5 + axis] = transformed[axis];
    }
    if (flags & 4)
        pose = (const uint8 *)static_cast<Character *>(instance.owner)->GetModelPoses(0, count);
    else
    {
        const ModelAnimation &animation_data = GetModelAnimation(GetModelBank(instance.bank).animations, instance.animation);
        if (animation_data.interval)
        {
            UpdateModelAnimationData(object);
            pose = (const uint8 *)GetModelPoseWorkspace(count);
        }
        else
            pose = (const uint8 *)GetModelAnimationPoses(animation_data, instance.frame * count, count);
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
        MultiplyPosedModelMatrix(0x800EE6F0, (const uint8 *)psx_addr(object + 324, 20), light);
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
        animation = GetEffectResourceGroup(animation_index);
        if (!animation)
        {
            GameDiagnostic("Missing model effect texture group\n");
            abort();
        }
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
        divisor = (sint32)animation->count;
        remainder = divisor == 0 ? dividend : (dividend == (sint32)0x80000000 && divisor == -1 ? 0 : dividend % divisor);
        uv0 = r_u16(object + 180);
        uv1 = r_u16(object + 182);
        rgb = r_u32(object + 176) & 0xFFFFFF;
        animation_frame = remainder;
    }
    w_u32(0x800FFB18, r_u32(0x800FFB44) ? 0x1F8003F8 : r_u32(0x800FFAC0) + 7992);
    SelectModelPalette(GetModelBank((uint32)instance.bank).colors);
    for (index = 0; index < count; ++index, pose += 24)
    {
        uint32 effects;
        w_u32(0x800FFB38, (r_u32(object + 372) & ((uint32)1 << (index & 31))) != 0);
        const ModelGeometry &model = models.models[index];
        SetupModelGteMatrices(rotation, light, (const uint32 *)pose);
        if (model.flags & 1)
        {
            MultiplyPosedModelMatrix(object + 324, pose, matrix);
            GenerateModelReflectionCoordinates(matrix, transformed, 0x800F3E70, model);
        }
        else if (flags & 128)
        {
            uint32 attributes = model.n_count;
            uint32 packets = model.poly_count;
            uint32 vertices = model.vertex_count;
            if (attributes != packets)
                attributes = vertices;
            GenerateModelNormalColors(model.normals, attributes);
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
                translation[axis] = xport_load_le16(pose + 18 + axis * 2);
            for (axis = 0; axis < 3; ++axis)
                xport_gte_write_data(9 + axis, translation[axis]);
            LoadModelRotationAndTransformIr((const uint32 *)psx_addr(object + 324, 20));
            for (axis = 0; axis < 3; ++axis)
                translation[axis] = xport_gte_read_data(25 + axis);
            for (axis = 0; axis < 3; ++axis)
                matrix[5 + axis] = translation[axis] + (uint32)((sint32)instance.pos[axis] >> 12);
        }
        if ((mode & 0x7F) && (!r_u32(0x800FFB3C) || (sint32)matrix[6] < ((sint32)r_u32(0x800FF37C) >> 12) - 40))
            RenderModelShadow(model, matrix, rgb, uv0, (uint32)(sint32)(sint16)uv1, *GetTextureFrame(animation, animation_frame).tex);
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




static void ReadModelPolygonUvs(const ModelPolygon *descriptor, uint32 flags, uint32 normals, uint32 scratch, const uint32 offsets[4], uint32 vertices, uint32 uv[4])
{
    if (flags & 1)
    {
        uv[0] = PackModelPolygonUv(*descriptor, 0);
        uv[1] = PackModelPolygonUv(*descriptor, 1);
        uv[2] = PackModelPolygonUv(*descriptor, 2);
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
static uint32 StoreModelPolygonPacket(uint32 packet, const ModelPolygon *descriptor, uint32 flags, uint32 normals, uint32 scratch, const uint32 offsets[4], uint32 vertices, uint32 colored, uint32 textured, uint32 first_color)
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

void BuildModelPolygonPackets(const ModelPolygon *descriptor, uint32 count)
{
    FUNCTION_MARKER(0x800817FC, "SLUS_003.73");
    /* TODO Validate near-plane and subdivision carrier composition */
    const uint32 scratch = 0x1F800000;
    const uint32 normals = 0x800F3E70;
    uint32 packet, projection, ordering, limit;
    if (!count)
        return;
    ordering = r_u32(0x800FFB30);
    packet = r_u32(0x800FF668) & 0xFFFFFF;
    projection = r_u32(0x800FFAC0);
    const ModelPalette *table = GetCurrentModelPalette();
    limit = r_u32(0x800FF374) - 100;
    xport_gte_write_control(5, 0);
    xport_gte_write_control(6, 0);
    xport_gte_write_control(7, 0);
    xport_gte_write_control(28, 0x10000000);
    xport_gte_write_control(27, 0);
    ResetModelPolygonQueue();
    for (;;)
    {
        uint32 offsets[4], points[4], depths[4], flags, clipping, area, fourth_screen;
        uint32 packed = (((uint32)descriptor->v[0] << 3) | ((uint32)descriptor->v[1] << 19));
        uint32 packed2 = (((uint32)descriptor->v[2] << 3) | ((uint32)descriptor->v[3] << 19));
        uint32 mask = r_u32(scratch + 0x144);
        uint32 depth, fog = 0, fog_shift, vertices, colored, textured, tag;
        uint32 first_color = 0, normal[3], index[4];
        uint32 i, bucket, previous, bytes, chain_head = packet;
        offsets[0] = packed & 0xFFFF;
        offsets[1] = packed >> 16;
        offsets[2] = packed2 & 0xFFFF;
        offsets[3] = packed2 >> 16;
        flags = ((((uint32)descriptor->source_stride << 16) | descriptor->flags) & (uint32)((sint32)mask >> 16)) | (mask & 0xFFFF);
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
            QueueNearPlanePolygon(descriptor, scratch, projection, flags, offsets, depths);
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
            packed = descriptor->color;
            index[0] = packed << 2;
            index[1] = packed >> 6;
            index[2] = packed >> 14;
            index[3] = packed >> 22;
            if ((flags & 12) == 0)
            {
                if (vertices == 4)
                {
                    xport_gte_write_data(6, ReadModelPaletteColor(table, (index[0] & 0x3FC) >> 2));
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
                        xport_gte_write_data(20 + i, ReadModelPaletteColor(table, (index[i] & 0x3FC) >> 2));
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
                    ReadRendererNormal(normals + (((uint32)descriptor->n << 3) | ((uint32)descriptor->surface << 16)), normal);
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
            xport_gte_write_data(6, descriptor->color);
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
                    LoadPackedProjectionIr(normals + (((uint32)descriptor->n << 3) | ((uint32)descriptor->surface << 16)));
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
            uint32 end, window_command = GetModelPolygonWindow(*descriptor);
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
        descriptor = descriptor->next;
        if (--count)
            continue;
        descriptor = ReadQueuedModelPolygons(count);
        if (!count)
            break;

    }
    w_u32(0x800FF668, packet);
    return;
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


