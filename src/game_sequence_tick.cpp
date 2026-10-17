#include "game_sdk_bindings.h"
extern "C"
{
#include "psx.h"
#include "game_sound_native.h"
}
#include "sequence_transport.h"
#include <stdio.h>
#include <stdlib.h>

extern "C" uint32 apocalypse_sequence_delta(uint32 record);

// Original 8009207C uses this image's contiguous key/reverb/noise mask layout
static uint32 SequenceQueueVoice(uint16 pitch)
{
    uint32 volume, left, right, header, identity, record, voice, offset, pan, index;
    uint32 low_mask = 0, high_mask = 0, low, high, key_low, key_high, result;
    header = r_u32(0x80105614);
    volume = (uint32)(sint32)(sint8)r_u8(0x80105624) * ((uint32)r_u8(header + 24) * 0x3FFF);
    volume = (uint32)((sint32)volume / 16129);
    volume *= (uint32)(sint32)(sint8)r_u8(0x8010562A);
    volume *= (uint32)(sint32)(sint8)r_u8(0x8010562D);
    volume /= 16129;
    voice = (uint32)(sint32)(sint16)r_u16(0x80105638);
    offset = 2 * (uint16)(voice * 8);
    identity = r_u16(0x80105634);
    record = r_u32(0x80104590 + (identity & 255) * 4) + 176 * ((identity & 0xFF00) >> 8);
    left = right = volume;
    if ((sint16)identity != 33)
    {
        left = (left * r_u16(record + 88)) / 127;
        right = (right * r_u16(record + 90)) / 127;
    }
    for (index = 0; index < 3; ++index)
    {
        pan = r_u8(index == 0 ? 0x8010562E : index == 1 ? 0x8010562B : 0x80105625);
        if (pan < 64)
            right = (right * pan) / 63;
        else
            left = (left * (127 - pan)) / 63;
    }
    if ((sint16)r_u16(0x80105608) == 1)
    {
        if (left < right)
            left = right;
        else
            right = left;
    }
    if ((sint16)r_u16(0x80105634) != 33)
    {
        left = (left * left) / 0x3FFF;
        right = (right * right) / 0x3FFF;
    }
    w_u16(0x8010535C + offset, pitch);
    w_u16(0x80105358 + offset, left);
    w_u16(0x8010535A + offset, right);
    voice = (uint32)(sint32)(sint16)r_u16(0x80105638);
    w_u8(0x80105338 + voice, r_u8(0x80105338 + voice) | 7);
    voice = (uint32)(sint32)(sint16)r_u16(0x80105638);
    w_u16(0x80104E2C + voice * 54, pitch);
    voice = (uint32)(sint32)(sint16)r_u16(0x80105638);
    if ((sint32)voice < 16)
        low_mask = 1u << (voice & 31);
    else
        high_mask = 1u << ((voice - 16) & 31);
    if (r_u8(0x80105632) & 4)
    {
        low = r_u16(0x80104E1C);
        high = r_u16(0x80104E1E);
        w_u16(0x80104E1C, low | low_mask);
        w_u16(0x80104E1E, high | high_mask);
    }
    else
    {
        low = r_u16(0x80104E1C);
        w_u16(0x80104E1C, low & ~low_mask);
        high = r_u16(0x80104E1E);
        w_u16(0x80104E1E, high & ~high_mask);
    }
    low = r_u16(0x80104E20);
    key_high = r_u16(0x80104E1A);
    w_u16(0x80104E20, low & ~low_mask);
    high = r_u16(0x80104E22) & ~high_mask;
    key_low = r_u16(0x80104E18);
    key_high |= high_mask;
    w_u16(0x80104E1A, key_high);
    w_u16(0x80104E22, high);
    low = r_u16(0x80105520);
    key_low |= low_mask;
    w_u16(0x80104E18, key_low);
    w_u16(0x80105520, low & ~key_low);
    result = r_u16(0x80105522) & ~key_high;
    w_u16(0x80105522, result);
    return result;
}

static uint32 SequenceVoiceMissing(uint32 address, uint32 argument0, uint32 argument1)
{
    switch (address)
    {
        case 0x8009159C: return _SsVmAlloc();
        case 0x800903DC: return _SsVmDoAllocate();
        case 0x8009182C: return note2pitch();
        case 0x8009207C: return SequenceQueueVoice((uint16)argument1);
        case 0x80091F6C: return vmNoiseOff((uint8)argument0);
        case 0x80091FAC: return _SsVmKeyOffNow();
    }
    /* TODO Implement this service when the native scenario reaches it */
    GameDiagnostic("Missing SDK sub_%08X (%08X,%08X)\n", address, argument0, argument1);
    abort();
    return 0;
}

