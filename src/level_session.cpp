#include "game_sdk_bindings.h"
#include "debug_finish_repro.h"
#include "game_random.h"
#include "hud_text.h"
#include <stdio.h>
#include <stdlib.h>
extern "C"
{
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
uint32 apocalypse_crt_initialize(void);
uint32 sub_8008BF9C(uint32 format, uint32 value);
}
#include "level_session.h"
#include "main_menu.h"
#include "loading_screen.h"
#include "credits.h"
#include "startup_resources.h"
#include "scene_lifecycle.h"
#include "movie_lifecycle.h"
#include "record_factory.h"
#include "game_object_allocation.h"
#include "game_heap.h"
#include "model_resources.h"
#include "record_commands.h"
#include "record_scripts.h"
#include "player_save_state.h"
#include "audio_session.h"
#include "resource_io.h"
#include "game_panel.h"
#include "player.h"
#include "camera.h"
#include "native_object_storage.h"
#include <new>

uint32 LoadLevelFromPlayerSave(uint32 record)
{
    uint32 entry, name, context, bank_name, sequence_name, player, camera;
    uint32 health, lives, minimum, active_camera;
    FUNCTION_MARKER(0x80018C30, "SLUS_003.73");
#if defined(_DEBUG)
    DebugLoadFinishPlayerSave(record);
#endif
    // The record address remains guest storage because the level retains its name pointer
    w_u32(0x800FF83C, record + 4);
    SetRandomSeed(0x12A6CC58);
    name = r_u32(0x800FF83C);
    entry = FindLevelEntry(name);
    name = r_u32(0x800FF83C);
    // TODO Preserve the original context read before the missing-level check
    context = r_u32(entry + 16);
    PrepareLevelLoadingScreen((const char *)psx_addr(name, 1), context);
    if (entry)
    {
        StopMusicPlayback();
        ReleaseAudioSession();
        ReleaseLevelSoundBank();
        bank_name = r_u32(entry + 12);
        LoadLevelSoundBank(bank_name);
        sequence_name = r_u32(entry + 8);
        LoadAudioSequence(sequence_name);
    }
    SetResourceReadOption(2000);
    name = r_u32(0x800FF83C);
    LoadLevelRecords(name);
    ResetLevelLoadDisplayState();
    SetResourceReadOption(4);
    player = sub_800625AC(656);
    if (player)
    {
        void *dram_addr = AllocateNativeObjectStorage(player, sizeof(Player));
        new (dram_addr) Player();
    }
    // TODO Preserve the original null-allocation fallthrough
    health = r_u16(record + 100);
    w_u16(player + 218, health);
    RestorePlayerInventory((uint8 *)psx_addr(record, 136));
    if (r_u8(record + 13))
        SelectStartRecord(record + 13);
    lives = r_u32(record + 108);
    minimum = (uint32)(sint32)(sint16)r_u16(0x800EC522);
    w_u32(0x800FF874, lives);
    if ((sint32)lives < (sint32)minimum)
        w_u32(0x800FF874, minimum);
    camera = sub_800625AC(580);
    if (camera)
    {
        void *dram_addr = AllocateNativeObjectStorage(camera, sizeof(Camera));
        new (dram_addr) Camera(Player::Attach(player));
    }
    ApplyStartRecord();
    CaptureCurrentPlayerSave();
    ResetPanelDisplayTimer();
    active_camera = r_u32(0x800FF904);
    w_u32(active_camera + 564, 3);
    w_u8(0x800A72B8, 1);
    w_u8(0x800A72B9, 0);
    w_u8(0x800A72BA, 0);
    w_u8(0x800A72BB, 0);
    w_u8(0x800C6328, 1);
    w_u8(0x800C6329, 0);
    w_u8(0x800C632A, 0);
    w_u8(0x800C632B, 0);
    return 0x800C6310;
}

extern "C" uint32 sub_80018C30(uint32 record)
{
    FUNCTION_MARKER(0x80018C30, "SLUS_003.73");
    return LoadLevelFromPlayerSave(record);
}

