#include "psx.h"
#include "game_sound_native.h"
#include <stdio.h>
#include <stdlib.h>

uint32 apocalypse_sequence_delta(uint32 record);

static uint32 sequence_note_missing(uint32 address, uint32 sequence_track,
                                    uint32 bank, uint32 program, uint32 note,
                                    uint32 velocity, uint32 pan)
{
    /* TODO Implement the observed note service from its complete original */
    fprintf(stderr, "Missing SDK sub_%08X (%08X,%08X,%08X,%08X,%08X,%08X)\n",
            address, sequence_track, bank, program, note, velocity, pan);
    abort();
    return 0u;
}

static uint32 sequence_voice_missing(uint32 address, uint32 argument0, uint32 argument1)
{
    /* TODO Implement this service when the native scenario reaches it */
    fprintf(stderr, "Missing SDK sub_%08X (%08X,%08X)\n", address, argument0, argument1);
    abort();
    return 0u;
}

/* Original 80091308 */
static uint32 sequence_note_off(uint32 sequence_track, uint32 bank, uint32 program, uint32 note)
{
    uint32 voice = 0u, count = 0u, index, settings;
    sequence_track = (uint32)(sint32)(sint16)sequence_track;
    bank = (uint32)(sint32)(sint16)bank;
    program = (uint32)(sint32)(sint16)program;
    note = (uint16)note;
    if ((sint8)r_u8(0x8010561Cu) <= 0)
        return 0u;
    do
    {
        index = (uint8)voice;
        settings = index * 54u;
        if (!(r_u32(0x800FD01Cu) & (1u << (index & 31u)))
            && (sint32)(sint16)r_u16(0x80104E36u + settings) == (sint32)note
            && (sint32)(sint16)r_u16(0x80104E3Cu + settings) == (sint32)program
            && (sint32)(sint16)r_u16(0x80104E38u + settings) == (sint32)sequence_track
            && (sint32)(sint16)r_u16(0x80104E40u + settings) == (sint32)bank)
        {
            if ((sint16)r_u16(0x80104E28u + settings) == 255)
            {
                ++count;
                sequence_voice_missing(0x80091F6Cu, index, 0u);
            }
            else
            {
                w_u16(0x80105638u, index);
                sequence_voice_missing(0x80091FACu, 0u, 0u);
                ++count;
            }
        }
        ++voice;
    } while ((sint32)(uint8)voice < (sint8)r_u8(0x8010561Cu));
    return count;
}

/* Original 80092F5C */
static uint32 sequence_select_tones(uint8 tones[128], uint8 samples[128])
{
    uint32 index = 0u, count = 0u, tone;
    sint32 note;
    if ((sint8)r_u8(0x80105620u) <= 0)
        return 0u;
    do
    {
        tone = r_u32(0x80105618u)
             + ((uint32)(sint32)(sint8)r_u8(0x80105627u) * 16u
             + (uint32)(sint32)(sint8)index) * 32u;
        note = (sint8)r_u8(0x80105622u);
        if (note >= (sint32)r_u8(tone + 6u) && (sint32)r_u8(tone + 7u) >= note)
        {
            samples[(uint8)count] = r_u8(tone + 22u);
            tones[(uint8)count] = (uint8)index;
            ++count;
        }
        ++index;
    } while ((sint8)index < (sint8)r_u8(0x80105620u));
    return (uint8)count;
}

static uint32 sequence_voice_offset(void)
{
    return (uint32)(sint32)(sint16)r_u16(0x80105638u) * 54u;
}

