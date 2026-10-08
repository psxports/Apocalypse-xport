#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
#include <stdlib.h>
/* TODO Missing call adapter xport_draft_missing_gte_adapter */
void sub_800858FC(uint32 a1, uint32 a2)
{
  uint32 xport_draft_asm_carrier; /* TODO GTE carrier */
  uint32 v5;
  (abort(),0u);
  xport_draft_asm_carrier = r_u32(((uint32)(((uint32)(((uint32)(4) * (uint32)((r_u32(a1) & 0xFFF)))) - (uint32)(2146466244)))));
  v5 = r_u32(((uint32)(((uint32)(((uint32)(4) * (uint32)((r_u32((a1+(1)*4u)) & 0xFFF)))) - (uint32)(2146466244)))));
  (abort(),0u);
  xport_draft_asm_carrier = ((xport_draft_asm_carrier>>16)&65535u);
  (abort(),0u);
  xport_draft_asm_carrier = ((unsigned short)(v5));
  (abort(),0u);
  xport_draft_asm_carrier = r_u32(((uint32)(((uint32)(((r_u32(a1) >> 14) & 0x3FFC)) - (uint32)(2146466244)))));
  xport_draft_asm_carrier = ((v5>>16)&65535u);
  (abort(),0u);
  xport_draft_asm_carrier = ((uint32)(((xport_draft_asm_carrier>>16)&65535u)) - (uint32)(((uint32)(xport_draft_asm_carrier) << (uint32)(16))));
  (abort(),0u);
  xport_draft_asm_carrier = (((unsigned short)((xport_draft_asm_carrier ^ v5))) ^ v5);
  (abort(),0u);
  xport_draft_asm_carrier = ((uint32)(((unsigned short)(xport_draft_asm_carrier))) - (uint32)(((uint32)(v5) << (uint32)(16))));
  (abort(),0u);
  w_u16(a2,xport_draft_asm_carrier);
  w_u16((a2+(6)*2u),xport_draft_asm_carrier);
  w_u16((a2+(3)*2u),xport_draft_asm_carrier);
  (abort(),0u);
  w_u16((a2+(1)*2u),xport_draft_asm_carrier);
  w_u16((a2+(7)*2u),xport_draft_asm_carrier);
  w_u16((a2+(5)*2u),-(((short)(xport_draft_asm_carrier))));
  w_u16((a2+(4)*2u),xport_draft_asm_carrier);
  (abort(),0u);
  w_u16((a2+(2)*2u),xport_draft_asm_carrier);
  w_u16((a2+(8)*2u),xport_draft_asm_carrier);
}


