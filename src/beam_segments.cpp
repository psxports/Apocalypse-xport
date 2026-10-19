#include <new>
#include <stdlib.h>

#include "scene_lifecycle.h"
#include "game_diagnostics.h"
#include "beam_segments.h"
#include "model_projection.h"
#include "effect_lifecycle.h"

struct BeamSegments::Projection
{
    uint32 scr;
    uint32 n[2];
    sint32 depth;
    uint32 width;
    uint32 clipped;
};

BeamSegments::BeamSegments(uint32 count, uint32 mode) : RenderableEffect(), mode(mode), point_count(count), points(0), projected(0)
{
    FUNCTION_MARKER(0x800260FC, "SLUS_003.73");
    if (count < 2 || count > 0x7FFFFFFF || count > 0xFFFFFFFF / 36 || count > (size_t)-1 / sizeof(*points) || count > (size_t)-1 / sizeof(*projected))
    {
        GameDiagnostic("Invalid beam point count %u\n", count);
        abort();
    }
    points = (BeamPoint *)malloc((size_t)count * sizeof(*points));
    projected = (Projection *)malloc((size_t)count * sizeof(*projected));
    if (!points || !projected)
    {
        GameDiagnostic("Beam geometry allocation failed\n");
        abort();
    }
    // Only RGB is consumed from secondary packet colors
    for (uint32 i = 0; i < count; ++i)
        points[i].rgbc.code = 0;
}

BeamSegments::~BeamSegments()
{
    free(projected);
    free(points);
}

uint32 BeamSegments::Destroy(uint32 reason)
{
    FUNCTION_MARKER(0x8002616C, "SLUS_003.73");
    free(projected);
    free(points);
    projected = 0;
    points = 0;
    uint32 result = RenderableEffect::Destroy(0);
    return reason & 1 ? ReleaseEffectStorage(this) : result;
}

uint32 BeamSegments::SetColors(uint32 r, uint32 g, uint32 b)
{
    FUNCTION_MARKER(0x800261CC, "SLUS_003.73");
    for (uint32 i = 0; i < point_count; ++i)
    {
        points[i].rgbc.r = r;
        points[i].rgbc.g = g;
        points[i].rgbc.b = b;
    }
    return 0;
}

uint32 BeamSegments::SetSize(uint32 size)
{
    FUNCTION_MARKER(0x8002622C, "SLUS_003.73");
    for (uint32 i = 0; i < point_count; ++i)
        points[i].size = size;
    return 0;
}

static sint32 DivideBeamScalar(uint32 numerator, uint32 denominator)
{
    if (!denominator)
        return (sint32)numerator < 0 ? 1 : -1;
    if (numerator == 0x80000000 && denominator == (uint32)-1)
        return (sint32)numerator;
    return (sint32)numerator / (sint32)denominator;
}

void BeamSegments::CalculateNormal(const Projection *p, sint32 *x, sint32 *y, uint32 min_len)
{
    FUNCTION_MARKER(0x80026370, "SLUS_003.73");
    if (!p[0].clipped && !p[1].clipped)
    {
        uint32 x0 = (sint16)p[0].scr;
        uint32 y0 = (sint16)(p[0].scr >> 16);
        uint32 x1 = (sint16)p[1].scr;
        uint32 y1 = (sint16)(p[1].scr >> 16);
        uint32 dx = DivideBeamScalar((x1 - x0) * 320, 512);
        uint32 dy = y1 - y0;
        *x = (sint32)dy;
        *y = (sint32)(0 - dx);
        if ((sint32)dx < 0)
            dx = 0 - dx;
        if ((sint32)dy < 0)
            dy = 0 - dy;
        uint32 norm = (sint32)dy < (sint32)dx ? dx + (uint32)((sint32)dy / 2) : dy + (uint32)((sint32)dx / 2);
        if ((sint32)norm >= (sint32)min_len)
        {
            *x = DivideBeamScalar((uint32)*x << 6, norm);
            *y = DivideBeamScalar((uint32)*y << 6, norm);
            *x = DivideBeamScalar((uint32)*x << 9, 320);
            return;
        }
    }
    *y = 0;
    *x = 0;
}

