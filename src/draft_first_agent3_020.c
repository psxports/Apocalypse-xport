#include "game_movie_startup.h"
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
#include <stdlib.h>
#include <stdio.h>

/* Unverified draft C; ABI and adapters remain TODO */
uint32 sub_8006838C(uint32 a1, uint32 a2)
{
    sint32 i;
    if ((((sint32)(r_u32(a1))) == a2))
    {
        w_u32(a1, r_u32(((uint32)((a2 + 8)))));
    }
    else
    {
        for (i = ((sint32)(r_u32(a1))); (r_u32(((uint32)((i + 8)))) != a2); i = r_u32(((uint32)((i + 8)))))
            ;

        w_u32(((uint32)((i + 8))), r_u32(((uint32)((a2 + 8)))));
    }
    return sub_80068270(a2);
}

uint32 sub_8006BC20(uint32 a1)
{
    uint32 v1;
    uint32 v2;
    sint32 result;
    v1 = ((uint32)((a1 - 8)));
    v2 = (((sint32)((r_u32(((uint32)(((a1 - 8) + 4)))) << 28))) >> 28);
    if ((v2 != -2))
        goto LABEL_6;
    result = r_u32(0x800FF730u);
    if ((v1 == ((uint32)(r_u32(0x800FF730u)))))
    {
        w_u32(0x800FF730u, 0);
        return result;
    }
    result = -1;
    if ((v1 == ((uint32)(r_u32(0x800FF734u)))))
    {
        w_u32(0x800FF734u, 0);
    }
    else
    {
    LABEL_6:
        result = (v2 < 2);

        if ((v2 == -1))
        {
            result = r_u32(0x800FF744u);
            w_u32(0x800FF744u, (a1 - 8));
            w_u32(v1, result);
        }
        else if ((v2 < 2))
        {
            w_u32((0x800FF748u + (v2) * 4u), ((r_u32((0x800FF748u + (v2) * 4u)) - 8) - (((uint32)(((sint32)(r_u32((v1 + (1) * 4u)))))) >> 4)));
            result = sub_8006B4B4(v1, v2);
            if (!v2)
            {
                result = 1;
                w_u32(0x800FF738u, (r_u32((0x800FF748u + (0) * 4u)) >= ((uint32)(r_u32(0x800FF73Cu)))));
            }
        }
    }
    return result;
}

uint32 sub_800690A0(uint32 a1)
{
    sint32 v2;
    uint32 v3;
    sint32 v4;
    sint32 v5;
    sint32 v6;
    sint32 v7;
    sint32 v8;
    sint32 result;
    v2 = 0;
    do
    {
        v3 = ((uint32)(r_u32(0x800FF67Cu)));
        v4 = 0;
        if (!r_u32(0x800FF67Cu))
            goto LABEL_15;
        while (1)
        {
            if (((((unsigned short)(((sint16)(r_u16((a1 + (1) * 2u)))))) == r_u16((v3 + (1) * 2u))) && (((unsigned short)(((sint16)(r_u16((a1 + (3) * 2u)))))) == r_u16((v3 + (3) * 2u)))))
            {
                v5 = r_u16(v3);
                v6 = ((unsigned short)(((sint16)(r_u16(a1)))));
                if (((v5 + r_u16((v3 + (2) * 2u))) == v6))
                    break;
                v7 = (v6 + ((unsigned short)(((sint16)(r_u16((a1 + (2) * 2u)))))));
            }
            else
            {
                if ((((((unsigned short)(((sint16)(r_u16(a1))))) != r_u16(v3)) || (((unsigned short)(((sint16)(r_u16((a1 + (2) * 2u)))))) != r_u16((v3 + (2) * 2u)))) || ((r_u16((v3 + (1) * 2u)) & 0x100) != (((sint16)(r_u16((a1 + (1) * 2u)))) & 0x100))))
                    goto LABEL_14;
                v5 = r_u16((v3 + (1) * 2u));
                v8 = ((unsigned short)(((sint16)(r_u16((a1 + (1) * 2u))))));
                if (((v5 + r_u16((v3 + (3) * 2u))) == v8))
                    break;
                v7 = (v8 + ((unsigned short)(((sint16)(r_u16((a1 + (3) * 2u)))))));
            }
            if ((v7 == v5))
                break;
        LABEL_14:
            v3 = ((uint32)(r_u32((((uint32)(v3)) + (2) * 4u))));

            if (!v3)
                goto LABEL_15;
        }

        v4 = 1;
        a1 = ((uint32)(sub_80068E80(((uint32)(v3)), a1)));
        v2 = 1;
    LABEL_15:
        result = v2;

    } while (v4);
    return result;
}

uint32 sub_80015EC8(void)
{
    sint16 rectangle[4];
    uint32 first = r_u32(0x800FF048u), second = r_u32(0x800FF04Cu);
    rectangle[0] = (sint16)first;
    rectangle[1] = (sint16)(first >> 16);
    rectangle[2] = (sint16)second;
    rectangle[3] = (sint16)(second >> 16);
    xport_draft_host_sub_8008847C_p1(rectangle, 0u, 0u, 0u);
    rectangle[1] = 511;
    rectangle[3] = 1;
    return xport_draft_host_sub_8008847C_p1(rectangle, 0u, 0u, 0u);
}

void sub_8009B12C(uint32 a1, uint32 a2, uint32 a3)
{
    w_u32(0x80105848u, a1);
    w_u32(0x80105824u, a2);
    w_u32(0x80105844u, a3);
}

/* TODO Postincrement memory expressions may require ordering refinement */
uint32 sub_800702E0(uint32 a1, uint32 a2)
{
    sint32 v2;
    sint32 v3;
    sint32 result;
    sint32 v5;
    v2 = r_u8(a1);
    (w_u32((((uint32)(a1)) + (3) * 4u), (r_u32((((uint32)(a1)) + (3) * 4u)) + 1u)), (r_u32((((uint32)(a1)) + (3) * 4u)) + 1u));
    if (!v2)
    {
        if (!a2)
            goto LABEL_6;
        w_u8((a1 + (1) * 1u), 1);
        w_u32((((uint32)(a1)) + (3) * 4u), 0);
    }
    if (a2)
    {
        w_u32(0x800FF814u, 0);
        w_u8(a1, 1);
        goto LABEL_7;
    }
LABEL_6:
    w_u8(a1, 0);

LABEL_7:
    if (r_u8(a1))
    {
        v3 = r_u32((((uint32)(a1)) + (1) * 4u));
        w_u32((((uint32)(a1)) + (2) * 4u), 0);
        result = (v3 + 1);
        w_u32((((uint32)(a1)) + (1) * 4u), result);
    }
    else
    {
        v5 = r_u32((((uint32)(a1)) + (2) * 4u));
        w_u32((((uint32)(a1)) + (1) * 4u), 0);
        result = (v5 + 1);
        w_u32((((uint32)(a1)) + (2) * 4u), result);
    }

    return result;
}

/* TODO Missing call adapter sub_8009B06C */
uint32 sub_8002F990(void)
{
    uint32 frame, header;
    if (apocalypse_str_next(&frame, &header))
        return 0;
    w_u32(0x800FF274u, r_u32(header + 8));
    if (r_u32(0x800FF274u) >= r_u32(0x800FF27Cu))
        w_u32(0x800FF24Cu, 1);
    return frame;
}

void sub_8008EC4C(void)
{
    sub_8008E9FC(0);
}

uint32 sub_8006F238(uint32 a1, uint32 a2)
{
    sint8 v2;
    uint32 i;
    sint32 result;
    v2 = ((sint8)(r_u8(a1)));
    for (i = (0x800EADB8u + ((16 * r_u32(0x800FF7A0u))) * 1u); ((sint8)(r_u8(a1))); (i += 1u))
    {
        (a1 += 1u);
        w_u8(i, v2);
        v2 = ((sint8)(r_u8(a1)));
    }

    for (result = ((unsigned char)(r_u8(a2))); r_u8(a2); (i += 1u))
    {
        (a2 += 1u);
        w_u8(i, result);
        result = ((unsigned char)(r_u8(a2)));
    }

    w_u8(i, 0);
    return result;
}

