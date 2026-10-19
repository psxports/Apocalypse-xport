#include <string.h>
#include <new>
#include <stdio.h>
#include <stdlib.h>

#include "level_session.h"
#include "player_save_state.h"
#include "game_tick.h"
#include "game_pad_poll.h"

#include "menu_saves.h"
#include "scene_lifecycle.h"
#include "cheat_input.h"
#include "game_diagnostics.h"
#include "pause_menu.h"
#include "record_scripts.h"
#include "card_save.h"
#include "menu_input.h"
#include "menu_window.h"
#include "menu_lifecycle.h"
#include "game_string.h"
#include "audio_session.h"
#include "audio_voice.h"
#include "game_panel.h"
#include "hud_text.h"
#include "screen_flash.h"
#include "movie_lifecycle.h"

MenuRuntime menu_runtime = {0, 0, 0, 0, 0, 0, 0, 180, 0, 0, 0, 0, 0, 0, 0, 0};

void ResetMenuRuntime()
{
    ObjectBorrow borrow;
    if (menu_runtime.restart)
        DestroyMenuObject(menu_runtime.restart);
    if (menu_runtime.save)
        DestroyMenuObject(menu_runtime.save);
    if (menu_runtime.pause)
        DestroyMenuObject(menu_runtime.pause);
    if (menu_runtime.confirm)
        DestroyMenuObject(menu_runtime.confirm);
    MenuRuntime initial = {0};
    initial.prompt_delay = 180;
    menu_runtime = initial;
}