/* Original 80091308 */
static uint32 SequenceNoteOff(uint32 sequence_track, uint32 bank, uint32 program, uint32 note)
{
    uint32 voice = 0, count = 0, index, settings;
    sequence_track = (uint32)(sint32)(sint16)sequence_track;
    bank = (uint32)(sint32)(sint16)bank;
    program = (uint32)(sint32)(sint16)program;
    note = (uint16)note;
    if ((sint8)r_u8(0x8010561C) <= 0)
        return 0;
    do
    {
        index = (uint8)voice;
        settings = index * 54;
        if (!(r_u32(0x800FD01C) & (1u << (index & 31))) && (sint32)(sint16)r_u16(0x80104E36 + settings) == (sint32)note && (sint32)(sint16)r_u16(0x80104E3C + settings) == (sint32)program && (sint32)(sint16)r_u16(0x80104E38 + settings) == (sint32)sequence_track && (sint32)(sint16)r_u16(0x80104E40 + settings) == (sint32)bank)
        {
            if ((sint16)r_u16(0x80104E28 + settings) == 255)
            {
                ++count;
                SequenceVoiceMissing(0x80091F6C, index, 0);
            }
            else
            {
                w_u16(0x80105638, index);
                SequenceVoiceMissing(0x80091FAC, 0, 0);
                ++count;
            }
        }
        ++voice;
    } while ((sint32)(uint8)voice < (sint8)r_u8(0x8010561C));
    return count;
}

/* Original 80092F5C */
static uint32 SequenceSelectTones(uint8 tones[128], uint8 samples[128])
{
    uint32 index = 0, count = 0, tone;
    sint32 note;
    if ((sint8)r_u8(0x80105620) <= 0)
        return 0;
    do
    {
        tone = r_u32(0x80105618) + ((uint32)(sint32)(sint8)r_u8(0x80105627) * 16 + (uint32)(sint32)(sint8)index) * 32;
        note = (sint8)r_u8(0x80105622);
        if (note >= (sint32)r_u8(tone + 6) && (sint32)r_u8(tone + 7) >= note)
        {
            samples[(uint8)count] = r_u8(tone + 22);
            tones[(uint8)count] = (uint8)index;
            ++count;
        }
        ++index;
    } while ((sint8)index < (sint8)r_u8(0x80105620));
    return (uint8)count;
}

static uint32 SequenceVoiceOffset(void)
{
    return (uint32)(sint32)(sint16)r_u16(0x80105638) * 54;
}

