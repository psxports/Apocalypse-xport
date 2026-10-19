#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "geometry_commands.h"
#include "vector_distance.h"
#include "scene_lifecycle.h"

#include "model_render_view.h"

#include "game_diagnostics.h"

#include "model_render.h"

#include "effect_lifecycle.h"

#include "model_projection.h"
#include "model_matrix.h"

#include "model_lighting.h"

static ModelProjectedVertex projected_vertices[MODEL_PROJECTION_VERTEX_COUNT];

void ResetModelProjectionVertices()

{
    memset(projected_vertices, 0, sizeof(projected_vertices));
}

ModelProjectedVertex &GetModelProjectionVertex(uint32 index)

{
    if (index >= sizeof(projected_vertices) / sizeof(projected_vertices[0]))

    {
        GameDiagnostic("Model projection index exceeds its native owner: %u\n", index);

        abort();
    }

    return projected_vertices[index];
}

static ModelScratchVertex scratch_vertices[MODEL_SCRATCH_VERTEX_COUNT];

void ResetScratchProjectionVertices()

{
    memset(scratch_vertices, 0, sizeof(scratch_vertices));
}

ModelScratchVertex &GetScratchProjectionVertex(uint32 index)

{
    if (index >= sizeof(scratch_vertices) / sizeof(scratch_vertices[0]))

    {
        GameDiagnostic("Short model projection index exceeds its native owner: %u\n", index);

        abort();
    }

    return scratch_vertices[index];
}

static ModelProjectionConfig model_projection_cfg;

const ModelProjectionConfig &GetModelProjectionConfig()

{
    return model_projection_cfg;
}

uint32 ProjectOffsetScratchVertices(const SVECTOR *vertices, uint32 count, const sint32 position[3])
{
    FUNCTION_MARKER(0x800810F0, "SLUS_003.73");
    if (!count)
        return 0x60000000;
    const uint32 x = position[0];
    const uint32 y = position[1];
    const uint32 xy = (x & 65535) | (y << 16);
    const uint32 z = position[2];
    uint32 result = 0x60006000;
    xport_gte_write_control(5, 0);
    xport_gte_write_control(6, 0);
    xport_gte_write_control(7, 0);
    for (uint32 i = 0; i < count; ++i)
    {
        xport_gte_write_data(0, ((uint32)(uint16)vertices[i].vx | ((uint32)(uint16)vertices[i].vy << 16)) + xy);
        xport_gte_write_data(1, ((uint32)(uint16)vertices[i].vz | ((uint32)(uint16)vertices[i].pad << 16)) + z);
        xport_gte_execute(0x180001);
        uint32 scr = xport_gte_read_data(14);
        const uint32 depth = xport_gte_read_data(11);
        scr = (scr & 0xBFFFBFFF) | ~((scr - 0x00F00200) | 0xBFFFBFFF);
        ModelScratchVertex &vertex = GetScratchProjectionVertex(i);
        vertex.screen = scr;
        vertex.depth = depth;
        result &= scr;
    }
    return result;
}

uint32 ProjectModelScratchVertices(const SVECTOR *vertices, uint32 count)
{
    uint32 result = 0x60000000, output = 0, cache_idx, xy, tagged_z, screen, depth, reference;
    FUNCTION_MARKER(0x800811BC, "SLUS_003.73");
    if (!count)
        return result;
    result = 0x60006000;
    cache_idx = model_render_state.scratch_cache_index;
    do
    {
        xy = ((uint32)(uint16)vertices->vx | ((uint32)(uint16)vertices->vy << 16));
        tagged_z = ((uint32)(uint16)vertices->vz | ((uint32)(uint16)vertices->pad << 16));
        xport_gte_write_data(0, xy);
        xport_gte_write_data(1, tagged_z);
        xport_gte_execute(0x180001);
        if (tagged_z & 0x20000)
        {
            reference = MODEL_SCRATCH_VERTEX_COUNT - 1 - (xy >> 3);
            const ModelScratchVertex &vertex = GetScratchProjectionVertex(reference);
            screen = vertex.screen;
            depth = (uint32)(sint32)vertex.depth;
        }
        else
        {
            screen = xport_gte_read_data(14);
            depth = xport_gte_read_data(11);
            screen = (screen & 0xBFFFBFFF) | ~((screen - 0x00F00200) | 0xBFFFBFFF);
            if (tagged_z & 0x10000)
            {
                ModelScratchVertex &vertex = GetScratchProjectionVertex(cache_idx);
                vertex.screen = screen;
                vertex.depth = depth;
                --cache_idx;
            }
        }
        ModelScratchVertex &vertex = GetScratchProjectionVertex(output);
        vertex.screen = screen;
        vertex.depth = depth;
        result &= screen;
        ++vertices;
        --count;
        ++output;
    } while (count);
    model_render_state.scratch_cache_index = cache_idx;
    return result;
}

uint32 ProjectModelClippedVertices(const SVECTOR *vertices, uint32 count)
{
    FUNCTION_MARKER(0x8008131C, "SLUS_003.73");
    if (!count)
        return 255;
    xport_gte_write_control(5, 0);
    xport_gte_write_control(6, 0);
    xport_gte_write_control(7, 0);
    const ModelViewport &viewport = *model_render_state.viewport;
    ModelViewport bounds;
    bounds.right = viewport.right;
    bounds.bottom = viewport.bottom;
    bounds.left = viewport.left;
    bounds.top = viewport.top;
    bounds.near_depth = viewport.near_depth;
    uint32 result = 65535;
    uint32 vertex_idx = 0;
    uint32 xy = ((uint32)(uint16)vertices->vx | ((uint32)(uint16)vertices->vy << 16));
    uint32 tagged_z = ((uint32)(uint16)vertices->vz | ((uint32)(uint16)vertices->pad << 16));
    do
    {
        xport_gte_write_data(0, xy);
        xport_gte_write_data(1, tagged_z);
        ++vertices;
        --count;
        xport_gte_execute(0x180001);
        xy = count ? ((uint32)(uint16)vertices->vx | ((uint32)(uint16)vertices->vy << 16)) : 0;
        tagged_z = count ? ((uint32)(uint16)vertices->vz | ((uint32)(uint16)vertices->pad << 16)) : 0;
        uint32 scr = xport_gte_read_data(14);
        sint32 depth = (sint32)xport_gte_read_data(11);
        ModelProjectedVertex &vertex = GetModelProjectionVertex(vertex_idx);
        vertex.depth_flags = 32767;
        vertex.screen = scr;
        sint32 x = (sint16)scr;
        sint32 y = (sint32)scr >> 16;
        uint32 flags = (bounds.right < x) | ((uint32)(x < bounds.left) << 1) | ((uint32)(bounds.bottom < y) << 2) | ((uint32)(y < bounds.top) << 3) | ((uint32)(depth < bounds.near_depth) << 4);
        if (depth < 0)
            flags ^= 15;
        flags |= (flags << 8) ^ 0xFF00;
        vertex.depth_flags = (vertex.depth_flags & 65535) | ((uint32)(uint16)flags << 16);
        result &= flags;
        ++vertex_idx;
    } while (count);
    return result;
}

