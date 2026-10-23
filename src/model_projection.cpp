#include <string.h>
#include <stdlib.h>

#include "psx.h"
#include "global.h"
#include "scene_lifecycle.h"
#include "model_render_view.h"
#include "game_diagnostics.h"
#include "model_render.h"
#include "effect_lifecycle.h"
#include "model_projection.h"
#include "model_matrix.h"
#include "model_lighting.h"
#include "effect_object.h"

static ModelProjectedVertex proj_vertices[model_projection_vertex_count];

void ResetModelProjVertices()
{
    memset(proj_vertices, 0, sizeof(proj_vertices));
}

ModelProjectedVertex &GetModelProjVertex(uint32 idx)
{
    if (idx >= sizeof(proj_vertices) / sizeof(proj_vertices[0]))
    {
        GameDiagnostic(("Model projection index exceeds its native owner: %u\n", idx));
        abort();
    }
    return proj_vertices[idx];
}

static ModelScratchVertex scratch_vertices[model_scratch_vertex_count];

void ResetScratchProjVertices()
{
    memset(scratch_vertices, 0, sizeof(scratch_vertices));
}

ModelScratchVertex &GetScratchProjVertex(uint32 idx)
{
    if (idx >= sizeof(scratch_vertices) / sizeof(scratch_vertices[0]))
    {
        GameDiagnostic(("Short model projection index exceeds its native owner: %u\n", idx));
        abort();
    }
    return scratch_vertices[idx];
}

static ModelProjectionConfig model_proj_cfg;

const ModelProjectionConfig &GetModelProjConfig()
{
    return model_proj_cfg;
}

uint32 ProjectOffsetScratchVertices(const SVECTOR *vertices, uint32 count, const Vector3 *pos)
{
    FUNCTION_MARKER(0x800810F0, "SLUS_003.73");
    if (!count)
        return 0x60000000;
    const uint32 xy = ((uint32)pos->x & 65535) | ((uint32)pos->y << 16);
    const uint32 z = pos->z;
    uint32 result = 0x60006000;
    xport_gte_write_control(5, 0);
    xport_gte_write_control(6, 0);
    xport_gte_write_control(7, 0);
    for (uint32 i = 0; i < count; ++i)
    {
        xport_gte_write_data(0, ((uint16)vertices[i].vx | ((uint32)(uint16)vertices[i].vy << 16)) + xy);
        xport_gte_write_data(1, ((uint16)vertices[i].vz | ((uint32)(uint16)vertices[i].pad << 16)) + z);
        xport_gte_execute(0x180001);
        uint32 scr = xport_gte_read_data(14);
        const sint16 depth = xport_gte_read_data(11);
        scr = (scr & 0xBFFFBFFF) | ~((scr - 0x00F00200) | 0xBFFFBFFF);
        ModelScratchVertex &vertex = GetScratchProjVertex(i);
        vertex.scr = scr;
        vertex.depth = depth;
        result &= scr;
    }
    return result;
}