/* Original 80090DAC */
static uint32 SequenceNoteOn(uint32 sequence_track, uint32 bank, uint32 program, uint32 note, uint32 velocity, uint32 pan)
{
    uint8 tones[128], samples[128];
    uint32 record, program_data, tone_data, count, index, voice, offset, pitch;
    uint32 result = 0, product;
    sint32 signed_sequence = (sint16)sequence_track;
    program = (uint32)(sint32)(sint16)program;
    velocity = (uint16)velocity;
    pan = (uint16)pan;
    record = r_u32(0x80104590 + (sequence_track & 255) * 4) + ((uint32)signed_sequence & 0xFF00) / 256 * 176;
    if (apocalypse_sound_select_bank(bank, program))
        return 0xFFFFFFFF;
    w_u16(0x80105634, sequence_track);
    w_u8(0x80105622, note);
    w_u8(0x80105623, 0);
    if (signed_sequence == 33)
        w_u8(0x80105624, velocity);
    else
    {
        product = velocity * (uint32)(sint32)(sint16)r_u16(record + r_u8(record + 23) * 2 + 96);
        w_u8(0x80105624, (uint32)((sint32)product / 127));
    }
    w_u8(0x80105625, pan);
    program_data = r_u32(0x8010560C) + program * 16;
    w_u8(0x8010562A, r_u8(program_data + 1));
    w_u8(0x8010562B, r_u8(program_data + 4));
    w_u8(0x80105620, r_u8(program_data));
    if ((sint8)r_u8(0x80105627) >= (sint32)r_u16(r_u32(0x80105614) + 18))
        return 0xFFFFFFFF;
    if (!velocity)
        return SequenceNoteOff((uint32)signed_sequence, (uint32)(sint32)(sint16)bank, program, (uint16)note);
    count = SequenceSelectTones(tones, samples);
    if (!(uint8)count)
        return result;
    index = 0;
    do
    {
        w_u16(0x80105636, samples[index]);
        tone_data = ((uint32)(sint32)(sint8)tones[index] + (uint32)(sint32)(sint8)r_u8(0x80105627) * 16) & 0xFFFF;
        w_u8(0x8010562C, tones[index]);
        tone_data = r_u32(0x80105618) + tone_data * 32;
        w_u8(0x8010562F, r_u8(tone_data));
        w_u8(0x8010562D, r_u8(tone_data + 2));
        w_u8(0x8010562E, r_u8(tone_data + 3));
        w_u8(0x80105630, r_u8(tone_data + 4));
        w_u8(0x80105631, r_u8(tone_data + 5));
        w_u8(0x80105632, r_u8(tone_data + 1));
        voice = (uint8)SequenceVoiceMissing(0x8009159C, 0, 0);
        w_u16(0x80105638, voice);
        if ((sint32)voice >= (sint8)r_u8(0x8010561C))
            result = 0xFFFFFFFF;
        else
        {
            w_u8(0x80104E45 + voice * 54, 1);
            w_u16(0x80104E2A + SequenceVoiceOffset(), 0);
            w_u16(0x80104E38 + SequenceVoiceOffset(), sequence_track);
            offset = SequenceVoiceOffset();
            w_u16(0x80104E40 + offset, (uint32)(sint32)(sint8)r_u8(0x80105621));
            offset = SequenceVoiceOffset();
            w_u16(0x80104E3A + offset, (uint32)(sint32)(sint8)r_u8(0x80105627));
            w_u16(0x80104E3C + SequenceVoiceOffset(), program);
            if (signed_sequence != 33)
            {
                w_u16(0x80104E30 + SequenceVoiceOffset(), velocity);
                offset = SequenceVoiceOffset();
                w_u16(0x80104E34 + offset, r_u8(record + 23));
            }
            w_u8(0x80104E32 + SequenceVoiceOffset(), pan);
            offset = SequenceVoiceOffset();
            w_u16(0x80104E3E + offset, (uint32)(sint32)(sint8)r_u8(0x8010562C));
            w_u16(0x80104E36 + SequenceVoiceOffset(), note);
            offset = SequenceVoiceOffset();
            w_u16(0x80104E42 + offset, (uint32)(sint32)(sint8)r_u8(0x8010562F));
            offset = SequenceVoiceOffset();
            w_u16(0x80104E28 + offset, r_u16(0x80105636));
            SequenceVoiceMissing(0x800903DC, 0, 0);
            if ((sint16)r_u16(0x80105636) == 255)
                SequenceVoiceMissing(0x800919EC, r_u8(0x80105638), 0);
            else
            {
                pitch = SequenceVoiceMissing(0x8009182C, 0, 0);
                SequenceVoiceMissing(0x8009207C, (uint8)count, (uint16)pitch);
            }
            result |= 1u << (r_u16(0x80105638) & 31);
        }
        index = (uint8)(index + 1);
    } while (index < (uint8)count);
    return result;
}

/* Original 8008CDFC */
static uint32 SequenceNote(uint32 sequence, uint32 track, uint32 note, uint32 velocity)
{
    uint32 record, channel, pan, sequence_track, bank, program, result;
    record = r_u32(0x80104590 + (uint32)(sint32)(sint16)sequence * 4) + (uint32)(sint32)(sint16)track * 176;
    channel = r_u8(record + 23);
    pan = r_u8(record + channel + 39);
    sequence_track = (uint32)(sint32)(sint16)(sequence | (track << 8));
    if ((uint8)velocity)
    {
        result = ((uint32)((sint32)(sint16)r_u16(record + 128) >> (channel & 31))) & 1;
        if (result)
            return result;
        bank = (uint32)(sint32)(sint8)r_u8(record + 38);
        program = r_u8(record + channel + 55);
        return SequenceNoteOn(sequence_track, bank, program, (uint8)note, (uint8)velocity, pan);
    }
    bank = (uint32)(sint32)(sint8)r_u8(record + 38);
    program = r_u8(record + channel + 55);
    return SequenceNoteOff(sequence_track, bank, program, (uint8)note);
}

