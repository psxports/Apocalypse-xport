#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* Unverified draft; TODO Recover omitted call arguments, host-buffer adapters and signed field widths */

void sub_8003334C(uint32 a1, uint32 a2)
{
    w_u16(((uint32)((a1 + 72))), a2);
}

/* Preserve signed queue priorities and semantic host coordinates */
static uint32 explosion_mark_objects(uint32 object, uint32 result)
{
    for (; object; object = r_u32(object + 28u))
    {
        result = r_u16(object + 78u) & 64u;
        if (!result)
        {
            result = r_u16(object + 76u) | 4u;
            w_u16(object + 76u, result);
        }
    }
    return result;
}

static uint32 explosion_queue(uint32 guest_position, const void *host_position,
    uint32 mode, uint32 radius, uint32 strength, uint32 priority)
{
    uint32 count = r_u32(0x800FF648u);
    uint32 index, destination, words[3], result;
    if ((sint32)count < 3)
    {
        index = count;
        result = 0x800A7294u;
    }
    else
    {
        for (index = 0u; index < 3u; ++index)
            if ((sint32)r_u32(0x800A7294u + 4u * index) < (sint32)priority)
                break;
        if (index == 3u)
            return 0u;
        result = 0x800A7288u + 4u * index;
    }
    if (host_position)
        memcpy(words, host_position, sizeof(words));
    else
    {
        words[0] = r_u32(guest_position);
        words[1] = r_u32(guest_position + 4u);
        words[2] = r_u32(guest_position + 8u);
    }
    destination = 0x800A724Cu + 12u * index;
    w_u32(destination, words[0]);
    w_u32(destination + 4u, words[1]);
    w_u32(destination + 8u, words[2]);
    w_u32(0x800A7270u + 4u * index, mode);
    w_u32(0x800A727Cu + 4u * index, radius);
    w_u32(0x800A7288u + 4u * index, strength);
    w_u32(0x800A7294u + 4u * index, priority);
    if ((sint32)count < 3)
        w_u32(0x800FF648u, count + 1u);
    result = explosion_mark_objects(r_u32(0x800FF4E8u), result);
    return explosion_mark_objects(r_u32(0x800FF5DCu), result);
}

uint32 sub_80067388(uint32 position, uint32 mode, uint32 radius, uint32 strength, uint32 priority)
{
    return explosion_queue(position, NULL, mode, radius, strength, priority);
}

uint32 xport_draft_host_sub_80067388_p1(const void *position, uint32 mode, uint32 radius, uint32 strength, uint32 priority)
{
    return explosion_queue(0u, position, mode, radius, strength, priority);
}

uint32 sub_80067338(uint32 object)
{
    /* Empty list has no defined C return input; queue calls preserve their carrier */
    return explosion_mark_objects(object, 0u);
}

static uint32 explosion_proximity(uint32 a1, const void *host_position, uint32 a2)
{
    sint32 result;
    sint32 v5;
    sint32 v6;
    uint32 v7;
    uint32 v8;
    sint32 v9;
    sint32 v10;
    sint32 v11;
    uint32 v12;
    sint32 v13;
    sint32 v14;
    unsigned char v15;
    sint32 v16;
    short v17;
    sint32 listener[3];
    result = r_u32(0x800FF738u);
    if (!r_u32(0x800FF738u))
    {
        listener[0] = (r_u32(0x800ED520u) << 12);
        listener[1] = (r_u32(0x800ED524u) << 12);
        listener[2] = (r_u32(0x800ED528u) << 12);
        v5 = host_position ? apocalypse_position_distance_native(host_position, listener) : xport_draft_host_sub_8006696C_p2(a1, listener);
        if (a2)
        {
            result = 1;
            if ((a2 == 1))
            {
                v11 = 2;
                if (((r_u32(0x800FF2F0u) & 1) != 0))
                    v11 = 1;
                if (host_position)
                    xport_draft_host_sub_80069EF4_p2(v11, host_position, 0);
                else
                    sub_80069EF4(v11, a1, 0);
                v12 = ((uint32)(sub_80032DC0(160)));
                if (v12)
                {
                    if (host_position)
                        apocalypse_explosion_debris_native(v12, host_position, 20, 200, 150, 10, 5000, 10, 0, 0, 1, 0, 7, 250, 5, 20, 65);
                    else
                        sub_8001D484(v12, a1, 20, 200, 150, 10, 5000, 10, 0, 0, 1, 0, 7, 250, 5, 20, 65);
                }
                if ((v5 < 6000))
                    sub_8001BE78(0x46u, 0x1Eu, 0, 0x14u, 0, 0);
                result = (v5 < 2000);
                if ((v5 < 4000))
                {
                    v8 = a1;
                    if ((v5 < 2000))
                    {
                        v9 = r_u32(0x800FF904u);
                        v10 = 0;
                        return apocalypse_explosion_camera_native(v9, v10);
                    }
                    goto LABEL_21;
                }
            }
        }
        else
        {
            v6 = 2;
            if (((r_u32(0x800FF2F0u) & 1) != 0))
                v6 = 1;
            if (host_position)
                xport_draft_host_sub_80069EF4_p2(v6, host_position, 0);
            else
                sub_80069EF4(v6, a1, 0);
            v7 = ((uint32)(sub_80032DC0(160)));
            if (v7)
            {
                if (host_position)
                    apocalypse_explosion_debris_native(v7, host_position, 10, 100, 70, 10, 3000, 10, 0, 0, 1, 0, 10, 250, 14, 10, 128);
                else
                    sub_8001D484(v7, a1, 10, 100, 70, 10, 3000, 10, 0, 0, 1, 0, 10, 250, 14, 10, 128);
            }
            result = (v5 < 2000);
            if ((v5 < 4000))
            {
                v8 = a1;
                if ((v5 >= 2000))
                {
                    v9 = r_u32(0x800FF904u);
                    v10 = 2;
                    return apocalypse_explosion_camera_native(v9, v10);
                }
            LABEL_21:
                v9 = r_u32(0x800FF904u);

                v8 = a1;
                v10 = 1;
                return apocalypse_explosion_camera_native(v9, v10);
            }
        }
    }
    return result;
}

uint32 sub_8001E1B8(uint32 a1, uint32 a2)
{
    return explosion_proximity(a1, NULL, a2);
}

uint32 apocalypse_explosion_proximity_native(const void *position, uint32 kind)
{
    return explosion_proximity(0u, position, kind);
}

