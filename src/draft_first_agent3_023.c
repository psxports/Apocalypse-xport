#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

uint32 sub_8006696C(uint32 a1, uint32 a2)
{
    sint32 v2;
    sint32 v3;
    sint32 v4;
    sint32 v6;
    sint32 v7;
    sint32 v8;
    v6 = (r_u32(a1) - r_u32(a2));
    v7 = (r_u32((a1 + (1) * 4u)) - r_u32((a2 + (1) * 4u)));
    v8 = (r_u32((a1 + (2) * 4u)) - r_u32((a2 + (2) * 4u)));
    if ((v6 < 0))
        v6 = (r_u32(a2) - r_u32(a1));
    if (((r_u32((a1 + (1) * 4u)) - r_u32((a2 + (1) * 4u))) < 0))
        v7 = (r_u32((a2 + (1) * 4u)) - r_u32((a1 + (1) * 4u)));
    if (((r_u32((a1 + (2) * 4u)) - r_u32((a2 + (2) * 4u))) < 0))
        v8 = (r_u32((a2 + (2) * 4u)) - r_u32((a1 + (2) * 4u)));
    if ((v6 >= v7))
    {
        if ((v6 >= v8))
        {
            v3 = (v8 >> 2);
            if ((v8 < v7))
            {
                v4 = (v7 >> 1);
            }
            else
            {
                v3 = (v7 >> 2);
                v4 = (v8 >> 1);
            }
            v2 = ((v3 + v4) + v6);
        }
        else
        {
            v2 = (((v7 >> 2) + (v6 >> 1)) + v8);
        }
    }
    else if ((v7 >= v8))
    {
        if ((v8 >= v6))
            v2 = (((v6 >> 2) + (v8 >> 1)) + v7);
        else
            v2 = (((v8 >> 2) + (v6 >> 1)) + v7);
    }
    else
    {
        v2 = (((v6 >> 2) + (v7 >> 1)) + v8);
    }
    return (v2 >> 12);
}

void nullsub_22(void)
{
    ;
}

/* TODO Missing call adapter BYTE4 */
/* TODO Missing call adapter HIDWORD */
/* TODO Missing call adapter LODWORD */
/* TODO Missing call adapter SBYTE4 */
/* TODO Missing call adapter SHIWORD */
/* TODO Missing call adapter SLOWORD */
/* TODO Missing call adapter abs32 */
/* TODO Missing call adapter indirect */
/* TODO Missing call adapter sub_8001922C */
/* TODO Missing call adapter sub_80019318 */
/* TODO Missing call adapter sub_800197DC */
/* TODO Missing call adapter sub_8001ECFC */
/* TODO Missing call adapter sub_8005C338 */
/* TODO Missing call adapter sub_80061D8C */
/* TODO Missing call adapter sub_8007823C */
/* TODO Missing call adapter sub_800878DC */
/* TODO Missing call adapter xport_draft_missing_gte_adapter */
/* TODO Postincrement memory expressions may require ordering refinement */
/* TODO Missing call adapter SHIWORD */
/* TODO Missing call adapter SLOWORD */
/* TODO Missing call adapter abs32 */
/* TODO Missing call adapter indirect */
/* TODO Missing call adapter sub_8001922C */
/* TODO Missing call adapter sub_80019318 */
/* TODO Missing call adapter sub_800197DC */
/* TODO Missing call adapter sub_8001ECFC */
/* TODO Missing call adapter sub_8005C338 */
/* TODO Missing call adapter sub_80061D8C */
/* TODO Missing call adapter sub_8007823C */
/* TODO Missing call adapter sub_800878DC */
/* TODO Missing call adapter xport_draft_missing_gte_adapter */
/* TODO Postincrement memory expressions may require ordering refinement */
/* Missing dependencies remain explicit until native execution requires them */
static uint32 player_update_missing(const char *operation)
{
    fprintf(stderr, "TODO 8005F688: %s\n", operation);
    abort();
}

static uint32 player_update_abs(sint32 value)
{
    return value < 0 ? 0u - (uint32)value : (uint32)value;
}

void apocalypse_object_virtual20(uint32 target, uint32 receiver)
{
    switch (target)
    {
        case 0x80062F0Cu: sub_80062F0C(receiver); break;
        case 0x80020264u: sub_80020264(receiver); break;
        case 0x8004B8C8u: sub_8004B8C8(receiver); break;
        case 0x8005DD40u: sub_8005DD40(receiver); break;
        case 0x8001D240u: sub_8001D240(receiver); break;
        case 0x8002396Cu: sub_8002396C(receiver); break;
        case 0x800236D4u: sub_800236D4(receiver); break;
        case 0x80035A54u: sub_80035A54(receiver); break;
        case 0x800355A0u: sub_800355A0(receiver); break;
        case 0x800326E8u: sub_800326E8(receiver); break;
        case 0x80035758u: sub_80035758(receiver); break;
        default: player_update_missing("Object virtual slot20 target");
    }
}

void apocalypse_object_virtual52(uint32 target, uint32 receiver, uint32 argument, uint32 vector, uint32 zero)
{
    switch (target)
    {
        case 0x8005DDF0u: sub_8005DDF0(receiver, argument, vector, zero); break;
        default: player_update_missing("Object virtual slot52 target");
    }
}

