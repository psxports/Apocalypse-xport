#include "game_sdk_bindings.h"
#include "sequence_transport.h"
extern "C"
{
#include "game_sound_native.h"
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
}
#include <stdio.h>
#include <stdlib.h>

/* Unverified project translations of the observed LIBSND sequence controls */
static uint32 ReadSequenceControlRecord(uint32 table, uint32 displacement)
{
    uint32 record = r_u32(table) + displacement;
    if (!r_u32(table) || (record & 3) || (record & 0x1FFFFFFF) > PSX_DRAM_SIZE - 176)
    {
        GameDiagnostic("Missing native sequence record initialization at %08X\n", table);
        abort();
    }
    return record;
}

static uint32 MultiplyControlSignedHigh(uint32 value, uint32 multiplier)
{
    return (uint32)(((sint64)(sint32)value * (sint64)(sint32)multiplier) >> 32);
}

static uint32 ScaleSequenceControlSigned(uint32 value, uint32 multiplier, uint32 shift)
{
    uint32 sum = MultiplyControlSignedHigh(value, multiplier) + value;
    return (uint32)((sint32)sum >> shift) - (uint32)((sint32)value >> 31);
}

static uint32 ScaleSequenceControlUnsigned(uint32 value, uint32 multiplier, uint32 shift)
{
    uint32 high = (uint32)(((uint64)value * multiplier) >> 32);
    return (high + ((value - high) >> 1)) >> shift;
}

uint32 SetSequenceVoiceVolumes(uint32 sequence, uint32 left, uint32 right)
{
    FUNCTION_MARKER(0x8009282C, "SLUS_003.73");
    uint32 record = ReadSequenceControlRecord(0x80104590 + (sequence & 255) * 4, ((sequence & 0xFF00) >> 8) * 176);
    uint32 voice = 0;
    w_u16(record + 88, (uint16)left);
    left = r_u16(record + 88) < 127;
    w_u16(record + 90, (uint16)right);
    if (!left)
        w_u16(record + 88, 127);
    if (r_u16(record + 90) >= 127)
        w_u16(record + 90, 127);
    if ((sint8)r_u8(0x8010561C) > 0)
    {
        do
        {
            uint32 index = (uint32)(sint32)(sint16)voice;
            uint32 settings = index * 54, value, amplitude, tone, program, pan, left_level, right_level;
            if (!(r_u32(0x800FD01C) & (1u << (index & 31))) && (sint16)r_u16(0x80104E38 + settings) == (sint16)sequence && (sint16)r_u16(0x80104E40 + settings) == (sint8)r_u8(record + 38))
            {
                (void)apocalypse_sound_select_bank((uint32)(sint32)(sint16)r_u16(0x80104E40 + settings), (uint32)(sint32)(sint16)r_u16(0x80104E3A + settings));
                value = (uint32)(sint32)(sint16)r_u16(0x80104E34 + settings);
                amplitude = (uint32)(sint32)(sint16)r_u16(0x80104E30 + settings) * (uint32)(sint32)(sint16)r_u16(record + 96 + value * 2);
                amplitude = ScaleSequenceControlSigned(amplitude, 0x81020409, 6);
                amplitude = ((amplitude << 14) - amplitude) * r_u8(r_u32(0x80105614) + 24);
                amplitude = ScaleSequenceControlSigned(amplitude, 0x82061029, 13);
                value = (uint32)(sint32)(sint16)r_u16(0x80104E3C + settings);
                amplitude *= r_u8(r_u32(0x8010560C) + (value << 4) + 1);
                program = (uint32)(sint32)(sint16)r_u16(0x80104E3A + settings);
                tone = (uint32)(sint32)(sint16)r_u16(0x80104E3E + settings);
                tone = ((program << 4) + tone) * 32 + r_u32(0x80105618);
                amplitude *= r_u8(tone + 2);
                amplitude = ScaleSequenceControlUnsigned(amplitude, 0x040C2051, 13);
                left_level = amplitude * r_u16(record + 88);
                right_level = amplitude * r_u16(record + 90);
                left_level = ScaleSequenceControlUnsigned(left_level, 0x02040811, 6);
                right_level = ScaleSequenceControlUnsigned(right_level, 0x02040811, 6);
                pan = r_u8(tone + 3);
                if (pan < 64)
                    right_level = ScaleSequenceControlUnsigned(right_level * pan, 0x04104105, 5);
                else
                    left_level = ScaleSequenceControlUnsigned(left_level * (127 - pan), 0x04104105, 5);
                value = (uint32)(sint32)(sint16)r_u16(0x80104E3C + index * 54);
                pan = r_u8(r_u32(0x8010560C) + (value << 4) + 4);
                if (pan < 64)
                {
                    value = (right_level & 0xFFFF) * pan;
                    right_level = (MultiplyControlSignedHigh(value, 0x82082083) + value) >> 5;
                }
                else
                    left_level = ScaleSequenceControlSigned((left_level & 0xFFFF) * (127 - pan), 0x82082083, 5);
                pan = r_u8(0x80104E32 + index * 54);
                if (pan < 64)
                {
                    value = (right_level & 0xFFFF) * pan;
                    right_level = (MultiplyControlSignedHigh(value, 0x82082083) + value) >> 5;
                }
                else
                    left_level = ScaleSequenceControlSigned((left_level & 0xFFFF) * (127 - pan), 0x82082083, 5);
                if ((sint16)r_u16(0x80105608) == 1)
                {
                    if ((left_level & 0xFFFF) < (right_level & 0xFFFF))
                        left_level = right_level;
                    else
                        right_level = left_level;
                }
                value = (left_level & 0xFFFF) * (left_level & 0xFFFF);
                left_level = ScaleSequenceControlSigned(value, 0x80020009, 13);
                value = (right_level & 0xFFFF) * (right_level & 0xFFFF);
                right_level = ScaleSequenceControlSigned(value, 0x80020009, 13);
                w_u16(0x80105358 + index * 16, (uint16)left_level);
                w_u16(0x8010535A + index * 16, (uint16)right_level);
                w_u8(0x80105338 + index, r_u8(0x80105338 + index) | 3);
            }
            ++voice;
        } while ((sint16)voice < (sint8)r_u8(0x8010561C));
    }
    return (uint32)(sint32)(sint16)sequence;
}

