#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
#include <stdlib.h>
#include <stdio.h>

uint32 sub_80061F54(uint32 a1)
{
    sint32 result;
    sint32 v3;
    sint32 v4;
    sint32 v5;
    if (((r_u16(((uint32)((a1 + 76)))) & 1) != 0))
    {
        sub_80062034(a1);
        sub_80062044(a1);
        w_u16(((uint32)((a1 + 76))), (r_u16(((uint32)((a1 + 76)))) & (~1u)));
    }
    result = r_u32(((uint32)((a1 + 320))));
    if (result)
    {
        v3 = (r_u32(((uint32)((a1 + 324)))) + 1);
        w_u32(((uint32)((a1 + 324))), v3);
        result = (v3 < 129);
        if (!result)
        {
            w_u32(((uint32)((a1 + 320))), 0);
            v4 = r_u32(((uint32)((a1 + 300))));
            v5 = r_u32(((uint32)((a1 + 304))));
            w_u32(((uint32)((a1 + 4))), r_u32(((uint32)((a1 + 296)))));
            w_u32(((uint32)((a1 + 8))), v4);
            w_u32(((uint32)((a1 + 12))), v5);
            result = sub_8006E080(r_u32(((uint32)((a1 + 332)))), r_u8(((uint32)((a1 + 27)))));
            w_u16(((uint32)((a1 + 22))), result);
        }
    }
    else if (r_u32(0x800FF5A0u))
    {
        result = (sub_8006696C((r_u32(0x800FF5A0u) + 4), (a1 + 308)) < 192);
        if (result)
        {
            result = r_u32(0x800FF5A0u);
            w_u32(((uint32)((r_u32(0x800FF5A0u) + 520))), a1);
        }
    }
    return result;
}

static void voices_6A294_stop(sint16 voice)
{
    uint32 index = (uint16)voice, low, high, offset;
    if (r_u32(0x80104584u) == 1u) return;
    w_u32(0x80104584u, 1u);
    if (index < 24u)
    {
        w_u16(0x80105638u, index);
        offset = index * 54u;
        low = index < 16u ? 1u << index : 0u;
        high = index >= 16u ? 1u << (index - 16u) : 0u;
        w_u8(0x80104E45u + offset, 0u);
        low |= r_u16(0x80105520u);
        high |= r_u16(0x80105522u);
        w_u16(0x80104E2Cu + offset, 0u);
        w_u16(0x80104E28u + offset, 0u);
        w_u16(0x80105520u, low);
        w_u16(0x80104E18u, r_u16(0x80104E18u) & ~low);
        w_u16(0x80105522u, high);
        w_u16(0x80104E1Au, r_u16(0x80104E1Au) & ~high);
    }
    w_u32(0x80104584u, 0u);
}

void sub_8006A294(uint32 mask)
{
    for (uint32 voice = 0; voice < 24u; ++voice)
        if (mask & (1u << voice))
            voices_6A294_stop((sint16)voice);
}

uint32 sub_80062034(uint32 a1)
{
    sint32 result;
    result = 1;
    w_u32(((uint32)((a1 + 320))), 1);
    w_u32(((uint32)((a1 + 324))), 0);
    return result;
}

/* TODO Missing call adapter sub_80078178 */
/* TODO Missing call adapter sub_80087A3C */
uint32 sub_8005F300(uint32 object)
{
    sint32 scaled[3], position[3], offset[3];
    uint32 scale = 64, offset_scale = 128;
    if ((r_u16(object + 216) & 0x40u) == 0)
        return 0;
    sub_80063038(object, 10, 1, 0xFFFFFFFFu);
    w_u32(object + 460, 4096);
    if ((r_u16(object + 216) & 0x80u) == 0)
    {
        w_u32(object + 8, r_u32(object + 544) + 0x80000u);
        return 1;
    }
    xport_draft_host_sub_8006C40C_p13(scaled, object + 552, &scale);
    xport_draft_host_sub_8006C34C_p13(position, object + 540, scaled);
    w_u32(object + 4, position[0]);
    w_u32(object + 8, position[1]);
    w_u32(object + 12, position[2]);
    w_u32(object + 8, r_u32(object + 8) + 0x80000u);
    w_u16(object + 18, (1024 - ratan2((sint32)r_u32(object + 560), (sint32)r_u32(object + 552))) & 0xFFFu);
    xport_draft_host_sub_8006C47C_p12(offset, &offset_scale, object + 552);
    xport_draft_host_sub_8006C3AC_p13(position, object + 4, offset);
    position[1] = (sint32)((uint32)position[1] - 884736u);
    if (!r_u32(object + 428))
        xport_draft_host_sub_80078178_p2(r_u32(0x800FF904u), position, 36);
    return 1;
}

uint32 sub_8003304C(uint32 a1)
{
    sub_80032F7C(((sint32)(a1)));
    w_u32((a1 + (17) * 4u), 0x800A1D98u);
    sub_80032E50(a1, 0x800FF458u);
    return a1;
}

/* TODO 64-bit guest field width remains TODO */
/* TODO Missing call adapter HIDWORD */
/* TODO Missing call adapter LODWORD */
/* TODO 64-bit guest field width remains TODO */
uint32 sub_800293D8(uint32 a1, uint32 a2, uint32 a3, uint32 a4)
{
    long long v4;
    sint32 v5;
    if ((((sint32)(a2)) < 0))
        (a2 = ((a2 & 0xFFFFFF00u) | (((0) & 0xFFu) << 0)));
    if ((((sint32)(a3)) < 0))
        (a3 = ((a3 & 0xFFFFFF00u) | (((0) & 0xFFu) << 0)));
    if ((((sint32)(a4)) < 0))
        (a4 = ((a4 & 0xFFFFFF00u) | (((0) & 0xFFu) << 0)));
    (v4 = ((unsigned long long)r_u32(((uint32)((a1 + 76)))) | ((unsigned long long)r_u32((((uint32)((a1 + 76)))) + 4u) << 32)));
    (v5 = 0);
    if ((((sint32)(v4)) > 0))
    {
        (v4 = (((unsigned long long)(v4) & 0xFFFFFFFFULL) | (((unsigned long long)(((((unsigned long long)(v4) >> 32) & 0xFFFFFFFFu) + 92)) & 0xFFFFFFFFULL) << 32)));
        do
        {
            w_u16(((uint32)(((((unsigned long long)(v4) >> 32) & 0xFFFFFFFFu) - 87))), 0);
            w_u8(((uint32)(((((unsigned long long)(v4) >> 32) & 0xFFFFFFFFu) - 88))), 0);
            w_u16(((uint32)(((((unsigned long long)(v4) >> 32) & 0xFFFFFFFFu) - 75))), 0);
            w_u8(((uint32)(((((unsigned long long)(v4) >> 32) & 0xFFFFFFFFu) - 76))), 0);
            w_u8(((uint32)(((((unsigned long long)(v4) >> 32) & 0xFFFFFFFFu) - 64))), a2);
            w_u8(((uint32)(((((unsigned long long)(v4) >> 32) & 0xFFFFFFFFu) - 52))), a2);
            w_u8(((uint32)(((((unsigned long long)(v4) >> 32) & 0xFFFFFFFFu) - 63))), a3);
            w_u8(((uint32)(((((unsigned long long)(v4) >> 32) & 0xFFFFFFFFu) - 51))), a3);
            w_u8(((uint32)(((((unsigned long long)(v4) >> 32) & 0xFFFFFFFFu) - 62))), a4);
            w_u8(((uint32)(((((unsigned long long)(v4) >> 32) & 0xFFFFFFFFu) - 50))), a4);
            w_u8(((uint32)(((((unsigned long long)(v4) >> 32) & 0xFFFFFFFFu) - 36))), a2);
            w_u8(((uint32)(((((unsigned long long)(v4) >> 32) & 0xFFFFFFFFu) - 24))), a2);
            w_u8(((uint32)(((((unsigned long long)(v4) >> 32) & 0xFFFFFFFFu) - 35))), a3);
            w_u8(((uint32)(((((unsigned long long)(v4) >> 32) & 0xFFFFFFFFu) - 23))), a3);
            w_u8(((uint32)(((((unsigned long long)(v4) >> 32) & 0xFFFFFFFFu) - 34))), a4);
            w_u8(((uint32)(((((unsigned long long)(v4) >> 32) & 0xFFFFFFFFu) - 22))), a4);
            w_u16(((uint32)(((((unsigned long long)(v4) >> 32) & 0xFFFFFFFFu) - 11))), 0);
            w_u8(((uint32)(((((unsigned long long)(v4) >> 32) & 0xFFFFFFFFu) - 12))), 0);
            w_u16(((uint32)(((((unsigned long long)(v4) >> 32) & 0xFFFFFFFFu) + 1))), 0);
            w_u8(((uint32)((((unsigned long long)(v4) >> 32) & 0xFFFFFFFFu))), 0);
            (v4 = (((unsigned long long)(v4) & 0xFFFFFFFF00000000ULL) | (((unsigned long long)((++v5 < r_u32(((uint32)((a1 + 76)))))) & 0xFFFFFFFFULL) << 0)));
            (v4 = (((unsigned long long)(v4) & 0xFFFFFFFFULL) | (((unsigned long long)(((((unsigned long long)(v4) >> 32) & 0xFFFFFFFFu) + 140)) & 0xFFFFFFFFULL) << 32)));
        } while ((v5 < r_u32(((uint32)((a1 + 76))))));
    }
    return v4;
}

/* TODO Missing call adapter SLOWORD */
/* TODO Missing call adapter indirect */
/* Preserve signed DIV edge results without a guest frame */
static sint32 cluster_027_div(sint32 numerator, sint32 denominator)
{
    if (!denominator) return numerator < 0 ? 1 : -1;
    if ((uint32)numerator == 0x80000000u && denominator == -1) return numerator;
    return numerator / denominator;
}

