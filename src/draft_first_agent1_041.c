#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
#include <stdlib.h>

void sub_80084814(uint32 geometry, uint32 mirror_flags,
    const xport_draft_bounds *reference, const uint32 translation[3],
    xport_draft_bounds *result)
{
    uint32 packed[3], minimum[3], maximum[3], axis;
    FUNCTION_MARKER(0x80084814u, "SLUS_003.73");
    packed[0] = r_u32(geometry + 0x14u);
    packed[1] = r_u32(geometry + 0x18u);
    packed[2] = r_u32(geometry + 0x1Cu);
    for (axis = 0u; axis < 3u; ++axis)
    {
        minimum[axis] = (uint32)(sint32)(sint16)packed[axis] + translation[axis] + 2u;
        maximum[axis] = (uint32)(sint32)(sint16)(packed[axis] >> 16) + translation[axis] - 2u;
        if (mirror_flags & (1u << axis))
        {
            uint32 center = reference->minimum[axis] + reference->maximum[axis];
            uint32 mirrored_minimum = center - maximum[axis];
            maximum[axis] = center - minimum[axis];
            minimum[axis] = mirrored_minimum;
        }
    }
    for (axis = 0u; axis < 3u; ++axis)
    {
        result->minimum[axis] = minimum[axis];
        result->maximum[axis] = maximum[axis];
    }
}
#include <stdio.h>
#include <string.h>

/* Unverified draft; TODO Recover omitted call arguments, host-buffer adapters and signed field widths */

uint32 sub_8005E9E4(uint32 a1)
{
    sint32 result;
    sub_80063038(a1, 0, 0, -1);
    result = 1;
    w_u32(((uint32)((a1 + 460))), 1);
    return result;
}

uint32 sub_8007CA50(uint32 a1, uint32 a2)
{
    uint32 result;
    result = (0x800EAEF8u + ((16 * r_u8(((uint32)((a1 + 27)))))) * 4u);
    w_u32((result + (7) * 4u), (a2 + 4));
    return result;
}

uint32 sub_800772B8(uint32 a1, uint32 a2)
{
    uint32 v4;
    sint32 v5;
    sint32 v6;
    uint32 v7;
    sint32 v8;
    sint32 v9;
    sint32 result;
    sint32 v11;
    short v12;
    sub_80062924(a1);
    w_u32(((uint32)((a1 + 68))), 0x800A3DA4u);
    w_u32(((uint32)((a1 + 296))), a2);
    w_u32(((uint32)((a1 + 336))), a2);
    w_u32(((uint32)((a1 + 372))), 256);
    w_u32(((uint32)((a1 + 384))), 0);
    w_u16(((uint32)((a1 + 412))), 0);
    w_u16(((uint32)((a1 + 414))), 0);
    w_u32(((uint32)((a1 + 428))), 0);
    w_u32(((uint32)((a1 + 432))), 0);
    w_u32(((uint32)((a1 + 436))), 0);
    w_u32(((uint32)((a1 + 440))), 4096);
    w_u32(((uint32)((a1 + 444))), 0);
    w_u32(((uint32)((a1 + 448))), 0);
    w_u32(((uint32)((a1 + 452))), 0);
    w_u32(((uint32)((a1 + 456))), 4096);
    w_u32(((uint32)((a1 + 460))), 0);
    w_u32(((uint32)((a1 + 464))), 0);
    w_u32(((uint32)((a1 + 468))), 0);
    w_u32(((uint32)((a1 + 472))), 4096);
    w_u32(((uint32)((a1 + 476))), 0);
    w_u32(((uint32)((a1 + 480))), 0);
    w_u32(((uint32)((a1 + 484))), 0);
    w_u32(((uint32)((a1 + 488))), 4096);
    v4 = r_u32(((uint32)((a1 + 296))));
    w_u32(((uint32)((a1 + 536))), -1);
    w_u32(((uint32)((a1 + 540))), -1);
    w_u16(((uint32)((a1 + 546))), 784);
    w_u16(((uint32)((a1 + 544))), 256);
    w_u32(((uint32)((a1 + 568))), 0);
    w_u32(((uint32)((a1 + 572))), 15);
    w_u32(((uint32)((a1 + 576))), 0);
    v5 = r_u32((v4 + (2) * 4u));
    v6 = r_u32((v4 + (3) * 4u));
    w_u32(((uint32)((a1 + 308))), r_u32((v4 + (1) * 4u)));
    w_u32(((uint32)((a1 + 312))), v5);
    w_u32(((uint32)((a1 + 316))), v6);
    w_u32(((uint32)((a1 + 332))), 0);
    v7 = r_u32(((uint32)((a1 + 336))));
    v8 = r_u32((v7 + (2) * 4u));
    v9 = r_u32((v7 + (3) * 4u));
    w_u32(((uint32)((a1 + 344))), r_u32((v7 + (1) * 4u)));
    w_u32(((uint32)((a1 + 348))), v8);
    w_u32(((uint32)((a1 + 352))), v9);
    w_u16(((uint32)((a1 + 58))), 99);
    w_u32(((uint32)((a1 + 368))), 0);
    w_u16(((uint32)(a1)), 1);
    sub_80077680(a1);
    sub_80062A38(a1, 0x800FF904u);
    result = a1;
    w_u32(((uint32)((a1 + 564))), 7);
    v11 = r_u32(0x800FF900u);
    v12 = r_u16(((uint32)((a1 + 78))));
    w_u16(((uint32)((a1 + 212))), 0);
    w_u16(((uint32)((a1 + 78))), (v12 & 0xFFFD));
    w_u32(0x800FF900u, (v11 + 1));
    return result;
}

uint32 sub_800785D8(uint32 a1, uint32 a2, uint32 a3)
{
    sint32 result;
    result = a2;
    if (a3)
    {
        result = ((a2 - r_u32(0x800FF908u)) / a3);
        w_u32(0x800FF90Cu, a3);
        w_u32(0x800FFD0Cu, result);
    }
    else
    {
        w_u32(0x800FF908u, a2);
        w_u32(0x800FF90Cu, 0);
    }
    return result;
}

uint32 sub_800786A0(uint32 a1, uint32 a2, uint32 a3)
{
    sint32 result;
    if (a3)
    {
        result = ((a2 - r_u32(0x800FF920u)) / a3);
        w_u32(0x800FF924u, a3);
        w_u32(0x800FFD24u, result);
    }
    else
    {
        result = a2;
        w_u32(0x800FF920u, a2);
        w_u32(0x800FF924u, 0);
    }
    return result;
}

uint32 sub_80067CCC(uint32 a1, uint32 a2, uint32 a3)
{
    uint32 v3;
    sint32 result;
    sint32 v5;
    sint32 v6;
    sint32 v7;
    v3 = ((uint32)(r_u32(0x800FF794u)));
    result = -2146500608;
    while (v3)
    {
        result = ((r_u32((0x800EAEF8u + (((16 * r_u8((((uint32)(v3)) + (27) * 1u))) + 2)) * 4u)) >> 16) & 255u);
        if (((r_u32((0x800EAEF8u + (((16 * r_u8((((uint32)(v3)) + (27) * 1u))) + 2)) * 4u)) >> 16) & 255u))
        {
            v5 = r_u32((((uint32)(v3)) + (1) * 4u));
            v6 = r_u32((((uint32)(v3)) + (2) * 4u));
            v7 = r_u32((((uint32)(v3)) + (3) * 4u));
            result = (v5 < r_u32(a1));
            if ((v5 >= r_u32(a1)))
            {
                result = (r_u32(a2) < v5);
                if ((r_u32(a2) >= v5))
                {
                    result = (v7 < r_u32((a1 + (2) * 4u)));
                    if ((v7 >= r_u32((a1 + (2) * 4u))))
                    {
                        result = (r_u32((a2 + (2) * 4u)) < v7);
                        if ((r_u32((a2 + (2) * 4u)) >= v7))
                        {
                            result = (v6 < r_u32((a1 + (1) * 4u)));
                            if ((v6 >= r_u32((a1 + (1) * 4u))))
                            {
                                result = (r_u32((a2 + (1) * 4u)) < v6);
                                if ((r_u32((a2 + (1) * 4u)) >= v6))
                                {
                                    if (a3)
                                        result = (r_u16(v3) & 0xFFFE);
                                    else
                                        result = (r_u16(v3) | 1);
                                    w_u16(v3, result);
                                }
                            }
                        }
                    }
                }
            }
        }
        v3 = ((uint32)(r_u32((((uint32)(v3)) + (7) * 4u))));
    }

    return result;
}

