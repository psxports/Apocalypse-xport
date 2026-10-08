#include "game_str_vlc.h"

/* Original stream state remains resumable at DC boundaries */
typedef struct StrVlcState
{
    uint32 source, output, bits, consumed, quantization, block;
    uint32 predictor[3];
} StrVlcState;

static void vlc_consume(StrVlcState *state, uint32 count)
{
    uint32 total = state->consumed + count;
    state->bits <<= count & 31u;
    state->consumed = total & 15u;
    if (total & 16u)
    {
        state->bits |= (uint32)r_u16(state->source) << state->consumed;
        state->source += 2u;
    }
}

static void vlc_escape(StrVlcState *state)
{
    uint32 value;
    w_u16(state->output, state->bits >> 16);
    state->output += 2u;
    value = r_u16(state->source);
    state->source += 2u;
    state->bits = (state->bits << 16) | (value << state->consumed);
}

static uint32 vlc_suspend(const StrVlcState *state)
{
    w_u32(0x800FDFB0u, state->source);
    w_u32(0x800FDFB4u, state->output);
    w_u32(0x800FDFB8u, state->bits);
    w_u32(0x800FDFBCu, state->consumed);
    w_u32(0x800FDFC0u, state->quantization);
    w_u32(0x800FDFC4u, state->block);
    w_u32(0x800FDFC8u, state->predictor[0]);
    w_u32(0x800FDFCCu, state->predictor[1]);
    w_u32(0x800FDFD0u, state->predictor[2]);
    return 1u;
}

uint32 apocalypse_str_vlc_decode(uint32 source, uint32 destination, uint32 table)
{
    StrVlcState state;
    uint32 chunk_limit, table_base = table + 2048u;
    uint32 first_code, additional, coefficient, count, index;
    uint32 dc_table, dc_prefix, dc_width, delta, leading, header;
    if (source)
    {
        state.block = 0u;
        state.predictor[0] = state.predictor[1] = state.predictor[2] = 0u;
        chunk_limit = destination + 2u * r_u32(0x800FDFACu);
        header = r_u32(source);
        state.quantization = (uint32)r_u16(source + 4u) << 10;
        if (r_u16(source + 6u) >= 3u) state.block = 1u;
        state.bits = ((uint32)r_u16(source + 8u) << 16) | r_u16(source + 10u);
        state.source = source + 12u;
        state.consumed = 0u;
        w_u32(destination, header);
        w_u32(0x800FDFD4u, destination + 4u * (header & 65535u) + 4u);
        state.output = destination + 2u;
        goto decode_dc;
    }
    state.source = r_u32(0x800FDFB0u);
    state.output = r_u32(0x800FDFB4u);
    state.bits = r_u32(0x800FDFB8u);
    state.consumed = r_u32(0x800FDFBCu);
    state.quantization = r_u32(0x800FDFC0u);
    state.block = r_u32(0x800FDFC4u);
    state.predictor[0] = r_u32(0x800FDFC8u);
    state.predictor[1] = r_u32(0x800FDFCCu);
    state.predictor[2] = r_u32(0x800FDFD0u);
    chunk_limit = state.output + 2u * r_u32(0x800FDFACu);

decode_ac:
    index = table_base + 8u * (state.bits >> 19);
    first_code = r_u32(index);
    if (first_code)
        additional = r_u32(index + 4u);
    else
    {
        vlc_consume(&state, 8u);
        first_code = r_u32(table_base + 65536u + 4u * (state.bits >> 23));
        additional = 0u;
    }
    vlc_consume(&state, first_code & 255u);
    coefficient = first_code >> 16;
    if (coefficient == 0x7C1Fu)
    {
        vlc_escape(&state);
        goto decode_ac;
    }
    w_u16(state.output, coefficient);
    if (coefficient == 0xFE00u) goto decode_dc;
    state.output += 2u;
    if (!additional) goto decode_ac;
    coefficient = additional & 65535u;
    if (coefficient == 0x7C1Fu)
    {
        vlc_escape(&state);
        goto decode_ac;
    }
    w_u16(state.output, coefficient);
    if (coefficient == 0xFE00u) goto decode_dc;
    state.output += 2u;
    coefficient = additional >> 16;
    if (!coefficient) goto decode_ac;
    if (coefficient == 0x7C1Fu)
    {
        vlc_escape(&state);
        goto decode_ac;
    }
    w_u16(state.output, coefficient);
    if (coefficient == 0xFE00u) goto decode_dc;
    state.output += 2u;
    goto decode_ac;

decode_dc:
    coefficient = state.bits >> 22;
    state.output += 2u;
    if (coefficient == (state.block ? 0x3FFu : 0x1FFu)) goto finished;
    if (state.block)
    {
        dc_table = table_base - (state.block >= 3u ? 2048u : 1024u);
        index = dc_table + 4u * (state.bits >> 24);
        dc_prefix = r_u16(index);
        dc_width = r_u16(index + 2u);
        state.bits <<= dc_prefix & 31u;
        delta = 0u;
        count = state.consumed;
        if (dc_width)
        {
            delta = state.bits >> ((32u - dc_width) & 31u);
            leading = state.bits & 0x80000000u;
            state.bits <<= dc_width & 31u;
            if (!leading) delta -= 0xFFFFFFFFu >> ((32u - dc_width) & 31u);
            count += dc_width;
        }
        count += dc_prefix;
        state.consumed = count & 15u;
        if (count & 16u)
        {
            state.bits |= (uint32)r_u16(state.source) << state.consumed;
            state.source += 2u;
        }
        index = state.block > 2u ? 2u : state.block - 1u;
        state.predictor[index] += delta;
        coefficient = state.quantization | ((state.predictor[index] << 2) & 1023u);
        ++state.block;
        if (state.block == 7u) state.block = 1u;
    }
    else
    {
        vlc_consume(&state, 10u);
        coefficient |= state.quantization;
    }
    w_u16(state.output, coefficient);
    leading = state.output;
    state.output += 2u;
    if ((sint32)(leading - chunk_limit) >= 0) return vlc_suspend(&state);
    goto decode_ac;

finished:
    chunk_limit = r_u32(0x800FDFD4u);
    while ((sint32)(state.output - chunk_limit) < 0)
    {
        w_u16(state.output, 0xFE00u);
        state.output += 2u;
    }
    /* Native C does not require the original CP0 cache-control write */
    return 0u;
}
