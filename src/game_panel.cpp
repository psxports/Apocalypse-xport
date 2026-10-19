#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pause_menu.h"
#include "cd_voice_event.h"
#include "level_session.h"
#include "scene_lifecycle.h"
#include "game_tuning.h"
#include "input_recording.h"
#include "game_pad_poll.h"
#include "game_settings.h"

#include "game_tick.h"

#include "record_scripts.h"

#include "player.h"
#include "weapon_nodes.h"
#include "visual_object.h"
#include "cheat_input.h"
#include "resource_io.h"
#include "game_diagnostics.h"

#include "game_panel.h"
static uint32 SeedMenuFire(uint32 sparks);
static uint32 DrawMenuRightHint(void);
#include "menu_window.h"
#include "hud_text.h"
#include "hud_sprites.h"
#include "record_data.h"
#include "model_resources.h"
#include "effect_lifecycle.h"
#include "game_heap.h"
#include "game_random.h"

struct MenuFireBuffers
{
    uint8 *buffers[2];
    uint8 selected;
    uint16 width;
    uint16 height;
    uint8 intensity;
    PSX_RECT rectangle;
};

PanelText panel_text = {"", 0, -20, 0, {'?', '?', 0}, 0};

void ResetPanelText()
{
    const PanelText initial = {"", 0, -20, 0, {'?', '?', 0}, 0};
    panel_text = initial;
}

static MenuFireBuffers menu_fire;

void ResetMenuFireBuffers()
{
    ReleaseMenuBuffers();
    MenuFireBuffers initial = {0};
    menu_fire = initial;
}

static const TextureGroup *panel_group;

struct HealthTracker
{
    ModelReference object;
    uint32 prev_health;
    uint16 x;
    uint16 y;
    uint32 initial_health;
    uint32 scale;
    uint32 flash;

    HealthTracker() : prev_health(0), x(0), y(0), initial_health(0), scale(0), flash(0)
    {
    }
};

static HealthTracker health_tracker;

void ForgetPanelTexture(const TextureGroup *grp)
{
    if (panel_group == grp)
        panel_group = 0;
}

void ClearPanelTextureBinding()
{
    panel_group = 0;
}

void LoadPanelResource()
{
    FUNCTION_MARKER(0x8006CDA0, "SLUS_003.73");
    panel_group = FindModelTextureGroup("Panel");
}

uint32 ResetPanelDisplayTimer()
{
    FUNCTION_MARKER(0x8006CDC4, "SLUS_003.73");
    panel_text.lives_timer = 150;
    return 150;
}

uint32 DrawDebugPanel()
{
    RecordDataBorrow record_borrow;
    uint32 enabled = GetCheatOptions().debug_panel;
    const char *text = "";
    ModelInstance *player;
    uint32 trigger, numerator, denominator, percentage;
    Vector3 position;
    sint32 high;
    FUNCTION_MARKER(0x8006CDD4, "SLUS_003.73");
    if (!enabled)
        return enabled;
    trigger = GetStartRecordIndex();
    if (trigger != 0xFFFF && (sint16)(GetLevelRecord(trigger)).Read16() == 8)
        text = (ReadRecordPosition(position.words, trigger) + 6).CStr();
    QueueHudText(24, 193, text, 32, 32, 114, 1);
    player = visual_lists.players;
    QueueHudNumber(23, 205, (uint32)((sint32)player->pos.words[0] >> 12), 32, 32, 114, 1);
    player = visual_lists.players;
    QueueHudNumber(90, 205, (uint32)((sint32)player->pos.words[1] >> 12), 32, 32, 114, 1);
    player = visual_lists.players;
    QueueHudNumber(152, 205, (uint32)((sint32)player->pos.words[2] >> 12), 32, 32, 114, 1);
    numerator = scene_render.used * 100;
    high = (sint32)(((sint64)(sint32)numerator * (sint64)0x4BDA12F7) >> 32);
    percentage = (uint32)(high >> 15) - (uint32)((sint32)numerator >> 31);
    QueueHudNumber(24, 217, percentage, 32, 32, 114, 1);
    numerator = GetGameHeapUsed(0) * 100;
    denominator = GetGameHeapCapacity(0);
    percentage = denominator ? numerator / denominator : 0xFFFFFFFF;
    QueueHudNumber(49, 217, percentage, 32, 32, 114, 1);
    player = visual_lists.players;
    if (player)
        QueueHudNumber(74, 217, static_cast<Player *>(player->owner)->sel_weapon, 32, 32, 114, 1);
    QueueHudNumber(109, 217, GetResourceReadPhase(), 32, 32, 114, 1);
    return QueueHudNumber(127, 217, cd_voice.active, 32, 32, 114, 1);
}

uint32 TrackObjectHealth(VisualObject &object, uint32 x, uint32 y, uint32 scale)
{
    FUNCTION_MARKER(0x8006D390, "SLUS_003.73");
    uint32 health = (uint32)(sint32)object.health;
    health_tracker.object.SetObject(&object.model);
    health_tracker.x = x;
    health_tracker.y = y;
    health_tracker.scale = scale;
    health_tracker.initial_health = health;
    health_tracker.prev_health = health;
    return health;
}

void ClearTrackedObjectHealth()
{
    FUNCTION_MARKER(0x8006D3B4, "SLUS_003.73");
    health_tracker.object.Reset();
}

