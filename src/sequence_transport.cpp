#include <stdlib.h>
#include <string.h>

#include "sequence_transport.h"
#include "game_diagnostics.h"

static SpuSequenceSchedulerTrack *track_storage;

static void RequireSequenceTrack(sint16 sequence, sint16 track)
{
    if (sequence < 0 || sequence >= spu_sequence_scheduler.sequence_count || track < 0 || track >= spu_sequence_scheduler.separation_count || !spu_sequence_scheduler.tracks[sequence])
    {
        GameDiagnostic(("Sequence track outside its native table\n"));
        abort();
    }
}

static uint32 ReadSequenceDelta(sint16 sequence, sint16 track)
{
    RequireSequenceTrack(sequence, track);
    return spu_sequence_scheduler_read_delta(&spu_sequence_scheduler, sequence, track);
}

static uint32 StopSequenceVoices(sint16 handle)
{
    FUNCTION_MARKER(0x80092EA4, "SLUS_003.73");
    return spu_sequence_vm_sequence_key_off(handle);
}

static uint32 SaveSequenceVolume(sint16 handle, uint16 *left, uint16 *right)
{
    FUNCTION_MARKER(0x80092DB0, "SLUS_003.73");
    RequireSequenceTrack((uint16)handle & 255, (uint16)handle >> 8);
    return spu_sequence_vm_get_sequence_volume(&spu_sequence_scheduler, handle, left, right);
}

static uint32 SetSequenceVolumeCallback(sint16 handle, sint16 left, sint16 right, sint16 mode)
{
    RequireSequenceTrack((uint16)handle & 255, (uint16)handle >> 8);
    return spu_sequence_vm_set_sequence_volume(&spu_sequence_scheduler, handle, left, right, mode);
}

static uint32 SetSequenceVoiceVolumes(sint16 handle, sint16 left, sint16 right)
{
    FUNCTION_MARKER(0x8009282C, "SLUS_003.73");
    return SetSequenceVolumeCallback(handle, left, right, 1);
}

static uint32 SetSequenceChannelVoices(sint16 handle, sint16 bank, sint16 program, uint16 volume, uint16 pan)
{
    FUNCTION_MARKER(0x8009302C, "SLUS_003.73");
    return spu_sequence_vm_set_volume(&spu_sequence_scheduler, handle, bank, program, volume, pan);
}

static uint32 StartSequenceNote(sint16 handle, sint16 bank, sint16 program, uint16 note, uint16 velocity, uint16 pan)
{
    return spu_sequence_vm_key_on(&spu_sequence_scheduler, handle, bank, program, note, velocity, pan);
}

static uint32 SequenceNote(sint16 sequence, sint16 track, uint8 note, uint8 velocity)
{
    FUNCTION_MARKER(0x8008CDFC, "SLUS_003.73");
    return spu_sequence_scheduler_note_on(&spu_sequence_scheduler, sequence, track, note, velocity, StartSequenceNote, spu_sequence_vm_key_off);
}

static uint32 SequenceProgram(sint16 sequence, sint16 track, uint8 program)
{
    FUNCTION_MARKER(0x8008CEDC, "SLUS_003.73");
    return spu_sequence_scheduler_program_change(&spu_sequence_scheduler, sequence, track, program, ReadSequenceDelta);
}

static uint32 SequenceChannelVolume(sint16 sequence, sint16 track, uint8 volume)
{
    FUNCTION_MARKER(0x8008C8CC, "SLUS_003.73");
    return spu_sequence_scheduler_volume(&spu_sequence_scheduler, sequence, track, volume, SetSequenceChannelVoices, ReadSequenceDelta);
}

static uint32 SequenceChannelPan(sint16 sequence, sint16 track, uint8 pan)
{
    FUNCTION_MARKER(0x8008C99C, "SLUS_003.73");
    return spu_sequence_scheduler_pan(&spu_sequence_scheduler, sequence, track, pan, SetSequenceChannelVoices, ReadSequenceDelta);
}

static uint32 SequenceDamper(sint16 sequence, sint16 track, uint8 val)
{
    FUNCTION_MARKER(0x8008CA6C, "SLUS_003.73");
    return spu_sequence_scheduler_damper(&spu_sequence_scheduler, sequence, track, val, ReadSequenceDelta);
}

static uint32 SequencePitchBend(sint16 sequence, sint16 track)
{
    FUNCTION_MARKER(0x8008CB1C, "SLUS_003.73");
    return spu_sequence_scheduler_pitch_bend(&spu_sequence_scheduler, sequence, track, spu_sequence_vm_sequence_bend, ReadSequenceDelta);
}

static uint32 SequenceMeta(sint16 sequence, sint16 track)
{
    return spu_sequence_scheduler_meta_event(&spu_sequence_scheduler, sequence, track, ReadSequenceDelta);
}

