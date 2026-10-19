#include <stdlib.h>

#include "geometry_commands.h"
#include "model_projection.h"
#include "model_subdivision.h"
#include "game_diagnostics.h"

struct SubdivisionPoint
{
    uint32 screen;
    uint32 uv;
    uint32 color;
};

class SubdivisionGrid
{
public:
    SubdivisionPoint &At(sint32 index)
    {
        if (index < 0 || index >= 64)
        {
            GameDiagnostic("Subdivision grid index exceeds its native owner: %d\n", index);
            abort();
        }
        return points[index];
    }

    const SubdivisionPoint &At(sint32 index) const
    {
        if (index < 0 || index >= 64)
        {
            GameDiagnostic("Subdivision grid index exceeds its native owner: %d\n", index);
            abort();
        }
        return points[index];
    }

private:
    SubdivisionPoint points[64];
};

struct PolygonStripContext
{
    uint8 *packet_cursor;
    uint8 *packet_limit;
    uint32 packet_words[6];
    uint32 texture_high0;
    uint32 texture_high1;
    uint32 clipping_mask;
    uint32 frustum_mask;
    uint32 remaining_segments;
    uint32 packet_stride;
    uint32 disposition;
};

static uint8 *SelectSubdivisionResolution(uint8 *packet, uint32 area, uint32 *step, uint32 *subdivisions)
{
    uint32 command, leading_count, count;
    FUNCTION_MARKER(0x80082394, "SLUS_003.73");
    // Caller state is held by native locals
    xport_gte_write_data(30, area >> 11);
    command = ((P_TAG *)packet)->rgbc.rgbc;
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

static void LoadSubdivisionGeometry(const ModelProjectedVertex &input0, const ModelProjectedVertex &input1, const ModelProjectedVertex &input2)
{
    uint32 row0 = input0.xy;
    uint32 tail0 = (input0.depth_flags & 65535);
    uint32 row1 = input1.xy;
    uint32 tail1 = (input1.depth_flags & 65535);
    uint32 row2 = input2.xy;
    uint32 tail2 = (input2.depth_flags & 65535);
    FUNCTION_MARKER(0x80082508, "SLUS_003.73");
    xport_gte_write_control(0, (row0 & 0xFFFF) | (row1 << 16));
    xport_gte_write_control(1, ((row2 ^ row0) & 0xFFFF) ^ row0);
    xport_gte_write_control(2, (row1 >> 16) | (row2 & 0xFFFF0000));
    xport_gte_write_control(3, tail0 | (tail1 << 16));
    xport_gte_write_control(4, tail2);
}

static void InterpolateSubdivisionGrid(SubdivisionGrid &grid, const SubdivisionPoint *vertices, sint32 stride, uint32 flags, const ModelProjectedVertex *const geometry[3], uint32 subdivisions, uint32 step)
{
    const uint32 clip_bias = 0x00F00200, clip_mask = 0xBFFFBFFF;
    uint32 color0, color1, color2, row_remaining, inner_remaining;
    uint32 vertical = 0, remaining, horizontal, packed;
    sint32 row_start, row_end = 0, point;
    uint32 screen, flag, color;
    FUNCTION_MARKER(0x80082638, "SLUS_003.73");
    if (flags & 0x40000000)
    {
        color0 = vertices[0].uv;
        color1 = vertices[stride].uv;
        color2 = vertices[2 * stride].uv;
        xport_gte_write_control(8, ((color0 & 255) << 4) | ((color1 & 255) << 20));
        xport_gte_write_control(9, ((color2 & 255) << 4) | ((color0 & 0xFF00) << 12));
        xport_gte_write_control(10, ((color1 & 0xFF00) >> 4) | ((color2 & 0xFF00) << 12));
        xport_gte_write_control(11, 0);
        xport_gte_write_control(12, 0);
    }
    if ((sint32)flags < 0)
    {
        color0 = vertices[0].color;
        color1 = vertices[stride].color;
        color2 = vertices[2 * stride].color;
        xport_gte_write_control(16, ((color0 & 255) << 4) | ((color1 & 255) << 20));
        xport_gte_write_control(17, ((color2 & 255) << 4) | ((color0 & 0xFF00) << 12));
        xport_gte_write_control(18, ((color1 & 0xFF00) >> 4) | ((color2 & 0xFF00) << 12));
        xport_gte_write_control(19, ((color0 >> 12) & 0xFF0) | (((color1 >> 12) & 0xFF0) << 16));
        xport_gte_write_control(20, (color2 >> 12) & 0xFF0);
    }
    LoadSubdivisionGeometry(*geometry[0], *geometry[1], *geometry[2]);
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
        point = row_start - 1;
        inner_remaining = row_remaining;
        do
        {
            xport_gte_execute(0x180001);
            ++point;
            remaining -= step;
            horizontal += step;
            packed = (horizontal << 16) | remaining;
            if ((sint32)(horizontal - 3840) >= 0)
                packed = 0x10000000;
            screen = xport_gte_read_data(14);
            flag = xport_gte_read_flag();
            xport_gte_execute(0x4A6412);
            screen = ((screen & clip_mask) | ~((screen - clip_bias) | clip_mask)) & 0x7FFFFFFF;
            ExecuteGeometryCommand(0x198003D);
            grid.At(point).screen = screen | (flag & 0x80000000);
            color = xport_gte_read_data(22);
            grid.At(point).uv = color;
            if ((sint32)flags < 0)
            {
                xport_gte_execute(0x4C6412);
                ExecuteGeometryCommand(0x198003D);
                grid.At(point).color = xport_gte_read_data(22);
            }
            xport_gte_write_data(0, packed);
        } while (inner_remaining-- != 0);
        vertical += step;
        row_end = row_start + 8;
    } while (row_remaining-- != 0);
    /* The MIPS continuation returns to the caller with its packet cursor unchanged */
}