uint32 sub_8006F500(void)
{
    uint32 v0;
    sint32 result;
    sint32 v2;
    sint32 v3;
    unsigned char v4;
    uint32 v5;
    uint32 v6;
    sint32 v7;
    sint8 v9;
    sint32 v10;
    sint32 v11;
    sint32 v12;
    v0 = (0x800EADB8u + ((16 * r_u32(0x800FF79Cu))) * 1u);
    result = ((unsigned char)(r_u8((0x800EADC5u + ((16 * r_u32(0x800FF79Cu))) * 1u))));
    if (r_u8((0x800EADC5u + ((16 * r_u32(0x800FF79Cu))) * 1u)))
    {
        result = (r_u32(0x800FF7B0u) < 2);
        if ((r_u32(0x800FF7B0u) == 1))
        {
            result = 2;
            if (!r_u32(0x800FF700u))
                w_u32(0x800FF7B0u, 2);
        }
        else
        {
            if ((r_u32(0x800FF7B0u) < 2))
            {
                if (r_u32(0x800FF7B0u))
                    return result;
                v2 = sub_8006B04C((0x800EADB8u + ((16 * r_u32(0x800FF79Cu))) * 1u));
                v3 = v2;
                if (!r_u8((0x800EADC6u + ((((uint32)(v0)) + 2146521672)) * 1u)))
                {
                    v10 = v2;
                    v11 = 1;
                LABEL_22:
                    w_u32((0x800EAEF8u + (((16 * ((unsigned char)(((sint8)(r_u8((v0 + (15) * 1u))))))) + 5)) * 4u), sub_8006B864(v10, v11, 1));

                    sub_8006B234(r_u32((0x800EAEF8u + (((16 * ((unsigned char)(((sint8)(r_u8((v0 + (15) * 1u))))))) + 5)) * 4u)));
                    result = 1;
                    goto LABEL_23;
                }
                result = 1;
                if ((r_u8((0x800EADC6u + ((((uint32)(v0)) + 2146521672)) * 1u)) != 1))
                {
                LABEL_23:
                    w_u32(0x800FF7B0u, 1);

                    return result;
                }
                if ((r_u32((0x800FF778u + (0) * 4u)) != -1))
                {
                    if ((r_u32(0x800FF77Cu) != -1))
                    {
                        v4 = r_u32((0x800FF778u + (r_u32(0x800FF798u)) * 4u));
                        w_u8((0x800EADC7u + ((((uint32)(v0)) + 2146521672)) * 1u), v4);
                        sub_8006EE94(v4, 1);
                        w_u32((0x800FF778u + (r_u32(0x800FF798u)) * 4u), ((unsigned char)(r_u8((0x800EADC7u + ((((uint32)(v0)) + 2146521672)) * 1u)))));
                    LABEL_16:
                        v5 = v0;

                        goto LABEL_17;
                    }
                    if ((r_u32((0x800FF778u + (0) * 4u)) != r_u32(0x800FF77Cu)))
                    {
                        w_u32(0x800FF77Cu, ((unsigned char)(r_u8((0x800EADC7u + ((((uint32)(v0)) + 2146521672)) * 1u)))));
                        goto LABEL_16;
                    }
                }
                w_u32((0x800FF778u + (0) * 4u), ((unsigned char)(r_u8((0x800EADC7u + ((((uint32)(v0)) + 2146521672)) * 1u)))));
                v5 = v0;
            LABEL_17:
                v6 = (0x800EAEF8u + ((16 * ((unsigned char)(r_u8((0x800EADC7u + ((((uint32)(v0)) + 2146521672)) * 1u)))))) * 4u);

                v7 = 0;
                if ((((sint8)(r_u8(v0))) != 46))
                {
                    do
                    {
                        if ((v7++ >= 9))
                            break;
                        v9 = r_u32((v5 += 1u, v5 - 1u));
                        w_u8(((uint32)(v6)), v9);
                        v6 = ((uint32)((((uint32)(v6)) + (1) * 1u)));
                    } while ((((sint8)(r_u8(v5))) != 46));
                }
                v10 = v3;
                v11 = -2;
                w_u8(((uint32)(v6)), 0);
                goto LABEL_22;
            }
            result = 2;
            if ((r_u32(0x800FF7B0u) == 2))
            {
                if (r_u8((0x800EADC6u + ((16 * r_u32(0x800FF79Cu))) * 1u)))
                {
                    if ((r_u8((0x800EADC6u + ((16 * r_u32(0x800FF79Cu))) * 1u)) == 1))
                    {
                        sub_8006E4A0(((unsigned char)(r_u8((0x800EADC7u + ((16 * r_u32(0x800FF79Cu))) * 1u)))));
                        sub_8006EBF4(((unsigned char)(r_u8((0x800EADC7u + ((((uint32)(v0)) + 2146521672)) * 1u)))));
                        v12 = sub_8006F4D0(((unsigned char)(r_u8((0x800EADC7u + ((((uint32)(v0)) + 2146521672)) * 1u)))));
                        w_u32((0x800FF7B4u + (v12) * 4u), ((sint32)(sub_8006E360(((uint32)(r_u32((0x800EAEF8u + (((16 * ((unsigned char)(r_u8((0x800EADC7u + ((((uint32)(v0)) + 2146521672)) * 1u))))) + 5)) * 4u))))))));
                    }
                }
                else
                {
                    sub_8006E4A0(((unsigned char)(r_u8((0x800EADC7u + ((16 * r_u32(0x800FF79Cu))) * 1u)))));
                }
                w_u8((0x800EADC5u + ((((uint32)(v0)) + 2146521672)) * 1u), 0);
                w_u32(0x800FF7B0u, 0);
                result = (20 * ((r_u32(0x800FF79Cu) + 1) / 20));
                w_u32(0x800FF79Cu, ((r_u32(0x800FF79Cu) + 1) % 20));
            }
        }
    }
    return result;
}

/* TODO Missing call adapter sub_800885A4 */
/* TODO Missing call adapter sub_8008AF9C */
/* TODO Postincrement memory expressions may require ordering refinement */
static void texture_6E4A0_missing_output(uint32 format, uint32 texture, uint32 resource)
{
    /* TODO Native XY output is undefined for unsupported original formats */
    abort();
}

