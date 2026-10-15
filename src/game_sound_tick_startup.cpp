#include "game_sdk_bindings.h"
extern "C"
{
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
#include "game_spu_startup.h"
#include "game_sound_tick_startup.h"
#include "psx_spu.h"
}
#include "sequence_transport.h"
#include <stdio.h>
#include <stdlib.h>

static uint32 timer_events[3];

extern "C" uint32 sub_8008ED0C(uint32 counter, uint32 target, uint32 mode)
{
    FUNCTION_MARKER(0x8008ED0C, "SLUS_003.73");
    return (uint32)SetRCnt(counter, (uint16)target, mode);
}

extern "C" uint32 sub_8008EE44(uint32 counter)
{
    FUNCTION_MARKER(0x8008EE44, "SLUS_003.73");
    return (uint32)ResetRCnt(counter);
}

extern "C" uint32 sub_8008658C(uint32 slot, uint32 callback)
{
    FUNCTION_MARKER(0x8008658C, "SLUS_003.73");
    uint32 previous, mask, bit;
    sint32 event;
    if (slot >= 11)
    {
        GameDiagnostic("Invalid InterruptCallback slot: %u\n", slot);
        abort();
    }
    previous = r_u32(0x800F71D8 + slot * 4);
    if (previous == callback || r_u16(0x800F71D4) == 0)
        return previous;
    mask = r_u16(0x1F801074);
    w_u16(0x1F801074, 0);
    bit = 1 << slot;
    w_u32(0x800F71D8 + slot * 4, callback);
    if (callback)
    {
        mask |= bit;
        w_u16(0x800F7204, r_u16(0x800F7204) | bit);
    }
    else
    {
        mask &= ~bit;
        w_u16(0x800F7204, r_u16(0x800F7204) & ~bit);
    }
    if (slot == 0)
    {
        ChangeClearPAD(callback == 0);
        ChangeClearRCnt(3, callback == 0);
        VSyncCallbackPSX(callback);
    }
    else if (slot >= 4 && slot <= 6)
    {
        uint32 index = slot - 4;
        ChangeClearRCnt(index, callback == 0);
        if (timer_events[index])
        {
            CloseEventPSX(timer_events[index]);
            timer_events[index] = 0;
        }
        if (callback)
        {
            event = OpenEventPSX(0xF2000000 + index, 2, 0x1000, callback);
            if (event == -1)
            {
                GameDiagnostic("Cannot allocate sound timer event\n");
                abort();
            }
            timer_events[index] = (uint32)event;
            EnableEventPSX((uint32)event);
        }
    }
    else
    {
        /* TODO Bind remaining interrupt sources when selected by the game */
        GameDiagnostic("Unbound InterruptCallback source: %u\n", slot);
        abort();
    }
    w_u16(0x1F801074, mask);
    return previous;
}

extern "C" uint32 sub_8009013C(uint32 type)
{
    FUNCTION_MARKER(0x8009013C, "SLUS_003.73");
    uint32 magnitude = type;
    uint32 clear = (sint16)type < 0;
    SpuReverbAttr attribute;
    SpuReverbControl state;
    if (clear)
        magnitude = 0 - type;
    if ((uint16)magnitude >= 10)
        return 0xFFFFFFFF;
    w_u32(0x80105528, 1);
    w_u32(0x8010552C, (uint32)(sint32)(sint16)(magnitude | (clear ? 0x100 : 0)));
    if ((sint16)magnitude == 0)
    {
        SpuSetReverb(0);
        w_u32(0x800FD1D0, 0);
    }
    apocalypse_spu_bind_reverb_presets();
    attribute.mask = r_u32(0x80105528);
    attribute.mode = (sint32)r_u32(0x8010552C);
    attribute.depth.left = (sint16)r_u16(0x80105530);
    attribute.depth.right = (sint16)r_u16(0x80105532);
    attribute.delay = (sint32)r_u32(0x80105534);
    attribute.feedback = (sint32)r_u32(0x80105538);
    if (SpuSetReverbModeParam(&attribute) == 0)
    {
        spu_reverb_export(&state);
        w_u32(0x800FD1E0, (uint32)state.mode);
        w_u32(0x800FD1D8, state.work_address);
        w_u16(0x800FD1E4, (uint16)state.depth_left);
        w_u16(0x800FD1E6, (uint16)state.depth_right);
        w_u32(0x800FD1E8, (uint32)state.delay);
        w_u32(0x800FD1EC, (uint32)state.feedback);
    }
    return (uint32)(sint32)(sint16)magnitude;
}