static uint32 SequenceControl(sint16 sequence, sint16 track, uint8 control)
{
    FUNCTION_MARKER(0x8008CBCC, "SLUS_003.73");
    return spu_sequence_scheduler_control_change(&spu_sequence_scheduler, sequence, track, control, &spu_sequence_handlers, ReadSequenceDelta);
}

static uint32 SequenceDataEntry(sint16 sequence, sint16 track, uint8 val)
{
    SpuSequenceSchedulerTrack *rec = spu_sequence_scheduler.tracks[sequence] + track;
    return spu_sequence_scheduler_data_entry(rec, sequence, track, val, spu_tone_commands, 20, ReadSequenceDelta);
}

static uint32 EnableSequenceReverb(uint32 enabled)
{
    return SpuSetReverb(enabled);
}

static uint32 SequenceResetControllers(sint16 sequence, sint16 track)
{
    return spu_sequence_scheduler_reset_controllers(&spu_sequence_scheduler, sequence, track, EnableSequenceReverb, ReadSequenceDelta);
}

static uint32 SequenceNrpnLsb(sint16 sequence, sint16 track, uint8 val)
{
    return spu_sequence_scheduler_nrpn_lsb(&spu_sequence_scheduler, sequence, track, val, spu_sequence_dispatch_mark, ReadSequenceDelta);
}

static uint32 SequenceNrpnMsb(sint16 sequence, sint16 track, uint8 val)
{
    return spu_sequence_scheduler_nrpn_msb(&spu_sequence_scheduler, sequence, track, val, ReadSequenceDelta);
}

static uint32 SequenceRpnLsb(sint16 sequence, sint16 track, uint8 val)
{
    return spu_sequence_scheduler_rpn_lsb(&spu_sequence_scheduler, sequence, track, val, ReadSequenceDelta);
}

static uint32 SequenceRpnMsb(sint16 sequence, sint16 track, uint8 val)
{
    return spu_sequence_scheduler_rpn_msb(&spu_sequence_scheduler, sequence, track, val, ReadSequenceDelta);
}

static uint32 SequenceReverbDepth(sint16 sequence, sint16 track, uint8 val)
{
    return spu_sequence_scheduler_reverb_depth(&spu_sequence_scheduler, sequence, track, val, spu_sequence_vm_reverb_depth, ReadSequenceDelta);
}

template <uint8 attribute>
static uint32 SetSequenceToneAttribute(sint16 bank, sint16 program, sint16 tone, VagAtr attributes, uint8 val)
{
    return spu_vab_nrpn_attr_by_value(attribute, bank, program, tone, attributes, val);
}

static uint32 SetSequenceReverbType(uint8 val)
{
    return spu_sequence_vm_reverb_type(val);
}

static uint32 SetSequenceReverbDepth(uint8 val)
{
    return spu_sequence_vm_reverb_depth(val, val);
}

static const SpuSequenceHandlers default_handlers = {SequenceNote, SequenceProgram, SequencePitchBend, SequenceMeta, SequenceControl, SequenceDataEntry, SequenceChannelVolume, SequenceChannelPan, SequenceDamper, SequenceNrpnLsb, SequenceNrpnMsb, SequenceRpnLsb, SequenceRpnMsb, SequenceReverbDepth, SequenceResetControllers};

static const SpuToneCommand default_commands[20] = {{SetSequenceToneAttribute<0>, 0}, {SetSequenceToneAttribute<1>, 0}, {SetSequenceToneAttribute<2>, 0}, {SetSequenceToneAttribute<3>, 0}, {SetSequenceToneAttribute<4>, 0}, {SetSequenceToneAttribute<5>, 0}, {SetSequenceToneAttribute<6>, 0}, {SetSequenceToneAttribute<7>, 0}, {SetSequenceToneAttribute<8>, 0}, {SetSequenceToneAttribute<9>, 0}, {SetSequenceToneAttribute<10>, 0}, {SetSequenceToneAttribute<11>, 0}, {SetSequenceToneAttribute<12>, 0}, {SetSequenceToneAttribute<13>, 0}, {SetSequenceToneAttribute<14>, 0}, {0, SetSequenceReverbType}, {0, SetSequenceReverbDepth}, {0, spu_sequence_vm_reverb_feedback}, {0, spu_sequence_vm_reverb_delay}, {0, spu_sequence_vm_reverb_delay}};

void InitializeSequenceCallbacks()
{
    spu_sequence_handlers = default_handlers;
    memcpy(spu_tone_commands, default_commands, sizeof(default_commands));
}

static uint32 StartChainedSequence(sint16 sequence, sint16 track)
{
    FUNCTION_MARKER(0x8008DC3C, "SLUS_003.73");
    RequireSequenceTrack(sequence, track);
    return spu_sequence_scheduler_next_separation(&spu_sequence_scheduler, sequence, track);
}