void UpdateModelWaterClipFlags(uint32 count)
{
    FUNCTION_MARKER(0x8008128C, "SLUS_003.73");
    const ModelRenderView &camera = GetConfiguredModelRenderView();
    uint32 rotation0 = ReadModelRenderMatrixWord(camera.rotation, 1);
    uint32 rotation1 = ReadModelRenderMatrixWord(camera.rotation, 2);
    uint32 distance = (uint32)model_render_state.water_plane.distance;
    xport_gte_write_control(1, rotation0);
    xport_gte_write_control(2, rotation1);
    xport_gte_write_control(6, 0 - distance);
    uint32 vertex_idx = 0;
    do
    {
        ModelProjectedVertex &vertex = GetModelProjectionVertex(vertex_idx);
        uint32 xy = vertex.xy;
        uint32 depth_flags = vertex.depth_flags;
        xport_gte_write_data(0, xy);
        xport_gte_write_data(1, depth_flags);
        xport_gte_execute(0x480012);
        uint32 water_mask = ((xport_gte_read_data(10) >> 16) ^ 65535) & 0x8080;
        vertex.depth_flags = (vertex.depth_flags & 65535) | ((uint32)(uint16)((depth_flags >> 16) ^ water_mask) << 16);
        ++vertex_idx;
        --count;
    } while (count);
}

void GenerateModelReflectionCoordinates(const MATRIX &matrix, const uint32 *position, const ModelGeometry &model)
{
    uint32 rotation[5], light[4], index, product;
    uint32 saved_screen[3], saved_light[5], saved_rotation[5];
    Vector3 saved_translation;
    uint32 output = 0;
    const SVECTOR *vertices, *normals;
    uint32 count, uv_base, translated_xy, translated_z, delta_xy, delta_z;
    uint32 normal_xy, normal_z, reflected_xy, reflected_z, screen, uv, combined_uv;
    FUNCTION_MARKER(0x80081458, "SLUS_003.73");
    for (index = 0; index < 3; ++index)
        saved_translation.words[index] = ReadGeometryControl(5 + index);
    for (index = 0; index < 3; ++index)
        saved_screen[index] = ReadGeometryControl(24 + index);
    for (index = 0; index < 3; ++index)
        xport_gte_write_control(5 + index, 0);
    xport_gte_write_control(24, 0);
    xport_gte_write_control(25, 0);
    xport_gte_write_control(26, 1024);
    for (index = 0; index < 5; ++index)
        saved_light[index] = ReadGeometryControl(8 + index);
    for (index = 0; index < 5; ++index)
        saved_rotation[index] = ReadGeometryControl(index);
    for (index = 0; index < 5; ++index)
        xport_gte_write_control(8 + index, saved_rotation[index]);
    product = (uint32)(sint32)(sint16)saved_rotation[0] * 2560;
    light[0] = ((uint32)((sint32)product >> 12) & 65535) | (saved_rotation[1] & 0xFFFF0000);
    product = (uint32)((sint32)saved_rotation[0] >> 16) * 2560;
    light[1] = (saved_rotation[3] & 65535) | ((uint32)((sint32)product >> 12) << 16);
    light[2] = (saved_rotation[2] & 65535) | (saved_rotation[3] & 0xFFFF0000);
    product = (uint32)(sint32)(sint16)saved_rotation[1] * 2560;
    light[3] = ((uint32)((sint32)product >> 12) & 65535) | (saved_rotation[2] & 0xFFFF0000);
    xport_gte_write_control(8, light[0]);
    xport_gte_write_control(9, light[1]);
    xport_gte_write_control(10, light[2]);
    xport_gte_write_control(11, light[3]);
    xport_gte_write_control(12, saved_rotation[4]);
    for (index = 0; index < 5; ++index)
        rotation[index] = ReadModelRenderMatrixWord(matrix, index);
    for (index = 0; index < 5; ++index)
        xport_gte_write_control(index, rotation[index]);
    uint32 eye_x = 0 - position[0];
    uint32 eye_y = 0 - position[1];
    uint32 eye_z = 0 - position[2];
    xport_gte_write_data(9, eye_x);
    xport_gte_write_data(10, eye_y);
    xport_gte_write_data(11, eye_z);
    uv_base = model_render_state.reflection_uv;
    xport_gte_execute(0x4BE012);
    count = model.vertex_count;
    vertices = model.vertices;
    normals = model.normals;
    translated_xy = xport_gte_read_data(25) & 65535;
    translated_xy |= xport_gte_read_data(26) << 16;
    translated_z = xport_gte_read_data(27);
    xport_gte_write_control(10, 4096);
    xport_gte_write_control(11, 0);
    xport_gte_write_control(12, 4096);
    while (count)
    {
        uint32 vertex_z = ((uint32)(uint16)vertices->vz | ((uint32)(uint16)vertices->pad << 16));
        uint32 vertex_xy = ((uint32)(uint16)vertices->vx | ((uint32)(uint16)vertices->vy << 16));
        normal_xy = ((uint32)(uint16)normals->vx | ((uint32)(uint16)normals->vy << 16));
        normal_z = ((uint32)(uint16)normals->vz | ((uint32)(uint16)normals->pad << 16));
        delta_xy = translated_xy - vertex_xy;
        delta_z = (translated_z - vertex_z) & 65535;
        xport_gte_write_control(8, delta_xy);
        xport_gte_write_control(9, delta_z);
        xport_gte_write_data(0, normal_xy);
        xport_gte_write_data(1, normal_z);
        xport_gte_execute(0x4A6012);
        product = xport_gte_read_data(9);
        xport_gte_write_data(9, normal_xy);
        xport_gte_write_data(8, product << 1);
        ExecuteGeometryCommand(0x198003D);
        reflected_xy = xport_gte_read_data(9) & 65535;
        reflected_xy |= xport_gte_read_data(10) << 16;
        reflected_z = xport_gte_read_data(11);
        xport_gte_write_data(0, reflected_xy - delta_xy);
        xport_gte_write_data(1, reflected_z - delta_z);
        xport_gte_execute(0x180001);
        screen = xport_gte_read_data(14);
        uv = (((screen >> 5) + 32) & 63) | (((screen >> 13) + 0x2000) & 0x3F00);
        combined_uv = uv + uv_base;
        if ((uv ^ combined_uv) & (uv_base ^ combined_uv) & 0x80000000)
        {
            // Stop before the store as the original signed ADD traps
            GameDiagnostic("Original ADD overflow at 800816E8: uv=%08X base=%08X\n", uv, uv_base);
            abort();
        }
        GetModelLightingValue(output).uv = combined_uv;
        ++vertices;
        ++normals;
        ++output;
        --count;
    }
    for (index = 0; index < 3; ++index)
        xport_gte_write_control(5 + index, saved_translation.words[index]);
    for (index = 0; index < 3; ++index)
        xport_gte_write_control(24 + index, saved_screen[index]);
    for (index = 0; index < 5; ++index)
        xport_gte_write_control(8 + index, saved_light[index]);
    for (index = 0; index < 5; ++index)
        xport_gte_write_control(index, saved_rotation[index]);
}

