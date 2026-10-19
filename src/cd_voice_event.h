#ifndef CD_VOICE_EVENT_H
#define CD_VOICE_EVENT_H

extern "C"
{
#include "psx.h"
}

struct CdVoiceState
{
    uint32 base_sector;
    uint32 cooldown;
    uint32 enabled;
    uint32 active;
    uint32 available;
    uint32 prepared;
    uint32 kind;
    uint32 variant;
    uint32 last_frame;
    uint32 end_sector;
    uint32 current_sector;
    uint8 response[8];
};

extern CdVoiceState cd_voice;
void ResetCdVoiceState();
void SetCdAudioEnabled(uint32 enabled);
uint32 SeekCdAudioEntry(uint32 index);

uint32 LocateXaVoiceFile();
uint32 SelectCdVoice(uint32 kind, uint32 variant);
uint32 UpdateCdVoiceBalance(const uint32 *position);
uint32 PlayCdVoiceEvent(uint32 kind, uint32 variant, const uint32 *position, uint32 mode);

#endif