/* Original 8008CEDC */
static uint32 SequenceProgram(uint32 sequence, uint32 track, uint32 program)
{
    uint32 record, delta;
    sequence = (uint32)(sint32)(sint16)sequence;
    track = (uint32)(sint32)(sint16)track;
    record = r_u32(0x80104590 + sequence * 4) + track * 176;
    w_u8(record + r_u8(record + 23) + 55, (uint8)program);
    delta = apocalypse_sequence_delta(record);
    w_u32(record + 144, delta);
    return delta;
}

static uint32 SequenceSignedRatio(uint32 value, uint32 multiplier, uint32 shift)
{
    uint32 high = (uint32)(((sint64)(sint32)value * (sint64)(sint32)multiplier) >> 32);
    return (uint32)((sint32)(high + value) >> shift) - (uint32)((sint32)value >> 31);
}

static uint32 SequenceUnsignedRatio(uint32 value, uint32 multiplier, uint32 shift)
{
    uint32 high = (uint32)(((uint64)value * multiplier) >> 32);
    return (high + ((value - high) >> 1)) >> shift;
}

/* Original 8009302C */
static uint32 SequenceUpdateVolume(uint32 sequence_track, uint32 bank, uint32 program, uint32 volume, uint32 pan)
{
    uint32 record, voice = 0, count = 0, settings, channel_address, tone;
    uint32 amplitude, left, right, tone_pan, program_pan, index, master_volume;
    sint32 signed_sequence = (sint16)sequence_track;
    bank = (uint32)(sint32)(sint16)bank;
    program = (uint32)(sint32)(sint16)program;
    record = r_u32(0x80104590 + (sequence_track & 255) * 4) + ((sequence_track & 0xFF00) >> 8) * 176;
    pan = (uint16)pan;
    (void)apocalypse_sound_select_bank(bank, program);
    w_u16(0x80105634, sequence_track);
    if (!pan)
        pan = 1;
    volume = (uint16)volume;
    if (!volume)
        volume = 1;
    if ((sint8)r_u8(0x8010561C) <= 0)
        return 0;
    do
    {
        index = (uint32)(sint32)(sint16)voice;
        settings = index * 54;
        if (!(r_u32(0x800FD01C) & (1u << (index & 31))) && (sint16)r_u16(0x80104E38 + settings) == signed_sequence && (sint16)r_u16(0x80104E3C + settings) == (sint32)program && (sint16)r_u16(0x80104E40 + settings) == (sint32)bank)
        {
            channel_address = record + r_u8(record + 23) * 2 + 96;
            if ((sint16)r_u16(channel_address) != (sint32)volume && !r_u16(channel_address))
                w_u16(channel_address, 1);
            amplitude = (uint32)(sint32)(sint16)r_u16(0x80104E30 + settings) * volume;
            amplitude = SequenceSignedRatio(amplitude, 0x81020409, 6);
            master_volume = r_u8(r_u32(0x80105614) + 24);
            amplitude *= (master_volume << 14) - master_volume;
            amplitude = SequenceSignedRatio(amplitude, 0x82061029, 13);
            amplitude *= r_u8(r_u32(0x8010560C) + (program << 4) + 1);
            tone = ((uint32)(sint32)(sint16)r_u16(0x80104E3A + settings) << 4) + (uint32)(sint32)(sint16)r_u16(0x80104E3E + settings);
            tone = r_u32(0x80105618) + tone * 32;
            amplitude *= r_u8(tone + 2);
            amplitude = SequenceUnsignedRatio(amplitude, 0x040C2051, 13);
            left = SequenceUnsignedRatio(amplitude * r_u16(record + 88), 0x02040811, 6);
            right = SequenceUnsignedRatio(amplitude * r_u16(record + 90), 0x02040811, 6);
            tone_pan = r_u8(tone + 3);
            if (tone_pan < 64)
                right = SequenceUnsignedRatio(right * tone_pan, 0x04104105, 5);
            else
                left = SequenceUnsignedRatio(left * (127 - tone_pan), 0x04104105, 5);
            program_pan = r_u8(r_u32(0x8010560C) + ((uint32)(sint32)(sint16)r_u16(0x80104E3C + settings) << 4) + 4);
            if (program_pan < 64)
                right = SequenceUnsignedRatio(right * program_pan, 0x04104105, 5);
            else
                left = SequenceUnsignedRatio(left * (127 - program_pan), 0x04104105, 5);
            if ((uint8)pan < 64)
                right = SequenceUnsignedRatio(right * (uint8)pan, 0x04104105, 5);
            else
                left = SequenceUnsignedRatio(left * (127 - (uint8)pan), 0x04104105, 5);
            if ((sint16)r_u16(0x80105608) == 1)
            {
                if (left < right)
                    left = right;
                else
                    right = left;
            }
            w_u16(0x80105358 + index * 16, SequenceUnsignedRatio(left * left, 0x00040011, 13));
            w_u16(0x8010535A + index * 16, SequenceUnsignedRatio(right * right, 0x00040011, 13));
            ++count;
            w_u8(0x80105338 + index, r_u8(0x80105338 + index) | 3);
        }
        ++voice;
    } while ((sint16)voice < (sint8)r_u8(0x8010561C));
    return count;
}

