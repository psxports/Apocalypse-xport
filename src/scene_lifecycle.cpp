#include <stdlib.h>
#include <string.h>

#include "hd.h"
#include "model_bank_grid.h"
#include "model_instance.h"
#include "psx.h"
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
#include "game_pad_input.h"
#include "global.h"
#include "world_gravity.h"
#include "game_diagnostics.h"
#include "scene_lifecycle.h"
#include "player.h"
#include "activation_record.h"
#include "effect_lifecycle.h"
#include "hud_text.h"
#include "object_damage.h"
#include "resource_script.h"
#include "record_commands.h"
#include "audio_session.h"
#include "model_bank.h"
#include "model_reference.h"
#include "effect_object.h"

SceneRenderState scene_render = {0};

static bool scene_hd_frame;
static HD_FRAME scene_hd_state;
static ModelReference scene_hd_camera;
static bool scene_hd_wait;
static uint64 scene_hd_clock_field;
static float scene_hd_clock_fraction;

static float SceneRenderPhase()
{
    uint64 field;
    float fraction = xport_frame_fract(&field);
    if (field < scene_hd_clock_field || field - scene_hd_clock_field > 2)
        return 1;
    // One source update spans two physical fields
    float phase = ((float)(field - scene_hd_clock_field) + fraction - scene_hd_clock_fraction) / 2;
    return phase < 0 ? 0 : (phase > 1 ? 1 : phase);
}

// The self-copy omits the cache-clear prefix emitted by SetDrawMove
static DR_MOVE ordering_copy;
// The terminal packet carries four GPU NOP words
static uint32 ordering_nops[5];

void ResetSceneRenderState()
{
    memset(&scene_render, 0, sizeof(scene_render));
    memset(&ordering_copy, 0, sizeof(ordering_copy));
    memset(ordering_nops, 0, sizeof(ordering_nops));
    if (!gpu_register_packet_range(&ordering_copy, sizeof(ordering_copy)) || !gpu_register_packet_range(ordering_nops, sizeof(ordering_nops)))
        abort();
    ordering_nops[0] = 0x04FFFFFF;
    ordering_copy.code[0] = 0x80000000;
    ordering_copy.code[3] = 0x00010002;
    for (uint32 idx = 0; idx < 2; ++idx)
    {
        SceneRenderBuffer *buffer = scene_render.buffers + idx;
        if (!gpu_register_packet_range(buffer->ot, sizeof(buffer->ot)) || !gpu_register_packet_range(buffer->packets, sizeof(buffer->packets)))
            abort();
    }
}

SceneFrameState scene_frame = {false, false, 0, true};

void ResetSceneFrameState()
{
    SceneFrameState initial = {false, false, 0, true};
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
    ActivationRecord::DeleteAll();
    EffectObject::DestroyTransient();
    AnimatedHudText::ClearAll();
    return ModelInstance::InitializeFlashes();
}

sint32 ResetScene(bool retain_resources)
{
    Vector3 pos;
    FUNCTION_MARKER(0x80071288, "SLUS_003.73");
    MarkMusicRestorePending();
    ResetMusicPlaybackState();
    model_viewport.distance = 256;
    ResetMoviePlayback();
    ClearScene();
    ClearResourceScripts();
    if (retain_resources)
        ModelBank::UnloadActive();
    else
    {
        StopMusicPlayback();
        ReleaseAudioSession();
        ReleaseLevelSoundBank();
        DisableSoundReverb();
        ModelBank::UnloadUnprotected();
        ReleaseScriptWorkspace();
        UnloadLevelRecs();
        RebuildModelVramAllocations();
        ReleaseMenuBuffers();
        player_saves.cur.progress.checkpoint[0] = 0;
    }
    SetRandomSeed(0x12A6CC58);
    ActivationRecord::ClearAll();
    ModelBankGrid::ClearAll();
    ::ResetScrFlash();
    ModelInstance::InitializeFlashes();
    LoadPanelResource();
    pos.x = 0;
    pos.y = 0x8000;
    pos.z = 0;
    // The original stack vector is native storage under the admitted C ABI
    WorldGravity::SetVector(&pos);
    game_clock.tick = 0;
    GamePadInput::Initialize();
    visual_activation_distance = 13000;
    level_environment.water_damage = 0;
    panel_text.game_over_y = -20;
    return -20;
}

