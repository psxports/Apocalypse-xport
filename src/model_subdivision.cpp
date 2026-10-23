#include <stdlib.h>

#include "psx.h"
#include "model_projection.h"
#include "model_render_view.h"
#include "model_subdivision.h"
#include "game_diagnostics.h"

struct SubdivisionPoint
{
    // Bit 31 carries the subdivision clipping result
    uint32 scr;
    uint32 uv;
    uint32 color;
};

class SubdivisionGrid
{
public:
    SubdivisionPoint &At(sint32 idx)
    {
        if (idx < 0 || idx >= 64)
        {
            GameDiagnostic(("Subdivision grid index exceeds its native owner: %d\n", idx));
            abort();
        }
        return points[idx];
    }

    const SubdivisionPoint &At(sint32 idx) const
    {
        if (idx < 0 || idx >= 64)
        {
            GameDiagnostic(("Subdivision grid index exceeds its native owner: %d\n", idx));
            abort();
        }
        return points[idx];
    }

private:
    SubdivisionPoint points[64];
};

struct PolygonStripContext
{
    uint8 *packet_cursor;
    uint8 *packet_limit;
    uint32 packet_words[6];
    uint32 tex_high0;
    uint32 tex_high1;
    uint32 clipping_mask;
    uint32 frustum_mask;
    uint32 remaining_segs;
    uint32 packet_stride;
    bool exhausted;
};

static void SelectSubdivResolution(const uint8 *packet, uint32 area, uint32 &step, uint32 &subdivisions)
{
    FUNCTION_MARKER(0x80082394, "SLUS_003.73");
    xport_gte_write_data(30, area >> 11);
    uint32 command = ((const P_TAG *)packet)->rgbc.rgbc;
    xport_gte_write_data(6, (command & 0xFF000000) | 0x08000000);
    uint32 leading_count = xport_gte_read_data(31);
    uint32 count = 17 - (leading_count >> 1);
    if (count >= 8)
        count = 7;
    subdivisions = count;
    // LZCR is 0..32, so the clamped divisor is 1..7
    step = ONE / count;
}

static void LoadSubdivGeometry(const ModelProjectedVertex &input0, const ModelProjectedVertex &input1, const ModelProjectedVertex &input2)
{
    FUNCTION_MARKER(0x80082508, "SLUS_003.73");
    const ModelProjectedVertex *input[3] = {&input0, &input1, &input2};
    MATRIX matrix = {0};
    for (uint32 i = 0; i < 3; ++i)
    {
        XY vertex;
        vertex.xy = input[i]->xy;
        matrix.m[0][i] = vertex.x;
        matrix.m[1][i] = vertex.y;
        matrix.m[2][i] = input[i]->depth_flags;
    }
    for (uint32 i = 0; i < 5; ++i)
        xport_gte_write_control(i, PackModelRotationWord(matrix, i));
}

static void InterpolateSubdivGrid(SubdivisionGrid &grid, const SubdivisionPoint *vertices, sint32 stride, uint32 flags, const ModelProjectedVertex *const geometry[3], uint32 subdivisions, uint32 step)
{
    const uint32 clip_bias = 0x00F00200, clip_mask = 0xBFFFBFFF;
    uint32 color0, color1, color2, row_remaining, inner_remaining;
    uint32 vertical = 0, remaining, horizontal, packed;
    sint32 row_start, row_end = 0, point;
    uint32 scr, flag;
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
    if (flags & 0x80000000)
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
    LoadSubdivGeometry(*geometry[0], *geometry[1], *geometry[2]);
    row_remaining = subdivisions;
    do
    {
        row_start = row_end;
        if (vertical >= 3840)
            vertical = ONE;
        remaining = ONE - vertical;
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
            if (horizontal >= 3840)
                packed = 0x10000000;
            scr = xport_gte_read_data(14);
            flag = xport_gte_read_flag();
            xport_gte_execute(0x4A6412);
            scr = ((scr & clip_mask) | ~((scr - clip_bias) | clip_mask)) & 0x7FFFFFFF;
            xport_gte_execute(0x198003D);
            SubdivisionPoint &vertex = grid.At(point);
            vertex.scr = scr | (flag & 0x80000000);
            vertex.uv = xport_gte_read_data(22);
            if (flags & 0x80000000)
            {
                xport_gte_execute(0x4C6412);
                xport_gte_execute(0x198003D);
                vertex.color = xport_gte_read_data(22);
            }
            xport_gte_write_data(0, packed);
        } while (inner_remaining-- != 0);
        vertical += step;
        row_end = row_start + 8;
    } while (row_remaining-- != 0);
}

