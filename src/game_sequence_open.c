#include "psx.h"
#include <stdio.h>
#include <stdlib.h>

uint32 apocalypse_sequence_delta(uint32 record)
{
    uint32 cursor = r_u32(record), value = r_u8(cursor), byte;
    w_u32(record, cursor + 1u);
    if (!value)
        return 0u;
    if (value & 128u)
    {
        value &= 127u;
        do
        {
            cursor = r_u32(record);
            value <<= 7;
            byte = r_u8(cursor);
            w_u32(record, cursor + 1u);
            value += byte & 127u;
        } while (byte & 128u);
    }
    value *= 10u;
    w_u32(record + 136u, r_u32(record + 136u) + value);
    return value;
}

static uint32 sequence_read_byte(uint32 record)
{
    uint32 cursor = r_u32(record), value = r_u8(cursor);
    w_u32(record, cursor + 1u);
    return value;
}

static uint32 sequence_divide(uint32 numerator, uint32 denominator)
{
    if (!denominator)
    {
        fprintf(stderr, "Sequence header division by zero\n");
        abort();
    }
    return numerator / denominator;
}

static sint32 sequence_initialize(uint32 slot, sint32 bank, uint32 data)
{
    static const uint8 cleared_bytes[] = {24,25,30,26,27,31,23};
    static const uint8 later_bytes[] = {28,29,21,22};
    uint32 record = r_u32(0x80104590u + slot * 4u), index, first, tempo, rate;
    uint32 resolution, ticks, product, denominator, quotient, remainder, cursor;
    if (!record || (record & 3u) || (record & 0x1FFFFFFFu) > PSX_DRAM_SIZE - 176u)
    {
        fprintf(stderr, "Missing native sequence table initialization for slot %u\n", slot);
        abort();
    }
    w_u8(record + 38u, bank);
    w_u16(record + 80u, 0u);
    for (index = 0u; index < sizeof(cleared_bytes); ++index)
        w_u8(record + cleared_bytes[index], 0u);
    w_u32(record + 132u, 0u);
    w_u32(record + 136u, 0u);
    w_u32(record + 140u, 0u);
    w_u16(record + 86u, 0u);
    w_u8(record + 33u, 0u);
    w_u8(record + 32u, 1u);
    w_u8(record + 20u, 0u);
    w_u32(record + 144u, 0u);
    for (index = 0u; index < sizeof(later_bytes); ++index)
        w_u8(record + later_bytes[index], 0u);
    w_u16(record + 128u, 0u);
    w_u8(record + 36u, 0u);
    w_u8(record + 37u, 0u);
    for (index = 0u; index < 16u; ++index)
    {
        w_u8(record + 55u + index, index);
        w_u8(record + 39u + index, 64u);
        w_u16(record + 96u + index * 2u, 127u);
    }
    w_u16(record + 82u, 1u);
    w_u32(record, data);
    first = r_u8(data);
    if (first != 'S' && first != 'p')
    {
        printf("This is an old SEQ Data Format.\n");
        return 0;
    }
    w_u32(record, data + 7u);
    first = r_u8(data + 7u);
    w_u32(record, data + 8u);
    if (first != 1u)
    {
        printf("This is not SEQ Data.\n");
        return -1;
    }
    first = sequence_read_byte(record);
    resolution = sequence_read_byte(record) | (first << 8);
    w_u16(record + 80u, resolution);
    first = sequence_read_byte(record) << 16;
    first |= sequence_read_byte(record) << 8;
    tempo = first | sequence_read_byte(record);
    rate = sequence_divide(60000000u, tempo);
    remainder = 60000000u % tempo;
    w_u32(record + 140u, tempo);
    if ((sint32)(tempo >> 1) < (sint32)remainder)
        ++rate;
    w_u32(record + 140u, rate);
    w_u32(record + 148u, r_u32(record + 140u));
    first = sequence_read_byte(record);
    w_u8(record + 36u, first);
    first = sequence_read_byte(record);
    w_u8(record + 37u, first);
    first = apocalypse_sequence_delta(record);
    product = (uint32)(sint32)(sint16)r_u16(record + 80u) * r_u32(record + 140u);
    cursor = r_u32(record);
    w_u32(record + 8u, cursor);
    ticks = r_u32(0x8010458Cu);
    w_u32(record + 132u, first);
    w_u32(record + 144u, first);
    w_u32(record + 16u, 0u);
    w_u32(record + 12u, cursor);
    w_u32(record + 4u, cursor);
    denominator = 60u * ticks;
    if (10u * product < denominator)
    {
        quotient = sequence_divide(600u * ticks, product);
        w_u16(record + 82u, quotient);
        w_u16(record + 84u, quotient);
    }
    else
    {
        product = (uint32)(sint32)(sint16)r_u16(record + 80u) * r_u32(record + 140u);
        quotient = sequence_divide(10u * product, denominator);
        product = (uint32)(sint32)(sint16)r_u16(record + 80u) * r_u32(record + 140u);
        remainder = (10u * product) % denominator;
        w_u16(record + 82u, 65535u);
        w_u16(record + 84u, quotient);
        if (30u * ticks < remainder)
            w_u16(record + 84u, quotient + 1u);
    }
    w_u16(record + 86u, r_u16(record + 84u));
    return 0;
}

sint32 apocalypse_sequence_open(uint32 sequence, uint32 bank)
{
    uint32 mask = r_u32(0x80104588u), slot;
    sint32 result;
    if (mask != 0xFFFFFFFFu)
        for (slot = 0u; slot < 31u; ++slot)
            if (!(mask & (1u << slot)))
            {
                w_u32(0x80104588u, r_u32(0x80104588u) | (1u << slot));
                result = sequence_initialize(slot, (sint16)bank, sequence);
                return (sint16)result == -1 ? -1 : (sint32)(sint16)slot;
            }
    printf("Can't Open Sequence data any more\n\n");
    return -1;
}
