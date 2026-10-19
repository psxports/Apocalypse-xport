#ifndef MENU_WINDOW_H
#define MENU_WINDOW_H

extern "C"
{
#include "psx.h"
}

#include "object.h"

namespace MenuText
{
extern const char autotest[];
extern const char build_date[];
extern const char main_menu[];
extern const char game_over[];
extern const char press_start[];
extern const char continue_game[];
extern const char save_game_prompt[];
extern const char save_game[];
extern const char please_reinsert[];
extern const char controller[];
extern const char select_level[];
extern const char skip_to_restart[];
extern const char press_any_button[];
extern const char really_quit[];
extern const char really_reset_to_defaults[];
extern const char return_to_options_menu[];
extern const char fx_level[];
extern const char music_level[];
extern const char voice_level[];
extern const char read_error[];
extern const char new_game[];
extern const char load_game[];
extern const char difficulty[];
extern const char controller_config[];
extern const char misc_options[];
extern const char return_to_main_menu[];
extern const char view_credits[];
extern const char music_credits[];
extern const char view_intro[];
extern const char reset_to_defaults[];
extern const char center_screen[];
extern const char calibrate_analog_controller[];
extern const char vibration_settings[];
extern const char return_to_controller_config[];
extern const char use_the_directional[];
extern const char buttons_to_center[];
extern const char the_screen[];
extern const char move_the_left_stick[];
extern const char and_right_stick_to_all[];
extern const char four_corners_then_press[];
extern const char the_x_button[];
extern const char center_both_the_left_stick[];
extern const char and_the_right_stick_then[];
extern const char press_the_x_button[];
extern const char weapon_select[];
extern const char please_insert_a_memory[];
extern const char card_in_memory_card[];
extern const char slot_1_or_press_any[];
extern const char button_to_cancel[];
extern const char the_memory_card_in[];
extern const char memory_card_slot_1_is[];
extern const char unformatted_press[];
extern const char any_button_to_cancel[];
extern const char unformatted_do_you[];
extern const char want_to_format_it[];
extern const char formatting_memory_card[];
extern const char do_not_remove[];
extern const char memory_card[];
extern const char checking_memory_card[];
extern const char your_memory_card_in[];
extern const char memory_card_slot_1_is_full[];
extern const char insert_a_memory_card_with[];
extern const char at_least_1_free_block_or_delete[];
extern const char a_block_using_the_console[];
extern const char internal_memory_card_manager[];
extern const char okay_to_overwrite[];
extern const char previously_saved_game[];
extern const char in_this_save_slot[];
extern const char now_saving_data[];
extern const char attempt_to_write_to[];
extern const char memory_card_failed[];
extern const char format_failed[];
extern const char try_again_or_insert[];
extern const char a_new_memory_card[];
extern const char format_completed[];
extern const char save_completed[];
extern const char memory_card_contains[];
extern const char no_apocalypse_game_save[];
extern const char to_cancel[];
extern const char smart_bomb[];
extern const char empty[];
extern const char paused[];
extern const char kill_percent[];
extern const char quit[];
extern const char demo[];
extern const char go_back[];
extern const char select[];
extern const char accept[];
extern const char cancel[];
extern const char infinite_lives[];
extern const char yes[];
extern const char no[];
extern const char test[];
extern const char cycle[];
extern const char l2[];
extern const char r2[];
extern const char l1[];
extern const char r1[];
extern const char options[];
extern const char sound[];
extern const char none[];
extern const char some[];
extern const char full[];
extern const char easy[];
extern const char normal[];
extern const char hard[];
extern const char crouch[];
extern const char jump[];
extern const char empty_slot[];
} // namespace MenuText

struct MenuItem
{
    const char *label;
    char *owned_label;
    uint16 scale;
    uint16 target_scale;
    uint16 selected_scale;
    uint16 unselected_scale;
    uint8 height;
    uint8 enabled;
    uint8 color[2][3];
    uint8 brightness;
    uint16 pulse;
    uint16 glow;
};

void ResetMenuRepeat();

class MenuWindow : public Object
{
public:
    MenuWindow(uint32 x, uint32 y, uint32 mode, uint32 selected_scale, uint32 unselected_scale, uint32 spacing);
    virtual ~MenuWindow();
    virtual uint32 Destroy(uint32 reason);
    uint32 CenterVertical(void);
    uint32 Draw(void);
    uint32 Update(void);
    uint32 FindItem(const char *label);
    uint32 AddItem(const char *label);
    uint32 AddOwnedItem(const char *label);
    uint32 EnableItem(const char *label);
    uint32 DisableItem(const char *label);
    uint32 SetItemHeight(const char *label, uint32 height);
    uint32 IsSelected(const char *label);
    uint32 GetItemPosition(const char *label, uint32 *x, uint32 *y);
    const char *GetItemLabel(uint32 idx) const;
    const char *GetSelectedLabel() const;
    uint32 ResetSelection(uint32 selected);
    uint32 ResetSelection(void);

    uint8 alignment;
    uint8 selected;
    uint8 has_arrows;
    uint8 arrow_item;
    uint8 numbered;
    uint8 count;
    uint32 scr_pos[2];
    uint32 spacing;
    MenuItem items[15];
};

#endif
