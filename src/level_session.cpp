#include <stdio.h>
#include <stdlib.h>
#include <new>
#include <string.h>

#include "pause_menu.h"
#include "game_settings.h"
#include "game_tick.h"
#include "game_tuning.h"
#include "input_recording.h"
#include "game_pad_poll.h"

#include "record_data.h"

#include "visual_object.h"
#include "cheat_input.h"
#include "level_catalogue.h"
#include "game_string.h"
#include "game_diagnostics.h"
#include "debug_finish_repro.h"
#include "game_random.h"
#include "object_initializers.h"
#include "hud_text.h"

#include "level_session.h"
#include "main_menu.h"
#include "loading_screen.h"
#include "credits.h"
#include "startup_resources.h"
#include "scene_lifecycle.h"
#include "movie_lifecycle.h"
#include "record_factory.h"
#include "game_object_allocation.h"
#include "game_heap.h"
#include "model_resources.h"
#include "record_commands.h"
#include "record_scripts.h"
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
                GameDiagnostic("Cannot allocate level test caption\n");
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

static const char *current_level_name;

const char *GetCurrentLevelName()
{
    return current_level_name;
}

void ResetCurrentLevelName()
{
    current_level_name = 0;
}

GameSessionState game_session = {VibrationFull, 0, 0, 0, 0, 1, 0};

