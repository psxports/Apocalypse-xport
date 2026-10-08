#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
#include <stdlib.h>
/* TODO Missing call adapter sub_80018E74 */
/* TODO Missing call adapter sub_800196D4 */
/* TODO Missing call adapter sub_80019DA0 */
/* TODO Missing call adapter sub_8001A2E8 */
/* TODO Missing call adapter sub_8003D41C */
/* TODO Missing call adapter sub_800408A0 */
/* TODO Missing call adapter sub_8004509C */
/* TODO Missing call adapter sub_80047A9C */
/* TODO Missing call adapter sub_80049F24 */
/* TODO Missing call adapter sub_80053AB4 */
/* TODO Missing call adapter sub_80055F14 */
/* TODO Missing call adapter sub_80057FAC */
/* TODO Missing call adapter sub_800590DC */
/* TODO Missing call adapter sub_8005A630 */
/* TODO Missing call adapter sub_8005B3E0 */
/* TODO Missing call adapter sub_80061BD4 */
/* TODO Missing call adapter sub_80071CEC */
/* TODO Missing call adapter sub_800737A0 */
/* TODO Missing call adapter sub_8007488C */
/* TODO Missing host buffer adapter xport_draft_host_sub_80020EF8_p2 */
/* TODO Missing host buffer adapter xport_draft_host_sub_8006613C_p1 */
uint32 sub_800641E8(uint32 a1)
{
  sint32 v1;
  short v3;
  sint32 v4;
  sint32 v5;
  uint32 v6;
  uint32 v7;
  sint32 v8;
  uint32 v9;
  sint32 v10;
  uint32 v11;
  uint32 v12;
  uint32 v13;
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
  uint32 v33;
  sint32 v34;
  unsigned short v35;
  sint32 v36;
  uint32 result;
  uint32 v38;
  sint32 v39;
  int v40[4];
  v3 = -1;
  v4 = 0;
  v5 = 0;
  v6 = 0;
  v7 = r_u32(((uint32)(((uint32)(((uint32)(4) * (uint32)(a1))) + (uint32)(r_u32(0x800FF624u))))));
  v8 = ((sint16)(r_u16(v7)));
  v9 = ((uint32)((v7+(1)*2u)));
  if ((v8 == 5))
  {
    v35 = r_u16(v9);
    v36 = xport_draft_host_sub_8006613C_p1(v40,a1);
    v1 = 1;
    v5 = 1;
    v6 = ((uint32)(xport_draft_host_sub_80020EF8_p2(v35,v40,((uint32)(4) * (uint32)((r_u16(((uint32)(((uint32)(v36) + (uint32)(2))))) == 0))),r_u16(((uint32)(((uint32)(v36) + (uint32)(4))))),((sint32)(0x800A71CCu)))));
    goto LABEL_62;
  }
  if ((((sint32)(v8)) >= 6))
  {
    if ((v8 != 7))
      goto LABEL_62;
    v5 = 1;
  }
  else
    if ((v8 != 1))
  {
    LABEL_62:
    result = 0;

    if (v6)
    {
      w_u16((v6+(107)*2u),a1);
      w_u16((v6+(85)*2u),v3);
      if (v4)
        w_u16((v6+(39)*2u),(r_u16((v6+(39)*2u))&(~2u)));
      if (v5)
        w_u16((v6+(39)*2u),(r_u16((v6+(39)*2u))|(4u)));
      if (!v1)
      {
        v38 = 0;
        if ((v6 == ((uint32)(0x800FF4E8u))))
          v38 = 0x800FF4E8u;
        if ((v6 == ((uint32)(0x800FF5DCu))))
          v38 = 0x800FF5DCu;
        if ((v6 == ((uint32)(0x800FF204u))))
          v38 = 0x800FF204u;
        sub_80062B7C(((sint32)(v6)),v38);
      }
      sub_80029CC4(((sint32)(v6)));
      return v6;
    }
    return result;
  }
  v10 = ((short)(r_u16(v9)));
  v11 = ((uint32)((v9+(1)*2u)));
  v3 = ((sint16)(r_u16(v11)));
  v12 = ((uint32)((v11+(((uint32)(((sint16)(r_u16((v11+(1)*2u))))) + (uint32)(2)))*2u)));
  v1 = ((unsigned char)(sub_80064180(2u,v12)));
  v4 = ((unsigned char)(sub_80064180(4u,v12)));
  v13 = ((uint32)((((uint32)(((uint32)(sub_800641B8(v12)) + (uint32)(3)))) & 0xFFFFFFFC)));
  switch (v10)
  {
    case 200:
      v14 = sub_800625AC(624);
      if (!v14)
      goto LABEL_76;
      v6 = ((uint32)(sub_8004E35C(v14,((sint32)(v13)),a1)));
      goto LABEL_62;

    case 201:
      v15 = sub_800625AC(572);
      if (!v15)
      goto LABEL_76;
      v6 = ((uint32)(((void)(v15),(void)(((sint32)(v13))),(void)(a1),abort(),0u)));
      goto LABEL_62;

    case 202:
      v16 = sub_800625AC(544);
      if (!v16)
      goto LABEL_76;
      v6 = ((uint32)(((void)(v16),(void)(((sint32)(v13))),(void)(a1),abort(),0u)));
      goto LABEL_62;

    case 203:
      v24 = sub_800625AC(520);
      if (!v24)
      goto LABEL_76;
      v6 = ((uint32)(sub_8004D604(v24,((sint32)(v13)),a1)));
      goto LABEL_62;

    case 204:
      v25 = sub_800625AC(328);
      if (!v25)
      goto LABEL_76;
      v6 = ((uint32)(((void)(v25),(void)(((sint32)(v13))),(void)(a1),abort(),0u)));
      goto LABEL_62;

    case 205:
      v17 = sub_800625AC(576);
      if (!v17)
      goto LABEL_76;
      v6 = ((uint32)(((void)(v17),(void)(((sint32)(v13))),(void)(a1),abort(),0u)));
      goto LABEL_62;

    case 206:
      v23 = sub_800625AC(556);
      if (!v23)
      goto LABEL_76;
      v6 = ((uint32)(((void)(v23),(void)(((sint32)(v13))),(void)(a1),abort(),0u)));
      goto LABEL_62;

    case 207:
      v19 = sub_800625AC(528);
      if (!v19)
      goto LABEL_76;
      v6 = ((uint32)(((void)(v19),(void)(v13),(void)(a1),abort(),0u)));
      goto LABEL_62;

    case 208:
      v20 = sub_800625AC(496);
      if (!v20)
      goto LABEL_76;
      v6 = ((uint32)(((void)(v20),(void)(v13),(void)(a1),(void)(1),abort(),0u)));
      goto LABEL_62;

    case 209:
      v18 = sub_800625AC(504);
      if (!v18)
      goto LABEL_76;
      v6 = ((uint32)(((void)(v18),(void)(((sint32)(v13))),(void)(a1),abort(),0u)));
      goto LABEL_62;

    case 210:
      v22 = sub_800625AC(496);
      if (!v22)
      goto LABEL_76;
      v6 = ((uint32)(((void)(v22),(void)(v13),(void)(a1),(void)(0),abort(),0u)));
      goto LABEL_62;

    case 212:
      v21 = sub_800625AC(548);
      if (!v21)
      goto LABEL_76;
      v6 = ((uint32)(((void)(v21),(void)(v13),(void)(a1),abort(),0u)));
      goto LABEL_62;

    case 220:
      v26 = sub_800625AC(576);
      if (!v26)
      goto LABEL_76;
      v6 = ((uint32)(((void)(v26),(void)(((sint32)(v13))),(void)(a1),abort(),0u)));
      goto LABEL_62;

    case 221:
      v28 = sub_800625AC(568);
      if (!v28)
      goto LABEL_76;
      v6 = ((uint32)(((void)(v28),(void)(((sint32)(v13))),(void)(a1),abort(),0u)));
      goto LABEL_62;

    case 222:
      v29 = sub_800625AC(684);
      if (!v29)
      goto LABEL_76;
      v6 = ((uint32)(((void)(v29),(void)(((sint32)(v13))),(void)(a1),abort(),0u)));
      goto LABEL_62;

    case 223:
      v27 = sub_800625AC(656);
      if (!v27)
      goto LABEL_76;
      v6 = ((uint32)(((void)(v27),(void)(((sint32)(v13))),(void)(a1),abort(),0u)));
      goto LABEL_62;

    case 400:
      w_u32(0x800FF3ACu,0);
      v6 = ((uint32)(sub_800625AC(348)));
      if (v6)
      v6 = ((uint32)(((void)(((sint32)(v6))),(void)(v13),(void)(a1),abort(),0u)));
      goto LABEL_50;

    case 401:
      w_u32(0x800FF3ACu,0);
      v6 = ((uint32)(sub_800625AC(644)));
      if (v6)
      v6 = ((uint32)(((void)(((sint32)(v6))),(void)(v13),(void)(a1),abort(),0u)));
      goto LABEL_50;

    case 402:
      v31 = ((uint32)(sub_800625AC(540)));
      if (!v31)
      goto LABEL_76;
      v6 = sub_8003B964(v31,((sint32)(v13)),a1);
      goto LABEL_62;

    case 403:
      v32 = sub_800625AC(312);
      if (!v32)
      goto LABEL_76;
      v6 = ((uint32)(((void)(v32),(void)(((sint32)(v13))),(void)(a1),abort(),0u)));
      goto LABEL_62;

    case 404:
      w_u32(0x800FF3ACu,0);
      v6 = ((uint32)(sub_800625AC(332)));
      if (v6)
      v6 = ((uint32)(((void)(((sint32)(v6))),(void)(v13),(void)(a1),(void)(1),abort(),0u)));
      goto LABEL_50;

    case 405:
      w_u32(0x800FF3ACu,0);
      v6 = ((uint32)(sub_800625AC(324)));
      if (v6)
      v6 = ((uint32)(((void)(((sint32)(v6))),(void)(((sint32)(v13))),(void)(a1),abort(),0u)));
      goto LABEL_50;

    case 406:
      v33 = ((uint32)(sub_800625AC(336)));
      if (!v33)
      goto LABEL_76;
      v6 = sub_800620B8(v33,((sint32)(v13)),a1);
      goto LABEL_62;

    case 407:
      v34 = sub_800625AC(336);
      if (!v34)
      goto LABEL_76;
      v6 = ((uint32)(sub_80061DC8(v34,((sint32)(v13)),a1)));
      goto LABEL_62;

    case 408:
      w_u32(0x800FF3ACu,0);
      v6 = ((uint32)(sub_800625AC(332)));
      if (v6)
      v6 = ((uint32)(((void)(((sint32)(v6))),(void)(v13),(void)(a1),(void)(0),abort(),0u)));
      LABEL_50:
    w_u32(0x800FF3ACu,1);

      goto LABEL_62;

    case 409:
      v30 = sub_800625AC(504);
      if (v30)
    {
      v6 = ((uint32)(((void)(v30),(void)(((sint32)(v13))),(void)(a1),abort(),0u)));
      goto LABEL_62;
    }
      LABEL_76:
    result = 0;

      break;

    default:
      goto LABEL_62;

  }

  return result;
}



