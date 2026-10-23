#include <stdlib.h>
#include <string.h>

#include "psx.h"
#include "scene_lifecycle.h"
#include "game_tick.h"
#include "model_render_view.h"
#include "model_projection.h"
#include "global.h"
#include "hud_text.h"
#include "model_bank.h"
#include "game_diagnostics.h"
#include "texture_resources.h"

const HudTextAnimation hud_pickup_animation[3] = {
    {10, 1, 0, {0, 512}, {{255, 255, 255}, {255, 255, 255}}, {0, 0}, {{256, 90}, {256, 90}}, 0, 1},
    {10, 1, 0, {512, 512}, {{255, 255, 255}, {255, 255, 255}}, {0, 0}, {{256, 90}, {256, 90}}, 0, 1},
    {15, 1, 0, {512, 0}, {{255, 255, 255}, {255, 255, 255}}, {0, 0}, {{256, 90}, {256, 90}}, 0, 0},
};

const HudTextAnimation hud_extra_life_animation[3] = {
    {10, 1, 0, {0, 256}, {{255, 255, 255}, {255, 255, 255}}, {0, 0}, {{256, 90}, {256, 90}}, 0, 1},
    {10, 1, 0, {256, 256}, {{255, 255, 255}, {255, 255, 255}}, {0, 0}, {{256, 90}, {256, 90}}, 0, 1},
    {30, 1, 0, {256, 256}, {{255, 255, 255}, {255, 255, 255}}, {0, 0}, {{256, 90}, {256, 90}}, 5, 0},
};

const HudTextAnimation hud_weapon_animation[3] = {
    {10, 1, 0, {0, 256}, {{255, 255, 255}, {255, 255, 255}}, {3072, 4096}, {{256, 90}, {256, 90}}, 0, 1},
    {20, 1, 0, {256, 256}, {{255, 255, 255}, {255, 255, 255}}, {0, 0}, {{256, 90}, {256, 90}}, 0, 1},
    {15, 1, 0, {256, 0}, {{255, 255, 255}, {255, 255, 255}}, {0, 1024}, {{256, 90}, {256, 90}}, 0, 0},
};

const HudTextAnimation hud_pulse_laser_animation[3] = {
    {10, 1, 0, {0, 256}, {{0, 255, 0}, {0, 255, 0}}, {3072, 4096}, {{256, 90}, {256, 90}}, 0, 1},
    {20, 1, 0, {256, 256}, {{0, 255, 0}, {0, 255, 0}}, {0, 0}, {{256, 90}, {256, 90}}, 0, 1},
    {15, 1, 0, {256, 0}, {{0, 255, 0}, {0, 255, 0}}, {0, 1024}, {{256, 90}, {256, 90}}, 0, 0},
};

const HudTextAnimation hud_flamethrower_animation[3] = {
    {10, 1, 0, {0, 256}, {{255, 70, 0}, {255, 70, 0}}, {3072, 4096}, {{256, 90}, {256, 90}}, 0, 1},
    {20, 1, 0, {256, 256}, {{255, 70, 0}, {255, 70, 0}}, {0, 0}, {{256, 90}, {256, 90}}, 0, 1},
    {15, 1, 0, {256, 0}, {{255, 70, 0}, {255, 70, 0}}, {0, 1024}, {{256, 90}, {256, 90}}, 0, 0},
};

namespace HudMessage
{
const char checkpoint[] = "Checkpoint";
const char health20[] = "20% Health";
const char health50[] = "50% Health";
const char health100[] = "100% Health";
const char smart_bomb[] = "Smart Bomb";
const char extra_life[] = "Extra Life";
const char flamethrower[] = "Flamethrower";
const char pulse_laser[] = "Pulse Laser";
const char particle_beam[] = "Particle Beam";
const char rip_laser[] = "Rip Laser";
const char homing_missiles[] = "Homing Missiles";
const char rockets[] = "rockets";
const char grenades[] = "grenades";
} // namespace HudMessage

HudTextStyle hud_text_style = {0, {0x2C808080}, 0, 0, {0x0000C8FF}};
HudTextGlow hud_text_glow = {{0},
                             {0},
                             {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                              // Original curve index40 reads the adjacent extra-life animation header
                              65546},
                             32,
                             0};

void ResetHudTextState()
{
    AnimatedHudText::ClearAll();
    ResetHudTextQueue();
    const HudTextStyle style = {0, {0x2C808080}, 0, 0, {0x0000C8FF}};
    hud_text_style = style;
    memset(&hud_text_glow, 0, sizeof(hud_text_glow));
    hud_text_glow.curve[40] = 65546;
    hud_text_glow.timer = 32;
}