static uint32 explosion_debris(uint32 a1, uint32 a2, const void *host_position, uint32 a3, uint32 a4, uint32 a5, uint32 a6, uint32 a7, uint32 a8, uint32 a9, uint32 a10, uint32 a11, uint32 a12, uint32 a13, uint32 a14, uint32 a15, uint32 a16, uint32 a17)
{
    sint8 v21;
    sint32 v22;
    sint32 v24;
    sint32 v25;
    sint32 v26;
    uint32 v27;
    sint32 v28;
    sint32 v29;
    sint32 v30;
    sint32 v31;
    sint32 v32;
    sint32 v33;
    sint32 v34;
    short v35;
    short v36;
    sint32 v37;
    uint32 v38;
    sint32 v39;
    sint32 v40;
    sint32 v41;
    sint32 v42;
    sint32 v43;
    sint32 v44;
    sint32 v45;
    uint32 native_angles[2];
    uint32 origin[3], target[3];
    uint32 v49[8];
    sint32 position[3];
    sint32 temporary[3];
    char v56[16];
    sint32 v57;
    sint32 v58;
    sint32 v59;
    sint32 v60;
    uint32 v61;
    sint32 v62;
    sint32 v63;
    v21 = a11;
    v22 = a16;
    v24 = 0;
    sub_800330F4(a1);
    w_u32((a1 + (17) * 4u), 0x800A1184u);
    if (host_position)
    {
        memcpy(&target[0], host_position, 4u);
        memcpy(&v25, (const uint8 *)host_position + 4u, 4u);
        memcpy(&v26, (const uint8 *)host_position + 8u, 4u);
    }
    else
    {
        target[0] = r_u32(a2);
        v25 = r_u32(a2 + 4u);
        v26 = r_u32(a2 + 8u);
    }
    w_u32((a1 + (6) * 4u), target[0]);
    w_u32((a1 + (7) * 4u), v25);
    w_u32((a1 + (8) * 4u), v26);
    origin[0] = r_u32(0x800ED520u) << 12;
    origin[1] = 0;
    origin[2] = r_u32(0x800ED528u) << 12;
    if (host_position)
    {
        memcpy(&target[0], host_position, 4u);
        memcpy(&target[2], (const uint8 *)host_position + 8u, 4u);
    }
    else
    {
        target[0] = r_u32(a2);
        target[2] = r_u32(a2 + 8u);
    }
    target[1] = 0;
    xport_draft_host_sub_80066B8C_p123(native_angles, origin, target);
    native_angles[0] = (native_angles[0] & 65535u) | (((native_angles[0] >> 16) + 1024u) & 65535u) << 16;
    xport_draft_host_sub_800667CC_p13(v49, 1, native_angles);
    v27 = a1;
    v63 = (-4096 * a6);
    v28 = (0u - a7);
    v61 = (a1 + (6) * 4u);
    v62 = ((2 * a5) + 1);
    w_u32((a1 + (39) * 4u), v22);
    w_u32((a1 + (38) * 4u), a3);
    while (v24 < ((sint32)a3 / 2))
    {
        v57 = sub_80066570(a4);
        xport_draft_host_sub_8006C40C_p123(&temporary[0], v49, &v57);
        xport_draft_host_sub_8006C34C_p13(&position[0], v61, &temporary[0]);
        v29 = sub_80066570(v62);
        position[1] += ((v29 - a5) << 12);
        v30 = sub_80032DC0(120);
        if (v30)
            v30 = xport_draft_host_sub_80035478_p2(v30, position, 9, 1000, 1, 0, 0xFFFFFFFEu);
        w_u8(((uint32)((v30 + 66))), 1);
        w_u32((v27 + (18) * 4u), v30);
        xport_draft_host_sub_8006C40C_p123(&temporary[0], v49, &a9);
        v31 = temporary[1];
        v32 = temporary[2];
        w_u32(((uint32)((v30 + 36))), temporary[0]);
        w_u32(((uint32)((v30 + 40))), v31);
        w_u32(((uint32)((v30 + 44))), v32);
        w_u32(((uint32)((v30 + 40))), v63);
        xport_draft_host_sub_8006C40C_p123(&temporary[0], v49, &a10);
        v33 = temporary[1];
        v34 = temporary[2];
        w_u32(((uint32)((v30 + 48))), temporary[0]);
        w_u32(((uint32)((v30 + 52))), v33);
        w_u32(((uint32)((v30 + 56))), v34);
        w_u32(((uint32)((v30 + 52))), v28);
        w_u8(((uint32)((v30 + 60))), v21);
        w_u8(((uint32)((v30 + 62))), v21);
        w_u8(((uint32)((v30 + 61))), a8);
        v35 = sub_80066570(4096);
        v36 = a17;
        w_u16(((uint32)((v30 + 96))), v35);
        v37 = v36;
        w_u16(((uint32)((v30 + 98))), a12);
        w_u16(((uint32)((v30 + 100))), a13);
        w_u16(((uint32)((v30 + 72))), a15);
        w_u16(((uint32)((v30 + 74))), a14);
        sub_80033288(v30, v36);
        sub_80033240(v30, 8);
        v38 = (v27 + (1) * 4u);
        v58 = sub_80066570(a4);
        xport_draft_host_sub_8006C40C_p123(v56, v49, &v58);
        xport_draft_host_sub_8006C3AC_p13(&temporary[0], v61, v56);
        position[0] = temporary[0];
        position[1] = temporary[1];
        position[2] = temporary[2];
        v39 = sub_80066570(v62);
        position[1] += ((v39 - a5) << 12);
        v40 = sub_80032DC0(120);
        if (v40)
            v40 = xport_draft_host_sub_80035478_p2(v40, position, 9, 1000, 1, 0, 0xFFFFFFFEu);
        w_u8(((uint32)((v40 + 66))), 1);
        w_u32((v38 + (18) * 4u), v40);
        v27 = (v38 + (1) * 4u);
        ++v24;
        v59 = (0u - a9);
        xport_draft_host_sub_8006C40C_p123(&temporary[0], v49, &v59);
        v41 = a10;
        v42 = temporary[1];
        v43 = temporary[2];
        w_u32(((uint32)((v40 + 36))), temporary[0]);
        w_u32(((uint32)((v40 + 40))), v42);
        w_u32(((uint32)((v40 + 44))), v43);
        w_u32(((uint32)((v40 + 40))), v63);
        v60 = (sint32)(0u - (uint32)v41);
        xport_draft_host_sub_8006C40C_p123(&temporary[0], v49, &v60);
        v44 = temporary[1];
        v45 = temporary[2];
        w_u32(((uint32)((v40 + 48))), temporary[0]);
        w_u32(((uint32)((v40 + 52))), v44);
        w_u32(((uint32)((v40 + 56))), v45);
        w_u32(((uint32)((v40 + 52))), v28);
        w_u8(((uint32)((v40 + 60))), v21);
        w_u8(((uint32)((v40 + 62))), v21);
        w_u8(((uint32)((v40 + 61))), a8);
        w_u16(((uint32)((v40 + 96))), sub_80066570(4096));
        w_u16(((uint32)((v40 + 98))), (0u - a12));
        w_u16(((uint32)((v40 + 100))), (0u - a13));
        w_u16(((uint32)((v40 + 72))), a15);
        w_u16(((uint32)((v40 + 74))), a14);
        sub_80033288(v40, v37);
        sub_80033240(v40, 8);
    }

    return a1;
}

uint32 sub_8001D484(uint32 a1, uint32 a2, uint32 a3, uint32 a4, uint32 a5, uint32 a6, uint32 a7, uint32 a8, uint32 a9, uint32 a10, uint32 a11, uint32 a12, uint32 a13, uint32 a14, uint32 a15, uint32 a16, uint32 a17)
{
    return explosion_debris(a1, a2, NULL, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17);
}

uint32 apocalypse_explosion_debris_native(uint32 a1, const void *position, uint32 a3, uint32 a4, uint32 a5, uint32 a6, uint32 a7, uint32 a8, uint32 a9, uint32 a10, uint32 a11, uint32 a12, uint32 a13, uint32 a14, uint32 a15, uint32 a16, uint32 a17)
{
    return explosion_debris(a1, 0u, position, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17);
}

uint32 sub_800774BC(uint32 a1, uint32 a2)
{
    sint32 result;
    for (; a2; a2 = r_u32(((uint32)((a2 + 28)))))
    {
        result = (r_u16(((uint32)((a2 + 76)))) | 2);
        w_u16(((uint32)((a2 + 76))), result);
    }

    return result;
}

uint32 sub_800212C8(uint32 a1)
{
    sint32 v2;
    sint32 v3;
    sint32 v4;
    sint32 v5;
    sint32 result;
    v2 = (a1 + 24);
    sub_8006C0B8((a1 + 24), (a1 + 36));
    w_u32(((uint32)((a1 + 40))), (r_u32(((uint32)((a1 + 40)))) + (20500)));
    sub_8006C730((a1 + 152), (a1 + 192));
    v3 = r_u32(((uint32)((a1 + 200))));
    if (v3)
        sub_800369F8(v3, v2);
    sub_800344BC(a1);
    v4 = r_u16(((uint32)((a1 + 10))));
    v5 = (v4 == 0);
    result = (v4 - 1);
    if (v5)
        return sub_80032ED8(a1);
    w_u16(((uint32)((a1 + 10))), result);
    return result;
}

/* TODO Missing call adapter indirect */
static sint32 segment_divide(uint32 numerator, uint32 denominator);

uint32 sub_8004E624(uint32 object)
{
    uint32 index = 0u, result, table, radius, distance, argument;
    while (1)
    {
        result = (uint32)((sint32)index < (sint32)r_u32(0x800FF648u));
        if (!result)
            break;
        distance = sub_8006696C(object + 4u, 0x800A724Cu + 12u * index);
        radius = r_u32(0x800A727Cu + 4u * index);
        if ((sint32)distance < (sint32)radius)
        {
            argument = (uint32)segment_divide(r_u32(0x800A7288u + 4u * index) * (radius - distance), radius);
            table = r_u32(object + 68u);
            if (table != 0x800A32A4u)
            {
                /* TODO Review actual class extent before reading virtual slot52 */
                fprintf(stderr, "TODO 4E624 virtual52 class %08X object %08X\n", table, object);
                abort();
            }
            apocalypse_object_virtual52(r_u32(table + 52u), object + (uint32)(sint32)(short)r_u16(table + 48u), argument, 0x800A71CCu, 0u);
            result = (uint32)(sint32)(short)r_u16(object + 218u);
            if ((sint32)result <= 0)
                break;
        }
        ++index;
    }
    return result;
}

uint32 sub_8004F6C4(uint32 a1)
{
    sint32 result;
    uint32 v3;
    sint32 v4;
    sint32 v5;
    sint32 v6;
    sint32 v7;
    short v8;
    sint32 v9;
    sint32 v10;
    int v11[4];
    result = 128;
    if ((r_u16(((uint32)((a1 + 390)))) == 128))
    {
        v3 = (0x800F863Cu + ((sub_80066570(4096) & 0xFFF)) * 4u);
        v4 = (r_u16(((uint32)(v3))) * (sub_80066570(16) + 24));
        v5 = ((-32 - sub_80066570(16)) << 12);
        v6 = (r_u16((((uint32)(v3)) + (1) * 2u)) * (sub_80066570(16) + 24));
        v11[0] = v4;
        v11[1] = v5;
        v11[2] = v6;
        v7 = sub_800625AC(320);
        if (v7)
        {
            v8 = sub_80066570(r_u8(((uint32)((a1 + 615)))));
            xport_draft_host_sub_8005BBB0_p3(v7, a1 + 4u, v11, (uint16)(r_u8(a1 + 614u) + v8), r_u32(a1 + 8u) + ((uint32)(sint16)r_u16(a1 + 456u) << 12), 9, 24, r_u32(0x800FF62Cu));
        }
        v9 = (sub_80066570(2) == 0);
        result = 0x10000;
        if (!v9)
        {
            v10 = r_u32(((uint32)((a1 + 372))));
            if (((v10 & 0x10000) != 0))
            {
                if (((v10 & 0x1000) != 0))
                {
                    result = (v10 | 7);
                    if (((v10 & 1) == 0))
                        w_u32(((uint32)((a1 + 372))), result);
                    return result;
                }
                result = (v10 | 0x1300);
            }
            else
            {
                result = (v10 | 0x100C0);
            }
            w_u32(((uint32)((a1 + 372))), result);
        }
    }
    return result;
}

