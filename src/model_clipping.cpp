#include <stdlib.h>

#include "psx.h"
#include "model_render.h"
#include "model_projection.h"
#include "global.h"
#include "game_diagnostics.h"
#include "model_clipping.h"

struct WaterPlaneClipContext
{
    uint32 generated_vertex_idx;
    bool generated_cursor_valid;
    uint32 invalid_generated_cursor;
    ModelPolygon *queued_desc;
    uint16 vertex_indices[4];
    uint32 packed_uv;
};

struct ClippedPolygonNode
{
    ModelPolygon poly;
    ClippedPolygonNode *next;
};

static ClippedPolygonNode *clipped_head;
static ClippedPolygonNode *clipped_cursor;
static uint32 clipped_pending;

static ClippedPolygonNode *AllocateClippedPolygonNode()
{
    ClippedPolygonNode *node = (ClippedPolygonNode *)calloc(1, sizeof(ClippedPolygonNode));
    if (!node)
    {
        GameDiagnostic(("Cannot allocate clipped model polygon\n"));
        abort();
    }
    return node;
}

static ModelPolygon *GetQueuedModelPolygon()
{
    if (!clipped_head)
        clipped_head = AllocateClippedPolygonNode();
    if (!clipped_cursor)
        clipped_cursor = clipped_head;
    if (!clipped_cursor->next)
        clipped_cursor->next = AllocateClippedPolygonNode();
    clipped_cursor->poly.next = &clipped_cursor->next->poly;
    return &clipped_cursor->poly;
}

static ModelPolygon *AdvanceModelPolygonQueue()
{
    clipped_cursor = clipped_cursor->next;
    return GetQueuedModelPolygon();
}

void ResetModelPolygonQueue()
{
    clipped_cursor = clipped_head;
    clipped_pending = 0;
}

void ReleaseModelPolygonQueue()
{
    while (clipped_head)
    {
        ClippedPolygonNode *next = clipped_head->next;
        free(clipped_head);
        clipped_head = next;
    }
    clipped_cursor = 0;
    clipped_pending = 0;
}

const ModelPolygon *ReadQueuedModelPolygons(uint32 &count)
{
    count = clipped_pending;
    clipped_pending = 0;
    return clipped_head ? &clipped_head->poly : 0;
}

static void CopyClippedPolygonMetadata(const ModelPolygon &src, ModelPolygon &dst, bool triangle)
{
    dst.flags = triangle ? src.flags | 16 : src.flags & 0xFFEF;
    dst.src_stride = src.src_stride;
    dst.color = src.color;
    dst.n = src.n;
    dst.surface = src.surface;
    dst.clut = src.clut;
    dst.tpage = src.tpage;
    dst.window = GetModelPolygonWindow(src);
    dst.window_alias = 0;
    if (dst.src_stride == 20 && (dst.flags & 32))
    {
        // The original short descriptor writes its window over the next color
        dst.next->color = dst.window;
        dst.window_alias = &dst.next->color;
    }
    ++clipped_pending;
}

