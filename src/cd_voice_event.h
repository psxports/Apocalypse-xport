#ifndef CD_VOICE_EVENT_H
#define CD_VOICE_EVENT_H

extern "C"
{
#include "psx.h"
}

struct Vector3;

struct CdVoiceState
{
    uint32 base_sector;
    uint32 cooldown;
    bool enabled;
    bool active;
    bool available;
    bool prepared;
    uint32 kind;
    uint32 variant;
    uint32 last_frame;
    uint32 end_sector;
    uint32 cur_sector;
    uint8 response[8];
};

extern CdVoiceState cd_voice;
void ResetCdVoiceState();
void SetCdAudioEnabled(bool enabled);
uint32 UpdateCdVoiceBalance(const Vector3 *pos);
uint32 LocateXaVoiceFile();
uint32 SeekCdAudioEntry(uint32 idx);
uint32 PlayCdVoiceEvent(uint32 kind, uint32 variant, const Vector3 *pos);
uint32 SelectCdVoice(sint32 kind, uint32 variant);

#endif
