#include "game_sdk_bindings.h"
#include <stdio.h>
#include <stdlib.h>
extern "C"
{
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
}

extern uint32 CompleteCdReadDma(void);

static void NativeCdDmaCallback(void)
{
    CompleteCdReadDma();
}

extern "C" uint32 sub_80097DD0(uint32 callback)
{
    FUNCTION_MARKER(0x80097DD0, "SLUS_003.73");
    return CdSyncCallbackPSX(callback);
}

extern "C" uint32 sub_80097DE4(uint32 callback)
{
    FUNCTION_MARKER(0x80097DE4, "SLUS_003.73");
    return CdReadyCallbackPSX(callback);
}

extern "C" uint32 sub_80098214(uint32 callback)
{
    FUNCTION_MARKER(0x80098214, "SLUS_003.73");
    uint32 previous = r_u32(0x800F82BC);
    if (callback && callback != 0x8009A71C)
    {
        GameDiagnostic("Missing native CD DMA callback adapter: %08X\n", callback);
        abort();
    }
    CdDataCallback(callback ? NativeCdDmaCallback : 0);
    w_u32(0x800F82BC, callback);
    return previous;
}

extern "C" uint32 sub_80098238(uint32 mode)
{
    FUNCTION_MARKER(0x80098238, "SLUS_003.73");
    return (uint32)CdDataSync((sint32)mode);
}

extern "C" uint32 sub_8009C788(uint32 callback)
{
    FUNCTION_MARKER(0x8009C788, "SLUS_003.73");
    return sub_80098214(callback);
}

extern "C" uint32 sub_8008BF34(uint32 text)
{
    FUNCTION_MARKER(0x8008BF34, "SLUS_003.73");
    return (uint32)GameDiagnostic("%s\n", (const char *)psx_addr(text, 1));
}

extern "C" uint32 sub_80094F40(uint32 data, uint32 bytes)
{
    FUNCTION_MARKER(0x80094F40, "SLUS_003.73");
    // The existing bound SPU transfer implements DMA and programmed writes
    SpuWrite(psx_addr(data, bytes), bytes);
    return bytes;
}

extern "C" uint32 sub_8009986C(uint32 destination, uint32 words)
{
    FUNCTION_MARKER(0x8009986C, "SLUS_003.73");
    // Native CD transfers complete synchronously through the shared sector FIFO
    if (words > 585)
        return 0xFFFFFFFF;
    return CdGetSector(psx_addr(destination, words * 4), (sint32)words) ? 0 : 0xFFFFFFFF;
}

extern "C" uint32 sub_8009A7E8(uint32 reset_position)
{
    FUNCTION_MARKER(0x8009A7E8, "SLUS_003.73");
    sub_80097DD0(0);
    sub_80097DE4(0);
    if (r_u32(0x800FDD80) & 1)
        sub_80098214(0);
    if (r_u8(0x800FDA34) & 0x10)
    {
        if (!(sub_8008632C(0xFFFFFFFF) & 63))
            sub_8008BF34(0x800A485C);
        sub_80097F34(1, 0);
        w_u32(0x800FDD6C, sub_8008632C(0xFFFFFFFF));
        w_u32(0x800FDD64, 0xFFFFFFFF);
        return r_u32(0x800FDD64);
    }
    if (reset_position)
    {
        sub_8008BF34(0x800A4874);
        sub_80097DF8(9, 0, 0);
        if (!sub_80097DF8(2, 0x800FDA40, 0))
        {
            w_u32(0x800FDD64, 0xFFFFFFFF);
            return r_u32(0x800FDD64);
        }
    }
    CdFlush();
    w_u8(0x800FDD06, 0);
    w_u8(0x800FDD05, 0);
    w_u8(0x800FDD04, 2);
    uint8 mode = (uint8)r_u32(0x800FDD5C);
    if ((mode != r_u8(0x800FDA44) || reset_position) && !xport_draft_host_sub_80097DF8_p2(14, &mode, 0))
    {
        w_u32(0x800FDD64, 0xFFFFFFFF);
        return r_u32(0x800FDD64);
    }
    w_u32(0x800FDD70, sub_8009835C(0x800FDA40));
    sub_80097DE4(0x8009A4BC);
    if (r_u32(0x800FDD80) & 1)
        sub_80098214(0x8009A71C);
    w_u32(0x800FDD58, r_u32(0x800FDD54));
    sub_80097F34(6, 0);
    w_u32(0x800FDD64, r_u32(0x800FDD50));
    w_u32(0x800FDD68, sub_8008632C(0xFFFFFFFF));
    return r_u32(0x800FDD64);
}

extern "C" uint32 sub_800981F4(uint32 destination, uint32 words)
{
    FUNCTION_MARKER(0x800981F4, "SLUS_003.73");
    // The original asynchronous DMA is synchronous at the native CD boundary
    uint32 success = sub_8009986C(destination, words) == 0;
    if (success && r_u32(0x800F82BC) == 0x8009A71C)
        NativeCdDmaCallback();
    return success;
}


