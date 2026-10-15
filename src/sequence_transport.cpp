#include "sequence_transport.h"
extern "C" {
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
}

static uint32 QueueSequenceVoiceOff(void)
{
    uint32 voice, low, high, row, pending_low, pending_high, active, result;
    FUNCTION_MARKER(0x80091FAC, "SLUS_003.73");
    voice = r_u16(0x80105638);
    low = voice < 16 ? (uint32)1 << (voice & 31) : 0;
    high = voice < 16 ? 0 : (uint32)1 << ((voice - 16) & 31);
    row = 0x80104E28 + voice * 54;
    w_u8(row + 29, 0);
    pending_low = r_u16(0x80105520);
    pending_high = r_u16(0x80105522);
    w_u16(row + 4, 0);
    w_u16(row, 0);
    active = r_u16(0x80104E18);
    pending_low |= low;
    w_u16(0x80105520, pending_low);
    w_u16(0x80104E18, active & ~pending_low);
    active = r_u16(0x80104E1A);
    pending_high |= high;
    w_u16(0x80105522, pending_high);
    result = active & ~pending_high;
    w_u16(0x80104E1A, result);
    return result;
}

uint32 StopSequenceVoices(uint32 identity)
{
    uint32 voice = 0, bit, protection, result;
    sint32 count, owner;
    FUNCTION_MARKER(0x80092EA4, "SLUS_003.73");
    count = (sint8)r_u8(0x8010561C);
    if (count <= 0)
        return (uint32)count;
    identity = (uint32)(sint32)(sint16)identity;
    do
    {
        protection = r_u32(0x800FD01C);
        bit = (uint32)1 << ((voice & 255) & 31);
        if (!(protection & bit))
        {
            owner = (sint16)r_u16(0x80104E38 + (voice & 255) * 54);
            if ((uint32)owner == identity)
            {
                w_u16(0x80105638, voice & 255);
                QueueSequenceVoiceOff();
            }
        }
        ++voice;
        count = (sint8)r_u8(0x8010561C);
        result = (sint32)(voice & 255) < count;
    } while (result);
    return result;
}

uint32 SaveSequenceVolume(uint32 identity, uint32 left, uint32 right)
{
    uint32 record, value;
    FUNCTION_MARKER(0x80092DB0, "SLUS_003.73");
    record = r_u32(0x80104590 + (identity & 255) * 4);
    w_u16(0x80105634, identity);
    record += ((identity & 0xFF00) >> 8) * 176;
    value = r_u16(record + 88);
    w_u16(left, value);
    value = r_u16(record + 90);
    w_u16(right, value);
    return (uint32)(sint32)(sint16)r_u16(0x80105634);
}

uint32 RequestSequenceStop(uint32 sequence, uint32 track)
{
    uint32 table, displacement, identity, record, flags;
    FUNCTION_MARKER(0x8008E56C, "SLUS_003.73");
    table = 0x80104590 + (uint32)(sint32)(sint16)sequence * 4;
    displacement = (uint32)(sint32)(sint16)track * 176;
    identity = (uint32)(sint32)(sint16)(sequence | (track << 8));
    record = r_u32(table) + displacement;
    SaveSequenceVolume(identity, record + 92, record + 94);
    record = r_u32(table) + displacement;
    flags = r_u32(record + 152);
    w_u32(record + 152, flags & ~1);
    record = r_u32(table) + displacement;
    flags = r_u32(record + 152);
    w_u32(record + 152, flags & ~8);
    record = r_u32(table) + displacement;
    flags = r_u32(record + 152) | 2;
    w_u32(record + 152, flags);
    return flags;
}

uint32 StopSequenceTick(uint32 sequence, uint32 track)
{
    uint32 table, displacement, identity, record, flags;
    FUNCTION_MARKER(0x8008D47C, "SLUS_003.73");
    table = 0x80104590 + (uint32)(sint32)(sint16)sequence * 4;
    displacement = (uint32)(sint32)(sint16)track * 176;
    identity = (uint32)(sint32)(sint16)(sequence | (track << 8));
    record = r_u32(table) + displacement;
    StopSequenceVoices(identity);
    w_u8(record + 20, 0);
    record = r_u32(table) + displacement;
    flags = r_u32(record + 152) & ~2;
    w_u32(record + 152, flags);
    return flags;
}

uint32 ReplaySequenceTick(uint32 sequence, uint32 track)
{
    uint32 table, displacement, record, flags;
    FUNCTION_MARKER(0x8008DD3C, "SLUS_003.73");
    table = 0x80104590 + (uint32)(sint32)(sint16)sequence * 4;
    displacement = (uint32)(sint32)(sint16)track * 176;
    record = r_u32(table) + displacement;
    w_u8(record + 20, 1);
    record = r_u32(table) + displacement;
    flags = r_u32(record + 152);
    w_u32(record + 152, flags & ~8);
    return record;
}