uint32 sub_80066088(uint32 a1)
{
    uint32 v1;
    sint32 v2;
    sint32 result;
    v1 = r_u32(((uint32)(((4 * a1) + r_u32(0x800FF624u)))));
    v2 = r_u16(v1);
    if ((v2 == 6))
        return ((sint32)((v1 + (1) * 2u)));
    if ((r_u16(v1) >= 7u))
    {
        result = (r_u16(v1) < 8u);
        if ((r_u16(v1) >= 8u))
        {
            result = ((sint32)((v1 + (1) * 2u)));
            if ((r_u16(v1) >= 0xBu))
            {
                result = (r_u16(v1) < 0x3EAu);
                if ((r_u16(v1) < 0x3EAu))
                {
                    result = (r_u16(v1) < 0x3E8u);
                    if ((r_u16(v1) >= 0x3E8u))
                        return ((sint32)((v1 + (1) * 2u)));
                }
            }
        }
    }
    else
    {
        if ((r_u16(v1) < 4u))
        {
            result = 1;
            if ((r_u16(v1) < 2u))
            {
                if ((v2 == 1))
                    return ((sint32)((v1 + (3) * 2u)));
                return result;
            }
            return ((sint32)((v1 + (1) * 2u)));
        }
        result = 5;
        if ((v2 == 5))
            return ((sint32)((v1 + (2) * 2u)));
    }
    return result;
}

uint32 sub_8004B800(uint32 a1)
{
    sint32 result;
    sint32 v3;
    sub_80062F64(a1);
    result = a1;
    v3 = r_u32(0x800FF4D8u);
    w_u32(((uint32)((a1 + 68))), 0x800A2B18u);
    w_u16(((uint32)((a1 + 214))), -1);
    w_u16(((uint32)((a1 + 212))), 128);
    w_u16(((uint32)((a1 + 392))), 32);
    w_u16(((uint32)((a1 + 476))), 0);
    w_u32(((uint32)((a1 + 208))), 64);
    w_u32(((uint32)((a1 + 448))), 0);
    w_u32(0x800FF4D8u, (v3 + 1));
    return result;
}

uint32 sub_80062B50(uint32 a1, uint32 a2)
{
    short v2;
    uint32 v3;
    short v4;
    v2 = r_u16(a2);
    v3 = (a2 + (1) * 2u);
    w_u16((a1 + (8) * 2u), v2);
    v4 = r_u32(((v3 += 2u) - 2u));
    w_u16((a1 + (9) * 2u), v4);
    w_u16((a1 + (10) * 2u), r_u16(v3));
    return (v3 + (1) * 2u);
}

void sub_80029CC4(uint32 a1)
{
    if (r_u32(0x800FF21Cu))
    {
        if ((r_u32(((uint32)((r_u32(0x800FF21Cu) + 168)))) >= sub_8006696C((a1 + 4), (r_u32(0x800FF21Cu) + 24))))
            w_u16(((uint32)((a1 + 78))), (r_u16(((uint32)((a1 + 78)))) | (0x100u)));
    }
}

