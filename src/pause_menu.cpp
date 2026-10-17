#include "menu_saves.h"
#include "scene_lifecycle.h"
#include "cheat_input.h"
extern "C"
{
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
uint32 sub_800154E0(uint32 object, uint32 label);
}
#include "pause_menu.h"
#include "record_scripts.h"
#include "card_save.h"
#include "menu_input.h"
#include "menu_window.h"
#include "native_object_storage.h"
#include <new>
#include "game_string.h"
#include "game_heap.h"
#include "audio_session.h"
#include "audio_voice.h"
#include "game_panel.h"
#include "hud_text.h"
#include "screen_flash.h"
#include "movie_lifecycle.h"
#include <stdio.h>
#include <stdlib.h>



static void DestroyPauseMenuObject(uint32 object)
{
    uint32 table = r_u32(object);
    sint32 adjustment = (sint16)r_u16(table + 8);
    uint32 target = r_u32(table + 12);
    apocalypse_object_cleanup(target, object + (uint32)adjustment, 3);
}

void UpdatePauseMenu(void)
{
    FUNCTION_MARKER(0x80017914, "SLUS_003.73");
    sint32 card_status;
    uint32 copy_first, cleanup_menu, session_state, timer_snapshot;
    sint32 selected_sound;
    sint32 close_menu;
    sint32 v2;
    sint32 pause_menu;
    sint32 v4;
    sint32 v5;
    sint32 v6;
    sint32 level_menu;
    sint32 level_index;
    uint32 level_name_cursor;
    sint32 v11;
    sint32 progress_in_range;
    sint32 remaining;
    sint32 operation_failed;
    sint32 card_state;
    sint32 card_menu;
    sint32 confirmation_menu;
    sint32 next_state;
    sint32 status_state;
    sint32 menu_to_destroy;
    uint32 save_source;
    uint32 save_destination;
    sint32 save_word1;
    sint32 save_word2;
    sint32 save_word3;
    sint32 save_tail_word;
    uint32 overwrite_source;
    uint32 overwrite_destination;
    sint32 overwrite_word1;
    sint32 overwrite_word2;
    sint32 overwrite_word3;
    sint32 overwrite_tail_word;
    sint32 v33;
    sint32 retries_left;
    sint32 result;
    sint32 v36;
    sint32 v37;
    uint8 prefix[4];

    // TODO Original skipped-input path has no initialized button locals
    uint32 confirm = 0;
    uint32 cancel = 0;
    uint32 acknowledge = 0;
    uint32 pause_pressed = 0;
    selected_sound = -1;
    close_menu = 0;
    if ((!r_u8(0x800EC138) || (r_u32(0x800FF080) != 1)))
    {
        ReadMenuInput(&confirm, &cancel, &acknowledge, &pause_pressed);
        ClearCheatInputBuffer();
    }
    v2 = 0;
    if (r_u32(0x800FF080))
        UpdateCardStatus(0, 0);
    if ((r_u8(0x800EC1E8) && r_u8(0x800EC1D8)))
    {
        if (!r_u32(0x800FF098))
        {
            w_u32(0x800FF098, 1);
            w_u32(0x800FF09C, r_u32(0x800FF64C));
        }
        if ((((uint32)((r_u32(0x800FF64C) - r_u32(0x800FF09C)))) >= 0x79))
        {
            w_u32(0x800FF098, 0);
            w_u32(0x800FF2EC, 7);
            close_menu = 1;
        }
    }
    else
    {
        w_u32(0x800FF098, 0);
    }
    switch (r_u32(0x800FF080))
    {
        case 0:
            if ((pause_pressed || !r_u32(0x800EC264)))
            {
                ClearAnimatedHudText();
                sub_80070100(0x800EC0F8);
                w_u32(0x800FF00C, 0);
                SuspendAudioPlayback();
                PlaySoundEffect(23, 0x2000, 0);
                sub_80070288(0, 0);
                sub_80070288(0, 1);
                w_u32(0x800FF008, 1);
                pause_menu = AllocateZeroedHeapBlock(444);
                if (pause_menu)
                {
                    void *dram_addr = AllocateNativeObjectStorage(pause_menu, sizeof(MenuWindow));
                    new (dram_addr) MenuWindow(256, 0, 0, 320, 256, 26);
                }
                w_u32(0x800FF084, pause_menu);
                sub_80015474(pause_menu, ((sint32)(r_u32((0x800A554C + (0) * 4)))));
                sub_80015474(r_u32(0x800FF084), ((sint32)(r_u32((0x800A5580 + (0) * 4)))));
                if (!GetCheatOptions().level_skip)
                    sub_8001551C(r_u32(0x800FF084), ((sint32)(r_u32((0x800A5580 + (0) * 4)))));
                sub_80015474(r_u32(0x800FF084), ((sint32)(r_u32((0x800A555C + (0) * 4)))));
                sub_800152F8(r_u32(0x800FF084));
                MenuWindow::Attach(r_u32(0x800FF084)).pos[1] = (MenuWindow::Attach(r_u32(0x800FF084)).pos[1] + (5));
                if (!r_u32(0x800EC264))
                    goto LABEL_62;
                w_u32(0x800FF080, 1);
            }
            goto LABEL_190;

        case 1:
            ResetPanelDisplayTimer();
            if (!r_u32(0x800FF008))
                close_menu = 1;
            if (!r_u32(0x800EC264))
                w_u32(0x800FF080, 4);
            if (r_u8(0x800EC138))
            {
                if (UpdateCheatInput())
                {
                    w_u32(0x800FF00C, 32);
                    w_u32(0x800FF010, 0);
                }
                sub_80070100(0x800EC0F8);
                goto LABEL_190;
            }
            if (GetCheatOptions().level_skip)
                sub_800154E0(r_u32(0x800FF084), r_u32(0x800A5580));
            else
                sub_8001551C(r_u32(0x800FF084), ((sint32)(r_u32((0x800A5580 + (0) * 4)))));
            MenuWindow::Attach(r_u32(0x800FF084)).Update();
            v4 = -1;
            if (!confirm)
                goto LABEL_43;
            if (pause_pressed)
            {
                v5 = 19;
                close_menu = 1;
            }
            else
            {
                if (sub_800155D4(r_u32(0x800FF084), ((sint32)(r_u32((0x800A554C + (0) * 4))))))
                {
                    selected_sound = 19;
                    close_menu = 1;
                }
                if (sub_800155D4(r_u32(0x800FF084), ((sint32)(r_u32((0x800A5580 + (0) * 4))))))
                {
                    selected_sound = 23;
                    w_u32(0x800FF080, 2);
                }
                v6 = sub_800155D4(r_u32(0x800FF084), ((sint32)(r_u32((0x800A555C + (0) * 4)))));
                v4 = selected_sound;
                if (!v6)
                    goto LABEL_43;
                v5 = 23;
                sub_800153D8(r_u32(0x800FF024));
                w_u32(0x800FF080, 3);
            }
            v4 = v5;
        LABEL_43:
            PlaySoundEffect(v4, 0x2000, 0);

            goto LABEL_190;

        case 2:
            if (!r_u32(0x800FF088))
            {
                level_menu = AllocateZeroedHeapBlock(444);
                if (level_menu)
                {
                    void *dram_addr = AllocateNativeObjectStorage(level_menu, sizeof(MenuWindow));
                    new (dram_addr) MenuWindow(256, 0, 0, 256, 192, 26);
                }
                level_index = 0;
                level_name_cursor = 0x800A71FC;
                w_u32(0x800FF088, level_menu);
                MenuWindow::Attach(level_menu).spacing = 18;
                while (level_index < (sint32)r_u32(0x800FF618))
                {
                    prefix[0] = r_u8(r_u32(level_name_cursor));
                    prefix[1] = r_u8(r_u32(level_name_cursor) + 1);
                    prefix[2] = r_u8(r_u32(level_name_cursor) + 2);
                    prefix[3] = 0;
                    if (!NamesEqualIgnoringCase((const char *)prefix, (const char *)psx_addr(0x800FF0A8, 1)))
                        sub_80015474(r_u32(0x800FF088), r_u32(level_name_cursor));
                    (level_name_cursor += 4);
                    ++level_index;
                }

                sub_800152F8(r_u32(0x800FF088));
            }
            MenuWindow::Attach(r_u32(0x800FF088)).Update();
            if (confirm)
            {
                SelectStartRecord(MenuWindow::Attach(r_u32(0x800FF088)).GetSelectedLabel());
                if (r_u32(0x800FF088))
                    DestroyPauseMenuObject(r_u32(0x800FF088));
                close_menu = 1;
                w_u32(0x800FF088, 0);
                w_u32(0x800FF2EC, 6);
                PlaySoundEffect(23, 0x2000, 0);
            }
            if (cancel)
            {
                if (r_u32(0x800FF088))
                    DestroyPauseMenuObject(r_u32(0x800FF088));
                w_u32(0x800FF088, 0);
                w_u32(0x800FF080, 1);
                PlaySoundEffect(19, 0x2000, 0);
            }
            goto LABEL_190;

        case 3:
            if (!r_u32(0x800EC264))
            {
            LABEL_62:
                w_u32(0x800FF080, 4);

                goto LABEL_190;
            }
            MenuWindow::Attach(r_u32(0x800FF024)).Update();
            if (!confirm)
                goto LABEL_71;
            if (pause_pressed)
            {
                close_menu = 1;
            }
            else
            {
                if (sub_800155D4(r_u32(0x800FF024), r_u32((0x800A5594 + (0) * 4))))
                {
                    close_menu = 1;
                    w_u32(0x800FF2EC, 7);
                    v11 = 23;
                    goto LABEL_70;
                }
                w_u32(0x800FF080, 1);
            }
            v11 = 19;
        LABEL_70:
            PlaySoundEffect(v11, 0x2000, 0);

        LABEL_71:
            v2 = 19;

            if (cancel)
            {
                PlaySoundEffect(19, 0x2000, 0);
                goto LABEL_74;
            }
            goto LABEL_190;

        case 4:
            if (r_u32(0x800EC264))
            LABEL_74:
                w_u32(0x800FF080, 1);

            goto LABEL_190;

        case 5:
            if (r_u8(r_u32(0x800FF030) + r_u32(0x800FF034)))
            {
                v2 = 16;
                if ((((uint32)((r_u32(0x800FF64C) - r_u32(0x800FFB50)))) >= 4))
                {
                    w_u32(0x800FF034, r_u32(0x800FF034) + 1);
                    w_u32(0x800FFB50, r_u32(0x800FF64C));
                    PlaySoundEffect(16, 0x2000, 0);
                }
            }
            else
            {
                if (!r_u32(0x800FF040))
                    goto LABEL_86;
                w_u32(0x800FFB5C, PlaySoundEffect(11, 0x2000, 0));
                w_u32(0x800FF080, 6);
            }
            goto LABEL_190;

        case 6:
            progress_in_range = ((sint32)r_u32(0x800FFB54) < 101);
            if (((sint32)r_u32(0x800FFB54) < 0))
            {
                w_u32(0x800FFB54, 0);
                progress_in_range = 1;
            }
            if (!progress_in_range)
                w_u32(0x800FFB54, 100);
            remaining = (sint32)(r_u32(0x800FFB58) + 1);
            w_u32(0x800FFB58, (uint32)remaining);
            if ((sint32)r_u32(0x800FFB54) < remaining)
            {
                w_u32(0x800FFB58, r_u32(0x800FFB54));
                StopSoundVoices(r_u32(0x800FFB5C));
                goto LABEL_86;
            }
            goto LABEL_190;

        case 7:
            FadeScreenToWhite(32, 1);
            w_u32(0x800FF080, 8);
            goto LABEL_190;

        case 8:
            UpdateScreenFlash();
            if (IsScreenFadeComplete())
            {
                w_u32(0x800FF2EC, 3);
                close_menu = 1;
            }
            goto LABEL_190;

        case 9:
            MenuWindow::Attach(r_u32(0x800FF024)).Update();
            v2 = 23;
            if (confirm)
            {
                PlaySoundEffect(23, 0x2000, 0);
                if (!sub_800155D4(r_u32(0x800FF024), r_u32((0x800A5594 + (0) * 4))))
                    goto LABEL_176;
                w_u32(0x800FF044, 0);
                w_u32(0x800FF080, 10);
            }
            goto LABEL_190;

        case 10:
            timer_snapshot = r_u32(0x800FF02C);
            remaining = (sint32)(timer_snapshot - 1);
            if (timer_snapshot)
            {
                w_u32(0x800FF02C, (uint32)remaining);
                if (!remaining)
                    w_u32(0x800FF080, 11);
            }
            else
            {
                w_u32(0x800FF02C, 10);
            }
            goto LABEL_190;

        case 11:
            card_status = (sint32)r_u32(0x800FEEC4);
            if ((card_status == -1))
            {
                w_u32(0x800FF080, 18);
            }
            else if ((card_status >= 0))
            {
                if ((card_status == 1))
                {
                    operation_failed = QueryCardSaveFile() == 0;
                    card_state = 12;
                    if (!operation_failed)
                    {
                        ::WaitForGameFrames(1);
                        operation_failed = (sint32)GetFreeCardBlocks(0, 0) > 0;
                        card_state = 12;
                        if (!operation_failed)
                            card_state = 17;
                    }
                    w_u32(0x800FF080, card_state);
                    if ((card_state == 12))
                    {
                        card_menu = AllocateZeroedHeapBlock(444);
                        if (card_menu)
                        {
                            void *dram_addr = AllocateNativeObjectStorage(card_menu, sizeof(MenuWindow));
                            new (dram_addr) MenuWindow(256, 55, 0, 256, 192, 18);
                        }
                        w_u32(0x800FF028, card_menu);
                        MenuWindow::Attach(card_menu).numbered = 1;
                        ::PopulateSaveSelectionMenu(r_u32(0x800FF028));
                    }
                }
            }
            else if ((card_status == -2))
            {
                confirmation_menu = r_u32(0x800FF024);
                next_state = 19;
                goto LABEL_127;
            }
            goto LABEL_190;

        case 12:
            MenuWindow::Attach(r_u32(0x800FF028)).Update();
            if (((sint32)r_u32(0x800FEEC4) == -1))
            {
                menu_to_destroy = r_u32(0x800FF028);
                w_u32(0x800FF080, 18);
            LABEL_121:
                if (menu_to_destroy)
                    DestroyPauseMenuObject((uint32)menu_to_destroy);

                w_u32(0x800FF028, 0);
                goto LABEL_190;
            }
            if (cancel)
            {
                PlaySoundEffect(19, 0x2000, 0);
                ::EnterSaveSelectionMenu();
                menu_to_destroy = r_u32(0x800FF028);
                goto LABEL_121;
            }
            v2 = 23;
            if (confirm)
            {
                PlaySoundEffect(23, 0x2000, 0);
                if (r_u32(((uint32)((0x800A493C + (((136 * MenuWindow::Attach(r_u32(0x800FF028)).selected) + 512)) * 1)))))
                {
                    confirmation_menu = r_u32(0x800FF024);
                    next_state = 13;
                LABEL_127:
                    w_u32(0x800FF080, next_state);

                    sub_800153D8(confirmation_menu);
                }
                else
                {
                    save_source = 0x800A53D4;
                    w_u32(0x800A53D4, sub_80018B44(0x800A53D4));
                    v2 = (sint32)0x800A5454;
                    save_destination = (0x800A493C + (((136 * MenuWindow::Attach(r_u32(0x800FF028)).selected) + 512)) * 1);
                    do
                    {
                        copy_first = r_u32(save_source);
                        save_word1 = r_u32((save_source + (1) * 4));
                        save_word2 = r_u32((save_source + (2) * 4));
                        save_word3 = r_u32((save_source + (3) * 4));
                        w_u32(((uint32)(save_destination)), copy_first);
                        w_u32((((uint32)(save_destination)) + (1) * 4), save_word1);
                        w_u32((((uint32)(save_destination)) + (2) * 4), save_word2);
                        w_u32((((uint32)(save_destination)) + (3) * 4), save_word3);
                        save_source += (4) * 4;
                        save_destination += (16) * 1;
                    } while ((save_source != 0x800A5454));
                    copy_first = r_u32(save_source);
                    save_tail_word = r_u32((save_source + (1) * 4));
                    w_u32(((uint32)(save_destination)), copy_first);
                    w_u32((((uint32)(save_destination)) + (1) * 4), save_tail_word);
                    w_u32(0x800FF080, 14);
                    w_u32(0x800FF0A0, 4);
                    w_u32(0x800FF0A4, 4);
                }
            }
            goto LABEL_190;

        case 13:
            MenuWindow::Attach(r_u32(0x800FF024)).Update();
            if (confirm)
            {
                if (sub_800155D4(r_u32(0x800FF024), r_u32((0x800A5594 + (0) * 4))))
                {
                    PlaySoundEffect(23, 0x2000, 0);
                    overwrite_source = 0x800A53D4;
                    w_u32(0x800A53D4, sub_80018B44(0x800A53D4));
                    overwrite_destination = (0x800A493C + (((136 * MenuWindow::Attach(r_u32(0x800FF028)).selected) + 512)) * 1);
                    do
                    {
                        copy_first = r_u32(overwrite_source);
                        overwrite_word1 = r_u32((overwrite_source + (1) * 4));
                        overwrite_word2 = r_u32((overwrite_source + (2) * 4));
                        overwrite_word3 = r_u32((overwrite_source + (3) * 4));
                        w_u32(((uint32)(overwrite_destination)), copy_first);
                        w_u32((((uint32)(overwrite_destination)) + (1) * 4), overwrite_word1);
                        w_u32((((uint32)(overwrite_destination)) + (2) * 4), overwrite_word2);
                        w_u32((((uint32)(overwrite_destination)) + (3) * 4), overwrite_word3);
                        overwrite_source += (4) * 4;
                        overwrite_destination += (16) * 1;
                    } while ((overwrite_source != 0x800A5454));
                    copy_first = r_u32(overwrite_source);
                    overwrite_tail_word = r_u32((overwrite_source + (1) * 4));
                    w_u32(((uint32)(overwrite_destination)), copy_first);
                    w_u32((((uint32)(overwrite_destination)) + (1) * 4), overwrite_tail_word);
                    w_u32(0x800FF080, 14);
                    w_u32(0x800FF0A0, 4);
                    w_u32(0x800FF0A4, 4);
                }
                else
                {
                    PlaySoundEffect(19, 0x2000, 0);
                    w_u32(0x800FF080, 12);
                }
            }
            v2 = 19;
            if (cancel)
                goto LABEL_171;
            goto LABEL_190;

        case 14:
            if (((sint32)r_u32(0x800FEEC4) == -1))
            {
                if (r_u32(0x800FF028))
                    DestroyPauseMenuObject(r_u32(0x800FF028));
                w_u32(0x800FF028, 0);
                w_u32(0x800FF080, 18);
            }
            remaining = (sint32)(r_u32(0x800FF0A0) - 1);
            w_u32(0x800FF0A0, (uint32)remaining);
            if (remaining < 0 && r_u32(0x800FEEC4))
            {
                if (WriteCardSaveFile())
                {
                    retries_left = (r_u32(0x800FF0A4) - 1);
                    w_u32(0x800FF0A4, retries_left);
                    if ((retries_left >= 0))
                        goto LABEL_189;
                    w_u32(0x800FF080, 15);
                }
                else
                {
                    if (r_u32(0x800FF028))
                        DestroyPauseMenuObject(r_u32(0x800FF028));
                    w_u32(0x800FF028, 0);
                    w_u32(0x800FF080, 16);
                }
            }
            goto LABEL_190;

        case 15:
            v2 = 23;
            if (acknowledge)
            {
            LABEL_171:
                PlaySoundEffect(v2, 0x2000, 0);

                w_u32(0x800FF080, 12);
            }
            goto LABEL_190;

        case 16:
            v2 = 23;
            if (acknowledge)
            {
                PlaySoundEffect(23, 0x2000, 0);
            LABEL_176:
                w_u32(0x800FF080, 7);
            }
            goto LABEL_190;

        case 17:
            status_state = 18;
            card_status = (sint32)r_u32(0x800FEEC4);
            if ((card_status == -1))
                goto LABEL_144;
            if ((card_status >= 0))
            {
                status_state = 10;
                if ((card_status == 2))
                    goto LABEL_144;
            }
            else
            {
                status_state = 10;
                if ((card_status == -2))
                    goto LABEL_144;
            }
            goto LABEL_145;

        case 18:
            card_status = (sint32)r_u32(0x800FEEC4);
            if (((card_status == -2) || (((card_status >= -2) && (card_status < 3)) && (card_status > 0))))
            {
                status_state = 10;
            LABEL_144:
                w_u32(0x800FF080, status_state);
            }
        LABEL_145:
            if (acknowledge)
                goto LABEL_86;

            goto LABEL_190;

        case 19:
            MenuWindow::Attach(r_u32(0x800FF024)).Update();
            card_status = (sint32)r_u32(0x800FEEC4);
            if ((card_status == -1))
            {
                v33 = 18;
            LABEL_153:
                w_u32(0x800FF080, v33);

                goto LABEL_154;
            }
            if (((card_status >= -1) && (card_status < 3)))
            {
                v33 = 10;
                if ((card_status > 0))
                    goto LABEL_153;
            }
        LABEL_154:
            if (confirm)
            {
                if (sub_800155D4(r_u32(0x800FF024), r_u32((0x800A5594 + (0) * 4))))
                {
                    PlaySoundEffect(23, 0x2000, 0);
                    w_u32(0x800FF080, 20);
                    w_u32(0x800FF0A0, 4);
                    w_u32(0x800FF0A4, 4);
                }
                else
                {
                    cancel = 1;
                }
            }

            v2 = 19;
            if (cancel)
            {
            LABEL_169:
                PlaySoundEffect(v2, 0x2000, 0);

            LABEL_86:
                ::EnterSaveSelectionMenu();
            }
        LABEL_190:
            if (r_u32(0x800FF008))
            {
                result = r_u32(0x800FF0AC);
                v36 = 951648256;
                if (!result)
                    result = ::CreateMenuBuffers(951706923, 64, 64, 127);
            }
            else
            {
                result = ::HasMenuBuffers();
                w_u32(0x800FF0AC, result);
            }

            if (close_menu)
            {
                cleanup_menu = r_u32(0x800FF084);
                w_u32(0x800FF008, 0);
                w_u32(0x800FF080, 0);
                v37 = 3;
                if (cleanup_menu)
                    DestroyPauseMenuObject(cleanup_menu);
                w_u32(0x800FF084, 0);
                session_state = r_u32(0x800FF2EC);
                if (session_state != 7 && session_state != 3)
                    ResumeAudioPlayback();
                result = r_u32(0x800FF0AC);
                if (!result)
                {
                    ReleaseMovieFrameBuffers();
                    return;
                }
            }
            return;

        case 20:
            remaining = (sint32)(r_u32(0x800FF0A0) - 1);
            w_u32(0x800FF0A0, (uint32)remaining);
            if (remaining < 0)
            {
                v2 = 0;
                if ((sint32)r_u32(0x800FEEC4))
                {
                    if (FormatCard(0, 0) == 1)
                    {
                        w_u32(0x800FF080, 22);
                    }
                    else
                    {
                        retries_left = (r_u32(0x800FF0A4) - 1);
                        w_u32(0x800FF0A4, retries_left);
                        if ((retries_left >= 0))
                        LABEL_189:
                            w_u32(0x800FF0A0, (60 * (4 - (uint32)retries_left)));

                        else
                            w_u32(0x800FF080, 21);
                    }
                }
            }
            goto LABEL_190;

        case 21:
            if (((sint32)r_u32(0x800FEEC4) == -1))
                w_u32(0x800FF080, 18);
            v2 = 23;
            if (acknowledge)
                goto LABEL_169;
            goto LABEL_190;

        case 22:
            v2 = 23;
            if (acknowledge)
            {
                PlaySoundEffect(23, 0x2000, 0);
                w_u32(0x800FF080, 10);
            }
            goto LABEL_190;

        default:
            goto LABEL_190;
    }
}


