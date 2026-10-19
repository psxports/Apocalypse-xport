#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game_diagnostics.h"
#include "card_save.h"
#include "game_string.h"

static const uint8 card_title[21] = {0x82, 0x60, 0x82, 0x6F, 0x82, 0x6E, 0x82, 0x62, 0x82, 0x60, 0x82, 0x6B, 0x82, 0x78, 0x82, 0x6F, 0x82, 0x72, 0x82, 0x64, 0x00};

static const uint8 card_palette[32] = {0x00, 0x80, 0x6D, 0x98, 0xD6, 0xAC, 0x25, 0x88, 0x59, 0xBD, 0x91, 0xA0, 0x36, 0xB5, 0x4A, 0x90, 0xB3, 0xA8, 0x02, 0x80, 0xD9, 0xB0, 0xFC, 0xBC, 0xF1, 0xA4, 0x48, 0x8C, 0x6F, 0x98, 0x00, 0x80};

static const uint8 card_icon[128] = {0x00, 0x00, 0x00, 0x20, 0x1B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x19, 0xB5, 0x2B, 0x31, 0x00, 0x00, 0x00, 0x30, 0xAA, 0xAB, 0x48, 0x2A, 0x01, 0x00, 0x00, 0x23, 0x17, 0x3B, 0x4D, 0xC3, 0x5C, 0x00, 0x30, 0xD2, 0x89, 0x0C, 0x69, 0x0D, 0x59, 0xDE, 0xC0, 0x0C, 0x63, 0x0D, 0xC0, 0x75, 0x8E, 0x86, 0x40, 0x07, 0x61, 0x90, 0x61, 0x24, 0x55, 0x98, 0x49, 0x0D, 0x22, 0xA5, 0x56, 0x36, 0x90, 0x08, 0xB3, 0x7D, 0x46, 0xD8, 0x90, 0x76, 0x00, 0x0C, 0xB7, 0xAA, 0x7A, 0x00, 0x90, 0x52, 0x90, 0x05, 0x44, 0x88, 0x08, 0x00, 0x30, 0x28, 0x70, 0x0D, 0xD0, 0xA6, 0x01, 0x00, 0xD0, 0x48, 0x56, 0x03, 0x00, 0xB1, 0x03, 0x00, 0x30, 0xAA, 0x38, 0x00, 0x00, 0x27, 0xD8, 0x09, 0xD9, 0xBA, 0x01, 0x00, 0x00, 0x78, 0x73, 0x25, 0xA2, 0xAE, 0x05, 0x00, 0x30, 0x08, 0x00, 0x00, 0x00, 0x10, 0x9A, 0x00};

CardSaveImage card_save_image;
CardSaveState card_save_state;

void ResetCardSaveState()
{
    memset(&card_save_image, 0, sizeof(card_save_image));
    card_save_state = CardSaveState();
}

static const sint32 NativeSaveBytes = CardSaveSlotCount * PlayerSaveSize;

static sint32 ProbeNativeSaveStorage()
{
    uint8 slots[NativeSaveBytes];
    sint32 size = xport_load(slots, NativeSaveBytes);
    return size == 0 || size == NativeSaveBytes ? 1 : -1;
}

static uint32 InitializeCardSaveHeader(void)
{
    FUNCTION_MARKER(0x800102C0, "SLUS_003.73");
    uint8 *header = card_save_image.header;
    header[0] = 83;
    header[1] = 67;
    header[2] = 17;
    header[3] = 1;
    for (sint32 index = 63; index >= 0; --index)
        header[4 + index] = 0;
    CopyGameString((const char *)card_title, (char *)header + 4);
    for (sint32 index = 27; index >= 0; --index)
        header[68 + index] = 0;
    for (uint32 index = 0; index < 32; ++index)
        header[96 + index] = card_palette[index];
    for (uint32 index = 0; index < 128; ++index)
    {
        uint8 value = card_icon[index];
        header[128 + index] = value;
        if (header[2] >= 18)
            header[256 + index] = value;
        if (header[2] >= 19)
            header[384 + index] = value;
    }
    return 0;
}

uint32 QueryCardSaveFile(void)
{
    FUNCTION_MARKER(0x80010530, "SLUS_003.73");
    InitializeCardSaveHeader();
    uint8 data[NativeSaveBytes];
    sint32 size = xport_load(data, sizeof(data));
    if (size == NativeSaveBytes)
    {
        for (uint32 index = 0; index < CardSaveSlotCount; ++index)
            DecodePlayerSave(card_save_image.slots[index], data + index * PlayerSaveSize, PlayerSaveSize);
        return 0;
    }
    memset(card_save_image.slots, 0, sizeof(card_save_image.slots));
    if (size != 0)
        card_save_state.status = -1;
    return 1;
}

void InitializeCardStorage()
{
    FUNCTION_MARKER(0x80010610, "SLUS_003.73");
    card_save_state.phase = 0;
    card_save_state.loaded = 0;
}

uint32 UpdateCardStatus(uint32 port, uint32 slot)
{
    FUNCTION_MARKER(0x80010AC8, "SLUS_003.73");
    sint32 status = port == 0 && slot == 0 ? ProbeNativeSaveStorage() : -1;
    card_save_state.phase = 0;
    card_save_state.loaded = status == 1 ? 1 : 0;
    card_save_state.last_event = status;
    card_save_state.status = status;
    return (uint32)status;
}

uint32 WriteCardSaveFile(void)
{
    FUNCTION_MARKER(0x800103E4, "SLUS_003.73");
    uint8 data[NativeSaveBytes];
    for (uint32 index = 0; index < CardSaveSlotCount; ++index)
        EncodePlayerSave(data + index * PlayerSaveSize, PlayerSaveSize, card_save_image.slots[index]);
    sint32 saved = xport_save(data, sizeof(data));
    card_save_state.status = saved == 1 ? 1 : -1;
    return saved == 1 ? 0 : 1;
}

uint32 GetFreeCardBlocks(uint32 port, uint32 slot)
{
    FUNCTION_MARKER(0x80010A38, "SLUS_003.73");
    // Native slots share one host file and require no emulated card directory
    return port == 0 && slot == 0 && ProbeNativeSaveStorage() == 1 ? 1 : 0;
}

uint32 FormatCard(uint32 port, uint32 slot)
{
    FUNCTION_MARKER(0x80010CBC, "SLUS_003.73");
    if (port != 0 || slot != 0)
        return (uint32)-3;
    uint8 slots[NativeSaveBytes] = {0};
    return xport_save(slots, NativeSaveBytes) == 1 ? 1 : (uint32)-3;
}