uint32 ProjectModelScratchVertices(const SVECTOR *vertices, uint32 count)
{
    uint32 result = 0x60000000, output = 0, cache_idx, xy, tagged_z, scr, reference;
    sint16 depth;
    FUNCTION_MARKER(0x800811BC, "SLUS_003.73");
    if (!count)
        return result;
    result = 0x60006000;
    cache_idx = model_render_state.scratch_cache_idx;
    do
    {
        xy = (uint16)vertices->vx | ((uint32)(uint16)vertices->vy << 16);
        tagged_z = (uint16)vertices->vz | ((uint32)(uint16)vertices->pad << 16);
        xport_gte_write_data(0, xy);
        xport_gte_write_data(1, tagged_z);
        xport_gte_execute(0x180001);
        if (tagged_z & 0x20000)
        {
            reference = model_scratch_vertex_count - 1 - (xy >> 3);
            const ModelScratchVertex &vertex = GetScratchProjVertex(reference);
            scr = vertex.scr;
            depth = vertex.depth;
        }
        else
        {
            scr = xport_gte_read_data(14);
            depth = xport_gte_read_data(11);
            scr = (scr & 0xBFFFBFFF) | ~((scr - 0x00F00200) | 0xBFFFBFFF);
            if (tagged_z & 0x10000)
            {
                ModelScratchVertex &vertex = GetScratchProjVertex(cache_idx);
                vertex.scr = scr;
                vertex.depth = depth;
                --cache_idx;
            }
        }
        ModelScratchVertex &vertex = GetScratchProjVertex(output);
        vertex.scr = scr;
        vertex.depth = depth;
        result &= scr;
        ++vertices;
        --count;
        ++output;
    } while (count);
    model_render_state.scratch_cache_idx = cache_idx;
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
    uint32 xy = (uint16)vertices->vx | ((uint32)(uint16)vertices->vy << 16);
    uint32 tagged_z = (uint16)vertices->vz | ((uint32)(uint16)vertices->pad << 16);
    do
    {
        xport_gte_write_data(0, xy);
        xport_gte_write_data(1, tagged_z);
        ++vertices;
        --count;
        xport_gte_execute(0x180001);
        xy = count ? ((uint16)vertices->vx | ((uint32)(uint16)vertices->vy << 16)) : 0;
        tagged_z = count ? ((uint16)vertices->vz | ((uint32)(uint16)vertices->pad << 16)) : 0;
        XY scr;
        scr.xy = xport_gte_read_data(14);
        sint32 depth = xport_gte_read_data(11);
        ModelProjectedVertex &vertex = GetModelProjVertex(vertex_idx);
        vertex.depth_flags = 32767;
        vertex.scr = scr.xy;
        sint32 x = scr.x;
        sint32 y = scr.y;
        uint32 flags = (bounds.right < x) | ((x < bounds.left) << 1) | ((bounds.bottom < y) << 2) | ((y < bounds.top) << 3) | ((depth < bounds.near_depth) << 4);
        if (depth < 0)
            flags ^= 15;
        flags |= (flags << 8) ^ 0xFF00;
        vertex.depth_flags = (vertex.depth_flags & 65535) | (flags << 16);
        result &= flags;
        ++vertex_idx;
    } while (count);
    return result;
}

void UpdateModelWaterClipFlags(uint32 count)
{
    FUNCTION_MARKER(0x8008128C, "SLUS_003.73");
    const ModelRenderView &camera = GetConfiguredModelRenderView();
    uint32 rotation0 = PackModelRotationWord(camera.rotation, 1);
    uint32 rotation1 = PackModelRotationWord(camera.rotation, 2);
    uint32 distance = model_render_state.water_plane.distance;
    xport_gte_write_control(1, rotation0);
    xport_gte_write_control(2, rotation1);
    xport_gte_write_control(6, -distance);
    uint32 vertex_idx = 0;
    do
    {
        ModelProjectedVertex &vertex = GetModelProjVertex(vertex_idx);
        uint32 xy = vertex.xy;
        uint32 depth_flags = vertex.depth_flags;
        xport_gte_write_data(0, xy);
        xport_gte_write_data(1, depth_flags);
        xport_gte_execute(0x480012);
        uint32 water_mask = ((xport_gte_read_data(10) >> 16) ^ 65535) & 0x8080;
        vertex.depth_flags = (vertex.depth_flags & 65535) | (((depth_flags >> 16) ^ water_mask) << 16);
        ++vertex_idx;
        --count;
    } while (count);
}

