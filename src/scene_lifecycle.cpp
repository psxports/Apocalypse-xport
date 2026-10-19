#include <stdlib.h>
#include <string.h>

#include "game_image_resource.h"
#include "game_gpu_present.h"
#include "psx_spu.h"
#include "psx_gpu.h"
#include "game_data_io.h"
#include "cd_voice_event.h"
#include "player_save_state.h"
#include "input_recording.h"
#include "level_session.h"
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
#include "game_diagnostics.h"
#include "scene_lifecycle.h"
#include "player.h"
#include "activation_record.h"
#include "effect_lifecycle.h"
#include "hud_text.h"
#include "object_damage.h"
#include "record_scripts.h"
#include "record_commands.h"
#include "audio_session.h"
#include "model_resources.h"

static void DrawSceneFrame();

SceneRenderState scene_render = {0};

// GPU packets retain the original VRAM self-copy and four trailing NOP words
static uint32 ordering_tail[2][5];

void ResetSceneRenderState()
{
    memset(&scene_render, 0, sizeof(scene_render));
    memset(ordering_tail, 0, sizeof(ordering_tail));
    if (!gpu_register_packet_range(ordering_tail, sizeof(ordering_tail)))
        abort();
    ordering_tail[0][0] = 0x04FFFFFF;
    ordering_tail[1][1] = 0x80000000;
    ordering_tail[1][4] = 0x00010002;
    for (uint32 idx = 0; idx < 2; ++idx)
    {
        SceneRenderBuffer *buffer = scene_render.buffers + idx;
        if (!gpu_register_packet_range(buffer->ot, sizeof(buffer->ot)) || !gpu_register_packet_range(buffer->packets, sizeof(buffer->packets)))
            abort();
    }
}

SceneFrameState scene_frame = {0, 0, 0, 1};

void ResetSceneFrameState()
{
    SceneFrameState initial = {0, 0, 0, 1};
    scene_frame = initial;
}

struct SceneSmoothing
{
    bool saved;
    bool initialized;
    uint32 prev_tick;
};

static SceneSmoothing scene_smoothing = {true, false, 0};

void ResetSceneSmoothing()
{
    scene_smoothing.saved = true;
    scene_smoothing.initialized = false;
    scene_smoothing.prev_tick = 0;
}

static void DestroyTransientSceneObjects(ModelInstance *instance)
{
    ModelBorrow model_borrow;
    FUNCTION_MARKER(0x8007115C, "SLUS_003.73");
    while (instance)
    {
        ModelInstance *next = instance->next;
        VisualObject &object = *static_cast<VisualObject *>(instance->owner);
        if (object.IsAlive() && !(object.lifecycle_flags & 32))
            object.Destroy(3);
        instance = next;
    }
}

static uint32 ClearScene()
{
    ModelBorrow model_borrow;
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
    Vector3 position;
    FUNCTION_MARKER(0x80071288, "SLUS_003.73");
    MarkMusicRestorePending();
    ResetMusicPlaybackState();
    model_viewport.distance = 256;
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
        ReleaseScriptWorkspace();
        UnloadLevelRecords();
        RebuildModelVramAllocations();
        ReleaseMenuBuffers();
        player_saves.current.progress.checkpoint[0] = 0;
    }
    SetRandomSeed(0x12A6CC58);
    ClearActivationRecords();
    ClearModelBankGrids();
    ::ResetScreenFlash();
    InitializeObjectFlashes();
    LoadPanelResource();
    position.words[0] = 0;
    position.words[1] = 0x8000;
    position.words[2] = 0;
    // The original stack vector is native storage under the admitted C ABI
    SetGravityVector(position.words);
    game_clock.tick = 0;
    InitializeGamePads();
    visual_activation_distance = 13000;
    level_environment.water_damage = 0;
    panel_text.game_over_y = -20;
    return (uint32)-20;
}

