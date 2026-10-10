#include "game_mdec.h"
#include "draft_first_signatures.h"
#include "psx_press.h"
#include <stdio.h>
#include <stdlib.h>

static uint32 output_callback;

static void movie_output_complete(void)
{
    sub_8002F84C();
}

void sub_8009BC3C(uint32 mode)
{
    DecDCTReset((sint32)mode);
}

uint32 sub_8009BED8(uint32 callback)
{
    uint32 previous = output_callback;
    if (callback && callback != 0x8002F84Cu)
    {
        fprintf(stderr, "TODO MDEC output callback %08X\n", callback);
        abort();
    }
    DecDCToutCallback(callback ? movie_output_complete : NULL);
    output_callback = callback;
    return previous;
}

void sub_8009BDA0(uint32 input, uint32 mode)
{
    uint32 command = r_u32(input);
    DecDCTin((uint32 *)psx_addr(input, 4u + (command & 0xffffu) * 4u), (sint32)mode);
}

void sub_8009BE1C(uint32 output, uint32 words)
{
    DecDCTout((uint32 *)psx_addr(output, (words & ~31u) * 4u), (sint32)words);
}
