#include <stdlib.h>
#include <new>
#include <string.h>

#include "credits_player.h"
#include "pause_menu.h"
#include "game_settings.h"
#include "game_tick.h"
#include "game_tuning.h"
#include "input_recording.h"
#include "game_pad_input.h"
#include "record_data.h"
#include "visual_object.h"
#include "cheat_input.h"
#include "level_catalogue.h"
#include "global.h"
#include "game_diagnostics.h"
#include "debug_finish_repro.h"
#include "object_initializers.h"
#include "hud_text.h"
#include "level_session.h"
#include "main_menu.h"
#include "loading_screen.h"
#include "startup_resources.h"
#include "scene_lifecycle.h"
#include "movie_lifecycle.h"
#include "record_factory.h"
#include "game_object_allocation.h"
#include "game_heap.h"
#include "model_bank.h"
#include "record_commands.h"
#include "resource_script.h"
#include "player_save_state.h"
#include "audio_session.h"
#include "resource_io.h"
#include "game_panel.h"
#include "player.h"
#include "camera.h"

struct LevelTestCaption
{
    LevelTestCaption() : text(0)
    {
    }

    ~LevelTestCaption()
    {
        free(text);
    }

    void Set(const char *name)
    {
        char *copy = 0;
        if (name)
        {
            size_t length = strlen(name);
            copy = (char *)malloc(length + 1);
            if (!copy)
            {
                GameDiagnostic(("Cannot allocate level test caption\n"));
                abort();
            }
            memcpy(copy, name, length + 1);
        }
        free(text);
        text = copy;
    }

    char *text;
};

static LevelTestCaption test_caption;

GameSessionState game_session = {VibrationFull, 0, 0, 0, false, true, 0, 0};

void ResetGameSession()
{
    GameSessionState initial = {VibrationFull, 0, 0, 0, false, true, 0, 0};
    game_session = initial;
}

GameProgress game_progress = {{0, 0}, {0, 0, 0, 0, 0}, 0};

void ResetGameProgress()
{
    game_progress = GameProgress();
}

LevelEnvironment level_environment = {0, 0, 0};

void ResetLevelEnvironment()
{
    LevelEnvironment initial = {0, 0, 0};
    level_environment = initial;
}

const char *GetLevelTestCaption()
{
    return test_caption.text;
}

static const char *const completion_captions[13] = {
    "Prison level completed", "Sewers level completed", "City level completed", "Death level completed", "Rooftops level completed", "Graveyard level completed", "Plague level completed", "Unknown", "War factory level completed", "War level completed", "Whitehouse level completed", "Unknown", "Unknown",
};

uint32 CompleteLevel(bool completed)
{
    Player *owner = visual_lists.players ? static_cast<Player *>(visual_lists.players->owner) : 0;
    uint32 val, state;
    const LevelEntry *entry, *next_level;
    uint32 bonus = 0, count, denominator, numerator;
    const char *name;
    FUNCTION_MARKER(0x800174A8, "SLUS_003.73");
    scene_frame.paused = true;
    if (owner)
        owner->StopWeapon();
    AnimatedHudText::ClearAll();
    GamePadInput::ClearActuator(0, 0);
    GamePadInput::ClearActuator(0, 1);
    panel_text.lives_timer = 0;
    CaptureCurPlayerSave();
    entry = FindLevelEntry(GetCurLevelName());
    if (!entry)
    {
        GameDiagnostic(("Unknown completed level\n"));
        abort();
    }
    next_level = NextLevelEntry(entry);
    if (!next_level)
    {
        player_saves.cur.checksum = 0;
        game_session.exit_reason = 3;
        return 3;
    }
    player_saves.cur.progress.SetLevel(next_level->name);
    val = GetDefaultPlayerSave().progress.health;
    player_saves.cur.progress.checkpoint[0] = 0;
    game_progress.completion.completed = completed;
    player_saves.cur.progress.health = val;
    if (completed)
    {
        state = level_environment.area_kind;
        static const uint8 completion_bonus[11] = {5, 22, 26, 0, 11, 23, 0, 0, 3, 0, 18};
        if (state < 11)
            bonus = completion_bonus[state];
        count = game_progress.level.enemy_kills;
        val = 0;
        if (count)
        {
            denominator = game_progress.level.enemy_total;
            if (denominator)
            {
                numerator = 100 * (count + bonus);
                val = DivideSignedWord(numerator, denominator);
            }
        }
        game_progress.completion.target_percent = val;
        game_progress.completion.disp_percent = 0;
        game_progress.completion.blink_ticks = 43;
    }
    state = level_environment.area_kind;
    name = state < 13 ? completion_captions[state] : "Unknown";
    panel_text.caption = name;
    val = game_clock.vblank_frame;
    panel_text.visible = 0;
    menu_runtime.screen = MENU_SCREEN_RESULTS_CAPTION;
    panel_text.reveal_frame = val;
    return 5;
}

