extern "C" void xport_draft_gte_execute(unsigned int opcode);
extern "C"
{
#include "draft_first_signatures.h"
}

static void LoadSubdivisionGeometry(uint32 input0, uint32 input1, uint32 input2)
{
    uint32 row0 = r_u32(input0 + 8000);
    uint32 tail0 = r_u16(input0 + 4);
    uint32 row1 = r_u32(input1 + 8000);
    uint32 tail1 = r_u16(input1 + 4);
    uint32 row2 = r_u32(input2 + 8000);
    uint32 tail2 = r_u16(input2 + 4);
    FUNCTION_MARKER(0x80082508, "SLUS_003.73");
    xport_gte_write_control(0, (row0 & 0xFFFF) | (row1 << 16));
    xport_gte_write_control(1, ((row2 ^ row0) & 0xFFFF) ^ row0);
    xport_gte_write_control(2, (row1 >> 16) | (row2 & 0xFFFF0000));
    xport_gte_write_control(3, tail0 | (tail1 << 16));
    xport_gte_write_control(4, tail2);
}

static void InterpolateSubdivisionGrid(uint32 vertices, uint32 stride, uint32 flags, const uint32 geometry[3], uint32 subdivisions, uint32 step)
{
    const uint32 clip_bias = 0x00F00200, clip_mask = 0xBFFFBFFF;
    uint32 color0, color1, color2, row_remaining, inner_remaining;
    uint32 vertical = 0, row_start, row_end = 0x1F800000, point, remaining, horizontal, packed;
    uint32 screen, flag, color;
    FUNCTION_MARKER(0x80082638, "SLUS_003.73");
    if (flags & 0x40000000)
    {
        color0 = r_u32(vertices + 12);
        color1 = r_u32(vertices + stride + 12);
        color2 = r_u32(vertices + 2 * stride + 12);
        xport_gte_write_control(8, ((color0 & 255) << 4) | ((color1 & 255) << 20));
        xport_gte_write_control(9, ((color2 & 255) << 4) | ((color0 & 0xFF00) << 12));
        xport_gte_write_control(10, ((color1 & 0xFF00) >> 4) | ((color2 & 0xFF00) << 12));
        xport_gte_write_control(11, 0);
        xport_gte_write_control(12, 0);
    }
    if ((sint32)flags < 0)
    {
        color0 = r_u32(vertices + 4);
        color1 = r_u32(vertices + stride + 4);
        color2 = r_u32(vertices + 2 * stride + 4);
        xport_gte_write_control(16, ((color0 & 255) << 4) | ((color1 & 255) << 20));
        xport_gte_write_control(17, ((color2 & 255) << 4) | ((color0 & 0xFF00) << 12));
        xport_gte_write_control(18, ((color1 & 0xFF00) >> 4) | ((color2 & 0xFF00) << 12));
        xport_gte_write_control(19, ((color0 >> 12) & 0xFF0) | (((color1 >> 12) & 0xFF0) << 16));
        xport_gte_write_control(20, (color2 >> 12) & 0xFF0);
    }
    LoadSubdivisionGeometry(geometry[0], geometry[1], geometry[2]);
    row_remaining = subdivisions;
    do
    {
        row_start = row_end;
        if ((sint32)(vertical - 3840) >= 0)
            vertical = 4096;
        remaining = 4096 - vertical;
        horizontal = 0;
        xport_gte_write_data(0, remaining);
        xport_gte_write_data(1, vertical);
        point = row_start - 16;
        inner_remaining = row_remaining;
        do
        {
            xport_gte_execute(0x180001);
            point += 16;
            remaining -= step;
            horizontal += step;
            packed = (horizontal << 16) | remaining;
            if ((sint32)(horizontal - 3840) >= 0)
                packed = 0x10000000;
            screen = xport_gte_read_data(14);
            flag = xport_gte_read_flag();
            xport_gte_execute(0x4A6412);
            screen = ((screen & clip_mask) | ~((screen - clip_bias) | clip_mask)) & 0x7FFFFFFF;
            xport_draft_gte_execute(0x198003D);
            w_u32(point, screen | (flag & 0x80000000));
            color = xport_gte_read_data(22);
            w_u32(point + 8, color);
            if ((sint32)flags < 0)
            {
                xport_gte_execute(0x4C6412);
                xport_draft_gte_execute(0x198003D);
                w_u32(point + 12, xport_gte_read_data(22));
            }
            xport_gte_write_data(0, packed);
        } while (inner_remaining-- != 0);
        vertical += step;
        row_end = row_start + 128;
    } while (row_remaining-- != 0);
    /* The MIPS continuation returns to the caller with its packet cursor unchanged */
}