void sub_8002A138(uint32 a1)
{
    sint32 v2;
    sint32 v3;
    sint32 v4;
    sint32 v5;
    sint32 v6;
    sint32 v7;
    sint32 v8;
    uint32 v9;
    sint32 v10;
    sint32 v11;
    uint32 v12;
    sint32 v13;
    short v14;
    sint32 v15;
    sint32 i;
    sint32 v17;
    sint32 j;
    sint32 v19;
    sint32 k;
    sint32 v21;
    sint32 v22;
    sint32 v23;
    sint32 v24;
    sint32 v25;
    sint32 v26;
    sint32 v27;
    sint32 v28;
    sint32 vars0;
    sint32 vars4;
    sint32 vars8;
    v2 = r_u32((a1 + (42) * 4u));
    v3 = r_u32((a1 + (19) * 4u));
    w_u32((a1 + (42) * 4u), (v2 + 130));
    v4 = (v2 + v3);
    if (((v2 + 130) >= 2001))
        w_u32((a1 + (19) * 4u), (v3 + 100));
    sub_80029198(r_u32((a1 + (18) * 4u)), (a1 + (6) * 4u), r_u32((a1 + (19) * 4u)), r_u32((a1 + (42) * 4u)));
    sub_80029324(((uint32)(r_u32((a1 + (18) * 4u)))), r_u32(((uint32)((r_u32((a1 + (18) * 4u)) + 100)))), (r_u32(((uint32)((r_u32((a1 + (18) * 4u)) + 104)))) - 1), 2);
    v5 = r_u32((a1 + (18) * 4u));
    v6 = r_u32((a1 + (22) * 4u));
    w_u32((a1 + (20) * 4u), (r_u32((a1 + (20) * 4u)) - (3)));
    v7 = r_u32((a1 + (21) * 4u));
    v8 = r_u32((a1 + (20) * 4u));
    v6 -= 3;
    w_u32((a1 + (22) * 4u), v6);
    v7 -= 3;
    w_u32((a1 + (21) * 4u), v7);
    sub_800293D8(v5, v8, v7, v6);
    if ((((((sint32)(r_u32((a1 + (20) * 4u)))) <= 0) && (((sint32)(r_u32((a1 + (21) * 4u)))) <= 0)) && (((sint32)(r_u32((a1 + (22) * 4u)))) <= 0)))
        sub_80032ED8(a1);
    v9 = a1;
    if ((((sint32)(r_u32((a1 + (42) * 4u)))) >= 2001))
    {
        v10 = r_u32((a1 + (18) * 4u));
        v11 = 0;
        if ((((sint32)(r_u32(((uint32)((v10 + 76)))))) > 0))
        {
            v12 = a1;
            v13 = 0;
            v14 = (400 * r_u32(0x800FF2F0u));
            do
            {
                v15 = 200 * (sint32)(sint16)r_u16(0x800F863Cu + (((uint32)v14 + r_u32(v12 + 104u)) & 0xFFFu) * 4u);
                (v12 += 4u);
                w_u32(((uint32)(((v13 + r_u32(((uint32)((v10 + 80))))) + 132))), (r_u32(((uint32)(((v13 + r_u32(((uint32)((v10 + 80))))) + 132)))) + (((uint32)(v15 / 4096) << 12))));
                v10 = r_u32((a1 + (18) * 4u));
                ++v11;
                v13 += 140;
            } while ((v11 < r_u32(((uint32)((v10 + 76))))));
        }
        v9 = a1;
    }
    sub_80029F88(v9, r_u32(0x800FF4E8u), 1);
    sub_80029F88(a1, r_u32(0x800FF5DCu), 0);
    for (i = r_u32(0x800FF220u); i; i = r_u32(((uint32)((i + 28)))))
    {
        if (((r_u32(((uint32)((i + 76)))) & 0x1200000) == 0))
        {
            v17 = r_u16(((uint32)((i + 58))));
            if (((((v17 != 62) && (v17 != 66)) && (v17 != 64)) && (r_u32((a1 + (42) * 4u)) >= ((uint32)(sub_8006696C((a1 + (6) * 4u), (i + 4)))))))
            {
                apocalypse_object_virtual20(r_u32(r_u32((uint32)i + 68u) + 20u), (uint32)i + (uint32)(sint32)(sint16)r_u16(r_u32((uint32)i + 68u) + 16u));
                sub_8001E1B8(((uint32)((i + 4))), 0);
                w_u16(((uint32)((i + 78))), (r_u16(((uint32)((i + 78)))) | (0x100u)));
            }
        }
    }

    for (j = r_u32(0x800FF434u); j; j = r_u32(((uint32)((j + 4)))))
    {
        if (((r_u8(((uint32)((j + 67)))) == 1) && (r_u32((a1 + (42) * 4u)) >= ((uint32)(sub_8006696C((a1 + (6) * 4u), (j + 24)))))))
        {
            v19 = sub_80032DC0(120);
            if (v19)
                sub_8001E8B0(v19, ((uint32)((j + 24))), r_u8(((uint32)((j + 104)))), r_u8(((uint32)((j + 105)))), r_u8(((uint32)((j + 106)))), 8, 18, 50, 4);
            sub_80032ED8(j);
        }
    }

    for (k = r_u32(0x800FF458u); k; k = r_u32(((uint32)((k + 4)))))
    {
        if (((r_u8(((uint32)((k + 67)))) == 4) && (r_u32((a1 + (42) * 4u)) >= ((uint32)(sub_8006696C((a1 + (6) * 4u), (k + 24)))))))
        {
            v21 = sub_80032DC0(120);
            if (v21)
                sub_8001E8B0(v21, ((uint32)((k + 24))), r_u8(((uint32)((k + 265)))), r_u8(((uint32)((k + 266)))), r_u8(((uint32)((k + 267)))), 8, 18, 50, 4);
            sub_80032ED8(k);
        }
    }

    v22 = r_u32(0x800FF794u);
    v23 = r_u32((a1 + (42) * 4u));
    v24 = r_u32((a1 + (25) * 4u));
    v25 = (v23 + r_u32((a1 + (19) * 4u)));
    if (v24 >= v23)
        v26 = (sint32)(r_u32(a1 + 92u) - (uint32)cluster_027_div((sint32)((r_u32(a1 + 92u) - r_u32(a1 + 96u)) * (uint32)v23), v24)) >> 2;
    else
        v26 = (((sint32)(r_u32((a1 + (24) * 4u)))) >> 2);
    while (v22)
    {
        v27 = sub_8006696C((v22 + 4), (a1 + (6) * 4u));
        if (((v27 >= v4) && (v25 >= v27)))
            sub_8001E740(v22, 0, v26);
        v22 = r_u32(((uint32)((v22 + 28))));
    }
}

/* TODO Missing call adapter indirect */
uint32 sub_80029ED4(uint32 a1, uint32 a2)
{
    sint32 v4;
    sint32 result;
    v4 = r_u32(((uint32)((a1 + 72))));
    w_u32(((uint32)((a1 + 68))), 0x800A1380u);
    if (v4)
        apocalypse_object_cleanup(r_u32(r_u32((uint32)v4 + 68u) + 12u), (uint32)v4 + (uint32)(sint32)(sint16)r_u16(r_u32((uint32)v4 + 68u) + 8u), 3u);
    w_u32(0x800FF218u, 0);
    w_u32(0x800FF21Cu, 0);
    sub_80033138(a1, 0);
    result = (a2 & 1);
    if (((a2 & 1) != 0))
        return sub_80032E30(a1);
    return result;
}

uint32 sub_800290F4(uint32 a1, uint32 a2)
{
    sint8 v3;
    sint32 v4;
    sint32 result;
    v3 = a2;
    v4 = r_u32(((uint32)((a1 + 80))));
    w_u32(((uint32)((a1 + 68))), 0x800A1398u);
    sub_8006BC20(v4);
    result = sub_80033090(a1, 0);
    if (((v3 & 1) != 0))
        return sub_80032E30(a1);
    return result;
}

uint32 sub_80020EF8(uint32 a1, uint32 a2, uint32 a3, uint32 a4, uint32 a5)
{
    short v12;
    sint32 v13;
    sint32 result;
    sint32 v15;
    sint32 vars0;
    sint32 vars4;
    sint8 vars8;
    short varsC;
    v12 = a1;
    if ((r_u32(0x800FF384u) == 2))
    {
        v13 = a1;
        if ((a1 == 14))
            return 0;
        v15 = 16;
        if ((a1 == 15))
        {
            v12 = 14;
            goto LABEL_10;
        }
    LABEL_8:
        if ((v13 == v15))
            v12 = 15;

        goto LABEL_10;
    }
    v13 = a1;
    if (!r_u32(0x800FF384u))
    {
        v15 = 14;
        goto LABEL_8;
    }
LABEL_10:
    result = sub_800625AC(344);

    if (result)
        return sub_8001FD28(result, v12, a2, a5, a3, a4);
    return result;
}

/* TODO Missing call adapter indirect */
static void pickup_209EC_attach(uint32 object, uint32 position, sint16 yaw)
{
    uint32 table = r_u32(object + 68u);
    uint32 target = r_u32(table + 28u);
    uint32 receiver = object + (sint16)r_u16(table + 24u);
    /* TODO Exact virtual24 dispatcher with position and signed yaw */
    fprintf(stderr, "Missing 209EC virtual24: target=%08X receiver=%08X position=%08X yaw=%d\n", target, receiver, position, yaw);
    abort();
}

static void beam_27978_hit(uint32 object, uint32 source, const sint32 vector[3], uint32 amount)
{
    uint32 table = r_u32(object + 68u);
    uint32 target = r_u32(table + 52u);
    uint32 receiver = object + (sint16)r_u16(table + 48u);
    /* TODO Exact virtual48 dispatcher with native vector */
    (void)vector;
    fprintf(stderr, "Missing 27978 virtual48: target=%08X receiver=%08X source=%08X amount=%u\n", target, receiver, source, amount);
    abort();
}