uint32 sub_8003B964(uint32 a1, uint32 a2, uint32 a3)
{
  sint32 v6;
  short v7;
  sint32 v8;
  uint32 result;
  sub_8004B800(a1);
  v6 = r_u32(0x800FF62Cu);
  w_u32((((uint32)(a1))+(17)*4u),0x800A1DF0u);
  sub_800626C8(a1,v6);
  sub_80062A38(a1,0x800FF5DCu);
  v7 = ((sint16)(r_u16(a1)));
  w_u16((a1+(29)*2u),402);
  w_u16(a1,((v7 & 0xFEEC) | 0x111));
  v8 = sub_80062B0C(a1,a2);
  w_u32((((uint32)(a1))+(100)*4u),sub_80062B50(a1,v8));
  result = a1;
  w_u32((((uint32)(a1))+(131)*4u),-1);
  w_u16((a1+(218)*2u),32);
  w_u16((a1+(107)*2u),a3);
  w_u8((((uint32)(a1))+(380)*1u),1);
  return result;
}


/* TODO Postincrement memory expressions may require ordering refinement */
uint32 sub_8004E48C(uint32 a1)
{
  sint32 v2;
  sint32 result;
  (w_u32(0x800FF308u,(r_u32(0x800FF308u)+1u)),r_u32(0x800FF308u));
  w_u8(((uint32)(((uint32)(a1) + (uint32)(129)))),31);
  w_u8(((uint32)(((uint32)(a1) + (uint32)(131)))),31);
  w_u8(((uint32)(((uint32)(a1) + (uint32)(130)))),3);
  sub_80062A38(a1,0x800FF4E8u);
  v2 = r_u32(0x800FF4DCu);
  result = 200;
  w_u16(((uint32)(((uint32)(a1) + (uint32)(58)))),200);
  w_u16(((uint32)(((uint32)(a1) + (uint32)(472)))),0);
  w_u16(((uint32)(((uint32)(a1) + (uint32)(474)))),0);
  w_u32(0x800FF4DCu,((uint32)(v2) + (uint32)(1)));
  w_u8(((uint32)(((uint32)(a1) + (uint32)(381)))),0);
  return result;
}



uint32 sub_8004BD04(uint32 a1, uint32 a2, uint32 a3)
{
  sint32 v7;
  sint32 result;
  v7 = r_u32(((uint32)(((uint32)(a1) + (uint32)(448)))));
  if (v7)
    ((void)(a2),sub_8006BC20(v7));
  result = sub_8006B864(((uint32)(((uint32)(2) * (uint32)(a2))) * (uint32)(a3)),0,1);
  w_u32(((uint32)(((uint32)(a1) + (uint32)(448)))),result);
  w_u8(((uint32)(((uint32)(a1) + (uint32)(452)))),a2);
  w_u8(((uint32)(((uint32)(a1) + (uint32)(453)))),a3);
  return result;
}


/* TODO Missing call adapter sub_800878DC */
uint32 sub_8007BF2C(uint32 a1, uint32 a2)
{
  uint32 v2;
  sint32 result;
  sint32 v5;
  v2 = a1;
  if (a1)
  {
    result = r_u32(((uint32)(((uint32)(a2) + (uint32)(68)))));
    if (result)
    {
      ((void)(((uint32)(a2) + (uint32)(72))),abort(),0u);
      sub_80084C50(v2,0,a2,r_u16(((uint32)(((uint32)(a2) + (uint32)(138))))));
      for (result = r_u32(v2); r_u32(v2); result = r_u32(v2))
      {
        v5 = r_u16(((uint32)(((uint32)(a2) + (uint32)(138)))));
        if ((r_u16(((uint32)(((uint32)(r_u32(v2)) + (uint32)(2))))) != v5))
        {
          w_u16(((uint32)(((uint32)(r_u32(v2)) + (uint32)(2)))),v5);
          sub_8007C06C(r_u32(v2),a2);
        }
        (v2+=4u);
      }

    }
  }
  return result;
}



void sub_80084C50(uint32 list, uint32 unused, uint32 bounds, uint32 mark)
{
  sint32 low[3], high[3], first[3], second[3];
  uint32 object = r_u32(list), flip_bits, table, geometry;
  (void)unused;
  if (!object) return;
  sub_80084778(bounds,low,high,&flip_bits);
  table = 0x800EAEF8u + 64u*r_u8(object + 27u);
  table = r_u32(table + 16u);
  do
  {
    uint32 flags = r_u32(object);
    if (flags & 33u) w_u16(object + 2u,mark);
    else if ((flags >> 16) != mark)
    {
      geometry = r_u32(table + 4u*r_u16(object + 22u));
      /* TODO Logical adapter carries geometry bounds and translated object position */
      xport_draft_call_80084814(object,geometry,low,high,first,second);
      if (!xport_draft_call_800848D0(object,geometry,first,second))
        w_u16(object + 2u,mark);
    }
    list += 4u;
    object = r_u32(list);
  } while (object);
}


/* TODO Missing call adapter xport_draft_missing_gte_adapter */
void sub_800847AC(sint32 low[3], sint32 high[3], uint32 *flip_bits)
{
  uint32 axis;
  for (axis = 0; axis < 3u; ++axis)
  {
    uint32 delta = (uint32)high[axis] - (uint32)low[axis];
    if ((sint32)delta < 0)
    {
      sint32 temporary = low[axis];
      low[axis] = high[axis];
      high[axis] = temporary;
      delta = (uint32)high[axis] - (uint32)low[axis];
      *flip_bits ^= 1u << axis;
    }
    xport_draft_gte_control_write(axis * 2u, delta);
  }
}


/* TODO Missing call adapter xport_draft_missing_gte_adapter */
uint32 sub_800848D0(const sint32 query_low[3], const sint32 query_high[3], const sint32 object_low[3], const sint32 object_high[3])
{
  sint32 low[3], high[3], cross[3];
  uint32 axis;
  for (axis=0; axis<3; ++axis)
  {
    if ((sint32)((uint32)query_high[axis]-(uint32)object_low[axis])<0) return 0;
    high[axis]=(sint32)((uint32)object_high[axis]-(uint32)query_low[axis]);
    if (high[axis]<0) return 0;
    low[axis]=(sint32)((uint32)object_low[axis]-(uint32)query_low[axis]);
  }
  for (axis=0; axis<3; ++axis) xport_draft_gte_data_write(9+axis,(uint32)high[axis]);
  xport_draft_gte_execute(0x170000Cu);
  for (axis=0; axis<3; ++axis) cross[axis]=(sint32)xport_draft_gte_data_read(25+axis);
  if (cross[0]<0)
  {
    if (cross[1]<0) goto CheckYZ;
    xport_draft_gte_data_write(9,(uint32)low[0]);
    xport_draft_gte_data_write(10,(uint32)low[1]);
    xport_draft_gte_data_write(11,(uint32)high[2]);
    xport_draft_gte_execute(0x170000Cu);
    if ((sint32)xport_draft_gte_data_read(9)<0) return 0;
    if ((sint32)xport_draft_gte_data_read(10)>0) return 0;
    return 1;
  }
  if (cross[2]<0)
  {
    xport_draft_gte_data_write(9,(uint32)low[0]);
    xport_draft_gte_data_write(10,(uint32)high[1]);
    xport_draft_gte_data_write(11,(uint32)low[2]);
    xport_draft_gte_execute(0x170000Cu);
    if ((sint32)xport_draft_gte_data_read(9)>0) return 0;
    if ((sint32)xport_draft_gte_data_read(11)<0) return 0;
    return 1;
  }
CheckYZ:
  xport_draft_gte_data_write(9,(uint32)high[0]);
  xport_draft_gte_data_write(10,(uint32)low[1]);
  xport_draft_gte_data_write(11,(uint32)low[2]);
  xport_draft_gte_execute(0x170000Cu);
  if ((sint32)xport_draft_gte_data_read(10)<0) return 0;
  if ((sint32)xport_draft_gte_data_read(11)>0) return 0;
  return 1;
}