static void InterpolateReverseSubdivisionGrid(uint32 attributes, uint32 stride, uint32 attribute_flags, const uint32 geometry[3], uint32 subdivisions, uint32 step, uint32 destination)
{
    uint32 color0, color1, color2, row_count = subdivisions - 1;
    uint32 row_end = destination + 16 * (subdivisions - 8);
    uint32 row_start, point, vertical = 0, horizontal, remaining, inner_count;
    uint32 packed, screen, flag;
    FUNCTION_MARKER(0x8008267C, "SLUS_003.73");
    /* Generate the opposite triangle in the shared interpolation grid */
    if (attribute_flags & 0x40000000)
    {
        color0 = r_u32(attributes + 12);
        color1 = r_u32(attributes + stride + 12);
        color2 = r_u32(attributes + 2 * stride + 12);
        xport_gte_write_control(8, ((color0 & 255) << 4) | ((color1 & 255) << 20));
        xport_gte_write_control(9, ((color2 & 255) << 4) | ((color0 & 0xFF00) << 12));
        xport_gte_write_control(10, ((color1 & 0xFF00) >> 4) | ((color2 & 0xFF00) << 12));
        xport_gte_write_control(11, 0);
        xport_gte_write_control(12, 0);
    }
    if ((sint32)attribute_flags < 0)
    {
        color0 = r_u32(attributes + 4);
        color1 = r_u32(attributes + stride + 4);
        color2 = r_u32(attributes + 2 * stride + 4);
        xport_gte_write_control(16, ((color0 & 255) << 4) | ((color1 & 255) << 20));
        xport_gte_write_control(17, ((color2 & 255) << 4) | ((color0 & 0xFF00) << 12));
        xport_gte_write_control(18, ((color1 & 0xFF00) >> 4) | ((color2 & 0xFF00) << 12));
        xport_gte_write_control(19, ((color0 >> 12) & 0xFF0) | (((color1 >> 12) & 0xFF0) << 16));
        xport_gte_write_control(20, (color2 >> 12) & 0xFF0);
    }
    LoadSubdivisionGeometry(geometry[0], geometry[1], geometry[2]);
    do
    {
        row_start = row_end;
        if ((sint32)(vertical - 3840) >= 0)
            vertical = 4096;
        remaining = 4096 - vertical;
        horizontal = 0;
        xport_gte_write_data(0, remaining);
        xport_gte_write_data(1, vertical);
        point = row_start + 16;
        inner_count = row_count;
        do
        {
            xport_gte_execute(0x180001);
            point -= 16;
            remaining -= step;
            horizontal += step;
            packed = (horizontal << 16) | remaining;
            if ((sint32)(horizontal - 3840) >= 0)
                packed = 0x10000000;
            screen = xport_gte_read_data(14);
            flag = xport_gte_read_flag();
            xport_gte_execute(0x4A6412);
            screen = ((screen & 0xBFFFBFFF) | ~((screen - 0x00F00200) | 0xBFFFBFFF)) & 0x7FFFFFFF;
            xport_draft_gte_execute(0x198003D);
            w_u32(point, screen | (flag & 0x80000000));
            w_u32(point + 8, xport_gte_read_data(22));
            if ((sint32)attribute_flags < 0)
            {
                xport_gte_execute(0x4C6412);
                xport_draft_gte_execute(0x198003D);
                w_u32(point + 12, xport_gte_read_data(22));
            }
            xport_gte_write_data(0, packed);
        } while (inner_count-- != 0);
        vertical += step;
        row_end = row_start - 128;
    } while (row_count-- != 0);
    /* The packet cursor is unchanged by this interpolation helper */
}

extern "C" void sub_80082508(uint32 input0, uint32 input1, uint32 input2)
{
    FUNCTION_MARKER(0x80082508, "SLUS_003.73");
    LoadSubdivisionGeometry(input0, input1, input2);
}
extern "C" void sub_80082638(uint32 vertices, uint32 stride, uint32 flags, const uint32 geometry[3], uint32 subdivisions, uint32 step)
{
    FUNCTION_MARKER(0x80082638, "SLUS_003.73");
    InterpolateSubdivisionGrid(vertices, stride, flags, geometry, subdivisions, step);
}
extern "C" void sub_8008267C(uint32 attributes, uint32 stride, uint32 attribute_flags, const uint32 geometry[3], uint32 subdivisions, uint32 step, uint32 destination)
{
    FUNCTION_MARKER(0x8008267C, "SLUS_003.73");
    InterpolateReverseSubdivisionGrid(attributes, stride, attribute_flags, geometry, subdivisions, step, destination);
}

static uint32 SelectSubdivisionResolution(uint32 packet, uint32 area, uint32 *step, uint32 *subdivisions)
{
    uint32 command, leading_count, count;
    FUNCTION_MARKER(0x80082394, "SLUS_003.73");
    // TODO The native caller contract omits original CPU register saves in scratchpad
    xport_gte_write_data(30, area >> 11);
    command = r_u32(packet + 4);
    xport_gte_write_data(6, (command & 0xFF000000) | 0x08000000);
    leading_count = xport_gte_read_data(31);
    count = 17 - (leading_count >> 1);
    if ((sint32)count >= 8)
        count = 7;
    *subdivisions = count;
    // MIPS DIVU supplies all-one LO for a zero divisor
    *step = count == 0 ? 0xFFFFFFFF : 4096 / count;
    return packet;
}