/* Original 80090DAC */
static uint32 sequence_note_on(uint32 sequence_track, uint32 bank, uint32 program,
                              uint32 note, uint32 velocity, uint32 pan)
{
    uint8 tones[128], samples[128];
    uint32 record, program_data, tone_data, count, index, voice, offset, pitch;
    uint32 result = 0u, product;
    sint32 signed_sequence = (sint16)sequence_track;
    program = (uint32)(sint32)(sint16)program;
    velocity = (uint16)velocity;
    pan = (uint16)pan;
    record = r_u32(0x80104590u + (sequence_track & 255u) * 4u)
           + ((uint32)signed_sequence & 0xFF00u) / 256u * 176u;
    if (apocalypse_sound_select_bank(bank, program))
        return 0xFFFFFFFFu;
    w_u16(0x80105634u, sequence_track);
    w_u8(0x80105622u, note);
    w_u8(0x80105623u, 0u);
    if (signed_sequence == 33)
        w_u8(0x80105624u, velocity);
    else
    {
        product = velocity * (uint32)(sint32)(sint16)r_u16(record + r_u8(record + 23u) * 2u + 96u);
        w_u8(0x80105624u, (uint32)((sint32)product / 127));
    }
    w_u8(0x80105625u, pan);
    program_data = r_u32(0x8010560Cu) + program * 16u;
    w_u8(0x8010562Au, r_u8(program_data + 1u));
    w_u8(0x8010562Bu, r_u8(program_data + 4u));
    w_u8(0x80105620u, r_u8(program_data));
    if ((sint8)r_u8(0x80105627u) >= (sint32)r_u16(r_u32(0x80105614u) + 18u))
        return 0xFFFFFFFFu;
    if (!velocity)
        return sequence_note_off((uint32)signed_sequence,
                                 (uint32)(sint32)(sint16)bank, program, (uint16)note);
    count = sequence_select_tones(tones, samples);
    if (!(uint8)count)
        return result;
    index = 0u;
    do
    {
        w_u16(0x80105636u, samples[index]);
        tone_data = ((uint32)(sint32)(sint8)tones[index]
                   + (uint32)(sint32)(sint8)r_u8(0x80105627u) * 16u) & 0xFFFFu;
        w_u8(0x8010562Cu, tones[index]);
        tone_data = r_u32(0x80105618u) + tone_data * 32u;
        w_u8(0x8010562Fu, r_u8(tone_data));
        w_u8(0x8010562Du, r_u8(tone_data + 2u));
        w_u8(0x8010562Eu, r_u8(tone_data + 3u));
        w_u8(0x80105630u, r_u8(tone_data + 4u));
        w_u8(0x80105631u, r_u8(tone_data + 5u));
        w_u8(0x80105632u, r_u8(tone_data + 1u));
        voice = (uint8)sequence_voice_missing(0x8009159Cu, 0u, 0u);
        w_u16(0x80105638u, voice);
        if ((sint32)voice >= (sint8)r_u8(0x8010561Cu))
            result = 0xFFFFFFFFu;
        else
        {
            w_u8(0x80104E45u + voice * 54u, 1u);
            w_u16(0x80104E2Au + sequence_voice_offset(), 0u);
            w_u16(0x80104E38u + sequence_voice_offset(), sequence_track);
            offset = sequence_voice_offset();
            w_u16(0x80104E40u + offset, (uint32)(sint32)(sint8)r_u8(0x80105621u));
            offset = sequence_voice_offset();
            w_u16(0x80104E3Au + offset, (uint32)(sint32)(sint8)r_u8(0x80105627u));
            w_u16(0x80104E3Cu + sequence_voice_offset(), program);
            if (signed_sequence != 33)
            {
                w_u16(0x80104E30u + sequence_voice_offset(), velocity);
                offset = sequence_voice_offset();
                w_u16(0x80104E34u + offset, r_u8(record + 23u));
            }
            w_u8(0x80104E32u + sequence_voice_offset(), pan);
            offset = sequence_voice_offset();
            w_u16(0x80104E3Eu + offset, (uint32)(sint32)(sint8)r_u8(0x8010562Cu));
            w_u16(0x80104E36u + sequence_voice_offset(), note);
            offset = sequence_voice_offset();
            w_u16(0x80104E42u + offset, (uint32)(sint32)(sint8)r_u8(0x8010562Fu));
            offset = sequence_voice_offset();
            w_u16(0x80104E28u + offset, r_u16(0x80105636u));
            sequence_voice_missing(0x800903DCu, 0u, 0u);
            if ((sint16)r_u16(0x80105636u) == 255)
                sequence_voice_missing(0x800919ECu, r_u8(0x80105638u), 0u);
            else
            {
                pitch = sequence_voice_missing(0x8009182Cu, 0u, 0u);
                sequence_voice_missing(0x8009207Cu, (uint8)count, (uint16)pitch);
            }
            result |= 1u << (r_u16(0x80105638u) & 31u);
        }
        index = (uint8)(index + 1u);
    } while (index < (uint8)count);
    return result;
}

