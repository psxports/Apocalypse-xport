#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
#include <stdlib.h>

uint32 sub_800325B0(uint32 a1, uint32 a2, uint32 a3, uint32 a4, uint32 a5, uint32 a6, uint32 a7, uint32 a8, uint32 a9, uint32 a10, uint32 a11)
{
  sint32 result;
  sint32 v19;
  sint32 v20;
  sub_800330F4(a1);
  result = a1;
  w_u32(((uint32)(((uint32)(a1) + (uint32)(68)))),0x800A1B20u);
  v19 = r_u32((a2+(1)*4u));
  v20 = r_u32((a2+(2)*4u));
  w_u32(((uint32)(((uint32)(a1) + (uint32)(24)))),r_u32(a2));
  w_u32(((uint32)(((uint32)(a1) + (uint32)(28)))),v19);
  w_u32(((uint32)(((uint32)(a1) + (uint32)(32)))),v20);
  w_u16(((uint32)(((uint32)(a1) + (uint32)(10)))),a3);
  w_u32(((uint32)(((uint32)(a1) + (uint32)(72)))),a4);
  w_u8(((uint32)(((uint32)(a1) + (uint32)(76)))),a5);
  w_u8(((uint32)(((uint32)(a1) + (uint32)(77)))),a6);
  w_u8(((uint32)(((uint32)(a1) + (uint32)(78)))),a7);
  w_u32(((uint32)(((uint32)(a1) + (uint32)(80)))),a8);
  w_u32(((uint32)(((uint32)(a1) + (uint32)(84)))),a9);
  w_u32(((uint32)(((uint32)(a1) + (uint32)(88)))),a10);
  w_u32(((uint32)(((uint32)(a1) + (uint32)(92)))),a11);
  return result;
}


/* TODO Missing call adapter xport_draft_missing_gte_adapter */
void sub_80085674(uint32 a1, uint32 a2, uint32 a3)
{
  uint32 xport_draft_asm_carrier; /* TODO GTE carrier */
  sint32 v11;
  sint32 v12;
  xport_draft_asm_carrier = ((sint32)(r_u32(a2)));
  xport_draft_asm_carrier = ((sint32)(r_u32((a2+(1)*4u))));
  xport_draft_asm_carrier = ((sint32)(r_u32((a2+(2)*4u))));
  xport_draft_asm_carrier = ((sint32)(r_u32((a2+(3)*4u))));
  xport_draft_asm_carrier = ((sint32)(r_u32((a2+(4)*4u))));
  (abort(),0u);
  xport_draft_asm_carrier = ((sint16)(r_u16((((uint32)(a2))+(9)*2u))));
  xport_draft_asm_carrier = ((sint16)(r_u16((((uint32)(a2))+(10)*2u))));
  xport_draft_asm_carrier = ((sint16)(r_u16((((uint32)(a2))+(11)*2u))));
  (abort(),0u);
  v11 = r_u32(((uint32)(((uint32)(a3) + (uint32)(4)))));
  v12 = r_u32(((uint32)(((uint32)(a3) + (uint32)(8)))));
  xport_draft_asm_carrier = r_u32(((uint32)(((uint32)(a3) + (uint32)(12)))));
  xport_draft_asm_carrier = r_u32(((uint32)(((uint32)(a3) + (uint32)(16)))));
  xport_draft_asm_carrier = (((unsigned short)((r_u32(((uint32)(a3))) ^ v11))) ^ v11);
  (abort(),0u);
  xport_draft_asm_carrier = (((r_u32(((uint32)(a3)))>>16)&65535u) | ((uint32)(v12) << (uint32)(16)));
  xport_draft_asm_carrier = ((xport_draft_asm_carrier>>16)&65535u);
  (abort(),0u);
  xport_draft_asm_carrier = (((unsigned short)((v11 ^ v12))) ^ v12);
  (abort(),0u);
  w_u16(a1,xport_draft_asm_carrier);
  w_u16((a1+(3)*2u),xport_draft_asm_carrier);
  w_u16((a1+(6)*2u),xport_draft_asm_carrier);
  (abort(),0u);
  w_u16((a1+(1)*2u),xport_draft_asm_carrier);
  w_u16((a1+(4)*2u),xport_draft_asm_carrier);
  w_u16((a1+(7)*2u),xport_draft_asm_carrier);
  (abort(),0u);
  w_u16((a1+(2)*2u),xport_draft_asm_carrier);
  w_u16((a1+(5)*2u),xport_draft_asm_carrier);
  w_u16((a1+(8)*2u),xport_draft_asm_carrier);
  xport_draft_asm_carrier = ((sint16)(r_u16(((uint32)(((uint32)(a3) + (uint32)(18)))))));
  xport_draft_asm_carrier = ((sint16)(r_u16(((uint32)(((uint32)(a3) + (uint32)(20)))))));
  xport_draft_asm_carrier = ((sint16)(r_u16(((uint32)(((uint32)(a3) + (uint32)(22)))))));
  (abort(),0u);
  w_u16((a1+(9)*2u),xport_draft_asm_carrier);
  w_u16((a1+(10)*2u),xport_draft_asm_carrier);
  w_u16((a1+(11)*2u),xport_draft_asm_carrier);
}



uint32 sub_800665CC(uint32 a1, uint32 a2, uint32 a3)
{
  uint32 v3;
  sint32 result;
  v3 = (0x800F863Cu+((a3 & 0xFFF))*4u);
  w_u32(a1,(((uint32)(((uint32)((((sint32)(((sint32)(r_u32(a2))))) >> 3)) * (uint32)(((sint16)(r_u16((((uint32)(v3))+(1)*2u))))))) + (uint32)(((uint32)((((sint32)(((sint32)(r_u32((a2+(2)*4u)))))) >> 3)) * (uint32)(((sint16)(r_u16(((uint32)(v3))))))))) >> 9));
  w_u32((a1+(1)*4u),((sint32)(r_u32((a2+(1)*4u)))));
  result = (((uint32)(((uint32)((((sint32)(((sint32)(r_u32((a2+(2)*4u)))))) >> 3)) * (uint32)(((sint16)(r_u16((((uint32)(v3))+(1)*2u))))))) - (uint32)(((uint32)((((sint32)(((sint32)(r_u32(a2))))) >> 3)) * (uint32)(((sint16)(r_u16(((uint32)(v3))))))))) >> 9);
  w_u32((a1+(2)*4u),result);
  return result;
}


/* TODO Missing call adapter indirect */
uint32 sub_800210D8(uint32 a1, uint32 a2)
{
  sint32 v4;
  sint32 result;
  v4 = r_u32(((uint32)(((uint32)(a1) + (uint32)(200)))));
  w_u32(((uint32)(((uint32)(a1) + (uint32)(68)))),0x800A12C8u);
  if (v4)
    ((void)(((sint32)(((uint32)(v4) + (uint32)(((sint16)(r_u16(((uint32)(((uint32)(r_u32(((uint32)(((uint32)(v4) + (uint32)(68)))))) + (uint32)(8)))))))))))),(void)(3),abort(),0u);
  sub_800342B0(a1,0);
  result = (a2 & 1);
  if (((a2 & 1) != 0))
    return sub_80032E30(a1);
  return result;
}



uint32 sub_80036C5C(uint32 a1, uint32 a2)
{
  sint32 result;
  sint32 v5;
  w_u32((a1+(17)*4u),0x800A1BB0u);
  result = sub_80036828(a1,0);
  if (((a2 & 1) != 0))
    return sub_80032E30(((sint32)(a1)));
  return result;
}


/* TODO Missing call adapter indirect */
uint32 sub_80036828(uint32 a1, uint32 a2)
{
  sint8 v3;
  sint32 v4;
  sint32 v5;
  sint32 v6;
  sint8 v7;
  sint32 v8;
  sint32 result;
  v3 = a2;
  v4 = 0;
  v5 = r_u32((a1+(18)*4u));
  w_u32((a1+(17)*4u),0x800A1BC8u);
  if ((((sint32)(v5)) > 0))
  {
    do
    {
      v6 = r_u32(((uint32)(((uint32)(((uint32)(4) * (uint32)(v4))) + (uint32)(r_u32((a1+(23)*4u)))))));
      a2 = 3;
      if (v6)
        ((void)(((sint32)(((uint32)(v6) + (uint32)(((sint16)(r_u16(((uint32)(((uint32)(r_u32(((uint32)(((uint32)(v6) + (uint32)(68)))))) + (uint32)(8)))))))))))),(void)(3),abort(),0u);
      ++v4;
    }
    while ((((sint32)(v4)) < r_u32((a1+(18)*4u))));
  }
  ((void)(a2),sub_8006BC20(r_u32((a1+(23)*4u))));
  ((void)(v7),sub_8006BC20(r_u32((a1+(22)*4u))));
  sub_80033138(a1,0);
  result = (v3 & 1);
  if (((v3 & 1) != 0))
    return sub_80032E30(((sint32)(a1)));
  return result;
}