uint32 sub_80035638(uint32 a1, uint32 a2, uint32 a3, uint32 a4, uint32 a5, uint32 a6, uint32 a7)
{
    sint32 v15;
    sint32 v16;
    sint32 v17;
    sint32 v18;
    sint32 result;
    sub_80034D88(a1);
    w_u32(((uint32)((a1 + 68))), 0x800A1C60u);
    v15 = r_u32((a2 + (1) * 4u));
    v16 = r_u32((a2 + (2) * 4u));
    w_u32(((uint32)((a1 + 24))), r_u32(a2));
    w_u32(((uint32)((a1 + 28))), v15);
    w_u32(((uint32)((a1 + 32))), v16);
    v17 = r_u32((a3 + (1) * 4u));
    v18 = r_u32((a3 + (2) * 4u));
    w_u32(((uint32)((a1 + 36))), r_u32(a3));
    w_u32(((uint32)((a1 + 40))), v17);
    w_u32(((uint32)((a1 + 44))), v18);
    sub_80033354(a1, a4);
    sub_800332A4(a1);
    result = a1;
    w_u16(((uint32)((a1 + 94))), a5);
    w_u16(((uint32)((a1 + 74))), a6);
    w_u16(((uint32)((a1 + 72))), a7);
    return result;
}

uint32 sub_80033138(uint32 a1, uint32 a2)
{
    uint32 result;
    sint32 v5;
    w_u32((a1 + (17) * 4u), 0x800A1D80u);
    sub_80032E7C(a1, 0x800FF434u);
    result = sub_80032FB8(((sint32)(a1)), 0);
    if (((a2 & 1) != 0))
        return ((uint32)(sub_80032E30(((sint32)(a1)))));
    return result;
}

uint32 sub_80033590(uint32 a1, uint32 a2)
{
    uint32 v2;
    uint32 v3;
    sint32 result;
    sint32 v5;
    uint32 v6;
    unsigned char v7;
    sint32 v8;
    unsigned char v9;
    unsigned char v10;
    v2 = r_u32(((uint32)((a1 + 76))));
    v3 = (v2 >> 8);
    if (((v2 & 0xFFFFFF) != 0))
    {
        v5 = r_u8(((uint32)((a1 + 76))));
        v6 = ((v2 >> 16) & 65535u);
        if ((v5 >= r_u16(((uint32)((a1 + 72))))))
            v7 = (v5 - r_u8(((uint32)((a1 + 72)))));
        else
            v7 = 0;
        v8 = r_u16(((uint32)((a1 + 72))));
        if ((((unsigned char)(v3)) >= v8))
            v9 = (v3 - r_u8(((uint32)((a1 + 72)))));
        else
            v9 = 0;
        if ((((unsigned char)(v6)) >= v8))
            v10 = (v6 - r_u8(((uint32)((a1 + 72)))));
        else
            v10 = 0;
        result = 0;
        w_u32(((uint32)((a1 + 76))), ((((r_u32(((uint32)((a1 + 76)))) & 0xFF000000) | (v10 << 16)) | (v9 << 8)) | v7));
    }
    else
    {
        result = 1;
        if (a2)
        {
            sub_80032ED8(a1);
            return 1;
        }
    }
    return result;
}

uint32 sub_8006325C(uint32 a1, uint32 a2, uint32 a3)
{
    sint32 result;
    sint32 v4;
    sint32 v5;
    sint32 v6;
    if ((a2 != r_u8(((uint32)((a1 + 312))))))
        return 0;
    v4 = r_u16(((uint32)((a1 + 308))));
    v5 = r_u16(((uint32)((a1 + 310))));
    if ((v5 < v4))
    {
        if ((r_u8(((uint32)((a1 + 313)))) < 0))
        {
            result = 0;
            if ((a3 >= v5))
                return (v4 >= a3);
            return result;
        }
        return ((v5 >= a3) || (a3 >= v4));
    }
    if ((r_u8(((uint32)((a1 + 313)))) < 0))
        return ((v4 >= a3) || (a3 >= v5));
    v6 = (v5 < a3);
    if ((a3 < v4))
        return 0;
    result = 1;
    if (v6)
        return 0;
    return result;
}

uint32 sub_80035704(uint32 a1, uint32 a2)
{
    uint32 result;
    sint32 v5;
    w_u32((a1 + (17) * 4u), 0x800A1C60u);
    result = sub_80034DE8(a1, 0);
    if (((a2 & 1) != 0))
        return ((uint32)(sub_80032E30(((sint32)(a1)))));
    return result;
}

uint32 sub_800342B0(uint32 a1, uint32 a2)
{
    uint32 result;
    sint32 v5;
    w_u32((a1 + (17) * 4u), 0x800A1D20u);
    sub_80032E7C(a1, 0x800FF44Cu);
    result = sub_80032FB8(((sint32)(a1)), 0);
    if (((a2 & 1) != 0))
        return ((uint32)(sub_80032E30(((sint32)(a1)))));
    return result;
}

/* TODO Missing call adapter indirect */
uint32 sub_8001D918(uint32 object, uint32 reason)
{
    uint32 index = 0u, child, table;
    w_u32(object + 68u, 0x800A1184u);
    if ((sint32)r_u32(object + 152u) > 0)
    {
        do
        {
            child = r_u32(object + 72u + index * 4u);
            if (child)
            {
                table = r_u32(child + 68u);
                (void)apocalypse_object_cleanup(r_u32(table + 12u), child + (uint32)(sint32)(short)r_u16(table + 8u), 3u);
            }
            ++index;
        } while ((sint32)index < (sint32)r_u32(object + 152u));
    }
    sub_80033138(object, 0u);
    if (reason & 1u)
        return sub_80032E30(object);
    return reason & 1u;
}

/* TODO Postincrement memory expressions may require ordering refinement */
uint32 sub_8005C8C4(uint32 a1)
{
    sint32 result;
    sint32 v3;
    sint32 v4;
    sint32 v5;
    sint32 v6;
    sint32 v7;
    sint32 v8;
    sint32 v9;
    sint32 v10;
    (w_u32(0x800FF5ACu, (r_u32(0x800FF5ACu) + 1u)), (r_u32(0x800FF5ACu) + 1u));
    (w_u32(0x800FF30Cu, (r_u32(0x800FF30Cu) + 1u)), (r_u32(0x800FF30Cu) + 1u));
    result = ((sub_80066570(2) + 3) < r_u32(0x800FF5ACu));
    if (result)
    {
        result = (((uint32)((r_u32(0x800FF64Cu) - r_u32(0x800FF2CCu)))) < 0x384);
        v3 = 1;
        if ((((uint32)((r_u32(0x800FF64Cu) - r_u32(0x800FF2CCu)))) >= 0x384))
        {
            w_u32(0x800FF5ACu, 0);
            while (1)
            {
                if (!v3)
                    return sub_8002FC64(r_u32(0x800FF5B8u), r_u32(0x800FF5BCu), (a1 + 4), 0);
                if ((r_u32(0x800FF378u) == 1))
                {
                    v5 = sub_80066570(4);
                    v4 = 6;
                    if (v5)
                    {
                        w_u32(0x800FF5B8u, 3);
                        goto LABEL_25;
                    }
                LABEL_23:
                    v4 = 8;

                    goto LABEL_24;
                }
                if ((r_u32(0x800FF378u) >= 2))
                {
                    if ((r_u32(0x800FF378u) == 2))
                    {
                        v6 = sub_80066570(4);
                        v4 = 4;
                        if (v6)
                        {
                            w_u32(0x800FF5B8u, 4);
                            goto LABEL_25;
                        }
                        v7 = sub_80066570(2);
                        v4 = 6;
                        if (v7)
                        {
                            w_u32(0x800FF5B8u, 3);
                            goto LABEL_25;
                        }
                        goto LABEL_23;
                    }
                }
                else
                {
                    v4 = 8;
                    if (!r_u32(0x800FF378u))
                        goto LABEL_24;
                }
                if (sub_80066570(4))
                {
                    w_u32(0x800FF5B8u, 5);
                    v4 = 7;
                    goto LABEL_25;
                }
                v8 = sub_80066570(3);
                v4 = 4;
                if (!v8)
                {
                    w_u32(0x800FF5B8u, 4);
                    goto LABEL_25;
                }
                v9 = sub_80066570(3);
                v4 = 8;
                if (!v9)
                {
                    w_u32(0x800FF5B8u, 3);
                    v4 = 6;
                    goto LABEL_25;
                }
            LABEL_24:
                w_u32(0x800FF5B8u, 2);

            LABEL_25:
                w_u32(0x800FF5BCu, sub_80066570(v4));

                v10 = ((8 * r_u32(0x800FF5B8u)) + r_u32(0x800FF5BCu));
                if ((((((r_u32((0x800A71B8u + (0) * 4u)) != v10) && (r_u32((0x800A71B8u + (1) * 4u)) != v10)) && (r_u32((0x800A71B8u + (2) * 4u)) != v10)) && (r_u32((0x800A71B8u + (3) * 4u)) != v10)) && (r_u32((0x800A71B8u + (4) * 4u)) != v10)))
                {
                    v3 = 0;
                    w_u32((0x800A71B8u + (0) * 4u), r_u32((0x800A71B8u + (1) * 4u)));
                    w_u32((0x800A71B8u + (1) * 4u), r_u32((0x800A71B8u + (2) * 4u)));
                    w_u32((0x800A71B8u + (2) * 4u), r_u32((0x800A71B8u + (3) * 4u)));
                    w_u32((0x800A71B8u + (3) * 4u), r_u32((0x800A71B8u + (4) * 4u)));
                    w_u32((0x800A71B8u + (4) * 4u), ((8 * r_u32(0x800FF5B8u)) + r_u32(0x800FF5BCu)));
                }
            }
        }
    }
    return result;
}

