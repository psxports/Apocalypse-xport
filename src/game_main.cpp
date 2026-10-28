#include "collision_ray.h"
#include "model_bank_grid.h"
#include "model_instance.h"

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
#include "effect_object.h"
#include "primitive_particle.h"
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
#include "game_pad_input.h"
#include "scene_lifecycle.h"
#include "level_session.h"
#include "game_data_io.h"
#include "global.h"
#include "model_bank.h"
#include "main_menu.h"
#include "pause_menu.h"
#include "screen_flash.h"
#include "explosion_effects.h"
#include "menu_window.h"
#include "debug_finish_repro.h"
#include "enemy.h"
#include "visual_object.h"
#include "hd.h"

static void FreeHdResources()
{
    hd_reset();
    ModelBank::ReleaseHdCaches();
    hd_free();
}

extern "C" void xport_main(void)
{
    xport_set_cleanup(FreeHdResources);
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
    ResetStartupDisp();
    spu_vm_reset();
    InitializeScrFlash();
    ResetExplosionState();
    ResetFragmentState();
    ModelInstance::ResetDestruction();
    PrimitiveParticle::ResetSettings();
    EffectObject::ResetDrawMode();
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
    CollisionRay::ResetSharedState();
    Player::ResetRuntime();
    Swat::ResetPopulation();
    Enemy::ResetPopulation();
    ProjectileBase::ResetPopulation();
    GamePadInput::ResetAll();
    ClearInputRecording();
    input_recording.ResetTimestamp();
    ResetCardSaveState();
    ResetPlayerSaveStorage();
    ResetPlayerSaveTemplates();
    ResetCurLevelName();
    ResetGameSession();
    ResetGameProgress();
    ResetLevelEnvironment();
    ResetSceneFrameState();
    ResetSceneRenderState();
    ResetSceneSmoothing();
    ModelBank::ResetAll();
    ResetModelPoseWorkspace();
    ModelBank::ResetActive();
    ModelBankGrid::ClearAll();
    ModelBank::ResetLoadQueue();
    ResetMenuScenes();
    VisualObject::ResetLists();
    MenuWindow::ResetRepeat();
    ResetMenuRuntime();
    ResetPanelText();
    ResetHudTextState();
    if (!gpu_bind_psyq_state(0))
    {
        xport_set_exit_code(1);
        return;
    }
    // Game frame producers do not provide complete frame-skip recovery scopes
    RunGameSession();
}
