#include "game_pad_poll.h"
#include "game_movie_startup.h"
#include "game_str_vlc.h"
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
#include <stdlib.h>
#include "game_mdec.h"
extern uint32 apocalypse_str_pump(void);
#include <stdio.h>
#include "game_cd_startup.h"
#include "game_sound_tick_startup.h"
#include "game_vram_startup.h"

/* Unverified draft C; ABI and adapters remain TODO */
void sub_800879CC(uint32 a1, uint32 a2)
{
    SetGeomOffset((sint32)a1, (sint32)a2);
}
/* TODO Missing call adapter sub_8008BF9C */
/* TODO Missing call adapter sub_80097B4C */
/* TODO Missing call adapter sub_8009AA6C */
/* TODO Missing call adapter sub_8009AB6C */
/* TODO Missing host buffer adapter xport_draft_host_sub_80097DF8_p2 */
/* TODO Missing host buffer adapter xport_draft_host_sub_8009835C_p1 */
/* TODO Missing host buffer adapter xport_draft_host_sub_80099B3C_p1 */
/* TODO Resolve original data label aRetryCdinit */
uint32 sub_8006AEE4(void)
{
    uint8 file[24];
    uint32 sectors, result;
    sint32 i;
    w_u32(0x800FF700u, 0u);
    sub_80097D14(0u);
    while (!sub_80097B4C())
    {
        fprintf(stderr, "%s", (const char *)psx_addr(0x800A3A60u, 1u));
        for (i = 0; i < 100; ++i)
            VSync(0);
    }
    while (!xport_draft_host_sub_80099B3C_p1(file, 0x800A3A70u))
    {
        for (;;)
        {
            i = 0;
            if (sub_80097B4C())
                break;
            do
            {
                VSync(0);
                ++i;
            } while (i < 100);
        }
        do
        {
            VSync(0);
            ++i;
        } while (i < 100);
    }
    sectors = (((uint32)file[4] | ((uint32)file[5] << 8) | ((uint32)file[6] << 16) | ((uint32)file[7] << 24)) + 2047u) >> 11;
    xport_draft_host_sub_80097DF8_p2(2u, file, 0u);
    sub_8009AA6C(sectors, 0x801007A8u, 128u);
    while ((sint32)sub_8009AB6C(1u, 0u) > 0)
        VSync(0);
    for (;;)
    {
        i = 0;
        if (xport_draft_host_sub_80099B3C_p1(file, 0x800A3A7Cu))
            break;
        do
        {
            VSync(0);
            ++i;
        } while (i < 100);
    }
    result = xport_draft_host_sub_8009835C_p1(file);
    w_u32(0x800FFC38u, result);
    w_u32(0x800FF710u, 0u);
    return result;
}

/* TODO Missing call adapter sub_8008BF34 */
/* TODO Missing call adapter sub_8008BF9C */
/* TODO Postincrement memory expressions may require ordering refinement */
/* TODO Resolve original data label 0x800A4578u */
/* TODO Resolve original data label 0x800A45A8u */
/* TODO Resolve original data label 0x800A456Cu */
uint32 sub_800983DC(void)
{
    uint8 response[8];
    uint32 code, count = 0u, error = 0u, i, result, destination;
    uint32 interrupt;
    w_u8(r_u32(0x800FDCECu), 1u);
    interrupt = r_u32(0x800FDCF8u);
    code = r_u8(interrupt) & 7u;
    if (!code) return 0u;
    while (code != (r_u8(interrupt) & 7u))
        code = r_u8(interrupt) & 7u;
    while (count < 8u && (r_u8(r_u32(0x800FDCECu)) & 0x20u))
        response[count++] = r_u8(r_u32(0x800FDCF0u));
    for (i = count; i < 8u; ++i) response[i] = 0u;
    w_u8(r_u32(0x800FDCECu), 1u);
    w_u8(r_u32(0x800FDCF8u), 7u);
    w_u8(r_u32(0x800FDCF4u), 7u);
    if (code != 3u || r_u32(0x800FDBECu + (uint32)r_u8(0x800FDA45u) * 4u))
    {
        if (!(r_u32(0x800FDA34u) & 0x10u) && (response[0] & 0x10u))
            w_u32(0x800FDA3Cu, r_u32(0x800FDA3Cu) + 1u);
        error = response[0] & 0x1Du;
        w_u32(0x800FDA34u, response[0]);
        w_u32(0x800FDA38u, response[1]);
    }
    if (code == 5u && (sint32)r_u32(0x800FDA30u) > 0)
    {
        fprintf(stderr, "%s", (const char *)psx_addr(0x800A456Cu, 1u));
        if ((sint32)r_u32(0x800FDA30u) > 0)
            fprintf(stderr, (const char *)psx_addr(0x800A4578u, 1u), (const char *)psx_addr(r_u32(0x800FDA4Cu + (uint32)r_u8(0x800FDA45u) * 4u), 1u), r_u32(0x800FDA34u), r_u32(0x800FDA38u));
    }
    switch (code)
    {
    case 1u:
        if (error && count == 1u) error = 0u;
        w_u8(0x800FDD05u, error ? 5u : 1u);
        for (i = 0u; i < 8u; ++i) w_u8(0x80105860u + i, response[i]);
        w_u8(r_u32(0x800FDCECu), 0u);
        w_u8(r_u32(0x800FDCF8u), 0u);
        return 4u;
    case 2u:
        w_u8(0x800FDD04u, error ? 5u : 2u);
        destination = 0x80105858u; result = 2u;
        break;
    case 3u:
        if (error)
        {
            w_u8(0x800FDD04u, 5u); result = 2u;
        }
        else if (r_u32(0x800FDAECu + (uint32)r_u8(0x800FDA45u) * 4u))
        {
            w_u8(0x800FDD04u, 3u); result = 1u;
        }
        else
        {
            w_u8(0x800FDD04u, 2u); result = 2u;
        }
        destination = 0x80105858u;
        break;
    case 4u:
        w_u8(0x800FDD06u, 4u);
        w_u8(0x800FDD05u, r_u8(0x800FDD06u));
        for (i = 0u; i < 8u; ++i) w_u8(0x80105868u + i, response[i]);
        destination = 0x80105860u; result = 4u;
        break;
    case 5u:
        w_u8(0x800FDD05u, 5u);
        w_u8(0x800FDD04u, r_u8(0x800FDD05u));
        for (i = 0u; i < 8u; ++i) w_u8(0x80105858u + i, response[i]);
        destination = 0x80105860u; result = 6u;
        break;
    default:
        /* TODO Missing SDK diagnostic context service 8008BF34 */
        fprintf(stderr, "Missing SDK diagnostic context 8008BF34(format=800A4594, code=%u)\n", code);
        abort();
    }
    for (i = 0u; i < 8u; ++i) w_u8(destination + i, response[i]);
    return result;
}