/* Original 8008CDFC */
static uint32 sequence_note(uint32 sequence, uint32 track, uint32 note, uint32 velocity)
{
    uint32 record, channel, pan, sequence_track, bank, program, result;
    record = r_u32(0x80104590u + (uint32)(sint32)(sint16)sequence * 4u)
           + (uint32)(sint32)(sint16)track * 176u;
    channel = r_u8(record + 23u);
    pan = r_u8(record + channel + 39u);
    sequence_track = (uint32)(sint32)(sint16)(sequence | (track << 8));
    if ((uint8)velocity)
    {
        result = ((uint32)((sint32)(sint16)r_u16(record + 128u) >> (channel & 31u))) & 1u;
        if (result)
            return result;
        bank = (uint32)(sint32)(sint8)r_u8(record + 38u);
        program = r_u8(record + channel + 55u);
        return sequence_note_on(sequence_track, bank, program,
                                (uint8)note, (uint8)velocity, pan);
    }
    bank = (uint32)(sint32)(sint8)r_u8(record + 38u);
    program = r_u8(record + channel + 55u);
    return sequence_note_off(sequence_track, bank, program, (uint8)note);
}

/* Original 8008CEDC */
static uint32 sequence_program(uint32 sequence, uint32 track, uint32 program)
{
    uint32 record, delta;
    sequence = (uint32)(sint32)(sint16)sequence;
    track = (uint32)(sint32)(sint16)track;
    record = r_u32(0x80104590u + sequence * 4u) + track * 176u;
    w_u8(record + r_u8(record + 23u) + 55u, (uint8)program);
    delta = apocalypse_sequence_delta(record);
    w_u32(record + 144u, delta);
    return delta;
}

static uint32 sequence_signed_ratio(uint32 value, uint32 multiplier, uint32 shift)
{
    uint32 high = (uint32)(((sint64)(sint32)value * (sint64)(sint32)multiplier) >> 32);
    return (uint32)((sint32)(high + value) >> shift) - (uint32)((sint32)value >> 31);
}

static uint32 sequence_unsigned_ratio(uint32 value, uint32 multiplier, uint32 shift)
{
    uint32 high = (uint32)(((uint64)value * multiplier) >> 32);
    return (high + ((value - high) >> 1)) >> shift;
}

