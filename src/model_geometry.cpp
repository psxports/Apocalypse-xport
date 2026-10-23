#include <stdlib.h>
#include <string.h>

#include "game_diagnostics.h"
#include "model_geometry.h"
#include "model_instance.h"
#include "resource_file_layouts.h"
#include "psx.h"
#include "global.h"
#include "scene_lifecycle.h"
#include "level_session.h"
#include "game_tick.h"
#include "game_lighting.h"
#include "resource_script.h"
#include "model_render_view.h"
#include "character.h"
#include "model_bank.h"
#include "effect_lifecycle.h"
#include "model_render.h"
#include "model_matrix.h"
#include "model_subdivision.h"
#include "model_lighting.h"
#include "model_projection.h"
#include "model_animation.h"
#include "model_clipping.h"
#include "angle_math.h"
#include "effect_object.h"
#include "texture_resources.h"
#include "collision_ray.h"
#include "model_bank_grid.h"
#include "visual_object.h"
#include "scripted_level_object.h"
#include "collision.h"
#include "player.h"
#include "collision_ray_internal.h"

struct ModelGeometryFile
{
    uint32 flags;
    uint32 vertex_count;
    uint32 normal_count;
    uint32 polygon_count;
    uint32 radius;
    uint32 bounds[3];
};

typedef char ModelGeometryFileSize[sizeof(ModelGeometryFile) == 32 ? 1 : -1];

struct ModelPolygonFile
{
    uint16 flags;
    uint16 stride;
    uint16 vertices[4];
    uint32 color;
    uint16 normal;
    uint16 surface;
};

typedef char ModelPolygonFileSize[sizeof(ModelPolygonFile) == 20 ? 1 : -1];

struct ModelTexturePolygonFile
{
    ModelPolygonFile polygon;
    uint32 texture;
    uint16 uv[4];
};

typedef char ModelTexturePolygonFileSize[sizeof(ModelTexturePolygonFile) == 32 ? 1 : -1];

struct ModelWindowPolygonFile
{
    ModelTexturePolygonFile polygon;
    uint32 window;
};

typedef char ModelWindowPolygonFileSize[sizeof(ModelWindowPolygonFile) == 36 ? 1 : -1];

typedef char ModelVectorBoundarySize[sizeof(SVECTOR) == 8 ? 1 : -1];

static void BuildModelPolygonPackets(const ModelPolygon *descriptor, uint32 count);
static MATRIX unlit_matrix = {0};

static void BuildModelPolygonPackets(const ModelPolygon *desc, uint32 count);
static void LoadProjEffectVertex(uint32 slot, const ShortVector3 &center, const ShortVector3 &basis0, const ShortVector3 &basis1, sint32 sign0, sint32 sign1);
static void TransformProjIr(const MATRIX &matrix);
static void AccumulateProjVector(const ShortVector3 &vector, uint32 scale);
static void LoadCorner(uint32 vertex, const ShortVector3 &center, const ShortVector3 &edge0, const ShortVector3 &edge1, bool add0, bool add1);

static bool CheckGeometrySpan(uint32 bytes, uint32 pos, uint32 count, uint32 stride)
{
    return pos <= bytes && count <= (bytes - pos) / stride;
}

void ReleaseModelGeometrySet(ModelGeometrySet &owner)
{
    for (uint32 i = 0; i < owner.count; ++i)
    {
        RetireModelMemory(const_cast<SVECTOR *>(owner.models[i].vertices));
        RetireModelMemory(const_cast<SVECTOR *>(owner.models[i].normals));
        RetireModelMemory(owner.models[i].polys);
    }
    RetireModelMemory(owner.models);
    owner.models = 0;
    owner.count = 0;
}

uint32 GetModelPolygonWindow(const ModelPolygon &poly)
{
    return poly.window_alias ? *poly.window_alias : poly.window;
}

uint32 PackModelPolygonUv(const ModelPolygon &poly, uint32 word)
{
    switch (word)
    {
        case 0:
            return poly.uv[0].uv | ((uint32)poly.clut << 16);
        case 1:
            return poly.uv[1].uv | ((uint32)poly.tpage << 16);
        case 2:
            return poly.uv[2].uv | ((uint32)poly.uv[3].uv << 16);
    }
    GameDiagnostic(("Invalid model polygon UV word %u\n", word));
    abort();
    return 0;
}

static bool CheckModelGeometryInput(const uint8 *data, uint32 bytes, const TextureReferences &refs, uint32 &table, uint32 &count)
{
    if (!data || bytes < 16)
        return false;
    const ModelFileHeader &header = *(const ModelFileHeader *)data;
    const uint32 object_count = header.scene_count;
    if (!CheckGeometrySpan(bytes, 12, object_count, 36))
        return false;
    table = 12 + object_count * 36;
    if (!CheckGeometrySpan(bytes, table, 1, 4))
        return false;
    count = *(const uint32 *)(data + table);
    table += 4;
    if (!CheckGeometrySpan(bytes, table, count, 4) || count > 0xFFFFFFFF / sizeof(ModelGeometry))
        return false;
    const uint32 *offsets = (const uint32 *)(data + table);
    uint32 next = table + count * 4;
    for (uint32 i = 0; i < count; ++i)
    {
        const uint32 start = offsets[i];
        if (start != next || !CheckGeometrySpan(bytes, start, 1, 32))
            return false;
        const ModelGeometryFile &file = *(const ModelGeometryFile *)(data + start);
        const uint32 nv = file.vertex_count;
        const uint32 nn = file.normal_count;
        const uint32 np = file.polygon_count;
        if (nv > 0xFFFFFFFF / sizeof(SVECTOR) || nn > 0xFFFFFFFF / sizeof(SVECTOR) || np > 0xFFFFFFFF / sizeof(ModelPolygon))
            return false;
        uint32 pos = start + 32;
        if (!CheckGeometrySpan(bytes, pos, nv, 8))
            return false;
        pos += nv * 8;
        if (!CheckGeometrySpan(bytes, pos, nn, 8))
            return false;
        pos += nn * 8;
        if (!CheckGeometrySpan(bytes, pos, np, 20))
            return false;
        for (uint32 p = 0; p < np; ++p)
        {
            if (!CheckGeometrySpan(bytes, pos, 1, 20))
                return false;
            const ModelPolygonFile &polygon = *(const ModelPolygonFile *)(data + pos);
            const uint16 flags = polygon.flags;
            const uint16 stride = polygon.stride;
            if ((stride & 3) || stride < 20 || !CheckGeometrySpan(bytes, pos, 1, stride))
                return false;
            const uint32 vertices = flags & 16 ? 3 : 4;
            for (uint32 v = 0; v < vertices; ++v)
                if (polygon.vertices[v] >= nv)
                    return false;
            if (polygon.normal >= nn)
                return false;
            if ((flags & 3) == 1)
                return false;
            if ((flags & 3) == 3)
            {
                if (stride < 32)
                    return false;
                const uint32 idx = ((const ModelTexturePolygonFile *)(data + pos))->texture;
                if (idx >= refs.count || !refs.entries || !refs.entries[idx] || !refs.entries[idx]->loaded)
                    return false;
            }
            if ((flags & 32) && !CheckGeometrySpan(bytes, pos, 1, 36))
                return false;
            pos += stride;
        }
        next = pos;
    }
    return true;
}

