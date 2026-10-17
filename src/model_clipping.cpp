#include "game_sdk_bindings.h"
#include "model_clipping.h"
#include <stdlib.h>
extern "C" void xport_draft_gte_execute(unsigned int opcode);
extern "C"
{
#include "draft_first_signatures.h"
}

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
        GameDiagnostic("Cannot allocate clipped model polygon\n");
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
    dst.flags = (uint16)(triangle ? src.flags | 16 : src.flags & 0xFFEF);
    dst.source_stride = src.source_stride;
    dst.color = src.color;
    dst.n = src.n;
    dst.surface = src.surface;
    dst.clut = src.clut;
    dst.tpage = src.tpage;
    dst.window = GetModelPolygonWindow(src);
    dst.window_alias = 0;
    if (dst.source_stride == 20 && (dst.flags & 32))
    {
        // The original short descriptor writes its window over the next color
        dst.next->color = dst.window;
        dst.window_alias = &dst.next->color;
    }
    ++clipped_pending;
}

static uint32 DivideClipCoordinate(uint32 numerator, uint32 denominator)
{
    if (!denominator)
        return (sint32)numerator < 0 ? 1 : (uint32)-1;
    if (numerator == 0x80000000 && denominator == (uint32)-1)
        return numerator;
    return (uint32)((sint32)numerator / (sint32)denominator);
}

static void InterpolateNearPlaneEdge(const ModelPolygon *descriptor, NearPlaneClipContext *context, uint32 first_index, uint32 second_index, uint32 first_uv_offset, uint32 second_uv_offset)
{
    uint32 uv0, uv1, first_address, second_address, first_xy, second_xy, first_z, second_z;
    uint32 first_depth, second_depth, plane, numerator, fraction, first_x, first_y, second_x, second_y;
    uint32 x, y, z, packed, destination, first_u, first_v, second_u, second_v;
    uv0 = descriptor->uv[first_uv_offset].uv;
    uv1 = descriptor->uv[second_uv_offset].uv;
    context->packed_uv = uv0 | (uv1 << 16);
    first_address = context->projection_buffer + context->vertex_offsets[first_index];
    first_xy = r_u32(first_address + 8000);
    first_z = (uint32)(sint32)(sint16)r_u16(first_address + 4);
    xport_gte_write_control(0, first_xy);
    xport_gte_write_control(1, first_z);
    second_address = context->projection_buffer + context->vertex_offsets[second_index];
    second_xy = r_u32(second_address + 8000);
    second_z = (uint32)(sint32)(sint16)r_u16(second_address + 4);
    xport_gte_write_control(3, second_xy);
    xport_gte_write_control(4, second_z);
    xport_gte_write_data(0, r_u32(0x800FFB10));
    xport_gte_write_data(1, r_u32(0x800FFB14));
    xport_gte_execute(0x486012);
    plane = (uint32)(sint32)(sint16)r_u16(0x800FFB16);
    first_depth = xport_gte_read_data(25);
    second_depth = xport_gte_read_data(27);
    numerator = ((0 - plane) - first_depth) << 12;
    fraction = DivideClipCoordinate(numerator, second_depth - first_depth);
    first_x = (uint32)(sint32)(sint16)first_xy;
    first_y = (uint32)((sint32)first_xy >> 16);
    second_x = (uint32)(sint32)(sint16)second_xy;
    second_y = (uint32)((sint32)second_xy >> 16);
    xport_gte_write_data(25, first_x);
    xport_gte_write_data(26, first_y);
    xport_gte_write_data(27, first_z);
    xport_gte_write_data(9, second_x - first_x);
    xport_gte_write_data(10, second_y - first_y);
    xport_gte_write_data(11, second_z - first_z);
    xport_gte_write_control(0, 4096);
    xport_gte_write_control(1, 0);
    xport_gte_write_control(2, 4096);
    xport_gte_write_control(3, 0);
    xport_gte_write_control(4, 4096);
    xport_gte_write_control(5, 0);
    xport_gte_write_control(6, 0);
    xport_gte_write_control(7, 0);
    xport_gte_write_data(8, fraction);
    xport_draft_gte_execute(0x1A8003E);
    x = xport_gte_read_data(25);
    y = xport_gte_read_data(26);
    z = xport_gte_read_data(27);
    packed = (x & 65535) | (y << 16);
    xport_gte_write_data(0, packed);
    xport_gte_write_data(1, z);
    xport_gte_execute(0x180001);
    destination = context->generated_vertex_cursor;
    w_u32(destination + 8000, packed);
    w_u32(destination + 4, (z & 65535) | 0xFF000000);
    w_u32(destination, xport_gte_read_data(14));
    context->generated_vertex_cursor = destination + 8;
    packed = context->packed_uv;
    first_u = (packed << 4) & 0xFF0;
    first_v = (packed >> 4) & 0xFF0;
    xport_gte_write_data(25, first_u);
    xport_gte_write_data(26, first_v);
    second_u = (packed >> 12) & 0xFF0;
    second_v = (packed >> 20) & 0xFF0;
    xport_gte_write_data(9, second_u - first_u);
    xport_gte_write_data(10, second_v - first_v);
    xport_gte_write_data(8, fraction);
    xport_draft_gte_execute(0x1A8003E);
}