uint32 sub_800209EC(uint32 a1)
{
    sint32 result;
    sint32 v3;
    sint32 v4;
    sint32 v5;
    short v6;
    short v7;
    uint32 v8;
    sint32 v9;
    uint32 v10;
    sint32 v11;
    sint32 v12;
    sint32 v13;
    sint32 v14;
    sint32 v15;
    sub_80020518(a1);
    if ((r_u32(((uint32)((a1 + 220)))) >= 0x1F41u))
        return xport_draft_guest_call1(r_u32(r_u32(a1 + 68u) + 60u), a1 + (sint16)r_u16(r_u32(a1 + 68u) + 56u));
    sub_800202CC(a1);
    if (!(r_u8(((uint32)((a1 + 305))))))
    {
        if (!(r_u32(((uint32)((a1 + 340))))))
            sub_80020068(a1);
        pickup_209EC_attach(r_u32(a1 + 340u), a1 + 4u, (sint16)r_u16(a1 + 18u));
    }
    if (!(r_u32(((uint32)((a1 + 296))))))
    {
        if ((r_u16(((uint32)((a1 + 58)))) == 15))
        {
            v3 = sub_80032DC0(104);
            v4 = v3;
            if (!v3)
                goto LABEL_14;
            v5 = 3;
        }
        else
        {
            v3 = sub_80032DC0(104);
            v4 = v3;
            if (!v3)
                goto LABEL_14;
            v5 = 2;
        }
        v3 = sub_80034598(v4, 4, v5);
    LABEL_14:
        w_u32(((uint32)((a1 + 296))), v3);

        w_u8(((uint32)((r_u32(((uint32)((a1 + 296)))) + 94))), 1);
        sub_80034A18(r_u32(((uint32)((a1 + 296)))), 0, 0, 0);
        sub_80034A44(r_u32(((uint32)((a1 + 296)))), 0, 0, 0);
        sub_80034A9C(r_u32(((uint32)((a1 + 296)))), 1, 0, 0, 0u);
        sub_80034994(r_u32(((uint32)((a1 + 296)))), 0);
        w_u8(((uint32)((r_u32(((uint32)((a1 + 296)))) + 66))), 1);
        switch (r_u16(((uint32)((a1 + 58)))))
        {
            case 1:

            case 0xE:
                sub_80034A9C(r_u32(((uint32)((a1 + 296)))), 0, 64, 64, 64u);
                v6 = 60;
                goto LABEL_32;

            case 2:
                sub_80034A9C(r_u32(((uint32)((a1 + 296)))), 0, 160, 70, 0u);
                v6 = 60;
                goto LABEL_32;

            case 3:
                sub_80034994(r_u32(((uint32)((a1 + 296)))), 30);
                sub_80034A9C(r_u32(((uint32)((a1 + 296)))), 0, 0, 100, 0u);
                v6 = 60;
                goto LABEL_32;

            case 4:
                sub_80034A9C(r_u32(((uint32)((a1 + 296)))), 0, 16, 16, 64u);
                w_u16(((uint32)((a1 + 300))), 60);
                v7 = 5;
                goto LABEL_33;

            case 5:
                sub_80034A9C(r_u32(((uint32)((a1 + 296)))), 0, 128, 32, 0u);
                w_u16(((uint32)((a1 + 300))), 50);
                v7 = 5;
                goto LABEL_33;

            case 6:
                sub_80034A9C(r_u32(((uint32)((a1 + 296)))), 0, 100, 50, 0u);
                w_u16(((uint32)((a1 + 300))), 30);
                v7 = 5;
                goto LABEL_33;

            case 7:
                sub_80034A9C(r_u32(((uint32)((a1 + 296)))), 0, 90, 40, 0u);
                v6 = 60;
                goto LABEL_32;

            case 0xA:
                sub_80034A9C(r_u32(((uint32)((a1 + 296)))), 0, 255, 0, 0u);
                v6 = 80;
                goto LABEL_32;

            case 0xF:
                sub_80034A9C(r_u32(((uint32)((a1 + 296)))), 2, 0, 0, 0u);
                sub_80034A9C(r_u32(((uint32)((a1 + 296)))), 1, 128, 0, 0u);
                sub_80034A9C(r_u32(((uint32)((a1 + 296)))), 0, 64, 64, 64u);
                v6 = 60;
                goto LABEL_32;

            case 0x10:
                v8 = 0;
                if (r_u32(((uint32)((r_u32(((uint32)((a1 + 296)))) + 80)))))
                {
                    v9 = 0;
                    do
                    {
                        v10 = v8;
                        if (v9)
                        {
                            v11 = 128;
                            v12 = r_u32(((uint32)((a1 + 296))));
                            v13 = 0;
                        }
                        else
                        {
                            v11 = 64;
                            v12 = r_u32(((uint32)((a1 + 296))));
                            v13 = 64;
                        }
                        ++v8;
                        sub_80034B08(v12, v10, v11, v13, v9 ? 0u : 64u);
                        v9 = (v8 & 1);
                    } while ((v8 < r_u32(((uint32)((r_u32(((uint32)((a1 + 296)))) + 80))))));
                }
                sub_80034A44(r_u32(((uint32)((a1 + 296)))), 0, 0, 0);
                sub_80034A9C(r_u32(((uint32)((a1 + 296)))), 1, 0, 0, 0u);
                v6 = 60;
                goto LABEL_32;

            case 0x11:
                sub_80034A9C(r_u32(((uint32)((a1 + 296)))), 0, 64, 64, 64u);
                v6 = 100;
            LABEL_32:
                w_u16(((uint32)((a1 + 300))), v6);

                v7 = 10;
            LABEL_33:
                w_u16(((uint32)((a1 + 302))), v7);

                break;

            default:
                break;
        }
    }
    if (r_u8(((uint32)((a1 + 306)))))
        w_u32(((uint32)((r_u32(((uint32)((a1 + 296)))) + 100))), -1);
    else
        w_u32(((uint32)((r_u32(((uint32)((a1 + 296)))) + 100))), 0);
    sub_80032EE4(r_u32(((uint32)((a1 + 296)))), (a1 + 4));
    v14 = (r_u16(((uint32)((a1 + 300)))) + ((r_u16(((uint32)((a1 + 302)))) * ((sint16)(r_u16(((uint32)((((uint32)(0x800F863Cu)) + (((600 * r_u32(0x800FF2F0u)) & 0x3FF8)) * 1u))))))) / 4096));
    sub_800349D0(r_u32(((uint32)((a1 + 296)))), 0, v14);
    sub_800349D0(r_u32(((uint32)((a1 + 296)))), 1, v14);
    if ((r_u16(((uint32)((a1 + 58)))) == 15))
        sub_800349D0(r_u32(((uint32)((a1 + 296)))), 2, v14);
    v15 = r_u32(((uint32)((a1 + 296))));
    result = (r_u16(((uint32)((v15 + 96)))) + 50);
    w_u16(((uint32)((v15 + 96))), result);
    return result;
}

/* TODO Missing call adapter SLOWORD */
/* TODO Missing call adapter SLOWORD */

uint32 sub_800202CC(uint32 a1)
{
    sint32 v2;
    sint32 v3;
    sint32 v4;
    sint32 v5;
    sint32 v6;
    sint32 v7;
    sint32 v8;
    sint32 v9;
    sint32 v10;
    sint32 v11;
    sint32 v12;
    sint32 v13;
    sint32 v14;
    sint32 v15;
    sint32 v16;
    short v17;
    sint32 v18;
    sint32 v19;
    sint32 v20;
    sint32 v21;
    sint32 result;
    sint32 v23;
    sint32 v24;
    sint32 v25;
    (v2 = (a1 + 16));
    sub_8006C730((a1 + 16), (a1 + 132));
    sub_8006C624(v2);
    if (!(r_u8(((uint32)((a1 + 307))))))
    {
        (v15 = r_u32(((uint32)((a1 + 320)))));
        (v16 = r_u32(((uint32)((a1 + 324)))));
        w_u32(((uint32)((a1 + 4))), r_u32(((uint32)((a1 + 316)))));
        w_u32(((uint32)((a1 + 8))), v15);
        w_u32(((uint32)((a1 + 12))), v16);
        (v17 = (r_u16(((uint32)((a1 + 328)))) + r_u16(((uint32)((a1 + 334))))));
        { sint32 product = (sint32)r_u16(a1 + 332u) * (sint32)(sint16)r_u16(0x800F863Cu + (r_u16(a1 + 328u) & 0xFFFu) * 4u); w_u32(a1 + 8u, r_u32(a1 + 8u) + ((uint32)(product / 4096) << 12)); }
        w_u16(((uint32)((a1 + 328))), v17);
        goto LABEL_17;
    }
    if ((r_u8(((uint32)((a1 + 308)))) || ((v3 = r_u32(((uint32)((a1 + 312))))), (v4 = (v3 >= 0)), (v5 = (v3 < -4)), v4)))
    {
        (v6 = (a1 + 316));
    }
    else
    {
        (v6 = (a1 + 316));
        if (!v5)
        {
            if (((r_u32(0x800FF2F0u) & 1) != 0))
            {
                (v7 = sub_80067A18(v6, 0, 8000));
                if ((v7 == -1))
                {
                    (v8 = (r_u32(((uint32)((a1 + 312)))) - 1));
                    w_u32(((uint32)((a1 + 312))), v8);
                    if ((v8 < -4))
                        w_u8(((uint32)((a1 + 308))), 1);
                }
                else
                {
                    w_u32(((uint32)((a1 + 312))), v7);
                }
            }
            goto LABEL_17;
        }
    }
    sub_8006C0B8(v6, (a1 + 104));
    (v9 = (r_u32(((uint32)((a1 + 108)))) + r_u32(((uint32)((a1 + 120))))));
    (v10 = r_u8(((uint32)((a1 + 308)))));
    v11 = (sint32)((uint32)v9 - (uint32)(v9 >> (r_u8(a1 + 130u) & 31u)));
    w_u32(((uint32)((a1 + 108))), v9);
    w_u32(((uint32)((a1 + 108))), v11);
    if (!v10)
    {
        (v12 = (r_u32(((uint32)((a1 + 312)))) - (r_u8(((uint32)((a1 + 309)))) << 12)));
        if (v12 < (sint32)r_u32(a1 + 320u) && v11 > 0)
        {
            w_u32(((uint32)((a1 + 320))), v12);
            w_u32(((uint32)((a1 + 104))), 0);
            w_u32(((uint32)((a1 + 108))), 0);
            w_u32(((uint32)((a1 + 112))), 0);
            w_u8(((uint32)((a1 + 307))), 0);
        }
    }
    (v13 = r_u32(((uint32)((a1 + 320)))));
    (v14 = r_u32(((uint32)((a1 + 324)))));
    w_u32(((uint32)((a1 + 4))), r_u32(((uint32)((a1 + 316)))));
    w_u32(((uint32)((a1 + 8))), v13);
    w_u32(((uint32)((a1 + 12))), v14);
LABEL_17:
    (v18 = r_u32(((uint32)((a1 + 320)))));

    (v19 = r_u32(((uint32)((a1 + 324)))));
    w_u32(((uint32)((a1 + 184))), r_u32(((uint32)((a1 + 316)))));
    w_u32(((uint32)((a1 + 188))), v18);
    w_u32(((uint32)((a1 + 192))), v19);
    (v20 = r_u32(((uint32)((a1 + 312)))));
    v21 = (sint32)((uint32)v20 - r_u32(a1 + 8u)) >> 12;
    w_u16(((uint32)((a1 + 202))), v21);
    result = (sint32)((uint32)v21 << 16);
    (v23 = (result >> 16));
    w_u32(((uint32)((a1 + 188))), v20);
    if ((result >> 16))
    {
        (v24 = (60 * v23));
        (v4 = ((60 * v23) >= 0));
        (v25 = ((60 * v23) >> 7));
        if (!v4)
            (v25 = ((v24 + 127) >> 7));
        (result = (140 - v25));
        w_u16(((uint32)((a1 + 200))), (140 - v25));
    }
    else
    {
        w_u16(((uint32)((a1 + 200))), 0);
    }
    return result;
}