static uint32 CalculateOffsetVertexClip(sint32 x, sint32 y, sint32 z, const ModelViewport &bounds)
{
    uint32 flags = (bounds.right < x) | ((uint32)(x < bounds.left) << 1) | ((uint32)(bounds.bottom < y) << 2) | ((uint32)(y < bounds.top) << 3) | ((uint32)(z < bounds.near_depth) << 4) | ((uint32)(bounds.far_depth < z) << 5);
    if (z < 0)
        flags ^= 15;
    return flags;
}

static uint32 DivideOffsetProjectionScalar(uint32 numerator, uint32 denominator)
{
    sint32 signed_numerator = (sint32)numerator;
    sint32 signed_denominator = (sint32)denominator;
    if (!denominator)
        return signed_numerator < 0 ? 1 : 0xFFFFFFFF;
    if (numerator == 0x80000000 && denominator == 0xFFFFFFFF)
        return numerator;
    return (uint32)(signed_numerator / signed_denominator);
}

uint32 ProjectOffsetModelVertices(const SVECTOR *vertices, uint32 count, const sint32 translation[3])
{
    ModelViewport bounds;
    sint32 depth, x, y;
    const ModelViewport *viewport;
    uint32 bound_word, aggregate = 0xFFFF;
    uint32 tx, ty, tz, vertex_idx;
    uint32 packed_xy, input_depth, flags, screen, next_xy, next_depth;
    uint32 output, pair, interpolation;
    const SVECTOR *references;
    uint32 idx0, idx1, mac0, mac1, scr0, scr1, len;
    uint32 dx, dy, nx, ny, offset_xy;
    sint32 ox, oy;
    FUNCTION_MARKER(0x800809B0, "SLUS_003.73");
    if (!count)
        return 0xFF;
    viewport = model_render_state.viewport;
    bounds.right = viewport->right;
    bounds.bottom = viewport->bottom;
    bounds.left = viewport->left;
    bounds.top = viewport->top;
    bounds.near_depth = viewport->near_depth;
    bounds.far_depth = viewport->far_depth;
    tx = translation[0];
    ty = translation[1];
    tz = translation[2];
    xport_gte_write_control(5, 0);
    xport_gte_write_control(6, 0);
    xport_gte_write_control(7, 0);
    vertex_idx = 0;
    packed_xy = ((uint32)(uint16)vertices->vx | ((uint32)(uint16)vertices->vy << 16));
    input_depth = ((uint32)(uint16)vertices->vz | ((uint32)(uint16)vertices->pad << 16)) + tz;
    packed_xy = ((packed_xy + tx) & 0xFFFF) | (((uint32)((sint32)packed_xy >> 16) + ty) << 16);
    for (;;)
    {
        xport_gte_write_data(0, packed_xy);
        xport_gte_write_data(1, input_depth);
        flags = (uint16)vertices->pad;
        ++vertices;
        --count;
        xport_gte_execute(0x180001);
        if (flags & 0x10)
            break;
        /* Original loop fetches the following vertex before storing this result */
        next_xy = count ? ((uint32)(uint16)vertices->vx | ((uint32)(uint16)vertices->vy << 16)) : 0;
        next_depth = count ? ((uint32)(uint16)vertices->vz | ((uint32)(uint16)vertices->pad << 16)) + tz : 0;
        next_xy = ((next_xy + tx) & 0xFFFF) | (((uint32)((sint32)next_xy >> 16) + ty) << 16);
        x = (sint32)xport_gte_read_data(9);
        y = (sint32)xport_gte_read_data(10);
        depth = (sint32)xport_gte_read_data(11);
        ModelProjectedVertex &vertex = GetModelProjectionVertex(vertex_idx);
        vertex.xy = ((uint32)x & 0xFFFF) | ((uint32)y << 16);
        screen = xport_gte_read_data(14);
        vertex.depth_flags = (vertex.depth_flags & 0xFFFF0000) | (uint16)depth;
        vertex.screen = screen;
        flags = CalculateOffsetVertexClip((sint16)screen, (sint32)screen >> 16, depth, bounds);
        if ((sint32)xport_gte_read_flag() < 0)
            flags |= 0x40;
        flags |= (flags << 8) ^ 0xFF00;
        vertex.depth_flags = (vertex.depth_flags & 65535) | ((uint32)(uint16)flags << 16);
        aggregate &= flags;
        ++vertex_idx;
        packed_xy = next_xy;
        input_depth = next_depth;
        if (!count)
        {
            model_render_state.generated_vertex_index = vertex_idx;
            model_render_state.generated_cursor_valid = true;
            model_render_state.invalid_generated_cursor = 0;
            return aggregate;
        }
    }
    --vertices;
    ++count;
    xport_gte_write_control(0, 0x28F5);
    xport_gte_write_control(1, 0);
    xport_gte_write_control(2, 4096);
    xport_gte_write_control(3, 0);
    xport_gte_write_control(4, 4096);
    xport_gte_write_control(24, 0);
    xport_gte_write_control(25, 0);
    references = vertices;
    output = vertex_idx;
    do
    {
        pair = ((uint32)(uint16)references->vx | ((uint32)(uint16)references->vy << 16));
        interpolation = ((uint32)(uint16)references->vz | ((uint32)(uint16)references->pad << 16));
        idx0 = (pair & 0xFFFF) >> 3;
        scr0 = GetModelProjectionVertex(idx0).screen;
        depth = (sint16)GetModelProjectionVertex(idx0).depth_flags;
        idx1 = pair >> 19;
        scr1 = GetModelProjectionVertex(idx1).screen;
        oy = (sint32)scr0 >> 16;
        ox = (sint16)scr0;
        dx = (uint32)(sint32)(sint16)scr1 - (uint32)ox;
        dy = (uint32)oy - (uint32)((sint32)scr1 >> 16);
        xport_gte_write_data(9, dx);
        xport_gte_write_data(10, dy);
        xport_gte_execute(0xA00428);
        mac0 = xport_gte_read_data(25);
        mac1 = xport_gte_read_data(26);
        len = LookupDistanceRoot(mac0 + mac1);
        ny = DivideOffsetProjectionScalar(dx << 12, len);
        nx = DivideOffsetProjectionScalar(dy << 12, len);
        xport_gte_write_data(9, nx);
        xport_gte_write_data(10, ny);
        xport_gte_write_data(8, interpolation);
        ExecuteGeometryCommand(0x198003D);
        mac0 = xport_gte_read_data(25);
        mac1 = xport_gte_read_data(26);
        offset_xy = (mac0 & 0xFFFF) | (mac1 << 16);
        xport_gte_write_data(0, offset_xy);
        xport_gte_write_data(1, (uint32)depth);
        xport_gte_execute(0x180001);
        screen = xport_gte_read_data(14);
        x = (sint32)((uint32)(sint32)(sint16)screen + (uint32)ox);
        y = (sint32)((uint32)((sint32)screen >> 16) + (uint32)oy);
        flags = CalculateOffsetVertexClip(x, y, depth, bounds);
        flags |= (flags << 8) ^ 0xFF00;
        aggregate &= flags;
        ModelProjectedVertex &vertex = GetModelProjectionVertex(output);
        vertex.screen = ((uint32)x & 0xFFFF) | ((uint32)y << 16);
        vertex.depth_flags = (flags << 16) | ((uint32)depth & 0xFFFF);
        // Reference records do not write transformed XY in the original
        ++references;
        ++output;
        --count;
    } while (count);
    viewport = model_render_state.viewport;
    bound_word = (viewport->center_x | ((uint32)viewport->center_y << 16));
    xport_gte_write_control(24, bound_word << 16);
    xport_gte_write_control(25, (bound_word >> 16) << 16);
    // Original reference tail leaves the length in V0 instead of its output cursor
    model_render_state.generated_cursor_valid = false;
    model_render_state.invalid_generated_cursor = len;
    return aggregate;
}

