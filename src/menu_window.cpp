#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "texture_resources.h"
#include "effect_object.h"
#include "global.h"
#include "game_heap.h"
#include "effect_lifecycle.h"
#include "model_bank.h"
#include "record_data.h"
#include "hud_sprites.h"
#include "game_panel.h"
#include "resource_io.h"
#include "cheat_input.h"
#include "visual_object.h"
#include "weapon_type.h"
#include "player.h"
#include "resource_script.h"
#include "game_tick.h"
#include "game_settings.h"
#include "input_recording.h"
#include "game_tuning.h"
#include "level_session.h"
#include "cd_voice_event.h"
#include "pause_menu.h"
#include "scene_lifecycle.h"
#include "game_pad_input.h"
#include "game_diagnostics.h"
#include "menu_window.h"
#include "hud_text.h"
#include "audio_session.h"
#include "card_save.h"
#include "level_catalogue.h"
#include "player_save_state.h"

namespace MenuText
{
const char autotest[] = "Autotest";
const char build_date[] = "Oct 13 1998";
const char main_menu[] = "main menu";
const char game_over[] = "game over";
const char press_start[] = "Press start";
const char continue_game[] = "Continue";
const char save_game_prompt[] = "Save game ?";
const char save_game[] = "Save game";
const char please_reinsert[] = "Please reinsert";
const char controller[] = "controller";
const char select_level[] = "select level";
const char skip_to_restart[] = "skip to restart";
const char press_any_button[] = "press any button";
const char really_quit[] = "really quit ?";
const char really_reset_to_defaults[] = "really reset to defaults ?";
const char return_to_options_menu[] = "return to options menu";
const char fx_level[] = "fx level";
const char music_level[] = "music level";
const char voice_level[] = "voice level";
const char read_error[] = "read error";
const char new_game[] = "new game";
const char load_game[] = "load game";
const char difficulty[] = "difficulty";
const char controller_config[] = "controller config";
const char misc_options[] = "misc options";
const char return_to_main_menu[] = "return to main menu";
const char view_credits[] = "view credits";
const char music_credits[] = "music credits";
const char view_intro[] = "view intro";
const char reset_to_defaults[] = "reset to defaults";
const char center_scr[] = "center screen";
const char calibrate_analog_controller[] = "calibrate analog controller";
const char vibration_settings[] = "vibration settings";
const char return_to_controller_config[] = "return to controller config";
const char use_the_directional[] = "Use the directional";
const char buttons_to_center[] = "buttons to center";
const char the_scr[] = "the screen";
const char move_the_left_stick[] = "Move the left stick";
const char and_right_stick_to_all[] = "and right stick to all";
const char four_corners_then_press[] = "four corners then press";
const char the_x_button[] = "the x button";
const char center_both_the_left_stick[] = "center both the left stick";
const char and_the_right_stick_then[] = "and the right stick then";
const char press_the_x_button[] = "press the X button";
const char weapon_select[] = "weapon select";
const char please_insert_a_memory[] = "Please insert a memory";
const char card_in_memory_card[] = "card in memory card";
const char slot_1_or_press_any[] = "slot 1 or press any";
const char button_to_cancel[] = "button to cancel";
const char the_memory_card_in[] = "The memory card in";
const char memory_card_slot_1_is[] = "memory card slot 1 is";
const char unformatted_press[] = "unformatted. Press";
const char any_button_to_cancel[] = "any button to cancel.";
const char unformatted_do_you[] = "unformatted. Do you";
const char want_to_format_it[] = "want to format it ?";
const char formatting_memory_card[] = "formatting memory card";
const char do_not_remove[] = "do not remove";
const char memory_card[] = "memory card";
const char checking_memory_card[] = "checking memory card";
const char your_memory_card_in[] = "your memory card in";
const char memory_card_slot_1_is_full[] = "memory card slot 1 is full";
const char insert_a_memory_card_with[] = "insert a memory card with";
const char at_least_1_free_block_or_delete[] = "at least 1 free block or delete";
const char a_block_using_the_console[] = "a block using the console";
const char internal_memory_card_manager[] = "internal memory card manager";
const char okay_to_overwrite[] = "Okay to overwrite";
const char previously_saved_game[] = "previously saved game";
const char in_this_save_slot[] = "in this save slot ?";
const char now_saving_data[] = "Now Saving data";
const char attempt_to_write_to[] = "attempt to write to";
const char memory_card_failed[] = "memory card failed !";
const char format_failed[] = "format failed !";
const char try_again_or_insert[] = "try again or insert";
const char a_new_memory_card[] = "a new memory card";
const char format_completed[] = "format completed";
const char save_completed[] = "save completed";
const char memory_card_contains[] = "memory card contains";
const char no_apocalypse_game_save[] = "no apocalypse game save";
const char to_cancel[] = "to cancel";
const char smart_bomb[] = "Smart Bomb";
const char empty[] = "";
const char paused[] = "Paused";
const char kill_percent[] = "Kill %";
const char quit[] = "Quit";
const char demo[] = "Demo";
const char go_back[] = "go back";
const char select[] = "select";
const char accept[] = "accept";
const char cancel[] = "cancel";
const char infinite_lives[] = "inf";
const char yes[] = "yes";
const char no[] = "no";
const char test[] = "test";
const char cycle[] = "cycle";
const char l2[] = "l2";
const char r2[] = "r2";
const char l1[] = "l1";
const char r1[] = "r1";
const char options[] = "options";
const char sound[] = "sound";
const char none[] = "none";
const char some[] = "some";
const char full[] = "full";
const char easy[] = "easy";
const char normal[] = "normal";
const char hard[] = "hard";
const char crouch[] = "crouch";
const char jump[] = "jump";
const char empty_slot[] = "Empty";
} // namespace MenuText

