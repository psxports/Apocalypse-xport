#ifndef SEQUENCE_TRANSPORT_H
#define SEQUENCE_TRANSPORT_H

extern "C"
{
#include "psx.h"
#include "psx_spu.h"
}

extern "C" void TickMusicSequences(void);
void InitializeMusicSequenceTimer();
void ResetMusicSequencer();
void SetMusicSequenceTickMode(uint32 mode);
void StartMusicSequencer();

sint32 OpenAudioSequence(const uint8 *data, uint32 size, sint16 bank);

uint32 RequestSequenceReplay(uint32 sequence, uint32 track);
uint32 StopAndResetSequence(sint16 sequence);
uint32 PlaySequence(uint32 sequence, uint32 mode, uint32 loops);
uint32 SetSequenceVolumes(uint32 sequence, uint32 left, uint32 right);
void InitializeSequenceCallbacks();
uint32 InitializeSequenceTracks(sint16 sequences, sint16 tracks);

uint32 RequestSequenceStop(uint32 sequence, uint32 track);

uint32 StopSequenceImmediately(uint32 sequence, uint32 track);

#endif