uint32 sub_8001ECAC(uint32 a1)
{
    sub_8001EA60(a1);
    sub_800349D0(a1, 0, (r_u32(r_u32(((uint32)((a1 + 76))))) + r_u32(((uint32)((a1 + 120))))));
    return sub_800349D0(a1, 1, 50);
}

uint32 sub_80031974(uint32 a1, uint32 a2)
{
    uint32 v4;
    uint32 v5;
    short v6;
    uint32 v7;
    uint32 v8;
    sint32 v9;
    sint32 v10;
    sint32 v11;
    sint32 v12;
    uint32 v13;
    uint32 v14;
    sint32 v15;
    sint32 v16;
    sint32 v17;
    sint32 v18;
    uint32 v19;
    uint32 v20;
    sint32 v21;
    sint32 v22;
    sint32 v23;
    sint32 v24;
    sint32 v26[3];
    short v27[4];
    int v28[4];
    int v29[4];
    if (!(r_u32(((uint32)((a1 + 72))))))
    {
        v4 = ((uint32)(sub_80032DC0(84)));
        if (v4)
            v4 = sub_800260FC(v4, 3, 1);
        w_u32(((uint32)((a1 + 72))), v4);
        w_u8((((uint32)(v4)) + (66) * 1u), 1);
        sub_800261CC(r_u32(((uint32)((a1 + 72)))), 255, 128, 0);
        v5 = ((uint32)(sub_80032DC0(84)));
        if (v5)
            v5 = sub_800260FC(v5, 3, 0);
        w_u32(((uint32)((a1 + 76))), v5);
        w_u8((((uint32)(v5)) + (66) * 1u), 1);
        sub_800261CC(r_u32(((uint32)((a1 + 76)))), 185, 185, 185);
    }
    v6 = r_u16(((uint32)((r_u32(0x800FF904u) + 18))));
    v27[0] = 0;
    v27[2] = 0;
    v27[1] = (v6 + 1024);
    xport_draft_host_sub_800667CC_p13(v26, 100, v27);
    xport_draft_host_sub_8006C3AC_p13(v28, a2, v26);
    xport_draft_host_sub_8006C34C_p13(v29, a2, v26);
    v7 = r_u32(((uint32)((r_u32(((uint32)((a1 + 76)))) + 80))));
    v8 = r_u32(((uint32)((r_u32(((uint32)((a1 + 72)))) + 80))));
    v9 = v28[1];
    v10 = v28[2];
    w_u32(v7, v28[0]);
    w_u32((v7 + (1) * 4u), v9);
    w_u32((v7 + (2) * 4u), v10);
    v11 = r_u32((v7 + (1) * 4u));
    v12 = r_u32((v7 + (2) * 4u));
    w_u32(v8, r_u32(v7));
    w_u32((v8 + (1) * 4u), v11);
    w_u32((v8 + (2) * 4u), v12);
    v13 = r_u32(((uint32)((r_u32(((uint32)((a1 + 76)))) + 80))));
    v14 = r_u32(((uint32)((r_u32(((uint32)((a1 + 72)))) + 80))));
    v15 = r_u32((a2 + (1) * 4u));
    v16 = r_u32((a2 + (2) * 4u));
    w_u32((v13 + (9) * 4u), r_u32(a2));
    w_u32((v13 + (10) * 4u), v15);
    w_u32((v13 + (11) * 4u), v16);
    v17 = r_u32((v13 + (10) * 4u));
    v18 = r_u32((v13 + (11) * 4u));
    w_u32((v14 + (9) * 4u), r_u32((v13 + (9) * 4u)));
    w_u32((v14 + (10) * 4u), v17);
    w_u32((v14 + (11) * 4u), v18);
    v19 = r_u32(((uint32)((r_u32(((uint32)((a1 + 76)))) + 80))));
    v20 = r_u32(((uint32)((r_u32(((uint32)((a1 + 72)))) + 80))));
    v21 = v29[1];
    v22 = v29[2];
    w_u32((v19 + (18) * 4u), v29[0]);
    w_u32((v19 + (19) * 4u), v21);
    w_u32((v19 + (20) * 4u), v22);
    v23 = r_u32((v19 + (19) * 4u));
    v24 = r_u32((v19 + (20) * 4u));
    w_u32((v20 + (18) * 4u), r_u32((v19 + (18) * 4u)));
    w_u32((v20 + (19) * 4u), v23);
    w_u32((v20 + (20) * 4u), v24);
    sub_8002622C(r_u32(((uint32)((a1 + 72)))), 15);
    w_u16(((uint32)((r_u32(((uint32)((r_u32(((uint32)((a1 + 72)))) + 80)))) + 52))), (((5 * ((sint16)(r_u16(((uint32)((((uint32)(0x800F863Cu)) + (((1200 * r_u32(0x800FF2F0u)) & 0x3FF0)) * 1u))))))) / 4096) + 20));
    return sub_8002622C(r_u32(((uint32)((a1 + 76)))), 10);
}

uint32 sub_8002622C(uint32 a1, uint32 a2)
{
    sint32 result;
    sint32 v3;
    sint32 v4;
    result = r_u32(((uint32)((a1 + 76))));
    v3 = 0;
    if ((result > 0))
    {
        v4 = 0;
        do
        {
            ++v3;
            w_u16(((uint32)(((v4 + r_u32(((uint32)((a1 + 80))))) + 16))), a2);
            result = (v3 < r_u32(((uint32)((a1 + 76)))));
            v4 += 36;
        } while ((v3 < r_u32(((uint32)((a1 + 76))))));
    }
    return result;
}

void nullsub_28(void)
{
    ;
}

/* TODO Missing call adapter indirect */
uint32 sub_80067ABC(uint32 lower, uint32 upper, uint32 object, uint32 trigger)
{
    uint32 count = 0u;
    while (object)
    {
        uint32 next = r_u32(object + 28u);
        if (!sub_80062F48(object))
        {
            sint32 x = (sint32)r_u32(object + 4u);
            sint32 y = (sint32)r_u32(object + 8u);
            sint32 z = (sint32)r_u32(object + 12u);
            if (x >= (sint32)r_u32(lower) && x <= (sint32)r_u32(upper)
                && z >= (sint32)r_u32(lower + 8u) && z <= (sint32)r_u32(upper + 8u)
                && y >= (sint32)r_u32(lower + 4u) && y <= (sint32)r_u32(upper + 4u))
            {
                uint32 table = r_u32(object + 68u);
                if (trigger)
                    xport_draft_guest_call2(r_u32(table + 20u), object + (uint32)(sint32)(sint16)r_u16(table + 16u), 3u);
                else
                    apocalypse_object_cleanup(r_u32(table + 12u), object + (uint32)(sint32)(sint16)r_u16(table + 8u), 3u);
                ++count;
            }
        }
        object = next;
    }
    return count;
}

uint32 sub_80061EF0(uint32 a1, uint32 a2)
{
    sint32 result;
    w_u32(((uint32)((a1 + 68))), 0x800A3224u);
    sub_80062A64(a1, 0x800FF4E8u);
    result = sub_800629BC(a1, 0);
    if (((a2 & 1) != 0))
        return sub_80062608(a1);
    return result;
}

/* TODO Missing call adapter indirect */
/* TODO Postincrement memory expressions may require ordering refinement */
static void pickup_206C4_missing_kind(uint32 kind)
{
    /* TODO Mutated kind requires an inherited MIPS value */
    (void)kind; abort();
}