uint32 sub_8004D2B4(uint32 a1, uint32 a2)
{
  uint32 result;
  sint32 v4;
  sint32 v5;
  sint32 v6;
  uint32 v7;
  sint32 v8;
  uint32 v9;
  unsigned char v10;
  uint32 v11;
  unsigned short v12;
  uint32 v13;
  sint32 v14;
  sint32 v15;
  sint32 v16;
  switch (a2)
  {
    case 8448:
      result = ((sint16)(r_u16(((uint32)(((uint32)(a1) + (uint32)(218)))))));
      break;

    case 8480:
      v9 = r_u32(((uint32)(((uint32)(a1) + (uint32)(400)))));
      v10 = r_u8(v9);
      w_u32(((uint32)(((uint32)(a1) + (uint32)(400)))),(v9+(2)*1u));
      result = ((sint16)(r_u16(((uint32)(((uint32)(((uint32)(a1) + (uint32)(((uint32)(2) * (uint32)(v10))))) + (uint32)(460)))))));
      break;

    case 8489:
      v7 = r_u32(((uint32)(((uint32)(a1) + (uint32)(400)))));
      v8 = r_u16(v7);
      w_u32(((uint32)(((uint32)(a1) + (uint32)(400)))),(v7+(1)*2u));
      result = sub_80066570(v8);
      break;

    case 8490:
      result = r_u16(((uint32)(((uint32)(a1) + (uint32)(214)))));
      break;

    case 8491:
      v11 = r_u32(((uint32)(((uint32)(a1) + (uint32)(400)))));
      v12 = r_u16(v11);
      w_u32(((uint32)(((uint32)(a1) + (uint32)(400)))),(v11+(1)*2u));
      if (((v12 & 0x2000) != 0))
      v12 = ((void)(((sint32)(((uint32)(a1) + (uint32)(((sint16)(r_u16(((uint32)(((uint32)(r_u32(((uint32)(((uint32)(a1) + (uint32)(68)))))) + (uint32)(80)))))))))))),(void)(v12),abort(),0u);
      v13 = ((uint32)(sub_80066088(v12)));
      result = 0;
      if (r_u16(v13))
      result = ((unsigned short)(r_u16((v13+(1)*2u))));
      break;

    case 8492:
      result = r_u8(((uint32)(((uint32)(a1) + (uint32)(384)))));
      break;

    case 8493:
      result = r_u8(((uint32)(((uint32)(a1) + (uint32)(383)))));
      break;

    case 8494:
      result = r_u16(((uint32)(((uint32)(a1) + (uint32)(392)))));
      break;

    case 8498:
      v4 = ((uint32)(a1) + (uint32)(4));
      if (r_u32(0x800FF5A0u))
    {
      result = sub_80066918(v4,((uint32)(r_u32(0x800FF5A0u)) + (uint32)(4)));
      if ((result >= 0x2000))
        result = 0x1FFF;
    }
    else
    {
      result = 0x1FFF;
    }
      break;

    case 8499:
      if (!r_u32(0x800FF5A0u))
      goto LABEL_29;
      v5 = r_u32(((uint32)(((uint32)(a1) + (uint32)(8)))));
      v6 = r_u32(((uint32)(((uint32)(a1) + (uint32)(12)))));
      v14 = r_u32(((uint32)(((uint32)(a1) + (uint32)(4)))));
      v15 = v5;
      v16 = v6;
      v15 = ((uint32)(v5) - (uint32)(((uint32)(((sint16)(r_u16(((uint32)(((uint32)(a1) + (uint32)(458)))))))) << (uint32)(12))));
      result = xport_draft_host_sub_800679A4_p1(&v14,((uint32)(r_u32(0x800FF5A0u)) + (uint32)(4)));
      break;

    case 8502:
      result = r_u32(0x800FF274u);
      break;

    case 8512:
      result = (((sint32)(((sint32)(r_u32(((uint32)(((uint32)(a1) + (uint32)(4))))))))) >> 12);
      break;

    case 8513:
      result = (((sint32)(((sint32)(r_u32(((uint32)(((uint32)(a1) + (uint32)(8))))))))) >> 12);
      break;

    case 8514:
      result = (((sint32)(((sint32)(r_u32(((uint32)(((uint32)(a1) + (uint32)(12))))))))) >> 12);
      break;

    case 8528:
      if (!r_u32(0x800FF5A0u))
      goto LABEL_29;
      result = (((sint32)(((sint32)(r_u32(((uint32)(((uint32)(r_u32(0x800FF5A0u)) + (uint32)(4))))))))) >> 12);
      break;

    case 8529:
      if (!r_u32(0x800FF5A0u))
      goto LABEL_29;
      result = (((sint32)(((sint32)(r_u32(((uint32)(((uint32)(r_u32(0x800FF5A0u)) + (uint32)(8))))))))) >> 12);
      break;

    case 8530:
      if (!r_u32(0x800FF5A0u))
      goto LABEL_29;
      result = (((sint32)(((sint32)(r_u32(((uint32)(((uint32)(r_u32(0x800FF5A0u)) + (uint32)(12))))))))) >> 12);
      break;

    default:
      LABEL_29:
    result = 0;

      break;

  }

  return result;
}



uint32 sub_80053784(uint32 a1, uint32 a2)
{
  sint32 v4;
  sint32 v5;
  uint32 result;
  if ((r_u8(((uint32)(((uint32)(a1) + (uint32)(26))))) != 2))
    sub_80063118(a1,2,1);
  if ((((r_u16(((uint32)(((uint32)(a1) + (uint32)(390))))) == 128) && ((r_u32(((uint32)(((uint32)(a1) + (uint32)(372))))) & 9) != 0)) && (r_u8(((uint32)(((uint32)(a1) + (uint32)(26))))) != 1)))
    sub_80063118(a1,1,1);
  v4 = r_u32((a2+(1)*4u));
  v5 = r_u32((a2+(2)*4u));
  w_u32(((uint32)(((uint32)(a1) + (uint32)(484)))),r_u32(a2));
  w_u32(((uint32)(((uint32)(a1) + (uint32)(488)))),v4);
  w_u32(((uint32)(((uint32)(a1) + (uint32)(492)))),v5);
  result = ((r_u32(((uint32)(((uint32)(a1) + (uint32)(396))))) & 0xFFFFFFC6) | 1);
  w_u32(((uint32)(((uint32)(a1) + (uint32)(396)))),result);
  return result;
}



uint32 sub_8004BE30(uint32 a1)
{
  sint32 v1;
  uint32 v2;
  sint32 v3;
  sint32 result;
  v1 = 0;
  do
  {
    v2 = r_u32(((uint32)(((uint32)(a1) + (uint32)(400)))));
    v3 = r_u16(v2);
    w_u32(((uint32)(((uint32)(a1) + (uint32)(400)))),(v2+(1)*2u));
    result = ((uint32)(v3) - (uint32)(16658));
    if ((((unsigned short)(v3)) == 16672))
    {
      if ((v1-- == 0))
        return result;
      result = ((unsigned short)(v3));
    }
    else
    {
      result = ((unsigned short)(v3));
      if ((((unsigned short)(((uint32)(v3) - (uint32)(16658)))) < 5u))
        ++v1;
    }
  }
  while ((result != 16640));
  return result;
}


/* TODO Missing call adapter sub_80063A38 */
/* TODO Missing host buffer adapter xport_draft_host_sub_8008847C_p1 */
uint32 sub_80030AD4(void)
{
  sint32 v0;
  sint32 v1;
  sint32 v2;
  sint32 v3;
  sint32 v5;
  sint32 v6;
  sub_8005C580();
  w_u32(0x800FF64Cu,0);
  w_u32(0x800FF2F0u,0);
  w_u32(0x800FF2F4u,0);
  w_u8(0x800FF8F1u,0);
  sub_8007001C();
  w_u32(0x800FF7BCu,0);
  w_u32(0x800FF7C0u,0);
  w_u32(0x800FF38Cu,0);
  w_u32(0x800FF390u,0);
  w_u32(0x800FF2ECu,0);
  sub_8006654C(312921176);
  sub_800664E4(1);
  while (!r_u32(0x800FF2ECu))
  {
    nullsub_16();
    v0 = r_u32(0x800FF64Cu);
    sub_80068160();
    sub_8002FF54();
    sub_8002FF90();
    if (r_u32(0x800FF2ECu))
      break;
    sub_8002FF90();
    if (r_u32(0x800FF2ECu))
      break;
    sub_800307F0();
    sub_8002F284();
    if ((r_u32(0x800FF64Cu) == v0))
      sub_800664E4(1);
    w_u32(0x800FF65Cu,0);
    while (1)
    {
      sub_800664E4(1);
      if (!DrawSync(1))
        break;
      sub_800662A0();
    }

    sub_800681B8();
    if (!r_u32(0x800FF65Cu))
    {
      sub_800662A0();
      w_u32(0x800FF65Cu,1);
    }
    v1 = ((uint32)(((uint32)(r_u32(0x800FF64Cu)) - (uint32)(2))) - (uint32)(v0));
    if ((((!r_u32(0x800FF304u) && (r_u32(0x800FF818u) != 1)) && (((sint32)(v1)) > 0)) && !r_u8(0x800FF8F1u)))
    {
      sub_8002FF90();
      if (r_u32(0x800FF2ECu))
        break;
      if ((((sint32)(v1)) >= 2))
      {
        sub_8002FF90();
        if (r_u32(0x800FF2ECu))
          break;
      }
    }
    if (sub_80062F48(r_u32(0x800FF5A0u)))
      w_u32(0x800FF2ECu,2);
  }

  sub_8006F7D8();
  w_u16(0x800FFAACu,1);
  sub_8002E2B8();
  if (r_u32(0x800FF304u))
    (abort(),0u);
  v5 = r_u32(0x800FF368u);
  v6 = r_u32(0x800FF36Cu);
  xport_draft_host_sub_8008847C_p1(&v5,0,0,0);
  v5 = ((v5 & 0x0000FFFFu)|(((511) & 0xFFFFu)<<16));
  v6 = ((v6 & 0x0000FFFFu)|(((1) & 0xFFFFu)<<16));
  return xport_draft_host_sub_8008847C_p1(&v5,0,0,0);
}


