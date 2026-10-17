#include "game_tick.h"
#include "visual_object.h"
#include "model_render_view.h"
#include "debug_finish_repro.h"
#include "scene_effect_packets.h"
#include "camera.h"
#include "movie_lifecycle.h"
#include "model_render.h"
#include "screen_flash.h"
#include "game_panel.h"
#include "textured_quad.h"
#include "game_pad_poll.h"
#include "game_random.h"
#include "world_gravity.h"
extern "C"
{
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
}
#include "scene_lifecycle.h"
extern "C"
{
#include "game_sound_tick_startup.h"
}
#include "player.h"
#include "activation_record.h"
#include "effect_lifecycle.h"
#include "hud_text.h"
#include "object_damage.h"
#include "record_scripts.h"
#include "record_commands.h"
#include "audio_session.h"
#include "model_resources.h"

void DestroyTransientSceneObjects(ModelInstance *instance)
{
    FUNCTION_MARKER(0x8007115C, "SLUS_003.73");
    while (instance)
    {
        ModelInstance *next = instance->next;
        VisualObject &object = *static_cast<VisualObject *>(instance->owner);
        if (!(object.lifecycle_flags & 32))
            object.Destroy(3);
        instance = next;
    }
}

uint32 ClearScene()
{
    FUNCTION_MARKER(0x800711D0, "SLUS_003.73");
    DestroyTransientSceneObjects(visual_lists.players);
    DestroyTransientSceneObjects(visual_lists.cameras);
    DestroyTransientSceneObjects(visual_lists.actors);
    DestroyTransientSceneObjects(visual_lists.solids);
    DestroyTransientSceneObjects(visual_lists.registry);
    DestroyTransientSceneObjects(visual_lists.pickups);
    DestroyTransientSceneObjects(visual_lists.projectiles);
    DestroyTransientSceneObjects(visual_lists.suspended);
    DeleteActivationRecords();
    DestroyTransientEffects();
    ClearAnimatedHudText();
    return InitializeObjectFlashes();
}

uint32 ResetScene(uint32 retain_resources)
{
    uint32 position[3];
    FUNCTION_MARKER(0x80071288, "SLUS_003.73");
    MarkMusicRestorePending();
    ResetMusicPlaybackState();
    w_u16(0x800A67C8, 256);
    ResetMoviePlayback();
    ClearScene();
    ClearResourceScripts();
    if (retain_resources)
        ::UnloadActiveModelBanks();
    else
    {
        StopMusicPlayback();
        ReleaseAudioSession();
        ReleaseLevelSoundBank();
        DisableSoundReverb();
        UnloadUnprotectedModelBanks();
        ReleaseScriptHeap();
        UnloadLevelRecords();
        RebuildModelVramAllocations();
        ReleaseMovieFrameBuffers();
        w_u8(0x800A53E1, 0);
    }
    SetRandomSeed(0x12A6CC58);
    ClearActivationRecords();
    ClearModelBankGrids();
    ::ResetScreenFlash();
    InitializeObjectFlashes();
    LoadPanelResource();
    position[0] = 0;
    position[1] = 0x8000;
    position[2] = 0;
    // The original stack vector is native storage under the admitted C ABI
    SetGravityVector(position);
    w_u32(0x800FF2F0, 0);
    InitializeGamePads();
    w_u32(0x800FF5EC, 13000);
    w_u32(0x800FF380, 0);
    w_u16(0x800FF038, (uint32)-20);
    return (uint32)-20;
}