/* TODO Missing call adapter indirect */
/* TODO Missing host buffer adapter xport_draft_host_sub_8005BBB0_p3 */
/* TODO Missing host buffer adapter xport_draft_host_sub_8006696C_p2 */
uint32 sub_8004FB00(uint32 a1, uint32 a2)
{
  sint32 v4;
  sint32 i;
  sint32 v6;
  sint32 v7;
  signed int v8;
  uint32 v9;
  sint32 v10;
  short v11;
  sint32 v12;
  short v13;
  sint32 v15;
  sint32 v16;
  sint32 v17;
  sint32 v18;
  sint32 v19;
  sint32 v20;
  v4 = sub_80066570(1024);
  for (i = 0; (((sint32)(i)) < 6); ++i)
  {
    if ((((r_u32(((uint32)(((uint32)(a1) + (uint32)(220))))) >= 0x800u) || !r_u32(0x800FF904u)) || sub_80066570(4)))
    {
      v9 = (0x800F863Cu+((v4 & 0xFFF))*4u);
      v15 = ((uint32)(((sint16)(r_u16(((uint32)(v9)))))) * (uint32)(((uint32)(sub_80066570(16)) + (uint32)(24))));
      v16 = ((uint32)(((uint32)(-32) - (uint32)(sub_80066570(16)))) << (uint32)(12));
      v17 = ((uint32)(((sint16)(r_u16((((uint32)(v9))+(1)*2u))))) * (uint32)(((uint32)(sub_80066570(16)) + (uint32)(24))));
    }
    else
    {
      v6 = r_u32(((uint32)(((uint32)(r_u32(0x800FF904u)) + (uint32)(8)))));
      v7 = r_u32(((uint32)(((uint32)(r_u32(0x800FF904u)) + (uint32)(12)))));
      v18 = r_u32(((uint32)(((uint32)(r_u32(0x800FF904u)) + (uint32)(4)))));
      v20 = v7;
      v19 = ((uint32)(v6) + (uint32)(0x80000));
      v8 = (((uint32)(xport_draft_host_sub_8006696C_p2(((uint32)(a1) + (uint32)(4)),&v18))) >> 7);
      v15 = (((sint32)(((uint32)(v18) - (uint32)(r_u32(((uint32)(((uint32)(a1) + (uint32)(4))))))))) / ((sint32)(v8)));
      v17 = (((sint32)(((uint32)(v20) - (uint32)(r_u32(((uint32)(((uint32)(a1) + (uint32)(12))))))))) / ((sint32)(v8)));
      v16 = ((uint32)((((sint32)(((uint32)(v19) - (uint32)(r_u32(((uint32)(((uint32)(a1) + (uint32)(8))))))))) / ((sint32)(v8)))) - (uint32)(((uint32)(v8) << (uint32)(13))));
    }
    if ((a2 == 5))
    {
      v10 = sub_800625AC(320);
      if (v10)
      {
        v11 = sub_80066570(r_u8(((uint32)(((uint32)(a1) + (uint32)(615))))));
        if (xport_draft_host_sub_8005BBB0_p3(v10,a1+4u,&v15,(uint16)(r_u8(a1+614u)+v11),r_u32(a1+8u)+((uint32)(sint16)r_u16(a1+456u)<<12),a2,24,r_u32(0x800FF62Cu)))
        {
          sub_800626F8(a1,0,0,0,0);
          v4 += 682;
          continue;
        }
      }
    }
    else
      if (((r_u16(((uint32)(((uint32)(a1) + (uint32)(390))))) & 0x180) != 0))
    {
      v12 = sub_800625AC(320);
      if (v12)
        goto LABEL_16;
    }
    else
    {
      v12 = sub_800625AC(320);
      if (v12)
      {
        LABEL_16:
        v13 = sub_80066570(r_u8(((uint32)(((uint32)(a1) + (uint32)(615))))));

        xport_draft_host_sub_8005BBB0_p3(v12,a1+4u,&v15,(uint16)(r_u8(a1+614u)+v13),r_u32(a1+8u)+((uint32)(sint16)r_u16(a1+456u)<<12),3,24,r_u32(0x800FF62Cu));
      }
    }
    v4 += 682;
  }

  return ((void)(((sint32)(((uint32)(a1) + (uint32)(((sint16)(r_u16(((uint32)(((uint32)(r_u32(((uint32)(((uint32)(a1) + (uint32)(68)))))) + (uint32)(16)))))))))))),abort(),0u);
}



uint32 sub_800650EC(uint32 a1)
{
  sint32 v1;
  uint32 v2;
  uint32 result;
  v1 = r_u16(a1);
  v2 = ((uint32)((a1+(1)*2u)));
  if ((((uint32)(v1)) >= 0x8A))
  {
    if ((v1 == 160))
      goto LABEL_70;
    if ((((sint32)(v1)) >= 161))
    {
      if ((v1 == 172))
        goto LABEL_70;
      if ((((sint32)(v1)) >= 173))
      {
        if ((v1 == 175))
          return ((uint32)(v2));
        if ((((sint32)(v1)) >= 176))
        {
          if ((v1 == 177))
            goto LABEL_70;
          if ((((sint32)(v1)) >= 177))
          {
            result = ((uint32)(v2));
            if ((v1 == 0xFFFF))
              return 0;
            return result;
          }
          return sub_800650B4(v2);
        }
        if ((v1 == 173))
          return ((uint32)(v2));
      }
      else
        if ((v1 != 167))
      {
        if ((((sint32)(v1)) < 168))
        {
          result = ((uint32)(v2));
          if ((((sint32)(v1)) < 163))
            return result;
          v2 += (2)*1u;
          return ((uint32)(v2));
        }
        if ((((sint32)(v1)) >= 171))
          return ((uint32)((((uint32)((v2+(3)*1u))) & 0xFFFFFFFC)) + (uint32)(10));
        goto LABEL_70;
      }
    }
    else
    {
      if ((((sint32)(v1)) >= 147))
      {
        if ((v1 == 149))
          return ((uint32)(v2));
        if ((((sint32)(v1)) >= 149))
        {
          if ((((sint32)(v1)) < 157))
          {
            result = ((uint32)(v2));
            if ((((sint32)(v1)) < 153))
              return result;
            v2 += (2)*1u;
          }
          return ((uint32)(v2));
        }
        goto LABEL_70;
      }
      if ((((sint32)(v1)) < 143))
      {
        if ((v1 == 140))
          return sub_800650B4(v2);
        if ((((sint32)(v1)) >= 141))
        {
          if ((v1 != 141))
            return sub_800650B4(v2);
          v2 += (2)*1u;
          goto LABEL_68;
        }
        if ((v1 == 138))
          goto LABEL_70;
        result = ((uint32)(v2));
        if ((v1 != 139))
          return result;
      }
    }
    LABEL_61:
    v2 += (4)*1u;

    return ((uint32)(v2));
  }
  if ((((sint32)(v1)) >= 136))
    return ((uint32)(v2));
  if ((v1 == 115))
    return sub_800650B4(v2);
  if ((((sint32)(v1)) >= 116))
  {
    if ((v1 == 129))
      return ((uint32)(v2));
    if ((((sint32)(v1)) < 130))
    {
      if ((((sint32)(v1)) >= 123))
      {
        result = ((uint32)(v2));
        if ((((sint32)(v1)) < 126))
          return result;
      }
      else
      {
        if ((((sint32)(v1)) >= 121))
          return ((uint32)(v2));
        result = ((uint32)(v2));
        if ((v1 != 119))
          return result;
      }
      return sub_800650B4(v2);
    }
    if ((((sint32)(v1)) < 133))
    {
      if ((((sint32)(v1)) < 131))
      {
        v2 += (4)*1u;
        return ((uint32)(v2));
      }
      LABEL_70:
      v2 += (2)*1u;

      return ((uint32)(v2));
    }
    if ((v1 == 134))
      goto LABEL_70;
    if ((((sint32)(v1)) < 135))
    {
      LABEL_68:
      while ((r_u16(((uint32)(v2))) != 255))
        v2 = ((uint32)(((uint32)((((uint32)((v2+(3)*1u))) & 0xFFFFFFFC)) + (uint32)(24))));


      goto LABEL_70;
    }
    goto LABEL_61;
  }
  if ((v1 == 13))
    goto LABEL_70;
  if ((((sint32)(v1)) < 14))
  {
    if (((((sint32)(v1)) < 6) && (((sint32)(v1)) < 3)))
    {
      result = ((uint32)(v2));
      if ((v1 != 2))
        return result;
      while (r_u8(v2))
        v2 = ((uint32)(sub_800650B4(v2)));

      v2 += (2)*1u;
    }
    return ((uint32)(v2));
  }
  if ((v1 == 104))
  {
    v2 += (6)*1u;
    return ((uint32)(v2));
  }
  result = ((uint32)(v2));
  if ((((sint32)(v1)) >= 105))
  {
    result = ((uint32)(v2));
    if ((((sint32)(v1)) < 107))
    {
      v2 += (2)*1u;
      return ((uint32)(v2));
    }
  }
  return result;
}


/* TODO Missing call adapter indirect */
/* TODO Postincrement memory expressions may require ordering refinement */
uint32 sub_8004E508(uint32 a1, uint32 a2)
{
  sint32 v4;
  sint32 v5;
  sint32 v6;
  sint32 v7;
  sint32 v8;
  sint32 v9;
  sint32 result;
  w_u32(((uint32)(((uint32)(a1) + (uint32)(68)))),0x800A2A68u);
  sub_80062A64(a1,0x800FF4E8u);
  v4 = r_u32(((uint32)(((uint32)(a1) + (uint32)(608)))));
  if (v4)
    ((void)(((sint32)(((uint32)(v4) + (uint32)(((sint16)(r_u16(((uint32)(((uint32)(r_u32(((uint32)(((uint32)(v4) + (uint32)(68)))))) + (uint32)(8)))))))))))),(void)(3),abort(),0u);
  v5 = r_u32(((uint32)(((uint32)(a1) + (uint32)(620)))));
  v6 = 3;
  if (v5)
    ((void)(((sint32)(((uint32)(v5) + (uint32)(((sint16)(r_u16(((uint32)(((uint32)(r_u32(((uint32)(v5)))) + (uint32)(8)))))))))))),abort(),0u);
  sub_8004BD94(a1,v6);
  v7 = r_u16(((uint32)(((uint32)(a1) + (uint32)(390)))));
  (w_u32(0x800FF4DCu,(r_u32(0x800FF4DCu)-1u)),r_u32(0x800FF4DCu));
  if ((v7 == 128))
    (w_u32(0x800FF4E4u,(r_u32(0x800FF4E4u)-1u)),r_u32(0x800FF4E4u));
  v8 = a1;
  if ((r_u16(((uint32)(((uint32)(a1) + (uint32)(390))))) == 256))
  {
    v9 = r_u32(((uint32)(((uint32)(a1) + (uint32)(480)))));
    if (v9)
    {
      sub_8006A294(v9);
      w_u32(((uint32)(((uint32)(a1) + (uint32)(480)))),0);
    }
    v8 = a1;
  }
  sub_8004B868(v8,0);
  result = (a2 & 1);
  if (((a2 & 1) != 0))
    return sub_80062608(a1);
  return result;
}



uint32 sub_800336D8(uint32 a1, uint32 a2)
{
  uint32 result;
  sint32 v5;
  w_u32((a1+(17)*4u),0x800A1D50u);
  sub_80032E7C(a1,0x800FF444u);
  result = sub_80032FB8(((sint32)(a1)),0);
  if (((a2 & 1) != 0))
    return ((uint32)(sub_80032E30(((sint32)(a1)))));
  return result;
}


/* TODO Missing call adapter indirect */
/* TODO Postincrement memory expressions may require ordering refinement */
uint32 sub_8003C490(uint32 a1, uint32 a2)
{
  sint32 v3;
  sint32 v4;
  sint32 v5;
  v3 = r_u16(((uint32)(((uint32)(a1) + (uint32)(438)))));
  (w_u8(((uint32)(((uint32)(a1) + (uint32)(383)))),(r_u8(((uint32)(((uint32)(a1) + (uint32)(383)))))+1u)),r_u8(((uint32)(((uint32)(a1) + (uint32)(383))))));
  if (v3)
  {
    v4 = ((uint32)(r_u16(((uint32)(((uint32)(a1) + (uint32)(218)))))) - (uint32)(a2));
    if ((((sint32)(((sint16)(r_u16(((uint32)(((uint32)(a1) + (uint32)(218))))))))) > 0))
    {
      w_u16(((uint32)(((uint32)(a1) + (uint32)(218)))),v4);
      if ((((uint32)(v4) << (uint32)(16)) > 0))
      {
        /* MIPS 8003C4D0 and 8003C52C define the fifth argument as 255 */
        sub_800626F8(a1,3,255,255,255);
      }
      else
      {
        ((void)(((sint32)(((uint32)(a1) + (uint32)(((sint16)(r_u16(((uint32)(((uint32)(r_u32(((uint32)(((uint32)(a1) + (uint32)(68)))))) + (uint32)(16)))))))))))),abort(),0u);
        v5 = sub_80066570(2);
        sub_80069EF4(((uint32)(v5) + (uint32)(1)),((uint32)(a1) + (uint32)(4)),0);
        sub_80022318(a1,0,1);
        w_u16(((uint32)(a1)),(r_u16(((uint32)(a1)))|(1u)));
      }
    }
  }
  return 1;
}