static void InterpolateReverseSubdivGrid(SubdivisionGrid &grid, const SubdivisionPoint *attributes, sint32 stride, uint32 attribute_flags, const ModelProjectedVertex *const geometry[3], uint32 subdivisions, uint32 step, sint32 dst)
{
    uint32 color0, color1, color2, row_count = subdivisions - 1;
    sint32 row_end = dst + subdivisions - 8;
    sint32 row_start, point;
    uint32 vertical = 0, horizontal, remaining, inner_count;
    uint32 packed, scr, flag;
    FUNCTION_MARKER(0x8008267C, "SLUS_003.73");
    // Generate the opposite triangle in the shared interpolation grid
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
    if (attribute_flags & 0x80000000)
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
    LoadSubdivGeometry(*geometry[0], *geometry[1], *geometry[2]);
    do
    {
        row_start = row_end;
        if (vertical >= 3840)
            vertical = ONE;
        remaining = ONE - vertical;
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
            if (horizontal >= 3840)
                packed = 0x10000000;
            scr = xport_gte_read_data(14);
            flag = xport_gte_read_flag();
            xport_gte_execute(0x4A6412);
            scr = ((scr & 0xBFFFBFFF) | ~((scr - 0x00F00200) | 0xBFFFBFFF)) & 0x7FFFFFFF;
            xport_gte_execute(0x198003D);
            SubdivisionPoint &vertex = grid.At(point);
            vertex.scr = scr | (flag & 0x80000000);
            vertex.uv = xport_gte_read_data(22);
            if (attribute_flags & 0x80000000)
            {
                xport_gte_execute(0x4C6412);
                xport_gte_execute(0x198003D);
                vertex.color = xport_gte_read_data(22);
            }
            xport_gte_write_data(0, packed);
        } while (inner_count-- != 0);
        vertical += step;
        row_end = row_start - 8;
    } while (row_count-- != 0);
}

static sint32 InterpolateSubdivEdge(SubdivisionGrid &grid, sint32 pos, uint32 count, sint32 stride, uint32 prev, uint32 next)
{
    uint32 difference = next - prev;
    sint32 edge_bias = difference & 0x2000;
    sint32 correction = (difference >> 28) & 2;
    sint32 delta = 16 * (edge_bias - 0x1000) - correction + 1;
    sint32 cursor = pos + stride;
    uint32 remaining = count - 1;
    FUNCTION_MARKER(0x800826C4, "SLUS_003.73");
    // Current polygon owners select a count from two through seven
    do
    {
        SubdivisionPoint &point = grid.At(cursor);
        --remaining;
        point.scr += delta;
        cursor += stride;
    } while (remaining != 0);
    return cursor;
}

static void PrepareTriangleSubdivEdges(SubdivisionGrid &grid, uint32 count, uint32 corner0, uint32 corner1, uint32 corner2)
{
    sint32 pos = 0;
    FUNCTION_MARKER(0x8008270C, "SLUS_003.73");
    // Polygon owners carry the clipping mask separately in their strip context
    pos = InterpolateSubdivEdge(grid, pos, count, 1, corner0, corner1);
    pos = InterpolateSubdivEdge(grid, pos, count, 7, corner1, corner2);
    InterpolateSubdivEdge(grid, pos, count, -8, corner2, corner0);
}

static void PrepareSubdivEdges(SubdivisionGrid &grid, uint32 count, uint32 corner0, uint32 corner1, uint32 corner2, uint32 corner3)
{
    sint32 pos = 0;
    FUNCTION_MARKER(0x80082750, "SLUS_003.73");
    // Polygon owners carry the clipping mask separately in their strip context
    pos = InterpolateSubdivEdge(grid, pos, count, 1, corner0, corner1);
    pos = InterpolateSubdivEdge(grid, pos, count, 8, corner1, corner3);
    pos = InterpolateSubdivEdge(grid, pos, count, -1, corner3, corner2);
    InterpolateSubdivEdge(grid, pos, count, -8, corner2, corner0);
}