/* TODO Postincrement memory expressions may require ordering refinement */
uint32 sub_8001BF34(void)
{
  sint32 result;
  sint32 v1;
  if (r_u32(0x800FF1E8u))
  {
    result = ((uint32)(r_u32(0x800FF1ECu)) - (uint32)(1));
    if (r_u32(0x800FF1ECu))
    {
      (w_u32(0x800FF1ECu,(r_u32(0x800FF1ECu)-1u)),r_u32(0x800FF1ECu));
      v1 = (result != 0);
      result = 16711680;
      if (v1)
      {
        w_u32(0x800FFB60u,(r_u32(0x800FFB60u)+(r_u32(0x800FFB6Cu))));
        w_u32(0x800FFB68u,(r_u32(0x800FFB68u)+(r_u32(0x800FFB6Cu))));
        result = ((uint32)(r_u32(0x800FFB64u)) + (uint32)(r_u32(0x800FFB6Cu)));
        w_u32(0x800FFB64u,(r_u32(0x800FFB64u)+(r_u32(0x800FFB6Cu))));
      }
      else
      {
        w_u32(0x800FFB68u,16711680);
        w_u32(0x800FFB64u,16711680);
        w_u32(0x800FFB60u,16711680);
      }
    }
  }
  else
  {
    result = ((uint32)(r_u32(0x800FF1E4u)) - (uint32)(1));
    if (r_u32(0x800FF1E4u))
    {
      (w_u32(0x800FF1E4u,(r_u32(0x800FF1E4u)-1u)),r_u32(0x800FF1E4u));
      if (result)
      {
        w_u32(0x800FFB60u,(r_u32(0x800FFB60u)-(r_u32(0x800FFB6Cu))));
        result = ((uint32)(r_u32(0x800FFB64u)) - (uint32)(r_u32(0x800FFB70u)));
        w_u32(0x800FFB64u,(r_u32(0x800FFB64u)-(r_u32(0x800FFB70u))));
        w_u32(0x800FFB68u,(r_u32(0x800FFB68u)-(r_u32(0x800FFB74u))));
      }
      else
      {
        w_u8(0x800FF1DCu,0);
      }
    }
  }
  return result;
}


/* TODO Missing call adapter indirect */
/* TODO Missing host buffer adapter xport_draft_host_sub_8006C3AC_p1 */
/* TODO Missing host buffer adapter xport_draft_host_sub_8006C4EC_p123 */
/* TODO Missing host buffer adapter xport_draft_host_sub_8006C808_p2 */
/* TODO Missing host buffer adapter xport_draft_host_sub_8006CB28_p1 */
uint32 sub_8006338C(uint32 a1)
{
  sint32 v2;
  sint8 v3;
  sint8 v4;
  sint32 result;
  uint32 v6;
  unsigned char v7;
  sint32 v8;
  sint32 v9;
  sint32 v10;
  sint32 v11;
  sint32 v12;
  sint8 v13;
  sint8 v14;
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
  char v25[16];
  sint32 v26;
  sint32 v27;
  if (((r_u16(((uint32)(a1))) & 2) != 0))
  {
    sub_80063164(a1);
    ((void)(((sint32)(((uint32)(a1) + (uint32)(((sint16)(r_u16(((uint32)(((uint32)(r_u32(((uint32)(((uint32)(a1) + (uint32)(68)))))) + (uint32)(32)))))))))))),abort(),0u);
    if ((r_u8(((uint32)(((uint32)(a1) + (uint32)(225))))) < 2u))
    {
      v2 = r_u32(((uint32)(((uint32)(a1) + (uint32)(68)))));
      w_u16(((uint32)(((uint32)(a1) + (uint32)(310)))),((sint8)(r_u8(((uint32)(((uint32)(a1) + (uint32)(24))))))));
      ((void)(((sint32)(((uint32)(a1) + (uint32)(((sint16)(r_u16(((uint32)(((uint32)(v2) + (uint32)(24)))))))))))),abort(),0u);
      v3 = r_u8(((uint32)(((uint32)(a1) + (uint32)(26)))));
      v4 = r_u8(((uint32)(((uint32)(a1) + (uint32)(297)))));
      result = ((sint8)(r_u8(((uint32)(((uint32)(a1) + (uint32)(24)))))));
      w_u16(((uint32)(((uint32)(a1) + (uint32)(308)))),result);
      w_u8(((uint32)(((uint32)(a1) + (uint32)(312)))),v3);
      w_u8(((uint32)(((uint32)(a1) + (uint32)(313)))),v4);
      return result;
    }
  }
  else
  {
    ((void)(((sint32)(((uint32)(a1) + (uint32)(((sint16)(r_u16(((uint32)(((uint32)(r_u32(((uint32)(((uint32)(a1) + (uint32)(68)))))) + (uint32)(32)))))))))))),abort(),0u);
    if ((r_u8(((uint32)(((uint32)(a1) + (uint32)(225))))) < 2u))
      return ((void)(((sint32)(((uint32)(a1) + (uint32)(((sint16)(r_u16(((uint32)(((uint32)(r_u32(((uint32)(((uint32)(a1) + (uint32)(68)))))) + (uint32)(24)))))))))))),abort(),0u);
  }
  v6 = r_u8(((uint32)(((uint32)(a1) + (uint32)(225)))));
  v7 = ((uint32)(r_u8(((uint32)(((uint32)(a1) + (uint32)(226)))))) + (uint32)(1));
  w_u8(((uint32)(((uint32)(a1) + (uint32)(226)))),v7);
  if ((v7 < v6))
  {
    sub_8006C0B8(((uint32)(a1) + (uint32)(4)),((uint32)(a1) + (uint32)(264)));
    sub_8006C730(((uint32)(a1) + (uint32)(16)),((uint32)(a1) + (uint32)(288)));
    return sub_8006C624(((uint32)(a1) + (uint32)(16)));
  }
  else
  {
    w_u8(((uint32)(((uint32)(a1) + (uint32)(226)))),0);
    v8 = r_u32(((uint32)(((uint32)(a1) + (uint32)(256)))));
    v9 = r_u32(((uint32)(((uint32)(a1) + (uint32)(260)))));
    w_u32(((uint32)(((uint32)(a1) + (uint32)(4)))),r_u32(((uint32)(((uint32)(a1) + (uint32)(252))))));
    w_u32(((uint32)(((uint32)(a1) + (uint32)(8)))),v8);
    w_u32(((uint32)(((uint32)(a1) + (uint32)(12)))),v9);
    v10 = r_u32(((uint32)(((uint32)(a1) + (uint32)(8)))));
    v11 = r_u32(((uint32)(((uint32)(a1) + (uint32)(12)))));
    w_u32(((uint32)(((uint32)(a1) + (uint32)(228)))),r_u32(((uint32)(((uint32)(a1) + (uint32)(4))))));
    w_u32(((uint32)(((uint32)(a1) + (uint32)(232)))),v10);
    w_u32(((uint32)(((uint32)(a1) + (uint32)(236)))),v11);
    v10 = ((v10 & 0xFFFF0000u)|(((r_u16(((uint32)(((uint32)(a1) + (uint32)(286)))))) & 0xFFFFu)<<0));
    w_u32(((uint32)(((uint32)(a1) + (uint32)(16)))),r_u32(((uint32)(((uint32)(a1) + (uint32)(282))))));
    w_u16(((uint32)(((uint32)(a1) + (uint32)(20)))),v10);
    v10 = ((v10 & 0xFFFF0000u)|(((r_u16(((uint32)(((uint32)(a1) + (uint32)(20)))))) & 0xFFFFu)<<0));
    w_u32(((uint32)(((uint32)(a1) + (uint32)(276)))),r_u32(((uint32)(((uint32)(a1) + (uint32)(16))))));
    w_u16(((uint32)(((uint32)(a1) + (uint32)(280)))),v10);
    if (((r_u16(((uint32)(a1))) & 2) != 0))
    {
      v12 = r_u32(((uint32)(((uint32)(a1) + (uint32)(68)))));
      w_u16(((uint32)(((uint32)(a1) + (uint32)(310)))),((sint8)(r_u8(((uint32)(((uint32)(a1) + (uint32)(24))))))));
      ((void)(((sint32)(((uint32)(a1) + (uint32)(((sint16)(r_u16(((uint32)(((uint32)(v12) + (uint32)(24)))))))))))),abort(),0u);
      v13 = r_u8(((uint32)(((uint32)(a1) + (uint32)(26)))));
      v14 = r_u8(((uint32)(((uint32)(a1) + (uint32)(297)))));
      w_u16(((uint32)(((uint32)(a1) + (uint32)(308)))),((sint8)(r_u8(((uint32)(((uint32)(a1) + (uint32)(24))))))));
      w_u8(((uint32)(((uint32)(a1) + (uint32)(312)))),v13);
      w_u8(((uint32)(((uint32)(a1) + (uint32)(313)))),v14);
    }
    else
    {
      ((void)(((sint32)(((uint32)(a1) + (uint32)(((sint16)(r_u16(((uint32)(((uint32)(r_u32(((uint32)(((uint32)(a1) + (uint32)(68)))))) + (uint32)(24)))))))))))),abort(),0u);
    }
    v15 = r_u32(((uint32)(((uint32)(a1) + (uint32)(8)))));
    v16 = r_u32(((uint32)(((uint32)(a1) + (uint32)(12)))));
    w_u32(((uint32)(((uint32)(a1) + (uint32)(252)))),r_u32(((uint32)(((uint32)(a1) + (uint32)(4))))));
    w_u32(((uint32)(((uint32)(a1) + (uint32)(256)))),v15);
    w_u32(((uint32)(((uint32)(a1) + (uint32)(260)))),v16);
    v17 = r_u32(((uint32)(((uint32)(a1) + (uint32)(232)))));
    v18 = r_u32(((uint32)(((uint32)(a1) + (uint32)(236)))));
    w_u32(((uint32)(((uint32)(a1) + (uint32)(4)))),r_u32(((uint32)(((uint32)(a1) + (uint32)(228))))));
    w_u32(((uint32)(((uint32)(a1) + (uint32)(8)))),v17);
    w_u32(((uint32)(((uint32)(a1) + (uint32)(12)))),v18);
    xport_draft_host_sub_8006C3AC_p1(v25,((uint32)(a1) + (uint32)(252)),((uint32)(a1) + (uint32)(228)));
    v26 = r_u8(((uint32)(((uint32)(a1) + (uint32)(225)))));
    xport_draft_host_sub_8006C4EC_p123(&v22,v25,&v26);
    v19 = v23;
    v20 = v24;
    w_u32(((uint32)(((uint32)(a1) + (uint32)(264)))),v22);
    w_u32(((uint32)(((uint32)(a1) + (uint32)(268)))),v19);
    w_u32(((uint32)(((uint32)(a1) + (uint32)(272)))),v20);
    v19 = ((v19 & 0xFFFF0000u)|(((r_u16(((uint32)(((uint32)(a1) + (uint32)(20)))))) & 0xFFFFu)<<0));
    w_u32(((uint32)(((uint32)(a1) + (uint32)(282)))),r_u32(((uint32)(((uint32)(a1) + (uint32)(16))))));
    w_u16(((uint32)(((uint32)(a1) + (uint32)(286)))),v19);
    v19 = ((v19 & 0xFFFF0000u)|(((r_u16(((uint32)(((uint32)(a1) + (uint32)(280)))))) & 0xFFFFu)<<0));
    w_u32(((uint32)(((uint32)(a1) + (uint32)(16)))),r_u32(((uint32)(((uint32)(a1) + (uint32)(276))))));
    w_u16(((uint32)(((uint32)(a1) + (uint32)(20)))),v19);
    xport_draft_host_sub_8006CB28_p1(&v22,((uint32)(a1) + (uint32)(282)),((uint32)(a1) + (uint32)(276)));
    v21 = v23;
    w_u32(((uint32)(((uint32)(a1) + (uint32)(288)))),v22);
    w_u16(((uint32)(((uint32)(a1) + (uint32)(292)))),v21);
    if ((((sint32)(((sint16)(r_u16(((uint32)(((uint32)(a1) + (uint32)(288))))))))) < -2048))
      w_u16(((uint32)(((uint32)(a1) + (uint32)(288)))),(r_u16(((uint32)(((uint32)(a1) + (uint32)(288)))))+(4096)));
    if ((((sint32)(((sint16)(r_u16(((uint32)(((uint32)(a1) + (uint32)(288))))))))) >= 2049))
      w_u16(((uint32)(((uint32)(a1) + (uint32)(288)))),(r_u16(((uint32)(((uint32)(a1) + (uint32)(288)))))-(4096)));
    if ((((sint32)(((sint16)(r_u16(((uint32)(((uint32)(a1) + (uint32)(290))))))))) < -2048))
      w_u16(((uint32)(((uint32)(a1) + (uint32)(290)))),(r_u16(((uint32)(((uint32)(a1) + (uint32)(290)))))+(4096)));
    if ((((sint32)(((sint16)(r_u16(((uint32)(((uint32)(a1) + (uint32)(290))))))))) >= 2049))
      w_u16(((uint32)(((uint32)(a1) + (uint32)(290)))),(r_u16(((uint32)(((uint32)(a1) + (uint32)(290)))))-(4096)));
    if ((((sint32)(((sint16)(r_u16(((uint32)(((uint32)(a1) + (uint32)(292))))))))) < -2048))
      w_u16(((uint32)(((uint32)(a1) + (uint32)(292)))),(r_u16(((uint32)(((uint32)(a1) + (uint32)(292)))))+(4096)));
    if ((((sint32)(((sint16)(r_u16(((uint32)(((uint32)(a1) + (uint32)(292))))))))) >= 2049))
      w_u16(((uint32)(((uint32)(a1) + (uint32)(292)))),(r_u16(((uint32)(((uint32)(a1) + (uint32)(292)))))-(4096)));
    v27 = r_u8(((uint32)(((uint32)(a1) + (uint32)(225)))));
    return xport_draft_host_sub_8006C808_p2(((uint32)(a1) + (uint32)(288)),&v27);
  }
}