void GenerateModelReflectionCoords(const MATRIX &matrix, const Vector3 *pos, const ModelGeometry &model)
{
    uint32 rotation[5], light[4], idx, product;
    uint32 saved_scr[3], saved_light[5], saved_rotation[5];
    Vector3 saved_translation;
    uint32 output = 0;
    const SVECTOR *vertices, *normals;
    uint32 count, uv_base, translated_xy, translated_z, delta_xy, delta_z;
    uint32 normal_xy, normal_z, reflected_xy, reflected_z, scr, uv, combined_uv;
    FUNCTION_MARKER(0x80081458, "SLUS_003.73");
    for (idx = 0; idx < 3; ++idx)
        saved_translation.comp[idx] = xport_gte_read_control(5 + idx);
    for (idx = 0; idx < 3; ++idx)
        saved_scr[idx] = xport_gte_read_control(24 + idx);
    for (idx = 0; idx < 3; ++idx)
        xport_gte_write_control(5 + idx, 0);
    xport_gte_write_control(24, 0);
    xport_gte_write_control(25, 0);
    xport_gte_write_control(26, 1024);
    for (idx = 0; idx < 5; ++idx)
        saved_light[idx] = xport_gte_read_control(8 + idx);
    for (idx = 0; idx < 5; ++idx)
        saved_rotation[idx] = xport_gte_read_control(idx);
    for (idx = 0; idx < 5; ++idx)
        xport_gte_write_control(8 + idx, saved_rotation[idx]);
    product = (uint32)(sint16)saved_rotation[0] * 2560;
    light[0] = ((uint32)((sint32)product >> 12) & 65535) | (saved_rotation[1] & 0xFFFF0000);
    product = (uint32)((sint32)saved_rotation[0] >> 16) * 2560;
    light[1] = (saved_rotation[3] & 65535) | ((uint32)((sint32)product >> 12) << 16);
    light[2] = (saved_rotation[2] & 65535) | (saved_rotation[3] & 0xFFFF0000);
    product = (uint32)(sint16)saved_rotation[1] * 2560;
    light[3] = ((uint32)((sint32)product >> 12) & 65535) | (saved_rotation[2] & 0xFFFF0000);
    xport_gte_write_control(8, light[0]);
    xport_gte_write_control(9, light[1]);
    xport_gte_write_control(10, light[2]);
    xport_gte_write_control(11, light[3]);
    xport_gte_write_control(12, saved_rotation[4]);
    for (idx = 0; idx < 5; ++idx)
        rotation[idx] = PackModelRotationWord(matrix, idx);
    for (idx = 0; idx < 5; ++idx)
        xport_gte_write_control(idx, rotation[idx]);
    uint32 eye_x = -(uint32)pos->x;
    uint32 eye_y = -(uint32)pos->y;
    uint32 eye_z = -(uint32)pos->z;
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
    xport_gte_write_control(10, ONE);
    xport_gte_write_control(11, 0);
    xport_gte_write_control(12, ONE);
    while (count)
    {
        uint32 vertex_z = (uint16)vertices->vz | ((uint32)(uint16)vertices->pad << 16);
        uint32 vertex_xy = (uint16)vertices->vx | ((uint32)(uint16)vertices->vy << 16);
        normal_xy = (uint16)normals->vx | ((uint32)(uint16)normals->vy << 16);
        normal_z = (uint16)normals->vz | ((uint32)(uint16)normals->pad << 16);
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
        xport_gte_execute(0x198003D);
        reflected_xy = xport_gte_read_data(9) & 65535;
        reflected_xy |= xport_gte_read_data(10) << 16;
        reflected_z = xport_gte_read_data(11);
        xport_gte_write_data(0, reflected_xy - delta_xy);
        xport_gte_write_data(1, reflected_z - delta_z);
        xport_gte_execute(0x180001);
        scr = xport_gte_read_data(14);
        uv = (((scr >> 5) + 32) & 63) | (((scr >> 13) + 0x2000) & 0x3F00);
        combined_uv = uv + uv_base;
        if ((uv ^ combined_uv) & (uv_base ^ combined_uv) & 0x80000000)
        {
            // Stop before the store as the original signed ADD traps
            GameDiagnostic(("Original ADD overflow at 800816E8: uv=%08X base=%08X\n", uv, uv_base));
            abort();
        }
        GetModelLightingVal(output).uv = combined_uv;
        ++vertices;
        ++normals;
        ++output;
        --count;
    }
    for (idx = 0; idx < 3; ++idx)
        xport_gte_write_control(5 + idx, (uint32)saved_translation.comp[idx]);
    for (idx = 0; idx < 3; ++idx)
        xport_gte_write_control(24 + idx, saved_scr[idx]);
    for (idx = 0; idx < 5; ++idx)
        xport_gte_write_control(8 + idx, saved_light[idx]);
    for (idx = 0; idx < 5; ++idx)
        xport_gte_write_control(idx, saved_rotation[idx]);
}

static uint32 CalculateOffsetVertexClip(sint32 x, sint32 y, sint32 z, const ModelViewport &bounds)
{
    uint32 flags = (bounds.right < x) | ((x < bounds.left) << 1) | ((bounds.bottom < y) << 2) | ((y < bounds.top) << 3) | ((z < bounds.near_depth) << 4) | ((bounds.far_depth < z) << 5);
    if (z < 0)
        flags ^= 15;
    return flags;
}

