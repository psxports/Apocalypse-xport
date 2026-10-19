#include <stdlib.h>
#include <string.h>

#include "geometry_commands.h"
#include "scene_lifecycle.h"
#include "game_tick.h"
#include "model_render_view.h"
#include "model_projection.h"
#include "game_random.h"
#include "hud_text.h"
#include "model_resources.h"
#include "vector_arithmetic.h"
#include "game_diagnostics.h"

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

struct AnimatedHudText
{
    const char *text;
    uint16 age;
    const HudTextAnimation *animation;
    AnimatedHudText *next;
    AnimatedHudText *prev;
};

static AnimatedHudText *animated_hud_text;
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
    ClearAnimatedHudText();
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
static const uint32 HUD_TEXT_CAPACITY = 99;

struct HudTextRow
{
    uint16 x;
    uint16 y;
    uint32 rgbc;
    uint8 projected;
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
        for (uint32 index = 0; index < count; ++index)
            free(rows[index].owned_text);
        count = 0;
    }

    HudTextRow rows[HUD_TEXT_CAPACITY];
    uint32 count;
};

static HudTextQueue hud_text;

static HudTextRow *AddHudTextRow(uint32 x, uint32 y, uint32 red, uint32 green, uint32 blue, uint32 flags)
{
    if (hud_text.count >= HUD_TEXT_CAPACITY)
    {
        GameDiagnostic("HUD text row capacity exceeded\n");
        abort();
    }
    HudTextRow *row = hud_text.rows + hud_text.count++;
    memset(row, 0, sizeof(*row));
    row->x = x;
    row->y = y;
    row->enlarged = flags;
    // Original 0x800FF6E4 is the constant textured-quad command
    row->rgbc = 0x2C000000 | ((blue & 255) << 16) | ((green & 255) << 8) | (red & 255);
    return row;
}

static void FormatHudNumber(uint32 value, char *text)
{
    uint32 emitted = 0, divisor = 1000000000, digit;
    FUNCTION_MARKER(0x8006A75C, "SLUS_003.73");
    if ((sint32)value < 0)
    {
        value = 0 - value;
        *text++ = '-';
    }
    // Preserve the signed comparison after negating the low 32-bit value
    while (divisor)
    {
        if ((sint32)value >= (sint32)divisor)
        {
            digit = (uint32)((sint32)value / (sint32)divisor);
            emitted = 1;
            *text++ = digit + '0';
            value -= digit * divisor;
        }
        else if (emitted || divisor == 1)
            *text++ = '0';
        divisor /= 10;
    }
    *text = 0;
}

static const TextureGroup *small_font;
static const TextureGroup *large_font;
static TextureResource *glow_tex;

const TextureGroup *GetHudFont()
{
    return large_font;
}

const TextureResource *GetHudGlowTexture()
{
    return glow_tex;
}

void ForgetHudTexture(const TextureGroup *grp)
{
    if (small_font == grp)
        small_font = 0;
    if (large_font == grp)
        large_font = 0;
}

void ForgetHudTexture(const TextureResource *tex)
{
    if (glow_tex == tex)
        glow_tex = 0;
}

void ClearHudTextureBindings()
{
    small_font = 0;
    large_font = 0;
    glow_tex = 0;
}

void InitializeHudFonts()
{
    FUNCTION_MARKER(0x8006A834, "SLUS_003.73");
    small_font = FindModelTextureGroup("Fontsmll");
    large_font = FindModelTextureGroup("Font");
}

void ResetHudTextQueue()
{
    FUNCTION_MARKER(0x8006A868, "SLUS_003.73");
    hud_text.Clear();
}

uint32 QueueHudText(uint32 x, uint32 y, const char *text, uint32 red, uint32 green, uint32 blue, uint32 flags)
{
    FUNCTION_MARKER(0x8006A890, "SLUS_003.73");
    HudTextRow *row = AddHudTextRow(x, y, red, green, blue, flags);
    size_t length = strlen(text);
    row->owned_text = (char *)malloc(length + 1);
    if (!row->owned_text)
    {
        GameDiagnostic("Cannot allocate queued HUD text\n");
        abort();
    }
    memcpy(row->owned_text, text, length + 1);
    row->text = row->owned_text;
    return 1;
}

uint32 QueueHudNumber(uint32 x, uint32 y, uint32 value, uint32 red, uint32 green, uint32 blue, uint32 flags)
{
    FUNCTION_MARKER(0x8006A8FC, "SLUS_003.73");
    HudTextRow *row = AddHudTextRow(x, y, red, green, blue, flags);
    FormatHudNumber(value, row->digits);
    row->text = row->digits;
    return 1;
}

