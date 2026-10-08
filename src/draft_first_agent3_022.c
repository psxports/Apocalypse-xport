#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
#include <stdlib.h>
/* TODO Missing call adapter sub_8008773C */
/* TODO Missing call adapter sub_800878DC */
/* TODO Missing call adapter xport_draft_missing_gte_adapter */
uint32 sub_8007BB24(uint32 a1)
{
  /* TODO Recover GTE register carriers and missing host GTE adapters */
  uint32 gte_S6 = 0u;
  uint32 gte_V0 = 0u;
  
  sint32 v2;
  sint32 v3;
  sint32 v4;
  sint32 v5;
  sint32 v7;
  sint32 v8;
  sint32 v10;
  sint32 v11;
  sint32 v12;
  sint32 v13;
  short v14;
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
  sint32 result;
  sint32 v29;
  w_u32(((uint32)((a1 + 64))),0x7FFFFFFF);
  v2 = r_u32(((uint32)((a1 + 12))));
  v3 = r_u32(((uint32)(a1)));
  w_u32(((uint32)((a1 + 104))),0);
  w_u32(((uint32)((a1 + 132))),-1);
  w_u8(((uint32)((a1 + 136))),0);
  w_u8(((uint32)((a1 + 137))),0);
  w_u16(0x800FFA3Cu,((v2 - v3) >> 12));
  v4 = ((r_u32(((uint32)((a1 + 16)))) - r_u32(((uint32)((a1 + 4))))) >> 12);
  w_u16(0x800FFA3Eu,v4);
  v5 = ((r_u32(((uint32)((a1 + 20)))) - r_u32(((uint32)((a1 + 8))))) >> 12);
  w_u16(0x800FFA40u,v5);
  gte_S6 = ((r_u16(0x800FFA3Cu) * r_u16(0x800FFA3Cu)) + (((short)(v5)) * ((short)(v5))));
  if (gte_S6)
  {
    (abort(),0u);
    v7 = ((v29 - 1) >> 1);
    v8 = sub_80085B54((gte_S6 << ((v29 - 1) & 0x1E)));
    w_u16(0x800FFA2Eu,((r_u16(0x800FFA3Cu) << (v7 + 12)) / v8));
    w_u16(0x800FFA2Cu,((r_u16(0x800FFA40u) << (v7 + 12)) / v8));
    w_u16(0x800ED740u,r_u16(0x800FFA2Cu));
    w_u16(0x800ED742u,0);
    w_u16(0x800ED744u,-r_u16(0x800FFA2Eu));
    w_u16(0x800ED746u,0);
    w_u16(0x800ED748u,4096);
    w_u16(0x800ED74Au,0);
    w_u16(0x800ED74Cu,r_u16(0x800FFA2Eu));
    w_u16(0x800ED74Eu,0);
    w_u16(0x800ED750u,r_u16(0x800FFA2Cu));
    gte_V0 = (gte_S6 + (r_u16(0x800FFA3Eu) * r_u16(0x800FFA3Eu)));
    (abort(),0u);
    v10 = sub_80085B54((gte_V0 << ((v29 - 1) & 0x1E)));
    v11 = ((v8 << 12) / v10);
    w_u32(((uint32)((a1 + 68))),(v10 >> v7));
    v12 = ((r_u16(0x800FFA3Eu) << (v7 + 12)) / v10);
    w_u16(0x800FFA2Cu,v11);
    w_u16(0x800FFA2Eu,v12);
    w_u32(((uint32)((a1 + 72))),4096);
    w_u16(((uint32)((a1 + 76))),0);
    w_u16(((uint32)((a1 + 78))),0);
    w_u16(((uint32)((a1 + 80))),v11);
    w_u32(((uint32)((a1 + 82))),((unsigned short)(-(((short)(v12))))));
    w_u16(((uint32)((a1 + 86))),v12);
    w_u16(((uint32)((a1 + 88))),v11);
    ((void)((a1 + 72)),(void)(0x800ED740u),abort(),0u);
    v13 = (a1 + 72);
  }
  else
  {
    if (((v4 & 0x8000u) == 0))
    {
      w_u16(0x800FFA2Eu,4096);
      w_u32(((uint32)((a1 + 68))),((short)(v4)));
      w_u8(((uint32)((a1 + 137))),1);
    }
    else
    {
      w_u16(0x800FFA2Eu,-4096);
      w_u32(((uint32)((a1 + 68))),-(((short)(v4))));
    }
    w_u16(((uint32)((a1 + 72))),4096);
    w_u16(((uint32)((a1 + 74))),0);
    w_u16(((uint32)((a1 + 76))),0);
    w_u16(((uint32)((a1 + 78))),0);
    w_u16(((uint32)((a1 + 80))),0);
    v14 = r_u16(0x800FFA2Eu);
    v13 = (a1 + 72);
    w_u16(((uint32)((a1 + 84))),0);
    w_u16(((uint32)((a1 + 88))),0);
    w_u16(((uint32)((a1 + 82))),-v14);
    w_u16(((uint32)((a1 + 86))),v14);
  }
  ((void)(v13),abort(),0u);
  v15 = r_u32(((uint32)(a1)));
  v16 = r_u32(((uint32)((a1 + 12))));
  w_u32(0x800FFA68u,a1);
  w_u32(0x800FFA6Cu,(a1 + 12));
  v17 = v16;
  if ((v15 >= v16))
  {
    v17 = r_u32(((uint32)(a1)));
    w_u32(((uint32)((a1 + 24))),v16);
  }
  else
  {
    w_u32(((uint32)((a1 + 24))),v15);
  }
  w_u32(((uint32)((a1 + 36))),v17);
  v18 = r_u32(0x800FFA68u);
  v19 = r_u32(0x800FFA6Cu);
  v20 = r_u32(((uint32)((r_u32(0x800FFA68u) + 4))));
  v21 = r_u32(((uint32)((r_u32(0x800FFA6Cu) + 4))));
  if ((v20 >= v21))
  {
    w_u32(((uint32)((a1 + 28))),v21);
    v22 = r_u32(((uint32)((v18 + 4))));
  }
  else
  {
    w_u32(((uint32)((a1 + 28))),v20);
    v22 = r_u32(((uint32)((v19 + 4))));
  }
  w_u32(((uint32)((a1 + 40))),v22);
  v23 = r_u32(0x800FFA68u);
  v24 = r_u32(0x800FFA6Cu);
  v25 = r_u32(((uint32)((r_u32(0x800FFA68u) + 8))));
  v26 = r_u32(((uint32)((r_u32(0x800FFA6Cu) + 8))));
  if ((v25 >= v26))
  {
    w_u32(((uint32)((a1 + 32))),v26);
    v27 = r_u32(((uint32)((v23 + 8))));
  }
  else
  {
    w_u32(((uint32)((a1 + 32))),v25);
    v27 = r_u32(((uint32)((v24 + 8))));
  }
  w_u32(((uint32)((a1 + 44))),v27);
  sub_8007BAB0();
  result = ((unsigned short)(r_u16(0x800FF968u)));
  w_u16(((uint32)((a1 + 138))),r_u16(0x800FF968u));
  return result;
}
/* TODO Postincrement memory expressions may require ordering refinement */
uint32 sub_8007BAB0(void)
{
  sint32 result;
  sint32 v1;
  sint32 i;
  (w_u16(0x800FF968u,(r_u16(0x800FF968u)+1u)),(r_u16(0x800FF968u)+1u));
  result = 1;
  if (!r_u16(0x800FF968u))
  {
    v1 = r_u32(0x800FF794u);
    for (w_u16(0x800FF968u,1); v1; v1 = r_u32(((uint32)((v1 + 28)))))
      w_u16(((uint32)((v1 + 2))),0);

    for (i = r_u32(0x800FF5DCu); i; i = r_u32(((uint32)((i + 28)))))
      w_u16(((uint32)((i + 2))),0);

  }
  return result;
}
uint32 sub_8007DD04(uint32 a1, uint32 a2)
{
  uint32 v2;
  sint32 v3;
  sint32 v4;
  sint32 v5;
  sint32 v6;
  sint32 v7;
  uint32 v8;
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
  sint32 v23;
  sint32 v24;
  sint32 v25;
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
  sint32 v49;
  sint32 v50;
  uint32 v51;
  sint32 v52;
  sint32 v53;
  sint32 v54;
  sint32 v55;
  sint32 v56;
  sint32 v57;
  sint32 v58;
  w_u32(0x800FF988u,0);
  w_u32(0x800FF984u,-1);
  if (r_u32(0x800FF974u))
    w_u32(0x800FF988u,0x400000);
  if (!r_u32(0x800FF970u))
    w_u32(0x800FF988u,(r_u32(0x800FF988u)^(0x200000u)));
  if (r_u32(0x800FF96Cu))
    w_u32(0x800FF984u,-1048577);
  if (r_u32(0x800FF978u))
    w_u32(0x800FF984u,(r_u32(0x800FF984u)^(0x20000u)));
  if (a2)
    sub_8007BEA0(r_u32(0x800FF5DCu),((sint32)(a1)));
  v51 = 0x800EDA30u;
  v52 = 0;
  do
  {
    v2 = (v51+(408)*4u);
    if (!(((sint32)(r_u32(v51)))))
      goto LABEL_106;
    v3 = ((sint32)(r_u32(a1)));
    v4 = ((sint32)(r_u32((a1+(3)*4u))));
    v5 = ((sint32)(r_u32((a1+(2)*4u))));
    v6 = ((sint32)(r_u32((v51+(1)*4u))));
    v7 = ((sint32)(r_u32((a1+(5)*4u))));
    v8 = v51;
    v9 = ((sint32)(r_u32((v51+(3)*4u))));
    v10 = ((sint32)(r_u32((v51+(2)*4u))));
    v11 = ((sint32)(r_u32((v51+(4)*4u))));
    v12 = ((sint32)(r_u32((v51+(5)*4u))));
    if (((((sint32)(r_u32(a1))) < v6) && (v4 < v6)))
      goto LABEL_105;
    if (((((v9 < v3) && (v9 < v4)) || ((v5 < v10) && (v7 < v10))) || ((v11 < v5) && (v11 < v7))))
      goto LABEL_104;
    if (((v3 == v4) && (v5 == v7)))
    {
      v13 = ((v3 - v6) / v12);
      v14 = ((v5 - v10) / v12);
      if ((v13 == ((sint16)(r_u16((((uint32)(v51))+(14)*2u))))))
        --v13;
      v15 = v14;
      if ((v14 != ((sint16)(r_u16((((uint32)(v51))+(15)*2u))))))
        goto LABEL_103;
      v16 = (v14 - 1);
    }
    else
    {
      if ((v3 < v6))
      {
        if ((v5 >= v7))
          v17 = (v7 + sub_80085BA4((v4 - v6),(v5 - v7),(v4 - v3)));
        else
          v17 = (v5 + sub_80085BA4((v6 - v3),(v7 - v5),(v4 - v3)));
        v5 = v17;
        v3 = v6;
      }
      if ((v4 < v6))
      {
        if ((v7 >= v5))
          v18 = (v5 + sub_80085BA4((v3 - v6),(v7 - v5),(v3 - v4)));
        else
          v18 = (v7 + sub_80085BA4((v6 - v4),(v5 - v7),(v3 - v4)));
        v7 = v18;
        v4 = v6;
      }
      if ((v9 < v3))
      {
        if ((v7 >= v5))
          v19 = (v5 + sub_80085BA4((v3 - v9),(v7 - v5),(v3 - v4)));
        else
          v19 = (v7 + sub_80085BA4((v9 - v4),(v5 - v7),(v3 - v4)));
        v5 = v19;
        v3 = v9;
      }
      if ((v9 < v4))
      {
        if ((v5 >= v7))
          v20 = (v7 + sub_80085BA4((v4 - v9),(v5 - v7),(v4 - v3)));
        else
          v20 = (v5 + sub_80085BA4((v9 - v3),(v7 - v5),(v4 - v3)));
        v7 = v20;
        v4 = v9;
      }
      if ((v5 < v10))
      {
        if ((v3 >= v4))
          v21 = (v4 + sub_80085BA4((v7 - v10),(v3 - v4),(v7 - v5)));
        else
          v21 = (v3 + sub_80085BA4((v10 - v5),(v4 - v3),(v7 - v5)));
        v3 = v21;
        v5 = v10;
      }
      if ((v7 < v10))
      {
        if ((v4 >= v3))
          v22 = (v3 + sub_80085BA4((v5 - v10),(v4 - v3),(v5 - v7)));
        else
          v22 = (v4 + sub_80085BA4((v10 - v7),(v3 - v4),(v5 - v7)));
        v4 = v22;
        v7 = v10;
      }
      if ((v11 < v5))
      {
        if ((v4 >= v3))
          v23 = (v3 + sub_80085BA4((v5 - v11),(v4 - v3),(v5 - v7)));
        else
          v23 = (v4 + sub_80085BA4((v11 - v7),(v3 - v4),(v5 - v7)));
        v3 = v23;
        v5 = v11;
      }
      if ((v11 < v7))
      {
        if ((v3 >= v4))
          v24 = (v4 + sub_80085BA4((v7 - v11),(v3 - v4),(v7 - v5)));
        else
          v24 = (v3 + sub_80085BA4((v11 - v5),(v4 - v3),(v7 - v5)));
        v4 = v24;
        v7 = v11;
      }
      v25 = (v4 - v3);
      if (((v4 - v3) >= 0))
      {
        v49 = 1;
      }
      else
      {
        v49 = -1;
        v25 = (v3 - v4);
      }
      v26 = (v7 - v5);
      if (((v7 - v5) >= 0))
      {
        v50 = 1;
      }
      else
      {
        v50 = -1;
        v26 = (v5 - v7);
      }
      v27 = ((v4 - v6) / v12);
      v28 = ((v3 - v6) / v12);
      v29 = ((v3 - v6) % v12);
      v16 = ((v5 - v10) / v12);
      v30 = ((v5 - v10) % v12);
      v31 = v51;
      v32 = ((v7 - v10) / v12);
      v33 = v29;
      v34 = ((sint16)(r_u16((((uint32)(v51))+(14)*2u))));
      v35 = v30;
      if ((v28 == v34))
      {
        --v28;
        v33 += v12;
        v31 = v51;
      }
      v36 = ((sint16)(r_u16((((uint32)(v31))+(15)*2u))));
      if ((v16 == v36))
      {
        --v16;
        v35 = (v30 + v12);
      }
      if ((v27 == v34))
        --v27;
      if ((v32 == v36))
        --v32;
      v53 = v26;
      v55 = v27;
      v57 = v25;
      v37 = sub_80085BA4(v25,v35,v12);
      v38 = v12;
      v39 = v37;
      v40 = sub_80085BA4(v53,v33,v38);
      v41 = v53;
      v42 = v55;
      v43 = v57;
      v13 = v28;
      if ((v3 >= v4))
        v44 = -v40;
      else
        v44 = (v40 - v53);
      if ((v5 >= v7))
        v45 = (v44 + v39);
      else
        v45 = ((v44 + v57) - v39);
      v46 = v45;
      while (((v13 != v42) || (v16 != v32)))
      {
        if ((v13 < 0))
          goto LABEL_104;
        v8 = v51;
        if (((v13 >= ((sint16)(r_u16((((uint32)(v51))+(14)*2u))))) || (v16 < 0)))
          break;
        if ((v16 >= ((sint16)(r_u16((((uint32)(v51))+(15)*2u))))))
          goto LABEL_99;
        v54 = v41;
        v56 = v42;
        v58 = v43;
        sub_8007BF2C(((uint32)(r_u32((0x800EDA30u+(((((20 * v13) + 8) + v16) + v52))*4u)))),((sint32)(a1)));
        v41 = v54;
        v42 = v56;
        v43 = v58;
        if ((v46 < 0))
        {
          v46 += v58;
          v16 += v50;
        }
        else
        {
          v46 -= v54;
          v13 += v49;
        }
      }

      if ((v13 < 0))
        goto LABEL_104;
      v8 = v51;
      LABEL_99:
      if (((v13 >= ((sint16)(r_u16((((uint32)(v8))+(14)*2u))))) || (v16 < 0)))
        goto LABEL_105;

      v47 = (v16 >= ((sint16)(r_u16((((uint32)(v8))+(15)*2u)))));
      v2 = (v8+(408)*4u);
      if (v47)
        goto LABEL_106;
    }
    v15 = v16;
    LABEL_103:
    sub_8007BF2C(((uint32)(r_u32((0x800EDA30u+(((((20 * v13) + 8) + v15) + v52))*4u)))),((sint32)(a1)));

    LABEL_104:
    v8 = v51;

    LABEL_105:
    v2 = (v8+(408)*4u);

    LABEL_106:
    v51 = v2;

    v52 += 408;
  }
  while ((((sint32)(v2)) < ((sint32)(0x800EE6F0u))));
  return sub_8007BFD4(((sint32)(a1)));
}
/* TODO Missing call adapter xport_draft_missing_gte_adapter */
uint32 sub_80084D4C(uint32 a1, uint32 a2, uint32 a3, uint32 a4)
{
  /* TODO Recover GTE register carriers and missing host GTE adapters */
  uint32 gte_T1 = 0u;
  uint32 gte_T2 = 0u;
  uint32 gte_T3 = 0u;
  uint32 gte_T4 = 0u;
  uint32 gte_T5 = 0u;
  uint32 gte_T6 = 0u;
  
  sint32 v4;
  sint32 v5;
  sint32 v6;
  sint32 v7;
  uint32 v8;
  sint32 result;
  sint32 v16;
  v4 = ((sint32)(r_u32(a4)));
  v5 = ((sint32)(r_u32((a4+(1)*4u))));
  v6 = ((sint32)(r_u32((a4+(2)*4u))));
  v7 = r_u32(((uint32)((a1 + 4))));
  v8 = ((uint32)((a1 + 32)));
  result = 1551;
  gte_T4 = (((sint32)(r_u32(v8))) + v4);
  gte_T5 = ((((sint32)(r_u32(v8))) >> 16) + v5);
  gte_T6 = (((sint32)(r_u32((v8+(1)*4u)))) + v6);
  do
  {
    (abort(),0u);
    v8 += (2)*4u;
    --v7;
    (abort(),0u);
    gte_T4 = (((sint32)(r_u32(v8))) + v4);
    gte_T5 = ((((sint32)(r_u32(v8))) >> 16) + v5);
    gte_T6 = (((sint32)(r_u32((v8+(1)*4u)))) + v6);
    (abort(),0u);
    v16 = ((((((4 * (gte_T1 < 0)) | (2 * (gte_T2 < 0))) | (gte_T3 < 0)) | (8 * (((sint32)(a3)) < gte_T3))) | ((gte_T1 > 0) << 10)) | ((gte_T2 > 0) << 9));
    w_u32(a2,(((unsigned short)(gte_T1)) | (gte_T2 << 16)));
    w_u32((a2+(1)*4u),gte_T3);
    a2 += (2)*4u;
    result &= v16;
    w_u16((((uint32)(a2))-(1)*2u),v16);
  }
  while (v7);
  return result;
}
/* TODO Missing call adapter xport_draft_missing_gte_adapter */
uint32 sub_80084E24(uint32 a1, uint32 a2, uint32 A2, uint32 a4)
{
  /* TODO Recover GTE register carriers and missing host GTE adapters */
  uint32 gte_A2 = 0u;
  uint32 gte_T0 = 0u;
  uint32 gte_T1 = 0u;
  uint32 gte_T2 = 0u;
  uint32 gte_T3 = 0u;
  uint32 gte_T4 = 0u;
  uint32 gte_T5 = 0u;
  uint32 gte_T6 = 0u;
  
  sint32 v5;
  sint32 v6;
  sint32 v7;
  sint32 v8;
  uint32 v9;
  uint32 v10;
  uint32 v11;
  sint32 v12;
  sint32 v13;
  sint32 v14;
  sint32 v15;
  uint32 v30;
  sint32 v36;
  uint32 result;
  gte_T0 = r_u32(((uint32)((gte_A2 + 68))));
  (abort(),0u);
  v5 = r_u32(0x800FF984u);
  v6 = r_u32(0x800FF988u);
  v7 = r_u32((a1+(3)*4u));
  v8 = ((sint32)((a1+(((2 * r_u32((a1+(1)*4u))) + 8))*4u)));
  v9 = ((uint32)((v8 + (8 * r_u32((a1+(2)*4u))))));
  do
  {
    v10 = r_u32((v9+(4)*4u));
    v11 = r_u32(v9);
    if (((((v10 & v6) == 0) && ((v10 | v5) == -1)) && ((((v10>>16)&65535u) & 3) != 1)))
    {
      v12 = (((unsigned short)(r_u32((v9+(1)*4u)))) + a2);
      v13 = (((r_u32((v9+(1)*4u))>>16)&65535u) + a2);
      v14 = (((unsigned short)(r_u32((v9+(2)*4u)))) + a2);
      v15 = (((r_u32((v9+(2)*4u))>>16)&65535u) + a2);
      if ((((((unsigned short)(((r_u16(((uint32)((v12 + 6)))) & r_u16(((uint32)((v13 + 6))))) & r_u16(((uint32)((v14 + 6))))))) & r_u16(((uint32)((v15 + 6))))) & 0x60F) == 0))
      {
        gte_T4 = r_u32(((uint32)(v12)));
        gte_T5 = r_u32(((uint32)(v13)));
        gte_T6 = r_u32(((uint32)(v14)));
        gte_T0 = (((sint32)(r_u32(((uint32)(v12))))) >> 16);
        gte_T1 = (((sint32)(r_u32(((uint32)(v13))))) >> 16);
        gte_T2 = (((sint32)(r_u32(((uint32)(v14))))) >> 16);
        (abort(),0u);
        if ((gte_T4 < 0))
        {
          if (((v11 & 0x10) != 0))
            goto LABEL_17;
          gte_T4 = r_u32(((uint32)(v15)));
          (abort(),0u);
          gte_T0 = (((sint32)(r_u32(((uint32)(v15))))) >> 16);
          (abort(),0u);
          gte_T5 = 0u-gte_T5;
          gte_T6 = 0u-gte_T6;
        }
        if (((gte_T5 | gte_T6) >= 0))
        {
          gte_T0 = (((unsigned short)(r_u32((v9+(4)*4u)))) + v8);
          (abort(),0u);
          v30 = ((uint32)((((unsigned short)(r_u32((v9+(1)*4u)))) + a2)));
          gte_T0 = ((sint32)(r_u32(v30)));
          gte_T1 = r_u16((((uint32)(v30))+(2)*2u));
          (abort(),0u);
          if ((gte_T0 <= 0))
          {
            v36 = (gte_T0 / gte_T3);
            if ((gte_T0 >= gte_T1))
            {
              if (((v10 & 0x20000) != 0))
              {
                if (r_u8(((uint32)((gte_A2 + 136)))))
                  w_u32(0x800FFA88u,r_u16(((uint32)((a4 + 22)))));
              }
              else
              {
                gte_T2 = (gte_T0 / gte_T3);
                if (((r_u32(((uint32)((gte_A2 + 64)))) - v36) > 0))
                {
                  w_u32(((uint32)((gte_A2 + 104))),a4);
                  w_u32(((uint32)((gte_A2 + 64))),v36);
                  w_u32(((uint32)((gte_A2 + 128))),v9);
                  w_u32(((uint32)((gte_A2 + 132))),gte_T1);
                  (abort(),0u);
                  gte_T0 = r_u32(((uint32)((gte_A2 + 84))));
                  gte_T1 = r_u32(((uint32)((gte_A2 + 88))));
                  (abort(),0u);
                  gte_T0 >>= 16;
                  (abort(),0u);
                  w_u32(0x800ED61Cu,(((unsigned short)(v10)) + v8));
                  w_u32(0x800ED618u,((sint32)(v9)));
                }
              }
            }
          }
        }
      }
    }
    LABEL_17:
    result = ((v11>>16)&65535u);

    v9 = ((uint32)((((uint32)(v9))+(result)*1u)));
    --v7;
  }
  while (v7);
  return result;
}
/* TODO Missing call adapter sub_800878DC */
/* TODO Missing call adapter xport_draft_missing_gte_adapter */
uint32 sub_8007BFD4(uint32 a1)
{
  /* TODO Recover GTE register carriers and missing host GTE adapters */
  uint32 gte_T4 = 0u;
  uint32 gte_T5 = 0u;
  uint32 gte_T6 = 0u;
  uint32 gte_V0 = 0u;
  
  sint32 v2;
  v2 = r_u32(((uint32)((a1 + 104))));
  if (!v2)
    return 0;
  sub_800858FC((v2 + 16),0x800ED720u);
  ((void)(0x800ED720u),abort(),0u);
  gte_V0 = r_u32(0x800ED61Cu);
  (abort(),0u);
  w_u16(((uint32)((a1 + 120))),gte_T4);
  w_u16(((uint32)((a1 + 122))),gte_T5);
  w_u16(((uint32)((a1 + 124))),gte_T6);
  return 1;
}
uint32 sub_8004CFDC(uint32 a1, uint32 a2)
{
  uint32 p, value, result, index;
  a2 &= 0xFFFFu;
  if (a2 - 8448u >= 67u) return 0u;
  switch (a2) {
  case 8501:
    p = (r_u32(a1 + 400u) + 3u) & 0xFFFFFFFCu;
    value = r_u32(p);
    w_u32(a1 + 400u, p + 4u);
    w_u32(a1 + 480u, value);
    return p + 4u;
  case 8495:
    p = (r_u32(a1 + 400u) + 3u) & 0xFFFFFFFCu;
    result = sub_8006E080(r_u32(p), r_u8(a1 + 27u));
    w_u16(a1 + 22u, result);
    w_u32(a1 + 400u, p + 4u);
    return result;
  case 8512: case 8513: case 8514:
    result = (uint32)((sint32)(sub_8004CF78(a1) << 16) >> 4);
    w_u32(a1 + 4u + (a2 - 8512u) * 4u, result);
    return result;
  case 8497:
    p = r_u32(a1 + 400u);
    value = r_u16(p);
    w_u32(a1 + 400u, p + 2u);
    result = r_u16(a1);
    result = value ? result | 8u : result & 0xFFF7u;
    w_u16(a1, result);
    return result;
  case 8480:
    p = r_u32(a1 + 400u);
    index = r_u16(p) & 0xFFu;
    w_u32(a1 + 400u, p + 2u);
    result = sub_8004CF78(a1);
    w_u16(a1 + 460u + index * 2u, result);
    return result;
  case 8485:
    p = r_u32(a1 + 400u);
    index = r_u16(p);
    p += 2u;
    w_u32(a1 + 400u, p);
    value = r_u16(p);
    w_u16(a1 + 436u + index * 2u, value);
    w_u32(a1 + 400u, p + 2u);
    return p + 2u;
  case 8486:
    p = r_u32(a1 + 400u);
    index = r_u16(p) & 0xFFu;
    p += 2u;
    w_u32(a1 + 400u, p);
    value = r_u16(p) & 0xFFu;
    w_u32(a1 + 400u, p + 2u);
    result = r_u16(p + 2u);
    w_u32(a1 + 400u, p + 4u);
    return sub_8004BDCC(a1, index, value, result);
  case 8448: case 8449: case 8468: case 8469:
  case 8481: case 8482: case 8492: case 8493: case 8494:
    p = r_u32(a1 + 400u);
    value = (a2 == 8492u || a2 == 8493u) ? r_u8(p) : r_u16(p);
    w_u32(a1 + 400u, p + 2u);
    switch (a2) {
    case 8448: w_u16(a1 + 218u, value); break;
    case 8449: w_u16(a1 + 212u, value); break;
    case 8468: w_u16(a1 + 472u, value); break;
    case 8469: w_u16(a1 + 474u, value); break;
    case 8481: w_u32(a1 + 304u, (uint32)(sint32)(sint16)value); break;
    case 8482: w_u16(a1 + 456u, value); break;
    case 8492: w_u8(a1 + 384u, value); break;
    case 8493: w_u8(a1 + 383u, value); break;
    case 8494: w_u16(a1 + 392u, value); break;
    }
    return p + 2u;
  default:
    return 0x8004D2A0u;
  }
}
/* TODO Missing call adapter indirect */
uint32 sub_8004CF78(uint32 a1)
{
  uint32 v1;
  unsigned short v2;
  v1 = r_u32(((uint32)((a1 + 400))));
  v2 = r_u16(v1);
  w_u32(((uint32)((a1 + 400))),(v1+(1)*2u));
  if ((((v2 & 0x8000) == 0) && ((v2 & 0x2000) != 0)))
    return ((void)((a1 + ((sint16)(r_u16(((uint32)((r_u32(((uint32)((a1 + 68)))) + 80)))))))),(void)(v2),abort(),0u);
  return v2;
}
/* TODO Missing call adapter HIDWORD */
/* TODO Missing call adapter LODWORD */
/* TODO Missing call adapter SHIDWORD */
/* TODO Missing call adapter indirect */
/* TODO Missing call adapter sub_8001DB68 */
/* TODO Missing call adapter sub_8002005C */
/* TODO Missing call adapter sub_8002EF58 */
/* TODO Missing call adapter indirect */
/* TODO Missing call adapter sub_8001DB68 */
/* TODO Missing call adapter sub_8002005C */
/* TODO Missing call adapter sub_8002EF58 */
uint32 sub_8004BF3C(uint32 a1, uint32 a2)
{
  sint32 result;
  sint32 v5;
  uint32 v6;
  unsigned short v7;
  uint32 v8;
  unsigned short v9;
  unsigned short v10;
  sint32 v11;
  sint32 v12;
  uint32 v13;
  sint32 v14;
  uint32 v15;
  unsigned short v16;
  sint32 v17;
  uint32 v18;
  uint32 v19;
  unsigned short v20;
  sint32 v21;
  short v22;
  uint32 v23;
  unsigned short v24;
  sint32 v25;
  short v26;
  uint32 v27;
  short v28;
  sint32 v29;
  uint32 v30;
  uint32 v31;
  unsigned short v32;
  sint32 v33;
  sint32 v34;
  uint32 v35;
  unsigned short v36;
  uint32 v37;
  unsigned short v38;
  uint32 v39;
  unsigned short v40;
  unsigned short v41;
  short v42;
  uint32 v43;
  short v44;
  uint32 v45;
  sint32 v46;
  uint32 v47;
  uint32 v48;
  sint32 v49;
  uint32 v50;
  unsigned short v51;
  sint32 v52;
  uint32 v53;
  sint32 v54;
  uint32 v55;
  sint32 v57;
  uint32 v58;
  sint32 v59;
  uint32 v60;
  uint32 v62;
  unsigned short v63;
  uint32 v64;
  unsigned short v65;
  sint32 v66;
  uint32 v67;
  sint32 v68;
  unsigned short v69;
  uint32 v70;
  uint32 v71;
  unsigned short v72;
  uint32 v73;
  unsigned short v74;
  uint32 v75;
  unsigned short v76;
  uint32 v77;
  unsigned short v78;
  uint32 v79;
  sint32 v80;
  sint32 v81;
  uint32 v82;
  short v83;
  short v84;
  uint32 v85;
  short v86;
  uint32 v87;
  short v88;
  sint32 v89;
  short v90;
  sint32 v91;
  sint32 v92;
  uint32 v93;
  sint32 v94;
  uint32 v95;
  sint32 v96;
  sint32 v97;
  uint32 v98;
  sint32 v99;
  uint32 v100;
  sint32 v101;
  sint32 v102;
  uint32 v103;
  sint32 v104;
  uint32 v105;
  sint32 v106;
  uint32 v107;
  short v108;
  uint32 v109;
  short v110;
  short v111;
  uint32 v112;
  unsigned short v113;
  unsigned short v114;
  uint32 v115;
  unsigned short v116;
  uint32 v117;
  short v118;
  uint32 v119;
  unsigned short v120;
  unsigned short v121;
  sint32 v122;
  uint32 v123;
  unsigned short v124;
  unsigned short v125;
  sint32 v126;
  uint32 v127;
  short v128;
  uint32 v129;
  unsigned short v130;
  sint32 v131;
  sint32 v132;
  sint32 v133;
  long long v134;
  uint32 v135;
  sint32 v136;
  char v137[16];
  if ((a2 == 17024))
  {
    (v107 = ((sint32)(r_u32(((uint32)((a1 + 400)))))));
    (v108 = ((sint16)(r_u16(v107))));
    w_u32(((uint32)((a1 + 400))),(v107+(1)*2u));
    w_u16(((uint32)((a1 + 476))),v108);
    (result = 0);
    if (((v108 & 0x2000) == 0))
      return result;
    w_u16(((uint32)((a1 + 476))),((void)((a1 + ((sint16)(r_u16(((uint32)((r_u32(((uint32)((a1 + 68)))) + 80)))))))),(void)(r_u16(((uint32)((a1 + 476))))),abort(),0u));
    return 0;
  }
  if ((a2 < 0x4281u))
  {
    if ((a2 == 16662))
    {
      (v75 = r_u32(((uint32)((a1 + 400)))));
      (v76 = r_u16(v75));
      w_u32(((uint32)((a1 + 400))),(v75+(1)*2u));
      (result = 1);
      if (((r_u32(((uint32)((a1 + 396)))) & v76) == 0))
        return result;
      goto LABEL_183;
    }
    if ((a2 < 0x4117u))
    {
      if ((a2 != 16647))
      {
        if ((a2 < 0x4108u))
        {
          if ((a2 == 16644))
          {
            (v64 = r_u32(((uint32)((a1 + 400)))));
            (v65 = r_u32((v64+=2u,v64-2u)));
            w_u32(((uint32)((a1 + 400))),v64);
            w_u32(((uint32)(((a1 + (4 * v65)) + 404))),v64);
            return 1;
          }
          if ((a2 < 0x4105u))
          {
            if ((a2 != 16641))
            {
              (result = 1);
              if ((a2 == 16642))
              {
                (v73 = r_u32(((uint32)((a1 + 400)))));
                (v74 = r_u16(v73));
                w_u32(((uint32)((a1 + 400))),(v73+(1)*2u));
                (result = 0);
                w_u32(((uint32)((a1 + 400))),r_u32(((uint32)(((a1 + (4 * v74)) + 404)))));
              }
              return result;
            }
            (v71 = r_u32(((uint32)((a1 + 400)))));
            (v72 = r_u16(v71));
            w_u32(((uint32)((a1 + 400))),(v71+(1)*2u));
            w_u32(((uint32)((a1 + 400))),r_u32(((uint32)(((a1 + (4 * v72)) + 404)))));
            return 1;
          }
          if ((a2 == 16645))
          {
            (v66 = r_u32(((uint32)((a1 + 400)))));
            while (1)
            {
              (v67 = r_u32(((uint32)((a1 + 400)))));
              (v68 = r_u16(v67));
              if ((v68 == 16640))
                break;
              w_u32(((uint32)((a1 + 400))),(v67+(1)*2u));
              if ((v68 == 16644))
              {
                (v69 = r_u16((v67+(1)*2u)));
                (v70 = (v67+(2)*2u));
                w_u32(((uint32)((a1 + 400))),v70);
                w_u32(((uint32)(((a1 + (4 * v69)) + 404))),v70);
              }
            }

            w_u32(((uint32)((a1 + 400))),v66);
            return 1;
          }
          (result = 1);
          if ((a2 != 16646))
            return result;
          (v62 = r_u32(((uint32)((a1 + 400)))));
          (v63 = r_u16(v62));
          w_u32(((uint32)((a1 + 400))),(v62+(1)*2u));
          if (((v63 & 0x2000) != 0))
            (v63 = ((void)((a1 + ((sint16)(r_u16(((uint32)((r_u32(((uint32)((a1 + 68)))) + 80)))))))),(void)(v63),abort(),0u));
          (v57 = (sub_8006613C(&v134,v63) + 6));
          LABEL_209:
          w_u32(((uint32)((a1 + 400))),v57);

          return 1;
        }
        if ((a2 == 16658))
        {
          (v93 = r_u32(((uint32)((a1 + 400)))));
          (v94 = r_u16(v93));
          w_u32(((uint32)((a1 + 400))),(v93+(1)*2u));
          if (((v94 & 0x2000) != 0))
            (v94 = ((void)((a1 + ((sint16)(r_u16(((uint32)((r_u32(((uint32)((a1 + 68)))) + 80)))))))),(void)(v94),abort(),0u));
          (v95 = r_u32(((uint32)((a1 + 400)))));
          (v96 = r_u16(v95));
          w_u32(((uint32)((a1 + 400))),(v95+(1)*2u));
          (v97 = (v96 < v94));
          if (((v96 & 0x2000) != 0))
            (v97 = (((void)((a1 + ((sint16)(r_u16(((uint32)((r_u32(((uint32)((a1 + 68)))) + 80)))))))),abort(),0u) < v94));
          (v33 = v97);
          (result = 1);
          if (!v33)
            goto LABEL_183;
        }
        else
        {
          if ((a2 < 0x4113u))
          {
            (result = 1);
            if ((a2 < 0x4110u))
              return result;
            (v5 = 0);
            (v79 = r_u32(((uint32)((a1 + 400)))));
            (v80 = r_u32(((uint32)((a1 + 68)))));
            (v81 = r_u16(v79));
            (v82 = ((uint32)((v79+(1)*2u))));
            w_u32(((uint32)((a1 + 400))),(v79+(1)*2u));
            (v83 = ((void)((a1 + ((sint16)(r_u16(((uint32)((v80 + 80)))))))),(void)(v81),abort(),0u));
            (v84 = v83);
            if ((v82 != ((sint32)(r_u32(((uint32)((a1 + 400))))))))
            {
              (v85 = v137);
              do
              {
                (v86 = r_u32((v82+=2u,v82-2u)));
                ++v5;
                w_u16(((uint32)(v85)),v86);
                (v85 += (2)*1u);
              }
              while ((v82 != ((sint32)(r_u32(((uint32)((a1 + 400))))))));
            }
            (v87 = ((sint32)(r_u32(((uint32)((a1 + 400)))))));
            (v88 = ((sint16)(r_u16(v87))));
            w_u32(((uint32)((a1 + 400))),(v87+(1)*2u));
            (v89 = (2 * v5));
            if (((v88 & 0x2000) != 0))
            {
              (v88 = ((void)((a1 + ((sint16)(r_u16(((uint32)((r_u32(((uint32)((a1 + 68)))) + 80)))))))),abort(),0u));
              (v89 = (2 * v5));
            }
            (v90 = (v84 - v88));
            if ((a2 == 16656))
              (v90 = (v84 + v88));
            w_u16(((uint32)(&v137[v89])),v90);
            (v91 = r_u32(((uint32)((a1 + 400)))));
            (v92 = r_u32(((uint32)((a1 + 68)))));
            w_u32(((uint32)((a1 + 400))),v137);
            ((void)((a1 + ((sint16)(r_u16(((uint32)((v92 + 72)))))))),(void)(v81),abort(),0u);
            w_u32(((uint32)((a1 + 400))),v91);
            return 1;
          }
          if ((a2 == 16660))
          {
            (v103 = r_u32(((uint32)((a1 + 400)))));
            (v104 = r_u16(v103));
            w_u32(((uint32)((a1 + 400))),(v103+(1)*2u));
            if (((v104 & 0x2000) != 0))
              (v104 = ((void)((a1 + ((sint16)(r_u16(((uint32)((r_u32(((uint32)((a1 + 68)))) + 80)))))))),(void)(v104),abort(),0u));
            (v105 = r_u32(((uint32)((a1 + 400)))));
            (v106 = r_u16(v105));
            w_u32(((uint32)((a1 + 400))),(v105+(1)*2u));
            if (((v106 & 0x2000) != 0))
              (v106 = ((void)((a1 + ((sint16)(r_u16(((uint32)((r_u32(((uint32)((a1 + 68)))) + 80)))))))),abort(),0u));
            (result = 1);
            if ((v104 != v106))
              goto LABEL_183;
          }
          else
          {
            if ((a2 >= 0x4115u))
            {
              (v77 = r_u32(((uint32)((a1 + 400)))));
              (v78 = r_u16(v77));
              w_u32(((uint32)((a1 + 400))),(v77+(1)*2u));
              (result = 1);
              if (((r_u32(((uint32)((a1 + 396)))) & v78) == v78))
                return result;
              goto LABEL_183;
            }
            (v98 = r_u32(((uint32)((a1 + 400)))));
            (v99 = r_u16(v98));
            w_u32(((uint32)((a1 + 400))),(v98+(1)*2u));
            if (((v99 & 0x2000) != 0))
              (v99 = ((void)((a1 + ((sint16)(r_u16(((uint32)((r_u32(((uint32)((a1 + 68)))) + 80)))))))),(void)(v99),abort(),0u));
            (v100 = r_u32(((uint32)((a1 + 400)))));
            (v101 = r_u16(v100));
            w_u32(((uint32)((a1 + 400))),(v100+(1)*2u));
            (v102 = (v99 < v101));
            if (((v101 & 0x2000) != 0))
              (v102 = (v99 < ((void)((a1 + ((sint16)(r_u16(((uint32)((r_u32(((uint32)((a1 + 68)))) + 80)))))))),abort(),0u)));
            (v33 = v102);
            (result = 1);
            if (!v33)
            {
              LABEL_183:
              sub_8004BE30(a1);

              return 1;
            }
          }
        }
        return result;
      }
      return 0;
    }
    if ((a2 == 16900))
    {
      w_u16(((uint32)(a1)),(r_u16(((uint32)(a1)))|(1u)));
      return 1;
    }
    if ((a2 < 0x4205u))
    {
      if ((a2 == 16897))
      {
        (v112 = r_u32(((uint32)((a1 + 400)))));
        (v113 = r_u32((v112+=2u,v112-2u)));
        w_u32(((uint32)((a1 + 400))),v112);
        (v114 = r_u16(v112));
        w_u32(((uint32)((a1 + 400))),(v112+(1)*2u));
        sub_80063118(a1,v113,((sint8)(v114)));
        return 1;
      }
      if ((a2 < 0x4202u))
      {
        if ((a2 == 16672))
          return 1;
        (result = 1);
        if ((a2 != 16896))
          return result;
        sub_800626C8(a1,r_u32(((uint32)((a1 + 400)))));
        (v58 = r_u32(((uint32)((a1 + 400)))));
        (v59 = (((unsigned char)(v58)) & 1));
        if (r_u8(v58))
        {
          (v60 = (v58+(1)*1u));
          while (r_u32((v60+=1u,v60-1u)))
            ;

          (v58 = (v60-(1)*1u));
          (v59 = (((unsigned char)(v58)) & 1));
        }
        (v33 = (v59 != 0));
        (v57 = ((sint32)((v58+(1)*1u))));
        if (!v33)
          (v57 = ((sint32)((v58+(2)*1u))));
        goto LABEL_209;
      }
      if ((a2 == 16898))
      {
        (v115 = r_u32(((uint32)((a1 + 400)))));
        (v116 = r_u16(v115));
        w_u32(((uint32)((a1 + 400))),(v115+(1)*2u));
        sub_80063038(a1,v116,0,-1);
        return 1;
      }
      (result = 1);
      if ((a2 == 16899))
      {
        w_u16(((uint32)(a1)),(r_u16(((uint32)(a1)))&(~1u)));
        return 1;
      }
      return result;
    }
    if ((a2 == 16935))
    {
      (v27 = ((sint32)(r_u32(((uint32)((a1 + 400)))))));
      (v28 = ((sint16)(r_u16(v27))));
      w_u32(((uint32)((a1 + 400))),(v27+(1)*2u));
      if (v28)
      {
        (v29 = r_u32(((uint32)((a1 + 396)))));
        w_u8(((uint32)((a1 + 386))),v28);
        w_u8(((uint32)((a1 + 387))),0);
        (v30 = (v29 | 2));
      }
      else
      {
        (v30 = (r_u32(((uint32)((a1 + 396)))) & 0xFFFFFFFD));
      }
      w_u32(((uint32)((a1 + 396))),v30);
      return 1;
    }
    if ((a2 < 0x4228u))
    {
      if ((a2 == 16901))
      {
        ((void)((a1 + ((sint16)(r_u16(((uint32)((r_u32(((uint32)((a1 + 68)))) + 16)))))))),abort(),0u);
        return 1;
      }
      (result = 1);
      if ((a2 == 16934))
      {
        w_u32(((uint32)((a1 + 112))),0);
        w_u32(((uint32)((a1 + 108))),0);
        w_u32(((uint32)((a1 + 104))),0);
        return 1;
      }
      return result;
    }
    if ((a2 == 16992))
    {
      (v109 = ((sint32)(r_u32(((uint32)((a1 + 400)))))));
      (v110 = 1);
    }
    else
    {
      if ((a2 < 0x4261u))
      {
        (result = 1);
        if ((a2 != 16960))
          return result;
        w_u8(((uint32)((a1 + 380))),0);
        return 0;
      }
      (result = 1);
      if ((a2 != 16993))
        return result;
      (v109 = ((sint32)(r_u32(((uint32)((a1 + 400)))))));
      (v110 = 0);
    }
    (v111 = ((sint16)(r_u16(v109))));
    w_u32(((uint32)((a1 + 400))),(v109+(1)*2u));
    sub_8004BDCC(a1,v111,0,v110);
    return 1;
  }
  if ((a2 == 17052))
  {
    sub_8001BE78(r_u8(r_u32(((uint32)((a1 + 400))))),r_u8(((uint32)((r_u32(((uint32)((a1 + 400)))) + 2)))),r_u8(((uint32)((r_u32(((uint32)((a1 + 400)))) + 4)))),r_u16(((uint32)((r_u32(((uint32)((a1 + 400)))) + 6)))),0,r_u16(((uint32)((r_u32(((uint32)((a1 + 400)))) + 8)))));
    (v57 = (r_u32(((uint32)((a1 + 400)))) + 10));
    goto LABEL_209;
  }
  if ((a2 < 0x429Du))
  {
    if ((a2 == 17044))
    {
      sub_8002E6D8(((uint32)((a1 + 4))));
      (v46 = 0);
      (v47 = ((uint32)(sub_80066088(r_u16(((uint32)((a1 + 214))))))));
      (v48 = v47);
      while ((v46 < r_u16(v47)))
      {
        (v49 = r_u16((v48+(1)*2u)));
        (v48+=2u);
        ++v46;
        sub_8006613C(&v134,v49);
        sub_8002E6D8(((uint32)((a1 + 4))));
      }

      sub_8002EF68();
      return 1;
    }
    if ((a2 < 0x4295u))
    {
      if ((a2 == 17041))
      {
        (v126 = (a1 + 4));
        (v127 = ((sint32)(r_u32(((uint32)((a1 + 400)))))));
        (v128 = ((sint16)(r_u16(v127))));
        w_u32(((uint32)((a1 + 400))),(v127+(1)*2u));
        sub_80069EF4(v128,v126,0);
        return 1;
      }
      if ((a2 >= 0x4292u))
      {
        if ((a2 == 17042))
        {
          (v129 = r_u32(((uint32)((a1 + 400)))));
          (v130 = r_u16(v129));
          (v33 = (r_u32(0x800FF3A8u) >= 200));
          w_u32(((uint32)((a1 + 400))),(v129+(1)*2u));
          if (!v33)
          {
            sub_80034F9C((a1 + 16));
            (v134 = ((v134&0xFFFF0000u)|(((512)&0xFFFFu)<<0)));
            w_u32(((uint32)((((uint32)(&v134))+(2)*1u))),4096);
            sub_80034FC4(((sint32)(&v134)));
            sub_800350E8(1);
            sub_800350FC(128,128,128);
            sub_80035110(4,4,4);
            w_u32(0x800FF3ACu,0);
            (v131 = 0);
            if (v130)
            {
              do
              {
                (v132 = sub_80032DC0(88));
                if (v132)
                  sub_80035124(v132,((uint32)((a1 + 4))),32,0x2000,32);
                ++v131;
              }
              while ((v131 < v130));
            }
            w_u32(0x800FF3ACu,1);
          }
        }
        else
        {
          (result = 1);
          if ((a2 != 17043))
            return result;
          (v45 = ((uint32)(((r_u32(((uint32)((a1 + 400)))) + 3) & 0xFFFFFFFC))));
          sub_8002E600(((sint32)(r_u32(v45))));
          w_u32(((uint32)((a1 + 400))),(v45+(1)*4u));
        }
        return 1;
      }
      if ((a2 == 17025))
      {
        (result = 1);
        if (((r_u16(((uint32)((a1 + 388)))) & 1) == 0))
        {
          (result = 0);
          w_u32(((uint32)((a1 + 400))),(r_u32(((uint32)((a1 + 400))))-(2)));
        }
      }
      else
      {
        (result = 1);
        if ((a2 == 17040))
        {
          (v117 = ((sint32)(r_u32(((uint32)((a1 + 400)))))));
          (v118 = ((sint16)(r_u16(v117))));
          w_u32(((uint32)((a1 + 400))),(v117+(1)*2u));
          sub_80069DF0(v118,0x2000,0);
          return 1;
        }
      }
      return result;
    }
    if ((a2 == 17047))
    {
      (v37 = r_u32(((uint32)((a1 + 400)))));
      (v38 = r_u16(v37));
      w_u32(((uint32)((a1 + 400))),(v37+(1)*2u));
      sub_8006A3C4();
      return 1;
    }
    if ((a2 < 0x4298u))
    {
      if ((a2 == 17045))
      {
        sub_8002F130();
        return 1;
      }
      else
      {
        (result = 1);
        if ((a2 == 17046))
        {
          (v35 = r_u32(((uint32)((a1 + 400)))));
          (v36 = r_u16(v35));
          w_u32(((uint32)((a1 + 400))),(v35+(1)*2u));
          sub_8006A428();
          return 1;
        }
      }
      return result;
    }
    if ((a2 == 17049))
    {
      (v43 = ((sint32)(r_u32(((uint32)((a1 + 400)))))));
      (v44 = ((sint16)(r_u16(v43))));
      w_u32(((uint32)((a1 + 400))),(v43+(1)*2u));
      sub_8002EE7C(v44);
      return 1;
    }
    if ((a2 < 0x4299u))
    {
      (v31 = r_u32(((uint32)((a1 + 400)))));
      (v32 = r_u16(v31));
      w_u32(((uint32)((a1 + 400))),(v31+(1)*2u));
      if (((v32 & 0x2000) != 0))
        (v32 = ((void)((a1 + ((sint16)(r_u16(((uint32)((r_u32(((uint32)((a1 + 68)))) + 80)))))))),(void)(v32),abort(),0u));
      if (!r_u32(0x800FF904u))
        return 1;
      if (v32)
      {
        (v33 = (v32 != 1));
        (v34 = (a1 + 4));
        if (v33)
          sub_800774EC(r_u32(0x800FF904u),v34,0);
        else
          sub_800774EC(r_u32(0x800FF904u),v34,1);
        return 1;
      }
      else
      {
        sub_800774EC(r_u32(0x800FF904u),(a1 + 4),2);
        return 1;
      }
    }
    (result = 1);
    if ((a2 != 17050))
      return result;
    LABEL_88:
    if ((a2 == 17050))
    {
      (v15 = r_u32(((uint32)((a1 + 400)))));
      (v16 = r_u16(v15));
      w_u32(((uint32)((a1 + 400))),(v15+(1)*2u));
      if (((v16 & 0x2000) != 0))
        (v16 = ((void)((a1 + ((sint16)(r_u16(((uint32)((r_u32(((uint32)((a1 + 68)))) + 80)))))))),(void)(v16),abort(),0u));
      sub_8006613C(&v134,v16);
    }
    else
    {
      (v17 = r_u32(((uint32)((a1 + 8)))));
      (v18 = r_u32(((uint32)((a1 + 12)))));
      (v134 = (((unsigned long long)(v134)&0xFFFFFFFF00000000ULL)|(((unsigned long long)(r_u32(((uint32)((a1 + 4)))))&0xFFFFFFFFULL)<<0)));
      (v134 = (((unsigned long long)(v134)&0xFFFFFFFFULL)|(((unsigned long long)(v17)&0xFFFFFFFFULL)<<32)));
      (v135 = v18);
    }

    (v19 = r_u32(((uint32)((a1 + 400)))));
    (v20 = r_u16(v19));
    (v21 = r_u16(v19));
    w_u32(((uint32)((a1 + 400))),(v19+(1)*2u));
    if ((v20 < 2u))
    {
      sub_8001E1B8(((uint32)((a1 + 4))),v21);
      return 1;
    }
    (result = 1);
    if ((v21 == 2))
    {
      (v22 = sub_8004CF78(a1));
      (v23 = ((uint32)((r_u32(((uint32)((a1 + 400)))) + 2))));
      w_u32(((uint32)((a1 + 400))),v23);
      (v24 = r_u16(v23));
      w_u32(((uint32)((a1 + 400))),(v23+(1)*2u));
      (v26 = v22);
      (v25 = sub_80032DC0(156));
      if (v25)
      {
        ((void)(v25),(void)(&v134),(void)(v26),(void)((v24 != 0)),(void)(v24),abort(),0u);
        return 1;
      }
      return 1;
    }
    return result;
  }
  if ((a2 == 17060))
  {
    (v39 = ((r_u32(((uint32)((a1 + 400)))) + 3) & 0xFFFFFFFC));
    w_u32(((uint32)((a1 + 400))),(v39 + 4));
    (v40 = r_u16(((uint32)((v39 + 4)))));
    w_u32(((uint32)((a1 + 400))),(v39 + 6));
    (v41 = r_u16(((uint32)((v39 + 6)))));
    w_u32(((uint32)((a1 + 400))),(v39 + 8));
    (v42 = r_u16(((uint32)((v39 + 8)))));
    w_u32(((uint32)((a1 + 400))),(v39 + 10));
    sub_8002E148(r_u32(((uint32)(v39))),v40,v41,v42);
    return 1;
  }
  if ((a2 < 0x42A5u))
  {
    if ((a2 == 17056))
    {
      (v6 = r_u32(((uint32)((a1 + 400)))));
      (v7 = r_u16(v6));
      w_u32(((uint32)((a1 + 400))),(v6+(1)*2u));
      if (((v7 & 0x2000) != 0))
        (v7 = ((void)((a1 + ((sint16)(r_u16(((uint32)((r_u32(((uint32)((a1 + 68)))) + 80)))))))),(void)(v7),abort(),0u));
      (v8 = r_u32(((uint32)((a1 + 400)))));
      (v9 = r_u32((v8+=2u,v8-2u)));
      w_u32(((uint32)((a1 + 400))),v8);
      (v10 = r_u16(v8));
      w_u32(((uint32)((a1 + 400))),(v8+(1)*2u));
      (v11 = r_u16((v8+(1)*2u)));
      w_u32(((uint32)((a1 + 400))),(v8+(2)*2u));
      (v12 = sub_80020EF8(v7,((uint32)((a1 + 4))),v9,v10,((sint32)(0x800A71CCu))));
      if (v12)
      {
        ((void)(v12),(void)((v11 << 12)),(void)(5),abort(),0u);
        return 1;
      }
      return 1;
    }
    if ((a2 >= 0x42A1u))
    {
      if ((a2 == 17058))
      {
        (v13 = r_u32(((uint32)((a1 + 400)))));
        (v14 = r_u16(v13));
        w_u32(((uint32)((a1 + 400))),(v13+(1)*2u));
        w_u32(0x800FF380u,v14);
        return 1;
      }
      else
      {
        (result = 1);
        if ((a2 == 17059))
        {
          (v119 = r_u32(((uint32)((a1 + 400)))));
          (v120 = r_u32((v119+=2u,v119-2u)));
          w_u32(((uint32)((a1 + 400))),v119);
          (v121 = r_u16(v119));
          w_u32(((uint32)((a1 + 400))),(v119+(1)*2u));
          sub_8002FD2C(v120,v121);
          return 1;
        }
      }
    }
    else
      if ((a2 == 17053))
    {
      sub_8002E6D8(((uint32)((a1 + 4))));
      (abort(),0u);
      return 1;
    }
    else
    {
      (result = 1);
      if ((a2 == 17054))
      {
        w_u32(0x800FF37Cu,r_u32(((uint32)((a1 + 8)))));
        return 1;
      }
    }
    return result;
  }
  if ((a2 == 17072))
  {
    (v53 = r_u32(((uint32)((a1 + 400)))));
    (v54 = (((unsigned char)(v53)) & 1));
    if (r_u8(v53))
    {
      (v55 = (v53+(1)*1u));
      while (r_u32((v55+=1u,v55-1u)))
        ;

      (v53 = (v55-(1)*1u));
      (v54 = (((unsigned char)(v53)) & 1));
    }
    (v33 = (v54 != 0));
    (v57 = ((sint32)((v53+(1)*1u))));
    if (!v33)
      (v57 = ((sint32)((v53+(2)*1u))));
    goto LABEL_209;
  }
  if ((a2 >= 0x42B1u))
  {
    if ((a2 < 0x42B3u))
    {
      (v50 = r_u32(((uint32)((a1 + 400)))));
      (v51 = r_u16(v50));
      w_u32(((uint32)((a1 + 400))),(v50+(1)*2u));
      if (((v51 & 0x2000) != 0))
        (v51 = ((void)((a1 + ((sint16)(r_u16(((uint32)((r_u32(((uint32)((a1 + 68)))) + 80)))))))),(void)(v51),abort(),0u));
      (v52 = sub_80066088(v51));
      if ((a2 == 17073))
        sub_80064E50(v52);
      else
        sub_80064A08(v52);
      return 1;
    }
    else
    {
      (result = 1);
      if ((a2 == 17674))
      {
        (v122 = (a1 + 4));
        (v123 = r_u32(((uint32)((a1 + 400)))));
        (v124 = r_u32((v123+=2u,v123-2u)));
        w_u32(((uint32)((a1 + 400))),v123);
        (v125 = r_u16(v123));
        w_u32(((uint32)((a1 + 400))),(v123+(1)*2u));
        sub_8002FC64(v124,v125,v122,0);
        return 1;
      }
    }
    return result;
  }
  if ((a2 == 17061))
  {
    sub_8002E2B8();
    return 1;
  }
  (result = 1);
  if ((a2 == 17062))
    goto LABEL_88;
  return result;
}