/* TODO The unrecorded controller constructor remains a named fail-fast boundary */
uint32 sub_80023B2C(uint32 object, uint32 target, uint32 flags);
uint32 sub_80051740(uint32 a1, uint32 a2)
{
    sint32 v4;
    short v5;
    sint32 v6;
    short v7;
    uint32 v8;
    unsigned short v9;
    short v10;
    sint32 v11;
    short v12;
    sint32 v13;
    sint8 v14;
    sint32 v15;
    short v16;
    sint32 v17;
    short v18;
    sint32 v19;
    sint32 v20;
    short v21;
    sint32 v22;
    short v23;
    sint32 v24;
    sint32 v25;
    sint32 result;
    a2 = (uint16)a2;
    switch (a2)
    {
        case 0:
            sub_800626C8(a1, 0x800FF524u);
            w_u16(((uint32)((a1 + 390))), 1);
            w_u16(((uint32)((a1 + 218))), 20);
            w_u16(((uint32)((a1 + 456))), 130);
            w_u16(((uint32)((a1 + 458))), 64);
            w_u8(((uint32)((a1 + 613))), 10);
            sub_8004BD04(a1, 2u, 5u);
            w_u16(((uint32)((a1 + 436))), 32);
            w_u16(((uint32)((a1 + 438))), 32);
            sub_8004BDCC(a1, 0, 1u, 20);
            sub_8004BDCC(a1, 1u, 1u, 10);
            sub_8004BDCC(a1, 1u, 2u, 64);
            sub_8004BDCC(a1, 1u, 3u, 16);
            sub_8004BDCC(a1, 1u, 4u, 45);
            v4 = a1;
            goto LABEL_7;

        case 1:
            sub_800626C8(a1, 0x800FF524u);
            w_u16(((uint32)((a1 + 390))), 1);
            v5 = r_u16(((uint32)(a1)));
            w_u16(((uint32)((a1 + 40))), 6144);
            w_u16(((uint32)((a1 + 38))), 6144);
            w_u16(((uint32)((a1 + 36))), 6144);
            w_u16(((uint32)(a1)), (v5 | 0x200));
            sub_800626F8(a1, 0, 208, 96, 96);
            w_u16(((uint32)((a1 + 218))), 60);
            w_u16(((uint32)((a1 + 456))), 200);
            w_u16(((uint32)((a1 + 458))), 96);
            w_u8(((uint32)((a1 + 613))), 10);
            sub_8004BD04(a1, 2u, 5u);
            w_u16(((uint32)((a1 + 436))), 32);
            w_u16(((uint32)((a1 + 438))), 32);
            sub_8004BDCC(a1, 0, 1u, 20);
            sub_8004BDCC(a1, 1u, 1u, 10);
            sub_8004BDCC(a1, 1u, 2u, 64);
            sub_8004BDCC(a1, 1u, 3u, 32);
            sub_8004BDCC(a1, 1u, 4u, 90);
            sub_8005171C(a1, 2, 0, 4096, 256, 128);
            sub_8005171C(a1, 4, 4064, 0x4000, 128, 128);
            sub_80062D70(a1);
            v6 = ((unsigned char)(r_u8(0x800FF644u)));
            v7 = r_u16(((uint32)((a1 + 456))));
            w_u16(((uint32)((a1 + 204))), 1024);
            w_u16(((uint32)((a1 + 200))), 96);
            goto LABEL_8;

        case 2:
            sub_800626C8(a1, 0x800FF524u);
            w_u16(((uint32)((a1 + 390))), 1);
            w_u16(((uint32)((a1 + 218))), 20);
            w_u16(((uint32)((a1 + 456))), 130);
            w_u16(((uint32)((a1 + 458))), 64);
            w_u8(((uint32)((a1 + 613))), 10);
            sub_8004BD04(a1, 2u, 5u);
            w_u16(((uint32)((a1 + 436))), 32);
            w_u16(((uint32)((a1 + 438))), 32);
            sub_8004BDCC(a1, 0, 1u, 20);
            sub_8004BDCC(a1, 1u, 1u, 10);
            sub_8004BDCC(a1, 1u, 2u, 64);
            sub_8004BDCC(a1, 1u, 3u, 16);
            sub_8004BDCC(a1, 1u, 4u, 45);
            v8 = ((uint32)(sub_8002FED8(124)));
            if (v8)
                v8 = sub_80023B2C(v8, 0x800FF5A0u, 0u);
            w_u32(((uint32)((a1 + 620))), v8);
            v9 = sub_8004BDFC(a1, 1u, 1u);
            v4 = a1;
            w_u32(((uint32)((r_u32(((uint32)((a1 + 620)))) + 40))), v9);
        LABEL_7:
            sub_8005171C(v4, 0, 128, 192, 256, 0);

            sub_8005171C(a1, 1, 0, 512, 128, 16);
            sub_8005171C(a1, 2, 512, 4096, 256, 0);
            sub_8005171C(a1, 3, 1024, 12288, 128, 96);
            sub_8005171C(a1, 4, 2048, 12288, 128, 256);
            sub_8005171C(a1, 5, 1024, 12288, 64, 256);
            sub_80062D70(a1);
            v6 = ((unsigned char)(r_u8(0x800FF644u)));
            v7 = r_u16(((uint32)((a1 + 456))));
            w_u16(((uint32)((a1 + 204))), 1024);
            w_u16(((uint32)((a1 + 200))), 64);
        LABEL_8:
            w_u16(((uint32)((a1 + 202))), v7);

            w_u8(((uint32)((a1 + 614))), sub_8006E080(-617804721, v6));
            w_u8(((uint32)((a1 + 615))), 10);
            goto LABEL_36;

        case 16:
            sub_800626C8(a1, 0x800A2754u);
            w_u16(((uint32)((a1 + 390))), 2);
            w_u16(((uint32)((a1 + 218))), 1);
            w_u16(((uint32)((a1 + 456))), 112);
            w_u16(((uint32)((a1 + 36))), 7000);
            w_u16(((uint32)((a1 + 38))), 5000);
            w_u16(((uint32)((a1 + 40))), 0x2000);
            v10 = r_u16(((uint32)(a1)));
            w_u16(((uint32)((a1 + 458))), 32);
            w_u8(((uint32)((a1 + 613))), 15);
            w_u16(((uint32)(a1)), (v10 | 0x200));
            sub_8004BD04(a1, 2u, 5u);
            w_u16(((uint32)((a1 + 436))), 16);
            w_u16(((uint32)((a1 + 438))), 32);
            sub_8004BDCC(a1, 0, 1u, 5);
            sub_8005171C(a1, 0, 128, 192, 256, 0);
            sub_8005171C(a1, 1, 0, 1024, 128, 64);
            goto LABEL_28;

        case 32:

        case 33:

        case 34:

        case 35:
            sub_800626C8(a1, 0x800FF52Cu);
            w_u16(((uint32)((a1 + 390))), 4);
            w_u16(((uint32)((a1 + 456))), 128);
            w_u16(((uint32)((a1 + 458))), 128);
            w_u8(((uint32)((a1 + 613))), 15);
            sub_8004BD04(a1, 2u, 5u);
            sub_80062D70(a1);
            v11 = ((unsigned char)(r_u8(0x800FF644u)));
            v12 = r_u16(((uint32)((a1 + 456))));
            w_u16(((uint32)((a1 + 204))), 1024);
            w_u16(((uint32)((a1 + 200))), 96);
            w_u16(((uint32)((a1 + 202))), v12);
            w_u8(((uint32)((a1 + 614))), sub_8006E080(-317030919, v11));
            w_u8(((uint32)((a1 + 615))), 10);
            switch (a2)
            {
                case ' ':
                    w_u16(((uint32)((a1 + 218))), 30);
                    w_u16(((uint32)((a1 + 436))), 32);
                    w_u16(((uint32)((a1 + 438))), 32);
                    sub_8004BDCC(a1, 0, 1u, 10);
                    sub_8004BDCC(a1, 1u, 1u, 5);
                    sub_8004BDCC(a1, 1u, 2u, 48);
                    sub_8004BDCC(a1, 1u, 3u, 16);
                    sub_8004BDCC(a1, 1u, 4u, 45);
                    sub_8005171C(a1, 0, 128, 192, 256, 0);
                    sub_8005171C(a1, 1, 0, 1024, 128, 64);
                    sub_8005171C(a1, 2, 512, 4096, 256, 64);
                    sub_8005171C(a1, 3, 1024, 0x2000, 128, 96);
                    sub_8005171C(a1, 4, 2048, 0x2000, 64, 128);
                    sub_8005171C(a1, 5, 1024, 0x2000, 64, 256);
                    v13 = (a1 + 4);
                    goto LABEL_37;

                case '!':
                    w_u16(((uint32)((a1 + 218))), 30);
                    w_u16(((uint32)((a1 + 436))), 32);
                    w_u16(((uint32)((a1 + 438))), 32);
                    sub_8004BDCC(a1, 0, 1u, 10);
                    sub_8004BDCC(a1, 1u, 1u, 5);
                    sub_8004BDCC(a1, 1u, 2u, 48);
                    sub_8004BDCC(a1, 1u, 3u, 16);
                    sub_8004BDCC(a1, 1u, 4u, 45);
                    sub_8005171C(a1, 0, 128, 256, 256, 0);
                    sub_8005171C(a1, 2, 232, 4096, 256, 0);
                    sub_8005171C(a1, 4, 2048, 12288, 128, 256);
                    break;

                case '"':
                    w_u16(((uint32)((a1 + 218))), 30);
                    w_u16(((uint32)((a1 + 436))), 32);
                    w_u16(((uint32)((a1 + 438))), 32);
                    sub_8004BDCC(a1, 0, 1u, 10);
                    sub_8004BDCC(a1, 1u, 1u, 5);
                    sub_8004BDCC(a1, 1u, 2u, 48);
                    sub_8004BDCC(a1, 1u, 3u, 16);
                    sub_8004BDCC(a1, 1u, 4u, 45);
                    sub_8005171C(a1, 0, 128, 192, 256, 0);
                    sub_8005171C(a1, 1, 0, 4096, 0, 128);
                    sub_8005171C(a1, 4, 3072, 12288, 32, 256);
                    break;

                default:
                    v13 = (a1 + 4);
                    if ((a2 != 35))
                        goto LABEL_37;
                    w_u16(((uint32)((a1 + 218))), 30);
                    sub_8004BDCC(a1, 1u, 1u, 5);
                    sub_8004BDCC(a1, 1u, 2u, 48);
                    sub_8004BDCC(a1, 1u, 3u, 32);
                    sub_8004BDCC(a1, 1u, 4u, 45);
                    sub_8005171C(a1, 2, 0, 4096, 256, 0);
                    sub_8005171C(a1, 4, 4096, 0x4000, 256, 256);
                    break;
            }

            v13 = (a1 + 4);
        LABEL_37:
            v24 = sub_80067A18(v13, 64, 512);

            if ((v24 == -1))
                return sub_80062D84(a1);
            v25 = (sint16)r_u16(a1 + 456u);
            result = (sint32)((uint32)v25 << 12);
            if (((sint32)((uint32)v24 - r_u32(a1 + 8u)) >> 12) < (sint32)((uint32)v25 << 1))
            {
                result = (sint32)((uint32)v24 - (uint32)result);
                w_u32(((uint32)((a1 + 8))), result);
            }
            return result;

        case 48:

        case 49:

        case 50:
            sub_800626C8(a1, 0x800FF534u);
            w_u16(((uint32)((a1 + 390))), 8);
            w_u16(((uint32)((a1 + 218))), 10);
            w_u16(((uint32)((a1 + 456))), 140);
            w_u16(((uint32)((a1 + 458))), 32);
            w_u8(((uint32)((a1 + 613))), 15);
            sub_8004BD04(a1, 2u, 5u);
            w_u16(((uint32)((a1 + 436))), 16);
            w_u16(((uint32)((a1 + 438))), 32);
            sub_8004BDCC(a1, 0, 1u, 5);
            sub_8004BDCC(a1, 1u, 1u, 10);
            sub_8004BDCC(a1, 1u, 3u, 32);
            switch (a2)
            {
                case '0':
                    sub_8005171C(a1, 0, 128, 192, 256, 0);
                    sub_8005171C(a1, 1, 0, 1024, 128, 64);
                    sub_8005171C(a1, 2, 512, 4096, 256, 0);
                    sub_8005171C(a1, 3, 1024, 12288, 128, 96);
                    sub_8005171C(a1, 4, 2048, 12288, 128, 256);
                    sub_8005171C(a1, 5, 1024, 12288, 64, 256);
                    break;

                case '1':
                    sub_8005171C(a1, 0, 128, 192, 256, 0);
                    sub_8005171C(a1, 2, 512, 4096, 256, 0);
                    sub_8005171C(a1, 4, 2048, 12288, 128, 256);
                    break;

                case '2':
                    sub_8005171C(a1, 0, 128, 192, 256, 0);
                    sub_8005171C(a1, 1, 0, 0x2000, 128, 64);
                    sub_8005171C(a1, 4, 4096, 12288, 128, 256);
                    sub_8005171C(a1, 5, 2048, 12288, 64, 256);
                    break;
            }

            w_u8(((uint32)((a1 + 614))), sub_8006E080(808338210, ((unsigned char)(r_u8(0x800FF644u)))));
            v14 = 15;
            goto LABEL_35;

        case 64:
            sub_800626C8(a1, 0x800FF53Cu);
            w_u16(((uint32)((a1 + 218))), 30);
            w_u16(((uint32)((a1 + 456))), 120);
            w_u16(((uint32)((a1 + 390))), 16);
            w_u16(((uint32)((a1 + 458))), 32);
            w_u8(((uint32)((a1 + 613))), 13);
            sub_8004BD04(a1, 2u, 5u);
            w_u16(((uint32)((a1 + 436))), 29);
            w_u16(((uint32)((a1 + 438))), 32);
            sub_8004BDCC(a1, 0, 1u, 5);
            sub_8004BDCC(a1, 1u, 1u, 17);
            sub_8004BDCC(a1, 1u, 3u, 32);
            sub_8005171C(a1, 0, 128, 192, 256, 0);
            sub_8005171C(a1, 1, 0, 1024, 128, 64);
            sub_8005171C(a1, 2, 512, 4096, 256, 0);
            sub_8005171C(a1, 3, 1024, 12288, 128, 96);
        LABEL_28:
            sub_8005171C(a1, 4, 2048, 12288, 128, 256);

            sub_8005171C(a1, 5, 1024, 12288, 64, 256);
            sub_80062D70(a1);
            v15 = ((unsigned char)(r_u8(0x800FF644u)));
            v16 = r_u16(((uint32)((a1 + 456))));
            w_u16(((uint32)((a1 + 204))), 1024);
            w_u16(((uint32)((a1 + 200))), 64);
            w_u16(((uint32)((a1 + 202))), v16);
            w_u8(((uint32)((a1 + 614))), sub_8006E080(1020334405, v15));
            w_u8(((uint32)((a1 + 615))), 16);
            goto LABEL_36;

        case 96:
            sub_800626C8(a1, 0x800FF544u);
            w_u16(((uint32)((a1 + 218))), 100);
            w_u16(((uint32)((a1 + 456))), 116);
            w_u16(((uint32)((a1 + 390))), 64);
            w_u16(((uint32)((a1 + 458))), 64);
            w_u8(((uint32)((a1 + 613))), 10);
            sub_8004BD04(a1, 2u, 5u);
            w_u16(((uint32)((a1 + 436))), 48);
            w_u16(((uint32)((a1 + 438))), 32);
            sub_8004BDCC(a1, 0, 1u, 20);
            sub_8004BDCC(a1, 1u, 1u, 10);
            sub_8004BDCC(a1, 1u, 2u, 64);
            sub_8004BDCC(a1, 1u, 3u, 64);
            sub_8004BDCC(a1, 1u, 4u, 45);
            sub_8005171C(a1, 0, 128, 192, 256, 0);
            sub_8005171C(a1, 1, 0, 512, 128, 16);
            sub_8005171C(a1, 2, 512, 4096, 256, 0);
            sub_8005171C(a1, 3, 1024, 12288, 128, 96);
            sub_8005171C(a1, 4, 3072, 12288, 128, 256);
            sub_8005171C(a1, 5, 2048, 12288, 64, 256);
            sub_80062D70(a1);
            v17 = ((unsigned char)(r_u8(0x800FF644u)));
            v18 = r_u16(((uint32)((a1 + 456))));
            w_u16(((uint32)((a1 + 204))), 1024);
            w_u16(((uint32)((a1 + 200))), 64);
            w_u16(((uint32)((a1 + 202))), v18);
            w_u8(((uint32)((a1 + 614))), sub_8006E080(-87017481, v17));
            v14 = 14;
            goto LABEL_35;

        case 112:
            sub_800626C8(a1, 0x800FF54Cu);
            v19 = r_u32(0x800FF4E4u);
            w_u16(((uint32)((a1 + 390))), 128);
            w_u16(((uint32)((a1 + 218))), 40);
            w_u16(((uint32)((a1 + 456))), 116);
            w_u16(((uint32)((a1 + 458))), 40);
            w_u32(0x800FF4E4u, (uint32)v19 + 1u);
            sub_8004BD04(a1, 2u, 5u);
            w_u16(((uint32)((a1 + 436))), 24);
            w_u16(((uint32)((a1 + 438))), 32);
            sub_8004BDCC(a1, 0, 1u, 5);
            sub_8005171C(a1, 1, 64, 4096, 128, 64);
            sub_8005171C(a1, 4, 0, 64, 256, 8);
            sub_8005171C(a1, 5, 256, 12288, 64, 256);
            sub_80062D70(a1);
            v20 = ((unsigned char)(r_u8(0x800FF644u)));
            v21 = r_u16(((uint32)((a1 + 456))));
            w_u16(((uint32)((a1 + 204))), 1024);
            w_u16(((uint32)((a1 + 200))), 64);
            w_u16(((uint32)((a1 + 202))), v21);
            w_u8(((uint32)((a1 + 614))), sub_8006E080(1020334405, v20));
            v14 = 16;
            goto LABEL_35;

        case 128:

        case 129:
            sub_800626C8(a1, 0x800FF554u);
            w_u16(((uint32)((a1 + 390))), 256);
            w_u16(((uint32)((a1 + 456))), 180);
            w_u16(((uint32)((a1 + 458))), 64);
            w_u8(((uint32)((a1 + 613))), 19);
            sub_8004BD04(a1, 2u, 5u);
            w_u16(((uint32)((a1 + 218))), 30);
            w_u16(((uint32)((a1 + 436))), 32);
            w_u16(((uint32)((a1 + 438))), 32);
            sub_8004BDCC(a1, 0, 1u, 10);
            sub_8004BDCC(a1, 1u, 1u, 10);
            sub_8004BDCC(a1, 1u, 2u, 96);
            sub_8004BDCC(a1, 1u, 3u, 16);
            sub_8004BDCC(a1, 1u, 4u, 45);
            sub_8005171C(a1, 0, 128, 192, 256, 0);
            if ((a2 == 128))
            {
                sub_8005171C(a1, 1, 0, 1024, 128, 64);
                sub_8005171C(a1, 2, 512, 4096, 256, 64);
                sub_8005171C(a1, 3, 1024, 0x2000, 128, 96);
                sub_8005171C(a1, 4, 2048, 0x2000, 64, 128);
                sub_8005171C(a1, 5, 1024, 0x2000, 64, 256);
            }
            else
            {
                sub_8005171C(a1, 2, 512, 4096, 256, 64);
                sub_8005171C(a1, 4, 2048, 0x2000, 64, 128);
            }
            sub_80062D70(a1);
            v22 = ((unsigned char)(r_u8(0x800FF644u)));
            v23 = r_u16(((uint32)((a1 + 456))));
            w_u16(((uint32)((a1 + 204))), 1024);
            w_u16(((uint32)((a1 + 200))), 64);
            w_u16(((uint32)((a1 + 202))), v23);
            w_u8(((uint32)((a1 + 614))), sub_8006E080(873792324, v22));
            v14 = 12;
        LABEL_35:
            w_u8(((uint32)((a1 + 615))), v14);

            goto LABEL_36;

        default:
        LABEL_36:
            v13 = (a1 + 4);

            goto LABEL_37;
    }
}