/* TODO Missing call adapter indirect */
/* TODO Missing host buffer adapter xport_draft_host_sub_8001C158_p23 */
/* TODO Missing host buffer adapter xport_draft_host_sub_8005CEE0_p2 */
/* TODO Missing host buffer adapter xport_draft_host_sub_80066B8C_p1 */
/* TODO Missing host buffer adapter xport_draft_host_sub_8007CC10_p1 */
uint32 sub_80053358(uint32 a1)
{
  sint32 result;
  sint32 v3;
  sint32 v4;
  unsigned short v5;
  sint32 v6;
  unsigned short v7;
  sint32 v8;
  sint32 v9;
  unsigned short v10;
  sint32 v11;
  unsigned short v12;
  sint32 v13;
  sint32 v14;
  unsigned short v15;
  sint32 v16;
  unsigned short v17;
  sint32 v18;
  sint32 v19;
  sint32 v20;
  unsigned short v21;
  sint32 v22;
  sint32 v23;
  sint32 v24;
  unsigned char v25;
  sint32 v26[3];
  unsigned char v27;
  unsigned char v28;
  char v29[8];
  sint32 v30[3];
  if (((r_u32(((uint32)(((uint32)(a1) + (uint32)(396))))) & 0x40) != 0))
  {
    w_u16(((uint32)(((uint32)(a1) + (uint32)(18)))),((uint32)(3072) - (uint32)(ratan2(((uint32)(r_u32(((uint32)(((uint32)(a1) + (uint32)(504)))))) - (uint32)(r_u32(((uint32)(((uint32)(a1) + (uint32)(12))))))),((uint32)(r_u32(((uint32)(((uint32)(a1) + (uint32)(496)))))) - (uint32)(r_u32(((uint32)(((uint32)(a1) + (uint32)(4)))))))))));
    sub_80063038(a1,1,0,-1);
    xport_draft_host_sub_8007CC10_p1(v26,a1,0);
    xport_draft_host_sub_80066B8C_p12(v29,v26,a1+496u);
  }
  else
  {
    result = r_u32(0x800FF5A0u);
    if (!r_u32(0x800FF5A0u))
      return result;
    w_u16(((uint32)(((uint32)(a1) + (uint32)(18)))),((uint32)(3072) - (uint32)(ratan2(((uint32)(r_u32(((uint32)(((uint32)(r_u32(0x800FF5A0u)) + (uint32)(12)))))) - (uint32)(r_u32(((uint32)(((uint32)(a1) + (uint32)(12))))))),((uint32)(r_u32(((uint32)(((uint32)(r_u32(0x800FF5A0u)) + (uint32)(4)))))) - (uint32)(r_u32(((uint32)(((uint32)(a1) + (uint32)(4)))))))))));
    sub_80063038(a1,1,0,-1);
    xport_draft_host_sub_8007CC10_p1(v26,a1,0);
    xport_draft_host_sub_8005CEE0_p2(r_u32(0x800FF5A0u),v30,9);
    v30[1] = (sint32)((uint32)v30[1] + 0x10000u);
    xport_draft_host_sub_80066B8C_p123(v29,v26,v30);
  }

  v3 = r_u16(((uint32)(((uint32)(a1) + (uint32)(390)))));
  if ((v3 == 4))
  {
    v4 = sub_80032DC0(108);
    if (v4)
    {
      v5 = sub_8004BDFC(a1,1u,1u);
      v6 = sub_8004B914(a1,v5);
      v8 = ((unsigned short)(sub_8004BDFC(a1,1u,4u)));
      v7 = sub_8004BDFC(a1,1u,2u);
      xport_draft_host_sub_8001C158_p23(v4,v26,v29,v6,v8,v7,16,240,240,240,32,0x80u,0x40u);
    }
  }
  else
    if ((v3 == 256))
  {
    v9 = sub_80032DC0(108);
    if (v9)
    {
      v10 = sub_8004BDFC(a1,1u,1u);
      v11 = sub_8004B914(a1,v10);
      v13 = ((unsigned short)(sub_8004BDFC(a1,1u,4u)));
      v12 = sub_8004BDFC(a1,1u,2u);
      xport_draft_host_sub_8001C158_p23(v9,v26,v29,v11,v13,v12,16,240,240,240,128,0x20u,0x20u);
    }
    xport_draft_host_sub_8007CC10_p1(v26,a1,1);
    v14 = sub_80032DC0(108);
    if (v14)
    {
      v15 = sub_8004BDFC(a1,1u,1u);
      v16 = sub_8004B914(a1,v15);
      v18 = ((unsigned short)(sub_8004BDFC(a1,1u,4u)));
      v17 = sub_8004BDFC(a1,1u,2u);
      xport_draft_host_sub_8001C158_p23(v14,v26,v29,v16,v18,v17,16,240,240,240,128,0x20u,0x20u);
    }
  }
  else
  {
    v19 = r_u32(((uint32)(((uint32)(a1) + (uint32)(620)))));
    if (v19)
    {
      ((void)(((sint32)(((uint32)(v19) + (uint32)(((sint16)(r_u16(((uint32)(((uint32)(r_u32(((uint32)(v19)))) + (uint32)(16)))))))))))),(void)(v26),(void)(v29),abort(),0u);
    }
    else
    {
      v20 = sub_80032DC0(108);
      if (v20)
      {
        v21 = sub_8004BDFC(a1,1u,1u);
        v22 = sub_8004B914(a1,v21);
        v23 = ((unsigned short)(sub_8004BDFC(a1,1u,4u)));
        v24 = ((unsigned short)(sub_8004BDFC(a1,1u,2u)));
        xport_draft_host_sub_8001C158_p23(v20,v26,v29,v22,v23,v24,16,240,240,240,64,0x20u,0x80u);
      }
    }
  }
  return sub_80069EF4(20,((uint32)(a1) + (uint32)(4)),2);
}


/* TODO Missing call adapter SLOWORD */
/* TODO Missing call adapter indirect */
/* TODO Missing host buffer adapter xport_draft_host_sub_800666DC_p1 */
/* TODO Missing host buffer adapter xport_draft_host_sub_8007C398_p13 */
uint32 sub_8001C598(uint32 a1)
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
  short v13;
  sint32 v14;
  sint32 v15;
  sint32 result;
  int v17[4];
  char v18[16];
  char v19[16];
  v2 = ((uint32)(a1) + (uint32)(24));
  v3 = r_u32(((uint32)(((uint32)(a1) + (uint32)(28)))));
  v4 = r_u32(((uint32)(((uint32)(a1) + (uint32)(32)))));
  v17[0] = r_u32(((uint32)(((uint32)(a1) + (uint32)(24)))));
  v17[1] = v3;
  v17[2] = v4;
  sub_8006C0B8(((uint32)(a1) + (uint32)(24)),((uint32)(a1) + (uint32)(36)));
  sub_80032EE4(r_u32(((uint32)(((uint32)(a1) + (uint32)(76))))),v2);
  w_u16(((uint32)(((uint32)(r_u32(((uint32)(((uint32)(a1) + (uint32)(76)))))) + (uint32)(96)))),(r_u16(((uint32)(((uint32)(r_u32(((uint32)(((uint32)(a1) + (uint32)(76)))))) + (uint32)(96)))))+(140)));
  v5 = r_u32(((uint32)(((uint32)(a1) + (uint32)(84)))));
  v6 = ((uint32)(r_u32(((uint32)(((uint32)(a1) + (uint32)(92)))))) + (uint32)(r_u32(((uint32)(((uint32)(a1) + (uint32)(88)))))));
  w_u32(((uint32)(((uint32)(a1) + (uint32)(92)))),v6);
  v7 = ((uint32)((((uint32)(6) * (uint32)(v5)) / 16)) * (uint32)(((void)(r_u32((0x800F863Cu+((v6 & 0xFFF))*4u))),abort(),0u)));
  v8 = (((sint32)(v7)) >> 12);
  if ((((sint32)(v7)) < 0))
    v8 = (((sint32)(((uint32)(v7) + (uint32)(4095)))) >> 12);
  v9 = ((sint32)(((uint32)(v5) + (uint32)(v8))));
  sub_80034918(r_u32(((uint32)(((uint32)(a1) + (uint32)(76))))),v9);
  sub_80034958(r_u32(((uint32)(((uint32)(a1) + (uint32)(76))))),v9);
  sub_800349D0(r_u32(((uint32)(((uint32)(a1) + (uint32)(76))))),0,(((uint32)(42) * (uint32)(v9)) / 16));
  v10 = xport_draft_host_sub_8007C398_p13(v17,v2,v18,r_u32(0x800FF5A0u));
  if (v10)
  {
    xport_draft_host_sub_800666DC_p1(v19,((uint32)(a1) + (uint32)(36)));
    ((void)(((sint32)(((uint32)(v10) + (uint32)(((sint16)(r_u16(((uint32)(((uint32)(r_u32(((uint32)(((uint32)(v10) + (uint32)(68)))))) + (uint32)(48)))))))))))),(void)(r_u32(((uint32)(((uint32)(a1) + (uint32)(72)))))),(void)(v19),(void)(29),abort(),0u);
    v11 = sub_80032DC0(124);
    if (v11)
      /* MIPS 8001C728..8001C77C defines all sixteen stack arguments */
      sub_8001CF9C(v11,v10+4u,4,255,100,0,2,255,100,0,20,80,0,0,30,30,15,15,0,0);
    return sub_80032ED8(a1);
  }
  v12 = r_u32(((uint32)(((uint32)(a1) + (uint32)(80)))));
  v13 = ((uint32)(r_u16(((uint32)(((uint32)(a1) + (uint32)(8)))))) + (uint32)(1));
  w_u16(((uint32)(((uint32)(a1) + (uint32)(8)))),v13);
  if ((((sint32)(v12)) < ((sint32)(v13))))
  {
    sub_80034B7C(r_u32(((uint32)(((uint32)(a1) + (uint32)(76))))),10);
    sub_80034B9C(r_u32(((uint32)(((uint32)(a1) + (uint32)(76))))),10);
  }
  if ((((sint32)(((sint16)(r_u16(((uint32)(((uint32)(a1) + (uint32)(8))))))))) >= ((sint32)(((sint32)(r_u16(((uint32)(((uint32)(a1) + (uint32)(10)))))))))))
  {
    v14 = sub_80032DC0(124);
    if (v14)
      /* MIPS 8001C7F4..8001C860 defines RGB and sixteen stack arguments */
      sub_8001CF9C(v14,v2,3,r_u8(a1+104u),r_u8(a1+105u),r_u8(a1+106u),2,r_u8(a1+104u),r_u8(a1+105u),r_u8(a1+106u),20,40,0,0,50,50,25,25,0,0);
    sub_80032ED8(a1);
    sub_80069EF4(24,v2,0);
    v15 = r_u32(((uint32)(((uint32)(a1) + (uint32)(96)))));
    if ((v15 && ((v15 & 3) == 0)))
      sub_8001E740(v15,r_u32(a1+100u),r_u32(a1+72u));
  }
  result = (r_u32(((uint32)(((uint32)(r_u32(((uint32)(((uint32)(a1) + (uint32)(76)))))) + (uint32)(88))))) & 0xFFFFFF);
  if (!result)
    return sub_80032ED8(a1);
  return result;
}