static void DrawTrackedObjectHealth()
{
    VisualObject *object;
    uint8 *packet, *cursor, *next, *limit;
    uint32 health, flash, scale, product;
    uint32 x, y, right, top, bottom, left, opcode, initial_health;
    FUNCTION_MARKER(0x8006D3C0, "SLUS_003.73");
    ModelInstance *instance = health_tracker.object.GetObject();
    if (!instance)
        return;
    object = static_cast<VisualObject *>(instance->owner);
    if (object->lifecycle_flags & 64)
    {
        ClearTrackedObjectHealth();
        return;
    }
    health = (uint32)(sint32)object->health;
    if ((sint32)health < 0)
        health = 0;
    if ((sint32)health < (sint32)health_tracker.prev_health)
    {
        health_tracker.prev_health = health;
        health_tracker.flash = 8;
    }
    packet = scene_render.cursor;
    limit = scene_render.limit;
    if (limit - packet < (sint32)(sizeof(POLY_F4)))
        return;
    next = packet + sizeof(POLY_F4);
    scene_render.cursor = next;
    POLY_F4 *health_poly = (POLY_F4 *)packet;
    setPolyF4(health_poly);
    health_poly->rgbc.code = 40;
    health_poly->rgbc.r = 128;
    flash = (uint8)health_tracker.flash;
    health_poly->rgbc.g = flash << 4;
    flash = (uint8)health_tracker.flash;
    health_poly->rgbc.b = flash << 4;
    scale = health_tracker.scale;
    x = health_tracker.x;
    product = (uint32)((sint32)health >> 2) * scale;
    opcode = health_poly->rgbc.code;
    y = health_tracker.y;
    health_poly->rgbc.code = opcode | 2;
    health_poly->p[0].x = x;
    health_poly->p[2].x = x;
    health_poly->p[0].y = y;
    health_poly->p[1].y = y;
    y += 6;
    health_poly->p[2].y = y;
    health_poly->p[3].y = y;
    cursor = scene_render.cursor;
    right = x + (uint32)((sint32)product >> 12);
    health_poly->p[1].x = right;
    health_poly->p[3].x = right;
    limit = scene_render.limit;
    setaddr(health_poly, getaddr(scene_render.cur->ot));
    setaddr(scene_render.cur->ot, gpu_packet_link((uintptr_t)health_poly));
    if (limit - cursor < (sint32)(sizeof(DR_TPAGE)))
        return;
    next = cursor + sizeof(DR_TPAGE);
    scene_render.cursor = next;
    DR_TPAGE *mode = (DR_TPAGE *)cursor;
    setlen(mode, 1);
    mode->code[0] = 0xE1000220;
    flash = health_tracker.flash;
    setaddr(mode, getaddr(scene_render.cur->ot));
    setaddr(scene_render.cur->ot, gpu_packet_link((uintptr_t)mode));
    if (flash)
        health_tracker.flash = flash - 1;
    packet = scene_render.cursor;
    limit = scene_render.limit;
    if (limit - packet < (sint32)(sizeof(LINE_F4)))
        return;
    next = packet + sizeof(LINE_F4);
    scene_render.cursor = next;
    LINE_F4 *outline = (LINE_F4 *)packet;
    setLineF4(outline);
    outline->rgbc.code = 76;
    outline->rgbc.r = 240;
    outline->rgbc.g = 0;
    outline->rgbc.b = 0;
    initial_health = health_tracker.initial_health;
    outline->pad = 0x55555555;
    scale = health_tracker.scale;
    y = health_tracker.y;
    cursor = scene_render.cursor;
    product = (uint32)((sint32)initial_health >> 2) * scale;
    top = y - 1;
    bottom = y + 6;
    x = health_tracker.x;
    outline->p[2].y = bottom;
    outline->p[3].y = bottom;
    outline->p[0].y = top;
    outline->p[1].y = top;
    left = x - 1;
    outline->p[0].x = left;
    outline->p[3].x = left;
    right = x + (uint32)((sint32)product >> 12) + 1;
    outline->p[1].x = right;
    outline->p[2].x = right;
    limit = scene_render.limit;
    setaddr(outline, getaddr(scene_render.cur->ot));
    setaddr(scene_render.cur->ot, gpu_packet_link((uintptr_t)outline));
    if (limit - cursor < (sint32)(sizeof(LINE_F2)))
        return;
    next = cursor + sizeof(LINE_F2);
    scene_render.cursor = next;
    LINE_F2 *left_line = (LINE_F2 *)cursor;
    setLineF2(left_line);
    left_line->rgbc.code = 64;
    left_line->rgbc.r = 240;
    left_line->rgbc.g = 0;
    left_line->rgbc.b = 0;
    y = health_tracker.y;
    x = health_tracker.x;
    top = y - 1;
    bottom = y + 6;
    left_line->p[1].y = bottom;
    left = x - 1;
    left_line->p[0].x = left;
    left_line->p[1].x = left;
    left_line->p[0].y = top;
    setaddr(left_line, getaddr(scene_render.cur->ot));
    setaddr(scene_render.cur->ot, gpu_packet_link((uintptr_t)left_line));
}

