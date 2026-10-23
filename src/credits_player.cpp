#include <new>
#include <stdlib.h>

#include "audio_session.h"
#include "credits_player.h"
#include "global.h"
#include "model_attachment.h"
#include "weapon_type.h"
#include "player.h"
#include "model_instance.h"
#include "game_pad_input.h"
#include "game_heap.h"
#include "game_lighting.h"
#include "game_tick.h"
#include "effect_lifecycle.h"
#include "game_panel.h"
#include "angle_math.h"
#include "model_matrix.h"
#include "model_render_view.h"
#include "game_diagnostics.h"
#include "scene_effect_packets.h"
#include "level_catalogue.h"
#include "menu_input.h"
#include "game_object_allocation.h"
#include "resource_io.h"
#include "hud_text.h"
#include "scene_lifecycle.h"
#include "model_render.h"
#include "effect_object.h"

static const uint32 RowCapacity = 400;

struct CreditsRow
{
    char text[2][40];
};

static bool DecodeRows(const char *data, uint32 bytes, CreditsRow *rows, uint32 &count);

CreditsPlayer::CreditsPlayer() : Player(), pose_ticks(0)
{
    FUNCTION_MARKER(0x80014818, "SLUS_003.73");
    SetPose(0);
    model.color = 0x404040;
    model.flags |= 4;
}

void CreditsPlayer::SetPose(uint32 pose)
{
    FUNCTION_MARKER(0x80014870, "SLUS_003.73");
    movement_state = pose;
    pose_ticks = 120;
    switch (pose)
    {
        case 0:
        case 2:
        case 3:
            motion.angular_velocity.y = 16;
            break;
        case 1:
        case 4:
            motion.angular_velocity.y = 0;
            break;
        default:
            break;
    }
}

void CreditsPlayer::ChoosePose()
{
    FUNCTION_MARKER(0x800148C0, "SLUS_003.73");
    uint32 pose = movement_state;
    for (uint32 attempt = 0; attempt < 100; ++attempt)
    {
        pose = RandomRange(5);
        if (pose != movement_state)
            break;
    }
    SetPose(pose);
}

uint32 CreditsPlayer::Update()
{
    FUNCTION_MARKER(0x80014920, "SLUS_003.73");
    if (pose_ticks)
        --pose_ticks;
    switch (movement_state)
    {
        case 0:
            if (model.animation != 1)
                StartAnimation(1, 1);
            if (!pose_ticks)
                ChoosePose();
            break;
        case 1:
            if (model.animation)
                StartAnimation(0, 1);
            if (!pose_ticks)
                ChoosePose();
            break;
        case 2:
        {
            uint8 animation = model.animation;
            if (animation != 9 && animation)
                SetAnimationRange(0, 0, -1);
            if (playback.complete && !model.animation)
                SetAnimationRange(9, 0, -1);
            if (!pose_ticks)
                ChoosePose();
            break;
        }
        case 3:
            if (model.animation != 5)
                SetAnimationRange(5, 0, -1);
            if (!pose_ticks)
            {
                uint8 frame_count = playback.frame_count;
                sint8 frame = model.frame;
                if (frame == frame_count - 1)
                    SetAnimationRange(5, -1, 0);
                if (!pose_ticks && !model.frame)
                    SetPose(0);
            }
            break;
        case 4:
        {
            uint8 animation = model.animation;
            if (animation != 5 && animation != 12)
                SetAnimationRange(5, 0, -1);
            if (playback.complete)
            {
                if (model.animation == 5)
                {
                    uint8 frame_count = playback.frame_count;
                    sint8 frame = model.frame;
                    if (frame == frame_count - 1)
                    {
                        PlaySoundEffect(4, 0x2000, 0);
                        SetAnimationRange(12, 0, -1);
                    }
                }
                if (playback.complete && model.animation == 12)
                    SetAnimationRange(5, -1, 0);
            }
            if (playback.complete && (model.frame == 0 && model.animation == 5))
                SetPose(0);
            break;
        }
        default:
            break;
    }
    model.angles.y += motion.angular_velocity.y;
    GetModelAttachment(muzzle_pos, *this, 0);
    weapons[0]->Fire(muzzle_pos, model.angles);
    AdvanceAnimation();
    return UpdateAnimationModel(PlayerHierarchy);
}