extern "C" uint32 sub_80082394(uint32 packet, uint32 area, uint32 *step, uint32 *subdivisions)
{
    FUNCTION_MARKER(0x80082394, "SLUS_003.73");
    return SelectSubdivisionResolution(packet, area, step, subdivisions);
}

static uint32 InterpolateSubdivisionEdge(uint32 position, uint32 count, uint32 stride, uint32 previous, uint32 next)
{
    uint32 difference = next - previous;
    uint32 delta = 16 * ((difference & 0x2000) - 0x1000) - (((uint32)((sint32)difference >> 28) & 2) - 1);
    uint32 cursor = position + stride;
    uint32 remaining = count - 1;
    FUNCTION_MARKER(0x800826C4, "SLUS_003.73");
    // TODO The original underflows its iteration counter when count is below two
    do
    {
        uint32 point = r_u32(cursor);
        --remaining;
        w_u32(cursor, point + delta);
        cursor += stride;
    } while (remaining != 0);
    return cursor;
}

static void PrepareSubdivisionEdges(uint32 count, uint32 corner0, uint32 corner1, uint32 corner2, uint32 corner3)
{
    uint32 position = 0x1F800000;
    FUNCTION_MARKER(0x80082750, "SLUS_003.73");
    // Polygon owners carry the clipping mask separately in their strip context
    position = InterpolateSubdivisionEdge(position, count, 16, corner0, corner1);
    position = InterpolateSubdivisionEdge(position, count, 128, corner1, corner3);
    position = InterpolateSubdivisionEdge(position, count, 0xFFFFFFF0, corner3, corner2);
    InterpolateSubdivisionEdge(position, count, 0xFFFFFF80, corner2, corner0);
}

static void PrepareTriangleSubdivisionEdges(uint32 count, uint32 corner0, uint32 corner1, uint32 corner2)
{
    uint32 position = 0x1F800000;
    FUNCTION_MARKER(0x8008270C, "SLUS_003.73");
    // Polygon owners carry the clipping mask separately in their strip context
    position = InterpolateSubdivisionEdge(position, count, 16, corner0, corner1);
    position = InterpolateSubdivisionEdge(position, count, 112, corner1, corner2);
    InterpolateSubdivisionEdge(position, count, 0xFFFFFF80, corner2, corner0);
}

extern "C" void sub_8008270C(uint32 count, uint32 corner0, uint32 corner1, uint32 corner2)
{
    FUNCTION_MARKER(0x8008270C, "SLUS_003.73");
    PrepareTriangleSubdivisionEdges(count, corner0, corner1, corner2);
}

extern "C" void sub_800826C4(uint32 position, uint32 count, uint32 stride, uint32 previous, uint32 next)
{
    FUNCTION_MARKER(0x800826C4, "SLUS_003.73");
    InterpolateSubdivisionEdge(position, count, stride, previous, next);
}
extern "C" void sub_80082750(uint32 count, uint32 corner0, uint32 corner1, uint32 corner2, uint32 corner3)
{
    FUNCTION_MARKER(0x80082750, "SLUS_003.73");
    PrepareSubdivisionEdges(count, corner0, corner1, corner2, corner3);
}

uint32 EmitFlatPolygonStrip(uint32 geometry, xport_draft_polygon_strip_context *context)
{
    uint32 packet = context->packet_cursor;
    uint32 previous = context->clipping_mask;
    uint32 remaining = context->remaining_segments;
    uint32 first, second, combined;
    context->disposition = 0;
    FUNCTION_MARKER(0x800827A4, "SLUS_003.73");
    do
    {
        w_u32(packet + 4, context->packet_words[1]);
        if (packet >= context->packet_limit)
        {
            context->packet_cursor = packet;
            context->clipping_mask = previous;
            context->remaining_segments = remaining;
            context->disposition = 1;
            return packet;
        }
        w_u32(packet + 8, context->packet_words[2]);
        w_u32(packet + 12, context->packet_words[3]);
        first = r_u32(geometry + 16);
        second = r_u32(geometry + 144);
        geometry += 16;
        combined = first & second;
        if (!(previous & combined))
        {
            packet += 24;
            w_u32(packet - 24, packet + 0x05000000);
            w_u32(packet - 8, first);
            w_u32(packet - 4, second);
            context->packet_stride = 24;
        }
        previous = combined & context->frustum_mask;
        context->packet_words[2] = first;
        context->packet_words[3] = second;
    } while (remaining-- != 0);
    context->packet_cursor = packet;
    context->clipping_mask = previous;
    context->remaining_segments = remaining;
    return packet;
}

extern "C" uint32 sub_800827A4(uint32 geometry, xport_draft_polygon_strip_context *context)
{
    FUNCTION_MARKER(0x800827A4, "SLUS_003.73");
    return EmitFlatPolygonStrip(geometry, context);
}

