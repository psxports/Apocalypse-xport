#include <new>
#include <stdlib.h>

#include "scene_lifecycle.h"
#include "game_tick.h"

#include "game_diagnostics.h"

#include "ribbon_segments.h"
#include "model_resources.h"
#include "model_projection.h"
#include "effect_lifecycle.h"

RibbonSegments::RibbonSegments(uint32 segments, uint32 mode) : RenderableEffect(), mode(mode), hidden(0), u_scroll(0), v_scroll(0), v_step(8), waving(0), point_count(segments), points(0), tex(0)
{
    FUNCTION_MARKER(0x80026A44, "SLUS_003.73");
    if (segments < 2 || segments > 0x7FFFFFFF || segments > 0xFFFFFFFF / 36 || segments > (size_t)-1 / sizeof(*points))
    {
        GameDiagnostic("Invalid ribbon point count %u\n", segments);
        abort();
    }
    points = (RibbonPoint *)malloc((size_t)segments * sizeof(*points));
    if (!points)
    {
        GameDiagnostic("Ribbon geometry allocation failed\n");
        abort();
    }
    for (uint32 i = 0; i < segments; ++i)
    {
        points[i].height = 0;
        points[i].width = 0;
        // Packet command bytes are set after copying these colors
        points[i].center.code = 0;
        points[i].edge.code = 0;
    }
}

RibbonSegments::~RibbonSegments()
{
    free(points);
}

uint32 RibbonSegments::Destroy(uint32 reason)
{
    FUNCTION_MARKER(0x80026AF4, "SLUS_003.73");
    free(points);
    points = 0;
    uint32 result = RenderableEffect::Destroy(0);
    return reason & 1 ? ReleaseEffectStorage(this) : result;
}

void RibbonSegments::SetMaterial(uint32 name)
{
    FUNCTION_MARKER(0x80026B54, "SLUS_003.73");
    tex = &GetTextureResource(name);
}

uint32 RibbonSegments::SetColors(uint32 r, uint32 g, uint32 b, uint32 tail_r, uint32 tail_g, uint32 tail_b)
{
    FUNCTION_MARKER(0x80026B80, "SLUS_003.73");
    for (uint32 i = 0; i < point_count; ++i)
    {
        RibbonPoint *point = points + i;
        point->center.r = r;
        point->center.g = g;
        point->center.b = b;
        point->edge.r = tail_r;
        point->edge.g = tail_g;
        point->edge.b = tail_b;
    }
    return 0;
}

uint32 RibbonSegments::SetSize(uint32 width, uint32 height)
{
    FUNCTION_MARKER(0x80026C18, "SLUS_003.73");
    for (uint32 i = 0; i < point_count; ++i)
    {
        points[i].width = width;
        points[i].height = height;
    }
    return 0;
}

static uint32 CalculateRibbonNormal(uint32 first, uint32 second, sint32 *output_x, sint32 *output_y, uint32 minimum)
{
    uint32 horizontal, vertical, norm, result;
    FUNCTION_MARKER(0x8002626C, "SLUS_003.73");
    horizontal = (uint32)(sint32)(sint16)second - (uint32)(sint32)(sint16)first;
    horizontal = (uint32)((sint32)(horizontal * 320) / 512);
    vertical = (uint32)((sint32)second >> 16) - (uint32)((sint32)first >> 16);
    *output_x = (sint32)(vertical);
    *output_y = (sint32)(0 - horizontal);
    if ((sint32)horizontal < 0)
        horizontal = 0 - horizontal;
    if ((sint32)vertical < 0)
        vertical = 0 - vertical;
    norm = vertical < horizontal ? horizontal + vertical / 2 : vertical + horizontal / 2;
    result = (sint32)norm < (sint32)minimum;
    if (result)
    {
        *output_y = (sint32)(0);
        *output_x = (sint32)(0);
        return result;
    }
    // A minimum of zero can reach the original DIV by zero
    result = (uint32)*output_x << 6;
    result = norm ? (uint32)((sint32)result / (sint32)norm) : ((sint32)result < 0 ? 1 : (uint32)-1);
    *output_x = (sint32)(result);
    result = (uint32)*output_y << 6;
    result = norm ? (uint32)((sint32)result / (sint32)norm) : ((sint32)result < 0 ? 1 : (uint32)-1);
    *output_y = (sint32)(result);
    result = (uint32)((sint32)((uint32)*output_x << 9) / 320);
    *output_x = (sint32)(result);
    return result;
}