bool DecodeModelGeometrySet(ModelGeometrySet &owner, const uint8 *data, uint32 bytes, const TextureReferences &refs)
{
    uint32 table, count;
    if (owner.count || owner.models || !CheckModelGeometryInput(data, bytes, refs, table, count))
        return false;
    ModelGeometrySet decoded = {0, 0};
    decoded.models = count ? (ModelGeometry *)calloc(count, sizeof(ModelGeometry)) : 0;
    if (count && !decoded.models)
        return false;
    decoded.count = count;
    const uint32 *offsets = (const uint32 *)(data + table);
    for (uint32 i = 0; i < count; ++i)
    {
        ModelGeometry &model = decoded.models[i];
        const uint32 start = offsets[i];
        const ModelGeometryFile &file = *(const ModelGeometryFile *)(data + start);
        model.flags = file.flags;
        model.vertex_count = file.vertex_count;
        model.n_count = file.normal_count;
        model.poly_count = file.polygon_count;
        model.radius = file.radius;
        for (uint32 axis = 0; axis < 3; ++axis)
        {
            uint32 packed = file.bounds[axis];
            if (!(model.flags & 8))
            {
                const sint32 extent = (sint32)(packed + 4095) >> 12;
                packed = ((uint32)-extent << 16) | (uint32)extent;
            }
            model.bounds[axis].max = packed;
            model.bounds[axis].min = packed >> 16;
        }
        model.flags |= 8;
        SVECTOR *vertices = model.vertex_count ? (SVECTOR *)malloc(model.vertex_count * sizeof(SVECTOR)) : 0;
        SVECTOR *normals = model.n_count ? (SVECTOR *)malloc(model.n_count * sizeof(SVECTOR)) : 0;
        model.vertices = vertices;
        model.normals = normals;
        model.polys = model.poly_count ? (ModelPolygon *)calloc(model.poly_count, sizeof(ModelPolygon)) : 0;
        if ((model.vertex_count && !model.vertices) || (model.n_count && !model.normals) || (model.poly_count && !model.polys))
        {
            ReleaseModelGeometrySet(decoded);
            return false;
        }
        uint32 pos = start + 32;
        if (model.vertex_count)
            memcpy(vertices, data + pos, model.vertex_count * sizeof(SVECTOR));
        pos += model.vertex_count * 8;
        if (model.n_count)
            memcpy(normals, data + pos, model.n_count * sizeof(SVECTOR));
        pos += model.n_count * 8;
        for (uint32 v = 0; v < model.vertex_count; ++v)
            if (vertices[v].pad & 2)
            {
                vertices[v].vx = (uint16)vertices[v].vy << 3;
                vertices[v].vy = 0;
            }
        uint32 combined = 0, common = 0xFFFFFFFF;
        for (uint32 p = 0; p < model.poly_count; ++p)
        {
            ModelPolygon &poly = model.polys[p];
            poly.next = p + 1 < model.poly_count ? &model.polys[p + 1] : 0;
            const ModelPolygonFile &polygon = *(const ModelPolygonFile *)(data + pos);
            poly.flags = polygon.flags;
            poly.src_stride = polygon.stride;
            for (uint32 v = 0; v < 4; ++v)
                poly.v[v] = polygon.vertices[v] & 8191;
            poly.color = polygon.color;
            poly.n = polygon.normal & 8191;
            poly.surface = polygon.surface;
            if (!(poly.flags & 64))
                poly.flags ^= 128;
            if (!(poly.flags & 2048))
                poly.color &= 0xFFFFFF;
            if ((poly.flags & 3) == 3)
            {
                const ModelTexturePolygonFile &textured = *(const ModelTexturePolygonFile *)(data + pos);
                const TextureResource &tex = *refs.entries[textured.texture];
                for (uint32 v = 0; v < 4; ++v)
                {
                    poly.uv[v].uv = textured.uv[v];
                    if (!(poly.flags & 32))
                        poly.uv[v].uv += tex.uv[0].uv;
                }
                poly.clut = tex.clut;
                poly.tpage = tex.tpage;
                if (poly.flags & 32)
                    poly.window = tex.window;
            }
            else if (poly.flags & 32)
            {
                if (poly.src_stride == 20 && p + 1 < model.poly_count)
                    poly.window_alias = &model.polys[p + 1].color;
                else
                    poly.window = ((const ModelWindowPolygonFile *)(data + pos))->window;
            }
            if (poly.flags & 16)
                poly.v[3] = poly.v[2];
            combined |= poly.flags;
            common &= poly.surface;
            pos += poly.src_stride;
        }
        if (common & 1)
            model.flags |= 16;
        if (!(combined & 192))
            model.flags |= 32;
        if (combined & 4096)
            model.flags |= 64;
    }
    owner = decoded;
    return true;
}

static void RenderClippedModel(const ModelGeometry &geometry, bool translated)
{
    model_render_state.translated = translated;
    if (ProjectModelClippedVertices(geometry.vertices, geometry.vertex_count) & 0xBF)
        return;
    SetColorMatrix(&game_lighting.colors);
    SetFarColor(game_lighting.far_color.r, game_lighting.far_color.g, game_lighting.far_color.b);
    SetBackColor(game_lighting.background_color.r, game_lighting.background_color.g, game_lighting.background_color.b);
    BuildModelPolygonPackets(geometry.polys, geometry.poly_count);
}

