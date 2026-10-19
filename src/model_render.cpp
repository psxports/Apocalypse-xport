#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "geometry_commands.h"
#include "vector_distance.h"
#include "scene_lifecycle.h"
#include "level_session.h"
#include "game_tick.h"
#include "game_lighting.h"
#include "record_scripts.h"
#include "model_render_view.h"
#include "character.h"
#include "model_resources.h"
#include "effect_lifecycle.h"
#include "model_render.h"
#include "model_matrix.h"
static void BuildModelPolygonPackets(const ModelPolygon *descriptor, uint32 count);
static uint32 RenderPosedModelObject(ModelInstance &instance);
#include "model_subdivision.h"
#include "model_lighting.h"
#include "model_projection.h"
#include "model_animation.h"
#include "model_shadow.h"
#include "model_clipping.h"
#include "angle_math.h"
#include "game_diagnostics.h"

static const ModelViewport initial_viewport = {512, 240, 0, 0, 10, 30000, 256, 120, 256};
ModelViewport model_viewport = {512, 240, 0, 0, 10, 30000, 256, 120, 256};
ModelRenderState model_render_state = {0};

void ResetModelRenderState()
{
    ModelRenderState empty = {0};
    model_render_state = empty;
    model_viewport = initial_viewport;
    ResetModelLightingValues();
    ResetScratchProjectionVertices();
    ResetModelProjectionVertices();
}

static const uint32 polygon_material_mask = 0xFFFF0000;
static MATRIX unlit_matrix = {0};

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

static void RenderClippedModel(const ModelGeometry &geometry, uint32 translated)
{
    uint32 count;
    count = geometry.vertex_count;
    model_render_state.translated = translated;
    if (ProjectModelClippedVertices(geometry.vertices, count) & 191)
        return;
    SetColorMatrix(&game_lighting.colors);
    SetFarColor(game_lighting.far_color.r, game_lighting.far_color.g, game_lighting.far_color.b);
    SetBackColor(game_lighting.background_color.r, game_lighting.background_color.g, game_lighting.background_color.b);
    BuildModelPolygonPackets(geometry.polys, geometry.poly_count);
}

static void RenderTranslatedClippedModel(const ModelGeometry &geometry)
{
    FUNCTION_MARKER(0x8007FA7C, "SLUS_003.73");
    RenderClippedModel(geometry, 1);
}

static void RenderScratchModel(const ModelGeometry &geometry)
{
    uint32 count, result;
    FUNCTION_MARKER(0x8007FBD4, "SLUS_003.73");
    count = geometry.vertex_count;
    model_render_state.translated = 1;
    result = ProjectModelScratchVertices(geometry.vertices, count);
    if (model_render_state.part_hidden)
        return;
    if (result & 0x60006000)
        return;
    EmitScratchModelPackets(geometry.polys, geometry.poly_count);
}

static void RenderUntranslatedClippedModel(const ModelGeometry &geometry)
{
    FUNCTION_MARKER(0x8007FD3C, "SLUS_003.73");
    RenderClippedModel(geometry, 0);
}

// TODO Classify original render V0 consumers beyond the current void render callers

static void RenderProjectedModel(const ModelGeometry &model)
{
    uint32 count, flags;
    FUNCTION_MARKER(0x8007F904, "SLUS_003.73");
    count = model.vertex_count;
    model_render_state.translated = 1;
    flags = ProjectAndClipModelVertices(model.vertices, count);
    if (model_render_state.water_clip_enabled)
        UpdateModelWaterClipFlags(count);
    if (flags & 191)
        return;
    SetLightMatrix(&unlit_matrix);
    SetColorMatrix(&game_lighting.colors);
    SetFarColor(game_lighting.far_color.r, game_lighting.far_color.g, game_lighting.far_color.b);
    SetBackColor(game_lighting.background_color.r, game_lighting.background_color.g, game_lighting.background_color.b);
    BuildModelPolygonPackets(model.polys, model.poly_count);
}

static void RenderOffsetScratchModel(const ModelGeometry &model, const sint32 position[3])
{
    uint32 count, flags;
    FUNCTION_MARKER(0x8007F9E0, "SLUS_003.73");
    count = model.vertex_count;
    model_render_state.translated = 1;
    flags = ProjectOffsetScratchVertices(model.vertices, count, position);
    if (flags & 0x60006000)
        return;
    SetFarColor(game_lighting.far_color.r, game_lighting.far_color.g, game_lighting.far_color.b);
    EmitScratchModelPackets(model.polys, model.poly_count);
}

// TODO Classify V0 consumers beyond the current callers that discard the render result

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
    model_render_state.draw.fog_start = 0x7FFFFFFF;
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

                model_render_state.draw.light.x = (color & 255) << 6;
                model_render_state.draw.light.y = (color & 0xFF00) >> 2;
                model_render_state.draw.light.z = ((color & 0xFFFFFF) >> 10) & 0x3FC0;
            }
            if (object->flags & 0x800)
                render_flags = (render_flags & 0xFFFFFE7F) | (object->render_mode & 0x180) | 0x40;
            matrix = &GetConfiguredModelRenderView();
            model_render_state.draw.mode = render_flags;
            SetRotMatrix((MATRIX *)&matrix->projection);
            if (((uint32)object->angles.words[0] | ((uint32)object->angles.words[1] << 16)) || object->angles.words[2])
            {
                BuildModelRotationMatrix(object->angles.words, rotation);
                TransformModelMatrix(rotation, combined);
                SetRotMatrix(&combined);
            }
            else
                InitializeModelIdentityMatrix(rotation);
            SelectModelPalette(GetModelBank(object->bank).colors);
            index = object->model_idx;

            packed_angles = ((uint32)object->angles.words[0] | ((uint32)object->angles.words[1] << 16));
            const ModelGeometry &geometry = GetModelGeometry(object->bank, index);
            if (packed_angles || object->angles.words[2])
                RenderTranslatedClippedModel(geometry);
            else
                RenderUntranslatedClippedModel(geometry);
        }
        object = object->next;
    } while (object);
    model_render_state.draw.fog_start = proj_cfg.fog_start;
}