uint32 sub_8004BDCC(uint32 a1, uint32 a2, uint32 a3, uint32 a4)
{
    sint32 result;
    result = (((2 * a2) * r_u8(((uint32)((a1 + 453))))) + r_u32(((uint32)((a1 + 448)))));
    w_u16(((uint32)(((2 * a3) + result))), a4);
    return result;
}

/* TODO Missing call adapter sub_800878DC */
void apocalypse_collision_objects(uint32 list, void *record)
{
    uint32 length;
    sint32 bounds[6];
    uint16 generation;
    if (!list)
        return;
    memcpy(&length, (uint8 *)record + 68u, 4u);
    if (!length)
        return;
    memcpy(&generation, (uint8 *)record + 138u, 2u);
    SetRotMatrix((MATRIX *)((uint8 *)record + 72u));
    memcpy(bounds, record, sizeof(bounds));
    apocalypse_mark_linked_collision_candidates(list, bounds, generation);
    while (list)
    {
        if (r_u16(list + 2u) != generation)
        {
            w_u16(list + 2u, generation);
            (void)apocalypse_collision_object(list, record);
        }
        list = r_u32(list + 28u);
    }
}

void sub_8007BEA0(uint32 list, uint32 record)
{
    apocalypse_collision_objects(list, psx_addr(record, 140u));
}

uint32 sub_80063118(uint32 a1, uint32 a2, uint32 a3)
{
    sint32 v3;
    sint32 result;
    v3 = r_u8(((uint32)((a1 + 27))));
    w_u8(((uint32)((a1 + 26))), a2);
    v3 = ((v3 & 0xFFFFFF00u) | (((r_u8(((uint32)((((8 * a2) + r_u32((0x800EAEF8u + (((16 * v3) + 6)) * 4u))) + 8))))) & 0xFFu) << 0));
    result = 1;
    w_u8(((uint32)((a1 + 296))), 1);
    w_u8(((uint32)((a1 + 297))), a3);
    w_u8(((uint32)((a1 + 24))), 0);
    w_u16(((uint32)((a1 + 300))), 0);
    w_u8(((uint32)((a1 + 303))), 0);
    w_u8(((uint32)((a1 + 302))), v3);
    return result;
}