uint32 sub_8006C8E8(uint32 a1, uint32 a2)
{
  uint32 result;
  short v3;
  sint32 v4;
  short v5;
  sint32 v6;
  result = a1;
  v3 = ((uint32)(((sint16)(r_u16(a1)))) - (uint32)((((sint32)(((sint16)(r_u16(a1))))) >> r_u8(a2))));
  v4 = ((sint16)(r_u16((a1+(1)*2u))));
  w_u16(result,v3);
  v5 = ((uint32)(r_u16((result+(1)*2u))) - (uint32)((((sint32)(v4)) >> r_u8((a2+(1)*1u)))));
  v6 = ((short)(r_u16((result+(2)*2u))));
  w_u16((result+(1)*2u),v5);
  w_u16((result+(2)*2u),(r_u16((result+(2)*2u))-((((sint32)(v6)) >> r_u8((a2+(2)*1u))))));
  return result;
}


/* TODO Missing call adapter HIDWORD */
/* TODO Missing call adapter LODWORD */
/* TODO Missing call adapter SBYTE4 */
uint32 sub_8005CB1C(uint32 a1)
{
  sint32 v2;
  sint32 result;
  sint32 v4;
  sint32 v5;
  uint32 v6;
  uint32 v7;
  sint8 v8;
  sint8 v9;
  uint32 v10;
  sint8 v11;
  uint32 v12;
  sint8 v13;
  unsigned char v14;
  sint32 v15;
  sint32 v16;
  long long v17;
  sint32 v18;
  long long v19;
  v2 = r_u32(((uint32)(((uint32)(a1) + (uint32)(452)))));
  result = 1;
  w_u8(((uint32)(((uint32)(a1) + (uint32)(468)))),0);
  w_u8(((uint32)(((uint32)(a1) + (uint32)(469)))),0);
  w_u8(((uint32)(((uint32)(a1) + (uint32)(471)))),0);
  w_u8(((uint32)(((uint32)(a1) + (uint32)(472)))),0);
  w_u8(((uint32)(((uint32)(a1) + (uint32)(470)))),1);
  if (v2)
  {
    w_u8(((uint32)(((uint32)(a1) + (uint32)(473)))),0);
  }
  else
  {
    v4 = r_u32(((uint32)(((uint32)(a1) + (uint32)(444)))));
    v5 = r_u32(((uint32)(((uint32)(v4) + (uint32)(364)))));
    if (((v5 == 115) || (v5 == 83)))
    {
      v6 = r_u32(((uint32)(((uint32)(a1) + (uint32)(444)))));
      w_u8(((uint32)(((uint32)(a1) + (uint32)(468)))),r_u8(((uint32)(((uint32)(v4) + (uint32)(360))))));
      w_u8(((uint32)(((uint32)(a1) + (uint32)(469)))),r_u8((v6+(361)*1u)));
      w_u8(((uint32)(((uint32)(a1) + (uint32)(471)))),r_u8((v6+(362)*1u)));
      w_u8(((uint32)(((uint32)(a1) + (uint32)(472)))),r_u8((v6+(363)*1u)));
    }
    if (!((r_u8(((uint32)(((uint32)(a1) + (uint32)(468))))) | r_u8(((uint32)(((uint32)(a1) + (uint32)(469))))))))
    {
      v7 = r_u32(((uint32)(((uint32)(a1) + (uint32)(444)))));
      v8 = 0x80;
      if (r_u8(v7 + 160u) || (v8 = 127, r_u8(v7 + 176u)))
      {
        w_u8(((uint32)(((uint32)(a1) + (uint32)(468)))),v8);
        v7 = r_u32(((uint32)(((uint32)(a1) + (uint32)(444)))));
      }
      v9 = 127;
      if (r_u8(v7 + 144u) || (v9 = 128, r_u8(v7 + 128u)))
        w_u8(((uint32)(((uint32)(a1) + (uint32)(469)))),v9);
    }
    if (r_u16(((uint32)(((uint32)(a1) + (uint32)(574))))))
    {
      w_u8(((uint32)(((uint32)(a1) + (uint32)(468)))),(r_u8(((uint32)(((uint32)(a1) + (uint32)(468)))))-((((uint32)(r_u16(((uint32)(((uint32)(a1) + (uint32)(574)))))) * (uint32)(((sint8)(r_u8(((uint32)(((uint32)(a1) + (uint32)(468))))))))) / r_u16(((uint32)(((uint32)(a1) + (uint32)(576)))))))));
      w_u8(((uint32)(((uint32)(a1) + (uint32)(469)))),(r_u8(((uint32)(((uint32)(a1) + (uint32)(469)))))-((((uint32)(r_u16(((uint32)(((uint32)(a1) + (uint32)(574)))))) * (uint32)(((sint8)(r_u8(((uint32)(((uint32)(a1) + (uint32)(469))))))))) / r_u16(((uint32)(((uint32)(a1) + (uint32)(576)))))))));
    }
    if ((r_u8(((uint32)(((uint32)(a1) + (uint32)(471))))) | r_u8(((uint32)(((uint32)(a1) + (uint32)(472)))))))
      goto LABEL_28;
    v10 = r_u32(((uint32)(((uint32)(a1) + (uint32)(444)))));
    w_u8(((uint32)(((uint32)(a1) + (uint32)(470)))),0);
    v11 = 0x80;
    if (r_u8(v10) || (v11 = 127, r_u8(v10 + 48u)))
      w_u8(((uint32)(((uint32)(a1) + (uint32)(471)))),v11);
    v12 = r_u32(((uint32)(((uint32)(a1) + (uint32)(444)))));
    v13 = 127;
    if (r_u8(v12 + 32u) || (v13 = 128, r_u8(v12 + 16u)))
    {
      w_u8(((uint32)(((uint32)(a1) + (uint32)(472)))),v13);
      v12 = r_u32(((uint32)(((uint32)(a1) + (uint32)(444)))));
    }
    if ((((r_u8(v12) || r_u8((v12+(48)*1u))) || r_u8((v12+(32)*1u))) || r_u8((v12+(16)*1u))))
    {
      v14 = ((uint32)(r_u8(((uint32)(((uint32)(a1) + (uint32)(473)))))) + (uint32)(1));
      w_u8(((uint32)(((uint32)(a1) + (uint32)(473)))),v14);
      if ((v14 >= 2u))
        w_u8(((uint32)(((uint32)(a1) + (uint32)(473)))),2);
    }
    else
    {
      LABEL_28:
      w_u8(((uint32)(((uint32)(a1) + (uint32)(473)))),0);

    }
    v15 = r_u8(a1 + 468u);
    v17 = r_u8(a1 + 469u);
    result = v15 | (uint32)v17;
    if (result)
    {
      result = ratan2(-(sint32)(sint8)v15, (sint8)v17);
      w_u16(a1 + 474u, (1024u - result) & 4095u);
    }
    v18 = r_u8(a1 + 471u);
    v19 = r_u8(a1 + 472u);
    result = v18 | (uint32)v19;
    if (result)
    {
      result = ratan2(-(sint32)(sint8)v18, (sint8)v19);
      w_u16(a1 + 476u, (1024u - result) & 4095u);
    }
  }
  return result;
}