/* TODO Missing call adapter sub_8008784C */
/* TODO Missing call adapter sub_800879FC */
/* TODO Resolve original data label & 0x800F25F0u */
uint32 sub_8007E63C(uint32 a1, uint32 a2, uint32 a3)
{
  uint32 v4;
  unsigned short v5;
  sint32 v6;
  sint32 v9;
  sint32 v10;
  sint32 v11;
  sint32 v12;
  sint32 v13;
  short v14;
  sint32 v15;
  sint32 v16;
  short v17;
  sint32 v18;
  sint32 v19;
  short v20;
  sint32 v21;
  sint32 v22;
  short v23;
  sint32 v24;
  sint32 v25;
  uint32 v26;
  uint32 v27;
  uint32 v28;
  sint32 v29;
  unsigned short v30;
  uint32 v31;
  short v32;
  short v33;
  uint32 v34;
  sint32 v35;
  sint32 v36;
  sint32 v37;
  sint32 v38;
  sint32 result;
  w_u32(0x800F25D0u,0);
  v4 = 0x800F25D0u;
  w_u32(0x800F25D4u,((r_u32(0x800F25D4u) & 0xFFFF0000u)|(((-4096) & 0xFFFFu)<<0)));
  v5 = r_u16((a2+(5)*2u));
  w_u32(0x800F25D8u,0);
  w_u32(0x800F25DCu,((r_u32(0x800F25DCu) & 0xFFFF0000u)|(((4096) & 0xFFFFu)<<0)));
  w_u32(0x800F25D4u,((r_u32(0x800F25D4u) & 0x0000FFFFu)|(((v5) & 0xFFFFu)<<16)));
  w_u32(0x800F25DCu,((r_u32(0x800F25DCu) & 0x0000FFFFu)|(((-r_u16((a2+(4)*2u))) & 0xFFFFu)<<16)));
  v6 = ((uint32)(r_u16((a2+(7)*2u))) - (uint32)(r_u16((a2+(3)*2u))));
  v9 = sub_80085B54(((uint32)(((sint32)(((uint32)(v6) * (uint32)(v6))))) + (uint32)(((uint32)(r_u16((a2+(8)*2u))) * (uint32)(r_u16((a2+(8)*2u)))))));
  w_u32(0x800F25E0u,((r_u32(0x800F25E0u) & 0xFFFF0000u)|(((0) & 0xFFFFu)<<0)));
  w_u32(0x800F25E0u,((r_u32(0x800F25E0u) & 0x0000FFFFu)|((((((uint32)(r_u16((a2+(8)*2u))) << (uint32)(12)) / ((sint32)(v9)))) & 0xFFFFu)<<16)));
  v9 = ((v9 & 0xFFFF0000u)|((((((uint32)(-4096) * (uint32)(((uint32)(r_u16((a2+(3)*2u))) - (uint32)(r_u16((a2+(7)*2u)))))) / ((sint32)(v9)))) & 0xFFFFu)<<0));
  w_u16(0x800F25E6u,0);
  w_u16(0x800F25E4u,v9);
  v10 = ((uint32)(r_u16((a2+(1)*2u))) - (uint32)(r_u16((a2+(7)*2u))));
  v11 = sub_80085B54(((uint32)(((sint32)(((uint32)(v10) * (uint32)(v10))))) + (uint32)(((uint32)(r_u16((a2+(8)*2u))) * (uint32)(r_u16((a2+(8)*2u)))))));
  w_u32(0x800F25E8u,((r_u32(0x800F25E8u) & 0xFFFF0000u)|(((0) & 0xFFFFu)<<0)));
  w_u32(0x800F25E8u,((r_u32(0x800F25E8u) & 0x0000FFFFu)|((((((uint32)(-4096) * (uint32)(r_u16((a2+(8)*2u)))) / ((sint32)(v11)))) & 0xFFFFu)<<16)));
  v11 = ((v11 & 0xFFFF0000u)|((((((uint32)(-4096) * (uint32)(((uint32)(r_u16((a2+(7)*2u))) - (uint32)(r_u16((a2+(1)*2u)))))) / ((sint32)(v11)))) & 0xFFFFu)<<0));
  w_u16(0x800F25EEu,0);
  w_u16(0x800F25ECu,v11);
  v12 = ((uint32)(r_u16((a2+(6)*2u))) - (uint32)(r_u16((a2+(2)*2u))));
  v13 = sub_80085B54(((uint32)(((uint32)(((uint32)(v12) * (uint32)(25))) * (uint32)(v12))) + (uint32)(((uint32)(r_u16((a2+(8)*2u))) * (uint32)(((uint32)(r_u16((a2+(8)*2u))) << (uint32)(6)))))));
  v14 = (((uint32)(r_u16((a2+(8)*2u))) << (uint32)(15)) / ((sint32)(v13)));
  w_u16(((uint32)((0x800F25F0u+(2)*4u))),0);
  w_u16(((uint32)(0x800F25F0u)),v14);
  v13 = ((v13 & 0xFFFF0000u)|((((((uint32)(-20480) * (uint32)(((uint32)(r_u16((a2+(2)*2u))) - (uint32)(r_u16((a2+(6)*2u)))))) / ((sint32)(v13)))) & 0xFFFFu)<<0));
  w_u16(((uint32)((0x800F25F0u+(6)*4u))),0);
  w_u16(((uint32)((0x800F25F0u+(4)*4u))),v13);
  v15 = ((uint32)(r_u16(a2)) - (uint32)(r_u16((a2+(6)*2u))));
  v16 = sub_80085B54(((uint32)(((uint32)(((uint32)(v15) * (uint32)(25))) * (uint32)(v15))) + (uint32)(((uint32)(r_u16((a2+(8)*2u))) * (uint32)(((uint32)(r_u16((a2+(8)*2u))) << (uint32)(6)))))));
  v17 = (((uint32)(-32768) * (uint32)(r_u16((a2+(8)*2u)))) / ((sint32)(v16)));
  w_u16(((uint32)((0x800F25F0u+(10)*4u))),0);
  w_u16(((uint32)((0x800F25F0u+(8)*4u))),v17);
  v16 = ((v16 & 0xFFFF0000u)|((((((uint32)(-20480) * (uint32)(((uint32)(r_u16((a2+(6)*2u))) - (uint32)(r_u16(a2))))) / ((sint32)(v16)))) & 0xFFFFu)<<0));
  w_u16(((uint32)((0x800F25F0u+(14)*4u))),0);
  w_u16(((uint32)((0x800F25F0u+(12)*4u))),v16);
  v18 = ((uint32)(r_u16((a2+(6)*2u))) - (uint32)(r_u16((a2+(2)*2u))));
  v19 = sub_80085B54(((uint32)(((sint32)(((uint32)(v18) * (uint32)(v18))))) + (uint32)(((uint32)(r_u16((a2+(8)*2u))) * (uint32)(r_u16((a2+(8)*2u)))))));
  v20 = (((uint32)(r_u16((a2+(8)*2u))) << (uint32)(12)) / ((sint32)(v19)));
  w_u32(0x800F2600u,((r_u32(0x800F2600u) & 0x0000FFFFu)|(((0) & 0xFFFFu)<<16)));
  w_u32(0x800F2600u,((r_u32(0x800F2600u) & 0xFFFF0000u)|(((v20) & 0xFFFFu)<<0)));
  v19 = ((v19 & 0xFFFF0000u)|((((((uint32)(-4096) * (uint32)(((uint32)(r_u16((a2+(2)*2u))) - (uint32)(r_u16((a2+(6)*2u)))))) / ((sint32)(v19)))) & 0xFFFFu)<<0));
  w_u16(0x800F2606u,0);
  w_u16(0x800F2604u,v19);
  v21 = ((uint32)(r_u16(a2)) - (uint32)(r_u16((a2+(6)*2u))));
  v22 = sub_80085B54(((uint32)(((sint32)(((uint32)(v21) * (uint32)(v21))))) + (uint32)(((uint32)(r_u16((a2+(8)*2u))) * (uint32)(r_u16((a2+(8)*2u)))))));
  v23 = (((uint32)(-4096) * (uint32)(r_u16((a2+(8)*2u)))) / ((sint32)(v22)));
  w_u32(0x800F2608u,((r_u32(0x800F2608u) & 0x0000FFFFu)|(((0) & 0xFFFFu)<<16)));
  w_u32(0x800F2608u,((r_u32(0x800F2608u) & 0xFFFF0000u)|(((v23) & 0xFFFFu)<<0)));
  v24 = ((uint32)(-4096) * (uint32)(((uint32)(r_u16((a2+(6)*2u))) - (uint32)(r_u16(a2)))));
  v25 = 0;
  v26 = 0x800F25E8u;
  w_u16(0x800F260Eu,0);
  v27 = r_u32(0x800F3E50u);
  v28 = r_u32(0x800F3E30u);
  w_u16(0x800F260Cu,(((sint32)(v24)) / ((sint32)(v22))));
  do
  {
    ((void)(((uint32)(a1) + (uint32)(52))),(void)(v4),(void)(v28),abort(),0u);
    ((void)(((uint32)(a1) + (uint32)(52))),(void)(v26),(void)(v27),abort(),0u);
    v27 = ((uint32)((((uint32)(v27))+(6)*1u)));
    v26 += (2)*4u;
    v28 = ((uint32)((((uint32)(v28))+(6)*1u)));
    ++v25;
    v4 += (2)*4u;
  }
  while ((((sint32)(v25)) < 3));
  v29 = 0;
  sub_800879EC(r_u16((a2+(8)*2u)));
  sub_800879CC(r_u16((a2+(6)*2u)),r_u16((a2+(7)*2u)));
  sub_8007E57C(a2,a3);
  v30 = r_u32(0x800FFAECu);
  w_u32(0x800FFB0Cu,a1);
  w_u32(0x800FFB08u,((sint32)(a2)));
  w_u32(0x800FFB30u,a3);
  w_u16((a2+(4)*2u),r_u32(0x800FFAE8u));
  w_u16((a2+(5)*2u),v30);
  ((void)(((uint32)(a1) + (uint32)(52))),(void)(((uint32)(a1) + (uint32)(84))),abort(),0u);
  v31 = ((uint32)(a1));
  do
  {
    v32 = ((uint32)(8) * (uint32)(r_u16((v31+(42)*2u))));
    ++v29;
    v33 = r_u16((v31+(48)*2u));
    w_u16((v31+(61)*2u),r_u16((v31+(45)*2u)));
    w_u16((v31+(64)*2u),v33);
    w_u16((v31+(58)*2u),(((sint32)(v32)) / 5));
    (v31+=2u);
  }
  while ((((sint32)(v29)) < 3));
  v34 = ((uint32)(sub_8006E278(-857728900)));
  v35 = ((uint32)(r_u16((((uint32)(v34))+(3)*2u))) << (uint32)(16));
  w_u32(0x800FFB24u,((uint32)(r_u16((((uint32)(v34))+(1)*2u))) << (uint32)(16)));
  w_u32(0x800FFB28u,v35);
  v36 = r_u8(v34);
  v37 = r_u8((v34+(1)*1u));
  w_u32(0x800FFAB4u,r_u32(0x800FFB40u));
  w_u32(0x800FFB40u,r_u32(0x800FF650u));
  w_u32(0x800FFB2Cu,(v36 | ((uint32)(v37) << (uint32)(8))));
  sub_80080398(r_u32((0x800FF778u+(0)*4u)));
  sub_80080398(r_u32(0x800FF77Cu));
  sub_80080398(((unsigned char)(r_u8(0x800FF644u))));
  if (!r_u32(0x800FFAB0u))
    sub_8007FE68();
  sub_8007FF70(((unsigned char)(r_u8(0x800FF644u))));
  w_u32(0x800FFB10u,((r_u32(0x800FFB10u) & 0xFFFF0000u)|(((-(r_u16(((uint32)(((uint32)(r_u32(0x800FFB0Cu)) + (uint32)(58))))))) & 0xFFFFu)<<0)));
  w_u32(0x800FFB10u,((r_u32(0x800FFB10u) & 0x0000FFFFu)|(((-(r_u16(((uint32)(((uint32)(r_u32(0x800FFB0Cu)) + (uint32)(60))))))) & 0xFFFFu)<<16)));
  w_u32(0x800FFB14u,((r_u32(0x800FFB14u) & 0xFFFF0000u)|(((-(r_u16(((uint32)(((uint32)(r_u32(0x800FFB0Cu)) + (uint32)(62))))))) & 0xFFFFu)<<0)));
  v38 = r_u16(((uint32)(((uint32)(r_u32(0x800FFB0Cu)) + (uint32)(8)))));
  result = ((uint32)((r_u32(0x800FF37Cu) >> 12)) - (uint32)(v38));
  w_u32(0x800FFB14u,((r_u32(0x800FFB14u) & 0x0000FFFFu)|(((((uint32)((r_u32(0x800FF37Cu) >> 12)) - (uint32)(v38))) & 0xFFFFu)<<16)));
  return result;
}


