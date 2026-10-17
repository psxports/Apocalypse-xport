#include "game_sdk_bindings.h"
#include "model_geometry.h"
#include "model_instance.h"
#include <stdlib.h>
#include <string.h>

typedef char ModelVectorBoundarySize[sizeof(SVECTOR) == 8 ? 1 : -1];

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
        case 0: return poly.uv[0].uv | ((uint32)poly.clut << 16);
        case 1: return poly.uv[1].uv | ((uint32)poly.tpage << 16);
        case 2: return poly.uv[2].uv | ((uint32)poly.uv[3].uv << 16);
    }
    GameDiagnostic("Invalid model polygon UV word %u\n", word);
    abort();
    return 0;
}

static bool CheckModelGeometryInput(const uint8 *data, uint32 bytes, const TextureReferences &refs, uint32 &table, uint32 &count)
{
    if (!data || bytes < 16)
        return false;
    const uint32 object_count = xport_load_le32(data + 8);
    if (!CheckGeometrySpan(bytes, 12, object_count, 36))
        return false;
    table = 12 + object_count * 36;
    if (!CheckGeometrySpan(bytes, table, 1, 4))
        return false;
    count = xport_load_le32(data + table);
    table += 4;
    if (!CheckGeometrySpan(bytes, table, count, 4) || count > 0xFFFFFFFF / sizeof(ModelGeometry))
        return false;
    uint32 next = table + count * 4;
    for (uint32 i = 0; i < count; ++i)
    {
        const uint32 start = xport_load_le32(data + table + i * 4);
        if (start != next || !CheckGeometrySpan(bytes, start, 1, 32))
            return false;
        const uint32 nv = xport_load_le32(data + start + 4);
        const uint32 nn = xport_load_le32(data + start + 8);
        const uint32 np = xport_load_le32(data + start + 12);
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
            const uint16 flags = xport_load_le16(data + pos);
            const uint32 stride = xport_load_le16(data + pos + 2);
            if ((stride & 3) || stride < 20 || !CheckGeometrySpan(bytes, pos, 1, stride))
                return false;
            const uint32 vertices = flags & 16 ? 3 : 4;
            for (uint32 v = 0; v < vertices; ++v)
                if (xport_load_le16(data + pos + 4 + v * 2) >= nv)
                    return false;
            if (xport_load_le16(data + pos + 16) >= nn)
                return false;
            if ((flags & 3) == 1)
                return false;
            if ((flags & 3) == 3)
            {
                if (stride < 32)
                    return false;
                const uint32 idx = xport_load_le32(data + pos + 20);
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
    for (uint32 i = 0; i < count; ++i)
    {
        ModelGeometry &model = decoded.models[i];
        const uint32 start = xport_load_le32(data + table + i * 4);
        model.flags = xport_load_le32(data + start);
        model.vertex_count = xport_load_le32(data + start + 4);
        model.n_count = xport_load_le32(data + start + 8);
        model.poly_count = xport_load_le32(data + start + 12);
        model.radius = xport_load_le32(data + start + 16);
        for (uint32 axis = 0; axis < 3; ++axis)
        {
            uint32 packed = xport_load_le32(data + start + 20 + axis * 4);
            if (!(model.flags & 8))
            {
                const uint32 extent = (uint32)((sint32)(packed + 4095) >> 12);
                packed = ((0 - extent) << 16) | extent;
            }
            model.bounds[axis].max = (sint16)packed;
            model.bounds[axis].min = (sint16)(packed >> 16);
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
                vertices[v].vx = (sint16)((uint16)vertices[v].vy << 3);
                vertices[v].vy = 0;
            }
        uint32 combined = 0, common = 0xFFFFFFFF;
        for (uint32 p = 0; p < model.poly_count; ++p)
        {
            ModelPolygon &poly = model.polys[p];
            poly.next = p + 1 < model.poly_count ? &model.polys[p + 1] : 0;
            poly.flags = xport_load_le16(data + pos);
            poly.source_stride = xport_load_le16(data + pos + 2);
            for (uint32 v = 0; v < 4; ++v)
                poly.v[v] = xport_load_le16(data + pos + 4 + v * 2) & 8191;
            poly.color = xport_load_le32(data + pos + 12);
            poly.n = xport_load_le16(data + pos + 16) & 8191;
            poly.surface = xport_load_le16(data + pos + 18);
            if (!(poly.flags & 64))
                poly.flags ^= 128;
            if (!(poly.flags & 2048))
                poly.color &= 0xFFFFFF;
            if ((poly.flags & 3) == 3)
            {
                const TextureResource &tex = *refs.entries[xport_load_le32(data + pos + 20)];
                uint16 uv[4];
                for (uint32 v = 0; v < 4; ++v)
                {
                    uv[v] = xport_load_le16(data + pos + 24 + v * 2);
                    if (!(poly.flags & 32))
                        uv[v] = (uint16)(uv[v] + tex.uv[0].uv);
                }
                for (uint32 v = 0; v < 4; ++v)
                    poly.uv[v].uv = uv[v];
                poly.clut = tex.clut;
                poly.tpage = tex.tpage;
                if (poly.flags & 32)
                    poly.window = tex.window;
            }
            else if (poly.flags & 32)
            {
                if (poly.source_stride == 20 && p + 1 < model.poly_count)
                    poly.window_alias = &model.polys[p + 1].color;
                else
                    poly.window = xport_load_le32(data + pos + 32);
            }
            if (poly.flags & 16)
                poly.v[3] = poly.v[2];
            combined |= poly.flags;
            common &= poly.surface;
            pos += poly.source_stride;
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