uint32 sub_8005F688(uint32 a1)
{
    /* TODO Recover GTE register carriers and missing host GTE adapters */

    sint32 v1;
    sint32 v3;
    sint32 v4;
    sint32 result;
    sint32 v6;
    sint32 v7;
    sint8 v8;
    sint32 v9;
    sint8 v10;
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
    short v21;
    sint32 v22;
    sint32 v23;
    sint32 v24;
    uint32 v25;
    sint32 v26;
    sint32 v27;
    sint32 v28;
    sint32 v29;
    sint32 v30;
    uint32 v31;
    sint32 v32;
    sint32 v33;
    sint32 v34;
    sint32 v35;
    uint32 v36;
    sint32 v37;
    sint32 v38;
    sint32 v39;
    sint32 v40;
    sint32 v41;
    sint32 v42;
    sint32 v43;
    sint32 v44;
    sint32 v45;
    sint32 v46;
    sint32 v47;
    sint32 v48;
    sint32 v49;
    sint32 v50;
    sint32 v51;
    sint32 v52;
    sint32 v53;
    sint32 v54;
    sint32 v55;
    sint32 v56;
    sint32 v57;
    sint32 v58;
    sint32 v59;
    sint32 v60;
    sint32 v61;
    sint32 v62;
    sint32 v63;
    sint32 v64;
    sint32 v65;
    sint32 v66;
    sint32 v67;
    short v68;
    short v69;
    sint32 v70;
    sint32 v71;
    sint32 v72;
    sint32 v73;
    sint32 v74;
    sint32 v75;
    sint32 v76;
    sint32 v77;
    sint32 v78;
    sint32 v79;
    sint32 v80;
    short v81;
    sint32 v82;
    sint32 v83;
    sint32 v84;
    sint32 v85;
    sint32 v86;
    sint32 v87;
    sint32 v88;
    sint32 v89;
    sint32 v90;
    sint32 v91;
    sint32 v92;
    sint32 v93;
    sint32 v94;
    sint32 v95;
    sint32 v96;
    sint32 v97;
    sint32 v98;
    sint32 v99;
    sint32 v100;
    sint32 v101;
    sint32 v102;
    sint32 v103;
    sint32 v104;
    sint32 v105;
    sint32 v106;
    sint32 v107;
    sint32 v108;
    sint32 v109;
    sint32 v110;
    sint32 v111;
    sint32 v112;
    sint32 v113;
    sint32 v114;
    sint32 v115;
    sint32 v116;
    sint32 v117;
    sint32 v118;
    sint32 v119;
    sint32 v120;
    sint32 v121;
    sint32 v122;
    sint32 v123;
    short v124;
    sint32 v125;
    sint32 v126;
    sint32 v127;
    sint32 v128;
    sint32 v129;
    sint32 v130;
    sint32 v131;
    sint32 v132;
    sint32 v133;
    sint32 v134;
    sint32 v135;
    uint32 v136;
    sint32 v137;
    sint32 v138;
    uint32 v139;
    sint32 v140;
    sint32 v141;
    uint32 v142;
    uint32 v143;
    uint32 v144;
    uint32 v145;
    uint32 v146;
    uint32 v147;
    sint32 v148;
    sint32 v149;
    sint32 v150;
    sint32 v151;
    short v152;
    short v153;
    sint32 v154;
    sint32 v156;
    short v157;
    short v158;
    short v159;
    sint32 v160;
    long long v161;
    signed int v162;
    sint32 v164;
    sint32 v165;
    sint32 v166;
    sint32 v167;
    sint32 v168;
    sint32 v169;
    sint32 v170;
    sint32 v171;
    sint32 v172;
    sint32 v173;
    sint32 v174;
    sint32 v175;
    sint32 v176;
    sint32 v177;
    sint32 v178;
    sint32 v179;
    uint32 v180;
    sint32 v181;
    sint32 v182;
    sint32 v183;
    sint32 v184;
    sint32 v185;
    sint32 v186;
    sint32 v187;
    sint32 v188;
    sint32 v189;
    sint32 v190;
    sint32 v191;
    sint32 v192;
    sint32 v193;
    sint32 v194;
    sint32 v195;
    sint32 v196;
    sint32 v197;
    sint32 v198;
    sint32 v199;
    sint32 v200;
    sint32 v201;
    sint32 v202;
    sint8 v203;
    sint32 v204;
    sint32 v205;
    short v206;
    sint32 v207;
    sint32 v208;
    sint32 v209;
    sint32 v210;
    sint32 v211;
    sint32 v212;
    sint32 v213;
    sint32 v214;
    sint32 v215;
    sint32 position[4];
#define v216 position[0]
#define v217 position[1]
#define v218 position[2]
#define v219 position[3]
    MATRIX v220;
    MATRIX v221;
    sint32 vectors[6];
#define v222 vectors[0]
#define v223 vectors[1]
#define v224 (vectors + 2)
#define v225 vectors[4]
#define v226 (*(sint16 *)(vectors + 5))
    sint32 collision_record[35];
#define v227 collision_record
#define v228 collision_record[2]
#define v229 collision_record[3]
#define v230 collision_record[4]
#define v231 collision_record[5]
#define v232 ((char *)(collision_record + 6))
#define v233 collision_record[26]
    sint32 v234[35];
    sint32 v236;
    sint32 v237;
    sint32 v238;
    sint32 v239;
    sint32 v240;
    sint32 v241;
    (v3 = r_u32(((uint32)((a1 + 456)))));
    (v4 = (v3 == 0));
    (result = (v3 - 1));
    if (!v4)
    {
        w_u32(((uint32)((a1 + 456))), result);
        return result;
    }
    (v6 = r_u32(((uint32)((a1 + 428)))));
    if ((v6 && ((r_u16(((uint32)((v6 + 78)))) & 0x40) != 0)))
        w_u32(((uint32)((a1 + 428))), 0);
    w_u32(0x800FF970u, 0);
    (v7 = r_u8(((uint32)((a1 + 465)))));
    (v4 = (v7 == 0));
    (v8 = (v7 - 1));
    if (!v4)
        w_u8(((uint32)((a1 + 465))), v8);
    (v9 = r_u8(((uint32)((a1 + 466)))));
    (v4 = (v9 == 0));
    (v10 = (v9 - 1));
    if (!v4)
    {
        w_u8(((uint32)((a1 + 466))), v10);
        if (v10)
            w_u32(((uint32)((a1 + 176))), (((r_u8(((uint32)((a1 + 466)))) | (r_u8(((uint32)((a1 + 466)))) << 8)) | 0x2000000) | (r_u8(((uint32)((a1 + 466)))) << 16)));
        else
            w_u32(((uint32)((a1 + 176))), 0);
    }
    (v11 = r_u32(((uint32)((a1 + 452)))));
    (v4 = (v11 == 0));
    (v12 = (v11 - 1));
    if (!v4)
    {
        (v13 = r_u32(((uint32)((a1 + 444)))));
        w_u32(((uint32)((a1 + 452))), v12);
        w_u8(((uint32)((v13 + 128))), 0);
        w_u8(((uint32)((r_u32(((uint32)((a1 + 444)))) + 144))), 0);
        w_u8(((uint32)((r_u32(((uint32)((a1 + 444)))) + 160))), 0);
        w_u8(((uint32)((r_u32(((uint32)((a1 + 444)))) + 176))), 0);
        w_u8(r_u32(((uint32)((a1 + 444)))), 0);
        w_u8(((uint32)((r_u32(((uint32)((a1 + 444)))) + 32))), 0);
        w_u8(((uint32)((r_u32(((uint32)((a1 + 444)))) + 48))), 0);
        w_u8(((uint32)((r_u32(((uint32)((a1 + 444)))) + 16))), 0);
        w_u8(((uint32)((r_u32(((uint32)((a1 + 444)))) + 240))), 0);
        w_u8(((uint32)((r_u32(((uint32)((a1 + 444)))) + 64))), 0);
        w_u8(((uint32)((r_u32(((uint32)((a1 + 444)))) + 80))), 0);
        w_u8(((uint32)((r_u32(((uint32)((a1 + 444)))) + 96))), 0);
        w_u8(((uint32)((r_u32(((uint32)((a1 + 444)))) + 112))), 0);
        w_u8(((uint32)((r_u32(((uint32)((a1 + 444)))) + 273))), 0);
        w_u8(((uint32)((r_u32(((uint32)((a1 + 444)))) + 81))), 0);
    }
    (v14 = r_u8(((uint32)((a1 + 26)))));
    w_u32(((uint32)((a1 + 304))), 0x10000);
    if (((v14 == 5) && (r_u8(((uint32)((a1 + 297)))) == 1)))
        w_u32(((uint32)((a1 + 304))), 0x20000);
    if (r_u32(0x800FF328u))
    {
        (v15 = r_u32(((uint32)((a1 + 444)))));
        if (r_u8(((uint32)((v15 + 81)))))
        {
            w_u8(((uint32)((v15 + 81))), 0);
            player_update_missing("sub_8005C338");
        }
    }
    (result = r_u8(((uint32)((a1 + 464)))));
    if (!(r_u8(((uint32)((a1 + 464))))))
    {
        w_u16(((uint32)(a1)), (r_u16(((uint32)(a1))) | (4u)));
        (v16 = r_u32(((uint32)((a1 + 8)))));
        (v17 = r_u32(((uint32)((a1 + 12)))));
        w_u32(((uint32)((a1 + 228))), r_u32(((uint32)((a1 + 4)))));
        w_u32(((uint32)((a1 + 232))), v16);
        w_u32(((uint32)((a1 + 236))), v17);
        (v18 = r_u32(((uint32)((a1 + 108)))));
        (v19 = (r_u32(((uint32)((a1 + 460)))) & 0xF000));
        w_u32(((uint32)((a1 + 32))), 8421504);
        if (!v19)
            sub_8003ACB8(a1);
        (v20 = r_u32(0x800FF378u));
        (v21 = (r_u16(((uint32)(a1))) & 0xFFF7));
        (v4 = (r_u32(0x800FF378u) == 1));
        w_u16(((uint32)(a1)), v21);
        if (((v4 || (v20 == 5)) && (r_u32(0x800FF37Cu) < (r_u32(((uint32)((a1 + 8)))) + (r_u8(((uint32)((a1 + 582)))) << 12)))))
        {
            (v22 = r_u32(0x800FF380u));
            (v4 = (r_u32(0x800FF380u) == 0));
            w_u16(((uint32)(a1)), (v21 | 8));
            if (v4)
            {
                if (((r_u32(0x800FF2F0u) & 3) == 0))
                {
                    (v23 = r_u32(((uint32)((a1 + 12)))));
                    (v216 = r_u32(((uint32)((a1 + 4)))));
                    (v218 = v23);
                    (v217 = r_u32(0x800FF37Cu));
                    (v24 = (sub_80066570(32) + 32));
                    (v25 = (0x800F863Cu + ((sub_80066570(4096) & 0xFFF)) * 4u));
                    (v216 += (v24 * ((sint16)(r_u16(((uint32)(v25)))))));
                    (v218 += (v24 * ((sint16)(r_u16((((uint32)(v25)) + (1) * 2u))))));
                    (v26 = sub_80032DC0(116));
                    if (v26)
                    {
                        (v28 = (sub_80066570(32) + 32));
                        (v27 = sub_80066570(4));
                        player_update_missing("sub_8001ECFC host vector");
                    }
                }
            }
            else
            {
apocalypse_object_virtual52(r_u32(a1 + 68u) == 0x800A32A4u ? r_u32(0x800A32D8u) : player_update_missing("Unreviewed object class extent for slot52"), a1 + (uint32)(sint32)(sint16)r_u16(r_u32(a1 + 68u) + 48u), v22, 0x800A71CCu, 0u);
            }
        }
        if (((r_u32(0x800FF32Cu) && r_u8(0x800EC138u)) && r_u8(0x800EC158u)))
        {
            (v29 = r_u32(((uint32)((a1 + 232)))));
            w_u32(((uint32)((a1 + 8))), v29);
            if (r_u8(0x800EC0F8u))
                w_u32(((uint32)((a1 + 8))), (v29 - 393216));
            if (r_u8(0x800EC128u))
                w_u32(((uint32)((a1 + 8))), (r_u32(((uint32)((a1 + 8)))) + (393216)));
            (v30 = (r_u32(((uint32)((a1 + 12)))) + (5 * (r_u32(((uint32)((a1 + 12)))) - r_u32(((uint32)((a1 + 236))))))));
            w_u32(((uint32)((a1 + 4))), (r_u32(((uint32)((a1 + 4)))) + ((5 * (r_u32(((uint32)((a1 + 4)))) - r_u32(((uint32)((a1 + 228)))))))));
            w_u32(((uint32)((a1 + 12))), v30);
        }
        (v31 = r_u16(((uint32)((a1 + 216)))));
        w_u16(((uint32)((a1 + 212))), 100);
        w_u16(((uint32)((a1 + 20))), 0);
        w_u16(((uint32)((a1 + 16))), 0);
        if ((((((unsigned char)((v31 >> 1))) ^ 1) & 1) != 0))
        {
            (w_u16(((uint32)((a1 + 570))), (r_u16(((uint32)((a1 + 570)))) + 1u)), (r_u16(((uint32)((a1 + 570)))) + 1u));
        }
        else
        {
            w_u16(((uint32)((a1 + 570))), 0);
            w_u8(((uint32)((a1 + 569))), 0);
        }
        if ((((!r_u32(0x800FF32Cu) && (r_u8(((uint32)((a1 + 26)))) == 17)) && r_u8(((uint32)((a1 + 303))))) && !(r_u32(((uint32)((a1 + 484)))))))
            sub_8005E708(a1);
        sub_8005CB1C(a1);
        if ((r_u8(((uint32)((a1 + 468)))) | r_u8(((uint32)((a1 + 469))))))
        {
            (v33 = r_u32(((uint32)((a1 + 608)))));
            if ((v33 < 8))
                w_u32(((uint32)((a1 + 608))), (v33 + 1));
        }
        else
        {
            (v34 = r_u32(((uint32)((a1 + 608)))));
            (v4 = (v34 == 0));
            (v35 = (v34 - 1));
            if (!v4)
                w_u32(((uint32)((a1 + 608))), v35);
        }
        (v36 = r_u32(((uint32)((a1 + 460)))));
        w_u8(((uint32)((a1 + 572))), 1);
        if ((v36 == 512))
        {
            w_u16(((uint32)((a1 + 580))), 0);
            (v106 = sub_8005EA1C(a1));
            (v37 = 0);
            if (v106)
                goto LABEL_232;
            (v107 = sub_8005EA80(a1));
            (v37 = 0);
            if (v107)
                goto LABEL_232;
            (v108 = sub_8005EB60(a1));
            goto LABEL_213;
        }
        if ((v36 < 0x201))
        {
            if ((v36 == 8))
            {
                w_u16(((uint32)((a1 + 580))), 0);
                (v83 = sub_8005EA1C(a1));
                (v37 = 0);
                if (v83)
                    goto LABEL_232;
                (v84 = sub_8005EA80(a1));
                (v37 = 0);
                if (v84)
                    goto LABEL_232;
                (v85 = sub_8005EAF0(a1));
                (v37 = 0);
                if (v85)
                    goto LABEL_232;
                (v86 = sub_8005EB60(a1));
                (v37 = 0);
                if (v86)
                    goto LABEL_232;
                (v87 = sub_8005F00C(a1));
                (v37 = 0);
                if (v87)
                    goto LABEL_232;
                (v88 = sub_8005F124(a1));
                (v37 = 0);
                if (v88)
                    goto LABEL_232;
                (v89 = sub_8005ED30(((uint32)(a1))));
                (v37 = 0);
                if (v89)
                    goto LABEL_232;
                (v90 = sub_8005ED28());
                (v37 = 0);
                if ((v90 || !(r_u8(((uint32)((a1 + 303)))))))
                    goto LABEL_232;
            LABEL_221:
                sub_8005E9E4(a1);

                (v37 = 0);
                goto LABEL_232;
            }
            if ((v36 < 9))
            {
                if ((v36 != 2))
                {
                    if ((v36 >= 3))
                    {
                        if ((v36 == 4))
                        {
                            (v4 = (r_u16(((uint32)((a1 + 570)))) < 0x31u));
                            w_u16(((uint32)((a1 + 580))), 0);
                            if ((v4 || (r_u8(((uint32)((a1 + 26)))) != 2)))
                            {
                                (v74 = r_u8(((uint32)((a1 + 26)))));
                                if ((v74 != 17))
                                {
                                    (v75 = 0);
                                    if ((((r_u32(((uint32)((a1 + 108)))) ^ v18) < 0) || (v74 != 2)))
                                        (v75 = 1);
                                    if (v75)
                                    {
                                        w_u32(((uint32)((a1 + 480))), r_u32(((uint32)((a1 + 8)))));
                                        sub_80063038(a1, 2, (r_u16(0x800EC4D4u) + 1), r_u16(0x800EC4D6u));
                                        w_u8(((uint32)((a1 + 130))), r_u8(0x800EC6DAu));
                                    }
                                }
                            }
                            else
                            {
                                sub_80063038(a1, 17, 0, -1);
                            }
                            (v76 = sub_8005F1B0(a1));
                            (v37 = 0);
                            if (!v76)
                            {
                                (v77 = sub_8005EEDC(a1));
                                (v37 = 0);
                                if (!v77)
                                {
                                    sub_8005F300(a1);
                                    (v37 = 0);
                                }
                            }
                        }
                        else
                        {
                            (v37 = 0);
                        }
                    }
                    else
                    {
                        (v37 = 0);
                        if ((v36 == 1))
                        {
                            (v38 = sub_8005EA1C(a1));
                            (v37 = 0);
                            if (!v38)
                            {
                                (v39 = sub_8005EA80(a1));
                                (v37 = 0);
                                if (!v39)
                                {
                                    (v40 = sub_8005EAF0(a1));
                                    (v37 = 0);
                                    if (!v40)
                                    {
                                        (v41 = sub_8005EB60(a1));
                                        (v37 = 0);
                                        if (!v41)
                                        {
                                            (v42 = sub_8005F00C(a1));
                                            (v37 = 0);
                                            if (!v42)
                                            {
                                                (v43 = sub_8005F124(a1));
                                                (v37 = 0);
                                                if (!v43)
                                                {
                                                    (v44 = sub_8005ED30(((uint32)(a1))));
                                                    (v37 = 0);
                                                    if (!v44)
                                                    {
                                                        (v45 = sub_8005EBD0(a1));
                                                        (v37 = 0);
                                                        if (!v45)
                                                        {
                                                            (v46 = sub_8005EC7C(a1));
                                                            (v37 = 0);
                                                            if (!v46)
                                                            {
                                                                (v47 = sub_8005ED28());
                                                                (v37 = 0);
                                                                if (!v47)
                                                                {
                                                                    if (r_u8(((uint32)((a1 + 303)))))
                                                                        sub_80063038(a1, 0, 0, -1);
                                                                    (v48 = ((unsigned short)((w_u16(((uint32)((a1 + 580))), (r_u16(((uint32)((a1 + 580)))) + 1u)), (r_u16(((uint32)((a1 + 580)))) + 1u)))));
                                                                    if ((v48 == 1350))
                                                                    {
                                                                        (v49 = sub_80066570(8));
                                                                        sub_8002FD2C(8, v49);
                                                                        (v37 = 0);
                                                                        goto LABEL_232;
                                                                    }
                                                                    (v37 = 0);
                                                                    if ((v48 == 4050))
                                                                    {
                                                                        (v50 = sub_80066570(8));
                                                                        sub_8002FC64(8, v50, (a1 + 4), 0);
                                                                        w_u16(((uint32)((a1 + 580))), 1350);
                                                                    LABEL_231:
                                                                        (v37 = 0);

                                                                        goto LABEL_232;
                                                                    }
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                    goto LABEL_232;
                }
                w_u16(((uint32)((a1 + 580))), 0);
                (v57 = sub_8005EEDC(a1));
                (v37 = 0);
                if (v57)
                    goto LABEL_232;
                if (sub_8005F300(a1))
                    goto LABEL_231;
                if (!(r_u8(((uint32)((r_u32(((uint32)((a1 + 444)))) + 272))))))
                    w_u32(0x800FF5A8u, 1);
                (v58 = 4);
                if (r_u8(((uint32)((a1 + 568)))))
                {
                    (v37 = 0);
                    if (r_u8(((uint32)((r_u32(((uint32)((a1 + 444)))) + 272)))))
                        goto LABEL_232;
                    (v58 = 4);
                }
            LABEL_230:
                w_u32(((uint32)((a1 + 460))), v58);

                goto LABEL_231;
            }
            if ((v36 == 32))
            {
                w_u16(((uint32)((a1 + 580))), 0);
                (v99 = sub_8005EAF0(a1));
                (v37 = 0);
                if (v99)
                    goto LABEL_232;
                (v100 = sub_8005EB60(a1));
                (v37 = 0);
                if (v100)
                    goto LABEL_232;
                (v101 = sub_8005F00C(a1));
                (v37 = 0);
                if (v101)
                    goto LABEL_232;
                (v102 = sub_8005F124(a1));
                (v37 = 0);
                if (v102)
                    goto LABEL_232;
                (v103 = sub_8005ED30(((uint32)(a1))));
                (v37 = 0);
                if (v103)
                    goto LABEL_232;
                (v104 = sub_8005EBD0(a1));
                (v37 = 0);
                if (v104)
                    goto LABEL_232;
                (v105 = sub_8005EC7C(a1));
                (v37 = 0);
                if (v105)
                    goto LABEL_232;
                (v98 = ((sint8)(r_u8(((uint32)((a1 + 468)))))));
                goto LABEL_220;
            }
            if ((v36 < 0x21))
            {
                (v37 = 0);
                if ((v36 != 16))
                    goto LABEL_232;
                w_u16(((uint32)((a1 + 580))), 0);
                (v91 = sub_8005EAF0(a1));
                (v37 = 0);
                if (v91)
                    goto LABEL_232;
                (v92 = sub_8005EB60(a1));
                (v37 = 0);
                if (v92)
                    goto LABEL_232;
                (v93 = sub_8005F00C(a1));
                (v37 = 0);
                if (v93)
                    goto LABEL_232;
                (v94 = sub_8005F124(a1));
                (v37 = 0);
                if (v94)
                    goto LABEL_232;
                (v95 = sub_8005ED30(((uint32)(a1))));
                (v37 = 0);
                if (v95)
                    goto LABEL_232;
                (v96 = sub_8005EBD0(a1));
                (v37 = 0);
                if (v96)
                    goto LABEL_232;
                (v97 = sub_8005EC7C(a1));
                (v37 = 0);
                if (v97)
                    goto LABEL_232;
                (v98 = ((sint8)(r_u8(((uint32)((a1 + 468)))))));
            LABEL_220:
                (v37 = 0);

                if (v98)
                    goto LABEL_232;
                goto LABEL_221;
            }
            if ((v36 == 128))
            {
                (v54 = r_u8(((uint32)((a1 + 303)))));
                w_u16(((uint32)((a1 + 580))), 0);
                if (v54)
                {
                    (v55 = r_u32(((uint32)((a1 + 428)))));
                    (v56 = (-4096 * r_u16(0x800EC4DAu)));
                    w_u32(((uint32)((a1 + 564))), v56);
                    if (v55)
                        w_u32(((uint32)((a1 + 564))), (v56 + r_u32(((uint32)((v55 + 108))))));
                    w_u32(((uint32)((a1 + 460))), 2);
                    w_u8(((uint32)((a1 + 568))), r_u8(0x800EC4D8u));
                }
                goto LABEL_231;
            }
            (v37 = 0);
            if ((v36 != 256))
                goto LABEL_232;
            (v51 = sub_8005F00C(a1));
            (v37 = 0);
            if (v51)
                goto LABEL_232;
            w_u16(((uint32)((a1 + 580))), 0);
            w_u16(((uint32)((a1 + 212))), 50);
            (v52 = sub_8005ED30(((uint32)(a1))));
            (v37 = 0);
            if (v52)
                goto LABEL_232;
            (v32 = 21);
            if (r_u8(((uint32)((r_u32(((uint32)((a1 + 444)))) + 256)))))
                goto LABEL_232;
            (v53 = a1);
            if ((((sint8)(r_u8(((uint32)((a1 + 24)))))) < 21))
                (v32 = ((sint8)(r_u8(((uint32)((a1 + 24)))))));
        LABEL_229:
            sub_80063038(v53, 5, v32, 0);

            (v58 = 1);
            goto LABEL_230;
        }
        if ((v36 == 0x4000))
        {
            (v70 = r_u8(((uint32)((a1 + 26)))));
            w_u16(((uint32)((a1 + 580))), 0);
            if ((v70 != 13))
                goto LABEL_231;
            (v71 = sub_8006325C(a1, 13, 3));
            if ((sub_80062D24(a1, v71, 1) || ((v72 = sub_8006325C(a1, 13, 33)), (v73 = sub_80062D24(a1, v72, 1)), (v37 = 0), v73)))
            {
                ((void)(r_u32(((uint32)((a1 + 512))))), (void)(64), player_update_missing("sub_800197DC"), 0u);
                (v37 = 0);
            }
            goto LABEL_232;
        }
        if ((v36 < 0x4001))
        {
            if ((v36 == 2048))
            {
                (v116 = sub_8005F00C(a1));
                (v37 = 0);
                if (v116)
                    goto LABEL_232;
                w_u16(((uint32)((a1 + 580))), 0);
                w_u16(((uint32)((a1 + 212))), 50);
                (v117 = sub_8005F124(a1));
                (v37 = 0);
                if (v117)
                    goto LABEL_232;
                (v118 = ((sint8)(r_u8(((uint32)((a1 + 24)))))));
                if (((v118 < 21) && (r_u8(((uint32)((r_u32(((uint32)((a1 + 444)))) + 256)))) || (v118 < 11))))
                    goto LABEL_232;
                (v58 = 256);
                if (r_u8(((uint32)((r_u32(((uint32)((a1 + 444)))) + 256)))))
                    goto LABEL_230;
                (v53 = a1);
                (v32 = 21);
                goto LABEL_229;
            }
            if ((v36 >= 0x801))
            {
                if ((v36 == 4096))
                {
                    w_u16(((uint32)((a1 + 580))), 0);
                    w_u32(((uint32)((a1 + 460))), 0x2000);
                    w_u32(((uint32)((a1 + 304))), 111408);
                    sub_80063038(a1, 11, 0, -1);
                    (v37 = 0);
                    goto LABEL_232;
                }
                (v37 = 0);
                if ((v36 == 0x2000))
                {
                    (v78 = r_u8(((uint32)((a1 + 303)))));
                    w_u16(((uint32)((a1 + 580))), 0);
                    w_u32(((uint32)((a1 + 304))), 111408);
                    if (!v78)
                        goto LABEL_231;
                    sub_80063038(a1, 0, 0, -1);
                    (v236 = 128);
                    sub_8006C47C(&v216, &v236, (a1 + 552));
                    sub_8006C0FC((a1 + 4), &v216);
                    (v79 = r_u32(((uint32)((a1 + 444)))));
                    w_u32(((uint32)((a1 + 8))), (r_u32(((uint32)((a1 + 8)))) - (884736)));
                    w_u8(((uint32)((v79 + 273))), 0);
                    (v80 = sub_80067A18((a1 + 4), 0, 2048));
                    if ((v80 != -1))
                    {
                        (v81 = (r_u16(((uint32)((a1 + 216)))) | 2));
                        w_u32(((uint32)((a1 + 8))), (v80 - (r_u8(((uint32)((a1 + 582)))) << 12)));
                        w_u16(((uint32)((a1 + 216))), v81);
                    }
                    w_u8(((uint32)((a1 + 578))), 4);
                    (v82 = r_u32(0x800FF904u));
                    w_u32(((uint32)((a1 + 460))), 1);
                    w_u32(((uint32)((a1 + 112))), 0);
                    w_u32(((uint32)((a1 + 104))), 0);
                    ((void)(v82), player_update_missing("sub_8007823C"), 0u);
                    (v37 = 0);
                }
                goto LABEL_232;
            }
            (v37 = 0);
            if ((v36 != 1024))
                goto LABEL_232;
            w_u16(((uint32)((a1 + 580))), 0);
            (v109 = sub_8005EA1C(a1));
            (v37 = 0);
            if (v109)
                goto LABEL_232;
            (v110 = sub_8005EA80(a1));
            (v37 = 0);
            if (v110)
                goto LABEL_232;
            (v108 = sub_8005EAF0(a1));
        LABEL_213:
            (v37 = 0);

            if (v108)
                goto LABEL_232;
            (v111 = sub_8005F00C(a1));
            (v37 = 0);
            if (v111)
                goto LABEL_232;
            (v112 = sub_8005F124(a1));
            (v37 = 0);
            if (v112)
                goto LABEL_232;
            (v113 = sub_8005ED30(((uint32)(a1))));
            (v37 = 0);
            if (v113)
                goto LABEL_232;
            (v114 = sub_8005EBD0(a1));
            (v37 = 0);
            if (v114)
                goto LABEL_232;
            (v115 = sub_8005EC7C(a1));
            (v37 = 0);
            if (v115)
                goto LABEL_232;
            (v98 = ((sint8)(r_u8(((uint32)((a1 + 469)))))));
            goto LABEL_220;
        }
        if ((v36 == 0x10000))
        {
            (v59 = r_u32(((uint32)((a1 + 516)))));
            (v60 = r_u32(((uint32)((a1 + 4)))));
            w_u16(((uint32)((a1 + 580))), 0);
            (v61 = r_u32(((uint32)((v59 + 296)))));
            if ((sint32)player_update_abs((sint32)((uint32)v61 - (uint32)v60) >> 3) <= 0x7FFF)
                w_u32(((uint32)((a1 + 4))), v61);
            else
                w_u32(((uint32)((a1 + 4))), (v60 + ((v61 - v60) >> 3)));
            (v62 = r_u32(((uint32)((a1 + 12)))));
            (v32 = r_u32(((uint32)((r_u32(((uint32)((a1 + 516)))) + 304)))));
            if ((sint32)player_update_abs((sint32)((uint32)v32 - (uint32)v62) >> 3) <= 0x7FFF)
                w_u32(((uint32)((a1 + 12))), v32);
            else
                w_u32(((uint32)((a1 + 12))), (v62 + ((v32 - v62) >> 3)));
            if (((((uint32)(r_u8(((uint32)((a1 + 24)))))) - 25) < 0x19))
            {
                (v63 = r_u32(((uint32)((a1 + 516)))));
                if (v63)
                {
                    w_u16(((uint32)((v63 + 20))), ((r_u16(((uint32)((v63 + 20)))) - 28) & 0xFFF));
                    if ((r_u8(((uint32)((a1 + 24)))) == 32))
                        sub_80062044(r_u32(a1 + 516u));
                }
            }
            if (r_u8(((uint32)((a1 + 303)))))
                w_u32(((uint32)((a1 + 460))), 1);
            goto LABEL_231;
        }
        if ((v36 <= 0x10000))
        {
            (v37 = 0);
            if ((v36 == 0x8000))
            {
                w_u16(((uint32)((a1 + 580))), 0);
                goto LABEL_231;
            }
        LABEL_232:
            (v119 = 0);

            if ((r_u8(((uint32)((a1 + 468)))) | r_u8(((uint32)((a1 + 469))))))
            {
                (v120 = r_u16(((uint32)((a1 + 474)))));
                (v121 = (v120 << 16));
                if ((((uint32)((v120 - 257))) < 0xDFF))
                {
                    (v120 = ((short)(v120)));
                    (v122 = ((v121 >> 16) - 256));
                    (v4 = (v122 >= 0));
                    (v123 = (v122 >> 9));
                    if (!v4)
                        (v123 = ((v120 + 255) >> 9));
                    (v37 = (v123 + 2));
                }
                else
                {
                    (v37 = 1);
                }
            }
            (v124 = 0);
            if ((r_u8(((uint32)((a1 + 471)))) | r_u8(((uint32)((a1 + 472))))))
            {
                (v125 = r_u16(((uint32)((a1 + 476)))));
                (v126 = (v125 << 16));
                if ((((uint32)((v125 - 257))) < 0xDFF))
                {
                    (v125 = ((short)(v125)));
                    (v127 = ((v126 >> 16) - 256));
                    (v4 = (v127 >= 0));
                    (v128 = (v127 >> 9));
                    if (!v4)
                        (v128 = ((v125 + 255) >> 9));
                    (v119 = (v128 + 2));
                    (v124 = 0);
                }
                else
                {
                    (v119 = 1);
                }
            }
            (v129 = (v37 - 1));
            (v130 = (v119 - 1));
            if (r_u32(0x800FF904u))
                (v124 = r_u16(((uint32)((r_u32(0x800FF904u) + 494)))));
            (v131 = r_u32(((uint32)((a1 + 460)))));
            if (((v131 & 0x630) != 0))
            {
                if (v119)
                {
                    (v136 = (((uint32)((0x800A6FD8u + ((12 * v129)) * 4u))) + (v130) * 1u));
                    (v137 = r_u8(v136));
                    (v138 = ((sint8)(r_u8((v136 + (8) * 1u)))));
                    if (((v137 != r_u8(((uint32)((a1 + 26))))) || ((v132 = a1), (v138 != ((sint8)(r_u8(((uint32)((a1 + 297))))))))))
                    {
                        sub_80063118(a1, v137, v138);
                        (v132 = a1);
                    }
                    (v134 = 0);
                    (v139 = (((uint32)((0x800A6FD8u + ((12 * v129)) * 4u))) + ((2 * v130)) * 1u));
                    (v133 = ((sint16)(r_u16((((uint32)(v139)) + (16) * 2u)))));
                    (v135 = ((short)((((r_u16(((uint32)((a1 + 474)))) + v124) + r_u16((((uint32)(v139)) + (8) * 2u))) - (((uint16)(v129)) << 9)))));
                }
                else
                {
                    (v132 = a1);
                    if (((r_u8(((uint32)((a1 + 26)))) != 1) || ((v133 = 0), (r_u8(((uint32)((a1 + 297)))) != 1))))
                    {
                        sub_80063118(a1, 1, 1);
                        (v132 = a1);
                        (v133 = 0);
                    }
                    (v134 = 0);
                    (v135 = ((short)((v124 + r_u16(((uint32)((a1 + 474))))))));
                }
                goto LABEL_296;
            }
            if ((v131 != 1))
            {
                if ((v131 == 256))
                {
                    (v132 = a1);
                    if (v119)
                    {
                        (v143 = (((uint32)(0x800A6FD8u)) + ((2 * v130)) * 1u));
                        (v134 = 0);
                        (v133 = (((r_u16((((uint32)(v143)) + (232) * 2u)) + r_u16(((uint32)((a1 + 476))))) - (((uint16)(v130)) << 9)) & 0xFFF));
                        (v135 = ((short)((v124 + r_u16((((uint32)(v143)) + (224) * 2u))))));
                    }
                    else
                    {
                        (v133 = 0);
                        (v135 = ((sint16)(r_u16(((uint32)((a1 + 18)))))));
                        (v134 = 0);
                    }
                    goto LABEL_296;
                }
                if (((v131 & 0x8E) != 0))
                {
                    (v132 = a1);
                    if (v119)
                    {
                        (v144 = (((uint32)(0x800A6FD8u)) + ((2 * v130)) * 1u));
                        (v134 = 0);
                        (v133 = (((r_u16((((uint32)(v144)) + (232) * 2u)) + r_u16(((uint32)((a1 + 476))))) - (((uint16)(v130)) << 9)) & 0xFFF));
                        (v135 = ((short)((v124 + r_u16((((uint32)(v144)) + (224) * 2u))))));
                    }
                    else
                    {
                        (v133 = 0);
                        (v134 = 0);
                        (v135 = ((short)((v124 + r_u16(((uint32)((a1 + 474))))))));
                    }
                    goto LABEL_296;
                }
                if ((v131 == 0x4000))
                {
                    if ((v119 || !((r_u8(((uint32)((a1 + 468)))) | r_u8(((uint32)((a1 + 469))))))))
                    {
                        if ((r_u8(((uint32)((a1 + 26)))) != 14))
                            sub_80063038(a1, 14, 0, -1);
                        (v132 = a1);
                        if (!v119)
                        {
                        LABEL_297:
                            if (r_u32(((uint32)((a1 + 440)))))
                            {
                                w_u16(((uint32)((r_u32(((uint32)((a1 + 360)))) + 86))), ((r_u16(((uint32)((r_u32(((uint32)((a1 + 360)))) + 86)))) + r_u16(((uint32)((a1 + 436))))) & 0xFFF));
                                (v4 = ((w_u32(((uint32)((a1 + 440))), (r_u32(((uint32)((a1 + 440)))) - 1u)), r_u32(((uint32)((a1 + 440))))) != 1));
                                if (!v4)
                                    w_u16(((uint32)((r_u32(((uint32)((a1 + 360)))) + 86))), r_u16(((uint32)((a1 + 432)))));
                            }

                            (v147 = r_u32(((uint32)((a1 + 460)))));
                            w_u32(((uint32)((a1 + 124))), 0);
                            w_u32(((uint32)((a1 + 116))), 0);
                            if ((v147 != 0x4000))
                            {
                                if ((v147 >= 0x4001))
                                {
                                    if ((v147 != 0x8000))
                                    {
                                    LABEL_306:
                                        w_u32(((uint32)((a1 + 120))), (r_u16(0x800EC4B0u) << 12));

                                    LABEL_307:
                                        if (((r_u32(((uint32)((a1 + 460)))) & 0x80000) == 0))
                                        {
                                            (v152 = r_u16(0x800FF5E8u));
                                            w_u32(a1 + 132u, r_u32(0x800FF5E4u));
                                            w_u16(((uint32)((a1 + 136))), v152);
                                            (v153 = r_u16(0x800FF5E8u));
                                            w_u32(a1 + 138u, r_u32(0x800FF5E4u));
                                            w_u16(((uint32)((a1 + 142))), v153);
                                        }

                                        (v154 = 0);
                                        if (r_u32(0x800FF904u))
                                        {
                                            (v222 = r_u32(((uint32)((r_u32(0x800FF904u) + 492)))));
                                            v223 = r_u16(r_u32(0x800FF904u) + 496u);
                                            RotMatrix((SVECTOR *)&v222, &v221);
                                        }
                                        { uint32 r, c; for (r = 0; r < 3u; ++r) for (c = 0; c < 3u; ++c) v220.m[r][c] = v221.m[c][r]; }
                                        SetRotMatrix(&v220);
                                        (v237 = 6);
                                        xport_draft_host_sub_8006C564_p13(v224, a1 + 104u, &v237);

                                        (v216 = v224[0]);
                                        (v217 = v224[1]);
                                        (v218 = v225);
                                        xport_gte_write_data(9u, (uint32)v216);
                                        xport_gte_write_data(10u, (uint32)v217);
                                        xport_gte_write_data(11u, (uint32)v218);
                                        xport_gte_execute(0x49E012u);
                                        v216 = (sint32)xport_gte_read_data(25u);
                                        v217 = (sint32)xport_gte_read_data(26u);
                                        v218 = (sint32)xport_gte_read_data(27u);
                                        (v238 = 6);
                                        xport_draft_host_sub_8006C22C_p12(&v216, &v238);
                                        (v156 = r_u32(((uint32)((a1 + 460)))));
                                        if (((v156 & 0x71100) == 0))
                                        {
                                            switch (v156)
                                            {
                                                case 0x800:
                                                    if ((((sint8)(r_u8(((uint32)((a1 + 24)))))) < 20))
                                                    {
v216 = (sint32)(0u - (r_u16(0x800EC4C0u) * (uint32)(sint32)(sint16)r_u16(0x800F863Cu + 4u * (r_u16(a1 + 606u) & 0xFFFu))));
                                                        (v154 = 1);
v218 = (sint32)(0u - (r_u16(0x800EC4C0u) * (uint32)(sint32)(sint16)r_u16(0x800F863Cu + 4u * (r_u16(a1 + 606u) & 0xFFFu) + 2u)));
                                                    }
                                                    break;

                                                case 0x4000:
                                                    if ((!((r_u8(((uint32)((a1 + 471)))) | r_u8(((uint32)((a1 + 472)))))) && r_u16(((uint32)((a1 + 468))))))
                                                    {
                                                        if (((((r_u16(((uint32)((r_u32(((uint32)((a1 + 512)))) + 320)))) - (v124 + r_u16(((uint32)((a1 + 474)))))) & 0xFFFu) - 1025) < 0x7FF))
                                                        {
                                                            (v159 = r_u16(0x800EC4C0u));
                                                            (v160 = r_u32(((uint32)((a1 + 512)))));
                                                            w_u32(((uint32)((a1 + 304))), 0x20000);
                                                            (v1 = (v159 / 4));
                                                            (v158 = ((r_u16(((uint32)((v160 + 320)))) + 2048) & 0xFFF));
                                                        }
                                                        else
                                                        {
                                                            (v157 = r_u16(0x800EC4C0u));
                                                            w_u32(((uint32)((a1 + 304))), 0x20000);
                                                            (v158 = r_u16(((uint32)((r_u32(((uint32)((a1 + 512)))) + 320)))));
                                                            (v1 = (v157 / -4));
                                                        }
                                                        w_u16(((uint32)((a1 + 18))), v158);
                                                        (v154 = 1);
                                                    }
                                                    break;

                                                case 0x8000:
v216 = (r_u16(0x800EC4C0u) / 4u) * 8u * (uint32)(sint32)(sint16)r_u16(0x800F863Cu + 4u * ((r_u16(r_u32(a1 + 512u) + 320u) - v124) & 0xFFFu));
                                                    (v154 = 1);
v218 = (r_u16(0x800EC4C0u) / 4u) * 8u * (uint32)(sint32)(sint16)r_u16(0x800F863Cu + 4u * ((r_u16(r_u32(a1 + 512u) + 320u) - v124) & 0xFFFu) + 2u);
                                                    break;

                                                default:
                                                    v161 = ((unsigned long long)r_u8(a1 + 468u) << 32) | r_u8(a1 + 469u);

                                                    if (v161)
                                                    {
                                                        v162 = player_update_abs(((sint32)(sint8)r_u8(a1 + 468u) * 4096) / 96) + player_update_abs(((sint32)(sint8)r_u8(a1 + 469u) * 4096) / 96);
                                                        if ((v162 >= 4097))
                                                            (v162 = 4096);
                                                        if ((((unsigned long long)(v161) >> 32) & 0xFFu))
                                                        {
v218 = (sint32)(0u - ((uint32)((sint32)((uint32)(sint32)(sint16)r_u16(0x800F863Cu + 4u * (r_u16(a1 + 474u) & 0xFFFu) + 2u) * (uint32)v162) >> 12) * r_u16(0x800EC4C0u)));
                                                            (v218 = ((v218 * r_u32(((uint32)((a1 + 608))))) / 8));
                                                            (v154 = 1);
                                                        }
                                                        if (r_u8(((uint32)((a1 + 469)))))
                                                        {
v216 = (sint32)(0u - ((uint32)((sint32)((uint32)(sint32)(sint16)r_u16(0x800F863Cu + 4u * (r_u16(a1 + 474u) & 0xFFFu)) * (uint32)v162) >> 12) * r_u16(0x800EC4C0u)));
                                                            (v216 = ((v216 * r_u32(((uint32)((a1 + 608))))) / 8));
                                                            (v154 = 1);
                                                        }
                                                    }
                                                    else if (!(r_u32(((uint32)((a1 + 528))))))
                                                    {
                                                        (v154 = 1);
                                                        (v216 = 0);
                                                        (v218 = 0);
                                                    }
                                                    break;
                                            }
                                        }
                                        SetRotMatrix(&v221);
                                        if (v154)
                                        {
                                            (v239 = 6);
                                            xport_draft_host_sub_8006C564_p123(&v222, &v216, &v239);

                                            (v164 = v223);
                                            (v165 = v224[0]);
                                            w_u32(((uint32)((a1 + 104))), v222);
                                            w_u32(((uint32)((a1 + 108))), v164);
                                            w_u32(((uint32)((a1 + 112))), v165);
                                            xport_gte_write_data(9u, r_u32(a1 + 104u));
                                            xport_gte_write_data(10u, r_u32(a1 + 108u));
                                            xport_gte_write_data(11u, r_u32(a1 + 112u));
                                            xport_gte_execute(0x49E012u);
                                            w_u32(a1 + 104u, xport_gte_read_data(25u));
                                            w_u32(a1 + 108u, xport_gte_read_data(26u));
                                            w_u32(a1 + 112u, xport_gte_read_data(27u));
                                            (v240 = 6);
                                            xport_draft_host_sub_8006C22C_p2(a1 + 104u, &v240);
                                            (v166 = r_u32(((uint32)((a1 + 460)))));
                                            if (((v166 & 0x8000) != 0))
                                            {
                                                sub_8006C0B8((a1 + 4), (a1 + 104));
                                                sub_8006C0B8((a1 + 496), (a1 + 104));
                                                if (!player_update_missing("sub_80019318 host output") || r_u8(r_u32(a1 + 444u) + 273u))
                                                {
                                                    w_u8(((uint32)((r_u32(((uint32)((a1 + 444)))) + 273))), 0);
                                                    (v167 = r_u32(((uint32)((a1 + 492)))));
                                                    w_u32(((uint32)((a1 + 460))), 4);
                                                    w_u32(((uint32)((a1 + 508))), 0);
                                                    sub_8006A294(v167);
                                                    goto LABEL_347;
                                                }
                                            LABEL_342:
                                                (v168 = v223);

                                                (v169 = v224[0]);
                                                w_u32(((uint32)((a1 + 4))), v222);
                                                w_u32(((uint32)((a1 + 8))), v168);
                                                w_u32(((uint32)((a1 + 12))), v169);
                                                (v170 = r_u32(((uint32)((a1 + 8)))));
                                                (v171 = r_u32(((uint32)((a1 + 12)))));
                                                w_u32(((uint32)((a1 + 496))), r_u32(((uint32)((a1 + 4)))));
                                                w_u32(((uint32)((a1 + 500))), v170);
                                                w_u32(((uint32)((a1 + 504))), v171);
                                                w_u32(((uint32)((a1 + 8))), (r_u32(((uint32)((a1 + 8)))) + (0x80000)));
                                                goto LABEL_347;
                                            }
                                            if (((v166 & 0x4000) == 0))
                                                goto LABEL_347;
                                            if (player_update_missing("sub_8001922C host output") && !r_u8(r_u32(a1 + 444u) + 273u))
                                            {
                                                goto LABEL_342;
                                            }
                                            w_u8(((uint32)((r_u32(((uint32)((a1 + 444)))) + 273))), 0);
                                        }
                                        else
                                        {
                                            if ((((r_u32(((uint32)((a1 + 460)))) & 0xC000) == 0) || ((v172 = r_u32(((uint32)((a1 + 444))))), !(r_u8(((uint32)((v172 + 273))))))))
                                            {
                                            LABEL_347:
                                                if (!(r_u32(((uint32)((a1 + 428))))))
                                                    goto LABEL_360;

                                                (v173 = 0);
                                                (v174 = r_u32(((uint32)((a1 + 8)))));
                                                (v175 = r_u32(((uint32)((a1 + 12)))));
                                                (v222 = r_u32(((uint32)((a1 + 4)))));
                                                (v223 = v174);
                                                (v224[0] = v175);
                                                if (((r_u32(((uint32)((a1 + 460)))) & 0x3000) != 0))
                                                {
                                                    sub_8006C0B8((a1 + 4), (r_u32(((uint32)((a1 + 428)))) + 104));
                                                    (v173 = 1);
                                                    goto LABEL_357;
                                                }
                                                if (r_u16(((uint32)((a1 + 570)))))
                                                {
                                                    if (((r_u8(((uint32)((a1 + 468)))) | r_u8(((uint32)((a1 + 469))))) || ((v176 = r_u32(((uint32)((a1 + 428))))), (v177 = r_u32(((uint32)((v176 + 104))))), !((v177 | r_u32(((uint32)((v176 + 112)))))))))
                                                    {
                                                    LABEL_357:
                                                        if (v173)
                                                        {
                                                            (v227[0] = v222);
                                                            (v227[1] = v223);
                                                            (v228 = v224[0]);
                                                            (v229 = r_u32(((uint32)((a1 + 4)))));
                                                            (v230 = r_u32(((uint32)((a1 + 8)))));
                                                            (v231 = r_u32(((uint32)((a1 + 12)))));
                                                            xport_draft_host_sub_8007BB24_p1(v227);
                                                            xport_draft_host_sub_8007DD04_p1(v227, 1u);
                                                            if (v233)
                                                            {
                                                                (v184 = v223);
                                                                (v185 = v224[0]);
                                                                w_u32(((uint32)((a1 + 4))), v222);
                                                                w_u32(((uint32)((a1 + 8))), v184);
                                                                w_u32(((uint32)((a1 + 12))), v185);
                                                            }
                                                        }

                                                    LABEL_360:
                                                        if (((r_u16(((uint32)((a1 + 216)))) & 0x100) != 0))
                                                            w_u32(0x800FF5A8u, 1);

                                                        if (((r_u8(((uint32)((a1 + 568)))) && r_u8(((uint32)((r_u32(((uint32)((a1 + 444)))) + 272))))) && !r_u32(0x800FF5A8u)))
                                                            w_u32(((uint32)((a1 + 108))), r_u32(((uint32)((a1 + 564)))));
                                                        (v186 = 74);
                                                        if (((r_u16(((uint32)(a1))) & 8) != 0))
                                                        {
                                                            if ((r_u32(0x800FF5D4u) == 74))
                                                                (v186 = 75);
                                                        }
                                                        else
                                                        {
                                                            do
                                                                (v186 = (sub_80066570(4) + 31));
                                                            while ((v186 == r_u32(0x800FF5D4u)));
                                                        }
                                                        (v187 = r_u8(((uint32)((a1 + 26)))));
                                                        switch (v187)
                                                        {
                                                            case 1:
                                                                (v188 = sub_8006325C(a1, 1, r_u16(0x800EC5E4u)));
                                                                (v189 = sub_80062D24(a1, v188, 1));
                                                                (v190 = v186);
                                                                if (!v189)
                                                                {
                                                                    (v191 = a1);
                                                                    (v192 = r_u16(0x800EC5E6u));
                                                                    (v193 = 1);
                                                                    break;
                                                                }
                                                            LABEL_383:
                                                                sub_80069DF0(v190, 0x3FFF, 0);

                                                                w_u32(0x800FF5D4u, v186);
                                                            LABEL_384:
                                                                (v202 = r_u8(((uint32)((a1 + 568)))));

                                                                (v4 = (v202 == 0));
                                                                (v203 = (v202 - 1));
                                                                if (!v4)
                                                                    w_u8(((uint32)((a1 + 568))), v203);
                                                                (v204 = r_u16(((uint32)((a1 + 574)))));
                                                                if (r_u16(((uint32)((a1 + 574)))))
                                                                {
                                                                    (v205 = (v204 << 8));
                                                                    if ((r_u16(((uint32)((a1 + 574)))) >= 0x81u))
                                                                    {
                                                                        (v204 = 128);
                                                                        (v205 = 0x8000);
                                                                    }
                                                                    w_u32(((uint32)((a1 + 176))), (((v204 | v205) | 0x6000000) | (v204 << 17)));
                                                                    w_u16(((uint32)((a1 + 180))), 0x2000);
                                                                    (v206 = r_u16(((uint32)((a1 + 574)))));
                                                                    w_u16(((uint32)((a1 + 182))), 10);
                                                                    w_u16(((uint32)((a1 + 574))), --v206);
                                                                    (v207 = (a1 + 228));
                                                                    if (v206)
                                                                    {
                                                                    LABEL_392:
                                                                        if (!sub_8006C304(v207, a1 + 4u))
                                                                        {
                                                                        LABEL_399:
                                                                            if (!r_u32(0x800FF5D8u))
                                                                                w_u32(0x800FF5D8u, 1);

                                                                            sub_8006C3AC(&v222, (a1 + 4), (a1 + 228));
                                                                            (v211 = a1);
                                                                            if ((r_u32(((uint32)((a1 + 460)))) != 0x40000))
                                                                            {
                                                                                if (((v222 | v223) | v224[0]))
                                                                                {
                                                                                    w_u32(0x800FFE90u, v222);
                                                                                    w_u32(0x800FFE94u, v223);
                                                                                    w_u32(0x800FFE98u, v224[0]);
                                                                                }
                                                                                sub_800666DC(&v225, 0x800FFE90u);
                                                                                (v241 = 16);
                                                                                sub_8006C40C(v232, &v225, &v241);
                                                                                sub_8006C3AC(&v228, (a1 + 228), v232);
                                                                                (v234[0] = v228);
                                                                                (v234[1] = v229);
                                                                                (v234[2] = v230);
                                                                                (v234[3] = r_u32(((uint32)((a1 + 4)))));
                                                                                (v234[4] = r_u32(((uint32)((a1 + 8)))));
                                                                                (v234[5] = r_u32(((uint32)((a1 + 12)))));
                                                                                xport_draft_host_sub_8007BB24_p1(v234);
                                                                                w_u32(0x800FF978u, 0);
                                                                                ((uint8 *)v234)[136] = 1u;
                                                                                xport_draft_host_sub_8007DD04_p1(v234, 1u);
                                                                                w_u32(0x800FF978u, 0);
                                                                                (v211 = a1);
                                                                            }
                                                                            sub_8006331C(v211, 0x800A6F20u);
                                                                            (v225 = ((v225 & 0xFFFF0000u) | (((0) & 0xFFFFu) << 0)));
                                                                            (v225 = ((v225 & 0x0000FFFFu) | (((v124) & 0xFFFFu) << 16)));
                                                                            (v226 = 0);
                                                                            if ((r_u8(((uint32)((a1 + 470)))) && (r_u8(((uint32)((a1 + 471)))) | r_u8(((uint32)((a1 + 472)))))))
                                                                            {
                                                                                (v225 = ((v225 & 0x0000FFFFu) | (((((v124 + r_u16(((uint32)((a1 + 476))))) & 0xFFF)) & 0xFFFFu) << 16)));
                                                                            }
                                                                            else
                                                                            {
                                                                                (v225 = ((v225 & 0x0000FFFFu) | ((((r_u16(((uint32)((a1 + 18)))) + r_u16(((uint32)((r_u32(((uint32)((a1 + 360)))) + 86)))))) & 0xFFFFu) << 16)));
                                                                            }
                                                                            apocalypse_player_action(a1, (const sint16 *)&v225, r_u8(a1 + 572u));
                                                                            sub_8005F470(a1);
                                                                            if ((((sint16)(r_u16(((uint32)((a1 + 218)))))) < 0))
                                                                                w_u16(((uint32)((a1 + 218))), 0);
                                                                            if (!(r_u32(((uint32)((a1 + 508))))))
                                                                                w_u32(((uint32)((a1 + 512))), 0);
                                                                            if (((r_u32(((uint32)((a1 + 460)))) & 0x180000) == 0))
                                                                                w_u32(((uint32)((a1 + 524))), 0);
                                                                            (v212 = r_u32(((uint32)((a1 + 460)))));
                                                                            (result = 0x20000);
                                                                            if ((v212 != 0x10000))
                                                                            {
                                                                                w_u32(((uint32)((a1 + 516))), 0);
                                                                                (v212 = r_u32(((uint32)((a1 + 460)))));
                                                                            }
                                                                            if ((v212 != 0x20000))
                                                                                w_u32(((uint32)((a1 + 520))), 0);
                                                                            w_u32(((uint32)((a1 + 528))), 0);
                                                                            return result;
                                                                        }

                                                                        (v208 = sub_80067508(v207, (a1 + 4), r_u32(0x800FF4E8u)));
                                                                        if (v208)
                                                                        {
                                                                            (v209 = (a1 + 228));
                                                                            if ((r_u16(((uint32)((v208 + 58)))) == 201))
                                                                                goto LABEL_397;
                                                                            sub_8005E07C(a1, v208);
                                                                        }
                                                                        (v209 = (a1 + 228));
                                                                    LABEL_397:
                                                                        (v210 = sub_80067508(v209, (a1 + 4), r_u32(0x800FF204u)));

                                                                        if (v210)
                                                                            sub_800206C4(v210, a1);
                                                                        goto LABEL_399;
                                                                    }
                                                                    w_u32(((uint32)((a1 + 176))), 0);
                                                                }
                                                                (v207 = (a1 + 228));
                                                                goto LABEL_392;

                                                            case 3:
                                                                (v194 = sub_8006325C(a1, 3, r_u16(0x800EC5E4u)));
                                                                (v195 = sub_80062D24(a1, v194, 1));
                                                                (v190 = v186);
                                                                if (v195)
                                                                    goto LABEL_383;
                                                                (v191 = a1);
                                                                (v192 = r_u16(0x800EC5E6u));
                                                                (v193 = 3);
                                                                break;

                                                            case 4:
                                                                (v196 = sub_8006325C(a1, 4, r_u16(0x800EC5E4u)));
                                                                (v197 = sub_80062D24(a1, v196, 1));
                                                                (v190 = v186);
                                                                if (v197)
                                                                    goto LABEL_383;
                                                                (v191 = a1);
                                                                (v192 = r_u16(0x800EC5E6u));
                                                                (v193 = 4);
                                                                break;

                                                            case 8:
                                                                (v198 = sub_8006325C(a1, 8, 7));
                                                                (v199 = sub_80062D24(a1, v198, 1));
                                                                (v190 = v186);
                                                                if (v199)
                                                                    goto LABEL_383;
                                                                (v191 = a1);
                                                                (v193 = 8);
                                                                (v192 = 18);
                                                                break;

                                                            default:
                                                                goto LABEL_384;
                                                        }

                                                        (v200 = sub_8006325C(v191, v193, v192));
                                                        (v201 = sub_80062D24(a1, v200, 2));
                                                        (v190 = v186);
                                                        if (!v201)
                                                            goto LABEL_384;
                                                        goto LABEL_383;
                                                    }
                                                    (v178 = (r_u32(((uint32)((a1 + 4)))) + v177));
                                                    (v179 = r_u32(((uint32)((a1 + 428)))));
                                                    w_u32(((uint32)((a1 + 4))), v178);
                                                }
                                                else
                                                {
                                                    (v180 = r_u32(((uint32)((a1 + 428)))));
                                                    (v181 = r_u32((v180 + (26) * 4u)));
                                                    if (!(((v181 | r_u32((v180 + (27) * 4u))) | r_u32((v180 + (28) * 4u)))))
                                                        goto LABEL_357;
                                                    (v182 = r_u32(((uint32)((a1 + 428)))));
                                                    w_u32(((uint32)((a1 + 4))), (r_u32(((uint32)((a1 + 4)))) + (v181)));
                                                    (v183 = (r_u32(((uint32)((a1 + 8)))) + r_u32(((uint32)((v182 + 108))))));
                                                    (v179 = r_u32(((uint32)((a1 + 428)))));
                                                    w_u32(((uint32)((a1 + 8))), v183);
                                                }
                                                (v173 = 1);
                                                w_u32(((uint32)((a1 + 12))), (r_u32(((uint32)((a1 + 12)))) + (r_u32(((uint32)((v179 + 112)))))));
                                                goto LABEL_357;
                                            }
                                            w_u8(((uint32)((v172 + 273))), 0);
                                        }
                                        w_u32(((uint32)((a1 + 460))), 4);
                                        w_u32(((uint32)((a1 + 508))), 0);
                                        goto LABEL_347;
                                    }
                                }
                                else if ((v147 != 4096))
                                {
                                    goto LABEL_306;
                                }
                            }
                            (v148 = r_u32(0x800A71D0u));
                            (v149 = r_u32(0x800A71D4u));
                            w_u32(((uint32)((a1 + 104))), 0x800A71CCu);
                            w_u32(((uint32)((a1 + 108))), v148);
                            w_u32(((uint32)((a1 + 112))), v149);
                            (v150 = r_u32(0x800A71D0u));
                            (v151 = r_u32(0x800A71D4u));
                            w_u32(((uint32)((a1 + 116))), 0x800A71CCu);
                            w_u32(((uint32)((a1 + 120))), v150);
                            w_u32(((uint32)((a1 + 124))), v151);
                            goto LABEL_307;
                        }
                        (v145 = (((uint32)(0x800A6FD8u)) + ((2 * v130)) * 1u));
                        (v134 = 1);
                        (v133 = (((r_u16((((uint32)(v145)) + (232) * 2u)) + r_u16(((uint32)((a1 + 476))))) - (((uint16)(v130)) << 9)) & 0xFFF));
                        (v135 = ((short)((v124 + r_u16((((uint32)(v145)) + (224) * 2u))))));
                    LABEL_296:
                        sub_80061A78(v132, v133, v135, v134);

                        goto LABEL_297;
                    }
                    if ((r_u8(((uint32)((a1 + 26)))) != 13))
                        sub_80063118(a1, 13, 1);
                    (v132 = a1);
                    (v135 = ((sint16)(r_u16(((uint32)((a1 + 18)))))));
                    (v133 = 0);
                }
                else if ((v131 == 0x8000))
                {
                    if ((r_u8(((uint32)((a1 + 26)))) != 14))
                        sub_80063038(a1, 14, 0, -1);
                    (v132 = a1);
                    if (v119)
                    {
                        (v146 = (((uint32)(0x800A6FD8u)) + ((2 * v130)) * 1u));
                        (v134 = 1);
                        (v133 = ((short)(((r_u16((((uint32)(v146)) + (232) * 2u)) + r_u16(((uint32)((a1 + 476))))) - (((uint16)(v130)) << 9)))));
                        (v135 = ((short)((v124 + r_u16((((uint32)(v146)) + (224) * 2u))))));
                        goto LABEL_296;
                    }
                    (v135 = ((sint16)(r_u16(((uint32)((a1 + 18)))))));
                    (v133 = 0);
                }
                else
                {
                    (v132 = a1);
                    if (((v131 & 0x1F3800) != 0))
                    {
                        (v135 = ((sint16)(r_u16(((uint32)((a1 + 18)))))));
                        (v133 = 0);
                    }
                    else
                    {
                        (v133 = 0);
                        (v135 = v124);
                    }
                }
                (v134 = 1);
                goto LABEL_296;
            }
            if (v119)
            {
                if ((r_u8(((uint32)((a1 + 26)))) != 9))
                    sub_80063118(a1, 9, 1);
                (v132 = a1);
                (v142 = (((uint32)(0x800A6FD8u)) + ((2 * v130)) * 1u));
                (v134 = 0);
                (v133 = (((r_u16((((uint32)(v142)) + (208) * 2u)) + r_u16(((uint32)((a1 + 476))))) - (((uint16)(v130)) << 9)) & 0xFFF));
                (v135 = ((short)((v124 + r_u16((((uint32)(v142)) + (200) * 2u))))));
                goto LABEL_296;
            }
            (v140 = r_u8(((uint32)((a1 + 26)))));
            if ((v140 == 6))
            {
                (v141 = a1);
                if (!(r_u8(((uint32)((a1 + 303))))))
                {
                LABEL_264:
                    (v132 = a1);

                    (v133 = 0);
                    (v135 = ((sint16)(r_u16(((uint32)((a1 + 18)))))));
                    (v134 = 0);
                    goto LABEL_296;
                }
            }
            else
            {
                if (!(r_u8(((uint32)((a1 + 26))))))
                    goto LABEL_264;
                (v141 = a1);
                if ((v140 == 5))
                    goto LABEL_264;
            }
            sub_80063038(v141, 0, 0, -1);
            goto LABEL_264;
        }
        if ((v36 == 0x20000))
        {
            (v64 = r_u32(((uint32)((a1 + 520)))));
            (v65 = r_u32(((uint32)((a1 + 4)))));
            w_u16(((uint32)((a1 + 580))), 0);
            (v66 = r_u32(((uint32)((v64 + 308)))));
            if ((sint32)player_update_abs((sint32)((uint32)v66 - (uint32)v65) >> 3) <= 0x7FFF)
                w_u32(((uint32)((a1 + 4))), v66);
            else
                w_u32(((uint32)((a1 + 4))), (v65 + ((v66 - v65) >> 3)));
            (v67 = r_u32(((uint32)((a1 + 12)))));
            (v32 = r_u32(((uint32)((r_u32(((uint32)((a1 + 520)))) + 316)))));
            if ((sint32)player_update_abs((sint32)((uint32)v32 - (uint32)v67) >> 3) <= 0x7FFF)
                w_u32(((uint32)((a1 + 12))), v32);
            else
                w_u32(((uint32)((a1 + 12))), (v67 + ((v32 - v67) >> 3)));
            if (((r_u8(((uint32)((a1 + 24)))) == 14) && (r_u8(((uint32)((a1 + 297)))) == 1)))
                sub_80062044(r_u32(((uint32)((a1 + 520)))));
            if (r_u8(((uint32)((a1 + 303)))))
            {
                if ((r_u8(((uint32)((a1 + 297)))) == 1))
                {
                    sub_80063038(a1, 18, ((sint8)(r_u8(((uint32)((a1 + 24)))))), 0);
                    (v37 = 0);
                    goto LABEL_232;
                }
                w_u32(((uint32)((a1 + 460))), 1);
            }
            goto LABEL_231;
        }
        (v37 = 0);
        if ((v36 != 0x40000))
            goto LABEL_232;
        (v68 = r_u16(((uint32)((a1 + 652)))));
        w_u16(((uint32)((a1 + 580))), 0);
        (v69 = (v68 + 1));
        w_u16(((uint32)((a1 + 652))), v69);
        if ((((v69 == 32) || (v69 == 64)) || (v69 == 96)))
        {
            if (r_u32(0x800FF904u))
            {
                (v32 = 34);
                if (r_u32(((uint32)((a1 + 448)))))
                    sub_8007851C(r_u32(0x800FF904u), ((r_u16(((uint32)((r_u32(0x800FF904u) + 494)))) + 1024) & 0xFFF), 34);
            }
            if ((r_u16(((uint32)((a1 + 652)))) == 64))
                sub_8001BEF8(128, 1);
        }
        if (!(r_u8(((uint32)((a1 + 303))))))
        {
            w_u32(0x800FFBE4u, 0);
            (v37 = 0);
            goto LABEL_232;
        }
        if ((r_u32(0x800FF874u) == 1))
        {
            if ((r_u32(0x800FFBE4u) < 128))
                goto LABEL_157;
        }
        else if ((r_u32(0x800FFBE4u) < 64))
        {
        LABEL_157:
            (w_u32(0x800FFBE4u, (r_u32(0x800FFBE4u) + 1u)), (r_u32(0x800FFBE4u) + 1u));

            (v37 = 0);
            goto LABEL_232;
        }
apocalypse_object_virtual20(r_u32(r_u32(a1 + 68u) + 20u), a1 + (uint32)(sint32)(sint16)r_u16(r_u32(a1 + 68u) + 16u));
        sub_8001BE54();
        goto LABEL_157;
    }
    return result;
}

#undef v216
#undef v217
#undef v218
#undef v219
#undef v222
#undef v223
#undef v224
#undef v225
#undef v226

#undef v227
#undef v228
#undef v229
#undef v230
#undef v231
#undef v232
#undef v233