static void RenderTranslatedModel(const ModelGeometry &geometry)
{
    FUNCTION_MARKER(0x8007FB34, "SLUS_003.73");
    uint32 count = geometry.vertex_count;
    model_render_state.translated = 1;
    uint32 flags = ProjectModelReferenceVertices(geometry.vertices, count);
    if (model_render_state.water_clip_enabled)
        UpdateModelWaterClipFlags(count);
    if (!model_render_state.part_hidden && !(flags & 191))
    {
        BuildModelPolygonPackets(geometry.polys, geometry.poly_count);
    }
}

/* Native helper for the post-projection portion of the original owner */
static void RenderOffsetModelPolygons(const ModelGeometry &model, uint32 count, uint32 flags)
{
    if (model_render_state.water_clip_enabled)
        UpdateModelWaterClipFlags(count);
    flags &= 0xBF;
    if (flags)
        return;
    SetLightMatrix(&game_lighting.directions);
    SetColorMatrix(&game_lighting.colors);
    SetFarColor(game_lighting.far_color.r, game_lighting.far_color.g, game_lighting.far_color.b);
    SetBackColor(game_lighting.background_color.r, game_lighting.background_color.g, game_lighting.background_color.b);
    BuildModelPolygonPackets(model.polys, model.poly_count);
}

static void RenderOffsetModel(const ModelGeometry &model, const sint32 vector[3])
{
    uint32 count = model.vertex_count, flags;
    FUNCTION_MARKER(0x8007FC60, "SLUS_003.73");
    model_render_state.translated = 0;
    flags = ProjectOffsetModelVertices(model.vertices, count, vector);
    RenderOffsetModelPolygons(model, count, flags);
}

