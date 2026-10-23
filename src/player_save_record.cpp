#include <stdlib.h>
#include <new>
#include <string.h>
#include <stddef.h>

#include "game_diagnostics.h"
#include "level_session.h"
#include "global.h"
#include "record_data.h"
#include "resource_script.h"
#include "weapon_type.h"
#include "weapon_node.h"
#include "visual_object.h"
#include "player_save_state.h"
#include "game_settings.h"
#include "player.h"
#include "credits_player.h"
#include "pause_menu.h"
#include "game_tick.h"
#include "game_tuning.h"
#include "input_recording.h"
#include "game_pad_input.h"
#include "cheat_input.h"
#include "level_catalogue.h"
#include "debug_finish_repro.h"
#include "object_initializers.h"
#include "hud_text.h"
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
#include "audio_session.h"
#include "resource_io.h"
#include "game_panel.h"
#include "camera.h"
#include "player_save_record.h"

static const char *cur_level_name;

uint32 PlayerSaveRecord::CalculateChecksum() const
{
    uint32 idx, result = 0;
    FUNCTION_MARKER(0x80018B44, "SLUS_003.73");
    uint32 data[PlayerSaveSize / 4];
    EncodePlayerSave((uint8 *)data, sizeof(data), *this);
    for (idx = 1; idx < PlayerSaveSize / 4; ++idx)
        result ^= data[idx];
    return result | 1;
}

uint8 PlayerSaveRecord::RestoreInventory()
{
    sint32 smartbombs;
    uint32 idx;
    uint8 mask, sel;
    WeaponNode *weapon;
    FUNCTION_MARKER(0x80018B70, "SLUS_003.73");
    smartbombs = progress.smartbombs;
    Player &player = *static_cast<Player *>(visual_lists.players->owner);
    if (smartbombs < 3)
        progress.smartbombs = 3;
    player.smartbombs = progress.smartbombs;
    for (idx = 1; idx < 8; ++idx)
    {
        mask = progress.weapon_mask;
        if ((mask >> idx) & 1)
        {
            player.ReplaceWeapon(idx);
            weapon = player.weapons[idx];
            weapon->ammo = progress.ammo[idx];
        }
    }
    sel = progress.sel_weapon;
    player.sel_weapon = sel;
    return sel;
}

void PlayerSaveRecord::LoadLevel()
{
    GameHeapBlock *allocation;
    GameHeapBlock *camera;
    Player *player = 0;
    const LevelEntry *entry;
    sint32 lives;
    sint16 minimum;
    Camera *active_camera;
    FUNCTION_MARKER(0x80018C30, "SLUS_003.73");
#if defined(_DEBUG)
    DebugLoadFinishPlayerSave(*this);
#endif
    cur_level_name = progress.level;
    SetRandomSeed(0x12A6CC58);
    entry = FindLevelEntry(cur_level_name);
    if (!entry)
    {
        GameDiagnostic(("Unknown level in player save\n"));
        abort();
    }
    PrepareLevelLoadingScr(cur_level_name, entry->loading_image);
    if (entry)
    {
        StopMusicPlayback();
        ReleaseAudioSession();
        ReleaseLevelSoundBank();
        LoadLevelSoundBank(entry->sound_bank);
        LoadAudioSequence(entry->music);
    }
    SetResourceReadOption(2000);
    LoadLevelRecs(cur_level_name);
    ResetLevelLoadDispState();
    SetResourceReadOption(4);
    allocation = AllocateZeroedObject(656);
    if (allocation)
    {
        player = new (allocation) Player();
    }
    if (!player)
    {
        GameDiagnostic(("Cannot allocate level player\n"));
        abort();
    }
    player->health = progress.health;
    RestoreInventory();
    if (progress.checkpoint[0])
        SelectStartRec(progress.checkpoint);
    lives = progress.lives;
    minimum = GetGameTuning().initial_lives;
    game_progress.lives = lives;
    if (lives < minimum)
        game_progress.lives = minimum;
    camera = AllocateZeroedObject(580);
    if (!camera)
    {
        GameDiagnostic(("Cannot allocate level camera\n"));
        abort();
    }
    new (camera) Camera(*player);
    ApplyStartRec();
    CaptureCurPlayerSave();
    ResetPanelDispTimer();
    active_camera = visual_lists.cameras ? static_cast<Camera *>(visual_lists.cameras->owner) : 0;
    if (!active_camera)
    {
        GameDiagnostic(("Missing camera after level startup\n"));
        abort();
    }
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

const char *GetCurLevelName()
{
    return cur_level_name;
}

void ResetCurLevelName()
{
    cur_level_name = 0;
}