uint32 CompleteLevel(uint32 completed)
{
    uint32 owner = r_u32(0x800FF5A0), descriptor, value, state, target;
    uint32 bonus = 0, count, denominator, numerator, name;
    FUNCTION_MARKER(0x800174A8, "SLUS_003.73");
    w_u32(0x800FF008, 1);
    if (owner)
        Player::Attach(owner).StopWeapon();
    ClearAnimatedHudText();
    sub_80070288(0, 0);
    sub_80070288(0, 1);
    w_u32(0x800FF758, 0);
    CaptureCurrentPlayerSave();
    descriptor = FindLevelEntry(r_u32(0x800FF83C)) + 24;
    value = r_u32(descriptor);
    if (!r_u8(value))
    {
        w_u32(0x800A53D4, 0);
        w_u32(0x800FF2EC, 3);
        return 3;
    }
    sub_80067808(r_u32(descriptor + 4), 0x800A53D8);
    value = r_u32(0x800A0578);
    w_u8(0x800A53E1, 0);
    w_u32(0x800FF040, completed);
    w_u32(0x800A5438, value);
    if (completed)
    {
        state = r_u32(0x800FF378);
        if (state < 11)
        {
            target = r_u32(0x800A063C + 4 * state);
            switch (target)
            {
                case 0x800175B4: bonus = 5; break;
                case 0x800175BC: bonus = 22; break;
                case 0x800175C4: bonus = 26; break;
                case 0x800175CC: bonus = 11; break;
                case 0x800175D4: bonus = 23; break;
                case 0x800175DC: bonus = 3; break;
                case 0x800175E4: bonus = 18; break;
                case 0x800175EC: break;
                default:
                    GameDiagnostic("TODO 800174A8 bonus table target %08X\n", target);
                    abort();
            }
        }
        count = r_u32(0x800FF30C);
        value = 0;
        if (count)
        {
            denominator = r_u32(0x800FF308);
            if (denominator)
            {
                numerator = 100 * (count + bonus);
                if (numerator == 0x80000000 && denominator == (uint32)-1)
                    value = numerator;
                else
                    value = (uint32)((sint32)numerator / (sint32)denominator);
            }
        }
        w_u32(0x800FFB54, value);
        w_u32(0x800FFB58, 0);
        w_u32(0x800FF044, 43);
    }
    state = r_u32(0x800FF378);
    name = 0x800FF090;
    if (state < 13)
    {
        target = r_u32(0x800A066C + 4 * state);
        switch (target)
        {
            case 0x80017688: name = r_u32(0x800A551C); break;
            case 0x80017698: name = r_u32(0x800A5520); break;
            case 0x800176A8: name = r_u32(0x800A5524); break;
            case 0x800176B8: name = r_u32(0x800A5528); break;
            case 0x800176C8: name = r_u32(0x800A552C); break;
            case 0x800176D8: name = r_u32(0x800A5530); break;
            case 0x800176E8: name = r_u32(0x800A5534); break;
            case 0x800176F8: name = r_u32(0x800A5538); break;
            case 0x80017708: name = r_u32(0x800A553C); break;
            case 0x80017718: name = r_u32(0x800A5540); break;
            case 0x80017728: break;
            default:
                GameDiagnostic("TODO 800174A8 name table target %08X\n", target);
                abort();
        }
    }
    w_u32(0x800FF030, name);
    value = r_u32(0x800FF64C);
    w_u32(0x800FF034, 0);
    w_u32(0x800FF080, 5);
    w_u32(0x800FFB50, value);
    return 5;
}

extern "C" uint32 sub_800174A8(uint32 completed)
{
    FUNCTION_MARKER(0x800174A8, "SLUS_003.73");
    return CompleteLevel(completed);
}

void ReleaseLevelLoadResources()
{
    FUNCTION_MARKER(0x80011B04, "SLUS_003.73");
    UnloadUnprotectedModelBanks();
    RebuildModelVramAllocations();
    ReleaseScriptHeap();
}