void ModelGeometry::RenderProjectionEffect(const MATRIX &matrix, const SVECTOR &vector, const Vector3 *pos, const ShortVector3 &dir_vector, uint32 color) const
{
    FUNCTION_MARKER(0x8007A8F8, "SLUS_003.73");
    MATRIX forward, inverse, diagonal, proj_matrix;
    MATRIX alignment = {0};
    ShortVector3 center, extent, n, cross;
    ShortVector3 axis = {0, 0, 0};
    ShortVector3 basis0, basis1, coeff, proj, edge0, edge1, dir;
    uint32 i, scale, depth;
    sint32 squares[3];
    uint8 *packet;
    sint32 avg, numerator, dist;
    sint16 len;
    ModelBounds bounds[3];
    for (i = 0; i < 3; ++i)
        bounds[i] = this->bounds[i];
    for (i = 0; i < 3; ++i)
    {
        sint32 max = (uint16)bounds[i].max;
        sint32 min = bounds[i].min;
        sint32 half = (max - min) >> 1;
        extent[i] = (half * (i == 1 ? 11 : 5)) >> (i == 1 ? 3 : 2);
    }
    // Original bounds use an unsigned maximum and a signed minimum
    center.x = ((uint16)bounds[0].max + bounds[0].min) >> 1;
    center.y = ((uint16)this->bounds[1].max + this->bounds[1].min) >> 1;
    center.z = ((uint16)this->bounds[2].max + this->bounds[2].min) >> 1;
    LoadProjIr(center);
    LoadModelRotationAndTransformIr(matrix);
    StoreProjIr(center);
    for (i = 0; i < 3; ++i)
    {
        uint16 offset;
        offset = matrix.t[i];
        center[i] = center[i] + offset;
    }
    avg = (extent.x + extent.y + extent.z) / 3;
    numerator = avg * ONE;
    /* Original 85C04 copies only the nine rotation halfwords */
    CopyModelRotationMatrix(matrix, forward);
    TransposeModelMatrix(forward, inverse);
    ClearModelRotationMatrix(diagonal);
    diagonal.m[0][0] = DivideSignedWord(numerator, extent.x);
    diagonal.m[1][1] = DivideSignedWord(numerator, extent.y);
    diagonal.m[2][2] = DivideSignedWord(numerator, extent.z);
    for (i = 0; i < 3; ++i)
        dir[i] = dir_vector[i];
    LoadProjIr(dir);
    LoadModelRotationAndTransformIr(inverse);
    (void)NormalizeProjIr();
    LoadModelRotationAndTransformIr(diagonal);
    StoreProjIr(n);
    len = MeasureProjIrLen();
    for (i = 0; i < 3; ++i)
        squares[i] = xport_gte_read_data(25 + i);
    if (squares[1] >= squares[0] && squares[2] >= squares[0])
        axis.x = ONE;
    else if (squares[0] >= squares[1] && squares[2] >= squares[1])
        axis.y = ONE;
    else
        axis.z = ONE;
    LoadProjIr(n);
    CrossProjIr(axis);
    StoreProjIr(cross);
    (void)NormalizeProjIr();
    scale = DivideSignedWord(extent.x * extent.y, avg);
    scale *= extent.z;
    scale *= len;
    scale = DivideSignedWord(scale, avg) >> 12;
    LoadModelRotationAndTransformIr(diagonal);
    LoadModelRotationAndTransformIr(forward);
    CrossProjIr(dir);
    ScaleProjIr(scale);
    StoreProjIr(basis0);
    LoadProjIr(n);
    CrossProjIr(cross);
    (void)NormalizeProjIr();
    LoadModelRotationAndTransformIr(diagonal);
    LoadModelRotationAndTransformIr(forward);
    CrossProjIr(dir);
    ScaleProjIr(scale);
    StoreProjIr(basis1);
    xport_gte_write_data(0, (uint16)vector.vx | ((uint32)(uint16)vector.vy << 16));
    xport_gte_write_data(1, (uint16)vector.vz | ((uint32)(uint16)vector.pad << 16));
    for (i = 0; i < 3; ++i)
        alignment.m[0][i] = dir[i];
    TransformProjIr(alignment);
    dist = (sint32)xport_gte_read_data(25);
    static const sint32 minimum_proj_alignment = 1024;
    if (dist < minimum_proj_alignment)
        return;
    for (i = 0; i < 3; ++i)
    {
        proj_matrix.m[0][i] = (pos->comp[i] >> 12) - center[i];
        proj_matrix.m[1][i] = basis0[i];
        proj_matrix.m[2][i] = basis1[i];
    }
    TransformProjIr(proj_matrix);
    StoreProjIr(coeff);
    if (coeff.x > 0)
        return;
    const ModelRenderView &camera = GetConfiguredModelRenderView();
    for (i = 0; i < 3; ++i)
        center[i] = center[i] - (uint16)(uint32)camera.pos.comp[i];
    LoadProjIr(dir);
    AccumulateProjVector(center, DivideSignedWord(coeff.x * ONE, dist));
    StoreProjIr(proj);
    LoadProjIr(dir);
    AccumulateProjVector(basis0, DivideSignedWord(-coeff.y * ONE, dist));
    StoreProjIr(edge0);
    LoadProjIr(dir);
    AccumulateProjVector(basis1, DivideSignedWord(-coeff.z * ONE, dist));
    StoreProjIr(edge1);
    SetRotMatrix(&GetConfiguredModelRenderView().projection);
    for (i = 0; i < 3; ++i)
        xport_gte_write_control(5 + i, 0);
    LoadProjEffectVertex(0, proj, edge0, edge1, -1, -1);
    LoadProjEffectVertex(2, proj, edge0, edge1, 1, -1);
    LoadProjEffectVertex(4, proj, edge0, edge1, -1, 1);
    xport_gte_execute(0x280030);
    for (i = 0; i < 3; ++i)
    {
        GetModelProjVertex(i).scr = xport_gte_read_data(12 + i);
    }
    LoadProjEffectVertex(0, proj, edge0, edge1, 1, 1);
    xport_gte_execute(0x180001);
    GetModelProjVertex(3).scr = xport_gte_read_data(14);
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
        poly->p[i].xy.xy = GetModelProjVertex(i).scr;
    setRGB0(poly, color, color >> 8, color >> 16);
    const TextureResource *tex = EffectObject::GetShadowTex();
    if (!tex)
    {
        GameDiagnostic(("Missing projection shadow texture\n"));
        abort();
    }
    setUV4(poly, tex->uv[0].u, tex->uv[0].v, tex->uv[1].u - 1, tex->uv[1].v, tex->uv[2].u, tex->uv[2].v - 1, tex->uv[3].u - 1, tex->uv[3].v - 1);
    poly->p[0].attr = tex->clut;
    uint32 tpage = tex->tpage;
    poly->p[1].attr = (tpage & 0xFF9F) | 0x40;
    setaddr(poly, getaddr(scene_render.cur->ot + depth));
    setaddr(scene_render.cur->ot + depth, gpu_packet_link((uintptr_t)poly));
}