uint32 sub_8004D604(uint32 a1, uint32 a2, uint32 a3)
{
  sint32 v6;
  short v7;
  short v8;
  sint32 result;
  sub_8004B800(a1);
  w_u32(((uint32)((a1 + 68))),0x800A2AC0u);
  v6 = sub_80062B0C(a1,a2);
  w_u32(((uint32)((a1 + 400))),sub_80062B50(a1,v6));
  w_u8(((uint32)((a1 + 380))),1);
  v7 = r_u16(((uint32)(a1)));
  w_u32(((uint32)((a1 + 516))),-1);
  v8 = r_u16(((uint32)((a1 + 78))));
  w_u16(((uint32)((a1 + 214))),a3);
  w_u16(((uint32)(a1)),(v7 | 1));
  w_u16(((uint32)((a1 + 78))),(v8 & 0xFFEF));
  sub_80062A38(a1,0x800FF4E8u);
  result = a1;
  w_u16(((uint32)((a1 + 212))),0);
  w_u16(((uint32)((a1 + 58))),203);
  return result;
}
/* TODO Missing call adapter sub_80087A3C */
uint32 sub_800620B8(uint32 a1, uint32 a2, uint32 a3)
{
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
  sint32 v16;
  sint32 v17;
  sint32 v18;
  sint32 v19;
  short v20;
  sint32 v21;
  sint32 v22;
  sub_80062924(a1);
  w_u32((((uint32)(a1))+(17)*4u),0x800A31E4u);
  v6 = ((sint16)(r_u16((a1+(39)*2u))));
  v7 = ((sint16)(r_u16(a1)));
  w_u16((a1+(106)*2u),0);
  w_u16((a1+(107)*2u),a3);
  w_u16((a1+(39)*2u),(v6 & 0xFFEF));
  w_u16(a1,(v7 | 1));
  sub_80062A38(a1,0x800FF4E8u);
  v8 = ((uint32)(sub_80062B0C(a1,a2)));
  w_u16((a1+(8)*2u),((sint16)(r_u16(v8))));
  w_u16((a1+(9)*2u),((sint16)(r_u16((v8+(1)*2u)))));
  w_u16((a1+(10)*2u),((sint16)(r_u16((v8+(2)*2u)))));
  v9 = sub_80066088(a3);
  sub_8006613C((a1+(162)*2u),r_u16(((uint32)((v9 + 2)))));
  v10 = ((uint32)(((((uint32)(v8)) + 9) & 0xFFFFFFFC)));
  v11 = r_u32((v10+=4u,v10-4u));
  v12 = r_u32((((uint32)(a1))+(83)*4u));
  v13 = r_u32((((uint32)(a1))+(3)*4u));
  v14 = r_u32((((uint32)(a1))+(81)*4u));
  w_u32((((uint32)(a1))+(74)*4u),(v11 << 12));
  v15 = r_u32((v10+=4u,v10-4u));
  v16 = ((v12 - v13) >> 12);
  v17 = r_u32((((uint32)(a1))+(1)*4u));
  w_u32((((uint32)(a1))+(75)*4u),(v15 << 12));
  v18 = r_u32((v10+=4u,v10-4u));
  w_u32((((uint32)(a1))+(76)*4u),(v18 << 12));
  v19 = r_u32((v10+=4u,v10-4u));
  w_u32((((uint32)(a1))+(77)*4u),(v19 << 12));
  w_u32((((uint32)(a1))+(78)*4u),(((sint32)(r_u32(v10))) << 12));
  w_u32((((uint32)(a1))+(79)*4u),(((sint32)(r_u32((v10+(1)*4u)))) << 12));
  v20 = ((void)(v16),(void)(((v14 - v17) >> 12)),abort(),0u);
  v21 = ((sint16)(r_u16((a1+(9)*2u))));
  v22 = ((3072 - v20) & 0xFFF);
  if ((v21 >= v22))
  {
    if ((v22 >= v21))
      return a1;
    if (((v21 - v22) >= 2048))
    {
      w_u8((((uint32)(a1))+(320)*1u),1);
      return a1;
    }
    LABEL_7:
    w_u8((((uint32)(a1))+(320)*1u),0);

    return a1;
  }
  if (((v22 - v21) >= 2048))
    goto LABEL_7;
  w_u8((((uint32)(a1))+(320)*1u),1);
  return a1;
}
uint32 sub_8005C580(void)
{
  sint32 result;
  sint32 v1;
  sint32 v2;
  result = r_u32(0x800FF5A0u);
  v1 = r_u32(((uint32)((r_u32(0x800FF5A0u) + 8))));
  v2 = r_u32(((uint32)((r_u32(0x800FF5A0u) + 12))));
  w_u32(((uint32)((r_u32(0x800FF5A0u) + 392))),r_u32(((uint32)((r_u32(0x800FF5A0u) + 4)))));
  w_u32(((uint32)((result + 396))),v1);
  w_u32(((uint32)((result + 400))),v2);
  v1 = ((v1&0xFFFF0000u)|(((r_u16(((uint32)((result + 20)))))&0xFFFFu)<<0));
  w_u32(((uint32)((result + 410))),r_u32(((uint32)((result + 16)))));
  w_u16(((uint32)((result + 414))),v1);
  return result;
}
/* TODO Missing call adapter nullsub_17 */
/* TODO Postincrement memory expressions may require ordering refinement */
uint32 sub_8002FF90(void)
{
  sint32 v0;
  sint32 result;
  sint32 v2;
  if (((r_u32(0x800FF818u) == 1) && r_u8(0x800EC1E8u)))
  {
    w_u32(0x800FF818u,0);
    w_u32(0x800FF2ECu,7);
    (abort(),0u);
  }
  v0 = (r_u32(0x800FF2F4u) & 1);
  result = (r_u32(0x800FF2F4u) + 1);
  (w_u32(0x800FF38Cu,(r_u32(0x800FF38Cu)+1u)),(r_u32(0x800FF38Cu)+1u));
  (w_u32(0x800FF2F4u,(r_u32(0x800FF2F4u)+1u)),(r_u32(0x800FF2F4u)+1u));
  if (!v0)
  {
    (w_u32(0x800FF2F0u,(r_u32(0x800FF2F0u)+1u)),(r_u32(0x800FF2F0u)+1u));
    sub_8001BB68();
    sub_8006A868();
    sub_80070748();
    if ((r_u32(0x800FF008u) || r_u32(0x800FF300u)))
    {
      sub_80063770(0x800FF904u);
    }
    else
    {
      sub_8001BF34();
      sub_8006737C();
      sub_80063738(0x800FF4E8u);
      sub_80063738(0x800FF5DCu);
      sub_80063CC4();
      sub_80063770(0x800FF5A0u);
      sub_80063CF0();
      sub_800372B8();
      sub_80063770(0x800FF204u);
      sub_80063770(0x800FF220u);
      sub_80063770(0x800FF5DCu);
      sub_80063770(0x800FF8A0u);
      sub_80063770(0x800FF4E8u);
      sub_8006FD18();
      sub_800638A4();
      sub_80032C88();
    }
    sub_80022860();
    sub_8006CDD4();
    sub_80017914();
    sub_8001B7C4();
    result = r_u32(0x800FF340u);
    if (!r_u32(0x800FF340u))
    {
      sub_8006F500();
      return sub_8006B2A8();
    }
  }
  return result;
}
uint32 sub_80063CC4(void)
{
  sint32 result;
  for (result = r_u32(0x800FF630u); result; result = r_u32(((uint32)((result + 20)))))
    w_u8(((uint32)((result + 4))),0);

  return result;
}
/* TODO Missing call adapter indirect */
void sub_80063770(uint32 a1)
{
  sint32 v1;
  sint32 v3;
  short v4;
  uint32 v5;
  sint32 v6;
  v3 = ((sint32)(r_u32(a1)));
  if (((sint32)(r_u32(a1))))
    v1 = r_u32(((uint32)((v3 + 28))));
  while (v3)
  {
    sub_80062858(v3);
    v4 = r_u16(((uint32)((v3 + 78))));
    if (((v4 & 0x40) != 0))
    {
      if (((v4 & 0x80) != 0))
      {
        ((void)((v3 + ((sint16)(r_u16(((uint32)((r_u32(((uint32)((v3 + 68)))) + 8)))))))),(void)(3),abort(),0u);
        v3 = v1;
        goto LABEL_14;
      }
      w_u16(((uint32)((v3 + 78))),(v4 | 0x80));
    }
    else
    {
      v5 = sub_8006696C((r_u32(0x800FF5A0u) + 4),(v3 + 4));
      if (!sub_8006FC84(v3,v5))
      {
        v6 = (r_u16(((uint32)((v3 + 78)))) & 2);
        w_u32(((uint32)((v3 + 220))),v5);
        if ((v6 && (r_u32(0x800FF5ECu) < v5)))
        {
          sub_80062B7C(v3,a1);
          v3 = v1;
          goto LABEL_14;
        }
        sub_8006338C(v3);
        sub_80062DD8(v3);
      }
    }
    v3 = v1;
    LABEL_14:
    if (!v1)
      return;

    v1 = r_u32(((uint32)((v1 + 28))));
  }

}
uint32 sub_80062858(uint32 a1)
{
  sint32 result;
  sint32 v3;
  sint32 v4;
  sint32 v5;
  short v6;
  short v7;
  short v8;
  result = r_u16(((uint32)((a1 + 60))));
  if (r_u16(((uint32)((a1 + 60)))))
  {
    v3 = r_u32(((uint32)((a1 + 32))));
    v4 = (((unsigned char)((((v3>>8)&255u) + r_u16(((uint32)((a1 + 64))))))) << 8);
    v5 = ((unsigned char)((r_u32(((uint32)((a1 + 32)))) + r_u16(((uint32)((a1 + 62)))))));
    v6 = r_u16(((uint32)((a1 + 66))));
    w_u16(((uint32)(a1)),(r_u16(((uint32)(a1)))|(0x400u)));
    v7 = r_u16(((uint32)((a1 + 60))));
    result = (((((unsigned char)((((v3>>16)&255u) + v6))) << 16) | v4) | v5);
    w_u32(((uint32)((a1 + 32))),result);
    w_u16(((uint32)((a1 + 60))),--v7);
    if (!v7)
    {
      if (((r_u32(((uint32)((a1 + 52)))) & 0x2000000) != 0))
        v8 = (r_u16(((uint32)(a1))) | 0x400);
      else
        v8 = (r_u16(((uint32)(a1))) & 0xFBFF);
      w_u16(((uint32)(a1)),v8);
      result = r_u32(((uint32)((a1 + 52))));
      w_u32(((uint32)((a1 + 52))),0);
      w_u32(((uint32)((a1 + 32))),result);
    }
  }
  return result;
}




