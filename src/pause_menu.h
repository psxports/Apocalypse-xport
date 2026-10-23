#ifndef PAUSE_MENU_H
#define PAUSE_MENU_H

extern "C"
{
#include "psx.h"
}

class MenuWindow;

enum MenuScreen
{
    MENU_SCREEN_GAMEPLAY = 0,
    MENU_SCREEN_PAUSED = 1,
    MENU_SCREEN_RESTART_SELECTION = 2,
    MENU_SCREEN_QUIT_CONFIRM = 3,
    MENU_SCREEN_CONTROLLER_MISSING = 4,
    MENU_SCREEN_RESULTS_CAPTION = 5,
    MENU_SCREEN_RESULTS_PERCENT = 6,
    MENU_SCREEN_EXIT_FADE_START = 7,
    MENU_SCREEN_EXIT_FADE = 8,
    MENU_SCREEN_SAVE_CONFIRM = 9,
    MENU_SCREEN_CARD_POLL_DELAY = 10,
    MENU_SCREEN_CARD_QUERY = 11,
    MENU_SCREEN_SAVE_SLOT = 12,
    MENU_SCREEN_SAVE_OVERWRITE = 13,
    MENU_SCREEN_CARD_WRITE = 14,
    MENU_SCREEN_CARD_WRITE_ERROR = 15,
    MENU_SCREEN_CARD_WRITE_COMPLETE = 16,
    MENU_SCREEN_CARD_FULL = 17,
    MENU_SCREEN_CARD_MISSING = 18,
    MENU_SCREEN_CARD_FORMAT_CONFIRM = 19,
    MENU_SCREEN_CARD_FORMAT = 20,
    MENU_SCREEN_CARD_FORMAT_ERROR = 21,
    MENU_SCREEN_CARD_FORMAT_COMPLETE = 22
};

struct MenuRuntime
{
    uint32 shake_amplitude;
    uint32 shake_phase;
    uint32 navigation_ticks;
    bool voice_previewed;
    MenuWindow *confirm;
    MenuWindow *save;
    uint32 card_poll_delay;
    uint32 prompt_delay;
    // Retain the original default handling for unknown screen words
    uint32 screen;
    MenuWindow *pause;
    MenuWindow *restart;
    bool exit_hold;
    uint32 exit_frame;
    uint32 operation_delay;
    sint32 retries_left;
    bool retained_fire_buffers;
};

extern MenuRuntime menu_runtime;
void ResetMenuRuntime();
void UpdatePauseMenu();

#endif