uint32 RequestSequenceReplay(uint32 sequence, uint32 track)
{
    uint32 table, displacement, record, flags, result;
    FUNCTION_MARKER(0x8008E72C, "SLUS_003.73");
    table = 0x80104590 + (uint32)(sint32)(sint16)sequence * 4;
    displacement = (uint32)(sint32)(sint16)track * 176;
    record = r_u32(table) + displacement;
    flags = r_u32(record + 152);
    result = flags & 0x100;
    if (!(flags & 0x204))
    {
        result = (uint32)-3;
        if (!(flags & 0x100))
        {
            w_u32(record + 152, flags & ~2);
            record = r_u32(table) + displacement;
            flags = r_u32(record + 152) | 8;
            w_u32(record + 152, flags);
            record = r_u32(table) + displacement;
            result = r_u32(record + 152) | 1;
            w_u32(record + 152, result);
        }
    }
    return result;
}

uint32 StopMusicSequence(void)
{
    uint32 sequence;
    FUNCTION_MARKER(0x8006A3C4, "SLUS_003.73");
    sequence = (uint32)(sint32)(sint16)r_u16(0x800FF6AC);
    RequestSequenceStop(sequence, 0);
    w_u32(0x800FF6B4, 1);
    w_u32(0x800FF6B8, 0);
    return 1;
}

extern "C" uint32 sub_8006A3C4(void)
{
    FUNCTION_MARKER(0x8006A3C4, "SLUS_003.73");
    return StopMusicSequence();
}

extern "C" uint32 apocalypse_sequence_stop_tick(uint32 sequence, uint32 track)
{
    return StopSequenceTick(sequence, track);
}

extern "C" uint32 apocalypse_sequence_replay_tick(uint32 sequence, uint32 track)
{
    return ReplaySequenceTick(sequence, track);
}

extern "C" uint32 apocalypse_sequence_replay(uint32 sequence)
{
    return RequestSequenceReplay((uint32)(sint32)(sint16)sequence, 0);
}

void SetMusicSequenceFadeStep(uint32 step)
{
    FUNCTION_MARKER(0x8006A490, "SLUS_003.73");
    w_u32(0x800FF6BC, step & 65535);
}

extern "C" void sub_8006A490(uint32 step)
{
    FUNCTION_MARKER(0x8006A490, "SLUS_003.73");
    SetMusicSequenceFadeStep(step);
}

uint32 StopSequenceImmediately(uint32 sequence, uint32 track)
{
    uint32 table, displacement, record, flags, identity, first, second, tempo, start, replay, index;
    static const uint32 fields[13] = {28, 24, 25, 30, 26, 27, 31, 23, 33, 28, 29, 21, 22};
    FUNCTION_MARKER(0x8008EE7C, "SLUS_003.73");
    table = 0x80104590 + (uint32)(sint32)(sint16)sequence * 4;
    displacement = (uint32)(sint32)(sint16)track * 176;
    record = r_u32(table) + displacement;
    first = record;
    flags = r_u32(record + 152);
    w_u32(record + 152, flags & ~1);
    record = r_u32(table) + displacement;
    flags = r_u32(record + 152);
    w_u32(record + 152, flags & ~2);
    record = r_u32(table) + displacement;
    flags = r_u32(record + 152);
    w_u32(record + 152, flags & ~8);
    record = r_u32(table) + displacement;
    flags = r_u32(record + 152);
    w_u32(record + 152, flags & ~0x400);
    record = r_u32(table) + displacement;
    flags = r_u32(record + 152);
    identity = (uint32)(sint32)(sint16)(sequence | (track << 8));
    w_u32(record + 152, flags | 4);
    StopSequenceVoices(identity);
    sub_800905AC();
    second = r_u32(first + 132);
    record = r_u32(first + 140);
    tempo = r_u16(first + 86);
    start = r_u32(first + 4);
    replay = r_u32(first + 4);
    w_u8(first + 20, 0);
    w_u32(first + 136, 0);
    for (index = 0; index < 13; ++index)
        w_u8(first + fields[index], 0);
    w_u32(first + 144, second);
    w_u32(first + 148, record);
    w_u16(first + 84, tempo);
    w_u32(first, start);
    w_u32(first + 8, replay);
    for (index = 0; index < 16; ++index)
    {
        w_u8(first + 55 + index, index);
        w_u8(first + 39 + index, 64);
        w_u16(first + 96 + index * 2, 127);
    }
    w_u16(first + 92, 127);
    w_u16(first + 94, 127);
    return 127;
}

uint32 StartChainedSequence(uint32 sequence, uint32 track)
{
    uint32 table, displacement, record, flags_record, result;
    static const uint32 masks[5] = {0x100, 8, 2, 4, 0x200};
    FUNCTION_MARKER(0x8008DC3C, "SLUS_003.73");
    table = 0x80104590 + (uint32)(sint32)(sint16)sequence * 4;
    displacement = (uint32)(sint32)(sint16)track * 176;
    record = r_u32(table) + displacement;
    w_u8(record + 32, 1);
    w_u8(record + 33, 0);
    for (uint32 index = 0; index < 5; ++index)
    {
        flags_record = r_u32(table) + displacement;
        w_u32(flags_record + 152, r_u32(flags_record + 152) & ~masks[index]);
    }
    result = r_u32(record + 4);
    w_u8(record + 20, 1);
    w_u32(record, result);
    flags_record = r_u32(table) + displacement;
    result = r_u32(flags_record + 152) | 1;
    w_u32(flags_record + 152, result);
    return result;
}