static uint8 *EmitFlatPolygonStrip(const SubdivisionGrid &grid, sint32 geometry, PolygonStripContext *context)
{
    uint8 *packet = context->packet_cursor;
    uint32 prev = context->clipping_mask;
    uint32 remaining = context->remaining_segs;
    uint32 scr0, scr1, combined;
    context->exhausted = false;
    FUNCTION_MARKER(0x800827A4, "SLUS_003.73");
    do
    {
        POLY_F4 *poly = (POLY_F4 *)packet;
        poly->rgbc.rgbc = context->packet_words[1];
        if (packet >= context->packet_limit)
        {
            context->packet_cursor = packet;
            context->clipping_mask = prev;
            context->remaining_segs = remaining;
            context->exhausted = true;
            return packet;
        }
        poly->p[0].xy = context->packet_words[2];
        poly->p[1].xy = context->packet_words[3];
        scr0 = grid.At(geometry + 1).scr;
        scr1 = grid.At(geometry + 9).scr;
        ++geometry;
        combined = scr0 & scr1;
        if (!(prev & combined))
        {
            packet += 24;
            poly->tag = gpu_packet_link((uintptr_t)packet) | 0x05000000;
            poly->p[2].xy = scr0;
            poly->p[3].xy = scr1;
            context->packet_stride = 24;
        }
        prev = combined & context->frustum_mask;
        context->packet_words[2] = scr0;
        context->packet_words[3] = scr1;
    } while (remaining-- != 0);
    context->packet_cursor = packet;
    context->clipping_mask = prev;
    context->remaining_segs = remaining;
    return packet;
}