/* Original 8008C8CC */
static uint32 SequenceChannelVolume(uint32 sequence, uint32 track, uint32 value)
{
    uint32 record, channel, bank, program, pan, delta, sequence_track;
    sequence_track = (uint32)(sint32)(sint16)(sequence | (track << 8));
    sequence = (uint32)(sint32)(sint16)sequence;
    track = (uint32)(sint32)(sint16)track;
    record = r_u32(0x80104590 + sequence * 4) + track * 176;
    channel = r_u8(record + 23);
    bank = (uint32)(sint32)(sint8)r_u8(record + 38);
    program = r_u8(record + channel + 55);
    pan = r_u8(record + channel + 39);
    (void)SequenceUpdateVolume(sequence_track, bank, program, (uint8)value, pan);
    w_u16(record + channel * 2 + 96, (uint8)value);
    delta = apocalypse_sequence_delta(record);
    w_u32(record + 144, delta);
    return delta;
}

/* Original 8008C99C */
static uint32 SequenceChannelPan(uint32 sequence, uint32 track, uint32 value)
{
    uint32 record, channel_address, channel, bank, program, volume, delta, sequence_track;
    sequence_track = (uint32)(sint32)(sint16)(sequence | (track << 8));
    sequence = (uint32)(sint32)(sint16)sequence;
    track = (uint32)(sint32)(sint16)track;
    record = r_u32(0x80104590 + sequence * 4) + track * 176;
    channel = r_u8(record + 23);
    channel_address = record + channel;
    bank = (uint32)(sint32)(sint8)r_u8(record + 38);
    program = r_u8(channel_address + 55);
    volume = r_u16(record + channel * 2 + 96);
    (void)SequenceUpdateVolume(sequence_track, bank, program, volume, (uint8)value);
    w_u8(channel_address + 39, value);
    delta = apocalypse_sequence_delta(record);
    w_u32(record + 144, delta);
    return delta;
}

static uint32 EnableSequenceDamper(void)
{
    FUNCTION_MARKER(0x800905BC, "SLUS_003.73");
    w_u16(0x801055C0, 2);
    return 2;
}

static void DisableSequenceDamper(void)
{
    FUNCTION_MARKER(0x800905AC, "SLUS_003.73");
    w_u16(0x801055C0, 0);
}

static uint32 SequenceDamper(uint32 sequence, uint32 track, uint32 value)
{
    FUNCTION_MARKER(0x8008CA6C, "SLUS_003.73");
    uint32 record = r_u32(0x80104590 + (uint32)(sint32)(sint16)sequence * 4) + (uint32)(sint32)(sint16)track * 176;
    if ((uint8)value < 64)
        DisableSequenceDamper();
    else
        EnableSequenceDamper();
    uint32 delta = apocalypse_sequence_delta(record);
    w_u32(record + 144, delta);
    return delta;
}

static uint32 DispatchSequenceControl(uint32 slot, uint32 sequence, uint32 track, uint32 value, uint32 argument_count)
{
    if (slot == 0x8010450C && r_u32(slot) == 0x8008C8CC)
        return SequenceChannelVolume(sequence, track, value);
    if (slot == 0x80104510 && r_u32(slot) == 0x8008C99C)
        return SequenceChannelPan(sequence, track, value);
    if (slot == 0x80104518 && r_u32(slot) == 0x8008CA6C)
        return SequenceDamper(sequence, track, value);
    // Unbound callbacks are null in the original table and cannot be silently skipped
    GameDiagnostic("Missing sequence control callback %08X at slot %08X (%08X,%08X,%08X), semantic args %u\n", r_u32(slot), slot, sequence, track, value, argument_count);
    abort();
    return 0;
}

