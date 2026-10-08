#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
#include <stdlib.h>
uint32 sub_800368F8(uint32 a1, uint32 a2, uint32 a3, uint32 a4)
{
  sint32 v5;
  sint32 result;
  v5 = 0;
  result = r_u32(((uint32)((a1 + 72))));
  if ((result > 0))
  {
    result = 0;
    do
    {
      sub_800332FC(r_u32(((uint32)((result + r_u32(((uint32)((a1 + 92)))))))),a2,a3,a4);
      ++v5;
      result = (4 * v5);
    }
    while ((v5 < r_u32(((uint32)((a1 + 72))))));
  }
  return result;
}
uint32 sub_800774EC(uint32 a1, uint32 a2, uint32 a3)
{
  sint32 v4;
  short v5;
  short v6;
  short v7;
  sint8 v8;
  sint8 v9;
  short v10;
  if ((a3 == 1))
  {
    v6 = r_u16(0x800FFCF4u);
    w_u32(((uint32)((a1 + 548))),0x800FFCF0u);
    w_u16(((uint32)((a1 + 552))),v6);
    goto LABEL_10;
  }
  if ((((sint32)(a3)) >= 2))
  {
    if ((a3 != 2))
    {
      v4 = a2;
      goto LABEL_11;
    }
    v7 = r_u16(0x800FFCECu);
    w_u32(((uint32)((a1 + 548))),0x800FFCE8u);
    w_u16(((uint32)((a1 + 552))),v7);
    LABEL_10:
    v8 = r_u8(0x800FFCE1u);

    v9 = r_u8(0x800FFCE2u);
    w_u8(((uint32)((a1 + 554))),0x800FFCE0u);
    w_u8(((uint32)((a1 + 555))),v8);
    w_u8(((uint32)((a1 + 556))),v9);
    v10 = r_u16(0x800FFCDCu);
    w_u32(((uint32)((a1 + 558))),0x800FFCD8u);
    w_u16(((uint32)((a1 + 562))),v10);
    v4 = a2;
    goto LABEL_11;
  }
  if (!a3)
  {
    v5 = r_u16(0x800FFCFCu);
    w_u32(((uint32)((a1 + 548))),0x800FFCF8u);
    w_u16(((uint32)((a1 + 552))),v5);
    goto LABEL_10;
  }
  v4 = a2;
  LABEL_11:
  sub_800774BC(v4,r_u32(0x800FF4E8u));

  return sub_800774BC(a2,r_u32(0x800FF5DCu));
}
/* TODO Missing call adapter _byteswap_ushort */
uint32 sub_80033430(uint32 a1)
{
  short v1;
  sint32 v2;
  sint32 result;
  v1 = (((void)(r_u16(((uint32)((a1 + 90))))),abort(),0u) + r_u16(((uint32)((a1 + 92)))));
  w_u8(((uint32)((a1 + 90))),((v1>>8)&255u));
  v2 = (((sint8)(r_u8(((uint32)((a1 + 90)))))) < ((sint32)(r_u8(((uint32)((a1 + 89)))))));
  w_u8(((uint32)((a1 + 91))),v1);
  if (!v2)
    w_u8(((uint32)((a1 + 90))),0);
  result = (8 * ((sint8)(r_u8(((uint32)((a1 + 90)))))));
  w_u32(((uint32)((a1 + 84))),(r_u32(((uint32)((a1 + 80)))) + result));
  return result;
}
/* TODO Postincrement memory expressions may require ordering refinement */
uint32 sub_8004F5C8(uint32 a1, uint32 a2, uint32 a3, uint32 a4)
{
  sint32 result;
  sint32 v8;
  sint32 v9;
  sint32 v10;
  sint32 v11;
  short v12;
  if ((r_u16(((uint32)((a1 + 390)))) == 64))
  {
    result = 0;
    if ((a4 == 5))
      return result;
  }
  v8 = r_u32(((uint32)((a1 + 396))));
  (w_u8(((uint32)((a1 + 383))),(r_u8(((uint32)((a1 + 383))))+1u)),(r_u8(((uint32)((a1 + 383))))+1u));
  v9 = ((sint16)(r_u16(((uint32)((a1 + 218))))));
  w_u32(((uint32)((a1 + 396))),(v8 | 0x400));
  v10 = (v9 <= 0);
  v11 = (r_u16(((uint32)((a1 + 218)))) - a2);
  if (v10)
    return 1;
  w_u16(((uint32)((a1 + 218))),v11);
  if (((v11 << 16) > 0))
  {
    sub_800626F8(a1,3,255,255,255);
    sub_8004F6C4(a1);
    return 1;
  }
  if (r_u32(0x800FF5A0u))
    sub_8005C8C4(r_u32(0x800FF5A0u));
  v12 = r_u16(((uint32)((a1 + 78))));
  w_u16(((uint32)((a1 + 140))),0);
  w_u16(((uint32)((a1 + 134))),0);
  w_u16(((uint32)((a1 + 78))),(v12 & 0xFFEF));
  sub_80052EA8(a1);
  sub_8004F840(a1,a3,a4);
  return 1;
}
uint32 sub_800626F8(uint32 object, uint32 duration, uint32 red, uint32 green, uint32 blue)
{
  uint32 result, flags, color;
  sint32 dr, dg, db;
  red &= 255u; green &= 255u; blue &= 255u;
  color = red | (green << 8) | (blue << 16);
  if (duration == 0u) {
    flags = r_u16(object);
    w_u16(object + 60u, 0u);
  } else if ((sint32)duration < 0) {
    uint32 divisor = (0u - duration) & 0xFFFFu;
    w_u16(object + 60u, divisor);
    /* TODO Original MIPS division by zero has a defined LO result */
    if (divisor == 0u) abort();
    dr = ((sint32)red - 128) / (sint32)divisor;
    dg = ((sint32)green - 128) / (sint32)divisor;
    db = ((sint32)blue - 128) / (sint32)divisor;
    flags = r_u16(object);
    w_u32(object + 32u, 0x808080u);
    result = flags | 0x400u;
    w_u16(object, result);
    w_u16(object + 62u, dr);
    w_u16(object + 64u, dg);
    w_u16(object + 66u, db);
    return result;
  } else {
    dr = (128 - (sint32)red) / (sint32)duration;
    dg = (128 - (sint32)green) / (sint32)duration;
    db = (128 - (sint32)blue) / (sint32)duration;
    w_u16(object + 66u, db);
    flags = r_u32(object + 52u);
    w_u16(object + 60u, duration + 1u);
    w_u16(object + 64u, dg);
    w_u16(object + 62u, dr);
    if (!(flags & 0x1000000u)) {
      uint32 old = r_u32(object + 32u) & 0xFFFFFFu;
      flags = r_u16(object);
      w_u32(object + 52u, old | 0x1000000u);
      if (flags & 0x400u) w_u32(object + 52u, old | 0x3000000u);
    }
    flags = r_u16(object);
  }
  w_u32(object + 32u, color);
  result = flags | 0x400u;
  w_u16(object, result);
  return result;
}
/* TODO Resolve original data label 0x800FF398u */
uint32 sub_800326E8(uint32 a1)
{
  sint32 v2;
  sint32 v3;
  sint32 v4;
  sint32 result;
  sint32 v6;
  sint32 v7;
  sint32 v8;
  sint32 v9;
  sint32 v10;
  v2 = 0;
  v8 = 0;
  v9 = 0;
  v10 = 0;
  while ((v2 < r_u32(((uint32)((a1 + 72))))))
  {
    v9 = (-4096 * (r_u32(((uint32)((a1 + 80)))) + sub_80066570(r_u32(((uint32)((a1 + 84)))))));
    v3 = sub_80032DC0(112);
    if (v3)
    {
      {
        uint32 lifetime = r_u32(a1+88) + sub_80066570(r_u32(a1+92));
        sint32 velocity[3] = {v8,v9,v10};
        v3 = xport_draft_host_sub_80035638_p3(v3,a1+24,velocity,0x800FF398u,lifetime,0,10);
      }
    }
    w_u16(((uint32)((v3 + 64))),100);
    w_u16(((uint32)((v3 + 96))),sub_80066570(4096));
    ++v2;
    sub_800332FC(v3,r_u8(((uint32)((a1 + 76)))),r_u8(((uint32)((a1 + 77)))),r_u8(((uint32)((a1 + 78)))));
  }

  v4 = r_u16(((uint32)((a1 + 10))));
  result = ((result&0xFFFF0000u)|((((r_u16(((uint32)((a1 + 8)))) + 1))&0xFFFFu)<<0));
  w_u16(((uint32)((a1 + 8))),result);
  result = ((short)(result));
  v6 = (((short)(result)) != v4);
  v7 = ((unsigned short)(result));
  if (!v6)
  {
    result = 0xFFFF;
    if ((v7 != 0xFFFF))
      return sub_80032ED8(a1);
  }
  return result;
}
uint32 sub_80032694(uint32 a1, uint32 a2)
{
  sint32 result;
  w_u32(((uint32)((a1 + 68))),0x800A1B20u);
  result = sub_80033138(a1,0);
  if (((a2 & 1) != 0))
    return sub_80032E30(a1);
  return result;
}
void sub_8001BB14(uint32 a1)
{
  sint32 v1;
  sint32 v2;
  sint32 v3;
  uint32 result;
  v1 = r_u32(0x800FF1D4u);
  v2 = 0;
  while (v1)
  {
    v3 = r_u32(((uint32)((v1 + 20))));
    if ((v1 == a1))
    {
      result = r_u32(0x800FF1D8u);
      w_u32(0x800FF1D8u,((uint32)(v1)));
      w_u32(((uint32)((v1 + 20))),result);
      if (v2)
        w_u32(((uint32)((v2 + 20))),v3);
      else
        w_u32(0x800FF1D4u,v3);
      return;
    }
    v2 = v1;
    v1 = r_u32(((uint32)((v1 + 20))));
  }

  return;
}
/* TODO Missing call adapter xport_draft_missing_gte_adapter */
void sub_800857A8(uint32 a1, uint32 a2, uint32 a3)
{
  /* TODO Recover GTE register carriers and missing host GTE adapters */
  uint32 gte_T0 = 0u;
  uint32 gte_T1 = 0u;
  uint32 gte_T2 = 0u;
  uint32 gte_T3 = 0u;
  uint32 gte_T4 = 0u;
  uint32 gte_T5 = 0u;
  
  uint32 v13;
  gte_T0 = ((sint32)(r_u32(a2)));
  gte_T1 = ((sint32)(r_u32((a2+(1)*4u))));
  gte_T2 = ((sint32)(r_u32((a2+(2)*4u))));
  gte_T3 = ((sint32)(r_u32((a2+(3)*4u))));
  gte_T4 = ((sint32)(r_u32((a2+(4)*4u))));
  (abort(),0u);
  gte_T0 = ((sint32)(r_u32(a3)));
  gte_T1 = ((sint32)(r_u32((a3+(1)*4u))));
  (abort(),0u);
  w_u16(a1,gte_T2);
  w_u16((a1+(3)*2u),gte_T3);
  w_u16((a1+(6)*2u),gte_T4);
  v13 = ((sint32)(r_u32((a3+(2)*4u))));
  gte_T0 = ((v13 << 16) | ((gte_T1>>16)&65535u));
  gte_T1 = ((v13>>16)&65535u);
  (abort(),0u);
  w_u16((a1+(1)*2u),gte_T2);
  w_u16((a1+(4)*2u),gte_T3);
  w_u16((a1+(7)*2u),gte_T4);
  gte_T0 = ((sint32)(r_u32((a3+(3)*4u))));
  gte_T1 = ((sint32)(r_u32((a3+(4)*4u))));
  (abort(),0u);
  w_u16((a1+(2)*2u),gte_T2);
  w_u16((a1+(5)*2u),gte_T3);
  w_u16((a1+(8)*2u),gte_T4);
  gte_T0 = r_u32(((uint32)(a1)));
  gte_T1 = r_u32((((uint32)(a1))+(1)*4u));
  gte_T2 = r_u32((((uint32)(a1))+(2)*4u));
  gte_T3 = r_u32((((uint32)(a1))+(3)*4u));
  gte_T4 = r_u32((((uint32)(a1))+(4)*4u));
  (abort(),0u);
  gte_T0 = ((sint16)(r_u16((((uint32)(a3))+(9)*2u))));
  gte_T1 = ((sint16)(r_u16((((uint32)(a3))+(10)*2u))));
  gte_T2 = ((sint16)(r_u16((((uint32)(a3))+(11)*2u))));
  (abort(),0u);
  gte_T1 = ((gte_T1&0xFFFF0000u)|((((r_u16((((uint32)(a2))+(10)*2u)) - gte_T4))&0xFFFFu)<<0));
  gte_T2 = ((gte_T2&0xFFFF0000u)|((((r_u16((((uint32)(a2))+(11)*2u)) - gte_T5))&0xFFFFu)<<0));
  w_u16((a1+(9)*2u),(r_u16((((uint32)(a2))+(9)*2u)) - gte_T3));
  w_u16((a1+(10)*2u),gte_T1);
  w_u16((a1+(11)*2u),gte_T2);
}
uint32 sub_800365C0(uint32 a1, uint32 a2)
{
  uint32 result;
  sint32 v5;
  w_u32((a1+(17)*4u),0x800A1BE0u);
  result = sub_80034E90(a1,0);
  if (((a2 & 1) != 0))
    return ((uint32)(sub_80032E30(((sint32)(a1)))));
  return result;
}
uint32 sub_80034E90(uint32 a1, uint32 a2)
{
  uint32 result;
  sint32 v5;
  w_u32((a1+(17)*4u),0x800A1CC0u);
  sub_80032E7C(a1,0x800FF43Cu);
  result = sub_800331EC(((sint32)(a1)),0);
  if (((a2 & 1) != 0))
    return ((uint32)(sub_80032E30(((sint32)(a1)))));
  return result;
}
uint32 sub_800333D0(uint32 a1, uint32 a2)
{
  sint32 v2;
  sint32 v3;
  sint32 result;
  w_u8(((uint32)((a1 + 90))),a2);
  v2 = ((sint8)(r_u8(((uint32)((a1 + 90))))));
  v3 = r_u32(((uint32)((a1 + 80))));
  w_u8(((uint32)((a1 + 91))),0);
  result = (8 * v2);
  w_u32(((uint32)((a1 + 84))),(v3 + result));
  return result;
}
/* TODO Missing call adapter sub_80058814 */
uint32 sub_80052EA8(uint32 a1)
{
  sint32 v1;
  sint32 result;
  v1 = r_u32(((uint32)((a1 + 616))));
  result = 206;
  if (v1)
  {
    if ((r_u16(((uint32)((v1 + 58)))) == 206))
      return (abort(),0u);
  }
  return result;
}
uint32 sub_8004F840(uint32 a1, uint32 a2, uint32 a3)
{
  sint32 v3;
  sint32 v5;
  sint32 v6;
  sint32 result;
  sint32 v8;
  sint32 v9;
  sint32 v10;
  sint32 v11;
  sint32 v12;
  sint32 v13;
  sint32 v14;
  sint32 v15;
  sint32 v16[3];
  sint16 v18[4];
  short v19;
  v3 = a1;
  v5 = ((((sint32)(r_u32(a2))) | ((sint32)(r_u32((a2+(1)*4u))))) | ((sint32)(r_u32((a2+(2)*4u)))));
  if ((r_u16(((uint32)((a1 + 390)))) == 128))
  {
    v6 = r_u32(((uint32)((a1 + 372))));
    result = 16;
    w_u32(((uint32)((a1 + 472))),16);
    w_u8(((uint32)((a1 + 380))),0);
    w_u32(((uint32)((a1 + 372))),(v6 | 0x113FF));
    return result;
  }
  if ((((r_u32(((uint32)((a1 + 396)))) & 0x200) != 0) || (r_u8(((uint32)((a1 + 26)))) == 4)))
    return sub_8004FB00(a1,0);
  w_u16(((uint32)((a1 + 136))),0);
  w_u16(((uint32)((a1 + 134))),0);
  w_u16(((uint32)((a1 + 132))),0);
  w_u16(((uint32)((a1 + 142))),0);
  w_u16(((uint32)((a1 + 140))),0);
  w_u16(((uint32)((a1 + 138))),0);
  if ((a3 != 5))
  {
    if ((((sint32)(a3)) < 6))
    {
      if ((a3 != 2))
      {
        LABEL_23:
        v12 = sub_80066570(2);

        a1 = v3;
        if (v12)
          return sub_8004FB00(a1,0);
        if ((((r_u16(((uint32)((v3 + 390)))) & 0x1F) != 0) && !sub_80066570(5)))
          return sub_8004FEC4(v3);
        v13 = r_u16(((uint32)((v3 + 390))));
        w_u8(((uint32)((v3 + 380))),0);
        if ((v13 == 4))
        {
          if (v5)
          {
            v14 = ((sint32)(r_u32((a2+(2)*4u))));
            v16[0] = ((sint32)(r_u32(a2)));
            v16[1] = 0;
            v16[2] = v14;
            v15 = (sub_8006BF04(v16) + 1);
            if ((v15 < 128))
            {
              v16[0] *= (128 / v15);
              v16[2] *= (128 / v15);
            }
            xport_draft_host_sub_80066B8C_p12(v18,v16,0x800A71CCu);
            v19 = v18[1];
            w_u16(((uint32)((v3 + 18))),v19);
          }
          result = 10;
        }
        else
        {
          result = 8;
        }
        goto LABEL_35;
      }
      return sub_8004FB00(a1,0);
    }
    if ((a3 == 9))
    {
      if ((((r_u16(((uint32)((a1 + 390)))) & 0x1F) == 0) || sub_80066570(3)))
      {
        v10 = sub_80066570(3);
        a1 = v3;
        if (!v10)
        {
          result = 8;
          w_u8(((uint32)((v3 + 380))),0);
          LABEL_35:
          w_u16(((uint32)((v3 + 472))),result);

          w_u16(((uint32)((v3 + 474))),0);
          return result;
        }
        return sub_8004FB00(a1,0);
      }
    }
    else
    {
      if ((a3 != 10))
        goto LABEL_23;
      if ((((r_u16(((uint32)((a1 + 390)))) & 0x1F) == 0) || sub_80066570(4)))
      {
        v11 = sub_80066570(2);
        a1 = v3;
        if (v11)
          return sub_8004FE20(v3);
        return sub_8004FB00(a1,0);
      }
    }
    return sub_8004FEC4(v3);
  }
  v8 = (sub_80066570(5) == 0);
  result = 11;
  if (v8)
    return sub_8004FB00(v3,5);
  v9 = r_u32(((uint32)((v3 + 396))));
  w_u8(((uint32)((v3 + 380))),0);
  w_u32(((uint32)((v3 + 472))),11);
  w_u32(((uint32)((v3 + 396))),(v9 | 4));
  return result;
}
/* TODO Missing call adapter SHIDWORD */
/* TODO Missing call adapter indirect */
/* TODO Missing call adapter sub_8006C774 */
uint32 sub_8005BD48(uint32 a1)
{
  sint32 v2;
  sint32 v3;
  short v4;
  sint32 v5;
  sint32 v6;
  sint32 v7;
  sint8 v8;
  sint32 v9;
  short v10;
  sint32 v11;
  sint32 v12;
  sint32 v13;
  short v14;
  sint8 v15;
  sint8 v16;
  sint8 v17;
  sint32 v18;
  sint32 i;
  sint32 v20;
  sint32 result;
  sint32 v22;
  long long v23;
  sint32 v24;
  sint32 v25;
  sint32 v26;
  sint32 v27;
  v2 = ((unsigned short)(((sint16)(r_u16((a1+(149)*2u))))));
  v3 = (v2 == 0);
  v4 = (v2 - 1);
  if (v3)
    return ((void)((((sint32)(a1)) + ((sint16)(r_u16(((uint32)((r_u32((((uint32)(a1))+(17)*4u)) + 16)))))))),abort(),0u);
  v5 = (((sint16)(r_u16((a1+(150)*2u)))) & 0x100);
  w_u16((a1+(149)*2u),v4);
  if (v5)
  {
    xport_draft_host_sub_80066B8C_p1(&v23,a1+4,a1+308);
    sub_800667CC((a1+(58)*2u),128,&v23);
    sub_8006C0B8((a1+(52)*2u),(a1+(58)*2u));
    sub_8006C270((a1+(52)*2u),(((uint32)(a1))+(129)*1u));
    sub_8006C05C((a1+(52)*2u));
  }
  v6 = r_u32((((uint32)(a1))+(2)*4u));
  v7 = r_u32((((uint32)(a1))+(3)*4u));
  v24 = r_u32((((uint32)(a1))+(1)*4u));
  v25 = v6;
  v26 = v7;
  sub_8006C0B8((a1+(2)*2u),(a1+(52)*2u));
  if ((((((sint16)(r_u16((a1+(150)*2u)))) & 1) != 0) && (r_u32((((uint32)(a1))+(76)*4u)) < r_u32((((uint32)(a1))+(2)*4u)))))
  {
    v8 = (r_u8((((uint32)(a1))+(296)*1u)) - 1);
    w_u8((((uint32)(a1))+(296)*1u),v8);
    if (!v8)
      return ((void)((((sint32)(a1)) + ((sint16)(r_u16(((uint32)((r_u32((((uint32)(a1))+(17)*4u)) + 16)))))))),abort(),0u);
    v9 = r_u32((((uint32)(a1))+(76)*4u));
    w_u32((((uint32)(a1))+(27)*4u),0u-r_u32((((uint32)(a1))+(27)*4u)));
    w_u32((((uint32)(a1))+(2)*4u),v9);
  }
  if (((((sint16)(r_u16((a1+(150)*2u)))) & 0x10) == 0))
    w_u32((((uint32)(a1))+(27)*4u),(r_u32((((uint32)(a1))+(27)*4u))+(0x4000)));
  sub_8006C730((a1+(8)*2u),(a1+(66)*2u));
  if (((((sint16)(r_u16((a1+(150)*2u)))) & 0x80) != 0))
  {
    v10 = ((sint16)(r_u16(a1)));
    v27 = 4;
    w_u16(a1,(v10 | 0x200));
    sub_8006CCE0(&v23,(a1+(18)*2u),&v27);
    ((void)((a1+(18)*2u)),(void)(&v23),abort(),0u);
  }
  if (((!r_u32(0x800FF738u) && ((((sint16)(r_u16((a1+(150)*2u)))) & 0xE) != 0)) && (r_u32(0x800FF3A8u) < 200)))
  {
    v23 = ((v23&0xFFFF0000u)|(((512)&0xFFFFu)<<0));
    w_u32(((uint32)((((uint32)(&v23))+(2)*1u))),4096);
    sub_80034FC4(((sint32)(&v23)));
    {
      sint32 origin[3] = {v24,v25,v26};
      xport_draft_host_sub_80066B8C_p12(&v23,origin,a1+4);
    }
    sub_80034F9C(((sint32)(&v23)));
    v11 = sub_8006696C((a1+(2)*2u),(r_u32(0x800FF904u) + 4));
    v3 = (v11 < 8193);
    v12 = (v11 < 1025);
    if (v3)
    {
      v13 = 4;
      if (!v12)
        v13 = 2;
    }
    else
    {
      v13 = 1;
    }
    sub_800350E8(v13);
    v14 = ((sint16)(r_u16((a1+(150)*2u))));
    if (((v14 & 2) != 0))
    {
      sub_800350FC(r_u16(0x800ECC94u),r_u16(0x800ECC96u),r_u16(0x800ECC98u));
      v15 = (((unsigned short)(r_u16(0x800ECC94u))) >> 5);
      v16 = (((unsigned short)(r_u16(0x800ECC96u))) >> 5);
      v17 = (((unsigned short)(r_u16(0x800ECC98u))) >> 5);
    }
    else
      if (((v14 & 4) != 0))
    {
      sub_800350FC(128,128,128);
      v15 = 6;
      v16 = 6;
      v17 = 6;
    }
    else
    {
      if (((v14 & 8) == 0))
        goto LABEL_26;
      v18 = sub_80066570(128);
      sub_800350FC(v18,128,0);
      v15 = (v18 >> 5);
      v16 = 6;
      v17 = 0;
    }
    sub_80035110(v15,v16,v17);
    LABEL_26:
    w_u32(0x800FF3ACu,0);

    for (i = 0; (i < 4); ++i)
    {
      v20 = sub_80032DC0(88);
      if (v20)
        sub_80035124(v20,(((uint32)(a1))+(1)*4u),16,0x2000,16);
    }

    w_u32(0x800FF3ACu,1);
  }
  result = (((sint16)(r_u16((a1+(150)*2u)))) & 0x40);
  if (result)
  {
    if (r_u32(0x800FF5A0u))
    {
      result = (sub_8006696C((a1+(2)*2u),(r_u32(0x800FF5A0u) + 4)) < 128);
      if (result)
      {
        ((void)((r_u32(0x800FF5A0u) + ((sint16)(r_u16(((uint32)((r_u32(((uint32)((r_u32(0x800FF5A0u) + 68)))) + 48)))))))),(void)(r_u8((((uint32)(a1))+(297)*1u))),(void)(0x800A71CCu),(void)(0),abort(),0u);
        return ((void)((((sint32)(a1)) + ((sint16)(r_u16(((uint32)((r_u32((((uint32)(a1))+(17)*4u)) + 16)))))))),abort(),0u);
      }
    }
  }
  return result;
}
uint32 sub_80052F34(uint32 a1)
{
  sint32 v1;
  sint32 result;
  v1 = r_u16(((uint32)((a1 + 390))));
  if ((v1 == 8))
    return sub_80063038(a1,1,0,-1);
  if ((r_u16(((uint32)((a1 + 390)))) >= 9u))
  {
    if ((v1 != 64))
    {
      result = 16;
      if ((r_u16(((uint32)((a1 + 390)))) >= 0x41u))
      {
        result = 256;
        if ((v1 != 256))
          return result;
        return sub_80053358(a1);
      }
      if ((v1 != 16))
        return result;
    }
    return sub_80063038(a1,1,0,-1);
  }
  result = 4;
  if (((v1 == 1) || (v1 == 4)))
    return sub_80053358(a1);
  return result;
}
uint32 sub_8004B914(uint32 a1, uint32 a2)
{
  sint32 result;
  result = a2;
  if ((r_u32(0x800FF384u) != 1))
  {
    if (r_u32(0x800FF384u))
      return (2 * a2);
    else
      return (a2 - (((sint32)(a2)) >> 2));
  }
  return result;
}