void ReleaseLevelLoadResources()
{
    FUNCTION_MARKER(0x80011B04, "SLUS_003.73");
    ModelBank::UnloadUnprotected();
    RebuildModelVramAllocations();
    ReleaseScriptWorkspace();
}

void RunGameSession()
{
    bool refresh = false;
    sint32 state;
    VibrationMode vibration;
    uint32 test_level;
    const LevelEntry *entry;
    sint32 idx;
    GameHeapBlock *object;
    Camera *created_camera;
    Player *created_player;
    GameHeapBlock *resource;
    Vector3 pos;
    uint32 count;
    FUNCTION_MARKER(0x80030D74, "SLUS_003.73");
    InitializeGameConstructors();
    InitializeGameRuntime();
    if (game_pads[0].buttons[14].down == 0)
        PlayMovie(0x1C);
restart:
    ResetScene(false);
    if (refresh)
    {
        refresh = false;
        CreditsPlayer::Run();
    }
    state = RunMainMenu();
    if (state == 1)
        goto restart;
    if (state < 2)
    {
        if (state == 0)
        {
            if (NamesEqualIgnoringCase(player_saves.sel.progress.level, GetLevelEntry(0)->name))
                PlayMovie(2);
            goto load_default;
        }
        goto frame;
    }
    if (state == 2)
    {
        player_saves.sel.progress = GetDefaultPlayerSave().progress;
        test_level = 0;
        for (;;)
        {
            if (test_level >= GetLevelCount())
                test_level = 0;
            ResetScene(false);
            if (test_level == 0)
            {
                GameDiagnostic(("\nHeap 0 space used = %u", GetGameHeapUsed(0)));
                GameDiagnostic(("\nHeap 1 space used = %u", GetGameHeapUsed(1)));
            }
            idx = 0;
            player_saves.sel.progress.SetLevel(GetLevelEntry(test_level)->name);
            player_saves.sel.progress.checkpoint[0] = 0;
            player_saves.sel.LoadLevel();
            while (idx < (sint32)GetLevelRecCount())
            {
                if (GetLevelRec(idx).Read16() == 8)
                {
                    RecordCursor checkpoint_name = ReadRecPos(&pos, idx) + 6;
                    ResetScene(true);
                    InitializeLevelRecs();
                    created_camera = 0;
                    object = AllocateZeroedObject(0x244);
                    if (object != 0)
                    {
                        created_player = 0;
                        resource = AllocateZeroedObject(0x290);
                        if (resource != 0)
                        {
                            created_player = new (resource) Player();
                        }
                        if (!created_player)
                        {
                            GameDiagnostic(("Cannot allocate session player\n"));
                            abort();
                        }
                        created_camera = new (object) Camera(*created_player);
                    }
                    if (!created_camera)
                    {
                        GameDiagnostic(("Cannot allocate session camera\n"));
                        abort();
                    }
                    created_camera->mode = 3;
                    SelectStartRec(checkpoint_name.CStr());
                    test_caption.Set(checkpoint_name.CStr());
                    checkpoint_name = RecordCursor();
                    ApplyStartRec();
                    input_recording.UseTestReplay();
                    RunLevelLoop();
                    state = game_session.exit_reason;
                    input_recording.ReleaseData();
                    if (state == 5)
                    {
                        input_recording.Stop();
                        test_caption.Set(0);
                        goto restart;
                    }
                }
                ++idx;
            }
            ++test_level;
        }
    }
    if (state != 3)
        goto frame;
    if (!game_session.demo_next)
    {
        game_session.movie_any_button_skip = true;
        PlayMovie(0x1C);
        game_session.demo_next = !game_session.demo_next;
        goto restart;
    }
    vibration = game_settings.vibration;
    game_settings.vibration = VibrationNone;
    game_session.saved_vibration = vibration;
    player_saves.replay_backup = player_saves.cur;
    idx = game_session.demo_idx;
    GetDemoPlayerSave(idx).LoadLevel();
    idx = game_session.demo_idx;
    input_recording.LoadReplay(GetDemoRecordingName(idx));
    game_session.demo_next = !game_session.demo_next;
frame:
    RunLevelLoop();
    if (input_recording.HasBuffer())
    {
        input_recording.ReleaseData();
        game_session.exit_reason = 4;
    }
    if (input_recording.GetMode() == InputRecording::Replay)
    {
        vibration = game_session.saved_vibration;
        game_settings.vibration = vibration;
        player_saves.cur = player_saves.replay_backup;
    }
    state = game_session.exit_reason;
    input_recording.Stop();
    switch (state)
    {
        case 1:

            goto restart;
        case 2:
            count = game_progress.lives;
            if (count != 0)
            {
                if (GetCheatOptions().unlimited_lives == 0)
                {
                    game_progress.lives = count - 1;
                    count = game_progress.lives;
                }
                player_saves.cur.progress.lives = count;
                if (count != 0)
                {
                    ResetScene(true);
                    InitializeLevelRecs();
                    created_camera = 0;
                    object = AllocateZeroedObject(0x244);
                    if (object != 0)
                    {
                        created_player = 0;
                        resource = AllocateZeroedObject(0x290);
                        if (resource != 0)
                        {
                            created_player = new (resource) Player();
                        }
                        if (!created_player)
                        {
                            GameDiagnostic(("Cannot allocate session player\n"));
                            abort();
                        }
                        created_camera = new (object) Camera(*created_player);
                    }
                    if (!created_camera)
                    {
                        GameDiagnostic(("Cannot allocate session camera\n"));
                        abort();
                    }
                    created_camera->mode = 3;
                    player_saves.cur.RestoreInventory();
                    goto restore;
                }
            }
            count = GetGameTuning().initial_lives;

            game_progress.lives = count;
            goto restart;
        case 3:
            ResetScene(false);
            entry = FindLevelEntry(GetCurLevelName());
            if (!entry)
            {
                GameDiagnostic(("Unknown level for completion movie\n"));
                abort();
            }
            PlayMovie(entry->completion_movie);
            if (player_saves.cur.checksum == 0)
            {
                refresh = true;
                goto restart;
            }
            player_saves.sel = player_saves.cur;
            goto load_default;
        case 4:
        case 5:
            ResetScene(false);
            ++game_session.demo_idx;
            if (GetDemoRecordingName(game_session.demo_idx)[0] == 0)
                game_session.demo_idx = 0;
            goto restart;
        case 6:
            ResetScene(true);
            InitializeLevelRecs();
            created_camera = 0;
            object = AllocateZeroedObject(0x244);
            if (object != 0)
            {
                created_player = 0;
                resource = AllocateZeroedObject(0x290);
                if (resource != 0)
                {
                    created_player = new (resource) Player();
                }
                if (!created_player)
                {
                    GameDiagnostic(("Cannot allocate session player\n"));
                    abort();
                }
                created_camera = new (object) Camera(*created_player);
            }
            if (!created_camera)
            {
                GameDiagnostic(("Cannot allocate session camera\n"));
                abort();
            }
            created_camera->mode = 3;
            goto restore;
        case 7:

            goto restart;
        default:
            goto restart;
    }
restore:
    ApplyStartRec();
    ResetPanelDispTimer();
    goto frame;
load_default:
    player_saves.sel.LoadLevel();
    goto frame;
}