uint32 ModelGeometry::RenderShadow(const MATRIX &matrix, uint32 rgb, uint32 scale, sint32 depth_bias, const TextureResource &tex) const
{
    const ModelRenderView *camera;
    uint32 product, axis;
    MATRIX forward, inverse, diagonal;
    uint32 factor, depth, result;
    sint16 len;
    sint32 min, max, avg, numerator, squares[3];
    Vector3 extents;
    uint8 *packet;
    ShortVector3 center, relative, normalized, basis;
    ShortVector3 edge[2];
    ShortVector3 sel;
    FUNCTION_MARKER(0x8007B2E4, "SLUS_003.73");
    for (axis = 0; axis < 3; ++axis)
    {
        max = (uint16)bounds[axis].max;
        min = bounds[axis].min;
        product = ((max - min) >> 1) * scale;
        extents.comp[axis] = (sint32)product >> 12;
        center[axis] = (max + min) >> 1;
    }
    LoadProjIr(center);
    LoadModelRotationAndTransformIr(matrix);
    StoreProjIr(center);
    for (axis = 0; axis < 3; ++axis)
        center[axis] = center[axis] + (uint16)matrix.t[axis];
    camera = &GetConfiguredModelRenderView();
    for (axis = 0; axis < 3; ++axis)
        center[axis] = center[axis] - (uint16)(uint32)camera->pos.comp[axis];
    LoadProjIr(center);
    NormalizeProjIr();
    StoreProjIr(relative);
    for (axis = 0; axis < 3; ++axis)
        extents.comp[axis] = (sint16)extents.comp[axis];
    avg = (extents.x + extents.y + extents.z) / 3;
    numerator = avg * ONE;
    CopyModelRotationMatrix(matrix, forward);
    TransposeModelMatrix(forward, inverse);
    ClearModelRotationMatrix(diagonal);
    diagonal.m[0][0] = DivideSignedWord(numerator, extents.x);
    diagonal.m[1][1] = DivideSignedWord(numerator, extents.y);
    diagonal.m[2][2] = DivideSignedWord(numerator, extents.z);
    LoadProjIr(relative);
    LoadModelRotationAndTransformIr(inverse);
    NormalizeProjIr();
    LoadModelRotationAndTransformIr(diagonal);
    StoreProjIr(normalized);
    len = MeasureProjIrLen();
    for (axis = 0; axis < 3; ++axis)
        squares[axis] = xport_gte_read_data(25 + axis);
    sel.x = sel.y = sel.z = 0;
    if (squares[1] >= squares[0] && squares[2] >= squares[0])
        sel.x = ONE;
    else if (squares[0] >= squares[1] && squares[2] >= squares[1])
        sel.y = ONE;
    else
        sel.z = ONE;
    LoadProjIr(normalized);
    CrossProjIr(sel);
    StoreProjIr(basis);
    NormalizeProjIr();
    factor = DivideSignedWord(extents.x * extents.y, avg);
    factor *= extents.z;
    factor *= len;
    factor = DivideSignedWord(factor, avg);
    factor = (sint32)factor >> 12;
    LoadModelRotationAndTransformIr(diagonal);
    LoadModelRotationAndTransformIr(forward);
    CrossProjIr(relative);
    ScaleProjIr(factor);
    StoreProjIr(edge[0]);
    LoadProjIr(normalized);
    CrossProjIr(basis);
    NormalizeProjIr();
    LoadModelRotationAndTransformIr(diagonal);
    LoadModelRotationAndTransformIr(forward);
    CrossProjIr(relative);
    ScaleProjIr(factor);
    StoreProjIr(edge[1]);
    camera = &GetConfiguredModelRenderView();
    SetRotMatrix((MATRIX *)&camera->projection);
    xport_gte_write_control(5, 0);
    xport_gte_write_control(6, 0);
    xport_gte_write_control(7, 0);
    LoadCorner(0, center, edge[0], edge[1], false, false);
    LoadCorner(1, center, edge[0], edge[1], true, false);
    LoadCorner(2, center, edge[0], edge[1], false, true);
    xport_gte_execute(0x280030);
    GetModelProjVertex(0).scr = xport_gte_read_data(12);
    GetModelProjVertex(1).scr = xport_gte_read_data(13);
    GetModelProjVertex(2).scr = xport_gte_read_data(14);
    LoadCorner(0, center, edge[0], edge[1], true, true);
    xport_gte_execute(0x180001);
    GetModelProjVertex(3).scr = xport_gte_read_data(14);
    xport_gte_execute(0x168002E);
    depth = xport_gte_read_data(7) + depth_bias;
    if (depth >= 4096)
        depth = 4095;
    packet = scene_render.cursor;
    result = scene_render.limit - packet < (sint32)sizeof(POLY_FT4);
    if (result)
        return result;
    scene_render.cursor = packet + sizeof(POLY_FT4);
    POLY_FT4 *poly = (POLY_FT4 *)packet;
    poly->rgbc.rgbc = rgb;
    setPolyFT4(poly);
    setSemiTrans(poly, 1);
    for (axis = 0; axis < 4; ++axis)
        poly->p[axis].xy.xy = GetModelProjVertex(axis).scr;
    setUV4(poly, tex.uv[0].u, tex.uv[0].v, tex.uv[1].u - 1, tex.uv[1].v, tex.uv[2].u, tex.uv[2].v - 1, tex.uv[3].u - 1, tex.uv[3].v - 1);
    poly->p[0].attr = tex.clut;
    poly->p[1].attr = (tex.tpage & 0xFF9F) | 32;
    uint32 *bucket = scene_render.cur->ot + depth;
    setaddr(poly, getaddr(bucket));
    setaddr(bucket, gpu_packet_link((uintptr_t)poly));
    return *bucket;
}

