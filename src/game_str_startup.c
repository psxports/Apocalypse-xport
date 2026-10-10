#include "game_movie_startup.h"
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
#include "psx_stream.h"
#include <stdio.h>
#include <stdlib.h>

static uint32 ring_address, ring_bytes;
static uint32 active;

uint32 sub_80097B1C(uint32 ring, uint32 count)
{
    ring_address = ring;
    ring_bytes = count * 2048u;
    StSetRing((uint32 *)psx_addr(ring, ring_bytes), count);
    return 0;
}

void sub_8009AEEC(uint32 mode, uint32 start, uint32 end, uint32 callback, uint32 argument)
{
    if (callback || argument)
    {
        fprintf(stderr, "TODO STR guest callbacks %08X %08X\n", callback, argument);
        abort();
    }
    StSetStream(mode, start, end, NULL, NULL);
}

uint32 apocalypse_str_start(uint32 mode)
{
    uint8 value = (uint8)mode;
    if (!xport_draft_host_sub_80097DF8_p2(14, &value, 0))
        return 0;
    stream_set_read_mode(mode);
    active = xport_draft_host_sub_80097DF8_p2(27, NULL, 0);
    return active;
}

uint32 apocalypse_str_pump(void)
{
    return active ? (uint32)stream_pump() : 0u;
}

uint32 apocalypse_str_next(uint32 *frame, uint32 *header)
{
    uint32 *native_frame, *native_header;
    uint8 *ring = (uint8 *)psx_addr(ring_address, ring_bytes);
    if (StGetNext(&native_frame, &native_header))
        return 1;
    *frame = ring_address + (uint32)((uint8 *)native_frame - ring);
    *header = ring_address + (uint32)((uint8 *)native_header - ring);
    return 0;
}

uint32 apocalypse_str_free(uint32 frame)
{
    return StFreeRing((uint32 *)psx_addr(frame, 4));
}

void apocalypse_str_stop(void)
{
    active = 0;
    StUnSetRing();
    w_u32(0x800FDA28u, 0);
}
