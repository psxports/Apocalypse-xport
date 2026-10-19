#include "level_session.h"
#include "scene_lifecycle.h"
#include "input_recording.h"
#include "game_pad_poll.h"
#include "cheat_input.h"
#include "game_diagnostics.h"
#include "game_tick.h"
#include "visual_object.h"
#include "pause_menu.h"
#include "audio_session.h"
#include "screen_flash.h"
#include "explosion_effects.h"
#include "object_damage.h"
#include "hud_text.h"
#include "enemy_list.h"
#include "record_scripts.h"
#include "activation_record.h"
#include "effect_lifecycle.h"
#include "game_panel.h"
#include "model_resources.h"
#include "resource_io.h"

GameClock game_clock = {0, 0, 0, 0, 0};

void ResetGameClock()
{
    game_clock.tick = 0;
    game_clock.step = 0;
    game_clock.elapsed = 0;
    game_clock.active_frame = 0;
    game_clock.vblank_frame = 0;
}

uint32 AdvanceGameTick(void)
{
    FUNCTION_MARKER(0x8002FF90, "SLUS_003.73");
    if (input_recording.GetMode() == InputRecording::Recording && game_pads[0].buttons[15].down)
    {
        input_recording.Stop();
        game_session.exit_reason = 7;
        // Original 8002FF88 is an empty handler
    }
    uint32 frame = game_clock.step;
    uint32 elapsed = game_clock.elapsed;
    uint32 result = frame + 1;
    game_clock.elapsed = elapsed + 1;
    game_clock.step = result;
    if (frame & 1)
        return result;
    ++game_clock.tick;
    UpdateObjectFlashes();
    ResetHudTextQueue();
    PollPadPackets();
    if (scene_frame.paused || scene_frame.camera_paused)
        UpdateVisualObjects(visual_lists.cameras);
    else
    {
        UpdateScreenFlash();
        ResetExplosionQueue();
        ClearObjectActivationFlags(visual_lists.actors);
        ClearObjectActivationFlags(visual_lists.solids);
        ClearScriptTouches();
        UpdateVisualObjects(visual_lists.players);
        ReleaseUntouchedScripts();
        UpdateEffects();
        UpdateVisualObjects(visual_lists.pickups);
        UpdateVisualObjects(visual_lists.projectiles);
        UpdateVisualObjects(visual_lists.solids);
        UpdateVisualObjects(visual_lists.registry);
        UpdateVisualObjects(visual_lists.actors);
        RestoreNearbyObjects();
        RestoreNearbySuspendedObjects();
        DestroyFinishedEffects();
    }
    FlushPendingPositionedSound();
    DrawDebugPanel();
    UpdatePauseMenu();
    UpdateAnimatedHudText();
    result = GetCheatOptions().suspend_resource_loading;
    if (!result)
    {
        ProcessModelBankLoadQueue();
        return ProcessResourceRead();
    }
    return result;
}