uint32 ProjectAndClipModelVertices(const SVECTOR *vertices, uint32 count)
{
    FUNCTION_MARKER(0x80080D84, "SLUS_003.73");
    if (!count)
        return 0xFF;
    const ModelViewport &viewport = *model_render_state.viewport;
    ModelViewport bounds;
    bounds.right = viewport.right;
    bounds.bottom = viewport.bottom;
    bounds.left = viewport.left;
    bounds.top = viewport.top;
    bounds.near_depth = viewport.near_depth;
    bounds.far_depth = viewport.far_depth;
    uint32 aggregate = 0xFFFF;
    uint32 vertex_idx = 0;
    uint32 vertex_xy = ((uint32)(uint16)vertices->vx | ((uint32)(uint16)vertices->vy << 16));
    uint32 vertex_z = ((uint32)(uint16)vertices->vz | ((uint32)(uint16)vertices->pad << 16));
    do
    {
        xport_gte_write_data(0, vertex_xy);
        xport_gte_write_data(1, vertex_z);
        ++vertices;
        --count;
        xport_gte_execute(0x180001);
        uint32 next_xy = count ? ((uint32)(uint16)vertices->vx | ((uint32)(uint16)vertices->vy << 16)) : 0;
        uint32 next_z = count ? ((uint32)(uint16)vertices->vz | ((uint32)(uint16)vertices->pad << 16)) : 0;
        sint32 x = (sint32)xport_gte_read_data(9);
        sint32 y = (sint32)xport_gte_read_data(10);
        sint32 z = (sint32)xport_gte_read_data(11);
        ModelProjectedVertex &vertex = GetModelProjectionVertex(vertex_idx);
        vertex.xy = ((uint32)x & 0xFFFF) | ((uint32)y << 16);
        uint32 scr = xport_gte_read_data(14);
        vertex.depth_flags = (vertex.depth_flags & 0xFFFF0000) | (uint16)z;
        vertex.screen = scr;
        uint32 clip = CalculateOffsetVertexClip((sint16)scr, (sint32)scr >> 16, z, bounds);
        clip |= (clip << 8) ^ 0xFF00;
        vertex.depth_flags = (vertex.depth_flags & 65535) | ((uint32)(uint16)clip << 16);
        aggregate &= clip;
        ++vertex_idx;
        vertex_xy = next_xy;
        vertex_z = next_z;
    } while (count);
    model_render_state.generated_vertex_index = vertex_idx;
    model_render_state.generated_cursor_valid = true;
    model_render_state.invalid_generated_cursor = 0;
    return aggregate;
}

uint32 ProjectWorldPoint(const uint32 *position, uint32 *screen)

{
    uint32 raw, offset, x, y, z;

    FUNCTION_MARKER(0x80025248, "SLUS_003.73");

    raw = position[0];

    offset = GetModelRenderView().pos.words[0];

    x = (uint32)((sint32)raw >> 12) - offset;

    raw = position[1];

    offset = GetModelRenderView().pos.words[1];

    y = (uint32)((sint32)raw >> 12) - offset;

    raw = position[2];

    offset = GetModelRenderView().pos.words[2];

    z = (uint32)((sint32)raw >> 12) - offset;

    xport_gte_write_data(0, (x & 0xFFFF) | (y << 16));

    xport_gte_write_data(1, z);

    xport_gte_execute(0x180001);

    raw = xport_gte_read_data(14);

    *screen = raw;

    return xport_gte_read_data(27);
}

void GenerateModelNormalColors(const SVECTOR *vertices, uint32 count)

{
    FUNCTION_MARKER(0x80081794, "SLUS_003.73");

    uint32 output = 0, first, second, x, y, z;

    if (!count)

        return;

    xport_gte_write_data(6, 0xFFFFFF);

    first = ((uint32)(uint16)vertices->vx | ((uint32)(uint16)vertices->vy << 16));

    second = ((uint32)(uint16)vertices->vz | ((uint32)(uint16)vertices->pad << 16));

    do

    {
        xport_gte_write_data(0, first);

        xport_gte_write_data(1, second);

        ++vertices;

        --count;

        xport_gte_execute(0x108041B);

        first = count ? ((uint32)(uint16)vertices->vx | ((uint32)(uint16)vertices->vy << 16)) : 0;

        second = count ? ((uint32)(uint16)vertices->vz | ((uint32)(uint16)vertices->pad << 16)) : 0;

        x = xport_gte_read_data(9);

        y = xport_gte_read_data(10);

        z = xport_gte_read_data(11);

        ModelLightingValue &value = GetModelLightingValue(output);

        value.xy = (x & 0xFFFF) | (y << 16);

        value.z = z;

        ++output;

    } while ((sint32)count > 0);
}

static void LoadProjectionEffectIr(const ShortVector3 &v)
{
    for (uint32 i = 0; i < 3; ++i)
        xport_gte_write_data(9 + i, (uint16)v[i]);
}

static void StoreProjectionEffectIr(ShortVector3 &v)
{
    for (uint32 i = 0; i < 3; ++i)
        v[i] = (sint16)xport_gte_read_data(9 + i);
}

static void TransformProjectionEffectMatrix(const MATRIX &matrix, uint32 opcode)
{
    (void)xport_gte_read_data(9);
    for (uint32 i = 0; i < 4; ++i)
        xport_gte_write_control(i, ReadModelRenderMatrixWord(matrix, i));
    xport_gte_write_control(4, (uint16)matrix.m[2][2]);
    xport_gte_execute(opcode);
}