static const uint8 small_glyphs[128] = {28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 255, 29, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 27, 30, 26, 28, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 28, 28, 28, 28, 28, 28, 28, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 28, 28, 28, 28, 28, 28, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 28, 28, 28, 28, 28};

static const uint8 large_glyphs[256] = {42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 255, 44, 42, 42, 42, 2,  42, 48, 42, 42, 42, 1,  42, 50, 49, 42, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 42, 42, 42, 42, 42, 42, 6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 42, 42, 42, 42, 0,  42, 6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 42, 42, 42, 42, 42,
                                        42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42,  42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 20, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 43, 42, 42, 42, 3, 42, 42, 42, 46, 45, 51, 42, 52, 42, 42, 42, 42, 42, 42, 42, 47, 42, 4,  42, 42, 42, 42, 42, 5,  42, 42, 42, 43, 42, 42, 42, 3, 42, 42, 42, 46, 45, 51, 42, 52, 42, 42, 42, 42, 42, 42, 42, 47, 42, 4,  42, 42, 42, 42, 42, 5,  42, 42, 42};

// Original 100 slots end at the glyph table and include the sentinel
static const uint32 hud_text_capacity = 99;

struct HudTextRow
{
    uint16 x;
    uint16 y;
    RGBC color;
    bool projected;
    uint8 enlarged;
    Vector3 pos;
    const char *text;
    char *owned_text;
    char digits[12];
};

struct HudTextQueue
{
    HudTextQueue() : count(0)
    {
    }

    ~HudTextQueue()
    {
        Clear();
    }

    void Clear()
    {
        for (uint32 idx = 0; idx < count; ++idx)
            free(rows[idx].owned_text);
        count = 0;
    }

    HudTextRow rows[hud_text_capacity];
    uint32 count;
};

static HudTextQueue hud_text;

static HudTextRow *AddHudTextRow(uint16 x, uint16 y, uint8 r, uint8 g, uint8 b, uint8 flags)
{
    if (hud_text.count >= hud_text_capacity)
    {
        GameDiagnostic(("HUD text row capacity exceeded\n"));
        abort();
    }
    HudTextRow *row = hud_text.rows + hud_text.count++;
    memset(row, 0, sizeof(*row));
    row->x = x;
    row->y = y;
    row->enlarged = flags;
    // Original 0x800FF6E4 is the constant textured-quad command
    row->color.r = r;
    row->color.g = g;
    row->color.b = b;
    row->color.code = 0x2C;
    return row;
}

static const TextureGroup *small_font;
static const TextureGroup *large_font;
static const TextureResource *glow_tex;

const TextureGroup *GetHudFont()
{
    return large_font;
}

const TextureResource *GetHudGlowTex()
{
    return glow_tex;
}

void ForgetHudTex(const TextureGroup *grp)
{
    if (small_font == grp)
        small_font = 0;
    if (large_font == grp)
        large_font = 0;
}

void ForgetHudTex(const TextureResource *tex)
{
    if (glow_tex == tex)
        glow_tex = 0;
}

void InitializeHudFonts()
{
    FUNCTION_MARKER(0x8006A834, "SLUS_003.73");
    small_font = TextureGroup::Find("Fontsmll");
    large_font = TextureGroup::Find("Font");
}

void ResetHudTextQueue()
{
    FUNCTION_MARKER(0x8006A868, "SLUS_003.73");
    hud_text.Clear();
}

uint32 QueueHudText(uint16 x, uint16 y, const char *text, uint8 r, uint8 g, uint8 b, uint8 flags)
{
    FUNCTION_MARKER(0x8006A890, "SLUS_003.73");
    HudTextRow *row = AddHudTextRow(x, y, r, g, b, flags);
    size_t len = strlen(text);
    row->owned_text = (char *)malloc(len + 1);
    if (!row->owned_text)
    {
        GameDiagnostic(("Cannot allocate queued HUD text\n"));
        abort();
    }
    memcpy(row->owned_text, text, len + 1);
    row->text = row->owned_text;
    return 1;
}

uint32 QueueHudNumber(uint16 x, uint16 y, sint32 val, uint8 r, uint8 g, uint8 b, uint8 flags)
{
    FUNCTION_MARKER(0x8006A8FC, "SLUS_003.73");
    HudTextRow *row = AddHudTextRow(x, y, r, g, b, flags);
    FormatNumber(val, row->digits);
    row->text = row->digits;
    return 1;
}