uint32 sub_80064E10(uint32 a1, uint32 a2)
{
  sint32 result;
  for (; a1; a1 = r_u32(((uint32)(((uint32)(a1) + (uint32)(28))))))
  {
    result = r_u16(((uint32)(((uint32)(a1) + (uint32)(214)))));
    if ((result == a2))
    {
      result = (r_u16(((uint32)(((uint32)(a1) + (uint32)(76))))) | 1);
      w_u16(((uint32)(((uint32)(a1) + (uint32)(76)))),result);
    }
  }

  return result;
}


/* TODO Missing call adapter sub_8001F718 */
/* TODO Missing call adapter sub_8006E0D0 */
uint32 sub_80064C1C(uint32 a1, uint32 a2)
{
  uint32 v3;
  sint32 v4;
  uint32 v5;
  sint32 v6;
  sint32 result;
  unsigned short v8;
  sint32 v9;
  uint32 v10;
  sint32 v11;
  uint32 v12;
  uint32 v13;
  uint32 v14;
  sint32 v15;
  v3 = ((uint32)(sub_80066088(a1)));
  v4 = r_u16(v3);
  v5 = (v3+(1)*2u);
  v6 = 0;
  LABEL_2:
  result = (((sint32)(v6)) < ((sint32)(v4)));

  while (result)
  {
    v8 = r_u16(v5);
    v9 = r_u16(v5);
    v10 = r_u32(((uint32)(((uint32)(((uint32)(4) * (uint32)(v9))) + (uint32)(r_u32(0x800FF624u))))));
    v11 = ((sint16)(r_u16(v10)));
    (v5+=2u);
    if ((v11 == 9))
    {
      v12 = (v10+(1)*2u);
      goto LABEL_15;
    }
    if ((((sint32)(v11)) < 10))
    {
      if ((v11 == 1))
      {
        sub_80064A68(v9,r_u32(0x800FF4E8u),a2);
        sub_80064A68(v9,r_u32(0x800FF5DCu),a2);
        ++v6;
        goto LABEL_2;
      }
      v12 = (v10+(1)*2u);
      if ((v11 != 2))
      {
        ++v6;
        goto LABEL_2;
      }
      LABEL_15:
      v13 = (v12+(((uint32)(((sint16)(r_u16(v12)))) + (uint32)(1)))*2u);

      if (((((unsigned char)(v13)) & 2) != 0))
        (v13+=2u);
      v14 = ((uint32)(((void)(r_u32(((uint32)(v13)))),abort(),0u)));
      if (v14)
      {
        if ((a2 == 1))
        {
          sub_8001E740(((sint32)(v14)),0,0xFFFF);
          ++v6;
          goto LABEL_2;
        }
        w_u16(v14,(r_u16(v14)|(1u)));
      }
      goto LABEL_28;
    }
    if ((v11 == 10))
    {
      ((void)(v9),abort(),0u);
      goto LABEL_28;
    }
    if ((((sint32)(v11)) >= 502))
      goto LABEL_28;
    if ((((sint32)(v11)) < 500))
    {
      ++v6;
      goto LABEL_29;
    }
    v15 = r_u32(0x800FF434u);
    if (r_u32(0x800FF434u))
    {
      while (((r_u8(((uint32)(((uint32)(v15) + (uint32)(67))))) != 2) || (r_u16(((uint32)(((uint32)(v15) + (uint32)(10))))) != v8)))
      {
        v15 = r_u32(((uint32)(((uint32)(v15) + (uint32)(4)))));
        if (!v15)
        {
          ++v6;
          goto LABEL_2;
        }
      }

      ++v6;
      sub_80032ED8(v15);
      result = (((sint32)(v6)) < ((sint32)(v4)));
    }
    else
    {
      LABEL_28:
      ++v6;

      LABEL_29:
      result = (((sint32)(v6)) < ((sint32)(v4)));

    }
  }

  return result;
}


/* TODO Missing call adapter indirect */
uint32 sub_80064A68(uint32 a1, uint32 a2, uint32 a3)
{
  sint32 i;
  sint32 result;
  for (i = a2; i; i = r_u32(((uint32)(((uint32)(i) + (uint32)(28))))))
  {
    result = r_u16(((uint32)(((uint32)(i) + (uint32)(214)))));
    if ((result == a1))
    {
      result = 1;
      if (a3)
      {
        if ((a3 == 1))
          result = ((void)(((sint32)(((uint32)(i) + (uint32)(((sint16)(r_u16(((uint32)(((uint32)(r_u32(((uint32)(((uint32)(i) + (uint32)(68)))))) + (uint32)(48)))))))))))),(void)(((sint16)(r_u16(((uint32)(((uint32)(i) + (uint32)(218)))))))),(void)(0x800A71CCu),(void)(0),abort(),0u);
      }
      else
      {
        result = ((void)(((sint32)(((uint32)(i) + (uint32)(((sint16)(r_u16(((uint32)(((uint32)(r_u32(((uint32)(((uint32)(i) + (uint32)(68)))))) + (uint32)(16)))))))))))),abort(),0u);
      }
    }
  }

  return result;
}



uint32 sub_80061DC8(uint32 a1, uint32 a2, uint32 a3)
{
  sint32 v6;
  uint32 v7;
  sint32 v8;
  sint32 v9;
  sint32 v10;
  uint32 v11;
  sint32 v12;
  short v13;
  sint32 result;
  sub_80062924(a1);
  v6 = r_u32(0x800FF62Cu);
  w_u32(((uint32)(((uint32)(a1) + (uint32)(68)))),0x800A3224u);
  sub_800626C8(a1,v6);
  sub_80062A38(a1,0x800FF4E8u);
  v7 = ((uint32)(sub_80062B0C(a1,a2)));
  v8 = r_u32(((uint32)(((uint32)(a1) + (uint32)(8)))));
  v9 = r_u32(((uint32)(((uint32)(a1) + (uint32)(12)))));
  w_u32(((uint32)(((uint32)(a1) + (uint32)(296)))),r_u32(((uint32)(((uint32)(a1) + (uint32)(4))))));
  w_u32(((uint32)(((uint32)(a1) + (uint32)(300)))),v8);
  w_u32(((uint32)(((uint32)(a1) + (uint32)(304)))),v9);
  v10 = r_u8(((uint32)(((uint32)(a1) + (uint32)(27)))));
  w_u16(((uint32)(((uint32)(a1) + (uint32)(16)))),r_u16(v7));
  w_u16(((uint32)(((uint32)(a1) + (uint32)(18)))),r_u16((v7+(1)*2u)));
  v11 = ((uint32)((((uint32)(((uint32)(v7))) + (uint32)(9)) & 0xFFFFFFFC)));
  w_u16(((uint32)(((uint32)(a1) + (uint32)(20)))),r_u16((v7+(2)*2u)));
  v12 = ((sint32)(r_u32(v11)));
  w_u32(((uint32)(((uint32)(a1) + (uint32)(332)))),((sint32)(r_u32(v11))));
  w_u16(((uint32)(((uint32)(a1) + (uint32)(22)))),sub_8006E080(v12,v10));
  w_u32(((uint32)(((uint32)(a1) + (uint32)(328)))),r_u32(((uint32)((((uint32)(((uint32)(v7))) + (uint32)(13)) & 0xFFFFFFFC)))));
  sub_800667CC(((uint32)(a1) + (uint32)(308)),-128,((uint32)(a1) + (uint32)(16)));
  sub_8006C0B8(((uint32)(a1) + (uint32)(308)),((uint32)(a1) + (uint32)(4)));
  v13 = r_u16(((uint32)(((uint32)(a1) + (uint32)(78)))));
  result = a1;
  w_u16(((uint32)(((uint32)(a1) + (uint32)(212)))),0);
  w_u32(((uint32)(((uint32)(a1) + (uint32)(320)))),0);
  w_u16(((uint32)(((uint32)(a1) + (uint32)(214)))),a3);
  w_u16(((uint32)(((uint32)(a1) + (uint32)(78)))),(v13 & 0xFFEF));
  return result;
}