static void InterpolateWaterPlaneEdge(const ModelPolygon *desc, WaterPlaneClipContext *context, uint32 idx0, uint32 idx1, uint32 uv_idx0, uint32 uv_idx1)
{
    uint32 uv0, uv1;
    XY xy0, xy1;
    uint32 depth0, depth1, plane, numerator, fraction;
    sint16 z0, z1;
    uint32 x, y, z, packed, dst, u0, v0, u1, v1;
    uv0 = desc->uv[uv_idx0].uv;
    uv1 = desc->uv[uv_idx1].uv;
    context->packed_uv = uv0 | (uv1 << 16);
    const ModelProjectedVertex &vertex0 = GetModelProjVertex(context->vertex_indices[idx0]);
    xy0.xy = vertex0.xy;
    z0 = vertex0.depth_flags;
    xport_gte_write_control(0, xy0.xy);
    xport_gte_write_control(1, z0);
    const ModelProjectedVertex &vertex1 = GetModelProjVertex(context->vertex_indices[idx1]);
    xy1.xy = vertex1.xy;
    z1 = vertex1.depth_flags;
    xport_gte_write_control(3, xy1.xy);
    xport_gte_write_control(4, z1);
    xport_gte_write_data(0, (uint16)model_render_state.water_plane.normal.x | ((uint32)(uint16)model_render_state.water_plane.normal.y << 16));
    xport_gte_write_data(1, (uint16)model_render_state.water_plane.normal.z | ((uint32)(uint16)model_render_state.water_plane.distance << 16));
    xport_gte_execute(0x486012);
    plane = model_render_state.water_plane.distance;
    depth0 = xport_gte_read_data(25);
    depth1 = xport_gte_read_data(27);
    numerator = (-plane - depth0) << 12;
    fraction = DivideSignedWord(numerator, depth1 - depth0);
    xport_gte_write_data(25, xy0.x);
    xport_gte_write_data(26, xy0.y);
    xport_gte_write_data(27, z0);
    xport_gte_write_data(9, xy1.x - xy0.x);
    xport_gte_write_data(10, xy1.y - xy0.y);
    xport_gte_write_data(11, z1 - z0);
    xport_gte_write_control(0, ONE);
    xport_gte_write_control(1, 0);
    xport_gte_write_control(2, ONE);
    xport_gte_write_control(3, 0);
    xport_gte_write_control(4, ONE);
    xport_gte_write_control(5, 0);
    xport_gte_write_control(6, 0);
    xport_gte_write_control(7, 0);
    xport_gte_write_data(8, fraction);
    xport_gte_execute(0x1A8003E);
    x = xport_gte_read_data(25);
    y = xport_gte_read_data(26);
    z = xport_gte_read_data(27);
    packed = (x & 65535) | (y << 16);
    xport_gte_write_data(0, packed);
    xport_gte_write_data(1, z);
    xport_gte_execute(0x180001);
    if (!context->generated_cursor_valid)
    {
        GameDiagnostic(("Original offset-reference projection produced an invalid water-clipping cursor: %u\n", context->invalid_generated_cursor));
        abort();
    }
    dst = context->generated_vertex_idx;
    ModelProjectedVertex &vertex = GetModelProjVertex(dst);
    vertex.xy = packed;
    vertex.depth_flags = (z & 65535) | 0xFF000000;
    vertex.scr = xport_gte_read_data(14);
    context->generated_vertex_idx = dst + 1;
    packed = context->packed_uv;
    u0 = (packed << 4) & 0xFF0;
    v0 = (packed >> 4) & 0xFF0;
    xport_gte_write_data(25, u0);
    xport_gte_write_data(26, v0);
    u1 = (packed >> 12) & 0xFF0;
    v1 = (packed >> 20) & 0xFF0;
    xport_gte_write_data(9, u1 - u0);
    xport_gte_write_data(10, v1 - v0);
    xport_gte_write_data(8, fraction);
    xport_gte_execute(0x1A8003E);
}

static void ClipModelEdge01(const ModelPolygon *desc, WaterPlaneClipContext *context)
{
    FUNCTION_MARKER(0x80083BF4, "SLUS_003.73");
    InterpolateWaterPlaneEdge(desc, context, 0, 1, 0, 1);
}

static void ClipModelEdge12(const ModelPolygon *desc, WaterPlaneClipContext *context)
{
    FUNCTION_MARKER(0x80083C0C, "SLUS_003.73");
    InterpolateWaterPlaneEdge(desc, context, 1, 2, 1, 2);
}

static void ClipModelEdge20(const ModelPolygon *desc, WaterPlaneClipContext *context)
{
    FUNCTION_MARKER(0x80083C24, "SLUS_003.73");
    InterpolateWaterPlaneEdge(desc, context, 2, 0, 2, 0);
}

static void ClipModelEdge13(const ModelPolygon *desc, WaterPlaneClipContext *context)
{
    FUNCTION_MARKER(0x80083C3C, "SLUS_003.73");
    InterpolateWaterPlaneEdge(desc, context, 1, 3, 1, 3);
}

static void ClipModelEdge32(const ModelPolygon *desc, WaterPlaneClipContext *context)
{
    FUNCTION_MARKER(0x80083C54, "SLUS_003.73");
    InterpolateWaterPlaneEdge(desc, context, 3, 2, 3, 2);
}

static void QueueClippedTriangle(const ModelPolygon *desc, WaterPlaneClipContext *context)
{
    FUNCTION_MARKER(0x80083E54, "SLUS_003.73");
    CopyClippedPolygonMetadata(*desc, *context->queued_desc, true);
}