static uint8 *EmitColoredPolygonStrip(const SubdivisionGrid &grid, sint32 geometry, PolygonStripContext *context)
{
    uint8 *packet = context->packet_cursor;
    uint32 clipping = context->clipping_mask;
    uint32 remaining = context->remaining_segs, next_color0, next_point0, next_color1, next_point1, combined;
    bool visible;
    context->exhausted = false;
    FUNCTION_MARKER(0x80082930, "SLUS_003.73");
    do
    {
        POLY_G4 *poly = (POLY_G4 *)packet;
        poly->p[0].rgbc.rgbc = context->packet_words[0];
        if (packet >= context->packet_limit)
        {
            context->packet_cursor = packet;
            context->clipping_mask = clipping;
            context->remaining_segs = remaining;
            context->exhausted = true;
            return packet;
        }
        poly->p[0].xy.xy = context->packet_words[1];
        poly->p[1].rgbc.rgbc = context->packet_words[3];
        poly->p[1].xy.xy = context->packet_words[4];
        next_color0 = grid.At(geometry + 1).color;
        next_point0 = grid.At(geometry + 1).scr;
        next_color1 = grid.At(geometry + 9).color;
        next_point1 = grid.At(geometry + 9).scr;
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
    context->remaining_segs = remaining;
    return packet;
}

static uint8 *EmitTexturedPolygonStrip(const SubdivisionGrid &grid, sint32 geometry, PolygonStripContext *context)
{
    uint8 *packet = context->packet_cursor;
    uint32 clipping = context->clipping_mask;
    uint32 remaining = context->remaining_segs;
    uint32 word0 = context->packet_words[0], word1 = context->packet_words[1];
    uint32 word2 = context->packet_words[2], word3 = context->packet_words[3];
    uint32 word4 = context->packet_words[4];
    bool visible;
    context->exhausted = false;
    FUNCTION_MARKER(0x80082B14, "SLUS_003.73");
    do
    {
        POLY_FT4 *poly = (POLY_FT4 *)packet;
        uint32 next0, next1, next2, next3, mask;
        if (packet >= context->packet_limit)
        {
            context->packet_cursor = packet;
            context->clipping_mask = clipping;
            context->remaining_segs = remaining;
            context->packet_words[0] = word0;
            context->packet_words[1] = word1;
            context->packet_words[2] = word2 | context->tex_high0;
            context->packet_words[3] = word3;
            context->packet_words[4] = word4;
            context->exhausted = true;
            return packet;
        }
        poly->rgbc.rgbc = word0;
        poly->p[0].xy.xy = word1;
        poly->p[0].uv.uv = word2 | context->tex_high0;
        poly->p[0].attr = (word2 | context->tex_high0) >> 16;
        poly->p[1].xy.xy = word3;
        poly->p[1].uv.uv = word4 | context->tex_high1;
        poly->p[1].attr = (word4 | context->tex_high1) >> 16;
        next0 = grid.At(geometry + 1).scr;
        next1 = (uint16)grid.At(geometry + 1).uv;
        next2 = grid.At(geometry + 9).scr;
        next3 = (uint16)grid.At(geometry + 9).uv;
        ++geometry;
        mask = next0 & next2;
        visible = !(clipping & mask);
        clipping = mask & context->frustum_mask;
        if (visible)
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
    } while (remaining-- != 0);
    context->packet_cursor = packet;
    context->clipping_mask = clipping;
    context->remaining_segs = remaining;
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
    uint32 remaining = context->remaining_segs;
    uint32 next_clip;
    bool visible;
    uint32 idx;
    context->exhausted = false;
    FUNCTION_MARKER(0x80082D1C, "SLUS_003.73");
    for (idx = 0; idx < 6; ++idx)
        words[idx] = context->packet_words[idx];
    do
    {
        POLY_GT4 *poly = (POLY_GT4 *)packet;
        words[2] |= context->tex_high0;
        if (packet >= context->packet_limit)
        {
            context->packet_cursor = packet;
            context->clipping_mask = clip;
            context->remaining_segs = remaining;
            for (idx = 0; idx < 6; ++idx)
                context->packet_words[idx] = words[idx];
            xport_gte_write_data(6, 0);
            context->exhausted = true;
            return packet;
        }
        poly->p[0].rgbc.rgbc = words[0];
        poly->p[0].xy.xy = words[1];
        poly->p[0].uv.uv = words[2];
        poly->p[0].attr = words[2] >> 16;
        poly->p[1].rgbc.rgbc = words[3];
        poly->p[1].xy.xy = words[4];
        poly->p[1].uv.uv = words[5] | context->tex_high1;
        poly->p[1].attr = (words[5] | context->tex_high1) >> 16;
        words[0] = grid.At(geometry + 1).color;
        words[1] = grid.At(geometry + 1).scr;
        words[2] = (uint16)grid.At(geometry + 1).uv;
        words[3] = grid.At(geometry + 9).color;
        words[4] = grid.At(geometry + 9).scr;
        words[5] = (uint16)grid.At(geometry + 9).uv;
        ++geometry;
        next_clip = words[1] & words[4];
        visible = !(clip & next_clip);
        clip = next_clip & context->frustum_mask;
        if (visible)
        {
            packet += 52;
            context->packet_stride = 52;
            poly->tag = gpu_packet_link((uintptr_t)packet) | 0x0C000000;
            for (idx = 0; idx < 2; ++idx)
            {
                poly->p[idx + 2].rgbc.rgbc = words[idx * 3];
                poly->p[idx + 2].xy.xy = words[idx * 3 + 1];
                poly->p[idx + 2].uv.uv = words[idx * 3 + 2];
                poly->p[idx + 2].attr = words[idx * 3 + 2] >> 16;
            }
        }
    } while (remaining-- != 0);
    context->packet_cursor = packet;
    context->clipping_mask = clip;
    context->remaining_segs = remaining;
    for (idx = 0; idx < 6; ++idx)
        context->packet_words[idx] = words[idx];
    return packet;
}

uint8 *SubdivColoredPolygon(uint8 *packet, uint8 *limit, const ModelProjectedVertex *const points[4], uint32 area, bool quad, uint32 &last_stride)
{
    uint8 *before;
    SubdivisionGrid grid;
    sint32 work = 0;
    const ModelProjectedVertex *geometry[3];
    uint32 parts, step, row, command, rgbc0, next_point, next_color;
    uint32 corner0, corner1, corner2, corner3;
    PolygonStripContext strip;
    SubdivisionPoint attributes[4];
    const POINT_C *input = quad ? ((POLY_G4 *)packet)->p : ((POLY_G3 *)packet)->p;
    uint32 vertex_count = quad ? 4 : 3;
    for (uint32 i = 0; i < vertex_count; ++i)
    {
        attributes[i].scr = input[i].xy.xy;
        attributes[i].color = input[i].rgbc.rgbc;
    }
    geometry[0] = points[0];
    geometry[1] = points[1];
    geometry[2] = points[2];
    SelectSubdivResolution(packet, area, step, parts);
    InterpolateSubdivGrid(grid, attributes, 1, 0x80000000, geometry, parts, step);
    if (quad)
    {
        geometry[0] = points[3];
        geometry[1] = points[2];
        geometry[2] = points[1];
        InterpolateReverseSubdivGrid(grid, attributes + 3, -1, 0x80000000, geometry, parts, step, (parts + 1) * 8);
        corner0 = attributes[0].scr;
        corner1 = attributes[1].scr;
        corner2 = attributes[2].scr;
        corner3 = attributes[3].scr;
        PrepareSubdivEdges(grid, parts, corner0, corner1, corner2, corner3);
    }
    else
    {
        corner0 = attributes[0].scr;
        corner1 = attributes[1].scr;
        corner2 = attributes[2].scr;
        PrepareTriangleSubdivEdges(grid, parts, corner0, corner1, corner2);
    }
    command = ((P_TAG *)packet)->rgbc.rgbc & 0xFF000000;
    if (!quad)
        command ^= 0x08000000;
    strip.packet_limit = limit;
    strip.frustum_mask = 0xE0006000;
    strip.packet_stride = quad ? 36 : 28;
    strip.exhausted = false;
    row = parts - 1;
    do
    {
        rgbc0 = grid.At(work).color;
        if (packet >= limit)
            break;
        strip.packet_cursor = packet;
        strip.packet_words[0] = rgbc0;
        strip.packet_words[1] = grid.At(work).scr;
        strip.packet_words[3] = grid.At(work + 8).color;
        strip.packet_words[4] = grid.At(work + 8).scr;
        strip.clipping_mask = strip.packet_words[1] & strip.packet_words[4] & strip.frustum_mask;
        before = packet;
        if (quad || row > 0)
        {
            strip.remaining_segs = quad ? parts - 1 : row - 1;
            packet = EmitColoredPolygonStrip(grid, work, &strip);
            work += quad ? parts : row;
            if (packet != before)
                last_stride = strip.packet_stride;
            if (strip.exhausted)
                break;
        }
        if (!quad)
        {
            POLY_G3 *poly = (POLY_G3 *)packet;
            poly->p[0].rgbc.rgbc = (strip.packet_words[0] | command) ^ 0x08000000;
            poly->p[0].xy.xy = strip.packet_words[1];
            poly->p[1].rgbc.rgbc = strip.packet_words[3];
            next_point = grid.At(work + 1).scr;
            next_color = grid.At(work + 1).color;
            poly->p[1].xy.xy = strip.packet_words[4];
            if (!(strip.clipping_mask & next_point))
            {
                packet += 28;
                poly->tag = gpu_packet_link((uintptr_t)packet) | 0x06000000;
                poly->p[2].rgbc.rgbc = next_color;
                poly->p[2].xy.xy = next_point;
                last_stride = 28;
                strip.packet_stride = 28;
            }
        }
        work = (work | 7) + 1;
    } while (row-- != 0);
    xport_gte_write_data(6, 0);
    return packet;
}

uint8 *SubdivFlatTriangle(uint8 *packet, uint8 *limit, const ModelProjectedVertex *const points[4], uint32 area, bool textured, uint32 &last_stride)
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
        attributes[i].scr = textured ? input[i].xy.xy : ((POLY_F3 *)packet)->p[i].xy;
        if (textured)
            attributes[i].uv = input[i].uv.uv | ((uint32)input[i].attr << 16);
    }
    geometry[0] = points[0];
    geometry[1] = points[1];
    geometry[2] = points[2];
    SelectSubdivResolution(packet, area, step, parts);
    InterpolateSubdivGrid(grid, attributes, textured ? 1 : 0, textured ? 0x40000000 : 0, geometry, parts, step);
    corner0 = attributes[0].scr;
    corner1 = attributes[1].scr;
    corner2 = attributes[2].scr;
    PrepareTriangleSubdivEdges(grid, parts, corner0, corner1, corner2);
    strip.packet_limit = limit;
    strip.frustum_mask = 0xE0006000;
    strip.packet_stride = textured ? 32 : 20;
    strip.exhausted = false;
    if (textured)
    {
        strip.tex_high0 = (uint32)input[0].attr << 16;
        strip.tex_high1 = (uint32)input[1].attr << 16;
    }
    command = ((P_TAG *)packet)->rgbc.rgbc;
    row = parts - 1;
    do
    {
        next_point = grid.At(work).scr;
        if (packet >= limit)
            break;
        strip.packet_cursor = packet;
        before = packet;
        if (textured)
        {
            strip.packet_words[0] = command ^ 0x08000000;
            strip.packet_words[1] = next_point;
            strip.packet_words[2] = (uint16)grid.At(work).uv;
            strip.packet_words[3] = grid.At(work + 8).scr;
            strip.packet_words[4] = (uint16)grid.At(work + 8).uv;
            strip.clipping_mask = strip.packet_words[1] & strip.packet_words[3] & strip.frustum_mask;
        }
        else
        {
            strip.packet_words[1] = command ^ 0x08000000;
            strip.packet_words[2] = next_point;
            strip.packet_words[3] = grid.At(work + 8).scr;
            strip.clipping_mask = strip.packet_words[2] & strip.packet_words[3] & strip.frustum_mask;
        }
        if (row > 0)
        {
            strip.remaining_segs = row - 1;
            packet = textured ? EmitTexturedPolygonStrip(grid, work, &strip) : EmitFlatPolygonStrip(grid, work, &strip);
            work += row;
            if (packet != before)
                last_stride = strip.packet_stride;
            if (strip.exhausted)
                break;
        }
        if (textured)
        {
            POLY_FT3 *poly = (POLY_FT3 *)packet;
            poly->rgbc.rgbc = command;
            poly->p[0].xy.xy = strip.packet_words[1];
            poly->p[0].uv.uv = strip.packet_words[2] | strip.tex_high0;
            poly->p[0].attr = (strip.packet_words[2] | strip.tex_high0) >> 16;
            poly->p[1].xy.xy = strip.packet_words[3];
            next_point = grid.At(work + 1).scr;
            next_uv = (uint16)grid.At(work + 1).uv;
            poly->p[1].uv.uv = strip.packet_words[4] | strip.tex_high1;
            poly->p[1].attr = (strip.packet_words[4] | strip.tex_high1) >> 16;
            if (!(strip.clipping_mask & next_point))
            {
                packet += 32;
                poly->tag = gpu_packet_link((uintptr_t)packet) | 0x07000000;
                poly->p[2].xy.xy = next_point;
                poly->p[2].uv.uv = next_uv;
                poly->p[2].attr = next_uv >> 16;
                last_stride = 32;
                strip.packet_stride = 32;
            }
        }
        else
        {
            POLY_F3 *poly = (POLY_F3 *)packet;
            poly->p[0].xy = strip.packet_words[2];
            next_point = grid.At(work + 1).scr;
            poly->p[1].xy = strip.packet_words[3];
            poly->rgbc.rgbc = command;
            if (!(strip.clipping_mask & next_point))
            {
                packet += 20;
                poly->tag = gpu_packet_link((uintptr_t)packet) | 0x04000000;
                poly->p[2].xy = next_point;
                last_stride = 20;
                strip.packet_stride = 20;
            }
        }
        work = (work | 7) + 1;
    } while (row-- != 0);
    xport_gte_write_data(6, 0);
    return packet;
}