/* TODO Missing host buffer adapter xport_draft_host_sub_800890BC_p2 */
uint32 sub_8007E57C(uint32 a1, uint32 a2)
{
  sint32 v3;
  uint32 result;
  short v5[4];
  v5[0] = r_u16((a1+(2)*2u));
  v5[1] = ((uint32)(r_u16((a1+(3)*2u))) + (uint32)(r_u16(((uint32)(((uint32)(r_u32(0x800FF660u)) + (uint32)(2)))))));
  v3 = r_u32(0x800FF668u);
  v5[2] = ((uint32)(r_u16(a1)) - (uint32)(r_u16((a1+(2)*2u))));
  v5[3] = ((uint32)(r_u16((a1+(1)*2u))) - (uint32)(r_u16((a1+(3)*2u))));
  xport_draft_host_sub_800890BC_p2(r_u32(0x800FF668u),v5);
  w_u32(((uint32)(v3)),((r_u32(((uint32)(v3))) & 0xFF000000) | (r_u32(((uint32)(((uint32)(a2) + (uint32)(16380))))) & 0xFFFFFF)));
  result = ((r_u32(((uint32)(((uint32)(a2) + (uint32)(16380))))) & 0xFF000000) | (v3 & 0xFFFFFF));
  w_u32(((uint32)(((uint32)(a2) + (uint32)(16380)))),result);
  w_u32(0x800FF668u,((uint32)(v3) + (uint32)(12)));
  return result;
}


/* TODO Missing call adapter xport_draft_missing_gte_adapter */
uint32 sub_80080398(uint32 a1)
{
  uint32 xport_draft_asm_carrier; /* TODO GTE carrier */
  sint32 result;
  uint32 v2;
  uint32 v3;
  uint32 v4;
  sint32 v5;
  uint32 v6;
  sint32 v7;
  sint32 v8;
  sint32 v9;
  sint32 v10;
  uint32 v11;
  sint32 v12;
  uint32 v13;
  sint32 v14;
  sint32 v15;
  sint32 v16;
  sint32 v17;
  uint32 v18;
  uint32 v22;
  sint32 v23;
  sint32 v24;
  sint32 v25;
  int v26[4];
  result = -1;
  if ((a1 != -1))
  {
    result = ((sint32)(0x800EAEF8u));
    v2 = (0x800EAEF8u+(((uint32)(16) * (uint32)(a1)))*4u);
    v3 = ((uint32)(((sint32)(r_u32((v2+(10)*4u))))));
    if (v3)
    {
      w_u32(0x800FFB04u,((sint32)(r_u32((v2+(8)*4u)))));
      v4 = (v3+(r_u32((((uint32)(v3))-(1)*4u)))*1u);
      v5 = 1;
      if ((r_u32(0x800FFB40u) >= ((uint32)(r_u32(0x800FFAB4u)))))
        v5 = ((uint32)(r_u32(0x800FFB40u)) - (uint32)(r_u32(0x800FFAB4u)));
      while (1)
      {
        result = (v3 < v4);
        v22 = (v3+(4)*1u);
        if ((v3 >= v4))
          break;
        v6 = v3;
        v7 = r_u8((v3+(2)*1u));
        v8 = r_u8((v3+(1)*1u));
        v9 = ((uint32)(r_u8((v3+(3)*1u))) + (uint32)(v5));
        v10 = v7;
        if ((((sint32)(v9)) >= r_u8((v22+(((uint32)(((uint32)(4) * (uint32)(v7))) + (uint32)(3)))*1u))))
        {
          v11 = (v22+(((uint32)(4) * (uint32)(v7)))*1u);
          do
          {
            v12 = r_u8((v11+(3)*1u));
            v11 += (4)*1u;
            ++v7;
            v9 -= v12;
            if ((v7 == v8))
            {
              v11 = v22;
              v7 = 0;
            }
          }
          while ((((sint32)(v9)) >= r_u8((v11+(3)*1u))));
          v10 = v7;
        }
        w_u8((v6+(2)*1u),v7);
        v13 = (v22+(((uint32)(4) * (uint32)(v10)))*1u);
        w_u8((v6+(3)*1u),v9);
        v14 = ((uint32)(16) * (uint32)(r_u8(v13)));
        v23 = v14;
        v15 = ((uint32)(16) * (uint32)(r_u8((v13+(1)*1u))));
        v24 = v15;
        v16 = r_u8((v13+(2)*1u));
        v17 = ((uint32)(v10) + (uint32)(1));
        v25 = ((uint32)(16) * (uint32)(v16));
        if ((((uint32)(v10) + (uint32)(1)) == v8))
          v17 = 0;
        v18 = (v22+(((uint32)(4) * (uint32)(v17)))*1u);
        v26[0] = ((uint32)(((uint32)(16) * (uint32)(r_u8(v18)))) - (uint32)(v14));
        v26[1] = ((uint32)(((uint32)(16) * (uint32)(r_u8((v18+(1)*1u))))) - (uint32)(v15));
        xport_draft_asm_carrier = 0;
        v26[2] = ((uint32)(((uint32)(16) * (uint32)(r_u8((v18+(2)*1u))))) - (uint32)(((uint32)(16) * (uint32)(v16))));
        (abort(),0u);
        xport_draft_asm_carrier = (((uint32)(v9) << (uint32)(12)) / r_u8((v22+(((uint32)(((uint32)(4) * (uint32)(v10))) + (uint32)(3)))*1u)));
        (abort(),0u);
        xport_draft_asm_carrier = v26;
        (abort(),0u);
        v3 = (v6+(((uint32)(((uint32)(4) * (uint32)(v8))) + (uint32)(4)))*1u);
      }

    }
  }
  return result;
}