/* TODO Missing call adapter dword_800FEDF8 */
/* TODO Missing call adapter sub_8009D8FC */
/* TODO Missing call adapter v24 */
static uint32 apocalypse_agent2_pad_callback(uint32 target, uint32 receiver)
{
    /* TODO Native controller callback adapter */
    fprintf(stderr, "Missing controller callback target=%08X receiver=%08X\n", target, receiver);
    abort();
}

uint32 sub_8009F240(uint32 a1)
{
    sint32 result;
    uint32 v3;
    sint32 v4;
    sint32 v5;
    uint32 v6;
    uint32 v7;
    sint8 v8;
    uint32 v9;
    uint32 v10;
    uint32 v11;
    sint32 i;
    sint8 v13;
    sint32 v14;
    sint32 v15;
    uint32 v16;
    sint8 v17;
    sint32 v18;
    sint32 v19;
    sint32 v20;
    sint32 v21;
    sint32 v22;
    sint32 v23;
    uint32 v24;
    if ((((a1 != r_u32(((uint32)(((uint32)(a1) + (uint32)(16)))))) && r_u8(((uint32)(((uint32)(r_u32(((uint32)(((uint32)(a1) + (uint32)(60)))))) + (uint32)(1)))))) && (r_u8(((uint32)(((uint32)(r_u32(((uint32)(((uint32)(a1) + (uint32)(60)))))) + (uint32)(1))))) != 90)))
        return sub_8009F5C0(a1);
    v3 = r_u32(((uint32)(((uint32)(a1) + (uint32)(48)))));
    v4 = r_u8(((uint32)(((uint32)(a1) + (uint32)(232)))));
    v5 = (r_u8(r_u32(((uint32)(((uint32)(a1) + (uint32)(60)))))) >> 4);
    if ((v4 == 8))
    {
        w_u8(v3, 0);
    }
    else if ((v5 != 15))
    {
        v6 = r_u32(((uint32)(((uint32)(a1) + (uint32)(60)))));
        w_u8(((uint32)(((uint32)(a1) + (uint32)(232)))), v5);
        w_u8(v3, 0);
        v7 = (v3 + (1) * 1u);
        v8 = ((sint8)(r_u8(v6)));
        v9 = (v6 + (1) * 1u);
        w_u8(v7, v8);
        v10 = (v7 + (1) * 1u);
        if ((a1 == r_u32(((uint32)(((uint32)(a1) + (uint32)(16)))))))
        {
            v14 = (v5 != 8);
            v15 = 2;
            if (v14)
            {
                v16 = (v9 + (1) * 1u);
                if ((r_u8(((uint32)(((uint32)(a1) + (uint32)(68))))) > 2u))
                {
                    do
                    {
                        v17 = r_u8(((v16 += 1u) - 1u));
                        ++v15;
                        w_u8(((v10 += 1u) - 1u), v17);
                    } while ((((sint32)(v15)) < r_u8(((uint32)(((uint32)(a1) + (uint32)(68)))))));
                }
            }
            else
            {
                w_u8(((uint32)(((uint32)(a1) + (uint32)(68)))), 2);
            }
        }
        else
        {
            w_u8(((uint32)(((uint32)(a1) + (uint32)(68)))), 8);
            v11 = (v9 + (1) * 1u);
            for (i = 2; (((sint32)(i)) < 8); ++i)
            {
                v13 = r_u8(((v11 += 1u) - 1u));
                w_u8(((v10 += 1u) - 1u), v13);
            }
        }
    }
    if ((((!(r_u8(((uint32)(((uint32)(r_u32(((uint32)(((uint32)(a1) + (uint32)(60)))))) + (uint32)(1)))))) && ((r_u8(((uint32)(((uint32)(a1) + (uint32)(70))))) != 1) || r_u32(((uint32)(((uint32)(a1) + (uint32)(20))))))) && !(r_u8(((uint32)(((uint32)(a1) + (uint32)(80))))))) || ((((r_u8(((uint32)(((uint32)(a1) + (uint32)(232))))) != v4) && !(r_u8(((uint32)(((uint32)(a1) + (uint32)(74))))))) && !sub_8009F87C(a1)) && ((a1 != r_u32(((uint32)(((uint32)(a1) + (uint32)(16))))))) ? (v18 = r_u8(((uint32)(((uint32)(a1) + (uint32)(56)))))) : (v18 = r_u8(((uint32)(((uint32)(a1) + (uint32)(55)))))), !v18)))
    {
        apocalypse_agent2_pad_callback(r_u32(0x800FEDF8u), a1);
    }
    v19 = r_u8(((uint32)(((uint32)(a1) + (uint32)(70)))));
    result = 255;
    w_u8(((uint32)(((uint32)(a1) + (uint32)(74)))), 0);
    if ((v19 != 255))
    {
        if (((((unsigned char)(((uint32)(v19) - (uint32)(2)))) < 0xFCu) && (r_u8(r_u32(((uint32)(((uint32)(a1) + (uint32)(60)))))) != 243)))
            apocalypse_agent2_pad_callback(r_u32(0x800FEDF8u), a1);
        v20 = r_u8(((uint32)(((uint32)(a1) + (uint32)(70)))));
        if ((r_u8(((uint32)(((uint32)(a1) + (uint32)(70))))) && (v20 != 255)))
        {
            if ((a1 == r_u32(((uint32)(((uint32)(a1) + (uint32)(16)))))))
                result = r_u8(((uint32)(((uint32)(a1) + (uint32)(54)))));
            else
                result = r_u8(((uint32)(((uint32)(a1) + (uint32)(55)))));
            if (!result)
                return result;
            v20 = r_u8(((uint32)(((uint32)(a1) + (uint32)(70)))));
        }
        if ((v20 == 1))
        {
            v23 = r_u8(((uint32)(((uint32)(a1) + (uint32)(70)))));
            w_u8(((uint32)(((uint32)(a1) + (uint32)(71)))), 0);
            goto LABEL_52;
        }
        result = 254;
        if ((((sint32)(v20)) >= 2))
        {
            result = 255;
            if ((v20 == 254))
            {
                w_u8(((uint32)(((uint32)(a1) + (uint32)(70)))), -1);
                return result;
            }
            if ((v20 == 255))
                return result;
            v24 = ((sint32)(r_u32(((uint32)(((uint32)(a1) + (uint32)(24)))))));
            if (v24)
                result = apocalypse_agent2_pad_callback(v24, a1);
            else
                result = apocalypse_agent2_pad_callback(0x8009D8FCu, a1);
            v22 = r_u8(a1 + 70u) + (uint32)result;
            goto LABEL_58;
        }
        if (r_u8(((uint32)(((uint32)(a1) + (uint32)(232))))))
        {
            v21 = r_u32(((uint32)(((uint32)(a1) + (uint32)(12)))));
            if (v21 || (result = 8, !r_u8(a1 + 54u)))
            {
                if ((r_u8(((uint32)(((uint32)(a1) + (uint32)(232))))) == 8))
                {
                    v22 = r_u8(r_u32(((uint32)(((uint32)(v21) + (uint32)(60))))));
                    result = 2;
                    if ((v22 == 255))
                    {
                        w_u8(((uint32)(((uint32)(a1) + (uint32)(73)))), 2);
                    LABEL_58:
                        w_u8(((uint32)(((uint32)(a1) + (uint32)(70)))), v22);

                        return result;
                    }
                }
                v23 = r_u8(((uint32)(((uint32)(a1) + (uint32)(70)))));
                w_u8(((uint32)(((uint32)(a1) + (uint32)(73)))), 1);
            LABEL_52:
                result = ((uint32)(v23) + (uint32)(1));

                w_u8(((uint32)(((uint32)(a1) + (uint32)(70)))), result);
            }
        }
    }
    return result;
}

