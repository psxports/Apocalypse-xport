#include "game_sdk_bindings.h"
extern "C"
{
#include "game_movie_startup.h"
#include "game_mdec.h"
#include "game_str_vlc.h"
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
#include "psx_press.h"
#include "psx_stream.h"
#include "psx_spu.h"
}
#include <stdio.h>
#include <stdlib.h>

static uint32 ring_address, ring_bytes;
static uint32 active;

extern "C" uint32 sub_80097B1C(uint32 ring, uint32 count)
{
    FUNCTION_MARKER(0x80097B1C, "SLUS_003.73");
    ring_address = ring;
    ring_bytes = count * 2048;
    StSetRing((uint32 *)psx_addr(ring, ring_bytes), count);
    return 0;
}

extern "C" void sub_8009AEEC(uint32 mode, uint32 start, uint32 end, uint32 callback, uint32 argument)
{
    FUNCTION_MARKER(0x8009AEEC, "SLUS_003.73");
    if (callback || argument)
    {
        GameDiagnostic("TODO STR guest callbacks %08X %08X\n", callback, argument);
        abort();
    }
    StSetStream(mode, start, end, 0, 0);
}

extern "C" uint32 apocalypse_str_start(uint32 mode)
{
    uint8 value = (uint8)mode;
    if (!xport_draft_host_sub_80097DF8_p2(14, &value, 0))
        return 0;
    stream_set_read_mode(mode);
    active = xport_draft_host_sub_80097DF8_p2(27, 0, 0);
    return active;
}

extern "C" uint32 apocalypse_str_pause(void)
{
    // Preserve queued STR sectors, XA samples and decoder history
    cd_set_reading_native(0);
    active = 0;
    w_u8(0x800FDA45, 9);
    return 1;
}

extern "C" uint32 apocalypse_str_resume(uint32 mode)
{
    uint8 value = (uint8)mode;
    if (!xport_draft_host_sub_80097DF8_p2(14, &value, 0))
        return 0;
    stream_set_read_mode(mode);
    cd_set_reading_native(1);
    active = 1;
    w_u8(0x800FDA45, 27);
    return 1;
}

static uint32 PumpMovieFrame()
{
    uint32 consumed = 0;
    if (!active)
        return 0;
    for (uint32 remaining = ring_bytes / 2048; remaining; --remaining)
    {
        uint16 status = StNextStatus(0, 0);
        if (status == StCOMPLETE || status == StLOCK || stream_pump() <= 0)
            break;
        ++consumed;
    }
    return consumed;
}

extern "C" uint32 apocalypse_str_pump(void)
{
    return PumpMovieFrame();
}

extern "C" uint32 apocalypse_str_next(uint32 *frame, uint32 *header)
{
    uint32 *native_frame, *native_header;
    uint8 *ring = (uint8 *)psx_addr(ring_address, ring_bytes);
    PumpMovieFrame();
    if (StGetNext(&native_frame, &native_header))
        return 1;
    *frame = ring_address + (uint32)((uint8 *)native_frame - ring);
    *header = ring_address + (uint32)((uint8 *)native_header - ring);
    return 0;
}

extern "C" uint32 apocalypse_str_free(uint32 frame)
{
    return StFreeRing((uint32 *)psx_addr(frame, 4));
}

extern "C" void apocalypse_str_stop(void)
{
    active = 0;
    StUnSetRing();
    w_u32(0x800FDA28, 0);
}

static uint32 output_callback;

static void CompleteMovieOutput(void)
{
    sub_8002F84C();
}

extern "C" void sub_8009BC3C(uint32 mode)
{
    FUNCTION_MARKER(0x8009BC3C, "SLUS_003.73");
    DecDCTReset((sint32)mode);
}

extern "C" uint32 sub_8009BED8(uint32 callback)
{
    FUNCTION_MARKER(0x8009BED8, "SLUS_003.73");
    uint32 previous = output_callback;
    if (callback && callback != 0x8002F84C)
    {
        GameDiagnostic("TODO MDEC output callback %08X\n", callback);
        abort();
    }
    DecDCToutCallback(callback ? CompleteMovieOutput : 0);
    output_callback = callback;
    return previous;
}

extern "C" void sub_8009BDA0(uint32 input, uint32 mode)
{
    FUNCTION_MARKER(0x8009BDA0, "SLUS_003.73");
    uint32 command = r_u32(input);
    DecDCTin((uint32 *)psx_addr(input, 4 + (command & 0xffff) * 4), (sint32)mode);
}

extern "C" void sub_8009BE1C(uint32 output, uint32 words)
{
    FUNCTION_MARKER(0x8009BE1C, "SLUS_003.73");
    DecDCTout((uint32 *)psx_addr(output, (words & ~31) * 4), (sint32)words);
}

extern "C" uint32 sub_8009C65C(uint32 destination)
{
    FUNCTION_MARKER(0x8009C65C, "SLUS_003.73");
    DecDCTvlcBuild((uint16 *)psx_addr(destination, sizeof(DECDCTTAB)));
    return 1;
}

extern "C" uint32 apocalypse_str_vlc_decode(uint32 source, uint32 destination, uint32 table)
{
    return (uint32)DecDCTvlc2(source ? (uint32 *)psx_addr(source, 12) : 0, destination ? (uint32 *)psx_addr(destination, 4) : 0, (uint16 *)psx_addr(table, sizeof(DECDCTTAB)));
}


extern "C" void sub_8009B12C(uint32 a1, uint32 a2, uint32 a3)
{
    FUNCTION_MARKER(0x8009B12C, "SLUS_003.73");
    w_u32(0x80105848, a1);
    w_u32(0x80105824, a2);
    w_u32(0x80105844, a3);
}
