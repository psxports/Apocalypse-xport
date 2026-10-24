#include <stdlib.h>

#include "scene_lifecycle.h"
#include "game_diagnostics.h"
#include "beam_segments.h"
#include "model_projection.h"
#include "global.h"
#include "effect_lifecycle.h"
#include "effect_object.h"
#include "renderable_effect.h"
#include "hd.h"

struct BeamSegments::Projection
{
    XY scr;
    sint32 nx;
    sint32 ny;
    sint32 depth;
    sint32 width;
    bool clipped;
};

BeamSegments::BeamSegments(uint32 count, uint32 mode) : RenderableEffect(), point_count(count), points(0), mode(mode), proj(0), hd_points(0), hd_history_valid(false), hd_history(0)
{
    FUNCTION_MARKER(0x800260FC, "SLUS_003.73");
    if (count < 2 || count > 0x7FFFFFFF || count > 0xFFFFFFFF / 36 || count > (size_t)-1 / sizeof(*points) || count > (size_t)-1 / sizeof(*proj) || count > (size_t)-1 / sizeof(*hd_points))
    {
        GameDiagnostic(("Invalid beam point count %u\n", count));
        abort();
    }
    points = (BeamPoint *)malloc(count * sizeof(*points));
    proj = (Projection *)malloc(count * sizeof(*proj));
    hd_points = (HD_BEAM_POINT *)calloc(count, sizeof(*hd_points));
    if (!points || !proj || !hd_points)
    {
        GameDiagnostic(("Beam geometry allocation failed\n"));
        abort();
    }
    // Only RGB is consumed from secondary packet colors
    for (uint32 i = 0; i < count; ++i)
        points[i].rgbc.code = 0;
}

BeamSegments::~BeamSegments()
{
    hd_beam_free(hd_history);
    free(hd_points);
    free(proj);
    free(points);
}

uint32 BeamSegments::Destroy(uint32 reason)
{
    FUNCTION_MARKER(0x8002616C, "SLUS_003.73");
    free(hd_points);
    hd_points = 0;
    free(proj);
    free(points);
    proj = 0;
    points = 0;
    uint32 result = RenderableEffect::Destroy(0);
    return reason & 1 ? ReleaseStorage() : result;
}

uint32 BeamSegments::SetColors(uint8 r, uint8 g, uint8 b)
{
    FUNCTION_MARKER(0x800261CC, "SLUS_003.73");
    for (sint32 i = 0; i < point_count; ++i)
    {
        BeamPoint *point = points + i;
        point->rgbc.r = r;
        point->rgbc.g = g;
        point->rgbc.b = b;
    }
    return 0;
}

uint32 BeamSegments::SetSize(uint16 size)
{
    FUNCTION_MARKER(0x8002622C, "SLUS_003.73");
    for (sint32 i = 0; i < point_count; ++i)
        points[i].size = size;
    return 0;
}

void BeamSegments::CalculateNormal(const Projection *p, sint32 *x, sint32 *y, sint32 min_len)
{
    FUNCTION_MARKER(0x80026370, "SLUS_003.73");
    if (!p[0].clipped && !p[1].clipped)
    {
        sint16 x0 = p[0].scr.x;
        sint16 y0 = p[0].scr.y;
        sint16 x1 = p[1].scr.x;
        sint16 y1 = p[1].scr.y;
        sint32 dx = DivideSignedWord((x1 - x0) * 320, 512);
        sint32 dy = y1 - y0;
        *x = dy;
        *y = -dx;
        if (dx < 0)
            dx = -dx;
        if (dy < 0)
            dy = -dy;
        sint32 norm = dy < dx ? dx + dy / 2 : dy + dx / 2;
        if (norm >= min_len)
        {
            // Signed screen differences keep Q6 products within int32
            *x = DivideSignedWord(*x * (1 << 6), norm);
            *y = DivideSignedWord(*y * (1 << 6), norm);
            *x = DivideSignedWord(*x * (1 << 9), 320);
            return;
        }
    }
    *y = 0;
    *x = 0;
}

