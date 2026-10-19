#include <new>
#include <stdio.h>
#include <stdlib.h>

#include "game_pad_poll.h"
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
#include "credits.h"
#include "level_catalogue.h"
#include "audio_session.h"
#include "menu_input.h"
#include "credits_player.h"
#include "game_object_allocation.h"
#include "resource_io.h"
#include "hud_text.h"
#include "scene_lifecycle.h"
#include "model_render.h"

static const uint32 credits_row_capacity = 400;

struct CreditsRow
{
    char text[2][40];
};

static bool DecodeCreditsRows(const uint8 *data, uint32 bytes, CreditsRow *rows, uint32 &count)
{
    uint32 cursor = 0;
    count = 0;
    while (count < credits_row_capacity)
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
            row->text[0][column++] = (char)data[cursor++];
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
                row->text[1][column++] = (char)data[cursor++];
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

uint32 RunCredits(void)
{
    FUNCTION_MARKER(0x80014BFC, "SLUS_003.73");
    SwitchMenuAudioSequence(GetLevelEntry(0)->music);
    ::RestoreMusicPlayback();
    uint32 size = LookupResource("credits.txt");
    uint8 *source = (uint8 *)malloc(size);
    CreditsRow *rows = (CreditsRow *)malloc(credits_row_capacity * sizeof(CreditsRow));
    if (!source || !rows)
    {
        free(source);
        free(rows);
        GameDiagnostic("Cannot allocate credits text\n");
        abort();
    }
    BeginResourceRead(source);
    WaitForResourceRead();
    uint32 count;
    bool decoded = DecodeCreditsRows(source, size, rows, count);
    free(source);
    if (!decoded)
    {
        free(rows);
        GameDiagnostic("Invalid credits text format\n");
        abort();
    }
    GameHeapBlock *allocation = AllocateZeroedObject(660);
    CreditsPlayer *player = 0;
    if (allocation)
    {
        player = new (allocation) CreditsPlayer();
    }
    // TODO Preserve the original null-allocation fallthrough into the draw loop
    GetModelRenderView().pos.words[2] = (uint32)-310;
    GetModelRenderView().pos.words[0] = 0;
    GetModelRenderView().pos.words[1] = 0;
    GetModelRenderView().angles.words[0] = (uint16)(0);
    GetModelRenderView().angles.words[1] = (uint16)(0);
    GetModelRenderView().angles.words[2] = (uint16)(0);
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
        PollPadPackets();
        uint32 confirm, cancel, any, start;
        ReadMenuInput(&confirm, &cancel, &any, &start);
        if (cancel)
        {
            PlaySoundEffect(19, 0x2000, 0);
            break;
        }
        scroll = (sint32)((uint32)scroll - 64);
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
        uint32 tick_count = game_clock.tick;
        game_clock.tick = tick_count + 1;
        player->Update();
        ::UpdateEffects();
        ::DestroyFinishedEffects();
        ::BlurMenuBuffers();
        ::BlurMenuBuffers();
        DrawAnimatedHudText();
        while (y < 240 && row < rows + count)
        {
            uint32 glow = 0;
            // The signed multiply-high sequence divides this bounded value by sixty
            if (y > 60 && y < 121)
                glow = (uint32)((y - 60) * 128 / 60);
            if (row->text[1][0])
            {
                SetHudTextAlignment(2);
                DrawStyledHudText(231, (uint32)y, row->text[0], 0, glow, 256);
                SetHudTextAlignment(1);
                DrawStyledHudText(281, (uint32)y, row->text[1], 0, glow, 256);
            }
            else
            {
                SetHudTextAlignment(0);
                DrawStyledHudText(256, (uint32)y, row->text[0], 0, glow, 256);
            }
            y += 26;
            ++row;
        }
        BuildModelRotationMatrix(GetModelRenderView().angles.words, GetModelRenderView().rotation);
        SetModelRenderViewTranslation(GetModelRenderView());
        ConfigureModelRenderView(GetModelRenderView(), model_viewport, scene_render.cur->ot);
        RenderVisibleModelObjectList(player ? &player->model : 0);
        AdvanceModelRenderCounter();
        BuildSceneEffectPackets();
        if (game_clock.vblank_frame == frame)
            WaitForGameFrames(1);
        scene_frame.service_enabled = 0;
        for (;;)
        {
            WaitForGameFrames(1);
            if (!DrawSync(1))
                break;
            ::ServiceGameFrame();
        }
        SubmitSceneFrame();
        if (!scene_frame.service_enabled)
        {
            ::ServiceGameFrame();
            scene_frame.service_enabled = 1;
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
    ::DestroyTransientEffects();
    ::StopMusicPlayback();
    return ::ReleaseAudioSession();
}