uint32 DrawHudTextQueue()
{
    const ModelProjectionConfig &proj_cfg = GetModelProjectionConfig();
    uint32 shift;
    Vector3 camera, delta, scaled;
    uint32 rotation_words[5], projected, command, x, y, depth, screen;
    const char *text;
    uint32 character, glyph, metric_width;
    uint8 *packet, *next, *limit;
    uint32 u_right, u_left, v_bottom, v_top, width, height;
    uint32 byte, top, bottom, right;
    FUNCTION_MARKER(0x8006AAB8, "SLUS_003.73");
    // Packed GTE rotation words retain the final padding halfword
    rotation_words[0] = ReadModelRenderMatrixWord(GetModelRenderView().projection, 0);
    rotation_words[1] = ReadModelRenderMatrixWord(GetModelRenderView().projection, 1);
    xport_gte_write_control(0, rotation_words[0]);
    xport_gte_write_control(1, rotation_words[1]);
    rotation_words[2] = ReadModelRenderMatrixWord(GetModelRenderView().projection, 2);
    rotation_words[3] = ReadModelRenderMatrixWord(GetModelRenderView().projection, 3);
    rotation_words[4] = ReadModelRenderMatrixWord(GetModelRenderView().projection, 4);
    xport_gte_write_control(2, rotation_words[2]);
    xport_gte_write_control(3, rotation_words[3]);
    xport_gte_write_control(4, rotation_words[4]);
    xport_gte_write_control(5, 0);
    xport_gte_write_control(6, 0);
    xport_gte_write_control(7, 0);
    camera.words[0] = GetModelRenderView().pos.words[0] << 12;
    camera.words[1] = GetModelRenderView().pos.words[1] << 12;
    camera.words[2] = GetModelRenderView().pos.words[2] << 12;
    for (uint32 index = 0; index < hud_text.count; ++index)
    {
        const HudTextRow *row = hud_text.rows + index;
        projected = row->projected;
        text = row->text;
        // TODO Original depth is defined only when this row is projected
        depth = 0;
        if (projected)
        {
            SubtractVectors(delta.words, row->pos.words, camera.words);
            shift = 12;
            ShiftVectorRight(scaled.words, delta.words, shift);
            xport_gte_write_data(0, (scaled.words[0] & 65535) | (scaled.words[1] << 16));
            xport_gte_write_data(1, scaled.words[2]);
            ExecuteGeometryCommand(0x180001);
            command = row->rgbc;
            depth = xport_gte_read_data(27);
            if ((sint32)depth < (sint32)proj_cfg.near_depth)
                continue;
            if ((sint32)proj_cfg.max_distance < (sint32)depth)
                continue;
            screen = xport_gte_read_data(14);
            x = screen & 65535;
            y = screen >> 16;
        }
        else
        {
            x = row->x;
            y = row->y;
            command = row->rgbc;
        }
        for (;;)
        {
            character = (uint8)*text;
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
            const TextureFrame &frame = GetTextureFrame(small_font, (sint32)glyph);
            metric_width = frame.w;
            const TextureResource &tex = *frame.tex;
            if (metric_width < 4)
                ++x;
            packet = scene_render.cursor;
            limit = scene_render.limit;
            if (limit - packet < (sint32)(sizeof(POLY_FT4)))
                return 0x09000000;
            next = packet + sizeof(POLY_FT4);
            scene_render.cursor = next;
            POLY_FT4 *poly = (POLY_FT4 *)packet;
            poly->tag = 0;
            setPolyFT4(poly);
            poly->rgbc.rgbc = command;
            uint32 packed_uv0 = PackTextureUv(tex, 0);
            poly->p[0].uv.uv = packed_uv0;
            poly->p[0].attr = packed_uv0 >> 16;
            uint32 packed_uv1 = PackTextureUv(tex, 1);
            poly->p[1].uv.uv = packed_uv1;
            poly->p[1].attr = packed_uv1 >> 16;
            const uint32 uv_word = PackTextureUv(tex, 2);
            u_right = poly->p[1].uv.u;
            // Original third word includes the fourth UV in its attribute half
            poly->p[2].uv.uv = uv_word;
            poly->p[2].attr = uv_word >> 16;
            u_left = poly->p[0].uv.u;
            const uint16 uv_tail = tex.uv[3].uv;
            v_bottom = poly->p[2].uv.v;
            width = u_right - u_left;
            v_top = poly->p[0].uv.v;
            poly->p[3].uv.uv = uv_tail;
            projected = row->enlarged;
            height = v_bottom - v_top;
            if (projected)
            {
                width = (sint32)(width * 55) >> 5;
                height = (sint32)(height * 55) >> 5;
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
            projected = row->projected;
            setaddr(poly, getaddr(scene_render.cur->ot + (projected ? (depth & 0x3FFC) >> 2 : 0)));
            setaddr(scene_render.cur->ot + (projected ? (depth & 0x3FFC) >> 2 : 0), gpu_packet_link((uintptr_t)poly));
            x += 3 + width;
            if ((sint32)width < 4)
                ++x;
        }
    }
    return 1;
}

static AnimatedHudText *AllocateAnimatedHudTextNode()
{
    FUNCTION_MARKER(0x8001B654, "SLUS_003.73");
    AnimatedHudText *node = (AnimatedHudText *)malloc(sizeof(AnimatedHudText));
    if (!node)
    {
        GameDiagnostic("Cannot allocate animated HUD text\n");
        abort();
    }
    AnimatedHudText *prev = animated_hud_text;
    node->prev = 0;
    animated_hud_text = node;
    node->next = prev;
    if (prev)
        prev->prev = node;
    return node;
}

static void RemoveAnimatedHudTextNode(AnimatedHudText *node)
{
    FUNCTION_MARKER(0x8001B694, "SLUS_003.73");
    AnimatedHudText *next = node->next;
    if (next)
        next->prev = node->prev;
    AnimatedHudText *prev = node->prev;
    if (prev)
        prev->next = node->next;
    if (node == animated_hud_text)
        animated_hud_text = node->next;
    free(node);
}

void ClearAnimatedHudText()
{
    FUNCTION_MARKER(0x8001B708, "SLUS_003.73");
    AnimatedHudText *node = animated_hud_text;
    while (node)
    {
        AnimatedHudText *next = node->next;
        free(node);
        node = next;
    }
    animated_hud_text = 0;
}

void AddAnimatedHudText(const char *text, const HudTextAnimation *animation)
{
    FUNCTION_MARKER(0x8001B788, "SLUS_003.73");
    AnimatedHudText *node = AllocateAnimatedHudTextNode();
    node->text = text;
    node->animation = animation;
    node->age = 0;
}

uint32 UpdateAnimatedHudText()
{
    FUNCTION_MARKER(0x8001B7C4, "SLUS_003.73");
    AnimatedHudText *node = animated_hud_text;
    while (node)
    {
        AnimatedHudText *next = node->next;
        const HudTextAnimation *animation = node->animation;
        uint32 age = node->age;
        if (age < animation->duration)
            node->age = (uint16)(age + 1);
        else
        {
            sint32 step = animation->step;
            if (!step)
                RemoveAnimatedHudTextNode(node);
            else if (step != 127)
            {
                node->animation = animation + step;
                node->age = 0;
            }
        }
        node = next;
    }
    uint32 timer = hud_text_glow.timer;
    if (timer)
    {
        --timer;
        hud_text_glow.timer = timer;
    }
    else
    {
        timer = hud_text_glow.alternate ? RandomRange(16) + 16 : RandomRange(24);
        hud_text_glow.timer = timer;
        hud_text_glow.alternate = 0;
    }
    return timer;
}

static uint32 InterpolateHudText(uint32 first, uint32 last, uint32 age, uint32 duration)
{
    uint32 product = (last - first) * age;
    sint32 quotient;
    if (!duration)
        quotient = (sint32)product < 0 ? 1 : -1;
    else if (product == 0x80000000 && duration == (uint32)-1)
        quotient = (sint32)product;
    else
        quotient = (sint32)product / (sint32)duration;
    return first + (uint32)quotient;
}

void DrawAnimatedHudText()
{
    FUNCTION_MARKER(0x8001B8BC, "SLUS_003.73");
    AnimatedHudText *node = animated_hud_text;
    while (node)
    {
        const HudTextAnimation *animation = node->animation;
        uint32 period = animation->blink;
        if (period && !((node->age / period) & 1))
        {
            node = node->next;
            continue;
        }
        uint32 duration = animation->duration;
        hud_text_style.align = animation->align;
        hud_text_style.scale = (uint16)InterpolateHudText(animation->scale[0], animation->scale[1], node->age, duration);
        uint32 r = InterpolateHudText(animation->rgb[0][0], animation->rgb[1][0], node->age, duration) & 255;
        uint32 g = InterpolateHudText(animation->rgb[0][1], animation->rgb[1][1], node->age, duration) & 255;
        uint32 b = InterpolateHudText(animation->rgb[0][2], animation->rgb[1][2], node->age, duration) & 255;
        uint32 rgb = r | (g << 8) | (b << 16);
        hud_text_style.color.rgbc = rgb;
        hud_text_style.color.rgbc = rgb | (animation->semi ? 0x2E000000 : 0x2C000000);
        uint32 x = InterpolateHudText((uint32)(sint32)animation->scr_pos[0][0], (uint32)(sint32)animation->scr_pos[1][0], node->age, duration);
        uint32 y = InterpolateHudText((uint32)(sint32)animation->scr_pos[0][1], (uint32)(sint32)animation->scr_pos[1][1], node->age, duration);
        uint32 filter = InterpolateHudText(animation->filter[0], animation->filter[1], node->age, duration);
        DrawStyledHudText(x, y, node->text, filter, 0, 256);
        node = node->next;
    }
}

sint32 GetHudGlyphIndex(uint32 character)
{
    uint32 index;
    FUNCTION_MARKER(0x8001A784, "SLUS_003.73");
    index = large_glyphs[character & 255];
    return index == 255 ? -1 : (sint32)index;
}

void SetHudTextAlignment(uint32 alignment)
{
    FUNCTION_MARKER(0x8001A7B0, "SLUS_003.73");
    hud_text_style.align = (uint8)(alignment);
}

void SetHudTextScale(uint32 scale)
{
    FUNCTION_MARKER(0x8001A7BC, "SLUS_003.73");
    hud_text_style.scale = (uint16)(scale);
}

void SetHudTextDepth(uint32 depth)
{
    FUNCTION_MARKER(0x8001A7C8, "SLUS_003.73");
    hud_text_style.depth = (uint16)(depth);
}

uint32 SetHudTextColor(uint32 red, uint32 green, uint32 blue, uint32 semi_transparent)
{
    uint32 rgb, result;
    FUNCTION_MARKER(0x8001A7D4, "SLUS_003.73");
    rgb = ((blue & 255) << 16) | ((green & 255) << 8) | (red & 255);
    hud_text_style.color.rgbc = rgb;
    result = rgb | (semi_transparent ? 0x2E000000 : 0x2C000000);
    hud_text_style.color.rgbc = result;
    return result;
}

void LoadHudTextGlowResource()
{
    FUNCTION_MARKER(0x8001A760, "SLUS_003.73");
    glow_tex = FindModelResourceEntry(0x38B9E52B);
}

uint32 InitializeHudTextGlowCurve()
{
    uint32 index, value;
    FUNCTION_MARKER(0x8001A858, "SLUS_003.73");
    for (index = 0; index < 40; ++index)
    {
        value = RandomRange(10);
        hud_text_glow.amplitude[index] = value + 10;
        hud_text_glow.curve[index] = 40;
        value = RandomRange(30);
        hud_text_glow.rate[index] = value + 30;
    }
    return 0;
}

uint32 UpdateHudTextGlowCurve()
{
    uint32 index, rate, tick, phase, amplitude, product;
    sint32 sine;
    FUNCTION_MARKER(0x8001A8E8, "SLUS_003.73");
    for (index = 0; index < 40; ++index)
    {
        rate = hud_text_glow.rate[index];
        tick = game_clock.vblank_frame;
        phase = (rate * tick) & 4095;
        sine = rsin(phase);
        amplitude = hud_text_glow.amplitude[index];
        product = amplitude * (uint32)sine;
        if ((sint32)product < 0)
            product += 4095;
        hud_text_glow.curve[index] = (uint32)((sint32)product >> 12) + 40;
    }
    // Original updates40 entries while the packet owner cycles through41 indices
    return 0;
}

uint32 MeasureStyledHudText(const char *text)
{
    uint32 cursor = 0, character, width = 0, count = 0;
    uint32 advance, bearing, scale;
    sint32 index;
    FUNCTION_MARKER(0x8001A97C, "SLUS_003.73");
    for (;;)
    {
        character = (uint8)text[cursor];
        if (!character)
            break;
        index = GetHudGlyphIndex(character);
        if (index < 0)
            width += 10;
        else
        {
            const TextureFrame &frame = GetTextureFrame(large_font, index);
            advance = frame.w;
            bearing = (uint32)(sint32)frame.ox;
            count += 1;
            width += advance + bearing;
        }
        cursor += 1;
    }
    scale = hud_text_style.scale;
    return (uint32)((sint32)(width * scale) >> 8) + count * 3;
}

static void SetHudTextShadowColor(uint32 red, uint32 green, uint32 blue)
{
    FUNCTION_MARKER(0x8001A838, "SLUS_003.73");
    hud_text_style.shadow.r = red & 255;
    hud_text_style.shadow.g = green & 255;
    hud_text_style.shadow.b = blue & 255;
}

void ResetHudTextShadowColor()
{
    FUNCTION_MARKER(0x8001A810, "SLUS_003.73");
    SetHudTextShadowColor(255, 200, 0);
}