uint8 *SubdivColoredTexturedPolygon(uint8 *packet, uint8 *limit, const ModelProjectedVertex *const points[4], uint32 area, bool quad, uint32 &last_stride)
{
    uint8 *before;
    uint32 parts, step, row, command;
    uint32 rgbc0, next_color, next_point, next_uv;
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
        attributes[i].scr = input[i].xy.xy;
        attributes[i].color = input[i].rgbc.rgbc;
        attributes[i].uv = input[i].uv.uv | ((uint32)input[i].attr << 16);
    }
    SelectSubdivResolution(packet, area, step, parts);
    InterpolateSubdivGrid(grid, attributes, 1, 0xC0000000, geometry, parts, step);
    if (quad)
    {
        geometry[0] = points[3];
        geometry[1] = points[2];
        geometry[2] = points[1];
        InterpolateReverseSubdivGrid(grid, attributes + 3, -1, 0xC0000000, geometry, parts, step, (parts + 1) * 8);
        corner0 = attributes[0].scr;
        corner1 = attributes[1].scr;
        corner2 = attributes[2].scr;
        corner3 = attributes[3].scr;
        PrepareSubdivEdges(grid, parts, corner0, corner1, corner2, corner3);
    }
    else
    {
        corner0 = attributes[0].scr;
        corner1 = attributes[1].scr;
        corner2 = attributes[2].scr;
        PrepareTriangleSubdivEdges(grid, parts, corner0, corner1, corner2);
    }
    command = (((P_TAG *)packet)->rgbc.rgbc & 0xFF000000) ^ 0x08000000;
    strip.tex_high0 = (uint32)input[0].attr << 16;
    strip.tex_high1 = (uint32)input[1].attr << 16;
    strip.packet_limit = limit;
    strip.frustum_mask = 0xE0006000;
    // The constructed packet supplies the initial stride
    strip.packet_stride = quad ? 52 : 40;
    strip.exhausted = false;
    row = parts - 1;
    do
    {
        rgbc0 = grid.At(work).color;
        if (packet >= limit)
            break;
        strip.packet_cursor = packet;
        strip.packet_words[0] = rgbc0;
        strip.packet_words[1] = grid.At(work).scr;
        strip.packet_words[2] = (uint16)grid.At(work).uv;
        strip.packet_words[3] = grid.At(work + 8).color;
        strip.packet_words[4] = grid.At(work + 8).scr;
        strip.packet_words[5] = (uint16)grid.At(work + 8).uv;
        strip.clipping_mask = strip.packet_words[1] & strip.packet_words[4] & strip.frustum_mask;
        before = packet;
        if (quad || row > 0)
        {
            strip.remaining_segs = quad ? parts - 1 : row - 1;
            packet = EmitColoredTexturedPolygonStrip(grid, work, &strip);
            work += quad ? parts : row;
            if (packet != before)
                last_stride = strip.packet_stride;
            if (strip.exhausted)
                break;
        }
        if (!quad)
        {
            POLY_GT3 *poly = (POLY_GT3 *)packet;
            poly->p[0].rgbc.rgbc = (strip.packet_words[0] | command) ^ 0x08000000;
            poly->p[0].xy.xy = strip.packet_words[1];
            poly->p[0].uv.uv = strip.packet_words[2] | strip.tex_high0;
            poly->p[0].attr = (strip.packet_words[2] | strip.tex_high0) >> 16;
            poly->p[1].rgbc.rgbc = strip.packet_words[3];
            poly->p[1].xy.xy = strip.packet_words[4];
            next_color = grid.At(work + 1).color;
            next_point = grid.At(work + 1).scr;
            next_uv = (uint16)grid.At(work + 1).uv;
            poly->p[1].uv.uv = strip.packet_words[5] | strip.tex_high1;
            poly->p[1].attr = (strip.packet_words[5] | strip.tex_high1) >> 16;
            if (!(strip.clipping_mask & next_point))
            {
                packet += 40;
                poly->tag = gpu_packet_link((uintptr_t)packet) | 0x09000000;
                poly->p[2].rgbc.rgbc = next_color;
                poly->p[2].xy.xy = next_point;
                poly->p[2].uv.uv = next_uv;
                poly->p[2].attr = next_uv >> 16;
                last_stride = 40;
                strip.packet_stride = 40;
            }
        }
        work = (work | 7) + 1;
    } while (row-- != 0);
    xport_gte_write_data(6, 0);
    return packet;
}

