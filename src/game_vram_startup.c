#include "game_vram_startup.h"
#include "draft_first_adapters.h"
#include <stdio.h>
#include <string.h>

static void store_native_word(void *output, uint32 value)
{
    memcpy(output, &value, sizeof(value));
}

static uint32 allocate_vram(uint32 a1, uint32 a2, uint32 a3, void *a4, void *a5, uint32 a6, uint32 a7, uint32 a8)
{
    sint32 v12;
    sint32 v14;
    uint32 v15;
    sint32 v16;
    sint32 v17;
    sint32 v18;
    sint32 v19;
    unsigned short v20;
    unsigned short v21;
    sint32 v22;
    uint32 v23;
    uint32 v24;
    uint32 v25;
    uint32 v26;
    sint32 result;
    sint8 v28;
    unsigned short v29;
    uint32 v30;
    uint32 v31;
    sint32 v32;
    signed int v33;
    signed int v34;
    sint32 v35;
    sint32 v36;
    uint32 v37;
    sint32 v38;
    signed int v39;
    sint32 v40;
    uint32 v41;
    sint32 v42;
    uint32 v43;
    sint32 v44;
    sint32 v45;
    short v46;
    sint32 v47;
    sint32 v48;
    sint32 v49;
    sint32 v50;
    sint32 v51;
    uint32 v52;
    short v53;
    uint32 v54;
    short v55;
    short v56;
    short v57;
    uint32 v58;
    sint32 v59;
    sint32 v60;
    sint32 v61;
    sint32 v62;
    short v63;
    uint32 v64;
    short v65;
    short v66;
    short v67;
    uint32 v68;
    sint8 v69;
    short v70;
    sint32 v71;
    sint32 v72;
    sint32 v73;
    sint32 v74;
    v12 = a2;
    v73 = a2;
    v14 = a2;
    v74 = a3;
    if (!a7)
        goto LABEL_28;
    if (((a2 == 256) && (a3 == 256)))
        a7 = 0;
    if (!a7)
    {
    LABEL_28:
        if ((a6 != 16))
            v14 = (((sint32)(((uint32)(a2) + (uint32)(1)))) / 2);

        if ((a6 == 4))
            v14 = (((sint32)(((uint32)(v14) + (uint32)(1)))) / 2);
        v31 = r_u32(0x800FF67Cu);
        v30 = 0;
        if (r_u32(0x800FF67Cu))
        {
            do
            {
                v32 = r_u16(v31);
                v33 = ((sint32)(((uint32)(v32) + (uint32)(v14))));
                if ((a6 == 8))
                    v34 = ((uint32)((v32 & 0xFFFFFFC0)) + (uint32)(128));
                else
                    v34 = ((uint32)((v32 & 0xFFFFFFC0)) + (uint32)(64));
                v35 = (((sint32)(v34)) < ((sint32)(v33)));
                if (((!r_u32(0x800FF674u) || ((r_u16(v31 + 2u) + a3) & 255u)) && ((((sint32)(a1)) < 0) || !v35)))
                {
                    v36 = r_u16((v31 + (2) * 2u));
                    if (((v36 == v14) && (r_u16((v31 + (3) * 2u)) == a3)))
                    {
                        v30 = v31;
                        break;
                    }
                    if (((((((!v30 || ((r_u16((v30 + (2) * 2u)) != v14) && (r_u16((v30 + (3) * 2u)) != a3))) || (v36 == v14)) || (r_u16((v31 + (3) * 2u)) == a3)) && (r_u16((v31 + (2) * 2u)) >= ((sint32)(v14)))) && (r_u16((v31 + (3) * 2u)) >= ((sint32)(a3)))) && (!v30 || (((uint32)(r_u16((v31 + (2) * 2u))) * (uint32)(r_u16((v31 + (3) * 2u)))) < ((uint32)(r_u16((v30 + (2) * 2u))) * (uint32)(r_u16((v30 + (3) * 2u))))))))
                    {
                        v30 = v31;
                    }
                }
                v31 = ((uint32)(r_u32((((uint32)(v31)) + (2) * 4u))));
            } while (v31);
        }
        if (!v30)
        {
            v37 = r_u32(0x800FF67Cu);
            if (r_u32(0x800FF67Cu))
            {
                do
                {
                    v38 = ((uint32)(r_u16(v37)) + (uint32)(r_u16((v37 + (2) * 2u))));
                    if ((a6 == 8))
                        v39 = ((uint32)((((sint32)(((uint32)(v38) - (uint32)(v14)))) & 0xFFFFFFC0)) + (uint32)(128));
                    else
                        v39 = ((uint32)((((sint32)(((uint32)(v38) - (uint32)(v14)))) & 0xFFFFFFC0)) + (uint32)(64));
                    v40 = (((sint32)(v39)) < ((sint32)(v38)));
                    if ((((!r_u32(0x800FF674u) || ((r_u16(v37 + 2u) + a3) & 255u)) && ((((sint32)(a1)) < 0) || !v40)) && (((r_u16((v37 + (2) * 2u)) == v14) && (r_u16((v37 + (3) * 2u)) == a3)) || ((((((!v30 || ((r_u16((v30 + (2) * 2u)) != v14) && (r_u16((v30 + (3) * 2u)) != a3))) || (r_u16((v37 + (2) * 2u)) == v14)) || (r_u16((v37 + (3) * 2u)) == a3)) && (r_u16((v37 + (2) * 2u)) >= ((sint32)(v14)))) && (r_u16((v37 + (3) * 2u)) >= ((sint32)(a3)))) && (!v30 || (((uint32)(r_u16((v37 + (2) * 2u))) * (uint32)(r_u16((v37 + (3) * 2u)))) < ((uint32)(r_u16((v30 + (2) * 2u))) * (uint32)(r_u16((v30 + (3) * 2u))))))))))
                    {
                        v30 = v37;
                    }
                    v37 = ((uint32)(r_u32((((uint32)(v37)) + (2) * 4u))));
                } while (v37);
            }
            if (!v30)
            {
                v41 = r_u32(0x800FF67Cu);
                if (r_u32(0x800FF67Cu))
                {
                    do
                    {
                        if ((!r_u32(0x800FF674u) || ((r_u16(v41 + 2u) + a3) & 255u)) && (!v30 || (r_u16(v30 + 4u) != v14 && r_u16(v30 + 6u) != a3) || r_u16(v41 + 4u) == v14 || r_u16(v41 + 6u) == a3) && (sint32)(r_u16(v41 + 4u) - (((r_u16(v41) + 64u) & 0xFFFFFFC0u) - r_u16(v41))) >= v14 && (sint32)r_u16(v41 + 6u) >= (sint32)a3 && (!v30 || (sint32)((uint32)r_u16(v41 + 4u) * r_u16(v41 + 6u)) < (sint32)((uint32)r_u16(v30 + 4u) * r_u16(v30 + 6u))))
                        {
                            v30 = v41;
                        }
                        v41 = ((uint32)(r_u32((((uint32)(v41)) + (2) * 4u))));
                    } while (v41);
                }
                if (!v30)
                {
                    fprintf(stderr, "Out of VRAM whilst packing %x, (%d,%d)\n", a8, v73, v74);
                    return 0;
                }
                v42 = r_u16(v30);
                v43 = (((uint32)(v42) + (uint32)(64)) & 0xFFFFFFC0);
                v44 = r_u16((v30 + (2) * 2u));
                v45 = ((uint32)(v43) - (uint32)(v42));
                v46 = ((sint32)(((uint32)(v44) - (uint32)(v45))));
                if ((((sint32)(((sint32)(((uint32)(v44) - (uint32)(v45)))))) > 0))
                {
                    sub_800682CC(v43, r_u16((v30 + (1) * 2u)), ((sint32)(((uint32)(v44) - (uint32)(v45)))), r_u16((v30 + (3) * 2u)));
                    w_u16((v30 + (2) * 2u), (r_u16((v30 + (2) * 2u)) - (v46)));
                    a2 = v12;
                }
                result = allocate_vram(a1, a2, a3, a4, a5, a6, 0u, a8);
                w_u8(((uint32)(((uint32)(result) + (uint32)(14)))), a6);
                w_u8(((uint32)(((uint32)(result) + (uint32)(15)))), a7);
                w_u8(((uint32)(((uint32)(result) + (uint32)(12)))), a1);
                goto LABEL_114;
            }
            v47 = ((sint32)(v30));
            v48 = r_u16(v30);
            v49 = r_u16((v30 + (1) * 2u));
            v50 = r_u16(((uint32)(((uint32)(v47) + (uint32)(4)))));
            v51 = r_u16(((uint32)(((uint32)(v47) + (uint32)(6)))));
            sub_8006838C(0x800FF67Cu, v47);
            v52 = sub_8006832C(((sint32)(((uint32)(((sint32)(((uint32)(v48) + (uint32)(v50))))) - (uint32)(v14)))), v49, v14, a3);
            if ((v50 == v14))
            {
                v53 = ((sint32)(((uint32)(v49) + (uint32)(a3))));
                if ((v51 == a3))
                {
                LABEL_103:
                    store_native_word(a4, ((sint32)(((uint32)(((sint32)(((uint32)(v48) + (uint32)(v50))))) - (uint32)(v14)))));

                LABEL_113:
                    store_native_word(a5, v49);

                    w_u8((v52 + (14) * 1u), a6);
                    result = ((sint32)(v52));
                    w_u8((v52 + (15) * 1u), a7);
                    w_u8((v52 + (12) * 1u), a1);
                LABEL_114:
                    v69 = r_u32(0x800FF674u);

                    w_u16(((uint32)(((uint32)(result) + (uint32)(16)))), v73);
                    v70 = v74;
                    w_u8(((uint32)(((uint32)(result) + (uint32)(13)))), v69);
                    w_u16(((uint32)(((uint32)(result) + (uint32)(18)))), v70);
                    return result;
                }
                v55 = v48;
                v56 = v50;
            }
            else
            {
                v53 = v49;
                if ((v51 == a3))
                {
                    v55 = v48;
                    v56 = ((sint32)(((uint32)(v50) - (uint32)(v14))));
                    v57 = v51;
                LABEL_102:
                    v58 = sub_800682CC(v55, v53, v56, v57);

                    sub_800690A0(v58);
                    goto LABEL_103;
                }
                v54 = sub_800682CC(v48, v49, ((sint32)(((uint32)(v50) - (uint32)(v14)))), v51);
                sub_800690A0(v54);
                v55 = ((sint32)(((uint32)(((sint32)(((uint32)(v48) + (uint32)(v50))))) - (uint32)(v14))));
                v53 = ((sint32)(((uint32)(v49) + (uint32)(a3))));
                v56 = v14;
            }
            v57 = ((sint32)(((uint32)(v51) - (uint32)(a3))));
            goto LABEL_102;
        }
        v59 = ((sint32)(v30));
        v60 = r_u16(v30);
        v49 = r_u16((v30 + (1) * 2u));
        v61 = r_u16(((uint32)(((uint32)(v59) + (uint32)(4)))));
        v62 = r_u16(((uint32)(((uint32)(v59) + (uint32)(6)))));
        sub_8006838C(0x800FF67Cu, v59);
        v52 = sub_8006832C(v60, v49, v14, a3);
        if ((v61 == v14))
        {
            v63 = ((sint32)(((uint32)(v49) + (uint32)(a3))));
            if ((v62 == a3))
            {
            LABEL_112:
                store_native_word(a4, v60);

                goto LABEL_113;
            }
            v65 = v60;
            v66 = v61;
        }
        else
        {
            v63 = v49;
            if ((v62 == a3))
            {
                v65 = ((sint32)(((uint32)(v60) + (uint32)(v14))));
                v66 = ((sint32)(((uint32)(v61) - (uint32)(v14))));
                v67 = v62;
            LABEL_111:
                v68 = sub_800682CC(v65, v63, v66, v67);

                sub_800690A0(v68);
                goto LABEL_112;
            }
            v64 = sub_800682CC(((sint32)(((uint32)(v60) + (uint32)(v14)))), v49, ((sint32)(((uint32)(v61) - (uint32)(v14)))), v62);
            sub_800690A0(v64);
            v65 = v60;
            v63 = ((sint32)(((uint32)(v49) + (uint32)(a3))));
            v66 = v14;
        }
        v67 = ((sint32)(((uint32)(v62) - (uint32)(a3))));
        goto LABEL_111;
    }
    if (((a2 == 256) || (a3 == 256)))
    {
        v15 = ((uint32)(xport_draft_host_sub_80068450_p45(a1, 256, 256, &v71, &v72, a6, 0u, 0x800A3A08u)));
        v16 = 128;
        v17 = 256;
        if ((a6 != 16))
            v14 = (((sint32)(((uint32)(v14) + (uint32)(1)))) >> 1);
        if ((a6 == 4))
        {
            v14 = (((sint32)(((uint32)(v14) + (uint32)(1)))) >> 1);
            v16 = 64;
        }
        v18 = r_u16(v15);
        v19 = r_u16((v15 + (1) * 2u));
    }
    else
    {
        v15 = ((uint32)(xport_draft_host_sub_80068450_p45(a1, 2u * a2, 2u * a3, &v71, &v72, a6, 0u, a8)));
        if ((a6 != 16))
            v14 = (((sint32)(((uint32)(v14) + (uint32)(1)))) / 2);
        v16 = ((uint32)(2) * (uint32)(v14));
        if ((a6 == 4))
        {
            v14 = (((sint32)(((uint32)(v14) + (uint32)(1)))) / 2);
            v16 = ((uint32)(2) * (uint32)(v14));
        }
        v17 = ((uint32)(2) * (uint32)(a3));
        v18 = r_u16(v15);
        v19 = r_u16((v15 + (1) * 2u));
        v71 = (((uint32)(((sint32)(((uint32)(v71) + (uint32)(v14))))) - (uint32)(1)) & -v14);
        v72 = (((uint32)(((sint32)(((uint32)(v72) + (uint32)(a3))))) - (uint32)(1)) & (0u - a3));
    }
    v20 = v71;
    v21 = v72;
    v22 = v71;
    w_u16((v15 + (2) * 2u), v14);
    w_u16((v15 + (3) * 2u), a3);
    w_u16(v15, v20);
    w_u16((v15 + (1) * 2u), v21);
    store_native_word(a4, v22);
    store_native_word(a5, v72);
    if ((((sint32)(v18)) < ((sint32)(v71))))
    {
        v23 = sub_800682CC(v18, v19, ((sint32)(((uint32)(v71) - (uint32)(v18)))), v17);
        sub_800690A0(v23);
        v16 -= ((sint32)(((uint32)(v71) - (uint32)(v18))));
    }
    if ((((sint32)(v14)) < ((sint32)(v16))))
    {
        v24 = sub_800682CC(((sint32)(((uint32)(v71) + (uint32)(v14)))), v19, ((sint32)(((uint32)(v16) - (uint32)(v14)))), v17);
        sub_800690A0(v24);
    }
    if ((((sint32)(v19)) < ((sint32)(v72))))
    {
        v25 = sub_800682CC(v71, v19, v14, ((sint32)(((uint32)(v72) - (uint32)(v19)))));
        sub_800690A0(v25);
        v17 -= ((sint32)(((uint32)(v72) - (uint32)(v19))));
    }
    if ((((sint32)(a3)) < ((sint32)(v17))))
    {
        v26 = sub_800682CC(v71, ((sint32)(((uint32)(v72) + (uint32)(a3)))), v14, ((sint32)(((uint32)(v17) - (uint32)(a3)))));
        sub_800690A0(v26);
    }
    w_u8((((uint32)(v15)) + (14) * 1u), a6);
    result = ((sint32)(v15));
    w_u8((((uint32)(v15)) + (15) * 1u), a7);
    w_u8((((uint32)(v15)) + (12) * 1u), a1);
    v28 = r_u32(0x800FF674u);
    w_u16((v15 + (8) * 2u), v73);
    v29 = v74;
    w_u8((((uint32)(v15)) + (13) * 1u), v28);
    w_u16((v15 + (9) * 2u), v29);
    return result;
}

uint32 xport_draft_host_sub_80068450_p45(uint32 owner, uint32 width, uint32 height, void *x, void *y, uint32 depth, uint32 flags, uint32 name)
{
    return allocate_vram(owner, width, height, x, y, depth, flags, name);
}

uint32 apocalypse_allocate_vram(uint32 owner, uint32 width, uint32 height, uint32 x, uint32 y, uint32 depth, uint32 flags, uint32 name)
{
    return allocate_vram(owner, width, height, psx_addr(x, 4u), psx_addr(y, 4u), depth, flags, name);
}