uint32 RunLevelLoop(void)
{
    uint32 frame_start;
    sint32 elapsed;
    sint16 rectangle[4];
    uint32 packed;
    FUNCTION_MARKER(0x80030AD4, "SLUS_003.73");
    SavePlayerTransform();
    w_u32(0x800FF64C, 0);
    w_u32(0x800FF2F0, 0);
    w_u32(0x800FF2F4, 0);
    w_u8(0x800FF8F1, 0);
    InitializeGamePads();
    w_u32(0x800FF7BC, 0);
    w_u32(0x800FF7C0, 0);
    w_u32(0x800FF38C, 0);
    w_u32(0x800FF390, 0);
    w_u32(0x800FF2EC, 0);
    SetRandomSeed(312921176);
#if defined(_DEBUG)
    DebugRestoreFinishMenu();
#endif
    WaitForGameFrames(1);
    while (!r_u32(0x800FF2EC))
    {
        nullsub_16();
        frame_start = r_u32(0x800FF64C);
        BeginSceneFrame();
        UpdateScenePacketLimit();
        ::AdvanceGameTick();
        if (r_u32(0x800FF2EC))
            break;
        ::AdvanceGameTick();
        if (r_u32(0x800FF2EC))
            break;
        DrawSceneFrame();
        UpdateMoviePlayback();
        if ((r_u32(0x800FF64C) == frame_start))
            WaitForGameFrames(1);
        w_u32(0x800FF65C, 0);
        while (1)
        {
            WaitForGameFrames(1);
            if (!DrawSync(1))
                break;
            ServiceGameFrame();
        }

        SubmitSceneFrame();
        if (!r_u32(0x800FF65C))
        {
            ServiceGameFrame();
            w_u32(0x800FF65C, 1);
        }
        elapsed = (sint32)(r_u32(0x800FF64C) - 2 - frame_start);
        if ((((!r_u32(0x800FF304) && (r_u32(0x800FF818) != 1)) && (elapsed > 0)) && !r_u8(0x800FF8F1)))
        {
            ::AdvanceGameTick();
            if (r_u32(0x800FF2EC))
                break;
            if ((elapsed >= 2))
            {
                ::AdvanceGameTick();
                if (r_u32(0x800FF2EC))
                    break;
            }
        }
        if (sub_80062F48(visual_lists.players))
            w_u32(0x800FF2EC, 2);
    }

    ::WaitForModelBankLoads();
    w_u16(0x800FFAAC, 1);
    ::ReleaseMovieFrameBuffers();
    if (r_u32(0x800FF304))
        sub_80063A38();
    packed = r_u32(0x800FF368);
    rectangle[0] = (sint16)packed;
    rectangle[1] = (sint16)(packed >> 16);
    packed = r_u32(0x800FF36C);
    rectangle[2] = (sint16)packed;
    rectangle[3] = (sint16)(packed >> 16);
    xport_draft_host_sub_8008847C_p1(rectangle, 0, 0, 0);
    rectangle[1] = 511;
    rectangle[3] = 1;
    return xport_draft_host_sub_8008847C_p1(rectangle, 0, 0, 0);
}



uint32 WaitForGameFrames(uint32 frames)
{
    FUNCTION_MARKER(0x800664E4, "SLUS_003.73");
    uint32 target = r_u32(0x800FF64C) + frames;
    uint32 current = r_u32(0x800FF64C);
    uint32 waiting = current < target;
    while (waiting)
    {
        // Native adapter polls VBlank while the original counter wait is active
        VSync(-1);
        current = r_u32(0x800FF64C);
        waiting = current < target;
    }
    return waiting;
}