uint32 sub_8005EB60(uint32 a1)
{
  sint32 result;
  sint32 v2;
  sint32 v3;
  result = 0;
  if ((((sint32)(((sint8)(r_u8(((uint32)(((uint32)(a1) + (uint32)(469))))))))) > 0))
  {
    v2 = ((sint16)(r_u16(((uint32)(((uint32)(a1) + (uint32)(474)))))));
    result = 0;
    if ((((sint32)(v2)) >= 768))
    {
      if ((((sint32)(v2)) >= 1281))
      {
        return 0;
      }
      else
      {
        v3 = r_u8(((uint32)(((uint32)(a1) + (uint32)(26)))));
        w_u32(((uint32)(((uint32)(a1) + (uint32)(460)))),1024);
        result = 1;
        if ((v3 != 1))
        {
          sub_80063118(a1,1,1);
          return 1;
        }
      }
    }
  }
  return result;
}



uint32 sub_8005F124(uint32 a1)
{
  sint32 v2;
  sint32 v3;
  sint8 v4;
  sint32 result;
  if (((r_u16(((uint32)(((uint32)(a1) + (uint32)(216))))) & 2) != 0))
  {
    LABEL_4:
    result = 0;

    if (r_u8(((uint32)(((uint32)(a1) + (uint32)(578))))))
      return result;
    goto LABEL_5;
  }
  v2 = r_u8(((uint32)(((uint32)(a1) + (uint32)(578)))));
  v3 = (v2 == 0);
  v4 = ((uint32)(v2) - (uint32)(1));
  if (!v3)
  {
    w_u8(((uint32)(((uint32)(a1) + (uint32)(578)))),v4);
    goto LABEL_4;
  }
  LABEL_5:
  w_u32(((uint32)(((uint32)(a1) + (uint32)(480)))),r_u32(((uint32)(((uint32)(a1) + (uint32)(8))))));

  sub_80063038(a1,2,((uint32)(r_u16(0x800EC4D4u)) + (uint32)(1)),r_u16(0x800EC4D6u));
  result = 1;
  w_u32(((uint32)(((uint32)(a1) + (uint32)(460)))),4);
  return result;
}


/* TODO Missing call adapter abs32 */
uint32 sub_80061A78(uint32 a1, uint32 a2, uint32 a3, uint32 a4)
{
  sint32 v4;
  sint32 v5;
  short v6;
  sint32 result;
  sint32 v8;
  uint32 v9;
  sint32 v10;
  uint32 v11;
  short v12;
  short v13;
  sint32 v14;
  short v15;
  sint32 v16;
  if (r_u8(a1 + 470u) || (v4 = a3 & 4095u, r_u8(a1 + 473u) == 1))
  {
    a4 = 1;
    v4 = (a3 & 0xFFF);
  }
  v5 = (a2 & 0xFFF);
  v6 = (a2 & 0xFFF);
  if (a4 || (v8 = (sint16)r_u16(a1 + 18u), v9 = abs((sint32)(v4 - v8)), (uint32)(v9 - 1025u) < 2047u))
  {
    result = r_u32(((uint32)(((uint32)(a1) + (uint32)(360)))));
    LABEL_6:
    w_u16(((uint32)(((uint32)(a1) + (uint32)(18)))),v4);

    w_u16(((uint32)(((uint32)(result) + (uint32)(86)))),v5);
    w_u32(((uint32)(((uint32)(a1) + (uint32)(436)))),0);
    w_u32(((uint32)(((uint32)(a1) + (uint32)(440)))),0);
    return result;
  }
  v10 = r_u32(((uint32)(((uint32)(a1) + (uint32)(360)))));
  v11 = ((void)(((sint32)(((uint32)(v5) - (uint32)(((sint16)(r_u16(((uint32)(((uint32)(v10) + (uint32)(86)))))))))))),abort(),0u);
  v12 = r_u16(((uint32)(((uint32)(v10) + (uint32)(86)))));
  result = v10;
  if ((((uint32)(v11) - (uint32)(1025)) < 0x7FF))
    goto LABEL_6;
  if ((v9 && v11))
  {
    v13 = ((uint32)(v12) - (uint32)(v9));
    if ((((sint32)(v8)) >= ((sint32)(v4))))
      v13 = ((uint32)(v12) + (uint32)(v9));
    w_u16(((uint32)(((uint32)(v10) + (uint32)(86)))),(v13 & 0xFFF));
  }
  w_u32(((uint32)(((uint32)(a1) + (uint32)(440)))),8);
  w_u16(((uint32)(((uint32)(a1) + (uint32)(18)))),v4);
  v14 = r_u32(((uint32)(((uint32)(a1) + (uint32)(360)))));
  w_u32(((uint32)(((uint32)(a1) + (uint32)(432)))),v6);
  v15 = r_u16(((uint32)(((uint32)(v14) + (uint32)(86)))));
  if ((((sint32)(v15)) >= ((sint32)(v6))))
  {
    v16 = ((sint32)(((uint32)(v6) - (uint32)(v15))));
    if ((((sint32)(((sint32)(((uint32)(v15) - (uint32)(v6)))))) >= 1024))
      v16 = ((sint32)(((uint32)(v6) - (uint32)(((short)(((uint32)(v15) - (uint32)(4096))))))));
  }
  else
    if ((((sint32)(((sint32)(((uint32)(v6) - (uint32)(v15)))))) >= 1024))
  {
    v16 = ((sint32)(((uint32)(((short)(((uint32)(v6) - (uint32)(4096))))) - (uint32)(v15))));
  }
  else
  {
    v16 = ((sint32)(((uint32)(v6) - (uint32)(v15))));
  }
  result = (((sint32)(v16)) / 8);
  w_u32(((uint32)(((uint32)(a1) + (uint32)(436)))),result);
  return result;
}