static uint32 SequenceEnd(sint16 sequence, sint16 track)
{
    FUNCTION_MARKER(0x8008D648, "SLUS_003.73");
    return spu_sequence_scheduler_end_of_track(&spu_sequence_scheduler, sequence, track, StartChainedSequence, StopSequenceVoices);
}

static uint32 SequenceEvent(sint16 sequence, sint16 track)
{
    FUNCTION_MARKER(0x8008D88C, "SLUS_003.73");
    return spu_sequence_scheduler_read_event(&spu_sequence_scheduler, sequence, track, &spu_sequence_handlers, ReadSequenceDelta, SequenceEnd);
}

static uint32 TickSequenceTrack(sint16 sequence, sint16 track)
{
    FUNCTION_MARKER(0x8008D54C, "SLUS_003.73");
    return spu_sequence_scheduler_play_tick(&spu_sequence_scheduler, sequence, track, SequenceEvent);
}

static uint32 TickSequenceVolume(sint16 sequence, sint16 track)
{
    FUNCTION_MARKER(0x8008D26C, "SLUS_003.73");
    return spu_sequence_scheduler_crescendo(&spu_sequence_scheduler, sequence, track, SaveSequenceVolume, SetSequenceVolumeCallback);
}

static uint32 TickSequenceVolumeDown(sint16 sequence, sint16 track)
{
    return spu_sequence_scheduler_decrescendo(&spu_sequence_scheduler, sequence, track, SaveSequenceVolume, SetSequenceVolumeCallback);
}

static void TickSequenceTempo(sint16 sequence, sint16 track)
{
    FUNCTION_MARKER(0x8008F69C, "SLUS_003.73");
    spu_sequence_scheduler_tempo_ramp(&spu_sequence_scheduler, sequence, track);
}

static uint32 StopSequenceTick(sint16 sequence, sint16 track)
{
    FUNCTION_MARKER(0x8008D47C, "SLUS_003.73");
    return spu_sequence_scheduler_pause(&spu_sequence_scheduler, sequence, track, StopSequenceVoices);
}

static void ReplaySequenceTick(sint16 sequence, sint16 track)
{
    FUNCTION_MARKER(0x8008DD3C, "SLUS_003.73");
    spu_sequence_scheduler_resume(&spu_sequence_scheduler, sequence, track);
}

static uint32 StopSequenceTrack(sint16 sequence, sint16 track)
{
    FUNCTION_MARKER(0x8008EE7C, "SLUS_003.73");
    return spu_sequence_scheduler_stop(&spu_sequence_scheduler, sequence, track, StopSequenceVoices);
}

static const SpuSequencePumpOps pump_ops = {spu_sequence_vm_flush, TickSequenceTrack, TickSequenceVolume, TickSequenceVolumeDown, TickSequenceTempo, StopSequenceTick, ReplaySequenceTick, StopSequenceTrack};

extern "C" void TickMusicSequences(void)
{
    FUNCTION_MARKER(0x8008CFFC, "SLUS_003.73");
    if (track_storage)
        spu_sequence_scheduler_pump(&spu_sequence_scheduler, &pump_ops);
}

static void ReleaseSequenceTracks()
{
    for (uint32 sequence = 0; sequence < spu_sequence_scheduler.sequence_count; ++sequence)
    {
        SpuSequenceSchedulerTrack *tracks = spu_sequence_scheduler.tracks[sequence];
        if (tracks)
        {
            for (uint32 track = 0; track < spu_sequence_scheduler.separation_count; ++track)
            {
                free((void *)tracks[track].stream.data);
                spu_sequence_stream_clear(&tracks[track].stream);
            }
        }
        spu_sequence_scheduler.tracks[sequence] = 0;
    }
    free(track_storage);
    track_storage = 0;
    spu_sequence_scheduler.sequence_count = 0;
    spu_sequence_scheduler.separation_count = 0;
    spu_sequence_scheduler.used_mask = 0;
}

void ResetMusicSequencer()
{
    spu_sequence_scheduler.processing = 1;
    spu_sequence_scheduler_stop_timer(spu_sequence_clock());
    ReleaseSequenceTracks();
    spu_sequence_scheduler_initialize(&spu_sequence_scheduler, spu_sequence_vm_initialize);
    SpuSequenceClock clock = {0, 1, TickMusicSequences, 0, 0, 0, 127, 0};
    *spu_sequence_clock() = clock;
    InitializeSequenceCallbacks();
}