uint32 sub_8006E4A0(uint32 a1)
{
    uint32 texture_rectangle[2];
    sint16 upload_rectangle[4];
    sint32 v1;
    sint32 v2;
    sint32 v3;
    uint32 v4;
    uint32 v5;
    uint32 v6;
    uint32 v7;
    uint32 v8;
    uint32 v9;
    uint32 v10;
    uint32 v11;
    uint32 v12;
    uint32 v13;
    sint32 v14;
    sint32 i;
    uint32 v16;
    uint32 v17;
    sint32 v18;
    uint32 v19;
    uint32 v22;
    uint32 v23;
    sint32 v24;
    uint32 v25;
    sint32 v26;
    uint32 j;
    uint32 v28;
    uint32 v29;
    uint32 v30;
    sint32 v31;
    sint32 v32;
    sint32 v33;
    uint32 v34;
    sint32 v35;
    sint32 v36;
    unsigned short v37;
    sint32 v38;
    sint32 v39;
    short v40;
    short v41;
    short v42;
    short v43;
    uint32 v44;
    sint32 v45;
    sint32 v46;
    short v47;
    uint8 v48;
    uint8 v49;
    uint8 v50;
    uint8 v51;
    uint8 v52;
    uint8 v53;
    uint8 v54;
    uint8 v55;
    uint8 v56;
    uint8 v57;
    sint32 v58;
    uint32 v59;
    uint32 v60;
    uint32 v61;
    sint32 v62;
    uint32 v63;
    sint32 v64;
    uint32 v65;
    sint32 v66;
    uint32 v67;
    sint32 result;
    sint32 v71;
    sint32 v72;
    uint32 v73;
    uint32 v74;
    uint32 v75;
    uint32 v76;
    sint32 v77;
    v1 = r_u32((0x800EAEF8u + (((16 * a1) + 5)) * 4u));
    v2 = ((36 * r_u32(((uint32)((v1 + 8))))) + v1);
    v3 = (v2 + 16);
    v4 = r_u32(((uint32)((v2 + 12))));
    w_u32(0x800FF788u, 0);
    v5 = 0;
    if (v4)
    {
        v6 = ((uint32)((v2 + 16)));
        do
        {
            ++v5;
            w_u32(v6, r_u32(v6) + (uint32)v1);
            v6 += 4u;
        } while ((v5 < v4));
    }
    v7 = (0x800EAEF8u + ((16 * a1)) * 4u);
    w_u32((v7 + (4) * 4u), v3);
    v8 = sub_8006E360(((uint32)(v1)));
    v9 = 0;
    v73 = (v8 + ((v4 + 1)) * 4u);
    v10 = v73;
    w_u32((v7 + (3) * 4u), ((sint32)(v8)));
    v11 = (v8 + ((v4 + 1)) * 4u);
    v12 = ((sint32)(r_u32((v73 - (1) * 4u))));
    while (1)
    {
        v13 = 0;
        if ((v9 >= v12))
            break;
        v14 = sub_8006E278(((sint32)(r_u32(v11))));
        if (!v14)
            v14 = sub_8006E178(((sint32)(r_u32(v11))));
        (v11 += 4u);
        ++v9;
        w_u16((uint32)v14 + 16u, r_u16((uint32)v14 + 16u) + 1u);
        w_u32((v10 += 4u, v10 - 4u), v14);
    }

    for (i = v3;; i += 4)
    {
        v16 = 0;
        if ((v13 >= v4))
            break;
        v17 = r_u32(((uint32)((r_u32(((uint32)(i))) + 12))));
        v18 = (((r_u32(((uint32)(i))) + (8 * r_u32(((uint32)((r_u32(((uint32)(i))) + 4)))))) + 32) + (8 * r_u32(((uint32)((r_u32(((uint32)(i))) + 8))))));
        if (v17)
        {
            do
            {
                if (((r_u32(((uint32)(v18))) & 1) != 0))
                    w_u32(((uint32)((v18 + 20))), ((sint32)(r_u32((v73 + (r_u32(((uint32)((v18 + 20))))) * 4u)))));
                ++v16;
                v18 += (r_u16(((uint32)((v18 + 2)))) & 0xFFFC);
            } while ((v16 < v17));
        }
        ++v13;
    }

    v19 = 0;
    sub_8006E414();
    sub_800695EC();
    v22 = sub_8006E360(((uint32)(v1)));
    v74 = (((uint32)((v22 + (((v4 + 1) + v12)) * 4u))) - v1);
    v23 = r_u32((v22 + (((v4 + 1) + v12)) * 4u));
    v24 = ((sint32)((v22 + (((v4 + 2) + v12)) * 4u)));
    if (v23)
    {
        do
        {
            if (!sub_800695B4(r_u32(((uint32)(v24)))))
                sub_80069758(r_u32(((uint32)(v24))), ((uint32)((v24 + 4))), 1);
            ++v19;
            v24 += 36;
        } while ((v19 < v23));
    }
    v25 = r_u32(((uint32)(v24)));
    v26 = (v24 + 4);
    for (j = 0; (j < v25); v26 += 516)
    {
        if (!sub_800695B4(r_u32(((uint32)(v26)))))
            sub_80069758(r_u32(((uint32)(v26))), ((uint32)((v26 + 4))), 2);
        ++j;
    }

    w_u32(0x800FF674u, 0);
    v28 = (0x800EAEF8u + ((16 * a1)) * 4u);
    if ((((((r_u8(((uint32)(v28))) == 98) && (r_u8((((uint32)(v28)) + (1) * 1u)) == 105)) && (r_u8((((uint32)(v28)) + (2) * 1u)) == 116)) && (r_u8((((uint32)(v28)) + (3) * 1u)) == 115)) && !(r_u8((((uint32)(v28)) + (4) * 1u)))))
    {
        w_u32(0x800FF674u, 1);
    }
    v29 = r_u32(((uint32)(v26)));
    v30 = ((uint32)((v26 + 4)));
    v76 = 0;
    v75 = v29;
    /* Rectangle outputs are packed pairs of 16-bit fields */
    while ((v76 < v75))
    {
        v31 = (v1 + r_u32(v30));
        v32 = r_u32(((uint32)(v31)));
        v33 = r_u32(((uint32)((v31 + 4))));
        v34 = ((uint32)(((sint32)(r_u32((v73 + (r_u32(((uint32)((v31 + 12))))) * 4u))))));
        (v30 += 4u);
        v77 = r_u32(((uint32)((v31 + 8))));
        if ((v34 && !((sint8)(r_u8((v34 + (18) * 1u))))))
        {
            v35 = r_u32((((uint32)(v34)) + (5) * 4u));
            if ((v35 == -857728900))
                v32 = ((v32 & 0xFFFFFF00u) | ((((v32 | 1)) & 0xFFu) << 0));
            if ((v35 == 1756868918))
                v32 = ((v32 & 0xFFFFFF00u) | ((((v32 | 1)) & 0xFFu) << 0));
            v36 = r_u16(((uint32)((v31 + 16))));
            v37 = r_u16(((uint32)((v31 + 18))));
            v38 = (v31 + 20);
            if ((v33 == 256))
            {
                v39 = r_u16(((uint32)((v31 + 18))));
                if (((v32 & 1) != 0))
                    xport_draft_host_sub_80068450_p45(0, ((v36 + 1) & 0xFFFFFFFE), v39, texture_rectangle, &texture_rectangle[1], 8, 2, r_u32((((uint32)(v34)) + (5) * 4u)));
                else
                    xport_draft_host_sub_80068450_p45(0, ((v36 + 1) & 0xFFFFFFFE), v39, texture_rectangle, &texture_rectangle[1], 8, 0, r_u32((((uint32)(v34)) + (5) * 4u)));
                v71 = texture_rectangle[0];
                v72 = texture_rectangle[1];
                v40 = v71;
                v41 = v72;
                w_u16((((uint32)(v34)) + (14) * 2u), v71);
                w_u16((((uint32)(v34)) + (15) * 2u), v41);
                upload_rectangle[0] = v40;
                upload_rectangle[1] = v41;
                upload_rectangle[2] = (sint16)((((uint32)v36 + 1u) & ~1u) / 2u);
                upload_rectangle[3] = (sint16)v37;
                xport_draft_host_sub_800885A4_p1(upload_rectangle, (uint32)v38);
            }
            if ((v33 == 16))
            {
                if (((v32 & 1) != 0))
                    xport_draft_host_sub_80068450_p45(0, ((((unsigned short)(v36)) + 3) & 0xFFFFFFFC), v37, texture_rectangle, &texture_rectangle[1], 4, 2, r_u32((((uint32)(v34)) + (5) * 4u)));
                else
                    xport_draft_host_sub_80068450_p45(0, ((((unsigned short)(v36)) + 3) & 0xFFFFFFFC), v37, texture_rectangle, &texture_rectangle[1], 4, 0, r_u32((((uint32)(v34)) + (5) * 4u)));
                v71 = texture_rectangle[0];
                v72 = texture_rectangle[1];
                v42 = v71;
                v43 = v72;
                w_u16((((uint32)(v34)) + (14) * 2u), v71);
                w_u16((((uint32)(v34)) + (15) * 2u), v43);
                upload_rectangle[0] = v42;
                upload_rectangle[1] = v43;
                upload_rectangle[2] = (sint16)((((uint32)v36 + 3u) & ~3u) / 4u);
                upload_rectangle[3] = (sint16)v37;
                xport_draft_host_sub_800885A4_p1(upload_rectangle, (uint32)v38);
            }
            v44 = ((uint32)(sub_800695B4(v77)));
            if (v33 != 16u && v33 != 256u)
                texture_6E4A0_missing_output((uint32)v33, v34, (uint32)v31);
            v71 = texture_rectangle[0];
            v72 = texture_rectangle[1];
            v45 = v71;
            v46 = v72;
            w_u16(v44 + 4u, r_u16(v44 + 4u) + 1u);
            w_u32((((uint32)(v34)) + (6) * 4u), v44);
            w_u16((((uint32)(v34)) + (1) * 2u), r_u16(v44));
            v47 = GetTPage(v33 == 256, 0, v45, v46);
            v48 = v71;
            w_u16((((uint32)(v34)) + (3) * 2u), v47);
            v49 = (v48 & 0x3F);
            if ((v33 == 256))
                v50 = (2 * v49);
            else
                v50 = (4 * v49);
            w_u8(v34, v50);
            v51 = v72;
            v52 = ((v36 + v50) - 1);
            w_u8((v34 + (18) * 1u), 1);
            w_u8((v34 + (4) * 1u), v52);
            w_u8((v34 + (10) * 1u), v52);
            w_u8((v34 + (1) * 1u), v51);
            v53 = v51;
            v54 = (v37 + v51);
            v55 = r_u8(v34);
            w_u8((v34 + (9) * 1u), (v54 - 1));
            v56 = r_u8(v34);
            w_u8((v34 + (5) * 1u), v53);
            w_u8((v34 + (8) * 1u), v55);
            v57 = r_u8(v34 + 9u);
            w_u32((((uint32)(v34)) + (3) * 4u), ((((((v53 & 0xF8) << 12) | ((v56 & 0xF8) << 7)) | 0xE2000000) | (4 * (-v37 & 0xF8))) | ((-(((unsigned short)(v36))) & 0xF8) >> 3)));
            w_u8((v34 + (11) * 1u), v57);
        }
        ++v76;
    }

    w_u32(0x800FF674u, 0);
    sub_8006BD14(v1, v74);
    v58 = 0;
    sub_8007D8E8(a1);
    v59 = (0x800EAEF8u + ((16 * a1)) * 4u);
    w_u8((((uint32)(v59)) + (9) * 1u), 0);
    w_u32((v59 + (9) * 4u), 0);
    w_u32((v59 + (10) * 4u), 0);
    v60 = v59;
    v61 = ((uint32)((v1 + r_u32(((uint32)((v1 + 4)))))));
    while (1)
    {
        v62 = ((sint32)(r_u32(v61)));
        if ((((sint32)(r_u32(v61))) == -1))
            break;
        v63 = (v61 + (1) * 4u);
        v64 = ((sint32)(r_u32(v63)));
        v65 = (v63 + (1) * 4u);
        if ((v62 == 10))
        {
            v66 = sub_8006F4D0(a1);
            sub_8007E41C(v66, v65);
            v61 = ((uint32)((((uint32)(v65)) + (v64) * 1u)));
        }
        else if ((v62 >= 11))
        {
            if ((v62 == 69))
            {
                sub_8006F048(v1, v65);
                v58 = 1;
                goto LABEL_72;
            }
            if ((v62 >= 70))
            {
                if ((v62 == 1933723474))
                {
                    w_u32((v60 + (8) * 4u), ((sint32)(v65)));
                    goto LABEL_72;
                }
                v61 = ((uint32)((((uint32)(v65)) + (v64) * 1u)));
            }
            else
            {
                if ((v62 == 42))
                {
                    w_u32((v60 + (6) * 4u), ((sint32)(v65)));
                    w_u8((((uint32)(v60)) + (9) * 1u), 1);
                    goto LABEL_72;
                }
                v61 = ((uint32)((((uint32)(v65)) + (v64) * 1u)));
            }
        }
        else if ((v62 == 6))
        {
            w_u32((v60 + (9) * 4u), ((sint32)(v65)));
            w_u32(0x800FF788u, ((sint32)((v65 - (2) * 4u))));
            v61 = ((uint32)((((uint32)(v65)) + (v64) * 1u)));
        }
        else if ((v62 == 7))
        {
            w_u32((v60 + (10) * 4u), ((sint32)(v65)));
        LABEL_72:
            v61 = ((uint32)((((uint32)(v65)) + (v64) * 1u)));
        }
        else
        {
            v61 = ((uint32)((((uint32)(v65)) + (v64) * 1u)));
        }
    }

    v67 = (v61 + (1) * 4u);
    if (v58)
        sub_8006BD14(v1, (((uint32)(v67)) - v1));
    result = 1;
    w_u8(0x800EAEF8u + 64u * a1 + 10u, 1);
    return result;
}