uint32 EmitTexturedPolygonStrip(uint32 geometry, xport_draft_polygon_strip_context *context)
{
    uint32 packet = context->packet_cursor, clipping = context->clipping_mask;
    uint32 remaining = context->remaining_segments;
    uint32 word0 = context->packet_words[0], word1 = context->packet_words[1];
    uint32 word2 = context->packet_words[2], word3 = context->packet_words[3];
    uint32 word4 = context->packet_words[4];
    uint32 repeat;
    context->disposition = 0;
    FUNCTION_MARKER(0x80082B14, "SLUS_003.73");
    do
    {
        uint32 next0, next1, next2, next3, mask;
        if (packet >= context->packet_limit)
        {
            context->packet_cursor = packet;
            context->clipping_mask = clipping;
            context->remaining_segments = remaining;
            context->packet_words[0] = word0;
            context->packet_words[1] = word1;
            context->packet_words[2] = word2 | context->texture_high0;
            context->packet_words[3] = word3;
            context->packet_words[4] = word4;
            context->disposition = 1;
            return packet;
        }
        w_u32(packet + 4, word0);
        w_u32(packet + 8, word1);
        w_u32(packet + 12, word2 | context->texture_high0);
        w_u32(packet + 16, word3);
        w_u32(packet + 20, word4 | context->texture_high1);
        next0 = r_u32(geometry + 16);
        next1 = r_u16(geometry + 24);
        next2 = r_u32(geometry + 144);
        next3 = r_u16(geometry + 152);
        geometry += 16;
        mask = next0 & next2;
        repeat = clipping & mask;
        clipping = mask & context->frustum_mask;
        if (!repeat)
        {
            packet += 40;
            w_u32(packet - 40, packet + 0x09000000);
            w_u32(packet - 16, next0);
            w_u32(packet - 12, next1);
            w_u32(packet - 8, next2);
            w_u32(packet - 4, next3);
            context->packet_stride = 40;
        }
        word1 = next0;
        word2 = next1;
        word3 = next2;
        word4 = next3;
        repeat = remaining != 0;
        --remaining;
    } while (repeat);
    context->packet_cursor = packet;
    context->clipping_mask = clipping;
    context->remaining_segments = remaining;
    context->packet_words[0] = word0;
    context->packet_words[1] = word1;
    context->packet_words[2] = word2;
    context->packet_words[3] = word3;
    context->packet_words[4] = word4;
    return packet;
}

extern "C" uint32 sub_80082B14(uint32 geometry, xport_draft_polygon_strip_context *context)
{
    FUNCTION_MARKER(0x80082B14, "SLUS_003.73");
    return EmitTexturedPolygonStrip(geometry, context);
}

uint32 EmitColoredPolygonStrip(uint32 geometry, xport_draft_polygon_strip_context *context)
{
    uint32 packet = context->packet_cursor, clipping = context->clipping_mask;
    uint32 remaining = context->remaining_segments, next_color0, next_point0, next_color1, next_point1, combined, visible;
    context->disposition = 0;
    FUNCTION_MARKER(0x80082930, "SLUS_003.73");
    do
    {
        w_u32(packet + 4, context->packet_words[0]);
        if (packet >= context->packet_limit)
        {
            context->packet_cursor = packet;
            context->clipping_mask = clipping;
            context->remaining_segments = remaining;
            context->disposition = 1;
            return packet;
        }
        w_u32(packet + 8, context->packet_words[1]);
        w_u32(packet + 12, context->packet_words[3]);
        w_u32(packet + 16, context->packet_words[4]);
        next_color0 = r_u32(geometry + 28);
        next_point0 = r_u32(geometry + 16);
        next_color1 = r_u32(geometry + 156);
        next_point1 = r_u32(geometry + 144);
        geometry += 16;
        combined = next_point0 & next_point1;
        visible = !(clipping & combined);
        clipping = combined & context->frustum_mask;
        if (visible)
        {
            packet += 36;
            w_u32(packet - 36, packet + 0x08000000);
            w_u32(packet - 16, next_color0);
            w_u32(packet - 12, next_point0);
            w_u32(packet - 8, next_color1);
            w_u32(packet - 4, next_point1);
            context->packet_stride = 36;
        }
        context->packet_words[0] = next_color0;
        context->packet_words[1] = next_point0;
        context->packet_words[3] = next_color1;
        context->packet_words[4] = next_point1;
    } while (remaining-- != 0);
    context->packet_cursor = packet;
    context->clipping_mask = clipping;
    context->remaining_segments = remaining;
    return packet;
}