bool BeamSegments::CaptureHd(bool logic)
{
    for (sint32 index = 0; index < point_count; ++index)
    {
        HD_BEAM_POINT &point = hd_points[index];
        point.point.vx = points[index].pos.x / 4096.0f;
        point.point.vy = points[index].pos.y / 4096.0f;
        point.point.vz = points[index].pos.z / 4096.0f;
        point.size = points[index].size;
        point.color = points[index].rgbc.rgbc;
    }
    const ModelProjectionConfig &cfg = GetModelProjConfig();
    uint16 page = EffectObject::GetDrawMode().code[0] & 0x1FF;
    return (logic ? hd_beam_tick(&hd_history, hd_points, point_count, mode, hd_history_valid, depth_bias, page) : hd_beam_capture(&hd_history, hd_points, point_count, mode, hd_history_valid, depth_bias, page, cfg.near_depth, cfg.max_distance)) != 0;
}

bool BeamSegments::CaptureHdTick()
{
    return CaptureHd(true);
}

uint32 BeamSegments::Render()
{
    FUNCTION_MARKER(0x80026488, "SLUS_003.73");
    const ModelProjectionConfig &proj_cfg = GetModelProjConfig();
    if (!hd_ui_collecting() || mode)
    {
        for (sint32 i = 0; i < point_count; ++i)
        {
            Projection *p = proj + i;
            p->depth = ProjectWorldPoint(&points[i].pos, &p->scr);
            p->clipped = p->depth < (sint32)proj_cfg.near_depth || (sint32)proj_cfg.max_distance < p->depth;
            // Clipped normals are zero, so this width contributes to no packet
            p->width = 0;
            if (!p->clipped)
                p->width = DivideSignedWord(400 * points[i].size, p->depth);
        }
    }
    if (hd_ui_collecting())
    {
        if (!CaptureHd(false))
        {
            GameDiagnostic(("Cannot capture HD beam segments\n"));
            abort();
        }
        hd_history_valid = true;
    }
    else
    {
        hd_history_valid = false;
        hd_beam_reset(hd_history);
        sint32 nx, ny;
        CalculateNormal(proj, &nx, &ny, 2);
        proj[0].nx = nx;
        proj[0].ny = ny;
        for (sint32 i = 1; i < point_count - 1; ++i)
        {
            sint32 x, y;
            CalculateNormal(proj + i, &x, &y, 2);
            proj[i].nx = (x + nx) / 2;
            proj[i].ny = (y + ny) / 2;
            nx = x;
            ny = y;
        }
        proj[point_count - 1].nx = nx;
        proj[point_count - 1].ny = ny;
        sint32 dx = proj[0].nx;
        sint32 dy = proj[0].ny;
        sint32 width = proj[0].width;
        sint32 product_x = (uint32)dx * width;
        sint32 product_y = (uint32)dy * width;
        sint32 offset_x = product_x >> 6;
        sint32 offset_y = product_y >> 6;
        XY prev_plus = proj[0].scr;
        XY prev_minus = proj[0].scr;
        prev_plus.x += offset_x;
        prev_plus.y += offset_y;
        prev_minus.x -= offset_x;
        prev_minus.y -= offset_y;
        for (sint32 i = 1; i < point_count; ++i)
        {
            dx = proj[i].nx;
            dy = proj[i].ny;
            if ((dx | dy) && (proj[i - 1].nx | proj[i - 1].ny))
            {
                width = proj[i].width;
                product_x = (uint32)dx * width;
                product_y = (uint32)dy * width;
                offset_x = product_x >> 6;
                offset_y = product_y >> 6;
                uint32 depth = proj[i].depth;
                uint32 *bucket = scene_render.cur->ot + (((depth - depth_bias) & 0x3FFC) >> 2);
                XY cur_plus = proj[i].scr;
                XY cur_minus = proj[i].scr;
                cur_plus.x += offset_x;
                cur_plus.y += offset_y;
                cur_minus.x -= offset_x;
                cur_minus.y -= offset_y;
                uint8 *packet, *limit, *minus_packet, *history_packet, *state_packet;
                uint32 draw_word, draw_param;
                sint32 history_mode;
                uint32 header, color, minus_color, tag, state_header;
                packet = scene_render.cursor;
                limit = scene_render.limit;
                if (limit - packet < (sint32)(2 * sizeof(POLY_G4) + sizeof(DR_TPAGE)))
                    break;
                minus_packet = packet + sizeof(POLY_G4);
                state_packet = packet + 2 * sizeof(POLY_G4);
                history_packet = state_packet + sizeof(DR_TPAGE);
                draw_word = EffectObject::GetDrawMode().tag;
                draw_param = EffectObject::GetDrawMode().code[0];
                DR_TPAGE *state = (DR_TPAGE *)state_packet;
                state->tag = draw_word;
                state->code[0] = draw_param;
                history_mode = mode;
                scene_render.cursor = history_packet;
                if (history_mode >= 3)
                {
                    if (limit - history_packet < (sint32)sizeof(POLY_G4))
                        break;
                    scene_render.cursor = history_packet + sizeof(POLY_G4);
                    POLY_G4 *history_poly = (POLY_G4 *)history_packet;
                    history_poly->tag = 0;
                    setPolyG4(history_poly);
                    history_poly->p[0].rgbc.rgbc = 0x3A000000;
                    history_poly->p[1].rgbc.rgbc = 0;
                    history_poly->p[3].rgbc.rgbc = points[i - 1].rgbc.rgbc;
                    history_poly->p[2].rgbc.rgbc = points[i].rgbc.rgbc;
                    history_poly->p[1].xy = points[i - 1].history[1];
                    history_poly->p[0].xy = points[i].history[1];
                    history_poly->p[3].xy = proj[i - 1].scr;
                    history_poly->p[2].xy = proj[i].scr;
                    depth = history_poly->tag;
                    draw_word = *bucket;
                    history_poly->tag = (depth & 0xFF000000) | (draw_word & 0xFFFFFF);
                    *bucket = (*bucket & 0xFF000000) | gpu_packet_link((uintptr_t)history_packet);
                }
                POLY_G4 *minus_poly = (POLY_G4 *)minus_packet;
                POLY_G4 *plus_poly = (POLY_G4 *)packet;
                minus_poly->tag = 0;
                setPolyG4(minus_poly);
                plus_poly->tag = 0;
                setPolyG4(plus_poly);
                minus_poly->p[0].rgbc.rgbc = 0x3A000000;
                plus_poly->p[0].rgbc.rgbc = 0x3A000000;
                plus_poly->p[1].rgbc.rgbc = 0;
                draw_word = points[i - 1].rgbc.rgbc;
                header = plus_poly->tag;
                plus_poly->p[3].rgbc.rgbc = draw_word;
                color = points[i].rgbc.rgbc;
                plus_poly->p[0].xy = cur_plus;
                plus_poly->p[1].xy = prev_plus;
                plus_poly->p[2].rgbc.rgbc = color;
                plus_poly->p[2].xy = proj[i].scr;
                plus_poly->p[3].xy = proj[i - 1].scr;
                plus_poly->tag = (header & 0xFF000000) | (*bucket & 0xFFFFFF);
                *bucket = (*bucket & 0xFF000000) | gpu_packet_link((uintptr_t)packet);
                minus_poly->p[1].rgbc.rgbc = 0;
                minus_poly->p[3].rgbc.rgbc = points[i - 1].rgbc.rgbc;
                minus_color = points[i].rgbc.rgbc;
                minus_poly->p[0].xy = cur_minus;
                minus_poly->p[1].xy = prev_minus;
                minus_poly->p[2].rgbc.rgbc = minus_color;
                minus_poly->p[2].xy = proj[i].scr;
                prev_minus = cur_minus;
                minus_poly->p[3].xy = proj[i - 1].scr;
                depth = minus_poly->tag;
                draw_word = *bucket;
                minus_poly->tag = (depth & 0xFF000000) | (draw_word & 0xFFFFFF);
                tag = (*bucket & 0xFF000000) | gpu_packet_link((uintptr_t)minus_packet);
                *bucket = tag;
                state_header = state->tag;
                prev_plus = cur_plus;
                state->tag = (state_header & 0xFF000000) | (tag & 0xFFFFFF);
                *bucket = (*bucket & 0xFF000000) | gpu_packet_link((uintptr_t)state_packet);
            }
        }
    }
    uint32 result = mode;
    if (mode)
    {
        for (sint32 i = 0; i < point_count; ++i)
        {
            if (mode == 1)
            {
                points[i].history[0] = proj[i].scr;
                points[i].history[1] = proj[i].scr;
            }
            else
            {
                points[i].history[1] = points[i].history[0];
                points[i].history[0] = proj[i].scr;
            }
        }
        result = mode + 1;
        mode = result;
    }
    return result;
}