uint32 sub_8005BBB0(uint32 a1, uint32 a2, uint32 a3, uint32 a4, uint32 a5, uint32 a6, uint32 a7, uint32 a8)
{
    short v16;
    sint32 v17;
    sint32 v18;
    sint32 v19;
    sint32 v20;
    short v21;
    short v22;
    short v23;
    sint32 result;
    sub_80062924(a1);
    w_u32(((uint32)((a1 + 68))), 0x800A27D8u);
    sub_800626C8(a1, a8);
    w_u16(((uint32)((a1 + 22))), a4);
    sub_80062A38(a1, 0x800FF4E8u);
    v16 = r_u16(((uint32)((a1 + 78))));
    w_u16(((uint32)((a1 + 212))), 0);
    w_u16(((uint32)((a1 + 78))), (v16 & 0xFFEF));
    v17 = r_u32((a2 + (1) * 4u));
    v18 = r_u32((a2 + (2) * 4u));
    w_u32(((uint32)((a1 + 4))), r_u32(a2));
    w_u32(((uint32)((a1 + 8))), v17);
    w_u32(((uint32)((a1 + 12))), v18);
    v19 = r_u32((a3 + (1) * 4u));
    v20 = r_u32((a3 + (2) * 4u));
    w_u32(((uint32)((a1 + 104))), r_u32(a3));
    w_u32(((uint32)((a1 + 108))), v19);
    w_u32(((uint32)((a1 + 112))), v20);
    if (((a6 & 0x20) == 0))
    {
        v21 = (sub_80066570(256) - 128);
        v23 = (sub_80066570(256) - 128);
        v22 = sub_80066570(256);
        w_u16(((uint32)((a1 + 132))), v21);
        w_u16(((uint32)((a1 + 134))), v23);
        w_u16(((uint32)((a1 + 136))), (v22 - 128));
    }
    result = a1;
    w_u16(((uint32)((a1 + 298))), a7);
    w_u16(((uint32)((a1 + 300))), a6);
    w_u32(((uint32)((a1 + 304))), a5);
    w_u8(((uint32)((a1 + 296))), 3);
    return result;
}

uint32 sub_8004BD94(uint32 a1, uint32 a2)
{
    sint32 result;
    result = sub_8006BC20(r_u32(((uint32)((a1 + 448)))));
    w_u32(((uint32)((a1 + 448))), 0);
    w_u8(((uint32)((a1 + 452))), 0);
    w_u8(((uint32)((a1 + 453))), 0);
    return result;
}

uint32 sub_8004BDFC(uint32 a1, uint32 a2, uint32 a3)
{
    return r_u16(((uint32)((((2 * a3) + ((2 * a2) * r_u8(((uint32)((a1 + 453)))))) + r_u32(((uint32)((a1 + 448))))))));
}

uint32 sub_8001C158(uint32 a1, uint32 a2, uint32 a3, uint32 a4, uint32 a5, uint32 a6, uint32 a7, uint32 a8, uint32 a9, uint32 a10, uint32 a11, uint32 a12, uint32 a13)
{
    a8 &= 255u;
    a9 &= 255u;
    a10 &= 255u;
    a11 &= 255u;
    a12 &= 255u;
    a13 &= 255u;
    sint32 v18;
    sint32 v19;
    sint32 v20;
    sint32 v21;
    sint32 i;
    sint32 result;
    sub_800330F4(a1);
    w_u32(((uint32)((a1 + 68))), 0x800A109Cu);
    w_u8(((uint32)((a1 + 67))), 1);
    w_u8(((uint32)((a1 + 104))), ((a8 + a11) >> 1));
    w_u8(((uint32)((a1 + 105))), ((a9 + a12) >> 1));
    w_u8(((uint32)((a1 + 106))), ((a10 + a13) >> 1));
    v18 = sub_80032DC0(124);
    if (v18)
        sub_8001CF9C(v18, a2, 4u, r_u8(a1 + 104u), r_u8(a1 + 105u), r_u8(a1 + 106u), 2u, r_u8(a1 + 104u), r_u8(a1 + 105u), r_u8(a1 + 106u), 20u, 2u * a7, 0u, 0u, 2u * a7, 0u, 2u * a7, 0u, 0u, 0u);
    v19 = r_u32((a2 + (1) * 4u));
    v20 = r_u32((a2 + (2) * 4u));
    w_u32(((uint32)((a1 + 24))), r_u32(a2));
    w_u32(((uint32)((a1 + 28))), v19);
    w_u32(((uint32)((a1 + 32))), v20);
    sub_800667CC((a1 + 36), a6, a3);
    w_u32(((uint32)((a1 + 72))), a4);
    w_u32(((uint32)((a1 + 84))), a7);
    w_u32(((uint32)((a1 + 80))), a5);
    w_u32(((uint32)((a1 + 88))), 370);
    v21 = sub_80032DC0(104);
    if (v21)
        v21 = sub_80034598(v21, 2, 1);
    w_u32(((uint32)((a1 + 76))), v21);
    sub_80034A18(r_u32(a1 + 76u), a8, a9, a10);
    sub_80034A44(r_u32(((uint32)((a1 + 76)))), a11, a12, a13);
    sub_80034A9C(r_u32(a1 + 76u), 0u, 0u, 0u, 0u);
    sub_80034918(r_u32(((uint32)((a1 + 76)))), a7);
    sub_80034958(r_u32(((uint32)((a1 + 76)))), a7);
    sub_800349D0(r_u32(((uint32)((a1 + 76)))), 0, ((42 * a7) / 16));
    sub_80032EE4(r_u32(((uint32)((a1 + 76)))), (a1 + 24));
    w_u8(((uint32)((r_u32(((uint32)((a1 + 76)))) + 66))), 1);
    w_u16(((uint32)((a1 + 10))), (r_u16(((uint32)((a1 + 80)))) + 26));
    w_u32(0x800ED638u, r_u32(((uint32)((a1 + 24)))));
    w_u32(0x800ED63Cu, r_u32(((uint32)((a1 + 28)))));
    w_u32(0x800ED640u, r_u32(((uint32)((a1 + 32)))));
    w_u32(0x800ED644u, (r_u32(((uint32)((a1 + 24)))) + (r_u16(((uint32)((a1 + 10)))) * r_u32(((uint32)((a1 + 36)))))));
    w_u32(0x800ED648u, (r_u32(((uint32)((a1 + 28)))) + (r_u16(((uint32)((a1 + 10)))) * r_u32(((uint32)((a1 + 40)))))));
    w_u32(0x800ED64Cu, (r_u32(((uint32)((a1 + 32)))) + (r_u16(((uint32)((a1 + 10)))) * r_u32(((uint32)((a1 + 44)))))));
    sub_8007BB24(0x800ED638u);
    w_u32(0x800FF974u, 1);
    sub_8007DD04(0x800ED638u, 0);
    w_u32(0x800FF974u, 0);
    if (r_u32(0x800ED6A0u))
    {
        for (i = r_u32(0x800FF794u); i; i = r_u32(((uint32)((i + 28)))))
        {
            if ((i == r_u32(0x800ED6A0u)))
                break;
        }

        w_u32(((uint32)((a1 + 96))), r_u32(0x800ED6A0u));
        w_u32(((uint32)((a1 + 100))), r_u32(0x800ED6B8u));
        w_u16(((uint32)((a1 + 10))), (r_u32(0x800ED678u) / a6));
    }
    result = a1;
    if ((r_u16(((uint32)((a1 + 10)))) >= 0x97u))
    {
        w_u16(((uint32)((a1 + 10))), 150);
        return a1;
    }
    return result;
}

uint32 sub_80034918(uint32 a1, uint32 a2)
{
    uint32 v2;
    sint32 result;
    v2 = 1;
    for (result = (r_u32(((uint32)((a1 + 80)))) > 1u); (v2 < r_u32(((uint32)((a1 + 80))))); result = (v2 < r_u32(((uint32)((a1 + 80))))))
    {
        w_u32(((uint32)(((8 * v2) + r_u32(((uint32)((a1 + 72))))))), a2);
        v2 += 2;
    }

    return result;
}

uint32 sub_80034958(uint32 a1, uint32 a2)
{
    sint32 result;
    uint32 v3;
    result = r_u32(((uint32)((a1 + 80))));
    v3 = 0;
    if (result)
    {
        do
        {
            w_u32(((uint32)(((8 * v3) + r_u32(((uint32)((a1 + 72))))))), a2);
            v3 += 2;
            result = (v3 < r_u32(((uint32)((a1 + 80)))));
        } while ((v3 < r_u32(((uint32)((a1 + 80))))));
    }
    return result;
}

uint32 apocalypse_visibility_native(const void *start, uint32 end)
{
    uint32 collision[36], value;
    memcpy(&value, start, sizeof(value));
    collision[0] = value;
    memcpy(&value, (const uint8 *)start + 4u, sizeof(value));
    collision[1] = value;
    memcpy(&value, (const uint8 *)start + 8u, sizeof(value));
    collision[2] = value;
    collision[3] = r_u32(end);
    collision[4] = r_u32(end + 4u);
    collision[5] = r_u32(end + 8u);
    xport_draft_host_sub_8007BB24_p1(collision);
    xport_draft_host_sub_8007DD04_p1(collision, 1u);
    return collision[26] == 0u;
}

