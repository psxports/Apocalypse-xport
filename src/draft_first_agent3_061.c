#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
#include <stdlib.h>

/* Unverified draft C; missing adapters remain TODO */
/* TODO Missing call adapter indirect */
/* TODO Missing call adapter nullsub_18 */
/* TODO Missing call adapter sub_800878DC */
/* TODO Missing call adapter sub_8008790C */
/* TODO Missing call adapter sub_8008793C */
void sub_8007EBDC(uint32 a1)
{
  sint32 gte_T2;
  sint32 gte_T4;
  sint32 gte_T5;
  sint32 gte_T6;
  sint32 gte_V0;
  sint32 gte_V1;
  uint32 v1;
  sint32 v5;
  unsigned short v6;
  sint32 v7;
  sint32 v8;
  unsigned short v9;
  sint32 v10;
  uint32 v11;
  uint32 v12;
  sint32 v15;
  uint32 v16;
  sint32 v17;
  uint32 v21[4];
  int position[3];
  char v25[32];
  char v26[32];
  v1 = a1;
  if (a1)
  {
    ((void)(0x800F3E30u),abort(),0u);
    ((void)(0x800F3E50u),abort(),0u);
    gte_T2 = 0;
    gte_V1 = (((sint32)(r_u32(0x800F25D4u))) >> 17);
    gte_V0 = (((sint32)(r_u32(0x800F25DCu))) >> 17);
    xport_draft_gte_control_write(13,gte_V1);
    xport_draft_gte_control_write(14,gte_V0);
    xport_draft_gte_control_write(15,gte_T2);
    sub_80080560(v1);
    if ((v1 == ((uint32)(r_u32(0x800FF794u)))))
    {
      sub_8007FF70(r_u32((0x800FF778u+(0)*4u)));
      sub_8007FF70(r_u32(0x800FF77Cu));
    }
    while (1)
    {
      if (!v1)
        return;
      if ((((r_u16(v1) & 0x8001) == 0) && ((r_u32((0x800EAEF8u+(((16 * r_u8((((uint32)(v1))+(27)*1u))) + 2))*4u))>>16)&255u)))
        break;
      LABEL_41:
      v1 = ((uint32)(r_u32((((uint32)(v1))+(7)*4u))));

    }

    v5 = r_u32(0x800FFAA0u);
    v6 = r_u16(v1);
    w_u32(0x1F800164,r_u32(0x800FFACCu));
    w_u32(0x1F800174,r_u32(0x800FFAD4u));
    w_u32(0x1F800184,r_u32(0x800FFB24u));
    w_u32(0x1F800194,r_u32(0x800FFB28u));
    w_u32(0x1F800204,((short)(r_u16(0x800FFAACu))));
    w_u32(0x1F800214,((short)(r_u16(0x800FFAAEu))));
    if (((v6 & 0x80) != 0))
    {
      if (((v6 & 0x1000) != 0))
        v5 = (r_u32(0x800FFAA0u) & 0xFFF70000);
      else
        v5 = (r_u32(0x800FFAA0u) | 0xC);
      if (((r_u16(v1) & 0x400) != 0))
      {
        v7 = r_u32((((uint32)(v1))+(8)*4u));
        w_u32(0x800FFB20u,(v7 & 0xFFFFFF));
        w_u16(0x800FFA96u,(((unsigned short)((v7 & 0xFF00))) >> 2));
        w_u16(0x800FFA98u,(((v7 & 0xFFFFFFu) >> 10) & 0x3FC0));
        w_u16(0x800FFA94u,(((unsigned char)(v7)) << 6));
      }
    }
    else
      if (((v6 & 0x400) != 0))
    {
      v5 = ((r_u32(0x800FFAA0u) & 0xFFFB0000) | 8);
      v8 = r_u32((((uint32)(v1))+(8)*4u));
      w_u32(0x800FFB20u,(v8 & 0xFFFFFF));
      w_u32(0x1F800154,(v8 & 0xFFFFFF));
      w_u32(0x1F8001A4,(((unsigned char)(v8)) << 6));
      w_u32(0x1F8001B4,(((unsigned short)((v8 & 0xFF00))) >> 2));
      w_u32(0x1F8001C4,(((v8 & 0xFFFFFFu) >> 10) & 0x3FC0));
    }
    if (((r_u16(v1) & 0x800) != 0))
      v5 = (((v5 & 0xFFFFFE7F) | (r_u32((((uint32)(v1))+(11)*4u)) & 0x180)) | 0x40);
    v9 = r_u16(v1);
    w_u32(0x1F800144,v5);
    if (((v9 & 0x80) != 0))
      ((void)((((sint32)(v1)) + ((sint16)(r_u16(((uint32)((r_u32((((uint32)(v1))+(17)*4u)) + 40)))))))),abort(),0u);
    v10 = (r_u16(v1) & 2);
    w_u32(0x800FFB3Cu,((r_u16(v1) >> 3) & 1));
    if (v10)
    {
      sub_8007F340(v1);
      if (((r_u16(v1) & 0x40) != 0))
        (abort(),0u);
      goto LABEL_41;
    }
    v11 = ((uint32)(((4 * r_u16((v1+(11)*2u))) + r_u32((0x800EAEF8u+(((16 * r_u8((((uint32)(v1))+(27)*1u))) + 4))*4u)))));
    v12 = r_u32(v11);
    if (((r_u32(r_u32(v11)) & 0x20) != 0))
      goto LABEL_41;
    ((void)((r_u32(0x800FFB0Cu) + 116)),abort(),0u);
    xport_draft_gte_control_write(5,0u);
    xport_draft_gte_control_write(6,0u);
    xport_draft_gte_control_write(7,0u);
    position[0] = ((((sint32)(r_u32((((uint32)(v1))+(1)*4u)))) >> 12) - r_u32(((uint32)((r_u32(0x800FFB0Cu) + 4)))));
    position[1] = ((((sint32)(r_u32((((uint32)(v1))+(2)*4u)))) >> 12) - r_u32(((uint32)((r_u32(0x800FFB0Cu) + 8)))));
    position[2] = ((((sint32)(r_u32((((uint32)(v1))+(3)*4u)))) >> 12) - r_u32(((uint32)((r_u32(0x800FFB0Cu) + 12)))));
    gte_T4 = (((unsigned short)(position[0])) | (((unsigned short)(position[1])) << 16));
    xport_draft_gte_transform_local_vector_to(0x486012,position,v21);
    v15 = r_u32((((uint32)(v1))+(4)*4u));
    w_u32(0x800FFB44u,0);
    if ((v15 || r_u16((v1+(10)*2u))))
    {
      xport_draft_host_sub_800858FC_p2((v1+(8)*2u),v26);
      if (((r_u16(v1) & 0x200) == 0))
      {
        LABEL_29:
        xport_draft_host_sub_800854F4_p12(v26,v25);

        xport_draft_host_sub_800878DC_p1(v25);
        goto LABEL_30;
      }
      v16 = v1;
    }
    else
    {
      xport_draft_host_sub_800854D8_p1(v26);
      v16 = v1;
      if (((r_u16(v1) & 0x200) == 0))
      {
        LABEL_30:
        v17 = (r_u32(v12) & 1);

        w_u32(0x800FFB04u,r_u32((0x800EAEF8u+(((16 * r_u8((((uint32)(v1))+(27)*1u))) + 8))*4u)));
        if (v17)
        {
          xport_draft_host_sub_80081458_p12(v26,v21,0x800F3E70u,v12);
        }
        else
          if (((r_u16(v1) & 0x80) != 0))
        {
          sub_80081794(v12 + 32u + r_u32(v12 + 4u) * 8u, r_u32(v12 + 8u) == r_u32(v12 + 12u) ? r_u32(v12 + 8u) : r_u32(v12 + 4u));
        }
        if (r_u32(0x800FFB44u))
        {
          xport_draft_host_sub_8007F9E0_p2(v12,position);
        }
        else
          if (((r_u32((((uint32)(v1))+(4)*4u)) || r_u16((v1+(10)*2u))) || ((r_u16(v1) & 0x200) != 0)))
        {
          gte_T4 = v21[0];
          gte_T5 = v21[1];
          gte_T6 = v21[2];
          xport_draft_gte_control_write(5,gte_T4);
          xport_draft_gte_control_write(6,gte_T5);
          xport_draft_gte_control_write(7,gte_T6);
          sub_8007F904(v12);
        }
        else
        {
          xport_draft_host_sub_8007FC60_p2(v12,position);
        }
        goto LABEL_41;
      }
    }
    xport_draft_host_sub_80085A08_p2(v16,v26);
    goto LABEL_29;
  }
}


