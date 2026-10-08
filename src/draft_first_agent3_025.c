#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
#include <stdlib.h>
/* TODO Missing call adapter xport_draft_missing_gte_adapter */
uint32 sub_80080EF4(uint32 a1, uint32 a2)
{
  /* TODO Recover GTE register carriers and missing host GTE adapters */
  uint32 gte_T1 = 0u;
  uint32 gte_T2 = 0u;
  uint32 gte_T4 = 0u;
  uint32 gte_T5 = 0u;
  uint32 gte_T6 = 0u;
  
  sint32 result;
  sint32 v3;
  sint32 v4;
  sint32 v5;
  sint32 v6;
  sint32 v7;
  sint32 v8;
  sint32 v9;
  uint32 v10;
  uint32 v11;
  sint32 v12;
  uint32 v13;
  uint32 v16;
  sint32 v20;
  sint32 v22;
  sint32 v23;
  sint32 v24;
  uint32 v25;
  uint32 v26;
  sint32 v27;
  result = 255;
  if (a2)
  {
    v3 = ((r_u32(((uint32)(r_u32(0x800FFB08u))))>>16)&65535u);
    v4 = ((unsigned short)(r_u32(((uint32)(r_u32(0x800FFB08u))))));
    v5 = ((r_u32(((uint32)((r_u32(0x800FFB08u) + 4))))>>16)&65535u);
    v6 = ((unsigned short)(r_u32(((uint32)((r_u32(0x800FFB08u) + 4))))));
    v7 = ((r_u32(((uint32)((r_u32(0x800FFB08u) + 8))))>>16)&65535u);
    v8 = ((unsigned short)(r_u32(((uint32)((r_u32(0x800FFB08u) + 8))))));
    v9 = 0xFFFF;
    v10 = ((uint32)(r_u32(0x800FFAC0u)));
    v11 = ((uint32)(r_u32(0x800FFAC4u)));
    v12 = (r_u32(0x800FFAC0u) + 7992);
    v13 = ((uint32)(0x800FFB18u));
    gte_T1 = ((sint32)(r_u32(a1)));
    gte_T2 = ((sint32)(r_u32((a1+(1)*4u))));
    do
    {
      (abort(),0u);
      a1 += (2)*4u;
      --a2;
      (abort(),0u);
      v16 = ((gte_T2>>16)&65535u);
      if (((gte_T2 & 0x20000) != 0))
      {
        v25 = ((uint32)((v12 - gte_T1)));
        v26 = ((sint32)(r_u32((v25+(1)*4u))));
        v27 = r_u32(((uint32)(((((uint32)(v11))+(((uint32)(v25)))*1u) - ((uint32)(v10))))));
        w_u32(v10,((sint32)(r_u32(v25))));
        w_u32((v10+(1)*4u),v26);
        w_u32(v11,v27);
        v9 &= ((v26>>16)&65535u);
        gte_T1 = ((sint32)(r_u32(a1)));
        gte_T2 = ((sint32)(r_u32((a1+(1)*4u))));
      }
      else
      {
        gte_T1 = ((sint32)(r_u32(a1)));
        gte_T2 = ((sint32)(r_u32((a1+(1)*4u))));
        (abort(),0u);
        v20 = (((unsigned short)(gte_T4)) | (gte_T5 << 16));
        (abort(),0u);
        v22 = ((((((v4 < ((short)(gte_T5))) | (2 * (((short)(gte_T5)) < v6))) | (4 * (v3 < (gte_T5 >> 16)))) | (8 * ((gte_T5 >> 16) < v5))) | (16 * (gte_T6 < v8))) | (32 * (v7 < gte_T6)));
        if ((gte_T6 < 0))
          v22 ^= 0xFu;
        v23 = (v22 | ((v22 << 8) ^ 0xFF00));
        v9 &= v23;
        v24 = ((v23 << 16) | ((unsigned short)(gte_T6)));
        w_u32(v10,gte_T5);
        w_u32((v10+(1)*4u),v24);
        w_u32(v11,v20);
        if (((v16 & 1) != 0))
        {
          w_u32(v13,gte_T5);
          w_u32((v13+(1)*4u),v24);
          w_u32(((uint32)(((((uint32)(v11))+(((uint32)(v13)))*1u) - ((uint32)(v10))))),v20);
          v13 -= (2)*4u;
        }
      }
      v10 += (2)*4u;
      v11 += (2)*4u;
    }
    while (a2);
    w_u32(0x800FFB18u,((sint32)(v13)));
    w_u32(0x1F8001D4,v10);
    return v9;
  }
  return result;
}
/* TODO Missing call adapter xport_draft_missing_gte_adapter */
void sub_800821E0(uint32 geometry)
{
  uint32 packed = r_u32(geometry);
  uint32 depth = r_u32(geometry+4);
  xport_draft_gte_data_write(9,packed);
  xport_draft_gte_data_write(10,(uint32)((sint32)packed>>16));
  xport_draft_gte_data_write(11,depth);
}
/* TODO Missing call adapter sub_800878DC */
/* TODO Missing call adapter xport_draft_missing_gte_adapter */
uint32 sub_8007A8F8(uint32 a1, uint32 a2, uint32 a3, uint32 a4, uint32 a5, uint32 a6, uint32 a7, uint32 a8, uint32 a9, uint32 a10)
{
  /* TODO Recover GTE register carriers and missing host GTE adapters */
  uint32 gte_T4 = 0u;
  uint32 gte_T5 = 0u;
  uint32 gte_T6 = 0u;
  uint32 gte_T8 = 0u;
  
  sint32 v12;
  sint32 v13;
  sint32 v14;
  sint32 v15;
  sint32 v16;
  sint32 v17;
  sint32 v18;
  sint32 v19;
  sint32 v26;
  sint32 v27;
  short v34;
  sint32 v41;
  uint32 result;
  sint32 v53;
  sint32 v57;
  sint32 v77;
  uint32 v78;
  sint32 v79;
  short v80;
  sint32 v81;
  sint32 v82;
  char v83[24];
  char v84[24];
  short v85[12];
  short v86[12];
  short v87;
  short v88;
  short v89;
  short v90;
  short v91;
  short v92;
  unsigned short v93;
  unsigned short v94;
  unsigned short v95;
  uint16 v96[4];
  short v97;
  short v98;
  short v99;
  short v100;
  short v101;
  short v102;
  short v103;
  short v104;
  short v105;
  unsigned short v106;
  unsigned short v107;
  unsigned short v108;
  short v109;
  short v110;
  short v111;
  short v112;
  short v113;
  short v114;
  short v115[4];
  sint32 v116;
  sint32 v117;
  sint32 v118;
  sint32 v119;
  uint32 v120;
  short v121;
  short v122;
  short v123;
  v12 = r_u32((a1+(6)*4u));
  v13 = r_u32((a1+(7)*4u));
  v14 = ((unsigned short)(r_u32((a1+(5)*4u))));
  v15 = (((sint32)(r_u32((a1+(5)*4u)))) >> 16);
  v106 = ((v14 + v15) >> 1);
  v107 = ((((unsigned short)(r_u32((a1+(6)*4u)))) + (((sint32)(r_u32((a1+(6)*4u)))) >> 16)) >> 1);
  v16 = r_u32((a1+(7)*4u));
  v17 = ((5 * ((v14 - v15) >> 1)) >> 2);
  v121 = v17;
  v18 = ((11 * ((((unsigned short)(v12)) - (v12 >> 16)) >> 1)) >> 3);
  v19 = ((5 * ((((unsigned short)(v13)) - (v13 >> 16)) >> 1)) >> 2);
  v108 = ((((unsigned short)(v16)) + (v16 >> 16)) >> 1);
  v122 = v18;
  v123 = v19;
  gte_T4 = v106;
  gte_T5 = v107;
  gte_T6 = v108;
  (abort(),0u);
  sub_80085C94(a2);
  (abort(),0u);
  v87 = gte_T4;
  v88 = gte_T5;
  v89 = gte_T6;
  v87 = (gte_T4 + r_u16((a2+(10)*2u)));
  v88 = (gte_T5 + r_u16((a2+(12)*2u)));
  v89 = (gte_T6 + r_u16((a2+(14)*2u)));
  v26 = (((((short)(v17)) + ((short)(v18))) + ((short)(v19))) / 3);
  v27 = ((v26 << 16) >> 4);
  xport_draft_host_sub_80085C04_p2(a2,v83);
  sub_80085C34(v83,v84);
  sub_80085BE8(v85);
  v85[0] = (v27 / ((short)(v17)));
  v85[4] = (v27 / ((short)(v18)));
  v85[8] = (v27 / ((short)(v19)));
  gte_T4 = r_u16(a9);
  gte_T5 = r_u16((a9+(1)*2u));
  gte_T6 = r_u16((a9+(2)*2u));
  (abort(),0u);
  sub_80085C94(v84);
  sub_80085D64();
  sub_80085C94(v85);
  (abort(),0u);
  v93 = gte_T4;
  v94 = gte_T5;
  v95 = gte_T6;
  v34 = sub_80085D14();
  (abort(),0u);
  v92 = 0;
  v91 = 0;
  v90 = 0;
  if (((v117 < v116) || (v118 < v116)))
  {
    if (((v116 < v117) || (v118 < v117)))
      v92 = 4096;
    else
      v91 = 4096;
  }
  else
  {
    v90 = 4096;
  }
  gte_T4 = v93;
  gte_T5 = v94;
  gte_T6 = v95;
  (abort(),0u);
  sub_80085D30(&v90);
  (abort(),0u);
  v96[0] = gte_T4;
  v96[1] = gte_T5;
  v96[2] = gte_T6;
  sub_80085D64();
  v41 = ((((((v121 * v122) / ((short)(v26))) * v123) * v34) / ((short)(v26))) >> 12);
  sub_80085C94(v85);
  sub_80085C94(v83);
  sub_80085D30(a9);
  sub_80085DBC(v41);
  (abort(),0u);
  v97 = gte_T4;
  v98 = gte_T5;
  v99 = gte_T6;
  gte_T4 = v93;
  gte_T5 = v94;
  gte_T6 = v95;
  (abort(),0u);
  sub_80085D30(v96);
  sub_80085D64();
  sub_80085C94(v85);
  sub_80085C94(v83);
  sub_80085D30(a9);
  sub_80085DBC(v41);
  (abort(),0u);
  v100 = gte_T4;
  v101 = gte_T5;
  v102 = gte_T6;
  gte_T8 = a3;
  (abort(),0u);
  sub_80085CD4(a9);
  (abort(),0u);
  result = (v119 < r_u32(0x800FF964u));
  if ((v119 >= r_u32(0x800FF964u)))
  {
    v86[0] = ((((sint32)(r_u32(a4))) >> 12) - v87);
    v86[1] = ((((sint32)(r_u32((a4+(1)*4u)))) >> 12) - v88);
    v53 = ((sint32)(r_u32((a4+(2)*4u))));
    v86[4] = v98;
    v86[5] = v99;
    v86[6] = v100;
    v86[7] = v101;
    v86[8] = v102;
    v86[3] = v97;
    v86[2] = ((v53 >> 12) - v89);
    sub_80085CD4(v86);
    result = ((uint32)(&v103));
    (abort(),0u);
    v103 = gte_T4;
    v104 = gte_T5;
    v105 = gte_T6;
    v57 = ((short)(gte_T4));
    if ((((short)(gte_T4)) <= 0))
    {
      v87 -= r_u16(((uint32)((r_u32(0x800FFB0Cu) + 4))));
      v88 -= r_u16(((uint32)((r_u32(0x800FFB0Cu) + 8))));
      v89 -= r_u16(((uint32)((r_u32(0x800FFB0Cu) + 12))));
      gte_T4 = r_u16(a9);
      gte_T5 = r_u16((a9+(1)*2u));
      gte_T6 = r_u16((a9+(2)*2u));
      (abort(),0u);
      sub_80085DD8(&v87,((v57 << 12) / v119));
      (abort(),0u);
      v106 = gte_T4;
      v107 = gte_T5;
      v108 = gte_T6;
      gte_T4 = r_u16(a9);
      gte_T5 = r_u16((a9+(1)*2u));
      gte_T6 = r_u16((a9+(2)*2u));
      (abort(),0u);
      sub_80085DD8(&v97,((-4096 * v104) / v119));
      (abort(),0u);
      v109 = gte_T4;
      v110 = gte_T5;
      v111 = gte_T6;
      gte_T4 = r_u16(a9);
      gte_T5 = r_u16((a9+(1)*2u));
      gte_T6 = r_u16((a9+(2)*2u));
      (abort(),0u);
      sub_80085DD8(&v100,((-4096 * v105) / v119));
      (abort(),0u);
      v112 = gte_T4;
      v113 = gte_T5;
      v114 = gte_T6;
      ((void)((r_u32(0x800FFB0Cu) + 116)),abort(),0u);
      (abort(),0u);
      gte_T4 = v115;
      (abort(),0u);
      v115[0] = ((v112 + v106) + v109);
      v115[1] = ((v113 + v107) + v110);
      v115[2] = ((v114 + v108) + v111);
      (abort(),0u);
      if ((v120 >= 0x1000))
        v120 = 4095;
      v77 = r_u32(0x800FF668u);
      result = (r_u32(0x800FF374u) < ((uint32)((r_u32(0x800FF668u) + 40))));
      w_u32(0x800FF668u,(r_u32(0x800FF668u)+(40)));
      if (result)
      {
        w_u32(0x800FF668u,v77);
      }
      else
      {
        sub_8008AFFC(v77);
        w_u8(((uint32)((v77 + 7))),(r_u8(((uint32)((v77 + 7))))|(2u)));
        v78 = ((uint32)(r_u32(0x800FFAC0u)));
        w_u16(((uint32)((v77 + 8))),r_u16(((uint32)(r_u32(0x800FFAC0u)))));
        w_u16(((uint32)((v77 + 10))),r_u16((v78+(1)*2u)));
        w_u16(((uint32)((v77 + 16))),r_u16((v78+(4)*2u)));
        w_u16(((uint32)((v77 + 18))),r_u16((v78+(5)*2u)));
        w_u16(((uint32)((v77 + 24))),r_u16((v78+(8)*2u)));
        w_u16(((uint32)((v77 + 26))),r_u16((v78+(9)*2u)));
        w_u16(((uint32)((v77 + 32))),r_u16((v78+(12)*2u)));
        w_u16(((uint32)((v77 + 34))),r_u16((v78+(13)*2u)));
        w_u16(((uint32)((v77 + 4))),a10);
        w_u8(((uint32)((v77 + 6))),((a10>>16)&255u));
        w_u8(((uint32)((v77 + 12))),r_u8(((uint32)(r_u32(0x800FF408u)))));
        w_u8(((uint32)((v77 + 13))),r_u8(((uint32)((r_u32(0x800FF408u) + 1)))));
        w_u8(((uint32)((v77 + 20))),(r_u8(((uint32)((r_u32(0x800FF408u) + 4)))) - 1));
        w_u8(((uint32)((v77 + 21))),r_u8(((uint32)((r_u32(0x800FF408u) + 5)))));
        w_u8(((uint32)((v77 + 28))),r_u8(((uint32)((r_u32(0x800FF408u) + 8)))));
        w_u8(((uint32)((v77 + 29))),(r_u8(((uint32)((r_u32(0x800FF408u) + 9)))) - 1));
        w_u8(((uint32)((v77 + 36))),(r_u8(((uint32)((r_u32(0x800FF408u) + 10)))) - 1));
        w_u8(((uint32)((v77 + 37))),(r_u8(((uint32)((r_u32(0x800FF408u) + 11)))) - 1));
        v79 = r_u32(0x800FF408u);
        w_u16(((uint32)((v77 + 14))),r_u16(((uint32)((r_u32(0x800FF408u) + 2)))));
        v80 = r_u16(((uint32)((v79 + 6))));
        v81 = r_u32(((uint32)(v77)));
        w_u16(((uint32)((v77 + 22))),((v80 & 0xFF9F) | 0x40));
        v82 = ((4 * v120) + r_u32(0x800FF660u));
        w_u32(((uint32)(v77)),((v81 & 0xFF000000) | (r_u32(((uint32)((v82 + 112)))) & 0xFFFFFF)));
        result = ((r_u32(((uint32)((v82 + 112)))) & 0xFF000000) | (v77 & 0xFFFFFF));
        w_u32(((uint32)((v82 + 112))),result);
      }
    }
  }
  return result;
}
/* TODO Missing call adapter v10 */
/* TODO Missing call adapter xport_draft_missing_gte_adapter */
uint32 sub_80085D64(void)
{
  /* TODO Recover GTE register carriers and missing host GTE adapters */
  uint32 gte_T1 = 0u;
  uint32 gte_T2 = 0u;
  uint32 gte_T3 = 0u;
  
  sint32 v3;
  sint32 v4;
  sint32 v6;
  sint32 v8;
  uint32 v10;
  (abort(),0u);
  v3 = sub_80085D14();
  gte_T1 = ((v4 << 12) / v3);
  gte_T2 = ((v6 << 12) / v3);
  gte_T3 = ((v8 << 12) / v3);
  (abort(),0u);
  return (abort(),0u);
}
/* TODO Missing call adapter xport_draft_missing_gte_adapter */
uint32 sub_80085D14(void)
{
  /* TODO Recover GTE register carriers and missing host GTE adapters */
  uint32 gte_T1 = 0u;
  uint32 gte_T2 = 0u;
  uint32 gte_T3 = 0u;
  
  (abort(),0u);
  return sub_80085B54(((gte_T1 + gte_T2) + gte_T3));
}
/* TODO Missing call adapter xport_draft_missing_gte_adapter */
void sub_80085D30(uint32 a1)
{
  /* TODO Recover GTE register carriers and missing host GTE adapters */
  uint32 gte_T1 = 0u;
  uint32 gte_T2 = 0u;
  uint32 gte_T3 = 0u;
  
  gte_T3 = r_u32((a1+(1)*4u));
  gte_T2 = ((r_u32(a1)>>16)&65535u);
  gte_T1 = ((unsigned short)(r_u32(a1)));
  (abort(),0u);
}
/* TODO Missing call adapter xport_draft_missing_gte_adapter */
void sub_80085CD4(uint32 a1)
{
  /* TODO Recover GTE register carriers and missing host GTE adapters */
  uint32 gte_T0 = 0u;
  uint32 gte_T1 = 0u;
  uint32 gte_T2 = 0u;
  uint32 gte_T3 = 0u;
  uint32 gte_T4 = 0u;
  
  gte_T0 = ((sint32)(r_u32(a1)));
  gte_T1 = ((sint32)(r_u32((a1+(1)*4u))));
  gte_T2 = ((sint32)(r_u32((a1+(2)*4u))));
  gte_T3 = ((sint32)(r_u32((a1+(3)*4u))));
  gte_T4 = ((sint32)(r_u32((a1+(4)*4u))));
  (abort(),0u);
}
uint32 sub_8008AFFC(uint32 a1)
{
  sint32 result;
  w_u8(((uint32)((a1 + 3))),9);
  result = 44;
  w_u8(((uint32)((a1 + 7))),44);
  return result;
}
uint32 sub_8006D164(uint32 a1, uint32 a2, uint32 a3, uint32 a4)
{
  short v4;
  short v5;
  short v6;
  sint32 v7;
  sint32 result;
  short v9;
  short v10;
  v4 = ((unsigned char)(((sint8)(r_u8((a4+(2)*1u))))));
  v5 = ((unsigned char)(((sint8)(r_u8((a4+(3)*1u))))));
  v6 = (a1 + ((sint8)(r_u8(a4))));
  v7 = (a2 + ((sint8)(r_u8((a4+(1)*1u)))));
  w_u16((a3+(4)*2u),v6);
  w_u16((a3+(5)*2u),v7);
  w_u16((a3+(8)*2u),(v6 + v4));
  result = v7;
  w_u16((a3+(13)*2u),(v7 + v5));
  v9 = v6;
  v10 = r_u16((a3+(8)*2u));
  v7 = ((v7&0xFFFF0000u)|(((r_u16((a3+(13)*2u)))&0xFFFFu)<<0));
  w_u16((a3+(9)*2u),result);
  w_u16((a3+(12)*2u),v9);
  w_u16((a3+(16)*2u),v10);
  w_u16((a3+(17)*2u),v7);
  return result;
}
uint32 sub_8006D294(uint32 a1, uint32 a2, uint32 a3, uint32 a4, uint32 a5, uint32 a6, uint32 a7, uint32 a8, uint32 a9, uint32 a10, uint32 a11)
{
  sint32 v15;
  uint32 result;
  sint32 v17;
  sint32 v18;
  v15 = r_u32(0x800FF668u);
  result = (r_u32(0x800FF374u) < ((uint32)((r_u32(0x800FF668u) + 36))));
  if ((r_u32(0x800FF374u) >= ((uint32)((r_u32(0x800FF668u) + 36)))))
  {
    w_u32(0x800FF668u,(r_u32(0x800FF668u)+(36)));
    w_u8(((uint32)((v15 + 3))),8);
    w_u8(((uint32)((v15 + 7))),56);
    w_u16(((uint32)((v15 + 16))),(a1 + a3));
    w_u16(((uint32)((v15 + 32))),(a1 + a3));
    w_u8(((uint32)((v15 + 4))),a8);
    w_u8(((uint32)((v15 + 12))),a5);
    w_u8(((uint32)((v15 + 5))),a9);
    w_u8(((uint32)((v15 + 13))),a6);
    w_u8(((uint32)((v15 + 6))),a10);
    w_u8(((uint32)((v15 + 14))),a7);
    w_u8(((uint32)((v15 + 20))),a8);
    w_u8(((uint32)((v15 + 28))),a5);
    w_u8(((uint32)((v15 + 21))),a9);
    w_u8(((uint32)((v15 + 29))),a6);
    w_u8(((uint32)((v15 + 22))),a10);
    w_u8(((uint32)((v15 + 30))),a7);
    w_u16(((uint32)((v15 + 26))),(a2 + a4));
    w_u16(((uint32)((v15 + 34))),(a2 + a4));
    v17 = r_u32(0x800FF660u);
    w_u16(((uint32)((v15 + 10))),a2);
    w_u16(((uint32)((v15 + 18))),a2);
    w_u16(((uint32)((v15 + 8))),a1);
    w_u16(((uint32)((v15 + 24))),a1);
    v18 = ((4 * a11) + v17);
    w_u32(((uint32)(v15)),((r_u32(((uint32)(v15))) & 0xFF000000) | (r_u32(((uint32)((v18 + 112)))) & 0xFFFFFF)));
    result = ((r_u32(((uint32)((v18 + 112)))) & 0xFF000000) | (v15 & 0xFFFFFF));
    w_u32(((uint32)((v18 + 112))),result);
  }
  return result;
}
/* TODO Missing call adapter SHIWORD */
/* TODO Missing call adapter sub_80098068 */
/* TODO Missing call adapter sub_8009AC6C */
/* TODO Missing call adapter sub_8009BDA0 */
/* TODO Missing call adapter sub_8009BE1C */
/* TODO Postincrement memory expressions may require ordering refinement */
uint32 sub_8002F284(void)
{
  sint32 result;
  sint32 v1;
  unsigned char v2;
  uint32 v3;
  sint32 v4;
  sint32 v5;
  sint32 v6;
  sint32 v7;
  sint32 v8;
  sint32 v9;
  sint32 v10;
  uint32 v11;
  sint32 v12;
  result = r_u32(0x800FF250u);
  if (r_u32(0x800FF250u))
  {
    if (!r_u32(0x800FF26Cu))
    {
      if (r_u32(0x800FF24Cu))
      {
        w_u32(0x800FF24Cu,0);
        if ((!r_u32(0x800FF248u) && !r_u32(0x800FF378u)))
        {
          v1 = (sub_80066570(4) == 1);
          v2 = 31;
          if (!v1)
            v2 = (sub_80066570(11) + 4);
          w_u8(0x800FF299u,v2);
          v3 = (0x800A5B8Cu+((7 * v2))*4u);
          v4 = r_u16((((uint32)(v3))+(4)*2u));
          v5 = ((sint32)(((sint32)(r_u32((v3+(6)*4u))))));
          w_u32(0x800FF27Cu,v4);
          w_u32(0x800FF278u,v5);
        }
        sub_8002FA48();
      }
      if (r_u32(0x800FF260u))
      {
        ((void)(r_u32((0x800FF2A4u+(r_u32(0x800FF25Cu))*4u))),(void)(0),abort(),0u);
        v6 = r_u32((0x800A5F68u+(r_u32(0x800FF258u))*4u));
        w_u32(0x800FF25Cu,(1 - r_u32(0x800FF25Cu)));
        ((void)(v6),(void)(((((short)(r_u32(0x800FF2A0u))) * ((void)(r_u32(0x800FF2A0u)),abort(),0u)) / 2)),abort(),0u);
      }
      w_u32(0x800FF260u,sub_8002F9E4());
      v7 = (r_u32(0x800FF270u) - 1);
      if (r_u32(0x800FF270u))
      {
        (w_u32(0x800FF270u,(r_u32(0x800FF270u)-1u)),(r_u32(0x800FF270u)-1u));
        v8 = 0;
        if (!v7)
        {
          v9 = 0xFFFFFF;
          v10 = 0;
          v11 = 0x800A5B2Cu;
          while ((v10 < r_u32(0x800FF294u)))
          {
            v12 = sub_8006696C((r_u32(0x800FF904u) + 4),v11);
            if ((v12 < v9))
            {
              v9 = v12;
              v8 = v10;
            }
            v11 += (3)*4u;
            ++v10;
          }

          sub_8002FB34((0x800A5B2Cu+((3 * v8))*4u));
          w_u32(0x800FF270u,4);
        }
      }
    }
    if ((r_u32(0x800FF008u) || r_u32(0x800FF300u)))
    {
      result = 1;
      if (!r_u32(0x800FF26Cu))
      {
        w_u32(0x800FF26Cu,1);
        sub_8009B12C(1,0,-1);
        return ((void)(9),(void)(0),(void)(0),abort(),0u);
      }
    }
    else
    {
      result = r_u32(0x800FF26Cu);
      if (r_u32(0x800FF26Cu))
      {
        w_u32(0x800FF26Cu,0);
        sub_8009B12C(0,0,-1);
        return ((void)(480),abort(),0u);
      }
    }
  }
  return result;
}
/* TODO Missing call adapter indirect */
/* TODO Missing call adapter indirect */
uint32 sub_8005F1B0(uint32 a1)
{
  sint32 v2;
  sint32 v3;
  sint32 v4;
  sint32 v6;
  sint32 v7;
  if (((r_u16(((uint32)((a1 + 216)))) & 2) == 0))
    return 0;
  (v2 = r_u32(((uint32)((a1 + 484)))));
  w_u8(((uint32)((a1 + 578))),4);
  if ((v2 && ((v3 = ((r_u32(((uint32)((a1 + 8)))) - r_u32(((uint32)((a1 + 480))))) >> 12)),(v2 < v3))))
  {
    ((void)((a1 + ((sint16)(r_u16(((uint32)((r_u32(((uint32)((a1 + 68)))) + 48)))))))),(void)((((v3 - v2) * r_u16(0x800EC526u)) / (r_u32(((uint32)((a1 + 488)))) - v2))),(void)(0x800A71CCu),(void)(0),abort(),0u);
    (v4 = a1);
    if ((((sint16)(r_u16(((uint32)((a1 + 218)))))) <= 0))
      return 1;
  }
  else
  {
    if (r_u32(0x800FF2FCu))
      sub_8007011C(0,4,0,1);
    (v4 = a1);
  }
  sub_80063038(v4,2,(r_u16(0x800EC4D6u) + 1),-1);
  (v6 = 19);
  if (((r_u16(((uint32)(a1))) & 8) != 0))
    (v6 = 56);
  sub_80069DF0(v6,0x2000,0);
  (v7 = r_u32(((uint32)((a1 + 444)))));
  w_u32(((uint32)((a1 + 460))),8);
  w_u8(((uint32)((v7 + 273))),0);
  w_u8(((uint32)((a1 + 538))),0);
  w_u8(((uint32)((a1 + 537))),0);
  w_u32(0x800FF5A4u,0);
  return 1;
}