extern "C" uint32 apocalypse_sequence_play(uint32 sequence, uint32 mode, uint32 loops)
{
    uint32 table, record, value0, value1, value2, flags_record, result = 1;
    sequence = (uint32)(sint32)(sint16)sequence;
    mode = (uint32)(sint32)(sint8)mode;
    loops = (uint32)(sint32)(sint16)loops;
    table = 0x80104590 + sequence * 4;
    record = ReadSequenceControlRecord(table, 0);
    value0 = r_u32(record + 4);
    value1 = r_u32(record + 4);
    value2 = r_u32(record + 4);
    w_u32(record, value0);
    w_u32(record + 8, value1);
    w_u32(record + 12, value2);
    flags_record = ReadSequenceControlRecord(table, 0);
    w_u32(flags_record + 152, r_u32(flags_record + 152) & ~0x200);
    flags_record = ReadSequenceControlRecord(table, 0);
    w_u32(flags_record + 152, r_u32(flags_record + 152) & ~4);
    w_u8(record + 32, (uint8)loops);
    if (mode == 1)
    {
        uint32 left, right;
        flags_record = ReadSequenceControlRecord(table, 0);
        w_u32(flags_record + 152, r_u32(flags_record + 152) | 1);
        left = r_u16(record + 88);
        w_u8(record + 20, (uint8)mode);
        right = r_u16(record + 90);
        w_u8(record + 33, 0);
        return SetSequenceVoiceVolumes(sequence, left, right);
    }
    if (!mode)
    {
        flags_record = ReadSequenceControlRecord(table, 0);
        result = r_u32(flags_record + 152) | 2;
        w_u32(flags_record + 152, result);
    }
    return result;
}