uint32 ProjectOffsetModelVertices(const SVECTOR *vertices, uint32 count, const Vector3 *translation)
{
    ModelViewport bounds;
    sint32 depth, x, y;
    const ModelViewport *viewport;
    uint32 bound_word, aggregate = 0xFFFF;
    uint32 tx, ty, tz, vertex_idx;
    uint32 packed_xy, input_depth, flags, next_xy, next_depth;
    XY scr;
    uint32 output, pair, interpolation;
    const SVECTOR *references;
    uint32 idx0, idx1, mac0, mac1, len;
    XY scr0, scr1;
    sint32 dx, dy, nx, ny;
    uint32 offset_xy;
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
    tx = translation->comp[0];
    ty = translation->comp[1];
    tz = translation->comp[2];
    xport_gte_write_control(5, 0);
    xport_gte_write_control(6, 0);
    xport_gte_write_control(7, 0);
    vertex_idx = 0;
    packed_xy = (uint16)vertices->vx | ((uint32)(uint16)vertices->vy << 16);
    input_depth = ((uint16)vertices->vz | ((uint32)(uint16)vertices->pad << 16)) + tz;
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
        next_xy = count ? ((uint16)vertices->vx | ((uint32)(uint16)vertices->vy << 16)) : 0;
        next_depth = count ? ((uint16)vertices->vz | ((uint32)(uint16)vertices->pad << 16)) + tz : 0;
        next_xy = ((next_xy + tx) & 0xFFFF) | (((uint32)((sint32)next_xy >> 16) + ty) << 16);
        x = xport_gte_read_data(9);
        y = xport_gte_read_data(10);
        depth = xport_gte_read_data(11);
        ModelProjectedVertex &vertex = GetModelProjVertex(vertex_idx);
        vertex.xy = ((uint32)x & 0xFFFF) | ((uint32)y << 16);
        scr.xy = xport_gte_read_data(14);
        vertex.depth_flags = (vertex.depth_flags & 0xFFFF0000) | (uint16)depth;
        vertex.scr = scr.xy;
        flags = CalculateOffsetVertexClip(scr.x, scr.y, depth, bounds);
        if ((sint32)xport_gte_read_flag() < 0)
            flags |= 0x40;
        flags |= (flags << 8) ^ 0xFF00;
        vertex.depth_flags = (vertex.depth_flags & 65535) | (flags << 16);
        aggregate &= flags;
        ++vertex_idx;
        packed_xy = next_xy;
        input_depth = next_depth;
        if (!count)
        {
            model_render_state.generated_vertex_idx = vertex_idx;
            model_render_state.generated_cursor_valid = true;
            model_render_state.invalid_generated_cursor = 0;
            return aggregate;
        }
    }
    --vertices;
    ++count;
    xport_gte_write_control(0, 0x28F5);
    xport_gte_write_control(1, 0);
    xport_gte_write_control(2, ONE);
    xport_gte_write_control(3, 0);
    xport_gte_write_control(4, ONE);
    xport_gte_write_control(24, 0);
    xport_gte_write_control(25, 0);
    references = vertices;
    output = vertex_idx;
    do
    {
        pair = (uint16)references->vx | ((uint32)(uint16)references->vy << 16);
        interpolation = (uint16)references->vz | ((uint32)(uint16)references->pad << 16);
        idx0 = (pair & 0xFFFF) >> 3;
        scr0.xy = GetModelProjVertex(idx0).scr;
        depth = (sint16)GetModelProjVertex(idx0).depth_flags;
        idx1 = pair >> 19;
        scr1.xy = GetModelProjVertex(idx1).scr;
        oy = scr0.y;
        ox = scr0.x;
        dx = scr1.x - ox;
        dy = oy - scr1.y;
        xport_gte_write_data(9, dx);
        xport_gte_write_data(10, dy);
        xport_gte_execute(0xA00428);
        mac0 = xport_gte_read_data(25);
        mac1 = xport_gte_read_data(26);
        len = LookupDistanceRoot(mac0 + mac1);
        ny = DivideSignedWord(dx * ONE, len);
        nx = DivideSignedWord(dy * ONE, len);
        xport_gte_write_data(9, nx);
        xport_gte_write_data(10, ny);
        xport_gte_write_data(8, interpolation);
        xport_gte_execute(0x198003D);
        mac0 = xport_gte_read_data(25);
        mac1 = xport_gte_read_data(26);
        offset_xy = (mac0 & 0xFFFF) | (mac1 << 16);
        xport_gte_write_data(0, offset_xy);
        xport_gte_write_data(1, depth);
        xport_gte_execute(0x180001);
        scr.xy = xport_gte_read_data(14);
        x = scr.x + ox;
        y = scr.y + oy;
        flags = CalculateOffsetVertexClip(x, y, depth, bounds);
        flags |= (flags << 8) ^ 0xFF00;
        aggregate &= flags;
        ModelProjectedVertex &vertex = GetModelProjVertex(output);
        vertex.scr = ((uint32)x & 0xFFFF) | ((uint32)y << 16);
        vertex.depth_flags = (flags << 16) | ((uint32)depth & 0xFFFF);
        // Reference records do not write transformed XY in the original
        ++references;
        ++output;
        --count;
    } while (count);
    viewport = model_render_state.viewport;
    bound_word = viewport->center_x | ((uint32)viewport->center_y << 16);
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
    uint32 vertex_xy = (uint16)vertices->vx | ((uint32)(uint16)vertices->vy << 16);
    uint32 vertex_z = (uint16)vertices->vz | ((uint32)(uint16)vertices->pad << 16);
    do
    {
        xport_gte_write_data(0, vertex_xy);
        xport_gte_write_data(1, vertex_z);
        ++vertices;
        --count;
        xport_gte_execute(0x180001);
        uint32 next_xy = count ? ((uint16)vertices->vx | ((uint32)(uint16)vertices->vy << 16)) : 0;
        uint32 next_z = count ? ((uint16)vertices->vz | ((uint32)(uint16)vertices->pad << 16)) : 0;
        sint32 x = xport_gte_read_data(9);
        sint32 y = xport_gte_read_data(10);
        sint32 z = xport_gte_read_data(11);
        ModelProjectedVertex &vertex = GetModelProjVertex(vertex_idx);
        vertex.xy = ((uint32)x & 0xFFFF) | ((uint32)y << 16);
        XY scr;
        scr.xy = xport_gte_read_data(14);
        vertex.depth_flags = (vertex.depth_flags & 0xFFFF0000) | (uint16)z;
        vertex.scr = scr.xy;
        uint32 clip = CalculateOffsetVertexClip(scr.x, scr.y, z, bounds);
        clip |= (clip << 8) ^ 0xFF00;
        vertex.depth_flags = (vertex.depth_flags & 65535) | (clip << 16);
        aggregate &= clip;
        ++vertex_idx;
        vertex_xy = next_xy;
        vertex_z = next_z;
    } while (count);
    model_render_state.generated_vertex_idx = vertex_idx;
    model_render_state.generated_cursor_valid = true;
    model_render_state.invalid_generated_cursor = 0;
    return aggregate;
}