uint32 sub_80080560(uint32 a1)
{
  sint32 gte_S0;
  sint32 gte_S1;
  sint32 gte_S2;
  sint32 gte_S3;
  sint32 gte_S4;
  sint32 gte_S5;
  sint32 gte_T0;
  sint32 gte_T1;
  sint32 gte_T2;
  sint32 gte_T3;
  sint32 gte_T4;
  sint32 result;
  sint32 v7;
  sint32 v8;
  sint32 v9;
  sint32 v21;
  sint32 v22;
  sint32 v23;
  short v24;
  sint32 v25;
  sint32 v26;
  sint32 v27;
  sint32 v28;
  uint32 v31;
  sint32 v32;
  sint32 v36;
  sint32 v40;
  sint32 v41;
  sint32 v42;
  uint32 v49;
  uint32 v50;
  sint32 v51;
  result = 528482304;
  if (a1)
  {
    gte_T0 = xport_draft_gte_control_read(8);
    gte_T1 = xport_draft_gte_control_read(9);
    gte_T2 = xport_draft_gte_control_read(10);
    gte_T3 = xport_draft_gte_control_read(11);
    gte_T4 = xport_draft_gte_control_read(12);
    v7 = ((((gte_T0 >> 12) & 8) | ((gte_T0 >> 27) & 0x10)) | ((gte_T1 >> 10) & 0x20));
    v8 = ((((gte_T1 >> 28) & 8) | ((gte_T2 >> 11) & 0x10)) | ((gte_T2 >> 26) & 0x20));
    v9 = ((((gte_T3 >> 12) & 8) | ((gte_T3 >> 27) & 0x10)) | ((gte_T4 >> 10) & 0x20));
    gte_T0 = xport_draft_gte_control_read(16);
    gte_T1 = xport_draft_gte_control_read(17);
    gte_T2 = xport_draft_gte_control_read(18);
    gte_T3 = xport_draft_gte_control_read(19);
    gte_T4 = xport_draft_gte_control_read(20);
    gte_S0 = (v7 + 528482304);
    gte_S1 = (v8 + 528482304);
    gte_S2 = (v9 + 528482304);
    gte_S3 = (((((gte_T0 >> 12) & 8) | ((gte_T0 >> 27) & 0x10)) | ((gte_T1 >> 10) & 0x20)) + 528482304);
    gte_S4 = (((((gte_T1 >> 28) & 8) | ((gte_T2 >> 11) & 0x10)) | ((gte_T2 >> 26) & 0x20)) + 528482304);
    gte_S5 = (((((gte_T3 >> 12) & 8) | ((gte_T3 >> 27) & 0x10)) | ((gte_T4 >> 10) & 0x20)) + 528482304);
    v21 = r_u32(((uint32)((0x800FFB0Cu + 4))));
    v22 = r_u32(((uint32)((0x800FFB0Cu + 8))));
    v23 = r_u32(((uint32)((0x800FFB0Cu + 12))));
    do
    {
      while (1)
      {
        v24 = r_u16(((uint32)(a1)));
        if (((r_u16(((uint32)(a1))) & 1) != 0))
          break;
        v25 = (((sint32)(r_u32((a1+(3)*4u)))) >> 12);
        v26 = ((((sint32)(r_u32((a1+(1)*4u)))) >> 12) - v21);
        v27 = ((((sint32)(r_u32((a1+(2)*4u)))) >> 12) - v22);
        v28 = (v25 - v23);
        if ((((((((v25 - v23) >= 0x7FFF) || ((v25 - v23) < -32767)) || (v26 >= 0x7FFF)) || (v26 < -32767)) || (v27 >= 0x7FFF)) || (v27 < -32767)))
          break;
        gte_T1 = (((unsigned short)((v26 >> 1))) | ((v27 >> 1) << 16));
        gte_T2 = (v28 >> 1);
        xport_draft_gte_data_write(0,gte_T1);
        xport_draft_gte_data_write(1,gte_T2);
        xport_draft_gte_execute(0x4A2012);
        v31 = r_u32(((uint32)(((4 * r_u16((((uint32)(a1))+(11)*2u))) + r_u32((0x800EAEF8u+(((16 * r_u8((((uint32)(a1))+(27)*1u))) + 4))*4u))))));
        v32 = (r_u32((v31+(4)*4u)) >> 13);
        if (((r_u16(((uint32)(a1))) & 0x200) != 0))
        {
          v51 = ((sint16)(r_u16((((uint32)(a1))+(18)*2u))));
          if (((((sint16)(r_u16((((uint32)(a1))+(19)*2u)))) - v51) > 0))
            v51 = ((sint16)(r_u16((((uint32)(a1))+(19)*2u))));
          if (((((sint16)(r_u16((((uint32)(a1))+(20)*2u)))) - v51) > 0))
            v51 = ((sint16)(r_u16((((uint32)(a1))+(20)*2u))));
          v32 = (((uint32)((v32 * v51))) >> 12);
        }
        gte_T1 = xport_draft_gte_data_read(25);
        gte_T2 = xport_draft_gte_data_read(26);
        gte_T3 = xport_draft_gte_data_read(27);
        xport_draft_gte_execute(0x4C6012);
        v36 = -v32;
        if ((gte_T1 < v36))
          break;
        if ((gte_T2 < v36))
          break;
        if ((gte_T3 < v36))
          break;
        gte_T1 = xport_draft_gte_data_read(25);
        gte_T2 = xport_draft_gte_data_read(26);
        gte_T3 = xport_draft_gte_data_read(27);
        if ((gte_T1 < v36))
          break;
        if ((gte_T2 < v36))
          break;
        if ((gte_T3 < v36))
          break;
        v40 = r_u32((v31+(5)*4u));
        if (!(((((sint32)(r_u32((a1+(4)*4u)))) | r_u16((((uint32)(a1))+(10)*2u))) | (v24 & 0x202))))
        {
          v41 = r_u32((v31+(6)*4u));
          v42 = r_u32((v31+(7)*4u));
          w_u16(0x1F800000,(((short)((v26 + v40))) >> 1));
          w_u16(0x1F800010,(((short)((v26 + v40))) >> 1));
          w_u16(0x1F800020,(((short)((v26 + v40))) >> 1));
          w_u16(0x1F800030,(((short)((v26 + v40))) >> 1));
          w_u16(0x1F800008,((v26 + (v40 >> 16)) >> 1));
          w_u16(0x1F800018,r_u16(0x1F800008));
          w_u16(0x1F800028,r_u16(0x1F800008));
          w_u16(0x1F800038,r_u16(0x1F800008));
          w_u16(0x1F800002,(((short)((v27 + v41))) >> 1));
          w_u16(0x1F80000A,(((short)((v27 + v41))) >> 1));
          w_u16(0x1F800022,(((short)((v27 + v41))) >> 1));
          w_u16(0x1F80002A,(((short)((v27 + v41))) >> 1));
          w_u16(0x1F800012,((v27 + (v41 >> 16)) >> 1));
          w_u16(0x1F80001A,r_u16(0x1F800012));
          w_u16(0x1F800032,r_u16(0x1F800012));
          w_u16(0x1F80003A,r_u16(0x1F800012));
          w_u16(0x1F800004,(((short)((v28 + v42))) >> 1));
          w_u16(0x1F80000C,(((short)((v28 + v42))) >> 1));
          w_u16(0x1F800014,(((short)((v28 + v42))) >> 1));
          w_u16(0x1F80001C,(((short)((v28 + v42))) >> 1));
          w_u16(0x1F800024,((v28 + (v42 >> 16)) >> 1));
          w_u16(0x1F80002C,r_u16(0x1F800024));
          w_u16(0x1F800034,r_u16(0x1F800024));
          w_u16(0x1F80003C,r_u16(0x1F800024));
          xport_draft_gte_data_write(0,r_u32((gte_S0 + 0u)));
          xport_draft_gte_data_write(1,r_u32((gte_S0 + 4u)));
          xport_draft_gte_execute(0x4A2012);
          gte_T1 = xport_draft_gte_data_read(9);
          xport_draft_gte_data_write(2,r_u32((gte_S1 + 0u)));
          xport_draft_gte_data_write(3,r_u32((gte_S1 + 4u)));
          if ((gte_T1 < 0))
            break;
          xport_draft_gte_execute(0x4AA012);
          gte_T2 = xport_draft_gte_data_read(10);
          xport_draft_gte_data_write(4,r_u32((gte_S2 + 0u)));
          xport_draft_gte_data_write(5,r_u32((gte_S2 + 4u)));
          if ((gte_T2 < 0))
            break;
          xport_draft_gte_execute(0x4B2012);
          gte_T3 = xport_draft_gte_data_read(11);
          xport_draft_gte_data_write(0,r_u32((gte_S3 + 0u)));
          xport_draft_gte_data_write(1,r_u32((gte_S3 + 4u)));
          if ((gte_T3 < 0))
            break;
          xport_draft_gte_execute(0x4C6012);
          gte_T1 = xport_draft_gte_data_read(9);
          xport_draft_gte_data_write(2,r_u32((gte_S4 + 0u)));
          xport_draft_gte_data_write(3,r_u32((gte_S4 + 4u)));
          if ((gte_T1 < 0))
            break;
          xport_draft_gte_execute(0x4CE012);
          gte_T2 = xport_draft_gte_data_read(10);
          xport_draft_gte_data_write(4,r_u32((gte_S5 + 0u)));
          xport_draft_gte_data_write(5,r_u32((gte_S5 + 4u)));
          if ((gte_T2 < 0))
            break;
          xport_draft_gte_execute(0x4D6012);
          gte_T3 = xport_draft_gte_data_read(11);
          if ((gte_T3 < 0))
            break;
        }
        v49 = ((uint32)(((sint32)(r_u32((a1+(7)*4u))))));
        w_u16(((uint32)(a1)),(v24 & 0x7FFF));
        a1 = ((uint32)(v49));
        if (!v49)
          return result;
      }

      v50 = ((uint32)(((sint32)(r_u32((a1+(7)*4u))))));
      w_u16(((uint32)(a1)),(v24 | 0x8000));
      a1 = ((uint32)(v50));
    }
    while (v50);
  }
  return result;
}