uint32 sub_800662A0(void)
{
    uint32 status, result;
    uint8 location[8];
    apocalypse_music_tick();
    if (r_u32(0x800FF2B8u))
    {
        status = sub_80097D90(1u, 0x800FF2D8u);
        if (status == 5u)
        {
            w_u32(0x800FF2B8u, 0u);
            w_u32(0x800FF2BCu, 1u);
            sub_80097F34(11u, 0u);
        }
        else if (status == 2u)
        {
            if ((sint32)r_u32(0x800FF2D4u) < (sint32)r_u32(0x800FF2D0u))
            {
                uint32 found = 0u;
                if (r_u8(0x800FDA45u) == 17u)
                {
                    status = sub_8009835C(0x800FF2DDu);
                    found = (sint32)status > 0;
                }
                if (found)
                {
                    xport_draft_host_sub_80097DF8_p2(1u, NULL, 0x800FF2D8u);
                    if (!(r_u8(0x800FF2D8u) & 0x40u))
                        w_u32(0x800FF2D4u, status);
                }
                sub_80097F34(17u, 0u);
            }
            else
            {
                w_u32(0x800FF2BCu, 1u);
                w_u32(0x800FF2B8u, 0u);
                w_u32(0x800FF2B0u, 30u);
                sub_80097F34(11u, 0u);
                xport_draft_host_sub_80098258_p2(r_u32(0x800FF2ACu), location);
                xport_draft_host_sub_80097F34_p2(22u, location);
            }
        }
    }
    else if (r_u32(0x800FF2B0u))
    {
        w_u32(0x800FF2B0u, r_u32(0x800FF2B0u) - 1u);
    }
    result = r_u32(0x800FF6CCu) - 1u;
    if (r_u32(0x800FF6CCu))
    {
        w_u32(0x800FF6CCu, result);
        if (!result)
            return sub_8006A4A0(32u, 32u);
    }
    return result;
}