/* TODO Postincrement memory expressions may require ordering refinement */
uint32 sub_8006E414(void)
{
    sint32 result;
    uint32 v1;
    sub_8006E2C4();
    while (1)
    {
        result = sub_8006E2DC();
        v1 = ((uint32)(result));
        if (!result)
            break;
        if (r_u8(((uint32)((result + 18)))))
        {
            if (!(r_u16(((uint32)((result + 16))))))
            {
                (w_u16(((uint32)((r_u32(((uint32)((result + 24)))) + 4))), (r_u16(((uint32)((r_u32(((uint32)((result + 24)))) + 4)))) - 1u)), (r_u16(((uint32)((r_u32(((uint32)((result + 24)))) + 4)))) - 1u));
                sub_80068DC4(r_u16(((uint32)((result + 28)))), r_u16(((uint32)((result + 30)))));
                sub_8006E1EC(v1);
            }
        }
    }

    return result;
}

uint32 sub_8006E2C4(void)
{
    sint32 result;
    result = r_u32((0x800EB8F8u + (0) * 4u));
    w_u32(0x800FFC60u, 0);
    w_u32(0x800FFC64u, r_u32((0x800EB8F8u + (0) * 4u)));
    return result;
}

uint32 sub_800696A8(void)
{
    sint32 v0;
    v0 = 0;
    if (!r_u8((0x80100590u + (0) * 1u)))
    {
        v0 = 1;
        while ((v0 < 148))
        {
            if (r_u8((0x80100590u + (v0++) * 1u)))
            {
                --v0;
                break;
            }
        }
    }
    w_u8((0x80100590u + (v0) * 1u), 0);
    return v0;
}

/* TODO Postincrement memory expressions may require ordering refinement */
uint32 sub_8006F048(uint32 a1, uint32 a2)
{
    uint32 v3;
    uint32 v4;
    sint32 v5;
    uint32 v6;
    uint32 v7;
    uint32 v8;
    sint32 result;
    uint32 v10;
    uint32 v11;
    uint32 v12;
    uint32 i;
    sint32 v15;
    sint32 v16;
    sint8 v17;
    v3 = (sub_8006E360(a1) + 4u);
    v4 = ((uint32)(sub_8006B864(8, 0, 1)));
    v5 = r_u32(0x800FF7A4u);
    v6 = 0;
    w_u32(v4, a2);
    w_u32(0x800FF7A4u, ((sint32)(v4)));
    w_u32((v4 + (1) * 4u), ((uint32)(v5)));
    v7 = r_u32(a2);
    v8 = (a2 + (1) * 4u);
    while (1)
    {
        result = (v6 < v7);
        v10 = (v8 + (2) * 4u);
        if ((v6 >= v7))
            break;
        v11 = r_u32(v10);
        v8 = (v10 + (1) * 4u);
        v12 = 0;
        for (i = (v8 + (1) * 4u); (v12++ < v11); i += (2) * 4u)
        {
            v15 = r_u32((v3 + (r_u32(i)) * 4u));
            w_u32(i, v15);
            w_u16(((uint32)((v15 + 6))), (r_u16(((uint32)((v15 + 6)))) | (0x20u)));
            (w_u8(((uint32)((r_u32(i) + 4))), (r_u8(((uint32)((r_u32(i) + 4)))) + 1u)), (r_u8(((uint32)((r_u32(i) + 4)))) + 1u));
            (w_u8(((uint32)((r_u32(i) + 9))), (r_u8(((uint32)((r_u32(i) + 9)))) + 1u)), (r_u8(((uint32)((r_u32(i) + 9)))) + 1u));
            v8 += (2) * 4u;
            (w_u8(((uint32)((r_u32(i) + 10))), (r_u8(((uint32)((r_u32(i) + 10)))) + 1u)), (r_u8(((uint32)((r_u32(i) + 10)))) + 1u));
            v16 = r_u32(i);
            v17 = r_u8(((uint32)((r_u32(i) + 11))));
            w_u8(((uint32)((v16 + 11))), (v17 + 1));
        }

        ++v6;
    }

    return result;
}

/* TODO Resolve original data label 0x800FF760u */
uint32 sub_8006CDA0(void)
{
    sint32 result;
    result = sub_8006F164(0x800FF760u);
    w_u32(0x800FF75Cu, result);
    return result;
}

/* TODO Resolve original data label 0x800FF6E8u */
/* TODO Resolve original data label 0x800A3A3Cu */
uint32 sub_8006A834(void)
{
    sint32 result;
    w_u32(0x800FF6DCu, sub_8006F164(0x800A3A3Cu));
    result = sub_8006F164(0x800FF6E8u);
    w_u32(0x800FF6E0u, result);
    return result;
}

/* TODO Missing call adapter sub_8008AF9C */
/* TODO Missing call adapter sub_8008B01C */
/* TODO Resolve original data label 0x800FF40Cu */
uint32 sub_80032A0C(void)
{
    uint32 i, result;
    w_u32(0x800FF3A8u, 0);
    w_u32(0x800FF434u, 0);
    w_u32(0x800FF438u, 0);
    w_u32(0x800FF43Cu, 0);
    w_u32(0x800FF440u, 0);
    w_u32(0x800FF45Cu, 0);
    w_u32(0x800FF460u, 0);
    w_u32(0x800FF444u, 0);
    w_u32(0x800FF448u, 0);
    w_u32(0x800FF44Cu, 0);
    w_u32(0x800FF450u, 0);
    w_u32(0x800FF454u, 0);
    w_u32(0x800FF464u, 0);
    w_u32(0x800FF458u, 0);
    /* GetTPage(0,1,0,0) and SetDrawTPage(packet,1,1,tpage) */
    w_u8(0x800FF46Bu, 1);
    w_u32(0x800FF46Cu, 0xE1000620u);
    for (i = 0; i < 15; ++i)
        w_u32(0x800A6808u + 4 * i, sub_8006F164(r_u32(0x800A67CCu + 4 * i)));
    w_u32(0x800FF01Cu, sub_8006F164(0x800FF40Cu));
    result = sub_8006E278(0x2A3B2550u);
    w_u32(0x800FF408u, result);
    return result;
}

uint32 sub_8001A858(void)
{
    uint32 i;
    for (i = 0; i < 40; ++i)
    {
        w_u32(0x800A57FCu + 4 * i, sub_80066570(10) + 10);
        w_u32(0x800A593Cu + 4 * i, 40);
        w_u32(0x800A589Cu + 4 * i, sub_80066570(30) + 30);
    }
    return 0;
}

uint32 sub_8006A0EC(void)
{
    sint32 result;
    result = 1;
    w_u32(0x800FFC20u, 1);
    return result;
}

uint32 sub_8006A0FC(void)
{
    sint32 result;
    result = 1;
    w_u32(0x800FF6B0u, 0);
    w_u32(0x800FF6B4u, 0);
    w_u32(0x800FF6B8u, 1);
    return result;
}

uint32 sub_800711D0(void)
{
    sub_8007115C(r_u32(0x800FF5A0u));
    sub_8007115C(r_u32(0x800FF904u));
    sub_8007115C(r_u32(0x800FF4E8u));
    sub_8007115C(r_u32(0x800FF5DCu));
    sub_8007115C(r_u32(0x800FF8A0u));
    sub_8007115C(r_u32(0x800FF204u));
    sub_8007115C(r_u32(0x800FF220u));
    sub_8007115C(r_u32(0x800FF5E0u));
    sub_8006FB60();
    sub_80032B64();
    sub_8001B708();
    return sub_8001BAC0();
}