uint32 RunLevelLoop(void)
{
    uint32 frame_start;
    sint32 elapsed;
    PSX_RECT rectangle;
    FUNCTION_MARKER(0x80030AD4, "SLUS_003.73");
    SavePlayerTransform();
    game_clock.vblank_frame = 0;
    game_clock.tick = 0;
    game_clock.step = 0;
    game_session.catchup_blocked = 0;
    InitializeGamePads();
    game_clock.elapsed = 0;
    input_recording.ResetTimestamp();
    game_session.exit_reason = 0;
    SetRandomSeed(312921176);
#if defined(_DEBUG)
    DebugRestoreFinishMenu();
#endif
    WaitForGameFrames(1);
    while (!game_session.exit_reason)
    {
        (void)0;
        frame_start = game_clock.vblank_frame;
        BeginSceneFrame();
        UpdateScenePacketLimit();
        ::AdvanceGameTick();
        if (game_session.exit_reason)
            break;
        ::AdvanceGameTick();
        if (game_session.exit_reason)
            break;
        DrawSceneFrame();
        UpdateMoviePlayback();
        if ((game_clock.vblank_frame == frame_start))
            WaitForGameFrames(1);
        scene_frame.service_enabled = 0;
        while (1)
        {
            WaitForGameFrames(1);
            if (!DrawSync(1))
                break;
            ServiceGameFrame();
        }

        SubmitSceneFrame();
        if (!scene_frame.service_enabled)
        {
            ServiceGameFrame();
            scene_frame.service_enabled = 1;
        }
        elapsed = (sint32)(game_clock.vblank_frame - 2 - frame_start);
        if ((((!game_session.demo_mode && (input_recording.GetMode() != InputRecording::Recording)) && (elapsed > 0)) && !game_session.catchup_blocked))
        {
            ::AdvanceGameTick();
            if (game_session.exit_reason)
                break;
            if ((elapsed >= 2))
            {
                ::AdvanceGameTick();
                if (game_session.exit_reason)
                    break;
            }
        }
        if (static_cast<VisualObject *>(visual_lists.players->owner)->IsFinished())
            game_session.exit_reason = 2;
    }

    ::WaitForModelBankLoads();
    model_render_state.depth_bias[0] = 1;
    ::ReleaseMenuBuffers();
    if (game_session.demo_mode)
        input_recording.Clear();
    rectangle.x = 0;
    rectangle.y = 0;
    rectangle.w = 512;
    rectangle.h = 511;
    ClearImageRectangle(rectangle, 0, 0, 0);
    rectangle.y = 511;
    rectangle.h = 1;
    return ClearImageRectangle(rectangle, 0, 0, 0);
}

uint32 WaitForGameFrames(uint32 frames)
{
    FUNCTION_MARKER(0x800664E4, "SLUS_003.73");
    uint32 target = game_clock.vblank_frame + frames;
    uint32 current = game_clock.vblank_frame;
    uint32 waiting = current < target;
    while (waiting)
    {
        // Native adapter polls VBlank while the original counter wait is active
        VSync(-1);
        current = game_clock.vblank_frame;
        waiting = current < target;
    }
    return waiting;
}

uint32 ServiceGameFrame()
{
    FUNCTION_MARKER(0x800662A0, "SLUS_003.73");
    CdlLOC location;
    uint32 active = cd_voice.active;
    if (active)
    {
        uint32 status = SyncDataCd(1, cd_voice.response);
        if (status == 5)
        {
            cd_voice.active = 0;
            cd_voice.available = 1;
            FastControlDataCd(11, 0);
        }
        else if (status == 2)
        {
            uint32 current = cd_voice.current_sector;
            uint32 end = cd_voice.end_sector;
            if ((sint32)current < (sint32)end)
            {
                bool found = false;
                if (CdLastCom() == 17)
                {
                    status = CdPosToInt((CdlLOC *)(cd_voice.response + 5));
                    found = (sint32)status > 0;
                }
                if (found)
                {
                    ControlDataCd(1, 0, cd_voice.response);
                    if (!(cd_voice.response[0] & 64))
                        cd_voice.current_sector = status;
                }
                FastControlDataCd(17, 0);
            }
            else
            {
                cd_voice.available = 1;
                cd_voice.active = 0;
                cd_voice.cooldown = 30;
                FastControlDataCd(11, 0);
                CdIntToPos((sint32)cd_voice.base_sector, &location);
                FastControlDataCd(22, &location);
            }
        }
    }
    else
    {
        uint32 delay = cd_voice.cooldown;
        if (delay)
            cd_voice.cooldown = delay - 1;
    }
    uint32 fade = audio_playback.reverb_ticks;
    uint32 result = fade - 1;
    if (fade)
    {
        audio_playback.reverb_ticks = result;
        if (!result)
            return SetSoundReverbDepth(32, 32);
    }
    return result;
}