uint32 sub_800679A4(uint32 start, uint32 end)
{
    return apocalypse_visibility_native(psx_addr(start, 12u), end);
}

uint32 sub_8002BBF0(uint32 a1, uint32 a2)
{
    sint32 result;
    w_u32(((uint32)((a1 + 68))), 0x800A1518u);
    result = sub_80032128(a1, 0u);
    if (((a2 & 1) != 0))
        return sub_80032E30(a1);
    return result;
}

uint32 sub_80032128(uint32 a1, uint32 a2)
{
    sint8 v3;
    sint32 v4;
    sint32 result;
    v3 = a2;
    v4 = r_u32(((uint32)((a1 + 80))));
    w_u32(((uint32)((a1 + 68))), 0x800A1B50u);
    sub_8006BC20(v4);
    sub_80032E7C(a1, 0x800FF45Cu);
    result = sub_80032FB8(a1, 0);
    if (((v3 & 1) != 0))
        return sub_80032E30(a1);
    return result;
}

void nullsub_20(void)
{
    ;
}

uint32 sub_80034B7C(uint32 a1, uint32 a2)
{
    return sub_80032D3C(((uint32)((a1 + 88))), a2);
}

uint32 sub_8001BE78(uint32 a1, uint32 a2, uint32 a3, uint32 a4, uint32 a5, uint32 a6)
{
    sint32 result;
    a1 &= 255u;
    a2 &= 255u;
    a3 &= 255u;
    a5 &= 255u;
    result = (a5 < ((uint32)(((unsigned char)(r_u8(0x800FF1DCu))))));
    if ((a5 >= ((uint32)(((unsigned char)(r_u8(0x800FF1DCu)))))))
    {
        if (a4)
        {
            w_u32(0x800FFB68u, (a3 << 16));
            result = a6;
            w_u32(0x800FFB60u, (a1 << 16));
            w_u32(0x800FFB64u, (a2 << 16));
            w_u32(0x800FF1E4u, a4);
            w_u8(0x800FF1DCu, a5);
            w_u32(0x800FF1E0u, a6);
            w_u32(0x800FFB6Cu, ((a1 << 16) / a4));
            w_u32(0x800FFB70u, (r_u32(0x800FFB64u) / a4));
            w_u32(0x800FFB74u, ((a3 << 16) / a4));
        }
    }
    return result;
}

/* TODO Missing call adapter abs32 */
/* TODO Missing call adapter sub_80087A3C */
uint32 sub_8004BA64(uint32 a1, uint32 a2, uint32 a3, uint32 a4, uint32 a5, uint32 a6, uint32 a7, uint32 a8, uint32 a9)
{
    sint16 angle;
    uint32 table, radius, delta, sign, absolute;
    sint32 height;
    uint32 collision[35];
    uint32 pass;
    angle = (sint16)(1024u - (uint32)ratan2((sint32)(r_u32(a1 + 12u) - r_u32(a3 + 8u)), (sint32)(r_u32(a1 + 4u) - r_u32(a3))));
    table = 0x800F863Cu + 4u * (((uint32)(sint32)angle + (uint16)sub_80066570((uint32)(sint32)(sint16)a7) - (uint32)((sint32)(sint16)a7 >> 1)) & 4095u);
    radius = a4 + sub_80066570(a5 - a4);
    w_u32(a2, r_u32(a3) + radius * (uint32)(sint32)(sint16)r_u16(table));
    w_u32(a2 + 4u, r_u32(a3 + 4u));
    w_u32(a2 + 8u, r_u32(a3 + 8u) + radius * (uint32)(sint32)(sint16)r_u16(table + 2u));
    if (a8 != 0u)
    {
        height = (sint32)sub_80067A18(a2, 512u, 512u);
        if (height == -1)
            return 0u;
        w_u32(a2 + 4u, (uint32)height - ((uint32)(sint32)(sint16)r_u16(a1 + 456u) << 12));
    }
    if (a9 != 0u)
    {
        for (pass = 0u; pass < 2u; ++pass)
        {
            uint32 offset = pass == 0u ? (uint32)(sint32)(sint16)r_u16(a1 + 458u) << 12 : 0u;
            collision[0] = r_u32(a1 + 4u);
            collision[1] = r_u32(a1 + 8u) - offset;
            collision[2] = r_u32(a1 + 12u);
            collision[3] = r_u32(a2);
            collision[4] = r_u32(a2 + 4u) - offset;
            collision[5] = r_u32(a2 + 8u);
            xport_draft_host_sub_8007BB24_p1(collision);
            ((uint8 *)collision)[136] = 0u;
            w_u32(0x800FF970u, 1u);
            xport_draft_host_sub_8007DD04_p1(collision, 1u);
            w_u32(0x800FF970u, 0u);
            if (collision[26] != 0u)
                return 0u;
        }
    }
    delta = r_u32(a1 + 8u) - r_u32(a2 + 4u);
    sign = (uint32)((sint32)delta >> 31);
    absolute = (delta ^ sign) - sign;
    return (uint32)(((sint32)absolute >> 12) < (sint32)a6);
}

/* TODO Missing call adapter sub_80019318 */
/* TODO The existing 19318 adapter remains a named missing dependency */
uint32 sub_80019318(uint32 record, uint32 position, uint32 output);

uint32 sub_8005EEDC(uint32 object)
{
    uint32 record = r_u32(object + 512u);
    uint32 x, y, z, kind;
    if (!record || r_u32(0x800FF5A4u))
        return 0u;
    if (!sub_80019318(record, object + 4u, object + 496u))
        return 0u;
    x = r_u32(object + 496u);
    y = r_u32(object + 500u);
    z = r_u32(object + 504u);
    w_u32(object + 4u, x);
    w_u32(object + 8u, y);
    w_u32(object + 12u, z);
    record = r_u32(object + 512u);
    w_u32(object + 8u, r_u32(object + 8u) + 0x80000u);
    kind = r_u16(record + 58u);
    if (kind != 400u && kind != 401u)
        return 0u;
    w_u32(object + 460u, kind == 400u ? 0x8000u : 0x4000u);
    w_u32(object + 108u, 0u);
    record = r_u32(object + 512u);
    w_u16(object + 18u, (r_u16(record + 320u) + 2048u) & 0xFFFu);
    w_u8(r_u32(object + 444u) + 273u, 0u);
    w_u32(object + 508u, 1u);
    w_u32(0x800FF5A4u, 1u);
    if (kind == 400u)
        w_u32(object + 492u, sub_80069DF0(100u, 0x2000u, 0u));
    return 1u;
}

uint32 sub_80028FB8(uint32 a1, uint32 a2, uint32 a3)
{
    sint32 v6;
    sint32 v7;
    short v8;
    sub_8003304C(a1);
    w_u32((a1 + (17) * 4u), 0x800A1398u);
    w_u32((a1 + (20) * 4u), sub_8006B864((140 * a2), 0, 1));
    w_u32((a1 + (19) * 4u), a2);
    w_u32((a1 + (18) * 4u), sub_8006E278(a3));
    v6 = 0;
    if ((((sint32)(r_u32((a1 + (19) * 4u)))) > 0))
    {
        v7 = (r_u32((a1 + (20) * 4u)) + 26);
        do
        {
            w_u8(((uint32)((v7 + 29))), 12);
            w_u8(((uint32)((v7 + 33))), 62);
            w_u16(((uint32)((v7 + 40))), r_u16(((uint32)((r_u32((a1 + (18) * 4u)) + 2)))));
            v8 = r_u16(((uint32)((r_u32((a1 + (18) * 4u)) + 6))));
            w_u8(((uint32)((v7 - 23))), 12);
            w_u8(((uint32)((v7 - 19))), 62);
            w_u16(((uint32)((v7 + 52))), v8);
            w_u16(((uint32)((v7 - 12))), r_u16(((uint32)((r_u32((a1 + (18) * 4u)) + 2)))));
            ++v6;
            w_u16(((uint32)(v7)), r_u16(((uint32)((r_u32((a1 + (18) * 4u)) + 6)))));
            v7 += 140;
        } while ((v6 < r_u32((a1 + (19) * 4u))));
    }
    sub_800293D8(a1, 128, 128, 128);
    sub_80029324(a1, 0, 0, 2);
    w_u32((a1 + (27) * 4u), -1);
    return a1;
}

uint32 sub_80029324(uint32 a1, uint32 a2, uint32 a3, uint32 a4)
{
    uint32 v5;
    sint8 v6;
    sint8 v7;
    sint32 result;
    sint32 v9;
    sint32 v10;
    sint8 v11;
    sint8 v12;
    uint32 v13;
    sint8 v14;
    v5 = ((uint32)(r_u32((a1 + (18) * 4u))));
    w_u32((a1 + (25) * 4u), a2);
    w_u32((a1 + (26) * 4u), a3);
    v6 = (r_u8(v5) + (a2 & 0x3F));
    v7 = (r_u8((v5 + (1) * 1u)) + (a3 & 0x3F));
    result = r_u32((a1 + (19) * 4u));
    v9 = ((a4 << 6) / result);
    v10 = 0;
    if ((result > 0))
    {
        v11 = (v7 + 32);
        v12 = (v7 + 64);
        v13 = ((uint32)((r_u32((a1 + (20) * 4u)) + 101)));
        do
        {
            v14 = (v6 + v9);
            w_u8((v13 - (89) * 1u), v6);
            w_u8((v13 - (65) * 1u), v6);
            w_u8((v13 - (37) * 1u), v6);
            w_u8((v13 - (13) * 1u), v6);
            v6 = ((v6 + v9) & 0x3F);
            w_u8((v13 - (77) * 1u), v14);
            w_u8((v13 - (88) * 1u), v7);
            w_u8((v13 - (76) * 1u), v7);
            w_u8((v13 - (53) * 1u), v14);
            w_u8((v13 - (64) * 1u), v11);
            w_u8((v13 - (52) * 1u), v11);
            w_u8((v13 - (25) * 1u), v14);
            w_u8((v13 - (36) * 1u), v11);
            w_u8((v13 - (24) * 1u), v11);
            w_u8((v13 - (1) * 1u), v14);
            w_u8((v13 - (12) * 1u), v12);
            w_u8(v13, v12);
            result = (++v10 < r_u32((a1 + (19) * 4u)));
            v13 += (140) * 1u;
        } while ((v10 < r_u32((a1 + (19) * 4u))));
    }
    return result;
}

