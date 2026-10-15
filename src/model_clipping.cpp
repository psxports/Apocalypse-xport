extern "C" void xport_draft_gte_execute(unsigned int opcode);
extern "C"
{
#include "draft_first_signatures.h"
}

static uint32 DivideClipCoordinate(uint32 numerator, uint32 denominator)
{
    if (!denominator)
        return (sint32)numerator < 0 ? 1 : (uint32)-1;
    if (numerator == 0x80000000 && denominator == (uint32)-1)
        return numerator;
    return (uint32)((sint32)numerator / (sint32)denominator);
}

static void InterpolateNearPlaneEdge(uint32 descriptor, NearPlaneClipContext *context, uint32 first_index, uint32 second_index, uint32 first_uv_offset, uint32 second_uv_offset)
{
    uint32 uv0, uv1, first_address, second_address, first_xy, second_xy, first_z, second_z;
    uint32 first_depth, second_depth, plane, numerator, fraction, first_x, first_y, second_x, second_y;
    uint32 x, y, z, packed, destination, first_u, first_v, second_u, second_v;
    uv0 = r_u16(descriptor + first_uv_offset);
    uv1 = r_u16(descriptor + second_uv_offset);
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

void ClipModelEdge01(uint32 descriptor, NearPlaneClipContext *context)
{
    FUNCTION_MARKER(0x80083BF4, "SLUS_003.73");
    InterpolateNearPlaneEdge(descriptor, context, 0, 1, 20, 24);
}

void ClipModelEdge12(uint32 descriptor, NearPlaneClipContext *context)
{
    FUNCTION_MARKER(0x80083C0C, "SLUS_003.73");
    InterpolateNearPlaneEdge(descriptor, context, 1, 2, 24, 28);
}

void ClipModelEdge20(uint32 descriptor, NearPlaneClipContext *context)
{
    FUNCTION_MARKER(0x80083C24, "SLUS_003.73");
    InterpolateNearPlaneEdge(descriptor, context, 2, 0, 28, 20);
}

void ClipModelEdge13(uint32 descriptor, NearPlaneClipContext *context)
{
    FUNCTION_MARKER(0x80083C3C, "SLUS_003.73");
    InterpolateNearPlaneEdge(descriptor, context, 1, 3, 24, 30);
}

void ClipModelEdge32(uint32 descriptor, NearPlaneClipContext *context)
{
    FUNCTION_MARKER(0x80083C54, "SLUS_003.73");
    InterpolateNearPlaneEdge(descriptor, context, 3, 2, 30, 28);
}

void QueueClippedTriangle(uint32 descriptor, NearPlaneClipContext *context)
{
    uint32 flags, color, parameters, clut, page, window, destination, count;
    FUNCTION_MARKER(0x80083E54, "SLUS_003.73");
    flags = r_u32(descriptor);
    color = r_u32(descriptor + 12);
    parameters = r_u32(descriptor + 16);
    clut = (uint32)(sint32)(sint16)r_u16(descriptor + 22);
    page = (uint32)(sint32)(sint16)r_u16(descriptor + 26);
    window = r_u32(descriptor + 32);
    destination = context->queued_descriptor;
    w_u32(destination, flags | 16);
    w_u32(destination + 12, color);
    w_u32(destination + 16, parameters);
    w_u16(destination + 22, clut);
    w_u16(destination + 26, page);
    w_u32(destination + 32, window);
    count = r_u32(context->scratch_base + 500);
    w_u32(context->scratch_base + 500, count + 1);
}

extern "C" void sub_80083BF4(uint32 descriptor, NearPlaneClipContext *context)
{
    FUNCTION_MARKER(0x80083BF4, "SLUS_003.73");
    ClipModelEdge01(descriptor, context);
}
extern "C" void sub_80083C0C(uint32 descriptor, NearPlaneClipContext *context)
{
    FUNCTION_MARKER(0x80083C0C, "SLUS_003.73");
    ClipModelEdge12(descriptor, context);
}
extern "C" void sub_80083C24(uint32 descriptor, NearPlaneClipContext *context)
{
    FUNCTION_MARKER(0x80083C24, "SLUS_003.73");
    ClipModelEdge20(descriptor, context);
}
extern "C" void sub_80083C3C(uint32 descriptor, NearPlaneClipContext *context)
{
    FUNCTION_MARKER(0x80083C3C, "SLUS_003.73");
    ClipModelEdge13(descriptor, context);
}
extern "C" void sub_80083C54(uint32 descriptor, NearPlaneClipContext *context)
{
    FUNCTION_MARKER(0x80083C54, "SLUS_003.73");
    ClipModelEdge32(descriptor, context);
}
extern "C" void sub_80083E54(uint32 descriptor, NearPlaneClipContext *context)
{
    FUNCTION_MARKER(0x80083E54, "SLUS_003.73");
    QueueClippedTriangle(descriptor, context);
}

enum NearPlaneEdge
{
    Edge01,
    Edge12,
    Edge20,
    Edge13,
    Edge32
};

static void InterpolateSelectedClipEdge(uint32 descriptor, NearPlaneClipContext *context, NearPlaneEdge edge)
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

static void DuplicateClipTriangle(uint32 descriptor, NearPlaneClipContext *context, uint32 flags, uint32 marked, uint32 first, uint32 second, uint32 third, uint32 first_uv, uint32 second_uv, uint32 third_uv)
{
    uint32 address, value, destination, uv0, uv1, uv2;
    address = context->projection_buffer + context->vertex_offsets[marked];
    value = r_u16(address + 6);
    w_u16(address + 6, (value & 0xFF7F) | 0x8000);
    QueueClippedTriangle(descriptor, context);
    destination = context->queued_descriptor;
    w_u16(destination + 4, context->vertex_offsets[first]);
    w_u16(destination + 6, context->vertex_offsets[second]);
    w_u16(destination + 8, context->vertex_offsets[third]);
    w_u16(destination + 10, context->vertex_offsets[third]);
    uv0 = r_u16(descriptor + first_uv);
    uv1 = r_u16(descriptor + second_uv);
    uv2 = r_u16(descriptor + third_uv);
    w_u16(destination + 20, uv0);
    w_u16(destination + 24, uv1);
    w_u16(destination + 28, uv2);
    context->queued_descriptor += flags >> 16;
}

static void QueueClippedQuadMetadata(uint32 descriptor, NearPlaneClipContext *context)
{
    uint32 flags, color, parameters, clut, page, window, destination, count;
    flags = r_u32(descriptor);
    color = r_u32(descriptor + 12);
    parameters = r_u32(descriptor + 16);
    clut = (uint32)(sint32)(sint16)r_u16(descriptor + 22);
    page = (uint32)(sint32)(sint16)r_u16(descriptor + 26);
    window = r_u32(descriptor + 32);
    destination = context->queued_descriptor;
    w_u32(destination, flags & 0xFFFFFFEF);
    w_u32(destination + 12, color);
    w_u32(destination + 16, parameters);
    w_u16(destination + 22, clut);
    w_u16(destination + 26, page);
    w_u32(destination + 32, window);
    count = r_u32(context->scratch_base + 500);
    w_u32(context->scratch_base + 500, count + 1);
}

void QueueNearPlanePolygon(uint32 descriptor, uint32 scratch, uint32 projection, uint32 flags, const uint32 offsets[4], const uint32 depths[4])
{
    NearPlaneClipContext context;
    NearPlaneEdge first_edge, second_edge;
    uint32 outside0, outside1, outside2, outside3, first_uv_high, kept0, kept1 = 0, triangle;
    uint32 axis, destination, generated_offset, uv0, uv1, current_projection;
    context.projection_buffer = projection;
    context.generated_vertex_cursor = r_u32(scratch + 468);
    context.queued_descriptor = r_u32(scratch + 484);
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
            DuplicateClipTriangle(descriptor, &context, flags, 0, 2, 0, 1, 28, 20, 24);
            first_edge = Edge32; second_edge = Edge13;
            first_uv_high = 1; kept0 = 2; kept1 = 1;
        }
        else if (!outside0 && !outside1 && !outside3)
        {
            DuplicateClipTriangle(descriptor, &context, flags, 1, 0, 1, 3, 20, 24, 30);
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
            DuplicateClipTriangle(descriptor, &context, flags, 2, 3, 2, 0, 30, 28, 20);
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
            DuplicateClipTriangle(descriptor, &context, flags, 3, 1, 3, 2, 24, 30, 28);
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
    w_u16(destination + 20, uv0);
    InterpolateSelectedClipEdge(descriptor, &context, second_edge);
    if (!triangle)
        w_u16(destination + 24, context.packed_uv);
    current_projection = r_u32(0x800FFAC0);
    w_u16(destination + 4, context.vertex_offsets[kept0]);
    if (triangle)
    {
        generated_offset = context.generated_vertex_cursor - current_projection - 16;
        w_u16(destination + 6, generated_offset);
        generated_offset += 8;
        w_u16(destination + 8, generated_offset);
        w_u16(destination + 10, generated_offset);
        uv0 = xport_gte_read_data(20);
        uv1 = xport_gte_read_data(22);
        w_u16(destination + 24, uv0);
        w_u16(destination + 28, uv1);
        QueueClippedTriangle(descriptor, &context);
    }
    else
    {
        w_u16(destination + 6, context.vertex_offsets[kept1]);
        generated_offset = context.generated_vertex_cursor - current_projection - 16;
        w_u16(destination + 8, generated_offset);
        w_u16(destination + 10, generated_offset + 8);
        uv0 = xport_gte_read_data(20);
        uv1 = xport_gte_read_data(22);
        w_u16(destination + 28, uv0);
        w_u16(destination + 30, uv1);
        QueueClippedQuadMetadata(descriptor, &context);
    }
    context.queued_descriptor += flags >> 16;
    w_u32(scratch + 468, context.generated_vertex_cursor);
    w_u32(scratch + 484, context.queued_descriptor);
}

extern "C" void apocalypse_model_nearplane_native(uint32 descriptor, uint32 scratch, uint32 projection, uint32 flags, const uint32 offsets[4], const uint32 depths[4])
{
    QueueNearPlanePolygon(descriptor, scratch, projection, flags, offsets, depths);
}
