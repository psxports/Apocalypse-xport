#ifndef CD_VOICE_EVENT_H
#define CD_VOICE_EVENT_H

extern "C"
{
#include "psx.h"
}

uint32 LocateXaVoiceFile();
uint32 PrepareCdVoice(void);
uint32 SelectCdVoice(uint32 kind, uint32 variant);
uint32 UpdateCdVoiceBalance(const uint32 *position);
uint32 PlayCdVoiceEvent(uint32 kind, uint32 variant, const uint32 *position, uint32 mode);

#endif