/* Bind cleanup methods from existing native object constructors */
uint32 apocalypse_object_cleanup(uint32 target, uint32 receiver, uint32 reason)
{
    switch (target)
    {
        case 0x8001C51Cu:
            return sub_8001C51C(receiver, reason);
        case 0x8001C9ECu:
            return sub_8001C9EC(receiver, reason);
        case 0x8001D918u:
            return sub_8001D918(receiver, reason);
        case 0x8001EC58u:
            return sub_8001EC58(receiver, reason);
        case 0x8001FCD4u:
            return sub_8001FCD4(receiver, reason);
        case 0x800201F8u:
            return sub_800201F8(receiver, reason);
        case 0x800290F4u:
            return sub_800290F4(receiver, reason);
        case 0x80029ED4u:
            return sub_80029ED4(receiver, reason);
        case 0x8002BBF0u:
            return sub_8002BBF0(receiver, reason);
        case 0x8002BC44u:
            return sub_8002BC44(receiver, reason);
        case 0x800318C0u:
            return sub_800318C0(receiver, reason);
        case 0x80032128u:
            return sub_80032128(receiver, reason);
        case 0x80032694u:
            return sub_80032694(receiver, reason);
        case 0x80032FB8u:
            return sub_80032FB8(receiver, reason);
        case 0x800331ECu:
            return sub_800331EC(receiver, reason);
        case 0x800348A8u:
            return sub_800348A8(receiver, reason);
        case 0x8003525Cu:
            return sub_8003525C(receiver, reason);
        case 0x8003554Cu:
            return sub_8003554C(receiver, reason);
        case 0x80035704u:
            return sub_80035704(receiver, reason);
        case 0x80035A00u:
            return sub_80035A00(receiver, reason);
        case 0x8004B868u:
            return sub_8004B868(receiver, reason);
        case 0x8004D6B4u:
            return sub_8004D6B4(receiver, reason);
        case 0x8004E508u:
            return sub_8004E508(receiver, reason);
        case 0x8005BCE4u:
            return sub_8005BCE4(receiver, reason);
        case 0x80061EF0u:
            return sub_80061EF0(receiver, reason);
        case 0x80062254u:
            return sub_80062254(receiver, reason);
        case 0x80062650u:
            return sub_80062650(receiver, reason);
        case 0x800629BCu:
            return sub_800629BC(receiver, reason);
        case 0x80062FB8u:
            return sub_80062FB8(receiver, reason);
        case 0x8007741Cu:
            return sub_8007741C(receiver, reason);
        default:
            /* TODO Bind other object classes when native execution requires them */
            fprintf(stderr, "Unimplemented object cleanup %08X receiver %08X reason %u\n", target, receiver, reason);
            abort();
    }
}

void sub_8007115C(uint32 object)
{
    while (object)
    {
        uint32 next = r_u32(object + 28u);
        if (!(r_u16(object + 78u) & 0x20u))
        {
            uint32 method_table = r_u32(object + 68u);
            uint32 receiver = object + (uint32)(sint32)(sint16)r_u16(method_table + 8u);
            uint32 method = r_u32(method_table + 12u);
            apocalypse_object_cleanup(method, receiver, 3u);
        }
        object = next;
    }
}

void sub_80032B64(void)
{
    sub_80032AF4(r_u32(0x800FF434u));
    sub_80032AF4(r_u32(0x800FF438u));
    sub_80032AF4(r_u32(0x800FF43Cu));
    sub_80032AF4(r_u32(0x800FF440u));
    sub_80032AF4(r_u32(0x800FF45Cu));
    sub_80032AF4(r_u32(0x800FF460u));
    sub_80032AF4(r_u32(0x800FF444u));
    sub_80032AF4(r_u32(0x800FF448u));
    sub_80032AF4(r_u32(0x800FF44Cu));
    sub_80032AF4(r_u32(0x800FF450u));
    sub_80032AF4(r_u32(0x800FF454u));
    sub_80032AF4(r_u32(0x800FF464u));
    sub_80032AF4(r_u32(0x800FF458u));
}

void sub_8001B708(void)
{
    sint32 v2;
    sint32 i;
    v2 = r_u32(0x800FF1ACu);
    if (r_u32(0x800FF1ACu))
    {
        for (i = r_u32(((uint32)((r_u32(0x800FF1ACu) + 12))));; i = r_u32(((uint32)((i + 12)))))
        {
            sub_8006BC20(v2);
            v2 = i;
            if (!i)
                break;
        }
    }
    w_u32(0x800FF1ACu, 0);
}

uint32 sub_8001BAC0(void)
{
    uint32 entry = 0x800F2610u;
    uint32 next = 0x800F2628u;
    w_u32(0x800FF1D4u, 0u);
    w_u32(0x800FF1D8u, entry);
    do
    {
        w_u32(entry + 20u, entry < 0x800F3DF8u ? next : 0u);
        w_u32(entry + 16u, 0u);
        entry += 24u;
        next += 24u;
    } while (entry < 0x800F3E10u);
    return entry < 0x800F3E10u;
}

uint32 sub_80069A94(void)
{
    uint32 result = r_u32(0x800FF6ACu);
    if ((sint32)result >= 0)
    {
        sub_8008DF18((uint32)(sint32)(sint16)r_u16(0x800FF6ACu));
        result = 0xFFFFFFFFu;
        w_u32(0x800FF6ACu, result);
    }
    if (r_u32(0x800FF6A8u))
    {
        result = sub_8006BC20(r_u32(0x800FF6A8u));
        w_u32(0x800FF6A8u, 0);
    }
    if ((sint32)r_u32(0x800FF6A4u) >= 0)
    {
        sub_80069CF8(r_u32(0x800FF6A4u), r_u32(0x800FF6D8u));
        result = 0xFFFFFFFFu;
        w_u32(0x800FF6A4u, result);
        w_u32(0x800FF6D8u, 0);
    }
    return result;
}

uint32 sub_80069B8C(void)
{
    sint32 result = (sint32)r_u32(0x800FF6A0u);
    if (((sint32)r_u32(0x800FF6A0u) >= 0))
    {
        sub_80069CF8(r_u32(0x800FF6A0u), r_u32(0x800FF6D4u));
        result = -1;
        w_u32(0x800FF6A0u, -1);
        w_u32(0x800FF6D4u, 0);
    }
    return result;
}

void sub_80063F20(void)
{
    if (r_u32(0x800FF61Cu))
    {
        sub_8006BC20(r_u32(0x800FF61Cu));
        w_u32(0x800FF61Cu, 0u);
    }
    w_u32(0x800FF618u, 0u);
}

void sub_8002E2B8(void)
{
    if (r_u32(0x800FF230u))
    {
        sub_8006BC20(r_u32(0x800FF230u));
        w_u32(0x800FF230u, 0);
        w_u32(0x800FF234u, 0);
    }
}

uint32 sub_8007E41C(uint32 a1, uint32 a2)
{
    sint32 v2;
    uint32 v3;
    sint32 v4;
    uint32 v5;
    sint32 v6;
    sint32 v7;
    sint32 v8;
    sint32 v9;
    sint32 v10;
    sint32 v11;
    uint32 v12;
    sint32 v13;
    uint32 v14;
    uint32 v15;
    sint32 result;
    sint32 v17;
    sint32 v18;
    uint32 v19;
    sint32 v20;
    uint32 v21;
    sint32 v22;
    sint32 v23;
    sint32 v24;
    sint32 v25;
    v2 = ((sint32)(r_u32(a2)));
    v3 = (a2 + (1) * 4u);
    v4 = (408 * a1);
    v5 = (0x800EDA30u + ((408 * a1)) * 4u);
    w_u32((v5 + (1) * 4u), v2);
    v6 = r_u32((v3 += 4u, v3 - 4u));
    w_u32((v5 + (2) * 4u), v6);
    v7 = r_u32((v3 += 4u, v3 - 4u));
    w_u32((v5 + (3) * 4u), v7);
    v8 = ((sint32)(r_u32(v3)));
    v9 = ((sint32)(r_u32((v5 + (3) * 4u))));
    (v3 += 4u);
    w_u32((v5 + (4) * 4u), v8);
    w_u16((((uint32)(v5)) + (14) * 2u), r_u16(((uint32)(v3))));
    v10 = ((v9 - v2) / ((sint16)(r_u16((((uint32)(v5)) + (14) * 2u)))));
    v11 = 0;
    v12 = v5;
    v13 = a1;
    v14 = (((uint32)(v3)) + (1) * 2u);
    v15 = (v3 + (1) * 4u);
    v5 = ((v5 & 0xFFFF0000u) | (((r_u16(v14)) & 0xFFFFu) << 0));
    w_u32(v12, 1);
    w_u16((((uint32)(v12)) + (15) * 2u), ((uint16)(v5)));
    w_u32((v12 + (5) * 4u), v10);
    while (1)
    {
        result = (v11 < ((sint16)(r_u16((((uint32)(v12)) + (15) * 2u)))));
        v17 = 0;
        if ((v11 >= ((sint16)(r_u16((((uint32)(v12)) + (15) * 2u))))))
            break;
        v18 = v11;
        while ((v17 < ((sint16)(r_u16((((uint32)(v12)) + (14) * 2u))))))
        {
            v19 = (v15 + (2) * 4u);
            v20 = ((sint32)(r_u32(v19)));
            v21 = (v19 + (1) * 4u);
            w_u32((0x800EDA30u + (((v18 + 8) + v4)) * 4u), ((sint32)(v21)));
            v22 = r_u32((0x800EAEF8u + (((16 * r_u32((0x800FF778u + (v13) * 4u))) + 5)) * 4u));
            v23 = (v20 <= 0);
            v24 = (v20 - 1);
            if (!v23)
            {
                do
                {
                    v25 = v24--;
                    w_u32(v21, ((v22 + (36 * r_u32(v21))) + 12));
                    (v21 += 4u);
                } while ((v25 > 0));
            }
            v15 = (v21 + (1) * 4u);
            v18 += 20;
            ++v17;
        }

        ++v11;
    }

    return result;
}