uint32 sub_80015228(uint32 a1, uint32 a2, uint32 a3, uint32 a4, uint32 a5, uint32 a6, uint32 a7)
{
    sint32 v11;
    sint32 v12;
    v11 = a1;
    v12 = 0;
    w_u32(((uint32)(a1)), 0x800A05C4u);
    w_u32(((uint32)(((uint32)(a1) + (uint32)(12)))), a2);
    w_u32(((uint32)(((uint32)(a1) + (uint32)(16)))), a3);
    w_u8(((uint32)(((uint32)(a1) + (uint32)(4)))), a4);
    w_u32(((uint32)(((uint32)(a1) + (uint32)(20)))), a7);
    do
    {
        w_u16(((uint32)(((uint32)(a1) + (uint32)(46)))), 0);
        w_u16(((uint32)(((uint32)(a1) + (uint32)(48)))), 0);
        w_u8(((uint32)(((uint32)(a1) + (uint32)(38)))), -56);
        w_u8(((uint32)(((uint32)(a1) + (uint32)(39)))), -56);
        w_u8(((uint32)(((uint32)(a1) + (uint32)(40)))), -56);
        w_u8(((uint32)(((uint32)(a1) + (uint32)(41)))), 0x80);
        w_u8(((uint32)(((uint32)(a1) + (uint32)(42)))), 0x80);
        w_u8(((uint32)(((uint32)(a1) + (uint32)(43)))), 0x80);
        w_u8(((uint32)(((uint32)(a1) + (uint32)(44)))), 0x80);
        w_u16(((uint32)(((uint32)(a1) + (uint32)(32)))), a5);
        w_u16(((uint32)(((uint32)(a1) + (uint32)(34)))), a6);
        ++v12;
        a1 += 28;
    } while ((((sint32)(v12)) < 15));
    sub_800153D8(v11);
    return v11;
}

/* TODO Missing host buffer adapter xport_draft_host_sub_8009835C_p1 */
/* TODO Missing host buffer adapter xport_draft_host_sub_80099B3C_p1 */
/* TODO Resolve original data label 0x800A1998u */
uint32 sub_8002FBB4(void)
{
    sint32 result;
    char v1[24];
    xport_draft_host_sub_80099B3C_p1(v1, 0x800A1998u);
    result = xport_draft_host_sub_8009835C_p1(v1);
    w_u32(0x800FF2ACu, result);
    return result;
}

/* TODO Missing call adapter sub_8008BF9C */
/* TODO Missing host buffer adapter xport_draft_host_sub_80068450_p4 */
uint32 sub_80068450(uint32 owner, uint32 width, uint32 height, uint32 x, uint32 y, uint32 depth, uint32 flags, uint32 name)
{
    return apocalypse_allocate_vram(owner, width, height, x, y, depth, flags, name);
}

/* TODO Postincrement memory expressions may require ordering refinement */
uint32 sub_8007D76C(uint32 a1, uint32 a2, uint32 a3)
{
    sint32 result;
    w_u32(0x800FFAE8u, a1);
    w_u32(0x800FFACCu, a2);
    w_u32(0x800FFAC8u, ((uint32)(a2) + (uint32)(a3)));
    w_u32(0x800FFAECu, ((uint32)(a2) + (uint32)(a3)));
    w_u32(0x800FFAD8u, (((sint32)(((sint32)(((uint32)(((uint32)(a2) + (uint32)(a3))) * (uint32)(SquareRoot0(((uint32)(((uint32)(((uint32)(r_u32(0x800FFADCu)) * (uint32)(r_u32(0x800FFADCu)))) + (uint32)(((uint32)(r_u32(0x800FFAE0u)) * (uint32)(r_u32(0x800FFAE0u)))))) + (uint32)(((uint32)(r_u32(0x800FFAE4u)) * (uint32)(r_u32(0x800FFAE4u)))))))))))) / r_u32(0x800FFAE4u)));
    result = (a3 < 0x1000);
    if ((a3 >= 0x1001))
    {
        w_u32(0x800FFAD4u, 0);
        do
            result = (((uint32)(4096) << (uint32)(-(((sint8)((w_u32(0x800FFAD4u, (r_u32(0x800FFAD4u) - 1u)), r_u32(0x800FFAD4u))))))) < a3);
        while ((((uint32)(4096) << (uint32)(-(((sint8)(r_u32(0x800FFAD4u)))))) < a3));
    }
    else
    {
        w_u32(0x800FFAD4u, 0);
        if ((a3 < 0x1000))
        {
            do
                result = (a3 < (4096 >> (w_u32(0x800FFAD4u, (r_u32(0x800FFAD4u) + 1u)), r_u32(0x800FFAD4u))));
            while ((a3 < (4096 >> r_u32(0x800FFAD4u))));
        }
    }
    return result;
}