/* TODO Missing host buffer adapter xport_draft_host_sub_8006C0B8_p2 */
/* TODO Missing host buffer adapter xport_draft_host_sub_8006C564_p13 */
uint32 sub_8005DDF0(uint32 a1, uint32 a2, uint32 a3, uint32 a4)
{
  sint32 result;
  sint32 v9;
  sint32 v10;
  sint32 v11;
  sint32 v12;
  int v13[2];
  unsigned char v14;
  sint32 v15;
  sint32 v16;
  if (r_u32(0x800FF31Cu))
    return 0;
  result = 0;
  if ((((sint32)(((sint16)(r_u16(((uint32)(((uint32)(a1) + (uint32)(218))))))))) <= 0))
    return result;
  if (sub_80062F48(a1))
    return 0;
  result = 0;
  if (!(r_u32(((uint32)(((uint32)(a1) + (uint32)(452)))))))
  {
    result = 0;
    if (!(r_u32(((uint32)(((uint32)(a1) + (uint32)(456)))))))
    {
      v16 = 1;
      xport_draft_host_sub_8006C564_p13(v13,a3,&v16);
      xport_draft_host_sub_8006C0B8_p2(((uint32)(a1) + (uint32)(104)),v13);
      if ((((sint32)(a2)) >= 2))
      {
        if ((((sint32)(((sint16)(r_u16(((uint32)(((uint32)(a1) + (uint32)(218))))))))) < (r_u16(0x800EC526u) / 5)))
          a2 >>= 1;
        if (!r_u32(0x800FF384u))
          a2 >>= 1;
      }
      v9 = ((uint32)(r_u16(((uint32)(((uint32)(a1) + (uint32)(218)))))) - (uint32)(a2));
      w_u16(((uint32)(((uint32)(a1) + (uint32)(218)))),v9);
      if ((((uint32)(v9) << (uint32)(16)) > 0))
      {
        w_u8(((uint32)(((uint32)(a1) + (uint32)(465)))),6);
        if (((a4 == 5) || (a4 == 31)))
        {
          w_u8(((uint32)(((uint32)(a1) + (uint32)(466)))),90);
          /* MIPS 8005DF74 stores zero as the fifth argument */
          sub_800626F8(a1,90,255,0,0);
          w_u16(((uint32)(((uint32)(a1) + (uint32)(180)))),10240);
          v10 = r_u8(((uint32)(((uint32)(a1) + (uint32)(466)))));
          w_u16(((uint32)(((uint32)(a1) + (uint32)(182)))),10);
          w_u32(((uint32)(((uint32)(a1) + (uint32)(176)))),(((((uint32)(2) * (uint32)(v10)) | ((uint32)(v10) << (uint32)(9))) | 0x2000000) | ((uint32)(v10) << (uint32)(17))));
        }
        if (r_u32(0x800FF2FCu))
        {
          sub_8007011C(0,4,0,1);
          sub_8007011C(1,4,1,255);
        }
        sub_8001BE78(0x80u,0,0,8u,0,0);
        v11 = sub_80066570(4);
        sub_80069DF0(((uint32)(v11) + (uint32)(4)),0x2000,0);
        result = 1;
        if (!(r_u8(((uint32)(((uint32)(a1) + (uint32)(26)))))))
        {
          v12 = sub_80066570(4);
          sub_80063038(a1,6,r_u8((((uint32)(0x800FF5CCu))+(((uint32)(2) * (uint32)(v12)))*1u)),r_u8(((((uint32)(0x800FF5CCu))+(((uint32)(2) * (uint32)(v12)))*1u)+(1)*1u)));
          return 1;
        }
      }
      else
      {
        w_u16(((uint32)(((uint32)(a1) + (uint32)(218)))),0);
        sub_8005E708(a1);
        sub_80070288(0,0);
        sub_80070288(0,1);
        return 1;
      }
    }
  }
  return result;
}


/* TODO Missing host buffer adapter xport_draft_host_sub_8006BF04_p1 */
/* TODO Missing host buffer adapter xport_draft_host_sub_8006C0B8_p2 */
/* TODO Missing host buffer adapter xport_draft_host_sub_8006C3AC_p1 */
/* TODO Missing host buffer adapter xport_draft_host_sub_8006C47C_p13 */
/* TODO Missing host buffer adapter xport_draft_host_sub_8006C4EC_p123 */
/* TODO Missing host buffer adapter xport_draft_host_sub_8006C564_p123 */
/* TODO Missing host buffer adapter xport_draft_host_sub_8006C5C4_p123 */
uint32 sub_8005E07C(uint32 a1, uint32 a2)
{
  char v5[4];
  sint32 v6;
  char v7[16];
  char v8[16];
  char v9[16];
  char v10[16];
  sint32 v11;
  sint32 v12;
  sint32 v13;
  xport_draft_host_sub_8006C3AC_p1(v5,((uint32)(a1) + (uint32)(4)),((uint32)(a2) + (uint32)(4)));
  v6 = 0;
  v12 = xport_draft_host_sub_8006BF04_p1(v5);
  if (!v12)
    v12 = 1;
  v11 = 12;
  xport_draft_host_sub_8006C564_p123(v10,v5,&v11);
  xport_draft_host_sub_8006C47C_p13(v9,((uint32)(a2) + (uint32)(208)),v10);
  xport_draft_host_sub_8006C4EC_p123(v8,v9,&v12);
  v13 = 12;
  xport_draft_host_sub_8006C5C4_p123(v7,v8,&v13);
  return xport_draft_host_sub_8006C0B8_p2(((uint32)(a1) + (uint32)(104)),v7);
}


/* TODO Missing host buffer adapter xport_draft_host_sub_800667CC_p1 */
/* TODO Missing host buffer adapter xport_draft_host_sub_8006C0B8_p2 */
uint32 sub_80062044(uint32 a1)
{
  sint32 v2;
  sint32 result;
  char v4[16];
  v2 = sub_80066088(r_u16(((uint32)(((uint32)(a1) + (uint32)(214))))));
  sub_80064A08(v2);
  sub_80069DF0(47,0x2000,0);
  xport_draft_host_sub_800667CC_p1(v4,16,((uint32)(a1) + (uint32)(16)));
  xport_draft_host_sub_8006C0B8_p2(((uint32)(a1) + (uint32)(4)),v4);
  result = sub_8006E080(r_u32(((uint32)(((uint32)(a1) + (uint32)(328))))),r_u8(((uint32)(((uint32)(a1) + (uint32)(27))))));
  w_u16(((uint32)(((uint32)(a1) + (uint32)(22)))),result);
  return result;
}



uint32 sub_80029D24(uint32 a1, uint32 a2, uint32 a3, uint32 a4)
{
  uint32 v8;
  sint32 v9;
  sint32 v10;
  uint32 v11;
  sint32 v12;
  sint32 v13;
  sint32 v14;
  sint32 v15;
  sint32 v16;
  sint32 v17;
  sub_800330F4(a1);
  w_u8(((uint32)(((uint32)(a1) + (uint32)(67)))),3);
  v8 = ((uint32)(r_u32(0x800FF5A0u)));
  w_u32(((uint32)(((uint32)(a1) + (uint32)(68)))),0x800A1380u);
  v9 = r_u32((v8+(2)*4u));
  v10 = r_u32((v8+(3)*4u));
  w_u32(((uint32)(((uint32)(a1) + (uint32)(24)))),r_u32((v8+(1)*4u)));
  w_u32(((uint32)(((uint32)(a1) + (uint32)(28)))),v9);
  w_u32(((uint32)(((uint32)(a1) + (uint32)(32)))),v10);
  w_u32(((uint32)(((uint32)(a1) + (uint32)(92)))),a2);
  w_u32(((uint32)(((uint32)(a1) + (uint32)(96)))),a3);
  w_u32(((uint32)(((uint32)(a1) + (uint32)(100)))),a4);
  if (!a4)
    w_u32(((uint32)(((uint32)(a1) + (uint32)(100)))),1);
  v11 = ((uint32)(sub_80032DC0(116)));
  if (v11)
    v11 = sub_80028FB8(v11,16,-857728900);
  w_u32(((uint32)(((uint32)(a1) + (uint32)(72)))),v11);
  w_u8((((uint32)(v11))+(66)*1u),1);
  v12 = 0;
  sub_80029198(r_u32(((uint32)(((uint32)(a1) + (uint32)(72))))),((uint32)(((uint32)(a1) + (uint32)(24)))),0,0);
  v14 = sub_80066570(64);
  v13 = sub_80066570(64);
  sub_80029324(r_u32(((uint32)(((uint32)(a1) + (uint32)(72))))),v14,v13,2);
  w_u32(((uint32)(((uint32)(a1) + (uint32)(80)))),255);
  w_u32(((uint32)(((uint32)(a1) + (uint32)(84)))),100);
  v15 = r_u32(((uint32)(((uint32)(a1) + (uint32)(72)))));
  w_u32(((uint32)(((uint32)(a1) + (uint32)(88)))),0);
  sub_800293D8(v15,255,100,0);
  v16 = r_u32(0x800FF4E8u);
  w_u16(((uint32)(((uint32)(r_u32(((uint32)(((uint32)(a1) + (uint32)(72)))))) + (uint32)(114)))),1);
  sub_80029F58(a1,v16);
  sub_80029F58(a1,r_u32(0x800FF5DCu));
  v17 = a1;
  do
  {
    w_u32(((uint32)(((uint32)(v17) + (uint32)(104)))),sub_80066570(4096));
    ++v12;
    v17 += 4;
  }
  while ((((sint32)(v12)) < 16));
  sub_800774EC(r_u32(0x800FF904u),((uint32)(a1) + (uint32)(24)),1);
  w_u32(0x800FF218u,1);
  w_u32(0x800FF21Cu,a1);
  sub_80069DF0(10,0x2000,0);
  return a1;
}


/* TODO Missing host buffer adapter xport_draft_host_sub_800667CC_p13 */
/* TODO Missing host buffer adapter xport_draft_host_sub_8006C34C_p13 */
/* TODO Missing host buffer adapter xport_draft_host_sub_8006C40C_p12 */
uint32 sub_80029198(uint32 a1, uint32 a2, uint32 a3, uint32 a4)
{
  sint32 v5;
  sint32 v6;
  sint32 v7;
  sint32 v8;
  uint32 v9;
  sint32 i;
  sint32 result;
  sint32 v12;
  sint32 v13;
  sint32 v14;
  sint32 v15;
  sint32 v16;
  sint32 v17;
  sint32 v18;
  short v19;
  char v20[16];
  int v21[4];
  int v22[4];
  int v23[4];
  v5 = 0;
  v6 = r_u32((a2+(1)*4u));
  v7 = r_u32((a2+(2)*4u));
  w_u32(((uint32)(((uint32)(a1) + (uint32)(24)))),r_u32(a2));
  w_u32(((uint32)(((uint32)(a1) + (uint32)(28)))),v6);
  w_u32(((uint32)(((uint32)(a1) + (uint32)(32)))),v7);
  w_u32(((uint32)(((uint32)(a1) + (uint32)(96)))),a4);
  w_u32(((uint32)(((uint32)(a1) + (uint32)(92)))),a3);
  v6 = ((v6 & 0xFFFF0000u)|(((r_u16(((uint32)(((uint32)(a1) + (uint32)(88)))))) & 0xFFFFu)<<0));
  v18 = r_u32(((uint32)(((uint32)(a1) + (uint32)(84)))));
  v19 = v6;
  v8 = r_u32(((uint32)(((uint32)(a1) + (uint32)(80)))));
  v9 = ((uint32)(((uint32)(v8) + (uint32)(128))));
  for (i = (4096 / r_u32(((uint32)(((uint32)(a1) + (uint32)(76))))));; v18 = ((v18 & 0x0000FFFFu)|(((i) & 0xFFFFu)<<16)))
  {
    result = (((sint32)(v5)) < r_u32(((uint32)(((uint32)(a1) + (uint32)(76))))));
    if ((((sint32)(v5)) >= r_u32(((uint32)(((uint32)(a1) + (uint32)(76)))))))
      break;
    xport_draft_host_sub_800667CC_p13(v20,1,&v18);
    xport_draft_host_sub_8006C40C_p12(v22,v20,((uint32)(a1) + (uint32)(92)));
    xport_draft_host_sub_8006C34C_p13(v21,((uint32)(a1) + (uint32)(24)),v22);
    v12 = v21[1];
    v13 = v21[2];
    w_u32((v9-(6)*4u),v21[0]);
    w_u32((v9-(5)*4u),v12);
    w_u32((v9-(4)*4u),v13);
    xport_draft_host_sub_8006C40C_p12(v21,v20,((uint32)(a1) + (uint32)(96)));
    xport_draft_host_sub_8006C34C_p13(v23,((uint32)(v8) + (uint32)(104)),v21);
    v14 = v23[1];
    v15 = v23[2];
    w_u32((v9-(3)*4u),v23[0]);
    w_u32((v9-(2)*4u),v14);
    w_u32((v9-(1)*4u),v15);
    xport_draft_host_sub_8006C34C_p13(v22,((uint32)(v8) + (uint32)(116)),v21);
    v16 = v22[1];
    v17 = v22[2];
    w_u32(v9,v22[0]);
    w_u32((v9+(1)*4u),v16);
    w_u32((v9+(2)*4u),v17);
    v9 += (35)*4u;
    v8 += 140;
    ++v5;
  }

  return result;
}