/* Original 8008CBCC */
static uint32 SequenceControl(uint32 sequence, uint32 track, uint32 controller)
{
    uint32 record, cursor, value, slot = 0, delta;
    sequence = (uint32)(sint32)(sint16)sequence;
    track = (uint32)(sint32)(sint16)track;
    record = r_u32(0x80104590 + sequence * 4) + track * 176;
    cursor = r_u32(record);
    controller = (uint8)controller;
    value = r_u8(cursor);
    w_u32(record, cursor + 1);
    switch (controller)
    {
        case 0:
            w_u8(record + 38, value);
            break;
        case 6:
            slot = 0x80104508;
            break;
        case 7:
            slot = 0x8010450C;
            break;
        case 10:
            slot = 0x80104510;
            break;
        case 11:
            slot = 0x80104514;
            break;
        case 64:
            slot = 0x80104518;
            break;
        case 91:
            slot = 0x8010452C;
            break;
        case 98:
            slot = 0x8010451C;
            break;
        case 99:
            slot = 0x80104520;
            break;
        case 100:
            slot = 0x80104524;
            break;
        case 101:
            slot = 0x80104528;
            break;
        case 121:
            return DispatchSequenceControl(0x80104530, sequence, track, controller, 2);
        default:
            break;
    }
    if (slot)
        return DispatchSequenceControl(slot, sequence, track, value, 3);
    delta = apocalypse_sequence_delta(record);
    w_u32(record + 144, delta);
    return delta;
}

static uint32 SequenceBendVoice(uint32 voice, uint32 identity, uint32 bank, uint32 program, uint32 bend)
{
    uint32 row = 0x80104E28 + (uint32)(sint32)(sint16)voice * 54;
    uint32 note, tone, fine = 0;
    sint32 displacement = (sint16)(bend - 64), product, whole;
    FUNCTION_MARKER(0x8009254C, "SLUS_003.73");
    if ((sint16)r_u16(row + 16) != (sint16)identity || (sint16)r_u16(row + 24) != (sint16)bank || (sint16)r_u16(row + 20) != (sint16)program)
        return 0;
    note = r_u16(row + 14);
    tone = r_u32(0x80105618) + (uint32)(uint16)(r_u16(row + 22) + (sint8)r_u8(0x80105627) * 16) * 32;
    if (displacement > 0)
    {
        product = displacement * r_u8(tone + 13);
        whole = product / 63;
        note += (uint32)whole;
        fine = (uint32)(2 * (product - whole * 63));
    }
    else if (displacement < 0)
    {
        product = displacement * r_u8(tone + 12);
        whole = product / 64;
        note += (uint32)whole - 1;
        fine = (uint32)(2 * (product - whole * 64) + 127);
    }
    w_u16(0x80105638, voice);
    w_u8(0x8010562C, r_u8(row + 22));
    tone = r_u32(0x80105618) + (uint32)(sint32)(sint16)((sint8)r_u8(0x8010562C) + (sint8)r_u8(0x80105627) * 16) * 32;
    uint32 pitch = apocalypse_sound_pitch((uint32)(sint32)(sint16)note, (uint32)(sint32)(sint16)fine, r_u8(tone + 4), r_u8(tone + 5));
    w_u16(0x8010535C + (uint32)(sint32)(sint16)voice * 16, pitch);
    row = 0x80105338 + (uint32)(sint32)(sint16)voice;
    w_u8(row, r_u8(row) | 4);
    return 1;
}

static uint32 SequenceBendVoices(uint32 identity, uint32 bank, uint32 program, uint32 bend)
{
    uint32 voice = 0, result = 0;
    FUNCTION_MARKER(0x80092748, "SLUS_003.73");
    (void)apocalypse_sound_select_bank((uint32)(sint32)(sint16)bank, (uint32)(sint32)(sint16)program);
    w_u16(0x80105634, identity);
    while ((sint16)voice < (sint8)r_u8(0x8010561C))
    {
        result += (uint32)(sint32)(sint16)SequenceBendVoice(voice, identity, bank, program, (uint16)bend);
        ++voice;
    }
    return result;
}

static uint32 SequencePitchBend(uint32 sequence, uint32 track)
{
    uint32 identity = (uint32)(sint32)(sint16)(sequence | (track << 8));
    uint32 record = r_u32(0x80104590 + (uint32)(sint32)(sint16)sequence * 4) + (uint32)(sint32)(sint16)track * 176;
    uint32 cursor = r_u32(record), bend = r_u8(cursor);
    uint32 bank = (uint32)(sint32)(sint8)r_u8(record + 38);
    uint32 channel = r_u8(record + 23);
    FUNCTION_MARKER(0x8008CB1C, "SLUS_003.73");
    w_u32(record, cursor + 1);
    SequenceBendVoices(identity, bank, r_u8(record + channel + 55), bend);
    uint32 delta = apocalypse_sequence_delta(record);
    w_u32(record + 144, delta);
    return delta;
}