uint32 sub_8002EE7C(uint32 a1)
{
    sint32 result;
    unsigned char v2;
    sint32 v3;
    uint32 v4;
    short v5;
    sint32 v6;
    sint32 v7;
    sint32 v8;
    sint32 v9;
    result = r_u32(0x800FF250u);
    v2 = a1;
    if (!r_u32(0x800FF250u))
    {
        if ((a1 == 1))
        {
            v2 = 4;
        }
        else if ((a1 == 26))
        {
            v3 = sub_80066570(5);
            v2 = 0;
            if (v3)
            {
                if ((v3 == 1))
                {
                    v2 = 27;
                }
                else
                {
                    v2 = 29;
                    if ((v3 != 2))
                    {
                        v2 = 32;
                        if ((v3 == 3))
                            v2 = 30;
                    }
                }
            }
        }
        v4 = (0x800A5B8Cu + (((uint32)(7) * (uint32)(v2))) * 4u);
        v5 = r_u16((((uint32)(v4)) + (3) * 2u));
        v6 = r_u16((((uint32)(v4)) + (4) * 2u));
        v7 = ((sint32)(r_u32((v4 + (3) * 4u))));
        v8 = ((sint32)(r_u32((v4 + (4) * 4u))));
        w_u16(0x800FF290u, r_u16((((uint32)(v4)) + (2) * 2u)));
        v9 = ((sint32)(r_u32((v4 + (6) * 4u))));
        w_u8(0x800FF299u, v2);
        w_u16(0x800FF292u, v5);
        w_u32(0x800FF27Cu, v6);
        w_u32(0x800FF280u, v7);
        w_u32(0x800FF284u, v8);
        result = r_u8((((uint32)(v4)) + (20) * 1u));
        w_u32(0x800FF278u, v9);
        w_u8(0x800FF298u, result);
    }
    return result;
}

/* TODO Missing host buffer adapter xport_draft_host_sub_80097DF8_p2 */
/* TODO Missing host buffer adapter xport_draft_host_sub_80098258_p2 */
uint32 sub_8002FA48(void)
{
    sint32 result;
    char v1[8];
    char v2[8];
    v2[0] = 0x80;
    xport_draft_host_sub_80098258_p2(r_u32(0x800FF278u), v1);
    do
    {
        while (!xport_draft_host_sub_80097DF8_p2(2, v1, 0))
            ;

        while (!xport_draft_host_sub_80097DF8_p2(14, v2, 0))
            ;

        result = apocalypse_str_start(480);
    } while (!result);
    return result;
}

/* TODO Missing call adapter _byteswap_ushort */
/* TODO Missing call adapter sub_80070354 */
/* TODO Missing call adapter sub_80070410 */
/* TODO Missing call adapter sub_800706B0 */
/* TODO Postincrement memory expressions may require ordering refinement */
uint32 sub_80070748(void)
{
    return apocalypse_pad_poll();
}

uint32 sub_8007047C(void)
{
    return apocalypse_pad_normalize();
}

uint32 sub_8002F9E4(void)
{
    uint32 frame = sub_8002F990();
    if (!frame)
        return 0;
    apocalypse_str_vlc_decode(frame, r_u32(0x800FF2A4u + 4 * r_u32(0x800FF25Cu)), r_u32(0x800FFB88u));
    apocalypse_str_free(frame);
    return 1;
}

/* TODO Missing call adapter SHIWORD */
/* TODO Missing call adapter sub_8009B14C */
/* TODO Missing call adapter sub_8009BE1C */
/* TODO Missing host buffer adapter xport_draft_host_sub_800885A4_p1 */
/* TODO Postincrement memory expressions may require ordering refinement */
uint32 sub_8002F84C(void)
{
    sint16 rectangle[4];
    uint32 index, pixels, next_index;
    sint32 x, width, height, product;
    unsigned i;
    if (r_u8(0x800FF298u) && r_u32(0x8010581Cu))
    {
        apocalypse_str_pump();
        w_u32(0x8010581Cu, 0);
    }
    for (i = 0; i < 4; ++i)
        rectangle[i] = (sint16)r_u16(0x800FF29Cu + i * 2);
    index = r_u32(0x800FF258u);
    pixels = r_u32(0x800A5F68u + index * 4);
    next_index = index + 1;
    if ((sint32)next_index >= (sint32)r_u8(0x800FF288u))
        next_index = 0;
    w_u32(0x800FF258u, next_index);
    width = (sint16)r_u16(0x800FF2A0u);
    x = (sint16)((uint32)r_u16(0x800FF29Cu) + (uint32)width);
    w_u16(0x800FF29Cu, (uint16)x);
    if (x < (sint32)r_u16(0x800FF28Eu))
    {
        height = (sint16)r_u16(0x800FF2A2u);
        product = (sint32)((uint32)width * (uint32)height);
        sub_8009BE1C(r_u32(0x800A5F68u + next_index * 4), (uint32)((product + (sint32)((uint32)product >> 31)) >> 1));
    }
    else
    {
        w_u16(0x800FF29Cu, r_u16(0x800FF28Au));
        if (r_u32(0x800FF248u))
        {
            sint32 y = (sint16)r_u16(0x800FF29Eu);
            w_u16(0x800FF29Eu, (uint16)(y < 256 ? y + 256 : y - 256));
        }
    }
    return xport_draft_host_sub_800885A4_p1(rectangle, pixels);
}

