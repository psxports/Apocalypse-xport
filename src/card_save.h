#ifndef CARD_SAVE_H
#define CARD_SAVE_H

#include "psx.h"

uint32 InitializeCardSaveHeader(void);
uint32 QueryCardSaveFile(void);
uint32 WriteCardSaveFile(void);
uint32 GetFreeCardBlocks(uint32 port, uint32 slot);
uint32 FormatCard(uint32 port, uint32 slot);
uint32 DrainCardEvents(void);
uint32 PollCardEvents(void);
uint32 WaitForCardHardwareEvent(void);
uint32 DrainCardHardwareEvents(void);
uint32 UpdateCardStatus(uint32 port, uint32 slot);

#endif