uint32 SubdivideColoredPolygon(uint32 packet, uint32 limit, uint32 scratch, const uint32 points[4], uint32 area, uint32 quad, uint32 *last_stride)
{
    uint32 parts, step, geometry[3], work = scratch, row, command, before, first_color, next_point, next_color;
    uint32 corner0, corner1, corner2, corner3;
    xport_draft_polygon_strip_context strip;
    geometry[0] = points[0]; geometry[1] = points[1]; geometry[2] = points[2];
    packet = SelectSubdivisionResolution(packet, area, &step, &parts);
    InterpolateSubdivisionGrid(packet, 8, 0x80000000, geometry, parts, step);
    if (quad)
    {
        geometry[0] = points[3]; geometry[1] = points[2]; geometry[2] = points[1];
        InterpolateReverseSubdivisionGrid(packet + 24, (uint32)-8, 0x80000000, geometry, parts, step, scratch + (parts + 1) * 128);
        corner0 = r_u32(packet + 8);
        corner1 = r_u32(packet + 16);
        corner2 = r_u32(packet + 24);
        corner3 = r_u32(packet + 32);
        PrepareSubdivisionEdges(parts, corner0, corner1, corner2, corner3);
    }
    else
    {
        corner0 = r_u32(packet + 8);
        corner1 = r_u32(packet + 16);
        corner2 = r_u32(packet + 24);
        PrepareTriangleSubdivisionEdges(parts, corner0, corner1, corner2);
    }
    command = r_u32(packet + 4) & 0xFF000000;
    if (!quad)
        command ^= 0x08000000;
    strip.packet_limit = limit;
    strip.frustum_mask = 0xE0006000;
    strip.packet_stride = quad ? 36 : 28;
    strip.disposition = 0;
    row = parts - 1;
    do
    {
        first_color = r_u32(work + 12);
        if (packet >= limit)
            break;
        strip.packet_cursor = packet;
        strip.packet_words[0] = first_color;
        strip.packet_words[1] = r_u32(work);
        strip.packet_words[3] = r_u32(work + 140);
        strip.packet_words[4] = r_u32(work + 128);
        strip.clipping_mask = strip.packet_words[1] & strip.packet_words[4] & strip.frustum_mask;
        before = packet;
        if (quad || (sint32)(row - 1) >= 0)
        {
            strip.remaining_segments = quad ? parts - 1 : row - 1;
            packet = EmitColoredPolygonStrip(work, &strip);
            work += (quad ? parts : row) * 16;
            if (packet != before)
                *last_stride = strip.packet_stride;
            if (strip.disposition)
                break;
        }
        if (!quad)
        {
            w_u32(packet + 4, (strip.packet_words[0] | command) ^ 0x08000000);
            w_u32(packet + 8, strip.packet_words[1]);
            w_u32(packet + 12, strip.packet_words[3]);
            next_point = r_u32(work + 16);
            next_color = r_u32(work + 28);
            w_u32(packet + 16, strip.packet_words[4]);
            if (!(strip.clipping_mask & next_point))
            {
                packet += 28;
                w_u32(packet - 28, packet + 0x06000000);
                w_u32(packet - 8, next_color);
                w_u32(packet - 4, next_point);
                *last_stride = 28;
                strip.packet_stride = 28;
            }
        }
        work = (work | 127) + 1;
    } while (row-- != 0);
    xport_gte_write_data(6, 0);
    return packet;
}