void sub_8001024C(uint32 a1, uint32 a2, uint32 a3)
{
    sub_80010184();
    if (a1)
    {
        sub_80010028(a2);
        sub_80010074();
    }
    else
    {
        sub_80010138(a2);
    }
    sub_800101CC(a3);
    sub_80010184();
}

/* TODO Missing call adapter sub_8009375C */
/* TODO Missing call adapter sub_80093C4C */
uint32 apocalypse_load_sound_bank(const char *name, uint32 bank_header_output)
{
    uint32 file_size = apocalypse_resource_lookup(name);
    uint32 header = sub_8006B864(file_size + 64u, 1, 1);
    uint32 header_bytes, total_bytes;
    sint16 bank;
    sub_8006B234(header);
    sub_8006B44C();
    header_bytes = ((uint32)r_u16(header + 18u) << 9) + 0xA20u;
    total_bytes = header_bytes + ((r_u32(header + 12u) - header_bytes + 63u) & ~63u);
    w_u32(header + 12u, total_bytes);
    bank = SsVabOpenHead((uint8 *)psx_addr(header, header_bytes), -1);
    SsVabTransBody((uint8 *)psx_addr(header + header_bytes, total_bytes - header_bytes), bank);
    SsVabTransCompleted(1);
    sub_8006BD14(header, header_bytes);
    w_u32(bank_header_output, header);
    return (uint32)(sint32)bank;
}

uint32 sub_80069D2C(uint32 name, uint32 bank_header_output)
{
    return apocalypse_load_sound_bank((const char *)psx_addr(name, 1u), bank_header_output);
}

uint32 sub_8006F29C(uint32 a1, uint32 a2)
{
    sint32 v4;
    uint32 v5;
    sint32 v6;
    sint32 result;
    uint32 i;
    sint32 v9;
    uint32 v10;
    uint32 v11;
    sint32 v12;
    sint32 v13;
    sint32 v14;
    uint32 v15;
    uint32 v16;
    uint32 v17;
    sint32 v18;
    sint8 v19;
    v4 = 0;
    v5 = ((uint32)(0x800EAEF8u));
    while (1)
    {
        if (r_u8(v5))
        {
            v6 = (sub_80067724(a1, v5) != 0);
            result = v4;
            if (v6)
                return result;
        }
        ++v4;
        v5 += (64) * 1u;
        if ((((sint32)(v4)) >= 40))
        {
            for (i = 0x800EADB8u;; i += (16) * 1u)
            {
                v9 = 0;
                if ((((sint32)(((sint32)(i)))) >= ((sint32)(((sint32)(0x800EAEF8u))))))
                    break;
                v10 = i;
                if (((sint8)(r_u8((i + (13) * 1u)))))
                {
                    v11 = a1;
                    v12 = 1;
                    if (r_u8(a1))
                    {
                        while (1)
                        {
                            v13 = r_u8(((v11 += 1u) - 1u));
                            if ((v13 != ((unsigned char)(((sint8)(r_u8(v10)))))))
                                break;
                            (v10 += 1u);
                            if (!(r_u8(v11)))
                                goto LABEL_11;
                        }

                        v12 = 0;
                    }
                LABEL_11:
                    if (v12)
                    {
                        if ((((sint8)(r_u8(v10))) != 46))
                            v12 = 0;
                        if (v12)
                            return ((unsigned char)(((sint8)(r_u8((i + (15) * 1u))))));
                    }
                }
            }

            v14 = -1;
            v15 = 0x800EAEF8u;
            while (r_u8(((uint32)(v15))))
            {
                ++v9;
                v15 += (16) * 4u;
                if ((((sint32)(v9)) >= 40))
                    goto LABEL_22;
            }

            v14 = v9;
        LABEL_22:
            v16 = ((uint32)(a1));

            if (!a2)
            {
                v17 = (0x800EAEF8u + (((uint32)(16) * (uint32)(v14))) * 4u);
                v18 = 0;
                if (r_u8(a1))
                {
                    do
                    {
                        if ((((sint32)(v18)) >= 9))
                            break;
                        v19 = r_u8(((v16 += 1u) - 1u));
                        ++v18;
                        w_u8(((uint32)(v17)), v19);
                        v17 = ((uint32)((((uint32)(v17)) + (1) * 1u)));
                    } while (((sint8)(r_u8(v16))));
                }
                w_u8(((uint32)(v17)), 0);
                v16 = ((uint32)(a1));
            }
            sub_8006F238(v16, 0x800FF7A8u);
            w_u8((0x800EADC6u + (((uint32)(16) * (uint32)(r_u32(0x800FF7A0u)))) * 1u), (a2 != 0));
            w_u8((0x800EADC7u + (((uint32)(16) * (uint32)(r_u32(0x800FF7A0u)))) * 1u), v14);
            w_u8((0x800EADC5u + (((uint32)(16) * (uint32)(r_u32(0x800FF7A0u)))) * 1u), 1);
            result = v14;
            w_u32(0x800FF7A0u, ((sint32)(r_u32(0x800FF7A0u) + 1u) % 20));
            return result;
        }
    }
}