uint32 sub_800189EC(void)
{
    uint32 source = r_u32(0x800FF83Cu), destination = 0x800A53D8u;
    uint32 position[4], owner, object, index, result;
    while (r_u8(source))
    {
        uint32 character = r_u8(source);
        ++source;
        w_u8(destination, character);
        ++destination;
    }
    w_u8(destination, 0u);
    source = xport_draft_host_sub_8006613C_p1(position, r_u32(0x800FF620u)) + 6u;
    destination = 0x800A53E1u;
    while (r_u8(source))
    {
        uint32 character = r_u8(source);
        ++source;
        w_u8(destination, character);
        ++destination;
    }
    w_u8(destination, 0u);
    owner = r_u32(0x800FF5A0u);
    if (owner)
    {
        w_u32(0x800A5438u, (uint32)(sint32)(short)r_u16(owner + 218u));
        w_u8(0x800A5416u, 0u);
        w_u32(0x800A5440u, r_u32(0x800FF874u));
        w_u32(0x800A543Cu, r_u32(owner + 532u));
        for (index = 0u; index < 8u; ++index)
        {
            object = r_u32(owner + 612u + index * 4u);
            if (object)
            {
                w_u8(0x800A5416u, r_u8(0x800A5416u) | (1u << index));
                w_u32(0x800A5418u + index * 4u, r_u32(object + 56u));
            }
        }
        w_u8(0x800A5417u, r_u8(owner + 644u));
    }
    sub_80015F7C(0x800A5444u);
    result = sub_80018B44(0x800A53D4u);
    w_u32(0x800A53D4u, result);
    return result;
}

void sub_8001BB68(void)
{
    uint32 i;
    uint32 v1;
    sint32 v2;
    sint32 v3;
    sint32 v4;
    short v5;
    sint32 v6;
    sint32 v7;
    sint32 v8;
    uint32 result;
    for (i = ((uint32)(r_u32(0x800FF1D4u))); i; i = v1)
    {
        v1 = ((uint32)(r_u32((((uint32)(i)) + (5) * 4u))));
        if (r_u8((((uint32)(i)) + (13) * 1u)))
        {
            v2 = r_u16(i);
            v3 = r_u16((i + (4) * 2u));
            v4 = ((r_u8((((uint32)(i)) + (10) * 1u)) | (v2 << 8)) + r_u16((i + (2) * 2u)));
            w_u8((((uint32)(i)) + (10) * 1u), (r_u8((((uint32)(i)) + (10) * 1u)) + (r_u16((i + (2) * 2u)))));
            v5 = ((r_u8((((uint32)(i)) + (11) * 1u)) | (v2 & 0xFF00)) + r_u16((i + (3) * 2u)));
            w_u8((((uint32)(i)) + (11) * 1u), (r_u8((((uint32)(i)) + (11) * 1u)) + (r_u8((((uint32)(i)) + (6) * 1u)))));
            v6 = ((r_u8((((uint32)(i)) + (12) * 1u)) | ((r_u32(((uint32)(i))) >> 8) & 0xFF00)) + v3);
            w_u8((((uint32)(i)) + (12) * 1u), (r_u8((((uint32)(i)) + (12) * 1u)) + (v3)));
            v7 = r_u32((((uint32)(i)) + (4) * 4u));
            v8 = (((((v6 << 16) >> 24) << 16) | (v5 & 0xFF00)) | (((uint32)((v4 << 16))) >> 24));
            w_u32(((uint32)(i)), v8);
            w_u32(((uint32)((v7 + 32))), v8);
            result = ((uint32)((r_u8((((uint32)(i)) + (13) * 1u)) - 1)));
            w_u8((((uint32)(i)) + (13) * 1u), ((uint8)(result)));
        }
        else
        {
            w_u16(r_u32((((uint32)(i)) + (4) * 4u)), (r_u16(r_u32((((uint32)(i)) + (4) * 4u))) & (~0x400u)));
            sub_8001BB14(((sint32)(i)));
        }
    }

    return;
}

void sub_80063738(uint32 a1)
{
    while (a1)
    {
        uint32 next = r_u32(a1 + 28u);
        w_u16(a1 + 76u, r_u16(a1 + 76u) & 1u);
        a1 = next;
    }
}

/* TODO Missing call adapter indirect */
uint32 sub_8006FC84(uint32 object, uint32 distance)
{
    uint32 allocation, table;
    if (!(r_u16(object + 78u) & 4u) || (sint32)(short)r_u16(object + 218u) < 0 || distance < 0x2328u)
        return 0u;
    allocation = sub_8002FED8(24u);
    if (allocation)
        sub_8006FBB0(allocation, r_u16(object + 214u), (uint32)(sint32)(short)r_u16(object + 218u));
    table = r_u32(object + 68u);
    apocalypse_object_virtual20(r_u32(table + 20u), object + (uint32)(sint32)(short)r_u16(table + 16u));
    return 1u;
}

uint32 sub_80063164(uint32 a1)
{
    sint32 v1;
    sint32 v2;
    sint32 v3;
    sint32 v4;
    sint32 result;
    sint32 v6;
    sint32 v7;
    sint32 v8;
    sint8 v9;
    v1 = ((sint8)(r_u8(((uint32)(((sint32)((uint32)(a1) + (uint32)(297))))))));
    v2 = (((sint32)((uint32)(((sint8)(r_u8(((uint32)(((sint32)((uint32)(a1) + (uint32)(24))))))))) << (uint32)(16))) | r_u16(((uint32)(((sint32)((uint32)(a1) + (uint32)(300)))))));
    v3 = r_u32(((uint32)(((sint32)((uint32)(a1) + (uint32)(304))))));
    if ((v1 == 1))
        v2 += v3;
    if ((v1 == -1))
        v2 -= v3;
    v4 = r_u8(((uint32)(((sint32)((uint32)(a1) + (uint32)(296))))));
    result = (v2 >> 16);
    w_u8(((uint32)(((sint32)((uint32)(a1) + (uint32)(24))))), ((v2 >> 16) & 255u));
    w_u16(((uint32)(((sint32)((uint32)(a1) + (uint32)(300))))), v2);
    if (v4)
    {
        if ((v4 == 1))
        {
            if ((((sint8)(r_u8(((uint32)(((sint32)((uint32)(a1) + (uint32)(24)))))))) >= ((sint32)(r_u8(((uint32)(((sint32)((uint32)(a1) + (uint32)(302))))))))))
                w_u8(((uint32)(((sint32)((uint32)(a1) + (uint32)(24))))), 0);
            result = ((sint8)(r_u8(((uint32)(((sint32)((uint32)(a1) + (uint32)(24))))))));
            if ((result < 0))
            {
                result = ((uint32)(r_u8(((uint32)(((sint32)((uint32)(a1) + (uint32)(302))))))) - (uint32)(1));
                w_u8(((uint32)(((sint32)((uint32)(a1) + (uint32)(24))))), result);
            }
        }
    }
    else
    {
        v6 = ((sint8)(r_u8(((uint32)(((sint32)((uint32)(a1) + (uint32)(297))))))));
        if ((((v6 == 1) && (v7 = ((sint8)(r_u8(((uint32)(((sint32)((uint32)(a1) + (uint32)(298)))))))), result = (((sint8)(r_u8(((uint32)(((sint32)((uint32)(a1) + (uint32)(24)))))))) < v7), (((sint8)(r_u8(((uint32)(((sint32)((uint32)(a1) + (uint32)(24)))))))) >= v7))) || ((v6 == -1) && (v8 = ((sint8)(r_u8(((uint32)(((sint32)((uint32)(a1) + (uint32)(24)))))))), result = (((sint8)(r_u8(((uint32)(((sint32)((uint32)(a1) + (uint32)(298)))))))) < v8), (((sint8)(r_u8(((uint32)(((sint32)((uint32)(a1) + (uint32)(298)))))))) >= v8)))))
        {
            v9 = r_u8(((uint32)(((sint32)((uint32)(a1) + (uint32)(298))))));
            result = 1;
            w_u8(((uint32)(((sint32)((uint32)(a1) + (uint32)(303))))), 1);
            w_u8(((uint32)(((sint32)((uint32)(a1) + (uint32)(24))))), v9);
        }
    }
    return result;
}

uint32 sub_8006C0B8(uint32 a1, uint32 a2)
{
    uint32 result;
    result = a1;
    w_u32(a1, (r_u32(a1) + (r_u32(a2))));
    w_u32((a1 + (1) * 4u), (r_u32((a1 + (1) * 4u)) + (r_u32((a2 + (1) * 4u)))));
    w_u32((a1 + (2) * 4u), (r_u32((a1 + (2) * 4u)) + (r_u32((a2 + (2) * 4u)))));
    return result;
}

uint32 sub_8006C270(uint32 a1, uint32 a2)
{
    uint32 result;
    result = a1;
    w_u32(a1, (r_u32(a1) - ((r_u32(a1) >> r_u8(a2)))));
    w_u32((a1 + (1) * 4u), (r_u32((a1 + (1) * 4u)) - ((r_u32((a1 + (1) * 4u)) >> r_u8((a2 + (1) * 1u))))));
    w_u32((a1 + (2) * 4u), (r_u32((a1 + (2) * 4u)) - ((r_u32((a1 + (2) * 4u)) >> r_u8((a2 + (2) * 1u))))));
    return result;
}