uint32 sub_8001E8B0(uint32 a1, uint32 a2, uint32 a3, uint32 a4, uint32 a5, uint32 a6, uint32 a7, uint32 a8, uint32 a9)
{
  sint32 v17;
  sint32 v18;
  sub_80034598(a1,a9,2);
  w_u32(((uint32)(((uint32)(a1) + (uint32)(68)))),0x800A113Cu);
  v17 = r_u32((a2+(1)*4u));
  v18 = r_u32((a2+(2)*4u));
  w_u32(((uint32)(((uint32)(a1) + (uint32)(24)))),r_u32(a2));
  w_u32(((uint32)(((uint32)(a1) + (uint32)(28)))),v17);
  w_u32(((uint32)(((uint32)(a1) + (uint32)(32)))),v18);
  w_u8(((uint32)(((uint32)(a1) + (uint32)(104)))),a3);
  w_u8(((uint32)(((uint32)(a1) + (uint32)(105)))),a4);
  w_u8(((uint32)(((uint32)(a1) + (uint32)(106)))),a5);
  w_u32(((uint32)(((uint32)(a1) + (uint32)(108)))),a7);
  w_u32(((uint32)(((uint32)(a1) + (uint32)(112)))),0);
  w_u32(((uint32)(((uint32)(a1) + (uint32)(116)))),a6);
  w_u8(((uint32)(((uint32)(a1) + (uint32)(94)))),1);
  sub_80034A18(a1,0,0,0);
  sub_80034A44(a1,0,0,0);
  sub_80034994(a1,0);
  sub_800349D0(a1,0,a8);
  sub_800349D0(a1,1,a8);
  sub_80034A9C(a1,0,r_u8(a1+104u),r_u8(a1+105u),r_u8(a1+106u));
  sub_80034A9C(a1,1,0,0,0);
  w_u16(((uint32)(((uint32)(a1) + (uint32)(96)))),sub_80066570(1024));
  return a1;
}



uint32 sub_8001EA60(uint32 a1)
{
  sint32 v2;
  sint32 v3;
  sint32 v4;
  sint32 v5;
  v2 = r_u8(((uint32)(((uint32)(a1) + (uint32)(104)))));
  v3 = (((sint32)(v2)) < r_u32(((uint32)(((uint32)(a1) + (uint32)(108))))));
  w_u32(((uint32)(((uint32)(a1) + (uint32)(112)))),(r_u32(((uint32)(((uint32)(a1) + (uint32)(112)))))+(r_u32(((uint32)(((uint32)(a1) + (uint32)(116))))))));
  if (v3)
    w_u8(((uint32)(((uint32)(a1) + (uint32)(104)))),0);
  else
    w_u8(((uint32)(((uint32)(a1) + (uint32)(104)))),((uint32)(v2) - (uint32)(r_u8(((uint32)(((uint32)(a1) + (uint32)(108))))))));
  v4 = r_u8(((uint32)(((uint32)(a1) + (uint32)(105)))));
  if ((((sint32)(v4)) < r_u32(((uint32)(((uint32)(a1) + (uint32)(108)))))))
    w_u8(((uint32)(((uint32)(a1) + (uint32)(105)))),0);
  else
    w_u8(((uint32)(((uint32)(a1) + (uint32)(105)))),((uint32)(v4) - (uint32)(r_u8(((uint32)(((uint32)(a1) + (uint32)(108))))))));
  v5 = r_u8(((uint32)(((uint32)(a1) + (uint32)(106)))));
  if ((((sint32)(v5)) < r_u32(((uint32)(((uint32)(a1) + (uint32)(108)))))))
    w_u8(((uint32)(((uint32)(a1) + (uint32)(106)))),0);
  else
    w_u8(((uint32)(((uint32)(a1) + (uint32)(106)))),((uint32)(v5) - (uint32)(r_u8(((uint32)(((uint32)(a1) + (uint32)(108))))))));
  if (!(((r_u8(((uint32)(((uint32)(a1) + (uint32)(106))))) | r_u8(((uint32)(((uint32)(a1) + (uint32)(104)))))) | r_u8(((uint32)(((uint32)(a1) + (uint32)(105))))))))
    sub_80032ED8(a1);
  sub_80034994(a1,r_u32(((uint32)(((uint32)(a1) + (uint32)(112))))));
  return sub_80034A9C(a1,0,r_u8(a1+104u),r_u8(a1+105u),r_u8(a1+106u));
}



uint32 sub_8001EA0C(uint32 a1, uint32 a2)
{
  sint32 result;
  w_u32(((uint32)(((uint32)(a1) + (uint32)(68)))),0x800A113Cu);
  result = sub_800348A8(a1,0);
  if (((a2 & 1) != 0))
    return sub_80032E30(a1);
  return result;
}



uint32 sub_8001EB64(uint32 a1, uint32 a2, uint32 a3, uint32 a4, uint32 a5, uint32 a6, uint32 a7)
{
  uint32 v14;
  uint32 v15;
  sint32 v16;
  uint32 result;
  sint32 v18;
  sint32 vars0;
  sint32 vars4;
  sint32 vars8;
  v14 = a3;
  v15 = a4;
  sub_8001E8B0(a1,a2,a3,a4,a5,0,0,0,6);
  v16 = ((v16 & 0xFFFFFF00u)|(((a3) & 0xFFu)<<0));
  w_u32((a1+(17)*4u),0x800A1124u);
  w_u32((a1+(30)*4u),(((sint32)(a6)) / ((sint32)(a7))));
  if ((v14 < a5))
    v16 = ((v16 & 0xFFFFFF00u)|(((a5) & 0xFFu)<<0));
  v16 = ((unsigned char)(v16));
  if ((((unsigned char)(v16)) < v15))
    v16 = a4;
  result = a1;
  w_u32((a1+(27)*4u),(((sint32)(v16)) / ((sint32)(a7))));
  return result;
}


/* TODO Missing call adapter sub_800314CC */
/* TODO Missing call adapter sub_80031BA8 */
/* TODO Missing call adapter sub_80031E10 */
uint32 sub_80020068(uint32 a1)
{
  sint32 v2;
  sint32 v3;
  sint32 v4;
  sint32 result;
  v2 = r_u16(((uint32)(((uint32)(a1) + (uint32)(58)))));
  w_u32(0x800FF3ACu,0);
  if ((v2 == 2))
  {
    v3 = sub_80032DC0(80);
    if (v3)
    {
      w_u32(((uint32)(((uint32)(a1) + (uint32)(340)))),sub_8003188C(v3));
      goto LABEL_16;
    }
    goto LABEL_15;
  }
  if ((((sint32)(v2)) >= 3))
  {
    if ((v2 == 3))
    {
      v3 = sub_80032DC0(80);
      if (v3)
        v3 = ((void)(v3),abort(),0u);
    }
    else
    {
      if ((v2 != 7))
        goto LABEL_16;
      v3 = sub_80032DC0(76);
      if (v3)
      {
        w_u32(((uint32)(((uint32)(a1) + (uint32)(340)))),((void)(v3),abort(),0u));
        goto LABEL_16;
      }
    }
    LABEL_15:
    w_u32(((uint32)(((uint32)(a1) + (uint32)(340)))),v3);

    goto LABEL_16;
  }
  if ((v2 == 1))
  {
    v3 = sub_80032DC0(76);
    if (v3)
    {
      w_u32(((uint32)(((uint32)(a1) + (uint32)(340)))),((void)(v3),abort(),0u));
      goto LABEL_16;
    }
    goto LABEL_15;
  }
  LABEL_16:
  v4 = r_u32(((uint32)(((uint32)(a1) + (uint32)(340)))));

  if (v4)
    w_u8(((uint32)(((uint32)(v4) + (uint32)(66)))),1);
  result = 1;
  w_u32(0x800FF3ACu,1);
  return result;
}



uint32 sub_800260FC(uint32 a1, uint32 a2, uint32 a3)
{
  uint32 result;
  sub_8003304C(a1);
  w_u32((a1+(17)*4u),0x800A15B8u);
  w_u32((a1+(19)*4u),a2);
  w_u32((a1+(20)*4u),sub_8006B864(((uint32)(36) * (uint32)(a2)),0,1));
  result = a1;
  w_u32((a1+(18)*4u),a3);
  return result;
}


/* TODO Missing call adapter xport_draft_missing_gte_adapter */
uint32 sub_80025248(uint32 a1)
{
  uint32 xport_draft_asm_carrier; /* TODO GTE carrier */
  int v3[4];
  xport_draft_asm_carrier = (((unsigned short)(((uint32)((((sint32)(((sint32)(r_u32(a1))))) >> 12)) - (uint32)(r_u32(0x800ED520u))))) | ((uint32)(((unsigned short)(((uint32)((((sint32)(((sint32)(r_u32((a1+(1)*4u)))))) >> 12)) - (uint32)(r_u32(0x800ED524u)))))) << (uint32)(16)));
  (abort(),0u);
  return v3[2];
}



uint32 sub_80062BF0(uint32 a1)
{
  sint32 result;
  result = (r_u16(((uint32)(((uint32)(a1) + (uint32)(78))))) & 1);
  if (result)
  {
    sub_80062A64(a1,0x800FF5E0u);
    sub_80062A38(a1,((sint32)(r_u32(((uint32)(((uint32)(a1) + (uint32)(72))))))));
    result = (r_u16(((uint32)(((uint32)(a1) + (uint32)(78))))) & 0xFFFE);
    w_u16(((uint32)(((uint32)(a1) + (uint32)(78)))),result);
  }
  return result;
}