void ClipModelEdge01(const ModelPolygon *descriptor, NearPlaneClipContext *context)
{
    FUNCTION_MARKER(0x80083BF4, "SLUS_003.73");
    InterpolateNearPlaneEdge(descriptor, context, 0, 1, 0, 1);
}

void ClipModelEdge12(const ModelPolygon *descriptor, NearPlaneClipContext *context)
{
    FUNCTION_MARKER(0x80083C0C, "SLUS_003.73");
    InterpolateNearPlaneEdge(descriptor, context, 1, 2, 1, 2);
}

void ClipModelEdge20(const ModelPolygon *descriptor, NearPlaneClipContext *context)
{
    FUNCTION_MARKER(0x80083C24, "SLUS_003.73");
    InterpolateNearPlaneEdge(descriptor, context, 2, 0, 2, 0);
}

void ClipModelEdge13(const ModelPolygon *descriptor, NearPlaneClipContext *context)
{
    FUNCTION_MARKER(0x80083C3C, "SLUS_003.73");
    InterpolateNearPlaneEdge(descriptor, context, 1, 3, 1, 3);
}

void ClipModelEdge32(const ModelPolygon *descriptor, NearPlaneClipContext *context)
{
    FUNCTION_MARKER(0x80083C54, "SLUS_003.73");
    InterpolateNearPlaneEdge(descriptor, context, 3, 2, 3, 2);
}

void QueueClippedTriangle(const ModelPolygon *descriptor, NearPlaneClipContext *context)
{
    FUNCTION_MARKER(0x80083E54, "SLUS_003.73");
    CopyClippedPolygonMetadata(*descriptor, *context->queued_descriptor, true);
}









enum NearPlaneEdge
{
    Edge01,
    Edge12,
    Edge20,
    Edge13,
    Edge32
};

static void InterpolateSelectedClipEdge(const ModelPolygon *descriptor, NearPlaneClipContext *context, NearPlaneEdge edge)
{
    switch (edge)
    {
        case Edge01: ClipModelEdge01(descriptor, context); break;
        case Edge12: ClipModelEdge12(descriptor, context); break;
        case Edge20: ClipModelEdge20(descriptor, context); break;
        case Edge13: ClipModelEdge13(descriptor, context); break;
        case Edge32: ClipModelEdge32(descriptor, context); break;
    }
}