extern "C" uint32 xport_libsnd_sequence_call(uint32 target, uint32 argc, const uint32 *arguments)
{
    if (arguments)
    {
        if (target == 0x8008CDFC && argc == 4)
            return SequenceNote(arguments[0], arguments[1], arguments[2], arguments[3]);
        if (target == 0x8008CEDC && argc == 3)
            return SequenceProgram(arguments[0], arguments[1], arguments[2]);
        if (target == 0x8008CBCC && argc == 3)
            return SequenceControl(arguments[0], arguments[1], arguments[2]);
        if (target == 0x8008CB1C && argc == 2)
            return SequencePitchBend(arguments[0], arguments[1]);
    }
    GameDiagnostic("Missing native sequence callback %08X argc %u\n", target, argc);
    abort();
    return 0;
}

static void SequenceDispatchCallback(uint32 slot, uint32 sequence, uint32 track, uint32 data, uint32 velocity)
{
    if (slot == 0x80104500 && r_u32(slot) == 0x8008CBCC)
    {
        (void)SequenceControl(sequence, track, data);
        return;
    }
    if (slot == 0x801044F0 && r_u32(slot) == 0x8008CDFC)
    {
        (void)SequenceNote(sequence, track, data, velocity);
        return;
    }
    if (slot == 0x801044F4 && r_u32(slot) == 0x8008CEDC)
    {
        (void)SequenceProgram(sequence, track, data);
        return;
    }
    if (slot == 0x801044F8 && r_u32(slot) == 0x8008CB1C)
    {
        SequencePitchBend(sequence, track);
        return;
    }
    /* TODO Translate the observed callback before enabling this event */
    GameDiagnostic("Missing sequence callback %08X at slot %08X (%08X,%08X,%08X,%08X)\n", r_u32(slot), slot, sequence, track, data, velocity);
    abort();
}

static uint32 SequenceEnd(uint32 sequence, uint32 track)
{
    uint32 table, displacement, record, count, flags_record, start_offset, result, identity;
    sint32 limit;
    static const uint32 clear_masks[3] = {1, 8, 2};
    FUNCTION_MARKER(0x8008D648, "SLUS_003.73");
    table = 0x80104590 + (uint32)(sint32)(sint16)sequence * 4;
    displacement = (uint32)(sint32)(sint16)track * 176;
    record = r_u32(table) + displacement;
    count = r_u8(record + 33) + 1;
    limit = (sint8)r_u8(record + 32);
    w_u8(record + 33, count);
    if (!limit || (sint8)count < limit)
    {
        w_u32(record + 136, 0);
        w_u8(record + 28, 0);
        w_u32(record + 144, 0);
        start_offset = (r_u32(r_u32(table) + displacement + 152) & 0x400) ? 12 : 4;
        result = r_u32(record + start_offset);
        if (limit)
        {
            uint32 replay = r_u32(record + start_offset);
            w_u32(record, result);
            w_u32(record + 8, replay);
        }
        else
            w_u32(record, result);
        return result;
    }
    for (uint32 index = 0; index < 3; ++index)
    {
        flags_record = r_u32(table) + displacement;
        w_u32(flags_record + 152, r_u32(flags_record + 152) & ~clear_masks[index]);
    }
    flags_record = r_u32(table) + displacement;
    w_u32(flags_record + 152, r_u32(flags_record + 152) | 0x200);
    flags_record = r_u32(table) + displacement;
    w_u32(flags_record + 152, r_u32(flags_record + 152) | 4);
    w_u8(record + 20, 0);
    start_offset = (r_u32(r_u32(table) + displacement + 152) & 0x400) ? 12 : 4;
    w_u32(record + 8, r_u32(record + start_offset));
    identity = (uint32)(sint32)(sint16)(sequence | (track << 8));
    if ((sint8)r_u8(record + 34) != -1)
    {
        uint32 next_sequence = (uint32)(sint32)(sint8)r_u8(record + 34);
        uint32 next_track = (uint32)(sint32)(sint8)r_u8(record + 35);
        w_u8(record + 20, 0);
        StartChainedSequence(next_sequence, next_track);
        StopSequenceVoices(identity);
    }
    StopSequenceVoices(identity);
    result = (uint32)(sint32)(sint16)r_u16(record + 84);
    w_u32(record + 144, result);
    return result;
}