static uint32 menu_repeat;

void MenuWindow::ResetRepeat()
{
    menu_repeat = 0;
}

uint32 MenuWindow::SetDarkRedText()
{
    FUNCTION_MARKER(0x80011594, "SLUS_003.73");
    return SetHudTextColor(149, 20, 20, 0);
}

MenuWindow::~MenuWindow()
{
    for (uint32 idx = 0; idx < 15; ++idx)
        free(items[idx].owned_label);
}

void MenuWindow::DrawVolumeSlider()
{
    FUNCTION_MARKER(0x800115C0, "SLUS_003.73");
    if (IsSel(MenuText::return_to_options_menu))
        return;
    sint32 width = 0;
    bool width_known = false;
    if (IsSel(MenuText::fx_level))
    {
        width = MenuWindow::ReadSliderWidth(game_settings.effects_volume, 0x80020009, 13);
        width_known = true;
    }
    if (IsSel(MenuText::music_level))
    {
        width = MenuWindow::ReadSliderWidth(game_settings.music_volume, 0x81020409, 6);
        width_known = true;
    }
    if (IsSel(MenuText::voice_level))
    {
        width = MenuWindow::ReadSliderWidth(game_settings.voice_volume, 0x80808081, 7);
        width_known = true;
    }
    if (!width_known)
    {
        // TODO Original unmatched-item branch reads an undefined saved width
        GameDiagnostic(("%s", "Undefined original menu volume slider selection\n"));
        abort();
    }
    AllocateHudRect((uint32)width + 108, 41, 296 - (uint32)width, 10, 0, 0, 0, 0);
    AllocateHudRect(106, 40, 300, 12, 100, 0, 0, 0);
}

sint32 MenuWindow::ReadNavigationStep()
{
    bool left, right;
    uint32 counter;
    sint32 dir = 0;
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
            dir = 1;
        if (left)
            dir = -1;
    }
    counter = menu_runtime.navigation_ticks;
    menu_runtime.navigation_ticks = counter + 1;
    return dir;
}

const char *MenuWindow::GetButtonLabel(uint32 button)
{
    FUNCTION_MARKER(0x800117D8, "SLUS_003.73");
    if (button == 2)
        return MenuText::r2;
    if (button == 4)
        return MenuText::l1;
    if (button == 8)
        return MenuText::r1;
    if (button == 1)
        return MenuText::l2;
    return MenuText::empty;
}

sint16 MenuWindow::AssignButton(uint32 button, sint16 *sel, sint16 *binding0, sint16 *binding1, sint16 *binding2)
{
    FUNCTION_MARKER(0x80011860, "SLUS_003.73");
    *sel = button;
    sint16 val0 = *binding0;
    sint16 val1 = *binding1;
    sint16 val2 = *binding2;
    uint32 available = ~(button | val0 | val1 | val2) & 15;
    if (available & 8)
        available = 8;
    if (available & 4)
        available = 4;
    if (available & 2)
        available = 2;
    if (available & 1)
        available = 1;
    if (button == val0)
        *binding0 = available;
    val1 = *binding1;
    if (button == val1)
        *binding1 = available;
    val2 = *binding2;
    if (button == val2)
        *binding2 = available;
    return val2;
}