static void PackModelPolygonCommand(uint8 *packet, uint32 color, uint32 command, uint32 flags)
{
    FUNCTION_MARKER(0x80082188, "SLUS_003.73");
    P_TAG *header = (P_TAG *)packet;
    header->rgbc.rgbc = color | command | ((flags & 0x40) << 19);
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

static const sint16 model_uv_waves[16][64] = {
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

static void UpdateModelTextureCoordinates(uint32 bank_index)
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
            uint32 tick = model_render_state.frame;
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

static void SetModelDrawArea(const ModelViewport &bounds, uint32 *ordering_table)
{
    PSX_RECT rectangle;
    uint32 top, offset, right, left, bottom;
    uint8 *packet, *next_packet;
    FUNCTION_MARKER(0x8007E57C, "SLUS_003.73");
    rectangle.x = (sint16)bounds.left;
    top = bounds.top;
    offset = (uint16)scene_render.cur->draw.clip.y;
    rectangle.y = (sint16)(top + offset);
    right = bounds.right;
    left = bounds.left;
    packet = scene_render.cursor;
    rectangle.w = (sint16)(right - left);
    bottom = bounds.bottom;
    top = bounds.top;
    rectangle.h = (sint16)(bottom - top);
    DR_AREA *draw_area = (DR_AREA *)packet;
    SetDrawArea(draw_area, &rectangle);
    next_packet = packet + sizeof(DR_AREA);
    setaddr(draw_area, getaddr(ordering_table + 4095));
    setaddr(ordering_table + 4095, gpu_packet_link((uintptr_t)draw_area));
    scene_render.cursor = next_packet;
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
    uint32 words[5], axis;
    Vector3 result;
    for (axis = 0; axis < 5; ++axis)
        words[axis] = ReadModelRenderMatrixWord(matrix, axis);
    for (axis = 0; axis < 5; ++axis)
        xport_gte_write_control(axis, words[axis]);
    xport_gte_write_data(0, PackClipPlaneHalves(input.nx, input.ny));
    xport_gte_write_data(1, PackClipPlaneHalves(input.nz, input.offset));
    xport_gte_execute(0x486012);
    for (axis = 0; axis < 3; ++axis)
        result.words[axis] = xport_gte_read_data(9 + axis);
    for (axis = 0; axis < 3; ++axis)
        output[axis] = (sint16)result.words[axis];
}

static void ConfigureModelClipPlanes(ModelRenderView &camera, const ModelViewport &bounds)
{
    uint32 depth, difference, length, axis, result, first, second;
    model_clip_planes[0].nx = (sint16)(0);
    model_clip_planes[0].ny = (sint16)(0);
    model_clip_planes[0].nz = (sint16)(0xF000);
    result = bounds.far_depth;
    model_clip_planes[1].nx = (sint16)(0);
    model_clip_planes[1].ny = (sint16)(0);
    model_clip_planes[1].nz = (sint16)(0x1000);
    model_clip_planes[0].offset = (sint16)(result);
    model_clip_planes[1].offset = (sint16)(0 - bounds.near_depth);

    first = bounds.center_y;
    second = bounds.top;
    difference = first - second;
    depth = bounds.distance;
    length = LookupDistanceRoot(difference * difference + depth * depth);
    model_clip_planes[2].nx = (sint16)(0);
    model_clip_planes[2].ny = (sint16)(DivideRenderViewScalar(bounds.distance << 12, length));
    first = bounds.top;
    second = bounds.center_y;
    difference = first - second;
    model_clip_planes[2].offset = (sint16)(0);
    model_clip_planes[2].nz = (sint16)(DivideRenderViewScalar(0 - (difference << 12), length));

    first = bounds.bottom;
    second = bounds.center_y;
    difference = first - second;
    depth = bounds.distance;
    length = LookupDistanceRoot(difference * difference + depth * depth);
    model_clip_planes[3].nx = (sint16)(0);
    model_clip_planes[3].ny = (sint16)(DivideRenderViewScalar(0 - (bounds.distance << 12), length));
    first = bounds.center_y;
    second = bounds.bottom;
    difference = first - second;
    model_clip_planes[3].offset = (sint16)(0);
    model_clip_planes[3].nz = (sint16)(DivideRenderViewScalar(0 - (difference << 12), length));

    first = bounds.center_x;
    second = bounds.left;
    difference = first - second;
    depth = bounds.distance;
    length = LookupDistanceRoot(difference * 25 * difference + depth * (depth << 6));
    model_clip_planes[4].ny = (sint16)(0);
    model_clip_planes[4].nx = (sint16)(DivideRenderViewScalar(bounds.distance << 15, length));
    first = bounds.left;
    second = bounds.center_x;
    difference = first - second;
    model_clip_planes[4].offset = (sint16)(0);
    model_clip_planes[4].nz = (sint16)(DivideRenderViewScalar(0 - ((difference * 5) << 12), length));

    first = bounds.right;
    second = bounds.center_x;
    difference = first - second;
    depth = bounds.distance;
    length = LookupDistanceRoot(difference * 25 * difference + depth * (depth << 6));
    model_clip_planes[5].ny = (sint16)(0);
    model_clip_planes[5].nx = (sint16)(DivideRenderViewScalar(0 - (bounds.distance << 15), length));
    first = bounds.center_x;
    second = bounds.right;
    difference = first - second;
    model_clip_planes[5].offset = (sint16)(0);
    model_clip_planes[5].nz = (sint16)(DivideRenderViewScalar(0 - ((difference * 5) << 12), length));

    first = bounds.center_x;
    second = bounds.left;
    difference = first - second;
    depth = bounds.distance;
    length = LookupDistanceRoot(difference * difference + depth * depth);
    model_clip_planes[6].ny = (sint16)(0);
    model_clip_planes[6].nx = (sint16)(DivideRenderViewScalar(bounds.distance << 12, length));
    first = bounds.left;
    second = bounds.center_x;
    difference = first - second;
    model_clip_planes[6].offset = (sint16)(0);
    model_clip_planes[6].nz = (sint16)(DivideRenderViewScalar(0 - (difference << 12), length));

    first = bounds.right;
    second = bounds.center_x;
    difference = first - second;
    depth = bounds.distance;
    length = LookupDistanceRoot(difference * difference + depth * depth);
    model_clip_planes[7].ny = (sint16)(0);
    model_clip_planes[7].nx = (sint16)(DivideRenderViewScalar(0 - (bounds.distance << 12), length));
    first = bounds.center_x;
    second = bounds.right;
    difference = first - second;
    model_clip_planes[7].offset = (sint16)(0);
    model_clip_planes[7].nz = (sint16)(DivideRenderViewScalar(0 - (difference << 12), length));
    for (axis = 0; axis < 3; ++axis)
    {
        RotateRenderViewNormal(camera.rotation, model_clip_planes[axis], model_clip_matrix[0].m[axis]);
        RotateRenderViewNormal(camera.rotation, model_clip_planes[axis + 3], model_clip_matrix[1].m[axis]);
    }
}

uint32 ConfigureModelRenderView(ModelRenderView &camera, ModelViewport &bounds, uint32 *ordering_table)
{
    const ModelProjectionConfig &proj_cfg = GetModelProjectionConfig();
    ModelRenderView *owner;
    uint32 depth, difference, axis, result;
    uint32 first, second, animation_bank, next_tick;
    FUNCTION_MARKER(0x8007E63C, "SLUS_003.73");
    ConfigureModelClipPlanes(camera, bounds);
    SetGeomScreen(bounds.distance);
    first = bounds.center_x;
    second = bounds.center_y;
    SetGeomOffset(first, second);
    SetModelDrawArea(bounds, ordering_table);
    result = (uint16)proj_cfg.near_depth;
    depth = (uint16)proj_cfg.far_depth;
    SetConfiguredModelRenderView(camera);
    model_render_state.viewport = &bounds;
    model_render_state.ordering_table = ordering_table;
    bounds.near_depth = result;
    bounds.far_depth = depth;
    TransposeRotationMatrix(camera.rotation, camera.inverse);
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
    difference = model_render_state.frame;
    model_render_state.reflection_clut = result;
    model_render_state.reflection_tpage = depth;
    first = texture.uv[0].u;
    second = texture.uv[0].v;
    next_tick = game_clock.active_frame;
    result = first | (second << 8);
    model_render_state.previous_frame = difference;
    model_render_state.frame = next_tick;
    model_render_state.reflection_uv = result;
    UpdateModelColorAnimation(animation_bank);
    UpdateModelColorAnimation(GetActiveModelBank(1));
    UpdateModelColorAnimation(GetRecordModelBankSlot());
    UpdateModelTextureCoordinates(GetRecordModelBankSlot());
    owner = &GetConfiguredModelRenderView();
    model_render_state.water_plane.normal.x = -owner->rotation.m[1][0];
    model_render_state.water_plane.normal.y = -owner->rotation.m[1][1];
    model_render_state.water_plane.normal.z = -owner->rotation.m[1][2];
    first = (uint32)level_environment.water_y;
    second = ((uint32)(uint16)owner->pos.words[1]);
    result = (uint32)((sint32)first >> 12) - second;
    model_render_state.water_plane.distance = result;
    return result;
}

uint32 AdvanceModelRenderCounter()
{
    uint32 counter;
    FUNCTION_MARKER(0x8007EBC4, "SLUS_003.73");
    counter = model_render_state.pass_count + 1;
    model_render_state.pass_count = counter;
    return counter;
}

static void SelectModelBoxCorners(uint32 bank, uint32 corners[3])
{
    uint32 words[5];
    for (uint32 index = 0; index < 5; ++index)
        words[index] = ReadGeometryControl(bank + index);
    corners[0] = ((words[0] >> 15) & 1) | ((words[0] >> 30) & 2) | ((words[1] >> 13) & 4);
    corners[1] = ((words[1] >> 31) & 1) | ((words[2] >> 14) & 2) | ((words[2] >> 29) & 4);
    corners[2] = ((words[3] >> 15) & 1) | ((words[3] >> 30) & 2) | ((words[4] >> 13) & 4);
}

static void WriteModelBoxAxis(Vector3 corners[8], uint32 axis, sint32 position, uint32 bounds)
{
    sint32 lower = (sint16)((uint32)position + bounds) >> 1;
    sint32 upper = (sint32)((uint32)position + (uint32)((sint32)bounds >> 16)) >> 1;
    for (uint32 index = 0; index < 8; ++index)
    {
        sint16 value = index & (1 << axis) ? upper : lower;
        if (axis == 0)
            corners[index].x = value;
        else if (axis == 1)
            corners[index].y = value;
        else
            corners[index].z = value;
    }
}

static void LoadModelBoxCorner(uint32 slot, const Vector3 &corner)
{
    xport_gte_write_data(slot, (uint16)corner.x | ((uint32)(uint16)corner.y << 16));
    xport_gte_write_data(slot + 1, (uint32)(uint16)corner.z);
}

static bool IsModelBoxOutside(const Vector3 corners[8], const uint32 light_corners[3], const uint32 color_corners[3])
{
    LoadModelBoxCorner(0, corners[light_corners[0]]);
    xport_gte_execute(0x4A2012);
    sint32 plane = (sint32)xport_gte_read_data(9);
    LoadModelBoxCorner(2, corners[light_corners[1]]);
    if (plane < 0)
        return true;
    xport_gte_execute(0x4AA012);
    plane = (sint32)xport_gte_read_data(10);
    LoadModelBoxCorner(4, corners[light_corners[2]]);
    if (plane < 0)
        return true;
    xport_gte_execute(0x4B2012);
    plane = (sint32)xport_gte_read_data(11);
    LoadModelBoxCorner(0, corners[color_corners[0]]);
    if (plane < 0)
        return true;
    xport_gte_execute(0x4C6012);
    plane = (sint32)xport_gte_read_data(9);
    LoadModelBoxCorner(2, corners[color_corners[1]]);
    if (plane < 0)
        return true;
    xport_gte_execute(0x4CE012);
    plane = (sint32)xport_gte_read_data(10);
    LoadModelBoxCorner(4, corners[color_corners[2]]);
    if (plane < 0)
        return true;
    xport_gte_execute(0x4D6012);
    return (sint32)xport_gte_read_data(11) < 0;
}

static void CullModelObjectList(ModelInstance *object)
{
    ModelBorrow borrow;
    FUNCTION_MARKER(0x80080560, "SLUS_003.73");
    if (!object)
        return;
    uint32 light_corners[3], color_corners[3];
    Vector3 corners[8];
    SelectModelBoxCorners(8, light_corners);
    SelectModelBoxCorners(16, color_corners);
    ModelRenderView *camera = &GetConfiguredModelRenderView();
    uint32 camera_x = camera->pos.words[0];
    uint32 camera_y = camera->pos.words[1];
    uint32 camera_z = camera->pos.words[2];
    do
    {
        uint32 flags = object->flags;
        uint32 position_x = object->pos.words[0];
        uint32 position_y = object->pos.words[1];
        uint32 position_z = object->pos.words[2];
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
                sint32 scale_x = (sint16)object->scale.words[0];
                sint32 scale_y = (sint16)object->scale.words[1];
                sint32 scale_z = (sint16)object->scale.words[2];
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
            uint32 rotation_xy = ((uint32)object->angles.words[0] | ((uint32)object->angles.words[1] << 16));
            uint32 rotation_z = object->angles.words[2];
            uint32 bounds_x = ((uint16)geometry.bounds[0].max | ((uint32)(uint16)geometry.bounds[0].min << 16));
            if (!(rotation_xy | rotation_z | (flags & 514)))
            {
                uint32 bounds_y = ((uint16)geometry.bounds[1].max | ((uint32)(uint16)geometry.bounds[1].min << 16));
                uint32 bounds_z = ((uint16)geometry.bounds[2].max | ((uint32)(uint16)geometry.bounds[2].min << 16));
                WriteModelBoxAxis(corners, 0, x, bounds_x);
                WriteModelBoxAxis(corners, 1, y, bounds_y);
                WriteModelBoxAxis(corners, 2, z, bounds_z);
                if (IsModelBoxOutside(corners, light_corners, color_corners))
                    break;
            }
            outside = false;
        } while (false);
        ModelInstance *next = object->next;
        object->flags = outside ? flags | 32768 : flags & 32767;
        object = next;
    } while (object);
    return;
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
        Vector3 position, transformed;
        MATRIX matrix, combined;
        uint32 render_limit, depth_shift, fog_scale, fog_bias, far_start, far_end;
        uint32 vertex_index, object_value, camera_value, geometry_flags;
        uint32 rotation_xy, rotation_z, attributes, attribute_end, vertex_count;
        uint32 secondary_flags, callback_flags;
        bool transform_matrix;
        if (flags & 0x8001)
            goto NextObject;
        if (!GetModelBank((uint32)owner->bank).loaded)
            goto NextObject;
        render_limit = proj_cfg.fog_start;
        mode = polygon_material_mask;
        depth_shift = proj_cfg.fog_shift;
        fog_scale = model_render_state.reflection_clut;
        fog_bias = model_render_state.reflection_tpage;
        far_start = model_render_state.depth_bias[0];
        far_end = model_render_state.depth_bias[1];
        flags = owner->flags;
        model_render_state.draw.fog_start = render_limit;
        model_render_state.draw.fog_shift = depth_shift;
        model_render_state.draw.clut = fog_scale;
        model_render_state.draw.tpage = fog_bias;
        model_render_state.draw.near_bias = far_start;
        model_render_state.draw.far_bias = far_end;
        if (flags & 0x80)
        {
            mode = (flags & 0x1000) ? (mode & 0xFFF70000) : (mode | 12);
            if (owner->flags & 0x400)
            {
                uint32 color = owner->color;

                game_lighting.background_color.g = (color & 0xFF00) >> 2;
                uint32 red = color & 255;
                game_lighting.background_color.b = ((color & 0xFFFFFF) >> 10) & 0x3FC0;
                game_lighting.background_color.r = red << 6;
            }
        }
        else if (flags & 0x400)
        {
            uint32 color = owner->color;
            mode = (mode & 0xFFFB0000) | 8;

            model_render_state.draw.light.x = (color & 0xFF) << 6;
            model_render_state.draw.light.y = (color & 0xFF00) >> 2;
            model_render_state.draw.light.z = ((color & 0xFFFFFF) >> 10) & 0x3FC0;
        }
        if (owner->flags & 0x800)
            mode = (mode & 0xFFFFFE7F) | (owner->render_mode & 0x180) | 0x40;
        callback_flags = owner->flags;
        model_render_state.draw.mode = mode;
        if (callback_flags & 0x80)
            static_cast<VisualObject *>(owner->owner)->UpdateLighting();
        flags = owner->flags;
        secondary_flags = owner->flags;
        model_render_state.water_clip_enabled = (flags & 8) != 0;
        if (secondary_flags & 2)
        {
            RenderPosedModelObject(*owner);
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
            object_value = owner->pos.words[axis];
            camera_value = camera->pos.words[axis];
            position.components[axis] = (sint32)((uint32)((sint32)object_value >> 12) - camera_value);
        }
        xport_gte_write_data(0, ((uint32)position.components[0] & 65535) | ((uint32)position.components[1] << 16));
        xport_gte_write_data(1, (uint32)position.components[2]);
        xport_gte_execute(0x486012);
        for (axis = 0; axis < 3; ++axis)
            transformed.words[axis] = xport_gte_read_data(25 + axis);
        rotation_xy = ((uint32)owner->angles.words[0] | ((uint32)owner->angles.words[1] << 16));
        model_render_state.use_scratch = 0;
        rotation_z = rotation_xy ? 0 : owner->angles.words[2];
        transform_matrix = rotation_xy || rotation_z;
        if (transform_matrix)
            BuildModelRotationMatrix(owner->angles.words, matrix);
        else
            InitializeModelIdentityMatrix(matrix);
        if (owner->flags & 512)
        {
            ScaleModelRotationMatrix(owner->scale.words, matrix);
            transform_matrix = true;
        }
        if (transform_matrix)
        {
            TransformModelMatrix(matrix, combined);
            SetRotMatrix(&combined);
        }
        geometry_flags = geometry->flags;
        SelectModelPalette(GetModelBank((uint32)owner->bank).colors);
        if (geometry_flags & 1)
            GenerateModelReflectionCoordinates(matrix, transformed.words, *geometry);
        else if (owner->flags & 128)
        {
            attributes = geometry->n_count;
            attribute_end = geometry->poly_count;
            vertex_count = geometry->vertex_count;
            if (attributes != attribute_end)
                attributes = vertex_count;
            GenerateModelNormalColors(geometry->normals, attributes);
        }
        if (model_render_state.use_scratch)
            RenderOffsetScratchModel(*geometry, position.components);
        else if (((uint32)owner->angles.words[0] | ((uint32)owner->angles.words[1] << 16)) || owner->angles.words[2] || (owner->flags & 0x200))
        {
            for (axis = 0; axis < 3; ++axis)
                xport_gte_write_control(5 + axis, transformed.words[axis]);
            RenderProjectedModel(*geometry);
        }
        else
            RenderOffsetModel(*geometry, position.components);
    NextObject:
        owner = owner->next;
    }
}