static void DuplicateClipTriangle(const ModelPolygon *descriptor, NearPlaneClipContext *context, uint32 flags, uint32 marked, uint32 first, uint32 second, uint32 third, uint32 first_uv, uint32 second_uv, uint32 third_uv)
{
    uint32 address, value, uv0, uv1, uv2;
    ModelPolygon *destination;
    address = context->projection_buffer + context->vertex_offsets[marked];
    value = r_u16(address + 6);
    w_u16(address + 6, (value & 0xFF7F) | 0x8000);
    QueueClippedTriangle(descriptor, context);
    destination = context->queued_descriptor;
    destination->v[0] = (uint16)(context->vertex_offsets[first]) >> 3;
    destination->v[1] = (uint16)(context->vertex_offsets[second]) >> 3;
    destination->v[2] = (uint16)(context->vertex_offsets[third]) >> 3;
    destination->v[3] = (uint16)(context->vertex_offsets[third]) >> 3;
    uv0 = descriptor->uv[first_uv].uv;
    uv1 = descriptor->uv[second_uv].uv;
    uv2 = descriptor->uv[third_uv].uv;
    destination->uv[0].uv = (uint16)(uv0);
    destination->uv[1].uv = (uint16)(uv1);
    destination->uv[2].uv = (uint16)(uv2);
    context->queued_descriptor = AdvanceModelPolygonQueue();
}

static void QueueClippedQuadMetadata(const ModelPolygon *descriptor, NearPlaneClipContext *context)
{
    CopyClippedPolygonMetadata(*descriptor, *context->queued_descriptor, false);
}


