#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pause_menu.h"
#include "cd_voice_event.h"
#include "level_session.h"
#include "scene_lifecycle.h"
#include "game_tuning.h"
#include "input_recording.h"
#include "game_pad_input.h"
#include "game_settings.h"
#include "game_tick.h"
#include "resource_script.h"
#include "player.h"
#include "weapon_type.h"
#include "weapon_node.h"
#include "visual_object.h"
#include "cheat_input.h"
#include "resource_io.h"
#include "game_diagnostics.h"
#include "game_panel.h"
#include "menu_window.h"
#include "hud_text.h"
#include "hud_sprites.h"
#include "record_data.h"
#include "model_bank.h"
#include "effect_lifecycle.h"
#include "game_heap.h"
#include "global.h"
#include "effect_object.h"
#include "texture_resources.h"

static uint32 SeedMenuFire(uint32 sparks);
static uint32 DrawMenuRightHint(void);

struct MenuFireBuffers
{
    uint8 *buffers[2];
    bool sel;
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

static const TextureGroup *panel_grp;

void ForgetPanelTex(const TextureGroup *grp)
{
    if (panel_grp == grp)
        panel_grp = 0;
}

void LoadPanelResource()
{
    FUNCTION_MARKER(0x8006CDA0, "SLUS_003.73");
    panel_grp = TextureGroup::Find("Panel");
}

uint32 ResetPanelDispTimer()
{
    FUNCTION_MARKER(0x8006CDC4, "SLUS_003.73");
    panel_text.lives_timer = 150;
    return 150;
}

uint32 DrawDebugPanel()
{
    RecordDataBorrow rec_borrow;
    const char *text = "";
    ModelInstance *player;
    uint16 trigger;
    uint32 numerator, denominator, percentage;
    Vector3 pos;
    sint32 packet_usage;
    FUNCTION_MARKER(0x8006CDD4, "SLUS_003.73");
    if (!GetCheatOptions().debug_panel)
        return 0;
    trigger = GetStartRecIdx();
    if (trigger != 0xFFFF && GetLevelRec(trigger).Read16() == 8)
        text = (ReadRecPos(&pos, trigger) + 6).CStr();
    QueueHudText(24, 193, text, 32, 32, 114, 1);
    player = visual_lists.players;
    QueueHudNumber(23, 205, player->pos.x >> 12, 32, 32, 114, 1);
    player = visual_lists.players;
    QueueHudNumber(90, 205, player->pos.y >> 12, 32, 32, 114, 1);
    player = visual_lists.players;
    QueueHudNumber(152, 205, player->pos.z >> 12, 32, 32, 114, 1);
    packet_usage = scene_render.used * 100;
    // The original HUD uses a 110592-byte budget
    percentage = packet_usage / 110592;
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

static void DrawWeaponAmmoIndicator(uint32 slot)
{
    Player *hero;

    struct Position
    {
        sint16 x, y;
    };

    static const Position positions[8] = {{0, 0}, {414, 37}, {424, 32}, {439, 32}, {449, 37}, {454, 43}, {454, 49}, {449, 55}};
    uint8 *packet, *bar, *next, *limit;
    sint32 ammo, extent;
    sint16 x, y;
    WeaponNode *weapon;
    FUNCTION_MARKER(0x8006D704, "SLUS_003.73");
    hero = visual_lists.players ? static_cast<Player *>(visual_lists.players->owner) : 0;
    if (!hero)
        return;
    if (slot >= 8 || !slot)
        return;
    weapon = hero->weapons[slot];
    if (!weapon)
        return;
    x = positions[slot].x;
    y = positions[slot].y;
    if (hd_ui_collecting())
    {
        extent = weapon->ammo / 125;
        if (extent)
            AllocateHudRect(x, y, extent, 1, 0, 255, 0, 0);
        AllocateHudRect(x, y, 8, 1, 0, 64, 0, 0);
        if (hero->sel_weapon == slot)
            AllocateHudRect(x - 3, y - 1, 13, 4, 128, 128, 128, 0);
        return;
    }
    packet = scene_render.cursor;
    limit = scene_render.limit;
    if (limit - packet < (sint32)(2 * sizeof(LINE_F2)))
        return;
    next = packet + 2 * sizeof(LINE_F2);
    bar = packet + sizeof(LINE_F2);
    ammo = weapon->ammo;
    scene_render.cursor = next;
    extent = ammo / 125;
    if (extent)
    {
        LINE_F2 *ammo_line = (LINE_F2 *)bar;
        setLineF2(ammo_line);
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
        AllocateHudRect(x - 3, y - 1, 13, 4, 128, 128, 128, 0);
}

void DrawGamePanel()
{
    Player *hero = 0;
    const char *text;
    uint32 timer, kind;
    WeaponNode *weapon;
    uint32 idx, g;
    sint32 x, y, percentage, width, product, height;
    sint32 count, maximum, numerator, sine;
    const TextureFrame *tex;
    FUNCTION_MARKER(0x8006D91C, "SLUS_003.73");
    timer = panel_text.lives_timer;
    if (timer)
    {
        tex = &TextureGroup::GetFrame(panel_grp, 1);
        panel_text.lives_timer = timer - 1;
        DrawHudSprite(40, 27, TextureGroup::GetFrame(panel_grp, 1), false, 0x808080);
        if (panel_text.lives_timer)
        {
            if (GetCheatOptions().unlimited_lives)
                text = MenuText::infinite_lives;
            else
            {
                sint32 lives = game_progress.lives - 1;
                sint32 tens = lives / 10;
                text = panel_text.lives;
                panel_text.lives[0] = tens ? tens + '0' : ' ';
                panel_text.lives[1] = lives % 10 + '0';
            }
            SetHudTextScale(256);
            SetHudTextColor(128, 128, 128, 0);
            SetHudTextAlignment(1);
            DrawStyledHudText(65, 27, text, 0, 0, 256);
        }
    }
    if (scene_frame.paused)
        return;
    bool demo = game_session.demo_mode != 0;
    hero = visual_lists.players ? static_cast<Player *>(visual_lists.players->owner) : 0;
    if (demo)
    {
        SetHudTextScale(256);
        SetHudTextColor(128, 128, 128, 0);
        DrawStyledHudText(58, 50, MenuText::demo, 0, 0, 256);
    }
    if (!GetCheatOptions().status_hud)
        return;
    if (!hero)
    {
        GameDiagnostic(("Game panel has no player\n"));
        abort();
    }
    count = hero->smartbombs;
    if (count >= 16)
        count = 15;
    x = 412;
    for (sint32 item = 0; item < count && item < 8; ++item)
    {
        DrawHudSprite(x, 25, TextureGroup::GetFrame(panel_grp, 10), true, 0x808080);
        x += 8;
    }
    if (count >= 9)
    {
        x -= 8;
        y = 30;
        count -= 8;
        for (sint32 item = 0; item < count; ++item)
        {
            DrawHudSprite(x, y, TextureGroup::GetFrame(panel_grp, 10), true, 0x808080);
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
    DrawHudSprite(436, 46, TextureGroup::GetFrame(panel_grp, 0), true, 0x5A5A5A);
    hero = visual_lists.players ? static_cast<Player *>(visual_lists.players->owner) : 0;
    if (hero)
    {
        weapon = hero->weapons[hero->sel_weapon];
        kind = weapon ? weapon->type : 0;
        tex = &TextureGroup::GetFrame(panel_grp, 0);
        switch (kind)
        {
            case WPN_FLAMETHROWER:
                tex = &TextureGroup::GetFrame(panel_grp, 9);
                break;
            case WPN_PULSE_LASER:
                tex = &TextureGroup::GetFrame(panel_grp, 3);
                break;
            case WPN_PARTICLE_BEAM:
                tex = &TextureGroup::GetFrame(panel_grp, 5);
                break;
            case WPN_RIP_LASER:
                tex = &TextureGroup::GetFrame(panel_grp, 6);
                break;
            case WPN_GRENADES:
                tex = &TextureGroup::GetFrame(panel_grp, 4);
                break;
            case WPN_HOMING_MISSILES:
                tex = &TextureGroup::GetFrame(panel_grp, 8);
                break;
            case WPN_ROCKETS:
                tex = &TextureGroup::GetFrame(panel_grp, 7);
                break;
            case WPN_MACHINE_GUN:
                tex = 0;
                AllocateHudRect(425, 39, 22, 14, 40, 40, 40, 0);
                break;
            case WPN_SPIRAL_RAY:
                tex = 0;
                break;
            default:
                break;
        }
        if (tex)
        {
            DrawHudSprite(436, 46, *tex, true, 0x808080);
        }
    }
    if (!hero)
    {
        GameDiagnostic(("Game panel lost its player\n"));
        abort();
    }
    maximum = (sint16)GetGameTuning().player.maximum_health;
    numerator = (maximum - hero->health) * 100;
    percentage = DivideSignedWord(numerator, maximum);
    product = percentage * 80;
    width = product / 100;
    AllocateHudRect(469 - width, 16, width, 6, 0, 0, 0, 0);
    if (width < 41)
        QueueHudGradientRect(429, 16, 40, 6, 0, 255, 0, 255, 255, 0, 0);
    if (percentage >= 61)
    {
        product = (percentage - 60) * 350;
        sint32 pulse = product / 40;
        idx = ((pulse + 50) * game_clock.tick) & 4095;
        sine = rsin(idx);
        product = sine * sine;
        g = product >> 16;
        if (g >= 256)
            g = 255;
        QueueHudGradientRect(389, 16, 40, 6, 255, g, 0, 255, g, 0, 0);
    }
    else
        QueueHudGradientRect(389, 16, 40, 6, 255, 255, 0, 255, 0, 0, 0);
    weapon = hero->weapons[hero->sel_weapon];
    if (weapon)
    {
        if (weapon->type == WPN_MACHINE_GUN)
        {
            y = 25;
            height = 50;
        }
        else
        {
            sint32 missing_ammo = 1000 - weapon->ammo;
            height = missing_ammo / 20;
            AllocateHudRect(474, 25, 10, height, 0, 0, 0, 0);
            y = height + 25;
            height = 50 - height;
        }
        AllocateHudRect(474, y, 10, height, 0, 100, 200, 0);
    }
    VisualObject::DrawTrackedHealth();
}

static void DrawPauseMenuHeading()
{
    FUNCTION_MARKER(0x8001664C, "SLUS_003.73");
    SetHudTextScale(512);
    SetHudTextColor(149, 20, 20, 0);
    DrawStyledHudText(256, 34, MenuText::paused, 0, 0, 256);
    SetHudTextScale(256);
}

void DrawPauseMenuDisp()
{
    FUNCTION_MARKER(0x80016884, "SLUS_003.73");
    bool blink;
    ModelInstance *player;
    uint32 amplitude, phase, countdown;
    sint32 text_count;
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
    const sint32 caption_capacity = sizeof(caption);
    if (GetCheatOptions().debug_panel)
        /* MIPS 800168B0..800168C0 stores zero for all four stack arguments */
        AllocateHudRect(19, 190, 194, 42, 0, 0, 0, 0);
    if (scene_frame.paused)
    {
        ::UpdateHudTextGlowCurve();
    }
    SetHudTextAlignment(0);
    if (input_recording.HasBuffer() && ((game_clock.vblank_frame & 0x20) != 0))
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
        DrawStyledHudText(256, panel_text.game_over_y, MenuText::game_over, 0, 128, 256);
        panel_text.game_over_y += 8;
        if (panel_text.game_over_y >= 121)
            panel_text.game_over_y = 120;
    }
    SetHudTextScale(256);
    switch (menu_runtime.screen)
    {
        case MENU_SCREEN_PAUSED:
            DrawPauseMenuHeading();
            amplitude = menu_runtime.shake_amplitude;
            shake_remaining = amplitude - 2;
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
                sint32 product = menu_runtime.shake_amplitude * rcos(phase & 4095);
                pause_window = menu_runtime.pause;
                pause_window->scr_pos.x = product / 4096 + 256;
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
            window->scr_pos.y = 135;
            window->Draw();
            return;

        case MENU_SCREEN_CONTROLLER_MISSING:
            DrawPauseMenuHeading();
            MenuWindow::SetRedText();
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
            if (text_count < 0 || text_count >= caption_capacity)
            {
                GameDiagnostic(("Menu text length exceeds original 104-byte buffer\n"));
                abort();
            }
            i = 0;
            if (text_count > 0)
            {
                do
                {
                    if (i >= caption_capacity - 1)
                    {
                        GameDiagnostic(("Menu text grew beyond original buffer\n"));
                        abort();
                    }
                    caption[i] = panel_text.caption[i];
                    ++i;
                    text_count = panel_text.visible;
                } while (i < text_count);
            }
            text_count = panel_text.visible;
            if (text_count < 0 || text_count >= caption_capacity)
            {
                GameDiagnostic(("Menu text terminator exceeds original buffer\n"));
                abort();
            }
            caption[text_count] = 0;
            SetHudTextScale(192);
            SetHudTextColor(128, 128, 128, 0);
            SetHudTextAlignment(1);
            caption_width = MeasureStyledHudText(panel_text.caption);
            DrawStyledHudText(256 - (caption_width >> 1), 34, caption, 0, 0, 256);
            return;

        case MENU_SCREEN_RESULTS_PERCENT:
            goto DrawResults;

        case MENU_SCREEN_SAVE_CONFIRM:
            SetHudTextColor(149, 20, 20, 0);
            DrawStyledHudText(256, 103, MenuText::save_game_prompt, 0, 0, 256);
            confirm_window = menu_runtime.confirm;
            confirm_window->scr_pos.y = 143;
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
            if (game_progress.completion.disp_percent == 100)
            {
                countdown = game_progress.completion.blink_ticks;
                if (countdown)
                    game_progress.completion.blink_ticks = countdown - 1;
                blink = (game_progress.completion.blink_ticks >> 2) & 1;
                if (blink)
                    return;
            }
            snprintf(percent, sizeof(percent), " %d", (sint32)game_progress.completion.disp_percent);
            message_x = 270;
            message_y = 68;
            message = percent;
            goto DrawMessage;

        case MENU_SCREEN_CARD_POLL_DELAY:

        case MENU_SCREEN_CARD_QUERY:
            MenuWindow::SetGreenText();
            DrawStyledHudText(256, 90, MenuText::checking_memory_card, 0, 0, 256);
            MenuWindow::SetRedText();
            DrawStyledHudText(256, 140, MenuText::do_not_remove, 0, 0, 256);
            prompt = MenuText::memory_card;
            prompt_y = 166;
            goto DrawPrompt;

        case MENU_SCREEN_SAVE_SLOT:
            DrawMenuHints();
            MenuWindow::SetRedText();
            DrawStyledHudText(256, 25, MenuText::save_game, 0, 0, 256);
            window = menu_runtime.save;
            window->Draw();
            return;

        case MENU_SCREEN_SAVE_OVERWRITE:
            DrawMenuHints();
            MenuWindow::SetGreenText();
            DrawStyledHudText(256, 40, MenuText::okay_to_overwrite, 0, 0, 256);
            DrawStyledHudText(256, 66, MenuText::previously_saved_game, 0, 0, 256);
            DrawStyledHudText(256, 92, MenuText::in_this_save_slot, 0, 0, 256);
            window = menu_runtime.confirm;
            window->Draw();
            return;

        case MENU_SCREEN_CARD_WRITE:
            MenuWindow::SetGreenText();
            DrawStyledHudText(256, 110, MenuText::now_saving_data, 0, 0, 256);
            MenuWindow::SetRedText();
            DrawStyledHudText(256, 140, MenuText::do_not_remove, 0, 0, 256);
            prompt = MenuText::memory_card;
            prompt_y = 166;
            goto DrawPrompt;

        case MENU_SCREEN_CARD_WRITE_ERROR:
            MenuWindow::SetRedText();
            DrawStyledHudText(256, 110, MenuText::attempt_to_write_to, 0, 0, 256);
            error_y = 136;
            error_prompt = MenuText::memory_card_failed;
            goto DrawCardError;

        case MENU_SCREEN_CARD_WRITE_COMPLETE:
            MenuWindow::SetGreenText();
            DrawStyledHudText(256, 110, MenuText::save_completed, 0, 0, 256);
            prompt_y = 140;
            goto ShowAcknowledgePrompt;

        case MENU_SCREEN_CARD_FULL:
            MenuWindow::SetRedText();
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
            MenuWindow::SetRedText();
            DrawStyledHudText(256, 90, MenuText::please_insert_a_memory, 0, 0, 256);
            DrawStyledHudText(256, 116, MenuText::card_in_memory_card, 0, 0, 256);
            DrawStyledHudText(256, 142, MenuText::slot_1_or_press_any, 0, 0, 256);
            prompt = MenuText::button_to_cancel;
            prompt_y = 168;
            goto DrawPrompt;

        case MENU_SCREEN_CARD_FORMAT_CONFIRM:
            DrawMenuHints();
            MenuWindow::SetRedText();
            DrawStyledHudText(256, 35, MenuText::the_memory_card_in, 0, 0, 256);
            DrawStyledHudText(256, 61, MenuText::memory_card_slot_1_is, 0, 0, 256);
            DrawStyledHudText(256, 87, MenuText::unformatted_do_you, 0, 0, 256);
            DrawStyledHudText(256, 113, MenuText::want_to_format_it, 0, 0, 256);
            window = menu_runtime.confirm;
            window->scr_pos.y = 148;
            window->Draw();
            return;

        case MENU_SCREEN_CARD_FORMAT:
            MenuWindow::SetGreenText();
            if (((sint32)game_clock.tick / 10) & 1)
                DrawStyledHudText(256, 110, MenuText::formatting_memory_card, 0, 0, 256);
            MenuWindow::SetRedText();
            DrawStyledHudText(256, 140, MenuText::do_not_remove, 0, 0, 256);
            prompt = MenuText::memory_card;
            prompt_y = 166;
            goto DrawPrompt;

        case MENU_SCREEN_CARD_FORMAT_ERROR:
            MenuWindow::SetRedText();
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
            MenuWindow::SetGreenText();
            if (((sint32)game_clock.tick / 10) & 1)
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

uint32 DrawMenuLeftHint(void)
{
    FUNCTION_MARKER(0x800166AC, "SLUS_003.73");
    SetHudTextAlignment(1);
    SetHudTextScale(192);
    SetHudTextColor(149, 20, 20, 0);
    DrawStyledHudText(92, 210, MenuText::go_back, 0, 0, 256);
    SetHudTextAlignment(0);
    uint32 result = 0;
    result = DrawHudSprite(64, 211, TextureGroup::GetFrame(EffectObject::GetButtonResourceGrp(), 1), true, 0x808080);
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
    uint32 result = 0;
    result = DrawHudSprite(320, 211, TextureGroup::GetFrame(EffectObject::GetButtonResourceGrp(), 0), true, 0x808080);
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
    return DrawHudSprite(199, 214, TextureGroup::GetFrame(EffectObject::GetButtonResourceGrp(), 0), true, 0x808080);
}

uint32 HasMenuBuffers()
{
    FUNCTION_MARKER(0x8002E13C, "SLUS_003.73");
    return menu_fire.buffers[0] != 0;
}

uint32 CreateMenuBuffers(uint32 resource, uint16 width, uint16 height, uint8 intensity)
{
    FUNCTION_MARKER(0x8002E148, "SLUS_003.73");
    uint32 result = HasMenuBuffers();
    if (result)
        return result;
    const TextureResource &tex = TextureResource::Get(resource);
    uint32 page = tex.tpage;
    uint32 pixel_shift = (2 - ((page & 384) >> 7)) & 31;
    uint32 tex_x = tex.uv[0].u;
    menu_fire.rectangle.x = ((page & 15) << 6) + (tex_x >> pixel_shift);
    uint32 tex_y = tex.uv[0].v;
    menu_fire.rectangle.w = width >> 1;
    menu_fire.width = width;
    menu_fire.rectangle.h = height;
    menu_fire.height = height;
    menu_fire.intensity = intensity;
    menu_fire.rectangle.y = tex_y + ((page << 4) & 256);
    if (!height || width < 4 || (width & 3))
    {
        GameDiagnostic(("Invalid menu fire dimensions: %u x %u\n", width, height));
        abort();
    }
    uint32 bytes = (uint32)width * height;
    uint8 *allocation = (uint8 *)calloc(height, 2 * width);
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
    uint8 *sel, *other;
    bool toggle;
    uint32 row;
    sel = menu_fire.buffers[0];
    if (!sel)
        return 0;
    toggle = !menu_fire.sel;
    menu_fire.sel = toggle;
    sel = menu_fire.buffers[toggle];
    other = menu_fire.buffers[!toggle];
    for (row = 1; row + 1 < menu_fire.height; ++row)
    {
        uint16 stride = menu_fire.width;
        const uint8 *top_row = sel + (row - 1) * stride;
        const uint8 *bottom_row = sel + (row + 1) * stride;
        uint8 top[4] = {top_row[0], top_row[1], top_row[2], top_row[3]};
        uint8 bottom[4] = {bottom_row[0], bottom_row[1], bottom_row[2], bottom_row[3]};
        uint32 carry = 0;
        sint32 column = 1;
        for (;;)
        {
            uint8 output[4];
            output[0] = carry;
            sint32 next_column = column + 2;
            uint32 top_offset;
            output[1] = (top[0] + top[2] + bottom[0] + bottom[2]) >> 2;
            output[2] = (top[1] + top[3] + bottom[1] + bottom[3]) >> 2;
            stride = menu_fire.width;
            top_offset = (row - 1) * stride;
            if (next_column >= stride - 1)
                break;
            top_row = sel + top_offset + next_column + 1;
            bottom_row = sel + (row + 1) * stride + next_column + 1;
            uint8 next_top[4] = {top_row[0], top_row[1], top_row[2], top_row[3]};
            uint8 next_bottom[4] = {bottom_row[0], bottom_row[1], bottom_row[2], bottom_row[3]};
            output[3] = (top[2] + next_top[0] + bottom[2] + next_bottom[0]) >> 2;
            uint8 *dst = sel + row * stride + next_column - 3;
            dst[0] = output[0];
            dst[1] = output[1];
            dst[2] = output[2];
            dst[3] = output[3];
            dst = other + top_offset + next_column - 3;
            dst[0] = output[0];
            dst[1] = output[1];
            dst[2] = output[2];
            dst[3] = output[3];
            carry = (top[3] + next_top[1] + bottom[3] + next_bottom[3]) >> 2;
            top[0] = next_top[0];
            top[1] = next_top[1];
            top[2] = next_top[2];
            top[3] = next_top[3];
            bottom[0] = next_bottom[0];
            bottom[1] = next_bottom[1];
            bottom[2] = next_bottom[2];
            bottom[3] = next_bottom[3];
            column = next_column + 2;
        }
    }
    SeedMenuFire(4);
    return LoadImagePSX(&menu_fire.rectangle, (uint32 *)sel);
}

static uint32 SeedMenuFire(uint32 sparks)
{
    FUNCTION_MARKER(0x8002E4D8, "SLUS_003.73");
    uint32 height = menu_fire.height;
    uint32 width = menu_fire.width;
    uint32 row_offset = (height - 1) * width;
    uint8 *buffer = menu_fire.buffers[menu_fire.sel];
    uint8 *cursor = buffer + row_offset;
    uint32 words = width >> 2;
    if (words)
    {
        uint32 idx = 0;
        do
        {
            memset(cursor, 0, sizeof(uint32));
            width = menu_fire.width;
            ++idx;
            words = width >> 2;
            cursor += 4;
        } while (idx < words);
    }
    uint32 result = RandomRange(menu_fire.intensity >> 1);
    uint8 intensity = (menu_fire.intensity >> 1) + result;
    while (sparks)
    {
        result = RandomRange(menu_fire.width >> 1);
        height = menu_fire.height;
        width = menu_fire.width;
        result += width >> 2;
        row_offset = (height - 1) * width;
        buffer[row_offset + result] = intensity;
        buffer[row_offset + result + 1] = intensity;
        --sparks;
        buffer[row_offset + result - 1] = intensity;
    }
    return result;
}

// Native extraction retains the signed multiply-high quotient sequence