uint32 sub_80069DF0(uint32 a1, uint32 a2, uint32 a3)
{
    uint32 product, volume;
    if ((sint32)a1 < 0)
        return 0;
    product = (uint32)(sint32)(sint16)a2 * (uint32)(sint32)(sint16)r_u16(0x800ECC7Au);
    volume = (uint32)((sint32)(product << 2) >> 16);
    return sub_8006A4C4(r_u32(0x800E53A8u + a1 * 8u), r_u8(0x800E53A8u + ((a1 * 8u) | 4u)), volume, volume, a3);
}

uint32 sub_80094544(void)
{
    sint32 result;
    result = 1;
    if ((r_u32(0x80104584u) == 1))
        return 255;
    w_u32(0x80104584u, 1);
    return result;
}

uint32 sub_8002E148(uint32 a1, uint32 a2, uint32 a3, uint32 a4)
{
    sint32 result;
    uint32 v11;
    uint32 v12;
    sint32 v13;
    short v14;
    short v15;
    sint32 v16;
    sint32 v17;
    sint32 v18;
    uint32 v19;
    result = sub_8002E13C();
    if (!result)
    {
        v11 = ((uint32)(sub_8006E278(a1)));
        v12 = a2;
        v13 = a3;
        v14 = r_u16((((uint32)(v11)) + (3) * 2u));
        w_u16(0x800FF240u, (((v14 & 0xF) << 6) + (((sint32)(r_u8(v11))) >> (2 - (((unsigned short)((v14 & 0x180))) >> 7)))));
        v15 = r_u8((v11 + (1) * 1u));
        w_u16(0x800FF244u, (v12 >> 1));
        w_u16(0x800FF23Au, a2);
        w_u16(0x800FF246u, a3);
        w_u16(0x800FF23Cu, a3);
        w_u8(0x800FF23Eu, a4);
        w_u16(0x800FF242u, (v15 + ((16 * v14) & 0x100)));
        v16 = (v12 * a3);
        result = sub_8006B864((2 * v16), 0, 1);
        v17 = 0;
        if (result)
        {
            w_u32(0x800FF230u, result);
            w_u32(0x800FF234u, (result + v16));
            while (1)
            {
                result = (v17 < v13);
                if ((v17 >= v13))
                    break;
                v18 = 0;
                if (v12)
                {
                    do
                    {
                        w_u8(((uint32)(((r_u32(0x800FF230u) + (v17 * ((unsigned short)(r_u16(0x800FF23Au))))) + v18))), 0);
                        v19 = ((uint32)(((r_u32(0x800FF234u) + (v17 * ((unsigned short)(r_u16(0x800FF23Au))))) + v18++)));
                        w_u8(v19, 0);
                    } while ((v18 < ((sint32)(v12))));
                }
                ++v17;
            }
        }
    }
    return result;
}

uint32 sub_8002E13C(void)
{
    return (r_u32(0x800FF230u) != 0);
}

uint32 sub_800153F8(uint32 a1, uint32 a2)
{
    sint32 v4;
    sint32 i;
    sint32 v6;
    sint32 result;
    v4 = 0;
    if (!(r_u8(((uint32)((a1 + 10))))))
        return 0;
    for (i = a1;; i += 28)
    {
        v6 = (sub_80067724(a2, r_u32(((uint32)((i + 24))))) != 0);
        result = v4;
        if (v6)
            break;
        if ((++v4 >= r_u8(((uint32)((a1 + 10))))))
            return 0;
    }

    return result;
}

uint32 sub_80015590(uint32 a1, uint32 a2, uint32 a3)
{
    sint32 result;
    result = sub_800153F8(a1, a2);
    w_u8(((uint32)(((a1 + (28 * result)) + 36))), a3);
    return result;
}

uint32 sub_80016014(uint32 a1)
{
    sint32 v2;
    short v3;
    sint32 v4;
    sint32 v5;
    sint32 v6;
    v2 = r_u32(((uint32)(a1)));
    w_u16(0x800ECC7Au, r_u16(((uint32)((a1 + 4)))));
    w_u16(0x800ECC78u, r_u16(((uint32)((a1 + 8)))));
    v3 = r_u16(((uint32)((a1 + 12))));
    w_u32(0x800FF384u, v2);
    w_u16(0x800ECC7Cu, v3);
    w_u16(0x800EC47Au, r_u8(((uint32)((a1 + 16)))));
    v4 = r_u8(((uint32)((a1 + 17))));
    w_u16(0x800EC474u, r_u8(((uint32)((a1 + 17)))));
    v5 = r_u8(((uint32)((a1 + 18))));
    w_u16(0x800EC476u, r_u8(((uint32)((a1 + 18)))));
    v6 = r_u8(((uint32)((a1 + 19))));
    w_u16(0x800EC478u, r_u8(((uint32)((a1 + 19)))));
    w_u16(0x800EC49Au, r_u8(((uint32)((a1 + 20)))));
    w_u16(0x800EC494u, r_u8(((uint32)((a1 + 21)))));
    w_u16(0x800EC496u, r_u8(((uint32)((a1 + 22)))));
    w_u16(0x800EC498u, r_u8(((uint32)((a1 + 23)))));
    sub_8006FDFC(0x800EC0F8u, v4, v5, v6, r_u8(a1 + 16u));
    return sub_8006FE14(0x800EC0F8u, 3, 2, 1, 0u, (uint32)(sint32)(sint16)r_u16(0x800EC494u), (uint32)(sint32)(sint16)r_u16(0x800EC496u), (uint32)(sint32)(sint16)r_u16(0x800EC498u), (uint32)(sint32)(sint16)r_u16(0x800EC49Au));
}

/* Blur the alternating menu buffers before uploading the selected image */
uint32 sub_8002E2E8(void)
{
    uint32 selected, other, toggle;
    sint32 row;
    if (r_u32(0x800FF230u) == 0u)
        return r_u32(0x800FF230u);
    toggle = r_u8(0x800FF238u) == 0u;
    w_u8(0x800FF238u, toggle);
    selected = r_u32(0x800FF230u + 4u * toggle);
    other = r_u32(0x800FF230u + 4u * (toggle == 0u));
    for (row = 1; row < (sint32)r_u16(0x800FF23Cu) - 1; ++row)
    {
        uint32 stride = r_u16(0x800FF23Au);
        uint32 top = r_u32(selected + (uint32)(row - 1) * stride);
        uint32 bottom = r_u32(selected + (uint32)(row + 1) * stride);
        uint32 carry = 0u;
        sint32 column = 1;
        for (;;)
        {
            uint32 top_second = (top >> 16) & 255u;
            uint32 bottom_second = (bottom >> 16) & 255u;
            uint32 top_last = top >> 24;
            uint32 bottom_last = bottom >> 24;
            uint32 output = carry;
            uint32 next_top, next_bottom;
            sint32 next_column = column + 2;
            uint32 top_offset;
            output |= (((top & 255u) + top_second + (bottom & 255u) + bottom_second) >> 2) << 8;
            output |= ((((top >> 8) & 255u) + top_last + ((bottom >> 8) & 255u) + bottom_last) >> 2) << 16;
            stride = r_u16(0x800FF23Au);
            top_offset = (uint32)(row - 1) * stride;
            if (next_column >= (sint32)stride - 1)
                break;
            next_top = r_u32(selected + top_offset + (uint32)next_column + 1u);
            next_bottom = r_u32(selected + (uint32)(row + 1) * stride + (uint32)next_column + 1u);
            output |= ((top_second + (next_top & 255u) + bottom_second + (next_bottom & 255u)) >> 2) << 24;
            w_u32(selected + (uint32)row * stride + (uint32)next_column - 3u, output);
            w_u32(other + top_offset + (uint32)next_column - 3u, output);
            carry = (top_last + ((next_top >> 8) & 255u) + bottom_last + (next_bottom >> 24)) >> 2;
            top = next_top;
            bottom = next_bottom;
            column = next_column + 2;
        }
    }
    sub_8002E4D8(4u);
    return xport_draft_host_sub_800885A4_p1((const sint16 *)psx_addr(0x800FF240u, 8u), selected);
}