uint32 sub_80029F58(uint32 a1, uint32 a2)
{
    sint32 result;
    for (; a2; a2 = r_u32(((uint32)((a2 + 28)))))
    {
        result = (r_u16(((uint32)((a2 + 78)))) & 0xFEFF);
        w_u16(((uint32)((a2 + 78))), result);
    }

    return result;
}

/* TODO Missing call adapter indirect */
/* TODO Resolve host-vector normalization and class-qualified slot-52 dispatch */
static void area_damage_normalize(sint32 vector[3], sint32 *length)
{
    abort();
}

static void area_damage_virtual52(uint32 object, uint32 damage, const sint32 vector[3], uint32 reason)
{
    abort();
}

static sint32 area_damage_divide(uint32 numerator, uint32 denominator)
{
    if (!denominator)
        return (sint32)numerator < 0 ? 1 : -1;
    return (sint32)((long long)(sint32)numerator / (long long)(sint32)denominator);
}

/* TODO Resolve host-vector normalization and class-qualified slot-52 dispatch */

void sub_80029F88(uint32 source, uint32 object, uint32 effects)
{
    while (object)
    {
        uint32 flags = r_u16(object + 78u);
        if ((flags & 0x10u) && r_u16(object + 58u) != 206u && !(flags & 0x100u))
        {
            sint32 length = (sint32)sub_8006696C(source + 24u, object + 4u);
            if ((sint32)r_u32(source + 168u) >= length)
            {
                sint32 vector[3];
                uint32 range, radius, damage;
                if (effects && !r_u32(0x800FF738u))
                    sub_8001D320(object + 4u, 70u, 240u, 200u, 0u, 5u, 0u, 100u);
                xport_draft_host_sub_8006C3AC_p1(vector, object + 4u, source + 24u);
                if (length)
                    area_damage_normalize(vector, &length);
                else
                    vector[0] = vector[1] = vector[2] = 0;
                radius = r_u32(source + 168u);
                range = r_u32(source + 100u);
                if ((sint32)range >= (sint32)radius)
                    damage = r_u32(source + 92u) - (uint32)area_damage_divide((r_u32(source + 92u) - r_u32(source + 96u)) * radius, range);
                else
                    damage = r_u32(source + 96u);
                area_damage_virtual52(object, damage, vector, 30u);
                w_u16(object + 78u, r_u16(object + 78u) | 0x100u);
            }
        }
        object = r_u32(object + 28u);
    }
}

uint32 sub_8004FEC4(uint32 a1)
{
    sint32 result;
    result = r_u16(((uint32)((a1 + 390))));
    w_u8(((uint32)((a1 + 380))), 0);
    w_u16(((uint32)((a1 + 472))), 14);
    w_u16(((uint32)((a1 + 474))), 0);
    switch (result)
    {
        case 1:
            result = (r_u32(((uint32)((a1 + 372)))) | 0x1000);
            w_u32(((uint32)((a1 + 372))), result);
            break;

        case 2:
            result = (r_u32(((uint32)((a1 + 372)))) | 0x8000);
            w_u32(((uint32)((a1 + 372))), result);
            break;

        case 4:
            result = (r_u32(((uint32)((a1 + 372)))) | 4);
            w_u32(((uint32)((a1 + 372))), result);
            break;

        case 8:
            result = (r_u32(((uint32)((a1 + 372)))) | 0x10000);
            w_u32(((uint32)((a1 + 372))), result);
            break;

        case 16:
            result = (r_u32(((uint32)((a1 + 372)))) | 0x2000);
            w_u32(((uint32)((a1 + 372))), result);
            break;

        default:
            return result;
    }

    return result;
}

/* TODO Missing call adapter indirect */
/* TODO Bind actual selected object virtual slot60 targets */
static void collision_attach_virtual60(uint32 target, uint32 receiver)
{
    fprintf(stderr, "TODO 62B7C virtual60 target %08X receiver %08X\n", target, receiver);
    abort();
}

uint32 sub_80062B7C(uint32 object, uint32 list)
{
    uint32 table = r_u32(object + 68u), result;
    if (table != 0x800A32A4u)
    {
        /* TODO Review actual class extent before reading virtual slot60 */
        fprintf(stderr, "TODO 62B7C virtual60 class %08X object %08X\n", table, object);
        abort();
    }
    collision_attach_virtual60(r_u32(table + 60u), object + (uint32)(sint32)(short)r_u16(table + 56u));
    w_u32(object + 72u, list);
    sub_80062A64(object, list);
    sub_80062A38(object, 0x800FF5E0u);
    result = r_u16(object + 78u) | 1u;
    w_u16(object + 78u, result);
    return result;
}

uint32 sub_80033090(uint32 a1, uint32 a2)
{
    uint32 result;
    sint32 v5;
    w_u32((a1 + (17) * 4u), 0x800A1D98u);
    sub_80032E7C(a1, 0x800FF458u);
    result = sub_80032FB8(((sint32)(a1)), 0);
    if (((a2 & 1) != 0))
        return ((uint32)(sub_80032E30(((sint32)(a1)))));
    return result;
}

uint32 sub_80034994(uint32 a1, uint32 a2)
{
    sint32 result;
    uint32 v3;
    result = r_u32(((uint32)((a1 + 80))));
    v3 = 0;
    if (result)
    {
        do
        {
            w_u32(((uint32)(((8 * v3++) + r_u32(((uint32)((a1 + 72))))))), a2);
            result = (v3 < r_u32(((uint32)((a1 + 80)))));
        } while ((v3 < r_u32(((uint32)((a1 + 80))))));
    }
    return result;
}

/* TODO Resolve original data label 0x800FF208u */
/* TODO Resolve original data label 0x800A119Cu */
uint32 sub_8001FD28(uint32 a1, uint32 a2, uint32 a3, uint32 a4, uint32 a5, uint32 a6)
{
    sint8 v14;
    sint32 v15;
    sint32 v16;
    sint32 v17;
    sint32 v18;
    sint32 v19;
    sint32 v20;
    sint32 v21;
    sint32 v22;
    short v23;
    short v24;
    short v25;
    sint32 result;
    sub_80062924(a1);
    w_u32(((uint32)((a1 + 68))), 0x800A11ACu);
    w_u16(((uint32)((a1 + 58))), a2);
    w_u16(((uint32)((a1 + 336))), a6);
    v14 = r_u8(0x800EC530u);
    w_u8(((uint32)((a1 + 306))), 1);
    w_u8(((uint32)((a1 + 309))), v14);
    v15 = r_u32((a3 + (1) * 4u));
    v16 = r_u32((a3 + (2) * 4u));
    w_u32(((uint32)((a1 + 316))), r_u32(a3));
    w_u32(((uint32)((a1 + 320))), v15);
    w_u32(((uint32)((a1 + 324))), v16);
    v17 = r_u32((a3 + (1) * 4u));
    v18 = r_u32((a3 + (2) * 4u));
    w_u32(((uint32)((a1 + 4))), r_u32(a3));
    w_u32(((uint32)((a1 + 8))), v17);
    w_u32(((uint32)((a1 + 12))), v18);
    if (((a5 & 8) != 0))
    {
        w_u32(((uint32)((a1 + 108))), -245760);
    }
    else
    {
        v19 = r_u32((a4 + (1) * 4u));
        v20 = r_u32((a4 + (2) * 4u));
        w_u32(((uint32)((a1 + 104))), r_u32(a4));
        w_u32(((uint32)((a1 + 108))), v19);
        w_u32(((uint32)((a1 + 112))), v20);
    }
    w_u8(((uint32)((a1 + 129))), 5);
    w_u8(((uint32)((a1 + 130))), 5);
    w_u8(((uint32)((a1 + 131))), 5);
    w_u16(((uint32)((a1 + 134))), 100);
    w_u32(((uint32)((a1 + 120))), 0x8000);
    w_u16(((uint32)((a1 + 334))), (sub_80066570(20) + 90));
    w_u16(((uint32)((a1 + 330))), 50);
    w_u16(((uint32)((a1 + 332))), 50);
    if (((a5 & 0x10) != 0))
    {
        w_u8(((uint32)((a1 + 307))), 1);
        w_u8(((uint32)((a1 + 308))), 1);
    }
    else if (((a5 & 4) != 0))
    {
        w_u8(((uint32)((a1 + 307))), 1);
        w_u32(((uint32)((a1 + 312))), sub_80067A18((a1 + 316), 0, 8000));
    }
    w_u16(((uint32)((a1 + 212))), 300);
    sub_80062D70(a1);
    w_u16(((uint32)((a1 + 204))), 300);
    sub_800626C8(a1, 0x800FF208u);
    switch (r_u16(((uint32)((a1 + 58)))))
    {
        case 1:

        case 2:

        case 3:

        case 7:
            w_u8(((uint32)((a1 + 305))), 0);
            break;

        case 4:
            w_u8(((uint32)((a1 + 305))), 1);
            v21 = ((unsigned char)(r_u8(0x800FF645u)));
            v22 = 890148492;
            goto LABEL_18;

        case 5:
            w_u8(((uint32)((a1 + 305))), 1);
            v21 = ((unsigned char)(r_u8(0x800FF645u)));
            v22 = -737471138;
            goto LABEL_18;

        case 6:
            w_u8(((uint32)((a1 + 305))), 1);
            v21 = ((unsigned char)(r_u8(0x800FF645u)));
            v22 = 1373924015;
            goto LABEL_18;

        case 0xA:
            w_u8(((uint32)((a1 + 305))), 1);
            v21 = ((unsigned char)(r_u8(0x800FF645u)));
            v22 = 1294753148;
            goto LABEL_18;

        case 0xE:
            v22 = 2121593812;
            v23 = r_u16(0x800EC656u);
            w_u8(((uint32)((a1 + 305))), 1);
            v21 = ((unsigned char)(r_u8(0x800FF645u)));
            w_u16(((uint32)((a1 + 134))), 100);
            w_u16(((uint32)((a1 + 218))), v23);
            goto LABEL_18;

        case 0xF:
            v22 = 2121593812;
            v24 = r_u16(0x800EC658u);
            w_u8(((uint32)((a1 + 305))), 1);
            v21 = ((unsigned char)(r_u8(0x800FF645u)));
            w_u16(((uint32)((a1 + 134))), 150);
            w_u16(((uint32)((a1 + 218))), v24);
            goto LABEL_18;

        case 0x10:
            v22 = 2121593812;
            v25 = r_u16(0x800EC65Au);
            w_u8(((uint32)((a1 + 305))), 1);
            v21 = ((unsigned char)(r_u8(0x800FF645u)));
            w_u16(((uint32)((a1 + 134))), 200);
            w_u16(((uint32)((a1 + 218))), v25);
        LABEL_18:
            w_u16(((uint32)((a1 + 22))), sub_8006E080(v22, v21));

            break;

        case 0x11:
            sub_800626C8(a1, 0x800A119Cu);
            w_u8(((uint32)((a1 + 305))), 1);
            w_u8(((uint32)((a1 + 309))), -106);
            break;

        case 0x13:

        case 0x14:
            w_u8(((uint32)((a1 + 305))), 1);
            w_u16(((uint32)((a1 + 22))), 0);
            break;

        default:
            break;
    }

    sub_80062A38(a1, 0x800FF204u);
    result = a1;
    if (!(r_u8(((uint32)((a1 + 305))))))
    {
        w_u16(((uint32)(a1)), (r_u16(((uint32)(a1))) | (1u)));
        return a1;
    }
    return result;
}