const char *MenuWindow::GetItemLabel(uint32 idx) const
{
    if (idx >= 15)
    {
        GameDiagnostic(("Menu item index out of bounds\n"));
        abort();
    }
    return items[idx].label;
}

const char *MenuWindow::GetSelLabel() const
{
    return GetItemLabel(sel);
}

uint32 MenuWindow::SetRedText(void)
{
    FUNCTION_MARKER(0x800151D0, "SLUS_003.73");
    return SetHudTextColor(255, 0, 0, 0);
}

uint32 MenuWindow::SetGreenText(void)
{
    FUNCTION_MARKER(0x800151FC, "SLUS_003.73");
    return SetHudTextColor(0, 255, 0, 0);
}

uint32 MenuWindow::AddOwnedItem(const char *label)
{
    uint32 idx = count;
    uint32 result = AddItem(label);
    size_t len = strlen(label);
    char *copy = (char *)malloc(len + 1);
    if (!copy)
    {
        GameDiagnostic(("Cannot allocate menu label\n"));
        abort();
    }
    memcpy(copy, label, len + 1);
    items[idx].owned_label = copy;
    items[idx].label = copy;
    return result;
}

MenuWindow::MenuWindow(uint32 x, uint32 y, uint8 alignment, sint16 sel_scale, sint16 unselected_scale, uint32 spacing)
{
    FUNCTION_MARKER(0x80015228, "SLUS_003.73");
    scr_pos.x = x;
    scr_pos.y = y;
    this->alignment = alignment;
    this->spacing = spacing;
    has_arrows = false;
    arrow_item = 0;
    numbered = false;
    count = 0;
    const MenuItem empty = {0};
    for (uint32 idx = 0; idx < 15; ++idx)
        items[idx] = empty;
    for (uint32 idx = 0; idx < 15; ++idx)
    {
        MenuItem *row = items + idx;
        row->pulse = 0;
        row->glow = 0;
        row->color[0].r = 200;
        row->color[0].g = 200;
        row->color[0].b = 200;
        row->color[1].r = 128;
        row->color[1].g = 128;
        row->color[1].b = 128;
        row->brightness = 128;
        row->sel_scale = sel_scale;
        row->unselected_scale = unselected_scale;
    }
    ResetSelection();
}

uint32 MenuWindow::Destroy(uint32 reason)
{
    FUNCTION_MARKER(0x800152C4, "SLUS_003.73");
    if (reason & 1)
        ReleaseStorage();
    return 0;
}

uint32 MenuWindow::CenterVertical(void)
{
    FUNCTION_MARKER(0x800152F8, "SLUS_003.73");
    uint32 rows = count;
    uint32 enabled = 0, height = 0;
    for (uint32 idx = 0; idx < rows; ++idx)
    {
        MenuItem *row = items + idx;
        if (row->enabled)
        {
            ++enabled;
            height += row->height;
        }
    }
    uint32 spacing = this->spacing;
    uint32 span = (enabled - 1) * spacing + height;
    sint32 result = 240 - span;
    result /= 2;
    scr_pos.y = result;
    return result;
}

uint32 MenuWindow::ResetSelection(uint8 sel)
{
    FUNCTION_MARKER(0x80015374, "SLUS_003.73");
    this->sel = sel;
    for (uint32 idx = 0; idx < 15; ++idx)
    {
        MenuItem *row = items + idx;
        uint8 selection = this->sel;
        sint16 scale = idx == selection ? row->sel_scale : row->unselected_scale;
        row->target_scale = scale;
        row->pulse = 0;
        row->glow = 0;
        row->brightness = 128;
        row->scale = scale;
        row->hd_text.initialized = 0;
        row->hd_arrows.initialized = 0;
    }
    return 0;
}

uint32 MenuWindow::ResetSelection(void)
{
    FUNCTION_MARKER(0x800153D8, "SLUS_003.73");
    return ResetSelection(0);
}

uint32 MenuWindow::FindItem(const char *label)
{
    FUNCTION_MARKER(0x800153F8, "SLUS_003.73");
    if (!this->count)
        return 0;
    uint32 idx = 0;
    for (;;)
    {
        if (NamesEqualIgnoringCase(label, GetItemLabel(idx)))
            return idx;
        uint32 count = this->count;
        ++idx;
        if (idx >= count)
            return 0;
    }
}