uint32 sub_8006C05C(uint32 a1)
{
    sint32 result;
    if ((((uint32)((r_u32(a1) + 2048))) < 0x1001))
        w_u32(a1, 0);
    if ((((uint32)((r_u32((a1 + (1) * 4u)) + 2048))) < 0x1001))
        w_u32((a1 + (1) * 4u), 0);
    result = (((uint32)((r_u32((a1 + (2) * 4u)) + 2048))) < 0x1001);
    if ((((uint32)((r_u32((a1 + (2) * 4u)) + 2048))) < 0x1001))
        w_u32((a1 + (2) * 4u), 0);
    return result;
}

uint32 sub_8005D20C(uint32 a1, uint32 a2, uint32 a3)
{
    unsigned char v3;
    sint32 v4;
    sint32 v5;
    sint32 v6;
    sint32 i;
    sint32 v8;
    uint32 v9;
    sint32 result;
    uint32 v11;
    sint32 v12;
    uint32 v13;
    uint32 v14;
    uint32 v15;
    v4 = 0;
    if (((r_u16(((uint32)(a2))) & 0x800) != 0))
    {
        v5 = r_u32((0x800EAEF8u + (((16 * r_u8(((uint32)((a3 + 27))))) + 8)) * 4u));
        v6 = 3;
        if (((r_u16(((uint32)(a2))) & 0x10) == 0))
            v6 = 4;
        for (i = 0; (i < v6); ++i)
        {
            v8 = r_u8(((uint32)(((i + a2) + 12))));
            v9 = (r_u32(((uint32)(((4 * v8) + v5)))) & 0xFFFFFF);
            if ((((unsigned char)(v4)) < ((uint32)(r_u8(((uint32)(((4 * v8) + v5))))))))
            {
                v4 = (r_u32(((uint32)(((4 * v8) + v5)))) & 0xFFFFFF);
                v3 = r_u8(((uint32)(((i + a2) + 12))));
            }
            if ((((unsigned char)(v4)) < ((uint32)(((v9 >> 8) & 255u)))))
            {
                v4 = (v9 >> 8);
                v3 = r_u8(((uint32)(((i + a2) + 12))));
            }
            if ((((unsigned char)(v4)) < ((v9 >> 16) & 65535u)))
            {
                v4 = ((v9 >> 16) & 65535u);
                v3 = r_u8(((uint32)(((i + a2) + 12))));
            }
        }

        result = (r_u32(((uint32)(((4 * v3) + v5)))) + (v4 << 24));
        w_u32(((uint32)((a1 + 32))), result);
    }
    else
    {
        v11 = (r_u32(((uint32)((a2 + 12)))) & 0xFFFFFF);
        v12 = ((r_u16(((uint32)(a2))) & 0x800u) >= r_u8(((uint32)((a2 + 12)))));
        v13 = 0;
        if (!v12)
            v13 = v11;
        v14 = ((v11 >> 16) & 65535u);
        if ((((unsigned char)(v13)) < ((uint32)(((v11 >> 8) & 255u)))))
            v13 = (v11 >> 8);
        v15 = (v13 << 24);
        if ((((unsigned char)(v13)) < v14))
            v15 = (v14 << 24);
        result = (v11 + v15);
        w_u32(((uint32)((a1 + 32))), result);
    }
    return result;
}

uint32 sub_8006C624(uint32 a1)
{
    short v1;
    short v2;
    sint32 result;
    v1 = r_u16((a1 + (2) * 2u));
    w_u16(a1, (r_u16(a1) & (0xFFFu)));
    v2 = r_u16((a1 + (1) * 2u));
    w_u16((a1 + (2) * 2u), (v1 & 0xFFF));
    result = (v2 & 0xFFF);
    w_u16((a1 + (1) * 2u), result);
    return result;
}

uint32 sub_8005EA1C(uint32 a1)
{
    sint32 result;
    sint32 v2;
    result = 0;
    if ((r_u8(((uint32)((a1 + 468)))) < 0))
    {
        result = 0;
        if (((((uint32)(r_u16(((uint32)((a1 + 474)))))) - 768) >= 0xA01))
        {
            v2 = r_u8(((uint32)((a1 + 26))));
            w_u32(((uint32)((a1 + 460))), 16);
            result = 1;
            if ((v2 != 1))
            {
                sub_80063118(a1, 1, 1);
                return 1;
            }
        }
    }
    return result;
}

uint32 sub_8005EAF0(uint32 a1)
{
    sint32 result;
    sint32 v2;
    sint32 v3;
    result = 0;
    if ((r_u8(((uint32)((a1 + 469)))) < 0))
    {
        v2 = r_u16(((uint32)((a1 + 474))));
        result = 0;
        if ((v2 >= 2816))
        {
            if ((v2 >= 3329))
            {
                return 0;
            }
            else
            {
                v3 = r_u8(((uint32)((a1 + 26))));
                w_u32(((uint32)((a1 + 460))), 512);
                result = 1;
                if ((v3 != 1))
                {
                    sub_80063118(a1, 1, 1);
                    return 1;
                }
            }
        }
    }
    return result;
}

/* TODO Postincrement memory expressions may require ordering refinement */
uint32 sub_8005F00C(uint32 a1)
{
    sint32 result;
    sint32 v3;
    sint32 v4;
    sint32 v5;
    sint32 v6;
    sint32 v7;
    sint32 v8;
    result = 0;
    if (r_u8(((uint32)((r_u32(((uint32)((a1 + 444)))) + 273)))))
    {
        result = 0;
        if (!(r_u8(((uint32)((a1 + 569))))))
        {
            sub_80069DF0(5, 0x2000, 0);
            if (((r_u16(((uint32)(a1))) & 8) != 0))
                sub_80069DF0(63, 0x2000, 0);
            (w_u8(((uint32)((a1 + 569))), (r_u8(((uint32)((a1 + 569)))) + 1u)), (r_u8(((uint32)((a1 + 569)))) + 1u));
            v3 = r_u32(((uint32)((a1 + 428))));
            v4 = (-4096 * r_u16(0x800EC4DAu));
            w_u32(((uint32)((a1 + 564))), v4);
            if (v3)
            {
                v5 = r_u32(((uint32)((v3 + 108))));
                if ((v5 >= -262144))
                    v6 = (v4 + v5);
                else
                    v6 = (v4 + (2 * v5));
                w_u32(((uint32)((a1 + 564))), v6);
            }
            w_u32(((uint32)((a1 + 460))), 2);
            sub_80063038(a1, 2, (r_u16(0x800EC4B6u) + 1), r_u16(0x800EC4D4u));
            v7 = (r_u16(((uint32)((a1 + 216)))) & 1);
            w_u8(((uint32)((a1 + 568))), r_u8(0x800EC4D8u));
            w_u32(0x800FF5A8u, 0);
            v8 = (v7 == 0);
            result = 1;
            if (!v8)
                w_u32(((uint32)((a1 + 608))), 0);
        }
    }
    return result;
}

uint32 sub_8006C564(uint32 a1, uint32 a2, uint32 a3)
{
    uint32 components[3];
    components[0] = (uint32)((sint32)r_u32(a2) >> (r_u32(a3) & 31u));
    components[1] = (uint32)((sint32)r_u32(a2 + 4u) >> (r_u32(a3) & 31u));
    components[2] = (uint32)((sint32)r_u32(a2 + 8u) >> (r_u32(a3) & 31u));
    w_u32(a1, components[0]);
    w_u32(a1 + 4u, components[1]);
    w_u32(a1 + 8u, components[2]);
    return a1;
}

uint32 sub_8006331C(uint32 a1, uint32 a2)
{
    sint32 result;
    if (!(r_u32(((uint32)((a1 + 360))))))
    {
        sub_8007CAC8(a1, a2);
        w_u32(((uint32)((a1 + 368))), a2);
    }
    sub_8007CECC(a1);
    result = (r_u16(((uint32)(a1))) & 4);
    if (result)
    {
        sub_8007FDF0(a1);
        return sub_8007D148(a1);
    }
    return result;
}

uint32 sub_8007CECC(uint32 a1)
{
    uint32 v1;
    sint32 v2;
    sint32 result;
    v1 = (0x800EAEF8u + ((16 * r_u8(((uint32)((a1 + 27)))))) * 4u);
    v2 = r_u32((v1 + (6) * 4u));
    result = r_u16(((uint32)((((8 * r_u8(((uint32)((a1 + 26))))) + v2) + 10))));
    if (r_u16(((uint32)((((8 * r_u8(((uint32)((a1 + 26))))) + v2) + 10)))))
        return sub_8007CF38((4 * r_u32(((uint32)((r_u32((v1 + (4) * 4u)) - 4))))), (result + 1), v2, a1, 0, r_u32(r_u32(v1 + 16u) - 4u));
    return result;
}

