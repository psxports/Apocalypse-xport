#include "model_instance.h"
#include "level_session.h"
#include "scene_lifecycle.h"
#include "input_recording.h"
#include "game_pad_input.h"
#include "cheat_input.h"
#include "game_tick.h"
#include "visual_object.h"
#include "pause_menu.h"
#include "audio_session.h"
#include "screen_flash.h"
#include "explosion_effects.h"
#include "object_damage.h"
#include "hud_text.h"
#include "enemy_list.h"
#include "resource_script.h"
#include "activation_record.h"
#include "effect_lifecycle.h"
#include "game_panel.h"
#include "model_bank.h"
#include "resource_io.h"
#include "effect_object.h"

GameClock game_clock = {0, 0, 0, 0, 0};
#if defined(_DEBUG)
SceneTimingCounters scene_timing = {0};
#endif

void ResetGameClock()
{
    game_clock.tick = 0;
    game_clock.step = 0;
    game_clock.elapsed = 0;
    game_clock.active_frame = 0;
    game_clock.vblank_frame = 0;
}

uint32 AdvanceGameTick()
{
    FUNCTION_MARKER(0x8002FF90, "SLUS_003.73");
    if (input_recording.GetMode() == InputRecording::Recording && game_pads[0].buttons[15].down)
    {
        input_recording.Stop();
        game_session.exit_reason = 7;
        // Original 8002FF88 is an empty handler
    }
#if defined(_DEBUG)
    if (scene_timing.active)
    {
        ++scene_timing.logic_calls;
        scene_timing.sampled_us = xport_timer_get();
    }
#endif
    uint32 step = game_clock.step;
    ++game_clock.elapsed;
    game_clock.step = step + 1;
    if (step & 1)
        return game_clock.step;
    ++game_clock.tick;
#if defined(_DEBUG)
    if (scene_timing.active)
        ++scene_timing.logic_ticks;
#endif
    ModelInstance::UpdateFlashes();
    ResetHudTextQueue();
    GamePadInput::Poll();
    if (scene_frame.paused || scene_frame.camera_paused)
        VisualObject::UpdateList(visual_lists.cameras);
    else
    {
        UpdateScrFlash();
        ResetExplosionQueue();
        VisualObject::ClearActivationFlags(visual_lists.actors);
        VisualObject::ClearActivationFlags(visual_lists.solids);
        ClearScriptTouches();
        VisualObject::UpdateList(visual_lists.players);
        ReleaseUntouchedScripts();
        EffectObject::UpdateAll();
        VisualObject::UpdateList(visual_lists.pickups);
        VisualObject::UpdateList(visual_lists.projectiles);
        VisualObject::UpdateList(visual_lists.solids);
        VisualObject::UpdateList(visual_lists.registry);
        VisualObject::UpdateList(visual_lists.actors);
        ActivationRecord::RestoreNearby();
        VisualObject::RestoreNearbySuspended();
        EffectObject::DestroyFinished();
    }
    FlushPendingPositionedSound();
    DrawDebugPanel();
    UpdatePauseMenu();
    AnimatedHudText::UpdateAll();
    uint32 loading_suspended = GetCheatOptions().suspend_resource_loading;
    uint32 result = loading_suspended;
    if (!loading_suspended)
    {
        ModelBank::ProcessLoadQueue();
        result = ProcessResourceRead();
    }
    CaptureHdLogicState();
    return result;
}