// Native helpers retain the original inline packet and coordinate operations
static uint32 PackRibbonOffset(uint32 center, uint32 x, uint32 y)
{
    uint32 center_x = (uint32)(sint32)(sint16)center;
    uint32 center_y = (uint32)((sint32)center >> 16);
    return ((center_y + y) << 16) | ((center_x + x) & 65535);
}

static void MakeRibbonEdges(uint32 center, uint32 normal_x, uint32 normal_y, uint32 width, uint32 height, uint32 edges[4])
{
    uint32 x = (uint32)((sint32)(normal_x * width) >> 6);
    uint32 y = (uint32)((sint32)(normal_y * width) >> 6);
    edges[0] = PackRibbonOffset(center, x, y);
    edges[1] = PackRibbonOffset(center, 0 - x, 0 - y);
    x = (uint32)((sint32)(normal_x * height) >> 6);
    y = (uint32)((sint32)(normal_y * height) >> 6);
    edges[2] = PackRibbonOffset(center, x, y);
    edges[3] = PackRibbonOffset(center, 0 - x, 0 - y);
}

static uint32 LinkRibbonPacket(uint8 *packet, uint32 *ordering_table, uint32 tag, uint32 previous)
{
    uint32 next;
    P_TAG *header = (P_TAG *)packet;
    header->addr = previous;
    header->len = tag >> 24;
    next = *ordering_table;
    next = (next & 0xFF000000) | gpu_packet_link((uintptr_t)packet);
    *ordering_table = next;
    return next;
}

static uint32 InsertRibbonPacket(uint8 *packet, uint32 *ordering_table)
{
    P_TAG *header = (P_TAG *)packet;
    uint32 tag = header->addr | ((uint32)header->len << 24);
    uint32 previous = *ordering_table;
    return LinkRibbonPacket(packet, ordering_table, tag, previous);
}

