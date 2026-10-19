#ifndef CARD_SAVE_H
#define CARD_SAVE_H

#include "player_save_state.h"

enum
{
    CardSaveSlotCount = 8,
    CardSaveHeaderSize = 512,
    CardSaveTailSize = 128
};

struct CardSaveImage
{
    uint8 header[CardSaveHeaderSize];
    PlayerSaveRecord slots[CardSaveSlotCount];
    uint8 reserved[CardSaveTailSize];
};

struct CardSaveState
{
    sint32 status;
    uint32 loaded;
    sint32 last_event;
    uint32 phase;
};

extern CardSaveImage card_save_image;
extern CardSaveState card_save_state;
void ResetCardSaveState();
void InitializeCardStorage();

uint32 QueryCardSaveFile(void);
uint32 WriteCardSaveFile(void);
uint32 GetFreeCardBlocks(uint32 port, uint32 slot);
uint32 FormatCard(uint32 port, uint32 slot);
uint32 UpdateCardStatus(uint32 port, uint32 slot);

#endif