uint32 sub_80067724(uint32 a1, uint32 a2)
{
    sint32 result;
    sint32 v3;
    unsigned char v4;
    sint32 v5;
    sint32 v6;
    sint32 v7;
    if (!a1)
    {
        result = 1;
        if (!a2)
            return result;
        return 0;
    }
    if (!a2)
        return 0;
    v3 = r_u8(a1);
    v4 = r_u8(a2);
    if ((((uint32)(((uint32)(v3) - (uint32)(65)))) < 0x1A))
        v3 = ((v3 & 0xFFFFFF00u) | (((((uint32)(v3) + (uint32)(32))) & 0xFFu) << 0));
    if ((((unsigned char)(((uint32)(v4) - (uint32)(65)))) < 0x1Au))
        v4 += 32;
    v5 = v4;
    if ((((unsigned char)(v3)) != v4))
    {
    LABEL_19:
        v7 = ((unsigned char)(v3));

        goto LABEL_20;
    }
    if (((uint8)(v3)))
    {
        while (1)
        {
            v6 = (v5 == 0);
            v7 = ((unsigned char)(v3));
            if (v6)
                break;
            (a1 += 1u);
            (a2 += 1u);
            v3 = r_u8(a1);
            v4 = r_u8(a2);
            if ((((uint32)(((uint32)(v3) - (uint32)(65)))) < 0x1A))
                v3 = ((v3 & 0xFFFFFF00u) | (((((uint32)(v3) + (uint32)(32))) & 0xFFu) << 0));
            if ((((unsigned char)(((uint32)(v4) - (uint32)(65)))) < 0x1Au))
                v4 += 32;
            v7 = ((unsigned char)(v3));
            if ((((unsigned char)(v3)) != v4))
                break;
            v5 = v4;
            if (!(((uint8)(v3))))
                goto LABEL_19;
        }

    LABEL_20:
        v6 = (v7 != 0);

        result = 0;
        if (v6)
            return result;
    }
    result = 1;
    if (v4)
        return 0;
    return result;
}