uint32 ServiceGameFrame()
{
    FUNCTION_MARKER(0x800662A0, "SLUS_003.73");
    uint8 location[8];
    apocalypse_music_tick();
    uint32 active = r_u32(0x800FF2B8);
    if (active)
    {
        uint32 status = sub_80097D90(1, 0x800FF2D8);
        if (status == 5)
        {
            w_u32(0x800FF2B8, 0);
            w_u32(0x800FF2BC, 1);
            sub_80097F34(11, 0);
        }
        else if (status == 2)
        {
            uint32 current = r_u32(0x800FF2D4);
            uint32 end = r_u32(0x800FF2D0);
            if ((sint32)current < (sint32)end)
            {
                bool found = false;
                if (sub_80097C6C() == 17)
                {
                    status = sub_8009835C(0x800FF2DD);
                    found = (sint32)status > 0;
                }
                if (found)
                {
                    xport_draft_host_sub_80097DF8_p2(1, 0, 0x800FF2D8);
                    if (!(r_u8(0x800FF2D8) & 64))
                        w_u32(0x800FF2D4, status);
                }
                sub_80097F34(17, 0);
            }
            else
            {
                w_u32(0x800FF2BC, 1);
                w_u32(0x800FF2B8, 0);
                w_u32(0x800FF2B0, 30);
                sub_80097F34(11, 0);
                xport_draft_host_sub_80098258_p2(r_u32(0x800FF2AC), location);
                xport_draft_host_sub_80097F34_p2(22, location);
            }
        }
    }
    else
    {
        uint32 delay = r_u32(0x800FF2B0);
        if (delay)
            w_u32(0x800FF2B0, delay - 1);
    }
    uint32 fade = r_u32(0x800FF6CC);
    uint32 result = fade - 1;
    if (fade)
    {
        w_u32(0x800FF6CC, result);
        if (!result)
            return sub_8006A4A0(32, 32);
    }
    return result;
}



uint32 DrawSceneFrame()
{
    FUNCTION_MARKER(0x800307F0, "SLUS_003.73");
    if (r_u32(0x800FF2E0))
    {
        SetHudTextColor(255, 0, 0, 0);
        SetHudTextScale(256);
        SetHudTextAlignment(0);
        sub_8001AA28(256, 60, r_u32(0x800FF2E0), 0, 0, 256);
    }
    bool smoothed = false;
    ModelInstance *object = visual_lists.cameras;
    w_u16(0x800A67C8, static_cast<Camera *>(object->owner)->GetProjDistance());
    BlurMenuBuffers();
    if (!r_u32(0x800FF360))
    {
        uint32 frame = r_u32(0x800FF2F0);
        w_u32(0x800FF360, 1);
        w_u32(0x800FFB8C, frame);
    }
    uint32 current_frame = r_u32(0x800FF2F0);
    uint32 previous_frame = r_u32(0x800FFB8C);
    if ((sint32)(current_frame - previous_frame) >= 2 && r_u32(0x800FF35C))
    {
        object = visual_lists.players;
        w_u32(0x800FF35C, 0);
        smoothed = true;
        SmoothObjectPositions(object);
        SmoothObjectPositions(visual_lists.actors);
        SmoothObjectPositions(visual_lists.solids);
        SmoothTexturedQuadPositions();
    }
    else
    {
        object = visual_lists.players;
        w_u32(0x800FF35C, 1);
        ::SaveObjectPositions(object);
        ::SaveObjectPositions(visual_lists.actors);
        ::SaveObjectPositions(visual_lists.solids);
        SaveTexturedQuadPositions();
    }
    w_u32(0x800FFB8C, r_u32(0x800FF2F0));
    UpdateVisualObjects(visual_lists.cameras);
    ConfigureModelRenderView(GetModelRenderView(), 0x800A67B8, r_u32(0x800FF660) + 112);
    RenderVisibleModelObjectList(GetModelSceneObjects());
    RenderVisibleModelObjectList(visual_lists.solids);
    object = visual_lists.players;
    uint32 first_clip = r_u16(0x800FFAAC);
    uint32 second_clip = r_u16(0x800FFAAE);
    w_u16(0x800FFAAC, 8);
    w_u16(0x800FFAAE, (uint32)-8);
    RenderVisibleModelObjectList(object);
    object = visual_lists.actors;
    w_u16(0x800FFAAC, first_clip);
    w_u16(0x800FFAAE, second_clip);
    RenderVisibleModelObjectList(object);
    RenderVisibleModelObjectList(visual_lists.pickups);
    RenderVisibleModelObjectList(visual_lists.projectiles);
    RenderModelObjectList(visual_lists.registry);
    AdvanceModelRenderCounter();
    BuildSceneEffectPackets();
    DrawHudTextQueue();
    DrawGamePanel();
    DrawPauseMenuDisplay();
    DrawAnimatedHudText();
    DrawScreenFlash();
    uint32 packet_end = r_u32(0x800FF668);
    uint32 packet_base = r_u32(0x800FF660);
    uint32 result = (packet_base + 16496) & 0x7FFFFFFF;
    w_u32(0x800FF34C, (packet_end & 0x7FFFFFFF) - result);
    if (smoothed)
    {
        ::RestoreObjectPositions(visual_lists.players);
        ::RestoreObjectPositions(visual_lists.actors);
        ::RestoreObjectPositions(visual_lists.solids);
        return RestoreTexturedQuadPositions();
    }
    return result;
}



