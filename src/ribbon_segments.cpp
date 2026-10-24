#include <stdlib.h>

#include "scene_lifecycle.h"
#include "game_tick.h"
#include "game_diagnostics.h"
#include "ribbon_segments.h"
#include "model_projection.h"
#include "global.h"
#include "effect_lifecycle.h"
#include "effect_object.h"
#include "renderable_effect.h"
#include "texture_resources.h"
#include "hd.h"

sint32 RibbonSegments::CalculateNormal(XY xy0, XY xy1, sint32 *x_ret, sint32 *y_ret, sint32 min_len)
{
    FUNCTION_MARKER(0x8002626C, "SLUS_003.73");
    sint32 dx = (xy1.x - xy0.x) * 320 / 512;
    sint32 dy = xy1.y - xy0.y;
    *x_ret = dy;
    *y_ret = -dx;
    if (dx < 0)
        dx = -dx;
    if (dy < 0)
        dy = -dy;
    sint32 norm = dy < dx ? dx + dy / 2 : dy + dx / 2;
    if (norm < min_len)
    {
        *y_ret = 0;
        *x_ret = 0;
        return 1;
    }
    // A zero minimum retains the original DIV by zero
    sint32 scaled = *x_ret * 64;
    *x_ret = DivideSignedWord(scaled, norm);
    scaled = *y_ret * 64;
    *y_ret = DivideSignedWord(scaled, norm);
    *x_ret = *x_ret * 512 / 320;
    return *x_ret;
}

RibbonSegments::RibbonSegments(uint32 segs, uint8 mode) : RenderableEffect(), mode(mode), hd_history(0), hidden(false), u_scroll(0), v_scroll(0), v_step(8), waving(false), point_count(segs), points(0), tex(0)
{
    FUNCTION_MARKER(0x80026A44, "SLUS_003.73");
    if (segs < 2 || segs > 0x7FFFFFFF || segs > 0xFFFFFFFF / 36 || segs > (size_t)-1 / sizeof(*points))
    {
        GameDiagnostic(("Invalid ribbon point count %u\n", segs));
        abort();
    }
    points = (RibbonPoint *)malloc(segs * sizeof(*points));
    if (!points)
    {
        GameDiagnostic(("Ribbon geometry allocation failed\n"));
        abort();
    }
    for (uint32 i = 0; i < segs; ++i)
    {
        RibbonPoint *point = points + i;
        point->height = 0;
        point->width = 0;
        // Packet command bytes are set after copying these colors
        point->center.code = 0;
        point->edge.code = 0;
    }
}

RibbonSegments::~RibbonSegments()
{
    hd_ribbon_free(hd_history);
    free(points);
}

uint32 RibbonSegments::Destroy(uint32 reason)
{
    FUNCTION_MARKER(0x80026AF4, "SLUS_003.73");
    free(points);
    points = 0;
    uint32 result = RenderableEffect::Destroy(0);
    return reason & 1 ? ReleaseStorage() : result;
}

void RibbonSegments::SetMaterial(uint32 name)
{
    FUNCTION_MARKER(0x80026B54, "SLUS_003.73");
    tex = &TextureResource::Get(name);
}