uint32 QueueSelectedLevelResources()
{
    FUNCTION_MARKER(0x80011B34, "SLUS_003.73");
    uint32 selection = r_u32(0x800FEFF0);
    uint32 name = r_u32(0x800A5374 + (selection << 5));
    QueueModelBankLoad(name, 0);
    selection = r_u32(0x800FEFF0);
    name = r_u32(0x800A5378 + (selection << 5));
    QueueModelBankLoad(name, 1);
    return AllocateScriptHeap(250000);
}

extern "C" void sub_80011B04(void)
{
    FUNCTION_MARKER(0x80011B04, "SLUS_003.73");
    ReleaseLevelLoadResources();
}
extern "C" uint32 sub_80011B34(void)
{
    FUNCTION_MARKER(0x80011B34, "SLUS_003.73");
    return QueueSelectedLevelResources();
}

void RunGameSession()
{
    uint32 refresh = 0;
    uint32 state;
    uint32 source;
    uint32 destination;
    uint32 pointer;
    uint32 word0, word1, word2, word3;
    uint32 index;
    uint32 object;
    uint32 resource;
    uint32 filename;
    uint32 position[3];
    uint32 count;
    uint32 toggle;
    FUNCTION_MARKER(0x80030D74, "SLUS_003.73");
    apocalypse_crt_initialize();
    InitializeGameRuntime();
    if (r_u8(0x800EC1D8) == 0)
        PlayMovie(0x1C);
restart:
    ResetScene(0);
    if (refresh != 0)
    {
        refresh = 0;
        RunCredits();
    }
    state = RunMainMenu();
    if (state == 1)
        goto restart;
    if ((sint32)state < 2)
    {
        if (state == 0)
        {
            if (sub_80067724(0x800A5460, r_u32(0x800A5258)) != 0)
                PlayMovie(2);
            goto load_default;
        }
        goto frame;
    }
    if (state == 2)
    {
        destination = 0x800A5460;
        source = 0x800A0518;
        do
        {
            word0 = r_u32(source);
            word1 = r_u32(source + 4);
            word2 = r_u32(source + 8);
            word3 = r_u32(source + 12);
            w_u32(destination, word0);
            w_u32(destination + 4, word1);
            w_u32(destination + 8, word2);
            w_u32(destination + 12, word3);
            source += 16;
            destination += 16;
        } while (source != 0x800A0578);
        word0 = r_u32(source);
        word1 = r_u32(source + 4);
        word2 = r_u32(source + 8);
        w_u32(destination, word0);
        w_u32(destination + 4, word1);
        w_u32(destination + 8, word2);
        pointer = 0x800A5254;
        for (;;)
        {
            if (r_u8(r_u32(pointer + 4)) == 0)
                pointer = 0x800A5254;
            ResetScene(0);
            if (pointer == 0x800A5254)
            {
                sub_8008BF9C(0x800A19D4, r_u32(0x800FF748));
                sub_8008BF9C(0x800A19F0, r_u32(0x800FF74C));
            }
            index = 0;
            sub_80067808(r_u32(pointer + 4), 0x800A5460);
            w_u8(0x800A5469, 0);
            LoadLevelFromPlayerSave(0x800A545C);
            while ((sint32)index < (sint32)r_u32(0x800FF628))
            {
                if ((sint16)r_u16(r_u32(r_u32(0x800FF624) + index * 4)) == 8)
                {
                    filename = xport_draft_host_sub_8006613C_p1(position, index) + 6;
                    ResetScene(1);
                    InitializeLevelRecords();
                    object = AllocateZeroedObject(0x244);
                    if (object != 0)
                    {
                        resource = AllocateZeroedObject(0x290);
                        if (resource != 0)
                            resource = sub_8005C5C0(resource);
                        object = sub_800772B8(object, resource);
                    }
                    // TODO Original allocation failure path still writes through a zero object
            w_u32(object + 0x234, 3);
                    SelectStartRecord(filename);
                    w_u32(0x800FF2E0, filename);
                    ApplyStartRecord();
                    w_u32(0x800FF81C, 0x800A6558);
                    w_u32(0x800FF818, 2);
                    RunLevelLoop();
                    state = r_u32(0x800FF2EC);
                    w_u32(0x800FF81C, 0);
                    if (state == 5)
                    {
                        w_u32(0x800FF818, 0);
                        w_u32(0x800FF2E0, 0);
                        goto restart;
                    }
                }
                ++index;
            }
            pointer += 0x18;
        }
    }
    if (state != 3)
        goto frame;
    if (r_u8(0x800FF314) == 0)
    {
        w_u32(0x800FF310, 1);
        PlayMovie(0x1C);
        toggle = r_u8(0x800FF314);
        w_u8(0x800FF314, toggle ^ 1);
        goto restart;
    }
    destination = 0x800FFD68;
    source = 0x800A53D4;
    state = r_u32(0x800FF2FC);
    w_u32(0x800FF2FC, 0);
    w_u32(0x800FF2E4, state);
    do
    {
        word0 = r_u32(source);
        word1 = r_u32(source + 4);
        word2 = r_u32(source + 8);
        word3 = r_u32(source + 12);
        w_u32(destination, word0);
        w_u32(destination + 4, word1);
        w_u32(destination + 8, word2);
        w_u32(destination + 12, word3);
        source += 16;
        destination += 16;
    } while (source != 0x800A5454);
    index = r_u8(0x800FF2E9);
    word0 = r_u32(source);
    word1 = r_u32(source + 4);
    w_u32(destination, word0);
    w_u32(destination + 4, word1);
    LoadLevelFromPlayerSave(0x800A6578 + index * 0x8C);
    index = r_u8(0x800FF2E9);
    resource = sub_8006B04C(r_u32(0x800A6600 + index * 0x8C));
    w_u32(0x800FF394, resource);
    resource = AllocateHeapBlock(resource, 1, 1);
    w_u32(0x800FF81C, resource);
    BeginResourceRead(resource);
    WaitForResourceRead();
    resource = r_u32(0x800FF81C);
    toggle = r_u8(0x800FF314);
    w_u32(0x800FF818, 2);
    w_u32(0x800FF820, resource);
    w_u8(0x800FF314, toggle ^ 1);
frame:
    RunLevelLoop();
    resource = r_u32(0x800FF81C);
    if (resource != 0)
    {
        ReleaseHeapBlock(resource);
        w_u32(0x800FF81C, 0);
        w_u32(0x800FF2EC, 4);
    }
    if (r_u32(0x800FF818) == 2)
    {
        destination = 0x800A53D4;
        source = 0x800FFD68;
        state = r_u32(0x800FF2E4);
        w_u32(0x800FF2FC, state);
        do
        {
            word0 = r_u32(source);
            word1 = r_u32(source + 4);
            word2 = r_u32(source + 8);
            word3 = r_u32(source + 12);
            w_u32(destination, word0);
            w_u32(destination + 4, word1);
            w_u32(destination + 8, word2);
            w_u32(destination + 12, word3);
            source += 16;
            destination += 16;
        } while (source != 0x800FFDE8);
        word0 = r_u32(source);
        word1 = r_u32(source + 4);
        w_u32(destination, word0);
        w_u32(destination + 4, word1);
    }
    state = r_u32(0x800FF2EC);
    w_u32(0x800FF818, 0);
    switch (state)
    {
        case 1:
            w_u32(0x800FEF00, 5);
            goto restart;
        case 2:
            count = r_u32(0x800FF874);
            if (count != 0)
            {
                if (r_u32(0x800FF320) == 0)
                {
                    w_u32(0x800FF874, count - 1);
                    count = r_u32(0x800FF874);
                }
                w_u32(0x800A5440, count);
                if (count != 0)
                {
                    ResetScene(1);
                    InitializeLevelRecords();
                    object = AllocateZeroedObject(0x244);
                    if (object != 0)
                    {
                        resource = AllocateZeroedObject(0x290);
                        if (resource != 0)
                            resource = sub_8005C5C0(resource);
                        object = sub_800772B8(object, resource);
                    }
                    // TODO Original allocation failure path still writes through a zero object
            w_u32(object + 0x234, 3);
                    sub_80018B70(0x800A53D4);
                    goto restore;
                }
            }
            count = (uint32)(sint32)(sint16)r_u16(0x800EC522);
            w_u32(0x800FEF00, 20);
            w_u32(0x800FF874, count);
            goto restart;
        case 3:
            ResetScene(0);
            resource = sub_80018948(r_u32(0x800FF83C));
            PlayMovie(r_u8(resource + 0x14));
            if (r_u32(0x800A53D4) == 0)
            {
                refresh = 1;
                goto restart;
            }
            source = 0x800A53D4;
            destination = 0x800A545C;
            do
            {
                word0 = r_u32(source);
                word1 = r_u32(source + 4);
                word2 = r_u32(source + 8);
                word3 = r_u32(source + 12);
                w_u32(destination, word0);
                w_u32(destination + 4, word1);
                w_u32(destination + 8, word2);
                w_u32(destination + 12, word3);
                source += 16;
                destination += 16;
            } while (source != 0x800A5454);
            word0 = r_u32(source);
            word1 = r_u32(source + 4);
            w_u32(destination, word0);
            w_u32(destination + 4, word1);
            goto load_default;
        case 4:
        case 5:
            ResetScene(0);
            index = r_u8(0x800FF2E9) + 1;
            w_u8(0x800FF2E9, index);
            index &= 0xFF;
            if (r_u8(r_u32(0x800A6600 + index * 0x8C)) == 0)
                w_u8(0x800FF2E9, 0);
            goto restart;
        case 6:
            ResetScene(1);
            InitializeLevelRecords();
            object = AllocateZeroedObject(0x244);
            if (object != 0)
            {
                resource = AllocateZeroedObject(0x290);
                if (resource != 0)
                    resource = sub_8005C5C0(resource);
                object = sub_800772B8(object, resource);
            }
            // TODO Original allocation failure path still writes through a zero object
            w_u32(object + 0x234, 3);
            goto restore;
        case 7:
            w_u32(0x800FEF00, 20);
            goto restart;
        default:
            goto restart;
    }
restore:
    ApplyStartRecord();
    ResetPanelDisplayTimer();
    goto frame;
load_default:
    LoadLevelFromPlayerSave(0x800A545C);
    goto frame;
}

