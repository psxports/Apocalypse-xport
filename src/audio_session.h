#ifndef AUDIO_SESSION_H
#define AUDIO_SESSION_H

extern "C"
{
#include "psx.h"
}

uint32 InitializeAudioOutput();
uint32 InitializeAudioSession(const char *resource);
uint32 ReleaseAudioSession();

uint32 ReleaseSoundBank(sint16 bank, uint32 header);
uint32 LoadSoundBank(const char *name, uint32 header_output);

uint32 LoadLevelSoundBank(const char *basename);
void ReleaseLevelSoundBank();
uint32 LoadAudioSequence(const char *basename);
uint32 SwitchMenuAudioSequence(const char *basename);

uint32 PlaySoundEffect(sint32 sound_id, sint16 volume, uint32 pitch_variation);
uint32 PlayScaledSound(sint32 sound_id, sint16 left, sint16 right, uint32 pitch_variation);
uint32 PlayPositionedSound(sint32 sound_id, const uint32 *source, uint32 pitch_variation);

uint32 FlushPendingPositionedSound(void);

uint32 CalculateSoundBalance(const uint32 *origin, sint32 minimum, sint32 maximum);

uint32 SetSoundReverbMode(uint8 mode);
uint32 EnableSoundReverb();
uint32 DisableSoundReverb();

uint32 MarkMusicRestorePending();
uint32 ResetMusicPlaybackState();
uint32 StopMusicPlayback();
uint32 RestoreMusicPlayback();
uint32 SetSoundReverbDepth(uint8 left, uint8 right);

uint32 SuspendAudioPlayback();
uint32 ResumeAudioPlayback();

#endif