uint32 sub_8002948C(uint32 a1)
{
  sint32 gte_T4;
  sint32 v2;
  sint32 v3;
  sint32 v4;
  sint32 v5;
  uint32 v6;
  sint32 v8;
  short v9;
  sint32 v11;
  sint32 v12;
  sint32 v14;
  sint32 v15;
  sint32 v16;
  short v17;
  sint32 v18;
  sint32 v19;
  sint32 v20;
  sint32 v21;
  uint32 v22;
  sint32 v23;
  sint32 v24;
  sint32 v25;
  sint32 v26;
  sint32 v27;
  sint32 result;
  sint32 v29;
  uint32 v30;
  sint32 v31;
  sint32 v32;
  sint32 v33;
  uint32 v34;
  uint32 v35;
  sint32 v36;
  sint8 v37;
  sint32 v38;
  sint32 v39;
  sint32 v40;
  sint32 v41;
  sint32 v42;
  sint32 v43;
  sint32 v44;
  uint32 v45;
  sint32 v46;
  sint32 v47;
  sint32 v48;
  sint32 v49;
  sint32 v50;
  sint32 v51;
  int v52[2];
  short v53;
  short v54;
  short v55;
  short v56;
  sint32 v57;
  v2 = 528482304;
  v3 = 0;
  v4 = r_u32(((uint32)((a1 + 80))));
  v5 = 528482326;
  v6 = ((uint32)((v4 + 112)));
  while ((v3 < r_u32(((uint32)((a1 + 76))))))
  {
    w_u16(((uint32)(v5)),0);
    v46 = ((((sint32)(r_u32((v6+(4)*4u)))) >> 12) - r_u32(0x800ED520u));
    v49 = ((((sint32)(r_u32((v6+(5)*4u)))) >> 12) - r_u32(0x800ED524u));
    v52[0] = ((((sint32)(r_u32((v6+(6)*4u)))) >> 12) - r_u32(0x800ED528u));
    gte_T4 = (((unsigned short)(v46)) | (((unsigned short)(v49)) << 16));
    xport_draft_gte_data_write(0,gte_T4);
    xport_draft_gte_data_write(1,v52[0]);
    xport_draft_gte_execute(0x180001);
    w_u32(v2,xport_draft_gte_data_read(14));
    v57 = xport_draft_gte_data_read(27);
    v8 = r_u32(((uint32)(v2)));
    if (((((((r_u32(((uint32)(v2))) == 67044351) || (v8 == 67107840)) || (v8 == -67107841)) || (v8 == -67044352)) || (v57 < -20000)) || (v57 >= 20001)))
    {
      w_u16(((uint32)((v5 - 18))),0);
    }
    else
    {
      v9 = v57;
      w_u16(((uint32)((v5 - 18))),1);
      w_u16(((uint32)(v5)),v9);
    }
    v47 = ((((sint32)(r_u32((v6+(1)*4u)))) >> 12) - r_u32(0x800ED520u));
    v50 = ((((sint32)(r_u32((v6+(2)*4u)))) >> 12) - r_u32(0x800ED524u));
    v52[0] = ((((sint32)(r_u32((v6+(3)*4u)))) >> 12) - r_u32(0x800ED528u));
    gte_T4 = (((unsigned short)(v47)) | (((unsigned short)(v50)) << 16));
    xport_draft_gte_data_write(0,gte_T4);
    xport_draft_gte_data_write(1,v52[0]);
    xport_draft_gte_execute(0x180001);
    w_u32((v5 - 14),xport_draft_gte_data_read(14));
    v57 = xport_draft_gte_data_read(27);
    v11 = r_u32(((uint32)((v5 - 14))));
    if (((((((v11 == 67044351) || (v11 == 67107840)) || (v11 == -67107841)) || (v11 == -67044352)) || (v57 < -20000)) || (v57 >= 20001)))
    {
      w_u16(((uint32)((v5 - 10))),0);
    }
    else
    {
      v12 = (v57 >= ((sint16)(r_u16(((uint32)(v5))))));
      w_u16(((uint32)((v5 - 10))),1);
      if (!v12)
        w_u16(((uint32)(v5)),v57);
    }
    v48 = ((((sint32)(r_u32((v6-(2)*4u)))) >> 12) - r_u32(0x800ED520u));
    v51 = ((((sint32)(r_u32((v6-(1)*4u)))) >> 12) - r_u32(0x800ED524u));
    v52[0] = ((((sint32)(r_u32(v6))) >> 12) - r_u32(0x800ED528u));
    gte_T4 = (((unsigned short)(v48)) | (((unsigned short)(v51)) << 16));
    xport_draft_gte_data_write(0,gte_T4);
    xport_draft_gte_data_write(1,v52[0]);
    xport_draft_gte_execute(0x180001);
    w_u32((v5 - 6),xport_draft_gte_data_read(14));
    v57 = xport_draft_gte_data_read(27);
    v14 = r_u32(((uint32)((v5 - 6))));
    if (((((((v14 == 67044351) || (v14 == 67107840)) || (v14 == -67107841)) || (v14 == -67044352)) || (v57 < -20000)) || (v57 >= 20001)))
    {
      w_u16(((uint32)((v5 - 2))),0);
    }
    else
    {
      v12 = (v57 >= ((sint16)(r_u16(((uint32)(v5))))));
      w_u16(((uint32)((v5 - 2))),1);
      if (!v12)
        w_u16(((uint32)(v5)),v57);
    }
    v6 += (35)*4u;
    v5 += 24;
    v2 += 24;
    ++v3;
  }

  v15 = 1;
  v53 = r_u8(r_u32(((uint32)((a1 + 72)))));
  v16 = 528482350;
  v17 = r_u8(((uint32)((r_u32(((uint32)((a1 + 72)))) + 1))));
  v55 = 64;
  v56 = 64;
  v54 = v17;
  v18 = r_u32(0x1F800000);
  v19 = r_u32(0x1F800008);
  v20 = r_u32(0x1F800010);
  v21 = 528482328;
  v22 = r_u32(((uint32)((a1 + 80))));
  v23 = (sint16)r_u16(0x1F800004);
  v24 = (sint16)r_u16(0x1F80000C);
  v25 = (sint16)r_u16(0x1F800014);
  v26 = (sint16)r_u16(0x1F800016);
  while (1)
  {
    v27 = r_u32(((uint32)((a1 + 76))));
    result = (v27 < v15);
    if ((v27 < v15))
      break;
    if ((v15 == v27))
    {
      v16 = 528482326;
      v21 = 528482304;
    }
    if ((((sint16)(r_u16(((uint32)(v16))))) < v26))
      v26 = ((sint16)(r_u16(((uint32)(v16)))));
    if (((r_u16(((uint32)((a1 + 114)))) || v26) && (((r_u32(((uint32)((a1 + 108)))) >> v15) & 1) != 0)))
    {
      if (r_u16(((uint32)((a1 + 114)))))
        v29 = (4 * r_u16(((uint32)((a1 + 114)))));
      else
        v29 = ((((uint16)(v26)) - r_u16(((uint32)((a1 + 64))))) & 0x3FFC);
      v30 = ((uint32)(((r_u32(0x800FF660u) + v29) + 112)));
      v31 = r_u32(0x800FF668u);
      v32 = (r_u32(0x800FF668u) + 12);
      result = (r_u32(0x800FF374u) < ((uint32)((r_u32(0x800FF668u) + 32))));
      v33 = (r_u32(0x800FF668u) + 24);
      if ((r_u32(0x800FF374u) < ((uint32)((r_u32(0x800FF668u) + 32)))))
        return result;
      w_u32(0x800FF668u,(r_u32(0x800FF668u)+(32)));
      w_u8(((uint32)((v32 + 3))),2);
      w_u32(((uint32)((v32 + 8))),0);
      w_u32(((uint32)((v32 + 4))),-503316480);
      w_u8(((uint32)((v31 + 3))),2);
      v34 = ((unsigned char)(v54));
      v35 = ((unsigned char)(v53));
      v36 = v56;
      v37 = v55;
      w_u32(((uint32)((v31 + 8))),0);
      w_u32(((uint32)((v31 + 4))),((((((v34 >> 3) << 15) | ((v35 >> 3) << 10)) | 0xE2000000) | ((-4 * v36) & 0x3E0)) | (((sint32)(((unsigned char)(-v37)))) >> 3)));
      v38 = 0;
      v39 = r_u32(0x800FF46Cu);
      v40 = 0;
      w_u32(((uint32)((v31 + 24))),r_u32(0x800FF468u));
      w_u32(((uint32)((v33 + 4))),v39);
      w_u32(((uint32)((v31 + 12))),((r_u32(((uint32)((v31 + 12)))) & 0xFF000000) | (r_u32(v30) & 0xFFFFFF)));
      w_u32(v30,((r_u32(v30) & 0xFF000000) | (v32 & 0xFFFFFF)));
      if ((((v23 && v24) && r_u16(((uint32)((v16 - 18))))) && r_u16(((uint32)((v16 - 10))))))
      {
        v38 = r_u32(0x800FF668u);
        result = (r_u32(0x800FF374u) < ((uint32)((r_u32(0x800FF668u) + 52))));
        if ((r_u32(0x800FF374u) < ((uint32)((r_u32(0x800FF668u) + 52)))))
          return result;
        w_u32(0x800FF668u,(r_u32(0x800FF668u)+(52)));
        w_u32(((uint32)(v38)),r_u32(v22));
        w_u32(((uint32)((v38 + 4))),r_u32((v22+(1)*4u)));
        w_u32(((uint32)((v38 + 12))),r_u32((v22+(3)*4u)));
        w_u32(((uint32)((v38 + 16))),r_u32((v22+(4)*4u)));
        w_u32(((uint32)((v38 + 24))),r_u32((v22+(6)*4u)));
        w_u32(((uint32)((v38 + 28))),r_u32((v22+(7)*4u)));
        w_u32(((uint32)((v38 + 36))),r_u32((v22+(9)*4u)));
        w_u32(((uint32)((v38 + 40))),r_u32((v22+(10)*4u)));
        v41 = r_u32((v22+(12)*4u));
        w_u32(((uint32)((v38 + 8))),v18);
        w_u32(((uint32)((v38 + 48))),v41);
        v42 = r_u32(((uint32)(v21)));
        w_u32(((uint32)((v38 + 32))),v19);
        w_u32(((uint32)((v38 + 20))),v42);
        w_u32(((uint32)((v38 + 44))),r_u32(((uint32)((v16 - 14)))));
      }
      if ((((v25 && v24) && r_u16(((uint32)((v16 - 2))))) && r_u16(((uint32)((v16 - 10))))))
      {
        v40 = r_u32(0x800FF668u);
        result = (r_u32(0x800FF374u) < ((uint32)((r_u32(0x800FF668u) + 52))));
        if ((r_u32(0x800FF374u) < ((uint32)((r_u32(0x800FF668u) + 52)))))
          return result;
        w_u32(0x800FF668u,(r_u32(0x800FF668u)+(52)));
        w_u32(((uint32)(v40)),r_u32((v22+(13)*4u)));
        w_u32(((uint32)((v40 + 4))),r_u32((v22+(14)*4u)));
        w_u32(((uint32)((v40 + 12))),r_u32((v22+(16)*4u)));
        w_u32(((uint32)((v40 + 16))),r_u32((v22+(17)*4u)));
        w_u32(((uint32)((v40 + 24))),r_u32((v22+(19)*4u)));
        w_u32(((uint32)((v40 + 28))),r_u32((v22+(20)*4u)));
        w_u32(((uint32)((v40 + 36))),r_u32((v22+(22)*4u)));
        w_u32(((uint32)((v40 + 40))),r_u32((v22+(23)*4u)));
        v43 = r_u32((v22+(25)*4u));
        w_u32(((uint32)((v40 + 8))),v19);
        w_u32(((uint32)((v40 + 48))),v43);
        v44 = r_u32(((uint32)((v16 - 14))));
        w_u32(((uint32)((v40 + 32))),v20);
        w_u32(((uint32)((v40 + 20))),v44);
        w_u32(((uint32)((v40 + 44))),r_u32(((uint32)((v16 - 6)))));
      }
      if (v38)
      {
        w_u32(((uint32)(v38)),((r_u32(((uint32)(v38))) & 0xFF000000) | (r_u32(v30) & 0xFFFFFF)));
        w_u32(v30,((r_u32(v30) & 0xFF000000) | (v38 & 0xFFFFFF)));
      }
      if (v40)
      {
        w_u32(((uint32)(v40)),((r_u32(((uint32)(v40))) & 0xFF000000) | (r_u32(v30) & 0xFFFFFF)));
        w_u32(v30,((r_u32(v30) & 0xFF000000) | (v40 & 0xFFFFFF)));
      }
      w_u32(((uint32)(v31)),((r_u32(((uint32)(v31))) & 0xFF000000) | (r_u32(v30) & 0xFFFFFF)));
      v45 = ((r_u32(v30) & 0xFF000000) | (v31 & 0xFFFFFF));
      w_u32(v30,v45);
      w_u32(((uint32)(v33)),((r_u32(((uint32)(v33))) & 0xFF000000) | (v45 & 0xFFFFFF)));
      w_u32(v30,((r_u32(v30) & 0xFF000000) | (v33 & 0xFFFFFF)));
    }
    v19 = r_u32(((uint32)((v16 - 14))));
    v20 = r_u32(((uint32)((v16 - 6))));
    v23 = ((sint16)(r_u16(((uint32)((v16 - 18))))));
    v24 = ((sint16)(r_u16(((uint32)((v16 - 10))))));
    v25 = ((sint16)(r_u16(((uint32)((v16 - 2))))));
    v26 = ((sint16)(r_u16(((uint32)(v16)))));
    v16 += 24;
    v18 = r_u32(((uint32)(v21)));
    v21 += 24;
    ++v15;
  }

  return result;
}