/* TODO Missing call adapter indirect */
/* TODO Missing call adapter sub_80024428 */
/* TODO Missing call adapter sub_80025070 */
/* TODO Missing call adapter sub_800252CC */
/* TODO Missing call adapter sub_80028578 */
/* TODO Missing call adapter sub_8002A5C0 */
/* TODO Missing call adapter sub_8002B4EC */
/* Missing weapon services keep their native argument contract explicit */
static uint32 player_action_missing(const char *operation)
{
    fprintf(stderr, "TODO 8005D360: %s\n", operation);
    abort();
}

static uint32 player_action_virtual(uint32 target, uint32 receiver, const sint32 position[3], const sint16 rotation[3])
{
    if (target == 0x80022C90u)
        return apocalypse_weapon_fire_22C90(receiver, position, rotation);
    if (target == 0x80023380u)
    {
        /* The observed tick uses position and ignores the rotation carrier */
        return apocalypse_weapon_tick_23380(receiver, position);
    }
    fprintf(stderr, "TODO 8005D360 weapon target %08X receiver %08X position %p rotation %p\n", target, receiver, (const void *)position, (const void *)rotation);
    abort();
}

uint32 sub_8005D360(uint32 object, uint32 rotation, uint32 enabled)
{
    return apocalypse_player_action(object, (const sint16 *)psx_addr(rotation, 6u), enabled);
}

