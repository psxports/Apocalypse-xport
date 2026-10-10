#ifndef APOCALYPSE_SOUND_NATIVE_H
#define APOCALYPSE_SOUND_NATIVE_H
#include "psx.h"

/* Unverified project adapters for the observed menu sound path */
sint32 apocalypse_sound_select_bank(uint32 bank, uint32 program);
sint32 apocalypse_sound_actual_program(uint32 bank, uint32 program);
sint32 apocalypse_sound_tone_attributes(uint32 bank, uint32 program, uint32 tone, void *output);
uint32 apocalypse_sound_voice_settings(uint32 voice, const void *settings);
uint32 apocalypse_sound_pitch(uint32 note, uint32 fine, uint32 center, uint32 shift);
sint32 apocalypse_sound_vag_address(uint32 bank, uint32 vag);
void apocalypse_sound_queue_registers(uint32 voice, const sint16 registers[8]);
void apocalypse_sound_queue_key_on(uint32 mask);
void apocalypse_sound_queue_reverb(uint32 voices, uint32 enabled);

#endif