uint32 sub_8007FF70(uint32 a1)
{
  sint32 result;
  uint32 v2;
  uint32 v3;
  sint32 v4;
  sint32 v5;
  sint32 v6;
  uint32 v7;
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
  sint32 v23;
  sint32 v24;
  sint32 v25;
  short v26;
  sint32 i;
  sint32 v28;
  uint32 v29;
  sint32 v30;
  sint32 v31;
  sint32 v32;
  sint32 v33;
  sint32 v34;
  sint32 v35;
  sint32 v36;
  result = -1;
  if ((a1 != -1))
  {
    result = ((uint32)(a1) << (uint32)(6));
    v2 = (0x800EAEF8u+(((uint32)(16) * (uint32)(a1)))*4u);
    v3 = ((uint32)(((sint32)(r_u32((v2+(9)*4u))))));
    if (v3)
    {
      result = r_u32(((uint32)(v3)));
      for (i = ((sint32)(r_u32((v2+(5)*4u)))); r_u32(((uint32)(v3))); result = r_u32(((uint32)(v3))))
      {
        v4 = ((uint32)(i) + (uint32)(r_u32(((uint32)(v3)))));
        v29 = (v3+(16)*1u);
        v30 = r_u32((((uint32)(v3))+(3)*4u));
        if (((r_u16(((uint32)(v4))) & 0x8000) != 0))
        {
          v3 += (((uint32)(((uint32)(16) * (uint32)(r_u32((((uint32)(v3))+(3)*4u))))) + (uint32)(16)))*1u;
        }
        else
        {
          v5 = r_u32(((uint32)(((uint32)(((uint32)(4) * (uint32)(r_u16(((uint32)(((uint32)(v4) + (uint32)(22)))))))) + (uint32)(r_u32((0x800EAEF8u+(((uint32)(((uint32)(16) * (uint32)(r_u8(((uint32)(((uint32)(v4) + (uint32)(27)))))))) + (uint32)(4)))*4u)))))));
          v6 = ((uint32)(r_u32(0x800FFB40u)) * (uint32)(((sint16)(r_u16((((uint32)(v3))+(3)*2u))))));
          v31 = 0;
          v32 = (((uint32)(r_u32(0x800FFB40u)) * (uint32)(((sint16)(r_u16((((uint32)(v3))+(2)*2u)))))) >> 4);
          v28 = ((uint32)(((uint32)(((uint32)(v5) + (uint32)(((uint32)(8) * (uint32)(r_u32(((uint32)(((uint32)(v5) + (uint32)(4)))))))))) + (uint32)(32))) + (uint32)(((uint32)(8) * (uint32)(r_u32(((uint32)(((uint32)(v5) + (uint32)(8)))))))));
          w_u32(0x800FFB48u,(((uint32)(r_u32(0x800FFB40u)) * (uint32)(r_u32((((uint32)(v3))+(2)*4u)))) >> 10));
          v33 = (((sint32)(v6)) >> 4);
          if ((((sint32)(v30)) > 0))
          {
            v7 = (v3+(28)*1u);
            do
            {
              v8 = ((uint32)(r_u8((v7-(8)*1u))) << (uint32)(8));
              v9 = ((uint32)(r_u8(v29)) << (uint32)(8));
              v10 = ((uint32)(r_u8((v7-(4)*1u))) << (uint32)(8));
              v11 = ((uint32)(r_u8((v7-(11)*1u))) << (uint32)(8));
              v12 = ((uint32)(r_u8((v7-(7)*1u))) << (uint32)(8));
              v13 = ((uint32)(r_u8(v7)) << (uint32)(8));
              v14 = ((uint32)(r_u8((v7-(3)*1u))) << (uint32)(8));
              v15 = ((uint32)(r_u8((v7+(1)*1u))) << (uint32)(8));
              v34 = (((uint32)(-2048) * (uint32)((r_u32(((uint32)(((uint32)(v28) + (uint32)(32))))) & 0x1F))) & 0xF800);
              v35 = (((uint32)(-64) * (uint32)((r_u32(((uint32)(((uint32)(v28) + (uint32)(32))))) & 0x3E0))) & 0xFF00);
              if (v32)
              {
                v9 += v32;
                v8 += v32;
                v10 += v32;
                v13 += v32;
              }
              if (v33)
              {
                v11 += v33;
                v12 += v33;
                v14 += v33;
                v15 += v33;
              }
              v36 = ((sint32)(v7));
              v16 = ((uint32)(v9) + (uint32)(sub_8007FED0(((sint32)(v29)))));
              v17 = ((uint32)(v11) + (uint32)(sub_8007FF20(((sint32)(v29)))));
              v18 = ((uint32)(v8) + (uint32)(sub_8007FED0(((sint32)((v29+(4)*1u))))));
              v19 = ((uint32)(v12) + (uint32)(sub_8007FF20(((sint32)((v29+(4)*1u))))));
              v20 = ((uint32)(v10) + (uint32)(sub_8007FED0(((sint32)((v29+(8)*1u))))));
              v21 = ((uint32)(v14) + (uint32)(sub_8007FF20(((sint32)((v29+(8)*1u))))));
              v22 = ((uint32)(v13) + (uint32)(sub_8007FED0(v36)));
              v23 = ((uint32)(v15) + (uint32)(sub_8007FF20(v36)));
              if (((((((v16 & v18) & v20) & v22) ^ (((v16 | v18) | v20) | v22)) & 0x10000) != 0))
              {
                v16 += v34;
                do
                {
                  v18 += v34;
                  v20 += v34;
                  v22 += v34;
                  v24 = (((((v16 & v18) & v20) & v22) ^ (((v16 | v18) | v20) | v22)) & 0x10000);
                  v16 += v34;
                }
                while (v24);
                v16 = ((v16 & 0xFFFF0000u)|(((((sint32)(((uint32)(v16) - (uint32)(v34))))) & 0xFFFFu)<<0));
              }
              if (((((((v17 & v19) & v21) & v23) ^ (((v17 | v19) | v21) | v23)) & 0x10000) != 0))
              {
                v17 += v35;
                do
                {
                  v19 += v35;
                  v21 += v35;
                  v23 += v35;
                  v25 = (((((v17 & v19) & v21) & v23) ^ (((v17 | v19) | v21) | v23)) & 0x10000);
                  v17 += v35;
                }
                while (v25);
                v17 = ((v17 & 0xFFFF0000u)|(((((sint32)(((uint32)(v17) - (uint32)(v35))))) & 0xFFFFu)<<0));
              }
              ++v31;
              v29 += (16)*1u;
              w_u16(((uint32)(((uint32)(v28) + (uint32)(20)))),(((v16 & 0xFF00) >> 8) | (v17 & 0xFF00)));
              w_u16(((uint32)(((uint32)(v28) + (uint32)(24)))),(((v18 & 0xFF00) >> 8) | (v19 & 0xFF00)));
              v26 = r_u16(((uint32)(((uint32)(v28) + (uint32)(2)))));
              w_u32(((uint32)(((uint32)(v28) + (uint32)(28)))),(((((v20 & 0xFF00) >> 8) | (v21 & 0xFF00)) | ((uint32)((v22 & 0xFF00)) << (uint32)(8))) | ((uint32)((v23 & 0xFF00)) << (uint32)(16))));
              v28 += (v26 & 0xFFFC);
              v7 = ((uint32)(((uint32)(v36) + (uint32)(16))));
            }
            while ((((sint32)(v31)) < ((sint32)(v30))));
          }
          v3 = v29;
        }
      }

    }
  }
  return result;
}


