#if defined(LOCKSTEP_DEBUG)
    #error Native refactoring is outside lockstep; use the preserved decomp baseline
#endif

extern "C"
{
#include "psx.h"
#include "xport.h"
#include "psx_gpu.h"
#include "psx_spu.h"
#include "game_gpu_present.h"
}
#include "audio_session.h"
#include "object_initializers.h"
#include "object_fragments.h"
#include "object_damage.h"
#include "collision.h"
#include "particle_settings.h"
#include "effect_lifecycle.h"
#include "startup_resources.h"
#include "audio_voice.h"
#include "game_panel.h"
#include "hud_text.h"
#include "card_save.h"
#include "cd_voice_event.h"
#include "movie_lifecycle.h"
#include "input_recording.h"
#include "game_tick.h"
#include "game_settings.h"
#include "player_save_state.h"
#include "game_tuning.h"
#include "game_lighting.h"
#include "model_render.h"
#include "player.h"
#include "projectile_base.h"
#include "swat.h"
#include "game_pad_poll.h"
#include "scene_lifecycle.h"
#include "level_session.h"
#include "game_data_io.h"
#include "game_random.h"
#include "model_resources.h"
#include "main_menu.h"
#include "pause_menu.h"
#include "screen_flash.h"
#include "explosion_effects.h"
#include "menu_window.h"
#include "debug_finish_repro.h"

extern "C" void xport_main(void)
{
    if (!InitializeDataCd())
    {
        xport_message_error("Apocalypse", "Cannot initialize game DATA streams");
        xport_set_exit_code(1);
        return;
    }
    ResetGameConstructors();
#if defined(_DEBUG)
    ResetDebugFinishRepro();
#endif
    ResetStartupDisplay();
    spu_vm_reset();
    InitializeScreenFlash();
    ResetExplosionState();
    ResetFragmentState();
    ResetObjectDestruction();
    ResetParticleSettings();
    ResetEffectDrawMode();
    ResetRandomGenerators();
    ResetGameClock();
    ResetGameSettings();
    ResetAudioPlayback();
    ResetSoundEvents();
    ResetSoundBankHandles();
    ResetSoundVoicePitches();
    ResetCdVoiceState();
    ResetMoviePlaybackState();
    ResetMenuFireBuffers();
    ResetGameTuning();
    ResetGameLighting();
    ResetModelRenderState();
    ResetCollisionState();
    ResetPlayerRuntime();
    ResetSwatPopulation();
    ResetEnemyPopulation();
    ResetProjectilePopulation();
    ResetGamePadState();
    input_recording.Clear();
    input_recording.ResetTimestamp();
    ResetCardSaveState();
    ResetPlayerSaveStorage();
    ResetPlayerSaveTemplates();
    ResetCurrentLevelName();
    ResetGameSession();
    ResetGameProgress();
    ResetLevelEnvironment();
    ResetSceneFrameState();
    ResetSceneRenderState();
    ResetSceneSmoothing();
    ResetModelBanks();
    ResetModelPoseWorkspace();
    ResetActiveModelState();
    ClearModelBankGrids();
    ResetModelBankLoadQueue();
    ResetMenuScenes();
    ResetVisualLists();
    ResetMenuRepeat();
    ResetMenuRuntime();
    ResetPanelText();
    ResetHudTextState();
    if (!gpu_bind_psyq_state(0))
    {
        xport_set_exit_code(1);
        return;
    }
    // Game frame producers do not provide complete frame-skip recovery scopes
    gpu_set_frame_skip(0);
    RunGameSession();
}