uint32 DrawHudTextQueue()
{
    const ModelProjectionConfig &proj_cfg = GetModelProjConfig();
    Vector3 camera, delta, scaled;
    uint32 rotation_words[5], command, x, y, scr;
    sint32 depth;
    const char *text;
    uint8 character, glyph;
    uint8 *packet;
    uint8 u_right, u_left, v_bottom, v_top;
    sint32 width, height;
    uint8 byte;
    uint32 top, bottom, right;
    FUNCTION_MARKER(0x8006AAB8, "SLUS_003.73");
    // Pack the nine rotation fields for the GTE control bank
    rotation_words[0] = PackModelRotationWord(GetModelRenderView().projection, 0);
    rotation_words[1] = PackModelRotationWord(GetModelRenderView().projection, 1);
    xport_gte_write_control(0, rotation_words[0]);
    xport_gte_write_control(1, rotation_words[1]);
    rotation_words[2] = PackModelRotationWord(GetModelRenderView().projection, 2);
    rotation_words[3] = PackModelRotationWord(GetModelRenderView().projection, 3);
    rotation_words[4] = PackModelRotationWord(GetModelRenderView().projection, 4);
    xport_gte_write_control(2, rotation_words[2]);
    xport_gte_write_control(3, rotation_words[3]);
    xport_gte_write_control(4, rotation_words[4]);
    xport_gte_write_control(5, 0);
    xport_gte_write_control(6, 0);
    xport_gte_write_control(7, 0);
    camera.x = (uint32)GetModelRenderView().pos.x << 12;
    camera.y = (uint32)GetModelRenderView().pos.y << 12;
    camera.z = (uint32)GetModelRenderView().pos.z << 12;
    for (uint32 idx = 0; idx < hud_text.count; ++idx)
    {
        const HudTextRow *row = hud_text.rows + idx;
        text = row->text;
        // Depth contributes to the ordering table only for projected rows
        depth = 0;
        if (row->projected)
        {
            delta.Subtract(row->pos, camera);
            scaled.ShiftRight(delta, 12);
            xport_gte_write_data(0, ((uint32)scaled.x & 65535) | ((uint32)scaled.y << 16));
            xport_gte_write_data(1, (uint32)scaled.z);
            xport_gte_execute(0x180001);
            command = row->color.rgbc;
            depth = xport_gte_read_data(27);
            if (depth < (sint32)proj_cfg.near_depth)
                continue;
            if ((sint32)proj_cfg.max_distance < depth)
                continue;
            scr = xport_gte_read_data(14);
            x = scr & 65535;
            y = scr >> 16;
        }
        else
        {
            x = row->x;
            y = row->y;
            command = row->color.rgbc;
        }
        for (;;)
        {
            character = *text;
            if (!character)
                break;
            glyph = small_glyphs[character & 127];
            ++text;
            if (character == 95)
                glyph = small_glyphs[45];
            if (glyph == 255)
            {
                x += 5;
                continue;
            }
            const TextureFrame &frame = TextureGroup::GetFrame(small_font, glyph);
            const TextureResource &tex = *frame.tex;
            if (frame.w < 4)
                ++x;
            packet = scene_render.cursor;
            if (scene_render.limit - packet < (sint32)sizeof(POLY_FT4))
                return 0x09000000;
            scene_render.cursor = packet + sizeof(POLY_FT4);
            POLY_FT4 *poly = (POLY_FT4 *)packet;
            poly->tag = 0;
            setPolyFT4(poly);
            poly->rgbc.rgbc = command;
            poly->p[0].uv = tex.uv[0];
            poly->p[0].attr = tex.clut;
            poly->p[1].uv = tex.uv[1];
            poly->p[1].attr = tex.tpage;
            u_right = poly->p[1].uv.u;
            poly->p[2].uv = tex.uv[2];
            // Original third word includes the fourth UV in its attribute half
            poly->p[2].attr = tex.uv[3].uv;
            u_left = poly->p[0].uv.u;
            v_bottom = poly->p[2].uv.v;
            width = u_right - u_left;
            v_top = poly->p[0].uv.v;
            poly->p[3].uv = tex.uv[3];
            height = v_bottom - v_top;
            if (row->enlarged)
            {
                width = (width * 55) >> 5;
                height = (height * 55) >> 5;
                byte = poly->p[3].uv.u;
                poly->p[1].uv.u = u_right - 1;
                v_bottom = poly->p[2].uv.v;
                poly->p[3].uv.u = byte - 1;
                byte = poly->p[3].uv.v;
                poly->p[2].uv.v = v_bottom - 1;
                poly->p[3].uv.v = byte - 1;
            }
            right = x + width;
            top = y;
            if (glyph == 30)
                top += character == 95 ? 5 : 2;
            bottom = top + height;
            setXY4(poly, x, top, right, top, x, bottom, right, bottom);
            setaddr(poly, getaddr(scene_render.cur->ot + (row->projected ? (depth & 0x3FFC) >> 2 : 0)));
            setaddr(scene_render.cur->ot + (row->projected ? (depth & 0x3FFC) >> 2 : 0), gpu_packet_link((uintptr_t)poly));
            x += 3 + width;
            if (width < 4)
                ++x;
        }
    }
    return 1;
}