uint32 sub_800206C4(uint32 a1, uint32 a2)
{
    sint32 v2;
    uint32 v3;
    sint32 result;
    sint32 v6;
    sint32 v7;
    sint32 v8;
    uint32 v9;
    uint32 v10;
    sint32 v11;
    sint32 v12;
    sint32 v13;
    sint32 v14;
    sint32 v15;
    sint32 v16;
    sint32 v17;
    sint32 v18;
    sint32 v19;
    sint32 v20;
    sint32 v21;
    sint32 v22;
    sint32 v23;
    sint32 v24;
    sint32 v25;
    sint32 v26;
    sint32 v27;
    sint32 v28;
    result = r_u16(((uint32)((a1 + 58))));
    v6 = 0;
    switch (r_u16(((uint32)((a1 + 58)))))
    {
        case 1:
            result = sub_8005E260(a2, 4);
            v6 = result;
            v2 = 26;
            if (result)
            {
                sub_8001B708();
                v9 = r_u32((0x800A5744u + (0) * 4u));
                v10 = 0x800A5A30u;
                goto LABEL_32;
            }
            break;

        case 2:
            result = sub_8005E260(a2, 3);
            v6 = result;
            v2 = 26;
            if (result)
            {
                sub_8001B708();
                v9 = r_u32((0x800A5740u + (0) * 4u));
                v10 = 0x800A5A30u;
                goto LABEL_32;
            }
            break;

        case 3:
            result = sub_8005E260(a2, 2);
            v6 = result;
            v2 = 26;
            if (result)
            {
                sub_8001B708();
                v9 = r_u32((0x800A573Cu + (0) * 4u));
                v10 = 0x800A5A84u;
                goto LABEL_32;
            }
            break;

        case 4:
            result = sub_8005E260(a2, 7);
            v6 = result;
            v2 = 26;
            if (result)
            {
                sub_8001B708();
                v9 = r_u32((0x800A574Cu + (0) * 4u));
                v10 = 0x800A5A30u;
                goto LABEL_32;
            }
            break;

        case 5:
            result = sub_8005E260(a2, 6);
            v6 = result;
            v2 = 26;
            if (result)
            {
                sub_8001B708();
                v9 = r_u32((0x800A5748u + (0) * 4u));
                v10 = 0x800A5A30u;
                goto LABEL_32;
            }
            break;

        case 6:
            result = sub_8005E260(a2, 5);
            v6 = result;
            v2 = 26;
            if (result)
            {
                sub_8001B708();
                v9 = r_u32(0x800A5750u);
                v10 = 0x800A5A30u;
                goto LABEL_32;
            }
            break;

        case 7:
            result = sub_8005E260(a2, 1);
            v6 = result;
            v2 = 26;
            if (result)
            {
                sub_8001B708();
                v9 = r_u32((0x800A5738u + (0) * 4u));
                v10 = 0x800A5AD8u;
                goto LABEL_32;
            }
            break;

        case 0xA:
            v26 = r_u32(((uint32)((a2 + 532))));
            result = (v26 + 1);
            if (((sint32)v26 >= 15))
            {
                v6 = 0;
            }
            else
            {
                w_u32(((uint32)((a2 + 532))), result);
                v6 = 1;
            }
            v2 = 26;
            if (v6)
            {
                sub_8001B708();
                v9 = r_u32((0x800A5730u + (0) * 4u));
                goto LABEL_31;
            }
            break;

        case 0xE:

        case 0xF:

        case 0x10:
            result = sub_8005CE70(a2, ((sint16)(r_u16(((uint32)((a1 + 218)))))));
            v6 = result;
            v2 = 26;
            if (result)
            {
                sub_8001B708();
                v25 = r_u16(((uint32)((a1 + 58))));
                if ((v25 == 15))
                {
                    v9 = r_u32((0x800A5728u + (0) * 4u));
                }
                else if ((r_u16(((uint32)((a1 + 58)))) >= 0x10u))
                {
                    /* TODO Unexpected kind would retain an inherited assembly value */
                    if (v25 != 16)
                        pickup_206C4_missing_kind(v25);
                    if ((v25 == 16))
                        v9 = r_u32((0x800A572Cu + (0) * 4u));
                }
                else
                {
                    /* TODO Unexpected kind would retain an inherited assembly value */
                    if (v25 != 14)
                        pickup_206C4_missing_kind(v25);
                    if ((v25 == 14))
                        v9 = r_u32((0x800A5724u + (0) * 4u));
                }
            LABEL_31:
                v10 = 0x800A57A8u;

            LABEL_32:
                result = sub_8001B788(((sint32)(v9)), ((sint32)(v10)));

                v2 = 26;
            }
            break;

        case 0x11:
            w_u32(0x800FF874u, r_u32(0x800FF874u) + 1u);
            if ((sint32)r_u32(0x800FF874u) >= 101)
                w_u32(0x800FF874u, 100);
            v6 = 1;
            sub_8006CDC4();
            v2 = 26;
            sub_8001B708();
            result = sub_8001B788(((sint32)(r_u32((0x800A5734u + (0) * 4u)))), ((sint32)(0x800A59DCu)));
            break;

        default:
            break;
    }

    if (v6)
    {
        apocalypse_object_virtual20(r_u32(r_u32(a1 + 68u) + 20u), a1 + (sint16)r_u16(r_u32(a1 + 68u) + 16u));
        return sub_80069DF0(v2, 0x2000, 0);
    }
    return result;
}

/* TODO Missing call adapter indirect */
uint32 sub_80020168(uint32 a1)
{
    sint32 result;
    sint32 v3;
    sint32 v4;
    result = sub_80062D84(a1);
    v3 = r_u32(((uint32)((a1 + 340))));
    if (v3)
        result = apocalypse_object_cleanup(r_u32(r_u32((uint32)v3 + 68u) + 12u), (uint32)v3 + (uint32)(sint32)(sint16)r_u16(r_u32((uint32)v3 + 68u) + 8u), 3u);
    v4 = r_u32(((uint32)((a1 + 296))));
    w_u32(((uint32)((a1 + 340))), 0);
    if (v4)
        result = apocalypse_object_cleanup(r_u32(r_u32((uint32)v4 + 68u) + 12u), (uint32)v4 + (uint32)(sint32)(sint16)r_u16(r_u32((uint32)v4 + 68u) + 8u), 3u);
    w_u32(((uint32)((a1 + 296))), 0);
    return result;
}

uint32 sub_8001B694(uint32 a1, uint32 a2)
{
    sint32 v2;
    sint32 v3;
    v2 = r_u32(((uint32)((a1 + 12))));
    if (v2)
        w_u32(((uint32)((v2 + 16))), r_u32(((uint32)((a1 + 16)))));
    v3 = r_u32(((uint32)((a1 + 16))));
    if (v3)
        w_u32(((uint32)((v3 + 12))), r_u32(((uint32)((a1 + 12)))));
    if ((a1 == r_u32(0x800FF1ACu)))
        w_u32(0x800FF1ACu, r_u32(((uint32)((a1 + 12)))));
    return sub_8006BC20(a1);
}