static uint32 TickSequenceVolume(uint32 sequence, uint32 track)
{
    FUNCTION_MARKER(0x8008D26C, "SLUS_003.73");
    uint32 table = 0x80104590 + (uint32)(sint32)(sint16)sequence * 4;
    uint32 offset = (uint32)(sint32)(sint16)track * 176;
    uint32 record = r_u32(table) + offset;
    uint32 elapsed = r_u32(record + 160) + 1;
    sint32 duration = (sint32)r_u32(record + 156);
    uint32 identity = (uint32)(sint32)(sint16)(sequence | (track << 8));
    w_u32(record + 160, elapsed);
    if (duration < (sint32)elapsed)
    {
        uint32 current = r_u32(table) + offset;
        w_u32(current + 152, r_u32(current + 152) & ~0x10);
    }
    else
    {
        uint32 product = (uint32)(sint32)(sint16)r_u16(record + 72) * elapsed;
        if (!duration || (duration == -1 && product == 0x80000000))
        {
            // Original DIV guards raise BREAK rather than inventing a volume
            GameDiagnostic("Invalid sequence volume ramp division\n");
            abort();
        }
        sint32 ramp = (sint32)product / duration;
        sint32 delta = (sint32)((uint32)ramp - (uint32)(sint32)(sint16)r_u16(record + 74));
        if (delta)
        {
            uint32 volume_record = r_u32(0x80104590 + (identity & 255) * 4) + ((identity & 0xFF00) >> 8) * 176;
            w_u16(record + 74, (uint32)ramp);
            // The first volume read writes native locals instead of guest saved-register storage
            w_u16(0x80105634, identity);
            sint32 left = (sint32)(r_u16(volume_record + 88) + (uint32)delta);
            sint32 right = (sint32)(r_u16(volume_record + 90) + (uint32)delta);
            if (left >= 128) left = 127;
            if (left < 0) left = 0;
            if (right >= 128) right = 127;
            if (right < 0) right = 0;
            SetSequenceVoiceVolumes(identity, (uint32)left, (uint32)right);
            if ((left == 127 && right == 127) || (!left && !right))
            {
                uint32 current = r_u32(table) + offset;
                w_u32(current + 152, r_u32(current + 152) & ~0x10);
            }
        }
    }
    return SaveSequenceVolume(identity, record + 92, record + 94);
}

static uint32 TickSequenceTempo(uint32 sequence, uint32 track)
{
    FUNCTION_MARKER(0x8008F69C, "SLUS_003.73");
    uint32 table = 0x80104590 + (uint32)(sint32)(sint16)sequence * 4;
    uint32 offset = (uint32)(sint32)(sint16)track * 176;
    uint32 record = r_u32(table) + offset;
    uint32 remaining = r_u32(record + 168) - 1;
    w_u32(record + 168, remaining);
    if ((sint32)remaining >= 0)
    {
        sint32 step = (sint16)r_u16(record + 78);
        uint32 tempo = r_u32(record + 148), target = r_u32(record + 172);
        if (step > 0)
        {
            uint32 remainder = remaining % (uint32)step;
            if (remainder)
                return remainder;
            if (target < tempo) w_u32(record + 148, tempo - 1);
            else if (tempo < target) w_u32(record + 148, tempo + 1);
        }
        else if (target < tempo)
        {
            tempo += (uint32)step;
            w_u32(record + 148, tempo);
            if (tempo < target) w_u32(record + 148, target);
        }
        else if (tempo < target)
        {
            tempo -= (uint32)step;
            w_u32(record + 148, tempo);
            if (target < tempo) w_u32(record + 148, target);
        }
        uint32 numerator = (uint32)(sint32)(sint16)r_u16(record + 80) * r_u32(record + 148) * 10;
        uint32 denominator = r_u32(0x8010458C) * 60;
        if (!denominator)
        {
            GameDiagnostic("Invalid sequence tempo division\n");
            abort();
        }
        uint32 ticks = numerator / denominator;
        w_u16(record + 84, ticks);
        if ((sint32)(ticks << 16) <= 0)
            w_u16(record + 84, 1);
        if (r_u32(record + 168) && r_u32(record + 148) != r_u32(record + 172))
            return r_u32(record + 172);
    }
    record = r_u32(table) + offset;
    w_u32(record + 152, r_u32(record + 152) & ~0x40);
    record = r_u32(table) + offset;
    uint32 result = r_u32(record + 152) & ~0x80;
    w_u32(record + 152, result);
    return result;
}