void UpdatePauseMenu(void)
{
    FUNCTION_MARKER(0x80017914, "SLUS_003.73");
    sint32 card_status;
    uint32 session_state, timer_snapshot;
    MenuWindow *cleanup_menu;
    sint32 selected_sound;
    sint32 close_menu;
    sint32 action_sound;
    MenuWindow *pause_menu;
    sint32 pause_sound;
    sint32 selection_sound;
    sint32 quit_selected;
    MenuWindow *level_menu;
    uint32 level_index;
    sint32 quit_sound;
    sint32 progress_in_range;
    sint32 remaining;
    sint32 operation_failed;
    sint32 card_state;
    MenuWindow *card_menu;
    MenuWindow *confirmation_menu;
    sint32 next_state;
    sint32 status_state;
    MenuWindow *menu_to_destroy;
    sint32 next_card_screen;
    sint32 retries_left;
    sint32 result;
    uint8 prefix[4];

    // TODO Original skipped-input path has no initialized button locals
    uint32 confirm = 0;
    uint32 cancel = 0;
    uint32 acknowledge = 0;
    uint32 pause_pressed = 0;
    selected_sound = -1;
    close_menu = 0;
    if ((!game_pads[0].buttons[4].down || (menu_runtime.screen != MENU_SCREEN_PAUSED)))
    {
        ReadMenuInput(&confirm, &cancel, &acknowledge, &pause_pressed);
        ClearCheatInputBuffer();
    }
    if (menu_runtime.screen)
        UpdateCardStatus(0, 0);
    if ((game_pads[0].buttons[15].down && game_pads[0].buttons[14].down))
    {
        if (!menu_runtime.exit_hold)
        {
            menu_runtime.exit_hold = 1;
            menu_runtime.exit_frame = game_clock.vblank_frame;
        }
        if (game_clock.vblank_frame - menu_runtime.exit_frame >= 121)
        {
            menu_runtime.exit_hold = 0;
            game_session.exit_reason = 7;
            close_menu = 1;
        }
    }
    else
    {
        menu_runtime.exit_hold = 0;
    }
    switch (menu_runtime.screen)
    {
        case MENU_SCREEN_GAMEPLAY:
            if ((pause_pressed || !game_pads[0].type))
            {
                ClearAnimatedHudText();
                game_pads[0].ClearPresses();
                menu_runtime.shake_amplitude = 0;
                SuspendAudioPlayback();
                PlaySoundEffect(23, 0x2000, 0);
                ClearGamePadActuator(0, 0);
                ClearGamePadActuator(0, 1);
                scene_frame.paused = 1;
                pause_menu = new MenuWindow(256, 0, 0, 320, 256, 26);
                menu_runtime.pause = pause_menu;
                pause_menu->AddItem(MenuText::continue_game);
                menu_runtime.pause->AddItem(MenuText::skip_to_restart);
                if (!GetCheatOptions().level_skip)
                    menu_runtime.pause->DisableItem(MenuText::skip_to_restart);
                menu_runtime.pause->AddItem(MenuText::quit);
                menu_runtime.pause->CenterVertical();
                menu_runtime.pause->scr_pos[1] += 5;
                if (!game_pads[0].type)
                    goto ControllerDisconnected;
                menu_runtime.screen = MENU_SCREEN_PAUSED;
            }
            goto FinalizeMenuFrame;

        case MENU_SCREEN_PAUSED:
            ResetPanelDisplayTimer();
            if (!scene_frame.paused)
                close_menu = 1;
            if (!game_pads[0].type)
                menu_runtime.screen = MENU_SCREEN_CONTROLLER_MISSING;
            if (game_pads[0].buttons[4].down)
            {
                if (UpdateCheatInput())
                {
                    menu_runtime.shake_amplitude = 32;
                    menu_runtime.shake_phase = 0;
                }
                game_pads[0].ClearPresses();
                goto FinalizeMenuFrame;
            }
            if (GetCheatOptions().level_skip)
                menu_runtime.pause->EnableItem(MenuText::skip_to_restart);
            else
                menu_runtime.pause->DisableItem(MenuText::skip_to_restart);
            menu_runtime.pause->Update();
            pause_sound = -1;
            if (!confirm)
                goto PlayPauseSelectionSound;
            if (pause_pressed)
            {
                selection_sound = 19;
                close_menu = 1;
            }
            else
            {
                if (menu_runtime.pause->IsSelected(MenuText::continue_game))
                {
                    selected_sound = 19;
                    close_menu = 1;
                }
                if (menu_runtime.pause->IsSelected(MenuText::skip_to_restart))
                {
                    selected_sound = 23;
                    menu_runtime.screen = MENU_SCREEN_RESTART_SELECTION;
                }
                quit_selected = menu_runtime.pause->IsSelected(MenuText::quit);
                pause_sound = selected_sound;
                if (!quit_selected)
                    goto PlayPauseSelectionSound;
                selection_sound = 23;
                menu_runtime.confirm->ResetSelection();
                menu_runtime.screen = MENU_SCREEN_QUIT_CONFIRM;
            }
            pause_sound = selection_sound;
        PlayPauseSelectionSound:
            PlaySoundEffect(pause_sound, 0x2000, 0);

            goto FinalizeMenuFrame;

        case MENU_SCREEN_RESTART_SELECTION:
            if (!menu_runtime.restart)
            {
                level_menu = new MenuWindow(256, 0, 0, 256, 192, 26);
                level_index = 0;
                menu_runtime.restart = level_menu;
                level_menu->spacing = 18;
                while (level_index < checkpoint_names.Count())
                {
                    const char *name = checkpoint_names.Get(level_index);
                    // Short labels are zero-padded before the three-character comparison
                    uint32 length = 0;
                    while (length < 3 && name[length])
                    {
                        prefix[length] = name[length];
                        ++length;
                    }
                    while (length < 4)
                        prefix[length++] = 0;
                    if (!NamesEqualIgnoringCase((const char *)prefix, "de_"))
                        level_menu->AddOwnedItem(name);
                    ++level_index;
                }

                menu_runtime.restart->CenterVertical();
            }
            menu_runtime.restart->Update();
            if (confirm)
            {
                SelectStartRecord(menu_runtime.restart->GetSelectedLabel());
                if (menu_runtime.restart)
                    DestroyMenuObject(menu_runtime.restart);
                close_menu = 1;
                menu_runtime.restart = 0;
                game_session.exit_reason = 6;
                PlaySoundEffect(23, 0x2000, 0);
            }
            if (cancel)
            {
                if (menu_runtime.restart)
                    DestroyMenuObject(menu_runtime.restart);
                menu_runtime.restart = 0;
                menu_runtime.screen = MENU_SCREEN_PAUSED;
                PlaySoundEffect(19, 0x2000, 0);
            }
            goto FinalizeMenuFrame;

        case MENU_SCREEN_QUIT_CONFIRM:
            if (!game_pads[0].type)
            {
            ControllerDisconnected:
                menu_runtime.screen = MENU_SCREEN_CONTROLLER_MISSING;

                goto FinalizeMenuFrame;
            }
            menu_runtime.confirm->Update();
            if (!confirm)
                goto HandleQuitCancel;
            if (pause_pressed)
            {
                close_menu = 1;
            }
            else
            {
                if (menu_runtime.confirm->IsSelected(MenuText::yes))
                {
                    close_menu = 1;
                    game_session.exit_reason = 7;
                    quit_sound = 23;
                    goto PlayQuitSound;
                }
                menu_runtime.screen = MENU_SCREEN_PAUSED;
            }
            quit_sound = 19;
        PlayQuitSound:
            PlaySoundEffect(quit_sound, 0x2000, 0);

        HandleQuitCancel:

            if (cancel)
            {
                PlaySoundEffect(19, 0x2000, 0);
                goto ShowPause;
            }
            goto FinalizeMenuFrame;

        case MENU_SCREEN_CONTROLLER_MISSING:
            if (game_pads[0].type)
            ShowPause:
                menu_runtime.screen = MENU_SCREEN_PAUSED;

            goto FinalizeMenuFrame;

        case MENU_SCREEN_RESULTS_CAPTION:
            if ((uint8)panel_text.caption[panel_text.visible])
            {
                if (game_clock.vblank_frame - panel_text.reveal_frame >= 4)
                {
                    ++panel_text.visible;
                    panel_text.reveal_frame = game_clock.vblank_frame;
                    PlaySoundEffect(16, 0x2000, 0);
                }
            }
            else
            {
                if (!game_progress.completion.completed)
                    goto EnterSaveSelection;
                game_progress.completion.voices = PlaySoundEffect(11, 0x2000, 0);
                menu_runtime.screen = MENU_SCREEN_RESULTS_PERCENT;
            }
            goto FinalizeMenuFrame;

        case MENU_SCREEN_RESULTS_PERCENT:
            progress_in_range = ((sint32)game_progress.completion.target_percent < 101);
            if (((sint32)game_progress.completion.target_percent < 0))
            {
                game_progress.completion.target_percent = 0;
                progress_in_range = 1;
            }
            if (!progress_in_range)
                game_progress.completion.target_percent = 100;
            remaining = (sint32)(game_progress.completion.display_percent + 1);
            game_progress.completion.display_percent = (uint32)remaining;
            if ((sint32)game_progress.completion.target_percent < remaining)
            {
                game_progress.completion.display_percent = game_progress.completion.target_percent;
                StopSoundVoices(game_progress.completion.voices);
                goto EnterSaveSelection;
            }
            goto FinalizeMenuFrame;

        case MENU_SCREEN_EXIT_FADE_START:
            FadeScreenToWhite(32, 1);
            menu_runtime.screen = MENU_SCREEN_EXIT_FADE;
            goto FinalizeMenuFrame;

        case MENU_SCREEN_EXIT_FADE:
            UpdateScreenFlash();
            if (IsScreenFadeComplete())
            {
                game_session.exit_reason = 3;
                close_menu = 1;
            }
            goto FinalizeMenuFrame;

        case MENU_SCREEN_SAVE_CONFIRM:
            menu_runtime.confirm->Update();
            if (confirm)
            {
                PlaySoundEffect(23, 0x2000, 0);
                if (!menu_runtime.confirm->IsSelected(MenuText::yes))
                    goto ExitSaveMenu;
                game_progress.completion.blink_ticks = 0;
                menu_runtime.screen = MENU_SCREEN_CARD_POLL_DELAY;
            }
            goto FinalizeMenuFrame;

        case MENU_SCREEN_CARD_POLL_DELAY:
            timer_snapshot = menu_runtime.card_poll_delay;
            remaining = (sint32)(timer_snapshot - 1);
            if (timer_snapshot)
            {
                menu_runtime.card_poll_delay = (uint32)remaining;
                if (!remaining)
                    menu_runtime.screen = MENU_SCREEN_CARD_QUERY;
            }
            else
            {
                menu_runtime.card_poll_delay = 10;
            }
            goto FinalizeMenuFrame;

        case MENU_SCREEN_CARD_QUERY:
            card_status = card_save_state.status;
            if ((card_status == -1))
            {
                menu_runtime.screen = MENU_SCREEN_CARD_MISSING;
            }
            else if ((card_status >= 0))
            {
                if ((card_status == 1))
                {
                    operation_failed = QueryCardSaveFile() == 0;
                    card_state = MENU_SCREEN_SAVE_SLOT;
                    if (!operation_failed)
                    {
                        ::WaitForGameFrames(1);
                        operation_failed = (sint32)GetFreeCardBlocks(0, 0) > 0;
                        card_state = MENU_SCREEN_SAVE_SLOT;
                        if (!operation_failed)
                            card_state = MENU_SCREEN_CARD_FULL;
                    }
                    menu_runtime.screen = card_state;
                    if (card_state == MENU_SCREEN_SAVE_SLOT)
                    {
                        card_menu = new MenuWindow(256, 55, 0, 256, 192, 18);
                        menu_runtime.save = card_menu;
                        card_menu->numbered = 1;
                        ::PopulateSaveSelectionMenu(menu_runtime.save);
                    }
                }
            }
            else if ((card_status == -2))
            {
                confirmation_menu = menu_runtime.confirm;
                next_state = MENU_SCREEN_CARD_FORMAT_CONFIRM;
                goto OpenCardConfirmation;
            }
            goto FinalizeMenuFrame;

        case MENU_SCREEN_SAVE_SLOT:
            menu_runtime.save->Update();
            if ((card_save_state.status == -1))
            {
                menu_to_destroy = menu_runtime.save;
                menu_runtime.screen = MENU_SCREEN_CARD_MISSING;
            DestroySaveSelection:
                if (menu_to_destroy)
                    DestroyMenuObject(menu_to_destroy);

                menu_runtime.save = 0;
                goto FinalizeMenuFrame;
            }
            if (cancel)
            {
                PlaySoundEffect(19, 0x2000, 0);
                ::EnterSaveSelectionMenu();
                menu_to_destroy = menu_runtime.save;
                goto DestroySaveSelection;
            }
            if (confirm)
            {
                PlaySoundEffect(23, 0x2000, 0);
                if (card_save_image.slots[menu_runtime.save->selected].checksum)
                {
                    confirmation_menu = menu_runtime.confirm;
                    next_state = MENU_SCREEN_SAVE_OVERWRITE;
                OpenCardConfirmation:
                    menu_runtime.screen = next_state;

                    confirmation_menu->ResetSelection();
                }
                else
                {
                    player_saves.current.checksum = CalculatePlayerSaveChecksum(player_saves.current);
                    card_save_image.slots[menu_runtime.save->selected] = player_saves.current;
                    menu_runtime.screen = MENU_SCREEN_CARD_WRITE;
                    menu_runtime.operation_delay = 4;
                    menu_runtime.retries_left = 4;
                }
            }
            goto FinalizeMenuFrame;

        case MENU_SCREEN_SAVE_OVERWRITE:
            menu_runtime.confirm->Update();
            if (confirm)
            {
                if (menu_runtime.confirm->IsSelected(MenuText::yes))
                {
                    PlaySoundEffect(23, 0x2000, 0);
                    player_saves.current.checksum = CalculatePlayerSaveChecksum(player_saves.current);
                    card_save_image.slots[menu_runtime.save->selected] = player_saves.current;
                    menu_runtime.screen = MENU_SCREEN_CARD_WRITE;
                    menu_runtime.operation_delay = 4;
                    menu_runtime.retries_left = 4;
                }
                else
                {
                    PlaySoundEffect(19, 0x2000, 0);
                    menu_runtime.screen = MENU_SCREEN_SAVE_SLOT;
                }
            }
            action_sound = 19;
            if (cancel)
                goto ReturnToSaveSelection;
            goto FinalizeMenuFrame;

        case MENU_SCREEN_CARD_WRITE:
            if ((card_save_state.status == -1))
            {
                if (menu_runtime.save)
                    DestroyMenuObject(menu_runtime.save);
                menu_runtime.save = 0;
                menu_runtime.screen = MENU_SCREEN_CARD_MISSING;
            }
            remaining = (sint32)(menu_runtime.operation_delay - 1);
            menu_runtime.operation_delay = remaining;
            if (remaining < 0 && (uint32)card_save_state.status)
            {
                if (WriteCardSaveFile())
                {
                    retries_left = (menu_runtime.retries_left - 1);
                    menu_runtime.retries_left = retries_left;
                    if ((retries_left >= 0))
                        goto ScheduleCardRetry;
                    menu_runtime.screen = MENU_SCREEN_CARD_WRITE_ERROR;
                }
                else
                {
                    if (menu_runtime.save)
                        DestroyMenuObject(menu_runtime.save);
                    menu_runtime.save = 0;
                    menu_runtime.screen = MENU_SCREEN_CARD_WRITE_COMPLETE;
                }
            }
            goto FinalizeMenuFrame;

        case MENU_SCREEN_CARD_WRITE_ERROR:
            action_sound = 23;
            if (acknowledge)
            {
            ReturnToSaveSelection:
                PlaySoundEffect(action_sound, 0x2000, 0);

                menu_runtime.screen = MENU_SCREEN_SAVE_SLOT;
            }
            goto FinalizeMenuFrame;

        case MENU_SCREEN_CARD_WRITE_COMPLETE:
            if (acknowledge)
            {
                PlaySoundEffect(23, 0x2000, 0);
            ExitSaveMenu:
                menu_runtime.screen = MENU_SCREEN_EXIT_FADE_START;
            }
            goto FinalizeMenuFrame;

        case MENU_SCREEN_CARD_FULL:
            status_state = MENU_SCREEN_CARD_MISSING;
            card_status = card_save_state.status;
            if ((card_status == -1))
                goto ShowCardStatus;
            if ((card_status >= 0))
            {
                status_state = MENU_SCREEN_CARD_POLL_DELAY;
                if ((card_status == 2))
                    goto ShowCardStatus;
            }
            else
            {
                status_state = MENU_SCREEN_CARD_POLL_DELAY;
                if ((card_status == -2))
                    goto ShowCardStatus;
            }
            goto HandleCardStatus;

        case MENU_SCREEN_CARD_MISSING:
            card_status = card_save_state.status;
            if (((card_status == -2) || (((card_status >= -2) && (card_status < 3)) && (card_status > 0))))
            {
                status_state = MENU_SCREEN_CARD_POLL_DELAY;
            ShowCardStatus:
                menu_runtime.screen = status_state;
            }
        HandleCardStatus:
            if (acknowledge)
                goto EnterSaveSelection;

            goto FinalizeMenuFrame;

        case MENU_SCREEN_CARD_FORMAT_CONFIRM:
            menu_runtime.confirm->Update();
            card_status = card_save_state.status;
            if ((card_status == -1))
            {
                next_card_screen = MENU_SCREEN_CARD_MISSING;
            ReturnFromFormatConfirm:
                menu_runtime.screen = next_card_screen;

                goto HandleFormatConfirmation;
            }
            if (((card_status >= -1) && (card_status < 3)))
            {
                next_card_screen = MENU_SCREEN_CARD_POLL_DELAY;
                if ((card_status > 0))
                    goto ReturnFromFormatConfirm;
            }
        HandleFormatConfirmation:
            if (confirm)
            {
                if (menu_runtime.confirm->IsSelected(MenuText::yes))
                {
                    PlaySoundEffect(23, 0x2000, 0);
                    menu_runtime.screen = MENU_SCREEN_CARD_FORMAT;
                    menu_runtime.operation_delay = 4;
                    menu_runtime.retries_left = 4;
                }
                else
                {
                    cancel = 1;
                }
            }

            action_sound = 19;
            if (cancel)
            {
            CancelSaveChoice:
                PlaySoundEffect(action_sound, 0x2000, 0);

            EnterSaveSelection:
                ::EnterSaveSelectionMenu();
            }
        FinalizeMenuFrame:
            if (scene_frame.paused)
            {
                result = menu_runtime.retained_fire_buffers;
                if (!result)
                    result = ::CreateMenuBuffers(951706923, 64, 64, 127);
            }
            else
            {
                result = ::HasMenuBuffers();
                menu_runtime.retained_fire_buffers = result;
            }

            if (close_menu)
            {
                cleanup_menu = menu_runtime.pause;
                scene_frame.paused = 0;
                menu_runtime.screen = MENU_SCREEN_GAMEPLAY;
                if (cleanup_menu)
                    DestroyMenuObject(cleanup_menu);
                menu_runtime.pause = 0;
                session_state = game_session.exit_reason;
                if (session_state != 7 && session_state != 3)
                    ResumeAudioPlayback();
                result = menu_runtime.retained_fire_buffers;
                if (!result)
                {
                    ReleaseMenuBuffers();
                    return;
                }
            }
            return;

        case MENU_SCREEN_CARD_FORMAT:
            remaining = (sint32)(menu_runtime.operation_delay - 1);
            menu_runtime.operation_delay = remaining;
            if (remaining < 0)
            {
                if (card_save_state.status)
                {
                    if (FormatCard(0, 0) == 1)
                    {
                        menu_runtime.screen = MENU_SCREEN_CARD_FORMAT_COMPLETE;
                    }
                    else
                    {
                        retries_left = (menu_runtime.retries_left - 1);
                        menu_runtime.retries_left = retries_left;
                        if ((retries_left >= 0))
                        ScheduleCardRetry:
                            menu_runtime.operation_delay = 60 * (4 - (uint32)retries_left);

                        else
                            menu_runtime.screen = MENU_SCREEN_CARD_FORMAT_ERROR;
                    }
                }
            }
            goto FinalizeMenuFrame;

        case MENU_SCREEN_CARD_FORMAT_ERROR:
            if ((card_save_state.status == -1))
                menu_runtime.screen = MENU_SCREEN_CARD_MISSING;
            action_sound = 23;
            if (acknowledge)
                goto CancelSaveChoice;
            goto FinalizeMenuFrame;

        case MENU_SCREEN_CARD_FORMAT_COMPLETE:
            if (acknowledge)
            {
                PlaySoundEffect(23, 0x2000, 0);
                menu_runtime.screen = MENU_SCREEN_CARD_POLL_DELAY;
            }
            goto FinalizeMenuFrame;

        default:
            goto FinalizeMenuFrame;
    }
}
