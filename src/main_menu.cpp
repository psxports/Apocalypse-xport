#include <new>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "pause_menu.h"
#include "player_save_state.h"
#include "game_pad_poll.h"
#include "game_settings.h"

#include "game_tick.h"
#include "cd_voice_event.h"
#include "level_session.h"
#include "model_projection.h"
#include "movie_lifecycle.h"
#include "resource_io.h"
#include "startup_resources.h"
#include "cheat_input.h"
#include "model_render.h"
#include "angle_math.h"
#include "model_matrix.h"
#include "model_render_view.h"
#include "game_diagnostics.h"
#include "level_catalogue.h"
#include "game_string.h"
#include "game_random.h"
#include "main_menu.h"
#include "game_heap.h"
#include "orbit_camera.h"
#include "model_resources.h"
#include "record_commands.h"
#include "audio_session.h"
#include "game_panel.h"
#include "menu_input.h"
#include "credits.h"
#include "card_save.h"
#include "menu_saves.h"
#include "menu_window.h"
#include "menu_lifecycle.h"
#include "scene_lifecycle.h"
#include "hud_text.h"
#include "hud_sprites.h"
#include "effect_lifecycle.h"
extern "C"
{
#include "game_menu_helpers.h"
}

static const MenuScene menu_scenes[] = {{"roof_lib", "roof_1", {{{0x00001CCE, (sint32)0xFFFFFB63, 0x000014D4}}}, 1744, 768}, {"sewr_lib", "sewr_4", {{{(sint32)0xFFFFD3F0, 0x00002AB2, 0x000150FD}}}, 3536, 2304}};

static uint32 cur_menu_scene = 0;

void ResetMenuScenes()
{
    cur_menu_scene = 0;
}

static void AdvanceMenuScene()
{
    ++cur_menu_scene;
    if ((sint32)cur_menu_scene >= (sint32)(sizeof(menu_scenes) / sizeof(menu_scenes[0])))
        cur_menu_scene = 0;
}

static const MenuScene &GetCurMenuScene()
{
    if (cur_menu_scene >= sizeof(menu_scenes) / sizeof(menu_scenes[0]))
    {
        GameDiagnostic("Invalid menu scene index\n");
        abort();
    }
    return menu_scenes[cur_menu_scene];
}

static void QueueSelectedLevelResources()
{
    FUNCTION_MARKER(0x80011B34, "SLUS_003.73");
    QueueModelBankLoad(GetCurMenuScene().library, 0);
    QueueModelBankLoad(GetCurMenuScene().bank, 1);
    AllocateScriptWorkspace(250000);
}

enum MainMenuPage
{
    MAIN_MENU_ROOT = 0,
    MAIN_MENU_LEVEL_SELECT = 1,
    MAIN_MENU_OPTIONS = 3,
    MAIN_MENU_EXTRAS = 4,
    MAIN_MENU_SCREEN_POSITION = 5,
    MAIN_MENU_DIFFICULTY = 6,
    MAIN_MENU_AUDIO = 7,
    MAIN_MENU_CONTROLLER = 8,
    MAIN_MENU_ANALOG_RANGE = 9,
    MAIN_MENU_ANALOG_CENTER = 10,
    MAIN_MENU_VIBRATION = 11,
    MAIN_MENU_RESET_CONFIRM = 12,
    MAIN_MENU_CARD_DELAY = 13,
    MAIN_MENU_CARD_QUERY = 14,
    MAIN_MENU_CARD_NO_SAVE = 15,
    MAIN_MENU_CARD_UNFORMATTED = 16,
    MAIN_MENU_LOAD_SLOTS = 17,
    MAIN_MENU_CARD_MISSING = 18
};