/* TODO Missing call adapter SLOWORD */
/* TODO Missing call adapter indirect */
/* TODO Missing call adapter SLOWORD */
/* TODO Missing call adapter indirect */
uint32 sub_80027978(uint32 a1, uint32 a2, uint32 a3)
{
    sint32 path_vector0[3], path_vector1[3], path_vector2[3], path_vector3[3];
    uint32 v6;
    sint32 v7;
    sint32 v8;
    sint32 v9;
    sint32 v10;
    uint32 v11;
    sint32 v12;
    sint32 v13;
    uint32 v14;
    sint32 v15;
    sint32 v16;
    uint32 v17;
    uint32 v18;
    sint32 v19;
    sint32 v20;
    sint32 v21;
    sint32 v22;
    uint32 i;
    sint32 v24;
    sint32 v25;
    sint32 v26;
    sint32 v27;
    uint32 v28;
    uint32 v29;
    sint32 v30;
    sint32 v31;
    sint32 v32;
    sint32 v33;
    sint32 v34;
    sint32 v35;
    sint32 v36;
    sint32 v37;
    sint32 v38;
    uint32 v39;
    uint32 v40;
    sint32 v41;
    sint32 v42;
    sint32 v43;
    sint32 v44;
    sint32 v45;
    sint32 v46;
    short v47;
    sint32 v48;
    sint8 v49;
    sint32 v50;
    uint32 v51;
    sint32 v52;
    sint32 v53;
    sint32 v54;
    sint32 v55;
    sint32 v56;
    sint32 v57;
    sint32 v58;
    uint32 v59;
    sint32 v60;
    uint32 v61;
    uint32 v62;
    sint32 v63;
    sint32 v64;
    sint32 v65;
    sint32 v66;
    sint32 v67;
    sint32 v68;
    sint32 v69;
    sint32 v70;
    sint32 v71;
    sint32 v72;
    uint32 v73;
    sint32 v74;
    sint32 v75;
    uint32 v76;
    sint32 v77;
    sint32 v78;
    sint32 v79;
    uint32 v80;
    sint32 v82;
    sint32 v83;
    sint32 v84;
    unsigned char v85;
    int v86[4];
    sint32 v87[3];
    sint32 v94[3];
    sint32 v101[3];
    sint32 v102;
    sint32 v103;
    sint32 v104;
    if (!(r_u32(((uint32)((a1 + 52))))))
        w_u32(((uint32)((a1 + 52))), sub_80069DF0(r_u32(((uint32)((a1 + 100)))), 0x2000, 0));
    sub_80022B34(a1, 4);
    xport_draft_host_sub_800667CC_p1(v87, r_u32(((uint32)((a1 + 72)))), a3);
    xport_draft_host_sub_8006C4EC_p12(path_vector0, v87, a1 + 72);
    (v86[0] = path_vector0[0]);
    (v86[1] = path_vector0[1]);
    (v86[2] = path_vector0[2]);
    if (!(r_u32(((uint32)((a1 + 60))))))
    {
        (v6 = ((uint32)(sub_80032DC0(84))));
        if (v6)
            (v6 = sub_800260FC(v6, 10, 1));
        w_u32(((uint32)((a1 + 60))), v6);
        w_u8((((uint32)(v6)) + (66) * 1u), 1);
        (v7 = 0);
        sub_800261CC(r_u32(((uint32)((a1 + 60)))), r_u8(((uint32)((a1 + 80)))), r_u8(((uint32)((a1 + 81)))), r_u8(((uint32)((a1 + 82)))));
        w_u16(((uint32)((r_u32(((uint32)((r_u32(((uint32)((a1 + 60)))) + 80)))) + 340))), 0);
        w_u8(((uint32)((r_u32(((uint32)((r_u32(((uint32)((a1 + 60)))) + 80)))) + 300))), (r_u8(((uint32)((r_u32(((uint32)((r_u32(((uint32)((a1 + 60)))) + 80)))) + 300)))) >> (1)));
        w_u8(((uint32)((r_u32(((uint32)((r_u32(((uint32)((a1 + 60)))) + 80)))) + 301))), (r_u8(((uint32)((r_u32(((uint32)((r_u32(((uint32)((a1 + 60)))) + 80)))) + 301)))) >> (1)));
        w_u8(((uint32)((r_u32(((uint32)((r_u32(((uint32)((a1 + 60)))) + 80)))) + 302))), (r_u8(((uint32)((r_u32(((uint32)((r_u32(((uint32)((a1 + 60)))) + 80)))) + 302)))) >> (1)));
        w_u8(((uint32)((r_u32(((uint32)((r_u32(((uint32)((a1 + 60)))) + 80)))) + 336))), 0);
        w_u8(((uint32)((r_u32(((uint32)((r_u32(((uint32)((a1 + 60)))) + 80)))) + 337))), 0);
        (v8 = 0);
        w_u8(((uint32)((r_u32(((uint32)((r_u32(((uint32)((a1 + 60)))) + 80)))) + 338))), 0);
        (v9 = ((sint32)(r_u32((a2 + (1) * 4u)))));
        (v10 = ((sint32)(r_u32((a2 + (2) * 4u)))));
        (v82 = ((sint32)(r_u32(a2))));
        (v83 = v9);
        (v84 = v10);
        do
        {
            (v11 = ((uint32)((v8 + r_u32(((uint32)((r_u32(((uint32)((a1 + 60)))) + 80))))))));
            (v12 = v83);
            (v13 = v84);
            w_u32(v11, v82);
            w_u32((v11 + (1) * 4u), v12);
            w_u32((v11 + (2) * 4u), v13);
            ++v7;
            {
                sint32 position[3] = {v82, v83, v84};
                xport_draft_host_sub_8006C0B8_p12(position, v87);
                v82 = position[0];
                v83 = position[1];
                v84 = position[2];
            };
            (v8 += 36);
        } while ((v7 < 10));
        (v14 = ((uint32)(sub_80032DC0(84))));
        if (v14)
            (v14 = sub_800260FC(v14, 10, 0));
        w_u32(((uint32)((a1 + 64))), v14);
        w_u8((((uint32)(v14)) + (66) * 1u), 1);
        sub_800261CC(r_u32(((uint32)((a1 + 64)))), r_u8(((uint32)((a1 + 83)))), r_u8(((uint32)((a1 + 84)))), r_u8(((uint32)((a1 + 85)))));
        sub_8002622C(r_u32(((uint32)((a1 + 64)))), 10);
        (v15 = 0);
        (v16 = 0);
        do
        {
            ++v15;
            (v17 = ((uint32)((v16 + r_u32(((uint32)((r_u32(((uint32)((a1 + 64)))) + 80))))))));
            (v18 = ((uint32)((v16 + r_u32(((uint32)((r_u32(((uint32)((a1 + 60)))) + 80))))))));
            (v19 = r_u32((v18 + (1) * 4u)));
            (v20 = r_u32((v18 + (2) * 4u)));
            w_u32(v17, r_u32(v18));
            w_u32((v17 + (1) * 4u), v19);
            w_u32((v17 + (2) * 4u), v20);
            (v16 += 36);
        } while ((v15 < 10));
        w_u8(((uint32)((r_u32(((uint32)((r_u32(((uint32)((a1 + 64)))) + 80)))) + 336))), 0);
        w_u8(((uint32)((r_u32(((uint32)((r_u32(((uint32)((a1 + 64)))) + 80)))) + 337))), 0);
        w_u8(((uint32)((r_u32(((uint32)((r_u32(((uint32)((a1 + 64)))) + 80)))) + 338))), 0);
        if (r_u32(((uint32)((a1 + 96)))))
        {
            (v21 = sub_80032DC0(104));
            if (v21)
                (v21 = sub_80034598(v21, 7, 1));
            w_u32(((uint32)((a1 + 92))), v21);
            w_u8(((uint32)((v21 + 66))), 1);
            sub_80034A18(r_u32(((uint32)((a1 + 92)))), r_u8(((uint32)((a1 + 89)))), r_u8(((uint32)((a1 + 90)))), r_u8(((uint32)((a1 + 91)))));
            sub_80034A44(r_u32(((uint32)((a1 + 92)))), r_u8(((uint32)((a1 + 86)))), r_u8(((uint32)((a1 + 87)))), r_u8(((uint32)((a1 + 88)))));
            sub_80034A9C(r_u32(((uint32)((a1 + 92)))), 0, 0, 0, 0u);
            sub_800349D0(r_u32(((uint32)((a1 + 92)))), 0, ((sint32)(30u * r_u32(a1 + 96u)) >> 8));
            (v22 = r_u32(((uint32)((a1 + 92)))));
            for ((i = 0); (i < r_u32(((uint32)((v22 + 80))))); ++i)
            {
                w_u32(((uint32)(((8 * i) + r_u32(((uint32)((v22 + 72))))))), 0);
                (v22 = r_u32(((uint32)((a1 + 92)))));
            }

            sub_80032EE4(r_u32(((uint32)((a1 + 92)))), (a1 + 8));
        }
    }
    (v24 = 10);
    if (!(r_u32(((uint32)((a1 + 76))))))
        (v24 = 2);
    (v102 = v24);
    (v25 = r_u32(((uint32)((r_u32(((uint32)((a1 + 60)))) + 76)))));
    (v26 = (v25 - 1));
    if (((v24 - 1) < (v25 - 1)))
    {
        (v27 = (36 * (v25 - 1)));
        do
        {
            (v28 = ((uint32)((v27 + r_u32(((uint32)((r_u32(((uint32)((a1 + 64)))) + 80))))))));
            (v29 = ((uint32)((v27 + r_u32(((uint32)((r_u32(((uint32)((a1 + 60)))) + 80))))))));
            xport_draft_host_sub_8006C40C_p123(path_vector1, v87, &v102);
            (v27 -= 36);
            xport_draft_host_sub_8006C34C_p13(path_vector0, (r_u32(((uint32)((r_u32(((uint32)((a1 + 60)))) + 80)))) + (36 * (v26 - v102))), path_vector1);
            (v30 = v102);
            --v26;
            (v31 = path_vector0[1]);
            (v32 = path_vector0[2]);
            w_u32(v29, path_vector0[0]);
            w_u32((v29 + (1) * 4u), v31);
            w_u32((v29 + (2) * 4u), v32);
            (v33 = r_u32((v29 + (1) * 4u)));
            (v34 = r_u32((v29 + (2) * 4u)));
            w_u32(v28, r_u32(v29));
            w_u32((v28 + (1) * 4u), v33);
            w_u32((v28 + (2) * 4u), v34);
        } while (((v30 - 1) < v26));
    }
    (v35 = ((sint32)(r_u32((a2 + (1) * 4u)))));
    (v36 = ((sint32)(r_u32((a2 + (2) * 4u)))));
    (v82 = ((sint32)(r_u32(a2))));
    (v83 = v35);
    (v84 = v36);
    (v37 = 0);
    if ((v102 > 0))
    {
        (v38 = 0);
        do
        {
            ++v37;
            (v39 = ((uint32)((v38 + r_u32(((uint32)((r_u32(((uint32)((a1 + 60)))) + 80))))))));
            (v40 = ((uint32)((v38 + r_u32(((uint32)((r_u32(((uint32)((a1 + 64)))) + 80))))))));
            (v41 = v83);
            (v42 = v84);
            w_u32(v39, v82);
            w_u32((v39 + (1) * 4u), v41);
            w_u32((v39 + (2) * 4u), v42);
            (v43 = r_u32((v39 + (1) * 4u)));
            (v44 = r_u32((v39 + (2) * 4u)));
            w_u32(v40, r_u32(v39));
            w_u32((v40 + (1) * 4u), v43);
            w_u32((v40 + (2) * 4u), v44);
            {
                sint32 position[3] = {v82, v83, v84};
                xport_draft_host_sub_8006C0B8_p12(position, v87);
                v82 = position[0];
                v83 = position[1];
                v84 = position[2];
            };
            (v38 += 36);
        } while ((v37 < v102));
    }
    (v45 = 0);
    (v46 = 0);
    (v47 = 0);
    (v48 = 0);
    while ((v45 < 9))
    {
        (v49 = sub_80066570(256));
        w_u8(((uint32)(((v48 + r_u32(((uint32)((r_u32(((uint32)((a1 + 64)))) + 80))))) + 12))), v49);
        w_u8(((uint32)(((v48 + r_u32(((uint32)((r_u32(((uint32)((a1 + 64)))) + 80))))) + 13))), v49);
        w_u8(((uint32)(((v48 + r_u32(((uint32)((r_u32(((uint32)((a1 + 64)))) + 80))))) + 14))), v49);
        w_u16(r_u32(r_u32(a1 + 60u) + 80u) + (uint32)v48 + 16u, 6 * (sint32)(sint16)r_u16(0x800F863Cu + ((900u * r_u32(0x800FF2F0u) + (uint32)v47) & 0xFFFu) * 4u) / 4096 + 19 + v46);
        (v46 += 6);
        (v47 -= 1150);
        (v48 += 36);
        ++v45;
    }

    (v50 = r_u32(((uint32)((a1 + 92)))));
    (v51 = 0);
    if (v50)
    {
        (v52 = ((sint32)(20u * r_u32(a1 + 96u)) >> 8));
        if (r_u32(((uint32)((v50 + 80)))))
        {
            do
                w_u32(((uint32)(((8 * v51++) + r_u32(((uint32)((r_u32(((uint32)((a1 + 92)))) + 72))))))), (v52 + sub_80066570(v52)));
            while ((v51 < r_u32(((uint32)((r_u32(((uint32)((a1 + 92)))) + 80))))));
        }
        w_u16(((uint32)((r_u32(((uint32)((a1 + 92)))) + 96))), sub_80066570(1024));
        sub_80032EE4(r_u32(((uint32)((a1 + 92)))), a2);
    }
    (v103 = 9);
    xport_draft_host_sub_8006C47C_p123(v94, &v103, v87);
    xport_draft_host_sub_8006C34C_p13(path_vector0, a2, v94);
    (v53 = ((sint32)(r_u32((a2 + (1) * 4u)))));
    (v54 = ((sint32)(r_u32((a2 + (2) * 4u)))));
    (path_vector1[0] = ((sint32)(r_u32(a2))));
    (path_vector1[1] = v53);
    (path_vector1[2] = v54);
    xport_draft_host_sub_80022B54_p2(a1, path_vector1, a3);
    (v55 = 0);
    (v56 = 0);
    w_u32(0x800ED638u, path_vector1[0]);
    w_u32(0x800ED63Cu, path_vector1[1]);
    w_u32(0x800ED640u, path_vector1[2]);
    w_u32(0x800ED644u, path_vector0[0]);
    w_u32(0x800ED648u, path_vector0[1]);
    w_u32(0x800ED64Cu, path_vector0[2]);
    sub_8007BB24(0x800ED638u);
    w_u32(0x800FF974u, 1);
    sub_8007DD04(0x800ED638u, 1);
    (v57 = ((sint32)(r_u32((a2 + (1) * 4u)))));
    (v58 = ((sint32)(r_u32((a2 + (2) * 4u)))));
    (v82 = ((sint32)(r_u32(a2))));
    (v83 = v57);
    (v84 = v58);
    (v59 = r_u32(((uint32)((a1 + 44)))));
    w_u32(0x800FF974u, 0);
    {
        uint32 start_position[3] = {(uint32)v82, (uint32)v83, (uint32)v84};
        uint32 end_position[3] = {(uint32)path_vector0[0], (uint32)path_vector0[1], (uint32)path_vector0[2]};
        uint32 intersection[4];
        v60 = xport_draft_host_sub_8007C398_p123(start_position, end_position, intersection, r_u32(v59), 0u);
        path_vector1[0] = intersection[0];
        path_vector1[1] = intersection[1];
        path_vector1[2] = intersection[2];
    }
    (v61 = ((uint32)(v60)));
    if (r_u32(0x800ED6A0u))
    {
        (path_vector0[0] = r_u32(0x800ED6A4u));
        (path_vector0[1] = r_u32(0x800ED6A8u));
        (path_vector0[2] = r_u32(0x800ED6ACu));
        if ((!v60 || ((v62 = sub_8006696C(a2, (v60 + 4))), (v62 >= xport_draft_host_sub_8006696C_p2(a2, path_vector0)))))
        {
            (v56 = 1);
            goto LABEL_39;
        }
    }
    else if (!v60)
    {
        goto LABEL_39;
    }
    (v55 = 1);
    (v63 = r_u32((v61 + (2) * 4u)));
    (v64 = r_u32((v61 + (3) * 4u)));
    (path_vector0[0] = r_u32((v61 + (1) * 4u)));
    (path_vector0[1] = v63);
    (path_vector0[2] = v64);
LABEL_39:
    if (v55)
        beam_27978_hit(v61, r_u32(a1 + 40u), v86, 10u);

    if (v56)
    {
        sub_8001E740(r_u32(0x800ED6A0u), r_u32(0x800ED6B8u), r_u32(((uint32)((a1 + 40)))));
        if (((r_u16(((uint32)(r_u32(0x800ED6A0u)))) & 0x10) != 0))
            beam_27978_hit(r_u32(0x800ED6A0u), r_u32(a1 + 40u), v86, 10u);
    }
    if ((v55 || v56))
    {
        (v65 = xport_draft_host_sub_8006689C_p2(a2, path_vector0));
        (v66 = 0);
        if ((v65 >= 101))
        {
            (v66 = cluster_027_div(v65, (sint32)r_u32(a1 + 72u)));
            if ((v66 >= 9))
            {
                if ((v66 >= 10))
                    (v66 = 9);
                (v69 = r_u32(((uint32)((r_u32(((uint32)((a1 + 60)))) + 80)))));
                (v70 = (v69 + (36 * v66)));
                (v68 = ((36 * v66) - 36));
            }
            else
            {
                (v68 = (36 * v66));
                (v69 = r_u32(((uint32)((r_u32(((uint32)((a1 + 60)))) + 80)))));
                (v70 = ((v69 + (36 * v66)) + 36));
            }
            xport_draft_host_sub_8006C3AC_p1(path_vector3, v70, v69 + v68);
            (path_vector2[0] = path_vector3[0]);
            (path_vector2[1] = path_vector3[1]);
            (path_vector2[2] = path_vector3[2]);
            (v104 = (v65 - (v66 * r_u32(((uint32)((a1 + 72)))))));
            xport_draft_host_sub_8006C40C_p123(v101, path_vector2, &v104);
            xport_draft_host_sub_8006C4EC_p12(path_vector3, v101, a1 + 72);
            (path_vector2[0] = path_vector3[0]);
            (path_vector2[1] = path_vector3[1]);
            (path_vector2[2] = path_vector3[2]);
            xport_draft_host_sub_8006C0B8_p1(path_vector2, (r_u32(((uint32)((r_u32(((uint32)((a1 + 60)))) + 80)))) + (36 * v66)));
            (v67 = (v66 < 10));
        }
        else
        {
            (path_vector2[0] = path_vector0[0]);
            (path_vector2[1] = path_vector0[1]);
            (path_vector2[2] = path_vector0[2]);
            (v67 = 1);
        }
        (v71 = v66);
        if (v67)
        {
            (v72 = (36 * v66));
            do
            {
                (v73 = ((uint32)((v72 + r_u32(((uint32)((r_u32(((uint32)((a1 + 60)))) + 80))))))));
                (v74 = path_vector2[1]);
                (v75 = path_vector2[2]);
                w_u32(v73, path_vector2[0]);
                w_u32((v73 + (1) * 4u), v74);
                w_u32((v73 + (2) * 4u), v75);
                ++v71;
                (v76 = ((uint32)((v72 + r_u32(((uint32)((r_u32(((uint32)((a1 + 64)))) + 80))))))));
                (v77 = path_vector2[1]);
                (v78 = path_vector2[2]);
                w_u32(v76, path_vector2[0]);
                w_u32((v76 + (1) * 4u), v77);
                w_u32((v76 + (2) * 4u), v78);
                (v72 += 36);
            } while ((v71 < 10));
        }
        (v79 = (r_u32(((uint32)((a1 + 68)))) + 1));
        w_u32(((uint32)((a1 + 68))), v79);
        if (((v79 & 1) != 0))
        {
            (v80 = ((uint32)(((36 * v66) + r_u32(((uint32)((r_u32(((uint32)((a1 + 60)))) + 80))))))));
            {
                sint32 position[3] = {path_vector2[0], path_vector2[1], path_vector2[2]};
                xport_draft_host_sub_8001D320_p1(position, 50, r_u8(v80 + 12), r_u8(v80 + 13), r_u8(v80 + 14), 3, 1, 100);
            }
        }
    }
    return 1;
}