/* TODO Missing call adapter JUMPOUT */
/* TODO Missing call adapter sub_800821B8 */
/* TODO Missing call adapter sub_80082224 */
/* TODO Missing call adapter sub_80083BF4 */
/* TODO Missing call adapter sub_80083C0C */
/* TODO Missing call adapter sub_80083C24 */
/* TODO Missing call adapter sub_80083C3C */
/* TODO Missing call adapter sub_80083C54 */
/* TODO Missing call adapter sub_80083E54 */
/* TODO Missing call adapter xport_draft_missing_gte_adapter */
/* TODO Postincrement memory expressions may require ordering refinement */
static void draft_817FC_normal(uint32 geometry, uint32 normal[3])
{
  normal[0] = r_u32(geometry);
  normal[1] = (uint32)((sint32)normal[0] >> 16);
  normal[2] = r_u32(geometry + 4);
}

uint32 sub_800817FC(uint32 descriptor, uint32 unused, uint32 count)
{
  const uint32 scratch = 0x1F800000u;
  const uint32 normals = 0x800F3E70u;
  uint32 packet, projection, table, ordering, limit;
  (void)unused;
  if (!count) return xport_draft_unknown_result_800817FC();
  packet = r_u32(0x800FF668u) & 0xFFFFFFu;
  projection = r_u32(0x800FFAC0u);
  table = r_u32(0x800FFB04u);
  ordering = r_u32(0x800FFB30u);
  limit = r_u32(0x800FF374u) - 100u;
  xport_draft_gte_control_write(5, 0);
  xport_draft_gte_control_write(6, 0);
  xport_draft_gte_control_write(7, 0);
  xport_draft_gte_control_write(28, 0x10000000u);
  xport_draft_gte_control_write(27, 0);
  w_u32(scratch + 0x1E4, 0x80082FACu);
  w_u32(scratch + 0x1F4, 0);
  for (;;) {
    uint32 offsets[4], points[4], depths[4], flags, clipping, area;
    uint32 packed = r_u32(descriptor + 4);
    uint32 packed2 = r_u32(descriptor + 8);
    uint32 mask = r_u32(scratch + 0x144);
    uint32 depth, fog = 0, vertices, colored, textured, tag, command;
    uint32 color[4], uv[4], normal[3], index[4];
    uint32 i, bucket, previous, bytes;
    offsets[0] = packed & 0xFFFFu; offsets[1] = packed >> 16;
    offsets[2] = packed2 & 0xFFFFu; offsets[3] = packed2 >> 16;
    flags = (r_u32(descriptor) & (uint32)((sint32)mask >> 16)) | (mask & 0xFFFFu);
    for (i = 0; i < 4; ++i) points[i] = projection + offsets[i];
    for (i = 0; i < 3; ++i) xport_draft_gte_data_write(12 + i, r_u32(points[i]));
    if (!(flags & 0xC0u)) goto next_descriptor;
    xport_draft_gte_execute(0x1400006u);
    for (i = 0; i < 4; ++i) depths[i] = r_u32(points[i] + 4);
    w_u32(packet + 8, xport_draft_gte_data_read(12));
    clipping = ((depths[0] & depths[1] & depths[2] & depths[3]) >> 16) ^ 0xFF00u;
    xport_draft_gte_data_write(15, r_u32(points[3]));
    area = xport_draft_gte_data_read(24);
    xport_draft_gte_execute(0x1400006u);
    if (clipping & 0x20FFu) goto next_descriptor;
    if (clipping & 0x5000u) {
      uint32 geometry0 = points[0] + 0x1F40u;
      uint32 geometry1 = points[1] + 0x1F40u;
      uint32 geometry2 = points[2] + 0x1F40u;
      uint32 cross[3], magnitude = 0;
      if (!(flags & 0x1000u)) goto next_descriptor;
      packed = r_u32(geometry0);
      xport_draft_gte_control_write(0, packed);
      xport_draft_gte_control_write(2, packed >> 16);
      xport_draft_gte_control_write(4, r_u32(points[0] + 4));
      packed = r_u32(geometry1);
      xport_draft_gte_data_write(9, packed);
      xport_draft_gte_data_write(10, packed >> 16);
      xport_draft_gte_data_write(11, r_u32(points[1] + 4));
      xport_draft_gte_execute(0x170000Cu);
      if ((sint32)xport_draft_gte_control_read(31) < 0) {
        uint32 shift;
        for (i = 0; i < 3; ++i) {
          cross[i] = xport_draft_gte_data_read(25 + i);
          magnitude |= (sint32)cross[i] < 0 ? 0u - cross[i] : cross[i];
        }
        xport_draft_gte_data_write(30, magnitude);
        shift = (17u - xport_draft_gte_data_read(31)) & 31u;
        for (i = 0; i < 3; ++i)
          xport_draft_gte_data_write(9 + i, (uint32)((sint32)cross[i] >> shift));
      }
      xport_draft_gte_control_write(0, r_u32(geometry2));
      xport_draft_gte_control_write(1, r_u32(points[2] + 4));
      xport_draft_gte_execute(0x41E012u);
      if ((sint32)xport_draft_gte_data_read(25) < 0) goto next_descriptor;
      area = 0x7FFF0000u;
    } else if ((sint32)area <= 0) {
      area = xport_draft_gte_data_read(24);
      if ((sint32)area >= 0) goto next_descriptor;
      area = 0u - area;
    }
    xport_draft_gte_data_write(24, area);
    if (packet >= limit) break;
    if (clipping & 0x8000u) {
      /* TODO Recover near-plane edge interpolation and queued descriptors at 800837AC */
      abort();
    }
    depth = depths[0] << 16;
    for (i = 1; i < 4; ++i) {
      uint32 candidate = depths[i] << 16;
      if ((sint32)(depth - candidate) < 0) depth = candidate;
    }
    depth = (uint32)((sint32)depth >> 16);
    packed = depth - r_u32(scratch + 0x164);
    if ((sint32)packed > 0) {
      uint32 shift = r_u32(scratch + 0x174);
      fog = (sint32)shift >= 0 ? packed << (shift & 31u)
          : (uint32)((sint32)packed >> ((0u - shift) & 31u));
    }
    xport_draft_gte_data_write(8, fog);
    vertices = flags & 0x10u ? 3u : 4u;
    colored = (flags & 0x800u) || ((flags & 12u) == 12u);
    textured = colored ? (flags & 1u) : (flags & 3u);
    if (flags & 0x800u) {
      packed = r_u32(descriptor + 12);
      index[0] = packed << 2; index[1] = packed >> 6;
      index[2] = packed >> 14; index[3] = packed >> 22;
      if ((flags & 12u) == 0) {
        if (vertices == 4) {
          xport_draft_gte_data_write(6, r_u32(table + (index[0] & 0x3FCu)));
          xport_draft_gte_execute(0x780010u);
          color[0] = xport_draft_gte_data_read(22);
          for (i = 1; i < 4; ++i)
            xport_draft_gte_data_write(19 + i, r_u32(table + (index[i] & 0x3FCu)));
        } else {
          for (i = 0; i < 3; ++i)
            xport_draft_gte_data_write(20 + i, r_u32(table + (index[i] & 0x3FCu)));
        }
        xport_draft_gte_execute(0xF8002Au);
      } else {
        if (!(flags & 4u)) {
          normal[0] = r_u32(scratch + 0x1A4);
          normal[1] = r_u32(scratch + 0x1B4);
          normal[2] = r_u32(scratch + 0x1C4);
        } else if (!(flags & 8u)) draft_817FC_normal(normals + r_u32(descriptor + 16), normal);
        for (i = 0; i < vertices; ++i) {
          if ((flags & 12u) == 12u) draft_817FC_normal(normals + offsets[i], normal);
          sub_80082200(index[i], table, normal[0], normal[1], normal[2]);
          if (i == 3) color[0] = xport_draft_gte_data_read(20);
          xport_draft_gte_execute(0x680029u);
        }
      }
    } else {
      xport_draft_gte_data_write(6, r_u32(descriptor + 12));
      if ((flags & 12u) == 12u) {
        for (i = 0; i < vertices; ++i) {
          sub_800821E0(normals + offsets[i]);
          if (i == 3) color[0] = xport_draft_gte_data_read(20);
          xport_draft_gte_execute(0x680029u);
        }
      } else {
        if (flags & 4u) sub_800821E0(normals + r_u32(descriptor + 16));
        else if (flags & 8u) sub_80082254(scratch);
        xport_draft_gte_execute(flags & 12u ? 0x680029u : 0x780010u);
      }
    }
    if (colored) {
      if (vertices == 3) color[0] = xport_draft_gte_data_read(20);
      for (i = 1; i < vertices; ++i)
        color[i] = xport_draft_gte_data_read((vertices == 3 ? 20u : 19u) + i);
    } else color[0] = xport_draft_gte_data_read(22);
    if (textured) {
      if (flags & 1u) {
        uv[0] = r_u32(descriptor + 20); uv[1] = r_u32(descriptor + 24);
        uv[2] = r_u32(descriptor + 28); uv[3] = uv[2] >> 16;
      } else {
        for (i = 0; i < vertices; ++i) uv[i] = r_u32(normals + offsets[i] + 4);
        uv[0] |= r_u32(scratch + 0x184);
        uv[1] |= r_u32(scratch + 0x194);
        if (vertices == 4) { uv[2] |= uv[3] << 16; uv[3] = uv[2] >> 16; }
      }
      uv[1] |= (flags & 0x180u) << 14;
    }
    command = 0x20000000u | (vertices == 4 ? 0x08000000u : 0)
        | (colored ? 0x10000000u : 0) | (textured ? 0x04000000u : 0);
    sub_80082188(packet, color[0], command, flags);
    if (colored) {
      uint32 stride = textured ? 12u : 8u;
      for (i = 0; i < vertices; ++i) {
        if (i) {
          w_u32(packet + 4 + i * stride, color[i]);
          w_u32(packet + 8 + i * stride, xport_draft_gte_data_read(11 + i));
        }
        if (textured) w_u32(packet + 12 + i * stride, uv[i]);
      }
      tag = ((vertices * (textured ? 3u : 2u)) << 24);
    } else {
      for (i = 0; i < vertices; ++i) {
        if (i) w_u32(packet + 8 + i * (textured ? 8u : 4u), xport_draft_gte_data_read(11 + i));
        if (textured) w_u32(packet + 12 + i * 8u, uv[i]);
      }
      tag = (1u + vertices * (textured ? 2u : 1u)) << 24;
    }
    if ((flags & 0x1000u) && (sint32)(0x800u - xport_draft_gte_data_read(24)) < 0) {
      /* TODO Recover subdivision helper contexts at 80082800 through 80082EB8 */
      abort();
    }
    if (flags & 0x6000u) depth += r_u32(scratch + (flags & 0x2000u ? 0x204u : 0x214u));
    bucket = ordering + (depth < 0x4000u ? depth & 0xFFFCu : 0x3FFCu);
    previous = r_u32(bucket);
    if (flags & 0x20u) {
      uint32 end;
      tag += 0x03000000u;
      w_u32(packet, previous | tag);
      end = packet + (tag >> 22) + 4;
      w_u32(bucket, end);
      w_u32(end - 12, 0); w_u32(end - 8, 0xE2000000u); w_u32(end - 4, 0);
      w_u32(end, packet | 0x02000000u);
      w_u32(end + 4, r_u32(descriptor + 32)); w_u32(end + 8, 0);
      packet = end + 12;
    } else {
      w_u32(bucket, packet); w_u32(packet, previous | tag);
      bytes = (tag >> 22) + 4;
      if ((flags & 0x41u) == 0x40u) {
        uint32 end = packet + bytes;
        w_u32(end + 4, 0xE1000200u | ((flags & 0x180u) >> 2));
        w_u32(end, packet | 0x01000000u); w_u32(bucket, end);
        packet = end + 8;
      } else packet += bytes;
    }
next_descriptor:
    descriptor += flags >> 16;
    if (--count) continue;
    count = r_u32(scratch + 0x1F4);
    if (!count) break;
    descriptor = 0x80082FACu;
    w_u32(scratch + 0x1F4, 0);
  }
  w_u32(0x800FF668u, packet);
  return packet;
}
uint32 sub_80016800(void)
{
  sint32 result;
  sub_8001A7B0(1);
  sub_8001A7BC(256);
  sub_8001A7D4(149,20,20,0);
  sub_8001AA28(227,213,r_u32((0x800A5578u+(0)*4u)),0,0x0u,0x100u);
  result = sub_8006D028(r_u32(0x800FF01Cu));
  if (result)
    return sub_8006D0D4(199,214,result,r_u32(0x800FF01Cu));
  return result;
}



