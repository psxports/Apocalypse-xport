#include <new>
#include <stdlib.h>
#include <string.h>

#include "hd.h"
#include "psx_gpu.h"
#include "credits_player.h"
#include "model_instance.h"
#include "texture_resources.h"
#include "pause_menu.h"
#include "player_save_state.h"
#include "game_pad_input.h"
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
#include "global.h"
#include "main_menu.h"
#include "vector3.h"
#include "game_heap.h"
#include "orbit_camera.h"
#include "model_bank.h"
#include "record_commands.h"
#include "audio_session.h"
#include "game_panel.h"
#include "menu_input.h"
#include "card_save.h"
#include "menu_window.h"
#include "menu_lifecycle.h"
#include "scene_lifecycle.h"
#include "hud_text.h"
#include "hud_sprites.h"
#include "effect_lifecycle.h"
#include "effect_object.h"

struct MenuScene
{
    const char *library;
    const char *bank;
    Vector3 pos;
    uint32 radius;
    uint32 height;
};

static const MenuScene menu_scenes[] = {{"roof_lib", "roof_1", {{{0x00001CCE, -0x49D, 0x000014D4}}}, 1744, 768}, {"sewr_lib", "sewr_4", {{{-0x2C10, 0x00002AB2, 0x000150FD}}}, 3536, 2304}};

static uint32 cur_menu_scene = 0;

void ResetMenuScenes()
{
    cur_menu_scene = 0;
}

static void AdvanceMenuScene()
{
    ++cur_menu_scene;
    if (cur_menu_scene >= sizeof(menu_scenes) / sizeof(menu_scenes[0]))
        cur_menu_scene = 0;
}

static const MenuScene &GetCurMenuScene()
{
    if (cur_menu_scene >= sizeof(menu_scenes) / sizeof(menu_scenes[0]))
    {
        GameDiagnostic(("Invalid menu scene index\n"));
        abort();
    }
    return menu_scenes[cur_menu_scene];
}