static void CrossProjectionEffectVector(const ShortVector3 &v)
{
    (void)xport_gte_read_data(9);
    xport_gte_write_control(0, (uint16)v.x);
    xport_gte_write_control(2, (uint16)v.y);
    xport_gte_write_control(4, (uint16)v.z);
    xport_gte_execute(0x178000C);
}

static sint32 DivideProjectionEffectScalar(sint32 numerator, sint32 denominator)
{
    // Raw MIPS DIV has defined zero and overflow results
    if (!denominator)
        return numerator < 0 ? 1 : -1;
    if ((uint32)numerator == 0x80000000 && denominator == -1)
        return numerator;
    return numerator / denominator;
}

static void ScaleProjectionEffectVector(const ShortVector3 &v, uint32 factor)
{
    (void)xport_gte_read_data(9);
    xport_gte_write_data(8, factor);
    for (uint32 i = 0; i < 3; ++i)
        xport_gte_write_data(25 + i, (uint32)(sint32)v[i]);
    ExecuteGeometryCommand(0x1A8003E);
}

static void LoadProjectionEffectVertex(uint32 slot, const ShortVector3 &center, const ShortVector3 &basis0, const ShortVector3 &basis1, sint32 sign0, sint32 sign1)
{
    sint16 x = (sint16)((uint32)(sint32)center.x + (uint32)(sign0 * basis0.x) + (uint32)(sign1 * basis1.x));
    sint16 y = (sint16)((uint32)(sint32)center.y + (uint32)(sign0 * basis0.y) + (uint32)(sign1 * basis1.y));
    sint16 z = (sint16)((uint32)(sint32)center.z + (uint32)(sign0 * basis0.z) + (uint32)(sign1 * basis1.z));
    xport_gte_write_data(slot, (uint16)x | ((uint32)(uint16)y << 16));
    xport_gte_write_data(slot + 1, (uint16)z);
}

static void CrossProjectionIr(const MATRIX &vector)

{
    FUNCTION_MARKER(0x80085D30, "SLUS_003.73");

    uint32 packed = ReadModelRenderMatrixWord(vector, 0);

    uint32 last = ReadModelRenderMatrixWord(vector, 1);

    (void)xport_gte_read_data(9);

    xport_gte_write_control(0, packed & 65535);

    xport_gte_write_control(2, packed >> 16);

    xport_gte_write_control(4, last);

    xport_gte_execute(0x178000C);
}

void LoadModelRotationAndTransformIr(const MATRIX &matrix)
{
    uint32 words[5], axis;
    FUNCTION_MARKER(0x80085C94, "SLUS_003.73");
    for (axis = 0; axis < 5; ++axis)
        words[axis] = ReadModelRenderMatrixWord(matrix, axis);
    // Original reads IR1 before loading the control bank
    (void)xport_gte_read_data(9);
    for (axis = 0; axis < 5; ++axis)
        xport_gte_write_control(axis, words[axis]);
    xport_gte_execute(0x49E012);
}

static void TransformProjectionIr(const MATRIX &matrix)

{
    FUNCTION_MARKER(0x80085CD4, "SLUS_003.73");

    uint32 words[5];

    for (uint32 index = 0; index < 5; ++index)

        words[index] = ReadModelRenderMatrixWord(matrix, index);

    (void)xport_gte_read_data(9);

    for (uint32 index = 0; index < 5; ++index)

        xport_gte_write_control(index, words[index]);

    xport_gte_execute(0x486012);
}