uint32 sub_80017364(uint32 a1, uint32 a2, uint32 a3, uint32 a4)
{
  sint32 v4;
  sint32 v5;
  sint32 result;
  v4 = (r_u8(0x800EC1D9u) == 0);
  w_u32(a4,((unsigned char)(r_u8(0x800EC1D9u))));
  if (!v4)
    w_u8(0x800EC1D9u,0);
  v5 = 0;
  if ((r_u8(0x800EC129u) || r_u32(a4)))
    v5 = 1;
  w_u32(a1,v5);
  if (v5)
    w_u8(0x800EC129u,0);
  v4 = (r_u8(0x800EC0F9u) == 0);
  w_u32(a2,((unsigned char)(r_u8(0x800EC0F9u))));
  if (!v4)
    w_u8(0x800EC0F9u,0);
  w_u32(a3,(r_u32(a2) | ((sint32)(r_u32(a1)))));
  if (r_u8(0x800EC109u))
  {
    w_u8(0x800EC109u,0);
    w_u32(a3,1);
  }
  if (r_u8(0x800EC119u))
  {
    w_u8(0x800EC119u,0);
    w_u32(a3,1);
  }
  if (r_u8(0x800EC1E9u))
  {
    w_u8(0x800EC1E9u,0);
    w_u32(a3,1);
  }
  if (r_u8(0x800EC139u))
  {
    w_u8(0x800EC139u,0);
    w_u32(a3,1);
  }
  if (r_u8(0x800EC149u))
  {
    w_u8(0x800EC149u,0);
    w_u32(a3,1);
  }
  if (r_u8(0x800EC159u))
  {
    w_u8(0x800EC159u,0);
    w_u32(a3,1);
  }
  if (r_u8(0x800EC169u))
  {
    w_u8(0x800EC169u,0);
    w_u32(a3,1);
  }
  result = ((sint32)(r_u32(a1)));
  if (((sint32)(r_u32(a1))))
    w_u32(a2,0);
  return result;
}