static void MultiplyPosedModelMatrix(const MATRIX &left, const MATRIX &right, MATRIX &output)
{
    MATRIX local;
    memset(&local, 0, sizeof(local));
    for (uint32 row = 0; row < 3; ++row)
        for (uint32 col = 0; col < 3; ++col)
            local.m[row][col] = right.m[row][col];
    MulMatrix0((MATRIX *)&left, &local, &output);
}

static void MultiplyPosedModelMatrix(const MATRIX &left, const ModelPose &right, MATRIX &output)
{
    MATRIX local;
    memset(&local, 0, sizeof(local));
    for (uint32 row = 0; row < 3; ++row)
        for (uint32 col = 0; col < 3; ++col)
            local.m[row][col] = right.rotation[row].words[col];
    MulMatrix0((MATRIX *)&left, &local, &output);
}

static uint32 RenderPosedModelObject(ModelInstance &instance)
{
    ModelBorrow borrow;
    FUNCTION_MARKER(0x8007F340, "SLUS_003.73");
    Character &character = *static_cast<Character *>(instance.owner);
    ModelRenderView *camera;
    uint32 flags = instance.flags, count;
    const ModelPose *pose;
    uint32 index, mode, rgb = 0, uv0 = 0, uv1 = 0;
    const TextureGroup *animation = 0;
    sint32 animation_frame = 0;
    MATRIX rotation, light, matrix;
    Vector3 transformed, relative;
    uint32 object_value, camera_value;
    // The unlit branch uses the approved zero light matrix
    uint32 axis;
    model_render_state.use_scratch = (flags & 0xC80) ? 0 : 1;
    camera = &GetConfiguredModelRenderView();
    model_render_state.use_scratch = 0;
    SetRotMatrix((MATRIX *)&camera->projection);
    TransformModelMatrix(character.world_transform, rotation);
    camera = &GetConfiguredModelRenderView();
    for (axis = 0; axis < 3; ++axis)
    {
        object_value = instance.pos.words[axis];
        camera_value = camera->pos.words[axis];
        relative.words[axis] = (uint32)((sint32)object_value >> 12) - camera_value;
    }
    for (axis = 0; axis < 3; ++axis)
        xport_gte_write_data(9 + axis, relative.words[axis]);
    xport_gte_execute(0x49E012);
    const ModelGeometrySet &models = GetModelBank(instance.bank).geometry;
    count = models.count;
    for (axis = 0; axis < 3; ++axis)
    {
        transformed.words[axis] = xport_gte_read_data(25 + axis);
        rotation.t[axis] = transformed.words[axis];
    }
    if (flags & 4)
        pose = static_cast<Character *>(instance.owner)->GetModelPoses(0, count);
    else
    {
        const ModelAnimation &animation_data = GetModelAnimation(GetModelBank(instance.bank).animations, instance.animation);
        if (animation_data.interval)
        {
            static_cast<Character *>(instance.owner)->UpdateModelAnimationData();
            pose = GetModelPoseWorkspace(count);
        }
        else
            pose = GetModelAnimationPoses(animation_data, instance.frame * count, count);
    }
    if (flags & 0x80)
    {
        SetColorMatrix(&game_lighting.colors);
        SetFarColor(game_lighting.far_color.r, game_lighting.far_color.g, game_lighting.far_color.b);
        SetBackColor(game_lighting.background_color.r, game_lighting.background_color.g, game_lighting.background_color.b);
        MultiplyPosedModelMatrix(game_lighting.directions, character.world_transform, light);
    }
    else
    {
        memset(&light, 0, sizeof(light));
    }
    mode = character.model_effect.color_mode >> 24;
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
        uint32 tick = game_clock.active_frame;
        uint32 previous_tick = model_render_state.effect_frame;
        if (previous_tick != tick)
        {
            uint32 render_counter = model_render_state.pass_count;
            uint32 next_tick = game_clock.active_frame;
            model_render_state.effect_pass = render_counter;
            model_render_state.effect_frame = next_tick;
        }
        dividend = (sint32)model_render_state.effect_pass;
        divisor = (sint32)animation->count;
        remainder = divisor == 0 ? dividend : (dividend == (sint32)0x80000000 && divisor == -1 ? 0 : dividend % divisor);
        uv0 = character.model_effect.scale;
        uv1 = character.model_effect.depth_bias;
        rgb = character.model_effect.color_mode & 0xFFFFFF;
        animation_frame = remainder;
    }
    if (model_render_state.use_scratch)
        model_render_state.scratch_cache_index = MODEL_SCRATCH_VERTEX_COUNT - 1;
    else
        model_render_state.cache_index = MODEL_PROJECTION_VERTEX_COUNT - 1;
    SelectModelPalette(GetModelBank((uint32)instance.bank).colors);
    for (index = 0; index < count; ++index, ++pose)
    {
        uint32 effects;
        model_render_state.part_hidden = (character.hidden_models & ((uint32)1 << (index & 31))) != 0;
        const ModelGeometry &model = models.models[index];
        SetupModelGteMatrices(rotation, light, *pose);
        if (model.flags & 1)
        {
            MultiplyPosedModelMatrix(character.world_transform, *pose, matrix);
            GenerateModelReflectionCoordinates(matrix, transformed.words, model);
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
            if (model_render_state.use_scratch)
                RenderScratchModel(model);
            else
                RenderTranslatedModel(model);
        }
        if (model_render_state.part_hidden)
            continue;
        effects = character.projection.enabled;
        if ((mode & 0x7F) || effects)
        {
            MultiplyPosedModelMatrix(character.world_transform, *pose, matrix);
            Vector3 translation;
            for (axis = 0; axis < 3; ++axis)
                translation.words[axis] = pose->translation.words[axis];
            for (axis = 0; axis < 3; ++axis)
                xport_gte_write_data(9 + axis, translation.words[axis]);
            LoadModelRotationAndTransformIr(character.world_transform);
            for (axis = 0; axis < 3; ++axis)
                translation.words[axis] = xport_gte_read_data(25 + axis);
            for (axis = 0; axis < 3; ++axis)
                matrix.t[axis] = translation.words[axis] + (uint32)((sint32)instance.pos.words[axis] >> 12);
        }
        if ((mode & 0x7F) && (!model_render_state.water_clip_enabled || matrix.t[1] < (level_environment.water_y >> 12) - 40))
            RenderModelShadow(model, matrix, rgb, uv0, (uint32)(sint32)(sint16)uv1, *GetTextureFrame(animation, animation_frame).tex);
        if (effects)
        {
            RenderModelProjectionEffect(model, matrix, character.surface.n, character.surface.pos.words, character.projection.dir, character.projection.color);
        }
    }
    return 0;
}