static void DrawWeaponAmmoIndicator(uint32 slot)
{
    Player *hero = 0;
    static const uint32 xs[8] = {0, 414, 424, 439, 449, 454, 454, 449};
    static const uint32 ys[8] = {0, 37, 32, 32, 37, 43, 49, 55};
    uint8 *packet, *bar, *next, *limit;
    uint32 ammo, extent, x, y;
    WeaponNode *weapon;
    sint32 high;
    FUNCTION_MARKER(0x8006D704, "SLUS_003.73");
    hero = visual_lists.players ? static_cast<Player *>(visual_lists.players->owner) : 0;
    if (!hero)
        return;
    if (slot >= 8 || !slot)
        return;
    weapon = hero->weapons[slot];
    if (!weapon)
        return;
    x = xs[slot];
    y = ys[slot];
    packet = scene_render.cursor;
    limit = scene_render.limit;
    if (limit - packet < (sint32)(2 * sizeof(LINE_F2)))
        return;
    next = packet + 2 * sizeof(LINE_F2);
    bar = packet + sizeof(LINE_F2);
    ammo = weapon->ammo;
    high = (sint32)(((sint64)(sint32)ammo * (sint64)0x10624DD3) >> 32);
    scene_render.cursor = next;
    extent = (uint32)(high >> 3) - (uint32)((sint32)ammo >> 31);
    if (extent)
    {
        LINE_F2 *ammo_line = (LINE_F2 *)bar;
        setLineF2(ammo_line);
        ammo_line->rgbc.code = 64;
        ammo_line->rgbc.g = 255;
        ammo_line->rgbc.r = 0;
        ammo_line->rgbc.b = 0;
        ammo_line->p[0].x = x;
        ammo_line->p[0].y = y;
        ammo_line->p[1].x = x + extent - 1;
        ammo_line->p[1].y = y;
        setaddr(ammo_line, getaddr(scene_render.cur->ot));
        setaddr(scene_render.cur->ot, gpu_packet_link((uintptr_t)ammo_line));
    }
    LINE_F2 *back_line = (LINE_F2 *)packet;
    setLineF2(back_line);
    back_line->rgbc.code = 64;
    back_line->rgbc.g = 64;
    back_line->rgbc.r = 0;
    back_line->rgbc.b = 0;
    back_line->p[0].x = x;
    back_line->p[0].y = y;
    back_line->p[1].x = x + 7;
    back_line->p[1].y = y;
    setaddr(back_line, getaddr(scene_render.cur->ot));
    setaddr(scene_render.cur->ot, gpu_packet_link((uintptr_t)back_line));
    if (hero->sel_weapon == slot)
        AllocateHudRectangle(x - 3, y - 1, 13, 4, 128, 128, 128, 0);
}

void DrawGamePanel()
{
    Player *hero = 0;
    const char *text;
    uint32 timer, value, quotient, flag, kind;
    WeaponNode *weapon;
    uint32 x, y, percentage, width, product, height, green, index;
    sint32 count, maximum, numerator, sine;
    POLY_FT4 *packet;
    const TextureFrame *texture;
    FUNCTION_MARKER(0x8006D91C, "SLUS_003.73");
    timer = panel_text.lives_timer;
    if (timer)
    {
        texture = &GetTextureFrame(panel_group, 1);
        panel_text.lives_timer = timer - 1;
        packet = AllocateHudSprite(*texture);
        if (packet)
            PlaceHudSprite(40, 27, packet, GetTextureFrame(panel_group, 1));
        if (panel_text.lives_timer)
        {
            if (GetCheatOptions().unlimited_lives)
                text = MenuText::infinite_lives;
            else
            {
                value = game_progress.lives - 1;
                quotient = (uint32)((sint32)value / 10);
                text = panel_text.lives;
                panel_text.lives[0] = (char)(quotient ? quotient + 48 : 32);
                panel_text.lives[1] = (char)(value - quotient * 10 + 48);
            }
            SetHudTextScale(256);
            SetHudTextColor(128, 128, 128, 0);
            SetHudTextAlignment(1);
            DrawStyledHudText(65, 27, text, 0, 0, 256);
        }
    }
    if (scene_frame.paused)
        return;
    flag = game_session.demo_mode;
    hero = visual_lists.players ? static_cast<Player *>(visual_lists.players->owner) : 0;
    if (flag)
    {
        SetHudTextScale(256);
        SetHudTextColor(128, 128, 128, 0);
        DrawStyledHudText(58, 50, MenuText::demo, 0, 0, 256);
    }
    if (!GetCheatOptions().status_hud)
        return;
    count = (sint32)hero->smartbombs;
    if (count >= 16)
        count = 15;
    x = 412;
    for (sint32 item = 0; item < count && item < 8; ++item)
    {
        packet = AllocateHudSprite(GetTextureFrame(panel_group, 10));
        if (packet)
            PlaceScaledHudSprite(x, 25, packet, GetTextureFrame(panel_group, 10));
        x += 8;
    }
    if (count >= 9)
    {
        x -= 8;
        y = 30;
        count -= 8;
        for (sint32 item = 0; item < count; ++item)
        {
            packet = AllocateHudSprite(GetTextureFrame(panel_group, 10));
            if (packet)
                PlaceScaledHudSprite(x, y, packet, GetTextureFrame(panel_group, 10));
            y += 5;
        }
    }
    DrawWeaponAmmoIndicator(1);
    DrawWeaponAmmoIndicator(2);
    DrawWeaponAmmoIndicator(3);
    DrawWeaponAmmoIndicator(4);
    DrawWeaponAmmoIndicator(5);
    DrawWeaponAmmoIndicator(6);
    DrawWeaponAmmoIndicator(7);
    packet = AllocateHudSprite(GetTextureFrame(panel_group, 0));
    if (packet)
    {
        PlaceScaledHudSprite(436, 46, packet, GetTextureFrame(panel_group, 0));
        packet->rgbc.b = 90;
        packet->rgbc.g = 90;
        packet->rgbc.r = 90;
    }
    hero = visual_lists.players ? static_cast<Player *>(visual_lists.players->owner) : 0;
    if (hero)
    {
        weapon = hero ? hero->weapons[hero->sel_weapon] : 0;
        kind = weapon ? weapon->type : 0;
        texture = &GetTextureFrame(panel_group, 0);
        switch (kind)
        {
            case 1:
                texture = &GetTextureFrame(panel_group, 9);
                break;
            case 2:
                texture = &GetTextureFrame(panel_group, 3);
                break;
            case 3:
                texture = &GetTextureFrame(panel_group, 5);
                break;
            case 4:
                texture = &GetTextureFrame(panel_group, 6);
                break;
            case 5:
                texture = &GetTextureFrame(panel_group, 4);
                break;
            case 6:
                texture = &GetTextureFrame(panel_group, 8);
                break;
            case 7:
                texture = &GetTextureFrame(panel_group, 7);
                break;
            case 8:
                texture = 0;
                AllocateHudRectangle(425, 39, 22, 14, 40, 40, 40, 0);
                break;
            case 9:
                texture = 0;
                break;
            default:
                break;
        }
        if (texture)
        {
            packet = AllocateHudSprite(*texture);
            if (packet)
                PlaceScaledHudSprite(436, 46, packet, *texture);
        }
    }
    // TODO Establish the original invariant requiring a player here
    maximum = (sint16)GetGameTuning().player.maximum_health;
    numerator = (sint32)(((uint32)maximum - (uint32)(sint32)hero->health) * 100);
    if (!maximum)
        percentage = numerator < 0 ? 1 : 0xFFFFFFFF;
    else if (numerator == (sint32)0x80000000 && maximum == -1)
        percentage = (uint32)numerator;
    else
        percentage = (uint32)(numerator / maximum);
    product = percentage * 80;
    width = (uint32)((sint32)product / 100);
    AllocateHudRectangle(469 - width, 16, width, 6, 0, 0, 0, 0);
    if ((sint32)width < 41)
        QueueHudGradientRectangle(429, 16, 40, 6, 0, 255, 0, 255, 255, 0, 0);
    if ((sint32)percentage >= 61)
    {
        product = (percentage - 60) * 350;
        value = (uint32)((sint32)product / 40);
        index = ((value + 50) * game_clock.tick) & 4095;
        sine = rsin(index);
        product = (uint32)sine * (uint32)sine;
        green = (uint32)((sint32)product >> 16);
        if ((sint32)green >= 256)
            green = 255;
        green &= 255;
        QueueHudGradientRectangle(389, 16, 40, 6, 255, green, 0, 255, green, 0, 0);
    }
    else
        QueueHudGradientRectangle(389, 16, 40, 6, 255, 255, 0, 255, 0, 0, 0);
    weapon = hero ? hero->weapons[hero->sel_weapon] : 0;
    if (weapon)
    {
        if (weapon->type == 8)
        {
            y = 25;
            height = 50;
        }
        else
        {
            value = 1000 - weapon->ammo;
            height = (uint32)((sint32)value / 20);
            AllocateHudRectangle(474, 25, 10, height, 0, 0, 0, 0);
            y = height + 25;
            height = 50 - height;
        }
        AllocateHudRectangle(474, y, 10, height, 0, 100, 200, 0);
    }
    DrawTrackedObjectHealth();
}