void ResetGameSession()
{
    GameSessionState initial = {VibrationFull, 0, 0, 0, 0, 1, 0};
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

static void LoadLevelFromPlayerSave(PlayerSaveRecord &record)
{
    GameHeapBlock *allocation;
    GameHeapBlock *camera;
    Player *player = 0;
    const LevelEntry *entry;
    uint32 health, lives, minimum;
    Camera *active_camera;
    FUNCTION_MARKER(0x80018C30, "SLUS_003.73");
#if defined(_DEBUG)
    DebugLoadFinishPlayerSave(record);
#endif
    current_level_name = record.progress.level;
    SetRandomSeed(0x12A6CC58);
    entry = FindLevelEntry(current_level_name);
    if (!entry)
    {
        GameDiagnostic("Unknown level in player save\n");
        abort();
    }
    PrepareLevelLoadingScreen(current_level_name, entry->loading_image);
    if (entry)
    {
        StopMusicPlayback();
        ReleaseAudioSession();
        ReleaseLevelSoundBank();
        LoadLevelSoundBank(entry->sound_bank);
        LoadAudioSequence(entry->music);
    }
    SetResourceReadOption(2000);
    LoadLevelRecords(current_level_name);
    ResetLevelLoadDisplayState();
    SetResourceReadOption(4);
    allocation = AllocateZeroedObject(656);
    if (allocation)
    {
        player = new (allocation) Player();
    }
    // TODO Preserve the original null-allocation fallthrough
    health = (uint16)record.progress.health;
    player->health = (sint16)(uint16)(health);
    RestorePlayerInventory(record);
    if (record.progress.checkpoint[0])
        SelectStartRecord(record.progress.checkpoint);
    lives = record.progress.lives;
    minimum = (uint32)GetGameTuning().initial_lives;
    game_progress.lives = lives;
    if ((sint32)lives < (sint32)minimum)
        game_progress.lives = minimum;
    camera = AllocateZeroedObject(580);
    if (camera)
    {
        new (camera) Camera(*player);
    }
    ApplyStartRecord();
    CaptureCurrentPlayerSave();
    ResetPanelDisplayTimer();
    active_camera = visual_lists.cameras ? static_cast<Camera *>(visual_lists.cameras->owner) : 0;
    active_camera->mode = 3;
    scene_render.buffers[0].draw.isbg = 1;
    scene_render.buffers[0].draw.r0 = 0;
    scene_render.buffers[0].draw.g0 = 0;
    scene_render.buffers[0].draw.b0 = 0;
    scene_render.buffers[1].draw.isbg = 1;
    scene_render.buffers[1].draw.r0 = 0;
    scene_render.buffers[1].draw.g0 = 0;
    scene_render.buffers[1].draw.b0 = 0;
}

static const char *const completion_captions[13] = {
    "Prison level completed", "Sewers level completed", "City level completed", "Death level completed", "Rooftops level completed", "Graveyard level completed", "Plague level completed", "Unknown", "War factory level completed", "War level completed", "Whitehouse level completed", "Unknown", "Unknown",
};

uint32 CompleteLevel(uint32 completed)
{
    Player *owner = visual_lists.players ? static_cast<Player *>(visual_lists.players->owner) : 0;
    uint32 value, state;
    const LevelEntry *entry, *next_level;
    uint32 bonus = 0, count, denominator, numerator;
    const char *name;
    FUNCTION_MARKER(0x800174A8, "SLUS_003.73");
    scene_frame.paused = 1;
    if (owner)
        owner->StopWeapon();
    ClearAnimatedHudText();
    ClearGamePadActuator(0, 0);
    ClearGamePadActuator(0, 1);
    panel_text.lives_timer = 0;
    CaptureCurrentPlayerSave();
    entry = FindLevelEntry(GetCurrentLevelName());
    if (!entry)
    {
        GameDiagnostic("Unknown completed level\n");
        abort();
    }
    next_level = NextLevelEntry(entry);
    if (!next_level)
    {
        player_saves.current.checksum = 0;
        game_session.exit_reason = 3;
        return 3;
    }
    player_saves.current.progress.SetLevel(next_level->name);
    value = GetDefaultPlayerSave().progress.health;
    player_saves.current.progress.checkpoint[0] = 0;
    game_progress.completion.completed = completed;
    player_saves.current.progress.health = value;
    if (completed)
    {
        state = level_environment.area_kind;
        static const uint8 completion_bonus[11] = {5, 22, 26, 0, 11, 23, 0, 0, 3, 0, 18};
        if (state < 11)
            bonus = completion_bonus[state];
        count = game_progress.level.enemy_kills;
        value = 0;
        if (count)
        {
            denominator = game_progress.level.enemy_total;
            if (denominator)
            {
                numerator = 100 * (count + bonus);
                if (numerator == 0x80000000 && denominator == (uint32)-1)
                    value = numerator;
                else
                    value = (uint32)((sint32)numerator / (sint32)denominator);
            }
        }
        game_progress.completion.target_percent = value;
        game_progress.completion.display_percent = 0;
        game_progress.completion.blink_ticks = 43;
    }
    state = level_environment.area_kind;
    name = state < 13 ? completion_captions[state] : "Unknown";
    panel_text.caption = name;
    value = game_clock.vblank_frame;
    panel_text.visible = 0;
    menu_runtime.screen = MENU_SCREEN_RESULTS_CAPTION;
    panel_text.reveal_frame = value;
    return 5;
}

void ReleaseLevelLoadResources()
{
    FUNCTION_MARKER(0x80011B04, "SLUS_003.73");
    UnloadUnprotectedModelBanks();
    RebuildModelVramAllocations();
    ReleaseScriptWorkspace();
}

void RunGameSession()
{
    uint32 refresh = 0;
    uint32 state;
    uint32 test_level;
    const LevelEntry *entry;
    uint32 index;
    GameHeapBlock *object;
    Camera *created_camera;
    Player *created_player;
    GameHeapBlock *resource;
    Vector3 position;
    uint32 count;
    uint32 toggle;
    FUNCTION_MARKER(0x80030D74, "SLUS_003.73");
    InitializeGameConstructors();
    InitializeGameRuntime();
    if (game_pads[0].buttons[14].down == 0)
        PlayMovie(0x1C);
restart:
    ResetScene(0);
    if (refresh != 0)
    {
        refresh = 0;
        RunCredits();
    }
    state = RunMainMenu();
    if (state == 1)
        goto restart;
    if ((sint32)state < 2)
    {
        if (state == 0)
        {
            if (NamesEqualIgnoringCase(player_saves.selected.progress.level, GetLevelEntry(0)->name))
                PlayMovie(2);
            goto load_default;
        }
        goto frame;
    }
    if (state == 2)
    {
        player_saves.selected.progress = GetDefaultPlayerSave().progress;
        test_level = 0;
        for (;;)
        {
            if (test_level >= GetLevelCount())
                test_level = 0;
            ResetScene(0);
            if (test_level == 0)
            {
                GameDiagnostic("\nHeap 0 space used = %ld", GetGameHeapUsed(0));
                GameDiagnostic("\nHeap 1 space used = %ld", GetGameHeapUsed(1));
            }
            index = 0;
            player_saves.selected.progress.SetLevel(GetLevelEntry(test_level)->name);
            player_saves.selected.progress.checkpoint[0] = 0;
            LoadLevelFromPlayerSave(player_saves.selected);
            while ((sint32)index < (sint32)GetLevelRecordCount())
            {
                if ((sint16)(GetLevelRecord(index)).Read16() == 8)
                {
                    RecordCursor checkpoint_name = ReadRecordPosition(position.words, index) + 6;
                    ResetScene(1);
                    InitializeLevelRecords();
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
                        created_camera = new (object) Camera(*created_player);
                    }
                    // TODO Original allocation failure path still writes through a zero object
                    created_camera->mode = 3;
                    SelectStartRecord(checkpoint_name.CStr());
                    test_caption.Set(checkpoint_name.CStr());
                    checkpoint_name = RecordCursor();
                    ApplyStartRecord();
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
                ++index;
            }
            ++test_level;
        }
    }
    if (state != 3)
        goto frame;
    if (game_session.demo_next == 0)
    {
        game_session.movie_any_button_skip = 1;
        PlayMovie(0x1C);
        toggle = game_session.demo_next;
        game_session.demo_next = toggle ^ 1;
        goto restart;
    }
    state = game_settings.vibration;
    game_settings.vibration = VibrationNone;
    game_session.saved_vibration = state;
    player_saves.replay_backup = player_saves.current;
    index = game_session.demo_index;
    LoadLevelFromPlayerSave(GetDemoPlayerSave(index));
    index = game_session.demo_index;
    input_recording.LoadReplay(GetDemoRecordingName(index));
    toggle = game_session.demo_next;
    game_session.demo_next = toggle ^ 1;
frame:
    RunLevelLoop();
    if (input_recording.HasBuffer())
    {
        input_recording.ReleaseData();
        game_session.exit_reason = 4;
    }
    if (input_recording.GetMode() == InputRecording::Replay)
    {
        state = game_session.saved_vibration;
        game_settings.vibration = state;
        player_saves.current = player_saves.replay_backup;
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
                player_saves.current.progress.lives = count;
                if (count != 0)
                {
                    ResetScene(1);
                    InitializeLevelRecords();
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
                        created_camera = new (object) Camera(*created_player);
                    }
                    // TODO Original allocation failure path still writes through a zero object
                    created_camera->mode = 3;
                    RestorePlayerInventory(player_saves.current);
                    goto restore;
                }
            }
            count = (uint32)GetGameTuning().initial_lives;

            game_progress.lives = count;
            goto restart;
        case 3:
            ResetScene(0);
            entry = FindLevelEntry(GetCurrentLevelName());
            if (!entry)
            {
                GameDiagnostic("Unknown level for completion movie\n");
                abort();
            }
            PlayMovie(entry->completion_movie);
            if (player_saves.current.checksum == 0)
            {
                refresh = 1;
                goto restart;
            }
            player_saves.selected = player_saves.current;
            goto load_default;
        case 4:
        case 5:
            ResetScene(0);
            index = game_session.demo_index + 1;
            game_session.demo_index = index;
            index &= 0xFF;
            if (GetDemoRecordingName(index)[0] == 0)
                game_session.demo_index = 0;
            goto restart;
        case 6:
            ResetScene(1);
            InitializeLevelRecords();
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
                created_camera = new (object) Camera(*created_player);
            }
            // TODO Original allocation failure path still writes through a zero object
            created_camera->mode = 3;
            goto restore;
        case 7:

            goto restart;
        default:
            goto restart;
    }
restore:
    ApplyStartRecord();
    ResetPanelDisplayTimer();
    goto frame;
load_default:
    LoadLevelFromPlayerSave(player_saves.selected);
    goto frame;
}