sint32 ProjectWorldPoint(const Vector3 *pos, XY *scr)
{
    FUNCTION_MARKER(0x80025248, "SLUS_003.73");
    uint32 x = (pos->x >> 12) - (uint32)GetModelRenderView().pos.x;
    uint32 y = (pos->y >> 12) - (uint32)GetModelRenderView().pos.y;
    uint32 z = (pos->z >> 12) - (uint32)GetModelRenderView().pos.z;
    xport_gte_write_data(0, (x & 0xFFFF) | (y << 16));
    xport_gte_write_data(1, z);
    xport_gte_execute(0x180001);
    scr->xy = xport_gte_read_data(14);
    return xport_gte_read_data(27);
}

void LoadProjIr(const ShortVector3 &v)
{
    for (uint32 i = 0; i < 3; ++i)
        xport_gte_write_data(9 + i, (uint16)v[i]);
}

void StoreProjIr(ShortVector3 &v)
{
    for (uint32 i = 0; i < 3; ++i)
        v[i] = xport_gte_read_data(9 + i);
}

void CrossProjIr(const ShortVector3 &vector)
{
    FUNCTION_MARKER(0x80085D30, "SLUS_003.73");
    const uint16 x = vector.x, y = vector.y, z = vector.z;
    (void)xport_gte_read_data(9);
    xport_gte_write_control(0, x);
    xport_gte_write_control(2, y);
    xport_gte_write_control(4, z);
    xport_gte_execute(0x178000C);
}