static void DrawSceneFrame()
{
    FUNCTION_MARKER(0x800307F0, "SLUS_003.73");
    const char *caption = GetLevelTestCaption();
    if (caption)
    {
        SetHudTextColor(255, 0, 0, 0);
        SetHudTextScale(256);
        SetHudTextAlignment(0);
        DrawStyledHudText(256, 60, caption, 0, 0, 256);
    }
    bool smoothed = false;
    ModelInstance *object = visual_lists.cameras;
    model_viewport.distance = static_cast<Camera *>(object->owner)->GetProjDistance();
    BlurMenuBuffers();
    if (!scene_smoothing.initialized)
    {
        uint32 frame = game_clock.tick;
        scene_smoothing.initialized = true;
        scene_smoothing.prev_tick = frame;
    }
    uint32 current_frame = game_clock.tick;
    uint32 previous_frame = scene_smoothing.prev_tick;
    if ((sint32)(current_frame - previous_frame) >= 2 && scene_smoothing.saved)
    {
        object = visual_lists.players;
        scene_smoothing.saved = false;
        smoothed = true;
        SmoothObjectPositions(object);
        SmoothObjectPositions(visual_lists.actors);
        SmoothObjectPositions(visual_lists.solids);
        SmoothTexturedQuadPositions();
    }
    else
    {
        object = visual_lists.players;
        scene_smoothing.saved = true;
        ::SaveObjectPositions(object);
        ::SaveObjectPositions(visual_lists.actors);
        ::SaveObjectPositions(visual_lists.solids);
        SaveTexturedQuadPositions();
    }
    scene_smoothing.prev_tick = game_clock.tick;
    UpdateVisualObjects(visual_lists.cameras);
    ConfigureModelRenderView(GetModelRenderView(), model_viewport, scene_render.cur->ot);
    RenderVisibleModelObjectList(GetModelSceneObjects());
    RenderVisibleModelObjectList(visual_lists.solids);
    object = visual_lists.players;
    sint16 first_clip = model_render_state.depth_bias[0];
    sint16 second_clip = model_render_state.depth_bias[1];
    model_render_state.depth_bias[0] = 8;
    model_render_state.depth_bias[1] = -8;
    RenderVisibleModelObjectList(object);
    object = visual_lists.actors;
    model_render_state.depth_bias[0] = first_clip;
    model_render_state.depth_bias[1] = second_clip;
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
    scene_render.used = scene_render.cursor - (uint8 *)scene_render.cur->packets;
    if (smoothed)
    {
        ::RestoreObjectPositions(visual_lists.players);
        ::RestoreObjectPositions(visual_lists.actors);
        ::RestoreObjectPositions(visual_lists.solids);
        RestoreTexturedQuadPositions();
    }
}

void BeginSceneFrame()
{
    FUNCTION_MARKER(0x80068160, "SLUS_003.73");
    scene_render.cur = scene_render.cur == &scene_render.buffers[0] ? &scene_render.buffers[1] : &scene_render.buffers[0];
    uint32 *ot = scene_render.cur->ot;
    ClearOTagR(ot, 4096);
    ordering_tail[1][0] = gpu_packet_link((uintptr_t)ordering_tail[0]) | 0x04000000;
    *ot = gpu_packet_link((uintptr_t)ordering_tail[1]);
    scene_render.cursor = (uint8 *)scene_render.cur->packets;
}

void SubmitSceneFrame()
{
    FUNCTION_MARKER(0x800681B8, "SLUS_003.73");
    ResetGameGraphics(1);
    PutDispEnv(&scene_render.cur->disp);
    PutDrawEnv(&scene_render.cur->draw);
    DrawOTag(scene_render.cur->ot + 4095);
}

void UpdateScenePacketLimit()
{
    FUNCTION_MARKER(0x8002FF54, "SLUS_003.73");
    scene_render.limit = (uint8 *)scene_render.cur->packets + sizeof(scene_render.cur->packets);
}

uint32 AdvanceSceneVBlank()
{
    FUNCTION_MARKER(0x80066458, "SLUS_003.73");
    uint32 frames = game_clock.vblank_frame;
    uint32 paused = scene_frame.paused;
    game_clock.vblank_frame = frames + 1;
    if (!paused && !scene_frame.camera_paused)
        ++game_clock.active_frame;
    if (scene_frame.submit_pending)
    {
        SubmitActiveGpuFrame();
        scene_frame.submit_pending = 0;
    }
    uint32 result = scene_frame.service_enabled;
    return result ? ServiceGameFrame() : result;
}

void ResetSceneDisplayOffsets()
{
    FUNCTION_MARKER(0x8006805C, "SLUS_003.73");
    scene_render.buffers[1].disp.screen.x = 0;
    scene_render.buffers[0].disp.screen.x = 0;
    scene_render.buffers[1].disp.screen.y = 0;
    scene_render.buffers[0].disp.screen.y = 0;
}

uint32 InitializeSceneDisplay()
{
    FUNCTION_MARKER(0x80068084, "SLUS_003.73");
    SetDefDrawEnv(&scene_render.buffers[0].draw, 0, 0, 512, 240);
    SetDefDrawEnv(&scene_render.buffers[1].draw, 0, 256, 512, 240);
    SetDefDispEnv(&scene_render.buffers[0].disp, 0, 256, 512, 240);
    SetDefDispEnv(&scene_render.buffers[1].disp, 0, 0, 512, 240);
    ResetSceneDisplayOffsets();
    scene_render.buffers[0].draw.isbg = 1;
    scene_render.buffers[0].draw.r0 = 0;
    scene_render.buffers[0].draw.g0 = 0;
    scene_render.buffers[0].draw.b0 = 0;
    scene_render.buffers[1].draw.isbg = 1;
    scene_render.buffers[1].draw.r0 = 0;
    scene_render.buffers[1].draw.g0 = 0;
    scene_render.buffers[1].draw.b0 = 0;
    return 1;
}