sint32 GetHudGlyphIdx(uint8 character)
{
    uint8 idx;
    FUNCTION_MARKER(0x8001A784, "SLUS_003.73");
    idx = large_glyphs[character];
    return idx == 255 ? -1 : idx;
}

void SetHudTextAlignment(uint8 alignment)
{
    FUNCTION_MARKER(0x8001A7B0, "SLUS_003.73");
    hud_text_style.align = alignment;
}

void SetHudTextScale(uint16 scale)
{
    FUNCTION_MARKER(0x8001A7BC, "SLUS_003.73");
    hud_text_style.scale = scale;
}

void SetHudTextDepth(uint16 depth)
{
    FUNCTION_MARKER(0x8001A7C8, "SLUS_003.73");
    hud_text_style.depth = depth;
}

uint32 SetHudTextColor(uint8 r, uint8 g, uint8 b, bool semi_transparent)
{
    FUNCTION_MARKER(0x8001A7D4, "SLUS_003.73");
    hud_text_style.color.r = r;
    hud_text_style.color.g = g;
    hud_text_style.color.b = b;
    hud_text_style.color.code = semi_transparent ? 0x2E : 0x2C;
    return hud_text_style.color.rgbc;
}

void LoadHudTextGlowResource()
{
    FUNCTION_MARKER(0x8001A760, "SLUS_003.73");
    glow_tex = TextureResource::Find(0x38B9E52B);
}

uint32 InitializeHudTextGlowCurve()
{
    uint32 idx, val;
    FUNCTION_MARKER(0x8001A858, "SLUS_003.73");
    for (idx = 0; idx < 40; ++idx)
    {
        val = RandomRange(10);
        hud_text_glow.amplitude[idx] = val + 10;
        hud_text_glow.curve[idx] = 40;
        val = RandomRange(30);
        hud_text_glow.rate[idx] = val + 30;
    }
    return 0;
}

uint32 UpdateHudTextGlowCurve()
{
    uint32 idx, tick, phase;
    uint8 rate;
    sint32 sine, product;
    FUNCTION_MARKER(0x8001A8E8, "SLUS_003.73");
    for (idx = 0; idx < 40; ++idx)
    {
        rate = hud_text_glow.rate[idx];
        tick = game_clock.vblank_frame;
        phase = (rate * tick) & 4095;
        sine = rsin(phase);
        product = hud_text_glow.amplitude[idx] * sine;
        // Q12 displacement truncates toward zero for both signs
        hud_text_glow.curve[idx] = product / ONE + 40;
    }
    // Original updates40 entries while the packet owner cycles through41 indices
    return 0;
}

uint32 MeasureStyledHudText(const char *text)
{
    uint32 cursor = 0, width = 0, count = 0;
    uint8 character;
    sint32 idx;
    FUNCTION_MARKER(0x8001A97C, "SLUS_003.73");
    for (;;)
    {
        character = text[cursor];
        if (!character)
            break;
        idx = GetHudGlyphIdx(character);
        if (idx < 0)
            width += 10;
        else
        {
            const TextureFrame &frame = TextureGroup::GetFrame(large_font, idx);
            ++count;
            width += frame.w + frame.ox;
        }
        cursor += 1;
    }
    return ((sint32)(width * hud_text_style.scale) >> 8) + count * 3;
}

static void SetHudTextShadowColor(uint8 r, uint8 g, uint8 b)
{
    FUNCTION_MARKER(0x8001A838, "SLUS_003.73");
    hud_text_style.shadow.r = r;
    hud_text_style.shadow.g = g;
    hud_text_style.shadow.b = b;
}

void ResetHudTextShadowColor()
{
    FUNCTION_MARKER(0x8001A810, "SLUS_003.73");
    SetHudTextShadowColor(255, 200, 0);
}