void nullsub_25(void)
{
  ;
}
uint32 sub_80030168(uint32 a1)
{
  sint32 position_vector0[3], position_vector1[3];
  sint32 result;
  uint32 v2;
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
  char v21[16];
  char v22[16];
  char v23[16];
  char v24[16];
  sint32 v25;
  sint32 v26;
  sint32 v27;
  sint32 v28;
  sint32 v29;
  sint32 v30;
  result = r_u32(0x800FF5A0u);
  v2 = a1;
  if ((a1 == ((uint32)(r_u32(0x800FF5A0u)))))
  {
    v3 = r_u32((a1+(47)*4u));
    v4 = r_u32((a1+(48)*4u));
    position_vector0[0] = r_u32((a1+(46)*4u));
    position_vector0[1] = v3;
    position_vector0[2] = v4;
    v25 = 2;
    xport_draft_host_sub_8006C564_p13(v21,(a1+(147)*4u),&v25);
    v26 = 2;
    xport_draft_host_sub_8006C564_p123(v23,&position_vector0[0],&v26);
    v27 = 3;
    xport_draft_host_sub_8006C47C_p123(v22,&v27,v23);
    result = xport_draft_host_sub_8006C34C_p123(&position_vector1[0],v21,v22);
    v5 = position_vector1[1];
    v6 = position_vector1[2];
    w_u32((v2+(46)*4u),position_vector1[0]);
    w_u32((v2+(47)*4u),v5);
    w_u32((v2+(48)*4u),v6);
    v7 = position_vector0[1];
    v8 = position_vector0[2];
    w_u32((v2+(147)*4u),position_vector0[0]);
    w_u32((v2+(148)*4u),v7);
    w_u32((v2+(149)*4u),v8);
  }
  while (v2)
  {
    v9 = r_u32((v2+(2)*4u));
    v10 = r_u32((v2+(3)*4u));
    position_vector0[0] = r_u32((v2+(1)*4u));
    position_vector0[1] = v9;
    position_vector0[2] = v10;
    v28 = 2;
    xport_draft_host_sub_8006C564_p13(v21,(v2+(60)*4u),&v28);
    v29 = 2;
    xport_draft_host_sub_8006C564_p123(v24,&position_vector0[0],&v29);
    v30 = 3;
    xport_draft_host_sub_8006C47C_p123(v22,&v30,v24);
    result = xport_draft_host_sub_8006C34C_p123(&position_vector1[0],v21,v22);
    v11 = position_vector1[1];
    v12 = position_vector1[2];
    w_u32((v2+(1)*4u),position_vector1[0]);
    w_u32((v2+(2)*4u),v11);
    w_u32((v2+(3)*4u),v12);
    v13 = position_vector0[1];
    v14 = position_vector0[2];
    w_u32((v2+(60)*4u),position_vector0[0]);
    w_u32((v2+(61)*4u),v13);
    w_u32((v2+(62)*4u),v14);
    v2 = ((uint32)(r_u32((v2+(7)*4u))));
  }

  return result;
}
uint32 sub_800303F4(void)
{
  sint32 position_vector0[3], position_vector1[3];
  uint32 i;
  sint32 v1;
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
  sint32 v17;
  sint32 v18;
  sint32 v19;
  sint32 v20;
  sint32 v21;
  sint32 v22;
  sint32 result;
  sint32 v24;
  sint32 v25;
  char v32[16];
  char v33[16];
  char v34[16];
  char v35[16];
  sint32 v36;
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
  for (i = ((uint32)(r_u32(0x800FF444u))); i; i = ((uint32)(r_u32((i+(1)*4u)))))
  {
    v1 = r_u32((i+(7)*4u));
    v2 = r_u32((i+(8)*4u));
    position_vector0[0] = r_u32((i+(6)*4u));
    position_vector0[1] = v1;
    position_vector0[2] = v2;
    v36 = 2;
    xport_draft_host_sub_8006C564_p13(v32,(i+(3)*4u),&v36);
    v37 = 2;
    xport_draft_host_sub_8006C564_p123(v34,&position_vector0[0],&v37);
    v38 = 3;
    xport_draft_host_sub_8006C47C_p123(v33,&v38,v34);
    xport_draft_host_sub_8006C34C_p123(&position_vector1[0],v32,v33);
    v3 = position_vector1[1];
    v4 = position_vector1[2];
    w_u32((i+(6)*4u),position_vector1[0]);
    w_u32((i+(7)*4u),v3);
    w_u32((i+(8)*4u),v4);
    v5 = position_vector0[1];
    v6 = position_vector0[2];
    w_u32((i+(3)*4u),position_vector0[0]);
    w_u32((i+(4)*4u),v5);
    w_u32((i+(5)*4u),v6);
    v7 = r_u32((i+(19)*4u));
    v8 = r_u32((i+(20)*4u));
    position_vector0[0] = r_u32((i+(18)*4u));
    position_vector0[1] = v7;
    position_vector0[2] = v8;
    v39 = 2;
    xport_draft_host_sub_8006C564_p13(v32,(i+(27)*4u),&v39);
    v40 = 2;
    xport_draft_host_sub_8006C564_p123(v35,&position_vector0[0],&v40);
    v41 = 3;
    xport_draft_host_sub_8006C47C_p123(v33,&v41,v35);
    xport_draft_host_sub_8006C34C_p123(&position_vector1[0],v32,v33);
    v9 = position_vector1[1];
    v10 = position_vector1[2];
    w_u32((i+(18)*4u),position_vector1[0]);
    w_u32((i+(19)*4u),v9);
    w_u32((i+(20)*4u),v10);
    v11 = position_vector0[1];
    v12 = position_vector0[2];
    w_u32((i+(27)*4u),position_vector0[0]);
    w_u32((i+(28)*4u),v11);
    w_u32((i+(29)*4u),v12);
    v13 = r_u32((i+(22)*4u));
    v14 = r_u32((i+(23)*4u));
    position_vector0[0] = r_u32((i+(21)*4u));
    position_vector0[1] = v13;
    position_vector0[2] = v14;
    v42 = 2;
    xport_draft_host_sub_8006C564_p13(v32,(i+(30)*4u),&v42);
    v43 = 2;
    xport_draft_host_sub_8006C564_p123(v34,&position_vector0[0],&v43);
    v44 = 3;
    xport_draft_host_sub_8006C47C_p123(v33,&v44,v34);
    xport_draft_host_sub_8006C34C_p123(&position_vector1[0],v32,v33);
    v15 = position_vector1[1];
    v16 = position_vector1[2];
    w_u32((i+(21)*4u),position_vector1[0]);
    w_u32((i+(22)*4u),v15);
    w_u32((i+(23)*4u),v16);
    v17 = position_vector0[1];
    v18 = position_vector0[2];
    w_u32((i+(30)*4u),position_vector0[0]);
    w_u32((i+(31)*4u),v17);
    w_u32((i+(32)*4u),v18);
    v19 = r_u32((i+(25)*4u));
    v20 = r_u32((i+(26)*4u));
    position_vector0[0] = r_u32((i+(24)*4u));
    position_vector0[1] = v19;
    position_vector0[2] = v20;
    v45 = 2;
    xport_draft_host_sub_8006C564_p13(v32,(i+(33)*4u),&v45);
    v46 = 2;
    xport_draft_host_sub_8006C564_p123(v34,&position_vector0[0],&v46);
    v47 = 3;
    xport_draft_host_sub_8006C47C_p123(v33,&v47,v34);
    xport_draft_host_sub_8006C34C_p123(&position_vector1[0],v32,v33);
    v21 = position_vector1[1];
    v22 = position_vector1[2];
    w_u32((i+(24)*4u),position_vector1[0]);
    w_u32((i+(25)*4u),v21);
    w_u32((i+(26)*4u),v22);
    result = position_vector0[0];
    v24 = position_vector0[1];
    v25 = position_vector0[2];
    w_u32((i+(33)*4u),position_vector0[0]);
    w_u32((i+(34)*4u),v24);
    w_u32((i+(35)*4u),v25);
  }

  return result;
}
uint32 sub_80034FC4(uint32 a1)
{
  sint32 result;
  short v2;
  result = r_u32(((uint32)(a1)));
  v2 = r_u16(((uint32)((a1 + 4))));
  w_u32(0x800FF41Cu,r_u32(((uint32)(a1))));
  w_u16(0x800FF420u,v2);
  return result;
}
void sub_80035110(uint32 a1, uint32 a2, uint32 a3)
{
  w_u8(0x800FF430u,a1);
  w_u8(0x800FF431u,a2);
  w_u8(0x800FF432u,a3);
}
uint32 sub_80035124(uint32 a1, uint32 a2, uint32 a3, uint32 a4, uint32 a5)
{
  sint8 v13;
  sint32 v14;
  sint32 v15;
  sint32 v16;
  sint32 v17;
  short v18;
  sint32 v19;
  sub_80034EF4(((uint32)(a1)));
  v13 = r_u32(0x800FF424u);
  w_u32(((uint32)((a1 + 68))),0x800A1C90u);
  v14 = (v13 & 0xF);
  if ((v14 == 1))
  {
    w_u8(((uint32)((a1 + 79))),104);
    v15 = 0x2000000;
  }
  else
  {
    w_u8(((uint32)((a1 + 79))),96);
    v14 = r_u32(0x800FF424u);
    v15 = 50331648;
  }
  w_u32(((uint32)((a1 + 72))),v15);
  w_u32(((uint32)((a1 + 80))),v14);
  w_u8(((uint32)((a1 + 76))),r_u8(0x800FF42Cu));
  w_u8(((uint32)((a1 + 77))),r_u8(0x800FF42Du));
  w_u8(((uint32)((a1 + 78))),r_u8(0x800FF42Eu));
  w_u8(((uint32)((a1 + 84))),r_u8(0x800FF430u));
  w_u8(((uint32)((a1 + 85))),r_u8(0x800FF431u));
  w_u8(((uint32)((a1 + 86))),r_u8(0x800FF432u));
  v16 = r_u32((a2+(1)*4u));
  v17 = r_u32((a2+(2)*4u));
  w_u32(((uint32)((a1 + 24))),r_u32(a2));
  w_u32(((uint32)((a1 + 28))),v16);
  w_u32(((uint32)((a1 + 32))),v17);
  sub_80034FEC((a1 + 36),a3);
  w_u32(((uint32)((a1 + 52))),a4);
  w_u8(((uint32)((a1 + 60))),3);
  w_u8(((uint32)((a1 + 61))),3);
  w_u8(((uint32)((a1 + 62))),3);
  v18 = sub_80066570(a5);
  v19 = (r_u32(0x800FF428u) == 0);
  w_u16(((uint32)((a1 + 10))),v18);
  if (!v19)
    w_u8(((uint32)((a1 + 79))),(r_u8(((uint32)((a1 + 79))))|(2u)));
  return a1;
}
/* TODO Missing call adapter SHIWORD */
uint32 sub_80034FEC(uint32 a1, uint32 a2)
{
  sint32 v5;
  short v6;
  v5 = 0x800FF414u;
  v6 = r_u16(0x800FF418u);
  if (((uint16)(r_u32(0x800FF41Cu))))
    v5 = ((v5&0xFFFF0000u)|(((((v5 + sub_80066570(((short)(r_u32(0x800FF41Cu))))) - (((short)(r_u32(0x800FF41Cu))) >> 1)))&0xFFFFu)<<0));
  if (((r_u32(0x800FF41Cu)>>16)&65535u))
    v5 = ((v5&0x0000FFFFu)|((((sub_80066570(((void)(r_u32(0x800FF41Cu)),abort(),0u)) - (((void)(r_u32(0x800FF41Cu)),abort(),0u) >> 1)))&0xFFFFu)<<16));
  if (r_u16(0x800FF420u))
    v6 += (sub_80066570(r_u16(0x800FF420u)) - (r_u16(0x800FF420u) >> 1));
  return sub_800667CC(a1,a2,&v5);
}
uint32 sub_8004B8C8(uint32 a1)
{
  sint32 result;
  short v3;
  result = sub_80062F48(a1);
  if (!result)
  {
    sub_8004B948(a1);
    result = (((unsigned short)(r_u16((a1+(39)*2u)))) | 0x40);
    v3 = (r_u16(a1) | 1);
    w_u16((a1+(39)*2u),result);
    w_u16(a1,v3);
  }
  return result;
}
/* TODO Missing call adapter indirect */
uint32 sub_8004D6B4(uint32 a1, uint32 a2)
{
  sint32 v4;
  sint32 v5;
  sint32 v6;
  sint32 v7;
  sint32 v8;
  sint32 v9;
  sint32 result;
  w_u32(((uint32)((a1 + 68))),0x800A2AC0u);
  sub_80062A64(a1,0x800FF4E8u);
  v4 = r_u32(((uint32)((a1 + 512))));
  if (v4)
    sub_8006A294(v4);
  v5 = r_u32(((uint32)((a1 + 496))));
  if (v5)
    ((void)((v5 + ((sint16)(r_u16(((uint32)((r_u32(((uint32)(v5))) + 8)))))))),(void)(3),abort(),0u);
  v6 = r_u32(((uint32)((a1 + 508))));
  v7 = 3;
  if (v6)
    ((void)((v6 + ((sint16)(r_u16(((uint32)((r_u32(((uint32)((v6 + 68)))) + 8)))))))),(void)(3),abort(),0u);
  v8 = 0;
  if (r_u32(((uint32)((a1 + 504)))))
  {
    while ((v8 < r_u16(((uint32)((a1 + 500))))))
    {
      v9 = r_u32(((uint32)(((r_u32(((uint32)((a1 + 504)))) + (16 * v8)) + 12))));
      v7 = 3;
      if (v9)
        ((void)((v9 + ((sint16)(r_u16(((uint32)((r_u32(((uint32)((v9 + 68)))) + 8)))))))),(void)(3),abort(),0u);
      ++v8;
    }

    sub_8006BC20(r_u32(((uint32)((a1 + 504)))));
  }
  sub_8004B868(a1,0);
  result = (a2 & 1);
  if (((a2 & 1) != 0))
    return sub_80062608(a1);
  return result;
}
uint32 sub_8004B868(uint32 a1, uint32 a2)
{
  sint32 v4;
  sint32 result;
  v4 = r_u32(0x800FF4D8u);
  w_u32(((uint32)((a1 + 68))),0x800A2B18u);
  w_u32(0x800FF4D8u,(v4 - 1));
  result = sub_80062FB8(a1,0);
  if (((a2 & 1) != 0))
    return sub_80062608(a1);
  return result;
}
uint32 sub_80062650(uint32 a1, uint32 a2)
{
  sint32 v4;
  sint32 result;
  v4 = (r_u16(((uint32)(a1))) & 0x400);
  w_u32(((uint32)((a1 + 68))),0x800A33C0u);
  if (v4)
    sub_8001BB14(((sint32)((0x800F2610u+((6 * r_u8(((uint32)((a1 + 25))))))*4u))));
  result = (a2 & 1);
  if (((a2 & 1) != 0))
    return sub_80062608(a1);
  return result;
}
uint32 sub_8005EC7C(uint32 a1)
{
  sint32 result;
  sint32 v3;
  short v4;
  sint32 v5;
  result = 0;
  if (r_u32(((uint32)((a1 + 520)))))
  {
    if ((r_u8(((uint32)((a1 + 471)))) | r_u8(((uint32)((a1 + 472))))))
    {
      w_u32(((uint32)((a1 + 112))),0);
      w_u32(((uint32)((a1 + 108))),0);
      w_u32(((uint32)((a1 + 104))),0);
      w_u32(((uint32)((a1 + 124))),0);
      w_u32(((uint32)((a1 + 120))),0);
      v3 = r_u32(((uint32)((a1 + 520))));
      w_u32(((uint32)((a1 + 116))),0);
      v4 = r_u16(((uint32)((v3 + 20))));
      w_u32(((uint32)((a1 + 16))),r_u32(((uint32)((v3 + 16)))));
      w_u16(((uint32)((a1 + 20))),v4);
      sub_80063038(a1,18,0,-1);
      v5 = r_u32(((uint32)((a1 + 520))));
      w_u32(((uint32)((a1 + 460))),0x20000);
      sub_80062034(v5);
      return 1;
    }
    else
    {
      return 0;
    }
  }
  return result;
}
/* TODO Missing call adapter indirect */
void sub_80022ABC(uint32 a1)
{
  sint32 v2;
  sint32 result;
  v2 = r_u32(((uint32)((a1 + 32))));
  if (v2)
    result = ((void)((v2 + ((sint16)(r_u16(((uint32)((r_u32(((uint32)((v2 + 68)))) + 8)))))))),(void)(3),abort(),0u);
  w_u32(((uint32)((a1 + 32))),0);
  return;
}
uint32 sub_80034A18(uint32 a1, uint32 a2, uint32 a3, uint32 a4)
{
  sint32 result;
  result = 838860800;
  w_u32(((uint32)((a1 + 88))),((((a4 << 16) | (a3 << 8)) | 0x32000000) | a2));
  return result;
}
uint32 sub_8001D114(uint32 a1)
{
  uint32 v2;
  sint32 v3;
  sint32 v4;
  sint32 v5;
  sint32 result;
  v2 = 0;
  if (r_u32(((uint32)((a1 + 80)))))
  {
    v3 = 0;
    do
    {
      if (((v2 & 1) != 0))
      {
        v4 = sub_80066570(((sint16)(r_u16(((uint32)((a1 + 112)))))));
        v5 = ((sint16)(r_u16(((uint32)((a1 + 110))))));
      }
      else
      {
        v4 = sub_80066570(((sint16)(r_u16(((uint32)((a1 + 116)))))));
        v5 = ((sint16)(r_u16(((uint32)((a1 + 114))))));
      }
      w_u32(((uint32)((v3 + r_u32(((uint32)((a1 + 72))))))),(v5 + v4));
      ++v2;
      v3 += 8;
    }
    while ((v2 < r_u32(((uint32)((a1 + 80))))));
  }
  if ((((sint16)(r_u16(((uint32)((a1 + 110)))))) >= ((sint16)(r_u16(((uint32)((a1 + 118))))))))
    w_u16(((uint32)((a1 + 110))),(r_u16(((uint32)((a1 + 110))))-(r_u16(((uint32)((a1 + 118)))))));
  if ((((sint16)(r_u16(((uint32)((a1 + 112)))))) >= ((sint16)(r_u16(((uint32)((a1 + 118))))))))
    w_u16(((uint32)((a1 + 112))),(r_u16(((uint32)((a1 + 112))))-(r_u16(((uint32)((a1 + 118)))))));
  if ((((sint16)(r_u16(((uint32)((a1 + 114)))))) >= ((sint16)(r_u16(((uint32)((a1 + 118))))))))
    w_u16(((uint32)((a1 + 114))),(r_u16(((uint32)((a1 + 114))))-(r_u16(((uint32)((a1 + 118)))))));
  if ((((sint16)(r_u16(((uint32)((a1 + 116)))))) >= ((sint16)(r_u16(((uint32)((a1 + 118))))))))
    w_u16(((uint32)((a1 + 116))),(r_u16(((uint32)((a1 + 116))))-(r_u16(((uint32)((a1 + 118)))))));
  result = sub_80066570(1024);
  w_u16(((uint32)((a1 + 96))),result);
  return result;
}
uint32 sub_8002374C(uint32 a1, uint32 a2, uint32 a3)
{
  sint32 position_vector0[3], position_vector1[3];
  sint16 angles[3];
  sint32 v6;
  sint32 v7;
  sint32 v8;
  sint32 v9;
  uint32 v10;
  sint32 v11;
  sint32 v12;
  sint32 v13;
  uint32 result;
  char v15[16];
  sub_80032068(a1,1);
  w_u32((a1+(17)*4u),0x800A1518u);
  w_u8(((uint32)((r_u32((a1+(20)*4u)) + 12))),64);
  w_u8(((uint32)((r_u32((a1+(20)*4u)) + 13))),64);
  w_u8(((uint32)((r_u32((a1+(20)*4u)) + 14))),30);
  sub_800667CC((a1+(24)*4u),3,a3);
  xport_draft_host_sub_800667CC_p1(v15,-45,a3);
  xport_draft_host_sub_8006C34C_p13(position_vector0,a2,v15);
  v6 = position_vector0[1];
  v7 = position_vector0[2];
  w_u32((a1+(6)*4u),position_vector0[0]);
  w_u32((a1+(7)*4u),v6);
  w_u32((a1+(8)*4u),v7);
  xport_draft_host_sub_8006C34C_p1(position_vector0,(a1+(6)*4u),(a1+(24)*4u));
  v8 = position_vector0[1];
  v9 = position_vector0[2];
  w_u32((a1+(21)*4u),position_vector0[0]);
  w_u32((a1+(22)*4u),v8);
  w_u32((a1+(23)*4u),v9);
  v10 = ((uint32)(r_u32((a1+(20)*4u))));
  xport_draft_host_sub_8006C3AC_p1(position_vector0,(a1+(6)*4u),(a1+(24)*4u));
  v11 = position_vector0[1];
  v12 = position_vector0[2];
  w_u32(v10,position_vector0[0]);
  w_u32((v10+(1)*4u),v11);
  w_u32((v10+(2)*4u),v12);
  angles[0] = (sint16)r_u16(a3);
  angles[1] = (sint16)(r_u16(a3+2)+1024u);
  angles[2] = (sint16)r_u16(a3+4);
  v13 = sub_80066570(4);
  xport_draft_host_sub_800667CC_p3(a1+36,v13+10,angles);
  w_u32((a1+(10)*4u),(r_u32((a1+(10)*4u))-(40960)));
  xport_draft_host_sub_800667CC_p13(position_vector1,145,angles);
  xport_draft_host_sub_8006C0B8_p1(position_vector1,a2);
  w_u32(0x800ED638u,position_vector1[0]);
  w_u32(0x800ED63Cu,position_vector1[1]);
  w_u32(0x800ED640u,position_vector1[2]);
  w_u32(0x800ED644u,position_vector1[0]);
  w_u32(0x800ED64Cu,position_vector1[2]);
  w_u32(0x800ED648u,(position_vector1[1] + 4096000));
  sub_8007BB24(0x800ED638u);
  sub_8007DD04(0x800ED638u,1);
  if (r_u32(0x800ED6A0u))
    w_u32((a1+(27)*4u),r_u32(0x800ED6A8u));
  else
    w_u32((a1+(27)*4u),0x7FFFFFFF);
  result = a1;
  w_u32((a1+(28)*4u),r_u32(0x800FF650u));
  return result;
}
/* TODO Missing call adapter xport_draft_missing_gte_adapter */
void sub_800855B4(uint32 a1, uint32 a2)
{
  /* TODO Recover GTE register carriers and missing host GTE adapters */
  uint32 gte_T0 = 0u;
  uint32 gte_T1 = 0u;
  uint32 gte_T2 = 0u;
  uint32 gte_T3 = 0u;
  uint32 gte_T4 = 0u;
  uint32 gte_T5 = 0u;
  uint32 gte_T6 = 0u;
  uint32 gte_T7 = 0u;
  uint32 gte_T8 = 0u;
  uint32 gte_T9 = 0u;
  
  gte_T0 = ((sint32)(r_u32(a1)));
  gte_T1 = ((sint32)(r_u32((a1+(1)*4u))));
  gte_T3 = ((sint32)(r_u32((a1+(3)*4u))));
  (abort(),0u);
  gte_T9 = ((gte_T1>>16)&65535u);
  (abort(),0u);
  gte_T0 = ((((sint32)(r_u32(a1)))>>16)&65535u);
  gte_T3 >>= 16;
  (abort(),0u);
  gte_T2 = ((sint32)(r_u32((a1+(2)*4u))));
  (abort(),0u);
  gte_T2 >>= 16;
  (abort(),0u);
  gte_T4 = ((sint32)(r_u32((a1+(4)*4u))));
  (abort(),0u);
  w_u32(a2,(((unsigned short)(gte_T5)) | (gte_T8 << 16)));
  w_u32((a2+(3)*4u),(((unsigned short)(gte_T7)) | (gte_T0 << 16)));
  (abort(),0u);
  w_u32((a2+(1)*4u),(((unsigned short)(gte_T1)) | (gte_T6 << 16)));
  w_u32((a2+(2)*4u),(((unsigned short)(gte_T9)) | (gte_T2 << 16)));
  w_u32((a2+(4)*4u),gte_T3);
}
uint32 sub_8006C190(uint32 a1, uint32 a2)
{
  uint32 result;
  result = a1;
  w_u32(a1,(r_u32(a1)/(((sint32)(r_u32(a2))))));
  w_u32((a1+(1)*4u),(r_u32((a1+(1)*4u))/(((sint32)(r_u32(a2))))));
  w_u32((a1+(2)*4u),(r_u32((a1+(2)*4u))/(((sint32)(r_u32(a2))))));
  return result;
}
uint32 sub_80035478(uint32 a1, uint32 a2, uint32 a3, uint32 a4, uint32 a5, uint32 a6, uint32 a7)
{
  sint32 v15;
  sint32 v16;
  sub_80034D88(a1);
  w_u32(((uint32)((a1 + 68))),0x800A1C78u);
  v15 = r_u32((a2+(1)*4u));
  v16 = r_u32((a2+(2)*4u));
  w_u32(((uint32)((a1 + 24))),r_u32(a2));
  w_u32(((uint32)((a1 + 28))),v15);
  w_u32(((uint32)((a1 + 32))),v16);
  sub_80033398(a1,a3);
  w_u16(((uint32)((a1 + 94))),a4);
  if (a5)
    sub_800332A4(a1);
  w_u32(((uint32)((a1 + 112))),a6);
  if ((a7 == 0xFFFFFFFFu))
    w_u32(((uint32)((a1 + 116))),(r_u8(((uint32)((a1 + 89)))) - 1));
  else
    w_u32(((uint32)((a1 + 116))),a7);
  return a1;
}
uint32 sub_8003319C(uint32 a1)
{
  sint32 result;
  sub_80032F7C(a1);
  result = a1;
  w_u32(((uint32)((a1 + 68))),0x800A1D68u);
  w_u16(((uint32)((a1 + 92))),128);
  w_u16(((uint32)((a1 + 94))),400);
  w_u32(((uint32)((a1 + 76))),746619008);
  return result;
}
uint32 sub_800332FC(uint32 a1, uint32 a2, uint32 a3, uint32 a4)
{
  uint32 result;
  result = ((((r_u32(((uint32)((a1 + 76)))) & 0xFF000000) | (a4 << 16)) | (a3 << 8)) | a2);
  w_u32(((uint32)((a1 + 76))),result);
  return result;
}
uint32 sub_80035A54(uint32 a1)
{
  sint32 v2;
  sint32 v3;
  sint8 v4;
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
  sint32 result;
  v2 = r_u32(((uint32)((a1 + 28))));
  v3 = r_u32(((uint32)((a1 + 40))));
  v4 = r_u8(((uint32)((a1 + 61))));
  w_u32(((uint32)((a1 + 24))),(r_u32(((uint32)((a1 + 24))))+(r_u32(((uint32)((a1 + 36)))))));
  v5 = r_u32(((uint32)((a1 + 32))));
  v6 = r_u32(((uint32)((a1 + 44))));
  w_u32(((uint32)((a1 + 28))),(v2 + v3));
  v7 = r_u32(((uint32)((a1 + 36))));
  v8 = r_u32(((uint32)((a1 + 48))));
  w_u32(((uint32)((a1 + 32))),(v5 + v6));
  v9 = r_u32(((uint32)((a1 + 40))));
  v10 = r_u32(((uint32)((a1 + 52))));
  w_u32(((uint32)((a1 + 36))),(v7 + v8));
  v11 = (v9 + v10);
  w_u32(((uint32)((a1 + 44))),(r_u32(((uint32)((a1 + 44))))+(r_u32(((uint32)((a1 + 56)))))));
  v7 = ((v7&0xFFFFFF00u)|(((r_u8(((uint32)((a1 + 60)))))&0xFFu)<<0));
  v10 = ((v10&0xFFFFFF00u)|(((r_u8(((uint32)((a1 + 90)))))&0xFFu)<<0));
  w_u32(((uint32)((a1 + 40))),v11);
  v12 = r_u32(((uint32)((a1 + 40))));
  v10 = ((v10&0xFFFFFF00u)|((((v10 + 1))&0xFFu)<<0));
  v13 = (r_u32(((uint32)((a1 + 36)))) - (((sint32)(r_u32(((uint32)((a1 + 36)))))) >> v7));
  v7 = ((v7&0xFFFFFF00u)|(((r_u8(((uint32)((a1 + 62)))))&0xFFu)<<0));
  w_u32(((uint32)((a1 + 36))),v13);
  v14 = r_u32(((uint32)((a1 + 44))));
  w_u8(((uint32)((a1 + 90))),v10);
  w_u32(((uint32)((a1 + 44))),(v14 - (v14 >> v7)));
  v15 = r_u8(((uint32)((a1 + 89))));
  w_u32(((uint32)((a1 + 40))),(v12 - (v12 >> v4)));
  if ((((sint8)(v10)) >= v15))
    w_u8(((uint32)((a1 + 90))),(v15 - 1));
  v16 = r_u16(((uint32)((a1 + 10))));
  v17 = (r_u16(((uint32)((a1 + 8)))) + 1);
  w_u16(((uint32)((a1 + 8))),v17);
  result = (v17 < v16);
  if (!result)
    return sub_80032ED8(a1);
  return result;
}
/* TODO Missing call adapter xport_draft_missing_gte_adapter */
void sub_80082254(uint32 geometry)
{
  uint32 x = r_u32(geometry+0x1A4u);
  uint32 y = r_u32(geometry+0x1B4u);
  uint32 z = r_u32(geometry+0x1C4u);
  xport_draft_gte_data_write(9,x);
  xport_draft_gte_data_write(10,y);
  xport_draft_gte_data_write(11,z);
}
/* TODO 64-bit guest field width remains TODO */
/* TODO Missing call adapter SHIDWORD */
/* TODO Missing call adapter xport_draft_missing_gte_adapter */
/* TODO 64-bit guest field width remains TODO */
/* TODO Missing call adapter xport_draft_missing_gte_adapter */
uint32 sub_800850A4(uint32 a1)
{
  long long _KR00_8;
  /* TODO Recover GTE register carriers and missing host GTE adapters */
  uint32 gte_A2 = 0u;
  uint32 gte_A3 = 0u;
  uint32 gte_T0 = 0u;
  uint32 gte_T1 = 0u;
  uint32 gte_T2 = 0u;
  uint32 gte_T3 = 0u;
  uint32 gte_T4 = 0u;
  uint32 gte_T5 = 0u;
  uint32 gte_T6 = 0u;
  uint32 gte_T7 = 0u;
  uint32 gte_T8 = 0u;
  uint32 gte_V0 = 0u;
  uint32 gte_V1 = 0u;
  
  sint32 v1;
  sint32 v2;
  sint32 v3;
  uint32 v18;
  sint32 v19;
  sint32 v20;
  uint32 v21;
  sint32 v38;
  sint32 v41;
  uint32 v42;
  sint32 v45;
  uint32 v46;
  sint32 v65;
  sint32 result;
  (v1 = r_u32(((uint32)((r_u32(0x800FFB0Cu) + 4)))));
  (v2 = r_u32(((uint32)((r_u32(0x800FFB0Cu) + 8)))));
  (v3 = r_u32(((uint32)((r_u32(0x800FFB0Cu) + 12)))));
  (gte_T2 = ((((sint32)(r_u32((a1+(2)*4u)))) >> 12) - v3));
  (gte_T0 = (((unsigned short)(((((sint32)(r_u32(a1))) >> 12) - v1))) | (((((sint32)(r_u32((a1+(1)*4u)))) >> 12) - v2) << 16)));
  (abort(),0u);
  (gte_T2 = ((((sint32)(r_u32((a1+(5)*4u)))) >> 12) - v3));
  (gte_T0 = (((unsigned short)(((((sint32)(r_u32((a1+(3)*4u)))) >> 12) - v1))) | (((((sint32)(r_u32((a1+(4)*4u)))) >> 12) - v2) << 16)));
  (abort(),0u);
  w_u32((a1+(6)*4u),gte_A2);
  w_u32((a1+(10)*4u),gte_T5);
  w_u32((a1+(8)*4u),0);
  w_u32((a1+(9)*4u),4096);
  (abort(),0u);
  (v18 = (gte_V0 | gte_V1));
  (v20 = ((((((gte_V0 | gte_V1) >= 0) && ((v18 = ((short)((gte_A3 - gte_A2)))),(v19 = (v18 + 1023)),(((sint32)((v18 - 1024))) < 0))) && ((v18 = (gte_A2 >> 16)),(v19 >= 0))) && ((v18 = ((gte_A3 >> 16) - v18)),(((sint32)((v18 - 512))) < 0))) && (((sint32)((v18 + 511))) >= 0)));
  (v21 = (v18 >> 22));
  if (v20)
  {
    w_u32((a1+(7)*4u),gte_A3);
    w_u32((a1+(11)*4u),gte_T8);
    (result = (((uint32)(gte_T5)) >> 2));
    if ((gte_T5 >= gte_T8))
      (result = (((uint32)(gte_T8)) >> 2));
  }
  else
  {
    if (((v21 & 7) != 0))
      return -1;
    if (((gte_T5 & gte_T8) < 0))
      return -1;
    (gte_T3 = (((unsigned short)(gte_T3)) | (gte_T4 << 16)));
    (abort(),0u);
    (gte_T6 = (((unsigned short)(gte_T6)) | (gte_T7 << 16)));
    (abort(),0u);
    (gte_T0 = 0x800F25D0u);
    (gte_T4 = r_u32(0x800F25E0u));
    (gte_T5 = ((unsigned short)(r_u16(0x800F25E4u))));
    (gte_T6 = (r_u32(0x800F25D4u) >> 16));
    (abort(),0u);
    (gte_T0 = ((((uint32)(((unsigned long long)r_u32(0x800F25D8u)|((unsigned long long)r_u32((0x800F25D8u)+4u)<<32)))) << 16) | ((unsigned short)(r_u32(0x800F25D4u)))));
    (abort(),0u);
    (_KR00_8 = (((long long)(((sint32)(((unsigned long long)(((unsigned long long)r_u32(0x800F25D8u)|((unsigned long long)r_u32((0x800F25D8u)+4u)<<32)))>>32)&0xFFFFFFFFu)))) << 16));
    (gte_T0 = (((unsigned long long)r_u32(0x800F25D8u)|((unsigned long long)r_u32((0x800F25D8u)+4u)<<32)) >> 16));
    (abort(),0u);
    (gte_T0 = r_u32(0x800F25E8u));
    (gte_T4 = r_u32(0x800F2608u));
    (gte_T5 = ((unsigned short)(r_u16(0x800F260Cu))));
    (abort(),0u);
    (gte_T0 = ((((uint32)(((unsigned long long)r_u32(0x800F2600u)|((unsigned long long)r_u32((0x800F2600u)+4u)<<32)))) << 16) | ((unsigned short)(r_u16(0x800F25ECu)))));
    (abort(),0u);
    (gte_T0 = (((unsigned long long)r_u32(0x800F2600u)|((unsigned long long)r_u32((0x800F2600u)+4u)<<32)) >> 16));
    (abort(),0u);
    (v38 = (((gte_V0 >> 19) & 0x38) | ((gte_T0 >> 22) & 7)));
    (abort(),0u);
    (v41 = (((gte_V1 >> 19) & 0x38) | ((gte_T0 >> 22) & 7)));
    if ((((v38 & v41) != 0) || (((((unsigned char)(v38)) | ((unsigned char)(v41))) & 0x20) != 0)))
      return -1;
    (v42 = 32);
    (gte_A2 = 0);
    (gte_A3 = 4096);
    do
    {
      if ((((v38 ^ v41) & v42) != 0))
      {
        (v45 = ((((sint32)(r_u32((a1+(12)*4u)))) << 12) / (((sint32)(r_u32((a1+(12)*4u)))) - ((sint32)(r_u32((a1+(18)*4u)))))));
        if (((v38 & v42) != 0))
        {
          if (((v45 - gte_A2) > 0))
            (gte_A2 = ((((sint32)(r_u32((a1+(12)*4u)))) << 12) / (((sint32)(r_u32((a1+(12)*4u)))) - ((sint32)(r_u32((a1+(18)*4u)))))));
        }
        else
          if (((v45 - gte_A3) < 0))
        {
          (gte_A3 = ((((sint32)(r_u32((a1+(12)*4u)))) << 12) / (((sint32)(r_u32((a1+(12)*4u)))) - ((sint32)(r_u32((a1+(18)*4u)))))));
        }
      }
      (v42 >>= 1);
      (a1+=4u);
    }
    while (v42);
    (v46 = (a1-(6)*4u));
    if (((gte_A2 - gte_A3) >= 0))
      return -1;
    w_u32((v46+(8)*4u),gte_A2);
    w_u32((v46+(9)*4u),gte_A3);
    (abort(),0u);
    (gte_T0 = ((8 * gte_T0) & 0xFFF8FFF8));
    (gte_T1 *= 8);
    (gte_T2 = ((8 * gte_T2) & 0xFFF8FFF8));
    (gte_T3 *= 8);
    (abort(),0u);
    (gte_T2 = (gte_T0 ^ ((unsigned short)((gte_T0 ^ gte_T2)))));
    (abort(),0u);
    (gte_T2 = (4096 - gte_A2));
    (abort(),0u);
    (gte_T3 = (4096 - gte_A3));
    (abort(),0u);
    (gte_V0 >>= 3);
    (abort(),0u);
    w_u32((v46+(6)*4u),gte_T0);
    w_u32((v46+(10)*4u),gte_V0);
    (abort(),0u);
    w_u32((v46+(7)*4u),gte_T1);
    (v65 = (gte_V1 >> 3));
    w_u32((v46+(11)*4u),v65);
    (abort(),0u);
    (v20 = (((sint32)(gte_V0)) < v65));
    (result = (gte_V0 >> 2));
    if (!v20)
      (result = (((uint32)(v65)) >> 2));
  }
  if (((result - 4096) >= 0))
    return 4095;
  return result;
}