static uint32 ReadMusicTrackFlags(uint32 sequence, uint32 offset)
{
    return r_u32(r_u32(0x80104590 + sequence * 4) + offset + 152);
}

extern "C" uint32 apocalypse_music_tick(void)
{
    sint32 sequence = 0, track;
    uint32 result = r_u32(0x80104584), offset;
    uint32 sequence_arg, track_arg;
    if (result == 1)
        return result;
    w_u32(0x80104584, 1);
    apocalypse_ss_flush();
    result = (uint32)(sint32)(sint16)r_u16(0x80104E10);
    if ((sint32)result > 0)
    {
        do
        {
            if (r_u32(0x80104588) & (1u << ((uint32)sequence & 31)))
            {
                track = 0;
                offset = 0;
                if ((sint16)r_u16(0x80104E12) > 0)
                {
                    do
                    {
                        sequence_arg = (uint32)(sint32)(sint16)sequence;
                        track_arg = (uint32)(sint32)(sint16)track;
                        if (ReadMusicTrackFlags((uint32)sequence, offset) & 1)
                        {
                            sub_8008D51C(sequence_arg, track_arg);
                            if (ReadMusicTrackFlags((uint32)sequence, offset) & 0x10)
                                TickSequenceVolume(sequence_arg, track_arg);
                            if (ReadMusicTrackFlags((uint32)sequence, offset) & 0x20)
                                TickSequenceVolume(sequence_arg, track_arg);
                            if (ReadMusicTrackFlags((uint32)sequence, offset) & 0x40)
                                TickSequenceTempo(sequence_arg, track_arg);
                            if (ReadMusicTrackFlags((uint32)sequence, offset) & 0x80)
                                TickSequenceTempo(sequence_arg, track_arg);
                        }
                        if (ReadMusicTrackFlags((uint32)sequence, offset) & 2)
                            apocalypse_sequence_stop_tick(sequence_arg, track_arg);
                        if (ReadMusicTrackFlags((uint32)sequence, offset) & 8)
                            apocalypse_sequence_replay_tick(sequence_arg, track_arg);
                        if (ReadMusicTrackFlags((uint32)sequence, offset) & 4)
                        {
                            StopSequenceImmediately(sequence_arg, track_arg);
                            w_u32(r_u32(0x80104590 + (uint32)sequence * 4) + offset + 152, 0);
                        }
                        ++track;
                        offset += 176;
                    } while (track < (sint16)r_u16(0x80104E12));
                }
            }
            ++sequence;
            result = sequence < (sint16)r_u16(0x80104E10);
        } while (result);
    }
    w_u32(0x80104584, 0);
    return result;
}

extern "C" uint32 apocalypse_music_callback(uint32 target)
{
    if (target == 0x8008CFFC)
        return apocalypse_music_tick();
    if (target == 0x8008EC6C)
    {
        uint32 previous = r_u32(0x800FD068);
        if (previous)
            apocalypse_dispatch_music_target(previous);
        return apocalypse_dispatch_music_target(r_u32(0x800FD064));
    }
    if (target == 0x8008ECB8)
    {
        if (r_u32(0x800FD070))
        {
            uint32 callback = r_u32(0x800FD064);
            w_u32(0x800FD070, 0);
            return apocalypse_dispatch_music_target(callback);
        }
        w_u32(0x800FD070, 1);
        return 1;
    }
    GameDiagnostic("Unbound music callback: %08X\n", target);
    abort();
}