uint32 sub_8007FE68(void)
{
    uint32 output = 0x800F5130u;
    uint32 amplitude, phase;
    for (amplitude = 0u; amplitude < 16u; ++amplitude)
    {
        for (phase = 0u; phase < 64u; ++phase)
        {
            sint32 product = (sint32)(sint16)r_u16(0x800F863Cu + phase * 0x100u) * (sint32)amplitude;
            w_u16(output, (uint32)(product >> 4));
            output += 2u;
        }
    }
    w_u32(0x800FFAB0u, 1u);
    return 1u;
}

void sub_800854D8(uint32 a1)
{
    w_u32(a1, 4096);
    w_u32((a1 + (1) * 4u), 0);
    w_u32((a1 + (2) * 4u), 4096);
    w_u32((a1 + (3) * 4u), 0);
    w_u32((a1 + (4) * 4u), 4096);
}

static uint32 draft_809B0_clip(sint32 x, sint32 y, sint32 z, const sint32 bounds[6])
{
    uint32 flags = (bounds[0] < x) | ((uint32)(x < bounds[1]) << 1) | ((uint32)(bounds[2] < y) << 2) | ((uint32)(y < bounds[3]) << 3) | ((uint32)(z < bounds[4]) << 4) | ((uint32)(bounds[5] < z) << 5);
    if (z < 0)
        flags ^= 15u;
    return flags;
}

static uint32 draft_809B0_divide(uint32 numerator, uint32 denominator)
{
    sint32 signed_numerator = (sint32)numerator;
    sint32 signed_denominator = (sint32)denominator;
    if (!denominator)
        return signed_numerator < 0 ? 1u : 0xFFFFFFFFu;
    if (numerator == 0x80000000u && denominator == 0xFFFFFFFFu)
        return numerator;
    return (uint32)(signed_numerator / signed_denominator);
}

uint32 sub_800809B0(uint32 vertices, uint32 count, uint32 translation)
{
    sint32 vector[3];
    if (!count)
        return 0xFFu;
    vector[0] = (sint32)r_u32(translation);
    vector[1] = (sint32)r_u32(translation + 4u);
    vector[2] = (sint32)r_u32(translation + 8u);
    return apocalypse_transform_geometry_vertices(vertices, count, vector);
}

uint32 apocalypse_transform_geometry_vertices(uint32 vertices, uint32 count, const sint32 translation[3])
{
    sint32 bounds[6], depth, x, y;
    uint32 bounds_source, bound_word, aggregate = 0xFFFFu;
    uint32 translate_x, translate_y, translate_z, projected, transformed;
    uint32 packed_xy, input_depth, flags, screen, next_xy, next_depth;
    uint32 final_value, references, output, projected_base, pair, interpolation;
    uint32 first, second, first_screen, second_screen, length;
    uint32 delta_x, delta_y, normalized_x, normalized_y, generated_xy;
    sint32 origin_x, origin_y;
    if (!count)
        return 0xFFu;
    bounds_source = r_u32(0x800FFB08u);
    bound_word = r_u32(bounds_source);
    bounds[0] = (sint32)(bound_word & 0xFFFFu);
    bounds[2] = (sint32)(bound_word >> 16);
    bound_word = r_u32(bounds_source + 4u);
    bounds[1] = (sint32)(bound_word & 0xFFFFu);
    bounds[3] = (sint32)(bound_word >> 16);
    bound_word = r_u32(bounds_source + 8u);
    bounds[4] = (sint32)(bound_word & 0xFFFFu);
    bounds[5] = (sint32)(bound_word >> 16);
    translate_x = (uint32)translation[0];
    translate_y = (uint32)translation[1];
    translate_z = (uint32)translation[2];
    xport_draft_gte_control_write(5u, 0u);
    xport_draft_gte_control_write(6u, 0u);
    xport_draft_gte_control_write(7u, 0u);
    projected = r_u32(0x800FFAC0u);
    transformed = r_u32(0x800FFAC4u);
    packed_xy = r_u32(vertices);
    input_depth = r_u32(vertices + 4u) + translate_z;
    packed_xy = ((packed_xy + translate_x) & 0xFFFFu) | (((uint32)((sint32)packed_xy >> 16) + translate_y) << 16);
    for (;;)
    {
        xport_draft_gte_data_write(0u, packed_xy);
        xport_draft_gte_data_write(1u, input_depth);
        flags = r_u16(vertices + 6u);
        vertices += 8u;
        --count;
        xport_draft_gte_execute(0x180001u);
        if (flags & 0x10u)
            break;
        /* Original loop fetches the following vertex before storing this result */
        next_xy = r_u32(vertices);
        next_depth = r_u32(vertices + 4u) + translate_z;
        next_xy = ((next_xy + translate_x) & 0xFFFFu) | (((uint32)((sint32)next_xy >> 16) + translate_y) << 16);
        x = (sint32)xport_draft_gte_data_read(9u);
        y = (sint32)xport_draft_gte_data_read(10u);
        depth = (sint32)xport_draft_gte_data_read(11u);
        w_u32(transformed, ((uint32)x & 0xFFFFu) | ((uint32)y << 16));
        screen = xport_draft_gte_data_read(14u);
        w_u16(projected + 4u, (uint16)depth);
        w_u32(projected, screen);
        flags = draft_809B0_clip((sint16)screen, (sint32)screen >> 16, depth, bounds);
        if ((sint32)xport_draft_gte_control_read(31u) < 0)
            flags |= 0x40u;
        flags |= (flags << 8) ^ 0xFF00u;
        w_u16(projected + 6u, (uint16)flags);
        aggregate &= flags;
        projected += 8u;
        transformed += 8u;
        packed_xy = next_xy;
        input_depth = next_depth;
        if (!count)
        {
            w_u32(0x1F8001D4u, projected);
            return aggregate;
        }
    }
    vertices -= 8u;
    ++count;
    xport_draft_gte_control_write(0u, 0x28F5u);
    xport_draft_gte_control_write(1u, 0u);
    xport_draft_gte_control_write(2u, 4096u);
    xport_draft_gte_control_write(3u, 0u);
    xport_draft_gte_control_write(4u, 4096u);
    xport_draft_gte_control_write(24u, 0u);
    xport_draft_gte_control_write(25u, 0u);
    references = vertices;
    output = projected;
    projected_base = r_u32(0x800FFAC0u);
    do
    {
        pair = r_u32(references);
        interpolation = r_u32(references + 4u);
        first = projected_base + (pair & 0xFFFFu);
        first_screen = r_u32(first);
        depth = (sint16)r_u32(first + 4u);
        second = projected_base + (pair >> 16);
        second_screen = r_u32(second);
        origin_y = (sint32)first_screen >> 16;
        origin_x = (sint16)first_screen;
        delta_x = (uint32)(sint32)(sint16)second_screen - (uint32)origin_x;
        delta_y = (uint32)origin_y - (uint32)((sint32)second_screen >> 16);
        xport_draft_gte_data_write(9u, delta_x);
        xport_draft_gte_data_write(10u, delta_y);
        xport_draft_gte_execute(0xA00428u);
        length = sub_80085B54(xport_draft_gte_data_read(25u) + xport_draft_gte_data_read(26u));
        normalized_y = draft_809B0_divide(delta_x << 12, length);
        normalized_x = draft_809B0_divide(delta_y << 12, length);
        xport_draft_gte_data_write(9u, normalized_x);
        xport_draft_gte_data_write(10u, normalized_y);
        xport_draft_gte_data_write(8u, interpolation);
        xport_draft_gte_execute(0x198003Du);
        generated_xy = (xport_draft_gte_data_read(25u) & 0xFFFFu) | (xport_draft_gte_data_read(26u) << 16);
        xport_draft_gte_data_write(0u, generated_xy);
        xport_draft_gte_data_write(1u, (uint32)depth);
        xport_draft_gte_execute(0x180001u);
        screen = xport_draft_gte_data_read(14u);
        x = (sint32)((uint32)(sint32)(sint16)screen + (uint32)origin_x);
        y = (sint32)((uint32)((sint32)screen >> 16) + (uint32)origin_y);
        flags = draft_809B0_clip(x, y, depth, bounds);
        flags |= (flags << 8) ^ 0xFF00u;
        aggregate &= flags;
        w_u32(output, ((uint32)x & 0xFFFFu) | ((uint32)y << 16));
        w_u32(output + 4u, (flags << 16) | ((uint32)depth & 0xFFFFu));
        references += 8u;
        output += 8u;
        --count;
        final_value = length;
    } while (count);
    bounds_source = r_u32(0x800FFB08u);
    bound_word = r_u32(bounds_source + 12u);
    xport_draft_gte_control_write(24u, bound_word << 16);
    xport_draft_gte_control_write(25u, (bound_word >> 16) << 16);
    w_u32(0x1F8001D4u, final_value);
    return aggregate;
}