uint32 InitializeSequenceTracks(sint16 sequences, sint16 tracks)
{
    FUNCTION_MARKER(0x8008F16C, "SLUS_003.73");
    if (sequences <= 0 || sequences > 32 || tracks <= 0 || tracks > 16)
    {
        GameDiagnostic(("Invalid native sequence table dimensions\n"));
        abort();
    }
    spu_sequence_scheduler.processing = 1;
    for (sint16 sequence = 0; sequence < (sint16)spu_sequence_scheduler.sequence_count; ++sequence)
        spu_sequence_scheduler_close(&spu_sequence_scheduler, sequence, SetSequenceVolumeCallback, StopSequenceVoices);
    ReleaseSequenceTracks();
    track_storage = (SpuSequenceSchedulerTrack *)calloc((uint32)sequences * tracks, sizeof(*track_storage));
    if (!track_storage)
    {
        GameDiagnostic(("Cannot allocate native sequence tracks\n"));
        abort();
    }
    uint32 result = spu_sequence_scheduler_set_table(&spu_sequence_scheduler, track_storage, sequences, tracks);
    spu_sequence_scheduler.processing = 0;
    return result;
}

static uint32 SequenceInitialize(sint16 sequence, sint16 bank, const uint8 *data)
{
    FUNCTION_MARKER(0x8008E21C, "SLUS_003.73");
    return spu_sequence_scheduler_initialize_track(&spu_sequence_scheduler, sequence, bank, data, ReadSequenceDelta);
}

sint32 OpenAudioSequence(const uint8 *data, uint32 size, sint16 bank)
{
    FUNCTION_MARKER(0x8008E14C, "SLUS_003.73");
    uint32 used = spu_sequence_scheduler.used_mask;
    sint16 result = spu_sequence_scheduler_open(&spu_sequence_scheduler, data, size, bank, SequenceInitialize, &spu_sequence_handlers, &default_handlers, spu_tone_commands, default_commands);
    if (result < 0)
    {
        uint32 opened = spu_sequence_scheduler.used_mask & ~used;
        for (uint32 slot = 0; slot < spu_sequence_scheduler.sequence_count; ++slot)
            if (opened & ((uint32)1 << slot))
                spu_sequence_scheduler_close(&spu_sequence_scheduler, slot, SetSequenceVolumeCallback, StopSequenceVoices);
    }
    return result;
}

uint32 PlaySequence(sint16 sequence, uint8 mode, sint16 loops)
{
    RequireSequenceTrack(sequence, 0);
    SpuSequenceSchedulerTrack *track = spu_sequence_scheduler.tracks[sequence];
    track->stream.cursor = track->stream.start;
    track->stream.loop_start = track->stream.start;
    track->stream.replay = track->stream.start;
    return spu_sequence_scheduler_play_mode(&spu_sequence_scheduler, sequence, 0, mode, loops, SetSequenceVolumeCallback);
}

uint32 SetSequenceVolumes(sint16 sequence, sint16 left, sint16 right)
{
    return SetSequenceVoiceVolumes(sequence, left, right);
}

uint32 StopAndResetSequence(sint16 sequence)
{
    FUNCTION_MARKER(0x8008DD9C, "SLUS_003.73");
    RequireSequenceTrack(sequence, 0);
    return spu_sequence_scheduler_close(&spu_sequence_scheduler, sequence, SetSequenceVolumeCallback, StopSequenceVoices);
}

uint32 RequestSequenceStop(uint32 sequence, uint32 track)
{
    FUNCTION_MARKER(0x8008E56C, "SLUS_003.73");
    RequireSequenceTrack((sint16)sequence, (sint16)track);
    return spu_sequence_scheduler_request_pause(&spu_sequence_scheduler, sequence, track, SaveSequenceVolume);
}

uint32 RequestSequenceReplay(sint16 sequence, sint16 track)
{
    FUNCTION_MARKER(0x8008E72C, "SLUS_003.73");
    RequireSequenceTrack(sequence, track);
    return spu_sequence_scheduler_replay_mode(&spu_sequence_scheduler, sequence, track);
}

uint32 StopSequenceImmediately(uint32 sequence, uint32 track)
{
    RequireSequenceTrack((sint16)sequence, (sint16)track);
    return StopSequenceTrack((sint16)sequence, (sint16)track);
}

static void MusicSequenceIrq0()
{
    spu_sequence_scheduler_irq0(spu_sequence_clock());
}

static void MusicSequenceHalfRate()
{
    spu_sequence_scheduler_half_rate(spu_sequence_clock());
}

void InitializeMusicSequenceTimer()
{
    spu_sequence_clock()->tick = TickMusicSequences;
}

void SetMusicSequenceTickMode(uint32 mode)
{
    spu_sequence_scheduler_tick_mode(&spu_sequence_scheduler, spu_sequence_clock(), mode, GetVideoMode());
}

void StartMusicSequencer()
{
    FUNCTION_MARKER(0x8008EC4C, "SLUS_003.73");
    spu_sequence_scheduler_start_timer(spu_sequence_clock(), 0, MusicSequenceIrq0, MusicSequenceHalfRate);
}
