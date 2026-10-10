#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
#include <stdlib.h>

/* Native renderer ignores incidental MIPS return values */
static uint32 renderer_3736C_divu(uint32 numerator, uint32 denominator)
{
    return denominator == 0u ? 0xFFFFFFFFu : numerator / denominator;
}

static sint32 renderer_3736C_div(uint32 numerator, uint32 denominator)
{
    sint32 dividend = (sint32)numerator;
    sint32 divisor = (sint32)denominator;
    if (divisor == 0)
        return dividend < 0 ? 1 : -1;
    if (dividend == (sint32)0x80000000u && divisor == -1)
        return dividend;
    return dividend / divisor;
}

void sub_8003736C(void)
{
    uint32 gte_T3;
    uint32 gte_T4;
    uint32 gte_T5;
    uint32 gte_T6;
    uint32 gte_T7;
    uint32 gte_T8;
    uint32 gte_T9;
    int vec303[3];
    int vec300[3];
    int vec297[3];
    sint32 v5;
    sint32 result;
    sint32 j;
    sint32 v10;
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
    uint32 v24;
    uint32 v25;
    sint32 v26;
    uint32 v27;
    uint32 v28;
    uint32 v29;
    sint32 v30;
    sint32 v31;
    sint32 v32;
    sint32 v33;
    uint32 v34;
    sint32 v35;
    sint32 v36;
    sint32 v37;
    sint32 v38;
    sint32 v39;
    sint32 v40;
    sint32 v41;
    sint32 v42;
    sint32 v43;
    uint32 v44;
    uint32 v45;
    uint32 v46;
    sint32 v47;
    sint32 v48;
    sint32 v49;
    sint32 v50;
    uint32 v51;
    sint32 v52;
    sint32 v53;
    sint32 v54;
    short v55;
    short v56;
    short v57;
    short v58;
    short v59;
    short v60;
    short v61;
    sint8 v62;
    short v63;
    sint32 v64;
    uint32 v65;
    uint32 v66;
    sint32 v67;
    sint32 v68;
    sint32 v69;
    sint32 v70;
    sint32 v71;
    sint32 v72;
    sint32 v73;
    sint32 v74;
    uint32 v75;
    sint32 v76;
    sint32 v77;
    short v78;
    short v79;
    short v80;
    short v81;
    short v82;
    short v83;
    short v84;
    short v85;
    short v86;
    short v87;
    sint32 v88;
    sint32 v89;
    uint32 v90;
    sint32 k;
    sint32 v92;
    sint32 v93;
    uint32 v94;
    uint32 v95;
    sint32 v96;
    sint32 v97;
    sint32 v98;
    sint32 v99;
    sint32 v100;
    sint32 v101;
    uint32 v102;
    sint32 v103;
    sint32 v104;
    sint32 m;
    sint32 v106;
    uint32 v107;
    uint32 v108;
    sint32 v109;
    sint32 v110;
    sint32 v111;
    sint32 v112;
    sint32 v113;
    sint32 v114;
    uint32 v115;
    sint32 v116;
    sint32 v117;
    sint32 n;
    sint32 v122;
    sint32 v123;
    sint32 v124;
    uint32 v125;
    sint32 v126;
    sint32 v127;
    sint32 v128;
    short v129;
    sint32 v130;
    sint32 v131;
    sint8 v132;
    sint8 v133;
    sint8 v134;
    uint32 v135;
    uint32 v136;
    sint32 v137;
    sint32 v138;
    sint32 v139;
    sint8 v140;
    sint32 v141;
    uint32 v142;
    uint32 v143;
    uint32 v144;
    sint32 v145;
    uint32 v146;
    uint32 v147;
    sint32 v148;
    sint32 v149;
    sint32 v150;
    sint32 v151;
    sint32 v152;
    sint32 v153;
    sint32 v154;
    sint32 v155;
    sint32 v156;
    uint32 v157;
    sint32 v158;
    sint32 v159;
    uint32 v160;
    uint32 v161;
    uint32 v162;
    uint32 v163;
    uint32 v164;
    sint32 v165;
    sint32 v166;
    sint32 v167;
    sint32 v168;
    sint32 v169;
    sint32 v170;
    short v171;
    sint32 v172;
    sint32 ii;
    sint32 v174;
    sint32 v175;
    sint32 v176;
    sint32 v177;
    uint32 v178;
    uint32 v179;
    uint32 v180;
    sint32 v184;
    sint32 v185;
    sint32 v186;
    sint32 v187;
    sint32 v188;
    sint32 v191;
    sint32 v192;
    sint32 v196;
    sint32 v197;
    short v198;
    short v199;
    sint32 v200;
    sint32 v201;
    sint32 v202;
    sint32 v204;
    uint32 v209;
    sint32 v210;
    uint32 v211;
    uint32 v212;
    sint32 v213;
    sint8 v214;
    uint32 v215;
    uint32 v216;
    sint32 v217;
    sint8 v218;
    sint32 v219;
    sint32 v223;
    sint32 v224;
    sint32 v228;
    sint32 v229;
    sint32 v233;
    sint32 v234;
    sint32 v238;
    sint32 v239;
    uint32 v240;
    sint32 v241;
    sint32 v244;
    sint32 v245;
    sint32 v246;
    short v247;
    sint32 v248;
    sint32 v249;
    sint32 v250;
    sint32 v251;
    sint32 v252;
    sint32 v255;
    sint32 v256;
    sint32 v257;
    sint32 v258;
    sint32 v259;
    sint32 v260;
    sint32 v263;
    sint32 v264;
    sint32 v265;
    sint32 v266;
    sint32 v267;
    short v268;
    uint32 v269;
    sint32 v270;
    sint32 v271;
    uint32 v272;
    sint32 v273;
    sint32 v274;
    sint32 v275;
    sint32 v276;
    sint32 v277;
    sint32 v278;
    sint32 v279;
    sint32 v280;
    sint32 v281;
    sint32 v282;
    sint32 v283;
    sint32 v284;
    sint32 v285;
    sint32 v286;
    sint32 v287;
    sint32 v288;
    sint8 v289;
    short v290;
    short v291;
    short v292;
    short v293;
    sint32 v294;
    sint32 v295;
    sint32 v296;
    uint32 v306[3];
    uint32 v307[3];
    sint32 v308;
    unsigned short v309;
    unsigned short v310;
    sint32 v311;
    sint32 v312;
    sint32 v313;
    sint32 v314;
    sint32 v315;
    sint32 v316;
    sint32 v317;
    sint32 v318;
    sint32 v319;
    sint32 v320;
    sint32 v321;
    sint32 v322;
    sint32 v323;
    sint32 v324;
    sint32 v325;
    sint32 v326;
    sint32 v327;
    sint32 v328;
    sint32 v329;
    sint32 v330;
    sint32 v331;
    sint32 v332;
    sint32 v333;
    sint32 v334;
    sint32 v335;
    sint32 v336;
    sint32 v337;
    sint32 v338;
    sint32 v339;
    sint32 v340;
    short v341[2];
    sint32 v342;
    short v343;
    short v344;
    sint32 i;
    uint32 v346;
    sint32 v347;
    sint32 v348;
    sint32 v349;
    uint32 v350;
    unsigned short v351;
    unsigned short v352;
    sint32 v353;
    sint32 v354;
    sint32 v355;
    sint32 v356;
    sint32 v357;
    sint32 v358;
    uint32 v359;
    uint32 v360;
    sint32 v361;
    sint32 v362;
    sint32 v363;
    sint32 v364;
    sint32 v365;
    sint32 v366;
    sint32 v367;
    sint32 v368;
    sint32 v369;
    sint32 v370;
    uint32 v371;
    sint32 v372;
    sint32 v375;
    sint32 v376;
    sint32 v377;
    uint32 v378;
    sint32 v379;
    gte_T4 = r_u32(0x800ED590u);
    gte_T5 = r_u32(0x800ED594u);
    xport_draft_gte_control_write(0, gte_T4);
    xport_draft_gte_control_write(1, gte_T5);
    gte_T4 = r_u32(0x800ED598u);
    gte_T5 = r_u32(0x800ED59Cu);
    gte_T6 = r_u32(0x800ED5A0u);
    xport_draft_gte_control_write(2, gte_T4);
    xport_draft_gte_control_write(3, gte_T5);
    xport_draft_gte_control_write(4, gte_T6);
    xport_draft_gte_control_write(5, 0);
    xport_draft_gte_control_write(6, 0);
    xport_draft_gte_control_write(7, 0);
    vec300[0] = r_u32(0x800ED520u);
    vec300[1] = r_u32(0x800ED524u);
    vec300[2] = r_u32(0x800ED528u);
    v5 = r_u32(0x800FF458u);
    result = (r_u16(((uint32)(((uint32)(r_u32(0x800FFB08u)) + (uint32)(16))))) >> 1);
    for (i = result; v5; v5 = r_u32(((uint32)(((sint32)((uint32)(v5) + (uint32)(4)))))))
        result = xport_draft_guest_call1(r_u32(((uint32)(((uint32)(r_u32(((uint32)(((sint32)((uint32)(v5) + (uint32)(68))))))) + (uint32)(28))))), ((sint32)((uint32)(v5) + (uint32)(((sint16)(r_u16(((uint32)(((uint32)(r_u32(((uint32)(((sint32)((uint32)(v5) + (uint32)(68))))))) + (uint32)(24)))))))))));

    for (j = r_u32(0x800FF450u);; j = r_u32(((uint32)(((sint32)((uint32)(j) + (uint32)(4)))))))
    {
        if (!j)
        {
            for (k = r_u32(0x800FF460u);; k = r_u32(((uint32)(((sint32)((uint32)(k) + (uint32)(4)))))))
            {
                v92 = 0;
                if (!k)
                {
                    for (m = r_u32(0x800FF45Cu);; m = r_u32(((uint32)(((sint32)((uint32)(m) + (uint32)(4)))))))
                    {
                        v106 = 0;
                        if (!m)
                            break;
                        w_u32(0x1F800000, r_u32(((uint32)(((sint32)((uint32)(m) + (uint32)(84)))))));
                        w_u32(0x1F800004, r_u32(((uint32)(((sint32)((uint32)(m) + (uint32)(88)))))));
                        w_u32(0x1F800008, r_u32(((uint32)(((sint32)((uint32)(m) + (uint32)(92)))))));
                        v107 = r_u32(((uint32)(((sint32)((uint32)(m) + (uint32)(80))))));
                        v108 = (v107 + ((uint32)(2)) * 4u);
                        while (1)
                        {
                            result = (v106 < r_u32(((uint32)(((sint32)((uint32)(m) + (uint32)(76)))))));
                            if ((v106 >= r_u32(((uint32)(((sint32)((uint32)(m) + (uint32)(76))))))))
                                break;
                            v109 = r_u32((v108 + (1) * 4u));
                            w_u32(0x1F80000C, r_u32(v107));
                            w_u32(0x1F800010, r_u32((v108 - ((uint32)(1)) * 4u)));
                            w_u32(0x1F800014, r_u32(v108));
                            v110 = sub_800850A4(528482304);
                            if ((v110 >= 0))
                            {
                                v111 = r_u32(0x800FF668u);
                                result = (r_u32(0x800FF374u) < ((uint32)(((uint32)(r_u32(0x800FF668u)) + (uint32)(24)))));
                                v112 = ((uint32)(r_u32(0x800FF668u)) + (uint32)(8));
                                if ((r_u32(0x800FF374u) < ((uint32)(((uint32)(r_u32(0x800FF668u)) + (uint32)(24))))))
                                    return;
                                w_u32(0x800FF668u, (r_u32(0x800FF668u) + (24)));
                                w_u32(((uint32)(((sint32)((uint32)(v111) + (uint32)(8))))), 50331648);
                                w_u32(((uint32)(((sint32)((uint32)(v112) + (uint32)(4))))), v109);
                                v113 = r_u8(((uint32)(((sint32)((uint32)(m) + (uint32)(72))))));
                                v17 = (v113 != 0);
                                v114 = ((uint32)(4) * (uint32)(v113));
                                if (!v17)
                                    v114 = ((uint32)(4) * (uint32)(v110));
                                v115 = ((uint32)(((uint32)(((uint32)(r_u32(0x800FF660u)) + (uint32)(v114))) + (uint32)(112))));
                                w_u32(((uint32)(((sint32)((uint32)(v112) + (uint32)(8))))), r_u32(0x1F800018));
                                w_u32(((uint32)(((sint32)((uint32)(v112) + (uint32)(12))))), r_u32(0x1F80001C));
                                w_u32(((uint32)(v112)), ((r_u32(((uint32)(v112))) & 0xFF000000) | (r_u32(v115) & 0xFFFFFF)));
                                w_u32(v115, ((r_u32(v115) & 0xFF000000) | (v112 & 0xFFFFFF)));
                                if (((v109 & 0x2000000) != 0))
                                {
                                    v116 = r_u32(0x800FF468u);
                                    v117 = r_u32(0x800FF46Cu);
                                    w_u32(((uint32)(v111)), r_u32(0x800FF468u));
                                    w_u32(((uint32)(((sint32)((uint32)(v111) + (uint32)(4))))), v117);
                                    w_u32(((uint32)(v111)), ((v116 & 0xFF000000) | (r_u32(v115) & 0xFFFFFF)));
                                    w_u32(v115, ((r_u32(v115) & 0xFF000000) | (v111 & 0xFFFFFF)));
                                }
                            }
                            v108 += (4) * 4u;
                            v107 += (4) * 4u;
                            ++v106;
                            w_u32(0x1F800000, r_u32(0x1F80000C));
                            w_u32(0x1F800004, r_u32(0x1F800010));
                            w_u32(0x1F800008, r_u32(0x1F800014));
                        }
                    }

                    for (n = r_u32(0x800FF438u);; n = r_u32(((uint32)(((sint32)((uint32)(n) + (uint32)(4)))))))
                    {
                        if (!n)
                            break;
                        vec297[0] = ((uint32)((((sint32)(r_u32(((uint32)(((sint32)((uint32)(n) + (uint32)(24)))))))) >> 12)) - (uint32)(vec300[0]));
                        vec297[1] = ((uint32)((((sint32)(r_u32(((uint32)(((sint32)((uint32)(n) + (uint32)(28)))))))) >> 12)) - (uint32)(vec300[1]));
                        vec297[2] = ((uint32)((((sint32)(r_u32(((uint32)(((sint32)((uint32)(n) + (uint32)(32)))))))) >> 12)) - (uint32)(vec300[2]));
                        gte_T4 = (((unsigned short)(vec297[0])) | ((uint32)(((unsigned short)(vec297[1]))) << (uint32)(16)));
                        xport_draft_gte_data_write(0, gte_T4);
                        xport_draft_gte_data_write(1, vec297[2]);
                        xport_draft_gte_execute(0x180001);
                        v122 = r_u32(0x800FF668u);
                        v123 = ((uint32)(r_u32(0x800FF668u)) + (uint32)(40));
                        result = 150994944;
                        if ((r_u32(0x800FF374u) < ((uint32)(((uint32)(r_u32(0x800FF668u)) + (uint32)(40))))))
                            return;
                        w_u32(((uint32)(r_u32(0x800FF668u))), 150994944);
                        v124 = r_u32(((uint32)(((sint32)((uint32)(n) + (uint32)(76))))));
                        w_u32(0x800FF668u, v123);
                        w_u32(((uint32)(((sint32)((uint32)(v122) + (uint32)(4))))), v124);
                        v311 = xport_draft_gte_data_read(27);
                        result = (v311 < i);
                        if (v311 >= i && (result = (sint32)r_u32(0x800FFAD8u) < v311,
                            (sint32)r_u32(0x800FFAD8u) >= v311) &&
                            ((v125 = renderer_3736C_divu((uint32)(sint32)(sint16)r_u16(n + 94u) << 7,
                                                        (uint32)v311)) < 0x201u ||
                             (r_u8(n + 88u) & 8u) != 0u))
                        {
                            v312 = xport_draft_gte_data_read(14);
                            v126 = r_u32(((uint32)(((uint32)(r_u32(((uint32)(((sint32)((uint32)(n) + (uint32)(84))))))) + (uint32)(4)))));
                            v127 = r_u32(((uint32)(((sint32)((uint32)(v126) + (uint32)(4))))));
                            v128 = r_u32(((uint32)(((sint32)((uint32)(v126) + (uint32)(8))))));
                            v129 = r_u16(((uint32)(((sint32)((uint32)(v126) + (uint32)(10))))));
                            if (r_u16(((uint32)(((sint32)((uint32)(n) + (uint32)(110)))))))
                                w_u32(((uint32)(((sint32)((uint32)(v122) + (uint32)(12))))), (((unsigned short)(r_u32(((uint32)(v126))))) | ((uint32)(r_u16(((uint32)(((sint32)((uint32)(n) + (uint32)(110))))))) << (uint32)(16))));
                            else
                                w_u32(((uint32)(((sint32)((uint32)(v122) + (uint32)(12))))), r_u32(((uint32)(v126))));
                            v130 = r_u8(((uint32)(((sint32)((uint32)(n) + (uint32)(109))))));
                            w_u32(((uint32)(((sint32)((uint32)(v122) + (uint32)(28))))), v128);
                            w_u16(((uint32)(((sint32)((uint32)(v122) + (uint32)(36))))), v129);
                            w_u32(((uint32)(((sint32)((uint32)(v122) + (uint32)(20))))), ((v127 & 0xFF9FFFFF) | ((sint32)((uint32)(v130) << (uint32)(16)))));
                            v131 = r_u16(((uint32)(((sint32)((uint32)(n) + (uint32)(96))))));
                            if (((v125 >= 0x100) || r_u16(((uint32)(((sint32)((uint32)(n) + (uint32)(96))))))))
                            {
                                v132 = r_u8(((uint32)(((sint32)((uint32)(v122) + (uint32)(29))))));
                                (w_u8(((uint32)(((sint32)((uint32)(v122) + (uint32)(20))))), (r_u8(((uint32)(((sint32)((uint32)(v122) + (uint32)(20)))))) - 1u)), (r_u8(((uint32)(((sint32)((uint32)(v122) + (uint32)(20)))))) - 1u));
                                v133 = r_u8(((uint32)(((sint32)((uint32)(v122) + (uint32)(36))))));
                                w_u8(((uint32)(((sint32)((uint32)(v122) + (uint32)(29))))), ((sint32)((uint32)(v132) - (uint32)(1))));
                                v134 = ((uint32)(r_u8(((uint32)(((sint32)((uint32)(v122) + (uint32)(37))))))) - (uint32)(1));
                                w_u8(((uint32)(((sint32)((uint32)(v122) + (uint32)(36))))), ((sint32)((uint32)(v133) - (uint32)(1))));
                                w_u8(((uint32)(((sint32)((uint32)(v122) + (uint32)(37))))), v134);
                            }
                            v135 = ((uint32)(v125) * (uint32)(((unsigned short)(r_u32(((uint32)(((sint32)((uint32)(n) + (uint32)(104))))))))));
                            v136 = r_u32(((uint32)(((sint32)((uint32)(n) + (uint32)(84))))));
                            v137 = ((unsigned char)(r_u8((v136 + (2) * 1u))));
                            v138 = ((unsigned char)(r_u8((v136 + (3) * 1u))));
                            v139 = (sint8)r_u8(v136 + 1u);
                            v140 = (r_u8(((uint32)(((sint32)((uint32)(n) + (uint32)(88)))))) & 1);
                            v141 = ((sint8)(r_u8(v136)));
                            v142 = ((uint32)(v125) * (uint32)(((r_u32(((uint32)(((sint32)((uint32)(n) + (uint32)(104)))))) >> 16) & 65535u)));
                            if (((sint32)((uint32)(v131) << (uint32)(16))))
                            {
                                v147 = (0x800F863Cu + ((v131 & 0xFFF)) * 4u);
                                v148 = ((sint16)(r_u16(((uint32)(v147)))));
                                v149 = ((sint32)((uint32)(v141) * (uint32)(v148)));
                                v150 = ((sint32)((uint32)(v139) * (uint32)(v148)));
                                v151 = ((sint16)(r_u16((((uint32)(v147)) + ((uint32)(1)) * 2u))));
                                v152 = ((sint32)((uint32)(v141) * (uint32)(v151)));
                                v153 = ((sint32)((uint32)(v139) * (uint32)(v151)));
                                v154 = ((sint32)((uint32)(v137) * (uint32)(v151)));
                                v155 = ((sint32)((uint32)(v149) + (uint32)(((sint32)((uint32)(v137) * (uint32)(v148))))));
                                v156 = ((sint32)((uint32)(v150) + (uint32)(((sint32)((uint32)(v138) * (uint32)(v148))))));
                                v157 = (v135 >> 12);
                                v158 = ((sint32)((uint32)(v152) + (uint32)(v154)));
                                v159 = ((sint32)((uint32)(v153) + (uint32)(((sint32)((uint32)(v138) * (uint32)(v151))))));
                                v160 = (v142 >> 12);
                                v161 = ((uint32)((v135 >> 12)) * (uint32)(((sint32)((uint32)(v152) - (uint32)(v150)))));
                                v162 = ((uint32)((v135 >> 12)) * (uint32)(((sint32)((uint32)(v158) - (uint32)(v150)))));
                                v163 = ((uint32)((v135 >> 12)) * (uint32)(((sint32)((uint32)(v152) - (uint32)(v156)))));
                                v164 = ((uint32)(v157) * (uint32)(((sint32)((uint32)(v158) - (uint32)(v156)))));
                                v165 = v312;
                                v166 = ((sint32)((uint32)(v312) + (uint32)((v162 >> 19))));
                                if (((r_u8(((uint32)(((sint32)((uint32)(n) + (uint32)(88)))))) & 1) != 0))
                                {
                                    w_u32(((uint32)(((sint32)((uint32)(v122) + (uint32)(16))))), ((sint32)((uint32)(((sint32)((uint32)(v312) + (uint32)((v161 >> 19))))) + (uint32)(((uint32)((((uint32)(v160) * (uint32)(((sint32)((uint32)(v149) + (uint32)(v153))))) >> 19)) << (uint32)(16))))));
                                    w_u32(((uint32)(((sint32)((uint32)(v122) + (uint32)(8))))), ((sint32)((uint32)(v166) + (uint32)(((uint32)((((uint32)(v160) * (uint32)(((sint32)((uint32)(v155) + (uint32)(v153))))) >> 19)) << (uint32)(16))))));
                                    w_u32(((uint32)(((sint32)((uint32)(v122) + (uint32)(32))))), ((sint32)((uint32)(((sint32)((uint32)(v165) + (uint32)((v163 >> 19))))) + (uint32)(((uint32)((((uint32)(v160) * (uint32)(((sint32)((uint32)(v149) + (uint32)(v159))))) >> 19)) << (uint32)(16))))));
                                    w_u32(((uint32)(((sint32)((uint32)(v122) + (uint32)(24))))), ((sint32)((uint32)(((sint32)((uint32)(v165) + (uint32)((v164 >> 19))))) + (uint32)(((uint32)((((uint32)(v160) * (uint32)(((sint32)((uint32)(v155) + (uint32)(v159))))) >> 19)) << (uint32)(16))))));
                                }
                                else
                                {
                                    w_u32(((uint32)(((sint32)((uint32)(v122) + (uint32)(8))))), ((sint32)((uint32)(((sint32)((uint32)(v312) + (uint32)((v161 >> 19))))) + (uint32)(((uint32)((((uint32)(v160) * (uint32)(((sint32)((uint32)(v149) + (uint32)(v153))))) >> 19)) << (uint32)(16))))));
                                    w_u32(((uint32)(((sint32)((uint32)(v122) + (uint32)(16))))), ((sint32)((uint32)(v166) + (uint32)(((uint32)((((uint32)(v160) * (uint32)(((sint32)((uint32)(v155) + (uint32)(v153))))) >> 19)) << (uint32)(16))))));
                                    w_u32(((uint32)(((sint32)((uint32)(v122) + (uint32)(24))))), ((sint32)((uint32)(((sint32)((uint32)(v165) + (uint32)((v163 >> 19))))) + (uint32)(((uint32)((((uint32)(v160) * (uint32)(((sint32)((uint32)(v149) + (uint32)(v159))))) >> 19)) << (uint32)(16))))));
                                    w_u32(((uint32)(((sint32)((uint32)(v122) + (uint32)(32))))), ((sint32)((uint32)(((sint32)((uint32)(v165) + (uint32)((v164 >> 19))))) + (uint32)(((uint32)((((uint32)(v160) * (uint32)(((sint32)((uint32)(v155) + (uint32)(v159))))) >> 19)) << (uint32)(16))))));
                                }
                            }
                            else
                            {
                                v143 = (((uint32)(v125) * (uint32)(v137)) >> 7);
                                v144 = ((uint32)((((uint32)(v125) * (uint32)(v138)) >> 7)) << (uint32)(16));
                                v145 = ((sint32)((uint32)(((sint32)((uint32)(v312) + (uint32)(((uint32)((((uint32)(v125) * (uint32)(v139)) >> 7)) << (uint32)(16)))))) + (uint32)(((short)((((sint32)(((uint32)(v125) * (uint32)(v141)))) >> 7))))));
                                v312 = v145;
                                if ((v143 < 5))
                                    v143 = 2;
                                if ((v144 <= 0x4FFFF))
                                    v144 = 0x10000;
                                v146 = ((sint32)((uint32)(v145) + (uint32)(v143)));
                                if (v140)
                                {
                                    w_u32(((uint32)(((sint32)((uint32)(v122) + (uint32)(16))))), v145);
                                    w_u32(((uint32)(((sint32)((uint32)(v122) + (uint32)(8))))), v146);
                                    w_u32(((uint32)(((sint32)((uint32)(v122) + (uint32)(32))))), ((sint32)((uint32)(v145) + (uint32)(v144))));
                                    w_u32(((uint32)(((sint32)((uint32)(v122) + (uint32)(24))))), ((uint32)(v146) + (uint32)(v144)));
                                }
                                else
                                {
                                    w_u32(((uint32)(((sint32)((uint32)(v122) + (uint32)(8))))), v145);
                                    w_u32(((uint32)(((sint32)((uint32)(v122) + (uint32)(16))))), v146);
                                    w_u32(((uint32)(((sint32)((uint32)(v122) + (uint32)(24))))), ((sint32)((uint32)(v145) + (uint32)(v144))));
                                    w_u32(((uint32)(((sint32)((uint32)(v122) + (uint32)(32))))), ((uint32)(v146) + (uint32)(v144)));
                                }
                            }
                            v167 = r_u8(((uint32)(((sint32)((uint32)(n) + (uint32)(108))))));
                            v17 = (v167 == 0);
                            v168 = ((uint32)(4) * (uint32)(v167));
                            if (v17)
                            {
                                v171 = v311;
                                v169 = r_u32(0x800FF660u);
                                w_u32(((uint32)(v122)), ((r_u32(((uint32)(v122))) & 0xFF000000) | (r_u32(((uint32)(((uint32)(((uint32)((((uint32)(((uint16)(v311))) - (uint32)(r_u16(((uint32)(((sint32)((uint32)(n) + (uint32)(64)))))))) & 0x3FFC)) + (uint32)(r_u32(0x800FF660u)))) + (uint32)(112))))) & 0xFFFFFF)));
                                v170 = (((sint32)((uint32)(v171) - (uint32)(r_u16(((uint32)(((sint32)((uint32)(n) + (uint32)(64))))))))) & 0x3FFC);
                            }
                            else
                            {
                                v169 = r_u32(0x800FF660u);
                                w_u32(((uint32)(v122)), ((r_u32(((uint32)(v122))) & 0xFF000000) | (r_u32(((uint32)(((sint32)((uint32)(((sint32)((uint32)(v168) + (uint32)(r_u32(0x800FF660u))))) + (uint32)(112)))))) & 0xFFFFFF)));
                                v170 = ((uint32)(4) * (uint32)(r_u8(((uint32)(((sint32)((uint32)(n) + (uint32)(108))))))));
                            }
                            v172 = ((sint32)((uint32)(v170) + (uint32)(v169)));
                            result = ((r_u32(((uint32)(((sint32)((uint32)(v172) + (uint32)(112)))))) & 0xFF000000) | (v122 & 0xFFFFFF));
                            w_u32(((uint32)(((sint32)((uint32)(v172) + (uint32)(112))))), result);
                        }
                        else
                        {
                            w_u32(0x800FF668u, v122);
                        }
                    }

                    for (ii = r_u32(0x800FF454u); ii; ii = r_u32(((uint32)(((sint32)((uint32)(ii) + (uint32)(4)))))))
                    {
                        vec303[0] = ((vec303[0] & 0xFFFF0000u) | (((((uint32)((((sint32)(r_u32(((uint32)(((sint32)((uint32)(ii) + (uint32)(72)))))))) >> 12)) - (uint32)(vec300[0]))) & 0xFFFFu) << 0));
                        vec303[0] = ((vec303[0] & 0x0000FFFFu) | (((((uint32)((((sint32)(r_u32(((uint32)(((sint32)((uint32)(ii) + (uint32)(76)))))))) >> 12)) - (uint32)(vec300[1]))) & 0xFFFFu) << 16));
                        vec303[1] = ((vec303[1] & 0xFFFF0000u) | (((((uint32)((((sint32)(r_u32(((uint32)(((sint32)((uint32)(ii) + (uint32)(80)))))))) >> 12)) - (uint32)(vec300[2]))) & 0xFFFFu) << 0));
                        xport_draft_gte_data_write(0, vec303[0]);
                        xport_draft_gte_data_write(1, vec303[1]);
                        vec303[0] = ((vec303[0] & 0xFFFF0000u) | (((((uint32)((((sint32)(r_u32(((uint32)(((sint32)((uint32)(ii) + (uint32)(84)))))))) >> 12)) - (uint32)(vec300[0]))) & 0xFFFFu) << 0));
                        vec303[0] = ((vec303[0] & 0x0000FFFFu) | (((((uint32)((((sint32)(r_u32(((uint32)(((sint32)((uint32)(ii) + (uint32)(88)))))))) >> 12)) - (uint32)(vec300[1]))) & 0xFFFFu) << 16));
                        vec303[1] = ((vec303[1] & 0xFFFF0000u) | (((((uint32)((((sint32)(r_u32(((uint32)(((sint32)((uint32)(ii) + (uint32)(92)))))))) >> 12)) - (uint32)(vec300[2]))) & 0xFFFFu) << 0));
                        xport_draft_gte_data_write(2, vec303[0]);
                        xport_draft_gte_data_write(3, vec303[1]);
                        vec303[0] = ((vec303[0] & 0xFFFF0000u) | (((((uint32)((((sint32)(r_u32(((uint32)(((sint32)((uint32)(ii) + (uint32)(96)))))))) >> 12)) - (uint32)(vec300[0]))) & 0xFFFFu) << 0));
                        vec303[0] = ((vec303[0] & 0x0000FFFFu) | (((((uint32)((((sint32)(r_u32(((uint32)(((sint32)((uint32)(ii) + (uint32)(100)))))))) >> 12)) - (uint32)(vec300[1]))) & 0xFFFFu) << 16));
                        vec303[1] = ((vec303[1] & 0xFFFF0000u) | (((((uint32)((((sint32)(r_u32(((uint32)(((sint32)((uint32)(ii) + (uint32)(104)))))))) >> 12)) - (uint32)(vec300[2]))) & 0xFFFFu) << 0));
                        xport_draft_gte_data_write(4, vec303[0]);
                        xport_draft_gte_data_write(5, vec303[1]);
                        xport_draft_gte_execute(0x280030);
                        v174 = r_u32(0x800FF668u);
                        v175 = ((uint32)(r_u32(0x800FF668u)) + (uint32)(28));
                        result = (r_u32(0x800FF374u) < ((uint32)(((uint32)(r_u32(0x800FF668u)) + (uint32)(28)))));
                        v176 = ((uint32)(r_u32(0x800FF668u)) + (uint32)(20));
                        if ((r_u32(0x800FF374u) < ((uint32)(((uint32)(r_u32(0x800FF668u)) + (uint32)(28))))))
                            return;
                        w_u32(((uint32)(r_u32(0x800FF668u))), 0x4000000);
                        w_u32(0x800FF668u, v175);
                        w_u8(((uint32)(((sint32)((uint32)(v174) + (uint32)(7))))), 34);
                        w_u8(((uint32)(((sint32)((uint32)(v174) + (uint32)(4))))), r_u8(((uint32)(((sint32)((uint32)(ii) + (uint32)(115)))))));
                        w_u8(((uint32)(((sint32)((uint32)(v174) + (uint32)(5))))), r_u8(((uint32)(((sint32)((uint32)(ii) + (uint32)(116)))))));
                        w_u8(((uint32)(((sint32)((uint32)(v174) + (uint32)(6))))), r_u8(((uint32)(((sint32)((uint32)(ii) + (uint32)(117)))))));
                        v177 = r_u32(0x800FF46Cu);
                        w_u32(((uint32)(((sint32)((uint32)(v174) + (uint32)(20))))), r_u32(0x800FF468u));
                        w_u32(((uint32)(((sint32)((uint32)(v176) + (uint32)(4))))), v177);
                        w_u32(((uint32)(((sint32)((uint32)(v174) + (uint32)(8))))), xport_draft_gte_data_read(12));
                        w_u32(((uint32)(((sint32)((uint32)(v174) + (uint32)(12))))), xport_draft_gte_data_read(13));
                        w_u32(((uint32)(((sint32)((uint32)(v174) + (uint32)(16))))), xport_draft_gte_data_read(14));
                        v313 = xport_draft_gte_data_read(27);
                        v178 = ((uint32)(((uint32)(((uint32)(r_u32(0x800FF660u)) + (uint32)((v313 & 0x3FFC)))) + (uint32)(112))));
                        w_u32(((uint32)(v174)), ((r_u32(((uint32)(v174))) & 0xFF000000) | (r_u32(v178) & 0xFFFFFF)));
                        v179 = ((r_u32(v178) & 0xFF000000) | (v174 & 0xFFFFFF));
                        w_u32(v178, v179);
                        w_u32(((uint32)(((sint32)((uint32)(v174) + (uint32)(20))))), ((r_u32(((uint32)(((sint32)((uint32)(v174) + (uint32)(20)))))) & 0xFF000000) | (v179 & 0xFFFFFF)));
                        result = ((r_u32(v178) & 0xFF000000) | (v176 & 0xFFFFFF));
                        w_u32(v178, result);
                    }

                    v180 = ((uint32)(r_u32(0x800FF464u)));
                    while (v180)
                    {
                        v314 = 12;
                        xport_draft_host_sub_8006C564_p13(v306, (v180 + ((uint32)(20)) * 4u), &v314);
                        xport_draft_host_sub_8006C3AC_p123(vec303, v306, vec300);
                        vec297[0] = vec303[0];
                        vec297[1] = vec303[1];
                        vec297[2] = vec303[2];
                        gte_T4 = (((unsigned short)(vec303[0])) | ((uint32)(((unsigned short)(vec303[1]))) << (uint32)(16)));
                        xport_draft_gte_data_write(0, gte_T4);
                        xport_draft_gte_data_write(1, vec297[2]);
                        xport_draft_gte_execute(0x180001);
                        v315 = xport_draft_gte_data_read(27);
                        result = (v315 < ((sint32)r_u32(0x800FFAE8u)));
                        if ((v315 >= ((sint32)r_u32(0x800FFAE8u))))
                        {
                            if ((((sint32)r_u32(0x800FFAD8u)) >= v315))
                            {
                                v316 = xport_draft_gte_data_read(14);
                                v317 = 12;
                                xport_draft_host_sub_8006C564_p13(v306, (v180 + ((uint32)(23)) * 4u), &v317);
                                xport_draft_host_sub_8006C3AC_p123(vec303, v306, vec300);
                                vec297[0] = vec303[0];
                                vec297[1] = vec303[1];
                                vec297[2] = vec303[2];
                                gte_T4 = (((unsigned short)(vec303[0])) | ((uint32)(((unsigned short)(vec303[1]))) << (uint32)(16)));
                                xport_draft_gte_data_write(0, gte_T4);
                                xport_draft_gte_data_write(1, vec297[2]);
                                xport_draft_gte_execute(0x180001);
                                v318 = xport_draft_gte_data_read(27);
                                result = (v318 < ((sint32)r_u32(0x800FFAE8u)));
                                if ((v318 >= ((sint32)r_u32(0x800FFAE8u))))
                                {
                                            if ((((sint32)r_u32(0x800FFAD8u)) >= v318))
                                    {
                                        v319 = xport_draft_gte_data_read(14);
                                        v184 = r_u32(0x800FF668u);
                                        v185 = ((uint32)(r_u32(0x800FF668u)) + (uint32)(20));
                                        result = 0x4000000;
                                        if ((r_u32(0x800FF374u) < ((uint32)(((uint32)(r_u32(0x800FF668u)) + (uint32)(20))))))
                                            return;
                                        w_u32(((uint32)(r_u32(0x800FF668u))), 0x4000000);
                                        v186 = r_u32((v180 + (18) * 4u));
                                        w_u32(0x800FF668u, v185);
                                        w_u32(((uint32)(((sint32)((uint32)(v184) + (uint32)(4))))), v186);
                                        w_u32(((uint32)(((sint32)((uint32)(v184) + (uint32)(12))))), r_u32((v180 + (19) * 4u)));
                                        w_u32(((uint32)(((sint32)((uint32)(v184) + (uint32)(8))))), v316);
                                        w_u32(((uint32)(((sint32)((uint32)(v184) + (uint32)(16))))), v319);
                                        if ((v318 < v315))
                                            v315 = v318;
                                        v187 = ((uint32)((v315 & 0x3FFC)) + (uint32)(r_u32(0x800FF660u)));
                                        w_u32(((uint32)(v184)), ((r_u32(((uint32)(v184))) & 0xFF000000) | (r_u32(((uint32)(((sint32)((uint32)(v187) + (uint32)(112)))))) & 0xFFFFFF)));
                                        result = ((r_u32(((uint32)(((sint32)((uint32)(v187) + (uint32)(112)))))) & 0xFF000000) | (v184 & 0xFFFFFF));
                                        w_u32(((uint32)(((sint32)((uint32)(v187) + (uint32)(112))))), result);
                                    }
                                }
                            }
                        }
                        v180 = ((uint32)(r_u32((v180 + (1) * 4u))));
                    }

                    v188 = r_u32(0x800FF444u);
                    while (v188)
                    {
                        result = r_u32(((uint32)(((sint32)((uint32)(v188) + (uint32)(148))))));
                        if (result)
                        {
                            v320 = 12;
                            xport_draft_host_sub_8006C564_p13(v306, ((sint32)((uint32)(v188) + (uint32)(24))), &v320);
                            xport_draft_host_sub_8006C3AC_p123(vec303, v306, vec300);
                            vec297[0] = vec303[0];
                            vec297[1] = vec303[1];
                            vec297[2] = vec303[2];
                            gte_T4 = (((unsigned short)(vec303[0])) | ((uint32)(((unsigned short)(vec303[1]))) << (uint32)(16)));
                            xport_draft_gte_data_write(0, gte_T4);
                            xport_draft_gte_data_write(1, vec297[2]);
                            xport_draft_gte_execute(0x180001);
                            v191 = r_u32(0x800FF668u);
                            v192 = r_u32(((uint32)(((sint32)((uint32)(v188) + (uint32)(148))))));
                            if ((r_u32(0x800FF374u) < ((uint32)(((uint32)(r_u32(0x800FF668u)) + (uint32)(40))))))
                                return;
                            w_u32(0x800FF668u, (r_u32(0x800FF668u) + (40)));
                            v321 = xport_draft_gte_data_read(27);
                            result = (v321 < ((sint32)r_u32(0x800FFAE8u)));
                            if ((v321 < ((sint32)r_u32(0x800FFAE8u))))
                                goto LABEL_170;
                            result = ((sint32)((uint32)(v191) + (uint32)(8)));
                            if ((((sint32)r_u32(0x800FFAD8u)) < v321))
                                goto LABEL_170;
                            w_u32(((uint32)(((sint32)((uint32)(v191) + (uint32)(8))))), xport_draft_gte_data_read(14));
                            v322 = 12;
                            xport_draft_host_sub_8006C564_p13(v306, ((sint32)((uint32)(v188) + (uint32)(72))), &v322);
                            xport_draft_host_sub_8006C3AC_p123(vec303, v306, vec300);
                            vec297[0] = vec303[0];
                            vec297[1] = vec303[1];
                            vec297[2] = vec303[2];
                            gte_T4 = (((unsigned short)(vec303[0])) | ((uint32)(((unsigned short)(vec303[1]))) << (uint32)(16)));
                            xport_draft_gte_data_write(0, gte_T4);
                            xport_draft_gte_data_write(1, vec297[2]);
                            xport_draft_gte_execute(0x180001);
                            v323 = xport_draft_gte_data_read(27);
                            result = (v323 < ((sint32)r_u32(0x800FFAE8u)));
                            if ((v323 < ((sint32)r_u32(0x800FFAE8u))))
                                goto LABEL_170;
                            result = ((sint32)((uint32)(v191) + (uint32)(16)));
                            if ((((sint32)r_u32(0x800FFAD8u)) < v323))
                                goto LABEL_170;
                            w_u32(((uint32)(((sint32)((uint32)(v191) + (uint32)(16))))), xport_draft_gte_data_read(14));
                            v324 = 12;
                            xport_draft_host_sub_8006C564_p13(v306, ((sint32)((uint32)(v188) + (uint32)(84))), &v324);
                            xport_draft_host_sub_8006C3AC_p123(vec303, v306, vec300);
                            vec297[0] = vec303[0];
                            vec297[1] = vec303[1];
                            vec297[2] = vec303[2];
                            gte_T4 = (((unsigned short)(vec303[0])) | ((uint32)(((unsigned short)(vec303[1]))) << (uint32)(16)));
                            xport_draft_gte_data_write(0, gte_T4);
                            xport_draft_gte_data_write(1, vec297[2]);
                            xport_draft_gte_execute(0x180001);
                            w_u32(((uint32)(v191)), 150994944);
                            w_u32(((uint32)(((sint32)((uint32)(v191) + (uint32)(4))))), r_u32(((uint32)(((sint32)((uint32)(v188) + (uint32)(144)))))));
                            if ((v321 < v323))
                                v321 = v323;
                            v325 = xport_draft_gte_data_read(27);
                            result = (v325 < ((sint32)r_u32(0x800FFAE8u)));
                            if ((v325 < ((sint32)r_u32(0x800FFAE8u))))
                                goto LABEL_170;
                            result = ((sint32)((uint32)(v191) + (uint32)(24)));
                            if ((((sint32)r_u32(0x800FFAD8u)) < v325))
                                goto LABEL_170;
                            w_u32(((uint32)(((sint32)((uint32)(v191) + (uint32)(24))))), xport_draft_gte_data_read(14));
                            v326 = 12;
                            xport_draft_host_sub_8006C564_p13(v306, ((sint32)((uint32)(v188) + (uint32)(96))), &v326);
                            xport_draft_host_sub_8006C3AC_p123(vec303, v306, vec300);
                            vec297[0] = vec303[0];
                            vec297[1] = vec303[1];
                            vec297[2] = vec303[2];
                            gte_T4 = (((unsigned short)(vec303[0])) | ((uint32)(((unsigned short)(vec303[1]))) << (uint32)(16)));
                            xport_draft_gte_data_write(0, gte_T4);
                            xport_draft_gte_data_write(1, vec297[2]);
                            xport_draft_gte_execute(0x180001);
                            v196 = r_u32(((uint32)(((sint32)((uint32)(v192) + (uint32)(4))))));
                            v197 = r_u32(((uint32)(((sint32)((uint32)(v192) + (uint32)(8))))));
                            v198 = r_u16(((uint32)(((sint32)((uint32)(v192) + (uint32)(10))))));
                            w_u32(((uint32)(((sint32)((uint32)(v191) + (uint32)(12))))), r_u32(((uint32)(v192))));
                            w_u32(((uint32)(((sint32)((uint32)(v191) + (uint32)(20))))), v196);
                            w_u32(((uint32)(((sint32)((uint32)(v191) + (uint32)(28))))), v197);
                            w_u16(((uint32)(((sint32)((uint32)(v191) + (uint32)(36))))), v198);
                            if (((r_u8(((uint32)(((sint32)((uint32)(v188) + (uint32)(152)))))) & 4) != 0))
                                w_u16(((uint32)(((sint32)((uint32)(v191) + (uint32)(22))))), ((r_u16(((uint32)(((sint32)((uint32)(v191) + (uint32)(22)))))) & 0xFF9F) | 0x40));
                            if ((v321 < v325))
                                v321 = v325;
                            v327 = xport_draft_gte_data_read(27);
                            result = (v327 < ((sint32)r_u32(0x800FFAE8u)));
                            if ((v327 < ((sint32)r_u32(0x800FFAE8u))))
                                goto LABEL_170;
                            result = ((sint32)((uint32)(v191) + (uint32)(32)));
                            if ((((sint32)r_u32(0x800FFAD8u)) >= v327))
                            {
                                w_u32(((uint32)(((sint32)((uint32)(v191) + (uint32)(32))))), xport_draft_gte_data_read(14));
                                if ((v321 < v327))
                                    v321 = v327;
                                v199 = v321;
                                v200 = r_u32(0x800FF660u);
                                w_u32(((uint32)(v191)), ((r_u32(((uint32)(v191))) & 0xFF000000) | (r_u32(((uint32)(((uint32)(((uint32)((((uint32)(((uint16)(v321))) - (uint32)(r_u16(((uint32)(((sint32)((uint32)(v188) + (uint32)(64)))))))) & 0x3FFC)) + (uint32)(r_u32(0x800FF660u)))) + (uint32)(112))))) & 0xFFFFFF)));
                                v201 = ((uint32)((((sint32)((uint32)(v199) - (uint32)(r_u16(((uint32)(((sint32)((uint32)(v188) + (uint32)(64))))))))) & 0x3FFC)) + (uint32)(v200));
                                result = ((r_u32(((uint32)(((sint32)((uint32)(v201) + (uint32)(112)))))) & 0xFF000000) | (v191 & 0xFFFFFF));
                                w_u32(((uint32)(((sint32)((uint32)(v201) + (uint32)(112))))), result);
                            }
                            else
                            {
                            LABEL_170:
                                w_u32(0x800FF668u, v191);
                            }
                        }
                        v188 = r_u32(((uint32)(((sint32)((uint32)(v188) + (uint32)(4))))));
                    }

                    v202 = r_u32(0x800FF44Cu);
                    while (v202)
                    {
                        result = ((uint32)(r_u32(0x800FF668u)) + (uint32)(152));
                        v204 = -1;
                        if ((r_u32(0x800FF374u) < ((uint32)(((uint32)(r_u32(0x800FF668u)) + (uint32)(152))))))
                            return;
                        xport_draft_host_sub_8006C3AC_p13(vec303, ((sint32)((uint32)(v202) + (uint32)(104))), vec300);
                        vec297[0] = vec303[0];
                        vec297[1] = vec303[1];
                        vec297[2] = vec303[2];
                        gte_T4 = (((unsigned short)(vec303[0])) | ((uint32)(((unsigned short)(vec303[1]))) << (uint32)(16)));
                        xport_draft_gte_data_write(0, gte_T4);
                        xport_draft_gte_data_write(1, vec297[2]);
                        xport_draft_gte_execute(0x180001);
                        v328 = xport_draft_gte_data_read(27);
                        v329 = xport_draft_gte_data_read(14);
                        xport_draft_host_sub_8006C3AC_p13(vec303, ((sint32)((uint32)(v202) + (uint32)(116))), vec300);
                        vec297[0] = vec303[0];
                        vec297[1] = vec303[1];
                        vec297[2] = vec303[2];
                        gte_T4 = (((unsigned short)(vec303[0])) | ((uint32)(((unsigned short)(vec303[1]))) << (uint32)(16)));
                        xport_draft_gte_data_write(0, gte_T4);
                        xport_draft_gte_data_write(1, vec297[2]);
                        xport_draft_gte_execute(0x180001);
                        result = (v328 < ((sint32)r_u32(0x800FFAE8u)));
                        if ((v328 >= ((sint32)r_u32(0x800FFAE8u))))
                        {
                            result = (v328 > -1);
                            if ((((sint32)r_u32(0x800FFAD8u)) >= v328))
                            {
                                if ((v328 > -1))
                                    v204 = v328;
                                v330 = xport_draft_gte_data_read(27);
                                v331 = xport_draft_gte_data_read(14);
                                xport_draft_host_sub_8006C3AC_p13(vec303, ((sint32)((uint32)(v202) + (uint32)(128))), vec300);
                                vec297[0] = vec303[0];
                                vec297[1] = vec303[1];
                                vec297[2] = vec303[2];
                                gte_T4 = (((unsigned short)(vec303[0])) | ((uint32)(((unsigned short)(vec303[1]))) << (uint32)(16)));
                                xport_draft_gte_data_write(0, gte_T4);
                                xport_draft_gte_data_write(1, vec297[2]);
                                xport_draft_gte_execute(0x180001);
                                result = (v330 < ((sint32)r_u32(0x800FFAE8u)));
                                if ((v330 >= ((sint32)r_u32(0x800FFAE8u))))
                                {
                                    result = (v204 < v330);
                                    if ((((sint32)r_u32(0x800FFAD8u)) >= v330))
                                    {
                                        if ((v204 < v330))
                                            v204 = v330;
                                        v332 = xport_draft_gte_data_read(27);
                                        v333 = xport_draft_gte_data_read(14);
                                        xport_draft_host_sub_8006C3AC_p13(vec303, ((sint32)((uint32)(v202) + (uint32)(140))), vec300);
                                        vec297[0] = vec303[0];
                                        vec297[1] = vec303[1];
                                        vec297[2] = vec303[2];
                                        gte_T4 = (((unsigned short)(vec303[0])) | ((uint32)(((unsigned short)(vec303[1]))) << (uint32)(16)));
                                        xport_draft_gte_data_write(0, gte_T4);
                                        xport_draft_gte_data_write(1, vec297[2]);
                                        xport_draft_gte_execute(0x180001);
                                        result = (v332 < ((sint32)r_u32(0x800FFAE8u)));
                                        if ((v332 >= ((sint32)r_u32(0x800FFAE8u))))
                                        {
                                            result = (v204 < v332);
                                            if ((((sint32)r_u32(0x800FFAD8u)) >= v332))
                                            {
                                                if ((v204 < v332))
                                                    v204 = v332;
                                                v334 = xport_draft_gte_data_read(27);
                                                result = (v334 < ((sint32)r_u32(0x800FFAE8u)));
                                                if ((v334 >= ((sint32)r_u32(0x800FFAE8u))))
                                                {
                                                    result = (v204 < v334);
                                                    if ((((sint32)r_u32(0x800FFAD8u)) >= v334))
                                                    {
                                                        if ((v204 < v334))
                                                            v204 = ((v204 & 0xFFFF0000u) | (((v334) & 0xFFFFu) << 0));
                                                        v335 = xport_draft_gte_data_read(14);
                                                        v209 = ((uint32)(((uint32)(((uint32)(r_u32(0x800FF660u)) + (uint32)((v204 & 0x3FFC)))) + (uint32)(112))));
                                                        if (r_u32(((uint32)(((sint32)((uint32)(v202) + (uint32)(188)))))))
                                                        {
                                                            v210 = r_u32(0x800FF668u);
                                                            vec303[1] = 0;
                                                            vec303[0] = 0;
                                                            w_u32(0x800FF668u, (r_u32(0x800FF668u) + (24)));
                                                            w_u8(((uint32)(((sint32)((uint32)(v210) + (uint32)(3))))), 2);
                                                            v211 = ((vec303[0] >> 16) & 255u);
                                                            v212 = ((unsigned char)(vec303[0]));
                                                            v213 = ((sint16)((vec303[1]) >> 16));
                                                            v214 = vec303[1];
                                                            w_u32(((uint32)(((sint32)((uint32)(v210) + (uint32)(8))))), 0);
                                                            w_u32(((uint32)(((sint32)((uint32)(v210) + (uint32)(4))))), ((((((uint32)((v211 >> 3)) << (uint32)(15)) | ((uint32)((v212 >> 3)) << (uint32)(10))) | 0xE2000000) | (((uint32)(-4) * (uint32)(v213)) & 0x3E0)) | (((sint32)(((unsigned char)(-v214)))) >> 3)));
                                                            v360 = ((uint32)(((sint32)((uint32)(v210) + (uint32)(12)))));
                                                            w_u8(((uint32)(((sint32)((uint32)(v210) + (uint32)(15))))), 2);
                                                            v215 = ((vec303[0] >> 16) & 255u);
                                                            v216 = ((unsigned char)(vec303[0]));
                                                            v217 = ((sint16)((vec303[1]) >> 16));
                                                            v218 = vec303[1];
                                                            w_u32(((uint32)(((sint32)((uint32)(v210) + (uint32)(20))))), 0);
                                                            w_u32(((uint32)(((sint32)((uint32)(v210) + (uint32)(16))))), ((((((uint32)((v215 >> 3)) << (uint32)(15)) | ((uint32)((v216 >> 3)) << (uint32)(10))) | 0xE2000000) | (((uint32)(-4) * (uint32)(v217)) & 0x3E0)) | (((sint32)(((unsigned char)(-v218)))) >> 3)));
                                                            w_u32(((uint32)(((sint32)((uint32)(v210) + (uint32)(16))))), r_u32(((uint32)(((sint32)((uint32)(v202) + (uint32)(188)))))));
                                                            if (r_u32(((uint32)(((sint32)((uint32)(v202) + (uint32)(188)))))))
                                                            {
                                                                w_u32(((uint32)(v210)), ((r_u32(((uint32)(v210))) & 0xFF000000) | (r_u32(v209) & 0xFFFFFF)));
                                                                w_u32(v209, ((r_u32(v209) & 0xFF000000) | (v210 & 0xFFFFFF)));
                                                            }
                                                        }
                                                        v219 = r_u32(0x800FF668u);
                                                        gte_T7 = v329;
                                                        gte_T8 = v331;
                                                        gte_T9 = v333;
                                                        xport_draft_gte_data_write(12, gte_T7);
                                                        xport_draft_gte_data_write(14, gte_T9);
                                                        xport_draft_gte_data_write(13, gte_T8);
                                                        xport_draft_gte_execute(0x1400006);
                                                        v336 = xport_draft_gte_data_read(24);
                                                        if ((v336 <= 0))
                                                        {
                                                            w_u32(((uint32)(r_u32(0x800FF668u))), 117440512);
                                                            w_u32(((uint32)(((sint32)((uint32)(v219) + (uint32)(4))))), (r_u32(((uint32)(((sint32)((uint32)(v202) + (uint32)(172)))))) | 0x24000000));
                                                            v223 = r_u32(((uint32)(v219)));
                                                            w_u32(((uint32)(((sint32)((uint32)(v219) + (uint32)(12))))), r_u32(((uint32)(((sint32)((uint32)(v202) + (uint32)(160)))))));
                                                            w_u32(((uint32)(((sint32)((uint32)(v219) + (uint32)(20))))), r_u32(((uint32)(((sint32)((uint32)(v202) + (uint32)(164)))))));
                                                            w_u16(((uint32)(((sint32)((uint32)(v219) + (uint32)(28))))), r_u16(((uint32)(((sint32)((uint32)(v202) + (uint32)(168)))))));
                                                            w_u32(((uint32)(((sint32)((uint32)(v219) + (uint32)(8))))), v329);
                                                            w_u32(((uint32)(((sint32)((uint32)(v219) + (uint32)(16))))), v331);
                                                            w_u32(((uint32)(((sint32)((uint32)(v219) + (uint32)(24))))), v333);
                                                            w_u32(((uint32)(v219)), ((v223 & 0xFF000000) | (r_u32(v209) & 0xFFFFFF)));
                                                            v224 = (v219 & 0xFFFFFF);
                                                            v219 += 32;
                                                            w_u32(v209, ((r_u32(v209) & 0xFF000000) | v224));
                                                        }
                                                        gte_T3 = v329;
                                                        gte_T7 = v335;
                                                        gte_T8 = v331;
                                                        xport_draft_gte_data_write(12, gte_T3);
                                                        xport_draft_gte_data_write(14, gte_T8);
                                                        xport_draft_gte_data_write(13, gte_T7);
                                                        xport_draft_gte_execute(0x1400006);
                                                        v336 = xport_draft_gte_data_read(24);
                                                        if ((v336 <= 0))
                                                        {
                                                            w_u32(((uint32)(v219)), 117440512);
                                                            w_u32(((uint32)(((sint32)((uint32)(v219) + (uint32)(4))))), (r_u32(((uint32)(((sint32)((uint32)(v202) + (uint32)(176)))))) | 0x24000000));
                                                            v228 = r_u32(((uint32)(v219)));
                                                            w_u32(((uint32)(((sint32)((uint32)(v219) + (uint32)(12))))), r_u32(((uint32)(((sint32)((uint32)(v202) + (uint32)(160)))))));
                                                            w_u32(((uint32)(((sint32)((uint32)(v219) + (uint32)(20))))), r_u32(((uint32)(((sint32)((uint32)(v202) + (uint32)(164)))))));
                                                            w_u16(((uint32)(((sint32)((uint32)(v219) + (uint32)(28))))), r_u16(((uint32)(((sint32)((uint32)(v202) + (uint32)(168)))))));
                                                            w_u32(((uint32)(((sint32)((uint32)(v219) + (uint32)(8))))), v329);
                                                            w_u32(((uint32)(((sint32)((uint32)(v219) + (uint32)(16))))), v335);
                                                            w_u32(((uint32)(((sint32)((uint32)(v219) + (uint32)(24))))), v331);
                                                            w_u32(((uint32)(v219)), ((v228 & 0xFF000000) | (r_u32(v209) & 0xFFFFFF)));
                                                            v229 = (v219 & 0xFFFFFF);
                                                            v219 += 32;
                                                            w_u32(v209, ((r_u32(v209) & 0xFF000000) | v229));
                                                        }
                                                        gte_T9 = v331;
                                                        gte_T3 = v335;
                                                        gte_T7 = v333;
                                                        xport_draft_gte_data_write(12, gte_T9);
                                                        xport_draft_gte_data_write(14, gte_T7);
                                                        xport_draft_gte_data_write(13, gte_T3);
                                                        xport_draft_gte_execute(0x1400006);
                                                        v336 = xport_draft_gte_data_read(24);
                                                        if ((v336 <= 0))
                                                        {
                                                            w_u32(((uint32)(v219)), 117440512);
                                                            w_u32(((uint32)(((sint32)((uint32)(v219) + (uint32)(4))))), (r_u32(((uint32)(((sint32)((uint32)(v202) + (uint32)(180)))))) | 0x24000000));
                                                            v233 = r_u32(((uint32)(v219)));
                                                            w_u32(((uint32)(((sint32)((uint32)(v219) + (uint32)(12))))), r_u32(((uint32)(((sint32)((uint32)(v202) + (uint32)(160)))))));
                                                            w_u32(((uint32)(((sint32)((uint32)(v219) + (uint32)(20))))), r_u32(((uint32)(((sint32)((uint32)(v202) + (uint32)(164)))))));
                                                            w_u16(((uint32)(((sint32)((uint32)(v219) + (uint32)(28))))), r_u16(((uint32)(((sint32)((uint32)(v202) + (uint32)(168)))))));
                                                            w_u32(((uint32)(((sint32)((uint32)(v219) + (uint32)(8))))), v331);
                                                            w_u32(((uint32)(((sint32)((uint32)(v219) + (uint32)(16))))), v335);
                                                            w_u32(((uint32)(((sint32)((uint32)(v219) + (uint32)(24))))), v333);
                                                            w_u32(((uint32)(v219)), ((v233 & 0xFF000000) | (r_u32(v209) & 0xFFFFFF)));
                                                            v234 = (v219 & 0xFFFFFF);
                                                            v219 += 32;
                                                            w_u32(v209, ((r_u32(v209) & 0xFF000000) | v234));
                                                        }
                                                        gte_T8 = v333;
                                                        gte_T9 = v335;
                                                        gte_T3 = v329;
                                                        xport_draft_gte_data_write(12, gte_T8);
                                                        xport_draft_gte_data_write(14, gte_T3);
                                                        xport_draft_gte_data_write(13, gte_T9);
                                                        xport_draft_gte_execute(0x1400006);
                                                        v336 = xport_draft_gte_data_read(24);
                                                        if ((v336 <= 0))
                                                        {
                                                            w_u32(((uint32)(v219)), 117440512);
                                                            w_u32(((uint32)(((sint32)((uint32)(v219) + (uint32)(4))))), (r_u32(((uint32)(((sint32)((uint32)(v202) + (uint32)(184)))))) | 0x24000000));
                                                            v238 = r_u32(((uint32)(v219)));
                                                            w_u32(((uint32)(((sint32)((uint32)(v219) + (uint32)(12))))), r_u32(((uint32)(((sint32)((uint32)(v202) + (uint32)(160)))))));
                                                            w_u32(((uint32)(((sint32)((uint32)(v219) + (uint32)(20))))), r_u32(((uint32)(((sint32)((uint32)(v202) + (uint32)(164)))))));
                                                            w_u16(((uint32)(((sint32)((uint32)(v219) + (uint32)(28))))), r_u16(((uint32)(((sint32)((uint32)(v202) + (uint32)(168)))))));
                                                            w_u32(((uint32)(((sint32)((uint32)(v219) + (uint32)(8))))), v333);
                                                            w_u32(((uint32)(((sint32)((uint32)(v219) + (uint32)(16))))), v335);
                                                            w_u32(((uint32)(((sint32)((uint32)(v219) + (uint32)(24))))), v329);
                                                            w_u32(((uint32)(v219)), ((v238 & 0xFF000000) | (r_u32(v209) & 0xFFFFFF)));
                                                            v239 = (v219 & 0xFFFFFF);
                                                            v219 += 32;
                                                            w_u32(v209, ((r_u32(v209) & 0xFF000000) | v239));
                                                        }
                                                        result = r_u32(((uint32)(((sint32)((uint32)(v202) + (uint32)(188))))));
                                                        w_u32(0x800FF668u, v219);
                                                        if (result)
                                                        {
                                                            v240 = ((uint32)(v360));
                                                            w_u32(v360, ((r_u32(v360) & 0xFF000000) | (r_u32(v209) & 0xFFFFFF)));
                                                            result = ((r_u32(v209) & 0xFF000000) | (v240 & 0xFFFFFF));
                                                            w_u32(v209, result);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        v202 = r_u32(((uint32)(((sint32)((uint32)(v202) + (uint32)(4))))));
                    }

                    v241 = r_u32(0x800FF440u);
                    while (2)
                    {
                        if (v241)
                        {
                            v337 = 12;
                            xport_draft_host_sub_8006C564_p13(v306, ((sint32)((uint32)(v241) + (uint32)(24))), &v337);
                            xport_draft_host_sub_8006C3AC_p123(vec303, v306, vec300);
                            vec297[0] = vec303[0];
                            vec297[1] = vec303[1];
                            vec297[2] = vec303[2];
                            gte_T4 = (((unsigned short)(vec303[0])) | ((uint32)(((unsigned short)(vec303[1]))) << (uint32)(16)));
                            xport_draft_gte_data_write(0, gte_T4);
                            xport_draft_gte_data_write(1, vec297[2]);
                            xport_draft_gte_execute(0x180001);
                            if (((r_u32(((uint32)(((sint32)((uint32)(v241) + (uint32)(80)))))) & 0xF) == 1))
                            {
                                v244 = r_u32(0x800FF668u);
                                result = (r_u32(0x800FF374u) < ((uint32)(((uint32)(r_u32(0x800FF668u)) + (uint32)(12)))));
                                if ((r_u32(0x800FF374u) < ((uint32)(((uint32)(r_u32(0x800FF668u)) + (uint32)(12))))))
                                    return;
                                v245 = r_u32(((uint32)(((sint32)((uint32)(v241) + (uint32)(72))))));
                                w_u32(0x800FF668u, (r_u32(0x800FF668u) + (12)));
                                w_u32(((uint32)(v244)), v245);
                                w_u32(((uint32)(((sint32)((uint32)(v244) + (uint32)(4))))), r_u32(((uint32)(((sint32)((uint32)(v241) + (uint32)(76)))))));
                                v338 = xport_draft_gte_data_read(27);
                                result = (v338 < ((sint32)r_u32(0x800FFAE8u)));
                                if ((v338 >= ((sint32)r_u32(0x800FFAE8u))))
                                {
                                    result = ((sint32)((uint32)(v244) + (uint32)(8)));
                                    if ((((sint32)r_u32(0x800FFAD8u)) >= v338))
                                    {
                                        w_u32(((uint32)(((sint32)((uint32)(v244) + (uint32)(8))))), xport_draft_gte_data_read(14));
                                        goto LABEL_220;
                                    }
                                }
                            LABEL_218:
                                w_u32(0x800FF668u, v244);
                            }
                            else
                            {
                                v244 = r_u32(0x800FF668u);
                                result = (r_u32(0x800FF374u) < ((uint32)(((uint32)(r_u32(0x800FF668u)) + (uint32)(16)))));
                                if ((r_u32(0x800FF374u) < ((uint32)(((uint32)(r_u32(0x800FF668u)) + (uint32)(16))))))
                                    return;
                                v246 = r_u32(((uint32)(((sint32)((uint32)(v241) + (uint32)(72))))));
                                w_u32(0x800FF668u, (r_u32(0x800FF668u) + (16)));
                                w_u32(((uint32)(v244)), v246);
                                w_u32(((uint32)(((sint32)((uint32)(v244) + (uint32)(4))))), r_u32(((uint32)(((sint32)((uint32)(v241) + (uint32)(76)))))));
                                w_u32(((uint32)(((sint32)((uint32)(v244) + (uint32)(12))))), r_u32(((uint32)(((sint32)((uint32)(v241) + (uint32)(80)))))));
                                v338 = xport_draft_gte_data_read(27);
                                result = (v338 < ((sint32)r_u32(0x800FFAE8u)));
                                if ((v338 < ((sint32)r_u32(0x800FFAE8u))))
                                    goto LABEL_218;
                                result = ((sint32)((uint32)(v244) + (uint32)(8)));
                                if ((((sint32)r_u32(0x800FFAD8u)) < v338))
                                    goto LABEL_218;
                                w_u32(((uint32)(((sint32)((uint32)(v244) + (uint32)(8))))), xport_draft_gte_data_read(14));
                            LABEL_220:
                                v247 = v338;

                                v248 = r_u32(0x800FF660u);
                                w_u32(((uint32)(v244)), ((r_u32(((uint32)(v244))) & 0xFF000000) | (r_u32(((uint32)(((uint32)(((uint32)((((uint32)(((uint16)(v338))) - (uint32)(r_u16(((uint32)(((sint32)((uint32)(v241) + (uint32)(64)))))))) & 0x3FFC)) + (uint32)(r_u32(0x800FF660u)))) + (uint32)(112))))) & 0xFFFFFF)));
                                v249 = ((uint32)((((sint32)((uint32)(v247) - (uint32)(r_u16(((uint32)(((sint32)((uint32)(v241) + (uint32)(64))))))))) & 0x3FFC)) + (uint32)(v248));
                                result = ((r_u32(((uint32)(((sint32)((uint32)(v249) + (uint32)(112)))))) & 0xFF000000) | (v244 & 0xFFFFFF));
                                w_u32(((uint32)(((sint32)((uint32)(v249) + (uint32)(112))))), result);
                            }
                            v241 = r_u32(((uint32)(((sint32)((uint32)(v241) + (uint32)(4))))));
                            continue;
                        }
                        break;
                    }

                    v250 = 0;
                    v251 = r_u32(0x800FF43Cu);
                    v361 = 0;
                    v362 = 0;
                    v363 = 0;
                    v364 = 0;
                    v365 = 0;
                    v366 = 0;
                    v367 = 0;
                    v368 = 0;
                    v369 = 0;
                    v370 = 0;
                    v375 = 0xFFFFFF;
                    while (2)
                    {
                        if (!v251)
                            return;
                        v252 = (r_u8(((uint32)(((sint32)((uint32)(v251) + (uint32)(88)))))) & 0x10);
                        v372 = 0;
                        if ((v252 || v250))
                            v372 = 1;
                        if (v372)
                        {
                            v339 = 12;
                            xport_draft_host_sub_8006C564_p13(v306, ((sint32)((uint32)(v251) + (uint32)(96))), &v339);
                            xport_draft_host_sub_8006C3AC_p123(vec303, v306, vec300);
                            vec297[0] = vec303[0];
                            vec297[1] = vec303[1];
                            vec297[2] = vec303[2];
                            gte_T4 = (((unsigned short)(vec303[0])) | ((uint32)(((unsigned short)(vec303[1]))) << (uint32)(16)));
                            xport_draft_gte_data_write(0, gte_T4);
                            xport_draft_gte_data_write(1, vec297[2]);
                            xport_draft_gte_execute(0x180001);
                            v255 = r_u32(((uint32)(((sint32)((uint32)(v251) + (uint32)(84))))));
                            v256 = r_u32(((uint32)(((sint32)((uint32)(v255) + (uint32)(4))))));
                            v257 = r_u8(((uint32)(((sint32)((uint32)(v255) + (uint32)(2))))));
                            v340 = xport_draft_gte_data_read(27);
                            result = (v340 < ((sint32)r_u32(0x800FFAE8u)));
                            v250 = 1;
                            if ((v340 >= ((sint32)r_u32(0x800FFAE8u))))
                            {
                                    if ((((sint32)r_u32(0x800FFAD8u)) >= v340))
                                {
                                    v341[0] = ((short)(xport_draft_gte_data_read(14)));
                                    v341[1] = ((short)((xport_draft_gte_data_read(14) >> 16)));
                                    v258 = v341[0];
                                    v259 = v341[1];
                                    v371 = renderer_3736C_divu((uint32)(sint32)(sint16)r_u16(v251 + 94u) * (uint32)v257, (uint32)v340) >> 1;
                                    if ((v371 < 2))
                                        v371 = 2;
                                LABEL_233:
                                    v342 = 12;

                                    xport_draft_host_sub_8006C564_p13(v306, ((sint32)((uint32)(v251) + (uint32)(108))), &v342);
                                    xport_draft_host_sub_8006C3AC_p123(vec303, v306, vec300);
                                    vec297[0] = vec303[0];
                                    vec297[1] = vec303[1];
                                    vec297[2] = vec303[2];
                                    gte_T4 = (((unsigned short)(vec303[0])) | ((uint32)(((unsigned short)(vec303[1]))) << (uint32)(16)));
                                    xport_draft_gte_data_write(0, gte_T4);
                                    xport_draft_gte_data_write(1, vec297[2]);
                                    xport_draft_gte_execute(0x180001);
                                    v263 = r_u32(0x800FF668u);
                                    v264 = ((uint32)(r_u32(0x800FF668u)) + (uint32)(40));
                                    result = 150994944;
                                    if ((r_u32(0x800FF374u) < ((uint32)(((uint32)(r_u32(0x800FF668u)) + (uint32)(40))))))
                                        return;
                                    w_u32(((uint32)(r_u32(0x800FF668u))), 150994944);
                                    v265 = r_u32(((uint32)(((sint32)((uint32)(v251) + (uint32)(76))))));
                                    w_u32(0x800FF668u, v264);
                                    w_u32(((uint32)(((sint32)((uint32)(v263) + (uint32)(4))))), v265);
                                    v266 = r_u32(((uint32)(((sint32)((uint32)(v256) + (uint32)(4))))));
                                    v267 = r_u32(((uint32)(((sint32)((uint32)(v256) + (uint32)(8))))));
                                    v268 = r_u16(((uint32)(((sint32)((uint32)(v256) + (uint32)(10))))));
                                    w_u32(((uint32)(((sint32)((uint32)(v263) + (uint32)(12))))), r_u32(((uint32)(v256))));
                                    w_u32(((uint32)(((sint32)((uint32)(v263) + (uint32)(20))))), v266);
                                    v265 = ((v265 & 0xFFFFFF00u) | (((r_u8(((uint32)(((sint32)((uint32)(v263) + (uint32)(20))))))) & 0xFFu) << 0));
                                    w_u32(((uint32)(((sint32)((uint32)(v263) + (uint32)(28))))), v267);
                                    v266 = ((v266 & 0xFFFFFF00u) | (((r_u8(((uint32)(((sint32)((uint32)(v263) + (uint32)(29))))))) & 0xFFu) << 0));
                                    w_u16(((uint32)(((sint32)((uint32)(v263) + (uint32)(36))))), v268);
                                    w_u8(((uint32)(((sint32)((uint32)(v263) + (uint32)(20))))), ((sint32)((uint32)(v265) - (uint32)(1))));
                                    v265 = ((v265 & 0xFFFFFF00u) | (((r_u8(((uint32)(((sint32)((uint32)(v263) + (uint32)(36))))))) & 0xFFu) << 0));
                                    w_u8(((uint32)(((sint32)((uint32)(v263) + (uint32)(29))))), ((sint32)((uint32)(v266) - (uint32)(1))));
                                    v266 = ((v266 & 0xFFFFFF00u) | (((((uint32)(r_u8(((uint32)(((sint32)((uint32)(v263) + (uint32)(37))))))) - (uint32)(1))) & 0xFFu) << 0));
                                    w_u8(((uint32)(((sint32)((uint32)(v263) + (uint32)(36))))), ((sint32)((uint32)(v265) - (uint32)(1))));
                                    w_u8(((uint32)(((sint32)((uint32)(v263) + (uint32)(37))))), v266);
                                    v340 = xport_draft_gte_data_read(27);
                                    result = (v340 < ((sint32)r_u32(0x800FFAE8u)));
                                    if ((v340 < ((sint32)r_u32(0x800FFAE8u))))
                                        goto LABEL_239;
                                            if ((((sint32)r_u32(0x800FFAD8u)) < v340))
                                        goto LABEL_239;
                                    v343 = ((short)(xport_draft_gte_data_read(14)));
                                    v344 = ((short)((xport_draft_gte_data_read(14) >> 16)));
                                    v269 = ((sint32)((uint32)(((sint16)(r_u16(((uint32)(((sint32)((uint32)(v251) + (uint32)(94))))))))) * (uint32)(v257)));
                                    v270 = v343;
                                    v271 = v344;
                                    v369 = v343;
                                    v272 = renderer_3736C_divu((uint32)v269, (uint32)v340) >> 1;
                                    v370 = v344;
                                    if ((v272 < 2))
                                        v272 = 2;
                                    v273 = ((sint32)((uint32)(v343) - (uint32)(v258)));
                                    v274 = ((sint32)((uint32)(v344) - (uint32)(v259)));
                                    result = sub_80085B54(((uint32)(((sint32)((uint32)(v273) * (uint32)(v273)))) + (uint32)(((sint32)((uint32)(v274) * (uint32)(v274))))));
                                    v275 = result;
                                    if (result)
                                    {
                                        if (v372)
                                        {
                                            v276 = (((sint32)(((sint32)((uint32)(v274) * (uint32)(v371))))) / result);
                                            v277 = (((sint32)(((sint32)((uint32)(v273) * (uint32)(v371))))) / result);
                                            v278 = ((sint32)((uint32)(v258) + (uint32)(v276)));
                                            v279 = ((sint32)((uint32)(v258) - (uint32)(v276)));
                                            v280 = ((sint32)((uint32)(v259) + (uint32)(v277)));
                                            v379 = ((sint32)((uint32)(v259) - (uint32)(v277)));
                                        }
                                        else
                                        {
                                            v278 = v365;
                                            v279 = v367;
                                            v280 = v368;
                                            v379 = v366;
                                        }
                                        v281 = (r_u8(((uint32)(((sint32)((uint32)(v251) + (uint32)(88)))))) & 0x20);
                                        if (((r_u8(((uint32)(((sint32)((uint32)(v251) + (uint32)(88)))))) & 0x10) != 0))
                                        {
                                            v361 = v278;
                                            v363 = v279;
                                            v364 = v280;
                                            v362 = v379;
                                        }
                                        v282 = ((sint32)((uint32)(v274) * (uint32)(v272)));
                                        if (v281)
                                        {
                                            v283 = ((v283 & 0xFFFF0000u) | (((v361) & 0xFFFFu) << 0));
                                            v284 = v362;
                                            v285 = v363;
                                            v286 = v364;
                                            v365 = v361;
                                        }
                                        else
                                        {
                                            v287 = (v282 / v275);
                                            v288 = (((sint32)(((sint32)((uint32)(v273) * (uint32)(v272))))) / v275);
                                            v283 = ((sint32)((uint32)(v270) + (uint32)((v282 / v275))));
                                            v285 = ((sint32)((uint32)(v270) - (uint32)(v287)));
                                            v284 = ((sint32)((uint32)(v271) - (uint32)(v288)));
                                            v286 = ((sint32)((uint32)(v271) + (uint32)(v288)));
                                            v365 = v283;
                                        }
                                        v366 = v284;
                                        v367 = v285;
                                        v289 = r_u8(((uint32)(((sint32)((uint32)(v251) + (uint32)(88))))));
                                        v368 = v286;
                                        if (((v289 & 2) != 0))
                                        {
                                            if (((v289 & 1) != 0))
                                            {
                                                w_u16(((uint32)(((sint32)((uint32)(v263) + (uint32)(8))))), v283);
                                                w_u16(((uint32)(((sint32)((uint32)(v263) + (uint32)(16))))), v285);
                                                w_u16(((uint32)(((sint32)((uint32)(v263) + (uint32)(10))))), v284);
                                                w_u16(((uint32)(((sint32)((uint32)(v263) + (uint32)(18))))), v286);
                                                w_u16(((uint32)(((sint32)((uint32)(v263) + (uint32)(24))))), v278);
                                                w_u16(((uint32)(((sint32)((uint32)(v263) + (uint32)(32))))), v279);
                                                v290 = v379;
                                                w_u16(((uint32)(((sint32)((uint32)(v263) + (uint32)(34))))), v280);
                                                w_u16(((uint32)(((sint32)((uint32)(v263) + (uint32)(26))))), v290);
                                            }
                                            else
                                            {
                                                w_u16(((uint32)(((sint32)((uint32)(v263) + (uint32)(8))))), v285);
                                                w_u16(((uint32)(((sint32)((uint32)(v263) + (uint32)(16))))), v283);
                                                w_u16(((uint32)(((sint32)((uint32)(v263) + (uint32)(10))))), v286);
                                                w_u16(((uint32)(((sint32)((uint32)(v263) + (uint32)(18))))), v284);
                                                w_u16(((uint32)(((sint32)((uint32)(v263) + (uint32)(24))))), v279);
                                                w_u16(((uint32)(((sint32)((uint32)(v263) + (uint32)(32))))), v278);
                                                w_u16(((uint32)(((sint32)((uint32)(v263) + (uint32)(26))))), v280);
                                                w_u16(((uint32)(((sint32)((uint32)(v263) + (uint32)(34))))), v379);
                                            }
                                        }
                                        else if (((v289 & 1) != 0))
                                        {
                                            w_u16(((uint32)(((sint32)((uint32)(v263) + (uint32)(8))))), v279);
                                            w_u16(((uint32)(((sint32)((uint32)(v263) + (uint32)(16))))), v285);
                                            w_u16(((uint32)(((sint32)((uint32)(v263) + (uint32)(10))))), v280);
                                            w_u16(((uint32)(((sint32)((uint32)(v263) + (uint32)(18))))), v286);
                                            w_u16(((uint32)(((sint32)((uint32)(v263) + (uint32)(24))))), v278);
                                            w_u16(((uint32)(((sint32)((uint32)(v263) + (uint32)(32))))), v283);
                                            v291 = v379;
                                            w_u16(((uint32)(((sint32)((uint32)(v263) + (uint32)(34))))), v284);
                                            w_u16(((uint32)(((sint32)((uint32)(v263) + (uint32)(26))))), v291);
                                        }
                                        else
                                        {
                                            w_u16(((uint32)(((sint32)((uint32)(v263) + (uint32)(8))))), v278);
                                            w_u16(((uint32)(((sint32)((uint32)(v263) + (uint32)(16))))), v283);
                                            v292 = v379;
                                            w_u16(((uint32)(((sint32)((uint32)(v263) + (uint32)(18))))), v284);
                                            w_u16(((uint32)(((sint32)((uint32)(v263) + (uint32)(24))))), v279);
                                            w_u16(((uint32)(((sint32)((uint32)(v263) + (uint32)(32))))), v285);
                                            w_u16(((uint32)(((sint32)((uint32)(v263) + (uint32)(26))))), v280);
                                            w_u16(((uint32)(((sint32)((uint32)(v263) + (uint32)(34))))), v286);
                                            w_u16(((uint32)(((sint32)((uint32)(v263) + (uint32)(10))))), v292);
                                        }
                                        v293 = v340;
                                        v294 = r_u32(0x800FF660u);
                                        v295 = v375;
                                        w_u32(((uint32)(v263)), ((r_u32(((uint32)(v263))) & 0xFF000000) | (r_u32(((uint32)(((uint32)(((uint32)((((uint32)(((uint16)(v340))) - (uint32)(r_u16(((uint32)(((sint32)((uint32)(v251) + (uint32)(64)))))))) & 0x3FFC)) + (uint32)(r_u32(0x800FF660u)))) + (uint32)(112))))) & v375)));
                                        v296 = ((uint32)((((sint32)((uint32)(v293) - (uint32)(r_u16(((uint32)(((sint32)((uint32)(v251) + (uint32)(64))))))))) & 0x3FFC)) + (uint32)(v294));
                                        v250 = 0;
                                        result = (v263 & v295);
                                        w_u32(((uint32)(((sint32)((uint32)(v296) + (uint32)(112))))), ((r_u32(((uint32)(((sint32)((uint32)(v296) + (uint32)(112)))))) & 0xFF000000) | (v263 & v295)));
                                    }
                                    else
                                    {
                                    LABEL_239:
                                        w_u32(0x800FF668u, v263);

                                        v250 = 1;
                                    }
                                }
                            }
                            v251 = r_u32(((uint32)(((sint32)((uint32)(v251) + (uint32)(4))))));
                            continue;
                        }
                        break;
                    }

                    v258 = v369;
                    v260 = r_u32(((uint32)(((sint32)((uint32)(v251) + (uint32)(84))))));
                    v259 = v370;
                    v256 = r_u32(((uint32)(((sint32)((uint32)(v260) + (uint32)(4))))));
                    v257 = r_u8(((uint32)(((sint32)((uint32)(v260) + (uint32)(2))))));
                    goto LABEL_233;
                }
                v93 = r_u32(((uint32)(((sint32)((uint32)(k) + (uint32)(92))))));
                w_u32(0x1F800000, r_u32(((uint32)(((sint32)((uint32)(k) + (uint32)(80)))))));
                w_u32(0x1F800004, r_u32(((uint32)(((sint32)((uint32)(k) + (uint32)(84)))))));
                w_u32(0x1F800008, r_u32(((uint32)(((sint32)((uint32)(k) + (uint32)(88)))))));
                v94 = r_u32(((uint32)(((sint32)((uint32)(k) + (uint32)(76))))));
                v95 = (v94 + ((uint32)(2)) * 4u);
                while (1)
                {
                    result = (v92 < r_u32(((uint32)(((sint32)((uint32)(k) + (uint32)(72)))))));
                    if ((v92 >= r_u32(((uint32)(((sint32)((uint32)(k) + (uint32)(72))))))))
                        break;
                    v96 = r_u32((v95 + (1) * 4u));
                    w_u32(0x1F80000C, r_u32(v94));
                    w_u32(0x1F800010, r_u32((v95 - ((uint32)(1)) * 4u)));
                    w_u32(0x1F800014, r_u32(v95));
                    v97 = sub_800850A4(528482304);
                    if ((v97 < 0))
                        goto LABEL_92;
                    v98 = r_u32(0x800FF668u);
                    result = (r_u32(0x800FF374u) < ((uint32)(((uint32)(r_u32(0x800FF668u)) + (uint32)(28)))));
                    v99 = ((uint32)(r_u32(0x800FF668u)) + (uint32)(8));
                    if ((r_u32(0x800FF374u) < ((uint32)(((uint32)(r_u32(0x800FF668u)) + (uint32)(28))))))
                        return;
                    w_u32(0x800FF668u, (r_u32(0x800FF668u) + (28)));
                    w_u32(((uint32)(((sint32)((uint32)(v98) + (uint32)(8))))), 0x4000000);
                    w_u32(((uint32)(((sint32)((uint32)(v99) + (uint32)(4))))), v93);
                    w_u32(((uint32)(((sint32)((uint32)(v99) + (uint32)(12))))), v96);
                    v100 = r_u8(((uint32)(((sint32)((uint32)(k) + (uint32)(96))))));
                    v17 = (v100 != 0);
                    v101 = ((uint32)(4) * (uint32)(v100));
                    if (!v17)
                        v101 = ((uint32)(4) * (uint32)(v97));
                    v102 = ((uint32)(((uint32)(((uint32)(r_u32(0x800FF660u)) + (uint32)(v101))) + (uint32)(112))));
                    w_u32(((uint32)(((sint32)((uint32)(v99) + (uint32)(8))))), r_u32(0x1F800018));
                    w_u32(((uint32)(((sint32)((uint32)(v99) + (uint32)(16))))), r_u32(0x1F80001C));
                    w_u32(((uint32)(v99)), ((r_u32(((uint32)(v99))) & 0xFF000000) | (r_u32(v102) & 0xFFFFFF)));
                    w_u32(v102, ((r_u32(v102) & 0xFF000000) | (v99 & 0xFFFFFF)));
                    v17 = ((v93 & 0x2000000) == 0);
                    v93 = v96;
                    if (!v17)
                    {
                        v103 = r_u32(0x800FF468u);
                        v104 = r_u32(0x800FF46Cu);
                        w_u32(((uint32)(v98)), r_u32(0x800FF468u));
                        w_u32(((uint32)(((sint32)((uint32)(v98) + (uint32)(4))))), v104);
                        w_u32(((uint32)(v98)), ((v103 & 0xFF000000) | (r_u32(v102) & 0xFFFFFF)));
                        w_u32(v102, ((r_u32(v102) & 0xFF000000) | (v98 & 0xFFFFFF)));
                    LABEL_92:
                        v93 = v96;
                    }
                    v95 += (4) * 4u;
                    v94 += (4) * 4u;
                    ++v92;
                    w_u32(0x1F800000, r_u32(0x1F80000C));
                    w_u32(0x1F800004, r_u32(0x1F800010));
                    w_u32(0x1F800008, r_u32(0x1F800014));
                }
            }
        }
        v308 = 12;
        xport_draft_host_sub_8006C564_p13(v307, ((sint32)((uint32)(j) + (uint32)(24))), &v308);
        xport_draft_host_sub_8006C3AC_p123(v306, v307, vec300);
        gte_T4 = (v306[0] & 0xFFFFu) | ((v306[1] & 0xFFFFu) << 16);
        xport_draft_gte_data_write(0, gte_T4);
        xport_draft_gte_data_write(1, v306[2]);
        xport_draft_gte_execute(0x180001);
        v10 = r_u32(0x800FF668u);
        v11 = ((uint32)(r_u32(0x800FF668u)) + (uint32)(8));
        result = (r_u32(0x800FF374u) < ((uint32)(((uint32)(r_u32(0x800FF668u)) + (uint32)(8)))));
        v346 = ((uint32)(r_u32(0x800FF668u)));
        if ((r_u32(0x800FF374u) < ((uint32)(((uint32)(r_u32(0x800FF668u)) + (uint32)(8))))))
            break;
        v12 = r_u32(0x800FF46Cu);
        w_u32(((uint32)(r_u32(0x800FF668u))), r_u32(0x800FF468u));
        w_u32(((uint32)(((sint32)((uint32)(v10) + (uint32)(4))))), v12);
        v349 = ((sint16)(r_u16(((uint32)(((sint32)((uint32)(j) + (uint32)(96))))))));
        v350 = r_u32(((uint32)(((sint32)((uint32)(j) + (uint32)(72))))));
        v13 = r_u32(((uint32)(((sint32)((uint32)(j) + (uint32)(80))))));
        w_u32(0x800FF668u, v11);
        v347 = v13;
        v348 = ((sint16)(r_u16(((uint32)(((sint32)((uint32)(j) + (uint32)(92))))))));
        vec303[0] = xport_draft_gte_data_read(25);
        vec303[1] = xport_draft_gte_data_read(26);
        vec303[2] = xport_draft_gte_data_read(27);
        result = (vec303[2] < 700);
        if ((((uint32)(vec303[2])) < 0x4E21))
        {
            v14 = 0;
            if ((vec303[2] < 700))
                v14 = renderer_3736C_div(255u * (uint32)vec303[2], 700u) + 1;
            if ((vec303[2] >= i))
            {
                v309 = ((unsigned short)(xport_draft_gte_data_read(14)));
                v310 = ((unsigned short)((xport_draft_gte_data_read(14) >> 16)));
                v351 = v309;
                v352 = v310;
            }
            else
            {
                v15 = ((uint32)(r_u16(((uint32)(((uint32)(r_u32(0x800FFB08u)) + (uint32)(16)))))) * (uint32)(vec303[0]));
                v16 = ((uint32)(r_u16(((uint32)(((uint32)(r_u32(0x800FFB08u)) + (uint32)(12)))))) + (uint32)((renderer_3736C_div((uint32)v15, (uint32)vec303[2]))));
                v351 = ((uint32)(r_u16(((uint32)(((uint32)(r_u32(0x800FFB08u)) + (uint32)(12)))))) + (uint32)((renderer_3736C_div((uint32)v15, (uint32)vec303[2]))));
                v17 = (((short)(v16)) < 1024);
                v18 = (((short)(v16)) < -1024);
                if (v17)
                {
                    if (v18)
                        v351 = -1024;
                }
                else
                {
                    v351 = 1023;
                }
                v19 = ((uint32)(r_u16(((uint32)(((uint32)(r_u32(0x800FFB08u)) + (uint32)(16)))))) * (uint32)(vec303[1]));
                v20 = ((uint32)(r_u16(((uint32)(((uint32)(r_u32(0x800FFB08u)) + (uint32)(14)))))) + (uint32)((renderer_3736C_div((uint32)v19, (uint32)vec303[2]))));
                v352 = ((uint32)(r_u16(((uint32)(((uint32)(r_u32(0x800FFB08u)) + (uint32)(14)))))) + (uint32)((renderer_3736C_div((uint32)v19, (uint32)vec303[2]))));
                v17 = (((short)(v20)) < 1024);
                v21 = (((short)(v20)) < -1024);
                if (v17)
                {
                    if (v21)
                        v352 = -1024;
                }
                else
                {
                    v352 = 1023;
                }
            }
            result = ((uint32)(v351) << (uint32)(16));
            v22 = ((short)(v351));
            if (((((short)(v351)) != -1024) && (v351 != 1023)))
            {
                result = ((uint32)(v352) << (uint32)(16));
                v23 = ((short)(v352));
                if (((((short)(v352)) != -1024) && (v352 != 1023)))
                {
                    v353 = 528482304;
                    v24 = (0x800F863Cu + ((v349 & 0xFFF)) * 4u);
                    v25 = ((uint32)(((uint32)(((uint32)(4) * (uint32)(r_u32(((uint32)(((sint32)((uint32)(j) + (uint32)(84))))))))) + (uint32)(528482308))));
                    v26 = ((uint32)(8) * (uint32)(v347));
                    v27 = (v350 + (((uint32)(2) * (uint32)(v347))) * 4u);
                    v28 = ((uint32)(((uint32)(((uint32)(r_u32(0x800FF660u)) + (uint32)((vec303[2] & 0x3FFC)))) + (uint32)(112))));
                    v29 = (((uint32)(400) * (uint32)(((sint32)(r_u32((v27 - ((uint32)(2)) * 4u)))))) / ((uint32)(vec303[2])));
                    v354 = (400 * (sint16)r_u16(v24)) / 256;
                    v355 = ((sint16)(r_u16((((uint32)(v24)) + ((uint32)(1)) * 2u))));
                    v30 = ((sint32)((uint32)(((short)(v351))) + (uint32)((((sint32)(((uint32)(v29) * (uint32)(v354)))) / 4096))));
                    v31 = ((sint32)((uint32)(((short)(v352))) - (uint32)((((sint32)(((uint32)(v29) * (uint32)(v355)))) / 4096))));
                    v32 = (v30 < -100);
                    if ((v30 >= 613))
                    {
                        v30 = ((v30 & 0xFFFF0000u) | (((612) & 0xFFFFu) << 0));
                        v32 = 0;
                    }
                    if (v32)
                        v30 = ((v30 & 0xFFFF0000u) | (((-100) & 0xFFFFu) << 0));
                    v33 = (v31 < -100);
                    if ((v31 >= 341))
                    {
                        v31 = ((v31 & 0xFFFF0000u) | (((340) & 0xFFFFu) << 0));
                        v33 = 0;
                    }
                    v34 = 0;
                    if (v33)
                        v31 = ((v31 & 0xFFFF0000u) | (((-100) & 0xFFFFu) << 0));
                    v35 = v353;
                    v36 = 528482304;
                    w_u16(((uint32)(v353)), v30);
                    w_u16(((uint32)(((sint32)((uint32)(v35) + (uint32)(2))))), v31);
                    v37 = ((sint32)(r_u32((v27 - ((uint32)(2)) * 4u))));
                    v378 = ((uint32)(((uint32)(((uint32)(r_u32(((uint32)(((sint32)((uint32)(j) + (uint32)(76))))))) + (uint32)(v26))) - (uint32)(8))));
                    while (1)
                    {
                        v38 = 0;
                        if ((v34 >= r_u32(((uint32)(((sint32)((uint32)(j) + (uint32)(84))))))))
                            break;
                        v37 = (sint32)((uint32)v37 + r_u32(v378));
                        v39 = renderer_3736C_div(400u * (uint32)v37, (uint32)vec303[2]);
                        v40 = ((sint32)((uint32)(v22) + (uint32)((((sint32)((uint32)(v39) * (uint32)(v354))) / 4096))));
                        v41 = ((sint32)((uint32)(v23) - (uint32)((((sint32)((uint32)(v39) * (uint32)(v355))) / 4096))));
                        v42 = (v40 < -100);
                        if ((v40 >= 613))
                        {
                            v40 = ((v40 & 0xFFFF0000u) | (((612) & 0xFFFFu) << 0));
                            v42 = 0;
                        }
                        if (v42)
                            v40 = ((v40 & 0xFFFF0000u) | (((-100) & 0xFFFFu) << 0));
                        v43 = (v41 < -100);
                        if ((v41 >= 341))
                        {
                            v41 = ((v41 & 0xFFFF0000u) | (((340) & 0xFFFFu) << 0));
                            v43 = 0;
                        }
                        if (v43)
                            v41 = ((v41 & 0xFFFF0000u) | (((-100) & 0xFFFFu) << 0));
                        ++v34;
                        v378 += (((uint32)(2) * (uint32)(v347))) * 4u;
                        w_u16(((uint32)(((sint32)((uint32)(v36) + (uint32)(4))))), v40);
                        v36 += 4;
                        w_u16(((uint32)(((sint32)((uint32)(v36) + (uint32)(2))))), v41);
                    }

                    v357 = ((short)(v351));
                    v349 += v348;
                    v358 = ((short)(v352));
                    v44 = (v350 + ((uint32)(1)) * 4u);
                    v359 = ((uint32)(8) * (uint32)(v347));
                    while ((v38 < v347))
                    {
                        v356 = ((r_u32(((uint32)(((sint32)((uint32)(j) + (uint32)(100)))))) >> ((uint32)v38 & 31u)) & 1);
                        if ((v38 == ((sint32)((uint32)(v347) - (uint32)(1)))))
                            v349 = ((sint16)(r_u16(((uint32)(((sint32)((uint32)(j) + (uint32)(96))))))));
                        v45 = (0x800F863Cu + ((v349 & 0xFFF)) * 4u);
                        v46 = (((uint32)(400) * (uint32)(((sint32)(r_u32(v350))))) / ((uint32)(vec303[2])));
                        v354 = (400 * (sint16)r_u16(v45)) / 256;
                        v355 = ((sint16)(r_u16((((uint32)(v45)) + ((uint32)(1)) * 2u))));
                        v47 = ((sint32)((uint32)(v357) + (uint32)((((sint32)(((uint32)(v46) * (uint32)(v354)))) / 4096))));
                        v48 = ((sint32)((uint32)(v358) - (uint32)((((sint32)(((uint32)(v46) * (uint32)(v355)))) / 4096))));
                        v49 = (v47 < -100);
                        if ((v47 >= 613))
                        {
                            v47 = ((v47 & 0xFFFF0000u) | (((612) & 0xFFFFu) << 0));
                            v49 = 0;
                        }
                        if (v49)
                            v47 = ((v47 & 0xFFFF0000u) | (((-100) & 0xFFFFu) << 0));
                        v50 = (v48 < -100);
                        if ((v48 >= 341))
                        {
                            v48 = ((v48 & 0xFFFF0000u) | (((340) & 0xFFFFu) << 0));
                            v50 = 0;
                        }
                        if (v50)
                            v48 = ((v48 & 0xFFFF0000u) | (((-100) & 0xFFFFu) << 0));
                        w_u16(v25, v47);
                        w_u16((v25 + (1) * 2u), v48);
                        v51 = 0;
                        if ((!(r_u8(((uint32)(((sint32)((uint32)(j) + (uint32)(94))))))) && v356))
                        {
                            v52 = r_u32(0x800FF668u);
                            v53 = ((uint32)(r_u32(0x800FF668u)) + (uint32)(28));
                            result = 100663296;
                            if ((r_u32(0x800FF374u) < ((uint32)(((uint32)(r_u32(0x800FF668u)) + (uint32)(28))))))
                                return;
                            w_u32(((uint32)(r_u32(0x800FF668u))), 100663296);
                            v54 = r_u32(((uint32)(((sint32)((uint32)(j) + (uint32)(88))))));
                            w_u32(0x800FF668u, v53);
                            w_u32(((uint32)(((sint32)((uint32)(v52) + (uint32)(4))))), v54);
                            w_u32(((uint32)(((sint32)((uint32)(v52) + (uint32)(12))))), r_u32(v44));
                            w_u32(((uint32)(((sint32)((uint32)(v52) + (uint32)(20))))), r_u32(v44));
                            if (v14)
                            {
                                v55 = ((uint32)(r_u8(((uint32)(((sint32)((uint32)(v52) + (uint32)(5))))))) * (uint32)(((uint16)(v14))));
                                v56 = ((uint32)(r_u8(((uint32)(((sint32)((uint32)(v52) + (uint32)(6))))))) * (uint32)(((uint16)(v14))));
                                v57 = ((uint32)(r_u8(((uint32)(((sint32)((uint32)(v52) + (uint32)(12))))))) * (uint32)(((uint16)(v14))));
                                v58 = ((uint32)(r_u8(((uint32)(((sint32)((uint32)(v52) + (uint32)(13))))))) * (uint32)(((uint16)(v14))));
                                v59 = ((uint32)(r_u8(((uint32)(((sint32)((uint32)(v52) + (uint32)(14))))))) * (uint32)(((uint16)(v14))));
                                v60 = ((uint32)(r_u8(((uint32)(((sint32)((uint32)(v52) + (uint32)(20))))))) * (uint32)(((uint16)(v14))));
                                v61 = ((uint32)(r_u8(((uint32)(((sint32)((uint32)(v52) + (uint32)(21))))))) * (uint32)(((uint16)(v14))));
                                w_u8(((uint32)(((sint32)((uint32)(v52) + (uint32)(4))))), (((unsigned short)(((uint32)(r_u8(((uint32)(((sint32)((uint32)(v52) + (uint32)(4))))))) * (uint32)(((uint16)(v14)))))) >> 8));
                                w_u8(((uint32)(((sint32)((uint32)(v52) + (uint32)(5))))), ((v55 >> 8) & 255u));
                                v62 = ((v61 >> 8) & 255u);
                                v63 = ((uint32)(r_u8(((uint32)(((sint32)((uint32)(v52) + (uint32)(22))))))) * (uint32)(((uint16)(v14))));
                                w_u8(((uint32)(((sint32)((uint32)(v52) + (uint32)(6))))), ((v56 >> 8) & 255u));
                                w_u8(((uint32)(((sint32)((uint32)(v52) + (uint32)(12))))), ((v57 >> 8) & 255u));
                                w_u8(((uint32)(((sint32)((uint32)(v52) + (uint32)(13))))), ((v58 >> 8) & 255u));
                                w_u8(((uint32)(((sint32)((uint32)(v52) + (uint32)(14))))), ((v59 >> 8) & 255u));
                                w_u8(((uint32)(((sint32)((uint32)(v52) + (uint32)(20))))), ((v60 >> 8) & 255u));
                                w_u8(((uint32)(((sint32)((uint32)(v52) + (uint32)(21))))), v62);
                                w_u8(((uint32)(((sint32)((uint32)(v52) + (uint32)(22))))), ((v63 >> 8) & 255u));
                            }
                            w_u16(((uint32)(((sint32)((uint32)(v52) + (uint32)(8))))), v351);
                            w_u16(((uint32)(((sint32)((uint32)(v52) + (uint32)(10))))), v352);
                            v64 = v353;
                            w_u16(((uint32)(((sint32)((uint32)(v52) + (uint32)(16))))), r_u16(((uint32)(v353))));
                            w_u16(((uint32)(((sint32)((uint32)(v52) + (uint32)(18))))), r_u16(((uint32)(((sint32)((uint32)(v64) + (uint32)(2)))))));
                            w_u16(((uint32)(((sint32)((uint32)(v52) + (uint32)(24))))), r_u16(v25));
                            w_u16(((uint32)(((sint32)((uint32)(v52) + (uint32)(26))))), r_u16((v25 + (1) * 2u)));
                            w_u32(((uint32)(v52)), ((r_u32(((uint32)(v52))) & 0xFF000000) | (r_u32(v28) & 0xFFFFFF)));
                            w_u32(v28, ((r_u32(v28) & 0xFF000000) | (v52 & 0xFFFFFF)));
                            v51 = 0;
                        }
                        v65 = v25;
                        v66 = ((uint32)(v353));
                        v67 = ((sint32)(r_u32(v350)));
                        v378 = ((uint32)(((uint32)(r_u32(((uint32)(((sint32)((uint32)(j) + (uint32)(76))))))) + (uint32)(((uint32)(8) * (uint32)(v38))))));
                        while ((v51 < r_u32(((uint32)(((sint32)((uint32)(j) + (uint32)(84))))))))
                        {
                            v67 = (sint32)((uint32)v67 + r_u32(v378));
                            v68 = renderer_3736C_div(400u * (uint32)v67, (uint32)vec303[2]);
                            v69 = ((sint32)((uint32)(v357) + (uint32)((((sint32)((uint32)(v68) * (uint32)(v354))) / 4096))));
                            v70 = ((sint32)((uint32)(v358) - (uint32)((((sint32)((uint32)(v68) * (uint32)(v355))) / 4096))));
                            v71 = (v69 < -100);
                            if ((v69 >= 613))
                            {
                                v69 = ((v69 & 0xFFFF0000u) | (((612) & 0xFFFFu) << 0));
                                v71 = 0;
                            }
                            if (v71)
                                v69 = ((v69 & 0xFFFF0000u) | (((-100) & 0xFFFFu) << 0));
                            v72 = (v70 < -100);
                            if ((v70 >= 341))
                            {
                                v70 = ((v70 & 0xFFFF0000u) | (((340) & 0xFFFFu) << 0));
                                v72 = 0;
                            }
                            if (v72)
                                v70 = ((v70 & 0xFFFF0000u) | (((-100) & 0xFFFFu) << 0));
                            w_u16((v65 + (2) * 2u), v69);
                            w_u16((v65 + (3) * 2u), v70);
                            if (v356)
                            {
                                v73 = r_u32(0x800FF668u);
                                v74 = ((uint32)(r_u32(0x800FF668u)) + (uint32)(36));
                                result = 0x8000000;
                                if ((r_u32(0x800FF374u) < ((uint32)(((uint32)(r_u32(0x800FF668u)) + (uint32)(36))))))
                                    return;
                                w_u32(((uint32)(r_u32(0x800FF668u))), 0x8000000);
                                v75 = v378;
                                v76 = r_u32((v378 + (1) * 4u));
                                w_u32(0x800FF668u, v74);
                                w_u32(((uint32)(((sint32)((uint32)(v73) + (uint32)(4))))), v76);
                                w_u32(((uint32)(((sint32)((uint32)(v73) + (uint32)(12))))), v76);
                                if (v51)
                                {
                                    v77 = r_u32(v75 - v359 + 4u);
                                    w_u32(((uint32)(((sint32)((uint32)(v73) + (uint32)(20))))), v77);
                                }
                                else
                                {
                                    w_u32(((uint32)(((sint32)((uint32)(v73) + (uint32)(20))))), r_u32(v44));
                                    v77 = r_u32(v44);
                                }
                                w_u32(((uint32)(((sint32)((uint32)(v73) + (uint32)(28))))), v77);
                                if (v14)
                                {
                                    v78 = ((uint32)(r_u8(((uint32)(((sint32)((uint32)(v73) + (uint32)(4))))))) * (uint32)(((uint16)(v14))));
                                    v79 = ((uint32)(r_u8(((uint32)(((sint32)((uint32)(v73) + (uint32)(5))))))) * (uint32)(((uint16)(v14))));
                                    v80 = ((uint32)(r_u8(((uint32)(((sint32)((uint32)(v73) + (uint32)(6))))))) * (uint32)(((uint16)(v14))));
                                    v81 = ((uint32)(r_u8(((uint32)(((sint32)((uint32)(v73) + (uint32)(12))))))) * (uint32)(((uint16)(v14))));
                                    v379 = ((uint32)(r_u8(((uint32)(((sint32)((uint32)(v73) + (uint32)(13))))))) * (uint32)(v14));
                                    v82 = ((uint32)(r_u8(((uint32)(((sint32)((uint32)(v73) + (uint32)(14))))))) * (uint32)(((uint16)(v14))));
                                    v377 = ((uint32)(r_u8(((uint32)(((sint32)((uint32)(v73) + (uint32)(20))))))) * (uint32)(v14));
                                    v83 = ((uint32)(r_u8(((uint32)(((sint32)((uint32)(v73) + (uint32)(21))))))) * (uint32)(((uint16)(v14))));
                                    v84 = ((uint32)(r_u8(((uint32)(((sint32)((uint32)(v73) + (uint32)(22))))))) * (uint32)(((uint16)(v14))));
                                    v376 = ((uint32)(r_u8(((uint32)(((sint32)((uint32)(v73) + (uint32)(28))))))) * (uint32)(v14));
                                    v85 = r_u8(((uint32)(((sint32)((uint32)(v73) + (uint32)(29))))));
                                    w_u8(((uint32)(((sint32)((uint32)(v73) + (uint32)(4))))), ((v78 >> 8) & 255u));
                                    v86 = ((sint32)((uint32)(v85) * (uint32)(v14)));
                                    w_u8(((uint32)(((sint32)((uint32)(v73) + (uint32)(5))))), ((v79 >> 8) & 255u));
                                    w_u8(((uint32)(((sint32)((uint32)(v73) + (uint32)(6))))), ((v80 >> 8) & 255u));
                                    w_u8(((uint32)(((sint32)((uint32)(v73) + (uint32)(12))))), ((v81 >> 8) & 255u));
                                    w_u8(((uint32)(((sint32)((uint32)(v73) + (uint32)(13))))), ((v379 >> 8) & 255u));
                                    v87 = r_u8(((uint32)(((sint32)((uint32)(v73) + (uint32)(30))))));
                                    w_u8(((uint32)(((sint32)((uint32)(v73) + (uint32)(14))))), ((v82 >> 8) & 255u));
                                    w_u8(((uint32)(((sint32)((uint32)(v73) + (uint32)(20))))), ((v377 >> 8) & 255u));
                                    w_u8(((uint32)(((sint32)((uint32)(v73) + (uint32)(21))))), ((v83 >> 8) & 255u));
                                    w_u8(((uint32)(((sint32)((uint32)(v73) + (uint32)(22))))), ((v84 >> 8) & 255u));
                                    w_u8(((uint32)(((sint32)((uint32)(v73) + (uint32)(28))))), ((v376 >> 8) & 255u));
                                    w_u8(((uint32)(((sint32)((uint32)(v73) + (uint32)(29))))), ((v86 >> 8) & 255u));
                                    w_u8(((uint32)(((sint32)((uint32)(v73) + (uint32)(30))))), (((unsigned short)(((sint32)((uint32)(v87) * (uint32)(v14))))) >> 8));
                                }
                                v88 = r_u32(((uint32)(v73)));
                                w_u16(((uint32)(((sint32)((uint32)(v73) + (uint32)(24))))), r_u16(v66));
                                w_u16(((uint32)(((sint32)((uint32)(v73) + (uint32)(26))))), r_u16((v66 + (1) * 2u)));
                                w_u16(((uint32)(((sint32)((uint32)(v73) + (uint32)(32))))), r_u16(v65));
                                w_u16(((uint32)(((sint32)((uint32)(v73) + (uint32)(34))))), r_u16((v65 + (1) * 2u)));
                                w_u16(((uint32)(((sint32)((uint32)(v73) + (uint32)(8))))), r_u16((v66 + (2) * 2u)));
                                w_u16(((uint32)(((sint32)((uint32)(v73) + (uint32)(10))))), r_u16((v66 + (3) * 2u)));
                                w_u16(((uint32)(((sint32)((uint32)(v73) + (uint32)(16))))), r_u16((v65 + (2) * 2u)));
                                w_u16(((uint32)(((sint32)((uint32)(v73) + (uint32)(18))))), r_u16((v65 + (3) * 2u)));
                                w_u32(((uint32)(v73)), ((v88 & 0xFF000000) | (r_u32(v28) & 0xFFFFFF)));
                                w_u32(v28, ((r_u32(v28) & 0xFF000000) | (v73 & 0xFFFFFF)));
                            }
                            v65 += (2) * 2u;
                            v66 += (2) * 2u;
                            ++v51;
                            v378 = ((uint32)((((uint32)(v378)) + ((uint32)(v359)) * 1u)));
                        }

                        v44 += (2) * 4u;
                        v89 = v353;
                        ++v38;
                        v353 = ((sint32)(v25));
                        v25 = ((uint32)(v89));
                        v349 += v348;
                        v350 += (2) * 4u;
                    }

                    v90 = ((uint32)(v346));
                    w_u32(v346, ((r_u32(v346) & 0xFF000000) | (r_u32(v28) & 0xFFFFFF)));
                    result = ((r_u32(v28) & 0xFF000000) | (v90 & 0xFFFFFF));
                    w_u32(v28, result);
                }
            }
        }
    }

    return;
}