uint32 sub_80082394(uint32 packet, uint32 area, uint32 *step, uint32 *subdivisions)
{
    uint32 leading_count = 32u, count, magnitude = area >> 11;
    /* The shifted area is nonnegative, so LZCS counts leading zero bits */
    while (magnitude)
    {
        --leading_count;
        magnitude >>= 1;
    }
    xport_draft_gte_data_write(6u, (r_u32(packet + 4u) & 0xFF000000u) | 0x08000000u);
    count = 17u - (leading_count >> 1);
    if ((sint32)count >= 8)
        count = 7u;
    *subdivisions = count;
    /* MIPS DIVU supplies all-one LO for a zero divisor */
    *step = count == 0u ? 0xFFFFFFFFu : 4096u / count;
    return packet;
}

void sub_8008270C(uint32 count, uint32 corner0, uint32 corner1, uint32 corner2)
{
    uint32 position = 0x1F800000u;
    sub_800826C4(position, count, 16u, corner0, corner1);
    position += count * 16u;
    sub_800826C4(position, count, 112u, corner1, corner2);
    position += count * 112u;
    sub_800826C4(position, count, 0xFFFFFF80u, corner2, corner0);
}

/* TODO Missing call adapter JUMPOUT */
/* TODO Missing call adapter xport_draft_missing_gte_adapter */
uint32 sub_80082D1C(uint32 geometry, xport_draft_polygon_strip_context *context)
{
    uint32 packet = context->packet_cursor;
    uint32 words[6];
    uint32 clip = context->clipping_mask;
    uint32 remaining = context->remaining_segments;
    uint32 old_remaining, next_clip;
    unsigned index;
    context->disposition = 0u;
    for (index = 0; index < 6; ++index)
        words[index] = context->packet_words[index];
    do
    {
        words[2] |= context->texture_high0;
        if (packet >= context->packet_limit)
        {
            context->packet_cursor = packet;
            context->clipping_mask = clip;
            context->remaining_segments = remaining;
            for (index = 0; index < 6; ++index)
                context->packet_words[index] = words[index];
            xport_draft_gte_data_write(6u, 0u);
            context->disposition = 1u;
            return packet;
        }
        for (index = 0; index < 5; ++index)
            w_u32(packet + 4u + index * 4u, words[index]);
        w_u32(packet + 24u, words[5] | context->texture_high1);
        words[0] = r_u32(geometry + 28u);
        words[1] = r_u32(geometry + 16u);
        words[2] = r_u16(geometry + 24u);
        words[3] = r_u32(geometry + 156u);
        words[4] = r_u32(geometry + 144u);
        words[5] = r_u16(geometry + 152u);
        geometry += 16u;
        next_clip = words[1] & words[4];
        old_remaining = clip & next_clip;
        clip = next_clip & context->frustum_mask;
        if (!old_remaining)
        {
            packet += 52u;
            context->packet_stride = 52u;
            w_u32(packet - 52u, packet + 0x0C000000u);
            for (index = 0; index < 6; ++index)
                w_u32(packet - 24u + index * 4u, words[index]);
        }
        old_remaining = remaining;
        --remaining;
    } while (old_remaining != 0u);
    context->packet_cursor = packet;
    context->clipping_mask = clip;
    context->remaining_segments = remaining;
    for (index = 0; index < 6; ++index)
        context->packet_words[index] = words[index];
    return packet;
}

uint32 sub_8001A97C(uint32 text)
{
    return apocalypse_text_width((const char *)psx_addr(text, 1u));
}

uint32 apocalypse_text_width(const char *a1)
{
    sint32 v2;
    sint32 v3;
    sint32 v4;
    sint32 v5;
    sint32 v6;
    v2 = 0;
    v3 = 0;
    while ((uint8)*a1)
    {
        v4 = sub_8001A784((uint8)*a1);
        v5 = (v4 < 0);
        v6 = (8 * v4);
        if (v5)
        {
            v2 += 10;
        }
        else
        {
            ++v3;
            v2 += (r_u8(((uint32)(((v6 + r_u32(0x800FF6E0u)) + 2)))) + ((sint8)(r_u8(((uint32)((v6 + r_u32(0x800FF6E0u))))))));
        }
        (a1 += 1u);
    }

    return (((v2 * ((unsigned short)(r_u16(0x800FF1B8u)))) >> 8) + (3 * v3));
}

/* TODO Missing call adapter SHIDWORD */
uint32 sub_8006D0D4(uint32 x, uint32 y, uint32 packet, uint32 texture)
{
    sint32 offset = (sint32)(sint8)r_u8(texture) * 512;
    sint64 offset_product = (sint64)offset * 1717986919LL;
    uint32 width = (uint32)r_u8(texture + 2u) << 9;
    uint32 top = y + (uint32)(sint32)(sint8)r_u8(texture + 1u);
    sint64 width_product = (sint64)width * 1717986919LL;
    uint32 height = r_u8(texture + 3u), left, right, bottom;
    sint32 offset_high = (sint32)(offset_product >> 32);
    sint32 width_high = (sint32)(width_product >> 32);
    w_u16(packet + 10u, (uint16)top);
    w_u16(packet + 26u, (uint16)(top + height));
    left = x + (uint32)((offset_high >> 7) - (offset >> 31));
    w_u16(packet + 8u, (uint16)left);
    right = left + (uint32)(width_high >> 7);
    w_u16(packet + 16u, (uint16)right);
    bottom = r_u16(packet + 26u);
    w_u16(packet + 24u, (uint16)left);
    w_u16(packet + 18u, (uint16)top);
    w_u16(packet + 32u, (uint16)right);
    w_u16(packet + 34u, (uint16)bottom);
    return bottom;
}

static void card_missing_boundary(uint32 address, uint32 channel)
{
    fprintf(stderr, "Missing native card dependency %08X (channel %08X)\n", address, channel);
    abort();
}

uint32 sub_80010AC8(uint32 port, uint32 slot)
{
    uint32 channel = (port << 4) + slot;
    sint32 event;
    switch (r_u32(0x800FEED0u))
    {
        case 0:
            _card_info((sint32)channel);
            w_u32(0x800FEED0u, 1u);
            w_u32(0x800FEEF4u, 0u);
            w_u32(0x800FEEC4u, 0u);
            break;
        case 1:
            event = (sint32)sub_8001081C();
            if (!event)
                break;
            if (event == 1)
            {
                w_u32(0x800FEECCu, 1u);
                w_u32(0x800FEED0u, r_u32(0x800FEEC8u) == 1u ? 4u : 2u);
                break;
            }
            if (event == 4)
            {
                w_u32(0x800FEECCu, 2u);
                /* TODO Outside selected set: drain card events and wait for event */
                card_missing_boundary(0x800109F0u, channel);
                _new_card();
                psx_bios_card_write_guest(channel, 63u, 0u);
                card_missing_boundary(0x80010980u, channel);
                w_u32(0x800FEED0u, 2u);
                w_u32(0x800FEEC8u, 0u);
                break;
            }
            w_u32(0x800FEECCu, event == 3 ? 0xFFFFFFFFu : 0xFFFFFFFDu);
            w_u32(0x800FEED0u, 4u);
            w_u32(0x800FEEC8u, 0u);
            break;
        case 2:
            sub_80010938();
            psx_bios_card_load_guest(channel);
            w_u32(0x800FEED0u, 3u);
            w_u32(0x800FEEF4u, 0u);
            break;
        case 3:
            event = (sint32)sub_8001081C();
            if (!event)
                break;
            w_u32(0x800FEED0u, 4u);
            if (event == 1)
            {
                w_u32(0x800FEEC8u, 1u);
                break;
            }
            w_u32(0x800FEECCu, event == 3 ? 0xFFFFFFFFu : event == 4 ? 0xFFFFFFFEu : 0xFFFFFFFDu);
            w_u32(0x800FEEC8u, 0u);
            break;
        case 4:
            w_u32(0x800FEED0u, 0u);
            w_u32(0x800FEEC4u, r_u32(0x800FEECCu));
            break;
        default:
            break;
    }
    return r_u32(0x800FEEC4u);
}

uint32 sub_8001081C(void)
{
    uint32 result = TestEvent(r_u32(0x800FEED4u)) == 1 ? 1u : 0u;
    uint32 ticks;
    if (TestEvent(r_u32(0x800FEED8u)) == 1)
        result = 2u;
    if (TestEvent(r_u32(0x800FEEDCu)) == 1)
        result = 3u;
    if (TestEvent(r_u32(0x800FEEE0u)) == 1)
        result = 4u;
    ticks = r_u32(0x800FEEF4u);
    w_u32(0x800FEEF4u, ticks + 1u);
    return (sint32)ticks < 120 ? result : 2u;
}

uint32 sub_800155D4(uint32 a1, uint32 a2)
{
    return sub_80067724(a2, r_u32(((uint32)(((a1 + (28 * r_u8(((uint32)((a1 + 6)))))) + 24)))));
}

uint32 sub_800167D8(void)
{
    sub_800166AC();
    return sub_80016744();
}
