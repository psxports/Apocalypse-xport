#ifndef AUDIO_VOICE_H
#define AUDIO_VOICE_H

extern "C"
{
#include "psx.h"
}

struct Vector3;

void ResetSoundVoicePitches();

uint32 PlaySoundVoices(uint32 cfg, uint8 note, sint16 left, sint16 right, uint32 pitch_variation);

void StopSoundVoices(uint32 mask);

void UpdatePositionedSoundVoices(uint32 voices, const Vector3 *origin, sint32 pitch_delta);

#endif