uint32 SubdivideFlatTriangle(uint32 packet, uint32 limit, uint32 scratch, const uint32 points[4], uint32 area, uint32 textured, uint32 *last_stride)
{
    uint32 parts, step, geometry[3], work = scratch, row, command, before, next_point, next_uv;
    uint32 corner0, corner1, corner2, corner3;
    xport_draft_polygon_strip_context strip;
    geometry[0] = points[0]; geometry[1] = points[1]; geometry[2] = points[2];
    packet = SelectSubdivisionResolution(packet, area, &step, &parts);
    InterpolateSubdivisionGrid(packet, textured ? 8 : scratch, textured ? 0x40000000 : 0, geometry, parts, step);
    corner0 = r_u32(packet + 8);
    corner1 = r_u32(packet + (textured ? 16 : 12));
    corner2 = r_u32(packet + (textured ? 24 : 16));
    PrepareTriangleSubdivisionEdges(parts, corner0, corner1, corner2);
    strip.packet_limit = limit;
    strip.frustum_mask = 0xE0006000;
    strip.packet_stride = textured ? 32 : 20;
    strip.disposition = 0;
    if (textured)
    {
        strip.texture_high0 = (uint32)(sint32)(sint16)r_u16(packet + 14) << 16;
        strip.texture_high1 = (uint32)(sint32)(sint16)r_u16(packet + 22) << 16;
    }
    command = r_u32(packet + 4);
    row = parts - 1;
    do
    {
        next_point = r_u32(work);
        if (packet >= limit)
            break;
        strip.packet_cursor = packet;
        before = packet;
        if (textured)
        {
            strip.packet_words[0] = command ^ 0x08000000;
            strip.packet_words[1] = next_point;
            strip.packet_words[2] = r_u16(work + 8);
            strip.packet_words[3] = r_u32(work + 128);
            strip.packet_words[4] = r_u16(work + 136);
            strip.clipping_mask = strip.packet_words[1] & strip.packet_words[3] & strip.frustum_mask;
        }
        else
        {
            strip.packet_words[1] = command ^ 0x08000000;
            strip.packet_words[2] = next_point;
            strip.packet_words[3] = r_u32(work + 128);
            strip.clipping_mask = strip.packet_words[2] & strip.packet_words[3] & strip.frustum_mask;
        }
        if ((sint32)(row - 1) >= 0)
        {
            strip.remaining_segments = row - 1;
            packet = textured ? EmitTexturedPolygonStrip(work, &strip) : EmitFlatPolygonStrip(work, &strip);
            work += row * 16;
            if (packet != before)
                *last_stride = strip.packet_stride;
            if (strip.disposition)
                break;
        }
        if (textured)
        {
            w_u32(packet + 4, command);
            w_u32(packet + 8, strip.packet_words[1]);
            w_u32(packet + 12, strip.packet_words[2] | strip.texture_high0);
            w_u32(packet + 16, strip.packet_words[3]);
            next_point = r_u32(work + 16);
            next_uv = r_u16(work + 24);
            w_u32(packet + 20, strip.packet_words[4] | strip.texture_high1);
            if (!(strip.clipping_mask & next_point))
            {
                packet += 32;
                w_u32(packet - 32, packet + 0x07000000);
                w_u32(packet - 8, next_point);
                w_u32(packet - 4, next_uv);
                *last_stride = 32;
                strip.packet_stride = 32;
            }
        }
        else
        {
            w_u32(packet + 8, strip.packet_words[2]);
            next_point = r_u32(work + 16);
            w_u32(packet + 12, strip.packet_words[3]);
            w_u32(packet + 4, command);
            if (!(strip.clipping_mask & next_point))
            {
                packet += 20;
                w_u32(packet - 20, packet + 0x04000000);
                w_u32(packet - 4, next_point);
                *last_stride = 20;
                strip.packet_stride = 20;
            }
        }
        work = (work | 127) + 1;
    } while (row-- != 0);
    xport_gte_write_data(6, 0);
    return packet;
}

extern "C" uint32 sub_80082930(uint32 geometry, xport_draft_polygon_strip_context *context)
{
    FUNCTION_MARKER(0x80082930, "SLUS_003.73");
    return EmitColoredPolygonStrip(geometry, context);
}
extern "C" uint32 apocalypse_model_subdivide_colored_native(uint32 packet, uint32 limit, uint32 scratch, const uint32 points[4], uint32 area, uint32 quad, uint32 *last_stride) { return SubdivideColoredPolygon(packet, limit, scratch, points, area, quad, last_stride); }
extern "C" uint32 apocalypse_model_subdivide_flat_triangle_native(uint32 packet, uint32 limit, uint32 scratch, const uint32 points[4], uint32 area, uint32 textured, uint32 *last_stride) { return SubdivideFlatTriangle(packet, limit, scratch, points, area, textured, last_stride); }


uint32 EmitColoredTexturedPolygonStrip(uint32 geometry, xport_draft_polygon_strip_context *context)
{
    uint32 packet = context->packet_cursor;
    uint32 words[6];
    uint32 clip = context->clipping_mask;
    uint32 remaining = context->remaining_segments;
    uint32 old_remaining, next_clip;
    unsigned index;
    context->disposition = 0;
    FUNCTION_MARKER(0x80082D1C, "SLUS_003.73");
    for (index = 0; index < 6; ++index)
        words[index] = context->packet_words[index];
    do
    {
        words[2] |= context->texture_high0;
        if (packet >= context->packet_limit)
        {
            context->packet_cursor = packet;
            context->clipping_mask = clip;
            context->remaining_segments = remaining;
            for (index = 0; index < 6; ++index)
                context->packet_words[index] = words[index];
            xport_gte_write_data(6, 0);
            context->disposition = 1;
            return packet;
        }
        for (index = 0; index < 5; ++index)
            w_u32(packet + 4 + index * 4, words[index]);
        w_u32(packet + 24, words[5] | context->texture_high1);
        words[0] = r_u32(geometry + 28);
        words[1] = r_u32(geometry + 16);
        words[2] = r_u16(geometry + 24);
        words[3] = r_u32(geometry + 156);
        words[4] = r_u32(geometry + 144);
        words[5] = r_u16(geometry + 152);
        geometry += 16;
        next_clip = words[1] & words[4];
        old_remaining = clip & next_clip;
        clip = next_clip & context->frustum_mask;
        if (!old_remaining)
        {
            packet += 52;
            context->packet_stride = 52;
            w_u32(packet - 52, packet + 0x0C000000);
            for (index = 0; index < 6; ++index)
                w_u32(packet - 24 + index * 4, words[index]);
        }
        old_remaining = remaining;
        --remaining;
    } while (old_remaining != 0);
    context->packet_cursor = packet;
    context->clipping_mask = clip;
    context->remaining_segments = remaining;
    for (index = 0; index < 6; ++index)
        context->packet_words[index] = words[index];
    return packet;
}