uint32 sub_800348A8(uint32 a1, uint32 a2)
{
  sint8 v3;
  sint32 v4;
  uint32 result;
  sint32 v6;
  v3 = a2;
  v4 = r_u32((a1+(18)*4u));
  w_u32((a1+(17)*4u),0x800A1D08u);
  sub_8006BC20(v4);
  sub_80032E7C(a1,0x800FF450u);
  result = sub_80032FB8(((sint32)(a1)),0);
  if (((v3 & 1) != 0))
    return ((uint32)(sub_80032E30(((sint32)(a1)))));
  return result;
}
uint32 sub_800331EC(uint32 a1, uint32 a2)
{
  uint32 result;
  sint32 v5;
  w_u32(((uint32)((a1 + 68))),0x800A1D68u);
  result = sub_80032FB8(a1,0);
  if (((a2 & 1) != 0))
    return ((uint32)(sub_80032E30(a1)));
  return result;
}
/* TODO Missing call adapter sub_800878DC */
/* TODO Missing call adapter sub_800878DC */
uint32 sub_80021CA8(uint32 a1, uint32 a2, uint32 a3, uint32 a4, uint32 a5, uint32 a6, uint32 a7)
{
  uint32 v11;
  sint32 v15;
  sint32 v16;
  sint32 v17;
  short v18;
  sint32 result;
  short v20;
  short v21;
  sint32 v22;
  short v23;
  short v24;
  sint32 v25;
  sint32 v26;
  sint32 v27;
  sint32 v28;
  short v29;
  sint32 v30;
  sint32 v31;
  sint32 v32;
  unsigned char v33;
  sint32 v34;
  sint32 v35;
  sint32 v36;
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
  uint32 v57;
  (v15 = r_u32(((uint32)(((4 * r_u16(((uint32)((a1 + 22))))) + r_u32((0x800EAEF8u+(((16 * r_u8(((uint32)((a1 + 27))))) + 4))*4u)))))));
  (v16 = (v15 + 32));
  (v17 = ((v15 + 32) + (8 * r_u32(((uint32)((v15 + 4)))))));
  if (a5)
  {
    (v18 = ((sint16)(r_u16((a2+(9)*2u)))));
    if ((((v18 & 8) == 0) || ((v18 & 1) != 0)))
      return 0;
    if (a6)
    {
      (v20 = ((sint16)(r_u16(a2))));
      w_u16((a2+(9)*2u),(v18 | 1));
      w_u16(a2,(v20 & 0xFE7F));
      if (a7)
        goto LABEL_11;
      (v21 = (v20 & 0xFE3F));
      if ((((v20 & 0x40) != 0) || ((v21 = (v20 & 0xFE3F)),((v20 & 1) == 0))))
        w_u16(a2,v21);
    }
  }
  (result = 1);
  if (!a7)
    return result;
  LABEL_11:
  if (((((sint16)(r_u16(a2))) & 1) != 0))
  {
    (v11 = ((uint32)((a2+(10)*2u))));
    if (((((sint16)(r_u16(a2))) & 2) == 0))
      (v11 = ((uint32)(r_u32((((uint32)(a2))+(5)*4u)))));
    (v22 = r_u16((((uint32)(v11))+(3)*2u)));
    w_u32(0x800FFB78u,r_u16((((uint32)(v11))+(1)*2u)));
    w_u32(0x800FFB7Cu,v22);
  }

  w_u32(0x800FF210u,(v17 + (((sint16)(r_u16((a2+(8)*2u)))) & 0xFFF8)));
  sub_800858FC((a1 + 16),&v38);
  ((void)(&v38),abort(),0u);
  sub_80084504((a1 + 4));
  sub_80021ADC(((sint32)(&v41)),v16,((sint32)((a2+(2)*2u))),0);
  sub_80021ADC(((sint32)(&v44)),v16,((sint32)((a2+(2)*2u))),1);
  sub_80021ADC(((sint32)(&v47)),v16,((sint32)((a2+(2)*2u))),2);
  (v23 = ((sint16)(r_u16(a2))));
  (v56 = 1);
  if (((v23 & 0x10) == 0))
  {
    (v57 = &v50);
    sub_80021ADC(((sint32)(&v50)),v16,((sint32)((a2+(2)*2u))),3);
    sub_80021B3C((((sint16)(r_u16(a2))) & 0x800),r_u32((((uint32)(a2))+(3)*4u)),4,r_u32((0x800EAEF8u+(((16 * r_u8(((uint32)((a1 + 27))))) + 8))*4u)));
    w_u32(0x800FFD58u,((((((v41 >> 12) + (v44 >> 12)) + (v47 >> 12)) + (v50 >> 12)) >> 2) << 12));
    w_u32(0x800FFD5Cu,((((((v42 >> 12) + (v45 >> 12)) + (v48 >> 12)) + (v51 >> 12)) >> 2) << 12));
    w_u32(0x800FFD60u,((((((v43 >> 12) + (v46 >> 12)) + (v49 >> 12)) + (v52 >> 12)) >> 2) << 12));
    (v29 = ((sint16)(r_u16(a2))));
    if ((((((sint16)(r_u16(a2))) & 0x40) == 0) && ((v29 & 1) != 0)))
    {
      (v32 = 0);
      if (((v29 & 0x20) != 0))
        (v32 = r_u32((((uint32)(v11))+(3)*4u)));
      if (!r_u32(0x800FF738u))
      {
        {
          sint32 vertex0[3] = {v41,v42,v43};
          sint32 vertex1[3] = {v44,v45,v46};
          sint32 vertex2[3] = {v47,v48,v49};
          xport_draft_host_sub_80021358_p123(vertex0,vertex1,vertex2,r_u8(v11),r_u8((v11+(1)*1u)),r_u8((v11+(4)*1u)),r_u8((v11+(5)*1u)),r_u8((v11+(8)*1u)),r_u8((v11+(9)*1u)),v32,a3);
        }
        {
          sint32 vertex0[3] = {v44,v45,v46};
          sint32 vertex1[3] = {v47,v48,v49};
          sint32 vertex2[3] = {v50,v51,v52};
          xport_draft_host_sub_80021358_p123(vertex0,vertex1,vertex2,r_u8((v11+(4)*1u)),r_u8((v11+(5)*1u)),r_u8((v11+(8)*1u)),r_u8((v11+(9)*1u)),r_u8((v11+(10)*1u)),r_u8((v11+(11)*1u)),v32,a3);
        }
      }
      goto LABEL_35;
    }
    (v25 = 30);
    if (r_u32(0x800FF738u))
      goto LABEL_29;
    (v30 = ((sint16)(r_u16(((uint32)(r_u32(0x800FF210u)))))));
    (v31 = ((sint16)(r_u16(((uint32)((r_u32(0x800FF210u) + 4)))))));
    (v54 = ((sint16)(r_u16(((uint32)((r_u32(0x800FF210u) + 2)))))));
    (v53 = v30);
    (v56 = 2);
    (v55 = v31);
    goto LABEL_28;
  }
  sub_80021B3C((v23 & 0x800),r_u32((((uint32)(a2))+(3)*4u)),3,r_u32((0x800EAEF8u+(((16 * r_u8(((uint32)((a1 + 27))))) + 8))*4u)));
  w_u32(0x800FFD58u,(((((v41 >> 12) + (v44 >> 12)) + (v47 >> 12)) / 3) << 12));
  w_u32(0x800FFD5Cu,(((((v42 >> 12) + (v45 >> 12)) + (v48 >> 12)) / 3) << 12));
  w_u32(0x800FFD60u,(((((v43 >> 12) + (v46 >> 12)) + (v49 >> 12)) / 3) << 12));
  (v24 = ((sint16)(r_u16(a2))));
  if ((((((sint16)(r_u16(a2))) & 0x40) == 0) && ((v24 & 1) != 0)))
  {
    (v28 = 0);
    if (((v24 & 0x20) != 0))
      (v28 = r_u32((((uint32)(v11))+(3)*4u)));
    if (!r_u32(0x800FF738u))
      {
          sint32 vertex0[3] = {v41,v42,v43};
          sint32 vertex1[3] = {v44,v45,v46};
          sint32 vertex2[3] = {v47,v48,v49};
          xport_draft_host_sub_80021358_p123(vertex0,vertex1,vertex2,r_u8(v11),r_u8((v11+(1)*1u)),r_u8((v11+(4)*1u)),r_u8((v11+(5)*1u)),r_u8((v11+(8)*1u)),r_u8((v11+(9)*1u)),v28,a3);
        }
    goto LABEL_35;
  }
  (v25 = 15);
  if (!r_u32(0x800FF738u))
  {
    (v26 = ((sint16)(r_u16(((uint32)((r_u32(0x800FF210u) + 2)))))));
    (v27 = ((sint16)(r_u16(((uint32)((r_u32(0x800FF210u) + 4)))))));
    (v50 = ((sint16)(r_u16(((uint32)(r_u32(0x800FF210u)))))));
    (v51 = v26);
    (v56 = 2);
    (v52 = v27);
    LABEL_28:
    {
      sint32 vertex0[3] = {v41,v42,v43};
      sint32 vertex1[3] = {v44,v45,v46};
      sint32 vertex2[3] = {v47,v48,v49};
      sint32 offset[3];
      if (v25 == 15) { offset[0]=v50; offset[1]=v51; offset[2]=v52; }
      else { offset[0]=v53; offset[1]=v54; offset[2]=v55; }
      xport_draft_host_sub_80022454_p2345(v25,vertex0,vertex1,vertex2,offset,r_u8(0x800FFB80u),v25==15?r_u8(0x800FFB81u):r_u8(0x800FFB82u),r_u8(0x800FFB82u));
    }

  }
  LABEL_29:
  if (a6)
    w_u16(a2,(r_u16(a2)&(~0x40u)));

  LABEL_35:
  if (a4)
  {
    if (!r_u32(0x800FF738u))
      sub_8001D320(0x800FFD58u,100,r_u16(0x800FFB80u),((r_u16(0x800FFB80u)>>8)&255u),((unsigned char)(r_u8(0x800FFB82u))),4,1,100);
  }

  return v56;
}