void ResetHdCamera(const ModelInstance *model)
{
    if (scene_hd_camera.GetObject() == model)
        hd_frame_camera_reset(&scene_hd_state);
}

static void BindHdCamera(ModelInstance *model)
{
    if (scene_hd_camera.GetObject() != model)
    {
        scene_hd_camera.SetObject(model);
        hd_frame_camera_reset(&scene_hd_state);
    }
}

void CaptureHdLogicState()
{
    if (!scene_hd_frame)
        return;
    hd_model_tick_begin(game_clock.tick);
    ModelInstance *camera_model = visual_lists.cameras;
    BindHdCamera(camera_model);
    if (camera_model && camera_model->owner)
    {
        const Camera *camera = static_cast<const Camera *>(camera_model->owner);
        ModelViewport bounds = model_viewport;
        bounds.distance = camera->proj_distance;
        HD_CAMERA snapshot;
        camera->CaptureHd(snapshot, bounds);
        hd_frame_camera_tick(&scene_hd_state, &snapshot, game_clock.tick);
    }
    ModelInstance *lists[5] = {visual_lists.players, visual_lists.actors, visual_lists.solids, visual_lists.pickups, visual_lists.projectiles};
    for (uint32 list = 0; list < 5; ++list)
        for (ModelInstance *model = lists[list]; model; model = model->next)
            if (!model->CaptureHdTick())
            {
                GameDiagnostic(("Cannot sample HD model logic state\n"));
                abort();
            }
    if (!CaptureHdEffectLogic())
    {
        GameDiagnostic(("Cannot sample HD effect logic state\n"));
        abort();
    }
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
    uint32 cur_frame = game_clock.tick;
    uint32 prev_frame = scene_smoothing.prev_tick;
    if ((sint32)(cur_frame - prev_frame) >= 2 && scene_smoothing.saved)
    {
        object = visual_lists.players;
        scene_smoothing.saved = false;
        smoothed = true;
        VisualObject::SmoothListPos(object);
        VisualObject::SmoothListPos(visual_lists.actors);
        VisualObject::SmoothListPos(visual_lists.solids);
        TexturedQuad::SmoothListPos();
    }
    else
    {
        object = visual_lists.players;
        scene_smoothing.saved = true;
        VisualObject::SaveListPos(object);
        VisualObject::SaveListPos(visual_lists.actors);
        VisualObject::SaveListPos(visual_lists.solids);
        TexturedQuad::SaveListPos();
    }
    scene_smoothing.prev_tick = game_clock.tick;
    VisualObject::UpdateList(visual_lists.cameras);
    GetModelRenderView().Configure(model_viewport, scene_render.cur->ot);
    if (scene_hd_frame)
    {
        BindHdCamera(visual_lists.cameras);
        static_cast<Camera *>(visual_lists.cameras->owner)->CaptureHd(scene_hd_state.camera, model_viewport);
        hd_scene_capture_begin(&scene_hd_state.camera);
    }
    ModelInstance::RenderVisibleList(ModelBank::GetSceneObjects());
    ModelInstance::RenderVisibleList(visual_lists.solids);
    object = visual_lists.players;
    sint16 saved_bias[2] = {model_render_state.depth_bias[0], model_render_state.depth_bias[1]};
    model_render_state.depth_bias[0] = 8;
    model_render_state.depth_bias[1] = -8;
    ModelInstance::RenderVisibleList(object);
    object = visual_lists.actors;
    model_render_state.depth_bias[0] = saved_bias[0];
    model_render_state.depth_bias[1] = saved_bias[1];
    ModelInstance::RenderVisibleList(object);
    ModelInstance::RenderVisibleList(visual_lists.pickups);
    ModelInstance::RenderVisibleList(visual_lists.projectiles);
    ModelInstance::RenderList(visual_lists.registry);
    if (scene_hd_frame)
    {
        hd_scene_capture_end();
        ModelBank::CaptureHdMaterials();
    }
    AdvanceModelRenderCounter();
    BuildSceneEffectPackets();
    DrawHudTextQueue();
    hd_ui_set_anchor(HD_UI_EDGES);
    DrawGamePanel();
    hd_ui_set_anchor(HD_UI_CENTER);
    DrawPauseMenuDisp();
    AnimatedHudText::DrawAll();
    DrawScrFlash();
    if (scene_hd_frame)
        hd_ui_end();
    scene_render.used = scene_render.cursor - (uint8 *)scene_render.cur->packets;
    if (smoothed)
    {
        VisualObject::RestoreListPos(visual_lists.players);
        VisualObject::RestoreListPos(visual_lists.actors);
        VisualObject::RestoreListPos(visual_lists.solids);
        TexturedQuad::RestoreListPos();
    }
}