static uint32 DrawPauseMenuHeading(void)
{
    FUNCTION_MARKER(0x8001664C, "SLUS_003.73");
    uint32 result;
    SetHudTextScale(512);
    SetHudTextColor(149, 20, 20, 0);
    result = DrawStyledHudText(256, 34, MenuText::paused, 0, 0, 256);
    SetHudTextScale(256);
    return result;
}

void DrawPauseMenuDisplay(void)
{
    FUNCTION_MARKER(0x80016884, "SLUS_003.73");
    uint32 blink;
    ModelInstance *player;
    const char *label;
    uint32 text_y, amplitude, phase, countdown, text_count;
    sint32 shake_remaining;
    MenuWindow *pause_window;
    MenuWindow *window;
    const char *prompt;
    sint32 prompt_y;
    sint32 i;
    sint32 caption_width;
    MenuWindow *confirm_window;
    sint32 message_x;
    sint32 message_y;
    const char *message;
    const char *error_prompt;
    sint32 error_y;
    char caption[104];
    char percent[13];
    if (GetCheatOptions().debug_panel)
        /* MIPS 800168B0..800168C0 stores zero for all four stack arguments */
        AllocateHudRectangle(19, 190, 194, 42, 0, 0, 0, 0);
    if (scene_frame.paused)
    {
        (void)0;
        ::UpdateHudTextGlowCurve();
    }
    SetHudTextAlignment(0);
    if ((input_recording.HasBuffer() && ((game_clock.vblank_frame & 0x20) != 0)))
    {
        SetHudTextColor(128, 128, 128, 0);
        SetHudTextScale(256);
        DrawStyledHudText(256, 35, MenuText::demo, 0, 0, 256);
    }
    player = visual_lists.players;
    if (player && static_cast<VisualObject *>(player->owner)->health <= 0 && game_progress.lives == 1)
    {
        if (!::HasMenuBuffers())
            ::CreateMenuBuffers(951706923, 64, 64, 127);
        SetHudTextColor(128, 128, 128, 0);
        SetHudTextScale(384);
        label = MenuText::game_over;
        text_y = (uint32)(sint32)panel_text.game_over_y;
        DrawStyledHudText(256, text_y, label, 0, 128, 256);
        text_y = (uint16)panel_text.game_over_y + 8;
        panel_text.game_over_y = (sint16)text_y;
        if ((sint16)text_y >= 121)
            panel_text.game_over_y = 120;
    }
    SetHudTextScale(256);
    switch (menu_runtime.screen)
    {
        case MENU_SCREEN_PAUSED:
            DrawPauseMenuHeading();
            amplitude = menu_runtime.shake_amplitude;
            shake_remaining = (sint32)(amplitude - 2);
            if (amplitude)
            {
                phase = menu_runtime.shake_phase;
                menu_runtime.shake_amplitude = shake_remaining;
                menu_runtime.shake_phase = phase + 690;
                if (shake_remaining < 0)
                {
                    menu_runtime.shake_amplitude = 0;
                    menu_runtime.shake_phase = 0;
                }
            }
            phase = menu_runtime.shake_phase;
            {
                uint32 cosine = (uint32)rcos(phase & 4095);
                uint32 product = menu_runtime.shake_amplitude * cosine;
                pause_window = menu_runtime.pause;
                if ((sint32)product < 0)
                    product += 4095;
                pause_window->scr_pos[0] = (uint32)((sint32)product >> 12) + 256;
            }
            pause_window->Draw();
            if (GetCheatOptions().debug_panel)
                DrawMenuRightHint();
            else
                DrawMenuCenteredHint();
            return;

        case MENU_SCREEN_RESTART_SELECTION:
            window = menu_runtime.restart;
            if (window)
                window->Draw();
            return;

        case MENU_SCREEN_EXIT_FADE_START:
        case MENU_SCREEN_EXIT_FADE:
            return;

        case MENU_SCREEN_QUIT_CONFIRM:
            DrawPauseMenuHeading();
            DrawMenuHints();
            SetHudTextColor(149, 20, 20, 0);
            DrawStyledHudText(256, 95, MenuText::really_quit, 0, 0, 256);
            window = menu_runtime.confirm;
            window->scr_pos[1] = 135;
            window->Draw();
            return;

        case MENU_SCREEN_CONTROLLER_MISSING:
            DrawPauseMenuHeading();
            SetMenuRedText();
            blink = ((sint32)game_clock.tick / 10) & 1;
            if (!blink)
                return;
            DrawStyledHudText(256, 171, MenuText::please_reinsert, 0, 0, 256);
            prompt = MenuText::controller;
            prompt_y = 197;
            goto DrawPrompt;

        case MENU_SCREEN_RESULTS_CAPTION:
            // Preserve the original 104-byte caption buffer
            text_count = panel_text.visible;
            if ((sint32)text_count < 0 || text_count >= sizeof(caption))
            {
                GameDiagnostic("Menu text length exceeds original 104-byte buffer\n");
                abort();
            }
            i = 0;
            if ((sint32)text_count > 0)
            {
                do
                {
                    if ((uint32)i >= sizeof(caption) - 1)
                    {
                        GameDiagnostic("Menu text grew beyond original buffer\n");
                        abort();
                    }
                    caption[i] = panel_text.caption[(uint32)i];
                    ++i;
                    text_count = panel_text.visible;
                } while (i < (sint32)text_count);
            }
            text_count = panel_text.visible;
            if (text_count >= sizeof(caption))
            {
                GameDiagnostic("Menu text terminator exceeds original buffer\n");
                abort();
            }
            caption[text_count] = 0;
            SetHudTextScale(192);
            SetHudTextColor(128, 128, 128, 0);
            SetHudTextAlignment(1);
            caption_width = MeasureStyledHudText(panel_text.caption);
            DrawStyledHudText(256 - (uint32)((sint32)caption_width >> 1), 34, caption, 0, 0, 256);
            return;

        case MENU_SCREEN_RESULTS_PERCENT:
            goto DrawResults;

        case MENU_SCREEN_SAVE_CONFIRM:
            SetHudTextColor(149, 20, 20, 0);
            DrawStyledHudText(256, 103, MenuText::save_game_prompt, 0, 0, 256);
            confirm_window = menu_runtime.confirm;
            confirm_window->scr_pos[1] = 143;
            confirm_window->Draw();
            DrawMenuCenteredHint();
        DrawResults:
            SetHudTextScale(192);

            SetHudTextColor(128, 128, 128, 0);
            SetHudTextAlignment(0);
            DrawStyledHudText(256, 34, panel_text.caption, 0, 0, 256);
            if (!game_progress.completion.completed)
                return;
            SetHudTextScale(256);
            SetHudTextAlignment(1);
            DrawStyledHudText(166, 68, MenuText::kill_percent, 0, 0, 256);
            if ((game_progress.completion.display_percent == 100))
            {
                countdown = game_progress.completion.blink_ticks;
                if (countdown)
                    game_progress.completion.blink_ticks = countdown - 1;
                blink = (game_progress.completion.blink_ticks >> 2) & 1;
                if (blink)
                    return;
            }
            snprintf(percent, sizeof(percent), " %d", (sint32)game_progress.completion.display_percent);
            message_x = 270;
            message_y = 68;
            message = percent;
            goto DrawMessage;

        case MENU_SCREEN_CARD_POLL_DELAY:

        case MENU_SCREEN_CARD_QUERY:
            SetMenuGreenText();
            DrawStyledHudText(256, 90, MenuText::checking_memory_card, 0, 0, 256);
            SetMenuRedText();
            DrawStyledHudText(256, 140, MenuText::do_not_remove, 0, 0, 256);
            prompt = MenuText::memory_card;
            prompt_y = 166;
            goto DrawPrompt;

        case MENU_SCREEN_SAVE_SLOT:
            DrawMenuHints();
            SetMenuRedText();
            DrawStyledHudText(256, 25, MenuText::save_game, 0, 0, 256);
            window = menu_runtime.save;
            window->Draw();
            return;

        case MENU_SCREEN_SAVE_OVERWRITE:
            DrawMenuHints();
            SetMenuGreenText();
            DrawStyledHudText(256, 40, MenuText::okay_to_overwrite, 0, 0, 256);
            DrawStyledHudText(256, 66, MenuText::previously_saved_game, 0, 0, 256);
            DrawStyledHudText(256, 92, MenuText::in_this_save_slot, 0, 0, 256);
            window = menu_runtime.confirm;
            window->Draw();
            return;

        case MENU_SCREEN_CARD_WRITE:
            SetMenuGreenText();
            DrawStyledHudText(256, 110, MenuText::now_saving_data, 0, 0, 256);
            SetMenuRedText();
            DrawStyledHudText(256, 140, MenuText::do_not_remove, 0, 0, 256);
            prompt = MenuText::memory_card;
            prompt_y = 166;
            goto DrawPrompt;

        case MENU_SCREEN_CARD_WRITE_ERROR:
            SetMenuRedText();
            DrawStyledHudText(256, 110, MenuText::attempt_to_write_to, 0, 0, 256);
            error_y = 136;
            error_prompt = MenuText::memory_card_failed;
            goto DrawCardError;

        case MENU_SCREEN_CARD_WRITE_COMPLETE:
            SetMenuGreenText();
            DrawStyledHudText(256, 110, MenuText::save_completed, 0, 0, 256);
            prompt_y = 140;
            goto ShowAcknowledgePrompt;

        case MENU_SCREEN_CARD_FULL:
            SetMenuRedText();
            SetHudTextScale(192);
            DrawStyledHudText(256, 67, MenuText::your_memory_card_in, 0, 0, 256);
            DrawStyledHudText(256, 85, MenuText::memory_card_slot_1_is_full, 0, 0, 256);
            DrawStyledHudText(256, 103, MenuText::insert_a_memory_card_with, 0, 0, 256);
            DrawStyledHudText(256, 121, MenuText::at_least_1_free_block_or_delete, 0, 0, 256);
            DrawStyledHudText(256, 139, MenuText::a_block_using_the_console, 0, 0, 256);
            DrawStyledHudText(256, 157, MenuText::internal_memory_card_manager, 0, 0, 256);
            SetHudTextScale(256);
            prompt_y = 188;
            goto ShowAcknowledgePrompt;

        case MENU_SCREEN_CARD_MISSING:
            SetMenuRedText();
            DrawStyledHudText(256, 90, MenuText::please_insert_a_memory, 0, 0, 256);
            DrawStyledHudText(256, 116, MenuText::card_in_memory_card, 0, 0, 256);
            DrawStyledHudText(256, 142, MenuText::slot_1_or_press_any, 0, 0, 256);
            prompt = MenuText::button_to_cancel;
            prompt_y = 168;
            goto DrawPrompt;

        case MENU_SCREEN_CARD_FORMAT_CONFIRM:
            DrawMenuHints();
            SetMenuRedText();
            DrawStyledHudText(256, 35, MenuText::the_memory_card_in, 0, 0, 256);
            DrawStyledHudText(256, 61, MenuText::memory_card_slot_1_is, 0, 0, 256);
            DrawStyledHudText(256, 87, MenuText::unformatted_do_you, 0, 0, 256);
            DrawStyledHudText(256, 113, MenuText::want_to_format_it, 0, 0, 256);
            window = menu_runtime.confirm;
            window->scr_pos[1] = 148;
            window->Draw();
            return;

        case MENU_SCREEN_CARD_FORMAT:
            SetMenuGreenText();
            if ((((((sint32)game_clock.tick) / 10) & 1) != 0))
                DrawStyledHudText(256, 110, MenuText::formatting_memory_card, 0, 0, 256);
            SetMenuRedText();
            DrawStyledHudText(256, 140, MenuText::do_not_remove, 0, 0, 256);
            prompt = MenuText::memory_card;
            prompt_y = 166;
            goto DrawPrompt;

        case MENU_SCREEN_CARD_FORMAT_ERROR:
            SetMenuRedText();
            DrawStyledHudText(256, 90, MenuText::format_failed, 0, 0, 256);
            DrawStyledHudText(256, 116, MenuText::try_again_or_insert, 0, 0, 256);
            error_prompt = MenuText::a_new_memory_card;
            error_y = 142;
        DrawCardError:
            DrawStyledHudText(256, error_y, error_prompt, 0, 0, 256);

            prompt_y = 170;
        ShowAcknowledgePrompt:
            prompt = MenuText::press_any_button;

        DrawPrompt:
            DrawStyledHudText(256, prompt_y, prompt, 0, 0, 256);

            break;

        case MENU_SCREEN_CARD_FORMAT_COMPLETE:
            SetMenuGreenText();
            if ((((((sint32)game_clock.tick) / 10) & 1) != 0))
                DrawStyledHudText(256, 110, MenuText::format_completed, 0, 0, 256);
            message_x = 256;
            message_y = 140;
            message = MenuText::press_any_button;
        DrawMessage:
            DrawStyledHudText(message_x, message_y, message, 0, 0, 256);

            break;

        default:
            return;
    }

    return;
}