static void InterpolateReverseSubdivisionGrid(SubdivisionGrid &grid, const SubdivisionPoint *attributes, sint32 stride, uint32 attribute_flags, const ModelProjectedVertex *const geometry[3], uint32 subdivisions, uint32 step, sint32 destination)
{
    uint32 color0, color1, color2, row_count = subdivisions - 1;
    sint32 row_end = destination + (sint32)subdivisions - 8;
    sint32 row_start, point;
    uint32 vertical = 0, horizontal, remaining, inner_count;
    uint32 packed, screen, flag;
    FUNCTION_MARKER(0x8008267C, "SLUS_003.73");
    /* Generate the opposite triangle in the shared interpolation grid */
    if (attribute_flags & 0x40000000)
    {
        color0 = attributes[0].uv;
        color1 = attributes[stride].uv;
        color2 = attributes[2 * stride].uv;
        xport_gte_write_control(8, ((color0 & 255) << 4) | ((color1 & 255) << 20));
        xport_gte_write_control(9, ((color2 & 255) << 4) | ((color0 & 0xFF00) << 12));
        xport_gte_write_control(10, ((color1 & 0xFF00) >> 4) | ((color2 & 0xFF00) << 12));
        xport_gte_write_control(11, 0);
        xport_gte_write_control(12, 0);
    }
    if ((sint32)attribute_flags < 0)
    {
        color0 = attributes[0].color;
        color1 = attributes[stride].color;
        color2 = attributes[2 * stride].color;
        xport_gte_write_control(16, ((color0 & 255) << 4) | ((color1 & 255) << 20));
        xport_gte_write_control(17, ((color2 & 255) << 4) | ((color0 & 0xFF00) << 12));
        xport_gte_write_control(18, ((color1 & 0xFF00) >> 4) | ((color2 & 0xFF00) << 12));
        xport_gte_write_control(19, ((color0 >> 12) & 0xFF0) | (((color1 >> 12) & 0xFF0) << 16));
        xport_gte_write_control(20, (color2 >> 12) & 0xFF0);
    }
    LoadSubdivisionGeometry(*geometry[0], *geometry[1], *geometry[2]);
    do
    {
        row_start = row_end;
        if ((sint32)(vertical - 3840) >= 0)
            vertical = 4096;
        remaining = 4096 - vertical;
        horizontal = 0;
        xport_gte_write_data(0, remaining);
        xport_gte_write_data(1, vertical);
        point = row_start + 1;
        inner_count = row_count;
        do
        {
            xport_gte_execute(0x180001);
            --point;
            remaining -= step;
            horizontal += step;
            packed = (horizontal << 16) | remaining;
            if ((sint32)(horizontal - 3840) >= 0)
                packed = 0x10000000;
            screen = xport_gte_read_data(14);
            flag = xport_gte_read_flag();
            xport_gte_execute(0x4A6412);
            screen = ((screen & 0xBFFFBFFF) | ~((screen - 0x00F00200) | 0xBFFFBFFF)) & 0x7FFFFFFF;
            ExecuteGeometryCommand(0x198003D);
            grid.At(point).screen = screen | (flag & 0x80000000);
            grid.At(point).uv = xport_gte_read_data(22);
            if ((sint32)attribute_flags < 0)
            {
                xport_gte_execute(0x4C6412);
                ExecuteGeometryCommand(0x198003D);
                grid.At(point).color = xport_gte_read_data(22);
            }
            xport_gte_write_data(0, packed);
        } while (inner_count-- != 0);
        vertical += step;
        row_end = row_start - 8;
    } while (row_count-- != 0);
    /* The packet cursor is unchanged by this interpolation helper */
}