uint32 RunLevelLoop()
{
    uint32 frame_start;
    sint32 elapsed;
    scene_hd_state.open = 0;
    scene_hd_camera.Reset();
    hd_frame_camera_reset(&scene_hd_state);
    scene_hd_wait = false;
    bool was_paused = scene_frame.paused || scene_frame.camera_paused;
    PSX_RECT rect;
    FUNCTION_MARKER(0x80030AD4, "SLUS_003.73");
    Player::SaveTransform();
    game_clock.vblank_frame = 0;
    game_clock.tick = 0;
    game_clock.step = 0;
    game_session.catchup_blocked = 0;
    GamePadInput::Initialize();
    game_clock.elapsed = 0;
    input_recording.ResetTimestamp();
    game_session.exit_reason = 0;
    SetRandomSeed(312921176);
#if defined(_DEBUG)
    DebugRestoreFinishMenu();
#endif
    WaitForGameFrames(1);
#if defined(_DEBUG)
    memset(&scene_timing, 0, sizeof(scene_timing));
    scene_timing.started_us = scene_timing.sampled_us = xport_timer_get();
    scene_timing.active = true;
#endif
    while (!game_session.exit_reason)
    {
        frame_start = game_clock.vblank_frame;
        scene_hd_clock_fraction = xport_frame_fract(&scene_hd_clock_field);
        hd_frame_begin(&scene_hd_state);
        scene_hd_frame = scene_hd_state.selected != 0;
        BeginSceneFrame();
        UpdateScenePacketLimit();
        ::AdvanceGameTick();
        if (game_session.exit_reason)
            break;
        ::AdvanceGameTick();
        if (game_session.exit_reason)
            break;
        bool paused = scene_frame.paused || scene_frame.camera_paused;
        if (paused != was_paused)
            hd_frame_abandon(&scene_hd_state);
        was_paused = paused;
        if (!scene_hd_state.open)
        {
            hd_frame_begin(&scene_hd_state);
            scene_hd_frame = scene_hd_state.selected != 0;
        }
#if defined(_DEBUG)
        uint64 source_start = xport_timer_get();
#endif
        DrawSceneFrame();
#if defined(_DEBUG)
        scene_timing.source_prepare_us += xport_timer_get() - source_start;
        ++scene_timing.source_frames;
#endif
        UpdateMoviePlayback();
        if (scene_hd_frame && scene_hd_state.open)
        {
            if (!hd_frame_present_phase(&scene_hd_state, SceneRenderPhase()))
            {
                GameDiagnostic(("Cannot draw interpolated game frame\n"));
                abort();
            }
#if defined(_DEBUG)
            ++scene_timing.hd_frames;
#endif
            VSync(-1);
            scene_hd_wait = hd_frame_can_redraw(&scene_hd_state) != 0;
        }
        if (game_clock.vblank_frame == frame_start)
            WaitForGameFrames(1);
        scene_frame.service_enabled = false;
        while (1)
        {
            WaitForGameFrames(1);
            if (!DrawSync(1))
                break;
            ServiceGameFrame();
        }

        scene_hd_wait = false;
        SubmitSceneFrame();
        if (!hd_frame_present(&scene_hd_state))
        {
            GameDiagnostic(("Cannot draw game frame\n"));
            abort();
        }
        if (!scene_frame.service_enabled)
        {
            ServiceGameFrame();
            scene_frame.service_enabled = true;
        }
        elapsed = (sint32)(game_clock.vblank_frame - 2 - frame_start);
        if (!game_session.demo_mode && input_recording.GetMode() != InputRecording::Recording && elapsed > 0 && !game_session.catchup_blocked)
        {
            ::AdvanceGameTick();
            if (game_session.exit_reason)
                break;
            if (elapsed >= 2)
            {
                ::AdvanceGameTick();
                if (game_session.exit_reason)
                    break;
            }
        }
        if (static_cast<VisualObject *>(visual_lists.players->owner)->IsFinished())
            game_session.exit_reason = 2;
    }

#if defined(_DEBUG)
    scene_timing.sampled_us = xport_timer_get();
    scene_timing.active = false;
#endif
    scene_hd_wait = false;
    hd_frame_abandon(&scene_hd_state);
    scene_hd_frame = false;
    scene_hd_camera.Reset();
    ModelBank::WaitForLoads();
    model_render_state.depth_bias[0] = 1;
    ::ReleaseMenuBuffers();
    if (game_session.demo_mode)
        ClearInputRecording();
    rect.x = 0;
    rect.y = 0;
    rect.w = 512;
    rect.h = 511;
    PSX_RECT clear_rect = rect;
    ClearImage(&clear_rect, 0, 0, 0);
    rect.y = 511;
    rect.h = 1;
    clear_rect = rect;
    return ClearImage(&clear_rect, 0, 0, 0);
}