uint32 sub_8006C0FC(uint32 a1, uint32 a2)
{
    uint32 result;
    result = a1;
    w_u32(a1, (r_u32(a1) - (r_u32(a2))));
    w_u32((a1 + (1) * 4u), (r_u32((a1 + (1) * 4u)) - (r_u32((a2 + (1) * 4u)))));
    w_u32((a1 + (2) * 4u), (r_u32((a1 + (2) * 4u)) - (r_u32((a2 + (2) * 4u)))));
    return result;
}

/* TODO Missing call adapter indirect */
uint32 sub_800318C0(uint32 a1, uint32 a2)
{
    sint32 v4;
    sint32 v5;
    sint32 result;
    v4 = r_u32((a1 + (18) * 4u));
    w_u32((a1 + (17) * 4u), 0x800A1A68u);
    if (v4)
    {
        apocalypse_object_cleanup(r_u32(r_u32((uint32)v4 + 68u) + 12u), (uint32)v4 + (uint32)(sint32)(sint16)r_u16(r_u32((uint32)v4 + 68u) + 8u), 3u);
        v5 = r_u32((a1 + (19) * 4u));
        if (v5)
        apocalypse_object_cleanup(r_u32(r_u32((uint32)v5 + 68u) + 12u), (uint32)v5 + (uint32)(sint32)(sint16)r_u16(r_u32((uint32)v5 + 68u) + 8u), 3u);
    }
    w_u32((a1 + (17) * 4u), 0x800A1AA8u);
    sub_80033138(a1, 0);
    result = (a2 & 1);
    if (((a2 & 1) != 0))
        return sub_80032E30(a1);
    return result;
}

