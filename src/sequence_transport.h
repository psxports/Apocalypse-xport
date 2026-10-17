#ifndef SEQUENCE_TRANSPORT_H
#define SEQUENCE_TRANSPORT_H

extern "C"
{
#include "psx.h"
}

uint32 StopSequenceVoices(uint32 identity);
uint32 SetSequenceVoiceVolumes(uint32 sequence, uint32 left, uint32 right);
uint32 SaveSequenceVolume(uint32 identity, uint32 left, uint32 right);
uint32 StartChainedSequence(uint32 sequence, uint32 track);

uint32 StopSequenceImmediately(uint32 sequence, uint32 track);

#endif