uint32 SetMenuRedText(void)
{
    FUNCTION_MARKER(0x800151D0, "SLUS_003.73");
    return SetHudTextColor(255, 0, 0, 0);
}

uint32 SetMenuGreenText(void)
{
    FUNCTION_MARKER(0x800151FC, "SLUS_003.73");
    return SetHudTextColor(0, 255, 0, 0);
}

uint32 DrawMenuLeftHint(void)
{
    FUNCTION_MARKER(0x800166AC, "SLUS_003.73");
    SetHudTextAlignment(1);
    SetHudTextScale(192);
    SetHudTextColor(149, 20, 20, 0);
    DrawStyledHudText(92, 210, MenuText::go_back, 0, 0, 256);
    SetHudTextAlignment(0);
    POLY_FT4 *packet = AllocateHudSprite(GetTextureFrame(GetButtonResourceGroup(), 1));
    uint32 result = 0;
    if (packet)
        result = PlaceScaledHudSprite(64, 211, packet, GetTextureFrame(GetButtonResourceGroup(), 1));
    SetHudTextScale(256);
    return result;
}

static uint32 DrawMenuRightHint(void)
{
    FUNCTION_MARKER(0x80016744, "SLUS_003.73");
    SetHudTextAlignment(1);
    SetHudTextScale(192);
    SetHudTextColor(149, 20, 20, 0);
    DrawStyledHudText(348, 210, MenuText::select, 0, 0, 256);
    SetHudTextAlignment(0);
    POLY_FT4 *packet = AllocateHudSprite(GetTextureFrame(GetButtonResourceGroup(), 0));
    uint32 result = 0;
    if (packet)
        result = PlaceScaledHudSprite(320, 211, packet, GetTextureFrame(GetButtonResourceGroup(), 0));
    SetHudTextScale(256);
    return result;
}