extern "C" void sub_80030D74(void)
{
    FUNCTION_MARKER(0x80030D74, "SLUS_003.73");
    RunGameSession();
}


void ResetLevelLoadDisplayState()
{
    FUNCTION_MARKER(0x80016554, "SLUS_003.73");
    w_u32(0x800FF050, 0);
}

extern "C" void sub_80016554(void)
{
    FUNCTION_MARKER(0x80016554, "SLUS_003.73");
    ResetLevelLoadDisplayState();
}


uint32 DrawLevelLoadProgress()
{
    union
    {
        uint32 words[2];
        sint16 values[4];
    } rectangle;
    uint32 result, value, increment, maximum;
    FUNCTION_MARKER(0x80016560, "SLUS_003.73");
    result = r_u32(0x800FF050);
    if (!result)
        return result;
    if (r_u32(0x800FF700))
    {
        w_u32(0x800FF074, 1);
        return 1;
    }
    if (!r_u32(0x800FF074))
        return 0x80100000;
    value = r_u32(0x800FF054);
    increment = r_u32(0x800FF6FC);
    maximum = r_u32(0x800FF058);
    rectangle.words[0] = r_u32(0x800FF078);
    rectangle.words[1] = r_u32(0x800FF07C);
    value += increment;
    w_u32(0x800FF054, value);
    if (maximum && maximum >= value)
    {
        rectangle.values[2] = (sint16)((296 * value) / maximum);
        result = xport_draft_host_sub_8008847C_p1(rectangle.values, 150, 0, 0);
    }
    else
    {
        rectangle.values[2] = 296;
        result = xport_draft_host_sub_8008847C_p1(rectangle.values, 0, 0, 255);
    }
    w_u32(0x800FF074, 0);
    return result;
}

extern "C" uint32 sub_80016560(void)
{
    FUNCTION_MARKER(0x80016560, "SLUS_003.73");
    return DrawLevelLoadProgress();
}