enum WaterPlaneEdge
{
    Edge01,
    Edge12,
    Edge20,
    Edge13,
    Edge32
};

static void InterpolateSelClipEdge(const ModelPolygon *desc, WaterPlaneClipContext *context, WaterPlaneEdge edge)
{
    switch (edge)
    {
        case Edge01:
            ClipModelEdge01(desc, context);
            break;
        case Edge12:
            ClipModelEdge12(desc, context);
            break;
        case Edge20:
            ClipModelEdge20(desc, context);
            break;
        case Edge13:
            ClipModelEdge13(desc, context);
            break;
        case Edge32:
            ClipModelEdge32(desc, context);
            break;
    }
}

static void DuplicateClipTriangle(const ModelPolygon *desc, WaterPlaneClipContext *context, uint32 marked, uint32 idx0, uint32 idx1, uint32 idx2, uint32 uv_idx0, uint32 uv_idx1, uint32 uv_idx2)
{
    uint32 val;
    UV uv[3];
    ModelPolygon *dst;
    ModelProjectedVertex &vertex = GetModelProjVertex(context->vertex_indices[marked]);
    val = vertex.depth_flags >> 16;
    vertex.depth_flags = (vertex.depth_flags & 65535) | (((val & 0xFF7F) | 0x8000) << 16);
    QueueClippedTriangle(desc, context);
    dst = context->queued_desc;
    dst->v[0] = context->vertex_indices[idx0];
    dst->v[1] = context->vertex_indices[idx1];
    dst->v[2] = context->vertex_indices[idx2];
    dst->v[3] = context->vertex_indices[idx2];
    uv[0] = desc->uv[uv_idx0];
    uv[1] = desc->uv[uv_idx1];
    uv[2] = desc->uv[uv_idx2];
    dst->uv[0] = uv[0];
    dst->uv[1] = uv[1];
    dst->uv[2] = uv[2];
    context->queued_desc = AdvanceModelPolygonQueue();
}