uint32 MenuWindow::AddItem(const char *label)
{
    FUNCTION_MARKER(0x80015474, "SLUS_003.73");
    uint32 count = this->count;
    if (count >= 15)
    {
        GameDiagnostic(("Menu item capacity exceeded\n"));
        abort();
    }
    MenuItem *row = items + count;
    row->hd_text.initialized = 0;
    row->hd_arrows.initialized = 0;
    row->label = label;
    row->enabled = true;
    row->height = 0;
    uint32 result = this->count + 1;
    this->count = result;
    return result;
}

uint32 MenuWindow::EnableItem(const char *label)
{
    FUNCTION_MARKER(0x800154E0, "SLUS_003.73");
    uint32 idx = FindItem(label);
    items[idx].enabled = true;
    return 1;
}

sint32 MenuWindow::ReadSliderWidth(sint16 volume, uint32 multiplier, uint32 shift)
{
    sint32 product = volume * 296;
    sint64 wide = (sint64)product * (sint32)multiplier;
    uint32 high = (uint64)wide >> 32;
    sint32 sum = high + product;
    return (uint32)(sum >> shift) - (product >> 31);
}

uint32 MenuWindow::DisableItem(const char *label)
{
    FUNCTION_MARKER(0x8001551C, "SLUS_003.73");
    uint32 idx = FindItem(label);
    items[idx].enabled = false;
    return idx;
}

uint32 MenuWindow::SetItemHeight(const char *label, uint8 height)
{
    FUNCTION_MARKER(0x80015590, "SLUS_003.73");
    uint32 idx = FindItem(label);
    items[idx].height = height;
    return idx;
}

bool MenuWindow::IsSel(const char *label)
{
    FUNCTION_MARKER(0x800155D4, "SLUS_003.73");
    return NamesEqualIgnoringCase(label, GetItemLabel(sel));
}

bool MenuWindow::GetItemPos(const char *label, uint32 *x, uint32 *y)
{
    FUNCTION_MARKER(0x80015614, "SLUS_003.73");
    *x = scr_pos.x;
    *y = scr_pos.y;
    bool result = this->count != 0;
    if (!result)
        return result;
    uint32 idx = 0;
    MenuItem *row = items;
    do
    {
        if (row->enabled)
        {
            *y += row->height;
            result = NamesEqualIgnoringCase(label, GetItemLabel(idx));
            if (result)
                return result;
            *y += spacing;
        }
        uint8 count = this->count;
        ++idx;
        result = idx < count;
        ++row;
    } while (result);
    return result;
}

