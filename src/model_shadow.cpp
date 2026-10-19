#include "scene_lifecycle.h"
#include "model_render_view.h"
#include "game_diagnostics.h"
#include "model_shadow.h"
#include "model_geometry.h"
#include "texture_resources.h"
#include "model_projection.h"

static uint32 DivideShadowValue(uint32 numerator, uint32 denominator)
{
    if (!denominator)
        return (sint32)numerator < 0 ? 1 : (uint32)-1;
    if (numerator == 0x80000000 && denominator == (uint32)-1)
        return numerator;
    return (uint32)((sint32)numerator / (sint32)denominator);
}

static void LoadShadowVector(const ShortVector3 &vector)
{
    uint32 axis;
    for (axis = 0; axis < 3; ++axis)
        xport_gte_write_data(9 + axis, vector.words[axis]);
}

static void StoreShadowVector(ShortVector3 &vector)
{
    uint32 axis;
    for (axis = 0; axis < 3; ++axis)
        vector.words[axis] = (uint16)xport_gte_read_data(9 + axis);
}

static void RotateShadowVector(const uint32 matrix[5])
{
    uint32 axis;
    xport_gte_read_data(9);
    for (axis = 0; axis < 5; ++axis)
        xport_gte_write_control(axis, matrix[axis]);
    xport_gte_execute(0x49E012);
}

static void CrossShadowVector(const ShortVector3 &vector)
{
    xport_gte_read_data(9);
    xport_gte_write_control(0, vector.words[0]);
    xport_gte_write_control(2, vector.words[1]);
    xport_gte_write_control(4, vector.words[2]);
    xport_gte_execute(0x178000C);
}

static void LoadShadowCorner(uint32 vertex, const ShortVector3 &center, const ShortVector3 &edge0, const ShortVector3 &edge1, uint32 add0, uint32 add1)
{
    uint32 axis;
    Vector3 values;
    for (axis = 0; axis < 3; ++axis)
    {
        values.words[axis] = center.words[axis];
        values.words[axis] = add0 ? values.words[axis] + edge0.words[axis] : values.words[axis] - edge0.words[axis];
        values.words[axis] = add1 ? values.words[axis] + edge1.words[axis] : values.words[axis] - edge1.words[axis];
        values.words[axis] &= 65535;
    }
    xport_gte_write_data(vertex * 2, values.words[0] | (values.words[1] << 16));
    xport_gte_write_data(vertex * 2 + 1, values.words[2]);
}