static sint32 InterpolateSubdivisionEdge(SubdivisionGrid &grid, sint32 position, uint32 count, sint32 stride, uint32 previous, uint32 next)
{
    uint32 difference = next - previous;
    uint32 delta = 16 * ((difference & 0x2000) - 0x1000) - (((uint32)((sint32)difference >> 28) & 2) - 1);
    sint32 cursor = position + stride;
    uint32 remaining = count - 1;
    FUNCTION_MARKER(0x800826C4, "SLUS_003.73");
    // Current polygon owners select a count from two through seven
    do
    {
        uint32 point = grid.At(cursor).screen;
        --remaining;
        grid.At(cursor).screen = point + delta;
        cursor += stride;
    } while (remaining != 0);
    return cursor;
}

static void PrepareTriangleSubdivisionEdges(SubdivisionGrid &grid, uint32 count, uint32 corner0, uint32 corner1, uint32 corner2)
{
    sint32 position = 0;
    FUNCTION_MARKER(0x8008270C, "SLUS_003.73");
    // Polygon owners carry the clipping mask separately in their strip context
    position = InterpolateSubdivisionEdge(grid, position, count, 1, corner0, corner1);
    position = InterpolateSubdivisionEdge(grid, position, count, 7, corner1, corner2);
    InterpolateSubdivisionEdge(grid, position, count, -8, corner2, corner0);
}

static void PrepareSubdivisionEdges(SubdivisionGrid &grid, uint32 count, uint32 corner0, uint32 corner1, uint32 corner2, uint32 corner3)
{
    sint32 position = 0;
    FUNCTION_MARKER(0x80082750, "SLUS_003.73");
    // Polygon owners carry the clipping mask separately in their strip context
    position = InterpolateSubdivisionEdge(grid, position, count, 1, corner0, corner1);
    position = InterpolateSubdivisionEdge(grid, position, count, 8, corner1, corner3);
    position = InterpolateSubdivisionEdge(grid, position, count, -1, corner3, corner2);
    InterpolateSubdivisionEdge(grid, position, count, -8, corner2, corner0);
}