void RenderModelProjectionEffect(const ModelGeometry &model, const MATRIX &matrix, const SVECTOR &vector, const uint32 *position, const MATRIX &direction, uint32 color)
{
    FUNCTION_MARKER(0x8007A8F8, "SLUS_003.73");
    MATRIX forward, inverse, diagonal, projection;
    ShortVector3 center, extent, n, cross;
    ShortVector3 axis = {0, 0, 0};
    ShortVector3 basis0, basis1, coeff, proj, edge0, edge1, dir;
    uint32 i, squares[3], scale, depth;
    uint8 *packet;
    sint32 avg, numerator, dist;
    sint16 len;
    ModelBounds bounds[3];
    for (i = 0; i < 3; ++i)
        bounds[i] = model.bounds[i];
    for (i = 0; i < 3; ++i)
    {
        sint32 max = (uint16)bounds[i].max;
        sint32 min = bounds[i].min;
        sint32 half = (max - min) >> 1;
        extent[i] = (sint16)((half * (i == 1 ? 11 : 5)) >> (i == 1 ? 3 : 2));
    }
    // Original bounds use an unsigned maximum and a signed minimum
    center.x = (sint16)(((uint16)bounds[0].max + (sint32)bounds[0].min) >> 1);
    center.y = (sint16)(((uint16)model.bounds[1].max + (sint32)model.bounds[1].min) >> 1);
    center.z = (sint16)(((uint16)model.bounds[2].max + (sint32)model.bounds[2].min) >> 1);
    LoadProjectionEffectIr(center);
    LoadModelRotationAndTransformIr(matrix);
    StoreProjectionEffectIr(center);
    for (i = 0; i < 3; ++i)
    {
        uint16 offset;
        offset = (uint16)matrix.t[i];
        center[i] = (sint16)((uint32)(sint32)center[i] + offset);
    }
    avg = ((sint32)extent.x + extent.y + extent.z) / 3;
    numerator = (sint32)((uint32)avg << 16) >> 4;
    /* Original 85C04 copies only the nine rotation halfwords */
    CopyModelRotationMatrix(matrix, forward);
    TransposeModelMatrix(forward, inverse);
    ClearModelRotationMatrix(diagonal);
    sint16 diagonal_x = (sint16)DivideProjectionEffectScalar(numerator, extent.x);
    sint16 diagonal_y = (sint16)DivideProjectionEffectScalar(numerator, extent.y);
    sint16 diagonal_z = (sint16)DivideProjectionEffectScalar(numerator, extent.z);
    diagonal.m[0][0] = diagonal_x;
    diagonal.m[1][1] = diagonal_y;
    diagonal.m[2][2] = diagonal_z;
    for (i = 0; i < 3; ++i)
        dir[i] = direction.m[0][i];
    LoadProjectionEffectIr(dir);
    TransformProjectionEffectMatrix(inverse, 0x49E012);
    (void)NormalizeProjectionIr();
    TransformProjectionEffectMatrix(diagonal, 0x49E012);
    StoreProjectionEffectIr(n);
    len = (sint16)MeasureProjectionIrLength();
    for (i = 0; i < 3; ++i)
        squares[i] = xport_gte_read_data(25 + i);
    if ((sint32)squares[1] >= (sint32)squares[0] && (sint32)squares[2] >= (sint32)squares[0])
        axis.x = 4096;
    else if ((sint32)squares[0] >= (sint32)squares[1] && (sint32)squares[2] >= (sint32)squares[1])
        axis.y = 4096;
    else
        axis.z = 4096;
    LoadProjectionEffectIr(n);
    CrossProjectionEffectVector(axis);
    StoreProjectionEffectIr(cross);
    (void)NormalizeProjectionIr();
    scale = (uint32)((sint32)extent.x * extent.y);
    scale = (uint32)DivideProjectionEffectScalar((sint32)scale, (sint16)avg);
    scale *= (uint32)(sint32)extent.z;
    scale *= (uint32)(sint32)len;
    scale = (uint32)(DivideProjectionEffectScalar((sint32)scale, (sint16)avg) >> 12);
    TransformProjectionEffectMatrix(diagonal, 0x49E012);
    TransformProjectionEffectMatrix(forward, 0x49E012);
    CrossProjectionIr(direction);
    ScaleProjectionIr(scale);
    StoreProjectionEffectIr(basis0);
    LoadProjectionEffectIr(n);
    CrossProjectionEffectVector(cross);
    (void)NormalizeProjectionIr();
    TransformProjectionEffectMatrix(diagonal, 0x49E012);
    TransformProjectionEffectMatrix(forward, 0x49E012);
    CrossProjectionIr(direction);
    ScaleProjectionIr(scale);
    StoreProjectionEffectIr(basis1);
    xport_gte_write_data(0, (uint16)vector.vx | ((uint32)(uint16)vector.vy << 16));
    xport_gte_write_data(1, (uint16)vector.vz | ((uint32)(uint16)vector.pad << 16));
    TransformProjectionIr(direction);
    dist = (sint32)xport_gte_read_data(25);
    static const sint32 minimum_projection_alignment = 1024;
    if (dist < minimum_projection_alignment)
        return;
    for (i = 0; i < 3; ++i)
    {
        projection.m[0][i] = (sint16)((uint32)((sint32)position[i] >> 12) - (uint32)(sint32)center[i]);
        projection.m[1][i] = basis0[i];
        projection.m[2][i] = basis1[i];
    }
    TransformProjectionEffectMatrix(projection, 0x486012);
    StoreProjectionEffectIr(coeff);
    if (coeff.x > 0)
        return;
    ModelRenderView *camera = &GetConfiguredModelRenderView();
    for (i = 0; i < 3; ++i)
        center[i] = (sint16)((uint32)(sint32)center[i] - ((uint32)(uint16)camera->pos.words[i]));
    LoadProjectionEffectIr(dir);
    ScaleProjectionEffectVector(center, (uint32)DivideProjectionEffectScalar((sint32)((uint32)(sint32)coeff.x << 12), dist));
    StoreProjectionEffectIr(proj);
    LoadProjectionEffectIr(dir);
    ScaleProjectionEffectVector(basis0, (uint32)DivideProjectionEffectScalar((sint32)(0 - ((uint32)(sint32)coeff.y << 12)), dist));
    StoreProjectionEffectIr(edge0);
    LoadProjectionEffectIr(dir);
    ScaleProjectionEffectVector(basis1, (uint32)DivideProjectionEffectScalar((sint32)(0 - ((uint32)(sint32)coeff.z << 12)), dist));
    StoreProjectionEffectIr(edge1);
    SetRotMatrix((MATRIX *)&GetConfiguredModelRenderView().projection);
    for (i = 0; i < 3; ++i)
        xport_gte_write_control(5 + i, 0);
    LoadProjectionEffectVertex(0, proj, edge0, edge1, -1, -1);
    LoadProjectionEffectVertex(2, proj, edge0, edge1, 1, -1);
    LoadProjectionEffectVertex(4, proj, edge0, edge1, -1, 1);
    xport_gte_execute(0x280030);
    for (i = 0; i < 3; ++i)
    {
        GetModelProjectionVertex(i).screen = xport_gte_read_data(12 + i);
    }
    LoadProjectionEffectVertex(0, proj, edge0, edge1, 1, 1);
    xport_gte_execute(0x180001);
    GetModelProjectionVertex(3).screen = xport_gte_read_data(14);
    xport_gte_execute(0x168002E);
    depth = xport_gte_read_data(7);
    if (depth >= 4096)
        depth = 4095;
    packet = scene_render.cursor;
    uint8 *limit = scene_render.limit;
    if (limit - packet < (sint32)sizeof(POLY_FT4))
        return;
    scene_render.cursor = packet + sizeof(POLY_FT4);
    POLY_FT4 *poly = (POLY_FT4 *)packet;
    setPolyFT4(poly);
    setSemiTrans(poly, 1);
    for (i = 0; i < 4; ++i)
        poly->p[i].xy.xy = GetModelProjectionVertex(i).screen;
    setRGB0(poly, color, color >> 8, color >> 16);
    const TextureResource *tex = GetShadowTexture();
    if (!tex)
    {
        GameDiagnostic("Missing projection shadow texture\n");
        abort();
    }
    setUV4(poly, tex->uv[0].u, tex->uv[0].v, tex->uv[1].u - 1, tex->uv[1].v, tex->uv[2].u, tex->uv[2].v - 1, tex->uv[3].u - 1, tex->uv[3].v - 1);
    poly->p[0].attr = tex->clut;
    uint32 tpage = tex->tpage;
    poly->p[1].attr = (tpage & 0xFF9F) | 0x40;
    setaddr(poly, getaddr(scene_render.cur->ot + depth));
    setaddr(scene_render.cur->ot + depth, gpu_packet_link((uintptr_t)poly));
}

uint32 MeasureProjectionIrLength()

{
    FUNCTION_MARKER(0x80085D14, "SLUS_003.73");

    xport_gte_execute(0xA00428);

    uint32 x = xport_gte_read_data(25);

    uint32 y = xport_gte_read_data(26);

    uint32 z = xport_gte_read_data(27);

    return LookupDistanceRoot(x + y + z);
}

uint32 NormalizeProjectionIr()

{
    FUNCTION_MARKER(0x80085D64, "SLUS_003.73");

    Vector3 saved, normalized;

    for (uint32 axis = 0; axis < 3; ++axis)

        saved.words[axis] = xport_gte_read_data(9 + axis);

    uint32 length = MeasureProjectionIrLength();

    for (uint32 axis = 0; axis < 3; ++axis)

        normalized.words[axis] = (uint32)DivideProjectionEffectScalar((sint32)(saved.words[axis] << 12), (sint32)length);

    for (uint32 axis = 0; axis < 3; ++axis)

        xport_gte_write_data(9 + axis, normalized.words[axis]);

    return length;
}