static void ReadModelPolygonUvs(const ModelPolygon *descriptor, uint32 flags, const uint32 offsets[4], uint32 vertices, uint32 uv[4])
{
    if (flags & 1)
    {
        uv[0] = PackModelPolygonUv(*descriptor, 0);
        uv[1] = PackModelPolygonUv(*descriptor, 1);
        uv[2] = PackModelPolygonUv(*descriptor, 2);
    }
    else
    {
        uint32 page = model_render_state.draw.clut;
        uv[0] = GetModelLightingValue(offsets[0] >> 3).uv;
        uv[1] = GetModelLightingValue(offsets[1] >> 3).uv;
        uv[0] |= page;
        page = model_render_state.draw.tpage;
        uv[2] = GetModelLightingValue(offsets[2] >> 3).uv;
        if (vertices == 4)
            uv[2] |= GetModelLightingValue(offsets[3] >> 3).uv << 16;
        uv[1] |= page;
    }
    uv[1] |= (flags & 0x180) << 14;
    if (vertices == 4)
        uv[3] = uv[2] >> 16;
}

/* Native helper for the original eight packet-format branches */
static uint32 StoreModelPolygonPacket(uint8 *packet, const ModelPolygon *descriptor, uint32 flags, const uint32 offsets[4], uint32 vertices, uint32 colored, uint32 textured, uint32 first_color)
{
    uint32 uv[4], i, command;
    POINT_CT *colored_tex_points = 0;
    POINT_C *colored_points = 0;
    POINT_T *tex_points = 0;
    XY *flat_points = 0;
    if (colored && textured)
        colored_tex_points = vertices == 3 ? ((POLY_GT3 *)packet)->p : ((POLY_GT4 *)packet)->p;
    else if (colored)
        colored_points = vertices == 3 ? ((POLY_G3 *)packet)->p : ((POLY_G4 *)packet)->p;
    else if (textured)
        tex_points = vertices == 3 ? ((POLY_FT3 *)packet)->p : ((POLY_FT4 *)packet)->p;
    else
        flat_points = vertices == 3 ? ((POLY_F3 *)packet)->p : ((POLY_F4 *)packet)->p;
    if (!colored && textured)
        ReadModelPolygonUvs(descriptor, flags, offsets, vertices, uv);
    for (i = 1; i < vertices; ++i)
    {
        uint32 scr = xport_gte_read_data(11 + i);
        if (colored_tex_points)
            colored_tex_points[i].xy.xy = scr;
        else if (colored_points)
            colored_points[i].xy.xy = scr;
        else if (tex_points)
            tex_points[i].xy.xy = scr;
        else
            flat_points[i].xy = scr;
    }
    if (textured)
    {
        if (colored)
            ReadModelPolygonUvs(descriptor, flags, offsets, vertices, uv);
        for (i = 0; i < vertices; ++i)
        {
            if (colored_tex_points)
            {
                colored_tex_points[i].uv.uv = uv[i];
                colored_tex_points[i].attr = uv[i] >> 16;
            }
            else
            {
                tex_points[i].uv.uv = uv[i];
                tex_points[i].attr = uv[i] >> 16;
            }
        }
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
        {
            uint32 color = xport_gte_read_data((vertices == 3 ? 20 : 19) + i);
            if (colored_tex_points)
                colored_tex_points[i].rgbc.rgbc = color;
            else
                colored_points[i].rgbc.rgbc = color;
        }
        return (vertices * (textured ? 3 : 2)) << 24;
    }
    return (1 + vertices * (textured ? 2 : 1)) << 24;
}