uint32 DrawMenuHints(void)
{
    FUNCTION_MARKER(0x800167D8, "SLUS_003.73");
    DrawMenuLeftHint();
    return DrawMenuRightHint();
}

uint32 DrawMenuCenteredHint(void)
{
    FUNCTION_MARKER(0x80016800, "SLUS_003.73");
    SetHudTextAlignment(1);
    SetHudTextScale(256);
    SetHudTextColor(149, 20, 20, 0);
    DrawStyledHudText(227, 213, MenuText::select, 0, 0, 256);
    POLY_FT4 *packet = AllocateHudSprite(GetTextureFrame(GetButtonResourceGroup(), 0));
    if (packet)
        return PlaceScaledHudSprite(199, 214, packet, GetTextureFrame(GetButtonResourceGroup(), 0));
    return 0;
}

uint32 HasMenuBuffers()
{
    FUNCTION_MARKER(0x8002E13C, "SLUS_003.73");
    return menu_fire.buffers[0] != 0;
}

uint32 CreateMenuBuffers(uint32 resource, uint32 width, uint32 height, uint32 mode)
{
    FUNCTION_MARKER(0x8002E148, "SLUS_003.73");
    uint32 result = HasMenuBuffers();
    if (result)
        return result;
    const TextureResource &texture = GetTextureResource(resource);
    width &= 65535;
    height &= 65535;
    uint32 page = texture.tpage;
    uint32 pixel_shift = (2 - ((page & 384) >> 7)) & 31;
    uint32 texture_x = texture.uv[0].u;
    menu_fire.rectangle.x = ((page & 15) << 6) + (texture_x >> pixel_shift);
    uint32 texture_y = texture.uv[0].v;
    menu_fire.rectangle.w = width >> 1;
    menu_fire.width = width;
    menu_fire.rectangle.h = height;
    menu_fire.height = height;
    menu_fire.intensity = mode;
    menu_fire.rectangle.y = texture_y + ((page << 4) & 256);
    uint32 bytes = width * height;
    uint8 *allocation = (uint8 *)calloc(height, 2 * (size_t)width);
    if (!allocation)
    {
        StopWithHeapErrorColor(0, 0, 128);
        return 0;
    }
    menu_fire.buffers[0] = allocation;
    menu_fire.buffers[1] = allocation + bytes;
    return 0;
}