uint32 MenuWindow::Draw(void)
{
    FUNCTION_MARKER(0x800156E0, "SLUS_003.73");
    uint8 row_height, r, g, b, brightness;
    bool enabled, numbered;
    sint16 draw_color, draw_glow;
    sint16 cur_pulse, cur_glow, cur_scale, unselected_scale;
    sint32 idx;
    MenuItem *row;
    uint32 y;
    sint32 result;
    sint16 pulse;
    sint16 glow;
    uint8 fade;
    const char *text;
    size_t len;
    char *suffix;
    sint16 text_scale;
    sint32 width;
    uint32 arrow_offset;
    POLY_F3 *left_poly;
    POLY_F3 *right_poly;
    uint16 left_center;
    sint16 left_edge;
    uint16 right_center;
    sint16 right_edge;
    uint32 *ordering_table;
    char numbered_label[104];
    idx = 0;
    SetHudTextAlignment(alignment);
    row = items;
    y = scr_pos.y;
    while (1)
    {
        result = idx < count;
        if (!result)
            return result;
        row_height = row->height;
        enabled = row->enabled;
        y += row_height;
        if (!enabled)
            goto NextRow;
        if (row->scale > 0)
        {
            if (idx == sel)
            {
                r = row->color[0].r;
                g = row->color[0].g;
                b = row->color[0].b;
                SetHudTextColor(r, g, b, 0);
                cur_pulse = row->pulse;
                pulse = cur_pulse - 16;
                if (cur_pulse < 129)
                {
                WrapSelectedPulse:
                    if (row->pulse < 128)
                        row->pulse = 255;
                }
                else
                {
                    row->pulse = pulse;
                    if (pulse < 128)
                    {
                        row->pulse = 128;
                        goto WrapSelectedPulse;
                    }
                }
                cur_glow = row->glow;
                glow = cur_glow + 32;
                if (cur_glow < 256)
                {
                    row->glow = glow;
                    if (glow >= 257)
                        row->glow = 256;
                }
                row->brightness = 0;
            }
            else
            {
                r = row->color[1].r;
                brightness = row->brightness;
                g = row->color[1].g;
                b = row->color[1].b;
                SetHudTextColor((r * brightness >> 7) & 255, (g * brightness >> 7) & 255, (b * brightness >> 7) & 255, 0);
                fade = row->brightness + 6;
                row->brightness = fade;
                if (fade >= 0x81)
                    row->brightness = 0x80;
                cur_pulse = row->pulse;
                if (cur_pulse < 16)
                    row->pulse = 0;
                else
                    row->pulse = cur_pulse - 16;
                if (row->pulse)
                    row->glow = 256;
                else
                    row->glow = 0;
            }
            cur_scale = row->scale;
            unselected_scale = row->unselected_scale;
            SetHudTextDepth(unselected_scale >= cur_scale);
            numbered = this->numbered;
            text = GetItemLabel(idx);
            if (numbered && !NamesEqualIgnoringCase(text, MenuText::empty_slot))
            {
                len = strlen(text);
                text = numbered_label;
                // Numbered save rows use catalogue titles or the read-error label
                if (len + 3 > sizeof(numbered_label))
                {
                    GameDiagnostic(("TODO Numbered menu label exceeds original local buffer\n"));
                    abort();
                }
                CopyGameString(GetItemLabel(idx), numbered_label);
                numbered_label[len] = 32;
                suffix = numbered_label + len + 1;
                *suffix = idx + 49;
                suffix[1] = 0;
            }
            SetHudTextScale(row->sel_scale);
            if (MeasureStyledHudText(text) < 491)
                text_scale = row->scale;
            else
                text_scale = row->unselected_scale;
            SetHudTextScale(text_scale);
            draw_color = row->pulse;
            draw_glow = row->glow;
            width = DrawStyledHudTextHistory(row->hd_text, scr_pos.x, y, text, 0, draw_color, draw_glow, hud_text_style.scale);
            if ((has_arrows && (idx == arrow_item)) && !alignment)
            {
                cur_scale = row->scale;
                sint32 width_product = (uint32)width * cur_scale;
                arrow_offset = width_product / 512 + cur_scale * 14 / 256;
                if (hd_ui_collecting())
                {
                    hd_ui_arrows(&row->hd_arrows, scr_pos.x, y, arrow_offset);
                }
                else
                {
                    left_poly = (POLY_F3 *)scene_render.cursor;
                    if (scene_render.limit - scene_render.cursor >= (sint32)(2 * sizeof(*left_poly)))
                    {
                        right_poly = left_poly + 1;
                        scene_render.cursor = (uint8 *)(left_poly + 2);
                        setPolyF3(left_poly);
                        setPolyF3(right_poly);
                        setRGB0(left_poly, 255, 0, 0);
                        left_center = scr_pos.x;
                        left_poly->p[0].y = y;
                        left_poly->p[0].x = left_center - arrow_offset;
                        left_poly->p[1].y = y - 6;
                        left_edge = left_poly->p[0].x;
                        left_poly->p[2].y = y + 6;
                        left_edge -= 20;
                        left_poly->p[1].x = left_edge;
                        left_poly->p[2].x = left_edge;
                        setRGB0(right_poly, 255, 0, 0);
                        right_center = scr_pos.x;
                        right_poly->p[0].y = y;
                        right_poly->p[0].x = right_center + arrow_offset;
                        right_poly->p[1].y = y - 6;
                        right_edge = right_poly->p[0].x;
                        ordering_table = scene_render.cur->ot;
                        right_poly->p[2].y = y + 6;
                        right_edge += 20;
                        right_poly->p[1].x = right_edge;
                        right_poly->p[2].x = right_edge;
                        setaddr(left_poly, getaddr(ordering_table));
                        setaddr(ordering_table, gpu_packet_link((uintptr_t)left_poly));
                        setaddr(right_poly, getaddr(ordering_table));
                        setaddr(ordering_table, gpu_packet_link((uintptr_t)right_poly));
                    }
                }
            }
        }
        y += spacing;
    NextRow:
        ++row;

        ++idx;
    }
}