void QueueNearPlanePolygon(const ModelPolygon *descriptor, uint32 scratch, uint32 projection, uint32 flags, const uint32 offsets[4], const uint32 depths[4])
{
    NearPlaneClipContext context;
    NearPlaneEdge first_edge, second_edge;
    uint32 outside0, outside1, outside2, outside3, first_uv_high, kept0, kept1 = 0, triangle;
    uint32 axis, generated_offset, uv0, uv1, current_projection;
    ModelPolygon *destination;
    context.projection_buffer = projection;
    context.generated_vertex_cursor = r_u32(scratch + 468);
    context.queued_descriptor = GetQueuedModelPolygon();
    context.scratch_base = scratch;
    context.packed_uv = 0;
    for (axis = 0; axis < 4; ++axis)
        context.vertex_offsets[axis] = offsets[axis];
    outside0 = depths[0] & 0x800000;
    outside1 = depths[1] & 0x800000;
    outside2 = depths[2] & 0x800000;
    triangle = 0;
    if (flags & 16)
    {
        if (!outside0 && !outside1)
        {
            first_edge = Edge20; second_edge = Edge12;
            first_uv_high = 1; kept0 = 0; kept1 = 1;
        }
        else if (!outside0 && !outside2)
        {
            first_edge = Edge12; second_edge = Edge01;
            first_uv_high = 1; kept0 = 2; kept1 = 0;
        }
        else if (!outside0)
        {
            first_edge = Edge01; second_edge = Edge20;
            first_uv_high = 0; kept0 = 0; triangle = 1;
        }
        else if (outside1)
        {
            first_edge = Edge20; second_edge = Edge12;
            first_uv_high = 0; kept0 = 2; triangle = 1;
        }
        else if (outside2)
        {
            first_edge = Edge12; second_edge = Edge01;
            first_uv_high = 0; kept0 = 1; triangle = 1;
        }
        else
        {
            first_edge = Edge01; second_edge = Edge20;
            first_uv_high = 1; kept0 = 1; kept1 = 2;
        }
    }
    else
    {
        outside3 = depths[3] & 0x800000;
        if (!outside0 && !outside1 && !outside2)
        {
            DuplicateClipTriangle(descriptor, &context, flags, 0, 2, 0, 1, 2, 0, 1);
            first_edge = Edge32; second_edge = Edge13;
            first_uv_high = 1; kept0 = 2; kept1 = 1;
        }
        else if (!outside0 && !outside1 && !outside3)
        {
            DuplicateClipTriangle(descriptor, &context, flags, 1, 0, 1, 3, 0, 1, 3);
            first_edge = Edge20; second_edge = Edge32;
            first_uv_high = 1; kept0 = 0; kept1 = 3;
        }
        else if (!outside0 && !outside1)
        {
            first_edge = Edge20; second_edge = Edge13;
            first_uv_high = 1; kept0 = 0; kept1 = 1;
        }
        else if (!outside0 && outside2)
        {
            first_edge = Edge01; second_edge = Edge20;
            first_uv_high = 0; kept0 = 0; triangle = 1;
        }
        else if (!outside0 && !outside3)
        {
            DuplicateClipTriangle(descriptor, &context, flags, 2, 3, 2, 0, 3, 2, 0);
            first_edge = Edge13; second_edge = Edge01;
            first_uv_high = 1; kept0 = 3; kept1 = 0;
        }
        else if (!outside0)
        {
            first_edge = Edge32; second_edge = Edge01;
            first_uv_high = 1; kept0 = 2; kept1 = 0;
        }
        else if (!outside1 && !outside2)
        {
            DuplicateClipTriangle(descriptor, &context, flags, 3, 1, 3, 2, 1, 3, 2);
            first_edge = Edge01; second_edge = Edge20;
            first_uv_high = 1; kept0 = 1; kept1 = 2;
        }
        else if (!outside1 && !outside3)
        {
            first_edge = Edge01; second_edge = Edge32;
            first_uv_high = 1; kept0 = 1; kept1 = 3;
        }
        else if (!outside1)
        {
            first_edge = Edge13; second_edge = Edge01;
            first_uv_high = 0; kept0 = 1; triangle = 1;
        }
        else if (!outside2 && !outside3)
        {
            first_edge = Edge13; second_edge = Edge20;
            first_uv_high = 1; kept0 = 3; kept1 = 2;
        }
        else if (!outside2)
        {
            first_edge = Edge20; second_edge = Edge32;
            first_uv_high = 0; kept0 = 2; triangle = 1;
        }
        else
        {
            first_edge = Edge32; second_edge = Edge13;
            first_uv_high = 0; kept0 = 3; triangle = 1;
        }
    }
    destination = context.queued_descriptor;
    InterpolateSelectedClipEdge(descriptor, &context, first_edge);
    uv0 = first_uv_high ? context.packed_uv >> 16 : context.packed_uv;
    destination->uv[0].uv = (uint16)(uv0);
    InterpolateSelectedClipEdge(descriptor, &context, second_edge);
    if (!triangle)
        destination->uv[1].uv = (uint16)(context.packed_uv);
    current_projection = r_u32(0x800FFAC0);
    destination->v[0] = (uint16)(context.vertex_offsets[kept0]) >> 3;
    if (triangle)
    {
        generated_offset = context.generated_vertex_cursor - current_projection - 16;
        destination->v[1] = (uint16)(generated_offset) >> 3;
        generated_offset += 8;
        destination->v[2] = (uint16)(generated_offset) >> 3;
        destination->v[3] = (uint16)(generated_offset) >> 3;
        uv0 = xport_gte_read_data(20);
        uv1 = xport_gte_read_data(22);
        destination->uv[1].uv = (uint16)(uv0);
        destination->uv[2].uv = (uint16)(uv1);
        QueueClippedTriangle(descriptor, &context);
    }
    else
    {
        destination->v[1] = (uint16)(context.vertex_offsets[kept1]) >> 3;
        generated_offset = context.generated_vertex_cursor - current_projection - 16;
        destination->v[2] = (uint16)(generated_offset) >> 3;
        destination->v[3] = (uint16)(generated_offset + 8) >> 3;
        uv0 = xport_gte_read_data(20);
        uv1 = xport_gte_read_data(22);
        destination->uv[2].uv = (uint16)(uv0);
        destination->uv[3].uv = (uint16)(uv1);
        QueueClippedQuadMetadata(descriptor, &context);
    }
    context.queued_descriptor = AdvanceModelPolygonQueue();
    w_u32(scratch + 468, context.generated_vertex_cursor);

}


