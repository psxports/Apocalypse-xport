#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "scene_lifecycle.h"
#include "game_pad_poll.h"

#include "game_diagnostics.h"

#include "menu_window.h"
#include "game_string.h"
#include "hud_text.h"
#include "audio_session.h"

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
const char center_screen[] = "center screen";
const char calibrate_analog_controller[] = "calibrate analog controller";
const char vibration_settings[] = "vibration settings";
const char return_to_controller_config[] = "return to controller config";
const char use_the_directional[] = "Use the directional";
const char buttons_to_center[] = "buttons to center";
const char the_screen[] = "the screen";
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

void ResetMenuRepeat()
{
    menu_repeat = 0;
}

MenuWindow::MenuWindow(uint32 x, uint32 y, uint32 mode, uint32 selected_scale, uint32 unselected_scale, uint32 spacing)
{
    FUNCTION_MARKER(0x80015228, "SLUS_003.73");
    this->scr_pos[0] = x;
    this->scr_pos[1] = y;
    this->alignment = mode;
    this->spacing = spacing;
    this->has_arrows = 0;
    this->arrow_item = 0;
    this->numbered = 0;
    this->count = 0;
    const MenuItem empty = {0};
    for (uint32 index = 0; index < 15; ++index)
        this->items[index] = empty;
    for (uint32 index = 0; index < 15; ++index)
    {
        this->items[index].label = 0;
        MenuItem *row = this->items + index;
        row->pulse = 0;
        row->glow = 0;
        row->color[0][0] = 200;
        row->color[0][1] = 200;
        row->color[0][2] = 200;
        row->color[1][0] = 128;
        row->color[1][1] = 128;
        row->color[1][2] = 128;
        row->brightness = 128;
        // TODO Confirm the original units of these selection scale values
        row->selected_scale = selected_scale;
        row->unselected_scale = unselected_scale;
    }
    ResetSelection();
}

MenuWindow::~MenuWindow()
{
    for (uint32 index = 0; index < 15; ++index)
        free(items[index].owned_label);
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
    uint32 rows = this->count;
    uint32 enabled = 0, height = 0;
    for (uint32 index = 0; index < rows; ++index)
    {
        MenuItem *row = this->items + index;
        if (row->enabled)
        {
            ++enabled;
            height += row->height;
        }
    }
    uint32 spacing = this->spacing;
    uint32 span = (enabled - 1) * spacing + height;
    uint32 result = (uint32)((sint32)(240 - span) / 2);
    this->scr_pos[1] = result;
    return result;
}

uint32 MenuWindow::ResetSelection(uint32 selected)
{
    FUNCTION_MARKER(0x80015374, "SLUS_003.73");
    this->selected = selected;
    for (uint32 index = 0; index < 15; ++index)
    {
        MenuItem *row = this->items + index;
        uint32 selection = this->selected;
        uint32 scale = (index == selection ? row->selected_scale : row->unselected_scale);
        row->target_scale = scale;
        scale = row->target_scale;
        row->pulse = 0;
        row->glow = 0;
        row->brightness = 128;
        row->scale = scale;
    }
    return 0;
}

uint32 MenuWindow::ResetSelection(void)
{
    FUNCTION_MARKER(0x800153D8, "SLUS_003.73");
    return ResetSelection(0);
}

// Native adapter for nullable original label pointers

const char *MenuWindow::GetItemLabel(uint32 idx) const
{
    if (idx >= 15)
    {
        GameDiagnostic("Menu item index out of bounds\n");
        abort();
    }
    return this->items[idx].label;
}

const char *MenuWindow::GetSelectedLabel() const
{
    return GetItemLabel(selected);
}

uint32 MenuWindow::FindItem(const char *label)
{
    FUNCTION_MARKER(0x800153F8, "SLUS_003.73");
    if (!this->count)
        return 0;
    uint32 index = 0;
    for (;;)
    {
        if (NamesEqualIgnoringCase(label, GetItemLabel(index)))
            return index;
        uint32 count = this->count;
        ++index;
        if (index >= count)
            return 0;
    }
}

