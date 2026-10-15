extern "C"
{
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
}
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

uint32 PollPadPackets(void);

uint32 AdvanceGameTick(void)
{
    FUNCTION_MARKER(0x8002FF90, "SLUS_003.73");
    if (r_u32(0x800FF818) == 1 && r_u8(0x800EC1E8))
    {
        w_u32(0x800FF818, 0);
        w_u32(0x800FF2EC, 7);
        // Original 8002FF88 is an empty handler
    }
    uint32 frame = r_u32(0x800FF2F4);
    uint32 elapsed = r_u32(0x800FF38C);
    uint32 result = frame + 1;
    w_u32(0x800FF38C, elapsed + 1);
    w_u32(0x800FF2F4, result);
    if (frame & 1)
        return result;
    w_u32(0x800FF2F0, r_u32(0x800FF2F0) + 1);
    UpdateObjectFlashes();
    ResetHudTextQueue();
    PollPadPackets();
    if (r_u32(0x800FF008) || r_u32(0x800FF300))
        UpdateVisualObjects((uint32 *)psx_addr(0x800FF904, 4));
    else
    {
        UpdateScreenFlash();
        ResetExplosionQueue();
        ClearObjectActivationFlags(r_u32(0x800FF4E8));
        ClearObjectActivationFlags(r_u32(0x800FF5DC));
        ClearScriptTouches();
        UpdateVisualObjects((uint32 *)psx_addr(0x800FF5A0, 4));
        ReleaseUntouchedScripts();
        UpdateEffects();
        UpdateVisualObjects((uint32 *)psx_addr(0x800FF204, 4));
        UpdateVisualObjects((uint32 *)psx_addr(0x800FF220, 4));
        UpdateVisualObjects((uint32 *)psx_addr(0x800FF5DC, 4));
        UpdateVisualObjects((uint32 *)psx_addr(0x800FF8A0, 4));
        UpdateVisualObjects((uint32 *)psx_addr(0x800FF4E8, 4));
        RestoreNearbyObjects();
        RestoreNearbySuspendedObjects();
        DestroyFinishedEffects();
    }
    FlushPendingPositionedSound();
    DrawDebugPanel();
    UpdatePauseMenu();
    UpdateAnimatedHudText();
    result = r_u32(0x800FF340);
    if (!result)
    {
        ProcessModelBankLoadQueue();
        return ProcessResourceRead();
    }
    return result;
}

extern "C" uint32 sub_8002FF90(void)
{
    FUNCTION_MARKER(0x8002FF90, "SLUS_003.73");
    return AdvanceGameTick();
}