void ModelGeometry::RenderProj() const
{
    FUNCTION_MARKER(0x8007F904, "SLUS_003.73");
    uint32 count = vertex_count;
    model_render_state.translated = true;
    uint32 flags = ProjectAndClipModelVertices(vertices, count);
    if (model_render_state.water_clip_enabled)
        UpdateModelWaterClipFlags(count);
    if (flags & 191)
        return;
    SetLightMatrix(&unlit_matrix);
    SetColorMatrix(&game_lighting.colors);
    SetFarColor(game_lighting.far_color.r, game_lighting.far_color.g, game_lighting.far_color.b);
    SetBackColor(game_lighting.background_color.r, game_lighting.background_color.g, game_lighting.background_color.b);
    BuildModelPolygonPackets(polys, poly_count);
}

void ModelGeometry::RenderOffsetScratch(const Vector3 *pos) const
{
    FUNCTION_MARKER(0x8007F9E0, "SLUS_003.73");
    model_render_state.translated = true;
    uint32 flags = ProjectOffsetScratchVertices(vertices, vertex_count, pos);
    if (flags & 0x60006000)
        return;
    SetFarColor(game_lighting.far_color.r, game_lighting.far_color.g, game_lighting.far_color.b);
    EmitScratchModelPackets(polys, poly_count);
}

void ModelGeometry::RenderTranslatedClipped() const
{
    FUNCTION_MARKER(0x8007FA7C, "SLUS_003.73");
    RenderClippedModel(*this, true);
}

void ModelGeometry::RenderTranslated() const
{
    FUNCTION_MARKER(0x8007FB34, "SLUS_003.73");
    uint32 count = vertex_count;
    model_render_state.translated = true;
    uint32 flags = ProjectModelReferenceVertices(vertices, count);
    if (model_render_state.water_clip_enabled)
        UpdateModelWaterClipFlags(count);
    if (!model_render_state.part_hidden && !(flags & 191))
    {
        BuildModelPolygonPackets(polys, poly_count);
    }
}

void ModelGeometry::RenderScratch() const
{
    FUNCTION_MARKER(0x8007FBD4, "SLUS_003.73");
    model_render_state.translated = true;
    uint32 flags = ProjectModelScratchVertices(vertices, vertex_count);
    if (model_render_state.part_hidden)
        return;
    if (flags & 0x60006000)
        return;
    EmitScratchModelPackets(polys, poly_count);
}

static void PackModelPolygonCommand(uint8 *packet, uint32 color, uint32 command, uint32 flags)
{
    FUNCTION_MARKER(0x80082188, "SLUS_003.73");
    P_TAG *header = (P_TAG *)packet;
    header->rgbc.rgbc = color | command | ((flags & 0x40) << 19);
}

static void ReadModelPolygonUvs(const ModelPolygon *desc, uint32 flags, uint32 vertices, uint32 uv[4])
{
    if (flags & 1)
    {
        uv[0] = PackModelPolygonUv(*desc, 0);
        uv[1] = PackModelPolygonUv(*desc, 1);
        uv[2] = PackModelPolygonUv(*desc, 2);
    }
    else
    {
        uv[0] = GetModelLightingVal(desc->v[0]).uv;
        uv[1] = GetModelLightingVal(desc->v[1]).uv;
        uv[0] |= model_render_state.draw.clut;
        uv[2] = GetModelLightingVal(desc->v[2]).uv;
        if (vertices == 4)
            uv[2] |= GetModelLightingVal(desc->v[3]).uv << 16;
        uv[1] |= model_render_state.draw.tpage;
    }
    uv[1] |= (flags & 0x180) << 14;
    if (vertices == 4)
        uv[3] = uv[2] >> 16;
}

static uint32 StoreModelPolygonPacket(uint8 *packet, const ModelPolygon *desc, uint32 flags, uint32 vertices, bool colored, bool textured, uint32 rgbc0)
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
        ReadModelPolygonUvs(desc, flags, vertices, uv);
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
            ReadModelPolygonUvs(desc, flags, vertices, uv);
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
        rgbc0 = xport_gte_read_data(22);
    else if (vertices == 3)
        rgbc0 = xport_gte_read_data(20);
    command = 0x20000000 | (vertices == 4 ? 0x08000000 : 0) | (colored ? 0x10000000 : 0) | (textured ? 0x04000000 : 0);
    PackModelPolygonCommand(packet, rgbc0, command, flags);
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