uint32 sub_8003BAE0(uint32 a1, uint32 a2)
{
  sint32 v4;
  sint32 result;
  w_u32(((uint32)(((uint32)(a1) + (uint32)(68)))),0x800A1DF0u);
  sub_80062A64(a1,0x800FF5DCu);
  v4 = r_u32(((uint32)(((uint32)(a1) + (uint32)(520)))));
  if (v4)
  {
    sub_8006A294(v4);
    w_u32(((uint32)(((uint32)(a1) + (uint32)(520)))),0);
  }
  sub_8004B868(a1,0);
  result = (a2 & 1);
  if (((a2 & 1) != 0))
    return sub_80062608(a1);
  return result;
}



uint32 sub_8005CE70(uint32 a1, uint32 a2)
{
  short v3;
  sint32 v4;
  v3 = ((uint32)(r_u16(((uint32)(((uint32)(a1) + (uint32)(218)))))) + (uint32)(a2));
  w_u16(((uint32)(((uint32)(a1) + (uint32)(218)))),v3);
  if ((r_u16(0x800EC526u) < ((sint32)(v3))))
    w_u16(((uint32)(((uint32)(a1) + (uint32)(218)))),r_u16(0x800EC526u));
  v4 = sub_80066570(2);
  sub_8002FC64(1,v4,((uint32)(a1) + (uint32)(4)),1);
  return 1;
}



uint32 sub_80027878(uint32 a1, uint32 a2, uint32 a3, uint32 a4, uint32 a5, uint32 a6, uint32 a7, uint32 a8, uint32 a9, uint32 a10, uint32 a11, uint32 a12, uint32 a13)
{
  sint32 result;
  result = a5;
  w_u8((a1+(80)*1u),a2);
  w_u8((a1+(81)*1u),a3);
  w_u8((a1+(82)*1u),a4);
  w_u8((a1+(83)*1u),a5);
  w_u8((a1+(84)*1u),a6);
  w_u8((a1+(85)*1u),a7);
  w_u8((a1+(86)*1u),a8);
  w_u8((a1+(87)*1u),a9);
  w_u8((a1+(88)*1u),a10);
  w_u8((a1+(89)*1u),a11);
  w_u8((a1+(90)*1u),a12);
  w_u8((a1+(91)*1u),a13);
  return result;
}



uint32 sub_80022B34(uint32 a1, uint32 a2)
{
  sint32 result;
  result = ((uint32)(r_u32(((uint32)(((uint32)(a1) + (uint32)(56)))))) - (uint32)(a2));
  w_u32(((uint32)(((uint32)(a1) + (uint32)(56)))),result);
  if ((((sint32)(result)) < 0))
    w_u32(((uint32)(((uint32)(a1) + (uint32)(56)))),0);
  return result;
}


/* TODO Missing host buffer adapter xport_draft_host_sub_800667CC_p1 */
/* TODO Missing host buffer adapter xport_draft_host_sub_8006C0FC_p2 */
uint32 sub_80022B54(uint32 a1, uint32 a2, uint32 a3)
{
  char v5[16];
  xport_draft_host_sub_800667CC_p1(v5,100,a3);
  return xport_draft_host_sub_8006C0FC_p2(a2,v5);
}


/* TODO Missing call adapter sub_8008BF8C */
uint32 sub_800284E8(uint32 a1, uint32 a2, uint32 a3, uint32 a4)
{
  sint32 v8;
  sint32 result;
  if (!(r_u32(((uint32)(((uint32)(a1) + (uint32)(60)))))))
    return 0;
  v8 = ((abort(),0u) >= 0x4000);
  result = 0;
  if (!v8)
  {
    w_u8(a2,r_u8(((uint32)(((uint32)(a1) + (uint32)(80))))));
    w_u8(a3,r_u8(((uint32)(((uint32)(a1) + (uint32)(81))))));
    result = 1;
    w_u8(a4,r_u8(((uint32)(((uint32)(a1) + (uint32)(82))))));
  }
  return result;
}



uint32 sub_8002616C(uint32 a1, uint32 a2)
{
  sint8 v3;
  sint32 v4;
  sint32 result;
  v3 = a2;
  v4 = r_u32(((uint32)(((uint32)(a1) + (uint32)(80)))));
  w_u32(((uint32)(((uint32)(a1) + (uint32)(68)))),0x800A15B8u);
  ((void)(a2),sub_8006BC20(v4));
  result = sub_80033090(a1,0);
  if (((v3 & 1) != 0))
    return sub_80032E30(a1);
  return result;
}


/* TODO Missing call adapter indirect */
uint32 sub_800278D0(uint32 a1)
{
  sint32 v2;
  sint32 result;
  sint32 v4;
  sint32 v5;
  v2 = r_u32((a1+(15)*4u));
  if (v2)
    result = ((void)(((sint32)(((uint32)(v2) + (uint32)(((sint16)(r_u16(((uint32)(((uint32)(r_u32(((uint32)(((uint32)(v2) + (uint32)(68)))))) + (uint32)(8)))))))))))),(void)(3),abort(),0u);
  v4 = r_u32((a1+(16)*4u));
  w_u32((a1+(15)*4u),0);
  if (v4)
    result = ((void)(((sint32)(((uint32)(v4) + (uint32)(((sint16)(r_u16(((uint32)(((uint32)(r_u32(((uint32)(((uint32)(v4) + (uint32)(68)))))) + (uint32)(8)))))))))))),(void)(3),abort(),0u);
  v5 = r_u32((a1+(23)*4u));
  w_u32((a1+(16)*4u),0);
  if (v5)
    result = ((void)(((sint32)(((uint32)(v5) + (uint32)(((sint16)(r_u16(((uint32)(((uint32)(r_u32(((uint32)(((uint32)(v5) + (uint32)(68)))))) + (uint32)(8)))))))))))),(void)(3),abort(),0u);
  w_u32((a1+(23)*4u),0);
  /* TODO No destructor call leaves an inherited return value */
  return xport_draft_unknown_result_800278D0();
}


/* TODO Missing call adapter indirect */
uint32 sub_8001CA68(uint32 a1)
{
  sint32 v2;
  sint32 v3;
  sint32 v4;
  sint32 v5;
  sint32 result;
  sint32 v7;
  sint32 v8;
  sint32 v9;
  v2 = (((uint32)(r_u32((a1+(6)*4u))) - (uint32)(r_u32(((uint32)(((uint32)(r_u32(0x800FF5A0u)) + (uint32)(4))))))) >> 12);
  if ((((sint32)(v2)) < 0))
    v2 = -v2;
  v3 = (((uint32)(r_u32((a1+(8)*4u))) - (uint32)(r_u32(((uint32)(((uint32)(r_u32(0x800FF5A0u)) + (uint32)(12))))))) >> 12);
  v4 = (((sint32)(v2)) < ((sint32)(v3)));
  if ((((sint32)(v3)) < 0))
  {
    v3 = -v3;
    v4 = (((sint32)(v2)) < ((sint32)(v3)));
  }
  if (v4)
    v5 = ((uint32)(v3) + (uint32)((((sint32)(v2)) / 2)));
  else
    v5 = ((uint32)(v2) + (uint32)((((sint32)(v3)) / 2)));
  result = (((sint32)(v5)) < r_u32((a1+(23)*4u)));
  if ((((sint32)(v5)) >= r_u32((a1+(23)*4u))))
  {
    v9 = r_u32((a1+(24)*4u));
    if (v9)
    {
      result = ((void)(((sint32)(((uint32)(v9) + (uint32)(((sint16)(r_u16(((uint32)(((uint32)(r_u32(((uint32)(((uint32)(v9) + (uint32)(68)))))) + (uint32)(8)))))))))))),(void)(3),abort(),0u);
      w_u32((a1+(24)*4u),0);
    }
  }
  else
  {
    v7 = ((uint32)(4) * (uint32)(v5));
    if (!r_u32((a1+(24)*4u)))
    {
      w_u32(0x800FF3ACu,0);
      v8 = sub_80032DC0(176);
      if (v8)
        v8 = sub_80034C0C(v8,a1+24u,r_u32(a1+72u),r_u32(a1+76u),r_u32(a1+88u),r_u8(a1+80u),r_u8(a1+81u),r_u8(a1+82u),r_u8(a1+83u),r_u8(a1+84u),r_u8(a1+85u));
      w_u32((a1+(24)*4u),v8);
      w_u32(0x800FF3ACu,1);
      w_u8(((uint32)(((uint32)(v8) + (uint32)(66)))),1);
      v7 = ((uint32)(4) * (uint32)(v5));
    }
    result = (((uint32)(2) * (uint32)(((sint32)(((uint32)(v7) + (uint32)(v5)))))) / 16);
    w_u16(((uint32)(((uint32)(r_u32((a1+(24)*4u))) + (uint32)(96)))),result);
  }
  return result;
}



uint32 sub_80034C0C(uint32 a1, uint32 a2, uint32 a3, uint32 a4, uint32 a5, uint32 a6, uint32 a7, uint32 a8, uint32 a9, uint32 a10, uint32 a11)
{
  uint32 v17;
  sint32 v18;
  uint32 v19;
  unsigned char vars0;
  unsigned char vars4;
  unsigned char vars8;
  unsigned char varsC;
  sub_800346A8(((sint32)(a1)),a2,a3,a4,a6,a7,a8,a9,a10,a11);
  w_u32((a1+(17)*4u),0x800A1CF0u);
  w_u32((a1+(43)*4u),a3);
  v17 = 0;
  v18 = r_u32((a1+(20)*4u));
  w_u32((a1+(42)*4u),(((sint32)(((uint32)(a3) * (uint32)(a5)))) / 256));
  if (v18)
  {
    v19 = a1;
    do
    {
      w_u32((v19+(34)*4u),sub_80066570(4096));
      w_u32((v19+(26)*4u),((uint32)(sub_80066570(50)) + (uint32)(50)));
      ++v17;
      (v19+=4u);
    }
    while ((v17 < r_u32((a1+(20)*4u))));
  }
  return a1;
}


/* TODO Missing call adapter SLOWORD */
uint32 sub_80034CF8(uint32 a1)
{
  sint32 result;
  uint32 v2;
  uint32 v3;
  sint32 v4;
  result = r_u32((a1+(20)*4u));
  v2 = 0;
  if (result)
  {
    v3 = a1;
    do
    {
      v4 = ((uint32)(r_u32((v3+(34)*4u))) + (uint32)(r_u32((v3+(26)*4u))));
      w_u32((v3+(34)*4u),v4);
      w_u32(((uint32)(((uint32)(((uint32)(8) * (uint32)(v2++))) + (uint32)(r_u32((a1+(18)*4u)))))),((uint32)(r_u32((a1+(43)*4u))) + (uint32)((((uint32)(r_u32((a1+(42)*4u))) * (uint32)(((void)(r_u32((0x800F863Cu+((v4 & 0xFFF))*4u))),abort(),0u))) / 4096))));
      result = (v2 < r_u32((a1+(20)*4u)));
      (v3+=4u);
    }
    while ((v2 < r_u32((a1+(20)*4u))));
  }
  return result;
}