uint32 RibbonSegments::Render()
{
    Vector3 line_start, line_end;
    ProjectedWorldLine line_projection;
    uint32 result, index = 0, first = 1, phase;
    const RibbonPoint *current, *next;
    uint32 normal_x = 0, normal_y = 0, start_x, start_y, end_x, end_y;
    uint32 current_depth, next_depth, current_width, current_height, next_width, next_height;
    uint32 previous_edges[4], edges[4], depth, value, u, v, tick, width;
    uint8 *packet, *quad_packet[4];
    uint32 *ordering_table;
    sint32 computed_x, computed_y, wave;
    POLY_GT4 *quad[4];
    DR_TWIN *apply_window, *reset_window;
    DR_TPAGE *draw_state;
    uint8 *apply_packet, *reset_packet, *state_packet;
    FUNCTION_MARKER(0x80026C68, "SLUS_003.73");
    result = this->hidden;
    if (result)
        return result;
    current = this->points;
    next = current + 1;
    line_start.x = current->pos.words[0];
    line_start.y = current->pos.words[1];
    line_start.z = current->pos.words[2];
    phase = this->v_scroll;
    for (;;)
    {
        result = (sint32)index < (sint32)(this->point_count - 1);
        if (!result)
            return result;
        line_end.x = next->pos.words[0];
        line_end.y = next->pos.words[1];
        line_end.z = next->pos.words[2];
        result = ProjectAndClipWorldLine(line_start, line_end, line_projection);
        if ((sint32)result < 0)
        {
            normal_x = 0;
            normal_y = 0;
            goto AdvanceSegment;
        }
        current_depth = line_projection.depth[0];
        current_width = current_height = 0;
        if (current_depth)
        {
            current_width = (uint32)((sint32)(400 * current->width) / (sint32)current_depth);
            current_height = (uint32)((sint32)(400 * current->height) / (sint32)current_depth);
        }
        next_depth = line_projection.depth[1];
        next_width = next_height = 0;
        if (next_depth)
        {
            next_width = (uint32)((sint32)(400 * next->width) / (sint32)next_depth);
            next_height = (uint32)((sint32)(400 * next->height) / (sint32)next_depth);
        }
        CalculateRibbonNormal(line_projection.screen[0], line_projection.screen[1], &computed_x, &computed_y, 3);
        if (normal_x | normal_y)
        {
            start_x = normal_x;
            start_y = normal_y;
            end_x = (uint32)computed_x;
            end_y = (uint32)computed_y;
            if (end_x | end_y)
            {
                normal_x = end_x;
                normal_y = end_y;
            }
            else
            {
                end_x = normal_x;
                end_y = normal_y;
            }
        }
        else
        {
            if (!(computed_x | computed_y))
            {
                first = 1;
                goto AdvanceSegment;
            }
            start_x = end_x = normal_x = (uint32)computed_x;
            start_y = end_y = normal_y = (uint32)computed_y;
        }
        if (first)
            MakeRibbonEdges(line_projection.screen[0], start_x, start_y, current_width, current_height, previous_edges);
        MakeRibbonEdges(line_projection.screen[1], end_x, end_y, next_width, next_height, edges);
        if (this->waving && (sint32)index >= (sint32)this->point_count / 2)
        {
            tick = game_clock.tick;
            wave = rsin((tick * 200 + 130) & 4095);
            edges[0] += (uint32)(wave / 2048) << 16;
            wave = rsin(((tick * 1000) & 0x3FF8) >> 2);
            value = (uint32)((wave * 3) / 4096);
            wave = rsin((tick * 150 + 100) & 4095);
            edges[1] += (value + (uint32)(wave / 2048)) << 16;
        }
        next_depth = line_projection.depth[1];
        current_depth = line_projection.depth[0];
        depth = (sint32)current_depth < (sint32)next_depth ? current_depth : next_depth;
        ordering_table = scene_render.cur->ot + ((depth & 0x3FFC) >> 2);
        packet = scene_render.cursor;
        result = scene_render.limit - packet < (sint32)(4 * sizeof(POLY_GT4) + 2 * sizeof(DR_TWIN) + sizeof(DR_TPAGE));
        if (result)
            return result;
        quad_packet[0] = packet;
        quad_packet[1] = packet + sizeof(POLY_GT4);
        quad_packet[2] = packet + 2 * sizeof(POLY_GT4);
        quad_packet[3] = packet + 3 * sizeof(POLY_GT4);
        apply_packet = packet + 4 * sizeof(POLY_GT4);
        reset_packet = apply_packet + sizeof(DR_TWIN);
        state_packet = reset_packet + sizeof(DR_TWIN);
        scene_render.cursor = state_packet + sizeof(DR_TPAGE);
        for (uint32 quad_idx = 0; quad_idx < 4; ++quad_idx)
            quad[quad_idx] = (POLY_GT4 *)quad_packet[quad_idx];
        reset_window = (DR_TWIN *)reset_packet;
        apply_window = (DR_TWIN *)apply_packet;
        setlen(reset_window, 2);
        reset_window->code[1] = 0;
        reset_window->code[0] = 0xE2000000;
        if (!tex)
        {
            GameDiagnostic("Ribbon texture is not selected\n");
            abort();
        }
        u = tex->uv[0].u;
        v = tex->uv[0].v;
        setlen(apply_window, 2);
        apply_window->code[1] = 0;
        apply_window->code[0] = 0xE2000318 | ((v >> 3) << 15) | ((u >> 3) << 10);
        InsertRibbonPacket(reset_packet, ordering_table);
        quad[3]->tag = 0;
        setPolyGT4(quad[3]);
        quad[2]->tag = 0;
        setPolyGT4(quad[2]);
        quad[1]->tag = 0;
        setPolyGT4(quad[1]);
        quad[0]->tag = 0;
        setPolyGT4(quad[0]);
        value = tex->clut;
        quad[3]->p[0].attr = value;
        quad[2]->p[0].attr = value;
        quad[1]->p[0].attr = value;
        quad[0]->p[0].attr = value;
        value = tex->tpage;
        quad[3]->p[1].attr = value;
        quad[2]->p[1].attr = value;
        quad[1]->p[1].attr = value;
        quad[0]->p[1].attr = value;
        value = this->u_scroll & 63;
        u = tex->uv[0].u + value;
        v = tex->uv[0].v + (phase & 63);
        // Original texture strips are written in quad order 2, 0, 1, 3
        quad[2]->p[0].uv.u = u;
        quad[2]->p[2].uv.u = u + 16;
        quad[2]->p[1].uv.u = u;
        quad[2]->p[0].uv.v = v;
        quad[2]->p[2].uv.v = v;
        value = (uint8)this->v_step;
        quad[2]->p[3].uv.u = u + 16;
        quad[2]->p[1].uv.v = value + v;
        quad[2]->p[3].uv.v = (uint8)this->v_step + v;
        quad[0]->p[0].uv.u = u + 16;
        quad[0]->p[0].uv.v = v;
        quad[0]->p[2].uv.u = u + 32;
        quad[0]->p[2].uv.v = v;
        quad[0]->p[1].uv.u = u + 16;
        quad[0]->p[1].uv.v = (uint8)this->v_step + v;
        quad[0]->p[3].uv.u = u + 32;
        quad[0]->p[3].uv.v = (uint8)this->v_step + v;
        quad[1]->p[0].uv.u = u + 48;
        quad[1]->p[0].uv.v = v;
        quad[1]->p[2].uv.u = u + 32;
        quad[1]->p[2].uv.v = v;
        quad[1]->p[1].uv.u = u + 48;
        value = (uint8)this->v_step;
        quad[1]->p[3].uv.u = u + 32;
        quad[1]->p[1].uv.v = value + v;
        quad[1]->p[3].uv.v = (uint8)this->v_step + v;
        quad[3]->p[0].uv.u = u + 64;
        quad[3]->p[0].uv.v = v;
        quad[3]->p[2].uv.u = u + 48;
        quad[3]->p[2].uv.v = v;
        quad[3]->p[1].uv.u = u + 64;
        value = (uint8)this->v_step;
        quad[3]->p[3].uv.u = u + 48;
        quad[3]->p[1].uv.v = value + v;
        quad[3]->p[3].uv.v = (uint8)this->v_step + v;
        value = next->edge.rgbc;
        width = this->v_step;
        quad[0]->p[0].rgbc.rgbc = value;
        quad[0]->p[1].rgbc.rgbc = current->edge.rgbc;
        quad[0]->p[2].rgbc.rgbc = next->center.rgbc;
        value = current->center.rgbc;
        quad[0]->p[0].xy.xy = edges[0];
        quad[0]->p[3].rgbc.rgbc = value;
        quad[0]->p[1].xy.xy = previous_edges[0];
        quad[0]->p[2].xy.xy = line_projection.screen[1];
        quad[0]->p[3].xy.xy = line_projection.screen[0];
        quad[2]->p[0].rgbc.rgbc = 0;
        quad[2]->p[1].rgbc.rgbc = 0;
        quad[2]->p[2].rgbc.rgbc = next->edge.rgbc;
        value = current->edge.rgbc;
        quad[2]->p[0].xy.xy = edges[2];
        quad[2]->p[1].xy.xy = previous_edges[2];
        quad[2]->p[2].xy.xy = edges[0];
        quad[2]->p[3].xy.xy = previous_edges[0];
        quad[2]->p[3].rgbc.rgbc = value;
        quad[1]->p[0].rgbc.rgbc = next->edge.rgbc;
        quad[1]->p[1].rgbc.rgbc = current->edge.rgbc;
        quad[1]->p[2].rgbc.rgbc = next->center.rgbc;
        value = current->center.rgbc;
        quad[1]->p[0].xy.xy = edges[1];
        quad[1]->p[1].xy.xy = previous_edges[1];
        quad[1]->p[3].rgbc.rgbc = value;
        quad[1]->p[2].xy.xy = line_projection.screen[1];
        quad[1]->p[3].xy.xy = line_projection.screen[0];
        quad[3]->p[0].rgbc.rgbc = 0;
        quad[3]->p[1].rgbc.rgbc = 0;
        quad[3]->p[2].rgbc.rgbc = next->edge.rgbc;
        quad[3]->p[3].rgbc.rgbc = current->edge.rgbc;
        quad[3]->p[0].xy.xy = edges[3];
        quad[3]->p[1].xy.xy = previous_edges[3];
        quad[3]->p[2].xy.xy = edges[1];
        quad[3]->p[3].xy.xy = previous_edges[1];
        quad[3]->p[0].rgbc.code = 62;
        quad[2]->p[0].rgbc.code = 62;
        quad[1]->p[0].rgbc.code = 62;
        value = quad[0]->tag;
        quad[0]->p[0].rgbc.code = 62;
        tick = *ordering_table;
        tick = LinkRibbonPacket(quad_packet[0], ordering_table, value, tick);
        value = quad[2]->tag;
        tick = LinkRibbonPacket(quad_packet[2], ordering_table, value, tick);
        value = quad[1]->tag;
        tick = LinkRibbonPacket(quad_packet[1], ordering_table, value, tick);
        value = quad[3]->tag;
        first = 0;
        tick = LinkRibbonPacket(quad_packet[3], ordering_table, value, tick);
        phase -= width;
        previous_edges[0] = edges[0];
        previous_edges[2] = edges[2];
        previous_edges[1] = edges[1];
        value = apply_window->tag;
        LinkRibbonPacket(apply_packet, ordering_table, value, tick);
        value = GetEffectDrawMode().tag;
        tick = GetEffectDrawMode().code[0];
        draw_state = (DR_TPAGE *)state_packet;
        draw_state->tag = value;
        draw_state->code[0] = tick;
        InsertRibbonPacket(state_packet, ordering_table);
        previous_edges[3] = edges[3];
    AdvanceSegment:
        ++next;
        ++current;
        value = line_end.x;
        tick = line_end.y;
        ++index;
        depth = line_end.z;
        line_start.x = value;
        line_start.y = tick;
        line_start.z = depth;
    }
}