/* Original 8009302C */
static uint32 sequence_update_volume(uint32 sequence_track, uint32 bank, uint32 program,
                                     uint32 volume, uint32 pan)
{
    uint32 record, voice = 0u, count = 0u, settings, channel_address, tone;
    uint32 amplitude, left, right, tone_pan, program_pan, index, master_volume;
    sint32 signed_sequence = (sint16)sequence_track;
    bank = (uint32)(sint32)(sint16)bank;
    program = (uint32)(sint32)(sint16)program;
    record = r_u32(0x80104590u + (sequence_track & 255u) * 4u)
           + ((sequence_track & 0xFF00u) >> 8) * 176u;
    pan = (uint16)pan;
    (void)apocalypse_sound_select_bank(bank, program);
    w_u16(0x80105634u, sequence_track);
    if (!pan) pan = 1u;
    volume = (uint16)volume;
    if (!volume) volume = 1u;
    if ((sint8)r_u8(0x8010561Cu) <= 0)
        return 0u;
    do
    {
        index = (uint32)(sint32)(sint16)voice;
        settings = index * 54u;
        if (!(r_u32(0x800FD01Cu) & (1u << (index & 31u)))
            && (sint16)r_u16(0x80104E38u + settings) == signed_sequence
            && (sint16)r_u16(0x80104E3Cu + settings) == (sint32)program
            && (sint16)r_u16(0x80104E40u + settings) == (sint32)bank)
        {
            channel_address = record + r_u8(record + 23u) * 2u + 96u;
            if ((sint16)r_u16(channel_address) != (sint32)volume && !r_u16(channel_address))
                w_u16(channel_address, 1u);
            amplitude = (uint32)(sint32)(sint16)r_u16(0x80104E30u + settings) * volume;
            amplitude = sequence_signed_ratio(amplitude, 0x81020409u, 6u);
            master_volume = r_u8(r_u32(0x80105614u) + 24u);
            amplitude *= (master_volume << 14) - master_volume;
            amplitude = sequence_signed_ratio(amplitude, 0x82061029u, 13u);
            amplitude *= r_u8(r_u32(0x8010560Cu) + (program << 4) + 1u);
            tone = ((uint32)(sint32)(sint16)r_u16(0x80104E3Au + settings) << 4)
                 + (uint32)(sint32)(sint16)r_u16(0x80104E3Eu + settings);
            tone = r_u32(0x80105618u) + tone * 32u;
            amplitude *= r_u8(tone + 2u);
            amplitude = sequence_unsigned_ratio(amplitude, 0x040C2051u, 13u);
            left = sequence_unsigned_ratio(amplitude * r_u16(record + 88u), 0x02040811u, 6u);
            right = sequence_unsigned_ratio(amplitude * r_u16(record + 90u), 0x02040811u, 6u);
            tone_pan = r_u8(tone + 3u);
            if (tone_pan < 64u)
                right = sequence_unsigned_ratio(right * tone_pan, 0x04104105u, 5u);
            else
                left = sequence_unsigned_ratio(left * (127u - tone_pan), 0x04104105u, 5u);
            program_pan = r_u8(r_u32(0x8010560Cu)
                              + ((uint32)(sint32)(sint16)r_u16(0x80104E3Cu + settings) << 4) + 4u);
            if (program_pan < 64u)
                right = sequence_unsigned_ratio(right * program_pan, 0x04104105u, 5u);
            else
                left = sequence_unsigned_ratio(left * (127u - program_pan), 0x04104105u, 5u);
            if ((uint8)pan < 64u)
                right = sequence_unsigned_ratio(right * (uint8)pan, 0x04104105u, 5u);
            else
                left = sequence_unsigned_ratio(left * (127u - (uint8)pan), 0x04104105u, 5u);
            if ((sint16)r_u16(0x80105608u) == 1)
            {
                if (left < right) left = right;
                else right = left;
            }
            w_u16(0x80105358u + index * 16u, sequence_unsigned_ratio(left * left, 0x00040011u, 13u));
            w_u16(0x8010535Au + index * 16u, sequence_unsigned_ratio(right * right, 0x00040011u, 13u));
            ++count;
            w_u8(0x80105338u + index, r_u8(0x80105338u + index) | 3u);
        }
        ++voice;
    } while ((sint16)voice < (sint8)r_u8(0x8010561Cu));
    return count;
}

/* Original 8008C8CC */
static uint32 sequence_channel_volume(uint32 sequence, uint32 track, uint32 value)
{
    uint32 record, channel, bank, program, pan, delta, sequence_track;
    sequence_track = (uint32)(sint32)(sint16)(sequence | (track << 8));
    sequence = (uint32)(sint32)(sint16)sequence;
    track = (uint32)(sint32)(sint16)track;
    record = r_u32(0x80104590u + sequence * 4u) + track * 176u;
    channel = r_u8(record + 23u);
    bank = (uint32)(sint32)(sint8)r_u8(record + 38u);
    program = r_u8(record + channel + 55u);
    pan = r_u8(record + channel + 39u);
    (void)sequence_update_volume(sequence_track, bank, program, (uint8)value, pan);
    w_u16(record + channel * 2u + 96u, (uint8)value);
    delta = apocalypse_sequence_delta(record);
    w_u32(record + 144u, delta);
    return delta;
}

/* Original 8008C99C */
static uint32 sequence_channel_pan(uint32 sequence, uint32 track, uint32 value)
{
    uint32 record, channel_address, channel, bank, program, volume, delta, sequence_track;
    sequence_track = (uint32)(sint32)(sint16)(sequence | (track << 8));
    sequence = (uint32)(sint32)(sint16)sequence;
    track = (uint32)(sint32)(sint16)track;
    record = r_u32(0x80104590u + sequence * 4u) + track * 176u;
    channel = r_u8(record + 23u);
    channel_address = record + channel;
    bank = (uint32)(sint32)(sint8)r_u8(record + 38u);
    program = r_u8(channel_address + 55u);
    volume = r_u16(record + channel * 2u + 96u);
    (void)sequence_update_volume(sequence_track, bank, program, volume, (uint8)value);
    w_u8(channel_address + 39u, value);
    delta = apocalypse_sequence_delta(record);
    w_u32(record + 144u, delta);
    return delta;
}