uint32 RenderModelShadow(const ModelGeometry &model, const MATRIX &matrix, uint32 rgb, uint32 scale, uint32 depth_bias, const TextureResource &texture)
{
    ModelRenderView *camera;
    uint32 minimum, maximum, product, axis, rotation[5], transpose[5], stretched[5];
    uint32 average, numerator, length, squares[3], factor, depth, result, projected;
    Vector3 extents;
    uint8 *packet;
    ShortVector3 center, relative, normalized, basis;
    ShortVector3 edge[2];
    ShortVector3 selected;
    FUNCTION_MARKER(0x8007B2E4, "SLUS_003.73");
    for (axis = 0; axis < 3; ++axis)
    {
        minimum = (uint16)model.bounds[axis].max;
        maximum = (uint32)(sint32)model.bounds[axis].min;
        product = (uint32)((sint32)(minimum - maximum) >> 1) * scale;
        extents.words[axis] = (uint32)((sint32)product >> 12);
        center.words[axis] = (uint16)((sint32)((uint16)model.bounds[axis].max + (uint32)(sint32)model.bounds[axis].min) >> 1);
    }
    LoadShadowVector(center);
    for (axis = 0; axis < 5; ++axis)
        rotation[axis] = ReadModelRenderMatrixWord(matrix, axis);
    RotateShadowVector(rotation);
    StoreShadowVector(center);
    for (axis = 0; axis < 3; ++axis)
        center.words[axis] = (uint16)(center.words[axis] + (uint16)matrix.t[axis]);
    camera = &GetConfiguredModelRenderView();
    for (axis = 0; axis < 3; ++axis)
        center.words[axis] = (uint16)(center.words[axis] - ((uint32)(uint16)camera->pos.words[axis]));
    LoadShadowVector(center);
    NormalizeProjectionIr();
    StoreShadowVector(relative);
    for (axis = 0; axis < 3; ++axis)
        extents.words[axis] = (uint32)(sint32)(sint16)extents.words[axis];
    average = (uint32)((sint32)(extents.words[0] + extents.words[1] + extents.words[2]) / 3);
    numerator = (uint32)((sint32)(average << 16) >> 4);
    for (axis = 0; axis < 4; ++axis)
        rotation[axis] = ReadModelRenderMatrixWord(matrix, axis);
    rotation[4] = (uint16)matrix.m[2][2];
    transpose[0] = ((rotation[0] ^ rotation[1]) & 65535) ^ rotation[1];
    transpose[1] = ((rotation[3] ^ rotation[0]) & 65535) ^ rotation[0];
    transpose[2] = ((rotation[2] ^ rotation[3]) & 65535) ^ rotation[3];
    transpose[3] = ((rotation[1] ^ rotation[2]) & 65535) ^ rotation[2];
    transpose[4] = rotation[4];
    stretched[0] = DivideShadowValue(numerator, extents.words[0]) & 65535;
    stretched[1] = 0;
    stretched[2] = DivideShadowValue(numerator, extents.words[1]) & 65535;
    stretched[3] = 0;
    stretched[4] = DivideShadowValue(numerator, extents.words[2]) & 65535;
    LoadShadowVector(relative);
    RotateShadowVector(transpose);
    NormalizeProjectionIr();
    RotateShadowVector(stretched);
    StoreShadowVector(normalized);
    length = MeasureProjectionIrLength();
    for (axis = 0; axis < 3; ++axis)
        squares[axis] = xport_gte_read_data(25 + axis);
    selected.words[0] = selected.words[1] = selected.words[2] = 0;
    if ((sint32)squares[1] >= (sint32)squares[0] && (sint32)squares[2] >= (sint32)squares[0])
        selected.words[0] = ONE;
    else if ((sint32)squares[0] >= (sint32)squares[1] && (sint32)squares[2] >= (sint32)squares[1])
        selected.words[1] = ONE;
    else
        selected.words[2] = ONE;
    LoadShadowVector(normalized);
    CrossShadowVector(selected);
    StoreShadowVector(basis);
    NormalizeProjectionIr();
    factor = extents.words[0] * extents.words[1];
    average = (uint32)(sint32)(sint16)average;
    factor = DivideShadowValue(factor, average);
    factor *= extents.words[2];
    factor *= (uint32)(sint32)(sint16)length;
    factor = DivideShadowValue(factor, average);
    factor = (uint32)((sint32)factor >> 12);
    RotateShadowVector(stretched);
    RotateShadowVector(rotation);
    CrossShadowVector(relative);
    ScaleProjectionIr(factor);
    StoreShadowVector(edge[0]);
    LoadShadowVector(normalized);
    CrossShadowVector(basis);
    NormalizeProjectionIr();
    RotateShadowVector(stretched);
    RotateShadowVector(rotation);
    CrossShadowVector(relative);
    ScaleProjectionIr(factor);
    StoreShadowVector(edge[1]);
    camera = &GetConfiguredModelRenderView();
    SetRotMatrix(&camera->projection);
    xport_gte_write_control(5, 0);
    xport_gte_write_control(6, 0);
    xport_gte_write_control(7, 0);
    LoadShadowCorner(0, center, edge[0], edge[1], 0, 0);
    LoadShadowCorner(1, center, edge[0], edge[1], 1, 0);
    LoadShadowCorner(2, center, edge[0], edge[1], 0, 1);
    xport_gte_execute(0x280030);
    GetModelProjectionVertex(0).screen = xport_gte_read_data(12);
    GetModelProjectionVertex(1).screen = xport_gte_read_data(13);
    GetModelProjectionVertex(2).screen = xport_gte_read_data(14);
    LoadShadowCorner(0, center, edge[0], edge[1], 1, 1);
    xport_gte_execute(0x180001);
    GetModelProjectionVertex(3).screen = xport_gte_read_data(14);
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
    projected = 0;
    for (axis = 0; axis < 4; ++axis)
    {
        poly->p[axis].xy.x = (uint16)GetModelProjectionVertex(projected + axis).screen;
        poly->p[axis].xy.y = GetModelProjectionVertex(projected + axis).screen >> 16;
    }
    poly->p[0].uv.u = texture.uv[0].u;
    poly->p[0].uv.v = texture.uv[0].v;
    poly->p[1].uv.u = texture.uv[1].u - 1;
    poly->p[1].uv.v = texture.uv[1].v;
    poly->p[2].uv.u = texture.uv[2].u;
    poly->p[2].uv.v = texture.uv[2].v - 1;
    poly->p[3].uv.u = texture.uv[3].u - 1;
    poly->p[3].uv.v = texture.uv[3].v - 1;
    poly->p[0].attr = texture.clut;
    product = texture.tpage;
    poly->p[1].attr = (product & 0xFF9F) | 32;
    uint32 *bucket = scene_render.cur->ot + depth;
    setaddr(poly, getaddr(bucket));
    setaddr(bucket, gpu_packet_link((uintptr_t)poly));
    return *bucket;
}
