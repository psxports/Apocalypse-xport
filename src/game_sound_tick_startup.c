#include "draft_first_signatures.h"
#include "game_spu_startup.h"
#include "game_sound_tick_startup.h"
#include "psx_spu.h"
#include <stdio.h>
#include <stdlib.h>

static uint32 timer_events[3];

uint32 sub_8008ED0C(uint32 counter, uint32 target, uint32 mode)
{
    return (uint32)SetRCnt(counter, (uint16)target, mode);
}

uint32 sub_8008EE44(uint32 counter)
{
    return (uint32)ResetRCnt(counter);
}

uint32 sub_8008658C(uint32 slot, uint32 callback)
{
    uint32 previous, mask, bit;
    sint32 event;
    if (slot >= 11u)
    {
        fprintf(stderr, "Invalid InterruptCallback slot: %u\n", slot);
        abort();
    }
    previous = r_u32(0x800F71D8u + slot * 4u);
    if (previous == callback || r_u16(0x800F71D4u) == 0u)
        return previous;
    mask = r_u16(0x1F801074u);
    w_u16(0x1F801074u, 0u);
    bit = 1u << slot;
    w_u32(0x800F71D8u + slot * 4u, callback);
    if (callback)
    {
        mask |= bit;
        w_u16(0x800F7204u, r_u16(0x800F7204u) | bit);
    }
    else
    {
        mask &= ~bit;
        w_u16(0x800F7204u, r_u16(0x800F7204u) & ~bit);
    }
    if (slot == 0u)
    {
        ChangeClearPAD(callback == 0u);
        ChangeClearRCnt(3u, callback == 0u);
        VSyncCallbackPSX(callback);
    }
    else if (slot >= 4u && slot <= 6u)
    {
        uint32 index = slot - 4u;
        ChangeClearRCnt(index, callback == 0u);
        if (timer_events[index])
        {
            CloseEventPSX(timer_events[index]);
            timer_events[index] = 0u;
        }
        if (callback)
        {
            event = OpenEventPSX(0xF2000000u + index, 2u, 0x1000u, callback);
            if (event == -1)
            {
                fprintf(stderr, "Cannot allocate sound timer event\n");
                abort();
            }
            timer_events[index] = (uint32)event;
            EnableEventPSX((uint32)event);
        }
    }
    else
    {
        /* TODO Bind remaining interrupt sources when selected by the game */
        fprintf(stderr, "Unbound InterruptCallback source: %u\n", slot);
        abort();
    }
    w_u16(0x1F801074u, mask);
    return previous;
}

uint32 sub_8009013C(uint32 type)
{
    uint32 magnitude = type;
    uint32 clear = (sint16)type < 0;
    SpuReverbAttr attribute;
    SpuReverbControl state;
    if (clear)
        magnitude = 0u - type;
    if ((uint16)magnitude >= 10u)
        return 0xFFFFFFFFu;
    w_u32(0x80105528u, 1u);
    w_u32(0x8010552Cu, (uint32)(sint32)(sint16)(magnitude | (clear ? 0x100u : 0u)));
    if ((sint16)magnitude == 0)
    {
        SpuSetReverb(0);
        w_u32(0x800FD1D0u, 0u);
    }
    apocalypse_spu_bind_reverb_presets();
    attribute.mask = r_u32(0x80105528u);
    attribute.mode = (sint32)r_u32(0x8010552Cu);
    attribute.depth.left = (sint16)r_u16(0x80105530u);
    attribute.depth.right = (sint16)r_u16(0x80105532u);
    attribute.delay = (sint32)r_u32(0x80105534u);
    attribute.feedback = (sint32)r_u32(0x80105538u);
    if (SpuSetReverbModeParam(&attribute) == 0)
    {
        spu_reverb_export(&state);
        w_u32(0x800FD1E0u, (uint32)state.mode);
        w_u32(0x800FD1D8u, state.work_address);
        w_u16(0x800FD1E4u, (uint16)state.depth_left);
        w_u16(0x800FD1E6u, (uint16)state.depth_right);
        w_u32(0x800FD1E8u, (uint32)state.delay);
        w_u32(0x800FD1ECu, (uint32)state.feedback);
    }
    return (uint32)(sint32)(sint16)magnitude;
}