static uint32 sequence_control_missing(uint32 slot, uint32 sequence, uint32 track,
                                       uint32 value, uint32 argument_count)
{
    if (slot == 0x8010450Cu && r_u32(slot) == 0x8008C8CCu)
        return sequence_channel_volume(sequence, track, value);
    if (slot == 0x80104510u && r_u32(slot) == 0x8008C99Cu)
        return sequence_channel_pan(sequence, track, value);
    /* TODO Translate the observed control callback before connecting it */
    fprintf(stderr, "Missing sequence control callback %08X at slot %08X (%08X,%08X,%08X), semantic args %u\n",
            r_u32(slot), slot, sequence, track, value, argument_count);
    abort();
    return 0u;
}

/* Original 8008CBCC */
static uint32 sequence_control(uint32 sequence, uint32 track, uint32 controller)
{
    uint32 record, cursor, value, slot = 0u, delta;
    sequence = (uint32)(sint32)(sint16)sequence;
    track = (uint32)(sint32)(sint16)track;
    record = r_u32(0x80104590u + sequence * 4u) + track * 176u;
    cursor = r_u32(record);
    controller = (uint8)controller;
    value = r_u8(cursor);
    w_u32(record, cursor + 1u);
    switch (controller)
    {
    case 0u:
        w_u8(record + 38u, value);
        break;
    case 6u: slot = 0x80104508u; break;
    case 7u: slot = 0x8010450Cu; break;
    case 10u: slot = 0x80104510u; break;
    case 11u: slot = 0x80104514u; break;
    case 64u: slot = 0x80104518u; break;
    case 91u: slot = 0x8010452Cu; break;
    case 98u: slot = 0x8010451Cu; break;
    case 99u: slot = 0x80104520u; break;
    case 100u: slot = 0x80104524u; break;
    case 101u: slot = 0x80104528u; break;
    case 121u:
        return sequence_control_missing(0x80104530u, sequence, track, controller, 2u);
    default:
        break;
    }
    if (slot)
        return sequence_control_missing(slot, sequence, track, value, 3u);
    delta = apocalypse_sequence_delta(record);
    w_u32(record + 144u, delta);
    return delta;
}

static void sequence_dispatch_callback(uint32 slot, uint32 sequence,
                                      uint32 track, uint32 data, uint32 velocity)
{
    if (slot == 0x80104500u && r_u32(slot) == 0x8008CBCCu)
    {
        (void)sequence_control(sequence, track, data);
        return;
    }
    if (slot == 0x801044F0u && r_u32(slot) == 0x8008CDFCu)
    {
        (void)sequence_note(sequence, track, data, velocity);
        return;
    }
    if (slot == 0x801044F4u && r_u32(slot) == 0x8008CEDCu)
    {
        (void)sequence_program(sequence, track, data);
        return;
    }
    /* TODO Translate the observed callback before enabling this event */
    fprintf(stderr, "Missing sequence callback %08X at slot %08X (%08X,%08X,%08X,%08X)\n",
            r_u32(slot), slot, sequence, track, data, velocity);
    abort();
}

static void sequence_missing_end(uint32 sequence, uint32 track, uint32 event)
{
    /* TODO Implement SDK sub_8008D648 when its end event is observed */
    fprintf(stderr, "Missing SDK sub_8008D648 (%08X,%08X,%08X)\n", sequence, track, event);
    abort();
}

static uint32 sequence_read_event_byte(uint32 record)
{
    uint32 cursor = r_u32(record), byte = r_u8(cursor);
    w_u32(record, cursor + 1u);
    return byte;
}