uint32 sub_80010938(void)
{
  TestEvent(r_u32(0x800FEED4u));
  TestEvent(r_u32(0x800FEED8u));
  TestEvent(r_u32(0x800FEEDCu));
  return TestEvent(r_u32(0x800FEEE0u));
}



uint32 sub_800166AC(void)
{
  sint32 v0;
  sub_8001A7B0(1);
  sub_8001A7BC(192);
  sub_8001A7D4(149,20,20,0);
  sub_8001AA28(92,210,r_u32(0x800A5574u),0,0,256);
  sub_8001A7B0(0);
  v0 = sub_8006D028(((uint32)(r_u32(0x800FF01Cu)) + (uint32)(8)));
  if (v0)
    v0 = sub_8006D0D4(64,211,v0,r_u32(0x800FF01Cu) + 8u);
  sub_8001A7BC(256);
  return v0;
}


/* TODO Missing call adapter sub_8008E14C */
/* TODO Missing host buffer adapter xport_draft_host_sub_80069D2C_p1 */
/* TODO Missing host buffer adapter xport_draft_host_sub_8006B04C_p1 */
uint32 sub_80069BC4(uint32 a1)
{
  sint32 i;
  uint32 v2;
  sint32 v3;
  uint32 v4;
  uint32 v5;
  sint32 v6;
  sint32 result;
  char v8[24];
  for (i = 0;; ++i)
  {
    v2 = &v8[i];
    if (!(((sint8)(r_u8(a1)))))
      break;
    w_u8(v2,r_u32(((a1+=1u)-1u)));
  }

  v3 = ((uint32)(i) + (uint32)(1));
  w_u8(v2,46);
  v8[((uint32)(i) + (uint32)(1))] = 86;
  v4 = &v8[((uint32)(i) + (uint32)(1))];
  w_u8((v4+(1)*1u),65);
  w_u16((((uint32)(v4))+(1)*2u),66);
  w_u32(0x800FF6A4u,xport_draft_host_sub_80069D2C_p1(v8,0x800FF6D8u));
  v8[v3] = 83;
  v5 = &v8[((uint32)(v3) + (uint32)(1))];
  w_u8(v5,69);
  w_u8((v5+(1)*1u),81);
  v6 = xport_draft_host_sub_8006B04C_p1(v8);
  w_u32(0x800FF6A8u,sub_8006B864(v6,1,1));
  sub_8006B234(r_u32(0x800FF6A8u));
  sub_8006B44C();
  w_u32(0x800FF6ACu,((short)(((void)(r_u32(0x800FF6A8u)),(void)(((short)(r_u32(0x800FF6A4u)))),abort(),0u))));
  w_u32(0x800FF6C8u,r_u8(((uint32)(((uint32)(r_u32(0x800FF6A8u)) + (uint32)(10))))));
  w_u32(0x800FF6C8u,(r_u32(0x800FF6C8u)+(((uint32)(r_u8(((uint32)(((uint32)(r_u32(0x800FF6A8u)) + (uint32)(11)))))) << (uint32)(8)))));
  result = ((uint32)(r_u8(((uint32)(((uint32)(r_u32(0x800FF6A8u)) + (uint32)(12)))))) << (uint32)(16));
  w_u32(0x800FF6C8u,(r_u32(0x800FF6C8u)+(result)));
  return result;
}


/* TODO Missing call adapter sub_80088B28 */
/* TODO Missing host buffer adapter xport_draft_host_sub_8008847C_p1 */
uint32 sub_80016354(uint32 a1, uint32 a2)
{
  uint32 v4;
  sint32 result;
  sint32 v6;
  uint32 v7;
  uint32 v9;
  sint32 v10;
  uint32 v12;
  uint32 v13;
  int v15[2];
  int v16[2];
  int v17[2];
  ((void)(0x800C636Cu),abort(),0u);
  v15[0] = r_u32(0x800FF05Cu);
  v15[1] = r_u32(0x800FF060u);
  xport_draft_host_sub_8008847C_p1(v15,0,0,0);
  sub_8001024C(0,a2,240);
  v16[0] = r_u32(0x800FF064u);
  v16[1] = r_u32(0x800FF068u);
  xport_draft_host_sub_8008847C_p1(v16,100,0,0);
  v17[0] = r_u32(0x800FF06Cu);
  v17[1] = r_u32(0x800FF070u);
  xport_draft_host_sub_8008847C_p1(v17,0,0,0);
  v4 = ((uint32)(r_u32(0x800FF004u)));
  w_u32(0x800FF050u,1);
  w_u32(0x800FF058u,0);
  while (1)
  {
    result = ((unsigned char)(r_u8(v4)));
    if (!(r_u8(v4)))
      break;
    if (sub_80067724(v4,a1))
    {
      v6 = ((sint32)((v4+(3)*1u)));
      if (r_u8(v4))
      {
        v7 = (v4+(1)*1u);
        while (r_u32(((v7+=1u)-1u)))
          ;

        v6 = ((sint32)((v7+(2)*1u)));
      }
      v9 = ((uint32)((v6 & 0xFFFFFFFC)));
      result = r_u32(((uint32)((v6 & 0xFFFFFFFC))));
      if ((result != -1))
      {
        do
        {
          v10 = r_u32(((v9+=4u)-4u));
          w_u32(0x800FF058u,(r_u32(0x800FF058u)+(v10)));
          result = ((sint32)(r_u32(v9)));
        }
        while ((((sint32)(r_u32(v9))) != -1));
      }
      break;
    }
    while (r_u32(((v4+=1u)-1u)))
      ;

    v12 = ((uint32)((((uint32)((v4+(2)*1u))) & 0xFFFFFFFC)));
    if ((r_u32(v12) != -1))
    {
      v13 = (v12+(1)*4u);
      while ((r_u32(((v13+=4u)-4u)) != -1))
        ;

      v12 = (v13-(1)*4u);
    }
    v4 = (v12+(1)*4u);
  }

  w_u32(0x800FF054u,0);
  return result;
}