uint8 *SubdivFlatTexturedQuad(uint8 *packet, uint8 *limit, const ModelProjectedVertex *const points[4], uint32 area, uint32 &last_stride)
{
    uint8 *before;
    uint32 parts, step, row, command, scr0;
    SubdivisionGrid grid;
    sint32 work = 0;
    const ModelProjectedVertex *geometry[3] = {points[0], points[1], points[2]};
    uint32 corner0, corner1, corner2, corner3;
    PolygonStripContext strip;
    SubdivisionPoint attributes[4];
    const POINT_T *input = ((POLY_FT4 *)packet)->p;
    for (uint32 i = 0; i < 4; ++i)
    {
        attributes[i].scr = input[i].xy.xy;
        attributes[i].uv = input[i].uv.uv | ((uint32)input[i].attr << 16);
    }
    SelectSubdivResolution(packet, area, step, parts);
    InterpolateSubdivGrid(grid, attributes, 1, 0x40000000, geometry, parts, step);
    geometry[0] = points[3];
    geometry[1] = points[2];
    geometry[2] = points[1];
    InterpolateReverseSubdivGrid(grid, attributes + 3, -1, 0x40000000, geometry, parts, step, (parts + 1) * 8);
    corner0 = attributes[0].scr;
    corner1 = attributes[1].scr;
    corner2 = attributes[2].scr;
    corner3 = attributes[3].scr;
    PrepareSubdivEdges(grid, parts, corner0, corner1, corner2, corner3);
    strip.tex_high0 = (uint32)input[0].attr << 16;
    strip.tex_high1 = (uint32)input[1].attr << 16;
    command = ((P_TAG *)packet)->rgbc.rgbc;
    strip.packet_limit = limit;
    strip.frustum_mask = 0xE0006000;
    strip.packet_stride = 40;
    strip.exhausted = false;
    row = parts - 1;
    do
    {
        scr0 = grid.At(work).scr;
        if (packet >= limit)
            break;
        strip.packet_cursor = packet;
        strip.packet_words[0] = command;
        strip.packet_words[1] = scr0;
        strip.packet_words[2] = (uint16)grid.At(work).uv;
        strip.packet_words[3] = grid.At(work + 8).scr;
        strip.packet_words[4] = (uint16)grid.At(work + 8).uv;
        strip.clipping_mask = strip.packet_words[1] & strip.packet_words[3] & strip.frustum_mask;
        strip.remaining_segs = parts - 1;
        before = packet;
        packet = EmitTexturedPolygonStrip(grid, work, &strip);
        if (packet != before)
            last_stride = strip.packet_stride;
        if (strip.exhausted)
            break;
        work = (work | 7) + 1;
    } while (row-- != 0);
    xport_gte_write_data(6, 0);
    return packet;
}