void ReleaseMenuBuffers()
{
    FUNCTION_MARKER(0x8002E2B8, "SLUS_003.73");
    if (menu_fire.buffers[0])
    {
        free(menu_fire.buffers[0]);
        menu_fire.buffers[0] = 0;
        menu_fire.buffers[1] = 0;
    }
}

uint32 BlurMenuBuffers(void)
{
    FUNCTION_MARKER(0x8002E2E8, "SLUS_003.73");
    uint8 *selected, *other;
    uint32 toggle;
    sint32 row;
    selected = menu_fire.buffers[0];
    if (!selected)
        return 0;
    toggle = menu_fire.selected == 0;
    menu_fire.selected = toggle;
    selected = menu_fire.buffers[toggle];
    other = menu_fire.buffers[toggle == 0];
    for (row = 1; row < (sint32)menu_fire.height - 1; ++row)
    {
        uint32 stride = menu_fire.width;
        uint32 top = xport_load_le32(selected + (uint32)(row - 1) * stride);
        uint32 bottom = xport_load_le32(selected + (uint32)(row + 1) * stride);
        uint32 carry = 0;
        sint32 column = 1;
        for (;;)
        {
            uint32 top_second = (top >> 16) & 255;
            uint32 bottom_second = (bottom >> 16) & 255;
            uint32 top_last = top >> 24;
            uint32 bottom_last = bottom >> 24;
            uint32 output = carry;
            uint32 next_top, next_bottom;
            sint32 next_column = column + 2;
            uint32 top_offset;
            output |= (((top & 255) + top_second + (bottom & 255) + bottom_second) >> 2) << 8;
            output |= ((((top >> 8) & 255) + top_last + ((bottom >> 8) & 255) + bottom_last) >> 2) << 16;
            stride = menu_fire.width;
            top_offset = (uint32)(row - 1) * stride;
            if (next_column >= (sint32)stride - 1)
                break;
            next_top = xport_load_le32(selected + top_offset + (uint32)next_column + 1);
            next_bottom = xport_load_le32(selected + (uint32)(row + 1) * stride + (uint32)next_column + 1);
            output |= ((top_second + (next_top & 255) + bottom_second + (next_bottom & 255)) >> 2) << 24;
            // Write four packed pixels at potentially unaligned byte positions
            xport_store_le32(selected + (uint32)row * stride + (uint32)next_column - 3, output);
            xport_store_le32(other + top_offset + (uint32)next_column - 3, output);
            carry = (top_last + ((next_top >> 8) & 255) + bottom_last + (next_bottom >> 24)) >> 2;
            top = next_top;
            bottom = next_bottom;
            column = next_column + 2;
        }
    }
    SeedMenuFire(4);
    return LoadImagePSX(&menu_fire.rectangle, (uint32 *)selected);
}