uint32 MenuWindow::Update(void)
{
    uint8 selection, count;
    uint32 repeat, idx;
    bool prev, next;
    MenuItem *row;
    sint16 cur, target;
    sint16 stepped;
    FUNCTION_MARKER(0x80015BA4, "SLUS_003.73");
    if (sel >= this->count)
    {
        GameDiagnostic(("Menu selection index out of bounds: index=%d count=%d\n", sel, this->count));
        abort();
    }
    // Callers populate enabled rows before navigation
    for (;;)
    {
        row = items + sel;
        if (row->enabled)
            break;
        row->target_scale = row->unselected_scale;
        row = items + sel;
        row->scale = row->unselected_scale;
        selection = sel + 1;
        count = this->count;
        sel = selection;
        if (selection >= count)
            sel = 0;
        row = items + sel;
        row->target_scale = row->sel_scale;
        row = items + sel;
        row->scale = row->unselected_scale;
    }
    prev = game_pads[0].buttons[10].down;
    next = prev ? 0 : game_pads[0].buttons[11].down;
    if (prev || next)
    {
        repeat = menu_repeat;
        if (!repeat || ((sint32)repeat >= 11 && !(repeat & 1)))
        {
            if (prev)
            {
                row = items + sel;
                row->target_scale = row->unselected_scale;
                do
                {
                    selection = sel;
                    selection = selection ? selection - 1 : this->count - 1;
                    sel = selection;
                    row = items + sel;
                } while (!row->enabled);
                row->target_scale = row->sel_scale;
                PlaySoundEffect(26, 0x2000, 0);
            }
            if (game_pads[0].buttons[11].down)
            {
                row = items + sel;
                row->target_scale = row->unselected_scale;
                count = this->count;
                do
                {
                    selection = sel + 1;
                    sel = selection;
                    if (selection >= count)
                        sel = 0;
                    row = items + sel;
                } while (!row->enabled);
                row->target_scale = row->sel_scale;
                PlaySoundEffect(26, 0x2000, 0);
            }
        }
        repeat = menu_repeat;
        menu_repeat = repeat + 1;
    }
    else
        menu_repeat = 0;
    count = this->count;
    if (!count)
        return 0;
    row = items;
    idx = 0;
    for (;;)
    {
        cur = row->scale;
        target = row->target_scale;
        if (cur < target)
        {
            stepped = cur + 40;
            row->scale = stepped;
            target = row->target_scale;
            if (target >= stepped)
                goto next_row;
            row->scale = target;
        }
        cur = row->scale;
        target = row->target_scale;
        if (target < cur)
        {
            stepped = cur - 40;
            row->scale = stepped;
            target = row->target_scale;
            if (stepped < target)
                row->scale = target;
        }
    next_row:
        count = this->count;
        ++idx;
        ++row;
        if (idx >= count)
            return 0;
    }
}

uint32 MenuWindow::EnterSaveSelection()
{
    MenuWindow *menu;
    uint8 idx;
    FUNCTION_MARKER(0x80017760, "SLUS_003.73");
    menu = menu_runtime.confirm;
    idx = menu->FindItem(MenuText::yes);
    menu = menu_runtime.confirm;
    menu->ResetSelection(idx);
    menu_runtime.screen = MENU_SCREEN_SAVE_CONFIRM;
    return MENU_SCREEN_SAVE_CONFIRM;
}

uint32 MenuWindow::PopulateSaveSelection()
{
    uint32 slot, checksum;
    MenuItem *row;
    const LevelEntry *entry;
    FUNCTION_MARKER(0x8001779C, "SLUS_003.73");
    for (slot = 0; slot < CardSaveSlotCount; ++slot)
    {
        const PlayerSaveRecord *rec = card_save_image.slots + slot;
        if (rec->checksum)
        {
            entry = FindLevelEntry(rec->progress.level);
            checksum = rec->CalculateChecksum();
            if (rec->checksum == checksum && entry)
                AddItem(entry->title);
            else
            {
                AddItem(MenuText::read_error);
                row = items + count - 1;
                row->color[0].r = 255;
                row->color[0].g = 0;
                row->color[0].b = 0;
                row->color[1].r = 255;
                row->color[1].g = 0;
                row->color[1].b = 0;
            }
        }
        else
        {
            AddItem(MenuText::empty_slot);
            row = items + count - 1;
            const MenuItem::Color white = {255, 255, 255};
            row->color[0] = white;
            row->color[1] = white;
        }
    }
    return 0;
}