uint32 BeginSceneFrame()
{
    FUNCTION_MARKER(0x80068160, "SLUS_003.73");
    uint32 base = r_u32(0x800FF660) == 0x800A72A0 ? 0x800C6310 : 0x800A72A0;
    uint32 ot = base + 112;
    w_u32(0x800FF660, base);
    ClearOTagR((uint32 *)psx_addr(ot, 4096 * 4), 4096);
    // Preserve the SDK packet tail address appended after clearing the OT
    w_u32(0x800FCF6C, (0x800FCF58 & 0xFFFFFF) | 0x04000000);
    w_u32(ot, 0x800FCF6C & 0xFFFFFF);
    uint32 result = (r_u32(0x800FF660) + 16496) & 0x7FFFFFFF;
    w_u32(0x800FF668, result);
    return result;
}

void SubmitSceneFrame()
{
    FUNCTION_MARKER(0x800681B8, "SLUS_003.73");
    apocalypse_reset_graph(1);
    PutDispEnv((DISPENV *)psx_addr(r_u32(0x800FF660) + 92, sizeof(DISPENV)));
    PutDrawEnv((DRAWENV *)psx_addr(r_u32(0x800FF660), sizeof(DRAWENV)));
    DrawOTag((uint32 *)psx_addr(r_u32(0x800FF660) + 16492, 4));
}





uint32 UpdateScenePacketLimit()
{
    uint32 limit;
    FUNCTION_MARKER(0x8002FF54, "SLUS_003.73");
    limit = (r_u32(0x800FF660) + 126832) & 0x7FFFFFFF;
    w_u32(0x800FF374, limit);
    return limit;
}



uint32 AdvanceSceneVBlank()
{
    FUNCTION_MARKER(0x80066458, "SLUS_003.73");
    uint32 frames = r_u32(0x800FF64C);
    uint32 paused = r_u32(0x800FF008);
    w_u32(0x800FF64C, frames + 1);
    if (!paused && !r_u32(0x800FF300))
        w_u32(0x800FF650, r_u32(0x800FF650) + 1);
    if (r_u32(0x800FF658))
    {
        sub_800681FC();
        w_u32(0x800FF658, 0);
    }
    uint32 result = r_u32(0x800FF65C);
    return result ? ServiceGameFrame() : result;
}

uint32 ResetSceneDisplayOffsets()
{
    FUNCTION_MARKER(0x8006805C, "SLUS_003.73");
    w_u16(0x800C6374, 0);
    w_u16(0x800A7304, 0);
    w_u16(0x800C6376, 0);
    w_u16(0x800A7306, 0);
    return 0x800A72A0;
}

uint32 InitializeSceneDisplay()
{
    FUNCTION_MARKER(0x80068084, "SLUS_003.73");
    sub_80087E5C(0x800A72A0, 0, 0, 512, 240);
    sub_80087E5C(0x800C6310, 0, 256, 512, 240);
    sub_80087F10(0x800A72FC, 0, 256, 512, 240);
    sub_80087F10(0x800C636C, 0, 0, 512, 240);
    ResetSceneDisplayOffsets();
    w_u8(0x800A72B8, 1);
    w_u8(0x800A72B9, 0);
    w_u8(0x800A72BA, 0);
    w_u8(0x800A72BB, 0);
    w_u8(0x800C6328, 1);
    w_u8(0x800C6329, 0);
    w_u8(0x800C632A, 0);
    w_u8(0x800C632B, 0);
    return 1;
}