uint32 RunMainMenu()
{
    sint32 prompt_visible_frames;
    sint32 prompt_hidden_frames;
    bool start_released;

    MenuWindow *main_window;

    MenuWindow *controller_window;

    MenuWindow *preset_window;
    sint32 menu_state;
    sint32 heading_y;
    const char *heading;
    MenuWindow *submenu;
    POLY_FT4 *cancel_sprite;
    POLY_FT4 *accept_sprite;
    bool preview_selected;
    const char *audio_action_label;
    POLY_FT4 *audio_select_sprite;
    bool binding_selected;
    POLY_FT4 *go_back_sprite;
    const char *binding_label;
    const char *message;
    sint32 message_y;
    sint32 message_x;
    MenuWindow *confirm_window;
    sint32 action_sound;
    sint32 frame_sound;
    bool binding_confirmed;
    sint32 button_mask;
    uint32 button_mask_minus_one;
    sint16 *sel_binding;
    sint32 volume_step;
    sint32 voice_preview;
    sint32 next_card_delay;

    bool close_save_window;
    bool invalid_slot;
    char saved_level_name[sizeof(player_saves.selected.progress.level)];
    uint32 item_pos[2];
    uint32 confirm_pressed;
    uint32 cancel_pressed;
    uint32 any_pressed;
    uint32 start_pressed;
    sint32 result_state;
    bool finished;
    const char *level_label;
    const LevelEntry *level_entry;
    uint32 level_idx;
    OrbitCamera *background_camera;
    MenuWindow *level_window;
    MenuWindow *options_window;
    MenuWindow *extras_window;
    MenuWindow *difficulty_window;
    MenuWindow *audio_window;
    MenuWindow *save_window;
    sint32 card_delay;
    sint32 card_timeout;
    sint32 saved_screen_pos[2];
    sint32 rollback_frames;
    sint32 music_preview_idx;
    uint32 last_activity_frame;
    uint32 frame_start;
    uint32 saved_range[4][2];
    FUNCTION_MARKER(0x80011BA0, "SLUS_003.73");
    result_state = 0;
    finished = false;
    BeginSceneFrame();
    SubmitSceneFrame();
    LoadStartupImage(0, "title.bmr", 240);
    scene_render.cur = &scene_render.buffers[1];
    BeginSceneFrame();
    UpdateScenePacketLimit();
    SetHudTextScale(256);
    SetHudTextAlignment(0);
    SetHudTextColor(128, 128, 128, 0);
    DrawStyledHudText(256, 120, MenuText::press_start, 0, 0, 256);
    scene_render.buffers[0].draw.isbg = 0;
    scene_render.buffers[1].draw.isbg = 0;
    SubmitSceneFrame();
    DrawSync(0);
    scene_render.buffers[0].draw.isbg = 1;
    scene_render.buffers[1].draw.isbg = 1;
    PutDispEnv(&scene_render.buffers[0].disp);
    AdvanceMenuScene();
    // Skip the original title-screen input and prompt delay
    menu_runtime.prompt_delay = 0;
    prompt_visible_frames = 144;
    QueueSelectedLevelResources();
    prompt_hidden_frames = 0;
    PollPadPackets();
    pad_polling.idle_ticks = 0;
    start_released = (game_pads[0].buttons[14].down == 0);
    do
    {
        PollPadPackets();
        if (menu_runtime.prompt_delay)
        {
            --menu_runtime.prompt_delay;
        }
        else
        {
            if (!game_pads[0].buttons[14].down)
                start_released = true;
            if ((start_released && game_pads[0].buttons[14].down))
            {
                PlaySoundEffect(23, 0x2000, 0);
                goto OpenMainMenu;
            }
            if (prompt_visible_frames)
            {
                PutDispEnv(&scene_render.buffers[1].disp);
                if (!(--prompt_visible_frames))
                    prompt_hidden_frames = 24;
            }
            else
            {
                PutDispEnv(&scene_render.buffers[0].disp);
            }
            if (prompt_hidden_frames)
            {
                if (!(--prompt_hidden_frames))
                    prompt_visible_frames = 144;
            }
        }
        ::ProcessResourceRead();
        ::ProcessModelBankLoadQueue();
        (void)0;
        ::WaitForGameFrames(1);
    } while (((sint32)pad_polling.idle_ticks < 1801));
    result_state = 3;
    finished = true;
OpenMainMenu:
    game_pads[0].buttons[14].pressed = 0;

    game_pads[0].buttons[3].pressed = 0;
    ::ClearStartupImageArea();
    ::WaitForModelBankLoads();
    ::CreateMenuBuffers(951706923, 64, 64, 127);
    ::ConfigureProjectionDepth(10, 10240, 0x2000);
    {
        const MenuScene &scene = GetCurMenuScene();
        background_camera = new OrbitCamera(scene.pos.words, scene.radius, scene.height);
    }
    main_window = new MenuWindow(256, 0, 0, 0x140, 0x100, 0x1A);
    main_window->AddItem(MenuText::continue_game);
    if (!player_saves.current.checksum || NamesEqualIgnoringCase(player_saves.selected.progress.level, GetLevelEntry(0)->name))
        main_window->DisableItem(MenuText::continue_game);
    main_window->AddItem(MenuText::new_game);
    if (GetCheatOptions().autotest)
        main_window->AddItem(MenuText::autotest);
    main_window->AddItem(MenuText::load_game);
    main_window->AddItem(MenuText::options);
    if (GetCheatOptions().level_select)
        main_window->AddItem(MenuText::select_level);
    main_window->CenterVertical();
    level_window = new MenuWindow(256, 0, 0, 0x100, 0xC0, 0x1A);
    level_window->spacing = 18;
    for (level_idx = 0; level_idx < GetLevelCount(); ++level_idx)
        level_window->AddItem(GetLevelEntry(level_idx)->title);
    level_window->CenterVertical();
    options_window = new MenuWindow(256, 63, 0, 0x140, 0x100, 0x1A);
    options_window->AddItem(MenuText::difficulty);
    options_window->AddItem(MenuText::sound);
    options_window->AddItem(MenuText::controller_config);
    options_window->AddItem(MenuText::misc_options);
    options_window->AddItem(MenuText::return_to_main_menu);
    extras_window = new MenuWindow(256, 42, 0, 0x140, 0x100, 0x1A);
    extras_window->AddItem(MenuText::view_credits);
    extras_window->AddItem(MenuText::music_credits);
    extras_window->AddItem(MenuText::view_intro);
    extras_window->AddItem(MenuText::reset_to_defaults);
    extras_window->AddItem(MenuText::center_screen);
    extras_window->AddItem(MenuText::return_to_options_menu);
    difficulty_window = new MenuWindow(256, 55, 0, 0x140, 0x100, 0x1A);
    difficulty_window->AddItem(MenuText::easy);
    difficulty_window->AddItem(MenuText::normal);
    difficulty_window->AddItem(MenuText::hard);
    difficulty_window->AddItem(MenuText::return_to_options_menu);
    difficulty_window->has_arrows = 1;
    audio_window = new MenuWindow(256, 85, 0, 0x140, 0x100, 0x1A);
    audio_window->AddItem(MenuText::fx_level);
    audio_window->AddItem(MenuText::music_level);
    audio_window->AddItem(MenuText::voice_level);
    audio_window->AddItem(MenuText::return_to_options_menu);
    controller_window = new MenuWindow(30, 25, 1, 0xC0, 0xC0, 0x1A);
    controller_window->AddItem(MenuText::weapon_select);
    controller_window->AddItem(MenuText::crouch);
    controller_window->AddItem(MenuText::jump);
    controller_window->AddItem(MenuText::smart_bomb);
    controller_window->AddItem(MenuText::vibration_settings);
    controller_window->AddItem(MenuText::calibrate_analog_controller);
    controller_window->SetItemHeight(MenuText::vibration_settings, 10);
    controller_window->DisableItem(MenuText::vibration_settings);
    controller_window->DisableItem(MenuText::calibrate_analog_controller);
    controller_window->AddItem(MenuText::reset_to_defaults);
    controller_window->AddItem(MenuText::return_to_options_menu);
    controller_window->spacing = 22;
    preset_window = new MenuWindow(256, 77, 0, 0x140, 0x100, 0x1A);
    save_window = 0;
    card_delay = 0;
    rollback_frames = 0;
    music_preview_idx = 0;
    menu_state = MAIN_MENU_ROOT;
    preset_window->AddItem(MenuText::none);
    preset_window->AddItem(MenuText::some);
    preset_window->AddItem(MenuText::full);
    preset_window->AddItem(MenuText::return_to_controller_config);
    preset_window->items[3].unselected_scale = 192;
    preset_window->items[3].selected_scale = 192;
    preset_window->has_arrows = 1;
    RestoreGameSettings(player_saves.selected.settings);
    pad_polling.idle_ticks = 0;
    last_activity_frame = game_clock.vblank_frame;
    while (!finished)
    {
        (void)0;
        BeginSceneFrame();
        UpdateScenePacketLimit();
        frame_start = game_clock.vblank_frame;
        if (rollback_frames)
        {
            if (!(--rollback_frames))
            {
                (scene_render.buffers[0].disp.screen.x = (sint16)(saved_screen_pos[0]));
                (scene_render.buffers[1].disp.screen.x = (sint16)(saved_screen_pos[0]));
                (scene_render.buffers[0].disp.screen.y = (sint16)(saved_screen_pos[1]));
                (scene_render.buffers[1].disp.screen.y = (sint16)(saved_screen_pos[1]));
            }
        }
        ::BlurMenuBuffers();
        ::BlurMenuBuffers();
        UpdateHudTextGlowCurve();
        BuildModelRotationMatrix(GetModelRenderView().angles.words, GetModelRenderView().rotation);
        SetModelRenderViewTranslation(GetModelRenderView());
        ConfigureModelRenderView(GetModelRenderView(), model_viewport, scene_render.cur->ot);
        ::RenderVisibleModelObjectList(GetModelSceneObjects());
        ::AdvanceModelRenderCounter();
        SetHudTextScale(256);
        SetHudTextAlignment(0);
        switch (menu_state)
        {
            case MAIN_MENU_ROOT:
                main_window->Draw();
                SetHudTextScale(256);
                if (GetCheatOptions().build_date)
                {
                    SetHudTextColor(149, 20, 20, 0);
                    heading_y = 20;
                    heading = MenuText::build_date;
                }
                else
                {
                    SetHudTextColor(149, 20, 20, 0);
                    heading_y = 28;
                    heading = MenuText::main_menu;
                }
                DrawStyledHudText(256, heading_y, heading, 0, 0, 256);
                ::DrawMenuCenteredHint();
                break;

            case MAIN_MENU_LEVEL_SELECT:
                level_window->Draw();
                break;

            case MAIN_MENU_OPTIONS:
                submenu = options_window;
                goto DrawSubmenuHints;

            case MAIN_MENU_EXTRAS:
                submenu = extras_window;
            DrawSubmenuHints:
                submenu->Draw();

                ::DrawMenuHints();
                break;

            case MAIN_MENU_SCREEN_POSITION:
                ::SetMenuGreenText();
                DrawStyledHudText(256, 80, MenuText::use_the_directional, 0, 0, 256);
                DrawStyledHudText(256, 106, MenuText::buttons_to_center, 0, 0, 256);
                DrawStyledHudText(256, 132, MenuText::the_screen, 0, 0, 256);
                SetHudTextAlignment(1);
                ::SetMenuDarkRedText();
                DrawStyledHudText(104, 210, MenuText::cancel, 0, 0, 256);
                DrawStyledHudText(328, 210, MenuText::accept, 0, 0, 256);
                cancel_sprite = AllocateHudSprite(GetTextureFrame(GetButtonResourceGroup(), 1));
                if (cancel_sprite)
                    PlaceScaledHudSprite(76, 211, cancel_sprite, GetTextureFrame(GetButtonResourceGroup(), 1));
                accept_sprite = AllocateHudSprite(GetTextureFrame(GetButtonResourceGroup(), 0));
                if (accept_sprite)
                    PlaceScaledHudSprite(300, 211, accept_sprite, GetTextureFrame(GetButtonResourceGroup(), 0));
                AllocateHudRectangle(0, 0, 512, 5, 0, 0, 255, 0);
                AllocateHudRectangle(0, 235, 512, 5, 0, 0, 255, 0);
                AllocateHudRectangle(0, 0, 7, 240, 0, 0, 255, 0);
                AllocateHudRectangle(505, 0, 7, 240, 0, 0, 255, 0);
                break;

            case MAIN_MENU_DIFFICULTY:
                ::DrawMenuHints();
                difficulty_window->Draw();
                break;

            case MAIN_MENU_AUDIO:
                preview_selected = false;
                ::DrawMenuLeftHint();
                SetHudTextAlignment(1);
                SetHudTextScale(192);
                if ((audio_window->IsSelected(MenuText::fx_level) || audio_window->IsSelected(MenuText::voice_level)))
                    preview_selected = true;
                if (preview_selected)
                {
                    audio_action_label = MenuText::test;
                }
                else if (audio_window->IsSelected(MenuText::music_level))
                {
                    audio_action_label = MenuText::cycle;
                }
                else
                {
                    audio_action_label = MenuText::select;
                }
                DrawStyledHudText(348, 210, audio_action_label, 0, 0, 256);
                SetHudTextAlignment(0);
                audio_select_sprite = AllocateHudSprite(GetTextureFrame(GetButtonResourceGroup(), 0));
                if (audio_select_sprite)
                    PlaceScaledHudSprite(320, 211, audio_select_sprite, GetTextureFrame(GetButtonResourceGroup(), 0));
                DrawMenuVolumeSlider(*audio_window);
                audio_window->Draw();
                break;

            case MAIN_MENU_CONTROLLER:
                controller_window->Draw();
                binding_selected = false;
                if ((((controller_window->IsSelected(MenuText::weapon_select) || controller_window->IsSelected(MenuText::crouch)) || controller_window->IsSelected(MenuText::jump)) || controller_window->IsSelected(MenuText::smart_bomb)))
                {
                    binding_selected = true;
                }
                if (binding_selected)
                {
                    SetHudTextAlignment(1);
                    SetHudTextScale(256);
                    ::SetMenuDarkRedText();
                    DrawStyledHudText(211, 210, MenuText::go_back, 0, 0, 256);
                    go_back_sprite = AllocateHudSprite(GetTextureFrame(GetButtonResourceGroup(), 1));
                    if (go_back_sprite)
                        PlaceScaledHudSprite(183, 211, go_back_sprite, GetTextureFrame(GetButtonResourceGroup(), 1));
                }
                else
                {
                    ::DrawMenuHints();
                }
                SetHudTextAlignment(1);
                ::SetMenuRedText();
                SetHudTextScale(192);
                if ((game_pads[0].type == 65))
                {
                    controller_window->GetItemPosition(MenuText::weapon_select, &item_pos[0], &item_pos[1]);
                    binding_label = ReadMenuButtonLabel(game_settings.bindings.digital[3]);
                    DrawStyledHudText(((uint32)(item_pos[0]) + (uint32)(315)), item_pos[1], binding_label, 0, 0, 256);
                    controller_window->GetItemPosition(MenuText::crouch, &item_pos[0], &item_pos[1]);
                    binding_label = ReadMenuButtonLabel(game_settings.bindings.digital[0]);
                    DrawStyledHudText(((uint32)(item_pos[0]) + (uint32)(315)), item_pos[1], binding_label, 0, 0, 256);
                    controller_window->GetItemPosition(MenuText::jump, &item_pos[0], &item_pos[1]);
                    binding_label = ReadMenuButtonLabel(game_settings.bindings.digital[1]);
                    DrawStyledHudText(((uint32)(item_pos[0]) + (uint32)(315)), item_pos[1], binding_label, 0, 0, 256);
                    controller_window->GetItemPosition(MenuText::smart_bomb, &item_pos[0], &item_pos[1]);
                    message = ReadMenuButtonLabel(game_settings.bindings.digital[2]);
                }
                else
                {
                    controller_window->GetItemPosition(MenuText::weapon_select, &item_pos[0], &item_pos[1]);
                    binding_label = ReadMenuButtonLabel(game_settings.bindings.analog[3]);
                    DrawStyledHudText(((uint32)(item_pos[0]) + (uint32)(315)), item_pos[1], binding_label, 0, 0, 256);
                    controller_window->GetItemPosition(MenuText::crouch, &item_pos[0], &item_pos[1]);
                    binding_label = ReadMenuButtonLabel(game_settings.bindings.analog[0]);
                    DrawStyledHudText(((uint32)(item_pos[0]) + (uint32)(315)), item_pos[1], binding_label, 0, 0, 256);
                    controller_window->GetItemPosition(MenuText::jump, &item_pos[0], &item_pos[1]);
                    binding_label = ReadMenuButtonLabel(game_settings.bindings.analog[1]);
                    DrawStyledHudText(((uint32)(item_pos[0]) + (uint32)(315)), item_pos[1], binding_label, 0, 0, 256);
                    controller_window->GetItemPosition(MenuText::smart_bomb, &item_pos[0], &item_pos[1]);
                    message = ReadMenuButtonLabel(game_settings.bindings.analog[2]);
                }
                message_y = item_pos[1];
                message_x = ((uint32)(item_pos[0]) + (uint32)(315));
                goto DrawMenuMessage;

            case MAIN_MENU_ANALOG_RANGE:
                ::SetMenuGreenText();
                DrawStyledHudText(256, 80, MenuText::move_the_left_stick, 0, 0, 256);
                DrawStyledHudText(256, 106, MenuText::and_right_stick_to_all, 0, 0, 256);
                DrawStyledHudText(256, 132, MenuText::four_corners_then_press, 0, 0, 256);
                message_x = 256;
                message = MenuText::the_x_button;
                message_y = 158;
                goto DrawMenuMessage;

            case MAIN_MENU_ANALOG_CENTER:
                ::SetMenuGreenText();
                DrawStyledHudText(256, 90, MenuText::center_both_the_left_stick, 0, 0, 256);
                DrawStyledHudText(256, 116, MenuText::and_the_right_stick_then, 0, 0, 256);
                message_x = 256;
                message = MenuText::press_the_x_button;
                message_y = 142;
                goto DrawMenuMessage;

            case MAIN_MENU_VIBRATION:
                ::SetMenuDarkRedText();
                DrawStyledHudText(256, 34, MenuText::vibration_settings, 0, 0, 256);
                ::DrawMenuHints();
                preset_window->Draw();
                break;

            case MAIN_MENU_RESET_CONFIRM:
                ::DrawMenuHints();
                ::SetMenuDarkRedText();
                DrawStyledHudText(256, 80, MenuText::really_reset_to_defaults, 0, 0, 256);
                confirm_window = menu_runtime.confirm;
                menu_runtime.confirm->scr_pos[1] = 131;
                confirm_window->Draw();
                break;

            case MAIN_MENU_CARD_DELAY:

            case MAIN_MENU_CARD_QUERY:
                ::SetMenuGreenText();
                DrawStyledHudText(256, 90, MenuText::checking_memory_card, 0, 0, 256);
                ::SetMenuRedText();
                DrawStyledHudText(256, 140, MenuText::do_not_remove, 0, 0, 256);
                message_x = 256;
                message = MenuText::memory_card;
                message_y = 166;
                goto DrawMenuMessage;

            case MAIN_MENU_CARD_NO_SAVE:
                ::SetMenuRedText();
                DrawStyledHudText(256, 90, MenuText::memory_card_contains, 0, 0, 256);
                DrawStyledHudText(256, 116, MenuText::no_apocalypse_game_save, 0, 0, 256);
                DrawStyledHudText(256, 142, MenuText::press_any_button, 0, 0, 256);
                message_x = 256;
                message_y = 168;
                message = MenuText::to_cancel;
                goto DrawMenuMessage;

            case MAIN_MENU_CARD_UNFORMATTED:
                ::SetMenuRedText();
                DrawStyledHudText(256, 90, MenuText::the_memory_card_in, 0, 0, 256);
                DrawStyledHudText(256, 116, MenuText::memory_card_slot_1_is, 0, 0, 256);
                DrawStyledHudText(256, 142, MenuText::unformatted_press, 0, 0, 256);
                message_x = 256;
                message = MenuText::any_button_to_cancel;
                message_y = 168;
                goto DrawMenuMessage;

            case MAIN_MENU_LOAD_SLOTS:
                ::DrawMenuHints();
                ::SetMenuRedText();
                DrawStyledHudText(256, 25, MenuText::load_game, 0, 0, 256);
                save_window->Draw();
                break;

            case MAIN_MENU_CARD_MISSING:
                ::SetMenuRedText();
                DrawStyledHudText(256, 90, MenuText::please_insert_a_memory, 0, 0, 256);
                DrawStyledHudText(256, 116, MenuText::card_in_memory_card, 0, 0, 256);
                DrawStyledHudText(256, 142, MenuText::slot_1_or_press_any, 0, 0, 256);
                message_x = 256;
                message = MenuText::button_to_cancel;
                message_y = 168;

            DrawMenuMessage:
                DrawStyledHudText(message_x, message_y, message, 0, 0, 256);

                break;

            default:
                break;
        }

        if (++game_clock.tick & 1)
            UpdateCardStatus(0, 0);
        PollPadPackets();
        if (menu_state)
            last_activity_frame = game_clock.vblank_frame;
        ReadMenuInput(&confirm_pressed, &cancel_pressed, &any_pressed, &start_pressed);
        action_sound = -1;
        if (confirm_pressed)
            action_sound = 23;
        if (cancel_pressed)
            action_sound = 19;
        switch (menu_state)
        {
            case MAIN_MENU_ROOT:
                main_window->Update();
                if (confirm_pressed)
                {
                    if (main_window->IsSelected(MenuText::continue_game))
                    {
                        memcpy(saved_level_name, player_saves.selected.progress.level, sizeof(saved_level_name));
                        player_saves.selected.progress = GetDefaultPlayerSave().progress;
                        player_saves.selected.progress.SetLevel(saved_level_name);
                        finished = true;
                        player_saves.selected.progress.checkpoint[0] = 0;
                        game_settings.difficulty = player_saves.selected.settings.difficulty;
                    }
                    if (main_window->IsSelected(MenuText::select_level))
                        menu_state = MAIN_MENU_LEVEL_SELECT;
                    if (main_window->IsSelected(MenuText::new_game))
                    {
                        action_sound = 23;
                        player_saves.selected.progress = GetDefaultPlayerSave().progress;
                        finished = true;
                    }
                    if (main_window->IsSelected(MenuText::autotest))
                    {
                        result_state = 2;
                        finished = true;
                    }
                    if (main_window->IsSelected(MenuText::load_game))
                        menu_state = MAIN_MENU_CARD_DELAY;
                    if (main_window->IsSelected(MenuText::options))
                        menu_state = MAIN_MENU_OPTIONS;
                }
                if (cancel_pressed)
                    action_sound = -1;
                if (!pad_polling.idle_ticks)
                {
                    last_activity_frame = game_clock.vblank_frame;
                    goto QueueActionSound;
                }
                frame_sound = action_sound;
                if ((((uint32)(((uint32)(game_clock.vblank_frame) - (uint32)(last_activity_frame)))) >= 0x709))
                {
                    result_state = 3;
                    finished = true;
                }
                goto FinalizeMenuFrame;

            case MAIN_MENU_LEVEL_SELECT:
                level_window->Update();
                if (!confirm_pressed)
                    goto HandleLevelCancel;
                level_label = level_window->GetSelectedLabel();
                player_saves.selected.progress.level[0] = 0;
                for (level_idx = 0; level_idx < GetLevelCount(); ++level_idx)
                {
                    level_entry = GetLevelEntry(level_idx);
                    if (NamesEqualIgnoringCase(level_label, level_entry->title))
                    {
                        player_saves.selected.progress.SetLevel(level_entry->name);
                        action_sound = 23;
                        goto FinishLevelSelection;
                    }
                }
                action_sound = 23;

            FinishLevelSelection:
                player_saves.selected.progress.checkpoint[0] = 0;

                finished = true;
            HandleLevelCancel:
                frame_sound = action_sound;

                if (cancel_pressed)
                    menu_state = MAIN_MENU_ROOT;
                goto FinalizeMenuFrame;

            case MAIN_MENU_OPTIONS:
                options_window->Update();
                if (confirm_pressed)
                {
                    if (options_window->IsSelected(MenuText::difficulty))
                    {
                        difficulty_window->arrow_item = game_settings.difficulty;
                        menu_state = MAIN_MENU_DIFFICULTY;
                        difficulty_window->ResetSelection(((uint8)(game_settings.difficulty)));
                    }
                    if (options_window->IsSelected(MenuText::sound))
                    {
                        action_sound = -1;
                        PlaySoundEffect(23, 0x2000, 0);
                        menu_state = MAIN_MENU_AUDIO;
                        music_preview_idx = 0;
                        SwitchMenuAudioSequence(GetLevelEntry(0)->music);
                    }
                    if (options_window->IsSelected(MenuText::controller_config))
                        menu_state = MAIN_MENU_CONTROLLER;
                    if (options_window->IsSelected(MenuText::misc_options))
                        menu_state = MAIN_MENU_EXTRAS;
                    if (options_window->IsSelected(MenuText::return_to_main_menu))
                    {
                        action_sound = 19;
                        menu_state = MAIN_MENU_ROOT;
                    }
                }
                frame_sound = action_sound;
                if (cancel_pressed)
                    menu_state = MAIN_MENU_ROOT;
                goto FinalizeMenuFrame;

            case MAIN_MENU_EXTRAS:
                extras_window->Update();
                if (confirm_pressed)
                {
                    if (extras_window->IsSelected(MenuText::view_credits))
                    {
                        PlaySoundEffect(action_sound, 0x2000, 0);
                        action_sound = -1;
                        RunCredits();
                    }
                    if (extras_window->IsSelected(MenuText::music_credits))
                    {
                        PlaySoundEffect(action_sound, 0x2000, 0);
                        action_sound = -1;
                        ::ReleaseLevelLoadResources();
                        ::PlayMovie(33);
                        QueueSelectedLevelResources();
                        ::WaitForModelBankLoads();
                    }
                    if (extras_window->IsSelected(MenuText::view_intro))
                    {
                        ::ReleaseLevelLoadResources();
                        ::PlayMovie(28);
                        QueueSelectedLevelResources();
                        ::WaitForModelBankLoads();
                    }
                    if (extras_window->IsSelected(MenuText::reset_to_defaults))
                    {
                        menu_state = MAIN_MENU_RESET_CONFIRM;
                        menu_runtime.confirm->ResetSelection();
                    }
                    if (extras_window->IsSelected(MenuText::center_screen))
                    {
                        menu_state = MAIN_MENU_SCREEN_POSITION;
                        saved_screen_pos[0] = (sint16)(uint16)scene_render.buffers[0].disp.screen.x;
                        saved_screen_pos[1] = (sint16)(uint16)scene_render.buffers[0].disp.screen.y;
                    }
                    if (extras_window->IsSelected(MenuText::return_to_options_menu))
                    {
                        action_sound = 19;
                        menu_state = MAIN_MENU_OPTIONS;
                    }
                }
                goto HandleOptionsCancel;

            case MAIN_MENU_SCREEN_POSITION:
                if ((game_pads[0].buttons[9].down && ((sint16)(uint16)scene_render.buffers[0].disp.screen.x < 32)))
                {
                    scene_render.buffers[0].disp.screen.x = (sint16)((uint16)scene_render.buffers[0].disp.screen.x + 1);
                    scene_render.buffers[1].disp.screen.x = (sint16)((uint16)scene_render.buffers[1].disp.screen.x + 1);
                }
                if ((game_pads[0].buttons[8].down && ((sint16)(uint16)scene_render.buffers[0].disp.screen.x > 0)))
                {
                    scene_render.buffers[0].disp.screen.x = (sint16)((uint16)scene_render.buffers[0].disp.screen.x - 1);
                    scene_render.buffers[1].disp.screen.x = (sint16)((uint16)scene_render.buffers[1].disp.screen.x - 1);
                }
                if ((game_pads[0].buttons[10].down && ((sint16)(uint16)scene_render.buffers[0].disp.screen.y > 0)))
                {
                    scene_render.buffers[0].disp.screen.y = (sint16)((uint16)scene_render.buffers[0].disp.screen.y - 1);
                    scene_render.buffers[1].disp.screen.y = (sint16)((uint16)scene_render.buffers[1].disp.screen.y - 1);
                }
                if ((game_pads[0].buttons[11].down && ((sint16)(uint16)scene_render.buffers[0].disp.screen.y < 32)))
                {
                    scene_render.buffers[0].disp.screen.y = (sint16)((uint16)scene_render.buffers[0].disp.screen.y + 1);
                    scene_render.buffers[1].disp.screen.y = (sint16)((uint16)scene_render.buffers[1].disp.screen.y + 1);
                }
                if (confirm_pressed)
                    menu_state = MAIN_MENU_EXTRAS;
                if (cancel_pressed)
                {
                    rollback_frames = 2;
                    menu_state = MAIN_MENU_EXTRAS;
                }
                goto QueueActionSound;

            case MAIN_MENU_DIFFICULTY:
                difficulty_window->Update();
                if (confirm_pressed)
                {
                    if (difficulty_window->IsSelected(MenuText::easy))
                    {
                        action_sound = 4;
                        game_settings.difficulty = 0;
                    }
                    if (difficulty_window->IsSelected(MenuText::normal))
                    {
                        action_sound = 6;
                        game_settings.difficulty = 1;
                    }
                    if (difficulty_window->IsSelected(MenuText::hard))
                    {
                        action_sound = 8;
                        game_settings.difficulty = 2;
                    }
                    if (difficulty_window->IsSelected(MenuText::return_to_options_menu))
                    {
                        action_sound = 19;
                        menu_state = MAIN_MENU_OPTIONS;
                    }
                }
                difficulty_window->arrow_item = game_settings.difficulty;
            HandleOptionsCancel:
                frame_sound = action_sound;

                if (cancel_pressed)
                    menu_state = MAIN_MENU_OPTIONS;
                goto FinalizeMenuFrame;

            case MAIN_MENU_AUDIO:
                audio_window->Update();
                volume_step = ReadMenuNavigationStep();
                if ((audio_window->IsSelected(MenuText::fx_level) && (volume_step || confirm_pressed)))
                {
                    PlaySoundEffect(16, 0x2000, 0);
                    game_settings.effects_volume = (game_settings.effects_volume + (((uint32)(((uint16)(volume_step))) << (uint32)(10))));
                    if (((game_settings.effects_volume & 0x8000) != 0))
                        game_settings.effects_volume = 0;
                    if ((game_settings.effects_volume >= 0x4000))
                        game_settings.effects_volume = 0x3FFF;
                }
                if (audio_window->IsSelected(MenuText::music_level))
                {
                    game_settings.music_volume = (game_settings.music_volume + (((uint32)(8) * (uint32)(volume_step))));
                    if (((game_settings.music_volume & 0x8000) != 0))
                        game_settings.music_volume = 0;
                    if ((game_settings.music_volume >= 128))
                        game_settings.music_volume = 127;
                    ((void)(0), ::RestoreMusicPlayback());
                    if (confirm_pressed)
                    {
                        if (++music_preview_idx >= (sint32)GetLevelCount())
                            music_preview_idx = 0;
                        SwitchMenuAudioSequence(GetLevelEntry(music_preview_idx)->music);
                        ((void)(0), ::RestoreMusicPlayback());
                    }
                }
                else
                {
                    ::StopMusicPlayback();
                }
                if (audio_window->IsSelected(MenuText::voice_level))
                {
                    if (((volume_step || confirm_pressed) || !menu_runtime.voice_previewed))
                    {
                        voice_preview = RandomRange(2);
                        ::SelectCdVoice(0, voice_preview);
                        game_settings.voice_volume = (game_settings.voice_volume + (((uint32)(16) * (uint32)(volume_step))));
                        if (((game_settings.voice_volume & 0x8000) != 0))
                            game_settings.voice_volume = 0;
                        if ((game_settings.voice_volume >= 256))
                            game_settings.voice_volume = 255;
                        menu_runtime.voice_previewed = 1;
                    }
                }
                else
                {
                    menu_runtime.voice_previewed = 0;
                }
                if (((!audio_window->IsSelected(MenuText::return_to_options_menu) || !confirm_pressed) && !cancel_pressed))
                    goto SkipMenuSound;
                menu_state = MAIN_MENU_OPTIONS;
                ::ReleaseAudioSession();
                menu_runtime.voice_previewed = 0;
                frame_sound = 19;
                goto FinalizeMenuFrame;

            case MAIN_MENU_CONTROLLER:
                if (((game_pads[0].type == 65) || (PadGetState(0) != 2)))
                    controller_window->DisableItem(MenuText::calibrate_analog_controller);
                else
                    controller_window->EnableItem(MenuText::calibrate_analog_controller);
                if ((PadGetState(0) == 6))
                    controller_window->EnableItem(MenuText::vibration_settings);
                else
                    controller_window->DisableItem(MenuText::vibration_settings);
                controller_window->Update();
                binding_confirmed = false;
                if (confirm_pressed)
                {
                    if ((((controller_window->IsSelected(MenuText::weapon_select) || controller_window->IsSelected(MenuText::crouch)) || controller_window->IsSelected(MenuText::jump)) || controller_window->IsSelected(MenuText::smart_bomb)))
                    {
                        binding_confirmed = true;
                    }
                    if (binding_confirmed)
                        action_sound = -1;
                }
                button_mask = (game_pads[0].buttons[5].down != 0);
                if (game_pads[0].buttons[7].down)
                    button_mask |= 2;
                if (game_pads[0].buttons[4].down)
                    button_mask |= 4;
                button_mask_minus_one = ((uint32)(button_mask) - (uint32)(1));
                if (game_pads[0].buttons[6].down)
                {
                    button_mask |= 8;
                    button_mask_minus_one = ((uint32)(button_mask) - (uint32)(1));
                }
                if ((((button_mask_minus_one >= 2) && (button_mask != 4)) && (button_mask != 8)))
                    goto HandleControllerActions;
                if ((game_pads[0].type == 65))
                {
                    if (controller_window->IsSelected(MenuText::weapon_select))
                        AssignMenuButton(button_mask, &game_settings.bindings.digital[3], &game_settings.bindings.digital[0], &game_settings.bindings.digital[1], &game_settings.bindings.digital[2]);
                    if (controller_window->IsSelected(MenuText::crouch))
                        AssignMenuButton(button_mask, &game_settings.bindings.digital[0], &game_settings.bindings.digital[3], &game_settings.bindings.digital[1], &game_settings.bindings.digital[2]);
                    if (controller_window->IsSelected(MenuText::jump))
                        AssignMenuButton(button_mask, &game_settings.bindings.digital[1], &game_settings.bindings.digital[3], &game_settings.bindings.digital[0], &game_settings.bindings.digital[2]);
                    if (controller_window->IsSelected(MenuText::smart_bomb))
                    {
                        sel_binding = &game_settings.bindings.digital[2];
                    AssignSmartBombBinding:
                        AssignMenuButton(button_mask, sel_binding, sel_binding + 1, sel_binding - 2, sel_binding - 1);
                    }
                }
                else
                {
                    if (controller_window->IsSelected(MenuText::weapon_select))
                        AssignMenuButton(button_mask, &game_settings.bindings.analog[3], &game_settings.bindings.analog[0], &game_settings.bindings.analog[1], &game_settings.bindings.analog[2]);
                    if (controller_window->IsSelected(MenuText::crouch))
                        AssignMenuButton(button_mask, &game_settings.bindings.analog[0], &game_settings.bindings.analog[3], &game_settings.bindings.analog[1], &game_settings.bindings.analog[2]);
                    if (controller_window->IsSelected(MenuText::jump))
                        AssignMenuButton(button_mask, &game_settings.bindings.analog[1], &game_settings.bindings.analog[3], &game_settings.bindings.analog[0], &game_settings.bindings.analog[2]);
                    if (controller_window->IsSelected(MenuText::smart_bomb))
                    {
                        sel_binding = &game_settings.bindings.analog[2];
                        goto AssignSmartBombBinding;
                    }
                }
            HandleControllerActions:
                if (confirm_pressed)
                {
                    if (controller_window->IsSelected(MenuText::return_to_options_menu))
                    {
                        action_sound = 19;
                        menu_state = MAIN_MENU_OPTIONS;
                    }
                    if (controller_window->IsSelected(MenuText::reset_to_defaults))
                    {
                        game_settings.vibration = VibrationFull;
                        RestoreDefaultControlBindings();
                    }
                    if (controller_window->IsSelected(MenuText::calibrate_analog_controller))
                    {
                        for (uint32 axis = 0; axis < 4; ++axis)
                        {
                            saved_range[axis][0] = pad_calibration[axis].min;
                            saved_range[axis][1] = pad_calibration[axis].max;
                            pad_calibration[axis].min = 255;
                            pad_calibration[axis].max = 0;
                        }
                        menu_state = MAIN_MENU_ANALOG_RANGE;
                    }
                    if (controller_window->IsSelected(MenuText::vibration_settings))
                    {
                        preset_window->arrow_item = game_settings.vibration;
                        menu_state = MAIN_MENU_VIBRATION;
                        preset_window->ResetSelection(((uint8)(game_settings.vibration)));
                    }
                }

                if (cancel_pressed)
                    menu_state = MAIN_MENU_OPTIONS;
                /* MIPS 80013CD4..80013CF0 passes four signed halfwords */
                game_pads[0].SetButtonMasks(game_settings.bindings.digital[0], game_settings.bindings.digital[1], game_settings.bindings.digital[2], game_settings.bindings.digital[3]);
                /* MIPS 80013D18..80013D2C passes zero then four signed halfwords */
                game_pads[0].SetAxes(3, 2, 1, 0, game_settings.bindings.analog[0], game_settings.bindings.analog[1], game_settings.bindings.analog[2], game_settings.bindings.analog[3]);
                frame_sound = action_sound;
                goto FinalizeMenuFrame;

            case MAIN_MENU_ANALOG_RANGE:
                if ((((uint8)(game_pads[0].raw_axis[1])) < (sint32)pad_calibration[1].min))
                    pad_calibration[1].min = ((uint8)(game_pads[0].raw_axis[1]));
                if (((sint32)pad_calibration[1].max < ((uint8)(game_pads[0].raw_axis[1]))))
                    pad_calibration[1].max = ((uint8)(game_pads[0].raw_axis[1]));
                if ((((uint8)(game_pads[0].raw_axis[0])) < (sint32)pad_calibration[0].min))
                    pad_calibration[0].min = ((uint8)(game_pads[0].raw_axis[0]));
                if (((sint32)pad_calibration[0].max < ((uint8)(game_pads[0].raw_axis[0]))))
                    pad_calibration[0].max = ((uint8)(game_pads[0].raw_axis[0]));
                if ((((uint8)(game_pads[0].raw_axis[3])) < (sint32)pad_calibration[3].min))
                    pad_calibration[3].min = ((uint8)(game_pads[0].raw_axis[3]));
                if (((sint32)pad_calibration[3].max < ((uint8)(game_pads[0].raw_axis[3]))))
                    pad_calibration[3].max = ((uint8)(game_pads[0].raw_axis[3]));
                if ((((uint8)(game_pads[0].raw_axis[2])) < (sint32)pad_calibration[2].min))
                    pad_calibration[2].min = ((uint8)(game_pads[0].raw_axis[2]));
                if (((sint32)pad_calibration[2].max < ((uint8)(game_pads[0].raw_axis[2]))))
                    pad_calibration[2].max = ((uint8)(game_pads[0].raw_axis[2]));
                if (confirm_pressed)
                    menu_state = MAIN_MENU_ANALOG_CENTER;
                frame_sound = action_sound;
                if (cancel_pressed)
                {
                    for (uint32 axis = 0; axis < 4; ++axis)
                    {
                        pad_calibration[axis].min = saved_range[axis][0];
                        pad_calibration[axis].max = saved_range[axis][1];
                    }
                    menu_state = MAIN_MENU_CONTROLLER;
                }
                goto FinalizeMenuFrame;

            case MAIN_MENU_ANALOG_CENTER:
                pad_calibration[1].centre = ((uint8)(game_pads[0].raw_axis[1]));
                pad_calibration[0].centre = ((uint8)(game_pads[0].raw_axis[0]));
                pad_calibration[3].centre = ((uint8)(game_pads[0].raw_axis[3]));
                pad_calibration[2].centre = ((uint8)(game_pads[0].raw_axis[2]));
                action_sound = 23;
                if (confirm_pressed)
                    menu_state = MAIN_MENU_CONTROLLER;
                else
                SkipMenuSound:
                    action_sound = -1;

                goto QueueActionSound;

            case MAIN_MENU_VIBRATION:
                preset_window->Update();
                if (confirm_pressed)
                {
                    if (preset_window->IsSelected(MenuText::none))
                        game_settings.vibration = VibrationNone;
                    if (preset_window->IsSelected(MenuText::some))
                        game_settings.vibration = VibrationSome;
                    if (preset_window->IsSelected(MenuText::full))
                        game_settings.vibration = VibrationFull;
                    if (preset_window->IsSelected(MenuText::return_to_controller_config))
                    {
                        menu_state = MAIN_MENU_CONTROLLER;
                        action_sound = 19;
                    }
                }
                preset_window->arrow_item = game_settings.vibration;
                frame_sound = action_sound;
                if (cancel_pressed)
                    menu_state = MAIN_MENU_CONTROLLER;
                goto FinalizeMenuFrame;

            case MAIN_MENU_RESET_CONFIRM:
                menu_runtime.confirm->Update();
                if (confirm_pressed)
                {
                    if (menu_runtime.confirm->IsSelected(MenuText::yes))
                    {
                        player_saves.selected = GetDefaultPlayerSave();
                        player_saves.current = GetDefaultPlayerSave();
                        RestoreGameSettings(GetDefaultPlayerSave().settings);
                        menu_state = MAIN_MENU_ROOT;
                        ResetSceneDisplayOffsets();
                        pad_calibration[1].max = 255;
                        pad_calibration[0].max = 255;
                        pad_calibration[3].max = 255;
                        pad_calibration[2].max = 255;
                        pad_calibration[1].min = 0;
                        pad_calibration[0].min = 0;
                        pad_calibration[3].min = 0;
                        pad_calibration[2].min = 0;
                        pad_calibration[1].centre = 128;
                        pad_calibration[0].centre = 128;
                        pad_calibration[3].centre = 128;
                        pad_calibration[2].centre = 128;
                        main_window->DisableItem(MenuText::continue_game);
                    }
                    else
                    {
                        action_sound = 19;
                        menu_state = MAIN_MENU_EXTRAS;
                    }
                }
                frame_sound = action_sound;
                if (cancel_pressed)
                    menu_state = MAIN_MENU_EXTRAS;
                goto FinalizeMenuFrame;

            case MAIN_MENU_CARD_DELAY:
                next_card_delay = ((uint32)(card_delay) - (uint32)(1));
                if (card_delay)
                {
                    --card_delay;
                    if (!next_card_delay)
                    {
                        card_timeout = 150;
                        menu_state = MAIN_MENU_CARD_QUERY;
                    }
                }
                else
                {
                    card_delay = 10;
                }
                goto QueueActionSound;

            case MAIN_MENU_CARD_QUERY:
                if ((card_save_state.status == -1))
                {
                    menu_state = MAIN_MENU_CARD_MISSING;
                    goto QueueActionSound;
                }
                if ((card_save_state.status < 0))
                {
                    if ((card_save_state.status == -2))
                    {
                        menu_state = MAIN_MENU_CARD_UNFORMATTED;
                    }
                    else
                    {
                    WaitForCardQuery:
                        if (card_timeout)
                            --card_timeout;
                        else
                        ShowNoSaves:
                            menu_state = MAIN_MENU_CARD_NO_SAVE;
                    }
                QueueActionSound:
                    frame_sound = action_sound;

                    goto FinalizeMenuFrame;
                }
                if ((card_save_state.status != 1))
                    goto WaitForCardQuery;
                if (QueryCardSaveFile())
                    goto ShowNoSaves;
                save_window = new MenuWindow(256, 55, 0, 0x100, 0xC0, 0x12);
                save_window->numbered = 1;
                menu_state = MAIN_MENU_LOAD_SLOTS;
                PopulateSaveSelectionMenu(save_window);
                frame_sound = action_sound;
            FinalizeMenuFrame:
                PlaySoundEffect(frame_sound, 0x2000, 0);

                background_camera->Advance();
                if ((game_clock.vblank_frame == frame_start))
                    ::WaitForGameFrames(1);
                scene_frame.service_enabled = 0;
                while (1)
                {
                    ::WaitForGameFrames(1);
                    if (!DrawSync(1))
                        break;
                    ::ServiceGameFrame();
                }

                SubmitSceneFrame();
                if (!scene_frame.service_enabled)
                {
                    ::ServiceGameFrame();
                    scene_frame.service_enabled = 1;
                }
                break;

            case MAIN_MENU_CARD_NO_SAVE:

            case MAIN_MENU_CARD_UNFORMATTED:
                if ((card_save_state.status == 2))
                    menu_state = MAIN_MENU_CARD_DELAY;
                goto HandleCardCancel;

            case MAIN_MENU_LOAD_SLOTS:
                close_save_window = false;
                if ((card_save_state.status == -1))
                {
                    menu_state = MAIN_MENU_CARD_MISSING;
                    close_save_window = true;
                }
                save_window->Update();
                if (confirm_pressed)
                {
                    invalid_slot = false;
                    if ((save_window->IsSelected(MenuText::read_error) || save_window->IsSelected(MenuText::empty_slot)))
                        invalid_slot = true;
                    action_sound = 28;
                    if (!invalid_slot)
                    {
                        action_sound = 23;
                        player_saves.selected = card_save_image.slots[save_window->selected];
                        RestoreGameSettings(player_saves.selected.settings);
                        close_save_window = true;
                        finished = true;
                    }
                }
                if (cancel_pressed)
                {
                    close_save_window = true;
                    menu_state = MAIN_MENU_ROOT;
                }
                if (close_save_window)
                {
                    if (save_window)
                        DestroyMenuObject(save_window);
                    save_window = 0;
                }
                goto QueueActionSound;

            case MAIN_MENU_CARD_MISSING:
                if (((card_save_state.status == -2) || ((((sint32)card_save_state.status >= -2) && ((sint32)card_save_state.status < 3)) && ((sint32)card_save_state.status > 0))))
                    menu_state = MAIN_MENU_CARD_DELAY;
            HandleCardCancel:
                frame_sound = action_sound;

                if (!any_pressed)
                    goto FinalizeMenuFrame;
                action_sound = 19;
                menu_state = MAIN_MENU_ROOT;
                goto QueueActionSound;

            default:
                goto QueueActionSound;
        }
    }

    ::ReleaseMenuBuffers();
    ::WaitForGameFrames(1);
    DrawSync(0);
    SubmitSceneFrame();
    ::ClearStartupImageArea();
    if (main_window)
        DestroyMenuObject(main_window);
    if (level_window)
        DestroyMenuObject(level_window);
    if (options_window)
        DestroyMenuObject(options_window);
    if (extras_window)
        DestroyMenuObject(extras_window);
    if (difficulty_window)
        DestroyMenuObject(difficulty_window);
    if (audio_window)
        DestroyMenuObject(audio_window);
    if (controller_window)
        DestroyMenuObject(controller_window);
    if (preset_window)
        DestroyMenuObject(preset_window);
    if (background_camera)
        DestroyMenuObject(background_camera);
    ::ReleaseLevelLoadResources();
    InitializeGamePads();
    SaveGameSettings(player_saves.selected.settings);
    return result_state;
}