uint32 sub_8007D8E8(uint32 a1)
{
    unsigned char v1;
    uint32 v2;
    uint32 v3;
    sint32 v4;
    sint32 v5;
    uint32 v6;
    sint32 v7;
    uint32 v8;
    uint32 v9;
    sint32 v10;
    uint32 v11;
    sint32 v12;
    uint32 v13;
    sint8 v14;
    sint32 v15;
    sint32 v16;
    sint32 v17;
    sint32 v18;
    uint32 v19;
    uint32 v20;
    uint32 v21;
    short v22;
    uint32 v23;
    uint32 v24;
    sint32 v25;
    uint32 v26;
    sint32 v27;
    unsigned short v28;
    unsigned short v29;
    unsigned short v30;
    uint32 v31;
    sint32 v32;
    sint32 v33;
    sint32 v34;
    sint32 v35;
    sint32 v36;
    sint32 v37;
    sint32 v38;
    uint32 v39;
    uint32 v40;
    uint32 v41;
    sint32 v42;
    v1 = a1;
    v2 = (0x800EAEF8u + (((uint32)(16) * (uint32)(a1))) * 4u);
    v3 = ((uint32)(((sint32)(r_u32((v2 + (4) * 4u))))));
    v4 = ((sint32)(r_u32((v2 + (5) * 4u))));
    v5 = 0;
    v6 = ((sint32)(r_u32(v3)));
    v7 = ((sint32)(((sint32)(r_u32((v3 - (1) * 4u))))));
    v8 = r_u32(((uint32)(((uint32)(v4) + (uint32)(8)))));
    v9 = ((uint32)(((uint32)(v4) + (uint32)(12))));
    if ((((sint32)(v7)) > 0))
    {
        do
        {
            v10 = 0;
            v11 = ((sint32)(r_u32((v6 + (1) * 4u))));
            v12 = ((sint32)(r_u32((v6 + (2) * 4u))));
            v13 = ((sint32)(r_u32((v6 + (3) * 4u))));
            v14 = -1;
            if (((((sint32)(r_u32(v6))) & 8) == 0))
            {
                v15 = ((sint32)(r_u32((v6 + (6) * 4u))));
                w_u32((v6 + (5) * 4u), (((uint32)(-65536) * (uint32)((((sint32)(((uint32)(((sint32)(r_u32((v6 + (5) * 4u))))) + (uint32)(4095)))) >> 12))) | (((sint32)(((uint32)(((sint32)(r_u32((v6 + (5) * 4u))))) + (uint32)(4095)))) >> 12)));
                v16 = (((sint32)(((uint32)(v15) + (uint32)(4095)))) >> 12);
                v17 = ((sint32)(r_u32((v6 + (7) * 4u))));
                w_u32((v6 + (6) * 4u), (((uint32)(-65536) * (uint32)(v16)) | v16));
                v18 = (((sint32)(r_u32(v6))) | 8);
                w_u32((v6 + (7) * 4u), (((uint32)(-65536) * (uint32)((((sint32)(((uint32)(v17) + (uint32)(4095)))) >> 12))) | (((sint32)(((uint32)(v17) + (uint32)(4095)))) >> 12)));
                w_u32(v6, v18);
            }
            v19 = 0;
            v20 = (v6 + (8) * 4u);
            if (v11)
            {
                v21 = (((uint32)(v6)) + (17) * 2u);
                do
                {
                    if (((r_u16((v21 + (2) * 2u)) & 2) != 0))
                    {
                        v22 = ((uint32)(8) * (uint32)(r_u16(v21)));
                        w_u16(v21, v22);
                        w_u16(v20, v22);
                        w_u16(v21, 0);
                    }
                    ++v19;
                    v21 += (4) * 2u;
                    v20 += (4) * 2u;
                } while ((v19 < v11));
            }
            v23 = (v20 + (((uint32)(4) * (uint32)(v12))) * 2u);
            v24 = 0;
            if (v13)
            {
                v25 = 0;
                do
                {
                    v26 = v23;
                    do
                    {
                        ++v25;
                        w_u16((v26 + (2) * 2u), (r_u16((v26 + (2) * 2u)) * (8)));
                        (v26 += 2u);
                    } while ((((sint32)(v25)) < 4));
                    v27 = r_u32(((uint32)(v23)));
                    w_u16((v23 + (8) * 2u), (r_u16((v23 + (8) * 2u)) * (8)));
                    if (((v27 & 0x40) == 0))
                        w_u32(((uint32)(v23)), (v27 ^ 0x80));
                    if (((r_u32(((uint32)(v23))) & 0x800) == 0))
                        w_u32((((uint32)(v23)) + (3) * 4u), (r_u32((((uint32)(v23)) + (3) * 4u)) & (0xFFFFFFu)));
                    if (((r_u32(((uint32)(v23))) & 3) == 3))
                    {
                        v28 = r_u16((v23 + (12) * 2u));
                        v29 = r_u16((v23 + (13) * 2u));
                        v30 = r_u16((v23 + (14) * 2u));
                        v31 = ((uint32)(r_u32((((uint32)(v23)) + (5) * 4u))));
                        v32 = ((unsigned short)(r_u16((v23 + (15) * 2u))));
                        v33 = r_u16((v31 + (1) * 2u));
                        v34 = r_u16((v31 + (3) * 2u));
                        if (((r_u32(((uint32)(v23))) & 0x20) != 0))
                        {
                            w_u32((((uint32)(v23)) + (8) * 4u), r_u32((((uint32)(v31)) + (3) * 4u)));
                        }
                        else
                        {
                            v35 = r_u16(v31);
                            v28 += v35;
                            v29 += v35;
                            v30 += v35;
                            v32 += v35;
                        }
                        w_u32((((uint32)(v23)) + (5) * 4u), (v28 | ((uint32)(v33) << (uint32)(16))));
                        w_u32((((uint32)(v23)) + (6) * 4u), (v29 | ((uint32)(v34) << (uint32)(16))));
                        w_u32((((uint32)(v23)) + (7) * 4u), (v30 | ((uint32)(v32) << (uint32)(16))));
                    }
                    v10 |= r_u32(((uint32)(v23)));
                    v14 &= r_u16((v23 + (9) * 2u));
                    if (((r_u32(((uint32)(v23))) & 0x10) != 0))
                        w_u16((v23 + (5) * 2u), r_u16((v23 + (4) * 2u)));
                    ++v24;
                    v23 = ((uint32)((((uint32)(v23)) + ((r_u16((v23 + (1) * 2u)) & 0xFFFC)) * 1u)));
                    v25 = 0;
                } while ((v24 < v13));
            }
            v36 = (v10 & 0xC0);
            if (((v14 & 1) != 0))
            {
                w_u32(v6, (r_u32(v6) | (0x10u)));
                v36 = (v10 & 0xC0);
            }
            v37 = (v36 != 0);
            v38 = (v10 & 0x1000);
            if (!v37)
            {
                w_u32(v6, (r_u32(v6) | (0x20u)));
                v38 = (v10 & 0x1000);
            }
            if (v38)
                w_u32(v6, (r_u32(v6) | (0x40u)));
            ++v5;
            v6 = ((uint32)(v23));
        } while ((((sint32)(v5)) < ((sint32)(v7))));
    }
    v39 = v9;
    v40 = 0;
    if (v8)
    {
        v41 = (v9 + (11) * 2u);
        do
        {
            if ((v40 >= ((uint32)(v8) - (uint32)(1))))
                w_u32(((uint32)((v41 + (3) * 2u))), 0);
            else
                w_u32(((uint32)((v41 + (3) * 2u))), (v39 + (18) * 2u));
            w_u8((((uint32)(v41)) + (5) * 1u), v1);
            v42 = r_u16(v41);
            w_u8((((uint32)(v41)) + (4) * 1u), 0);
            if (((r_u32(r_u32(((uint32)(((uint32)(((uint32)(4) * (uint32)(v42))) + (uint32)(r_u32((0x800EAEF8u + (((uint32)(((uint32)(16) * (uint32)(v1))) + (uint32)(4))) * 4u)))))))) & 0x10) != 0))
                w_u16(v39, (r_u16(v39) | (0x20u)));
            ++v40;
            v41 += (18) * 2u;
            v39 += (18) * 2u;
        } while ((v40 < v8));
    }
    return 1;
}