uint32 ProjectModelReferenceVertices(const SVECTOR *input, uint32 count)
{
    FUNCTION_MARKER(0x80080EF4, "SLUS_003.73");
    if (!count)
        return 255;
    const ModelViewport &viewport = *model_render_state.viewport;
    ModelViewport bounds;
    bounds.right = viewport.right;
    bounds.bottom = viewport.bottom;
    bounds.left = viewport.left;
    bounds.top = viewport.top;
    bounds.near_depth = viewport.near_depth;
    bounds.far_depth = viewport.far_depth;
    uint32 common = 0xFFFF;
    uint32 vertex_idx = 0;
    uint32 cache_idx = model_render_state.cache_index;
    uint32 packed = ((uint32)(uint16)input->vx | ((uint32)(uint16)input->vy << 16));
    uint32 tagged_z = ((uint32)(uint16)input->vz | ((uint32)(uint16)input->pad << 16));
    do
    {
        uint32 tag = tagged_z >> 16;
        xport_gte_write_data(0, packed);
        xport_gte_write_data(1, tagged_z);
        ++input;
        --count;
        xport_gte_execute(0x180001);
        if (tag & 2)
        {
            uint32 reference_idx = MODEL_PROJECTION_VERTEX_COUNT - 1 - (packed >> 3);
            const ModelProjectedVertex &cached = GetModelProjectionVertex(reference_idx);
            uint32 scr = cached.screen;
            uint32 depth_flags = cached.depth_flags;
            uint32 xy = cached.xy;
            ModelProjectedVertex &vertex = GetModelProjectionVertex(vertex_idx);
            vertex.screen = scr;
            vertex.depth_flags = depth_flags;
            vertex.xy = xy;
            common &= depth_flags >> 16;
            packed = count ? ((uint32)(uint16)input->vx | ((uint32)(uint16)input->vy << 16)) : 0;
            tagged_z = count ? ((uint32)(uint16)input->vz | ((uint32)(uint16)input->pad << 16)) : 0;
        }
        else
        {
            packed = count ? ((uint32)(uint16)input->vx | ((uint32)(uint16)input->vy << 16)) : 0;
            tagged_z = count ? ((uint32)(uint16)input->vz | ((uint32)(uint16)input->pad << 16)) : 0;
            uint32 ir_x = xport_gte_read_data(9);
            uint32 ir_y = xport_gte_read_data(10);
            sint32 depth = (sint32)xport_gte_read_data(11);
            uint32 xy = (ir_x & 65535) | (ir_y << 16);
            uint32 scr = xport_gte_read_data(14);
            uint32 flags = CalculateOffsetVertexClip((sint16)scr, (sint32)scr >> 16, depth, bounds);
            flags |= (flags << 8) ^ 0xFF00;
            common &= flags;
            uint32 depth_flags = (flags << 16) | ((uint32)depth & 0xFFFF);
            ModelProjectedVertex &vertex = GetModelProjectionVertex(vertex_idx);
            vertex.screen = scr;
            vertex.depth_flags = depth_flags;
            vertex.xy = xy;
            if (tag & 1)
            {
                ModelProjectedVertex &cached = GetModelProjectionVertex(cache_idx);
                cached.screen = scr;
                cached.depth_flags = depth_flags;
                cached.xy = xy;
                --cache_idx;
            }
        }
        ++vertex_idx;
    } while (count);
    model_render_state.cache_index = cache_idx;
    model_render_state.generated_vertex_index = vertex_idx;
    model_render_state.generated_cursor_valid = true;
    model_render_state.invalid_generated_cursor = 0;
    return common;
}

void LoadPackedProjectionIr(const ModelLightingValue &value)

{
    FUNCTION_MARKER(0x800821E0, "SLUS_003.73");

    uint32 packed = value.xy;

    uint32 depth = value.z;

    xport_gte_write_data(9, packed);

    xport_gte_write_data(10, (uint32)((sint32)packed >> 16));

    xport_gte_write_data(11, depth);
}