uint32 SubdivideColoredTexturedPolygon(uint32 packet, uint32 limit, uint32 scratch, const uint32 points[4], uint32 area, uint32 quad, uint32 *last_stride)
{
    uint32 parts, step, work = scratch, row, before, command;
    uint32 first_color, next_color, next_point, next_uv;
    uint32 geometry[3] = {points[0], points[1], points[2]};
    uint32 corner0, corner1, corner2, corner3;
    xport_draft_polygon_strip_context strip;
    SelectSubdivisionResolution(packet, area, &step, &parts);
    InterpolateSubdivisionGrid(packet, 12, 0xC0000000, geometry, parts, step);
    if (quad)
    {
        geometry[0] = points[3];
        geometry[1] = points[2];
        geometry[2] = points[1];
        InterpolateReverseSubdivisionGrid(packet + 36, 0xFFFFFFF4, 0xC0000000, geometry, parts, step, scratch + (parts + 1) * 128);
        corner0 = r_u32(packet + 8);
        corner1 = r_u32(packet + 20);
        corner2 = r_u32(packet + 32);
        corner3 = r_u32(packet + 44);
        PrepareSubdivisionEdges(parts, corner0, corner1, corner2, corner3);
    }
    else
    {
        corner0 = r_u32(packet + 8);
        corner1 = r_u32(packet + 20);
        corner2 = r_u32(packet + 32);
        PrepareTriangleSubdivisionEdges(parts, corner0, corner1, corner2);
    }
    command = (r_u32(packet + 4) & 0xFF000000) ^ 0x08000000;
    strip.texture_high0 = (uint32)(sint32)(sint16)r_u16(packet + 14) << 16;
    strip.texture_high1 = (uint32)(sint32)(sint16)r_u16(packet + 26) << 16;
    strip.packet_limit = limit;
    strip.frustum_mask = 0xE0006000;
    /* The original constructed packet supplies the initial native stride */
    strip.packet_stride = quad ? 52 : 40;
    strip.disposition = 0;
    row = parts - 1;
    do
    {
        first_color = r_u32(work + 12);
        if (packet >= limit)
            break;
        strip.packet_cursor = packet;
        strip.packet_words[0] = first_color;
        strip.packet_words[1] = r_u32(work);
        strip.packet_words[2] = r_u16(work + 8);
        strip.packet_words[3] = r_u32(work + 140);
        strip.packet_words[4] = r_u32(work + 128);
        strip.packet_words[5] = r_u16(work + 136);
        strip.clipping_mask = strip.packet_words[1] & strip.packet_words[4] & strip.frustum_mask;
        before = packet;
        if (quad || (sint32)(row - 1) >= 0)
        {
            strip.remaining_segments = quad ? parts - 1 : row - 1;
            packet = EmitColoredTexturedPolygonStrip(work, &strip);
            work += (quad ? parts : row) * 16;
            if (packet != before)
                *last_stride = strip.packet_stride;
            if (strip.disposition == 1)
                break;
        }
        if (!quad)
        {
            w_u32(packet + 4, (strip.packet_words[0] | command) ^ 0x08000000);
            w_u32(packet + 8, strip.packet_words[1]);
            w_u32(packet + 12, strip.packet_words[2] | strip.texture_high0);
            w_u32(packet + 16, strip.packet_words[3]);
            w_u32(packet + 20, strip.packet_words[4]);
            next_color = r_u32(work + 28);
            next_point = r_u32(work + 16);
            next_uv = r_u16(work + 24);
            w_u32(packet + 24, strip.packet_words[5] | strip.texture_high1);
            if (!(strip.clipping_mask & next_point))
            {
                packet += 40;
                w_u32(packet - 40, packet + 0x09000000);
                w_u32(packet - 12, next_color);
                w_u32(packet - 8, next_point);
                w_u32(packet - 4, next_uv);
                *last_stride = 40;
                strip.packet_stride = 40;
            }
        }
        work = (work | 127) + 1;
    } while (row-- != 0);
    /* The common continuation restores semantic caller state through native locals */
    xport_gte_write_data(6, 0);
    return packet;
}

extern "C" uint32 sub_80082D1C(uint32 geometry, xport_draft_polygon_strip_context *context)
{
    FUNCTION_MARKER(0x80082D1C, "SLUS_003.73");
    return EmitColoredTexturedPolygonStrip(geometry, context);
}
extern "C" uint32 apocalypse_model_subdivide_textured_native(uint32 packet, uint32 limit, uint32 scratch, const uint32 points[4], uint32 area, uint32 quad, uint32 *last_stride) { return SubdivideColoredTexturedPolygon(packet, limit, scratch, points, area, quad, last_stride); }