void nullsub_31(void)
{
  ;
}
/* TODO Missing call adapter BYTE4 */
/* TODO Missing call adapter HIDWORD */
/* TODO Missing call adapter LODWORD */
/* TODO Missing call adapter SHIDWORD */
/* TODO Missing call adapter WORD2 */
/* TODO Missing call adapter indirect */
/* TODO Missing call adapter sub_80087A3C */
/* TODO Missing call adapter indirect */
/* TODO Missing call adapter sub_80087A3C */
uint32 sub_8004FF6C(uint32 a1)
{
  union { sint32 words[36]; uint8 bytes[144]; } raycast;
  uint32 position_output[3];
  sint32 v1;
  sint32 result;
  uint32 v3;
  sint32 v4;
  sint32 v5;
  sint32 v6;
  sint32 v7;
  sint32 v8;
  short v9;
  sint32 v10;
  sint32 v11;
  sint32 v12;
  sint32 v13;
  sint32 v14;
  sint32 v15;
  short v16;
  sint32 v17;
  sint32 v18;
  sint32 v19;
  sint32 v20;
  unsigned short v21;
  sint32 v22;
  sint32 v23;
  uint32 v24;
  sint32 v25;
  sint32 v26;
  sint32 v27;
  sint32 v28;
  sint32 v29;
  sint32 v30;
  sint32 v31;
  sint32 v32;
  unsigned short v33;
  sint32 v34;
  sint32 v35;
  short v36;
  unsigned short v37;
  sint32 v38;
  sint32 v39;
  sint32 v40;
  sint32 v41;
  short v42;
  short v43;
  uint32 v44;
  sint32 v45;
  sint32 v46;
  sint32 v47;
  sint32 v48;
  sint32 v49;
  sint32 v50;
  sint32 v51;
  sint32 v52;
  sint32 v53;
  uint32 v54;
  sint32 v55;
  sint32 v56;
  sint32 v57;
  sint32 v58;
  sint32 v59;
  sint32 v60;
  sint32 v61;
  sint32 v62;
  sint32 v63;
  uint32 v64;
  short v65;
  sint32 v66;
  sint32 v67;
  sint32 v68;
  sint32 v69;
  uint32 v70;
  sint32 v71;
  sint32 v72;
  sint32 v73;
  sint32 v74;
  sint32 v75;
  uint32 v76;
  short v77;
  short v78;
  sint32 v79;
  sint32 v80;
  sint32 v81;
  sint32 v82;
  sint32 v83;
  sint32 v84;
  sint32 v85;
  sint32 v86;
  short v87;
  short v88;
  short v89;
  sint32 v90;
  short v91;
  sint32 v92;
  short v93;
  sint32 v94;
  sint32 v95;
  sint32 effect_position0[3], effect_position1[3];
  sint32 v96;
  uint32 v97;
  short v98;
  sint32 v99;
  sint32 v100;
  sint32 v101;
  sint32 v102;
  short v103;
  sint32 v104;
  sint32 v105;
  short v106;
  sint32 v107;
  sint32 v108;
  sint32 v109;
  sint32 v110;
  sint32 v111;
  sint32 v112;
  long long v113;
  sint32 v114;
  sint32 v115;
  sint32 v116;
  sint32 v117;
  int v127[4];
  sint32 v128;
  sint32 v129;
  (v1 = a1);
  (result = -2146828288);
  switch (r_u16(((uint32)((a1 + 472)))))
  {
    case 0:
      (v3 = r_u16(((uint32)((a1 + 474)))));
      (result = (v3 < 2));
      if ((v3 == 1))
    {
      if (((r_u32(((uint32)((a1 + 396)))) & 2) != 0))
      {
        (result = (r_u16(((uint32)((a1 + 216)))) & 2));
        if (!result)
          return result;
        (v10 = ((sint8)(r_u8(((uint32)((a1 + 24)))))));
        w_u8(((uint32)((a1 + 382))),0);
      }
      else
      {
        (v11 = (((sint16)(r_u16(((uint32)((a1 + 460)))))) << 14));
        (v12 = (((sint16)(r_u16(((uint32)((a1 + 456)))))) << 12));
        (result = (v11 < (r_u32(((uint32)((a1 + 8)))) + v12)));
        if ((v11 >= (r_u32(((uint32)((a1 + 8)))) + v12)))
          return result;
        w_u32(((uint32)((a1 + 8))),(v11 - v12));
        w_u8(((uint32)((a1 + 382))),0);
        w_u32(((uint32)((a1 + 112))),0);
        w_u32(((uint32)((a1 + 108))),0);
        (v10 = ((sint8)(r_u8(((uint32)((a1 + 24)))))));
        w_u32(((uint32)((a1 + 104))),0);
      }
      sub_80063038(a1,4,v10,-1);
      (v8 = r_u16(((uint32)((v1 + 390)))));
      w_u16(((uint32)((v1 + 474))),2);
      (result = 8);
      goto LABEL_20;
    }
      if ((r_u16(((uint32)((a1 + 474)))) < 2u))
    {
      if (r_u16(((uint32)((a1 + 474)))))
        return result;
      (v4 = r_u32(0x800FF5A0u));
      (v5 = (r_u32(0x800FF5A0u) == 0));
      w_u32(((uint32)((a1 + 396))),(r_u32(((uint32)((a1 + 396))))&(~1u)));
      if (!v5)
        w_u16(((uint32)((a1 + 18))),(3072 - ((void)((r_u32(((uint32)((v4 + 12)))) - r_u32(((uint32)((a1 + 12)))))),(void)((r_u32(((uint32)((v4 + 4)))) - r_u32(((uint32)((a1 + 4)))))),abort(),0u)));
      (v6 = sub_80067A18((v1 + 4),0,4096));
      if ((v6 == -1))
        return ((void)((v1 + ((sint16)(r_u16(((uint32)((r_u32(((uint32)((v1 + 68)))) + 16)))))))),abort(),0u);
      (v7 = ((sint16)(r_u16(((uint32)((v1 + 456)))))));
      if ((((v6 - r_u32(((uint32)((v1 + 8))))) >> 12) >= (2 * v7)))
      {
        w_u16(((uint32)((v1 + 460))),(v6 >> 14));
        w_u16(((uint32)((v1 + 474))),1);
        sub_80063038(v1,4,21,22);
        (v9 = r_u16(((uint32)((v1 + 78)))));
        w_u8(((uint32)((v1 + 382))),1);
        w_u16(((uint32)((v1 + 78))),(v9 & 0xFFEF));
        return sub_80062D84(v1);
      }
      w_u32(((uint32)((v1 + 8))),(v6 - (v7 << 12)));
      w_u16(((uint32)((v1 + 472))),1);
      w_u8(((uint32)((v1 + 382))),0);
      w_u32(((uint32)((v1 + 112))),0);
      w_u32(((uint32)((v1 + 108))),0);
      (v8 = r_u16(((uint32)((v1 + 390)))));
      (result = 8);
      w_u32(((uint32)((v1 + 104))),0);
      LABEL_20:
      if ((v8 != 8))
        return sub_80062D70(v1);

      return result;
    }
      (result = 2);
      if ((v3 == 2))
    {
      (result = r_u8(((uint32)((a1 + 303)))));
      if (r_u8(((uint32)((a1 + 303)))))
      {
        (v13 = r_u16(((uint32)((a1 + 78)))));
        w_u32(((uint32)((a1 + 472))),1);
        (result = (v13 | 0x10));
        w_u16(((uint32)((a1 + 78))),result);
      }
    }
      return result;

    case 1:
      (result = r_u16(((uint32)((a1 + 474)))));
      if (!(r_u16(((uint32)((a1 + 474))))))
    {
      (v14 = r_u32(((uint32)((a1 + 396)))));
      w_u8(((uint32)((a1 + 382))),0);
      w_u32(((uint32)((a1 + 396))),(v14 & 0xFFFFFFFE));
      w_u32(((uint32)((a1 + 112))),0);
      w_u32(((uint32)((a1 + 108))),0);
      w_u32(((uint32)((a1 + 104))),0);
      w_u16(((uint32)((a1 + 136))),0);
      w_u16(((uint32)((a1 + 134))),0);
      w_u16(((uint32)((a1 + 132))),0);
      w_u16(((uint32)((a1 + 142))),0);
      w_u16(((uint32)((a1 + 140))),0);
      (v15 = r_u8(((uint32)((a1 + 303)))));
      w_u16(((uint32)((a1 + 138))),0);
      if (v15)
        sub_80063038(a1,0,0,-1);
      (v16 = sub_800539A8(v1));
      (v17 = r_u16(((uint32)((v1 + 390)))));
      w_u16(((uint32)((v1 + 472))),(v16 + 2));
      (result = 4);
      w_u16(((uint32)((v1 + 474))),0);
      if ((v17 == 4))
      {
        (v18 = sub_80066570(16));
        (v19 = 43);
        if (!v18)
          return sub_80069DF0(v19,0x2000,0);
        (result = sub_80066570(16));
        (v19 = 44);
        if (!result)
          return sub_80069DF0(v19,0x2000,0);
      }
    }
      return result;

    case 2:
      (v20 = r_u16(((uint32)((a1 + 474)))));
      (result = 1);
      if (!(r_u16(((uint32)((a1 + 474))))))
    {
      w_u32(((uint32)((a1 + 112))),0);
      w_u32(((uint32)((a1 + 108))),0);
      w_u32(((uint32)((a1 + 104))),0);
      sub_80063038(a1,3,0,-1);
      (result = (r_u16(((uint32)((v1 + 474)))) + 1));
      goto LABEL_210;
    }
      if ((v20 == 1))
    {
      if (r_u32(0x800FF5A0u))
      {
        xport_draft_host_sub_80066B8C_p1(&v113,v1+4,r_u32(0x800FF5A0u)+4);
        (v113 = ((v113&0xFFFF0000u)|(((0)&0xFFFFu)<<0)));
        (v21 = r_u16(((uint32)((v1 + 20)))));
        (v114 = r_u32(((uint32)((v1 + 16)))));
        (v115 = ((v115&0xFFFF0000u)|(((v21)&0xFFFFu)<<0)));
        sub_80066CF0((((unsigned short)(v114)) | (((v114>>16)&65535u) << 16)),v21,(v1 + 132),(v1 + 138),(uint32)v113,(uint32)((unsigned long long)v113 >> 32),32);
      }
      (result = r_u8(((uint32)((v1 + 303)))));
      if (r_u8(((uint32)((v1 + 303)))))
      {
        w_u16(((uint32)((v1 + 140))),0);
        w_u16(((uint32)((v1 + 134))),0);
        goto LABEL_86;
      }
    }
      return result;

    case 3:
      (v22 = r_u16(((uint32)((a1 + 474)))));
      (result = 1);
      if (!(r_u16(((uint32)((a1 + 474))))))
    {
      (v23 = (a1 + 484));
      (v24 = ((uint32)((a1 + 484))));
      (v25 = 128);
      (v107 = 192);
      (v26 = 1024);
      goto LABEL_43;
    }
      if ((v22 != 1))
      return result;
      (v27 = r_u32(((uint32)((a1 + 396)))));
      if (((v27 & 0x400) != 0))
    {
      (result = (v27 & 0xFFFFFFFE));
      w_u32(((uint32)((v1 + 396))),(v27 & 0xFFFFFFFE));
      LABEL_47:
      w_u32(((uint32)((v1 + 472))),((unsigned short)(v22)));

      return result;
    }
      (result = (v27 & 1));
      if (((v27 & 1) == 0))
      goto LABEL_47;
      (result = r_u32(0x800FF5A0u));
      if (r_u32(0x800FF5A0u))
    {
      (result = -2);
      if ((r_u32(((uint32)((v1 + 220)))) < r_u32(((uint32)((v1 + 512))))))
      {
        w_u32(((uint32)((v1 + 396))),(v27 & 0xFFFFFFFE));
        (result = 2);
        goto LABEL_166;
      }
    }
      return result;

    case 4:
      (v28 = r_u16(((uint32)((a1 + 474)))));
      (result = 1);
      if (!(r_u16(((uint32)((a1 + 474))))))
    {
      w_u32(((uint32)((a1 + 112))),0);
      w_u32(((uint32)((a1 + 108))),0);
      (v29 = r_u32(((uint32)((a1 + 4)))));
      (v30 = r_u32(0x800FF5A0u));
      w_u32(((uint32)((a1 + 104))),0);
      (v113 = (((unsigned long long)(v113)&0xFFFFFFFF00000000ULL)|(((unsigned long long)(v29)&0xFFFFFFFFULL)<<0)));
      (v113 = (((unsigned long long)(v113)&0xFFFFFFFFULL)|(((unsigned long long)((r_u32(((uint32)((a1 + 8)))) - (((sint16)(r_u16(((uint32)((a1 + 458)))))) << 12)))&0xFFFFFFFFULL)<<32)));
      (v114 = r_u32(((uint32)((a1 + 12)))));
      (v5 = (sub_800679A4(&v113,(v30 + 4)) == 0));
      (result = 1);
      if (v5)
        goto LABEL_166;
      sub_80052F34(v1);
      (v31 = r_u16(((uint32)((v1 + 474)))));
      w_u16(((uint32)((v1 + 460))),0);
      goto LABEL_209;
    }
      if ((v28 == 1))
    {
      (v32 = ((sint16)(r_u16(((uint32)((a1 + 460)))))));
      (v5 = (v32 < ((unsigned short)(sub_8004BDFC(a1,1u,3u)))));
      (result = (v32 + 1));
      if (v5)
        w_u16(((uint32)((v1 + 460))),result);
      else
        w_u32(((uint32)((v1 + 472))),((unsigned short)(v28)));
      if (r_u32(0x800FF5A0u))
      {
        xport_draft_host_sub_80066B8C_p1(&v113,v1+4,r_u32(0x800FF5A0u)+4);
        (v113 = ((v113&0xFFFF0000u)|(((0)&0xFFFFu)<<0)));
        (v33 = r_u16(((uint32)((v1 + 20)))));
        (v116 = r_u32(((uint32)((v1 + 16)))));
        (v117 = ((v117&0xFFFF0000u)|(((v33)&0xFFFFu)<<0)));
        return sub_80066CF0((((unsigned short)(v116)) | (((v116>>16)&65535u) << 16)),v33,(v1 + 132),(v1 + 138),(uint32)v113,(uint32)((unsigned long long)v113 >> 32),32);
      }
    }
      return result;

    case 5:
      (v34 = r_u16(((uint32)((a1 + 474)))));
      (v23 = (v1 + 484));
      if (!(r_u16(((uint32)((v1 + 474))))))
    {
      (a1 = v1);
      (v24 = ((uint32)((v1 + 484))));
      (v25 = 1024);
      (v107 = 2048);
      (v26 = 512);
      LABEL_43:
      (v20 = ((v20&0xFFFF0000u)|(((1)&0xFFFFu)<<0)));

      (result = sub_8004BA64(a1,v24,((uint32)((r_u32(0x800FF5A0u) + 4))),v25,v107,r_u16(((uint32)((v1 + 392)))),v26,1,1));
      goto LABEL_85;
    }
      (result = 1);
      if ((v34 == 1))
    {
      (v35 = r_u32(((uint32)((v1 + 396)))));
      if (((v35 & 0x400) == 0))
        goto LABEL_89;
      (result = (v35 & 0xFFFFFFFE));
      w_u32(((uint32)((v1 + 396))),(v35 & 0xFFFFFFFE));
      goto LABEL_90;
    }
      return result;

    case 6:
      if (r_u16(((uint32)((a1 + 474)))))
    {
      (result = 128);
      if ((r_u16(((uint32)((a1 + 474)))) == 1))
      {
        if (((r_u16(((uint32)((a1 + 390)))) == 128) && r_u32(0x800FF5A0u)))
        {
          (v36 = ((void)((r_u32(((uint32)((r_u32(0x800FF5A0u) + 12)))) - r_u32(((uint32)((a1 + 12)))))),(void)((r_u32(((uint32)((r_u32(0x800FF5A0u) + 4)))) - r_u32(((uint32)((a1 + 4)))))),abort(),0u));
          (v113 = ((v113&0xFFFF0000u)|(((0)&0xFFFFu)<<0)));
          w_u32(((uint32)((((uint32)(&v113))+(2)*1u))),((3072 - v36) & 0xFFF));
          (v37 = r_u16(((uint32)((v1 + 20)))));
          (v114 = r_u32(((uint32)((v1 + 16)))));
          (v115 = ((v115&0xFFFF0000u)|(((v37)&0xFFFFu)<<0)));
          sub_80066CF0((((unsigned short)(v114)) | (((v114>>16)&65535u) << 16)),v37,(v1 + 132),(v1 + 138),(uint32)v113,(uint32)((unsigned long long)v113 >> 32),32);
        }
        (result = 1);
        if (((r_u32(((uint32)((v1 + 396)))) & 0x400) != 0))
          goto LABEL_166;
        (result = 1);
        if (r_u8(((uint32)((v1 + 303)))))
          goto LABEL_166;
        (result = r_u32(((uint32)((v1 + 548)))));
        if (result)
        {
          (result = 1);
          if ((r_u32(((uint32)((v1 + 220)))) < r_u32(((uint32)((v1 + 552))))))
          {
            w_u16(((uint32)((v1 + 472))),1);
            goto LABEL_167;
          }
        }
      }
    }
    else
    {
      if (r_u8(((uint32)((a1 + 26)))))
        sub_80063038(a1,0,0,-1);
      (result = 1);
      w_u16(((uint32)((v1 + 474))),1);
    }
      return result;

    case 7:
      (v34 = r_u16(((uint32)((a1 + 474)))));
      (result = 1);
      if (r_u16(((uint32)((v1 + 474)))))
    {
      if ((v34 != 1))
        return result;
      (v35 = r_u32(((uint32)((v1 + 396)))));
      if (((v35 & 0x400) != 0))
      {
        (result = (v35 & 0xFFFFFFFE));
        w_u32(((uint32)((v1 + 396))),(v35 & 0xFFFFFFFE));
      }
      else
      {
        LABEL_89:
        (result = (v35 & 1));

        if (((v35 & 1) != 0))
          return result;
      }
      LABEL_90:
      w_u32(((uint32)((v1 + 472))),((unsigned short)(v34)));

    }
    else
    {
      (v23 = (v1 + 484));
      (v20 = ((v20&0xFFFF0000u)|(((1)&0xFFFFu)<<0)));
      (result = sub_8004BA64(v1,((uint32)((v1 + 484))),((uint32)((v1 + 4))),1024,2048,r_u16(((uint32)((v1 + 392)))),4096,1,1));
      LABEL_85:
      (v38 = v1);

      if (result)
        goto LABEL_157;
      LABEL_86:
      w_u16(((uint32)((v1 + 472))),v20);

      w_u16(((uint32)((v1 + 474))),0);
    }
      return result;

    case 8:
      (result = 1);
      if (r_u16(((uint32)((a1 + 474)))))
    {
      if ((r_u16(((uint32)((a1 + 474)))) == 1))
      {
        (result = r_u8(((uint32)((a1 + 303)))));
        if (r_u8(((uint32)((a1 + 303)))))
        {
          (v41 = ((sint16)(r_u16(((uint32)((a1 + 460)))))));
          (v5 = (v41 == 0));
          (v42 = (v41 - 1));
          if (v5)
            ((void)((a1 + ((sint16)(r_u16(((uint32)((r_u32(((uint32)((a1 + 68)))) + 16)))))))),abort(),0u);
          else
            w_u16(((uint32)((a1 + 460))),v42);
          (result = ((sint16)(r_u16(((uint32)((v1 + 462)))))));
          if (!(r_u16(((uint32)((v1 + 462))))))
          {
            sub_800626F8(v1,(uint32)-32,0,0,0);
            (v43 = r_u16(((uint32)(v1))));
            w_u32(((uint32)((v1 + 44))),128);
            w_u16(((uint32)(v1)),(v43 | 0x800));
            (result = 1);
            w_u16(((uint32)((v1 + 462))),1);
          }
        }
      }
    }
    else
    {
      w_u32(((uint32)((a1 + 112))),0);
      w_u32(((uint32)((a1 + 108))),0);
      w_u32(((uint32)((a1 + 104))),0);
      w_u16(((uint32)((a1 + 136))),0);
      w_u16(((uint32)((a1 + 134))),0);
      w_u16(((uint32)((a1 + 132))),0);
      w_u16(((uint32)((a1 + 142))),0);
      w_u16(((uint32)((a1 + 140))),0);
      (v39 = r_u32(((uint32)((a1 + 396)))));
      w_u16(((uint32)((a1 + 138))),0);
      w_u16(((uint32)((a1 + 212))),0);
      w_u32(((uint32)((a1 + 396))),(v39 & 0xFFFFFFFE));
      sub_8004B948(a1);
      sub_80063038(v1,5,0,-1);
      sub_80062D84(v1);
      (v40 = r_u16(((uint32)((v1 + 390)))));
      w_u16(((uint32)((v1 + 474))),1);
      w_u16(((uint32)((v1 + 460))),32);
      (result = 256);
      w_u16(((uint32)((v1 + 462))),0);
      if ((v40 == 256))
        return sub_80069EF4(120,(v1 + 4),3);
    }
      return result;

    case 0xA:
      (v44 = r_u16(((uint32)((a1 + 474)))));
      (result = (v44 < 2));
      if ((v44 == 1))
    {
      (v58 = (a1 + 104));
      if (r_u16(((uint32)((a1 + 460)))))
      {
        sub_80034F9C((a1 + 16));
        sub_800350E8(2);
        (v59 = 0);
        sub_800350FC(r_u16(0x800ECC94u),r_u16(0x800ECC96u),r_u16(0x800ECC98u));
        sub_80035110((((unsigned short)(r_u16(0x800ECC94u))) >> 5),(((unsigned short)(r_u16(0x800ECC96u))) >> 5),(((unsigned short)(r_u16(0x800ECC98u))) >> 5));
        w_u32(0x800FF3ACu,0);
        do
        {
          (v60 = sub_80032DC0(88));
          if (v60)
            sub_80035124(v60,((uint32)((v1 + 4))),16,0x2000,16);
          ++v59;
        }
        while ((v59 < 16));
        (v61 = ((sint16)(r_u16(((uint32)((v1 + 460)))))));
        w_u32(0x800FF3ACu,1);
        (result = (v61 - 1));
        w_u16(((uint32)((v1 + 460))),result);
        return result;
      }
      LABEL_208:
      w_u32(((uint32)((v58 + 8))),0);

      w_u32(((uint32)((v58 + 4))),0);
      (v31 = r_u16(((uint32)((a1 + 474)))));
      w_u32(((uint32)((a1 + 104))),0);
      LABEL_209:
      (result = (v31 + 1));

      goto LABEL_210;
    }
      if ((r_u16(((uint32)((a1 + 474)))) < 2u))
    {
      if (r_u16(((uint32)((a1 + 474)))))
        return result;
      w_u16(((uint32)((a1 + 16))),0);
      w_u16(((uint32)((a1 + 136))),0);
      w_u16(((uint32)((a1 + 134))),0);
      w_u16(((uint32)((a1 + 132))),0);
      w_u16(((uint32)((a1 + 142))),0);
      w_u16(((uint32)((a1 + 140))),0);
      (v45 = r_u32(((uint32)((a1 + 396)))));
      w_u16(((uint32)((a1 + 138))),0);
      w_u16(((uint32)((a1 + 212))),0);
      w_u32(((uint32)((a1 + 396))),(v45 & 0xFFFFFFFE));
      sub_80062D84(a1);
      sub_800667CC((v1 + 484),1024,(v1 + 16));
      xport_draft_host_sub_8006C3AC_p1(position_output,(v1 + 4),(v1 + 484));
      v113 = (unsigned long long)position_output[0] | ((unsigned long long)position_output[1]<<32);
      v114 = position_output[2];
      (v46 = (((unsigned long long)(v113)>>32)&0xFFFFFFFFu));
      (v47 = v114);
      w_u32(((uint32)((v1 + 484))),v113);
      w_u32(((uint32)((v1 + 488))),v46);
      w_u32(((uint32)((v1 + 492))),v47);
      (raycast.words[0] = r_u32(((uint32)((v1 + 4)))));
      (raycast.words[1] = r_u32(((uint32)((v1 + 8)))));
      (raycast.words[2] = r_u32(((uint32)((v1 + 12)))));
      (raycast.words[3] = r_u32(((uint32)((v1 + 484)))));
      (raycast.words[4] = r_u32(((uint32)((v1 + 488)))));
      (raycast.words[5] = r_u32(((uint32)((v1 + 492)))));
      xport_draft_host_sub_8007BB24_p1(&raycast);
      (raycast.bytes[136] = 0);
      xport_draft_host_sub_8007DD04_p1(&raycast,1);
      if (raycast.words[26])
      {
        (v48 = (raycast.words[16] - 256));
        if (((raycast.words[16] - 256) >= 64))
        {
          (v49 = (raycast.words[16] - 256));
          if ((v48 < 0))
            (v49 = (raycast.words[16] - 193));
          w_u16(((uint32)((v1 + 460))),(v49 >> 6));
          sub_800667CC((v1 + 484),v48,(v1 + 16));
          xport_draft_host_sub_8006C3AC_p1(position_output,(v1 + 4),(v1 + 484));
      v113 = (unsigned long long)position_output[0] | ((unsigned long long)position_output[1]<<32);
      v114 = position_output[2];
          (v50 = (((unsigned long long)(v113)>>32)&0xFFFFFFFFu));
          (v51 = v114);
          w_u32(((uint32)((v1 + 484))),v113);
          w_u32(((uint32)((v1 + 488))),v50);
          w_u32(((uint32)((v1 + 492))),v51);
          xport_draft_host_sub_8006C3AC_p1(v127,v1+484,v1+4);
          (v128 = ((sint16)(r_u16(((uint32)((v1 + 460)))))));
          xport_draft_host_sub_8006C4EC_p123(position_output,v127,&v128);
          v113 = (unsigned long long)position_output[0] | ((unsigned long long)position_output[1]<<32);
          v114 = position_output[2];
          (v52 = (((unsigned long long)(v113)>>32)&0xFFFFFFFFu));
          (v53 = v114);
          w_u32(((uint32)((v1 + 104))),v113);
          w_u32(((uint32)((v1 + 108))),v52);
          w_u32(((uint32)((v1 + 112))),v53);
          (v54 = ((uint32)((v1 + 4))));
          goto LABEL_117;
        }
        w_u32(((uint32)((v1 + 112))),0);
        w_u32(((uint32)((v1 + 108))),0);
        w_u32(((uint32)((v1 + 104))),0);
        w_u16(((uint32)((v1 + 460))),1);
      }
      else
      {
        w_u16(((uint32)((v1 + 460))),16);
        xport_draft_host_sub_8006C3AC_p1(position_output,(v1 + 484),(v1 + 4));
      v113 = (unsigned long long)position_output[0] | ((unsigned long long)position_output[1]<<32);
      v114 = position_output[2];
        (v129 = 16);
        xport_draft_host_sub_8006C4EC_p123(v127,position_output,&v129);
        (v55 = v127[1]);
        (v56 = v127[2]);
        w_u32(((uint32)((v1 + 104))),v127[0]);
        w_u32(((uint32)((v1 + 108))),v55);
        w_u32(((uint32)((v1 + 112))),v56);
        (v57 = sub_80067A18((v1 + 484),((sint16)(r_u16(((uint32)((v1 + 456)))))),(4 * ((sint16)(r_u16(((uint32)((v1 + 456)))))))));
        (v54 = ((uint32)((v1 + 4))));
        if ((v57 != -1))
        {
          LABEL_117:
          sub_8001D320(v54,128,0x80u,0x40u,0,5,0,100);

          sub_80063038(v1,5,0,-1);
          sub_80069DF0(45,0x2000,0);
          w_u16(((uint32)((v1 + 212))),0);
          sub_8004B948(v1);
          (result = (r_u16(((uint32)((v1 + 474)))) + 1));
          goto LABEL_210;
        }
        w_u8(((uint32)((v1 + 382))),1);
      }
      (v54 = ((uint32)((v1 + 4))));
      goto LABEL_117;
    }
      (result = 2);
      if ((v44 == 2))
    {
      if (r_u8(((uint32)((a1 + 382)))))
      {
        LABEL_188:
        (result = r_u8(((uint32)((v1 + 303)))));

        if (r_u8(((uint32)((v1 + 303)))))
          return ((void)((v1 + ((sint16)(r_u16(((uint32)((r_u32(((uint32)((v1 + 68)))) + 16)))))))),abort(),0u);
        return result;
      }
      (result = r_u8(((uint32)((a1 + 303)))));
      (v62 = (a1 + 4));
      if (!(r_u8(((uint32)((v1 + 303))))))
        return result;
      (v63 = sub_80067A18(v62,0,256));
      (a1 = v1);
      if ((v63 == -1))
        return sub_8004FB00(a1,0);
      (result = (r_u16(((uint32)((v1 + 474)))) + 1));
      LABEL_210:
      w_u16(((uint32)((v1 + 474))),result);

    }
      return result;

    case 0xB:
      (v64 = r_u16(((uint32)((a1 + 474)))));
      (result = (v64 < 2));
      if ((v64 == 1))
      goto LABEL_138;
      if ((r_u16(((uint32)((a1 + 474)))) >= 2u))
    {
      (result = 256);
      if ((v64 != 2))
        return result;
      if ((r_u16(((uint32)((a1 + 390)))) == 256))
      {
        sub_8006A294(r_u32(((uint32)((a1 + 480)))));
        w_u32(((uint32)((v1 + 480))),0);
      }
      goto LABEL_188;
    }
      if (r_u16(((uint32)((a1 + 474)))))
      return result;
      sub_800626F8(a1,0,255,0,0);
      sub_80062D84(v1);
      sub_8004B948(v1);
      w_u32(((uint32)((v1 + 176))),37765184);
      w_u16(((uint32)((v1 + 180))),10240);
      w_u16(((uint32)((v1 + 212))),0);
      w_u16(((uint32)((v1 + 182))),10);
      sub_80063038(v1,6,0,-1);
      if ((r_u16(((uint32)((v1 + 390)))) == 256))
      w_u32(((uint32)((v1 + 480))),sub_80069EF4(18,(v1 + 4),3));
    else
      sub_80069EF4(25,(v1 + 4),3);
      w_u32(((uint32)((v1 + 396))),(r_u32(((uint32)((v1 + 396))))&(~1u)));
      w_u32(((uint32)((v1 + 112))),0);
      w_u32(((uint32)((v1 + 108))),0);
      (v65 = r_u16(((uint32)((v1 + 474)))));
      w_u32(((uint32)((v1 + 104))),0);
      w_u16(((uint32)((v1 + 474))),(v65 + 1));
      LABEL_138:
    (result = 256);

      if (!(r_u8(((uint32)((v1 + 303))))))
      return result;
      (v66 = r_u16(((uint32)((v1 + 390)))));
      if ((v66 != 256))
    {
      if (!sub_80066570(4))
        sub_80069EF4(25,(v1 + 4),3);
      (v66 = r_u16(((uint32)((v1 + 390)))));
    }
      (v67 = 4);
      if ((v66 == 256))
      (v67 = 2);
      (v68 = sub_80066570(v67));
      (v69 = v1);
      if ((!v68 || r_u32(0x800FF738u)))
    {
      sub_80063038(v1,9,0,-1);
      (result = (r_u16(((uint32)((v1 + 474)))) + 1));
      goto LABEL_210;
    }
      return sub_80063038(v69,6,0,-1);

    case 0xD:
      (v70 = r_u16(((uint32)((a1 + 474)))));
      (result = (v70 < 2));
      if ((v70 == 1))
    {
      (v113 = (((unsigned long long)(v113)&0xFFFFFFFF00000000ULL)|(((unsigned long long)(64512)&0xFFFFFFFFULL)<<0)));
      (v113 = (((unsigned long long)(v113)&0xFFFF0000FFFFFFFFULL)|(((unsigned long long)(0)&0xFFFFULL)<<32)));
      sub_80034F9C(((sint32)(&v113)));
      (v114 = 268435968);
      (v115 = ((v115&0xFFFF0000u)|(((0)&0xFFFFu)<<0)));
      sub_80034FC4(((sint32)(&v114)));
      sub_800350E8(2);
      (v72 = 0);
      sub_800350FC(r_u16(0x800ECC94u),r_u16(0x800ECC96u),r_u16(0x800ECC98u));
      (v73 = 24);
      sub_80035110((((unsigned short)(r_u16(0x800ECC94u))) >> 5),(((unsigned short)(r_u16(0x800ECC96u))) >> 5),(((unsigned short)(r_u16(0x800ECC98u))) >> 5));
      w_u32(0x800FF3ACu,0);
      do
      {
        (v74 = sub_80032DC0(88));
        if (v74)
          sub_80035124(v74,((uint32)((v1 + 4))),v73,0x2000,16);
        ++v72;
        (v73 += 4);
      }
      while ((v72 < 4));
      (v75 = r_u32(((uint32)((v1 + 396)))));
      w_u32(0x800FF3ACu,1);
      (result = (v75 & 1));
      if (result)
        return result;
      (v5 = (sub_80066570(2) == 0));
      (result = 8);
      if (!v5)
      {
        (result = (r_u16(((uint32)((v1 + 474)))) - 1));
        w_u16(((uint32)((v1 + 474))),result);
        return result;
      }
    }
    else
    {
      if ((r_u16(((uint32)((a1 + 474)))) >= 2u))
      {
        (result = 2);
        if ((v70 == 2))
          goto LABEL_188;
        return result;
      }
      if (r_u16(((uint32)((a1 + 474)))))
        return result;
      (v23 = (a1 + 484));
      (v71 = sub_8004BA64(a1,((uint32)((a1 + 484))),((uint32)((a1 + 4))),256,512,r_u16(((uint32)((a1 + 392)))),4096,1,1));
      (v38 = v1);
      if (v71)
      {
        LABEL_157:
        (result = sub_80053784(v38,v23));

        w_u16(((uint32)((v1 + 474))),1);
        return result;
      }
      (result = 8);
    }
      LABEL_166:
    w_u16(((uint32)((v1 + 472))),result);

      LABEL_167:
    w_u16(((uint32)((v1 + 474))),0);

      return result;

    case 0xE:
      (v76 = r_u16(((uint32)((a1 + 474)))));
      (result = (v76 < 2));
      if ((v76 == 1))
      goto LABEL_175;
      if ((r_u16(((uint32)((a1 + 474)))) >= 2u))
    {
      (result = 2);
      if ((v76 != 2))
        return result;
      (v113 = ((v113&0xFFFF0000u)|(((512)&0xFFFFu)<<0)));
      w_u32(((uint32)((((uint32)(&v113))+(2)*1u))),4096);
      sub_80034FC4(((sint32)(&v113)));
      sub_800350E8(2);
      (v83 = 0);
      sub_800350FC(r_u16(0x800ECC94u),r_u16(0x800ECC96u),r_u16(0x800ECC98u));
      (v84 = 32);
      sub_80035110((((unsigned short)(r_u16(0x800ECC94u))) >> 5),(((unsigned short)(r_u16(0x800ECC96u))) >> 5),(((unsigned short)(r_u16(0x800ECC98u))) >> 5));
      xport_draft_host_sub_8007CC10_p1(effect_position0,v1,1);
      (raycast.words[0] = 64512);
      (raycast.words[1] = ((raycast.words[1]&0xFFFF0000u)|(((0)&0xFFFFu)<<0)));
      sub_80034F9C(((sint32)(&raycast.words[0])));
      w_u32(0x800FF3ACu,0);
      effect_position0[1] += 0x20000;
      do
      {
        (v85 = sub_80032DC0(88));
        if (v85)
          xport_draft_host_sub_80035124_p2(v85,effect_position0,v84,0x2000,16);
        ++v83;
        (v84 += 4);
      }
      while ((v83 < 4));
      w_u32(0x800FF3ACu,1);
      goto LABEL_188;
    }
      if (r_u16(((uint32)((a1 + 474)))))
      return result;
      sub_80062D84(a1);
      (v77 = r_u16(((uint32)((v1 + 78)))));
      w_u16(((uint32)((v1 + 212))),0);
      w_u16(((uint32)((v1 + 78))),(v77 & 0xFFEF));
      sub_8004B948(v1);
      sub_80063038(v1,6,0,-1);
      w_u32(((uint32)((v1 + 396))),((r_u32(((uint32)((v1 + 396)))) & 0xFFFFEFFE) | 0x1000));
      w_u32(((uint32)((v1 + 112))),0);
      w_u32(((uint32)((v1 + 108))),0);
      (v78 = r_u16(((uint32)((v1 + 474)))));
      w_u32(((uint32)((v1 + 104))),0);
      w_u16(((uint32)((v1 + 474))),(v78 + 1));
      LABEL_175:
    (v113 = (((unsigned long long)(v113)&0xFFFFFFFF00000000ULL)|(((unsigned long long)(64512)&0xFFFFFFFFULL)<<0)));

      (v113 = (((unsigned long long)(v113)&0xFFFF0000FFFFFFFFULL)|(((unsigned long long)(0)&0xFFFFULL)<<32)));
      sub_80034F9C(((sint32)(&v113)));
      (v114 = 268435968);
      (v115 = ((v115&0xFFFF0000u)|(((0)&0xFFFFu)<<0)));
      sub_80034FC4(((sint32)(&v114)));
      sub_800350E8(2);
      (v79 = 0);
      sub_800350FC(r_u16(0x800ECC94u),r_u16(0x800ECC96u),r_u16(0x800ECC98u));
      (v80 = 32);
      sub_80035110((((unsigned short)(r_u16(0x800ECC94u))) >> 5),(((unsigned short)(r_u16(0x800ECC96u))) >> 5),(((unsigned short)(r_u16(0x800ECC98u))) >> 5));
      xport_draft_host_sub_8007CC10_p1(effect_position1,v1,1);
      w_u32(0x800FF3ACu,0);
      effect_position1[1] += 0x20000;
      do
    {
      (v81 = sub_80032DC0(88));
      if (v81)
        xport_draft_host_sub_80035124_p2(v81,effect_position1,v80,0x2000,16);
      ++v79;
      (v80 += 4);
    }
    while ((v79 < 4));
      w_u32(0x800FF3ACu,1);
      (result = r_u8(((uint32)((v1 + 303)))));
      if (!(r_u8(((uint32)((v1 + 303))))))
      return result;
      (v82 = sub_80066570(2));
      (v69 = v1);
      if (v82)
      return sub_80063038(v69,6,0,-1);
      sub_80063038(v1,9,0,-1);
      (result = (r_u16(((uint32)((v1 + 474)))) + 1));
      goto LABEL_210;

    case 0x10:
      (result = 1);
      if (!(r_u16(((uint32)((a1 + 474))))))
    {
      (v86 = sub_80066570(64));
      w_u32(((uint32)((v1 + 104))),0);
      w_u32(((uint32)((v1 + 108))),(-4096 * (v86 + 96)));
      w_u32(((uint32)((v1 + 112))),0);
      (v88 = (sub_80066570(256) - 128));
      (v87 = sub_80066570(256));
      w_u16(((uint32)((v1 + 132))),v88);
      w_u16(((uint32)((v1 + 134))),(v87 - 128));
      w_u16(((uint32)((v1 + 136))),0);
      w_u8(((uint32)((v1 + 144))),16);
      w_u8(((uint32)((v1 + 145))),16);
      w_u8(((uint32)((v1 + 146))),16);
      w_u8(((uint32)((v1 + 382))),1);
      w_u32(((uint32)((v1 + 396))),(r_u32(((uint32)((v1 + 396))))&(~1u)));
      (v89 = sub_80066570(32));
      (v90 = r_u32(((uint32)((v1 + 8)))));
      w_u16(((uint32)((v1 + 460))),(v89 + 16));
      (result = (r_u16(((uint32)((v1 + 456)))) + (v90 >> 12)));
      (v91 = (r_u16(((uint32)((v1 + 474)))) + 1));
      w_u16(((uint32)((v1 + 462))),result);
      w_u16(((uint32)((v1 + 474))),v91);
      return result;
    }
      if ((r_u16(((uint32)((a1 + 474)))) != 1))
      return result;
      (v92 = ((sint16)(r_u16(((uint32)((a1 + 460)))))));
      (v5 = (v92 == 0));
      (v93 = (v92 - 1));
      if (v5)
      return sub_8004FB00(a1,0);
      w_u16(((uint32)((a1 + 460))),v93);
      (v94 = ((sint16)(r_u16(((uint32)((a1 + 462)))))));
      (result = (v94 < (((sint32)(r_u32(((uint32)((a1 + 8)))))) >> 12)));
      (v5 = (v94 >= (((sint32)(r_u32(((uint32)((a1 + 8)))))) >> 12)));
      (v95 = (v94 << 12));
      if (!v5)
    {
      (v96 = r_u32(((uint32)((a1 + 108)))));
      w_u32(((uint32)((a1 + 8))),v95);
      (result = -v96);
      w_u32(((uint32)((a1 + 108))),result);
    }
      return result;

    case 0x20:
      (v97 = r_u16(((uint32)((a1 + 474)))));
      (result = (v97 < 2));
      if ((v97 == 1))
    {
      (v58 = (a1 + 104));
      if (((r_u32(((uint32)((a1 + 396)))) & 1) == 0))
        goto LABEL_208;
      (result = r_u32(0x800FF5A0u));
      if (r_u32(0x800FF5A0u))
      {
        (result = 2);
        if ((r_u32(((uint32)((a1 + 220)))) < r_u32(((uint32)((a1 + 512))))))
          w_u16(((uint32)((a1 + 474))),2);
      }
    }
    else
      if ((r_u16(((uint32)((a1 + 474)))) >= 2u))
    {
      (result = 2);
      if ((v97 == 2))
      {
        w_u32(((uint32)((a1 + 396))),(r_u32(((uint32)((a1 + 396))))&(~1u)));
        sub_80063038(a1,8,0,-1);
        (v98 = r_u16(((uint32)((v1 + 78)))));
        w_u16(((uint32)((v1 + 212))),128);
        (result = 8);
        w_u16(((uint32)((v1 + 476))),8);
        w_u32(((uint32)((v1 + 472))),1);
        w_u16(((uint32)((v1 + 78))),(v98 | 0x10));
      }
    }
    else
      if (!(r_u16(((uint32)((a1 + 474))))))
    {
      if (sub_8004BA64(a1,((uint32)((a1 + 484))),((uint32)((r_u32(0x800FF5A0u) + 4))),128,192,r_u16(((uint32)((a1 + 392)))),1024,1,1))
      {
        (result = sub_80053784(v1,(v1 + 484)));
        w_u16(((uint32)((v1 + 474))),1);
      }
      else
      {
        (result = 2);
        w_u16(((uint32)((v1 + 474))),2);
      }
    }
      return result;

    case 0x30:
      (v99 = r_u16(((uint32)((a1 + 474)))));
      (result = 1);
      if (!(r_u16(((uint32)((a1 + 474))))))
    {
      w_u16(((uint32)((a1 + 78))),(r_u16(((uint32)((a1 + 78))))&(~0x10u)));
      (v101 = r_u32(((uint32)((a1 + 8)))));
      goto LABEL_225;
    }
      (v100 = (a1 + 2));
      if ((v99 != 1))
      return result;
      (v102 = ((sint16)(r_u16(((uint32)((a1 + 462)))))));
      if ((v102 != 60))
      goto LABEL_228;
      w_u16(((uint32)((v1 + 78))),(r_u16(((uint32)((v1 + 78))))|(0x10u)));
      sub_80063038(v1,0,0,-1);
      (v103 = r_u16(((uint32)(v1))));
      w_u32(((uint32)((v1 + 472))),((unsigned short)(v99)));
      (result = (v103 & 0xFFF7));
      w_u16(((uint32)(v1)),result);
      return result;

    case 0x31:
      (v104 = r_u16(((uint32)((a1 + 474)))));
      (result = 1);
      if (r_u16(((uint32)((a1 + 474)))))
    {
      (v100 = (a1 + 2));
      if ((v104 == 1))
      {
        (v102 = ((sint16)(r_u16(((uint32)((a1 + 462)))))));
        if ((v102 == 60))
        {
          w_u16(((uint32)(v1)),(r_u16(((uint32)(v1)))&(~8u)));
          sub_80063038(v1,0,0,-1);
          (result = 1);
          w_u32(((uint32)((v1 + 472))),((unsigned short)(v104)));
          w_u8(((uint32)((v1 + 380))),1);
        }
        else
        {
          LABEL_228:
          w_u16(((uint32)((v100 + 460))),(v102 + 1));

          (result = (r_u32(((uint32)((v1 + 8)))) - 34952));
          w_u32(((uint32)((v1 + 8))),result);
        }
      }
    }
    else
    {
      w_u32(((uint32)((a1 + 112))),0);
      w_u32(((uint32)((a1 + 108))),0);
      (v101 = r_u32(((uint32)((a1 + 8)))));
      w_u32(((uint32)((a1 + 104))),0);
      LABEL_225:
      w_u32(((uint32)((a1 + 8))),(v101 + 0x200000));

      sub_80063118(a1,0,1);
      (v105 = r_u16(((uint32)((v1 + 474)))));
      (v106 = r_u16(((uint32)(v1))));
      w_u16(((uint32)((v1 + 462))),0);
      (result = (v105 + 1));
      w_u16(((uint32)((v1 + 474))),result);
      w_u16(((uint32)(v1)),(v106 | 8));
    }
      return result;

    default:
      return result;

  }

}