void QueueWaterPlanePolygon(const ModelPolygon *desc, uint32 flags, const uint32 depths[4])
{
    WaterPlaneClipContext context;
    WaterPlaneEdge edge0, edge1;
    bool outside[4], uv0_high, triangle;
    uint32 kept0, kept1 = 0;
    uint32 axis, uv0, uv1;
    ModelPolygon *dst;
    context.generated_vertex_idx = model_render_state.generated_vertex_idx;
    context.generated_cursor_valid = model_render_state.generated_cursor_valid;
    context.invalid_generated_cursor = model_render_state.invalid_generated_cursor;
    context.queued_desc = GetQueuedModelPolygon();
    context.packed_uv = 0;
    for (axis = 0; axis < 4; ++axis)
        context.vertex_indices[axis] = desc->v[axis];
    outside[0] = (depths[0] & 0x800000) != 0;
    outside[1] = (depths[1] & 0x800000) != 0;
    outside[2] = (depths[2] & 0x800000) != 0;
    triangle = false;
    if (flags & 16)
    {
        if (!outside[0] && !outside[1])
        {
            edge0 = Edge20;
            edge1 = Edge12;
            uv0_high = true;
            kept0 = 0;
            kept1 = 1;
        }
        else if (!outside[0] && !outside[2])
        {
            edge0 = Edge12;
            edge1 = Edge01;
            uv0_high = true;
            kept0 = 2;
            kept1 = 0;
        }
        else if (!outside[0])
        {
            edge0 = Edge01;
            edge1 = Edge20;
            uv0_high = false;
            kept0 = 0;
            triangle = true;
        }
        else if (outside[1])
        {
            edge0 = Edge20;
            edge1 = Edge12;
            uv0_high = false;
            kept0 = 2;
            triangle = true;
        }
        else if (outside[2])
        {
            edge0 = Edge12;
            edge1 = Edge01;
            uv0_high = false;
            kept0 = 1;
            triangle = true;
        }
        else
        {
            edge0 = Edge01;
            edge1 = Edge20;
            uv0_high = true;
            kept0 = 1;
            kept1 = 2;
        }
    }
    else
    {
        outside[3] = (depths[3] & 0x800000) != 0;
        if (!outside[0] && !outside[1] && !outside[2])
        {
            DuplicateClipTriangle(desc, &context, 0, 2, 0, 1, 2, 0, 1);
            edge0 = Edge32;
            edge1 = Edge13;
            uv0_high = true;
            kept0 = 2;
            kept1 = 1;
        }
        else if (!outside[0] && !outside[1] && !outside[3])
        {
            DuplicateClipTriangle(desc, &context, 1, 0, 1, 3, 0, 1, 3);
            edge0 = Edge20;
            edge1 = Edge32;
            uv0_high = true;
            kept0 = 0;
            kept1 = 3;
        }
        else if (!outside[0] && !outside[1])
        {
            edge0 = Edge20;
            edge1 = Edge13;
            uv0_high = true;
            kept0 = 0;
            kept1 = 1;
        }
        else if (!outside[0] && outside[2])
        {
            edge0 = Edge01;
            edge1 = Edge20;
            uv0_high = false;
            kept0 = 0;
            triangle = true;
        }
        else if (!outside[0] && !outside[3])
        {
            DuplicateClipTriangle(desc, &context, 2, 3, 2, 0, 3, 2, 0);
            edge0 = Edge13;
            edge1 = Edge01;
            uv0_high = true;
            kept0 = 3;
            kept1 = 0;
        }
        else if (!outside[0])
        {
            edge0 = Edge32;
            edge1 = Edge01;
            uv0_high = true;
            kept0 = 2;
            kept1 = 0;
        }
        else if (!outside[1] && !outside[2])
        {
            DuplicateClipTriangle(desc, &context, 3, 1, 3, 2, 1, 3, 2);
            edge0 = Edge01;
            edge1 = Edge20;
            uv0_high = true;
            kept0 = 1;
            kept1 = 2;
        }
        else if (!outside[1] && !outside[3])
        {
            edge0 = Edge01;
            edge1 = Edge32;
            uv0_high = true;
            kept0 = 1;
            kept1 = 3;
        }
        else if (!outside[1])
        {
            edge0 = Edge13;
            edge1 = Edge01;
            uv0_high = false;
            kept0 = 1;
            triangle = true;
        }
        else if (!outside[2] && !outside[3])
        {
            edge0 = Edge13;
            edge1 = Edge20;
            uv0_high = true;
            kept0 = 3;
            kept1 = 2;
        }
        else if (!outside[2])
        {
            edge0 = Edge20;
            edge1 = Edge32;
            uv0_high = false;
            kept0 = 2;
            triangle = true;
        }
        else
        {
            edge0 = Edge32;
            edge1 = Edge13;
            uv0_high = false;
            kept0 = 3;
            triangle = true;
        }
    }
    dst = context.queued_desc;
    InterpolateSelClipEdge(desc, &context, edge0);
    uv0 = uv0_high ? context.packed_uv >> 16 : context.packed_uv;
    dst->uv[0].uv = uv0;
    InterpolateSelClipEdge(desc, &context, edge1);
    if (!triangle)
        dst->uv[1].uv = context.packed_uv;
    dst->v[0] = context.vertex_indices[kept0];
    if (triangle)
    {
        dst->v[1] = context.generated_vertex_idx - 2;
        dst->v[2] = context.generated_vertex_idx - 1;
        dst->v[3] = context.generated_vertex_idx - 1;
        uv0 = xport_gte_read_data(20);
        uv1 = xport_gte_read_data(22);
        dst->uv[1].uv = uv0;
        dst->uv[2].uv = uv1;
        QueueClippedTriangle(desc, &context);
    }
    else
    {
        dst->v[1] = context.vertex_indices[kept1];
        dst->v[2] = context.generated_vertex_idx - 2;
        dst->v[3] = context.generated_vertex_idx - 1;
        uv0 = xport_gte_read_data(20);
        uv1 = xport_gte_read_data(22);
        dst->uv[2].uv = uv0;
        dst->uv[3].uv = uv1;
        CopyClippedPolygonMetadata(*desc, *context.queued_desc, false);
    }
    context.queued_desc = AdvanceModelPolygonQueue();
    model_render_state.generated_vertex_idx = context.generated_vertex_idx;
    model_render_state.generated_cursor_valid = context.generated_cursor_valid;
    model_render_state.invalid_generated_cursor = context.invalid_generated_cursor;
}