uint32 sub_8006EDD4(uint32 a1)
{
  sint32 result;
  sint32 v3;
  result = ((r_u32((0x800EAEF8u+(((uint32)(((uint32)(16) * (uint32)(sub_8006ED70(a1)))) + (uint32)(2)))*4u))>>16)&255u);
  if (result)
  {
    v3 = sub_8006ED70(a1);
    return sub_8006EE94(v3,1);
  }
  return result;
}



uint32 sub_80036D48(uint32 a1, uint32 a2, uint32 a3, uint32 a4, uint32 a5, uint32 a6, uint32 a7, uint32 a8, uint32 a9, uint32 a10)
{
  sint32 v16;
  sint32 v17;
  sint32 v18;
  sint32 v19;
  sint32 v20;
  sint32 v21;
  sint32 v22;
  sint32 v23;
  short v24;
  sint32 result;
  sub_80032F7C(a1);
  w_u32(((uint32)(((uint32)(a1) + (uint32)(68)))),0x800A1B98u);
  sub_80032E50(((uint32)(a1)),0x800FF454u);
  v16 = r_u32((a2+(1)*4u));
  v17 = r_u32((a2+(2)*4u));
  w_u32(((uint32)(((uint32)(a1) + (uint32)(24)))),r_u32(a2));
  w_u32(((uint32)(((uint32)(a1) + (uint32)(28)))),v16);
  w_u32(((uint32)(((uint32)(a1) + (uint32)(32)))),v17);
  v18 = ((uint32)(((uint32)(2) * (uint32)(a10))) + (uint32)(1));
  w_u32(((uint32)(((uint32)(a1) + (uint32)(72)))),((uint32)(r_u32(a2)) + (uint32)(((uint32)(((uint32)(sub_80066570(((uint32)(((uint32)(2) * (uint32)(a8))) + (uint32)(1)))) - (uint32)(a8))) << (uint32)(12)))));
  w_u32(((uint32)(((uint32)(a1) + (uint32)(76)))),((uint32)(r_u32((a2+(1)*4u))) + (uint32)(((uint32)(((uint32)(sub_80066570(((uint32)(((uint32)(2) * (uint32)(a9))) + (uint32)(1)))) - (uint32)(a9))) << (uint32)(12)))));
  w_u32(((uint32)(((uint32)(a1) + (uint32)(80)))),((uint32)(r_u32((a2+(2)*4u))) + (uint32)(((uint32)(((uint32)(sub_80066570(v18)) - (uint32)(a10))) << (uint32)(12)))));
  w_u32(((uint32)(((uint32)(a1) + (uint32)(84)))),((uint32)(r_u32(a2)) + (uint32)(((uint32)(((uint32)(sub_80066570(((uint32)(((uint32)(2) * (uint32)(a8))) + (uint32)(1)))) - (uint32)(a8))) << (uint32)(12)))));
  w_u32(((uint32)(((uint32)(a1) + (uint32)(88)))),((uint32)(r_u32((a2+(1)*4u))) + (uint32)(((uint32)(((uint32)(sub_80066570(((uint32)(((uint32)(2) * (uint32)(a9))) + (uint32)(1)))) - (uint32)(a9))) << (uint32)(12)))));
  w_u32(((uint32)(((uint32)(a1) + (uint32)(92)))),((uint32)(r_u32((a2+(2)*4u))) + (uint32)(((uint32)(((uint32)(sub_80066570(v18)) - (uint32)(a10))) << (uint32)(12)))));
  w_u32(((uint32)(((uint32)(a1) + (uint32)(96)))),((uint32)(r_u32(a2)) + (uint32)(((uint32)(((uint32)(sub_80066570(((uint32)(((uint32)(2) * (uint32)(a8))) + (uint32)(1)))) - (uint32)(a8))) << (uint32)(12)))));
  w_u32(((uint32)(((uint32)(a1) + (uint32)(100)))),((uint32)(r_u32((a2+(1)*4u))) + (uint32)(((uint32)(((uint32)(sub_80066570(((uint32)(((uint32)(2) * (uint32)(a9))) + (uint32)(1)))) - (uint32)(a9))) << (uint32)(12)))));
  v19 = (a5 >> 2);
  w_u32(((uint32)(((uint32)(a1) + (uint32)(104)))),((uint32)(r_u32((a2+(2)*4u))) + (uint32)(((uint32)(((uint32)(sub_80066570(v18)) - (uint32)(a10))) << (uint32)(12)))));
  v20 = r_u32((a3+(1)*4u));
  v21 = r_u32((a3+(2)*4u));
  w_u32(((uint32)(((uint32)(a1) + (uint32)(36)))),r_u32(a3));
  w_u32(((uint32)(((uint32)(a1) + (uint32)(40)))),v20);
  w_u32(((uint32)(((uint32)(a1) + (uint32)(44)))),v21);
  v22 = (a6 >> 2);
  w_u32(((uint32)(((uint32)(a1) + (uint32)(108)))),a4);
  w_u8(((uint32)(((uint32)(a1) + (uint32)(112)))),v19);
  w_u8(((uint32)(((uint32)(a1) + (uint32)(113)))),v22);
  w_u8(((uint32)(((uint32)(a1) + (uint32)(115)))),v19);
  w_u8(((uint32)(((uint32)(a1) + (uint32)(116)))),v22);
  v23 = (a7 >> 2);
  w_u8(((uint32)(((uint32)(a1) + (uint32)(114)))),v23);
  w_u8(((uint32)(((uint32)(a1) + (uint32)(117)))),v23);
  v24 = ((uint32)(sub_80066570(30)) + (uint32)(30));
  result = a1;
  w_u16(((uint32)(((uint32)(a1) + (uint32)(10)))),v24);
  w_u8(((uint32)(((uint32)(a1) + (uint32)(118)))),4);
  return result;
}


/* TODO Missing call adapter indirect */
uint32 sub_8001C9EC(uint32 a1, uint32 a2)
{
  sint32 v4;
  sint32 result;
  v4 = r_u32(((uint32)(((uint32)(a1) + (uint32)(96)))));
  w_u32(((uint32)(((uint32)(a1) + (uint32)(68)))),0x800A10B4u);
  if (v4)
    ((void)(((sint32)(((uint32)(v4) + (uint32)(((sint16)(r_u16(((uint32)(((uint32)(r_u32(((uint32)(((uint32)(v4) + (uint32)(68)))))) + (uint32)(8)))))))))))),(void)(3),abort(),0u);
  sub_80033138(a1,0);
  result = (a2 & 1);
  if (((a2 & 1) != 0))
    return sub_80032E30(a1);
  return result;
}



uint32 sub_8003A0C4(uint32 a1, uint32 a2)
{
  uint32 result;
  sint32 v5;
  w_u32((a1+(17)*4u),0x800A1CF0u);
  result = sub_800348A8(a1,0);
  if (((a2 & 1) != 0))
    return ((uint32)(sub_80032E30(((sint32)(a1)))));
  return result;
}



uint32 sub_8004FE20(uint32 a1)
{
  sint32 v1;
  sint32 result;
  v1 = r_u16(((uint32)(((uint32)(a1) + (uint32)(390)))));
  w_u16(((uint32)(((uint32)(a1) + (uint32)(472)))),13);
  w_u8(((uint32)(((uint32)(a1) + (uint32)(380)))),0);
  w_u16(((uint32)(((uint32)(a1) + (uint32)(474)))),0);
  if ((v1 == 2))
  {
    result = (r_u32(((uint32)(((uint32)(a1) + (uint32)(372))))) | 0xFF80);
    w_u32(((uint32)(((uint32)(a1) + (uint32)(372)))),result);
  }
  else
  {
    result = 1;
    if ((((sint32)(v1)) >= 3))
    {
      result = 8;
      if ((v1 == 4))
      {
        result = (r_u32(((uint32)(((uint32)(a1) + (uint32)(372))))) | 0x7D);
        w_u32(((uint32)(((uint32)(a1) + (uint32)(372)))),result);
      }
      else
        if ((v1 == 8))
      {
        result = (r_u32(((uint32)(((uint32)(a1) + (uint32)(372))))) | 0x1FF80);
        w_u32(((uint32)(((uint32)(a1) + (uint32)(372)))),result);
      }
    }
    else
      if ((v1 == 1))
    {
      result = (r_u32(((uint32)(((uint32)(a1) + (uint32)(372))))) | 0x1F80);
      w_u32(((uint32)(((uint32)(a1) + (uint32)(372)))),result);
    }
  }
  return result;
}



void sub_800774B4(uint32 a1, uint32 a2)
{
  w_u16(((uint32)(((uint32)(a1) + (uint32)(546)))),a2);
}



uint32 sub_8005DD40(uint32 a1)
{
  sint32 result;
  short v3;
  result = sub_80062F48(a1);
  if (!result)
  {
    result = (((unsigned short)(r_u16((a1+(39)*2u)))) | 0x40);
    v3 = (r_u16(a1) | 1);
    w_u16((a1+(39)*2u),result);
    w_u16(a1,v3);
  }
  return result;
}


/* TODO Missing call adapter sub_80097F34 */
/* TODO Missing host buffer adapter xport_draft_host_sub_80093ED8_p2 */
uint32 sub_8006A1DC(void)
{
  sint32 result;
  sint32 v1;
  uint32 v2;
  uint32 v3;
  short v4[8];
  result = r_u32(0x800FF6B0u);
  if (r_u32(0x800FF6B0u))
  {
    v1 = 0;
    v2 = ((uint32)(r_u32(0x800E5BD8u)));
    v3 = ((uint32)(r_u32(0x800E5BA8u)));
    w_u32(0x800FF6B0u,0);
    v4[3] = 3;
    do
    {
      if ((((unsigned short)(((sint16)(r_u16(v3))))) | ((unsigned short)(((sint16)(r_u16(v2)))))))
      {
        v4[0] = ((sint16)(r_u16(v3)));
        v4[1] = ((sint16)(r_u16(v2)));
        xport_draft_host_sub_80093ED8_p2(v1,v4);
      }
      (v2+=2u);
      ++v1;
      (v3+=2u);
    }
    while ((((sint32)(v1)) < 24));
    if (!r_u32(0x800FFC20u))
      ((void)(8),sub_8006A428());
    return ((void)(12),(void)(0),abort(),0u);
  }
  return result;
}