uint32 CreditsPlayer::Run()
{
    FUNCTION_MARKER(0x80014BFC, "SLUS_003.73");
    SwitchMenuAudioSequence(GetLevelEntry(0)->music);
    RestoreMusicPlayback();
    uint32 size = LookupResource("credits.txt");
    char *src = (char *)malloc(size);
    CreditsRow *rows = (CreditsRow *)malloc(RowCapacity * sizeof(CreditsRow));
    if (!src || !rows)
    {
        free(src);
        free(rows);
        GameDiagnostic(("Cannot allocate credits text\n"));
        abort();
    }
    BeginResourceRead(src);
    WaitForResourceRead();
    uint32 count;
    bool decoded = DecodeRows(src, size, rows, count);
    free(src);
    if (!decoded)
    {
        free(rows);
        GameDiagnostic(("Invalid credits text format\n"));
        abort();
    }
    GameHeapBlock *allocation = AllocateZeroedObject(660);
    CreditsPlayer *player = 0;
    if (allocation)
    {
        player = new (allocation) CreditsPlayer();
    }
    ModelRenderView &view = GetModelRenderView();
    view.pos.z = -310;
    view.pos.x = 0;
    view.pos.y = 0;
    view.angles.x = 0;
    view.angles.y = 0;
    view.angles.z = 0;
    game_lighting.background_color.r = 256;
    game_lighting.background_color.g = 256;
    game_lighting.background_color.b = 256;
    sint32 scroll = 0;
    SetHudTextScale(256);
    SetHudTextColor(64, 64, 64, 0);
    uint32 first_row = 0;
    while (first_row < count)
    {
        uint32 frame = game_clock.vblank_frame;
        BeginSceneFrame();
        UpdateScenePacketLimit();
        GamePadInput::Poll();
        uint32 confirm, cancel, any, start;
        ReadMenuInput(&confirm, &cancel, &any, &start);
        if (cancel)
        {
            PlaySoundEffect(19, 0x2000, 0);
            break;
        }
        scroll -= 64;
        if (confirm)
        {
            PlaySoundEffect(19, 0x2000, 0);
            break;
        }
        sint32 y = scroll >> 6;
        if (scroll < -1663)
        {
            scroll = 0;
            ++first_row;
            y = 0;
        }
        CreditsRow *row = rows + first_row;
        if (!player)
        {
            GameDiagnostic(("Missing credits player before update\n"));
            abort();
        }
        ++game_clock.tick;
        player->Update();
        EffectObject::UpdateAll();
        EffectObject::DestroyFinished();
        BlurMenuBuffers();
        BlurMenuBuffers();
        AnimatedHudText::DrawAll();
        while (y < 240 && row < rows + count)
        {
            uint32 glow = 0;
            // The signed multiply-high sequence divides this bounded value by sixty
            if (y > 60 && y < 121)
                glow = (y - 60) * 128 / 60;
            if (row->text[1][0])
            {
                SetHudTextAlignment(2);
                DrawStyledHudText(231, y, row->text[0], 0, glow, 256);
                SetHudTextAlignment(1);
                DrawStyledHudText(281, y, row->text[1], 0, glow, 256);
            }
            else
            {
                SetHudTextAlignment(0);
                DrawStyledHudText(256, y, row->text[0], 0, glow, 256);
            }
            y += 26;
            ++row;
        }
        view.angles.BuildRotationMatrix(view.rotation);
        view.SetTranslation();
        view.Configure(model_viewport, scene_render.cur->ot);
        ModelInstance::RenderVisibleList(player ? &player->model : 0);
        AdvanceModelRenderCounter();
        BuildSceneEffectPackets();
        if (game_clock.vblank_frame == frame)
            WaitForGameFrames(1);
        scene_frame.service_enabled = false;
        for (;;)
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
            scene_frame.service_enabled = true;
        }
    }
    WaitForGameFrames(1);
    DrawSync(0);
    SubmitSceneFrame();
    free(rows);
    if (player)
    {
        ObjectBorrow borrow;
        player->Destroy(3);
    }
    EffectObject::DestroyTransient();
    StopMusicPlayback();
    return ReleaseAudioSession();
}

static bool DecodeRows(const char *data, uint32 bytes, CreditsRow *rows, uint32 &count)
{
    uint32 cursor = 0;
    count = 0;
    while (count < RowCapacity)
    {
        if (cursor >= bytes)
            return false;
        if (data[cursor] == '#')
            return true;
        CreditsRow *row = rows + count;
        uint32 column = 0;
        while (cursor < bytes && data[cursor] != ',' && data[cursor] != '\r')
        {
            if (column >= sizeof(row->text[0]) - 1)
                return false;
            row->text[0][column++] = data[cursor++];
        }
        if (cursor >= bytes)
            return false;
        row->text[0][column] = 0;
        if (data[cursor] == '\r')
            row->text[1][0] = 0;
        else
        {
            ++cursor;
            column = 0;
            while (cursor < bytes && data[cursor] != '\r')
            {
                if (column >= sizeof(row->text[1]) - 1)
                    return false;
                row->text[1][column++] = data[cursor++];
            }
            if (cursor >= bytes)
                return false;
            row->text[1][column] = 0;
        }
        // Original skips the CR and the following byte without inspecting it
        if (bytes - cursor < 2)
            return false;
        cursor += 2;
        ++count;
    }
    return true;
}

uint32 CreditsPlayer::Destroy(uint32 reason)
{
    FUNCTION_MARKER(0x80015148, "SLUS_003.73");
    uint32 result = Player::Destroy(0);
    return (reason & 1) ? ReleaseStorage() : result;
}