static void QueueSelLevelResources()
{
    FUNCTION_MARKER(0x80011B34, "SLUS_003.73");
    ModelBank::QueueLoad(GetCurMenuScene().library, false);
    ModelBank::QueueLoad(GetCurMenuScene().bank, true);
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

static uint64 menu_hd_clock_field;
static float menu_hd_clock_fraction;

static float MenuRenderPhase()
{
    uint64 field;
    float fraction = xport_frame_fract(&field);
    if (field < menu_hd_clock_field || field - menu_hd_clock_field > 2)
        return 1;
    // The original menu loop waits across two physical fields
    float phase = ((float)(field - menu_hd_clock_field) + fraction - menu_hd_clock_fraction) / 2;
    return phase < 0 ? 0 : (phase > 1 ? 1 : phase);
}

static void PresentTitleFrame(HD_FRAME &frame, bool prompt)
{
    if (!frame.selected)
    {
        PSX_RECT source = {0, 256, 512, 240};
        MoveImage(&source, 0, 0);
        scene_render.cur = &scene_render.buffers[1];
        BeginSceneFrame();
        UpdateScenePacketLimit();
    }
    if (prompt)
    {
        SetHudTextScale(256);
        SetHudTextAlignment(0);
        SetHudTextColor(128, 128, 128, 0);
        DrawStyledHudText(256, 120, MenuText::press_start, 0, 0, 256);
    }
    if (!frame.selected)
    {
        uint8 background = scene_render.buffers[0].draw.isbg;
        scene_render.buffers[0].draw.isbg = 0;
        SubmitSceneFrame();
        scene_render.buffers[0].draw.isbg = background;
        PutDispEnv(prompt ? &scene_render.buffers[1].disp : &scene_render.buffers[0].disp);
    }
    if (!hd_frame_image(&frame, 0, 256, 512, 240, false))
    {
        GameDiagnostic(("Cannot present title frame\n"));
        abort();
    }
}

uint32 RunMainMenu()
{
    sint32 prompt_visible_frames;
    sint32 prompt_hidden_frames;
    bool start_released;

    MenuWindow *main_window;

    MenuWindow *controller_window;

    MenuWindow *preset_window;
    MainMenuPage menu_state;
    uint32 heading_y;
    const char *heading;
    MenuWindow *submenu;
    bool preview_sel;
    const char *audio_action_label;
    bool binding_sel;
    const char *binding_label;
    const char *message;
    uint32 message_y;
    uint32 message_x;
    MenuWindow *confirm_window;
    sint32 action_sound;
    sint32 frame_sound;
    uint32 button_mask;
    sint16 *sel_binding;
    sint32 volume_step;
    sint32 voice_preview;

    bool close_save_window;
    bool invalid_slot;
    char saved_level_name[sizeof(player_saves.sel.progress.level)];
    uint32 item_pos[2];
    uint32 confirm_pressed;
    uint32 cancel_pressed;
    uint32 any_pressed;
    uint32 start_pressed;
    uint32 result_state;
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
    sint16 saved_scr_pos[2];
    sint32 rollback_frames;
    uint32 music_preview_idx;
    uint32 last_activity_frame;
    uint32 frame_start;
    HD_FRAME graphics_frame = {0};
    HD_FRAME title_frame = {0};
    bool title_prompt;
    bool hd_presented_early;

    struct CalibrationRange
    {
        sint32 min, max;
    } saved_range[4];

    FUNCTION_MARKER(0x80011BA0, "SLUS_003.73");
    ModelRenderView &view = GetModelRenderView();
    result_state = 0;
    finished = false;
    BeginSceneFrame();
    SubmitSceneFrame();
    LoadStartupImage(false, "title.bmr", 240);
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
    QueueSelLevelResources();
    prompt_hidden_frames = 0;
    GamePadInput::Poll();
    pad_polling.idle_ticks = 0;
    start_released = game_pads[0].buttons[14].down == 0;
    do
    {
        hd_frame_begin(&title_frame);
        title_prompt = false;
        GamePadInput::Poll();
        if (menu_runtime.prompt_delay)
        {
            --menu_runtime.prompt_delay;
        }
        else
        {
            if (!game_pads[0].buttons[14].down)
                start_released = true;
            if (start_released && game_pads[0].buttons[14].down)
            {
                PlaySoundEffect(23, 0x2000, 0);
                goto OpenMainMenu;
            }
            if (prompt_visible_frames)
            {
                title_prompt = true;
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
        ProcessResourceRead();
        ModelBank::ProcessLoadQueue();
        PresentTitleFrame(title_frame, title_prompt);
        WaitForGameFrames(1);
    } while ((sint32)pad_polling.idle_ticks < 1801);
    result_state = 3;
    finished = true;
OpenMainMenu:
    hd_frame_abandon(&title_frame);
    game_pads[0].buttons[14].pressed = 0;

    game_pads[0].buttons[3].pressed = 0;
    ClearStartupImageArea();
    ModelBank::WaitForLoads();
    CreateMenuBuffers(951706923, 64, 64, 127);
    ConfigureProjDepth(10, 10240, 0x2000);
    const MenuScene &scene = GetCurMenuScene();
    background_camera = new OrbitCamera(&scene.pos, scene.radius, scene.height);
    main_window = new MenuWindow(256, 0, 0, 0x140, 0x100, 0x1A);
    main_window->AddItem(MenuText::continue_game);
    if (!player_saves.cur.checksum || NamesEqualIgnoringCase(player_saves.sel.progress.level, GetLevelEntry(0)->name))
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
    extras_window->AddItem(MenuText::center_scr);
    extras_window->AddItem(MenuText::return_to_options_menu);
    difficulty_window = new MenuWindow(256, 55, 0, 0x140, 0x100, 0x1A);
    difficulty_window->AddItem(MenuText::easy);
    difficulty_window->AddItem(MenuText::normal);
    difficulty_window->AddItem(MenuText::hard);
    difficulty_window->AddItem(MenuText::return_to_options_menu);
    difficulty_window->has_arrows = true;
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
    preset_window->items[3].sel_scale = 192;
    preset_window->has_arrows = true;
    RestoreGameSettings(player_saves.sel.settings);
    pad_polling.idle_ticks = 0;
    last_activity_frame = game_clock.vblank_frame;
    while (!finished)
    {
        menu_hd_clock_fraction = xport_frame_fract(&menu_hd_clock_field);
        hd_presented_early = false;
        hd_frame_begin(&graphics_frame);
        BeginSceneFrame();
        UpdateScenePacketLimit();
        frame_start = game_clock.vblank_frame;
        if (rollback_frames)
        {
            if (!(--rollback_frames))
            {
                scene_render.buffers[0].disp.screen.x = saved_scr_pos[0];
                scene_render.buffers[1].disp.screen.x = saved_scr_pos[0];
                scene_render.buffers[0].disp.screen.y = saved_scr_pos[1];
                scene_render.buffers[1].disp.screen.y = saved_scr_pos[1];
            }
        }
        BlurMenuBuffers();
        BlurMenuBuffers();
        UpdateHudTextGlowCurve();
        view.angles.BuildRotationMatrix(view.rotation);
        view.SetTranslation();
        view.Configure(model_viewport, scene_render.cur->ot);
        if (graphics_frame.selected)
        {
            view.CaptureHd(graphics_frame.camera, model_viewport);
            hd_scene_capture_begin(&graphics_frame.camera);
        }
        ModelInstance::RenderVisibleList(ModelBank::GetSceneObjects());
        if (graphics_frame.selected)
        {
            hd_scene_capture_end();
            ModelBank::CaptureHdMaterials();
        }
        AdvanceModelRenderCounter();
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
                DrawMenuCenteredHint();
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

                DrawMenuHints();
                break;

            case MAIN_MENU_SCREEN_POSITION:
                MenuWindow::SetGreenText();
                DrawStyledHudText(256, 80, MenuText::use_the_directional, 0, 0, 256);
                DrawStyledHudText(256, 106, MenuText::buttons_to_center, 0, 0, 256);
                DrawStyledHudText(256, 132, MenuText::the_scr, 0, 0, 256);
                SetHudTextAlignment(1);
                MenuWindow::SetDarkRedText();
                DrawStyledHudText(104, 210, MenuText::cancel, 0, 0, 256);
                DrawStyledHudText(328, 210, MenuText::accept, 0, 0, 256);
                DrawHudSprite(76, 211, TextureGroup::GetFrame(EffectObject::GetButtonResourceGrp(), 1), true, 0x808080);
                DrawHudSprite(300, 211, TextureGroup::GetFrame(EffectObject::GetButtonResourceGrp(), 0), true, 0x808080);
                AllocateHudRect(0, 0, 512, 5, 0, 0, 255, 0);
                AllocateHudRect(0, 235, 512, 5, 0, 0, 255, 0);
                AllocateHudRect(0, 0, 7, 240, 0, 0, 255, 0);
                AllocateHudRect(505, 0, 7, 240, 0, 0, 255, 0);
                break;

            case MAIN_MENU_DIFFICULTY:
                DrawMenuHints();
                difficulty_window->Draw();
                break;

            case MAIN_MENU_AUDIO:
                DrawMenuLeftHint();
                SetHudTextAlignment(1);
                SetHudTextScale(192);
                preview_sel = audio_window->IsSel(MenuText::fx_level) || audio_window->IsSel(MenuText::voice_level);
                if (preview_sel)
                {
                    audio_action_label = MenuText::test;
                }
                else if (audio_window->IsSel(MenuText::music_level))
                {
                    audio_action_label = MenuText::cycle;
                }
                else
                {
                    audio_action_label = MenuText::select;
                }
                DrawStyledHudText(348, 210, audio_action_label, 0, 0, 256);
                SetHudTextAlignment(0);
                DrawHudSprite(320, 211, TextureGroup::GetFrame(EffectObject::GetButtonResourceGrp(), 0), true, 0x808080);
                audio_window->DrawVolumeSlider();
                audio_window->Draw();
                break;

            case MAIN_MENU_CONTROLLER:
                controller_window->Draw();
                binding_sel = controller_window->IsSel(MenuText::weapon_select) || controller_window->IsSel(MenuText::crouch) || controller_window->IsSel(MenuText::jump) || controller_window->IsSel(MenuText::smart_bomb);
                if (binding_sel)
                {
                    SetHudTextAlignment(1);
                    SetHudTextScale(256);
                    MenuWindow::SetDarkRedText();
                    DrawStyledHudText(211, 210, MenuText::go_back, 0, 0, 256);
                    DrawHudSprite(183, 211, TextureGroup::GetFrame(EffectObject::GetButtonResourceGrp(), 1), true, 0x808080);
                }
                else
                {
                    DrawMenuHints();
                }
                SetHudTextAlignment(1);
                MenuWindow::SetRedText();
                SetHudTextScale(192);
                if (game_pads[0].type == 65)
                {
                    controller_window->GetItemPos(MenuText::weapon_select, &item_pos[0], &item_pos[1]);
                    binding_label = MenuWindow::GetButtonLabel(game_settings.bindings.digital[3]);
                    DrawStyledHudText(item_pos[0] + 315, item_pos[1], binding_label, 0, 0, 256);
                    controller_window->GetItemPos(MenuText::crouch, &item_pos[0], &item_pos[1]);
                    binding_label = MenuWindow::GetButtonLabel(game_settings.bindings.digital[0]);
                    DrawStyledHudText(item_pos[0] + 315, item_pos[1], binding_label, 0, 0, 256);
                    controller_window->GetItemPos(MenuText::jump, &item_pos[0], &item_pos[1]);
                    binding_label = MenuWindow::GetButtonLabel(game_settings.bindings.digital[1]);
                    DrawStyledHudText(item_pos[0] + 315, item_pos[1], binding_label, 0, 0, 256);
                    controller_window->GetItemPos(MenuText::smart_bomb, &item_pos[0], &item_pos[1]);
                    message = MenuWindow::GetButtonLabel(game_settings.bindings.digital[2]);
                }
                else
                {
                    controller_window->GetItemPos(MenuText::weapon_select, &item_pos[0], &item_pos[1]);
                    binding_label = MenuWindow::GetButtonLabel(game_settings.bindings.analog[3]);
                    DrawStyledHudText(item_pos[0] + 315, item_pos[1], binding_label, 0, 0, 256);
                    controller_window->GetItemPos(MenuText::crouch, &item_pos[0], &item_pos[1]);
                    binding_label = MenuWindow::GetButtonLabel(game_settings.bindings.analog[0]);
                    DrawStyledHudText(item_pos[0] + 315, item_pos[1], binding_label, 0, 0, 256);
                    controller_window->GetItemPos(MenuText::jump, &item_pos[0], &item_pos[1]);
                    binding_label = MenuWindow::GetButtonLabel(game_settings.bindings.analog[1]);
                    DrawStyledHudText(item_pos[0] + 315, item_pos[1], binding_label, 0, 0, 256);
                    controller_window->GetItemPos(MenuText::smart_bomb, &item_pos[0], &item_pos[1]);
                    message = MenuWindow::GetButtonLabel(game_settings.bindings.analog[2]);
                }
                message_y = item_pos[1];
                message_x = item_pos[0] + 315;
                goto DrawMenuMessage;

            case MAIN_MENU_ANALOG_RANGE:
                MenuWindow::SetGreenText();
                DrawStyledHudText(256, 80, MenuText::move_the_left_stick, 0, 0, 256);
                DrawStyledHudText(256, 106, MenuText::and_right_stick_to_all, 0, 0, 256);
                DrawStyledHudText(256, 132, MenuText::four_corners_then_press, 0, 0, 256);
                message_x = 256;
                message = MenuText::the_x_button;
                message_y = 158;
                goto DrawMenuMessage;

            case MAIN_MENU_ANALOG_CENTER:
                MenuWindow::SetGreenText();
                DrawStyledHudText(256, 90, MenuText::center_both_the_left_stick, 0, 0, 256);
                DrawStyledHudText(256, 116, MenuText::and_the_right_stick_then, 0, 0, 256);
                message_x = 256;
                message = MenuText::press_the_x_button;
                message_y = 142;
                goto DrawMenuMessage;

            case MAIN_MENU_VIBRATION:
                MenuWindow::SetDarkRedText();
                DrawStyledHudText(256, 34, MenuText::vibration_settings, 0, 0, 256);
                DrawMenuHints();
                preset_window->Draw();
                break;

            case MAIN_MENU_RESET_CONFIRM:
                DrawMenuHints();
                MenuWindow::SetDarkRedText();
                DrawStyledHudText(256, 80, MenuText::really_reset_to_defaults, 0, 0, 256);
                confirm_window = menu_runtime.confirm;
                confirm_window->scr_pos.y = 131;
                confirm_window->Draw();
                break;

            case MAIN_MENU_CARD_DELAY:

            case MAIN_MENU_CARD_QUERY:
                MenuWindow::SetGreenText();
                DrawStyledHudText(256, 90, MenuText::checking_memory_card, 0, 0, 256);
                MenuWindow::SetRedText();
                DrawStyledHudText(256, 140, MenuText::do_not_remove, 0, 0, 256);
                message_x = 256;
                message = MenuText::memory_card;
                message_y = 166;
                goto DrawMenuMessage;

            case MAIN_MENU_CARD_NO_SAVE:
                MenuWindow::SetRedText();
                DrawStyledHudText(256, 90, MenuText::memory_card_contains, 0, 0, 256);
                DrawStyledHudText(256, 116, MenuText::no_apocalypse_game_save, 0, 0, 256);
                DrawStyledHudText(256, 142, MenuText::press_any_button, 0, 0, 256);
                message_x = 256;
                message_y = 168;
                message = MenuText::to_cancel;
                goto DrawMenuMessage;

            case MAIN_MENU_CARD_UNFORMATTED:
                MenuWindow::SetRedText();
                DrawStyledHudText(256, 90, MenuText::the_memory_card_in, 0, 0, 256);
                DrawStyledHudText(256, 116, MenuText::memory_card_slot_1_is, 0, 0, 256);
                DrawStyledHudText(256, 142, MenuText::unformatted_press, 0, 0, 256);
                message_x = 256;
                message = MenuText::any_button_to_cancel;
                message_y = 168;
                goto DrawMenuMessage;

            case MAIN_MENU_LOAD_SLOTS:
                DrawMenuHints();
                MenuWindow::SetRedText();
                DrawStyledHudText(256, 25, MenuText::load_game, 0, 0, 256);
                save_window->Draw();
                break;

            case MAIN_MENU_CARD_MISSING:
                MenuWindow::SetRedText();
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
        GamePadInput::Poll();
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
                    if (main_window->IsSel(MenuText::continue_game))
                    {
                        memcpy(saved_level_name, player_saves.sel.progress.level, sizeof(saved_level_name));
                        player_saves.sel.progress = GetDefaultPlayerSave().progress;
                        player_saves.sel.progress.SetLevel(saved_level_name);
                        finished = true;
                        player_saves.sel.progress.checkpoint[0] = 0;
                        game_settings.difficulty = player_saves.sel.settings.difficulty;
                    }
                    if (main_window->IsSel(MenuText::select_level))
                        menu_state = MAIN_MENU_LEVEL_SELECT;
                    if (main_window->IsSel(MenuText::new_game))
                    {
                        action_sound = 23;
                        player_saves.sel.progress = GetDefaultPlayerSave().progress;
                        finished = true;
                    }
                    if (main_window->IsSel(MenuText::autotest))
                    {
                        result_state = 2;
                        finished = true;
                    }
                    if (main_window->IsSel(MenuText::load_game))
                        menu_state = MAIN_MENU_CARD_DELAY;
                    if (main_window->IsSel(MenuText::options))
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
                if (game_clock.vblank_frame - last_activity_frame >= 0x709)
                {
                    result_state = 3;
                    finished = true;
                }
                goto FinalizeMenuFrame;

            case MAIN_MENU_LEVEL_SELECT:
                level_window->Update();
                if (!confirm_pressed)
                    goto HandleLevelCancel;
                level_label = level_window->GetSelLabel();
                player_saves.sel.progress.level[0] = 0;
                for (level_idx = 0; level_idx < GetLevelCount(); ++level_idx)
                {
                    level_entry = GetLevelEntry(level_idx);
                    if (NamesEqualIgnoringCase(level_label, level_entry->title))
                    {
                        player_saves.sel.progress.SetLevel(level_entry->name);
                        action_sound = 23;
                        goto FinishLevelSelection;
                    }
                }
                action_sound = 23;

            FinishLevelSelection:
                player_saves.sel.progress.checkpoint[0] = 0;

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
                    if (options_window->IsSel(MenuText::difficulty))
                    {
                        difficulty_window->arrow_item = game_settings.difficulty;
                        menu_state = MAIN_MENU_DIFFICULTY;
                        difficulty_window->ResetSelection(game_settings.difficulty);
                    }
                    if (options_window->IsSel(MenuText::sound))
                    {
                        action_sound = -1;
                        PlaySoundEffect(23, 0x2000, 0);
                        menu_state = MAIN_MENU_AUDIO;
                        music_preview_idx = 0;
                        SwitchMenuAudioSequence(GetLevelEntry(0)->music);
                    }
                    if (options_window->IsSel(MenuText::controller_config))
                        menu_state = MAIN_MENU_CONTROLLER;
                    if (options_window->IsSel(MenuText::misc_options))
                        menu_state = MAIN_MENU_EXTRAS;
                    if (options_window->IsSel(MenuText::return_to_main_menu))
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
                    if (extras_window->IsSel(MenuText::view_credits))
                    {
                        PlaySoundEffect(action_sound, 0x2000, 0);
                        action_sound = -1;
                        hd_frame_abandon(&graphics_frame);
                        CreditsPlayer::Run();
                    }
                    if (extras_window->IsSel(MenuText::music_credits))
                    {
                        PlaySoundEffect(action_sound, 0x2000, 0);
                        action_sound = -1;
                        hd_frame_abandon(&graphics_frame);
                        ReleaseLevelLoadResources();
                        PlayMovie(33);
                        QueueSelLevelResources();
                        ModelBank::WaitForLoads();
                    }
                    if (extras_window->IsSel(MenuText::view_intro))
                    {
                        hd_frame_abandon(&graphics_frame);
                        ReleaseLevelLoadResources();
                        PlayMovie(28);
                        QueueSelLevelResources();
                        ModelBank::WaitForLoads();
                    }
                    if (extras_window->IsSel(MenuText::reset_to_defaults))
                    {
                        menu_state = MAIN_MENU_RESET_CONFIRM;
                        menu_runtime.confirm->ResetSelection();
                    }
                    if (extras_window->IsSel(MenuText::center_scr))
                    {
                        menu_state = MAIN_MENU_SCREEN_POSITION;
                        saved_scr_pos[0] = scene_render.buffers[0].disp.screen.x;
                        saved_scr_pos[1] = scene_render.buffers[0].disp.screen.y;
                    }
                    if (extras_window->IsSel(MenuText::return_to_options_menu))
                    {
                        action_sound = 19;
                        menu_state = MAIN_MENU_OPTIONS;
                    }
                }
                goto HandleOptionsCancel;

            case MAIN_MENU_SCREEN_POSITION:
                if (game_pads[0].buttons[9].down && scene_render.buffers[0].disp.screen.x < 32)
                {
                    ++scene_render.buffers[0].disp.screen.x;
                    ++scene_render.buffers[1].disp.screen.x;
                }
                if (game_pads[0].buttons[8].down && scene_render.buffers[0].disp.screen.x > 0)
                {
                    --scene_render.buffers[0].disp.screen.x;
                    --scene_render.buffers[1].disp.screen.x;
                }
                if (game_pads[0].buttons[10].down && scene_render.buffers[0].disp.screen.y > 0)
                {
                    --scene_render.buffers[0].disp.screen.y;
                    --scene_render.buffers[1].disp.screen.y;
                }
                if (game_pads[0].buttons[11].down && scene_render.buffers[0].disp.screen.y < 32)
                {
                    ++scene_render.buffers[0].disp.screen.y;
                    ++scene_render.buffers[1].disp.screen.y;
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
                    if (difficulty_window->IsSel(MenuText::easy))
                    {
                        action_sound = 4;
                        game_settings.difficulty = 0;
                    }
                    if (difficulty_window->IsSel(MenuText::normal))
                    {
                        action_sound = 6;
                        game_settings.difficulty = 1;
                    }
                    if (difficulty_window->IsSel(MenuText::hard))
                    {
                        action_sound = 8;
                        game_settings.difficulty = 2;
                    }
                    if (difficulty_window->IsSel(MenuText::return_to_options_menu))
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
                volume_step = MenuWindow::ReadNavigationStep();
                if (audio_window->IsSel(MenuText::fx_level) && (volume_step || confirm_pressed))
                {
                    PlaySoundEffect(16, 0x2000, 0);
                    game_settings.effects_volume += volume_step * 1024;
                    if (game_settings.effects_volume < 0)
                        game_settings.effects_volume = 0;
                    if (game_settings.effects_volume >= 0x4000)
                        game_settings.effects_volume = 0x3FFF;
                }
                if (audio_window->IsSel(MenuText::music_level))
                {
                    game_settings.music_volume += volume_step * 8;
                    if (game_settings.music_volume < 0)
                        game_settings.music_volume = 0;
                    if (game_settings.music_volume >= 128)
                        game_settings.music_volume = 127;
                    RestoreMusicPlayback();
                    if (confirm_pressed)
                    {
                        if (++music_preview_idx >= GetLevelCount())
                            music_preview_idx = 0;
                        SwitchMenuAudioSequence(GetLevelEntry(music_preview_idx)->music);
                        RestoreMusicPlayback();
                    }
                }
                else
                {
                    StopMusicPlayback();
                }
                if (audio_window->IsSel(MenuText::voice_level))
                {
                    if (volume_step || confirm_pressed || !menu_runtime.voice_previewed)
                    {
                        voice_preview = RandomRange(2);
                        SelectCdVoice(0, voice_preview);
                        game_settings.voice_volume += volume_step * 16;
                        if (game_settings.voice_volume < 0)
                            game_settings.voice_volume = 0;
                        if (game_settings.voice_volume >= 256)
                            game_settings.voice_volume = 255;
                        menu_runtime.voice_previewed = true;
                    }
                }
                else
                {
                    menu_runtime.voice_previewed = false;
                }
                if ((!audio_window->IsSel(MenuText::return_to_options_menu) || !confirm_pressed) && !cancel_pressed)
                    goto SkipMenuSound;
                menu_state = MAIN_MENU_OPTIONS;
                ReleaseAudioSession();
                menu_runtime.voice_previewed = false;
                frame_sound = 19;
                goto FinalizeMenuFrame;

            case MAIN_MENU_CONTROLLER:
                if (game_pads[0].type == 65 || PadGetState(0) != 2)
                    controller_window->DisableItem(MenuText::calibrate_analog_controller);
                else
                    controller_window->EnableItem(MenuText::calibrate_analog_controller);
                if (PadGetState(0) == 6)
                    controller_window->EnableItem(MenuText::vibration_settings);
                else
                    controller_window->DisableItem(MenuText::vibration_settings);
                controller_window->Update();
                if (confirm_pressed && (controller_window->IsSel(MenuText::weapon_select) || controller_window->IsSel(MenuText::crouch) || controller_window->IsSel(MenuText::jump) || controller_window->IsSel(MenuText::smart_bomb)))
                    action_sound = -1;
                button_mask = game_pads[0].buttons[5].down != 0;
                if (game_pads[0].buttons[7].down)
                    button_mask |= 2;
                if (game_pads[0].buttons[4].down)
                    button_mask |= 4;
                if (game_pads[0].buttons[6].down)
                    button_mask |= 8;
                if (button_mask != 1 && button_mask != 2 && button_mask != 4 && button_mask != 8)
                    goto HandleControllerActions;
                if (game_pads[0].type == 65)
                {
                    if (controller_window->IsSel(MenuText::weapon_select))
                        MenuWindow::AssignButton(button_mask, &game_settings.bindings.digital[3], &game_settings.bindings.digital[0], &game_settings.bindings.digital[1], &game_settings.bindings.digital[2]);
                    if (controller_window->IsSel(MenuText::crouch))
                        MenuWindow::AssignButton(button_mask, &game_settings.bindings.digital[0], &game_settings.bindings.digital[3], &game_settings.bindings.digital[1], &game_settings.bindings.digital[2]);
                    if (controller_window->IsSel(MenuText::jump))
                        MenuWindow::AssignButton(button_mask, &game_settings.bindings.digital[1], &game_settings.bindings.digital[3], &game_settings.bindings.digital[0], &game_settings.bindings.digital[2]);
                    if (controller_window->IsSel(MenuText::smart_bomb))
                    {
                        sel_binding = &game_settings.bindings.digital[2];
                    AssignSmartBombBinding:
                        MenuWindow::AssignButton(button_mask, sel_binding, sel_binding + 1, sel_binding - 2, sel_binding - 1);
                    }
                }
                else
                {
                    if (controller_window->IsSel(MenuText::weapon_select))
                        MenuWindow::AssignButton(button_mask, &game_settings.bindings.analog[3], &game_settings.bindings.analog[0], &game_settings.bindings.analog[1], &game_settings.bindings.analog[2]);
                    if (controller_window->IsSel(MenuText::crouch))
                        MenuWindow::AssignButton(button_mask, &game_settings.bindings.analog[0], &game_settings.bindings.analog[3], &game_settings.bindings.analog[1], &game_settings.bindings.analog[2]);
                    if (controller_window->IsSel(MenuText::jump))
                        MenuWindow::AssignButton(button_mask, &game_settings.bindings.analog[1], &game_settings.bindings.analog[3], &game_settings.bindings.analog[0], &game_settings.bindings.analog[2]);
                    if (controller_window->IsSel(MenuText::smart_bomb))
                    {
                        sel_binding = &game_settings.bindings.analog[2];
                        goto AssignSmartBombBinding;
                    }
                }
            HandleControllerActions:
                if (confirm_pressed)
                {
                    if (controller_window->IsSel(MenuText::return_to_options_menu))
                    {
                        action_sound = 19;
                        menu_state = MAIN_MENU_OPTIONS;
                    }
                    if (controller_window->IsSel(MenuText::reset_to_defaults))
                    {
                        game_settings.vibration = VibrationFull;
                        RestoreDefaultControlBindings();
                    }
                    if (controller_window->IsSel(MenuText::calibrate_analog_controller))
                    {
                        for (uint32 axis = 0; axis < 4; ++axis)
                        {
                            saved_range[axis].min = pad_calibration[axis].min;
                            saved_range[axis].max = pad_calibration[axis].max;
                            pad_calibration[axis].min = 255;
                            pad_calibration[axis].max = 0;
                        }
                        menu_state = MAIN_MENU_ANALOG_RANGE;
                    }
                    if (controller_window->IsSel(MenuText::vibration_settings))
                    {
                        preset_window->arrow_item = game_settings.vibration;
                        menu_state = MAIN_MENU_VIBRATION;
                        preset_window->ResetSelection(game_settings.vibration);
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
                if (game_pads[0].raw_axis[1] < pad_calibration[1].min)
                    pad_calibration[1].min = game_pads[0].raw_axis[1];
                if (pad_calibration[1].max < game_pads[0].raw_axis[1])
                    pad_calibration[1].max = game_pads[0].raw_axis[1];
                if (game_pads[0].raw_axis[0] < pad_calibration[0].min)
                    pad_calibration[0].min = game_pads[0].raw_axis[0];
                if (pad_calibration[0].max < game_pads[0].raw_axis[0])
                    pad_calibration[0].max = game_pads[0].raw_axis[0];
                if (game_pads[0].raw_axis[3] < pad_calibration[3].min)
                    pad_calibration[3].min = game_pads[0].raw_axis[3];
                if (pad_calibration[3].max < game_pads[0].raw_axis[3])
                    pad_calibration[3].max = game_pads[0].raw_axis[3];
                if (game_pads[0].raw_axis[2] < pad_calibration[2].min)
                    pad_calibration[2].min = game_pads[0].raw_axis[2];
                if (pad_calibration[2].max < game_pads[0].raw_axis[2])
                    pad_calibration[2].max = game_pads[0].raw_axis[2];
                if (confirm_pressed)
                    menu_state = MAIN_MENU_ANALOG_CENTER;
                frame_sound = action_sound;
                if (cancel_pressed)
                {
                    for (uint32 axis = 0; axis < 4; ++axis)
                    {
                        pad_calibration[axis].min = saved_range[axis].min;
                        pad_calibration[axis].max = saved_range[axis].max;
                    }
                    menu_state = MAIN_MENU_CONTROLLER;
                }
                goto FinalizeMenuFrame;

            case MAIN_MENU_ANALOG_CENTER:
                pad_calibration[1].centre = game_pads[0].raw_axis[1];
                pad_calibration[0].centre = game_pads[0].raw_axis[0];
                pad_calibration[3].centre = game_pads[0].raw_axis[3];
                pad_calibration[2].centre = game_pads[0].raw_axis[2];
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
                    if (preset_window->IsSel(MenuText::none))
                        game_settings.vibration = VibrationNone;
                    if (preset_window->IsSel(MenuText::some))
                        game_settings.vibration = VibrationSome;
                    if (preset_window->IsSel(MenuText::full))
                        game_settings.vibration = VibrationFull;
                    if (preset_window->IsSel(MenuText::return_to_controller_config))
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
                    if (menu_runtime.confirm->IsSel(MenuText::yes))
                    {
                        player_saves.sel = GetDefaultPlayerSave();
                        player_saves.cur = GetDefaultPlayerSave();
                        RestoreGameSettings(GetDefaultPlayerSave().settings);
                        menu_state = MAIN_MENU_ROOT;
                        ResetSceneDispOffsets();
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
                if (card_delay)
                {
                    --card_delay;
                    if (!card_delay)
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
                if (card_save_state.status == -1)
                {
                    menu_state = MAIN_MENU_CARD_MISSING;
                    goto QueueActionSound;
                }
                if (card_save_state.status < 0)
                {
                    if (card_save_state.status == -2)
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
                if (card_save_state.status != 1)
                    goto WaitForCardQuery;
                if (QueryCardSaveFile())
                    goto ShowNoSaves;
                save_window = new MenuWindow(256, 55, 0, 0x100, 0xC0, 0x12);
                save_window->numbered = true;
                menu_state = MAIN_MENU_LOAD_SLOTS;
                save_window->PopulateSaveSelection();
                frame_sound = action_sound;
            FinalizeMenuFrame:
                PlaySoundEffect(frame_sound, 0x2000, 0);

                background_camera->Advance();
                if (graphics_frame.selected && graphics_frame.open)
                {
                    if (!hd_frame_present_phase(&graphics_frame, MenuRenderPhase()))
                    {
                        GameDiagnostic(("Cannot draw main-menu frame\n"));
                        abort();
                    }
                    hd_presented_early = true;
                    VSync(-1);
                }
                if (game_clock.vblank_frame == frame_start)
                    WaitForRenderFrames(1, hd_presented_early ? &graphics_frame : 0, MenuRenderPhase);
                scene_frame.service_enabled = false;
                while (1)
                {
                    WaitForRenderFrames(1, hd_presented_early ? &graphics_frame : 0, MenuRenderPhase);
                    if (!DrawSync(1))
                        break;
                    ServiceGameFrame();
                }

                if (graphics_frame.open || hd_presented_early)
                {
                    SubmitSceneFrame();
                    if (!hd_frame_present(&graphics_frame))
                    {
                        GameDiagnostic(("Cannot draw main-menu frame\n"));
                        abort();
                    }
                }
                if (!scene_frame.service_enabled)
                {
                    ServiceGameFrame();
                    scene_frame.service_enabled = true;
                }
                break;

            case MAIN_MENU_CARD_NO_SAVE:

            case MAIN_MENU_CARD_UNFORMATTED:
                if (card_save_state.status == 2)
                    menu_state = MAIN_MENU_CARD_DELAY;
                goto HandleCardCancel;

            case MAIN_MENU_LOAD_SLOTS:
                close_save_window = false;
                if (card_save_state.status == -1)
                {
                    menu_state = MAIN_MENU_CARD_MISSING;
                    close_save_window = true;
                }
                save_window->Update();
                if (confirm_pressed)
                {
                    invalid_slot = false;
                    if (save_window->IsSel(MenuText::read_error) || save_window->IsSel(MenuText::empty_slot))
                        invalid_slot = true;
                    action_sound = 28;
                    if (!invalid_slot)
                    {
                        action_sound = 23;
                        player_saves.sel = card_save_image.slots[save_window->sel];
                        RestoreGameSettings(player_saves.sel.settings);
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
                if (card_save_state.status == -2 || (card_save_state.status > 0 && card_save_state.status < 3))
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

    hd_frame_abandon(&graphics_frame);
    ReleaseMenuBuffers();
    WaitForGameFrames(1);
    DrawSync(0);
    if (!graphics_frame.selected)
        SubmitSceneFrame();
    ClearStartupImageArea();
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
    ReleaseLevelLoadResources();
    GamePadInput::Initialize();
    SaveGameSettings(player_saves.sel.settings);
    return result_state;
}
