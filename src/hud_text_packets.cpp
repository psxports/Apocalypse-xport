#include <stdlib.h>

#include "texture_resources.h"
#include "scene_lifecycle.h"
#include "global.h"
#include "game_diagnostics.h"
#include "hud_text.h"
#include "hud_sprites.h"

static bool hd_text_pair;
static HD_UI_TEXT_HISTORY *hd_text_history;
static uint32 hd_text_previous_color, hd_text_previous_scale;
static float hd_text_layout_scale;

static int CaptureHudGlyph(uint8 character, HD_UI_IMAGE *image)
{
    sint32 index = GetHudGlyphIdx(character);
    if (index < 0)
        return 0;
    CaptureHudImage(TextureGroup::GetFrame(GetHudFont(), index), *image);
    return 1;
}

uint32 DrawStyledHudText(uint32 x, uint32 y, const char *text, uint32 filter, uint32 color, uint32 scale)
{
    uint32 glyph_count;
    sint32 measure_glyph;
    sint32 align_offset;
    uint8 character;
    sint32 glyph_idx;
    uint8 *glyph_address;
    uint32 glyph_color;
    uint8 glow_left_u;
    uint8 glow_base_v;
    uint16 glow_tpage;
    uint8 glow_edge_v;
    uint8 glow_right_u;
    sint32 edge_u1;
    sint32 edge_u0;
    sint32 fade_halfwidth_uv;
    sint32 fade_center_uv;
    uint16 glyph_scale;
    sint16 glyph_bottom;
    uint32 curve_idx;
    sint32 glow_y0;
    uint32 next_curve_idx;
    sint32 glow_y1;
    sint32 scaled_glow_height[2];
    sint32 glow_displacement;
    sint32 glow_x0;
    sint32 glow_x1;
    sint32 glow_top;
    sint16 glow_right_y;
    sint32 fade_halfwidth;
    sint32 fade_center;
    uint32 text_depth;
    uint32 alternate;
    sint32 spark_idx;
    sint32 spark_x0;
    sint32 spark_y0;
    sint32 spark_x1;
    sint32 spark_y1;
    uint8 *spark_address;
    sint16 spark_offset_y;
    sint16 spark_offset_x;
    uint32 spark_depth;
    uint32 curve_cursor;
    uint32 advanced_curve_idx;
    const char *measure_cursor;
    const char *cursor;
    uint32 raw_width;
    sint32 total_width;
    uint32 pen_x;
    uint32 pen_y;
    sint32 baseline_offset;
    sint32 glyph_scr_width;
    sint32 glyph_uv_width;
    sint32 glyph_scr_height;
    sint32 glyph_uv_height;
    uint32 glow_color;
    const TextureFrame *glyph_frame = 0;
    const TextureResource *glyph_tex = 0;
    const TextureResource *glow = GetHudGlowTex();
    uint8 glyph_width, cur_character;
    sint8 glyph_offset;
    sint32 tex_bottom, tex_top, tex_left, tex_right;
    const sint32 packet_bytes = 4 * sizeof(POLY_FT4) + sizeof(POLY_GT4);
    uint16 coord_x, coord_y, coord_right, coord_bottom;
    uint32 packet_word, head_word;
    uint32 *head_address;
    FUNCTION_MARKER(0x8001AA28, "SLUS_003.73");
    if (hd_ui_collecting())
    {
        if (hud_text_style.align > 2)
        {
            GameDiagnostic(("Undefined original text alignment\n"));
            abort();
        }
        HD_UI_TEXT_STYLE style;
        HD_UI_IMAGE glow_image;
        style.color = hud_text_style.color.rgbc & 0xFFFFFF;
        style.shadow = hud_text_style.shadow.rgbc & 0xFFFFFF;
        style.scale = hd_text_history ? hd_text_layout_scale : hud_text_style.scale;
        style.depth = hud_text_style.depth;
        style.alignment = hud_text_style.align;
        style.flags = (hud_text_style.color.code & 1 ? HD_RAW_TEXTURE : 0) | (hud_text_style.color.code & 2 ? HD_SEMITRANSPARENT : 0);
        style.curve = hud_text_glow.curve;
        style.alternate = hud_text_glow.alternate != 0;
        if (glow)
            CaptureHudTexture(*glow, glow_image);
        if (hd_text_history)
            return hd_ui_text_history(hd_text_history, (sint32)x, (sint32)y, text, &style, filter != 0, color, scale, glow ? &glow_image : 0, CaptureHudGlyph, RandomRange);
        if (hd_text_pair)
            return hd_ui_text_pair((sint32)x, (sint32)y, text, &style, &style, filter != 0, color, scale, hd_text_previous_color, hd_text_previous_scale, glow ? &glow_image : 0, CaptureHudGlyph, RandomRange);
        return hd_ui_text((sint32)x, (sint32)y, text, &style, filter != 0, color, scale, glow ? &glow_image : 0, CaptureHudGlyph, RandomRange);
    }
    // Packet links retain the original ordered header and ordering-table accesses
    if (glow)
        glow_color = color | (color << 16) | (color << 8);
    else
        glow_color = 0;
    glyph_count = 0;
    curve_cursor = 0;
    measure_cursor = text;
    raw_width = 0;
    while ((cur_character = *measure_cursor) != 0)
    {
        measure_glyph = GetHudGlyphIdx(cur_character);
        if (measure_glyph < 0)
        {
            raw_width += 10;
        }
        else
        {
            ++glyph_count;
            glyph_frame = &TextureGroup::GetFrame(GetHudFont(), measure_glyph);
            raw_width += glyph_frame->w + glyph_frame->ox;
        }
        ++measure_cursor;
    }

    total_width = ((sint32)(raw_width * hud_text_style.scale) >> 8) + 3 * glyph_count;
    cur_character = hud_text_style.align;
    if (cur_character == 0)
    {
        align_offset = -(uint32)total_width;
        align_offset /= 2;
    }
    else if (cur_character == 1)
        align_offset = 0;
    else if (cur_character == 2)
        align_offset = -(uint32)total_width;
    else
    {
        // TODO Original invalid alignment uses an undefined incoming saved register
        GameDiagnostic(("Undefined original text alignment\n"));
        abort();
    }
    pen_x = x + align_offset;

    pen_y = y;
    cursor = text;
    baseline_offset = (9 * hud_text_style.scale) >> 8;
    while (1)
    {
        character = *cursor;
        if (!character)
            return total_width;
        ++cursor;
        glyph_idx = GetHudGlyphIdx(character);
        if (glyph_idx < 0)
        {
            pen_x += (5 * hud_text_style.scale) >> 7;
        }
        else
        {
            glyph_address = scene_render.cursor;
            glyph_frame = &TextureGroup::GetFrame(GetHudFont(), glyph_idx);
            glyph_tex = glyph_frame->tex;
            if (scene_render.limit - glyph_address < packet_bytes)
                return total_width;
            scene_render.cursor = glyph_address + packet_bytes;
            POLY_FT4 *glyph_poly = (POLY_FT4 *)glyph_address;
            POLY_FT4 *black_shadow_poly = (POLY_FT4 *)(glyph_address + sizeof(POLY_FT4));
            POLY_FT4 *color_shadow_poly = (POLY_FT4 *)(glyph_address + 2 * sizeof(POLY_FT4));
            POLY_FT4 *glow_poly = (POLY_FT4 *)(glyph_address + 3 * sizeof(POLY_FT4));
            POLY_GT4 *fade_poly = (POLY_GT4 *)(glyph_address + 4 * sizeof(POLY_FT4));
            setPolyFT4(glyph_poly);
            glyph_color = hud_text_style.color.rgbc;
            glyph_poly->tag = 0x09000000;
            glyph_poly->rgbc.rgbc = glyph_color;
            glyph_poly->p[0].uv = glyph_tex->uv[0];
            glyph_poly->p[0].attr = glyph_tex->clut;
            glyph_poly->p[1].uv = glyph_tex->uv[1];
            glyph_poly->p[1].attr = glyph_tex->tpage;
            glyph_poly->p[2].uv = glyph_tex->uv[2];
            // Original third UV word carries the fourth UV in its attribute half
            glyph_poly->p[2].attr = glyph_tex->uv[3].uv;
            glyph_poly->p[3].uv = glyph_tex->uv[3];
            if (hud_text_style.scale >= 0x200 || filter)
            {
                tex_left = glyph_poly->p[1].uv.u;
                tex_bottom = glyph_poly->p[2].uv.v;
                glyph_poly->p[1].uv.u = tex_left - 1;
                tex_right = glyph_poly->p[3].uv.u;
                glyph_poly->p[2].uv.v = tex_bottom - 1;
                tex_top = glyph_poly->p[3].uv.v;
                glyph_poly->p[3].uv.u = tex_right - 1;
                glyph_poly->p[3].uv.v = tex_top - 1;
            }
            if (glow_color)
            {
                setPolyFT4(glow_poly);
                setPolyGT4(fade_poly);
                glow_poly->rgbc.rgbc = glow_color | 0x2E000000;
                glow_poly->tag = 0x09000000;
                glow_poly->p[0].uv = glow->uv[0];
                glow_poly->p[0].attr = glow->clut;
                glow_poly->p[1].uv = glow->uv[1];
                glow_poly->p[1].attr = glow->tpage;
                tex_bottom = glow->uv[2].v;
                tex_top = glow->uv[0].v;
                tex_left = glow->uv[0].u;
                glow_left_u = tex_left;
                glow_base_v = tex_bottom - ((tex_bottom - tex_top) >> 3);
                glow_poly->p[2].uv.u = tex_left;
                glow_poly->p[2].uv.v = glow_base_v;
                fade_poly->p[0].rgbc.rgbc = glow_color | 0x3E000000;
                fade_poly->p[1].rgbc.rgbc = glow_color;
                fade_poly->p[2].rgbc.rgbc = 0;
                fade_poly->p[3].rgbc.rgbc = 0;
                fade_poly->tag = 0x0C000000;
                fade_poly->p[0].attr = glow->clut;
                glow_tpage = glow->tpage;
                fade_poly->p[0].uv.u = glow_left_u;
                fade_poly->p[0].uv.v = glow_base_v;
                fade_poly->p[1].attr = glow_tpage;
                glow_right_u = glow->uv[1].u;
                glow_poly->p[3].uv.v = glow_base_v;
                glow_poly->p[3].uv.u = glow_right_u;
                fade_poly->p[1].uv.u = glow_right_u;
                fade_poly->p[1].uv.v = glow_base_v;
                edge_u1 = glow->uv[3].u;
                edge_u0 = glow->uv[2].u;
                fade_halfwidth_uv = (3 * (edge_u1 - edge_u0)) >> 4;
                fade_center_uv = (edge_u0 + edge_u1) >> 1;
                fade_poly->p[2].uv.u = fade_center_uv - fade_halfwidth_uv;
                glow_edge_v = glow->uv[2].v;
                fade_poly->p[3].uv.u = fade_center_uv + fade_halfwidth_uv;
                fade_poly->p[2].uv.v = glow_edge_v;
                fade_poly->p[3].uv.v = glow->uv[3].v;
                tex_top = fade_poly->p[2].uv.v;
                tex_bottom = fade_poly->p[3].uv.v;
                fade_poly->p[2].uv.v = tex_top - 1;
                fade_poly->p[3].uv.v = tex_bottom - 1;
            }
            glyph_scale = hud_text_style.scale;
            glyph_offset = glyph_frame->ox;
            glyph_poly->p[0].xy.x = pen_x + ((glyph_offset * glyph_scale) >> 8);
            glyph_offset = glyph_frame->oy;
            glyph_poly->p[0].xy.y = pen_y + ((glyph_offset * glyph_scale) >> 8) - baseline_offset;
            glyph_width = glyph_frame->w;
            coord_y = glyph_poly->p[0].xy.y;
            coord_x = glyph_poly->p[0].xy.x;
            glyph_poly->p[1].xy.y = coord_y;
            glyph_poly->p[2].xy.x = coord_x;
            glyph_poly->p[1].xy.x = coord_x + ((glyph_width * glyph_scale) >> 8);
            glyph_width = glyph_frame->h;
            coord_y = glyph_poly->p[0].xy.y;
            glyph_poly->p[2].xy.y = coord_y + ((glyph_width * glyph_scale) >> 8);
            coord_right = glyph_poly->p[1].xy.x;
            coord_bottom = glyph_poly->p[2].xy.y;
            glyph_poly->p[3].xy.x = coord_right;
            glyph_poly->p[3].xy.y = coord_bottom;
            glyph_bottom = coord_bottom;
            if (glow_color)
            {
                curve_idx = curve_cursor;
                advanced_curve_idx = curve_cursor + 1;
                glow_x0 = glyph_poly->p[0].xy.x;
                glow_y0 = glyph_poly->p[0].xy.y;
                glow_y1 = glyph_poly->p[1].xy.y;
                glow_displacement = hud_text_glow.curve[curve_idx];
                glow_x1 = glyph_poly->p[3].xy.x;
                glow_y0 -= glow_displacement;
                next_curve_idx = advanced_curve_idx < 41 ? advanced_curve_idx : 0;
                glow_displacement = hud_text_glow.curve[next_curve_idx];
                glow_y1 -= glow_displacement;
                curve_cursor = next_curve_idx + 1;
                if (curve_cursor >= 41)
                    curve_cursor = 0;
                // Keep the low-word scale products before the signed Q8 shifts
                scaled_glow_height[0] = (glyph_bottom - glow_y0) * scale;
                glow_top = glyph_bottom - (scaled_glow_height[0] >> 8);
                scaled_glow_height[1] = (glyph_bottom - glow_y1) * scale;
                glow_x0 -= 10;
                glow_x1 += 10;
                glow_poly->p[0].xy.x = glow_x0;
                glow_poly->p[1].xy.x = glow_x1;
                glow_poly->p[2].xy.x = glow_x0;
                glow_poly->p[3].xy.x = glow_x1;
                glow_poly->p[0].xy.y = glow_top;
                glow_poly->p[2].xy.y = glyph_bottom - ((glyph_bottom - glow_top) >> 3);
                glow_top = glyph_bottom - (scaled_glow_height[1] >> 8);
                glow_poly->p[1].xy.y = glow_top;
                glow_poly->p[3].xy.y = glyph_bottom - ((glyph_bottom - glow_top) >> 3);
                fade_poly->p[0].xy.x = glow_x0;
                fade_poly->p[0].xy.y = glow_poly->p[2].xy.y;
                fade_poly->p[1].xy.x = glow_poly->p[3].xy.x;
                glow_right_y = glow_poly->p[3].xy.y;
                fade_poly->p[2].xy.y = glyph_bottom + 2;
                fade_poly->p[3].xy.y = glyph_bottom + 2;
                fade_halfwidth = (3 * (glow_x1 - glow_x0)) >> 4;
                fade_center = (glow_x0 + glow_x1) >> 1;
                fade_poly->p[2].xy.x = fade_center - fade_halfwidth;
                fade_poly->p[3].xy.x = fade_center + fade_halfwidth;
                fade_poly->p[1].xy.y = glow_right_y;
            }
            // All three packets are disjoint and unlinked during preparation
            *black_shadow_poly = *glyph_poly;
            setRGB0(black_shadow_poly, 0, 0, 0);
            for (uint32 vertex = 0; vertex < 4; ++vertex)
            {
                black_shadow_poly->p[vertex].xy.x += 1;
                black_shadow_poly->p[vertex].xy.y += 1;
            }
            *color_shadow_poly = *glyph_poly;
            setRGB0(color_shadow_poly, hud_text_style.shadow.r, hud_text_style.shadow.g, hud_text_style.shadow.b);
            for (uint32 vertex = 0; vertex < 4; ++vertex)
            {
                color_shadow_poly->p[vertex].xy.x -= 2;
                color_shadow_poly->p[vertex].xy.y -= 1;
            }
            if (hud_text_glow.alternate != 0)
                goto DrawSparks;
            if (glow_color)
            {
                packet_word = glow_poly->p[1].attr;
                text_depth = hud_text_style.depth;
                glow_poly->p[1].attr = (packet_word & 0xFF9F) | 0x20;
                packet_word = fade_poly->p[1].attr;
                fade_poly->p[1].attr = (packet_word & 0xFF9F) | 0x20;
                head_address = scene_render.cur->ot + text_depth;
                packet_word = glow_poly->tag;
                head_word = *head_address;
                glow_poly->tag = (packet_word & 0xFF000000) | (head_word & 0xFFFFFF);
                head_word = *head_address;
                head_word = (head_word & 0xFF000000) | gpu_packet_link((uintptr_t)glow_poly);
                *head_address = head_word;
                packet_word = fade_poly->tag;
                fade_poly->tag = (packet_word & 0xFF000000) | (head_word & 0xFFFFFF);
                head_word = *head_address;
                *head_address = (head_word & 0xFF000000) | gpu_packet_link((uintptr_t)fade_poly);
            }
            text_depth = hud_text_style.depth;
            head_address = scene_render.cur->ot + text_depth;
            packet_word = glyph_poly->tag;
            alternate = hud_text_glow.alternate;
            head_word = *head_address;
            glyph_poly->tag = (packet_word & 0xFF000000) | (head_word & 0xFFFFFF);
            head_word = *head_address;
            head_word = (head_word & 0xFF000000) | gpu_packet_link((uintptr_t)glyph_poly);
            *head_address = head_word;
            packet_word = color_shadow_poly->tag;
            color_shadow_poly->tag = (packet_word & 0xFF000000) | (head_word & 0xFFFFFF);
            head_word = *head_address;
            head_word = (head_word & 0xFF000000) | gpu_packet_link((uintptr_t)color_shadow_poly);
            *head_address = head_word;
            packet_word = black_shadow_poly->tag;
            black_shadow_poly->tag = (packet_word & 0xFF000000) | (head_word & 0xFFFFFF);
            head_word = *head_address;
            *head_address = (head_word & 0xFF000000) | gpu_packet_link((uintptr_t)black_shadow_poly);
            if (alternate)
            {
            DrawSparks:
                spark_idx = 0;

                glyph_scr_width = glyph_poly->p[1].xy.x - glyph_poly->p[0].xy.x;
                glyph_uv_width = glyph_poly->p[1].uv.u - glyph_poly->p[0].uv.u;
                glyph_scr_height = glyph_poly->p[2].xy.y - glyph_poly->p[0].xy.y;
                glyph_uv_height = glyph_poly->p[2].uv.v - glyph_poly->p[0].uv.v;
                while (spark_idx < 3)
                {
                    spark_x0 = RandomRange(32);
                    spark_y0 = RandomRange(128);
                    spark_x1 = (spark_x0 + RandomRange(192 - spark_x0)) + 64;
                    spark_y1 = (spark_y0 + RandomRange(192 - spark_y0)) + 64;
                    spark_address = scene_render.cursor;
                    if (scene_render.limit - spark_address < (sint32)sizeof(POLY_FT4))
                        return total_width;
                    scene_render.cursor = spark_address + sizeof(POLY_FT4);
                    POLY_FT4 *spark_poly = (POLY_FT4 *)spark_address;
                    spark_offset_x = RandomRange(6) - 3;
                    spark_offset_y = RandomRange(6);
                    spark_poly->tag = glyph_poly->tag;
                    spark_poly->rgbc.rgbc = glyph_poly->rgbc.rgbc;
                    spark_offset_y -= 3;
                    spark_poly->p[0].attr = glyph_poly->p[0].attr;
                    spark_poly->p[1].attr = glyph_poly->p[1].attr;
                    coord_x = glyph_poly->p[0].xy.x;
                    spark_poly->p[0].xy.x = coord_x + ((spark_x0 * glyph_scr_width) >> 8) + spark_offset_x;
                    coord_y = glyph_poly->p[0].xy.y;
                    coord_x = spark_poly->p[0].xy.x;
                    spark_poly->p[0].xy.y = coord_y + ((spark_y0 * glyph_scr_height) >> 8) + spark_offset_y;
                    coord_right = glyph_poly->p[0].xy.x;
                    spark_poly->p[2].xy.x = coord_x;
                    coord_y = spark_poly->p[0].xy.y;
                    spark_poly->p[1].xy.x = coord_right + ((spark_x1 * glyph_scr_width) >> 8) + spark_offset_x;
                    spark_poly->p[1].xy.y = coord_y;
                    coord_bottom = glyph_poly->p[0].xy.y;
                    coord_right = spark_poly->p[1].xy.x;
                    coord_bottom += ((spark_y1 * glyph_scr_height) >> 8) + spark_offset_y;
                    ++spark_idx;
                    spark_poly->p[2].xy.y = coord_bottom;
                    spark_poly->p[3].xy.x = coord_right;
                    spark_poly->p[3].xy.y = coord_bottom;
                    tex_left = glyph_poly->p[0].uv.u;
                    spark_poly->p[0].uv.u = tex_left + ((spark_x0 * glyph_uv_width) >> 8);
                    tex_top = glyph_poly->p[0].uv.v;
                    spark_poly->p[0].uv.v = tex_top + ((spark_y0 * glyph_uv_height) >> 8);
                    tex_left = glyph_poly->p[0].uv.u;
                    tex_right = spark_poly->p[0].uv.u;
                    tex_top = spark_poly->p[0].uv.v;
                    spark_poly->p[1].uv.u = tex_left + ((spark_x1 * glyph_uv_width) >> 8);
                    spark_poly->p[1].uv.v = tex_top;
                    spark_poly->p[2].uv.u = tex_right;
                    tex_top = glyph_poly->p[0].uv.v;
                    packet_word = spark_poly->rgbc.code;
                    tex_right = spark_poly->p[1].uv.u;
                    spark_poly->rgbc.code = packet_word | 2;
                    spark_poly->p[2].uv.v = tex_top + ((spark_y1 * glyph_uv_height) >> 8);
                    packet_word = spark_poly->p[1].attr;
                    tex_bottom = spark_poly->p[2].uv.v;
                    spark_poly->p[3].uv.u = tex_right;
                    spark_poly->p[3].uv.v = tex_bottom;
                    spark_depth = hud_text_style.depth;
                    spark_poly->p[1].attr = packet_word | 0x60;
                    head_address = scene_render.cur->ot + spark_depth;
                    packet_word = spark_poly->tag;
                    head_word = *head_address;
                    spark_poly->tag = (packet_word & 0xFF000000) | (head_word & 0xFFFFFF);
                    head_word = *head_address;
                    *head_address = (head_word & 0xFF000000) | gpu_packet_link((uintptr_t)spark_poly);
                }
            }
            glyph_width = glyph_frame->w;
            glyph_offset = glyph_frame->ox;
            glyph_scale = hud_text_style.scale;
            pen_x += 3 + (((glyph_width + glyph_offset) * glyph_scale) >> 8);
        }
    }
}

uint32 DrawStyledHudTextPair(uint32 x, uint32 y, const char *text, uint32 filter, uint32 color, uint32 scale, uint32 previous_color, uint32 previous_scale)
{
    if (hd_text_pair || hd_text_history)
        abort();
    hd_text_pair = true;
    hd_text_previous_color = previous_color;
    hd_text_previous_scale = previous_scale;
    uint32 width = DrawStyledHudText(x, y, text, filter, color, scale);
    hd_text_pair = false;
    return width;
}

uint32 DrawStyledHudTextHistory(HD_UI_TEXT_HISTORY &history, uint32 x, uint32 y, const char *text, uint32 filter, uint32 color, uint32 scale, float text_scale)
{
    if (hd_text_pair || hd_text_history)
        abort();
    hd_text_history = &history;
    hd_text_layout_scale = text_scale;
    uint32 width = DrawStyledHudText(x, y, text, filter, color, scale);
    hd_text_history = 0;
    return width;
}
