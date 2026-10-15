#include "game_sdk_bindings.h"
extern "C"
{
#include "psx.h"
}
#include <stdio.h>
#include <stdlib.h>

extern "C" uint32 apocalypse_sequence_delta(uint32 record)
{
    uint32 cursor = r_u32(record), value = r_u8(cursor), byte;
    w_u32(record, cursor + 1);
    if (!value)
        return 0;
    if (value & 128)
    {
        value &= 127;
        do
        {
            cursor = r_u32(record);
            value <<= 7;
            byte = r_u8(cursor);
            w_u32(record, cursor + 1);
            value += byte & 127;
        } while (byte & 128);
    }
    value *= 10;
    w_u32(record + 136, r_u32(record + 136) + value);
    return value;
}

static uint32 SequenceReadByte(uint32 record)
{
    uint32 cursor = r_u32(record), value = r_u8(cursor);
    w_u32(record, cursor + 1);
    return value;
}

static uint32 SequenceDivide(uint32 numerator, uint32 denominator)
{
    if (!denominator)
    {
        GameDiagnostic("Sequence header division by zero\n");
        abort();
    }
    return numerator / denominator;
}

static sint32 SequenceInitialize(uint32 slot, sint32 bank, uint32 data)
{
    static const uint8 cleared_bytes[] = {24, 25, 30, 26, 27, 31, 23};
    static const uint8 later_bytes[] = {28, 29, 21, 22};
    uint32 record = r_u32(0x80104590 + slot * 4), index, first, tempo, rate;
    uint32 resolution, ticks, product, denominator, quotient, remainder, cursor;
    if (!record || (record & 3) || (record & 0x1FFFFFFF) > PSX_DRAM_SIZE - 176)
    {
        GameDiagnostic("Missing native sequence table initialization for slot %u\n", slot);
        abort();
    }
    w_u8(record + 38, bank);
    w_u16(record + 80, 0);
    for (index = 0; index < sizeof(cleared_bytes); ++index)
        w_u8(record + cleared_bytes[index], 0);
    w_u32(record + 132, 0);
    w_u32(record + 136, 0);
    w_u32(record + 140, 0);
    w_u16(record + 86, 0);
    w_u8(record + 33, 0);
    w_u8(record + 32, 1);
    w_u8(record + 20, 0);
    w_u32(record + 144, 0);
    for (index = 0; index < sizeof(later_bytes); ++index)
        w_u8(record + later_bytes[index], 0);
    w_u16(record + 128, 0);
    w_u8(record + 36, 0);
    w_u8(record + 37, 0);
    for (index = 0; index < 16; ++index)
    {
        w_u8(record + 55 + index, index);
        w_u8(record + 39 + index, 64);
        w_u16(record + 96 + index * 2, 127);
    }
    w_u16(record + 82, 1);
    w_u32(record, data);
    first = r_u8(data);
    if (first != 'S' && first != 'p')
    {
        GameDiagnostic("This is an old SEQ Data Format.\n");
        return 0;
    }
    w_u32(record, data + 7);
    first = r_u8(data + 7);
    w_u32(record, data + 8);
    if (first != 1)
    {
        GameDiagnostic("This is not SEQ Data.\n");
        return -1;
    }
    first = SequenceReadByte(record);
    resolution = SequenceReadByte(record) | (first << 8);
    w_u16(record + 80, resolution);
    first = SequenceReadByte(record) << 16;
    first |= SequenceReadByte(record) << 8;
    tempo = first | SequenceReadByte(record);
    rate = SequenceDivide(60000000, tempo);
    remainder = 60000000 % tempo;
    w_u32(record + 140, tempo);
    if ((sint32)(tempo >> 1) < (sint32)remainder)
        ++rate;
    w_u32(record + 140, rate);
    w_u32(record + 148, r_u32(record + 140));
    first = SequenceReadByte(record);
    w_u8(record + 36, first);
    first = SequenceReadByte(record);
    w_u8(record + 37, first);
    first = apocalypse_sequence_delta(record);
    product = (uint32)(sint32)(sint16)r_u16(record + 80) * r_u32(record + 140);
    cursor = r_u32(record);
    w_u32(record + 8, cursor);
    ticks = r_u32(0x8010458C);
    w_u32(record + 132, first);
    w_u32(record + 144, first);
    w_u32(record + 16, 0);
    w_u32(record + 12, cursor);
    w_u32(record + 4, cursor);
    denominator = 60 * ticks;
    if (10 * product < denominator)
    {
        quotient = SequenceDivide(600 * ticks, product);
        w_u16(record + 82, quotient);
        w_u16(record + 84, quotient);
    }
    else
    {
        product = (uint32)(sint32)(sint16)r_u16(record + 80) * r_u32(record + 140);
        quotient = SequenceDivide(10 * product, denominator);
        product = (uint32)(sint32)(sint16)r_u16(record + 80) * r_u32(record + 140);
        remainder = (10 * product) % denominator;
        w_u16(record + 82, 65535);
        w_u16(record + 84, quotient);
        if (30 * ticks < remainder)
            w_u16(record + 84, quotient + 1);
    }
    w_u16(record + 86, r_u16(record + 84));
    return 0;
}

extern "C" sint32 apocalypse_sequence_open(uint32 sequence, uint32 bank)
{
    uint32 mask = r_u32(0x80104588), slot;
    sint32 result;
    if (mask != 0xFFFFFFFF)
        for (slot = 0; slot < 31; ++slot)
            if (!(mask & (1 << slot)))
            {
                w_u32(0x80104588, r_u32(0x80104588) | (1 << slot));
                result = SequenceInitialize(slot, (sint16)bank, sequence);
                return (sint16)result == -1 ? -1 : (sint32)(sint16)slot;
            }
    GameDiagnostic("Can't Open Sequence data any more\n\n");
    return -1;
}