static uint32 SeedMenuFire(uint32 sparks)
{
    FUNCTION_MARKER(0x8002E4D8, "SLUS_003.73");
    uint32 height = menu_fire.height;
    uint32 width = menu_fire.width;
    uint32 row_offset = (height - 1) * width;
    uint32 selected = menu_fire.selected;
    uint8 *buffer = menu_fire.buffers[selected];
    uint8 *cursor = buffer + row_offset;
    uint32 words = width >> 2;
    if (words)
    {
        uint32 index = 0;
        do
        {
            memset(cursor, 0, sizeof(uint32));
            width = menu_fire.width;
            ++index;
            words = width >> 2;
            cursor += 4;
        } while (index < words);
    }
    uint32 result = RandomRange(menu_fire.intensity >> 1);
    uint32 intensity = (menu_fire.intensity >> 1) + result;
    while (sparks)
    {
        result = RandomRange(menu_fire.width >> 1);
        height = menu_fire.height;
        width = menu_fire.width;
        result += width >> 2;
        row_offset = (height - 1) * width;
        buffer[row_offset + result] = intensity;
        height = menu_fire.height;
        width = menu_fire.width;
        row_offset = (height - 1) * width;
        buffer[row_offset + result + 1] = intensity;
        height = menu_fire.height;
        width = menu_fire.width;
        row_offset = (height - 1) * width;
        --sparks;
        buffer[row_offset + result - 1] = intensity;
    }
    return result;
}

uint32 SetMenuDarkRedText()
{
    FUNCTION_MARKER(0x80011594, "SLUS_003.73");
    return SetHudTextColor(149, 20, 20, 0);
}

const char *ReadMenuButtonLabel(uint32 button)
{
    const char *label;
    FUNCTION_MARKER(0x800117D8, "SLUS_003.73");
    if (button == 2)
        label = MenuText::r2;
    else if (button == 4)
        label = MenuText::l1;
    else if (button == 8)
        label = MenuText::r1;
    else if (button == 1)
        label = MenuText::l2;
    else
        label = MenuText::empty;
    return label;
}

sint32 ReadMenuNavigationStep()
{
    uint32 left, right, counter;
    sint32 direction = 0;
    FUNCTION_MARKER(0x8001173C, "SLUS_003.73");
    left = game_pads[0].buttons[8].down;
    if (!left)
    {
        right = game_pads[0].buttons[9].down;
        if (!right)
        {
            menu_runtime.navigation_ticks = 0;
            return 0;
        }
    }
    counter = menu_runtime.navigation_ticks;
    if (!counter || ((sint32)counter >= 11 && !(counter & 1)))
    {
        right = game_pads[0].buttons[9].down;
        if (right)
            direction = 1;
        if (left)
            direction = -1;
    }
    counter = menu_runtime.navigation_ticks;
    menu_runtime.navigation_ticks = (uint32)(counter + 1);
    return direction;
}

sint32 AssignMenuButton(uint32 button, sint16 *sel, sint16 *binding0, sint16 *binding1, sint16 *binding2)
{
    FUNCTION_MARKER(0x80011860, "SLUS_003.73");
    *sel = button;
    sint32 val0 = *binding0;
    sint32 val1 = *binding1;
    sint32 val2 = *binding2;
    uint32 available = ~(button | (uint32)val0 | (uint32)val1 | (uint32)val2) & 15;
    if (available & 8)
        available = 8;
    if (available & 4)
        available = 4;
    if (available & 2)
        available = 2;
    if (available & 1)
        available = 1;
    if (button == (uint32)val0)
        *binding0 = available;
    val1 = *binding1;
    if (button == (uint32)val1)
        *binding1 = available;
    val2 = *binding2;
    if (button == (uint32)val2)
        *binding2 = available;
    return val2;
}

// Native extraction retains the signed multiply-high quotient sequence
static sint32 ReadMenuSliderWidth(uint16 volume, uint32 multiplier, uint32 shift)
{
    sint32 level = (sint16)volume;
    uint32 product = (uint32)level * 296;
    sint64 wide = (sint64)(sint32)product * (sint64)(sint32)multiplier;
    uint32 high = (uint32)((uint64)wide >> 32);
    sint32 sum = (sint32)(high + product);
    return (sint32)((uint32)(sum >> shift) - (uint32)((sint32)product >> 31));
}

void DrawMenuVolumeSlider(MenuWindow &menu)
{
    FUNCTION_MARKER(0x800115C0, "SLUS_003.73");
    if (menu.IsSelected(MenuText::return_to_options_menu))
        return;
    sint32 width = 0;
    bool width_known = false;
    if (menu.IsSelected(MenuText::fx_level))
    {
        width = ReadMenuSliderWidth(game_settings.effects_volume, 0x80020009, 13);
        width_known = true;
    }
    if (menu.IsSelected(MenuText::music_level))
    {
        width = ReadMenuSliderWidth(game_settings.music_volume, 0x81020409, 6);
        width_known = true;
    }
    if (menu.IsSelected(MenuText::voice_level))
    {
        width = ReadMenuSliderWidth(game_settings.voice_volume, 0x80808081, 7);
        width_known = true;
    }
    if (!width_known)
    {
        // TODO Original unmatched-item branch reads an undefined saved width
        GameDiagnostic("%s", "Undefined original menu volume slider selection\n");
        abort();
    }
    AllocateHudRectangle((uint32)width + 108, 41, 296 - (uint32)width, 10, 0, 0, 0, 0);
    AllocateHudRectangle(106, 40, 300, 12, 100, 0, 0, 0);
}
