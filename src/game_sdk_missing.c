#include "xport.h"
#include "psx.h"
#include "game_spu_startup.h"
#include "game_gpu_present.h"
#include <stdio.h>
#include <stdlib.h>

/* Original 800878DC installs the five packed rotation words */
void sub_800878DC(uint32 matrix)
{
    uint32 index;
    for (index = 0; index < 5u; ++index)
        xport_gte_write_control(index, r_u32(matrix + index * 4u));
}

void sub_8008798C(uint32 red, uint32 green, uint32 blue)
{
    SetBackColor((sint32)red, (sint32)green, (sint32)blue);
}

void sub_800879AC(uint32 red, uint32 green, uint32 blue)
{
    SetFarColor((sint32)red, (sint32)green, (sint32)blue);
}

/* TODO Bind missing SDK services to reviewed host contracts */
uint32 sub_8008632C(uint32 mode)
{
    return (uint32)VSync((sint32)mode);
}

uint32 sub_8008655C(void)
{
    extern uint32 apocalypse_reset_callbacks(void);
    return apocalypse_reset_callbacks();
}

uint32 sub_800865EC(uint32 callback)
{
    return VSyncCallbackPSX(callback);
}

uint32 sub_80087104()
{
    fprintf(stderr, "Unimplemented SDK: InitGeom at 80087104\n");
    abort();
}

void sub_8008793C(uint32 matrix)
{
    uint32 words[5];
    uint32 index;
    for (index = 0; index < 5u; ++index)
        words[index] = r_u32(matrix + index * 4u);
    for (index = 0; index < 5u; ++index)
        xport_gte_write_control(16u + index, words[index]);
}

uint32 sub_8008796C()
{
    fprintf(stderr, "Unimplemented SDK: SetTransMatrix at 8008796C\n");
    abort();
}

uint32 sub_80087E5C(uint32 destination, uint32 x, uint32 y, uint32 width, uint32 height)
{
    SetDefDrawEnv((DRAWENV *)psx_addr(destination, sizeof(DRAWENV)), (sint16)x, (sint16)y, (sint16)width, (sint32)height);
    w_u8(destination + 23u, (sint32)height < (r_u32(0x800FCE4Cu) != 0u ? 289 : 257));
    return destination;
}

uint32 sub_80087F10(uint32 destination, uint32 x, uint32 y, uint32 width, uint32 height)
{
    SetDefDispEnv((DISPENV *)psx_addr(destination, sizeof(DISPENV)), (sint16)x, (sint16)y, (sint16)width, (sint16)height);
    return destination;
}

uint32 sub_80087F7C(uint32 mode)
{
    return (uint32)apocalypse_reset_graph((sint32)mode);
}

uint32 sub_800880F0(uint32 level)
{
    return (uint32)SetGraphDebug((sint32)level);
}

uint32 sub_80088260()
{
    fprintf(stderr, "Unimplemented SDK: SetDispMask at 80088260\n");
    abort();
}

uint32 sub_800882F8(uint32 mode)
{
    return (uint32)DrawSync((sint32)mode);
}

uint32 sub_80088B28(uint32 environment)
{
    PutDispEnv((DISPENV *)psx_addr(environment, sizeof(DISPENV)));
    return environment;
}

uint32 sub_8008895C(uint32 environment)
{
    uint32 index;
    if (r_u8(0x800FCEAEu) >= 2u)
        printf("PutDrawEnv(%08x)...\n", environment);
    PutDrawEnv((DRAWENV *)psx_addr(environment, sizeof(DRAWENV)));
    for (index = 0u; index < 92u; ++index)
        w_u8(0x800FCEBCu + index, r_u8(environment + index));
    return environment;
}

uint32 sub_800888EC(uint32 ordering_table)
{
    if (r_u8(0x800FCEAEu) >= 2u)
        printf("DrawOTag(%08x)...\n", ordering_table);
    DrawOTag((uint32 *)psx_addr(ordering_table, 4u));
    /* Native submission uses the original immediate queue return contract */
    return 0u;
}

uint32 sub_8008BF34()
{
    fprintf(stderr, "Unimplemented SDK: puts at 8008BF34\n");
    abort();
}

uint32 sub_8008BF9C(uint32 format, uint32 value)
{
    const char *text = (const char *)psx_addr(format, 1u);
    return (uint32)printf(text, (long)(sint32)value);
}

uint32 sub_8008E01C(void)
{
    return apocalypse_ss_init();
}

uint32 sub_8008E93C(uint32 serial, uint32 attribute, uint32 value)
{
    return apocalypse_ss_serial_attr(serial, attribute, value);
}

uint32 sub_8008F05C(uint32 serial, uint32 left, uint32 right)
{
    return apocalypse_ss_serial_volume(serial, left, right);
}

uint32 sub_800900AC(uint32 left, uint32 right)
{
    SpuReverbAttr attribute = {0};
    sint32 left_depth = (32767 * (sint32)(sint16)left) / 127;
    sint32 right_depth = (32767 * (sint32)(sint16)right) / 127;
    w_u32(0x80105528u, 6u);
    w_u16(0x80105530u, (uint16)left_depth);
    w_u16(0x80105532u, (uint16)right_depth);
    /* Preserve the depth-only SpuSetReverbModeParam guest state */
    w_u16(r_u32(0x800FD23Cu) + 388u, r_u16(0x80105530u));
    w_u16(0x800FD1E4u, r_u16(0x80105530u));
    w_u16(r_u32(0x800FD23Cu) + 390u, r_u16(0x80105532u));
    w_u16(0x800FD1E6u, r_u16(0x80105532u));
    attribute.mask = 6u;
    attribute.depth.left = (sint16)r_u16(0x80105530u);
    attribute.depth.right = (sint16)r_u16(0x80105532u);
    SpuSetReverbDepth(&attribute);
    return 0u;
}