uint32 SubdivideFlatTexturedQuad(uint32 packet, uint32 limit, uint32 scratch, const uint32 points[4], uint32 area, uint32 *last_stride)
{
    uint32 parts, step, row, work = scratch, command, before, first_point;
    uint32 geometry[3] = {points[0], points[1], points[2]};
    uint32 corner0, corner1, corner2, corner3;
    xport_draft_polygon_strip_context strip;
    packet = SelectSubdivisionResolution(packet, area, &step, &parts);
    InterpolateSubdivisionGrid(packet, 8, 0x40000000, geometry, parts, step);
    geometry[0] = points[3];
    geometry[1] = points[2];
    geometry[2] = points[1];
    InterpolateReverseSubdivisionGrid(packet + 24, 0xFFFFFFF8, 0x40000000, geometry, parts, step, scratch + (parts + 1) * 128);
    corner0 = r_u32(packet + 8);
    corner1 = r_u32(packet + 16);
    corner2 = r_u32(packet + 24);
    corner3 = r_u32(packet + 32);
    PrepareSubdivisionEdges(parts, corner0, corner1, corner2, corner3);
    strip.texture_high0 = (uint32)(sint32)(sint16)r_u16(packet + 14) << 16;
    strip.texture_high1 = (uint32)(sint32)(sint16)r_u16(packet + 22) << 16;
    command = r_u32(packet + 4);
    strip.packet_limit = limit;
    strip.frustum_mask = 0xE0006000;
    strip.packet_stride = 40;
    strip.disposition = 0;
    row = parts - 1;
    do
    {
        first_point = r_u32(work);
        if (packet >= limit)
            break;
        strip.packet_cursor = packet;
        strip.packet_words[0] = command;
        strip.packet_words[1] = first_point;
        strip.packet_words[2] = r_u16(work + 8);
        strip.packet_words[3] = r_u32(work + 128);
        strip.packet_words[4] = r_u16(work + 136);
        strip.clipping_mask = strip.packet_words[1] & strip.packet_words[3] & strip.frustum_mask;
        strip.remaining_segments = parts - 1;
        before = packet;
        packet = EmitTexturedPolygonStrip(work, &strip);
        if (packet != before)
            *last_stride = strip.packet_stride;
        if (strip.disposition == 1)
            break;
        work = (work | 127) + 1;
    } while (row-- != 0);
    /* The common continuation retains caller state through native locals */
    xport_gte_write_data(6, 0);
    return packet;
}

uint32 SubdivideFlatQuad(uint32 packet, uint32 limit, uint32 scratch, const uint32 points[4], uint32 area, uint32 *last_stride)
{
    uint32 parts, step, row, work = scratch, command, before, first_point;
    uint32 corner0, corner1, corner2, corner3;
    uint32 geometry[3] = {points[0], points[1], points[2]};
    xport_draft_polygon_strip_context strip;
    packet = SelectSubdivisionResolution(packet, area, &step, &parts);
    InterpolateSubdivisionGrid(packet, scratch, 0, geometry, parts, step);
    geometry[0] = points[3];
    geometry[1] = points[2];
    geometry[2] = points[1];
    InterpolateReverseSubdivisionGrid(packet, scratch, 0, geometry, parts, step, scratch + (parts + 1) * 128);
    corner0 = r_u32(packet + 8);
    corner1 = r_u32(packet + 12);
    corner2 = r_u32(packet + 16);
    corner3 = r_u32(packet + 20);
    PrepareSubdivisionEdges(parts, corner0, corner1, corner2, corner3);
    command = r_u32(packet + 4);
    strip.packet_limit = limit;
    strip.frustum_mask = 0xE0006000;
    strip.packet_stride = 24;
    strip.disposition = 0;
    row = parts - 1;
    do
    {
        first_point = r_u32(work);
        if (packet >= limit)
            break;
        strip.packet_cursor = packet;
        strip.packet_words[1] = command;
        strip.packet_words[2] = first_point;
        strip.packet_words[3] = r_u32(work + 128);
        // The original retains corner2 in t2 and the reverse row stride in t5
        strip.clipping_mask = corner2 & 0xE0006000;
        strip.remaining_segments = parts - 1;
        before = packet;
        packet = EmitFlatPolygonStrip(work, &strip);
        if (packet != before)
            *last_stride = strip.packet_stride;
        if (strip.disposition == 1)
            break;
        work = (work | 127) + 1;
    } while (row-- != 0);
    /* The common continuation retains caller state through native locals */
    xport_gte_write_data(6, 0);
    return packet;
}

extern "C" uint32 apocalypse_model_subdivide_flat_textured_quad_native(uint32 packet, uint32 limit, uint32 scratch, const uint32 points[4], uint32 area, uint32 *last_stride) { return SubdivideFlatTexturedQuad(packet, limit, scratch, points, area, last_stride); }
extern "C" uint32 apocalypse_model_subdivide_flat_quad_native(uint32 packet, uint32 limit, uint32 scratch, const uint32 points[4], uint32 area, uint32 *last_stride) { return SubdivideFlatQuad(packet, limit, scratch, points, area, last_stride); }