static uint8 *EmitFlatPolygonStrip(const SubdivisionGrid &grid, sint32 geometry, PolygonStripContext *context)
{
    uint8 *packet = context->packet_cursor;
    uint32 previous = context->clipping_mask;
    uint32 remaining = context->remaining_segments;
    uint32 first, second, combined;
    context->disposition = 0;
    FUNCTION_MARKER(0x800827A4, "SLUS_003.73");
    do
    {
        POLY_F4 *poly = (POLY_F4 *)packet;
        poly->rgbc.rgbc = context->packet_words[1];
        if (packet >= context->packet_limit)
        {
            context->packet_cursor = packet;
            context->clipping_mask = previous;
            context->remaining_segments = remaining;
            context->disposition = 1;
            return packet;
        }
        poly->p[0].xy = context->packet_words[2];
        poly->p[1].xy = context->packet_words[3];
        first = grid.At(geometry + 1).screen;
        second = grid.At(geometry + 9).screen;
        ++geometry;
        combined = first & second;
        if (!(previous & combined))
        {
            packet += 24;
            poly->tag = gpu_packet_link((uintptr_t)packet) | 0x05000000;
            poly->p[2].xy = first;
            poly->p[3].xy = second;
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

static uint8 *EmitColoredPolygonStrip(const SubdivisionGrid &grid, sint32 geometry, PolygonStripContext *context)
{
    uint8 *packet = context->packet_cursor;
    uint32 clipping = context->clipping_mask;
    uint32 remaining = context->remaining_segments, next_color0, next_point0, next_color1, next_point1, combined, visible;
    context->disposition = 0;
    FUNCTION_MARKER(0x80082930, "SLUS_003.73");
    do
    {
        POLY_G4 *poly = (POLY_G4 *)packet;
        poly->p[0].rgbc.rgbc = context->packet_words[0];
        if (packet >= context->packet_limit)
        {
            context->packet_cursor = packet;
            context->clipping_mask = clipping;
            context->remaining_segments = remaining;
            context->disposition = 1;
            return packet;
        }
        poly->p[0].xy.xy = context->packet_words[1];
        poly->p[1].rgbc.rgbc = context->packet_words[3];
        poly->p[1].xy.xy = context->packet_words[4];
        next_color0 = grid.At(geometry + 1).color;
        next_point0 = grid.At(geometry + 1).screen;
        next_color1 = grid.At(geometry + 9).color;
        next_point1 = grid.At(geometry + 9).screen;
        ++geometry;
        combined = next_point0 & next_point1;
        visible = !(clipping & combined);
        clipping = combined & context->frustum_mask;
        if (visible)
        {
            packet += 36;
            poly->tag = gpu_packet_link((uintptr_t)packet) | 0x08000000;
            poly->p[2].rgbc.rgbc = next_color0;
            poly->p[2].xy.xy = next_point0;
            poly->p[3].rgbc.rgbc = next_color1;
            poly->p[3].xy.xy = next_point1;
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

static uint8 *EmitTexturedPolygonStrip(const SubdivisionGrid &grid, sint32 geometry, PolygonStripContext *context)
{
    uint8 *packet = context->packet_cursor;
    uint32 clipping = context->clipping_mask;
    uint32 remaining = context->remaining_segments;
    uint32 word0 = context->packet_words[0], word1 = context->packet_words[1];
    uint32 word2 = context->packet_words[2], word3 = context->packet_words[3];
    uint32 word4 = context->packet_words[4];
    uint32 repeat;
    context->disposition = 0;
    FUNCTION_MARKER(0x80082B14, "SLUS_003.73");
    do
    {
        POLY_FT4 *poly = (POLY_FT4 *)packet;
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
        poly->rgbc.rgbc = word0;
        poly->p[0].xy.xy = word1;
        poly->p[0].uv.uv = word2 | context->texture_high0;
        poly->p[0].attr = (word2 | context->texture_high0) >> 16;
        poly->p[1].xy.xy = word3;
        poly->p[1].uv.uv = word4 | context->texture_high1;
        poly->p[1].attr = (word4 | context->texture_high1) >> 16;
        next0 = grid.At(geometry + 1).screen;
        next1 = (uint32)(uint16)grid.At(geometry + 1).uv;
        next2 = grid.At(geometry + 9).screen;
        next3 = (uint32)(uint16)grid.At(geometry + 9).uv;
        ++geometry;
        mask = next0 & next2;
        repeat = clipping & mask;
        clipping = mask & context->frustum_mask;
        if (!repeat)
        {
            packet += 40;
            poly->tag = gpu_packet_link((uintptr_t)packet) | 0x09000000;
            poly->p[2].xy.xy = next0;
            poly->p[2].uv.uv = next1;
            poly->p[2].attr = next1 >> 16;
            poly->p[3].xy.xy = next2;
            poly->p[3].uv.uv = next3;
            poly->p[3].attr = next3 >> 16;
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

static uint8 *EmitColoredTexturedPolygonStrip(const SubdivisionGrid &grid, sint32 geometry, PolygonStripContext *context)
{
    uint8 *packet = context->packet_cursor;
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
        POLY_GT4 *poly = (POLY_GT4 *)packet;
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
        poly->p[0].rgbc.rgbc = words[0];
        poly->p[0].xy.xy = words[1];
        poly->p[0].uv.uv = words[2];
        poly->p[0].attr = words[2] >> 16;
        poly->p[1].rgbc.rgbc = words[3];
        poly->p[1].xy.xy = words[4];
        poly->p[1].uv.uv = words[5] | context->texture_high1;
        poly->p[1].attr = (words[5] | context->texture_high1) >> 16;
        words[0] = grid.At(geometry + 1).color;
        words[1] = grid.At(geometry + 1).screen;
        words[2] = (uint32)(uint16)grid.At(geometry + 1).uv;
        words[3] = grid.At(geometry + 9).color;
        words[4] = grid.At(geometry + 9).screen;
        words[5] = (uint32)(uint16)grid.At(geometry + 9).uv;
        ++geometry;
        next_clip = words[1] & words[4];
        old_remaining = clip & next_clip;
        clip = next_clip & context->frustum_mask;
        if (!old_remaining)
        {
            packet += 52;
            context->packet_stride = 52;
            poly->tag = gpu_packet_link((uintptr_t)packet) | 0x0C000000;
            for (index = 0; index < 2; ++index)
            {
                poly->p[index + 2].rgbc.rgbc = words[index * 3];
                poly->p[index + 2].xy.xy = words[index * 3 + 1];
                poly->p[index + 2].uv.uv = words[index * 3 + 2];
                poly->p[index + 2].attr = words[index * 3 + 2] >> 16;
            }
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

uint8 *SubdivideColoredPolygon(uint8 *packet, uint8 *limit, const ModelProjectedVertex *const points[4], uint32 area, uint32 quad, uint32 *last_stride)
{
    uint8 *before;
    SubdivisionGrid grid;
    sint32 work = 0;
    const ModelProjectedVertex *geometry[3];
    uint32 parts, step, row, command, first_color, next_point, next_color;
    uint32 corner0, corner1, corner2, corner3;
    PolygonStripContext strip;
    SubdivisionPoint attributes[4];
    const POINT_C *input = quad ? ((POLY_G4 *)packet)->p : ((POLY_G3 *)packet)->p;
    uint32 vertex_count = quad ? 4 : 3;
    for (uint32 i = 0; i < vertex_count; ++i)
    {
        attributes[i].screen = input[i].xy.xy;
        attributes[i].color = input[i].rgbc.rgbc;
    }
    geometry[0] = points[0];
    geometry[1] = points[1];
    geometry[2] = points[2];
    packet = SelectSubdivisionResolution(packet, area, &step, &parts);
    InterpolateSubdivisionGrid(grid, attributes, 1, 0x80000000, geometry, parts, step);
    if (quad)
    {
        geometry[0] = points[3];
        geometry[1] = points[2];
        geometry[2] = points[1];
        InterpolateReverseSubdivisionGrid(grid, attributes + 3, -1, 0x80000000, geometry, parts, step, (parts + 1) * 8);
        corner0 = attributes[0].screen;
        corner1 = attributes[1].screen;
        corner2 = attributes[2].screen;
        corner3 = attributes[3].screen;
        PrepareSubdivisionEdges(grid, parts, corner0, corner1, corner2, corner3);
    }
    else
    {
        corner0 = attributes[0].screen;
        corner1 = attributes[1].screen;
        corner2 = attributes[2].screen;
        PrepareTriangleSubdivisionEdges(grid, parts, corner0, corner1, corner2);
    }
    command = ((P_TAG *)packet)->rgbc.rgbc & 0xFF000000;
    if (!quad)
        command ^= 0x08000000;
    strip.packet_limit = limit;
    strip.frustum_mask = 0xE0006000;
    strip.packet_stride = quad ? 36 : 28;
    strip.disposition = 0;
    row = parts - 1;
    do
    {
        first_color = grid.At(work).color;
        if (packet >= limit)
            break;
        strip.packet_cursor = packet;
        strip.packet_words[0] = first_color;
        strip.packet_words[1] = grid.At(work).screen;
        strip.packet_words[3] = grid.At(work + 8).color;
        strip.packet_words[4] = grid.At(work + 8).screen;
        strip.clipping_mask = strip.packet_words[1] & strip.packet_words[4] & strip.frustum_mask;
        before = packet;
        if (quad || (sint32)(row - 1) >= 0)
        {
            strip.remaining_segments = quad ? parts - 1 : row - 1;
            packet = EmitColoredPolygonStrip(grid, work, &strip);
            work += quad ? parts : row;
            if (packet != before)
                *last_stride = strip.packet_stride;
            if (strip.disposition)
                break;
        }
        if (!quad)
        {
            POLY_G3 *poly = (POLY_G3 *)packet;
            poly->p[0].rgbc.rgbc = (strip.packet_words[0] | command) ^ 0x08000000;
            poly->p[0].xy.xy = strip.packet_words[1];
            poly->p[1].rgbc.rgbc = strip.packet_words[3];
            next_point = grid.At(work + 1).screen;
            next_color = grid.At(work + 1).color;
            poly->p[1].xy.xy = strip.packet_words[4];
            if (!(strip.clipping_mask & next_point))
            {
                packet += 28;
                poly->tag = gpu_packet_link((uintptr_t)packet) | 0x06000000;
                poly->p[2].rgbc.rgbc = next_color;
                poly->p[2].xy.xy = next_point;
                *last_stride = 28;
                strip.packet_stride = 28;
            }
        }
        work = (work | 7) + 1;
    } while (row-- != 0);
    xport_gte_write_data(6, 0);
    return packet;
}

uint8 *SubdivideFlatTriangle(uint8 *packet, uint8 *limit, const ModelProjectedVertex *const points[4], uint32 area, uint32 textured, uint32 *last_stride)
{
    uint8 *before;
    SubdivisionGrid grid;
    sint32 work = 0;
    const ModelProjectedVertex *geometry[3];
    uint32 parts, step, row, command, next_point, next_uv;
    uint32 corner0, corner1, corner2, corner3;
    PolygonStripContext strip;
    SubdivisionPoint attributes[4];
    const POINT_T *input = textured ? ((POLY_FT3 *)packet)->p : 0;
    for (uint32 i = 0; i < 3; ++i)
    {
        attributes[i].screen = textured ? input[i].xy.xy : ((POLY_F3 *)packet)->p[i].xy;
        if (textured)
            attributes[i].uv = input[i].uv.uv | ((uint32)input[i].attr << 16);
    }
    geometry[0] = points[0];
    geometry[1] = points[1];
    geometry[2] = points[2];
    packet = SelectSubdivisionResolution(packet, area, &step, &parts);
    InterpolateSubdivisionGrid(grid, attributes, textured ? 1 : 0, textured ? 0x40000000 : 0, geometry, parts, step);
    corner0 = attributes[0].screen;
    corner1 = attributes[1].screen;
    corner2 = attributes[2].screen;
    PrepareTriangleSubdivisionEdges(grid, parts, corner0, corner1, corner2);
    strip.packet_limit = limit;
    strip.frustum_mask = 0xE0006000;
    strip.packet_stride = textured ? 32 : 20;
    strip.disposition = 0;
    if (textured)
    {
        strip.texture_high0 = (uint32)(sint32)(sint16)input[0].attr << 16;
        strip.texture_high1 = (uint32)(sint32)(sint16)input[1].attr << 16;
    }
    command = ((P_TAG *)packet)->rgbc.rgbc;
    row = parts - 1;
    do
    {
        next_point = grid.At(work).screen;
        if (packet >= limit)
            break;
        strip.packet_cursor = packet;
        before = packet;
        if (textured)
        {
            strip.packet_words[0] = command ^ 0x08000000;
            strip.packet_words[1] = next_point;
            strip.packet_words[2] = (uint32)(uint16)grid.At(work).uv;
            strip.packet_words[3] = grid.At(work + 8).screen;
            strip.packet_words[4] = (uint32)(uint16)grid.At(work + 8).uv;
            strip.clipping_mask = strip.packet_words[1] & strip.packet_words[3] & strip.frustum_mask;
        }
        else
        {
            strip.packet_words[1] = command ^ 0x08000000;
            strip.packet_words[2] = next_point;
            strip.packet_words[3] = grid.At(work + 8).screen;
            strip.clipping_mask = strip.packet_words[2] & strip.packet_words[3] & strip.frustum_mask;
        }
        if ((sint32)(row - 1) >= 0)
        {
            strip.remaining_segments = row - 1;
            packet = textured ? EmitTexturedPolygonStrip(grid, work, &strip) : EmitFlatPolygonStrip(grid, work, &strip);
            work += row;
            if (packet != before)
                *last_stride = strip.packet_stride;
            if (strip.disposition)
                break;
        }
        if (textured)
        {
            POLY_FT3 *poly = (POLY_FT3 *)packet;
            poly->rgbc.rgbc = command;
            poly->p[0].xy.xy = strip.packet_words[1];
            poly->p[0].uv.uv = strip.packet_words[2] | strip.texture_high0;
            poly->p[0].attr = (strip.packet_words[2] | strip.texture_high0) >> 16;
            poly->p[1].xy.xy = strip.packet_words[3];
            next_point = grid.At(work + 1).screen;
            next_uv = (uint32)(uint16)grid.At(work + 1).uv;
            poly->p[1].uv.uv = strip.packet_words[4] | strip.texture_high1;
            poly->p[1].attr = (strip.packet_words[4] | strip.texture_high1) >> 16;
            if (!(strip.clipping_mask & next_point))
            {
                packet += 32;
                poly->tag = gpu_packet_link((uintptr_t)packet) | 0x07000000;
                poly->p[2].xy.xy = next_point;
                poly->p[2].uv.uv = next_uv;
                poly->p[2].attr = next_uv >> 16;
                *last_stride = 32;
                strip.packet_stride = 32;
            }
        }
        else
        {
            POLY_F3 *poly = (POLY_F3 *)packet;
            poly->p[0].xy = strip.packet_words[2];
            next_point = grid.At(work + 1).screen;
            poly->p[1].xy = strip.packet_words[3];
            poly->rgbc.rgbc = command;
            if (!(strip.clipping_mask & next_point))
            {
                packet += 20;
                poly->tag = gpu_packet_link((uintptr_t)packet) | 0x04000000;
                poly->p[2].xy = next_point;
                *last_stride = 20;
                strip.packet_stride = 20;
            }
        }
        work = (work | 7) + 1;
    } while (row-- != 0);
    xport_gte_write_data(6, 0);
    return packet;
}

uint8 *SubdivideColoredTexturedPolygon(uint8 *packet, uint8 *limit, const ModelProjectedVertex *const points[4], uint32 area, uint32 quad, uint32 *last_stride)
{
    uint8 *before;
    uint32 parts, step, row, command;
    uint32 first_color, next_color, next_point, next_uv;
    SubdivisionGrid grid;
    sint32 work = 0;
    const ModelProjectedVertex *geometry[3] = {points[0], points[1], points[2]};
    uint32 corner0, corner1, corner2, corner3;
    PolygonStripContext strip;
    SubdivisionPoint attributes[4];
    const POINT_CT *input = quad ? ((POLY_GT4 *)packet)->p : ((POLY_GT3 *)packet)->p;
    uint32 vertex_count = quad ? 4 : 3;
    for (uint32 i = 0; i < vertex_count; ++i)
    {
        attributes[i].screen = input[i].xy.xy;
        attributes[i].color = input[i].rgbc.rgbc;
        attributes[i].uv = input[i].uv.uv | ((uint32)input[i].attr << 16);
    }
    SelectSubdivisionResolution(packet, area, &step, &parts);
    InterpolateSubdivisionGrid(grid, attributes, 1, 0xC0000000, geometry, parts, step);
    if (quad)
    {
        geometry[0] = points[3];
        geometry[1] = points[2];
        geometry[2] = points[1];
        InterpolateReverseSubdivisionGrid(grid, attributes + 3, -1, 0xC0000000, geometry, parts, step, (parts + 1) * 8);
        corner0 = attributes[0].screen;
        corner1 = attributes[1].screen;
        corner2 = attributes[2].screen;
        corner3 = attributes[3].screen;
        PrepareSubdivisionEdges(grid, parts, corner0, corner1, corner2, corner3);
    }
    else
    {
        corner0 = attributes[0].screen;
        corner1 = attributes[1].screen;
        corner2 = attributes[2].screen;
        PrepareTriangleSubdivisionEdges(grid, parts, corner0, corner1, corner2);
    }
    command = (((P_TAG *)packet)->rgbc.rgbc & 0xFF000000) ^ 0x08000000;
    strip.texture_high0 = (uint32)(sint32)(sint16)input[0].attr << 16;
    strip.texture_high1 = (uint32)(sint32)(sint16)input[1].attr << 16;
    strip.packet_limit = limit;
    strip.frustum_mask = 0xE0006000;
    /* The original constructed packet supplies the initial native stride */
    strip.packet_stride = quad ? 52 : 40;
    strip.disposition = 0;
    row = parts - 1;
    do
    {
        first_color = grid.At(work).color;
        if (packet >= limit)
            break;
        strip.packet_cursor = packet;
        strip.packet_words[0] = first_color;
        strip.packet_words[1] = grid.At(work).screen;
        strip.packet_words[2] = (uint32)(uint16)grid.At(work).uv;
        strip.packet_words[3] = grid.At(work + 8).color;
        strip.packet_words[4] = grid.At(work + 8).screen;
        strip.packet_words[5] = (uint32)(uint16)grid.At(work + 8).uv;
        strip.clipping_mask = strip.packet_words[1] & strip.packet_words[4] & strip.frustum_mask;
        before = packet;
        if (quad || (sint32)(row - 1) >= 0)
        {
            strip.remaining_segments = quad ? parts - 1 : row - 1;
            packet = EmitColoredTexturedPolygonStrip(grid, work, &strip);
            work += quad ? parts : row;
            if (packet != before)
                *last_stride = strip.packet_stride;
            if (strip.disposition == 1)
                break;
        }
        if (!quad)
        {
            POLY_GT3 *poly = (POLY_GT3 *)packet;
            poly->p[0].rgbc.rgbc = (strip.packet_words[0] | command) ^ 0x08000000;
            poly->p[0].xy.xy = strip.packet_words[1];
            poly->p[0].uv.uv = strip.packet_words[2] | strip.texture_high0;
            poly->p[0].attr = (strip.packet_words[2] | strip.texture_high0) >> 16;
            poly->p[1].rgbc.rgbc = strip.packet_words[3];
            poly->p[1].xy.xy = strip.packet_words[4];
            next_color = grid.At(work + 1).color;
            next_point = grid.At(work + 1).screen;
            next_uv = (uint32)(uint16)grid.At(work + 1).uv;
            poly->p[1].uv.uv = strip.packet_words[5] | strip.texture_high1;
            poly->p[1].attr = (strip.packet_words[5] | strip.texture_high1) >> 16;
            if (!(strip.clipping_mask & next_point))
            {
                packet += 40;
                poly->tag = gpu_packet_link((uintptr_t)packet) | 0x09000000;
                poly->p[2].rgbc.rgbc = next_color;
                poly->p[2].xy.xy = next_point;
                poly->p[2].uv.uv = next_uv;
                poly->p[2].attr = next_uv >> 16;
                *last_stride = 40;
                strip.packet_stride = 40;
            }
        }
        work = (work | 7) + 1;
    } while (row-- != 0);
    /* The common continuation restores semantic caller state through native locals */
    xport_gte_write_data(6, 0);
    return packet;
}

uint8 *SubdivideFlatTexturedQuad(uint8 *packet, uint8 *limit, const ModelProjectedVertex *const points[4], uint32 area, uint32 *last_stride)
{
    uint8 *before;
    uint32 parts, step, row, command, first_point;
    SubdivisionGrid grid;
    sint32 work = 0;
    const ModelProjectedVertex *geometry[3] = {points[0], points[1], points[2]};
    uint32 corner0, corner1, corner2, corner3;
    PolygonStripContext strip;
    SubdivisionPoint attributes[4];
    const POINT_T *input = ((POLY_FT4 *)packet)->p;
    for (uint32 i = 0; i < 4; ++i)
    {
        attributes[i].screen = input[i].xy.xy;
        attributes[i].uv = input[i].uv.uv | ((uint32)input[i].attr << 16);
    }
    packet = SelectSubdivisionResolution(packet, area, &step, &parts);
    InterpolateSubdivisionGrid(grid, attributes, 1, 0x40000000, geometry, parts, step);
    geometry[0] = points[3];
    geometry[1] = points[2];
    geometry[2] = points[1];
    InterpolateReverseSubdivisionGrid(grid, attributes + 3, -1, 0x40000000, geometry, parts, step, (parts + 1) * 8);
    corner0 = attributes[0].screen;
    corner1 = attributes[1].screen;
    corner2 = attributes[2].screen;
    corner3 = attributes[3].screen;
    PrepareSubdivisionEdges(grid, parts, corner0, corner1, corner2, corner3);
    strip.texture_high0 = (uint32)(sint32)(sint16)input[0].attr << 16;
    strip.texture_high1 = (uint32)(sint32)(sint16)input[1].attr << 16;
    command = ((P_TAG *)packet)->rgbc.rgbc;
    strip.packet_limit = limit;
    strip.frustum_mask = 0xE0006000;
    strip.packet_stride = 40;
    strip.disposition = 0;
    row = parts - 1;
    do
    {
        first_point = grid.At(work).screen;
        if (packet >= limit)
            break;
        strip.packet_cursor = packet;
        strip.packet_words[0] = command;
        strip.packet_words[1] = first_point;
        strip.packet_words[2] = (uint32)(uint16)grid.At(work).uv;
        strip.packet_words[3] = grid.At(work + 8).screen;
        strip.packet_words[4] = (uint32)(uint16)grid.At(work + 8).uv;
        strip.clipping_mask = strip.packet_words[1] & strip.packet_words[3] & strip.frustum_mask;
        strip.remaining_segments = parts - 1;
        before = packet;
        packet = EmitTexturedPolygonStrip(grid, work, &strip);
        if (packet != before)
            *last_stride = strip.packet_stride;
        if (strip.disposition == 1)
            break;
        work = (work | 7) + 1;
    } while (row-- != 0);
    /* The common continuation retains caller state through native locals */
    xport_gte_write_data(6, 0);
    return packet;
}

uint8 *SubdivideFlatQuad(uint8 *packet, uint8 *limit, const ModelProjectedVertex *const points[4], uint32 area, uint32 *last_stride)
{
    uint8 *before;
    uint32 parts, step, row, command, first_point;
    uint32 corner0, corner1, corner2, corner3;
    SubdivisionGrid grid;
    sint32 work = 0;
    const ModelProjectedVertex *geometry[3] = {points[0], points[1], points[2]};
    PolygonStripContext strip;
    SubdivisionPoint attributes[4];
    const POLY_F4 *input = (POLY_F4 *)packet;
    for (uint32 i = 0; i < 4; ++i)
        attributes[i].screen = input->p[i].xy;
    packet = SelectSubdivisionResolution(packet, area, &step, &parts);
    InterpolateSubdivisionGrid(grid, attributes, 0, 0, geometry, parts, step);
    geometry[0] = points[3];
    geometry[1] = points[2];
    geometry[2] = points[1];
    InterpolateReverseSubdivisionGrid(grid, attributes, 0, 0, geometry, parts, step, (parts + 1) * 8);
    corner0 = attributes[0].screen;
    corner1 = attributes[1].screen;
    corner2 = attributes[2].screen;
    corner3 = attributes[3].screen;
    PrepareSubdivisionEdges(grid, parts, corner0, corner1, corner2, corner3);
    command = ((P_TAG *)packet)->rgbc.rgbc;
    strip.packet_limit = limit;
    strip.frustum_mask = 0xE0006000;
    strip.packet_stride = 24;
    strip.disposition = 0;
    row = parts - 1;
    do
    {
        first_point = grid.At(work).screen;
        if (packet >= limit)
            break;
        strip.packet_cursor = packet;
        strip.packet_words[1] = command;
        strip.packet_words[2] = first_point;
        strip.packet_words[3] = grid.At(work + 8).screen;
        // The original retains corner2 in t2 and the reverse row stride in t5
        strip.clipping_mask = corner2 & 0xE0006000;
        strip.remaining_segments = parts - 1;
        before = packet;
        packet = EmitFlatPolygonStrip(grid, work, &strip);
        if (packet != before)
            *last_stride = strip.packet_stride;
        if (strip.disposition == 1)
            break;
        work = (work | 7) + 1;
    } while (row-- != 0);
    /* The common continuation retains caller state through native locals */
    xport_gte_write_data(6, 0);
    return packet;
}