uint32 ProjectAndClipWorldLine(const Vector3 &start, const Vector3 &end, ProjectedWorldLine &proj)
{
    ModelRenderView *camera = &GetConfiguredModelRenderView();

    struct LineClipVertex
    {
        Vector3 pos;
        uint32 scr;
        uint32 flags;
        sint32 plane_dist[6];
        uint32 depth;
    };

    LineClipVertex vertex[2];
    Vector3 camera_pos;
    uint32 packed, z, clip_status, result, axis;
    uint32 saved_rotation[5], t_min = 0, t_max = 4096, mask, plane_idx;
    sint32 dx, dy;
    FUNCTION_MARKER(0x800850A4, "SLUS_003.73");
    camera_pos = camera->pos;
    packed = ((uint32)(start.x >> 12) - camera_pos.words[0]) & 0xFFFF;
    packed |= ((uint32)(start.y >> 12) - camera_pos.words[1]) << 16;
    z = (uint32)(start.z >> 12) - camera_pos.words[2];
    xport_gte_write_data(0, packed);
    xport_gte_write_data(1, z);
    xport_gte_execute(0x180001);
    packed = ((uint32)(end.x >> 12) - camera_pos.words[0]) & 0xFFFF;
    packed |= ((uint32)(end.y >> 12) - camera_pos.words[1]) << 16;
    z = (uint32)(end.z >> 12) - camera_pos.words[2];
    vertex[0].flags = ReadGeometryControl(31);
    vertex[0].scr = xport_gte_read_data(14);
    for (axis = 0; axis < 3; ++axis)
        vertex[0].pos.words[axis] = xport_gte_read_data(25 + axis);
    xport_gte_write_data(0, packed);
    xport_gte_write_data(1, z);
    xport_gte_execute(0x180001);
    proj.screen[0] = vertex[0].scr;
    proj.depth[0] = vertex[0].pos.words[2];
    vertex[1].flags = ReadGeometryControl(31);
    vertex[1].scr = xport_gte_read_data(14);
    for (axis = 0; axis < 3; ++axis)
        vertex[1].pos.words[axis] = xport_gte_read_data(25 + axis);
    clip_status = vertex[0].flags | vertex[1].flags;
    if ((sint32)clip_status >= 0)
    {
        dx = (sint16)(vertex[1].scr - vertex[0].scr);
        clip_status = (uint32)dx;
        if (dx < 1024)
        {
            // The lower X bound branch also changes the status used for rejection
            clip_status = (uint32)((sint32)vertex[0].scr >> 16);
            if (dx >= -1023)
            {
                dy = ((sint32)vertex[1].scr >> 16) - (sint32)clip_status;
                clip_status = (uint32)dy;
                if (dy < 512 && dy >= -511)
                {
                    proj.screen[1] = vertex[1].scr;
                    proj.depth[1] = vertex[1].pos.words[2];
                    result = (sint32)vertex[0].pos.words[2] < (sint32)vertex[1].pos.words[2] ? vertex[0].pos.words[2] >> 2 : vertex[1].pos.words[2] >> 2;
                    return (sint32)(result - 4096) < 0 ? result : 4095;
                }
            }
        }
    }
    if (((clip_status >> 22) & 7) || (sint32)(vertex[0].pos.words[2] & vertex[1].pos.words[2]) < 0)
        return 0xFFFFFFFF;
    xport_gte_write_data(0, (vertex[0].pos.words[0] & 0xFFFF) | (vertex[0].pos.words[1] << 16));
    xport_gte_write_data(1, vertex[0].pos.words[2]);
    xport_gte_write_data(2, (vertex[1].pos.words[0] & 0xFFFF) | (vertex[1].pos.words[1] << 16));
    xport_gte_write_data(3, vertex[1].pos.words[2]);
    LoadModelLineClipMatrices();
    xport_gte_execute(0x4A2412);
    vertex[0].flags = (ReadGeometryControl(31) >> 19) & 0x38;
    for (axis = 0; axis < 3; ++axis)
        vertex[0].plane_dist[axis] = xport_gte_read_data(25 + axis);
    xport_gte_execute(0x4C6412);
    vertex[0].flags |= (ReadGeometryControl(31) >> 22) & 7;
    for (axis = 0; axis < 3; ++axis)
        vertex[0].plane_dist[3 + axis] = xport_gte_read_data(25 + axis);
    xport_gte_execute(0x4AA412);
    vertex[1].flags = (ReadGeometryControl(31) >> 19) & 0x38;
    for (axis = 0; axis < 3; ++axis)
        vertex[1].plane_dist[axis] = xport_gte_read_data(25 + axis);
    xport_gte_execute(0x4CE412);
    vertex[1].flags |= (ReadGeometryControl(31) >> 22) & 7;
    for (axis = 0; axis < 3; ++axis)
        vertex[1].plane_dist[3 + axis] = xport_gte_read_data(25 + axis);
    if ((vertex[0].flags & vertex[1].flags) || ((vertex[0].flags | vertex[1].flags) & 0x20))
        return 0xFFFFFFFF;
    plane_idx = 0;
    for (mask = 32; mask; mask >>= 1, ++plane_idx)
    {
        if ((vertex[0].flags ^ vertex[1].flags) & mask)
        {
            uint32 dist = (uint32)vertex[0].plane_dist[plane_idx], other_dist = (uint32)vertex[1].plane_dist[plane_idx];
            uint32 t = DivideOffsetProjectionScalar(dist << 12, dist - other_dist);
            if (vertex[0].flags & mask)
            {
                if ((sint32)(t - t_min) > 0)
                    t_min = t;
            }
            else if ((sint32)(t - t_max) < 0)
                t_max = t;
        }
    }
    if ((sint32)(t_min - t_max) >= 0)
        return 0xFFFFFFFF;
    for (axis = 0; axis < 5; ++axis)
        saved_rotation[axis] = ReadGeometryControl(axis);
    packed = (xport_gte_read_data(0) << 3) & 0xFFF8FFF8;
    vertex[0].depth = xport_gte_read_data(1) << 3;
    z = (xport_gte_read_data(2) << 3) & 0xFFF8FFF8;
    vertex[1].depth = xport_gte_read_data(3) << 3;
    xport_gte_write_control(0, packed);
    xport_gte_write_control(2, z);
    xport_gte_write_control(1, packed ^ ((packed ^ z) & 0xFFFF));
    xport_gte_write_control(3, vertex[0].depth);
    xport_gte_write_control(4, vertex[1].depth);
    xport_gte_write_data(0, 4096 - t_min);
    xport_gte_write_data(1, t_min);
    xport_gte_execute(0x180001);
    vertex[0].scr = xport_gte_read_data(14);
    vertex[0].depth = xport_gte_read_data(11);
    xport_gte_write_data(0, 4096 - t_max);
    xport_gte_write_data(1, t_max);
    vertex[0].depth >>= 3;
    xport_gte_execute(0x180001);
    proj.screen[0] = vertex[0].scr;
    proj.depth[0] = vertex[0].depth;
    vertex[1].scr = xport_gte_read_data(14);
    vertex[1].depth = xport_gte_read_data(11);
    proj.screen[1] = vertex[1].scr;
    vertex[1].depth >>= 3;
    proj.depth[1] = vertex[1].depth;
    for (axis = 0; axis < 5; ++axis)
        xport_gte_write_control(axis, saved_rotation[axis]);
    result = (sint32)vertex[0].depth < (sint32)vertex[1].depth ? vertex[0].depth >> 2 : vertex[1].depth >> 2;
    return (sint32)(result - 4096) < 0 ? result : 4095;
}

void ScaleProjectionIr(uint32 scale)

{
    FUNCTION_MARKER(0x80085DBC, "SLUS_003.73");

    (void)xport_gte_read_data(9);

    xport_gte_write_data(8, scale);

    ExecuteGeometryCommand(0x198003D);
}

void AccumulateProjectionVector(const SVECTOR &vector, uint32 scale)

{
    FUNCTION_MARKER(0x80085DD8, "SLUS_003.73");

    uint32 x = vector.vx;

    uint32 y = vector.vy;

    uint32 z = vector.vz;

    (void)xport_gte_read_data(9);

    xport_gte_write_data(8, scale);

    xport_gte_write_data(25, x);

    xport_gte_write_data(26, y);

    xport_gte_write_data(27, z);

    ExecuteGeometryCommand(0x1A8003E);
}

uint32 ConfigureProjectionDepth(uint32 limit, uint32 base, uint32 extent)
{
    FUNCTION_MARKER(0x8007D76C, "SLUS_003.73");
    uint32 x = model_projection_cfg.scr[0];
    uint32 sq_x = x * x;
    uint32 y = model_projection_cfg.scr[1];
    uint32 sq_y = y * y;
    uint32 z = model_projection_cfg.scr[2];
    uint32 sq_z = z * z;
    model_projection_cfg.near_depth = limit;
    model_projection_cfg.fog_start = base;
    uint32 end = base + extent;
    model_projection_cfg.far_depth = end;
    uint32 root = SquareRoot0(sq_x + sq_y + sq_z);
    end = model_projection_cfg.far_depth;
    uint32 product = end * root;
    uint32 divisor = model_projection_cfg.scr[2];
    model_projection_cfg.max_distance = DivideOffsetProjectionScalar(product, divisor);
    uint32 result = extent < 4096;
    model_projection_cfg.fog_shift = 0;
    if (extent < 4096)
    {
        do
        {
            uint32 shift = model_projection_cfg.fog_shift + 1;
            model_projection_cfg.fog_shift = shift;
            result = extent < (4096 >> (shift & 31));
        } while (result);
    }
    else if (extent > 4096)
    {
        // TODO Original shift cycle cannot reach extents above 80000000
        do
        {
            uint32 shift = model_projection_cfg.fog_shift - 1;
            model_projection_cfg.fog_shift = shift;
            result = ((uint32)4096 << ((0 - shift) & 31)) < extent;
        } while (result);
    }
    return result;
}

void SetProjectionScreenParameters(uint32 x, uint32 y, uint32 depth)

{
    FUNCTION_MARKER(0x8007D8C0, "SLUS_003.73");

    model_projection_cfg.scr[0] = x;

    model_projection_cfg.scr[1] = y;

    model_projection_cfg.scr[2] = depth;
}