bool WaitForRenderFrames(uint32 frames, HD_FRAME *frame, float (*phase)())
{
    uint32 target = game_clock.vblank_frame + frames;
    uint32 cur = game_clock.vblank_frame;
    bool waiting = cur < target;
    while (waiting)
    {
        // Native adapter polls VBlank while the original counter wait is active
        VSync(-1);
        cur = game_clock.vblank_frame;
        waiting = cur < target;
        if (waiting && frame && phase && hd_frame_can_redraw(frame))
        {
            if (!hd_frame_redraw(frame, phase()))
            {
                GameDiagnostic(("Cannot redraw interpolated game frame\n"));
                abort();
            }
#if defined(_DEBUG)
            if (frame == &scene_hd_state)
            {
                ++scene_timing.hd_frames;
                ++scene_timing.hd_redraws;
            }
#endif
        }
    }
    return waiting;
}

bool WaitForGameFrames(uint32 frames)
{
    FUNCTION_MARKER(0x800664E4, "SLUS_003.73");
    return WaitForRenderFrames(frames, scene_hd_wait ? &scene_hd_state : 0, SceneRenderPhase);
}

uint32 ServiceGameFrame()
{
    FUNCTION_MARKER(0x800662A0, "SLUS_003.73");
#if defined(_DEBUG)
    if (scene_timing.active)
        ++scene_timing.service_calls;
#endif
    CdlLOC location;
    if (cd_voice.active)
    {
        sint32 status = SyncDataCd(1, cd_voice.response);
        if (status == 5)
        {
            cd_voice.active = false;
            cd_voice.available = true;
            FastControlDataCd(11, 0);
        }
        else if (status == 2)
        {
            sint32 cur = cd_voice.cur_sector;
            sint32 end = cd_voice.end_sector;
            if (cur < end)
            {
                bool found = false;
                if (CdLastCom() == 17)
                {
                    location.minute = cd_voice.response[5];
                    location.second = cd_voice.response[6];
                    location.sector = cd_voice.response[7];
                    location.track = 0;
                    status = CdPosToInt(&location);
                    found = status > 0;
                }
                if (found)
                {
                    ControlDataCd(1, 0, cd_voice.response);
                    if (!(cd_voice.response[0] & 64))
                        cd_voice.cur_sector = status;
                }
                FastControlDataCd(17, 0);
            }
            else
            {
                cd_voice.available = true;
                cd_voice.active = false;
                cd_voice.cooldown = 30;
                FastControlDataCd(11, 0);
                CdIntToPos(cd_voice.base_sector, &location);
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

void BeginSceneFrame()
{
    FUNCTION_MARKER(0x80068160, "SLUS_003.73");
    scene_render.cur = scene_render.cur == &scene_render.buffers[0] ? &scene_render.buffers[1] : &scene_render.buffers[0];
    uint32 *ot = scene_render.cur->ot;
    ClearOTagR(ot, 4096);
    setaddr(&ordering_copy, gpu_packet_link((uintptr_t)ordering_nops));
    setlen(&ordering_copy, 4);
    *ot = gpu_packet_link((uintptr_t)&ordering_copy);
    scene_render.cursor = (uint8 *)scene_render.cur->packets;
}

void SubmitSceneFrame()
{
    FUNCTION_MARKER(0x800681B8, "SLUS_003.73");
    ResetGameGraphics(1);
    PutDispEnv(&scene_render.cur->disp);
    PutDrawEnv(&scene_render.cur->draw);
    DrawOTag(scene_render.cur->ot + 4095);
    if (hd_frame_restoring_og())
    {
        // The other display page still contains the last software frame
        DISPENV display = scene_render.cur->disp;
        display.disp.x = scene_render.cur->draw.clip.x;
        display.disp.y = scene_render.cur->draw.clip.y;
        PutDispEnv(&display);
    }
}

void UpdateScenePacketLimit()
{
    FUNCTION_MARKER(0x8002FF54, "SLUS_003.73");
    scene_render.limit = (uint8 *)scene_render.cur->packets + sizeof(scene_render.cur->packets);
}

uint32 AdvanceSceneVBlank()
{
    FUNCTION_MARKER(0x80066458, "SLUS_003.73");
#if defined(_DEBUG)
    if (scene_timing.active)
        ++scene_timing.vblank_callbacks;
#endif
    uint32 frames = game_clock.vblank_frame;
    bool paused = scene_frame.paused;
    game_clock.vblank_frame = frames + 1;
    if (!paused && !scene_frame.camera_paused)
        ++game_clock.active_frame;
    if (scene_frame.submit_pending)
    {
        SubmitActiveGpuFrame();
        scene_frame.submit_pending = 0;
    }
    return scene_frame.service_enabled ? ServiceGameFrame() : 0;
}

void ResetSceneDispOffsets()
{
    FUNCTION_MARKER(0x8006805C, "SLUS_003.73");
    scene_render.buffers[1].disp.screen.x = 0;
    scene_render.buffers[0].disp.screen.x = 0;
    scene_render.buffers[1].disp.screen.y = 0;
    scene_render.buffers[0].disp.screen.y = 0;
}

uint32 InitializeSceneDisp()
{
    FUNCTION_MARKER(0x80068084, "SLUS_003.73");
    SetDefDrawEnv(&scene_render.buffers[0].draw, 0, 0, 512, 240);
    SetDefDrawEnv(&scene_render.buffers[1].draw, 0, 256, 512, 240);
    SetDefDispEnv(&scene_render.buffers[0].disp, 0, 256, 512, 240);
    SetDefDispEnv(&scene_render.buffers[1].disp, 0, 0, 512, 240);
    ResetSceneDispOffsets();
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
