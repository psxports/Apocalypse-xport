#include "game_sdk_bindings.h"
#include "scene_effect_packets.h"
#include "credits.h"
#include "menu_input.h"
#include "credits_player.h"
#include "native_object_storage.h"
#include "game_heap.h"
#include "game_object_allocation.h"
#include "resource_io.h"
#include "hud_text.h"
#include "scene_lifecycle.h"
#include "model_render.h"
extern "C"
{
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
}
#include <new>
#include <stdio.h>
#include <stdlib.h>

uint32 RunCredits(void)
{
    FUNCTION_MARKER(0x80014BFC, "SLUS_003.73");
    sub_80015F48(r_u32(0x800A525C));
    sub_8006A428();
    uint32 size = LookupResource((const char *)psx_addr(0x800A0334, 1));
    uint32 source = AllocateHeapBlock(size, 0, 1);
    BeginResourceRead(source);
    WaitForResourceRead();
    uint32 rows = AllocateHeapBlock(32000, 0, 1);
    uint32 cursor = source, row = rows, count = 0;
    while (count < 400)
    {
        uint32 value = r_u8(cursor);
        if (value == 35)
            break;
        uint32 column = 0;
        // TODO Original field copies have no forty-byte length bound
        while (value != 44 && value != 13)
        {
            ++cursor;
            w_u8(row + column, value);
            value = r_u8(cursor);
            ++column;
        }
        w_u8(row + column, 0);
        if (r_u8(cursor) == 13)
            w_u8(row + 40, 0);
        else
        {
            ++cursor;
            value = r_u8(cursor);
            column = 0;
            while (value != 13)
            {
                ++cursor;
                w_u8(row + column + 40, value);
                value = r_u8(cursor);
                ++column;
            }
            w_u8(row + column + 40, 0);
        }
        cursor += 2;
        row += 80;
        ++count;
    }
    ReleaseHeapBlock(source);
    uint32 player = AllocateZeroedObject(660);
    if (player)
    {
        void *dram_addr = AllocateNativeObjectStorage(player, sizeof(CreditsPlayer));
        new (dram_addr) CreditsPlayer();
    }
    // TODO Preserve the original null-allocation fallthrough into the draw loop
    w_u32(0x800ED528, (uint32)-310);
    w_u32(0x800ED520, 0);
    w_u32(0x800ED524, 0);
    w_u16(0x800ED548, 0);
    w_u16(0x800ED54A, 0);
    w_u16(0x800ED54C, 0);
    w_u32(0x800ED51C, 0);
    w_u16(0x800FFA94, 256);
    w_u16(0x800FFA96, 256);
    w_u16(0x800FFA98, 256);
    sint32 scroll = 0;
    SetHudTextScale(256);
    SetHudTextColor(64, 64, 64, 0);
    uint32 first_row = rows, end = rows + count * 80;
    while (first_row < end)
    {
        uint32 frame = r_u32(0x800FF64C);
        BeginSceneFrame();
        UpdateScenePacketLimit();
        sub_80070748();
        uint32 confirm, cancel, any, start;
        ReadMenuInput(&confirm, &cancel, &any, &start);
        if (cancel)
        {
            sub_80069DF0(19, 0x2000, 0);
            break;
        }
        scroll = (sint32)((uint32)scroll - 64);
        if (confirm)
        {
            sub_80069DF0(19, 0x2000, 0);
            break;
        }
        sint32 y = scroll >> 6;
        if (scroll < -1663)
        {
            scroll = 0;
            first_row += 80;
            y = 0;
        }
        uint32 text = first_row, second = first_row + 40;
        uint32 table = r_u32(player + 68);
        uint32 tick_count = r_u32(0x800FF2F0);
        sint32 adjustment = (sint16)r_u16(table + 24);
        w_u32(0x800FF2F0, tick_count + 1);
        uint32 target = r_u32(table + 28);
        uint32 receiver = player + (uint32)adjustment;
        if (target == 0x80014920)
            CreditsPlayer::Attach(receiver).CreditsPlayer::Update();
        else
        {
            // TODO Resolve another tick target if the credits table is replaced
            GameDiagnostic("Missing credits tick %08X receiver=%08X\n", target, receiver);
            abort();
        }
        sub_800372B8();
        sub_80032C88();
        sub_8002E2E8();
        sub_8002E2E8();
        DrawAnimatedHudText();
        while (y < 240 && text < end)
        {
            uint32 glow = 0;
            // The signed multiply-high sequence divides this bounded value by sixty
            if (y > 60 && y < 121)
                glow = (uint32)((y - 60) * 128 / 60);
            if (r_u8(second))
            {
                SetHudTextAlignment(2);
                DrawStyledHudText(231, (uint32)y, (const char *)psx_addr(text, 1), 0, glow, 256);
                SetHudTextAlignment(1);
                DrawStyledHudText(281, (uint32)y, (const char *)psx_addr(second, 1), 0, glow, 256);
            }
            else
            {
                SetHudTextAlignment(0);
                DrawStyledHudText(256, (uint32)y, (const char *)psx_addr(text, 1), 0, glow, 256);
            }
            y += 26;
            second += 80;
            text += 80;
        }
        sub_800858FC(0x800ED548, 0x800ED550);
        sub_800878AC(0x800ED550, 0x800ED520);
        sub_8007E63C(0x800ED51C, 0x800A67B8, r_u32(0x800FF660) + 112);
        RenderVisibleModelObjectList(player);
        AdvanceModelRenderCounter();
        BuildSceneEffectPackets();
        if (r_u32(0x800FF64C) == frame)
            WaitForGameFrames(1);
        w_u32(0x800FF65C, 0);
        for (;;)
        {
            WaitForGameFrames(1);
            if (!DrawSync(1))
                break;
            sub_800662A0();
        }
        SubmitSceneFrame();
        if (!r_u32(0x800FF65C))
        {
            sub_800662A0();
            w_u32(0x800FF65C, 1);
        }
    }
    WaitForGameFrames(1);
    DrawSync(0);
    SubmitSceneFrame();
    ReleaseHeapBlock(rows);
    if (player)
    {
        uint32 table = r_u32(player + 68);
        sint32 adjustment = (sint16)r_u16(table + 8);
        apocalypse_object_cleanup(r_u32(table + 12), player + (uint32)adjustment, 3);
    }
    sub_80032B64();
    sub_8006A3F0();
    return sub_80069A94();
}

extern "C" uint32 sub_80014BFC(void)
{
    FUNCTION_MARKER(0x80014BFC, "SLUS_003.73");
    return RunCredits();
}