/* TODO Missing host buffer adapter xport_draft_host_sub_8001C8E8_p2 */
/* TODO Missing host buffer adapter xport_draft_host_sub_8006613C_p1 */
uint32 sub_80065DE8(void)
{
  sint32 v0;
  sint32 result;
  uint32 v2;
  sint32 v3;
  sint32 v4;
  uint32 v5;
  uint32 v6;
  sint32 v7;
  uint32 v8;
  sint32 v9;
  sint8 v10;
  sint32 v11;
  sint8 v12;
  sint8 v13;
  v0 = 0;
  while (1)
  {
    result = (((sint32)(v0)) < r_u32(0x800FF628u));
    if ((((sint32)(v0)) >= r_u32(0x800FF628u)))
      return result;
    v2 = r_u32(((uint32)(((uint32)(((uint32)(4) * (uint32)(v0))) + (uint32)(r_u32(0x800FF624u))))));
    v3 = ((sint16)(r_u16(v2)));
    if ((v3 == 6))
    {
      v5 = ((uint32)((v2+(((uint32)(((sint16)(r_u16((v2+(1)*2u))))) + (uint32)(2)))*2u)));
      if (((((unsigned char)(v5)) & 2) != 0))
        v5 = ((uint32)((((uint32)(v5))+(2)*1u)));
      sub_80063B5C(((sint32)(r_u32(v5))),v0++,((sint32)((v5+(1)*4u))));
    }
    else
      if ((((sint32)(v3)) >= 7))
    {
      if ((v3 == 9))
      {
        LABEL_27:
        v6 = ((uint32)((v2+(((uint32)(((sint16)(r_u16((v2+(1)*2u))))) + (uint32)(2)))*2u)));

        if (((((unsigned char)(v6)) & 2) != 0))
          v6 = ((uint32)((((uint32)(v6))+(2)*1u)));
        sub_80063B5C(((sint32)(r_u32(v6))),v0++,((sint32)(0x800FF640u)));
      }
      else
        if ((((sint32)(v3)) >= 10))
      {
        if ((v3 == 500))
        {
          v7 = xport_draft_host_sub_8006613C_p1(&v11,v0);
          w_u32(0x800FF3ACu,0);
          v8 = ((uint32)(v7));
          v9 = sub_80032DC0(100);
          if (v9)
            xport_draft_host_sub_8001C8E8_p2(v9,&v11,v0,r_u16(v8),r_u16((v8+(1)*2u)),r_u16((v8+(2)*2u)),r_u16((v8+(3)*2u)),r_u8((((uint32)(v8))+(8)*1u)),r_u8((((uint32)(v8))+(10)*1u)),r_u8((((uint32)(v8))+(12)*1u)),r_u8((((uint32)(v8))+(14)*1u)),r_u8((((uint32)(v8))+(16)*1u)),r_u8((((uint32)(v8))+(18)*1u)));
          w_u32(0x800FF3ACu,1);
          LABEL_33:
          ++v0;

        }
        else
        {
          ++v0;
        }
      }
      else
      {
        if ((v3 == 7))
        {
          if (!(((unsigned char)(sub_80064180(1u,((uint32)((v2+(((uint32)(((sint16)(r_u16((v2+(3)*2u))))) + (uint32)(4)))*2u))))))))
            goto LABEL_33;
          goto LABEL_22;
        }
        ++v0;
      }
    }
    else
    {
      if ((v3 == 2))
        goto LABEL_27;
      if ((((sint32)(v3)) >= 3))
      {
        if ((v3 == 5))
        {
          if ((r_u16(((uint32)(xport_draft_host_sub_8006613C_p1(&v11,v0)))) != 1))
            goto LABEL_33;
          LABEL_22:
          v4 = sub_8002FED8(24);

          if (!v4)
            goto LABEL_33;
          sub_8006FBB0(v4,((unsigned short)(v0++)),-1);
        }
        else
        {
          ++v0;
        }
      }
      else
        if ((v3 == 1))
      {
        if (!(((unsigned char)(sub_80064180(1u,((uint32)((v2+(((uint32)(((sint16)(r_u16((v2+(3)*2u))))) + (uint32)(4)))*2u))))))))
          goto LABEL_33;
        sub_800641E8(v0++);
      }
      else
      {
        ++v0;
      }
    }
  }

}



uint32 sub_800901EC(void)
{
  uint32 result = (uint32)SpuSetReverb(1);
  w_u32(0x800FD1D0u, result);
  return result;
}


/* TODO Missing call adapter sub_8001F6A0 */
/* TODO Missing host buffer adapter xport_draft_host_sub_8001C8E8_p2 */
/* TODO Missing host buffer adapter xport_draft_host_sub_8006613C_p1 */
/* TODO Postincrement memory expressions may require ordering refinement */
uint32 sub_80064874(uint32 a1)
{
  sint32 v2;
  sint32 result;
  sint32 v4;
  sint32 v5;
  sint32 v6;
  uint32 v7;
  sint32 v8;
  sint8 v9;
  sint32 v10;
  sint8 v11;
  sint8 v12;
  v2 = ((sint16)(r_u16(r_u32(((uint32)(((uint32)(((uint32)(4) * (uint32)(a1))) + (uint32)(r_u32(0x800FF624u)))))))));
  if ((v2 == 6))
  {
    v4 = sub_80063C54(a1);
    v5 = r_u32(((uint32)(v4)));
    (w_u8(((uint32)(((uint32)(v4) + (uint32)(7)))),(r_u8(((uint32)(((uint32)(v4) + (uint32)(7)))))+1u)),r_u8(((uint32)(((uint32)(v4) + (uint32)(7))))));
    return sub_8006541C(v5,a1,0);
  }
  if ((((sint32)(v2)) < 7))
  {
    result = 5;
    if (((v2 != 1) && (v2 != 5)))
      return result;
    return ((sint32)(sub_800641E8(a1)));
  }
  if ((v2 == 10))
    return ((void)(a1),abort(),0u);
  result = 7;
  if ((((sint32)(v2)) < 11))
  {
    if ((v2 != 7))
      return result;
    return ((sint32)(sub_800641E8(a1)));
  }
  result = 501;
  if ((v2 == 501))
  {
    v6 = xport_draft_host_sub_8006613C_p1(&v10,a1);
    w_u32(0x800FF3ACu,0);
    v7 = ((uint32)(v6));
    v8 = sub_80032DC0(100);
    if (v8)
      xport_draft_host_sub_8001C8E8_p2(v8,&v10,a1,r_u16(v7),r_u16((v7+(1)*2u)),r_u16((v7+(2)*2u)),r_u16((v7+(3)*2u)),r_u8((((uint32)(v7))+(8)*1u)),r_u8((((uint32)(v7))+(10)*1u)),r_u8((((uint32)(v7))+(12)*1u)),r_u8((((uint32)(v7))+(14)*1u)),r_u8((((uint32)(v7))+(16)*1u)),r_u8((((uint32)(v7))+(18)*1u)));
    result = 1;
    w_u32(0x800FF3ACu,1);
  }
  return result;
}


