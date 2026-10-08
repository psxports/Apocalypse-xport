#ifndef GAME_STARTUP_ADAPTERS_H
#define GAME_STARTUP_ADAPTERS_H
#include "draft_first_signatures.h"
uint32 game_startup_read_begin(void *host_destination, uint32 guest_destination);
uint32 game_startup_read_step(void);
#endif