static void BuildModelPolygonPackets(const ModelPolygon *desc, uint32 count)
{
    FUNCTION_MARKER(0x800817FC, "SLUS_003.73");
    uint8 *packet, *limit;
    uint32 *ordering;
    if (!count)
        return;
    ordering = model_render_state.ordering_table;
    packet = scene_render.cursor;
    const ModelPalette *table = GetCurModelPal();
    limit = scene_render.limit - 100;
    xport_gte_write_control(5, 0);
    xport_gte_write_control(6, 0);
    xport_gte_write_control(7, 0);
    xport_gte_write_control(28, 0x10000000);
    xport_gte_write_control(27, 0);
    ResetModelPolygonQueue();
    for (;;)
    {
        uint32 depths[4], flags, clipping, area, scr3;
        const ModelProjectedVertex *points[4];
        uint32 packed;
        uint32 mask = model_render_state.draw.mode;
        uint32 depth, fog = 0, fog_shift, vertices, tag;
        bool colored, textured;
        uint32 rgbc0 = 0, ir[3], idx[4];
        uint32 i, prev, bytes;
        uint32 *bucket;
        POINT_C *vertex0;
        uint8 *chain_head = packet;
        flags = ((((uint32)desc->src_stride << 16) | desc->flags) & (uint32)((sint32)mask >> 16)) | (mask & 0xFFFF);
        for (i = 0; i < 4; ++i)
            points[i] = &GetModelProjVertex(desc->v[i]);
        for (i = 0; i < 3; ++i)
            xport_gte_write_data(12 + i, points[i]->scr);
        if (!(flags & 0xC0))
            goto next_desc;
        xport_gte_execute(0x1400006);
        scr3 = points[3]->scr;
        for (i = 0; i < 4; ++i)
            depths[i] = points[i]->depth_flags;
        vertex0 = (POINT_C *)(packet + sizeof(uint32));
        vertex0->xy.xy = xport_gte_read_data(12);
        clipping = ((depths[0] & depths[1] & depths[2] & depths[3]) >> 16) ^ 0xFF00;
        xport_gte_write_data(15, scr3);
        area = xport_gte_read_data(24);
        xport_gte_execute(0x1400006);
        if (clipping & 0x20FF)
            goto next_desc;
        if (clipping & 0x5000)
        {
            const ModelProjectedVertex *geometry0 = points[0];
            const ModelProjectedVertex *geometry1 = points[1];
            const ModelProjectedVertex *geometry2 = points[2];
            uint32 mag = 0, geometry_depth;
            Vector3 cross;
            if (!(flags & 0x1000))
                goto next_desc;
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
                    cross.comp[i] = xport_gte_read_data(25 + i);
                    mag |= cross.comp[i] < 0 ? -(uint32)cross.comp[i] : (uint32)cross.comp[i];
                }
                xport_gte_write_data(30, mag);
                shift = (17 - xport_gte_read_data(31)) & 31;
                for (i = 0; i < 3; ++i)
                    cross.comp[i] = xport_gte_read_data(25 + i);
                for (i = 0; i < 3; ++i)
                    xport_gte_write_data(9 + i, cross.comp[i] >> shift);
            }
            packed = geometry2->xy;
            geometry_depth = points[2]->depth_flags;
            xport_gte_write_control(0, packed);
            xport_gte_write_control(1, geometry_depth);
            xport_gte_execute(0x41E012);
            if ((sint32)xport_gte_read_data(25) < 0)
                goto next_desc;
            area = 0x7FFF0000;
        }
        else if ((sint32)area <= 0)
        {
            area = xport_gte_read_data(24);
            if ((sint32)area >= 0)
                goto next_desc;
            area = -area;
        }
        xport_gte_write_data(24, area);
        if (packet >= limit)
            break;
        if (clipping & 0x8000)
        {
            QueueWaterPlanePolygon(desc, flags, depths);
            goto next_desc;
        }
        depth = depths[0] << 16;
        for (i = 1; i < 4; ++i)
        {
            uint32 candidate = depths[i] << 16;
            if ((sint32)(depth - candidate) < 0)
                depth = candidate;
        }
        depth = (sint32)depth >> 16;
        packed = model_render_state.draw.fog_start;
        fog_shift = model_render_state.draw.fog_shift;
        packed = depth - packed;
        if ((sint32)packed > 0)
            fog = fog_shift & 0x80000000 ? packed >> (-fog_shift & 31) : packed << (fog_shift & 31);
        xport_gte_write_data(8, fog);
        vertices = flags & 0x10 ? 3 : 4;
        colored = (flags & 0x800) || ((flags & 12) == 12);
        textured = (flags & (colored ? 1 : 3)) != 0;
        if (flags & 0x800)
        {
            packed = desc->color;
            idx[0] = packed << 2;
            idx[1] = packed >> 6;
            idx[2] = packed >> 14;
            idx[3] = packed >> 22;
            if ((flags & 12) == 0)
            {
                if (vertices == 4)
                {
                    xport_gte_write_data(6, ReadModelPalColor(table, (idx[0] & 0x3FC) >> 2));
                    xport_gte_execute(0x780010);
                    for (i = 1; i < 4; ++i)
                    {
                        uint32 table_color = ReadModelTableColor(idx[i], table);
                        if (i == 3)
                            rgbc0 = xport_gte_read_data(22);
                        xport_gte_write_data(19 + i, table_color);
                    }
                }
                else
                {
                    for (i = 0; i < 3; ++i)
                        xport_gte_write_data(20 + i, ReadModelPalColor(table, (idx[i] & 0x3FC) >> 2));
                }
                xport_gte_execute(0xF8002A);
            }
            else
            {
                if (!(flags & 4))
                {
                    ir[0] = model_render_state.draw.light.x;
                    ir[1] = model_render_state.draw.light.y;
                    ir[2] = model_render_state.draw.light.z;
                }
                else if (!(flags & 8))
                {
                    const ModelLightingValue &val = GetModelLightingVal((((uint32)desc->n << 3) | ((uint32)desc->surface << 16)) >> 3);
                    uint32 packed = val.xy;
                    uint32 last = val.z;
                    // IR1 retains the complete packed input word
                    ir[0] = packed;
                    ir[1] = (sint32)packed >> 16;
                    ir[2] = last;
                }
                for (i = 0; i < vertices; ++i)
                {
                    if ((flags & 12) == 12)
                        LoadRendererVertexLighting(idx[i], table, GetModelLightingVal(desc->v[i]));
                    else
                        LoadModelTableColor(idx[i], table, ir[0], ir[1], ir[2]);
                    if (i == 3)
                        rgbc0 = xport_gte_read_data(20);
                    xport_gte_execute(0x680029);
                }
            }
        }
        else
        {
            xport_gte_write_data(6, desc->color);
            if ((flags & 12) == 12)
            {
                for (i = 0; i < vertices; ++i)
                {
                    LoadPackedProjIr(GetModelLightingVal(desc->v[i]));
                    if (i == 3)
                        rgbc0 = xport_gte_read_data(20);
                    xport_gte_execute(0x680029);
                }
            }
            else
            {
                if (flags & 4)
                    LoadPackedProjIr(GetModelLightingVal((((uint32)desc->n << 3) | ((uint32)desc->surface << 16)) >> 3));
                else if (flags & 8)
                    LoadModelDrawLighting();
                xport_gte_execute(flags & 12 ? 0x680029 : 0x780010);
            }
        }
        tag = StoreModelPolygonPacket(packet, desc, flags, vertices, colored, textured, rgbc0);
        if ((flags & 0x1000) && (sint32)(0x800 - xport_gte_read_data(24)) < 0)
        {
            if ((flags & 1) && ((flags & 0x800) || ((flags & 12) == 12)))
            {
                uint32 last_stride;
                uint8 *end = SubdivColoredTexturedPolygon(packet, limit, points, xport_gte_read_data(24), !(flags & 0x10), last_stride);
                if (end == packet)
                    goto next_desc;
                packet = end - last_stride;
                tag = (last_stride - 4) << 22;
            }
            else if (textured && !(flags & 0x10))
            {
                uint32 last_stride = 40;
                uint8 *end = SubdivFlatTexturedQuad(packet, limit, points, xport_gte_read_data(24), last_stride);
                if (end == packet)
                    goto next_desc;
                packet = end - last_stride;
                tag = (last_stride - 4) << 22;
            }
            else if (!(flags & (0x10 | 0x800)) && ((flags & 12) != 12) && !textured)
            {
                uint32 last_stride = 24;
                uint8 *end = SubdivFlatQuad(packet, limit, points, xport_gte_read_data(24), last_stride);
                if (end == packet)
                    goto next_desc;
                packet = end - last_stride;
                tag = (last_stride - 4) << 22;
            }
            else
            {
                uint32 last_stride;
                uint8 *end;
                if (colored)
                    end = SubdivColoredPolygon(packet, limit, points, xport_gte_read_data(24), vertices == 4, last_stride);
                else
                    end = SubdivFlatTriangle(packet, limit, points, xport_gte_read_data(24), textured, last_stride);
                if (end == packet)
                    goto next_desc;
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
        prev = *bucket;
        if (flags & 0x20)
        {
            uint32 window_command = GetModelPolygonWindow(*desc);
            uint8 *end;
            tag += 0x03000000;
            P_TAG *header = (P_TAG *)packet;
            header->addr = prev;
            header->len = (prev | tag) >> 24;
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
            header->addr = prev;
            header->len = (prev | tag) >> 24;
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
    next_desc:
        desc = desc->next;
        if (--count)
            continue;
        desc = ReadQueuedModelPolygons(count);
        if (!count)
            break;
    }
    scene_render.cursor = packet;
    return;
}

static void LoadProjEffectVertex(uint32 slot, const ShortVector3 &center, const ShortVector3 &basis0, const ShortVector3 &basis1, sint32 sign0, sint32 sign1)
{
    sint16 x = center.x + sign0 * basis0.x + sign1 * basis1.x;
    sint16 y = center.y + sign0 * basis0.y + sign1 * basis1.y;
    sint16 z = center.z + sign0 * basis0.z + sign1 * basis1.z;
    xport_gte_write_data(slot, (uint16)x | ((uint32)(uint16)y << 16));
    xport_gte_write_data(slot + 1, (uint16)z);
}

static void TransformProjIr(const MATRIX &matrix)
{
    FUNCTION_MARKER(0x80085CD4, "SLUS_003.73");
    uint32 words[5];
    for (uint32 idx = 0; idx < 5; ++idx)
        words[idx] = PackModelRotationWord(matrix, idx);
    (void)xport_gte_read_data(9);
    for (uint32 idx = 0; idx < 5; ++idx)
        xport_gte_write_control(idx, words[idx]);
    xport_gte_execute(0x486012);
}

static void AccumulateProjVector(const ShortVector3 &vector, uint32 scale)
{
    FUNCTION_MARKER(0x80085DD8, "SLUS_003.73");
    uint32 x = vector.x, y = vector.y, z = vector.z;
    (void)xport_gte_read_data(9);
    xport_gte_write_data(8, scale);
    xport_gte_write_data(25, x);
    xport_gte_write_data(26, y);
    xport_gte_write_data(27, z);
    xport_gte_execute(0x1A8003E);
}

void ModelGeometry::RenderOffset(const Vector3 *vector) const
{
    FUNCTION_MARKER(0x8007FC60, "SLUS_003.73");
    uint32 count = vertex_count;
    model_render_state.translated = false;
    uint32 flags = ProjectOffsetModelVertices(vertices, count, vector);
    if (model_render_state.water_clip_enabled)
        UpdateModelWaterClipFlags(count);
    if (flags & 0xBF)
        return;
    SetLightMatrix(&game_lighting.directions);
    SetColorMatrix(&game_lighting.colors);
    SetFarColor(game_lighting.far_color.r, game_lighting.far_color.g, game_lighting.far_color.b);
    SetBackColor(game_lighting.background_color.r, game_lighting.background_color.g, game_lighting.background_color.b);
    BuildModelPolygonPackets(polys, poly_count);
}

static void LoadCorner(uint32 vertex, const ShortVector3 &center, const ShortVector3 &edge0, const ShortVector3 &edge1, bool add0, bool add1)
{
    uint32 axis;
    ShortVector3 vals;
    for (axis = 0; axis < 3; ++axis)
    {
        vals[axis] = center[axis] + (add0 ? edge0[axis] : -edge0[axis]) + (add1 ? edge1[axis] : -edge1[axis]);
    }
    xport_gte_write_data(vertex * 2, (uint16)vals.x | ((uint32)(uint16)vals.y << 16));
    xport_gte_write_data(vertex * 2 + 1, (uint16)vals.z);
}

void ModelGeometry::RenderUntranslatedClipped() const
{
    FUNCTION_MARKER(0x8007FD3C, "SLUS_003.73");
    RenderClippedModel(*this, false);
}

uint16 ModelGeometry::TransformColVertices(CollisionVertex *output, sint32 depth_limit, const Vector3 &translation) const
{
    FUNCTION_MARKER(0x80084D4C, "SLUS_003.73");
    uint32 translation_x = translation.comp[0];
    uint32 translation_y = translation.comp[1];
    uint32 translation_z = translation.comp[2];
    uint32 remaining = vertex_count;
    if (!remaining)
        return 0x60F;
    const SVECTOR *vertex = vertices;
    uint32 x = vertex->vx + translation_x;
    uint32 y = vertex->vy + translation_y;
    uint32 z = vertex->vz + translation_z;
    uint16 result = 0x60F, mask;
    sint32 tx, ty, tz;
    do
    {
        xport_gte_write_data(9, x);
        xport_gte_write_data(10, y);
        xport_gte_write_data(11, z);
        ++vertex;
        --remaining;
        xport_gte_execute(0x4D8012);
        // Prefetch the next vertex before storing the current result
        x = (remaining ? vertex->vx : 0) + translation_x;
        y = (remaining ? vertex->vy : 0) + translation_y;
        z = (remaining ? vertex->vz : 0) + translation_z;
        tx = xport_gte_read_data(25);
        ty = xport_gte_read_data(26);
        tz = xport_gte_read_data(27);
        mask = ((tx < 0) << 2) | ((ty < 0) << 1) | (tz < 0) | ((depth_limit < tz) << 3) | ((tx > 0) << 10) | ((ty > 0) << 9);
        output->xy.x = tx;
        output->xy.y = ty;
        output->z = tz;
        result &= mask;
        output->clip = mask;
        ++output;
    } while (remaining);
    return result;
}

void ModelGeometry::IntersectColPolygons(const CollisionVertex *output, CollisionRay &rec, ModelInstance *object) const
{
    FUNCTION_MARKER(0x80084E24, "SLUS_003.73");
    uint32 required, rejected, count;
    uint32 flags;
    uint16 header;
    ModelPolygon *polygon;
    const CollisionVertex *vertices[4];
    XY xy[3];
    uint16 clip[4];
    sint32 cross0;
    uint32 cross1, cross2;
    const CollisionVertex *vertex0;
    const SVECTOR *normal;
    uint32 start, distance;
    sint32 numerator, endpoint, denominator;
    bool trigger_enabled;
    uint16 trigger_idx;
    uint32 axis;

    xport_gte_write_control(11, 0);
    xport_gte_write_control(12, rec.len);
    required = polygon_required_mask;
    rejected = polygon_rejected_mask;
    count = poly_count;
    if (!count)
        return;
    polygon = polys;
    do
    {
        flags = (polygon->n << 3) | ((uint32)polygon->surface << 16);
        header = polygon->flags;
        if (!(flags & rejected) && (flags | required) == 0xFFFFFFFF && ((flags >> 16) & 3) != 1)
        {
            vertices[0] = output + polygon->v[0];
            clip[0] = vertices[0]->clip;
            vertices[1] = output + polygon->v[1];
            clip[1] = vertices[1]->clip;
            vertices[2] = output + polygon->v[2];
            clip[2] = vertices[2]->clip;
            vertices[3] = output + polygon->v[3];
            clip[3] = vertices[3]->clip;
            if (!(clip[0] & clip[1] & clip[2] & clip[3] & 0x60F))
            {
                for (axis = 0; axis < 3; ++axis)
                    xy[axis] = vertices[axis]->xy;
                for (axis = 0; axis < 3; ++axis)
                    xport_gte_write_control(axis * 2, xy[axis].xy);
                for (axis = 0; axis < 3; ++axis)
                    xport_gte_write_data(9 + axis, xy[axis].y);
                xport_gte_execute(0x170000C);
                cross0 = xport_gte_read_data(25);
                cross1 = xport_gte_read_data(26);
                cross2 = xport_gte_read_data(27);
                if (cross0 < 0)
                {
                    if (header & 0x10)
                        goto next_polygon;
                    xy[0] = vertices[3]->xy;
                    xport_gte_write_data(10, xy[1].y);
                    xport_gte_write_data(11, xy[2].y);
                    xport_gte_write_control(0, xy[0].xy);
                    xport_gte_write_data(9, vertices[3]->xy.y);
                    xport_gte_execute(0x170000C);
                    cross1 = xport_gte_read_data(26);
                    cross2 = xport_gte_read_data(27);
                    cross1 = -cross1;
                    cross2 = -cross2;
                }
                if ((sint32)(cross1 | cross2) >= 0)
                {
                    normal = normals + polygon->n;
                    xport_gte_write_data(0, ((uint16)normal->vx | ((uint32)(uint16)normal->vy << 16)));
                    xport_gte_write_data(1, normal->vz);
                    vertex0 = output + polygon->v[0];
                    start = vertex0->xy.xy;
                    endpoint = vertex0->z;
                    xport_gte_execute(0x4C6012);
                    xport_gte_write_control(8, start);
                    xport_gte_write_control(9, endpoint);
                    denominator = xport_gte_read_data(11);
                    xport_gte_execute(0x43E012);
                    numerator = xport_gte_read_data(25);
                    endpoint = xport_gte_read_data(27);
                    if (numerator > 0)
                        goto next_polygon;
                    distance = DivideSignedWord(numerator, denominator);
                    if (numerator < endpoint)
                        goto next_polygon;
                    trigger_enabled = rec.triggers;
                    if (flags & 0x20000)
                    {
                        trigger_idx = object->model_idx;
                        if (trigger_enabled)
                            col_trigger = trigger_idx;
                    }
                    else if ((sint32)(rec.distance - distance) > 0)
                    {
                        rec.hit.SetObject(object);
                        rec.distance = distance;
                        rec.hit.SetPolygon(polygon);
                        rec.endpoint = endpoint;
                        for (axis = 0; axis < 3; ++axis)
                            xport_gte_write_data(25 + axis, (uint32)rec.endpoints[0].comp[axis]);
                        xport_gte_write_data(9, rec.basis.m[2][0]);
                        xport_gte_write_data(10, rec.basis.m[2][1]);
                        xport_gte_write_data(11, rec.basis.m[2][2]);
                        xport_gte_write_data(8, distance);
                        xport_gte_execute(0x1A0003E);
                        for (axis = 0; axis < 3; ++axis)
                            rec.hit_pos.comp[axis] = xport_gte_read_data(25 + axis);
                        normal = normals + polygon->n;
                        col_normal = *normal;
                    }
                }
            }
        }
    next_polygon:
        polygon = polygon->next;
        --count;
    } while (count);
    return;
}