uint32 MenuWindow::AddItem(const char *label)
{
    FUNCTION_MARKER(0x80015474, "SLUS_003.73");
    uint32 count = this->count;
    if (count >= 15)
    {
        GameDiagnostic("Menu item capacity exceeded\n");
        abort();
    }
    this->items[count].label = label;
    count = this->count;
    this->items[count].enabled = 1;
    count = this->count;
    this->items[count].height = 0;
    uint32 result = this->count + 1;
    this->count = result;
    return result;
}

uint32 MenuWindow::AddOwnedItem(const char *label)
{
    uint32 index = count;
    uint32 result = AddItem(label);
    size_t length = strlen(label);
    char *copy = (char *)malloc(length + 1);
    if (!copy)
    {
        GameDiagnostic("Cannot allocate menu label\n");
        abort();
    }
    memcpy(copy, label, length + 1);
    items[index].owned_label = copy;
    items[index].label = copy;
    return result;
}

uint32 MenuWindow::EnableItem(const char *label)
{
    FUNCTION_MARKER(0x800154E0, "SLUS_003.73");
    uint32 index = FindItem(label);
    this->items[index].enabled = 1;
    return 1;
}

uint32 MenuWindow::DisableItem(const char *label)
{
    FUNCTION_MARKER(0x8001551C, "SLUS_003.73");
    uint32 index = FindItem(label);
    this->items[index].enabled = 0;
    return index;
}

uint32 MenuWindow::SetItemHeight(const char *label, uint32 height)
{
    FUNCTION_MARKER(0x80015590, "SLUS_003.73");
    uint32 index = FindItem(label);
    this->items[index].height = height;
    return index;
}

uint32 MenuWindow::IsSelected(const char *label)
{
    FUNCTION_MARKER(0x800155D4, "SLUS_003.73");
    uint32 selected = this->selected;
    return NamesEqualIgnoringCase(label, GetItemLabel(selected));
}

uint32 MenuWindow::GetItemPosition(const char *label, uint32 *x, uint32 *y)
{
    FUNCTION_MARKER(0x80015614, "SLUS_003.73");
    *x = this->scr_pos[0];
    *y = this->scr_pos[1];
    uint32 result = this->count;
    if (!result)
        return result;
    uint32 index = 0;
    MenuItem *row = this->items;
    do
    {
        if (row->enabled)
        {
            uint32 height = row->height;
            uint32 position = *y;
            *y = position + height;
            result = NamesEqualIgnoringCase(label, GetItemLabel(index));
            if (result)
                return result;
            position = *y;
            uint32 spacing = this->spacing;
            *y = position + spacing;
        }
        uint32 count = this->count;
        ++index;
        result = index < count;
        ++row;
    } while (result);
    return result;
}