/* Original 8008D88C */
static uint32 sequence_event(uint32 sequence, uint32 track)
{
    uint32 table, record, byte, status, data, velocity, cursor;
    sequence = (uint32)(sint32)(sint16)sequence;
    track = (uint32)(sint32)(sint16)track;
    table = 0x80104590u + sequence * 4u;
    record = r_u32(table) + track * 176u;
    byte = sequence_read_event_byte(record);
    cursor = r_u32(record);
    if ((r_u32(r_u32(table) + track * 176u + 152u) & 0x401u) == 0x401u &&
        cursor == r_u32(record + 16u) + 1u)
    {
        sequence_missing_end(sequence, track, r_u8(r_u32(record + 16u) + 1u));
        return 0xFFFFFFFFu;
    }
    if (byte & 0x80u)
    {
        w_u8(record + 23u, byte & 15u);
        status = byte & 0xF0u;
        switch (status)
        {
        case 0x90u:
            w_u8(record + 22u, status);
            data = sequence_read_event_byte(record);
            velocity = sequence_read_event_byte(record);
            w_u32(record + 144u, apocalypse_sequence_delta(record));
            sequence_dispatch_callback(0x801044F0u, sequence, track, data, velocity);
            return 0u;
        case 0xB0u:
        case 0xC0u:
            w_u8(record + 22u, status);
            data = sequence_read_event_byte(record);
            sequence_dispatch_callback(status == 0xB0u ? 0x80104500u : 0x801044F4u,
                                      sequence, track, data, 0u);
            return 0u;
        case 0xE0u:
            w_u8(record + 22u, status);
            w_u32(record, r_u32(record) + 1u);
            sequence_dispatch_callback(0x801044F8u, sequence, track, status, 0u);
            return 0u;
        case 0xF0u:
            w_u8(record + 22u, 0xFFu);
            data = sequence_read_event_byte(record);
            if (data == 0x2Fu)
            {
                sequence_missing_end(sequence, track, 0x2Fu);
                return 1u;
            }
            sequence_dispatch_callback(0x801044FCu, sequence, track, data, 0u);
            return 0u;
        default:
            return 0u;
        }
    }
    status = r_u8(record + 22u);
    switch (status)
    {
    case 0x90u:
        velocity = sequence_read_event_byte(record);
        w_u32(record + 144u, apocalypse_sequence_delta(record));
        sequence_dispatch_callback(0x801044F0u, sequence, track, byte, velocity);
        return 0u;
    case 0xB0u:
    case 0xC0u:
        sequence_dispatch_callback(status == 0xB0u ? 0x80104500u : 0x801044F4u,
                                  sequence, track, byte, 0u);
        return 0u;
    case 0xE0u:
        /* TODO Resolve the carried raw argument type before connecting pitch */
        sequence_dispatch_callback(0x801044F8u, sequence, track, table, 0u);
        return 0u;
    case 0xFFu:
        if (byte == 0x2Fu)
        {
            sequence_missing_end(sequence, track, 0x2Fu);
            return 1u;
        }
        sequence_dispatch_callback(0x801044FCu, sequence, track, byte, 0u);
        return 0u;
    default:
        return 0u;
    }
}

/* Original 8008D54C */
uint32 apocalypse_sequence_tick(uint32 sequence, uint32 track)
{
    uint32 record, tick, remaining, difference, result, accumulated;
    sint32 step;
    sequence = (uint32)(sint32)(sint16)sequence;
    track = (uint32)(sint32)(sint16)track;
    record = r_u32(0x80104590u + sequence * 4u) + track * 176u;
    step = (sint16)r_u16(record + 84u);
    tick = r_u32(record + 144u);
    difference = tick - (uint32)step;
    result = step < (sint32)tick;
    if ((sint32)difference > 0)
    {
        remaining = r_u16(record + 82u);
        result = remaining - 1u;
        if ((sint16)remaining > 0)
        {
            w_u16(record + 82u, result);
            return result;
        }
        if ((sint16)remaining != 0)
        {
            w_u32(record + 144u, difference);
            return result;
        }
        w_u16(record + 82u, r_u16(record + 84u));
        result = r_u32(record + 144u) - 1u;
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
                sequence_event(sequence, track);
                tick = r_u32(record + 144u);
            } while (!tick);
            step = (sint16)r_u16(record + 84u);
            accumulated += tick;
            result = accumulated - (uint32)step;
        } while ((sint32)accumulated < step);
    }
    w_u32(record + 144u, result);
    return result;
}