extern "C" uint32 apocalypse_sequence_volume(uint32 sequence, uint32 left, uint32 right)
{
    uint32 record;
    sequence = (uint32)(sint32)(sint16)sequence;
    record = ReadSequenceControlRecord(0x80104590 + sequence * 4, 0);
    if (r_u32(record + 152) == 1)
        return SetSequenceVoiceVolumes(sequence, left & 0xFFFF, right & 0xFFFF);
    w_u16(record + 88, (uint16)left);
    w_u16(record + 90, (uint16)right);
    return 1;
}
static uint32 StopAndResetSequence(sint16 sequence)
{
    uint32 table, displacement = 0, record, result;
    sint32 track = 0, count;
    FUNCTION_MARKER(0x8008DD9C, "SLUS_003.73");
    SetSequenceVoiceVolumes((uint32)(sint32)sequence, 0, 0);
    StopSequenceVoices((uint32)(sint32)sequence);
    result = r_u32(0x80104588) & ~(1u << ((uint32)(sint32)sequence & 31));
    count = (sint16)r_u16(0x80104E12);
    w_u32(0x80104588, result);
    if (count <= 0)
        return result;
    table = 0x80104590 + (uint32)(sint32)sequence * 4;
    do
    {
        w_u32(displacement + r_u32(table) + 152, 0);
        w_u8(displacement + r_u32(table) + 34, 255);
        w_u8(displacement + r_u32(table) + 35, 0);
        w_u16(displacement + r_u32(table) + 72, 0);
        w_u16(displacement + r_u32(table) + 74, 0);
        w_u32(displacement + r_u32(table) + 156, 0);
        w_u32(displacement + r_u32(table) + 160, 0);
        w_u16(displacement + r_u32(table) + 76, 0);
        w_u32(displacement + r_u32(table) + 172, 0);
        w_u32(displacement + r_u32(table) + 168, 0);
        w_u32(displacement + r_u32(table) + 164, 0);
        w_u16(displacement + r_u32(table) + 78, 0);
        record = displacement + r_u32(table);
        ++track;
        w_u16(record + 88, 127);
        record = displacement + r_u32(table);
        count = (sint16)r_u16(0x80104E12);
        displacement += 176;
        result = track < count;
        w_u16(record + 90, 127);
    } while (result);
    return result;
}



extern "C" uint32 sub_8008DF18(uint32 sequence)
{
    FUNCTION_MARKER(0x8008DF18, "SLUS_003.73");
    return StopAndResetSequence((sint16)sequence);
}

extern "C" uint32 sub_8008D51C(uint32 sequence, uint32 track)
{
    FUNCTION_MARKER(0x8008D51C, "SLUS_003.73");
    return apocalypse_sequence_tick((uint32)(sint32)(sint16)sequence, (uint32)(sint32)(sint16)track);
}

extern "C" void sub_800905AC(void)
{
    FUNCTION_MARKER(0x800905AC, "SLUS_003.73");
    w_u16(0x801055C0, 0);
}

extern "C" uint32 sub_80094544(void)
{
    FUNCTION_MARKER(0x80094544, "SLUS_003.73");
    sint32 result;
    result = 1;
    if ((r_u32(0x80104584) == 1))
        return 255;
    w_u32(0x80104584, 1);
    return result;
}

extern "C" uint32 sub_80094570(void)
{
    FUNCTION_MARKER(0x80094570, "SLUS_003.73");
    sint32 result;
    result = 1;
    if (!r_u32(0x80104584))
        return 255;
    w_u32(0x80104584, 0);
    return result;
}

extern "C" uint32 sub_8008F16C(uint32 table, uint32 sequences, uint32 tracks)
{
    FUNCTION_MARKER(0x8008F16C, "SLUS_003.73");
    sint32 sequence_count = (sint16)sequences;
    sint32 track_count = (sint16)tracks;
    sint32 sequence, track;
    uint32 result;
    w_u16(0x80104E10, sequences);
    w_u16(0x80104E12, tracks);
    for (sequence = 0; sequence < sequence_count; ++sequence)
        w_u32(0x80104590 + (uint32)sequence * 4, table + (uint32)sequence * (uint32)track_count * 176);
    for (sequence = sequence_count; sequence < 32; ++sequence)
        w_u32(0x80104588, r_u32(0x80104588) | (1u << ((uint32)sequence & 31)));
    result = (uint32)(sint32)(sint16)r_u16(0x80104E10);
    for (sequence = 0; sequence < (sint16)r_u16(0x80104E10); ++sequence)
    {
        for (track = 0; track < (sint16)r_u16(0x80104E12); ++track)
        {
            uint32 entry = r_u32(0x80104590 + (uint32)sequence * 4) + (uint32)track * 176;
            w_u32(entry + 152, 0);
            w_u8(entry + 34, 255);
            w_u8(entry + 35, 0);
            w_u16(entry + 72, 0);
            w_u16(entry + 74, 0);
            w_u32(entry + 156, 0);
            w_u32(entry + 160, 0);
            w_u16(entry + 76, 0);
            w_u32(entry + 172, 0);
            w_u32(entry + 168, 0);
            w_u32(entry + 164, 0);
            w_u16(entry + 78, 0);
            w_u16(entry + 88, 127);
            w_u16(entry + 90, 127);
            w_u16(entry + 92, 127);
            w_u16(entry + 94, 127);
        }
        result = sequence + 1 < (sint16)r_u16(0x80104E10);
    }
    return result;
}
