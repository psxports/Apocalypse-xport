#ifndef GAME_PAD_POLL_H
#define GAME_PAD_POLL_H
#include "psx.h"
#ifdef __cplusplus
uint32 InitializeGamePads();
uint32 SetupGamePadInput();
uint32 LoadGamePadConfiguration();
uint32 ReloadGamePadConfiguration();
uint32 SetGamePadActuator(uint32 controller, uint32 value, uint32 actuator, uint32 enabled);
uint32 UpdateButtonState(uint32 state, uint32 pressed);
extern "C"
{
#endif
uint32 apocalypse_pad_poll(void);
uint32 apocalypse_pad_normalize(void);
void apocalypse_pad_stop(void);
void sub_8009C7EC(void);
#ifdef __cplusplus
}
#endif
#endif