void LoadModelRotationAndTransformIr(const MATRIX &matrix)
{
    uint32 words[5], axis;
    FUNCTION_MARKER(0x80085C94, "SLUS_003.73");
    for (axis = 0; axis < 5; ++axis)
        words[axis] = PackModelRotationWord(matrix, axis);
    // Original reads IR1 before loading the control bank
    (void)xport_gte_read_data(9);
    for (axis = 0; axis < 5; ++axis)
        xport_gte_write_control(axis, words[axis]);
    xport_gte_execute(0x49E012);
}

uint32 MeasureProjIrLen()
{
    FUNCTION_MARKER(0x80085D14, "SLUS_003.73");
    xport_gte_execute(0xA00428);
    uint32 x = xport_gte_read_data(25);
    uint32 y = xport_gte_read_data(26);
    uint32 z = xport_gte_read_data(27);
    return LookupDistanceRoot(x + y + z);
}

uint32 NormalizeProjIr()
{
    FUNCTION_MARKER(0x80085D64, "SLUS_003.73");
    Vector3 saved, normalized;
    for (uint32 axis = 0; axis < 3; ++axis)
        saved.comp[axis] = xport_gte_read_data(9 + axis);
    uint32 len = MeasureProjIrLen();
    for (uint32 axis = 0; axis < 3; ++axis)
        normalized.comp[axis] = DivideSignedWord((uint32)saved.comp[axis] << 12, len);
    for (uint32 axis = 0; axis < 3; ++axis)
        xport_gte_write_data(9 + axis, (uint32)normalized.comp[axis]);
    return len;
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
    uint32 packed = (uint16)input->vx | ((uint32)(uint16)input->vy << 16);
    uint32 tagged_z = (uint16)input->vz | ((uint32)(uint16)input->pad << 16);
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
            uint32 reference_idx = model_projection_vertex_count - 1 - (packed >> 3);
            const ModelProjectedVertex &cached = GetModelProjVertex(reference_idx);
            uint32 scr = cached.scr;
            uint32 depth_flags = cached.depth_flags;
            uint32 xy = cached.xy;
            ModelProjectedVertex &vertex = GetModelProjVertex(vertex_idx);
            vertex.scr = scr;
            vertex.depth_flags = depth_flags;
            vertex.xy = xy;
            common &= depth_flags >> 16;
            packed = count ? ((uint16)input->vx | ((uint32)(uint16)input->vy << 16)) : 0;
            tagged_z = count ? ((uint16)input->vz | ((uint32)(uint16)input->pad << 16)) : 0;
        }
        else
        {
            packed = count ? ((uint16)input->vx | ((uint32)(uint16)input->vy << 16)) : 0;
            tagged_z = count ? ((uint16)input->vz | ((uint32)(uint16)input->pad << 16)) : 0;
            uint32 ir_x = xport_gte_read_data(9);
            uint32 ir_y = xport_gte_read_data(10);
            sint32 depth = xport_gte_read_data(11);
            uint32 xy = (ir_x & 65535) | (ir_y << 16);
            XY scr;
            scr.xy = xport_gte_read_data(14);
            uint32 flags = CalculateOffsetVertexClip(scr.x, scr.y, depth, bounds);
            flags |= (flags << 8) ^ 0xFF00;
            common &= flags;
            uint32 depth_flags = (flags << 16) | ((uint32)depth & 0xFFFF);
            ModelProjectedVertex &vertex = GetModelProjVertex(vertex_idx);
            vertex.scr = scr.xy;
            vertex.depth_flags = depth_flags;
            vertex.xy = xy;
            if (tag & 1)
            {
                ModelProjectedVertex &cached = GetModelProjVertex(cache_idx);
                cached.scr = scr.xy;
                cached.depth_flags = depth_flags;
                cached.xy = xy;
                --cache_idx;
            }
        }
        ++vertex_idx;
    } while (count);
    model_render_state.cache_index = cache_idx;
    model_render_state.generated_vertex_idx = vertex_idx;
    model_render_state.generated_cursor_valid = true;
    model_render_state.invalid_generated_cursor = 0;
    return common;
}