/* TODO Missing call adapter xport_draft_missing_gte_adapter */
uint32 sub_80021ADC(uint32 a1, uint32 a2, uint32 a3, uint32 a4)
{
  /* TODO Recover GTE register carriers and missing host GTE adapters */
  uint32 gte_A1 = 0u;
  
  sint32 v6;
  gte_A1 = (a2 + (r_u16(((uint32)(((2 * a4) + a3)))) & 0xFFF8));
  (abort(),0u);
  v6 = 12;
  return sub_8006C22C(a1,&v6);
}
/* TODO Missing call adapter sub_800878DC */
/* TODO Missing call adapter xport_draft_missing_gte_adapter */
uint32 sub_800344BC(uint32 a1)
{
  /* TODO Recover GTE register carriers and missing host GTE adapters */
  uint32 gte_V0 = 0u;
  
  sint32 result;
  char v6[32];
  sub_800858FC((a1 + 152),v6);
  ((void)(v6),abort(),0u);
  sub_80084504((a1 + 24));
  gte_V0 = (a1 + 72);
  (abort(),0u);
  gte_V0 = (a1 + 80);
  (abort(),0u);
  gte_V0 = (a1 + 88);
  (abort(),0u);
  result = (a1 + 96);
  (abort(),0u);
  return result;
}
uint32 sub_80034314(uint32 a1, uint32 a2, uint32 a3, uint32 a4)
{
  sint32 v5;
  sint32 v6;
  sint32 v7;
  sint32 v8;
  sint32 v9;
  sint32 v10;
  sint32 v11;
  sint32 result;
  v5 = a4;
  v6 = a3;
  v7 = a2;
  w_u32((a1+(43)*4u),(((a4 << 16) | (a3 << 8)) | a2));
  v8 = sub_80066570(4096);
  w_u32((a1+(44)*4u),(((((v5 * v8) >> 12) << 16) | (((v6 * v8) >> 12) << 8)) | ((v7 * v8) >> 12)));
  v9 = sub_80066570(4096);
  w_u32((a1+(45)*4u),(((((v5 * v9) >> 12) << 16) | (((v6 * v9) >> 12) << 8)) | ((v7 * v9) >> 12)));
  v10 = sub_80066570(4096);
  v11 = ((((v5 * v10) >> 12) << 16) | (((v6 * v10) >> 12) << 8));
  result = ((v7 * v10) >> 12);
  w_u32((a1+(46)*4u),(v11 | result));
  return result;
}
/* TODO Resolve original data label 0x800FF3F8u */
uint32 sub_80036B00(uint32 a1, uint32 a2, uint32 a3, uint32 a4, uint32 a5, uint32 a6)
{
  sint32 v12;
  sint8 v13;
  sint32 v14;
  sint32 v15;
  sint32 v16;
  sint32 v17;
  sint32 v18;
  sint32 v19;
  sint32 v20;
  sint32 vars0;
  sint32 vars4;
  short vars8;
  sint32 varsC;
  sub_80036634(((sint32)(a1)),a2,a3,2,((sint32)(0x800FF3F8u)),((sint32)(0x800FF3F8u)),400,1);
  v12 = r_u32((a1+(18)*4u));
  v13 = a4;
  v14 = (a4 / v12);
  v15 = (a5 / v12);
  v16 = 0;
  w_u32((a1+(17)*4u),0x800A1BB0u);
  v17 = (a6 / v12);
  while (1)
  {
    v18 = r_u32((a1+(18)*4u));
    v19 = (v16 + 1);
    if ((v16 >= v18))
      break;
    v20 = (4 * v16);
    sub_800332FC(r_u32(((uint32)((v20 + r_u32((a1+(23)*4u)))))),(v13 - ((v18 - v19) * v14)),(a5 - ((v18 - v19) * v15)),(a6 - ((v18 - v19) * v17)));
    sub_8003334C(r_u32(((uint32)((v20 + r_u32((a1+(23)*4u)))))),8);
    v16 = v19;
  }

  return a1;
}
uint32 sub_8003658C(uint32 a1)
{
  sub_80034E4C(a1);
  w_u32((a1+(17)*4u),0x800A1BE0u);
  return a1;
}
uint32 sub_80034E4C(uint32 a1)
{
  sub_8003319C(((sint32)(a1)));
  w_u32((a1+(17)*4u),0x800A1CC0u);
  sub_80032E50(a1,0x800FF43Cu);
  return a1;
}
uint32 sub_80033354(uint32 a1, uint32 a2)
{
  sint32 v3;
  sint32 result;
  sint32 v5;
  v3 = sub_8006F164(a2);
  w_u32(((uint32)((a1 + 80))),v3);
  result = r_u8(((uint32)((v3 - 4))));
  v5 = r_u32(((uint32)((a1 + 80))));
  w_u8(((uint32)((a1 + 90))),0);
  w_u8(((uint32)((a1 + 91))),0);
  w_u8(((uint32)((a1 + 89))),result);
  w_u32(((uint32)((a1 + 84))),v5);
  return result;
}
void sub_80033290(uint32 a1, uint32 a2)
{
  w_u16(((uint32)((a1 + 94))),a2);
}