static void BuildModelPolygonPackets(const ModelPolygon *descriptor, uint32 count)
{
    FUNCTION_MARKER(0x800817FC, "SLUS_003.73");
    /* TODO Validate near-plane and subdivision carrier composition */
    uint8 *packet, *limit;
    uint32 *ordering;
    if (!count)
        return;
    ordering = model_render_state.ordering_table;
    packet = scene_render.cursor;
    const ModelPalette *table = GetCurrentModelPalette();
    limit = scene_render.limit - 100;
    xport_gte_write_control(5, 0);
    xport_gte_write_control(6, 0);
    xport_gte_write_control(7, 0);
    xport_gte_write_control(28, 0x10000000);
    xport_gte_write_control(27, 0);
    ResetModelPolygonQueue();
    for (;;)
    {
        uint32 offsets[4], depths[4], flags, clipping, area, fourth_screen;
        const ModelProjectedVertex *points[4];
        uint32 packed = (((uint32)descriptor->v[0] << 3) | ((uint32)descriptor->v[1] << 19));
        uint32 packed2 = (((uint32)descriptor->v[2] << 3) | ((uint32)descriptor->v[3] << 19));
        uint32 mask = model_render_state.draw.mode;
        uint32 depth, fog = 0, fog_shift, vertices, colored, textured, tag;
        uint32 first_color = 0, normal[3], index[4];
        uint32 i, previous, bytes;
        uint32 *bucket;
        POINT_C *first_point;
        uint8 *chain_head = packet;
        offsets[0] = packed & 0xFFFF;
        offsets[1] = packed >> 16;
        offsets[2] = packed2 & 0xFFFF;
        offsets[3] = packed2 >> 16;
        flags = ((((uint32)descriptor->source_stride << 16) | descriptor->flags) & (uint32)((sint32)mask >> 16)) | (mask & 0xFFFF);
        for (i = 0; i < 4; ++i)
            points[i] = &GetModelProjectionVertex(offsets[i] >> 3);
        for (i = 0; i < 3; ++i)
            xport_gte_write_data(12 + i, points[i]->screen);
        if (!(flags & 0xC0))
            goto next_descriptor;
        xport_gte_execute(0x1400006);
        fourth_screen = points[3]->screen;
        for (i = 0; i < 4; ++i)
            depths[i] = points[i]->depth_flags;
        first_point = (POINT_C *)(packet + sizeof(uint32));
        first_point->xy.xy = xport_gte_read_data(12);
        clipping = ((depths[0] & depths[1] & depths[2] & depths[3]) >> 16) ^ 0xFF00;
        xport_gte_write_data(15, fourth_screen);
        area = xport_gte_read_data(24);
        xport_gte_execute(0x1400006);
        if (clipping & 0x20FF)
            goto next_descriptor;
        if (clipping & 0x5000)
        {
            const ModelProjectedVertex *geometry0 = points[0];
            const ModelProjectedVertex *geometry1 = points[1];
            const ModelProjectedVertex *geometry2 = points[2];
            uint32 magnitude = 0, geometry_depth;
            Vector3 cross;
            if (!(flags & 0x1000))
                goto next_descriptor;
            packed = geometry0->xy;
            geometry_depth = points[0]->depth_flags;
            xport_gte_write_control(0, packed);
            xport_gte_write_control(2, packed >> 16);
            xport_gte_write_control(4, geometry_depth);
            packed = geometry1->xy;
            geometry_depth = points[1]->depth_flags;
            xport_gte_write_data(9, packed);
            xport_gte_write_data(10, packed >> 16);
            xport_gte_write_data(11, geometry_depth);
            xport_gte_execute(0x170000C);
            if ((sint32)xport_gte_read_flag() < 0)
            {
                uint32 shift;
                for (i = 0; i < 3; ++i)
                {
                    cross.words[i] = xport_gte_read_data(25 + i);
                    magnitude |= (sint32)cross.words[i] < 0 ? 0 - cross.words[i] : cross.words[i];
                }
                xport_gte_write_data(30, magnitude);
                shift = (17 - xport_gte_read_data(31)) & 31;
                for (i = 0; i < 3; ++i)
                    cross.words[i] = xport_gte_read_data(25 + i);
                for (i = 0; i < 3; ++i)
                    xport_gte_write_data(9 + i, (uint32)((sint32)cross.words[i] >> shift));
            }
            packed = geometry2->xy;
            geometry_depth = points[2]->depth_flags;
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
            QueueWaterPlanePolygon(descriptor, flags, offsets, depths);
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
        packed = model_render_state.draw.fog_start;
        fog_shift = model_render_state.draw.fog_shift;
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
                    normal[0] = model_render_state.draw.light.x;
                    normal[1] = model_render_state.draw.light.y;
                    normal[2] = model_render_state.draw.light.z;
                }
                else if (!(flags & 8))
                    ReadRendererNormal(GetModelLightingValue((((uint32)descriptor->n << 3) | ((uint32)descriptor->surface << 16)) >> 3), normal);
                for (i = 0; i < vertices; ++i)
                {
                    if ((flags & 12) == 12)
                        LoadRendererVertexLighting(index[i], table, GetModelLightingValue(offsets[i] >> 3));
                    else
                        LoadModelTableColor(index[i], table, normal[0], normal[1], normal[2]);
                    if (i == 3)
                        first_color = xport_gte_read_data(20);
                    ExecuteGeometryCommand(0x680029);
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
                    LoadPackedProjectionIr(GetModelLightingValue(offsets[i] >> 3));
                    if (i == 3)
                        first_color = xport_gte_read_data(20);
                    ExecuteGeometryCommand(0x680029);
                }
            }
            else
            {
                if (flags & 4)
                    LoadPackedProjectionIr(GetModelLightingValue((((uint32)descriptor->n << 3) | ((uint32)descriptor->surface << 16)) >> 3));
                else if (flags & 8)
                    LoadModelDrawLighting();
                ExecuteGeometryCommand(flags & 12 ? 0x680029 : 0x780010);
            }
        }
        tag = StoreModelPolygonPacket(packet, descriptor, flags, offsets, vertices, colored, textured, first_color);
        if ((flags & 0x1000) && (sint32)(0x800 - xport_gte_read_data(24)) < 0)
        {
            if ((flags & 1) && ((flags & 0x800) || ((flags & 12) == 12)))
            {
                uint32 last_stride;
                uint8 *end = SubdivideColoredTexturedPolygon(packet, limit, points, xport_gte_read_data(24), !(flags & 0x10), &last_stride);
                if (end == packet)
                    goto next_descriptor;
                packet = end - last_stride;
                tag = (last_stride - 4) << 22;
            }
            else if (textured && !(flags & 0x10))
            {
                uint32 last_stride = 40;
                uint8 *end = SubdivideFlatTexturedQuad(packet, limit, points, xport_gte_read_data(24), &last_stride);
                if (end == packet)
                    goto next_descriptor;
                packet = end - last_stride;
                tag = (last_stride - 4) << 22;
            }
            else if (!(flags & (0x10 | 0x800)) && ((flags & 12) != 12) && !textured)
            {
                uint32 last_stride = 24;
                uint8 *end = SubdivideFlatQuad(packet, limit, points, xport_gte_read_data(24), &last_stride);
                if (end == packet)
                    goto next_descriptor;
                packet = end - last_stride;
                tag = (last_stride - 4) << 22;
            }
            else
            {
                uint32 last_stride;
                uint8 *end;
                if (colored)
                    end = SubdivideColoredPolygon(packet, limit, points, xport_gte_read_data(24), vertices == 4, &last_stride);
                else
                    end = SubdivideFlatTriangle(packet, limit, points, xport_gte_read_data(24), textured, &last_stride);
                if (end == packet)
                    goto next_descriptor;
                packet = end - last_stride;
                tag = (last_stride - 4) << 22;
            }
        }
        if (flags & 0x6000)
        {
            uint32 bias = model_render_state.draw.near_bias;
            if (!(flags & 0x2000))
                bias = model_render_state.draw.far_bias;
            depth += bias;
        }
        bucket = ordering + ((depth < 0x4000 ? depth & 0xFFFC : 0x3FFC) >> 2);
        previous = *bucket;
        if (flags & 0x20)
        {
            uint32 window_command = GetModelPolygonWindow(*descriptor);
            uint8 *end;
            tag += 0x03000000;
            P_TAG *header = (P_TAG *)packet;
            header->addr = previous;
            header->len = (previous | tag) >> 24;
            end = packet + (tag >> 22) + 4;
            *bucket = gpu_packet_link((uintptr_t)end);
            DR_TWIN *reset_window = (DR_TWIN *)(end - sizeof(DR_TWIN));
            reset_window->tag = 0;
            reset_window->code[0] = 0xE2000000;
            reset_window->code[1] = 0;
            DR_TWIN *apply_window = (DR_TWIN *)end;
            apply_window->tag = gpu_packet_link((uintptr_t)chain_head) | 0x02000000;
            apply_window->code[0] = window_command;
            apply_window->code[1] = 0;
            packet = end + 12;
        }
        else
        {
            *bucket = gpu_packet_link((uintptr_t)chain_head);
            P_TAG *header = (P_TAG *)packet;
            header->addr = previous;
            header->len = (previous | tag) >> 24;
            bytes = (tag >> 22) + 4;
            if ((flags & 0x41) == 0x40)
            {
                uint8 *end = packet + bytes;
                DR_TPAGE *mode = (DR_TPAGE *)end;
                mode->code[0] = 0xE1000200 | ((flags & 0x180) >> 2);
                mode->tag = gpu_packet_link((uintptr_t)packet) | 0x01000000;
                *bucket = gpu_packet_link((uintptr_t)end);
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
    scene_render.cursor = packet;
    return;
}