sint32 ProjectAndClipWorldLine(const Vector3 &start, const Vector3 &end, ProjectedWorldLine &proj)
{
    const ModelRenderView &camera = GetConfiguredModelRenderView();

    struct LineClipVertex
    {
        Vector3 pos;
        XY scr;
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
    camera_pos = camera.pos;
    packed = ((start.x >> 12) - (uint32)camera_pos.x) & 0xFFFF;
    packed |= ((start.y >> 12) - (uint32)camera_pos.y) << 16;
    z = (start.z >> 12) - (uint32)camera_pos.z;
    xport_gte_write_data(0, packed);
    xport_gte_write_data(1, z);
    xport_gte_execute(0x180001);
    packed = ((end.x >> 12) - (uint32)camera_pos.x) & 0xFFFF;
    packed |= ((end.y >> 12) - (uint32)camera_pos.y) << 16;
    z = (end.z >> 12) - (uint32)camera_pos.z;
    vertex[0].flags = xport_gte_read_control(31);
    vertex[0].scr.xy = xport_gte_read_data(14);
    for (axis = 0; axis < 3; ++axis)
        vertex[0].pos.comp[axis] = xport_gte_read_data(25 + axis);
    xport_gte_write_data(0, packed);
    xport_gte_write_data(1, z);
    xport_gte_execute(0x180001);
    proj.scr[0] = vertex[0].scr;
    proj.depth[0] = vertex[0].pos.z;
    vertex[1].flags = xport_gte_read_control(31);
    vertex[1].scr.xy = xport_gte_read_data(14);
    for (axis = 0; axis < 3; ++axis)
        vertex[1].pos.comp[axis] = xport_gte_read_data(25 + axis);
    clip_status = vertex[0].flags | vertex[1].flags;
    if ((sint32)clip_status >= 0)
    {
        dx = (sint16)(vertex[1].scr.x - vertex[0].scr.x);
        clip_status = dx;
        if (dx < 1024)
        {
            // The lower X bound branch also changes the status used for rejection
            clip_status = vertex[0].scr.y;
            if (dx >= -1023)
            {
                dy = vertex[1].scr.y - vertex[0].scr.y;
                clip_status = dy;
                if (dy < 512 && dy >= -511)
                {
                    proj.scr[1] = vertex[1].scr;
                    proj.depth[1] = vertex[1].pos.z;
                    result = vertex[0].pos.z < vertex[1].pos.z ? (uint32)vertex[0].pos.z >> 2 : (uint32)vertex[1].pos.z >> 2;
                    return result < 4096 ? result : 4095;
                }
            }
        }
    }
    if (((clip_status >> 22) & 7) || (sint32)((uint32)vertex[0].pos.z & vertex[1].pos.z) < 0)
        return -1;
    xport_gte_write_data(0, ((uint32)vertex[0].pos.x & 0xFFFF) | ((uint32)vertex[0].pos.y << 16));
    xport_gte_write_data(1, (uint32)vertex[0].pos.z);
    xport_gte_write_data(2, ((uint32)vertex[1].pos.x & 0xFFFF) | ((uint32)vertex[1].pos.y << 16));
    xport_gte_write_data(3, (uint32)vertex[1].pos.z);
    LoadModelLineClipMatrices();
    xport_gte_execute(0x4A2412);
    vertex[0].flags = (xport_gte_read_control(31) >> 19) & 0x38;
    for (axis = 0; axis < 3; ++axis)
        vertex[0].plane_dist[axis] = xport_gte_read_data(25 + axis);
    xport_gte_execute(0x4C6412);
    vertex[0].flags |= (xport_gte_read_control(31) >> 22) & 7;
    for (axis = 0; axis < 3; ++axis)
        vertex[0].plane_dist[3 + axis] = xport_gte_read_data(25 + axis);
    xport_gte_execute(0x4AA412);
    vertex[1].flags = (xport_gte_read_control(31) >> 19) & 0x38;
    for (axis = 0; axis < 3; ++axis)
        vertex[1].plane_dist[axis] = xport_gte_read_data(25 + axis);
    xport_gte_execute(0x4CE412);
    vertex[1].flags |= (xport_gte_read_control(31) >> 22) & 7;
    for (axis = 0; axis < 3; ++axis)
        vertex[1].plane_dist[3 + axis] = xport_gte_read_data(25 + axis);
    if ((vertex[0].flags & vertex[1].flags) || ((vertex[0].flags | vertex[1].flags) & 0x20))
        return -1;
    plane_idx = 0;
    for (mask = 32; mask; mask >>= 1, ++plane_idx)
    {
        if ((vertex[0].flags ^ vertex[1].flags) & mask)
        {
            uint32 dist = vertex[0].plane_dist[plane_idx], other_dist = vertex[1].plane_dist[plane_idx];
            uint32 t = DivideSignedWord(dist << 12, dist - other_dist);
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
        return -1;
    for (axis = 0; axis < 5; ++axis)
        saved_rotation[axis] = xport_gte_read_control(axis);
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
    vertex[0].scr.xy = xport_gte_read_data(14);
    vertex[0].depth = xport_gte_read_data(11);
    xport_gte_write_data(0, 4096 - t_max);
    xport_gte_write_data(1, t_max);
    vertex[0].depth >>= 3;
    xport_gte_execute(0x180001);
    proj.scr[0] = vertex[0].scr;
    proj.depth[0] = vertex[0].depth;
    vertex[1].scr.xy = xport_gte_read_data(14);
    vertex[1].depth = xport_gte_read_data(11);
    proj.scr[1] = vertex[1].scr;
    vertex[1].depth >>= 3;
    proj.depth[1] = vertex[1].depth;
    for (axis = 0; axis < 5; ++axis)
        xport_gte_write_control(axis, saved_rotation[axis]);
    result = vertex[0].depth < vertex[1].depth ? vertex[0].depth >> 2 : vertex[1].depth >> 2;
    return result < 4096 ? result : 4095;
}

void ScaleProjIr(uint32 scale)
{
    FUNCTION_MARKER(0x80085DBC, "SLUS_003.73");
    (void)xport_gte_read_data(9);
    xport_gte_write_data(8, scale);
    xport_gte_execute(0x198003D);
}

uint32 ConfigureProjDepth(uint32 limit, uint32 base, uint32 extent)
{
    FUNCTION_MARKER(0x8007D76C, "SLUS_003.73");
    uint32 x = model_proj_cfg.scr.x;
    uint32 sq_x = x * x;
    uint32 y = model_proj_cfg.scr.y;
    uint32 sq_y = y * y;
    uint32 z = model_proj_cfg.scr.depth;
    uint32 sq_z = z * z;
    model_proj_cfg.near_depth = limit;
    model_proj_cfg.fog_start = base;
    uint32 end = base + extent;
    model_proj_cfg.far_depth = end;
    uint32 root = SquareRoot0(sq_x + sq_y + sq_z);
    end = model_proj_cfg.far_depth;
    uint32 product = end * root;
    uint32 divisor = model_proj_cfg.scr.depth;
    model_proj_cfg.max_distance = DivideSignedWord(product, divisor);
    uint32 result = extent < 4096;
    model_proj_cfg.fog_shift = 0;
    if (extent < 4096)
    {
        do
        {
            uint32 shift = model_proj_cfg.fog_shift + 1;
            model_proj_cfg.fog_shift = shift;
            result = extent < (4096 >> (shift & 31));
        } while (result);
    }
    else if (extent > 4096)
    {
        // TODO Original shift cycle cannot reach extents above 80000000
        do
        {
            uint32 shift = model_proj_cfg.fog_shift - 1;
            model_proj_cfg.fog_shift = shift;
            result = ((uint32)ONE << (-shift & 31)) < extent;
        } while (result);
    }
    return result;
}

void SetProjScrParameters(uint32 x, uint32 y, uint32 depth)
{
    FUNCTION_MARKER(0x8007D8C0, "SLUS_003.73");
    model_proj_cfg.scr.x = x;
    model_proj_cfg.scr.y = y;
    model_proj_cfg.scr.depth = depth;
}