uint32 BeamSegments::Render()
{
    FUNCTION_MARKER(0x80026488, "SLUS_003.73");
    const ModelProjectionConfig &proj_cfg = GetModelProjectionConfig();
    for (uint32 i = 0; i < point_count; ++i)
    {
        Projection *p = projected + i;
        p->depth = ProjectWorldPoint(points[i].pos.words, &p->scr);
        p->clipped = p->depth < (sint32)proj_cfg.near_depth || (sint32)proj_cfg.max_distance < p->depth;
        // Clipped normals are zero, so this width contributes to no packet
        p->width = 0;
        if (!p->clipped)
            p->width = DivideBeamScalar(400 * points[i].size, p->depth);
    }
    sint32 nx, ny;
    CalculateNormal(projected, &nx, &ny, 2);
    projected[0].n[0] = nx;
    projected[0].n[1] = ny;
    for (uint32 i = 1; i < point_count - 1; ++i)
    {
        sint32 x, y;
        CalculateNormal(projected + i, &x, &y, 2);
        projected[i].n[0] = (sint32)((uint32)x + (uint32)nx) / 2;
        projected[i].n[1] = (sint32)((uint32)y + (uint32)ny) / 2;
        nx = x;
        ny = y;
    }
    projected[point_count - 1].n[0] = nx;
    projected[point_count - 1].n[1] = ny;
    uint32 dx = projected[0].n[0];
    uint32 dy = projected[0].n[1];
    uint32 width = projected[0].width;
    sint32 offset_x = (sint32)(dx * width) >> 6;
    sint32 offset_y = (sint32)(dy * width) >> 6;
    uint32 x = (sint32)(sint16)projected[0].scr;
    sint32 y = (sint16)(projected[0].scr >> 16);
    uint32 prev_plus = (((uint32)y + (uint32)offset_y) << 16) | ((x + (uint32)offset_x) & 65535);
    uint32 prev_minus = (((uint32)y - (uint32)offset_y) << 16) | ((x - (uint32)offset_x) & 65535);
    for (uint32 i = 1; i < point_count; ++i)
    {
        dx = projected[i].n[0];
        dy = projected[i].n[1];
        if ((dx | dy) && (projected[i - 1].n[0] | projected[i - 1].n[1]))
        {
            width = projected[i].width;
            offset_x = (sint32)(dx * width) >> 6;
            offset_y = (sint32)(dy * width) >> 6;
            x = (sint32)(sint16)projected[i].scr;
            y = (sint16)(projected[i].scr >> 16);
            uint32 depth = projected[i].depth;
            uint32 *bucket = scene_render.cur->ot + (((depth - depth_bias) & 0x3FFC) >> 2);
            uint32 cur_plus = (((uint32)y + (uint32)offset_y) << 16) | ((x + (uint32)offset_x) & 65535);
            uint32 cur_minus = (((uint32)y - (uint32)offset_y) << 16) | ((x - (uint32)offset_x) & 65535);
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
            draw_word = GetEffectDrawMode().tag;
            draw_param = GetEffectDrawMode().code[0];
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
                history_poly->p[1].xy.xy = points[i - 1].history[1];
                history_poly->p[0].xy.xy = points[i].history[1];
                history_poly->p[3].xy.xy = projected[i - 1].scr;
                history_poly->p[2].xy.xy = projected[i].scr;
                depth = history_poly->tag;
                draw_word = *bucket;
                history_poly->tag = (depth & 0xFF000000) | (draw_word & 0xFFFFFF);
                *bucket = ((*bucket & 0xFF000000) | gpu_packet_link((uintptr_t)history_packet));
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
            plus_poly->p[0].xy.xy = cur_plus;
            plus_poly->p[1].xy.xy = prev_plus;
            plus_poly->p[2].rgbc.rgbc = color;
            plus_poly->p[2].xy.xy = projected[i].scr;
            plus_poly->p[3].xy.xy = projected[i - 1].scr;
            plus_poly->tag = ((header & 0xFF000000) | (*bucket & 0xFFFFFF));
            *bucket = ((*bucket & 0xFF000000) | gpu_packet_link((uintptr_t)packet));
            minus_poly->p[1].rgbc.rgbc = 0;
            minus_poly->p[3].rgbc.rgbc = points[i - 1].rgbc.rgbc;
            minus_color = points[i].rgbc.rgbc;
            minus_poly->p[0].xy.xy = cur_minus;
            minus_poly->p[1].xy.xy = prev_minus;
            minus_poly->p[2].rgbc.rgbc = minus_color;
            minus_poly->p[2].xy.xy = projected[i].scr;
            prev_minus = cur_minus;
            minus_poly->p[3].xy.xy = projected[i - 1].scr;
            depth = minus_poly->tag;
            draw_word = *bucket;
            minus_poly->tag = (depth & 0xFF000000) | (draw_word & 0xFFFFFF);
            tag = ((*bucket & 0xFF000000) | gpu_packet_link((uintptr_t)minus_packet));
            *bucket = tag;
            state_header = state->tag;
            prev_plus = cur_plus;
            state->tag = ((state_header & 0xFF000000) | (tag & 0xFFFFFF));
            *bucket = ((*bucket & 0xFF000000) | gpu_packet_link((uintptr_t)state_packet));
        }
    }
    uint32 result = mode;
    if (mode)
    {
        for (uint32 i = 0; i < point_count; ++i)
        {
            if (mode == 1)
            {
                points[i].history[0] = projected[i].scr;
                points[i].history[1] = projected[i].scr;
            }
            else
            {
                points[i].history[1] = points[i].history[0];
                points[i].history[0] = projected[i].scr;
            }
        }
        result = mode + 1;
        mode = result;
    }
    return result;
}