uint32 RibbonSegments::SetColors(uint8 r, uint8 g, uint8 b, uint8 tail_r, uint8 tail_g, uint8 tail_b)
{
    FUNCTION_MARKER(0x80026B80, "SLUS_003.73");
    for (sint32 i = 0; i < point_count; ++i)
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

uint32 RibbonSegments::SetSize(uint16 width, uint16 height)
{
    FUNCTION_MARKER(0x80026C18, "SLUS_003.73");
    for (sint32 i = 0; i < point_count; ++i)
    {
        RibbonPoint *point = points + i;
        point->width = width;
        point->height = height;
    }
    return 0;
}

// Native helpers retain the original inline packet and coordinate operations
static XY OffsetPoint(XY point, sint32 x, sint32 y)
{
    point.x += x;
    point.y += y;
    return point;
}

static void MakeEdges(XY center, sint32 normal_x, sint32 normal_y, sint32 width, sint32 height, XY edges[4])
{
    sint32 product_x = (uint32)normal_x * width;
    sint32 product_y = (uint32)normal_y * width;
    sint32 x = product_x >> 6;
    sint32 y = product_y >> 6;
    edges[0] = OffsetPoint(center, x, y);
    edges[1] = OffsetPoint(center, -x, -y);
    product_x = (uint32)normal_x * height;
    product_y = (uint32)normal_y * height;
    x = product_x >> 6;
    y = product_y >> 6;
    edges[2] = OffsetPoint(center, x, y);
    edges[3] = OffsetPoint(center, -x, -y);
}

static uint32 LinkPacket(uint8 *packet, uint32 *ordering_table, uint32 tag, uint32 prev)
{
    uint32 next;
    P_TAG *header = (P_TAG *)packet;
    header->addr = prev;
    header->len = tag >> 24;
    next = *ordering_table;
    next = (next & 0xFF000000) | gpu_packet_link((uintptr_t)packet);
    *ordering_table = next;
    return next;
}

static uint32 InsertPacket(uint8 *packet, uint32 *ordering_table)
{
    P_TAG *header = (P_TAG *)packet;
    uint32 tag = header->addr | ((uint32)header->len << 24);
    uint32 prev = *ordering_table;
    return LinkPacket(packet, ordering_table, tag, prev);
}

uint32 RibbonSegments::CaptureHd(bool logic) const
{
    if (hidden)
    {
        hd_ribbon_reset(hd_history);
        return 1;
    }
    if (!tex)
    {
        GameDiagnostic(("Ribbon texture is not selected\n"));
        abort();
    }
    HD_RIBBON_POINT *draw_points = hd_effect_ribbon_points(point_count);
    if (!draw_points)
    {
        GameDiagnostic(("Cannot capture HD ribbon points\n"));
        abort();
    }
    for (sint32 index = 0; index < point_count; ++index)
    {
        const RibbonPoint &point = points[index];
        HD_RIBBON_POINT &draw = draw_points[index];
        draw.point.vx = point.pos.x / 4096.0f;
        draw.point.vy = point.pos.y / 4096.0f;
        draw.point.vz = point.pos.z / 4096.0f;
        draw.center = point.center.rgbc;
        draw.edge = point.edge.rgbc;
        draw.width = point.width;
        draw.height = point.height;
    }
    HD_RIBBON ribbon = {0};
    ribbon.points = draw_points;
    ribbon.count = point_count;
    ribbon.page = tex->tpage;
    ribbon.clut = tex->clut;
    ribbon.texture_region = tex->region;
    ribbon.u = tex->uv[0].u;
    ribbon.v = tex->uv[0].v;
    ribbon.u_scroll = u_scroll;
    ribbon.v_scroll = v_scroll;
    ribbon.v_step = v_step;
    ribbon.tick = game_clock.tick;
    ribbon.waving = waving;
    const ModelProjectionConfig &cfg = GetModelProjConfig();
    if (!(logic ? hd_ribbon_tick(&hd_history, &ribbon, cfg.near_depth, cfg.max_distance) : hd_ribbon_capture(&hd_history, &ribbon, cfg.near_depth, cfg.max_distance)))
    {
        GameDiagnostic(("Cannot capture HD ribbon segments\n"));
        abort();
    }
    return 0;
}

bool RibbonSegments::CaptureHdTick()
{
    CaptureHd(true);
    return true;
}

uint32 RibbonSegments::Render()
{
    Vector3 line_start, line_end;
    ProjectedWorldLine line_proj;
    uint32 result, phase;
    sint32 idx = 0;
    bool start_strip = true;
    const RibbonPoint *cur, *next;
    sint32 normal_x = 0, normal_y = 0, start_x, start_y, end_x, end_y;
    sint32 cur_depth, next_depth, cur_width, cur_height, next_width, next_height;
    XY prev_edges[4], edges[4];
    sint32 depth;
    uint32 val, u, v, tick, width;
    uint8 *packet, *quad_packet[4];
    uint32 *ordering_table;
    sint32 computed_x, computed_y, wave, wave_offset, clip_result;
    POLY_GT4 *quad[4];
    DR_TWIN *apply_window, *reset_window;
    DR_TPAGE *draw_state;
    uint8 *apply_packet, *reset_packet, *state_packet;
    FUNCTION_MARKER(0x80026C68, "SLUS_003.73");
    if (hd_ui_collecting())
        return CaptureHd(false);
    hd_ribbon_reset(hd_history);
    result = hidden;
    if (result)
        return result;
    cur = points;
    next = cur + 1;
    line_start = cur->pos;
    phase = v_scroll;
    for (;;)
    {
        result = idx < point_count - 1;
        if (!result)
            return result;
        line_end = next->pos;
        clip_result = ProjectAndClipWorldLine(line_start, line_end, line_proj);
        if (clip_result < 0)
        {
            normal_x = 0;
            normal_y = 0;
            goto AdvanceSegment;
        }
        cur_depth = line_proj.depth[0];
        cur_width = cur_height = 0;
        if (cur_depth)
        {
            cur_width = 400 * cur->width / cur_depth;
            cur_height = 400 * cur->height / cur_depth;
        }
        next_depth = line_proj.depth[1];
        next_width = next_height = 0;
        if (next_depth)
        {
            next_width = 400 * next->width / next_depth;
            next_height = 400 * next->height / next_depth;
        }
        RibbonSegments::CalculateNormal(line_proj.scr[0], line_proj.scr[1], &computed_x, &computed_y, 3);
        if (normal_x | normal_y)
        {
            start_x = normal_x;
            start_y = normal_y;
            end_x = computed_x;
            end_y = computed_y;
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
                start_strip = true;
                goto AdvanceSegment;
            }
            start_x = end_x = normal_x = computed_x;
            start_y = end_y = normal_y = computed_y;
        }
        if (start_strip)
            MakeEdges(line_proj.scr[0], start_x, start_y, cur_width, cur_height, prev_edges);
        MakeEdges(line_proj.scr[1], end_x, end_y, next_width, next_height, edges);
        if (waving && idx >= point_count / 2)
        {
            tick = game_clock.tick;
            wave = rsin((tick * 200 + 130) & 4095);
            edges[0].y += wave / 2048;
            wave = rsin(((tick * 1000) & 0x3FF8) >> 2);
            wave_offset = wave * 3 / ONE;
            wave = rsin((tick * 150 + 100) & 4095);
            edges[1].y += wave_offset + wave / 2048;
        }
        next_depth = line_proj.depth[1];
        cur_depth = line_proj.depth[0];
        depth = cur_depth < next_depth ? cur_depth : next_depth;
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
            GameDiagnostic(("Ribbon texture is not selected\n"));
            abort();
        }
        u = tex->uv[0].u;
        v = tex->uv[0].v;
        setlen(apply_window, 2);
        apply_window->code[1] = 0;
        apply_window->code[0] = 0xE2000318 | ((v >> 3) << 15) | ((u >> 3) << 10);
        InsertPacket(reset_packet, ordering_table);
        quad[3]->tag = 0;
        setPolyGT4(quad[3]);
        quad[2]->tag = 0;
        setPolyGT4(quad[2]);
        quad[1]->tag = 0;
        setPolyGT4(quad[1]);
        quad[0]->tag = 0;
        setPolyGT4(quad[0]);
        val = tex->clut;
        quad[3]->p[0].attr = val;
        quad[2]->p[0].attr = val;
        quad[1]->p[0].attr = val;
        quad[0]->p[0].attr = val;
        val = tex->tpage;
        quad[3]->p[1].attr = val;
        quad[2]->p[1].attr = val;
        quad[1]->p[1].attr = val;
        quad[0]->p[1].attr = val;
        val = u_scroll & 63;
        u = tex->uv[0].u + val;
        v = tex->uv[0].v + (phase & 63);
        // Original texture strips are written in quad order 2, 0, 1, 3
        quad[2]->p[0].uv.u = u;
        quad[2]->p[2].uv.u = u + 16;
        quad[2]->p[1].uv.u = u;
        quad[2]->p[0].uv.v = v;
        quad[2]->p[2].uv.v = v;
        val = v_step;
        quad[2]->p[3].uv.u = u + 16;
        quad[2]->p[1].uv.v = val + v;
        quad[2]->p[3].uv.v = v_step + v;
        quad[0]->p[0].uv.u = u + 16;
        quad[0]->p[0].uv.v = v;
        quad[0]->p[2].uv.u = u + 32;
        quad[0]->p[2].uv.v = v;
        quad[0]->p[1].uv.u = u + 16;
        quad[0]->p[1].uv.v = v_step + v;
        quad[0]->p[3].uv.u = u + 32;
        quad[0]->p[3].uv.v = v_step + v;
        quad[1]->p[0].uv.u = u + 48;
        quad[1]->p[0].uv.v = v;
        quad[1]->p[2].uv.u = u + 32;
        quad[1]->p[2].uv.v = v;
        quad[1]->p[1].uv.u = u + 48;
        val = v_step;
        quad[1]->p[3].uv.u = u + 32;
        quad[1]->p[1].uv.v = val + v;
        quad[1]->p[3].uv.v = v_step + v;
        quad[3]->p[0].uv.u = u + 64;
        quad[3]->p[0].uv.v = v;
        quad[3]->p[2].uv.u = u + 48;
        quad[3]->p[2].uv.v = v;
        quad[3]->p[1].uv.u = u + 64;
        val = v_step;
        quad[3]->p[3].uv.u = u + 48;
        quad[3]->p[1].uv.v = val + v;
        quad[3]->p[3].uv.v = v_step + v;
        val = next->edge.rgbc;
        width = v_step;
        quad[0]->p[0].rgbc.rgbc = val;
        quad[0]->p[1].rgbc.rgbc = cur->edge.rgbc;
        quad[0]->p[2].rgbc.rgbc = next->center.rgbc;
        val = cur->center.rgbc;
        quad[0]->p[0].xy = edges[0];
        quad[0]->p[3].rgbc.rgbc = val;
        quad[0]->p[1].xy = prev_edges[0];
        quad[0]->p[2].xy = line_proj.scr[1];
        quad[0]->p[3].xy = line_proj.scr[0];
        quad[2]->p[0].rgbc.rgbc = 0;
        quad[2]->p[1].rgbc.rgbc = 0;
        quad[2]->p[2].rgbc.rgbc = next->edge.rgbc;
        val = cur->edge.rgbc;
        quad[2]->p[0].xy = edges[2];
        quad[2]->p[1].xy = prev_edges[2];
        quad[2]->p[2].xy = edges[0];
        quad[2]->p[3].xy = prev_edges[0];
        quad[2]->p[3].rgbc.rgbc = val;
        quad[1]->p[0].rgbc.rgbc = next->edge.rgbc;
        quad[1]->p[1].rgbc.rgbc = cur->edge.rgbc;
        quad[1]->p[2].rgbc.rgbc = next->center.rgbc;
        val = cur->center.rgbc;
        quad[1]->p[0].xy = edges[1];
        quad[1]->p[1].xy = prev_edges[1];
        quad[1]->p[3].rgbc.rgbc = val;
        quad[1]->p[2].xy = line_proj.scr[1];
        quad[1]->p[3].xy = line_proj.scr[0];
        quad[3]->p[0].rgbc.rgbc = 0;
        quad[3]->p[1].rgbc.rgbc = 0;
        quad[3]->p[2].rgbc.rgbc = next->edge.rgbc;
        quad[3]->p[3].rgbc.rgbc = cur->edge.rgbc;
        quad[3]->p[0].xy = edges[3];
        quad[3]->p[1].xy = prev_edges[3];
        quad[3]->p[2].xy = edges[1];
        quad[3]->p[3].xy = prev_edges[1];
        quad[3]->p[0].rgbc.code = 62;
        quad[2]->p[0].rgbc.code = 62;
        quad[1]->p[0].rgbc.code = 62;
        val = quad[0]->tag;
        quad[0]->p[0].rgbc.code = 62;
        tick = *ordering_table;
        tick = LinkPacket(quad_packet[0], ordering_table, val, tick);
        val = quad[2]->tag;
        tick = LinkPacket(quad_packet[2], ordering_table, val, tick);
        val = quad[1]->tag;
        tick = LinkPacket(quad_packet[1], ordering_table, val, tick);
        val = quad[3]->tag;
        start_strip = false;
        tick = LinkPacket(quad_packet[3], ordering_table, val, tick);
        phase -= width;
        prev_edges[0] = edges[0];
        prev_edges[2] = edges[2];
        prev_edges[1] = edges[1];
        val = apply_window->tag;
        LinkPacket(apply_packet, ordering_table, val, tick);
        val = EffectObject::GetDrawMode().tag;
        tick = EffectObject::GetDrawMode().code[0];
        draw_state = (DR_TPAGE *)state_packet;
        draw_state->tag = val;
        draw_state->code[0] = tick;
        InsertPacket(state_packet, ordering_table);
        prev_edges[3] = edges[3];
    AdvanceSegment:
        ++next;
        ++cur;
        ++idx;
        line_start = line_end;
    }
}