uint32 sub_800945BC(uint32 mode)
{
    return apocalypse_spu_init(mode);
}

uint32 sub_80094F40()
{
    fprintf(stderr, "Unimplemented SDK: _spu_Fw at 80094F40\n");
    abort();
}

uint32 sub_800968DC(uint32 mode)
{
    return apocalypse_spu_clear_reverb(mode);
}

uint32 sub_80097DD0()
{
    fprintf(stderr, "Unimplemented SDK: CdSyncCallback at 80097DD0\n");
    abort();
}

uint32 sub_80097DE4()
{
    fprintf(stderr, "Unimplemented SDK: CdReadyCallback at 80097DE4\n");
    abort();
}

uint32 sub_80098214()
{
    fprintf(stderr, "Unimplemented SDK: CdDataCallback at 80098214\n");
    abort();
}

uint32 sub_80098238()
{
    fprintf(stderr, "Unimplemented SDK: CdDataSync at 80098238\n");
    abort();
}

uint32 sub_8009986C()
{
    fprintf(stderr, "Unimplemented SDK: CD_getsector at 8009986C\n");
    abort();
}

uint32 sub_8009A7E8()
{
    fprintf(stderr, "Unimplemented SDK: CDREAD_OBJ_32C at 8009A7E8\n");
    abort();
}

uint32 sub_8009C788()
{
    fprintf(stderr, "Unimplemented SDK: DsDataCallback at 8009C788\n");
    abort();
}

uint32 sub_8009D878()
{
    fprintf(stderr, "Unimplemented SDK: _padSendAtLoadInfo at 8009D878\n");
    abort();
}

uint32 sub_8009DB54()
{
    fprintf(stderr, "Unimplemented SDK: PADCMD_OBJ_2F8 at 8009DB54\n");
    abort();
}

uint32 sub_8009DED8()
{
    fprintf(stderr, "Unimplemented SDK: PADCMD_OBJ_67C at 8009DED8\n");
    abort();
}

uint32 sub_8009E054()
{
    fprintf(stderr, "Unimplemented SDK: PADCMD_OBJ_7F8 at 8009E054\n");
    abort();
}

uint32 sub_8009E108()
{
    fprintf(stderr, "Unimplemented SDK: _padCmdParaMode at 8009E108\n");
    abort();
}

uint32 sub_8009EA80()
{
    fprintf(stderr, "Unimplemented SDK: PADPORTM_OBJ_2E4 at 8009EA80\n");
    abort();
}

uint32 sub_8009EB90()
{
    fprintf(stderr, "Unimplemented SDK: PADPORTM_OBJ_3F4 at 8009EB90\n");
    abort();
}

uint32 sub_8009EBD4()
{
    fprintf(stderr, "Unimplemented SDK: PADPORTM_OBJ_438 at 8009EBD4\n");
    abort();
}

uint32 sub_8009ECE4()
{
    fprintf(stderr, "Unimplemented SDK: PADPORTM_OBJ_548 at 8009ECE4\n");
    abort();
}

uint32 sub_8009F92C()
{
    fprintf(stderr, "Unimplemented SDK: setRC2wait at 8009F92C\n");
    abort();
}

uint32 sub_8009F94C()
{
    fprintf(stderr, "Unimplemented SDK: chkRC2wait at 8009F94C\n");
    abort();
}

uint32 sub_800936AC(uint32 count)
{
    if (((count - 1u) & 255u) >= 24u)
        return 0xFFFFFFFFu;
    w_u8(0x8010561Cu, count);
    return (uint32)(sint32)(sint8)count;
}

uint32 sub_8008F16C(uint32 table, uint32 sequences, uint32 tracks)
{
    sint32 sequence_count = (sint16)sequences;
    sint32 track_count = (sint16)tracks;
    sint32 sequence, track;
    uint32 result;
    w_u16(0x80104E10u, sequences);
    w_u16(0x80104E12u, tracks);
    for (sequence = 0; sequence < sequence_count; ++sequence)
        w_u32(0x80104590u + (uint32)sequence * 4u, table + (uint32)sequence * (uint32)track_count * 176u);
    for (sequence = sequence_count; sequence < 32; ++sequence)
        w_u32(0x80104588u, r_u32(0x80104588u) | (1u << ((uint32)sequence & 31u)));
    result = (uint32)(sint32)(sint16)r_u16(0x80104E10u);
    for (sequence = 0; sequence < (sint16)r_u16(0x80104E10u); ++sequence)
    {
        for (track = 0; track < (sint16)r_u16(0x80104E12u); ++track)
        {
            uint32 entry = r_u32(0x80104590u + (uint32)sequence * 4u) + (uint32)track * 176u;
            w_u32(entry + 152u, 0u);
            w_u8(entry + 34u, 255u);
            w_u8(entry + 35u, 0u);
            w_u16(entry + 72u, 0u);
            w_u16(entry + 74u, 0u);
            w_u32(entry + 156u, 0u);
            w_u32(entry + 160u, 0u);
            w_u16(entry + 76u, 0u);
            w_u32(entry + 172u, 0u);
            w_u32(entry + 168u, 0u);
            w_u32(entry + 164u, 0u);
            w_u16(entry + 78u, 0u);
            w_u16(entry + 88u, 127u);
            w_u16(entry + 90u, 127u);
            w_u16(entry + 92u, 127u);
            w_u16(entry + 94u, 127u);
        }
        result = sequence + 1 < (sint16)r_u16(0x80104E10u);
    }
    return result;
}