uint32 apocalypse_player_action(uint32 a1, const sint16 *a2, uint32 a3)
{
    sint32 result;
    sint32 v7;
    sint32 v8;
    sint32 v9;
    sint32 v10;
    sint32 v11;
    sint32 v12;
    uint32 v13;
    sint32 v14;
    uint32 v15;
    sint32 v16;
    uint32 v17;
    sint32 v18;
    uint32 v19;
    sint32 v20;
    uint32 v21;
    sint32 v22;
    uint32 v23;
    sint32 v24;
    uint32 v25;
    sint32 v26;
    sint32 v27;
    sint32 v28;
    sint32 v29;
    sint32 v30;
    sint32 v31;
    sint8 v32;
    short v33;
    sint32 v34;
    sint32 v35;
    sint32 v36;
    const sint16 *v37;
    sint32 v38;
    sint32 v39;
    sint32 v40;
    sint32 v41;
    sint32 v42;
    sint32 v43;
    uint32 v44;
    sint32 v45;
    sint32 v46;
    sint32 v47;
    sint32 v48;
    sint32 v49;
    sint32 v50;
    sint32 v51;
    sint32 v52;
    sint32 v53;
    int v54[4];
    uint32 angles[2];
    result = r_u32(((uint32)((a1 + 360))));
    if (!result)
        return result;
    v7 = 0;
    if (((((r_u8(((uint32)((r_u32(((uint32)((a1 + 444)))) + 288)))) && (r_u32(((uint32)((a1 + 460)))) != 0x40000)) && !r_u32(0x800FF218u)) && !(r_u32(((uint32)((a1 + 452)))))) && r_u32(((uint32)((a1 + 532))))))
    {
        v8 = sub_80032DC0(172);
        if (v8)
            sub_80029D24(v8, 100, 10, 0x2000);
        if ((r_u32(0x800FF2FCu) == 2))
            sub_8007011C(0, 12, 1, 120);
        if (!r_u32(0x800FF318u))
        {
            v9 = r_u32(((uint32)((a1 + 532))));
            v10 = (v9 == 0);
            v11 = (v9 - 1);
            if (!v10)
                w_u32(((uint32)((a1 + 532))), v11);
        }
        sub_8002FC64(0, 2, (a1 + 4), 1);
    }
    v12 = r_u32(((uint32)((a1 + 444))));
    if (r_u8(((uint32)((v12 + 305)))))
    {
        w_u8(((uint32)((v12 + 305))), 0);
        if (r_u32(0x800FF318u))
        {
            sub_80069DF0(26, 0x2000, 0);
            switch (r_u32(((uint32)((r_u32(((uint32)(((a1 + (4 * r_u32(((uint32)((a1 + 644)))))) + 612)))) + 4)))))
            {
                case 1:
                    if (!(r_u32(((uint32)((a1 + 620))))))
                    {
                        v15 = ((uint32)(sub_8002FED8(68)));
                        if (v15)
                            v15 = player_action_missing("800252CC weapon constructor");
                        v16 = 10;
                        v10 = (r_u32(0x800FF384u) != 0);
                        w_u32(((uint32)((a1 + 620))), v15);
                        if (!v10)
                            v16 = 15;
                        w_u32((v15 + (10) * 4u), v16);
                    }
                    w_u32(((uint32)((a1 + 648))), 1);
                    w_u32(((uint32)((a1 + 644))), 2);
                    break;

                case 2:
                    if (!(r_u32(((uint32)((a1 + 624))))))
                    {
                        v17 = ((uint32)(sub_8002FED8(104)));
                        if (v17)
                            v17 = sub_80027720(v17, 0x800FF4E8u, 3000u, 0u, 256u, 9u);
                        v18 = 10;
                        v10 = (r_u32(0x800FF384u) != 0);
                        w_u32(((uint32)((a1 + 624))), v17);
                        if (!v10)
                            v18 = 15;
                        w_u32((v17 + (10) * 4u), v18);
                    }
                    w_u32(((uint32)((a1 + 648))), 2);
                    w_u32(((uint32)((a1 + 644))), 3);
                    break;

                case 3:
                    if (!(r_u32(((uint32)((a1 + 628))))))
                    {
                        v19 = ((uint32)(sub_8002FED8(100)));
                        if (v19)
                            v19 = player_action_missing("80028578 weapon constructor");
                        v20 = 2;
                        v10 = (r_u32(0x800FF384u) != 0);
                        w_u32(((uint32)((a1 + 628))), v19);
                        if (!v10)
                            v20 = 3;
                        w_u32((v19 + (10) * 4u), v20);
                    }
                    w_u32(((uint32)((a1 + 648))), 3);
                    w_u32(((uint32)((a1 + 644))), 4);
                    break;

                case 4:
                    if (!(r_u32(((uint32)((a1 + 632))))))
                    {
                        v21 = ((uint32)(sub_8002FED8(68)));
                        if (v21)
                            v21 = player_action_missing("8002A5C0 weapon constructor");
                        v22 = 120;
                        v10 = (r_u32(0x800FF384u) != 0);
                        w_u32(((uint32)((a1 + 632))), v21);
                        if (!v10)
                            v22 = 180;
                        w_u32((v21 + (10) * 4u), v22);
                    }
                    w_u32(((uint32)((a1 + 648))), 4);
                    w_u32(((uint32)((a1 + 644))), 5);
                    break;

                case 5:
                    if (!(r_u32(((uint32)((a1 + 636))))))
                    {
                        v23 = ((uint32)(sub_8002FED8(68)));
                        if (v23)
                            v23 = player_action_missing("80025070 weapon constructor");
                        v24 = 15;
                        v10 = (r_u32(0x800FF384u) != 0);
                        w_u32(((uint32)((a1 + 636))), v23);
                        if (!v10)
                            v24 = 22;
                        w_u32((v23 + (10) * 4u), v24);
                    }
                    w_u32(((uint32)((a1 + 648))), 5);
                    w_u32(((uint32)((a1 + 644))), 6);
                    break;

                case 6:
                    if (!(r_u32(((uint32)((a1 + 640))))))
                    {
                        v25 = ((uint32)(sub_8002FED8(68)));
                        if (v25)
                            v25 = player_action_missing("8002B4EC weapon constructor");
                        v26 = 40;
                        v10 = (r_u32(0x800FF384u) != 0);
                        w_u32(((uint32)((a1 + 640))), v25);
                        if (!v10)
                            v26 = 60;
                        w_u32((v25 + (10) * 4u), v26);
                    }
                    w_u32(((uint32)((a1 + 648))), 6);
                    w_u32(((uint32)((a1 + 644))), 7);
                    break;

                case 7:
                    w_u32(((uint32)((a1 + 648))), 7);
                    w_u32(((uint32)((a1 + 644))), 0);
                    break;

                case 8:
                    if (!(r_u32(((uint32)((a1 + 616))))))
                    {
                        v13 = ((uint32)(sub_8002FED8(72)));
                        if (v13)
                            v13 = player_action_missing("80024428 weapon constructor");
                        v14 = 10;
                        v10 = (r_u32(0x800FF384u) != 0);
                        w_u32(((uint32)((a1 + 616))), v13);
                        if (!v10)
                            v14 = 15;
                        w_u32((v13 + (10) * 4u), v14);
                    }
                    w_u32(((uint32)((a1 + 648))), 0);
                    w_u32(((uint32)((a1 + 644))), 1);
                    break;

                default:
                    break;
            }
        }
        else
        {
            v28 = r_u32(((uint32)((a1 + 644))));
            v29 = (v28 + 1);
            if (((v28 + 1) < 8))
            {
                v30 = ((4 * (v28 + 1)) + a1);
                while (!(r_u32(((uint32)((v30 + 612))))))
                {
                    ++v29;
                    v30 += 4;
                    if ((v29 >= 8))
                        goto LABEL_73;
                }

                sub_80069DF0(26, 0x2000, 0);
                v27 = r_u32(((uint32)((a1 + 644))));
                w_u32(((uint32)((a1 + 644))), v29);
                w_u32(((uint32)((a1 + 648))), v27);
            }
        LABEL_73:
            if (((v28 == r_u32(((uint32)((a1 + 644))))) && v28))
            {
                sub_80069DF0(26, 0x2000, 0);
                v31 = r_u32(((uint32)((a1 + 644))));
                w_u32(((uint32)((a1 + 644))), 0);
                w_u32(((uint32)((a1 + 648))), v31);
            }
        }
    }
    xport_draft_host_sub_8007CC10_p1(v54, a1, 0u);
    if (((((r_u32(((uint32)((a1 + 460)))) & 0xF3800) != 0) || !((r_u8(((uint32)((a1 + 471)))) | r_u8(((uint32)((a1 + 472))))))) || !a3))
    {
        v47 = r_u32(((uint32)(((a1 + (4 * r_u32(((uint32)((a1 + 644)))))) + 612))));
        if (v47)
            player_action_virtual(r_u32(r_u32(v47) + 28u), v47 + (uint32)(sint32)(sint16)r_u16(r_u32(v47) + 24u), v54, a2);
        w_u8(((uint32)((a1 + 536))), 0);
        xport_draft_host_sub_8006C0B8_p1(v54, a1 + 104u);
        goto LABEL_126;
    }
    v32 = r_u8(((uint32)((a1 + 536))));
    w_u16(((uint32)((a1 + 580))), 0);
    w_u8(((uint32)((a1 + 536))), (v32 + 1));
    v33 = a2[2];
    memcpy(angles, a2, 4u);
    angles[1] = v33;
    if (!(r_u32(((uint32)(((a1 + (4 * r_u32(((uint32)((a1 + 644)))))) + 612))))))
        goto LABEL_122;
    v34 = v54[1];
    v35 = v54[2];
    w_u32(((uint32)((a1 + 416))), v54[0]);
    w_u32(((uint32)((a1 + 420))), v34);
    w_u32(((uint32)((a1 + 424))), v35);
    v36 = r_u32(((uint32)((r_u32(((uint32)(((a1 + (4 * r_u32(((uint32)((a1 + 644)))))) + 612)))) + 4))));
    if ((v36 == 5))
        goto LABEL_86;
    if ((v36 < 6))
    {
        if ((v36 != 2))
            goto LABEL_93;
    LABEL_86:
        if (((r_u8(0x800EC0F8u) || r_u8(0x800EC128u)) && (r_u8(0x800EC108u) || r_u8(0x800EC118u))))
            v37 = a2;
        else
            v37 = a2;

        apocalypse_select_aim_target((sint16 *)angles, (const uint32 *)v54, v37, 4096u,
            ((r_u8(0x800EC0F8u) || r_u8(0x800EC128u)) && (r_u8(0x800EC108u) || r_u8(0x800EC118u))) ? 1536u : 768u,
            3072u, 0u, 0u, r_u32(0x800FF4E8u));
        goto LABEL_93;
    }
    if (((v36 < 9) && (v36 >= 7)))
        goto LABEL_86;
LABEL_93:
    v38 = 0;

    if ((r_u8(((uint32)((a1 + 536)))) < 2u))
    {
        v40 = r_u32(((uint32)(((a1 + (4 * r_u32(((uint32)((a1 + 644)))))) + 612))));
            player_action_virtual(r_u32(r_u32(v40) + 28u), v40 + (uint32)(sint32)(sint16)r_u16(r_u32(v40) + 24u), v54, (const sint16 *)angles);
    }
    else
    {
        v39 = r_u32(((uint32)((a1 + 644))));
        w_u8(((uint32)((a1 + 536))), 1);
        { uint32 weapon = r_u32(a1 + 612u + 4u * v39); uint32 table = r_u32(weapon); v38 = player_action_virtual(r_u32(table + 20u), weapon + (uint32)(sint32)(sint16)r_u16(table + 16u), v54, (const sint16 *)angles); }
    }
    if (r_u32(0x800FF2FCu))
    {
        switch (v36)
        {
            case 1:
                if (((r_u32(0x800FF2FCu) == 2) && !(r_u8(((uint32)((a1 + 465)))))))
                {
                    v41 = 4;
                    v42 = 1;
                    v43 = 60;
                    goto LABEL_116;
                }
                break;

            case 2:
                if (((v38 && (r_u32(0x800FF2FCu) == 2)) && !(r_u8(((uint32)((a1 + 465)))))))
                {
                    v41 = 2;
                    goto LABEL_115;
                }
                break;

            case 3:

            case 4:
                if (((r_u32(0x800FF2FCu) == 2) && !(r_u8(((uint32)((a1 + 465)))))))
                {
                    v41 = 2;
                    goto LABEL_115;
                }
                break;

            case 5:

            case 6:

            case 7:
                if (((v38 && (r_u32(0x800FF2FCu) == 2)) && !(r_u8(((uint32)((a1 + 465)))))))
                {
                    v41 = 4;
                LABEL_115:
                    v42 = 0;

                    v43 = 1;
                    goto LABEL_116;
                }
                break;

            case 8:
                if (((r_u32(0x800FF2FCu) == 2) && !(r_u8(((uint32)((a1 + 465)))))))
                {
                    v41 = 4;
                    v42 = 1;
                    v43 = 120;
                LABEL_116:
                    sub_8007011C(0, v41, v42, v43);
                }
                break;

            default:
                break;
        }
    }
    v44 = r_u32(((uint32)(((a1 + (4 * r_u32(((uint32)((a1 + 644)))))) + 612))));
    v7 = 1;
    if ((r_u32((v44 + (1) * 4u)) == 8))
        goto LABEL_126;
    v45 = 0;
    if ((((sint32)(r_u32((v44 + (14) * 4u)))) > 0))
        goto LABEL_127;
    if (v44)
        player_action_missing("Weapon class destructor slot12");
    w_u32(((uint32)(((a1 + (4 * r_u32(((uint32)((a1 + 644)))))) + 612))), 0);
    v46 = r_u32(((uint32)((a1 + 644))));
    w_u32(((uint32)((a1 + 644))), 0);
    w_u32(((uint32)((a1 + 648))), v46);
LABEL_122:
    v7 = 1;

LABEL_126:
    v45 = 0;

LABEL_127:
    v48 = a1;

    do
    {
        if ((v45 != r_u32(((uint32)((a1 + 644))))))
        {
            v49 = r_u32(((uint32)((v48 + 612))));
            if (v49)
            player_action_virtual(r_u32(r_u32(v49) + 28u), v49 + (uint32)(sint32)(sint16)r_u16(r_u32(v49) + 24u), v54, a2);
        }
        ++v45;
        v48 += 4;
    } while ((v45 < 8));
    result = -2146500608;
    if (v7)
    {
        result = r_u16(0x800EC666u);
        w_u32(0x800FF5C4u, r_u16(0x800EC666u));
    }
    return result;
}

void sub_800372B8(void)
{
    sub_80037258(r_u32(0x800FF440u));
    sub_80037258(r_u32(0x800FF438u));
    sub_80037258(r_u32(0x800FF454u));
    sub_80037258(r_u32(0x800FF448u));
    sub_80037258(r_u32(0x800FF44Cu));
    sub_80037258(r_u32(0x800FF444u));
    sub_80037258(r_u32(0x800FF450u));
    sub_80037258(r_u32(0x800FF434u));
    sub_80037258(r_u32(0x800FF43Cu));
    sub_80037258(r_u32(0x800FF45Cu));
    sub_80037258(r_u32(0x800FF460u));
    sub_80037258(r_u32(0x800FF464u));
    { (void)(sub_80037258(r_u32(0x800FF458u))); return; }
}


void sub_80084778(uint32 bounds, sint32 low[3], sint32 high[3], uint32 *flip_bits)
{
  uint32 axis;
  for (axis=0u;axis<3u;++axis) {
    low[axis]=(sint32)r_u32(bounds+axis*4u)>>12;
    high[axis]=(sint32)r_u32(bounds+12u+axis*4u)>>12;
  }
  *flip_bits=0u;
  sub_800847AC(low,high,flip_bits);
}