/* TODO Missing call adapter xport_draft_missing_gte_adapter */
/* TODO Missing host buffer adapter xport_draft_host_sub_8006C22C_p2 */
/* TODO Missing host buffer adapter xport_draft_host_sub_8006C3AC_p1 */
/* TODO Missing host buffer adapter xport_draft_host_sub_8006C4EC_p12 */
/* TODO Missing host buffer adapter xport_draft_host_sub_8006C564_p123 */
/* TODO Missing host buffer adapter xport_draft_host_sub_8006C5C4_p13 */
uint32 sub_8007C398(uint32 a1, uint32 a2, uint32 a3, uint32 a4, uint32 a5)
{
  uint32 xport_draft_asm_carrier; /* TODO GTE carrier */
  sint32 result;
  sint32 v19;
  sint32 v20;
  sint32 v21;
  sint32 v22;
  sint32 v23;
  sint32 v24;
  sint32 vector[3];
  int v35[4];
  uint32 v36[4];
  sint32 v37;
  sint32 v38;
  sint32 v39;
  sint32 v40;
  sint32 v41;
  int *v42;
  w_u32(0x800FFA84u,0);
  w_u32(0x800ED634u,0x7FFFFFFF);
  xport_draft_host_sub_8006C3AC_p1(v35,a2,a1);
  v37 = 12;
  xport_draft_host_sub_8006C564_p123(vector,v35,&v37);
  xport_draft_asm_carrier = 0x800ED5C0u;
  w_u32(0x800ED5C0u,vector[0]);
  w_u32(0x800ED5C4u,vector[1]);
  w_u32(0x800ED5C8u,vector[2]);
  (abort(),0u);
  w_u32(0x800FFA14u,sub_80085B54(((uint32)(((uint32)(r_u32(0x800ED5E4u)) + (uint32)(r_u32(0x800ED5E8u)))) + (uint32)(r_u32(0x800ED5ECu)))));
  if (!r_u32(0x800FFA14u))
    return 0;
  v38 = 12;
  xport_draft_host_sub_8006C5C4_p13(v36,0x800ED5C0u,&v38);
  xport_draft_host_sub_8006C4EC_p12(vector,v36,0x800FFA14u);
  w_u32(0x800ED5C0u,vector[0]);
  w_u32(0x800ED5C4u,vector[1]);
  w_u32(0x800ED5C8u,vector[2]);
  xport_draft_asm_carrier = vector[0];
  xport_draft_asm_carrier = vector[1];
  (abort(),0u);
  xport_draft_asm_carrier = vector[2];
  (abort(),0u);
  w_u16(0x800FFA44u,vector[0]);
  w_u16(0x800FFA46u,vector[1]);
  w_u16(0x800FFA48u,vector[2]);
  xport_draft_asm_carrier = 0x800FFA44u;
  xport_draft_asm_carrier = r_u32(0x800FFA48u);
  (abort(),0u);
  if ((((sint32)(((sint32)(r_u32(a1))))) >= ((sint32)(((sint32)(r_u32(a2)))))))
  {
    w_u32(0x800ED5CCu,((sint32)(r_u32(a2))));
    v19 = ((sint32)(r_u32(a1)));
  }
  else
  {
    w_u32(0x800ED5CCu,((sint32)(r_u32(a1))));
    v19 = ((sint32)(r_u32(a2)));
  }
  w_u32(0x800ED5D8u,v19);
  if ((((sint32)(((sint32)(r_u32((a1+(1)*4u)))))) >= ((sint32)(((sint32)(r_u32((a2+(1)*4u))))))))
  {
    w_u32(0x800ED5D0u,((sint32)(r_u32((a2+(1)*4u)))));
    v20 = ((sint32)(r_u32((a1+(1)*4u))));
  }
  else
  {
    w_u32(0x800ED5D0u,((sint32)(r_u32((a1+(1)*4u)))));
    v20 = ((sint32)(r_u32((a2+(1)*4u))));
  }
  w_u32(0x800ED5DCu,v20);
  if ((((sint32)(((sint32)(r_u32((a1+(2)*4u)))))) >= ((sint32)(((sint32)(r_u32((a2+(2)*4u))))))))
  {
    w_u32(0x800ED5D4u,((sint32)(r_u32((a2+(2)*4u)))));
    v21 = ((sint32)(r_u32((a1+(2)*4u))));
  }
  else
  {
    w_u32(0x800ED5D4u,((sint32)(r_u32((a1+(2)*4u)))));
    v21 = ((sint32)(r_u32((a2+(2)*4u))));
  }
  w_u32(0x800ED5E0u,v21);
  if (a4)
  {
    v42 = vector;
    do
    {
      if (((r_u16(((uint32)(((uint32)(a4) + (uint32)(78))))) & 0x40) != 0))
        goto LABEL_30;
      if ((a4 == a5))
        goto LABEL_30;
      v22 = r_u32(((uint32)(((uint32)(a4) + (uint32)(4)))));
      w_u32(0x800FF9F0u,((uint32)(r_u16(((uint32)(((uint32)(a4) + (uint32)(212)))))) << (uint32)(12)));
      if ((((sint32)(((uint32)(v22) + (uint32)(r_u32(0x800FF9F0u))))) < 0x800ED5CCu))
        goto LABEL_30;
      if ((r_u32(0x800ED5D8u) < ((sint32)(((uint32)(v22) - (uint32)(r_u32(0x800FF9F0u)))))))
        goto LABEL_30;
      v23 = r_u32(((uint32)(((uint32)(a4) + (uint32)(8)))));
      if ((((sint32)(((uint32)(v23) + (uint32)(r_u32(0x800FF9F0u))))) < r_u32(0x800ED5D0u)))
        goto LABEL_30;
      if ((r_u32(0x800ED5DCu) < ((sint32)(((uint32)(v23) - (uint32)(r_u32(0x800FF9F0u)))))))
        goto LABEL_30;
      v24 = r_u32(((uint32)(((uint32)(a4) + (uint32)(12)))));
      if ((((sint32)(((uint32)(v24) + (uint32)(r_u32(0x800FF9F0u))))) < r_u32(0x800ED5D4u)))
        goto LABEL_30;
      if ((r_u32(0x800ED5E0u) < ((sint32)(((uint32)(v24) - (uint32)(r_u32(0x800FF9F0u)))))))
        goto LABEL_30;
      xport_draft_host_sub_8006C3AC_p1(v35,((uint32)(((uint32)(a4) + (uint32)(4)))),a1);
      v39 = 12;
      xport_draft_host_sub_8006C564_p123(vector,v35,&v39);
      w_u32(0x800ED5F0u,vector[0]);
      w_u32(0x800ED5F4u,vector[1]);
      w_u32(0x800ED5F8u,vector[2]);
      xport_draft_asm_carrier = 0x800ED5F0u;
      (abort(),0u);
      w_u32(0x800FFA10u,((uint32)(r_u16(((uint32)(((uint32)(a4) + (uint32)(212)))))) * (uint32)(r_u16(((uint32)(((uint32)(a4) + (uint32)(212))))))));
      if ((((uint32)(((uint32)(r_u32(0x800ED5E4u)) + (uint32)(r_u32(0x800ED5E8u)))) + (uint32)(r_u32(0x800ED5ECu))) >= r_u32(0x800FFA10u)))
        goto LABEL_30;
      xport_draft_asm_carrier = 0x800ED5F0u;
      xport_draft_asm_carrier = (((unsigned short)(0x800ED5F0u)) | ((uint32)(((unsigned short)(r_u32(0x800ED5F4u)))) << (uint32)(16)));
      (abort(),0u);
      if ((0x800FFA18u >= r_u32(0x800ED634u)))
        goto LABEL_30;
      if ((0x800FFA18u >= 0))
      {
        if ((0x800FFA14u >= 0x800FFA18u))
          goto LABEL_29;
        xport_draft_host_sub_8006C3AC_p1(v35,((uint32)(((uint32)(a4) + (uint32)(4)))),a2);
        v40 = 12;
        xport_draft_host_sub_8006C564_p123(v42,v35,&v40);
        xport_draft_asm_carrier = 0x800ED5FCu;
        w_u32(0x800ED5FCu,vector[0]);
        w_u32(0x800ED600u,vector[1]);
        w_u32(0x800ED604u,vector[2]);
        (abort(),0u);
      }
      else
      {
        xport_draft_asm_carrier = 0x800ED5F0u;
        (abort(),0u);
      }
      if ((((uint32)(((uint32)(r_u32(0x800ED5E4u)) + (uint32)(r_u32(0x800ED5E8u)))) + (uint32)(r_u32(0x800ED5ECu))) < r_u32(0x800FFA10u)))
      {
        LABEL_29:
        w_u32(0x800FFA84u,a4);

        w_u32(0x800ED634u,0x800FFA18u);
      }
      LABEL_30:
      a4 = r_u32(((uint32)(((uint32)(a4) + (uint32)(28)))));

    }
    while (a4);
  }
  result = r_u32(0x800FFA84u);
  if (r_u32(0x800FFA84u))
  {
    xport_draft_asm_carrier = 0x800ED5C0u;
    (abort(),0u);
    xport_draft_asm_carrier = r_u32(0x800ED634u);
    (abort(),0u);
    v41 = 12;
    xport_draft_host_sub_8006C22C_p2(a3,&v41);
    sub_8006C0B8(a3,a1);
    return r_u32(0x800FFA84u);
  }
  return result;
}


/* TODO Missing call adapter xport_draft_missing_gte_adapter */