uint32 apocalypse_trigger_object_create(uint32 a1, const uint32 position[3], uint32 a3, uint32 a4, uint32 a5, uint32 a6, uint32 a7, uint32 a8, uint32 a9, uint32 a10, uint32 a11, uint32 a12, uint32 a13)
{
    uint32 first;
    sint32 result;
    sint32 v21;
    sint32 v22;
    sub_800330F4(a1);
    result = a1;
    w_u32(((uint32)((a1 + 68))), 0x800A10B4u);
    first = position[0];
    v21 = position[1];
    v22 = position[2];
    w_u32(((uint32)((a1 + 24))), first);
    w_u32(((uint32)((a1 + 28))), v21);
    w_u32(((uint32)((a1 + 32))), v22);
    w_u32(((uint32)((a1 + 92))), a4);
    w_u8(((uint32)((a1 + 67))), 2);
    w_u16(((uint32)((a1 + 10))), a3);
    w_u32(((uint32)((a1 + 72))), a5);
    w_u32(((uint32)((a1 + 76))), a6);
    w_u8(((uint32)((a1 + 80))), a8);
    w_u8(((uint32)((a1 + 81))), a9);
    w_u8(((uint32)((a1 + 82))), a10);
    w_u8(((uint32)((a1 + 83))), a11);
    w_u8(((uint32)((a1 + 84))), a12);
    w_u8(((uint32)((a1 + 85))), a13);
    w_u32(((uint32)((a1 + 88))), a7);
    return result;
}

uint32 sub_8001C8E8(uint32 a1, uint32 a2, uint32 a3, uint32 a4, uint32 a5, uint32 a6, uint32 a7, uint32 a8, uint32 a9, uint32 a10, uint32 a11, uint32 a12, uint32 a13)
{
    return apocalypse_trigger_object_create(a1, (const uint32 *)psx_addr(a2, 12u), a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13);
}

/* TODO Missing call adapter sub_8008847C */
uint32 sub_8002E600(uint32 resource)
{
    uint32 result = r_u32(0x800FF250u);
    if (!result)
    {
        uint32 texture = sub_8006E278(resource);
        sint16 rectangle[4] = {(sint16)r_u16(texture + 28u), (sint16)r_u16(texture + 30u),
            (sint16)r_u16(0x800FF290u), (sint16)r_u16(0x800FF292u)};
        w_u16(0x800FF28Au, rectangle[0]);
        w_u16(0x800FF28Cu, rectangle[1]);
        xport_draft_host_sub_8008847C_p1(rectangle, 1, 1, 1);
        uint32 width = r_u16(0x800FF290u);
        if (r_u8(0x800FF298u)) width = 3u * width / 2u;
        w_u16(0x800FF28Eu, r_u16(0x800FF28Au) + width);
        uint32 x = r_u16(0x800FF28Au), y = r_u16(0x800FF28Cu);
        return sub_8002F50C(((y & 0x100u) >> 4) | ((x & 0x3FFu) >> 6) | 0x100u | ((y & 0x200u) << 2), texture);
    }
    return result;
}

uint32 sub_8002E6D8(uint32 a1)
{
    sint32 result;
    sint32 v2;
    uint32 v3;
    sint32 v4;
    sint32 v5;
    result = r_u32(0x800FF250u);
    if (!r_u32(0x800FF250u))
    {
        v2 = r_u32(0x800FF294u);
        result = (r_u32(0x800FF294u) < 8);
        if ((r_u32(0x800FF294u) < 8))
        {
            v3 = (0x800A5B2Cu + ((3 * r_u32(0x800FF294u))) * 4u);
            v4 = ((sint32)(r_u32((a1 + (1) * 4u))));
            v5 = ((sint32)(r_u32((a1 + (2) * 4u))));
            w_u32(v3, ((sint32)(r_u32(a1))));
            w_u32((v3 + (1) * 4u), v4);
            w_u32((v3 + (2) * 4u), v5);
            result = (v2 + 1);
            w_u32(0x800FF294u, (v2 + 1));
        }
    }
    return result;
}

/* TODO Missing call adapter SHIWORD */
uint32 sub_80067930(uint32 value)
{
    uint32 angle = 512u, negative = (sint32)value < 0;
    sint32 step = 256;
    if (negative) value = 0u - value;
    do
    {
        sint32 cosine = (sint16)r_u16(0x800F863Cu + angle * 4u + 2u);
        if ((sint32)value < cosine) angle += (uint32)step;
        else if ((sint32)value == cosine) break;
        else angle -= (uint32)step;
        step >>= 1;
    } while (step);
    return negative ? 2048u - angle : angle;
}

uint32 sub_80053950(uint32 a1, uint32 a2)
{
    sint32 result;
    sub_8006613C((a1 + 496), a2);
    result = (r_u32(((uint32)((a1 + 396)))) | 0x40);
    w_u32(((uint32)((a1 + 396))), result);
    return result;
}

uint32 sub_8006FBB0(uint32 a1, uint32 a2, uint32 a3)
{
    sint32 v4;
    v4 = r_u32(0x800FF7DCu);
    w_u16(((uint32)((a1 + 4))), a2);
    w_u32(((uint32)(a1)), 0x800A3ACCu);
    w_u32(0x800FF7DCu, a1);
    w_u16(((uint32)((a1 + 6))), a3);
    w_u32(((uint32)((a1 + 20))), v4);
    sub_8006613C(((uint32)((a1 + 8))), a2);
    return a1;
}

/* TODO Missing call adapter indirect */
uint32 sub_8005E708(uint32 object)
{
    sub_80069DF0(8, 0x2000, 0);
    sub_8005E90C(object);
    uint32 state = r_u32(object + 460u), animation;
    w_u16(object + 218u, 0);
    switch (state)
    {
        case 1: case 16: case 32: case 256: case 512: case 1024: case 2048:
            animation = 15; break;
        case 2: case 4: case 8: case 128: case 4096: case 8192: case 16384:
        case 32768: case 65536: case 131072: case 524288:
            animation = 16; break;
        case 262144:
            return 0x80000u;
        default:
        {
            uint32 table = r_u32(object + 68u);
            return xport_draft_guest_call1(r_u32(table + 20u), object + (sint16)r_u16(table + 16u));
        }
    }
    w_u32(object + 112u, 0);
    w_u32(object + 108u, 0);
    w_u32(object + 104u, 0);
    w_u32(object + 460u, 0x40000u);
    return sub_80063038(object, animation, 0, 0xFFFFFFFFu);
}

/* TODO Postincrement memory expressions may require ordering refinement */
uint32 sub_8005C7C0(uint32 a1, uint32 a2)
{
    sint32 v4;
    sint32 v5;
    sint32 result;
    v4 = r_u32((a1 + (79) * 4u));
    w_u32((a1 + (17) * 4u), 0x800A32A4u);
    if (v4)
        sub_8006BC20(v4);
    v5 = r_u32((a1 + (80) * 4u));
    if (v5)
        sub_8006BC20(v5);
    sub_80062A64(a1, 0x800FF5A0u);
    (w_u32(0x800FF59Cu, (r_u32(0x800FF59Cu) - 1u)), (r_u32(0x800FF59Cu) - 1u));
    sub_8005DD84(a1);
    sub_80062FB8(a1, 0);
    result = (a2 & 1);
    if (((a2 & 1) != 0))
        return sub_80062608(a1);
    return result;
}

/* TODO Missing call adapter indirect */
uint32 sub_8005DD84(uint32 object)
{
    for (uint32 index = 0; index < 8u; ++index)
    {
        uint32 slot = object + 612u + index * 4u;
        uint32 child = r_u32(slot);
        if (child)
        {
            uint32 table = r_u32(child);
            apocalypse_object_cleanup(r_u32(table + 12u), child + (sint16)r_u16(table + 8u), 3u);
            w_u32(slot, 0);
        }
    }
    return 0;
}

uint32 sub_80022C04(uint32 a1, uint32 a2)
{
    sint32 result;
    w_u32(a1, 0x800A1548u);
    result = sub_80022A48(a1, 0);
    if (((a2 & 1) != 0))
        return sub_8002FF34(a1);
    return result;
}

/* TODO Implement the missing PsyQ voice pause service */
static void apocalypse_missing_voice_pause(sint32 voice, uint32 first, uint32 second)
{
    fprintf(stderr, "Missing PsyQ voice pause 8009020C: voice=%d first=%08X second=%08X\n",
            voice, first, second);
    abort();
}

uint32 sub_8006A114(void)
{
    uint32 result = r_u32(0x800FF6B0u), first = 0x800E5BA8u, second = 0x800E5BD8u;
    sint32 voice;
    sint16 attributes[8];
    if (result)
        return result;
    w_u32(0x800FF6B0u, 1u);
    attributes[1] = 0;
    attributes[0] = 0;
    attributes[3] = 3;
    /* TODO Review untouched attribute fields when implementing the SDK boundary */
    for (voice = 0; voice < 24; ++voice)
    {
        apocalypse_missing_voice_pause((sint32)(sint16)voice, first, second);
        xport_draft_host_sub_80093ED8_p2((uint32)voice, attributes);
        second += 2u;
        first += 2u;
    }
    if (r_u32(0x800FF6B4u))
        w_u32(0x800FFC20u, 1u);
    else
    {
        sub_8006A3C4();
        w_u32(0x800FFC20u, 0u);
    }
    return sub_80097F34(11u, 0u);
}

/* TODO Implement the missing PsyQ sequence stop service */
static void apocalypse_missing_sequence_stop(sint32 sequence)
{
    fprintf(stderr, "Missing PsyQ sequence stop 8008E638: sequence=%d\n", sequence);
    abort();
}

uint32 sub_8006A3C4(void)
{
    apocalypse_missing_sequence_stop((sint32)(sint16)r_u16(0x800FF6ACu));
    w_u32(0x800FF6B4u, 1);
    w_u32(0x800FF6B8u, 0);
    return 1u;
}

void nullsub_19(void)
{
    ;
}