static uint32 SequenceReadEventByte(uint32 record)
{
    uint32 cursor = r_u32(record), byte = r_u8(cursor);
    w_u32(record, cursor + 1);
    return byte;
}

/* Original 8008D88C */
static uint32 SequenceEvent(uint32 sequence, uint32 track)
{
    uint32 table, record, byte, status, data, velocity, cursor;
    sequence = (uint32)(sint32)(sint16)sequence;
    track = (uint32)(sint32)(sint16)track;
    table = 0x80104590 + sequence * 4;
    record = r_u32(table) + track * 176;
    byte = SequenceReadEventByte(record);
    cursor = r_u32(record);
    if ((r_u32(r_u32(table) + track * 176 + 152) & 0x401) == 0x401 && cursor == r_u32(record + 16) + 1)
    {
        SequenceEnd(sequence, track);
        return 0xFFFFFFFF;
    }
    if (byte & 0x80)
    {
        w_u8(record + 23, byte & 15);
        status = byte & 0xF0;
        switch (status)
        {
            case 0x90:
                w_u8(record + 22, status);
                data = SequenceReadEventByte(record);
                velocity = SequenceReadEventByte(record);
                w_u32(record + 144, apocalypse_sequence_delta(record));
                SequenceDispatchCallback(0x801044F0, sequence, track, data, velocity);
                return 0;
            case 0xB0:
            case 0xC0:
                w_u8(record + 22, status);
                data = SequenceReadEventByte(record);
                SequenceDispatchCallback(status == 0xB0 ? 0x80104500 : 0x801044F4, sequence, track, data, 0);
                return 0;
            case 0xE0:
                w_u8(record + 22, status);
                w_u32(record, r_u32(record) + 1);
                SequenceDispatchCallback(0x801044F8, sequence, track, status, 0);
                return 0;
            case 0xF0:
                w_u8(record + 22, 0xFF);
                data = SequenceReadEventByte(record);
                if (data == 0x2F)
                {
                    SequenceEnd(sequence, track);
                    return 1;
                }
                SequenceDispatchCallback(0x801044FC, sequence, track, data, 0);
                return 0;
            default:
                return 0;
        }
    }
    status = r_u8(record + 22);
    switch (status)
    {
        case 0x90:
            velocity = SequenceReadEventByte(record);
            w_u32(record + 144, apocalypse_sequence_delta(record));
            SequenceDispatchCallback(0x801044F0, sequence, track, byte, velocity);
            return 0;
        case 0xB0:
        case 0xC0:
            SequenceDispatchCallback(status == 0xB0 ? 0x80104500 : 0x801044F4, sequence, track, byte, 0);
            return 0;
        case 0xE0:
            SequenceDispatchCallback(0x801044F8, sequence, track, table, 0);
            return 0;
        case 0xFF:
            if (byte == 0x2F)
            {
                SequenceEnd(sequence, track);
                return 1;
            }
            SequenceDispatchCallback(0x801044FC, sequence, track, byte, 0);
            return 0;
        default:
            return 0;
    }
}

/* Original 8008D54C */
extern "C" uint32 apocalypse_sequence_tick(uint32 sequence, uint32 track)
{
    uint32 record, tick, remaining, difference, result, accumulated;
    sint32 step;
    sequence = (uint32)(sint32)(sint16)sequence;
    track = (uint32)(sint32)(sint16)track;
    record = r_u32(0x80104590 + sequence * 4) + track * 176;
    step = (sint16)r_u16(record + 84);
    tick = r_u32(record + 144);
    difference = tick - (uint32)step;
    result = step < (sint32)tick;
    if ((sint32)difference > 0)
    {
        remaining = r_u16(record + 82);
        result = remaining - 1;
        if ((sint16)remaining > 0)
        {
            w_u16(record + 82, result);
            return result;
        }
        if ((sint16)remaining != 0)
        {
            w_u32(record + 144, difference);
            return result;
        }
        w_u16(record + 82, r_u16(record + 84));
        result = r_u32(record + 144) - 1;
    }
    else
    {
        accumulated = tick;
        if (result)
            return result;
        do
        {
            do
            {
                SequenceEvent(sequence, track);
                tick = r_u32(record + 144);
            } while (!tick);
            step = (sint16)r_u16(record + 84);
            accumulated += tick;
            result = accumulated - (uint32)step;
        } while ((sint32)accumulated < step);
    }
    w_u32(record + 144, result);
    return result;
}
