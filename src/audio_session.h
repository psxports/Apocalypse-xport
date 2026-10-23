#ifndef AUDIO_SESSION_H
#define AUDIO_SESSION_H

extern "C"
{
#include "psx.h"
}

#include "vector3.h"

struct SoundBankHandles
{
    sint16 common;
    sint16 level;
    sint16 music;
};

extern SoundBankHandles sound_banks;
void ResetSoundBankHandles();

struct PositionedSoundEvent
{
    bool pending;
    Vector3 pos;
};

extern PositionedSoundEvent positioned_sound_event;
void ResetSoundEvents();

struct AudioPlaybackState
{
    sint16 saved_left[24];
    sint16 saved_right[24];
    sint16 sequence;
    bool suspended;
    bool resume_pending;
    bool start_pending;
    uint32 tempo;
    uint32 reverb_ticks;
    bool restore_pending;
};

extern AudioPlaybackState audio_playback;
void ResetAudioPlayback();

uint32 InitializeAudioOutput();
uint32 InitializeAudioSession(const char *resource);
uint32 ReleaseAudioSession();

sint16 LoadLevelSoundBank(const char *basename);
void ReleaseLevelSoundBank();
uint32 LoadAudioSequence(const char *basename);
uint32 SwitchMenuAudioSequence(const char *basename);

uint32 PlaySoundEffect(sint32 sound_id, sint16 volume, uint32 pitch_variation);
uint32 PlayScaledSound(sint32 sound_id, sint16 left, sint16 right, uint32 pitch_variation);
uint32 PlayPositionedSound(sint32 sound_id, const Vector3 *src, uint32 pitch_variation);

uint32 FlushPendingPositionedSound(void);

uint32 CalculateSoundBalance(const Vector3 *origin, sint32 minimum, sint32 maximum);

uint32 SetSoundReverbMode(uint8 mode);
uint32 EnableSoundReverb();
uint32 DisableSoundReverb();

uint32 MarkMusicRestorePending();
uint32 ResetMusicPlaybackState();
uint32 StopMusicPlayback();
uint32 StopMusicSequence();
uint32 RestoreMusicPlayback();
uint32 SetSoundReverbDepth(uint8 left, uint8 right);

uint32 SuspendAudioPlayback();
uint32 ResumeAudioPlayback();

#endif