static void music_missing_branch(uint32 target, uint32 sequence, uint32 track)
{
    fprintf(stderr, "TODO Music tick %08X sequence=%d track=%d\n", target, (sint32)sequence, (sint32)track);
    abort();
}

static uint32 music_track_flags(uint32 sequence, uint32 offset)
{
    return r_u32(r_u32(0x80104590u + sequence * 4u) + offset + 152u);
}

uint32 apocalypse_music_tick(void)
{
    sint32 sequence = 0, track;
    uint32 result = r_u32(0x80104584u), offset;
    uint32 sequence_arg, track_arg;
    if (result == 1u)
        return result;
    w_u32(0x80104584u, 1u);
    apocalypse_ss_flush();
    result = (uint32)(sint32)(sint16)r_u16(0x80104E10u);
    if ((sint32)result > 0)
    {
        do
        {
            if (r_u32(0x80104588u) & (1u << ((uint32)sequence & 31u)))
            {
                track = 0;
                offset = 0u;
                if ((sint16)r_u16(0x80104E12u) > 0)
                {
                    do
                    {
                        sequence_arg = (uint32)(sint32)(sint16)sequence;
                        track_arg = (uint32)(sint32)(sint16)track;
                        if (music_track_flags((uint32)sequence, offset) & 1u)
                        {
                            sub_8008D51C(sequence_arg, track_arg);
                            if (music_track_flags((uint32)sequence, offset) & 0x10u)
                                music_missing_branch(0x8008D26Cu, sequence_arg, track_arg);
                            if (music_track_flags((uint32)sequence, offset) & 0x20u)
                                music_missing_branch(0x8008D26Cu, sequence_arg, track_arg);
                            if (music_track_flags((uint32)sequence, offset) & 0x40u)
                                music_missing_branch(0x8008F69Cu, sequence_arg, track_arg);
                            if (music_track_flags((uint32)sequence, offset) & 0x80u)
                                music_missing_branch(0x8008F69Cu, sequence_arg, track_arg);
                        }
                        if (music_track_flags((uint32)sequence, offset) & 2u)
                            music_missing_branch(0x8008D47Cu, sequence_arg, track_arg);
                        if (music_track_flags((uint32)sequence, offset) & 8u)
                            music_missing_branch(0x8008DD3Cu, sequence_arg, track_arg);
                        if (music_track_flags((uint32)sequence, offset) & 4u)
                        {
                            music_missing_branch(0x8008EE7Cu, sequence_arg, track_arg);
                            w_u32(r_u32(0x80104590u + (uint32)sequence * 4u) + offset + 152u, 0u);
                        }
                        ++track;
                        offset += 176u;
                    }
                    while (track < (sint16)r_u16(0x80104E12u));
                }
            }
            ++sequence;
            result = sequence < (sint16)r_u16(0x80104E10u);
        }
        while (result);
    }
    w_u32(0x80104584u, 0u);
    return result;
}

uint32 apocalypse_music_callback(uint32 target)
{
    if (target == 0x8008CFFCu)
        return apocalypse_music_tick();
    if (target == 0x8008EC6Cu)
    {
        uint32 previous = r_u32(0x800FD068u);
        if (previous)
            apocalypse_dispatch_music_target(previous);
        return apocalypse_dispatch_music_target(r_u32(0x800FD064u));
    }
    if (target == 0x8008ECB8u)
    {
        if (r_u32(0x800FD070u))
        {
            uint32 callback = r_u32(0x800FD064u);
            w_u32(0x800FD070u, 0u);
            return apocalypse_dispatch_music_target(callback);
        }
        w_u32(0x800FD070u, 1u);
        return 1u;
    }
    fprintf(stderr, "Unbound music callback: %08X\n", target);
    abort();
}