uint32 sub_800666DC(uint32 a1, uint32 a2)
{
  uint32 xport_draft_asm_carrier; /* TODO GTE carrier */
  sint32 v4;
  sint32 v5;
  sint32 result;
  int v8[4];
  v4 = ((sint32)(r_u32((a2+(2)*4u))));
  v5 = (((sint32)(((sint32)(r_u32((a2+(1)*4u)))))) >> 12);
  v8[0] = (((sint32)(((sint32)(r_u32(a2))))) >> 12);
  xport_draft_asm_carrier = v8;
  v8[1] = v5;
  v8[2] = (((sint32)(v4)) >> 12);
  (abort(),0u);
  result = sub_80085B54(((uint32)(((sint32)(((uint32)(v8[0]) + (uint32)(v5))))) + (uint32)((((sint32)(v4)) >> 12))));
  if (result)
  {
    w_u32(a1,(((sint32)(((sint32)(r_u32(a2))))) / ((sint32)(result))));
    w_u32((a1+(1)*4u),(((sint32)(((sint32)(r_u32((a2+(1)*4u)))))) / ((sint32)(result))));
    result = (((sint32)(((sint32)(r_u32((a2+(2)*4u)))))) / ((sint32)(result)));
    w_u32((a1+(2)*4u),result);
  }
  else
  {
    w_u32(a1,0);
    w_u32((a1+(1)*4u),0);
    w_u32((a1+(2)*4u),0);
  }
  return result;
}



uint32 sub_8007CAC8(uint32 a1, uint32 a2)
{
  sint32 v3;
  sint32 v4;
  sint32 result;
  sint32 v6;
  sint32 v7;
  uint32 v8;
  sint32 v9;
  sint32 v10;
  sint32 v11;
  sint32 v12;
  sint32 v13;
  v3 = r_u16(((uint32)(((uint32)(a2) + (uint32)(2)))));
  v4 = r_u8(((uint32)(((uint32)(a1) + (uint32)(27)))));
  w_u32(((uint32)(((uint32)(a1) + (uint32)(364)))),((uint32)(a2) + (uint32)(4)));
  w_u32(((uint32)(((uint32)(a1) + (uint32)(356)))),sub_8006B864(((uint32)(24) * (uint32)(r_u32(((uint32)(((uint32)(r_u32((0x800EAEF8u+(((uint32)(((uint32)(16) * (uint32)(v4))) + (uint32)(4)))*4u))) - (uint32)(4))))))),0,1));
  result = sub_8006B864(((uint32)(12) * (uint32)(v3)),0,1);
  v6 = 0;
  w_u32(((uint32)(((uint32)(a1) + (uint32)(360)))),result);
  if (v3)
  {
    v7 = 0;
    do
    {
      ++v6;
      v8 = ((uint32)(((uint32)(v7) + (uint32)(r_u32(((uint32)(((uint32)(a1) + (uint32)(360)))))))));
      w_u16((v8+(5)*2u),0);
      w_u16((v8+(4)*2u),0);
      w_u16((v8+(3)*2u),0);
      w_u16((v8+(2)*2u),0);
      w_u16((v8+(1)*2u),0);
      w_u16(v8,0);
      result = (((sint32)(v6)) < ((sint32)(v3)));
      v7 += 12;
    }
    while ((((sint32)(v6)) < ((sint32)(v3))));
  }
  v9 = 0;
  if (v3)
  {
    v10 = 0;
    do
    {
      v11 = 0;
      v12 = 0;
      while (1)
      {
        v13 = r_u32(((uint32)(((uint32)(a1) + (uint32)(364)))));
        if ((r_u16(((uint32)(((uint32)(((sint32)(((uint32)(v10) + (uint32)(v13))))) + (uint32)(2))))) == r_u16(((uint32)(((sint32)(((uint32)(v12) + (uint32)(v13)))))))))
          break;
        ++v11;
        v12 += 12;
        if ((((sint32)(v11)) >= ((sint32)(v3))))
          goto LABEL_10;
      }

      w_u16(((uint32)(((uint32)(((sint32)(((uint32)(v10) + (uint32)(v13))))) + (uint32)(10)))),v11);
      LABEL_10:
      if ((v11 == v3))
        w_u16(((uint32)(((uint32)(((uint32)(v10) + (uint32)(r_u32(((uint32)(((uint32)(a1) + (uint32)(364)))))))) + (uint32)(10)))),-1);

      result = (++v9 < ((sint32)(v3)));
      v10 += 12;
    }
    while ((((sint32)(v9)) < ((sint32)(v3))));
  }
  return result;
}


/* TODO Missing host buffer adapter xport_draft_host_sub_8007BB24_p1 */
/* TODO Missing host buffer adapter xport_draft_host_sub_8007DD04_p1 */
uint32 sub_8005F470(uint32 a1)
{
  sint32 result;
  sint32 v3;
  sint32 v4;
  sint32 v5;
  sint32 v6;
  union {
    uint32 words[35];
    struct {
      sint32 origin[3], target[3];
      uint8 workspace[0x68-6*sizeof(sint32)];
      uint32 hit;
      sint32 position[3];
      uint16 rotation[3];
    } fields;
  } ray;
  result = (r_u32(((uint32)(((uint32)(a1) + (uint32)(460))))) & 0x3000);
  if (result)
    goto LABEL_8;
  if (((r_u16(((uint32)(a1))) & 8) != 0))
  {
    v3 = r_u32(((uint32)(((uint32)(a1) + (uint32)(4)))));
    w_u8(((uint32)(((uint32)(a1) + (uint32)(314)))),1);
    v4 = r_u32(0x800FF37Cu);
    v5 = r_u32(((uint32)(((uint32)(a1) + (uint32)(12)))));
    result = -4096;
    w_u16(((uint32)(((uint32)(a1) + (uint32)(164)))),0);
    w_u32(((uint32)(((uint32)(a1) + (uint32)(166)))),61440);
    w_u32(((uint32)(((uint32)(a1) + (uint32)(184)))),v3);
    w_u32(((uint32)(((uint32)(a1) + (uint32)(188)))),v4);
    w_u32(((uint32)(((uint32)(a1) + (uint32)(192)))),v5);
    return result;
  }
  if (((r_u16(((uint32)(((uint32)(a1) + (uint32)(216))))) & 2) != 0))
  {
    w_u8(((uint32)(((uint32)(a1) + (uint32)(314)))),1);
    result = ((uint32)(r_u8(((uint32)(((uint32)(a1) + (uint32)(582)))))) << (uint32)(12));
    w_u32(((uint32)(((uint32)(a1) + (uint32)(188)))),((uint32)(r_u32(((uint32)(((uint32)(a1) + (uint32)(8)))))) + (uint32)(result)));
    return result;
  }
  ray.fields.origin[0] = r_u32(((uint32)(((uint32)(a1) + (uint32)(4)))));
  ray.fields.origin[1] = ((uint32)(r_u32(((uint32)(((uint32)(a1) + (uint32)(8)))))) - (uint32)(0x40000));
  v6 = r_u32(((uint32)(((uint32)(a1) + (uint32)(12)))));
  ray.fields.target[0] = ray.fields.origin[0];
  ray.fields.target[1] = ((uint32)(ray.fields.origin[1]) + (uint32)(0x800000));
  ray.fields.origin[2] = v6;
  ray.fields.target[2] = v6;
  xport_draft_host_sub_8007BB24_p1(&ray);
  xport_draft_host_sub_8007DD04_p1(&ray,1);
  result = 1;
  if (!ray.fields.hit)
  {
    LABEL_8:
    w_u8(((uint32)(((uint32)(a1) + (uint32)(314)))),0);

  }
  else
  {
    w_u8(((uint32)(((uint32)(a1) + (uint32)(314)))),1);
    w_u32(((uint32)(((uint32)(a1) + (uint32)(184)))),ray.fields.position[0]);
    w_u32(((uint32)(((uint32)(a1) + (uint32)(188)))),ray.fields.position[1]);
    w_u32(((uint32)(((uint32)(a1) + (uint32)(192)))),ray.fields.position[2]);
    w_u16(((uint32)(((uint32)(a1) + (uint32)(164)))),ray.fields.rotation[0]);
    w_u16(((uint32)(((uint32)(a1) + (uint32)(166)))),ray.fields.rotation[1]);
    result = ray.fields.rotation[2];
    w_u16(((uint32)(((uint32)(a1) + (uint32)(168)))),ray.fields.rotation[2]);
  }
  return result;
}



uint32 sub_80063CF0(void)
{
  sint32 i;
  sint32 result;
  for (i = r_u32(0x800FF630u); i; i = r_u32(((uint32)(((uint32)(i) + (uint32)(20))))))
  {
    result = r_u8(((uint32)(((uint32)(i) + (uint32)(5)))));
    if (r_u8(((uint32)(((uint32)(i) + (uint32)(5))))))
    {
      result = r_u8(((uint32)(((uint32)(i) + (uint32)(4)))));
      if (!(r_u8(((uint32)(((uint32)(i) + (uint32)(4)))))))
        w_u8(((uint32)(((uint32)(i) + (uint32)(5)))),0);
    }
  }

  return result;
}



uint32 sub_80069EF4(uint32 a1, uint32 a2, uint32 a3)
{
  sint32 v4;
  uint32 v5;
  if ((((sint32)(a1)) < 0))
    return 0;
  v4 = sub_80067E9C(a2,256,12000);
  v5 = (((uint32)((v4 & 0xFFFu)) * (uint32)(r_u16(0x800ECC7Au))) >> 12);
  if ((((sint32)(v4)) < 0))
    v5 = 0u - v5;
  return sub_8006A4C4(r_u32((0x800E53A8u+(((uint32)(2) * (uint32)(a1)))*4u)),r_u8((((uint32)(0x800E53A8u))+((((uint32)(8) * (uint32)(a1)) | 4))*1u)),((short)(v5)),((short)((((uint32)(((uint32)((((v4>>16)&65535u) & 0xFFF)) * (uint32)(r_u16(0x800ECC7Au))))) >> 12))),a3);
}