uint8 *SubdivFlatQuad(uint8 *packet, uint8 *limit, const ModelProjectedVertex *const points[4], uint32 area, uint32 &last_stride)
{
    uint8 *before;
    uint32 parts, step, row, command, scr0;
    uint32 corner0, corner1, corner2, corner3;
    SubdivisionGrid grid;
    sint32 work = 0;
    const ModelProjectedVertex *geometry[3] = {points[0], points[1], points[2]};
    PolygonStripContext strip;
    SubdivisionPoint attributes[4];
    const POLY_F4 *input = (POLY_F4 *)packet;
    for (uint32 i = 0; i < 4; ++i)
        attributes[i].scr = input->p[i].xy;
    SelectSubdivResolution(packet, area, step, parts);
    InterpolateSubdivGrid(grid, attributes, 0, 0, geometry, parts, step);
    geometry[0] = points[3];
    geometry[1] = points[2];
    geometry[2] = points[1];
    InterpolateReverseSubdivGrid(grid, attributes, 0, 0, geometry, parts, step, (parts + 1) * 8);
    corner0 = attributes[0].scr;
    corner1 = attributes[1].scr;
    corner2 = attributes[2].scr;
    corner3 = attributes[3].scr;
    PrepareSubdivEdges(grid, parts, corner0, corner1, corner2, corner3);
    command = ((P_TAG *)packet)->rgbc.rgbc;
    strip.packet_limit = limit;
    strip.frustum_mask = 0xE0006000;
    strip.packet_stride = 24;
    strip.exhausted = false;
    row = parts - 1;
    do
    {
        scr0 = grid.At(work).scr;
        if (packet >= limit)
            break;
        strip.packet_cursor = packet;
        strip.packet_words[1] = command;
        strip.packet_words[2] = scr0;
        strip.packet_words[3] = grid.At(work + 8).scr;
        // The original retains corner2 in t2 and the reverse row stride in t5
        strip.clipping_mask = corner2 & 0xE0006000;
        strip.remaining_segs = parts - 1;
        before = packet;
        packet = EmitFlatPolygonStrip(grid, work, &strip);
        if (packet != before)
            last_stride = strip.packet_stride;
        if (strip.exhausted)
            break;
        work = (work | 7) + 1;
    } while (row-- != 0);
    xport_gte_write_data(6, 0);
    return packet;
}
