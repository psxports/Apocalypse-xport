#ifndef GAME_SPU_STARTUP_H
#define GAME_SPU_STARTUP_H
#include "draft_first_signatures.h"
void apocalypse_spu_bind_reverb_presets(void);
uint32 apocalypse_spu_init(uint32 mode);
uint32 apocalypse_spu_clear_reverb(uint32 mode);
uint32 apocalypse_ss_init(void);
uint32 apocalypse_ss_serial_attr(uint32 serial, uint32 attribute, uint32 value);
uint32 apocalypse_ss_serial_volume(uint32 serial, uint32 left, uint32 right);
void apocalypse_ss_flush(void);
#endif
