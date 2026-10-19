#include <stdlib.h>

#include "scene_lifecycle.h"
#include "game_random.h"
#include "game_diagnostics.h"
#include "hud_text.h"

uint32 DrawStyledHudText(uint32 x, uint32 y, const char *text, uint32 filter, uint32 color, uint32 scale)
{
    sint32 glyph_count;
    sint32 measure_glyph;
    sint32 next_width;
    sint32 align_offset;
    uint8 character;
    sint32 glyph_index;
    uint8 *glyph_address;
    uint8 *black_shadow_address;
    uint8 *color_shadow_address;
    uint8 *glow_address;
    uint8 *fade_address;
    uint32 glyph_color;
    uint32 glyph_uv0;
    uint32 glyph_uv1;
    uint32 glyph_uv2;
    uint16 glyph_uv3;
    uint8 glow_left_u;
    uint8 glow_base_v;
    uint16 glow_tpage;
    uint8 glow_edge_v;
    uint8 glow_right_u;
    sint32 edge_u1;
    sint32 edge_u0;
    sint32 fade_halfwidth_uv;
    sint32 fade_center_uv;
    uint32 glyph_scale;
    sint16 glyph_bottom;
    uint32 curve_index;
    sint32 glow_y0;
    uint32 next_curve_index;
    sint32 glow_y1;
    sint32 scaled_glow_height;
    sint32 glow_x0;
    sint32 glow_x1;
    sint32 glow_top;
    sint16 glow_right_y;
    sint32 fade_halfwidth;
    sint32 fade_center;
    uint32 text_depth;
    sint32 alternate;
    sint32 spark_index;
    sint32 spark_x0;
    sint32 spark_y0;
    sint32 spark_x1;
    sint32 spark_y1;
    uint8 *spark_address;
    sint16 spark_offset_y;
    sint16 spark_offset_x;
    uint32 spark_depth;
    uint32 curve_cursor;
    uint32 advanced_curve_index;
    const char *measure_cursor;
    const char *cursor;
    sint32 raw_width;
    sint32 total_width;
    sint32 pen_x;
    uint32 pen_y;
    sint32 baseline_offset;
    sint32 glyph_screen_width;
    sint32 glyph_uv_width;
    sint32 glyph_screen_height;
    sint32 glyph_uv_height;
    uint32 glow_color;
    const TextureFrame *glyph_frame = 0;
    const TextureResource *glyph_tex = 0;
    const TextureResource *glow = GetHudGlowTexture();
    uint32 glyph_width, glyph_offset, current_character, texture_bottom, texture_top, texture_left, texture_right;
    uint8 *packet_end, *packet_limit;
    uint32 coord_x, coord_y, coord_right, coord_bottom, packet_word, head_word;
    uint32 *head_address;
    FUNCTION_MARKER(0x8001AA28, "SLUS_003.73");
    // Packet links retain the original ordered header and ordering-table accesses
    if (glow)
        glow_color = color | (color << 16) | (color << 8);
    else
        glow_color = 0;
    glyph_count = 0;
    curve_cursor = 0;
    measure_cursor = text;
    raw_width = 0;
    while ((current_character = (uint8)*measure_cursor) != 0)
    {
        measure_glyph = GetHudGlyphIndex(current_character);
        if (measure_glyph < 0)
        {
            next_width = (uint32)raw_width + 10;
        }
        else
        {
            glyph_count = (uint32)glyph_count + 1;
            glyph_frame = &GetTextureFrame(GetHudFont(), measure_glyph);
            glyph_width = glyph_frame->w;
            glyph_offset = (uint32)(sint32)glyph_frame->ox;
            next_width = (uint32)raw_width + glyph_width + glyph_offset;
        }
        raw_width = next_width;
        ++measure_cursor;
    }

    total_width = (sint32)((uint32)((sint32)((uint32)raw_width * hud_text_style.scale) >> 8) + 3 * (uint32)glyph_count);
    current_character = hud_text_style.align;
    if (current_character == 0)
    {
        glyph_offset = 0 - (uint32)total_width;
        align_offset = (sint32)(glyph_offset + (glyph_offset >> 31)) >> 1;
    }
    else if (current_character == 1)
        align_offset = 0;
    else if (current_character == 2)
        align_offset = (sint32)(0 - (uint32)total_width);
    else
    {
        // TODO Original invalid alignment uses an undefined incoming saved register
        GameDiagnostic("Undefined original text alignment\n");
        abort();
    }
    pen_x = x + (uint32)align_offset;

    pen_y = y;
    cursor = text;
    baseline_offset = (9 * hud_text_style.scale) >> 8;
    while (1)
    {
        character = (uint8)*cursor;
        if (!character)
            return total_width;
        ++cursor;
        glyph_index = GetHudGlyphIndex(character);
        if (glyph_index < 0)
        {
            pen_x = (sint32)((uint32)pen_x + ((5 * (uint32)hud_text_style.scale) >> 7));
        }
        else
        {
            glyph_address = scene_render.cursor;
            glyph_frame = &GetTextureFrame(GetHudFont(), glyph_index);
            packet_limit = scene_render.limit;
            glyph_tex = glyph_frame->tex;
            if (packet_limit - glyph_address < 212)
                return total_width;
            black_shadow_address = glyph_address + 40;
            color_shadow_address = glyph_address + 80;
            glow_address = glyph_address + 120;
            fade_address = glyph_address + 160;
            packet_end = glyph_address + 212;
            scene_render.cursor = packet_end;
            POLY_FT4 *glyph_poly = (POLY_FT4 *)glyph_address;
            POLY_FT4 *black_shadow_poly = (POLY_FT4 *)black_shadow_address;
            POLY_FT4 *color_shadow_poly = (POLY_FT4 *)color_shadow_address;
            POLY_FT4 *glow_poly = (POLY_FT4 *)glow_address;
            POLY_GT4 *fade_poly = (POLY_GT4 *)fade_address;
            setPolyFT4(glyph_poly);
            glyph_color = hud_text_style.color.rgbc;
            glyph_poly->tag = 0x09000000;
            glyph_poly->rgbc.rgbc = glyph_color;
            glyph_uv0 = PackTextureUv(*glyph_tex, 0);
            glyph_uv1 = PackTextureUv(*glyph_tex, 1);
            glyph_uv2 = PackTextureUv(*glyph_tex, 2);
            glyph_uv3 = glyph_tex->uv[3].uv;
            glyph_poly->p[0].uv.uv = glyph_uv0;
            glyph_poly->p[0].attr = glyph_uv0 >> 16;
            glyph_poly->p[1].uv.uv = glyph_uv1;
            glyph_poly->p[1].attr = glyph_uv1 >> 16;
            glyph_poly->p[2].uv.uv = glyph_uv2;
            glyph_poly->p[2].attr = glyph_uv2 >> 16;
            glyph_poly->p[3].uv.uv = glyph_uv3;
            if (hud_text_style.scale >= 0x200 || filter)
            {
                texture_left = glyph_poly->p[1].uv.u;
                texture_bottom = glyph_poly->p[2].uv.v;
                glyph_poly->p[1].uv.u = texture_left - 1;
                texture_right = glyph_poly->p[3].uv.u;
                glyph_poly->p[2].uv.v = texture_bottom - 1;
                texture_top = glyph_poly->p[3].uv.v;
                glyph_poly->p[3].uv.u = texture_right - 1;
                glyph_poly->p[3].uv.v = texture_top - 1;
            }
            if (glow_color)
            {
                setPolyFT4(glow_poly);
                setPolyGT4(fade_poly);
                glow_poly->rgbc.rgbc = (glow_color | 0x2E000000);
                glow_poly->tag = 0x09000000;
                uint32 packed_uv0 = PackTextureUv(*glow, 0);
                glow_poly->p[0].uv.uv = packed_uv0;
                glow_poly->p[0].attr = packed_uv0 >> 16;
                uint32 packed_uv1 = PackTextureUv(*glow, 1);
                glow_poly->p[1].uv.uv = packed_uv1;
                glow_poly->p[1].attr = packed_uv1 >> 16;
                texture_bottom = glow->uv[2].v;
                texture_top = glow->uv[0].v;
                texture_left = glow->uv[0].u;
                glow_left_u = texture_left;
                glow_base_v = texture_bottom - (uint32)(((sint32)texture_bottom - (sint32)texture_top) >> 3);
                glow_poly->p[2].uv.u = texture_left;
                glow_poly->p[2].uv.v = glow_base_v;
                fade_poly->p[0].rgbc.rgbc = (glow_color | 0x3E000000);
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
                fade_halfwidth_uv = ((3 * (edge_u1 - edge_u0)) >> 4);
                fade_center_uv = ((edge_u0 + edge_u1) >> 1);
                fade_poly->p[2].uv.u = (fade_center_uv - fade_halfwidth_uv);
                glow_edge_v = glow->uv[2].v;
                fade_poly->p[3].uv.u = (fade_center_uv + fade_halfwidth_uv);
                fade_poly->p[2].uv.v = glow_edge_v;
                fade_poly->p[3].uv.v = glow->uv[3].v;
                texture_top = fade_poly->p[2].uv.v;
                texture_bottom = fade_poly->p[3].uv.v;
                fade_poly->p[2].uv.v = texture_top - 1;
                fade_poly->p[3].uv.v = texture_bottom - 1;
            }
            glyph_scale = hud_text_style.scale;
            glyph_offset = (uint32)(sint32)glyph_frame->ox;
            glyph_poly->p[0].xy.x = (uint32)pen_x + (uint32)((sint32)(glyph_offset * glyph_scale) >> 8);
            glyph_offset = (uint32)(sint32)glyph_frame->oy;
            glyph_poly->p[0].xy.y = pen_y + (uint32)((sint32)(glyph_offset * glyph_scale) >> 8) - (uint32)baseline_offset;
            glyph_width = glyph_frame->w;
            coord_y = (uint16)glyph_poly->p[0].xy.y;
            coord_x = (uint16)glyph_poly->p[0].xy.x;
            glyph_poly->p[1].xy.y = coord_y;
            glyph_poly->p[2].xy.x = coord_x;
            glyph_poly->p[1].xy.x = coord_x + ((glyph_width * glyph_scale) >> 8);
            glyph_width = glyph_frame->h;
            coord_y = (uint16)glyph_poly->p[0].xy.y;
            glyph_poly->p[2].xy.y = coord_y + ((glyph_width * glyph_scale) >> 8);
            coord_right = (uint16)glyph_poly->p[1].xy.x;
            coord_bottom = (uint16)glyph_poly->p[2].xy.y;
            glyph_poly->p[3].xy.x = coord_right;
            glyph_poly->p[3].xy.y = coord_bottom;
            glyph_bottom = (sint16)coord_bottom;
            if (glow_color)
            {
                curve_index = curve_cursor;
                advanced_curve_index = curve_cursor + 1;
                glow_x0 = glyph_poly->p[0].xy.x;
                glow_y0 = glyph_poly->p[0].xy.y;
                glow_y1 = glyph_poly->p[1].xy.y;
                packet_word = hud_text_glow.curve[curve_index];
                glow_x1 = glyph_poly->p[3].xy.x;
                glow_y0 = (sint32)((uint32)glow_y0 - packet_word);
                next_curve_index = advanced_curve_index < 41 ? advanced_curve_index : 0;
                packet_word = hud_text_glow.curve[next_curve_index];
                glow_y1 = (sint32)((uint32)glow_y1 - packet_word);
                curve_cursor = next_curve_index + 1;
                if (curve_cursor >= 41)
                    curve_cursor = 0;
                glow_top = (sint32)((uint32)glyph_bottom - (uint32)((sint32)(((uint32)glyph_bottom - (uint32)glow_y0) * scale) >> 8));
                scaled_glow_height = ((uint32)glyph_bottom - (uint32)glow_y1) * scale;
                glow_x0 -= 10;
                glow_x1 += 10;
                glow_poly->p[0].xy.x = glow_x0;
                glow_poly->p[1].xy.x = glow_x1;
                glow_poly->p[2].xy.x = glow_x0;
                glow_poly->p[3].xy.x = glow_x1;
                glow_poly->p[0].xy.y = glow_top;
                glow_poly->p[2].xy.y = (uint32)glyph_bottom - (uint32)((sint32)((uint32)glyph_bottom - (uint32)glow_top) >> 3);
                glow_top = (sint32)((uint32)glyph_bottom - (uint32)(scaled_glow_height >> 8));
                glow_poly->p[1].xy.y = glow_top;
                glow_poly->p[3].xy.y = (uint32)glyph_bottom - (uint32)((sint32)((uint32)glyph_bottom - (uint32)glow_top) >> 3);
                fade_poly->p[0].xy.x = glow_x0;
                packet_word = (uint16)glow_poly->p[2].xy.y;
                fade_poly->p[0].xy.y = packet_word;
                packet_word = (uint16)glow_poly->p[3].xy.x;
                fade_poly->p[1].xy.x = packet_word;
                glow_right_y = (uint16)glow_poly->p[3].xy.y;
                fade_poly->p[2].xy.y = (uint32)glyph_bottom + 2;
                fade_poly->p[3].xy.y = (uint32)glyph_bottom + 2;
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
                black_shadow_poly->p[vertex].xy.x = (uint16)black_shadow_poly->p[vertex].xy.x + 1;
                black_shadow_poly->p[vertex].xy.y = (uint16)black_shadow_poly->p[vertex].xy.y + 1;
            }
            *color_shadow_poly = *glyph_poly;
            setRGB0(color_shadow_poly, hud_text_style.shadow.r, hud_text_style.shadow.g, hud_text_style.shadow.b);
            for (uint32 vertex = 0; vertex < 4; ++vertex)
            {
                color_shadow_poly->p[vertex].xy.x = (uint16)color_shadow_poly->p[vertex].xy.x - 2;
                color_shadow_poly->p[vertex].xy.y = (uint16)color_shadow_poly->p[vertex].xy.y - 1;
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
                head_word = (head_word & 0xFF000000) | gpu_packet_link((uintptr_t)glow_address);
                *head_address = head_word;
                packet_word = fade_poly->tag;
                fade_poly->tag = (packet_word & 0xFF000000) | (head_word & 0xFFFFFF);
                head_word = *head_address;
                *head_address = (head_word & 0xFF000000) | gpu_packet_link((uintptr_t)fade_address);
            }
            text_depth = hud_text_style.depth;
            head_address = scene_render.cur->ot + text_depth;
            packet_word = glyph_poly->tag;
            alternate = hud_text_glow.alternate;
            head_word = *head_address;
            glyph_poly->tag = (packet_word & 0xFF000000) | (head_word & 0xFFFFFF);
            head_word = *head_address;
            head_word = (head_word & 0xFF000000) | gpu_packet_link((uintptr_t)glyph_address);
            *head_address = head_word;
            packet_word = color_shadow_poly->tag;
            color_shadow_poly->tag = (packet_word & 0xFF000000) | (head_word & 0xFFFFFF);
            head_word = *head_address;
            head_word = (head_word & 0xFF000000) | gpu_packet_link((uintptr_t)color_shadow_address);
            *head_address = head_word;
            packet_word = black_shadow_poly->tag;
            black_shadow_poly->tag = (packet_word & 0xFF000000) | (head_word & 0xFFFFFF);
            head_word = *head_address;
            *head_address = (head_word & 0xFF000000) | gpu_packet_link((uintptr_t)black_shadow_address);
            if (alternate)
            {
            DrawSparks:
                spark_index = 0;

                coord_right = glyph_poly->p[1].xy.x;
                coord_x = glyph_poly->p[0].xy.x;
                texture_right = glyph_poly->p[1].uv.u;
                texture_left = glyph_poly->p[0].uv.u;
                glyph_screen_width = (sint32)(coord_right - coord_x);
                glyph_uv_width = (sint32)(texture_right - texture_left);
                coord_bottom = glyph_poly->p[2].xy.y;
                coord_y = glyph_poly->p[0].xy.y;
                texture_bottom = glyph_poly->p[2].uv.v;
                texture_top = glyph_poly->p[0].uv.v;
                glyph_screen_height = (sint32)(coord_bottom - coord_y);
                glyph_uv_height = (sint32)(texture_bottom - texture_top);
                while (spark_index < 3)
                {
                    spark_x0 = RandomRange(32);
                    spark_y0 = RandomRange(128);
                    spark_x1 = ((spark_x0 + RandomRange((192 - spark_x0))) + 64);
                    spark_y1 = ((spark_y0 + RandomRange((192 - spark_y0))) + 64);
                    spark_address = scene_render.cursor;
                    if (scene_render.limit - spark_address < 40)
                        return total_width;
                    scene_render.cursor = spark_address + 40;
                    POLY_FT4 *spark_poly = (POLY_FT4 *)spark_address;
                    spark_offset_x = (RandomRange(6) - 3);
                    spark_offset_y = RandomRange(6);
                    spark_poly->tag = glyph_poly->tag;
                    spark_poly->rgbc.rgbc = glyph_poly->rgbc.rgbc;
                    spark_offset_y -= 3;
                    spark_poly->p[0].attr = glyph_poly->p[0].attr;
                    spark_poly->p[1].attr = glyph_poly->p[1].attr;
                    coord_x = (uint16)glyph_poly->p[0].xy.x;
                    spark_poly->p[0].xy.x = coord_x + (uint32)((sint32)((uint32)spark_x0 * (uint32)glyph_screen_width) >> 8) + (uint32)spark_offset_x;
                    coord_y = (uint16)glyph_poly->p[0].xy.y;
                    coord_x = (uint16)spark_poly->p[0].xy.x;
                    spark_poly->p[0].xy.y = coord_y + (uint32)((sint32)((uint32)spark_y0 * (uint32)glyph_screen_height) >> 8) + (uint32)spark_offset_y;
                    coord_right = (uint16)glyph_poly->p[0].xy.x;
                    spark_poly->p[2].xy.x = coord_x;
                    coord_y = (uint16)spark_poly->p[0].xy.y;
                    spark_poly->p[1].xy.x = coord_right + (uint32)((sint32)((uint32)spark_x1 * (uint32)glyph_screen_width) >> 8) + (uint32)spark_offset_x;
                    spark_poly->p[1].xy.y = coord_y;
                    coord_bottom = (uint16)glyph_poly->p[0].xy.y;
                    coord_right = (uint16)spark_poly->p[1].xy.x;
                    coord_bottom += (uint32)((sint32)((uint32)spark_y1 * (uint32)glyph_screen_height) >> 8) + (uint32)spark_offset_y;
                    ++spark_index;
                    spark_poly->p[2].xy.y = coord_bottom;
                    spark_poly->p[3].xy.x = coord_right;
                    spark_poly->p[3].xy.y = coord_bottom;
                    texture_left = glyph_poly->p[0].uv.u;
                    spark_poly->p[0].uv.u = texture_left + (uint32)((sint32)((uint32)spark_x0 * (uint32)glyph_uv_width) >> 8);
                    texture_top = glyph_poly->p[0].uv.v;
                    spark_poly->p[0].uv.v = texture_top + (uint32)((sint32)((uint32)spark_y0 * (uint32)glyph_uv_height) >> 8);
                    texture_left = glyph_poly->p[0].uv.u;
                    texture_right = spark_poly->p[0].uv.u;
                    texture_top = spark_poly->p[0].uv.v;
                    spark_poly->p[1].uv.u = texture_left + (uint32)((sint32)((uint32)spark_x1 * (uint32)glyph_uv_width) >> 8);
                    spark_poly->p[1].uv.v = texture_top;
                    spark_poly->p[2].uv.u = texture_right;
                    texture_top = glyph_poly->p[0].uv.v;
                    packet_word = spark_poly->rgbc.code;
                    texture_right = spark_poly->p[1].uv.u;
                    spark_poly->rgbc.code = packet_word | 2;
                    spark_poly->p[2].uv.v = texture_top + (uint32)((sint32)((uint32)spark_y1 * (uint32)glyph_uv_height) >> 8);
                    packet_word = spark_poly->p[1].attr;
                    texture_bottom = spark_poly->p[2].uv.v;
                    spark_poly->p[3].uv.u = texture_right;
                    spark_poly->p[3].uv.v = texture_bottom;
                    spark_depth = hud_text_style.depth;
                    spark_poly->p[1].attr = packet_word | 0x60;
                    head_address = scene_render.cur->ot + spark_depth;
                    packet_word = spark_poly->tag;
                    head_word = *head_address;
                    spark_poly->tag = (packet_word & 0xFF000000) | (head_word & 0xFFFFFF);
                    head_word = *head_address;
                    *head_address = (head_word & 0xFF000000) | gpu_packet_link((uintptr_t)spark_address);
                }
            }
            glyph_width = glyph_frame->w;
            glyph_offset = (uint32)(sint32)glyph_frame->ox;
            current_character = hud_text_style.scale;
            pen_x = (sint32)((uint32)pen_x + 3 + (uint32)((sint32)((glyph_width + glyph_offset) * current_character) >> 8));
        }
    }
}