uint32 sub_8001EC58(uint32 a1, uint32 a2)
{
    sint32 result;
    w_u32(((uint32)((a1 + 68))), 0x800A1124u);
    result = sub_8001EA0C(a1, 0);
    if (((a2 & 1) != 0))
        return sub_80032E30(a1);
    return result;
}

uint32 sub_800261CC(uint32 a1, uint32 a2, uint32 a3, uint32 a4)
{
    sint32 result;
    sint32 v5;
    sint32 v6;
    result = r_u32(((uint32)((a1 + 76))));
    v5 = 0;
    if ((result > 0))
    {
        v6 = 0;
        do
        {
            w_u8(((uint32)(((v6 + r_u32(((uint32)((a1 + 80))))) + 12))), a2);
            w_u8(((uint32)(((v6 + r_u32(((uint32)((a1 + 80))))) + 13))), a3);
            ++v5;
            w_u8(((uint32)(((v6 + r_u32(((uint32)((a1 + 80))))) + 14))), a4);
            result = (v5 < r_u32(((uint32)((a1 + 76)))));
            v6 += 36;
        } while ((v5 < r_u32(((uint32)((a1 + 76))))));
    }
    return result;
}

static sint32 segment_divide(uint32 numerator, uint32 denominator);

uint32 sub_80026488(uint32 a1)
{
    uint32 v2;
    uint32 v3;
    sint32 v4;
    uint32 v5;
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
    uint32 v16;
    uint32 v17;
    uint32 v18;
    uint32 v19;
    uint32 v20;
    sint32 v21;
    uint32 v22;
    uint32 v23;
    sint32 v24;
    sint32 v25;
    unsigned long long v26;
    sint32 v27;
    sint32 v28;
    sint32 v29;
    uint32 v30;
    sint32 v31;
    sint32 v32;
    uint32 v33;
    uint32 v34;
    uint32 v35;
    uint32 v36;
    uint32 v37;
    sint32 v38;
    sint32 v39;
    sint32 v40;
    sint32 v41;
    sint32 v42;
    uint32 v43;
    sint32 v44;
    sint32 result;
    uint32 v46;
    sint32 v47;
    uint32 v48;
    sint32 v49;
    sint32 v50;
    sint32 v51;
    sint32 v52;
    sint32 v53;
    sint32 v54;
    v2 = 528482304;
    v3 = r_u32(((uint32)((a1 + 80))));
    v4 = 0;
    if (((sint32)r_u32(a1 + 76u) > 0))
    {
        v5 = 528482320;
        do
        {
            v6 = (sint32)sub_80025248((uint32)v3, (uint32)v2);
            v7 = (v6 < (sint32)r_u32(0x800FFAE8u));
            w_u32(((uint32)((v5 - 4))), v6);
            if ((v7 || ((sint32)r_u32(0x800FFAD8u) < v6)))
            {
                w_u32(((uint32)((v5 + 8))), 1);
            }
            else
            {
                w_u32(((uint32)((v5 + 8))), 0);
                w_u32(((uint32)(v5)), segment_divide(400u * r_u16((uint32)v3 + 16u), r_u32((uint32)v5 - 4u)));
            }
            v5 += 28;
            v2 += 28;
            ++v4;
            v3 += 36;
        } while ((v4 < (sint32)r_u32(a1 + 76u)));
    }
    apocalypse_segment_normal(0x1F800000u, &v50, &v51, 2u);
    v8 = 1;
    v9 = 528482340;
    w_u32(0x1F800004, v50);
    w_u32(0x1F800008, v51);
    v10 = 528482332;
    while ((v8 < (sint32)(r_u32(a1 + 76u) - 1u)))
    {
        apocalypse_segment_normal((uint32)v10, &v52, &v53, 2u);
        v10 += 28;
        ++v8;
        v11 = v52;
        v12 = v53;
        v13 = v51;
        w_u32(((uint32)((v9 - 4))), ((sint32)((uint32)v52 + (uint32)v50) / 2));
        w_u32(((uint32)(v9)), ((sint32)((uint32)v12 + (uint32)v13) / 2));
        v9 += 28;
        v50 = v11;
        v51 = v12;
    }

    v14 = v51;
    w_u32(((uint32)((v10 + 4))), v50);
    w_u32(((uint32)((v10 + 8))), v14);
    v15 = 1;
    v16 = 528482304;
    v17 = 528482348;
    v18 = 528482312;
    v19 = 528482332;
    v20 = r_u32(((uint32)((a1 + 80))));
    v21 = (sint32)(r_u32(0x1F800004u) * r_u32(0x1F800010u)) >> 6;
    v22 = ((uint32)((v20 + 48)));
    v23 = ((uint32)((v20 + 12)));
    v24 = (sint32)(r_u32(0x1F800008u) * r_u32(0x1F800010u)) >> 6;
    v54 = ((((uint32)(sint32)(short)r_u16(0x1F800002u) + v24) << 16) | ((unsigned short)(((uint32)(sint32)(short)r_u16(0x1F800000u) + v21))));
    v25 = ((((uint32)(sint32)(short)r_u16(0x1F800002u) - v24) << 16) | ((unsigned short)(((uint32)(sint32)(short)r_u16(0x1F800000u) - v21))));
    while ((v15 < (sint32)r_u32(a1 + 76u)))
    {
        v26 = ((unsigned long long)r_u32(((uint32)((v17 - 12)))) | ((unsigned long long)r_u32((((uint32)((v17 - 12)))) + 4u) << 32));
        if ((v26 && ((unsigned long long)r_u32(((uint32)((v18 - 4)))) | ((unsigned long long)r_u32((((uint32)((v18 - 4)))) + 4u) << 32))))
        {
            v27 = (sint32)((uint32)((unsigned long long)v26 >> 32) * r_u32((uint32)v17));
            v28 = (sint32)((uint32)v26 * r_u32((uint32)v17)) >> 6;
            v29 = (sint32)(short)r_u16((uint32)v19 + 2u);
            v30 = ((uint32)(((r_u32(0x800FF660u) + ((((unsigned short)(r_u32(((uint32)((v17 - 4)))))) - r_u16(((uint32)((a1 + 64))))) & 0x3FFC)) + 112)));
            v31 = ((((uint32)v29 + (uint32)(v27 >> 6)) << 16) | ((unsigned short)((r_u16(((uint32)(v19))) + v28))));
            v32 = ((((uint32)v29 - (uint32)(v27 >> 6)) << 16) | ((unsigned short)((r_u16(((uint32)(v19))) - v28))));
            v33 = r_u32(0x800FF668u);
            v34 = r_u32(0x800FF374u);
            v35 = (r_u32(0x800FF668u) + 36);
            v36 = (r_u32(0x800FF668u) + 80);
            v37 = (r_u32(0x800FF668u) + 72);
            if ((r_u32(0x800FF374u) < ((uint32)((r_u32(0x800FF668u) + 80)))))
                break;
            v38 = r_u32(0x800FF46Cu);
            w_u32(((uint32)((r_u32(0x800FF668u) + 72))), r_u32(0x800FF468u));
            w_u32(((uint32)((v37 + 4))), v38);
            v39 = r_u32(((uint32)((a1 + 72))));
            w_u32(0x800FF668u, v36);
            if ((v39 >= 3))
            {
                if ((v34 < (v33 + 116u)))
                    break;
                w_u32(0x800FF668u, (v33 + 116));
                w_u32(((uint32)((v33 + 80))), 0x8000000);
                w_u32(((uint32)((v36 + 4))), 973078528);
                w_u32(((uint32)((v36 + 12))), 0);
                w_u32(((uint32)((v36 + 28))), r_u32(v23));
                w_u32(((uint32)((v36 + 20))), r_u32(v22));
                w_u32(((uint32)((v36 + 16))), r_u32((v23 + (4) * 4u)));
                w_u32(((uint32)((v36 + 8))), r_u32((v22 + (4) * 4u)));
                w_u32(((uint32)((v36 + 32))), r_u32(((uint32)(v16))));
                w_u32(((uint32)((v36 + 24))), r_u32(((uint32)(v19))));
                w_u32(((uint32)((v33 + 80))), ((r_u32(((uint32)((v33 + 80)))) & 0xFF000000) | (r_u32(v30) & 0xFFFFFF)));
                w_u32(v30, ((r_u32(v30) & 0xFF000000) | (v36 & 0xFFFFFF)));
            }
            w_u32(((uint32)((v33 + 36))), 0x8000000);
            w_u32(((uint32)(v33)), 0x8000000);
            w_u32(((uint32)((v35 + 4))), 973078528);
            w_u32(((uint32)((v33 + 4))), 973078528);
            w_u32(((uint32)((v33 + 12))), 0);
            v40 = r_u32(((uint32)(v33)));
            w_u32(((uint32)((v33 + 28))), r_u32(v23));
            v41 = r_u32(v22);
            w_u32(((uint32)((v33 + 8))), v31);
            w_u32(((uint32)((v33 + 16))), v54);
            w_u32(((uint32)((v33 + 20))), v41);
            w_u32(((uint32)((v33 + 24))), r_u32(((uint32)(v19))));
            w_u32(((uint32)((v33 + 32))), r_u32(((uint32)(v16))));
            w_u32(((uint32)(v33)), ((v40 & 0xFF000000) | (r_u32(v30) & 0xFFFFFF)));
            w_u32(v30, ((r_u32(v30) & 0xFF000000) | (v33 & 0xFFFFFF)));
            w_u32(((uint32)((v35 + 12))), 0);
            w_u32(((uint32)((v35 + 28))), r_u32(v23));
            v42 = r_u32(v22);
            w_u32(((uint32)((v35 + 8))), v32);
            w_u32(((uint32)((v35 + 16))), v25);
            w_u32(((uint32)((v35 + 20))), v42);
            w_u32(((uint32)((v35 + 24))), r_u32(((uint32)(v19))));
            v25 = v32;
            w_u32(((uint32)((v35 + 32))), r_u32(((uint32)(v16))));
            w_u32(((uint32)((v33 + 36))), ((r_u32(((uint32)((v33 + 36)))) & 0xFF000000) | (r_u32(v30) & 0xFFFFFF)));
            v43 = ((r_u32(v30) & 0xFF000000) | (v35 & 0xFFFFFF));
            w_u32(v30, v43);
            v44 = r_u32(((uint32)((v33 + 72))));
            v54 = v31;
            w_u32(((uint32)((v33 + 72))), ((v44 & 0xFF000000) | (v43 & 0xFFFFFF)));
            w_u32(v30, ((r_u32(v30) & 0xFF000000) | (v37 & 0xFFFFFF)));
        }
        v18 += 28;
        v16 += 28;
        v17 += 28;
        v19 += 28;
        v23 += (9) * 4u;
        v22 += (9) * 4u;
        ++v15;
    }

    result = r_u32(((uint32)((a1 + 72))));
    v46 = 528482304;
    if (result)
    {
        v47 = 0;
        if (((sint32)r_u32(a1 + 76u) > 0))
        {
            v48 = ((uint32)((r_u32(((uint32)((a1 + 80)))) + 24)));
            do
            {
                if ((r_u32(((uint32)((a1 + 72)))) == 1))
                {
                    v49 = r_u32(((uint32)(v46)));
                    w_u32(v48, r_u32(((uint32)(v46))));
                    w_u32((v48 + (1) * 4u), v49);
                }
                else
                {
                    w_u32((v48 + (1) * 4u), r_u32(v48));
                    w_u32(v48, r_u32(((uint32)(v46))));
                }
                v48 += (9) * 4u;
                ++v47;
                v46 += 28;
            } while ((v47 < (sint32)r_u32(a1 + 76u)));
        }
        result = (r_u32(((uint32)((a1 + 72)))) + 1);
        w_u32(((uint32)((a1 + 72))), result);
    }
    return result;
}