uint32 MenuWindow::Draw(void)
{
    FUNCTION_MARKER(0x800156E0, "SLUS_003.73");
    uint32 row_height, enabled, red, green, blue, brightness, numbered, draw_color, draw_glow;
    sint32 current_pulse, current_glow, current_scale, unselected_scale;
    uint32 pulse_bits, glow_bits;
    sint32 index;
    MenuItem *row;
    uint32 y;
    sint32 result;
    sint16 pulse;
    sint16 glow;
    uint8 fade;
    const char *text;
    sint32 length;
    char *suffix;
    sint32 text_scale;
    sint32 width;
    uint32 arrow_offset;
    uint8 *left_arrow;
    uint8 *right_arrow;
    sint16 left_center;
    sint16 left_edge;
    sint16 right_center;
    sint16 right_edge;
    uint32 *ordering_table;
    char numbered_label[104];
    index = 0;
    SetHudTextAlignment(this->alignment);
    row = this->items;
    y = this->scr_pos[1];
    while (1)
    {
        result = (index < this->count);
        if (!result)
            return result;
        row_height = row->height;
        enabled = row->enabled;
        y += row_height;
        if (!enabled)
            goto NextRow;
        if (((sint16)row->scale > 0))
        {
            if ((index == this->selected))
            {
                red = row->color[0][0];
                green = row->color[0][1];
                blue = row->color[0][2];
                SetHudTextColor(red, green, blue, 0);
                current_pulse = (sint16)row->pulse;
                pulse_bits = row->pulse;
                pulse = (sint16)(pulse_bits - 16);
                if (current_pulse < 129)
                {
                WrapSelectedPulse:
                    if (((sint16)row->pulse < 128))
                        row->pulse = 255;
                }
                else
                {
                    row->pulse = pulse;
                    if ((pulse < 128))
                    {
                        row->pulse = 128;
                        goto WrapSelectedPulse;
                    }
                }
                current_glow = (sint16)row->glow;
                glow_bits = row->glow;
                glow = (sint16)(glow_bits + 32);
                if (current_glow < 256)
                {
                    row->glow = glow;
                    if ((glow >= 257))
                        row->glow = 256;
                }
                row->brightness = 0;
            }
            else
            {
                red = row->color[1][0];
                brightness = row->brightness;
                green = row->color[1][1];
                blue = row->color[1][2];
                SetHudTextColor((red * brightness >> 7) & 255, (green * brightness >> 7) & 255, (blue * brightness >> 7) & 255, 0);
                fade = (row->brightness + 6);
                row->brightness = fade;
                if ((fade >= 0x81))
                    row->brightness = 0x80;
                current_pulse = (sint16)row->pulse;
                pulse_bits = row->pulse;
                if (current_pulse < 16)
                    row->pulse = 0;
                else
                    row->pulse = pulse_bits - 16;
                if ((sint16)row->pulse)
                    row->glow = 256;
                else
                    row->glow = 0;
            }
            current_scale = (sint16)row->scale;
            unselected_scale = (sint16)row->unselected_scale;
            SetHudTextDepth(unselected_scale >= current_scale);
            numbered = this->numbered;
            text = GetItemLabel(index);
            if (numbered && !NamesEqualIgnoringCase(text, MenuText::empty_slot))
            {
                length = (sint32)strlen(text);
                text = numbered_label;
                // TODO Original numbered-label local buffer can overflow
                if ((uint32)length + 3 > sizeof(numbered_label))
                {
                    GameDiagnostic("TODO Numbered menu label exceeds original local buffer\n");
                    abort();
                }
                CopyGameString(GetItemLabel(index), numbered_label);
                numbered_label[length] = 32;
                suffix = &numbered_label[(length + 1)];
                *suffix = (char)(index + 49);
                suffix[1] = 0;
            }
            SetHudTextScale((sint16)row->selected_scale);
            if ((MeasureStyledHudText(text) < 491))
                text_scale = (sint16)row->scale;
            else
                text_scale = (sint16)row->unselected_scale;
            SetHudTextScale(text_scale);
            draw_color = (uint32)(sint32)(sint16)row->pulse;
            draw_glow = (uint32)(sint32)(sint16)row->glow;
            width = (sint32)DrawStyledHudText(this->scr_pos[0], y, text, 0, draw_color, draw_glow);
            if (((this->has_arrows && (index == this->arrow_item)) && !(this->alignment)))
            {
                current_scale = (sint16)row->scale;
                arrow_offset = (uint32)((sint32)((uint32)width * (uint32)current_scale) / 512) + (uint32)((sint32)((uint32)current_scale * 14) / 256);
                left_arrow = scene_render.cursor;
                if (scene_render.limit - left_arrow >= (sint32)(2 * sizeof(POLY_F3)))
                {
                    right_arrow = left_arrow + sizeof(POLY_F3);
                    scene_render.cursor = left_arrow + 2 * sizeof(POLY_F3);
                    POLY_F3 *left_poly = (POLY_F3 *)left_arrow;
                    POLY_F3 *right_poly = (POLY_F3 *)right_arrow;
                    setPolyF3(left_poly);
                    setPolyF3(right_poly);
                    setRGB0(left_poly, 255, 0, 0);
                    left_center = (sint16)this->scr_pos[0];
                    left_poly->p[0].y = y;
                    left_poly->p[0].x = (uint32)left_center - arrow_offset;
                    left_poly->p[1].y = y - 6;
                    left_edge = left_poly->p[0].x;
                    left_poly->p[2].y = y + 6;
                    left_edge -= 20;
                    left_poly->p[1].x = left_edge;
                    left_poly->p[2].x = left_edge;
                    setRGB0(right_poly, 255, 0, 0);
                    right_center = (sint16)this->scr_pos[0];
                    right_poly->p[0].y = y;
                    right_poly->p[0].x = (uint32)right_center + arrow_offset;
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
        y += this->spacing;
    NextRow:
        ++row;

        ++index;
    }
}

uint32 MenuWindow::Update(void)
{
    uint32 current_word, target_word;
    uint32 selection, count, repeat, previous, next, index;
    MenuItem *row;
    sint32 current, target;
    sint16 stepped;
    FUNCTION_MARKER(0x80015BA4, "SLUS_003.73");
    // TODO Original selection traversal requires at least one enabled row
    for (;;)
    {
        row = this->items + this->selected;
        if (row->enabled)
            break;
        row->target_scale = row->unselected_scale;
        row = this->items + this->selected;
        row->scale = row->unselected_scale;
        selection = this->selected + 1;
        count = this->count;
        this->selected = selection;
        if ((uint8)selection >= count)
            this->selected = 0;
        row = this->items + this->selected;
        row->target_scale = row->selected_scale;
        row = this->items + this->selected;
        row->scale = row->unselected_scale;
    }
    previous = game_pads[0].buttons[10].down;
    next = previous ? 0 : game_pads[0].buttons[11].down;
    if (previous || next)
    {
        repeat = menu_repeat;
        if (!repeat || ((sint32)repeat >= 11 && !(repeat & 1)))
        {
            if (previous)
            {
                row = this->items + this->selected;
                row->target_scale = row->unselected_scale;
                do
                {
                    selection = this->selected;
                    selection = selection ? selection - 1 : this->count - 1;
                    this->selected = selection;
                    row = this->items + this->selected;
                } while (!row->enabled);
                row->target_scale = row->selected_scale;
                PlaySoundEffect(26, 0x2000, 0);
            }
            if (game_pads[0].buttons[11].down)
            {
                row = this->items + this->selected;
                row->target_scale = row->unselected_scale;
                count = this->count;
                do
                {
                    selection = this->selected + 1;
                    this->selected = selection;
                    if ((uint8)selection >= count)
                        this->selected = 0;
                    row = this->items + this->selected;
                } while (!row->enabled);
                row->target_scale = row->selected_scale;
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
    row = this->items;
    index = 0;
    for (;;)
    {
        current = (sint16)row->scale;
        target = (sint16)row->target_scale;
        current_word = row->scale;
        if (current < target)
        {
            stepped = (sint16)(current_word + 40);
            row->scale = (uint16)stepped;
            target = (sint16)row->target_scale;
            target_word = row->target_scale;
            if (target >= stepped)
                goto next_row;
            row->scale = target_word;
        }
        current = (sint16)row->scale;
        target = (sint16)row->target_scale;
        current_word = row->scale;
        if (target < current)
        {
            stepped = (sint16)(current_word - 40);
            row->scale = (uint16)stepped;
            target = (sint16)row->target_scale;
            target_word = row->target_scale;
            if (stepped < target)
                row->scale = target_word;
        }
    next_row:
        count = this->count;
        ++index;
        ++row;
        if ((sint32)index >= (sint32)count)
            return 0;
    }
}