uint32 sub_800539A8(uint32 a1)
{
  uint32 v1;
  uint32 v2;
  sint32 v3;
  sint32 v4;
  uint32 v5;
  uint32 v6;
  sint32 v7;
  uint32 v8;
  sint32 v9;
  sint32 v10;
  uint32 i;
  sint32 result;
  char v13[24];
  v1 = a1;
  v2 = sub_8006689C((a1+(1)*4u),(r_u32(0x800FF5A0u) + 4));
  v3 = 0;
  v4 = 0;
  v5 = v13;
  while ((v4 < 6))
  {
    v6 = r_u32((v1+(128)*4u));
    v7 = 0;
    if ((v6 < v2))
    {
      v8 = r_u32((v1+(130)*4u));
      if ((v2 >= v8))
      {
        v3 += r_u32((v1+(131)*4u));
        goto LABEL_8;
      }
      v7 = ((((sint32)(((r_u32((v1+(131)*4u)) - r_u32((v1+(129)*4u))) * (v2 - v6)))) / ((sint32)((v8 - v6)))) + r_u32((v1+(129)*4u)));
    }
    v3 += v7;
    LABEL_8:
    w_u32(((uint32)(v5)),v3);

    v5 += (4)*1u;
    v1 += (4)*4u;
    ++v4;
  }

  v9 = sub_80066570(v3);
  v10 = 0;
  for (i = v13;; i += (4)*1u)
  {
    result = v10;
    if ((v9 < r_u32(((uint32)(i)))))
      break;
    if ((++v10 >= 6))
      return 4;
  }

  return result;
}
uint32 sub_8005BCE4(uint32 a1, uint32 a2)
{
  sint32 result;
  w_u32(((uint32)((a1 + 68))),0x800A27D8u);
  sub_80062A64(a1,0x800FF4E8u);
  result = sub_800629BC(a1,0);
  if (((a2 & 1) != 0))
    return sub_80062608(a1);
  return result;
}
uint32 sub_80034B9C(uint32 a1, uint32 a2)
{
  sint32 result;
  uint32 v5;
  result = r_u32(((uint32)((a1 + 80))));
  v5 = 0;
  if (result)
  {
    result = 0;
    do
    {
      sub_80032D3C(((uint32)(((r_u32(((uint32)((a1 + 72)))) + result) + 4))),a2);
      ++v5;
      result = (8 * v5);
    }
    while ((v5 < r_u32(((uint32)((a1 + 80))))));
  }
  return result;
}
/* TODO Missing call adapter indirect */
uint32 sub_8001C51C(uint32 a1, uint32 a2)
{
  sint32 v4;
  sint32 result;
  v4 = r_u32(((uint32)((a1 + 76))));
  w_u32(((uint32)((a1 + 68))),0x800A109Cu);
  if (v4)
    ((void)((v4 + ((sint16)(r_u16(((uint32)((r_u32(((uint32)((v4 + 68)))) + 8)))))))),(void)(3),abort(),0u);
  sub_80033138(a1,0);
  result = (a2 & 1);
  if (((a2 & 1) != 0))
    return sub_80032E30(a1);
  return result;
}
uint32 sub_8002FB34(uint32 a1)
{
  uint32 v1;
  uint32 v2;
  uint32 v3;
  uint32 v4;
  char v6[8];
  v1 = sub_80067E9C(a1,1024,0x4000);
  v2 = ((((v1>>16)&65535u) & 0xFFF) * r_u16(0x800ECC7Cu));
  v3 = (((v1 & 0xFFF) * r_u16(0x800ECC7Cu)) >> 12);
  v4 = (v2 >> 12);
  if ((v3 >= 0x100))
    v3 = ((v3&0xFFFFFF00u)|(((-1)&0xFFu)<<0));
  if (((v2 >> 12) >= 0x100))
    v4 = ((v4&0xFFFFFF00u)|(((-1)&0xFFu)<<0));
  v6[0] = v3;
  v6[1] = v4;
  v6[3] = 0;
  v6[2] = 0;
  return sub_800981B4(v6);
}