/* Signed division preserves machine zero and overflow results */
static sint32 segment_divide(uint32 numerator, uint32 denominator)
{
    if (!denominator)
        return (sint32)numerator < 0 ? 1 : -1;
    return (sint32)(uint32)((long long)(sint32)numerator / (long long)(sint32)denominator);
}

uint32 apocalypse_segment_normal(uint32 vertices, sint32 *x, sint32 *y, uint32 minimum_length)
{
    sint32 horizontal, vertical, norm;
    uint32 result = r_u32(vertices + 24u);
    if (result)
        goto empty;
    result = r_u32(vertices + 52u);
    if (result)
        goto empty;
    horizontal = ((sint32)(short)r_u16(vertices + 28u) - (sint32)(short)r_u16(vertices)) * 320;
    vertical = (sint32)(short)r_u16(vertices + 30u) - (sint32)(short)r_u16(vertices + 2u);
    horizontal /= 512;
    *x = vertical;
    *y = -horizontal;
    if (horizontal < 0)
        horizontal = -horizontal;
    if (vertical < 0)
        vertical = -vertical;
    norm = vertical < horizontal ? horizontal + vertical / 2 : vertical + horizontal / 2;
    result = (uint32)(norm < (sint32)minimum_length);
    if (result)
        goto empty;
    *x = segment_divide((uint32)*x << 6, (uint32)norm);
    *y = segment_divide((uint32)*y << 6, (uint32)norm);
    result = (uint32)segment_divide((uint32)*x << 9, 320u);
    *x = (sint32)result;
    return result;
empty:
    *y = 0;
    *x = 0;
    return result;
}

uint32 sub_80026370(uint32 vertices, uint32 x, uint32 y, uint32 minimum_length)
{
    sint32 normal_x, normal_y;
    uint32 result = apocalypse_segment_normal(vertices, &normal_x, &normal_y, minimum_length);
    w_u32(x, (uint32)normal_x);
    w_u32(y, (uint32)normal_y);
    return result;
}

uint32 sub_80067C20(uint32 a1, uint32 a2)
{
    sint32 v4;
    sint32 v5;
    sint32 v6;
    sint32 v7;
    v4 = sub_80067ABC(a1, a2, ((uint32)(r_u32(0x800FF4E8u))), 1);
    v5 = (v4 + sub_80067ABC(a1, a2, ((uint32)(r_u32(0x800FF5DCu))), 1));
    v6 = (v5 + sub_80067ABC(a1, a2, ((uint32)(r_u32(0x800FF204u))), 1));
    v7 = (v6 + sub_80067ABC(a1, a2, ((uint32)(r_u32(0x800FF5E0u))), 0));
    return (v7 + sub_8001F7A0(a1, a2));
}

uint32 sub_80062254(uint32 a1, uint32 a2)
{
    sint32 result;
    w_u32(((uint32)((a1 + 68))), 0x800A31E4u);
    sub_80062A64(a1, 0x800FF4E8u);
    if ((r_u32(0x800FF5A0u) && (r_u32(((uint32)((r_u32(0x800FF5A0u) + 524)))) == a1)))
        w_u32(((uint32)((r_u32(0x800FF5A0u) + 524))), 0);
    sub_800629BC(a1, 0);
    result = (a2 & 1);
    if (((a2 & 1) != 0))
        return sub_80062608(a1);
    return result;
}

/* TODO Missing call adapter indirect */
uint32 sub_8001F7A0(uint32 lower, uint32 upper)
{
    uint32 object = r_u32(0x800FF434u), next, table, count = 0u;
    while (object)
    {
        next = r_u32(object + 4u);
        if (r_u8(object + 67u) == 7u && (sint32)r_u32(object + 24u) >= (sint32)r_u32(lower) && (sint32)r_u32(upper) >= (sint32)r_u32(object + 24u) && (sint32)r_u32(object + 28u) >= (sint32)r_u32(lower + 4u) && (sint32)r_u32(upper + 4u) >= (sint32)r_u32(object + 28u) && (sint32)r_u32(object + 32u) >= (sint32)r_u32(lower + 8u) && (sint32)r_u32(upper + 8u) >= (sint32)r_u32(object + 32u))
        {
            table = r_u32(object + 68u);
            ++count;
            (void)apocalypse_object_cleanup(r_u32(table + 12u), object + (uint32)(sint32)(short)r_u16(table + 8u), 3u);
        }
        object = next;
    }
    return count;
}
