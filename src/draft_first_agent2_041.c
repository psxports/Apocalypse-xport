#include "game_menu_helpers.h"
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
#include <stdlib.h>
#include <string.h>

uint32 sub_80068E80(uint32 a1, uint32 a2)
{
  uint32 v4;
  uint32 v5;
  uint32 v6;
  uint32 v7;
  sint32 result;
  sint32 v9;
  short v10;
  sint32 v11;
  short v12;
  short v13;
  short v14;
  sint32 v15;
  sint32 v16;
  sint32 v17;
  short v18;
  short v19;
  short v20;
  short v21;
  v4 = ((unsigned short)(((sint16)(r_u16((a1+(1)*2u))))));
  v5 = ((unsigned short)(((sint16)(r_u16((a2+(1)*2u))))));
  if ((v4 == v5))
  {
    v6 = ((unsigned short)(((sint16)(r_u16(a1)))));
    v7 = ((unsigned short)(((sint16)(r_u16(a2)))));
    result = (v7 < v6);
    if ((v6 >= v7))
    {
      if ((v7 < v6))
      {
        v15 = ((unsigned short)(((sint16)(r_u16((a2+(2)*2u))))));
        result = ((uint32)(((unsigned short)(((sint16)(r_u16(a2)))))) + (uint32)(v15));
        if ((result == ((unsigned short)(((sint16)(r_u16(a1)))))))
        {
          result = ((unsigned short)(((sint16)(r_u16((a2+(3)*2u))))));
          v10 = ((sint16)(r_u16(a2)));
          if ((((unsigned short)(((sint16)(r_u16((a1+(3)*2u)))))) == result))
          {
            v11 = ((sint32)(a1));
            v12 = ((sint16)(r_u16((a2+(1)*2u))));
            v13 = ((sint16)(r_u16((a1+(3)*2u))));
            v14 = ((sint32)(((uint32)(((sint16)(r_u16((a1+(2)*2u))))) + (uint32)(v15))));
            goto LABEL_15;
          }
        }
      }
    }
    else
    {
      v9 = ((unsigned short)(((sint16)(r_u16((a1+(2)*2u))))));
      result = ((uint32)(v6) + (uint32)(v9));
      if ((((uint32)(v6) + (uint32)(v9)) == v7))
      {
        result = ((unsigned short)(((sint16)(r_u16((a2+(3)*2u))))));
        v10 = ((sint16)(r_u16(a1)));
        if ((((unsigned short)(((sint16)(r_u16((a1+(3)*2u)))))) == result))
        {
          v11 = ((sint32)(a1));
          v12 = ((sint16)(r_u16((a1+(1)*2u))));
          v13 = ((sint16)(r_u16((a1+(3)*2u))));
          v14 = ((sint32)(((uint32)(v9) + (uint32)(((sint16)(r_u16((a2+(2)*2u))))))));
          LABEL_15:
          sub_8006838C(0x800FF67Cu,v11);

          sub_8006838C(0x800FF67Cu,((sint32)(a2)));
          return ((sint32)(sub_800682CC(v10,v12,v14,v13)));
        }
      }
    }
  }
  else
  {
    result = ((unsigned short)(((sint16)(r_u16(a2)))));
    if ((((unsigned short)(((sint16)(r_u16(a1))))) != result))
      return result;
    result = (v5 < v4);
    if ((v4 >= v5))
    {
      if ((v5 < v4))
      {
        v17 = ((unsigned short)(((sint16)(r_u16((a2+(3)*2u))))));
        result = ((uint32)(((unsigned short)(((sint16)(r_u16((a2+(1)*2u))))))) + (uint32)(v17));
        if ((result == ((unsigned short)(((sint16)(r_u16((a1+(1)*2u))))))))
        {
          result = ((unsigned short)(((sint16)(r_u16((a2+(2)*2u))))));
          v18 = ((sint16)(r_u16((a2+(1)*2u))));
          if ((((unsigned short)(((sint16)(r_u16((a1+(2)*2u)))))) == result))
          {
            v19 = ((sint16)(r_u16(a2)));
            v20 = ((sint16)(r_u16((a1+(2)*2u))));
            v21 = ((sint32)(((uint32)(((sint16)(r_u16((a1+(3)*2u))))) + (uint32)(v17))));
            sub_8006838C(0x800FF67Cu,((sint32)(a1)));
            sub_8006838C(0x800FF67Cu,((sint32)(a2)));
            return ((sint32)(sub_800682CC(v19,v18,v20,v21)));
          }
        }
      }
    }
    else
    {
      v16 = ((unsigned short)(((sint16)(r_u16((a1+(3)*2u))))));
      result = ((uint32)(((unsigned short)(((sint16)(r_u16((a1+(1)*2u))))))) + (uint32)(v16));
      if ((result == ((unsigned short)(((sint16)(r_u16((a2+(1)*2u))))))))
      {
        result = ((unsigned short)(((sint16)(r_u16((a2+(2)*2u))))));
        v12 = ((sint16)(r_u16((a1+(1)*2u))));
        if ((((unsigned short)(((sint16)(r_u16((a1+(2)*2u)))))) == result))
        {
          v11 = ((sint32)(a1));
          v10 = ((sint16)(r_u16(a1)));
          v14 = ((sint16)(r_u16((a1+(2)*2u))));
          v13 = ((sint32)(((uint32)(v16) + (uint32)(((sint16)(r_u16((a2+(3)*2u))))))));
          goto LABEL_15;
        }
      }
    }
  }
  return result;
}


/* TODO Missing call adapter v1 */
void sub_8006FB60(void)
{
  uint32 v0;
  uint32 v1;
  uint32 v2;
  sint32 result;
  v0 = ((uint32)(r_u32(0x800FF7DCu)));
  while (v0)
  {
    v1 = ((sint32)(r_u32(((uint32)(((uint32)(r_u32(v0)) + (uint32)(12)))))));
    v2 = (((uint32)(v0))+(((sint16)(r_u16(((uint32)(((uint32)(r_u32(v0)) + (uint32)(8))))))))*1u);
    v0 = ((uint32)(r_u32((v0+(5)*4u))));
    result = ((void)(v2),(void)(3),abort(),0u);
  }


}


/* TODO Missing call adapter indirect */
uint32 sub_80032AF4(uint32 a1)
{
  sint32 v1;
  sint32 i;
  sint32 result;
  v1 = a1;
  if (a1)
  {
    for (i = r_u32(((uint32)(((uint32)(a1) + (uint32)(4)))));; i = r_u32(((uint32)(((uint32)(i) + (uint32)(4))))))
    {
      result = r_u8(((uint32)(((uint32)(v1) + (uint32)(66)))));
      if (!(r_u8(((uint32)(((uint32)(v1) + (uint32)(66)))))))
      {
        if (v1)
          result = ((void)(((sint32)(((uint32)(v1) + (uint32)(((sint16)(r_u16(((uint32)(((uint32)(r_u32(((uint32)(((uint32)(v1) + (uint32)(68)))))) + (uint32)(8)))))))))))),(void)(3),abort(),0u);
      }
      v1 = i;
      if (!i)
        break;
    }

  }
  return result;
}


/* TODO Missing call adapter sub_8008EFFC */
uint32 sub_8006A3F0(void)
{
  sint32 result;
  if ((r_u32(0x800FF6ACu) != -1))
    ((void)(((short)(r_u32(0x800FF6ACu)))),abort(),0u);
  result = 1;
  w_u32(0x800FF6B8u,1);
  w_u32(0x800FF6B4u,0);
  return result;
}



void sub_800653F4(void)
{
  uint32 allocation = r_u32(0x800FF63Cu);
  if (allocation) sub_8006BC20(allocation);
  w_u32(0x800FF63Cu, 0);
}


/* TODO Missing host buffer adapter xport_draft_host_sub_80068450_p459 */
uint32 sub_800691B8(void)
{
  sint32 v2;
  sint32 v3;
  uint32 result;
  sint32 v5;
  uint32 v6;
  uint32 v7;
  sint32 v8;
  sint32 v9;
  sint32 v10;
  sint32 v11;
  sint32 v12;
  uint32 i;
  sint32 v14;
  sint32 v15;
  uint32 j;
  sint16 rectangle[4];
  v2 = r_u32(0x800FF67Cu);
  while (1)
  {
    v3 = v2;
    if (!v2)
      break;
    v2 = r_u32(((uint32)(((uint32)(v2) + (uint32)(8)))));
    sub_80068270(v3);
  }

  w_u32(0x800FF67Cu,0);
  sub_800682CC(512,256,512,256);
  result = sub_800682CC(512,0,512,256);
  v6 = 0;
  v7 = ((uint32)(r_u32(0x800FF678u)));
  while (v7)
  {
    result = ((uint32)(sub_8006B864(20,0,1)));
    v8 = r_u32((v7+(1)*4u));
    v9 = r_u32((v7+(2)*4u));
    v10 = r_u32((v7+(3)*4u));
    w_u32(((uint32)(result)),r_u32(v7));
    w_u32((((uint32)(result))+(1)*4u),v8);
    w_u32((((uint32)(result))+(2)*4u),v9);
    w_u32((((uint32)(result))+(3)*4u),v10);
    w_u32((((uint32)(result))+(4)*4u),r_u32((v7+(4)*4u)));
    w_u32((((uint32)(result))+(2)*4u),v6);
    v7 = ((uint32)(r_u32((v7+(2)*4u))));
    v6 = result;
  }

  v11 = r_u32(0x800FF678u);
  while (1)
  {
    v12 = v11;
    if (!v11)
      break;
    v11 = r_u32(((uint32)(((uint32)(v11) + (uint32)(8)))));
    result = ((uint32)(sub_80068270(v12)));
  }

  w_u32(0x800FF678u,0);
  for (i = v6; i; i = ((uint32)(r_u32((((uint32)(i))+(2)*4u)))))
  {
    v14 = ((unsigned short)(r_u16((i+(8)*2u))));
    v15 = ((unsigned short)(r_u16((i+(9)*2u))));
    w_u32(0x800FF674u,r_u8((((uint32)(i))+(13)*1u)));
    result = xport_draft_host_sub_80068450_p45((uint32)(sint32)(sint8)r_u8(i + 12u), v14, v15, rectangle, &rectangle[2], r_u8(i + 14u), r_u8(i + 15u), 0u);
  }

  for (j = v6; v6; j = v6)
  {
    v6 = ((uint32)(r_u32((((uint32)(v6))+(2)*4u))));
    result = sub_8006BC20(j);
  }

  return result;
}


/* TODO Missing call adapter indirect */
uint32 sub_8006FB04(void)
{
  sint32 result;
  for (result = r_u32(0x800FF7DCu); r_u32(0x800FF7DCu); result = r_u32(0x800FF7DCu))
    ((void)(((uint32)(r_u32(0x800FF7DCu)) + (uint32)(((sint16)(r_u16(((uint32)(((uint32)(r_u32(((uint32)(r_u32(0x800FF7DCu))))) + (uint32)(8)))))))))),(void)(3),abort(),0u);

  return result;
}


/* TODO Missing host buffer adapter xport_draft_host_sub_8006C4EC_p13 */
static uint32 draft_3AA20_position(const uint32 position[3])
{
  uint32 magnitude, divisor;
  sint32 quotient, component;
  unsigned i;
  for (i = 0; i < 3; ++i) w_u32(0x800A6844u + i * 4u, position[i]);
  magnitude = sub_8006BF04(0x800A6844u);
  w_u32(0x800FF480u, magnitude);
  divisor = 0u - magnitude;
  for (i = 0; i < 3; ++i) {
    component = (sint32)position[i];
    if (!divisor) quotient = component < 0 ? 1 : -1;
    else if ((uint32)component == 0x80000000u && divisor == 0xFFFFFFFFu) quotient = component;
    else quotient = component / (sint32)divisor;
    w_u32(0x800A6850u + i * 4u, (uint32)quotient);
  }
  quotient = (sint32)magnitude / 10;
  w_u32(0x800FFB94u, (uint32)quotient);
  return (uint32)quotient;
}

uint32 xport_draft_host_sub_8003AA20_p1(const void *position)
{
  uint32 copied[3];
  memcpy(copied, position, sizeof(copied));
  return draft_3AA20_position(copied);
}

uint32 sub_8003AA20(uint32 position)
{
  uint32 copied[3];
  unsigned i;
  for (i = 0; i < 3; ++i) copied[i] = r_u32(position + i * 4u);
  return draft_3AA20_position(copied);
}


/* TODO Missing call adapter indirect */
/* TODO Missing call adapter sub_80010530 */
/* TODO Missing call adapter sub_80011860 */
/* TODO Missing call adapter sub_80014BFC */
/* TODO Missing call adapter sub_800151FC */
/* TODO Missing call adapter sub_800154E0 */
/* TODO Missing call adapter sub_8001779C */
/* TODO Missing call adapter sub_80067808 */
/* TODO Missing call adapter sub_80088B28 */
/* TODO Missing host buffer adapter xport_draft_host_sub_80015614_p34 */
/* TODO Missing host buffer adapter xport_draft_host_sub_80017364_p1234 */
/* TODO Missing host buffer adapter xport_draft_host_sub_80067808_p1 */
/* TODO Missing host buffer adapter xport_draft_host_sub_80067808_p2 */
/* TODO Postincrement memory expressions may require ordering refinement */
/* TODO Resolve original data label 0x800A031Cu */
/* TODO Resolve original data label 0x800A0328u */
/* TODO Resolve original data label aPrsnT */
/* TODO Resolve original data label & 0x800A0310u */
uint32 sub_80011BA0(void)
{
  uint32 v0;
  sint32 v1;
  sint32 v2;
  sint32 v3;
  sint32 v4;
  uint32 v5;
  sint32 v6;
  sint32 v7;
  uint32 v8;
  uint32 v9;
  sint32 v10;
  sint32 v11;
  uint32 v12;
  sint32 v13;
  sint32 v14;
  uint32 v15;
  sint32 v16;
  sint32 v17;
  sint32 v18;
  sint32 v19;
  uint32 v20;
  sint32 v21;
  sint32 v22;
  sint32 v23;
  sint32 v24;
  uint32 v25;
  sint32 v26;
  sint32 v27;
  sint32 v28;
  uint32 v29;
  uint32 v30;
  uint32 v31;
  uint32 v32;
  sint32 v33;
  sint32 v34;
  uint32 v35;
  uint32 v36;
  uint32 v37;
  sint32 v38;
  sint32 v39;
  uint32 v40;
  uint32 v41;
  sint32 v42;
  sint32 v43;
  sint32 v44;
  sint32 v45;
  sint32 v46;
  uint32 v47;
  uint32 v48;
  sint32 v49;
  sint32 v50;
  sint32 v51;
  sint32 v52;
  sint32 v53;
  sint32 v54;
  sint32 v55;
  uint32 v56;
  uint32 v57;
  sint32 v58;
  sint32 v59;
  uint32 v60;
  uint32 v61;
  sint32 v62;
  sint32 v63;
  sint32 v64;
  sint32 v65;
  uint32 v66;
  uint32 v67;
  sint32 v68;
  sint32 v69;
  sint32 v70;
  sint32 v71;
  uint32 v72;
  uint32 v73;
  sint32 v74;
  sint32 v75;
  sint32 v76;
  sint32 v77;
  sint32 v78;
  uint32 v79;
  sint32 v80;
  sint32 v81;
  uint32 v82;
  sint32 v83;
  sint32 v84;
  uint32 v85;
  uint32 v86;
  uint32 v87;
  uint32 v88;
  sint32 v89;
  sint32 v90;
  sint32 v91;
  sint32 v92;
  sint32 v94;
  sint32 v95;
  sint32 v96;
  uint32 v97;
  char v98[16];
  sint32 v99;
  sint32 v100;
  sint32 v101;
  sint32 v102;
  sint32 v103;
  char v104[4];
  sint32 v105;
  sint32 v106;
  uint32 v107;
  sint32 v108;
  sint32 v109;
  sint32 v110;
  uint32 v111;
  sint32 v112;
  uint32 v113;
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
  sint32 v124;
  sint32 v125;
  sint32 v126;
  sint32 v127;
  sint32 v128;
  sint32 v129;
  v105 = 0;
  v106 = 0;
  sub_80068160();
  sub_800681B8();
  sub_8001024C(0,((sint32)(0x800A0310u)),240);
  w_u32(0x800FF660u,((sint32)(0x800C6310u)));
  sub_80068160();
  sub_8002FF54();
  sub_8001A7BC(256);
  sub_8001A7B0(0);
  sub_8001A7D4(128,128,128,0);
  v94 = 256;
  sub_8001AA28(256,120,r_u32(0x800A54ECu),0,0,256);
  w_u8(0x800A72B8u,0);
  w_u8(0x800C6328u,0);
  sub_800681B8();
  DrawSync(0);
  w_u8(0x800A72B8u,1);
  w_u8(0x800C6328u,1);
  PutDispEnv((DISPENV *)psx_addr(0x800A72FCu,sizeof(DISPENV)));
  v0 = ((uint32)(0x800A5374u));
  v1 = 0;
  if (r_u8(r_u32(0x800A5374u)))
  {
    do
    {
      v0 += (8)*4u;
      ++v1;
    }
    while (r_u8(r_u32(v0)));
  }
  if (((w_u32(0x800FEFF0u,(r_u32(0x800FEFF0u)+1u)),r_u32(0x800FEFF0u)) >= ((sint32)(v1))))
    w_u32(0x800FEFF0u,0);
  v2 = 144;
  sub_80011B34();
  v3 = 0;
  sub_80070748();
  w_u32(0x800FF814u,0);
  v4 = (r_u8(0x800EC1D8u) == 0);
  do
  {
    sub_80070748();
    if (r_u32(0x800FF03Cu))
    {
      (w_u32(0x800FF03Cu,(r_u32(0x800FF03Cu)-1u)),r_u32(0x800FF03Cu));
    }
    else
    {
      if (!r_u8(0x800EC1D8u))
        v4 = 1;
      if ((v4 && r_u8(0x800EC1D8u)))
      {
        sub_80069DF0(23,0x2000,0);
        goto LABEL_22;
      }
      if (v2)
      {
        PutDispEnv((DISPENV *)psx_addr(0x800C636Cu,sizeof(DISPENV)));
        if (!(--v2))
          v3 = 24;
      }
      else
      {
        PutDispEnv((DISPENV *)psx_addr(0x800A72FCu,sizeof(DISPENV)));
      }
      if (v3)
      {
        if (!(--v3))
          v2 = 144;
      }
    }
    sub_8006B2A8();
    sub_8006F500();
    nullsub_16();
    sub_800664E4(1);
  }
  while ((r_u32(0x800FF814u) < 1801));
  v105 = 3;
  v106 = 1;
  LABEL_22:
  w_u8(0x800EC1D9u,0);

  w_u8(0x800EC129u,0);
  sub_80015EC8();
  sub_8006F7D8();
  sub_8002E148(951706923,64,64,127);
  sub_8007D76C(10,10240,0x2000);
  v5 = ((uint32)(sub_8002FED8(28)));
  v107 = v5;
  if (v5)
    v107 = sub_80011904(v5,(0x800A537Cu+(((uint32)(8) * (uint32)(r_u32(0x800FEFF0u))))*4u),r_u32((0x800A538Cu+(((uint32)(8) * (uint32)(r_u32(0x800FEFF0u))))*4u)),r_u32((0x800A5390u+(((uint32)(8) * (uint32)(r_u32(0x800FEFF0u))))*4u)));
  v6 = sub_8002FED8(444);
  if (v6)
  {
    v94 = 256;
    v95 = 26;
    v6 = sub_80015228(v6,256,0,0,0x140u,0x100u,0x1Au);
  }
  sub_80015474(v6,r_u32((0x800A554Cu+(0)*4u)));
  if ((!r_u32(0x800A53D4u) || sub_80067724(0x800A5460u,r_u32(0x800A5258u))))
    sub_8001551C(v6,r_u32((0x800A554Cu+(0)*4u)));
  sub_80015474(v6,r_u32((0x800A55D0u+(0)*4u)));
  if (r_u32(0x800FF334u))
    sub_80015474(v6,0x800A031Cu);
  sub_80015474(v6,r_u32((0x800A55D4u+(0)*4u)));
  sub_80015474(v6,r_u32((0x800A55D8u+(0)*4u)));
  if (r_u32(0x800FF330u))
    sub_80015474(v6,r_u32((0x800A557Cu+(0)*4u)));
  sub_800152F8(v6);
  v7 = sub_8002FED8(444);
  v108 = v7;
  if (v7)
  {
    v94 = 192;
    v95 = 26;
    v108 = sub_80015228(v7,256,0,0,0x100u,0xC0u,0x1Au);
  }
  w_u32(((uint32)(((uint32)(v108) + (uint32)(20)))),18);
  v8 = ((uint32)(0x800A5254u));
  if (r_u8(r_u32(0x800A5254u)))
  {
    v9 = 0x800A5254u;
    do
    {
      v8 += (6)*4u;
      sub_80015474(v108,r_u32(v9));
      v9 = v8;
    }
    while (r_u8(r_u32(v8)));
  }
  sub_800152F8(v108);
  v10 = sub_8002FED8(444);
  v109 = v10;
  if (v10)
  {
    v94 = 256;
    v95 = 26;
    v109 = sub_80015228(v10,256,63,0,0x140u,0x100u,0x1Au);
  }
  sub_80015474(v109,r_u32((0x800A55DCu+(0)*4u)));
  sub_80015474(v109,r_u32((0x800A55E0u+(0)*4u)));
  sub_80015474(v109,r_u32((0x800A55E4u+(0)*4u)));
  sub_80015474(v109,r_u32((0x800A55E8u+(0)*4u)));
  sub_80015474(v109,r_u32((0x800A55ECu+(0)*4u)));
  v11 = sub_8002FED8(444);
  v110 = v11;
  if (v11)
  {
    v94 = 256;
    v95 = 26;
    v110 = sub_80015228(v11,256,42,0,0x140u,0x100u,0x1Au);
  }
  sub_80015474(v110,r_u32((0x800A55F0u+(0)*4u)));
  sub_80015474(v110,r_u32((0x800A55F4u+(0)*4u)));
  sub_80015474(v110,r_u32((0x800A55F8u+(0)*4u)));
  sub_80015474(v110,r_u32((0x800A55FCu+(0)*4u)));
  sub_80015474(v110,r_u32((0x800A5600u+(0)*4u)));
  sub_80015474(v110,r_u32((0x800A55A4u+(0)*4u)));
  v12 = ((uint32)(sub_8002FED8(444)));
  v111 = v12;
  if (v12)
  {
    v94 = 256;
    v95 = 26;
    v111 = ((uint32)(sub_80015228(v12,256,55,0,0x140u,0x100u,0x1Au)));
  }
  sub_80015474(v111,r_u32((0x800A5648u+(0)*4u)));
  sub_80015474(v111,r_u32((0x800A564Cu+(0)*4u)));
  sub_80015474(v111,r_u32((0x800A5650u+(0)*4u)));
  sub_80015474(v111,r_u32((0x800A55A4u+(0)*4u)));
  w_u8((v111+(7)*1u),1);
  v13 = sub_8002FED8(444);
  v112 = v13;
  if (v13)
  {
    v94 = 256;
    v95 = 26;
    v112 = sub_80015228(v13,256,85,0,0x140u,0x100u,0x1Au);
  }
  sub_80015474(v112,r_u32((0x800A55A8u+(0)*4u)));
  sub_80015474(v112,r_u32((0x800A55ACu+(0)*4u)));
  sub_80015474(v112,r_u32((0x800A55B0u+(0)*4u)));
  sub_80015474(v112,r_u32((0x800A55A4u+(0)*4u)));
  v14 = sub_8002FED8(444);
  v15 = ((uint32)(v14));
  if (v14)
  {
    v94 = 192;
    v95 = 26;
    v15 = ((uint32)(sub_80015228(v14,30,25,1,0xC0u,0xC0u,0x1Au)));
  }
  sub_80015474(v15,r_u32((0x800A5654u+(0)*4u)));
  sub_80015474(v15,r_u32((0x800A5658u+(0)*4u)));
  sub_80015474(v15,r_u32((0x800A565Cu+(0)*4u)));
  sub_80015474(v15,r_u32((0x800A5730u+(0)*4u)));
  sub_80015474(v15,r_u32((0x800A5608u+(0)*4u)));
  sub_80015474(v15,r_u32((0x800A5604u+(0)*4u)));
  sub_80015590(v15,r_u32((0x800A5608u+(0)*4u)),10);
  sub_8001551C(v15,r_u32((0x800A5608u+(0)*4u)));
  sub_8001551C(v15,r_u32((0x800A5604u+(0)*4u)));
  sub_80015474(v15,r_u32((0x800A55FCu+(0)*4u)));
  sub_80015474(v15,r_u32((0x800A55A4u+(0)*4u)));
  w_u32((v15+(5)*4u),22);
  v16 = sub_8002FED8(444);
  v17 = v16;
  if (v16)
  {
    v94 = 256;
    v95 = 26;
    v17 = sub_80015228(v16,256,77,0,0x140u,0x100u,0x1Au);
  }
  v113 = 0;
  v114 = 0;
  v118 = 0;
  v119 = 0;
  v18 = 0;
  sub_80015474(v17,r_u32((0x800A560Cu+(0)*4u)));
  sub_80015474(v17,r_u32((0x800A5610u+(0)*4u)));
  sub_80015474(v17,r_u32((0x800A5614u+(0)*4u)));
  sub_80015474(v17,r_u32((0x800A5618u+(0)*4u)));
  w_u16(((uint32)(((uint32)(v17) + (uint32)(118)))),192);
  w_u16(((uint32)(((uint32)(v17) + (uint32)(116)))),192);
  w_u8(((uint32)(((uint32)(v17) + (uint32)(7)))),1);
  sub_80016014(0x800A54CCu);
  w_u32(0x800FF814u,0);
  v128 = r_u32(0x800FF64Cu);
  while (!v106)
  {
    nullsub_16();
    sub_80068160();
    sub_8002FF54();
    v129 = r_u32(0x800FF64Cu);
    if (v118)
    {
      if (!(--v118))
      {
        w_u16(0x800A7304u,v116);
        w_u16(0x800C6374u,v116);
        w_u16(0x800A7306u,v117);
        w_u16(0x800C6376u,v117);
      }
    }
    sub_8002E2E8();
    sub_8002E2E8();
    sub_8001A8E8();
    sub_800858FC(0x800ED548u,0x800ED550u);
    sub_800878AC(0x800ED550u,0x800ED520u);
    sub_8007E63C(0x800ED51Cu,0x800A67B8u,((uint32)(r_u32(0x800FF660u)) + (uint32)(112)));
    sub_8007EBDC(r_u32(0x800FF794u));
    sub_8007EBC4();
    sub_8001A7BC(256);
    sub_8001A7B0(0);
    switch (v18)
    {
      case 0:
        sub_800156E0(v6);
        sub_8001A7BC(256);
        if (r_u32(0x800FF348u))
      {
        sub_8001A7D4(149,20,20,0);
        v19 = 20;
        v20 = 0x800A0328u;
      }
      else
      {
        sub_8001A7D4(149,20,20,0);
        v19 = 28;
        v20 = r_u32((0x800A54E4u+(0)*4u));
      }
        v94 = 256;
        sub_8001AA28(256,v19,v20,0,0u,256u);
        sub_80016800();
        break;

      case 1:
        sub_800156E0(v108);
        break;

      case 3:
        v21 = v109;
        goto LABEL_63;

      case 4:
        v21 = v110;
        LABEL_63:
      sub_800156E0(v21);

        sub_800167D8();
        break;

      case 5:
        sub_800151FC();
        sub_8001AA28(256,80,r_u32((0x800A5620u+(0)*4u)),0,0u,256u);
        sub_8001AA28(256,106,r_u32((0x800A5624u+(0)*4u)),0,0u,256u);
        sub_8001AA28(256,132,r_u32((0x800A5628u+(0)*4u)),0,0u,256u);
        sub_8001A7B0(1);
        sub_80011594();
        sub_8001AA28(104,210,r_u32(0x800A5588u),0,0u,256u);
        sub_8001AA28(328,210,r_u32((0x800A5584u+(0)*4u)),0,0u,256u);
        v22 = sub_8006D028(((uint32)(r_u32(0x800FF01Cu)) + (uint32)(8)));
        if (v22)
        sub_8006D0D4(76,211,v22,((uint32)(r_u32(0x800FF01Cu)) + (uint32)(8)));
        v23 = sub_8006D028(r_u32(0x800FF01Cu));
        if (v23)
        sub_8006D0D4(300,211,v23,r_u32(0x800FF01Cu));
        sub_8006D1C0(0,0,512,5,0u,0u,255u,0u);
        sub_8006D1C0(0,235,512,5,0u,0u,255u,0u);
        sub_8006D1C0(0,0,7,240,0u,0u,255u,0u);
        v94 = 0;
        v95 = 255;
        v96 = 0;
        sub_8006D1C0(505,0,7,240,0u,0u,255u,0u);
        break;

      case 6:
        sub_800167D8();
        sub_800156E0(v111);
        break;

      case 7:
        v24 = 0;
        sub_800166AC();
        sub_8001A7B0(1);
        sub_8001A7BC(192);
        if ((sub_800155D4(v112,r_u32((0x800A55A8u+(0)*4u))) || sub_800155D4(v112,r_u32((0x800A55B0u+(0)*4u)))))
        v24 = 1;
        if (v24)
      {
        v25 = r_u32((0x800A55B4u+(0)*4u));
      }
      else
        if (sub_800155D4(v112,r_u32((0x800A55ACu+(0)*4u))))
      {
        v25 = r_u32(0x800A55B8u);
      }
      else
      {
        v25 = r_u32((0x800A5578u+(0)*4u));
      }
        v94 = 256;
        sub_8001AA28(348,210,v25,0,0u,256u);
        sub_8001A7B0(0);
        v26 = sub_8006D028(r_u32(0x800FF01Cu));
        if (v26)
        sub_8006D0D4(320,211,v26,r_u32(0x800FF01Cu));
        sub_800115C0(v112);
        sub_800156E0(v112);
        break;

      case 8:
        sub_800156E0(v15);
        v27 = 0;
        if ((((sub_800155D4(v15,r_u32((0x800A5654u+(0)*4u))) || sub_800155D4(v15,r_u32((0x800A5658u+(0)*4u)))) || sub_800155D4(v15,r_u32((0x800A565Cu+(0)*4u)))) || sub_800155D4(v15,r_u32((0x800A5730u+(0)*4u)))))
      {
        v27 = 1;
      }
        if (v27)
      {
        sub_8001A7B0(1);
        sub_8001A7BC(256);
        sub_80011594();
        sub_8001AA28(211,210,r_u32((0x800A5574u+(0)*4u)),0,0u,256u);
        v28 = sub_8006D028(((uint32)(r_u32(0x800FF01Cu)) + (uint32)(8)));
        if (v28)
          sub_8006D0D4(183,211,v28,((uint32)(r_u32(0x800FF01Cu)) + (uint32)(8)));
      }
      else
      {
        sub_800167D8();
      }
        sub_8001A7B0(1);
        sub_800151D0();
        sub_8001A7BC(192);
        if ((r_u32(0x800EC264u) == 65))
      {
        xport_draft_host_sub_80015614_p34(v15,r_u32((0x800A5654u+(0)*4u)),&v99,&v100);
        v29 = sub_800117D8(0x800EC47Au);
        sub_8001AA28(((uint32)(v99) + (uint32)(315)),v100,v29,0,0u,256u);
        xport_draft_host_sub_80015614_p34(v15,r_u32((0x800A5658u+(0)*4u)),&v99,&v100);
        v30 = sub_800117D8(r_u16(0x800EC474u));
        sub_8001AA28(((uint32)(v99) + (uint32)(315)),v100,v30,0,0u,256u);
        xport_draft_host_sub_80015614_p34(v15,r_u32((0x800A565Cu+(0)*4u)),&v99,&v100);
        v31 = sub_800117D8(0x800EC476u);
        sub_8001AA28(((uint32)(v99) + (uint32)(315)),v100,v31,0,0u,256u);
        xport_draft_host_sub_80015614_p34(v15,r_u32((0x800A5730u+(0)*4u)),&v99,&v100);
        v32 = ((uint32)(sub_800117D8(0x800EC478u)));
      }
      else
      {
        xport_draft_host_sub_80015614_p34(v15,r_u32((0x800A5654u+(0)*4u)),&v99,&v100);
        v35 = sub_800117D8(0x800EC49Au);
        sub_8001AA28(((uint32)(v99) + (uint32)(315)),v100,v35,0,0u,256u);
        xport_draft_host_sub_80015614_p34(v15,r_u32((0x800A5658u+(0)*4u)),&v99,&v100);
        v36 = sub_800117D8(0x800EC494u);
        sub_8001AA28(((uint32)(v99) + (uint32)(315)),v100,v36,0,0u,256u);
        xport_draft_host_sub_80015614_p34(v15,r_u32((0x800A565Cu+(0)*4u)),&v99,&v100);
        v37 = sub_800117D8(0x800EC496u);
        sub_8001AA28(((uint32)(v99) + (uint32)(315)),v100,v37,0,0u,256u);
        xport_draft_host_sub_80015614_p34(v15,r_u32((0x800A5730u+(0)*4u)),&v99,&v100);
        v32 = ((uint32)(sub_800117D8(0x800EC498u)));
      }
        v33 = v100;
        v94 = 256;
        v34 = ((uint32)(v99) + (uint32)(315));
        goto LABEL_104;

      case 9:
        sub_800151FC();
        sub_8001AA28(256,80,r_u32((0x800A562Cu+(0)*4u)),0,0u,256u);
        sub_8001AA28(256,106,r_u32((0x800A5630u+(0)*4u)),0,0u,256u);
        sub_8001AA28(256,132,r_u32((0x800A5634u+(0)*4u)),0,0u,256u);
        v34 = 256;
        v32 = r_u32((0x800A5638u+(0)*4u));
        v33 = 158;
        goto LABEL_103;

      case 10:
        sub_800151FC();
        sub_8001AA28(256,90,r_u32((0x800A563Cu+(0)*4u)),0,0u,256u);
        sub_8001AA28(256,116,r_u32((0x800A5640u+(0)*4u)),0,0u,256u);
        v34 = 256;
        v32 = r_u32((0x800A5644u+(0)*4u));
        v33 = 142;
        goto LABEL_103;

      case 11:
        sub_80011594();
        v94 = 256;
        sub_8001AA28(256,34,r_u32((0x800A5608u+(0)*4u)),0,0u,256u);
        sub_800167D8();
        sub_800156E0(v17);
        break;

      case 12:
        sub_800167D8();
        sub_80011594();
        v94 = 256;
        sub_8001AA28(256,80,r_u32((0x800A55A0u+(0)*4u)),0,0u,256u);
        v38 = r_u32(0x800FF024u);
        w_u32(((uint32)(((uint32)(r_u32(0x800FF024u)) + (uint32)(16)))),131);
        sub_800156E0(v38);
        break;

      case 13:

      case 14:
        sub_800151FC();
        sub_8001AA28(256,90,r_u32((0x800A56B4u+(0)*4u)),0,0u,256u);
        sub_800151D0();
        sub_8001AA28(256,140,r_u32((0x800A56ACu+(0)*4u)),0,0u,256u);
        v34 = 256;
        v32 = r_u32((0x800A56B0u+(0)*4u));
        v33 = 166;
        goto LABEL_103;

      case 15:
        sub_800151D0();
        sub_8001AA28(256,90,r_u32((0x800A5710u+(0)*4u)),0,0u,256u);
        sub_8001AA28(256,116,r_u32((0x800A5714u+(0)*4u)),0,0u,256u);
        sub_8001AA28(256,142,r_u32((0x800A5718u+(0)*4u)),0,0u,256u);
        v34 = 256;
        v33 = 168;
        v32 = r_u32((0x800A571Cu+(0)*4u));
        goto LABEL_103;

      case 16:
        sub_800151D0();
        sub_8001AA28(256,90,r_u32((0x800A5674u+(0)*4u)),0,0u,256u);
        sub_8001AA28(256,116,r_u32((0x800A5678u+(0)*4u)),0,0u,256u);
        sub_8001AA28(256,142,r_u32((0x800A567Cu+(0)*4u)),0,0u,256u);
        v34 = 256;
        v32 = r_u32(0x800A5680u);
        v33 = 168;
        goto LABEL_103;

      case 17:
        sub_800167D8();
        sub_800151D0();
        v94 = 256;
        sub_8001AA28(256,25,r_u32((0x800A55D4u+(0)*4u)),0,0u,256u);
        sub_800156E0(v113);
        break;

      case 18:
        sub_800151D0();
        sub_8001AA28(256,90,r_u32((0x800A5664u+(0)*4u)),0,0u,256u);
        sub_8001AA28(256,116,r_u32((0x800A5668u+(0)*4u)),0,0u,256u);
        sub_8001AA28(256,142,r_u32((0x800A566Cu+(0)*4u)),0,0u,256u);
        v34 = 256;
        v32 = r_u32((0x800A5670u+(0)*4u));
        v33 = 168;
        LABEL_103:
      v94 = 256;

        LABEL_104:
      sub_8001AA28(v34,v33,v32,0,0u,256u);

        break;

      default:
        break;

    }

    if ((((w_u32(0x800FF2F0u,(r_u32(0x800FF2F0u)+1u)),r_u32(0x800FF2F0u)) & 1) != 0))
      sub_80010AC8(0,0);
    sub_80070748();
    if (v18)
      v128 = r_u32(0x800FF64Cu);
    xport_draft_host_sub_80017364_p1234(&v101,&v102,&v103,v104);
    v39 = -1;
    if (v101)
      v39 = 23;
    if (v102)
      v39 = 19;
    switch (v18)
    {
      case 0:
        sub_80015BA4(v6);
        if (v101)
      {
        if (sub_800155D4(v6,r_u32((0x800A554Cu+(0)*4u))))
        {
          v40 = 0x800A5460u;
          xport_draft_host_sub_80067808_p2(0x800A5460u,v98);
          v41 = ((uint32)(aPrsnT));
          do
          {
            v42 = ((sint32)(r_u32((v41+(1)*4u))));
            v43 = ((sint32)(r_u32((v41+(2)*4u))));
            v44 = ((sint32)(r_u32((v41+(3)*4u))));
            w_u32(((uint32)(v40)),((sint32)(r_u32(v41))));
            w_u32((((uint32)(v40))+(1)*4u),v42);
            w_u32((((uint32)(v40))+(2)*4u),v43);
            w_u32((((uint32)(v40))+(3)*4u),v44);
            v41 += (4)*4u;
            v40 += (16)*1u;
          }
          while ((v41 != 0x800A0578u));
          v45 = ((sint32)(r_u32((v41+(1)*4u))));
          v46 = ((sint32)(r_u32((v41+(2)*4u))));
          w_u32(((uint32)(v40)),((sint32)(r_u32(v41))));
          w_u32((((uint32)(v40))+(1)*4u),v45);
          w_u32((((uint32)(v40))+(2)*4u),v46);
          xport_draft_host_sub_80067808_p1(v98,0x800A5460u);
          v106 = 1;
          w_u8(0x800A5469u,0);
          w_u32(0x800FF384u,r_u32(0x800A54CCu));
        }
        if (sub_800155D4(v6,r_u32((0x800A557Cu+(0)*4u))))
          v18 = 1;
        if (sub_800155D4(v6,r_u32((0x800A55D0u+(0)*4u))))
        {
          v39 = 23;
          v47 = 0x800A5460u;
          v48 = ((uint32)(aPrsnT));
          do
          {
            v49 = ((sint32)(r_u32((v48+(1)*4u))));
            v50 = ((sint32)(r_u32((v48+(2)*4u))));
            v51 = ((sint32)(r_u32((v48+(3)*4u))));
            w_u32(((uint32)(v47)),((sint32)(r_u32(v48))));
            w_u32((((uint32)(v47))+(1)*4u),v49);
            w_u32((((uint32)(v47))+(2)*4u),v50);
            w_u32((((uint32)(v47))+(3)*4u),v51);
            v48 += (4)*4u;
            v47 += (16)*1u;
          }
          while ((v48 != 0x800A0578u));
          v52 = ((sint32)(r_u32((v48+(1)*4u))));
          v53 = ((sint32)(r_u32((v48+(2)*4u))));
          w_u32(((uint32)(v47)),((sint32)(r_u32(v48))));
          w_u32((((uint32)(v47))+(1)*4u),v52);
          w_u32((((uint32)(v47))+(2)*4u),v53);
          v106 = 1;
        }
        if (sub_800155D4(v6,0x800A031Cu))
        {
          v105 = 2;
          v106 = 1;
        }
        if (sub_800155D4(v6,r_u32((0x800A55D4u+(0)*4u))))
          v18 = 13;
        if (sub_800155D4(v6,r_u32((0x800A55D8u+(0)*4u))))
          v18 = 3;
      }
        if (v102)
        v39 = -1;
        if (!r_u32(0x800FF814u))
      {
        v128 = r_u32(0x800FF64Cu);
        goto LABEL_373;
      }
        v54 = v39;
        if ((((uint32)(((uint32)(r_u32(0x800FF64Cu)) - (uint32)(v128)))) >= 0x709))
      {
        v105 = 3;
        v106 = 1;
      }
        goto LABEL_374;

      case 1:
        sub_80015BA4(v108);
        if (!v101)
        goto LABEL_145;
        v55 = r_u32(((uint32)(((uint32)(((uint32)(v108) + (uint32)(((uint32)(28) * (uint32)(r_u8(((uint32)(((uint32)(v108) + (uint32)(6)))))))))) + (uint32)(24)))));
        w_u8(0x800A5460u,0);
        if (!(r_u8(r_u32(0x800A5254u))))
        goto LABEL_143;
        v56 = 0x800A5254u;
        do
      {
        if (sub_80067724(v55,r_u32(v56)))
        {
          sub_80067808(r_u32(v56+4),0x800A5460u);
          v39 = 23;
          goto LABEL_144;
        }
        v56 += (6)*4u;
      }
      while (r_u8(((uint32)(r_u32(v56)))));
        LABEL_143:
      v39 = 23;

        LABEL_144:
      w_u8(0x800A5469u,0);

        v106 = 1;
        LABEL_145:
      v54 = v39;

        if (v102)
        v18 = 0;
        goto LABEL_374;

      case 3:
        sub_80015BA4(v109);
        if (v101)
      {
        if (sub_800155D4(v109,r_u32((0x800A55DCu+(0)*4u))))
        {
          v57 = v111;
          w_u8((v111+(8)*1u),r_u32(0x800FF384u));
          v18 = 6;
          sub_80015374(v57,((unsigned char)(r_u32(0x800FF384u))));
        }
        if (sub_800155D4(v109,r_u32((0x800A55E0u+(0)*4u))))
        {
          v39 = -1;
          sub_80069DF0(23,0x2000,0);
          v18 = 7;
          v119 = 0;
          sub_80015F48(r_u32((0x800A525Cu+(0)*4u)));
        }
        if (sub_800155D4(v109,r_u32((0x800A55E4u+(0)*4u))))
          v18 = 8;
        if (sub_800155D4(v109,r_u32((0x800A55E8u+(0)*4u))))
          v18 = 4;
        if (sub_800155D4(v109,r_u32((0x800A55ECu+(0)*4u))))
        {
          v39 = 19;
          v18 = 0;
        }
      }
        v54 = v39;
        if (v102)
        v18 = 0;
        goto LABEL_374;

      case 4:
        sub_80015BA4(v110);
        if (v101)
      {
        if (sub_800155D4(v110,r_u32((0x800A55F0u+(0)*4u))))
        {
          sub_80069DF0(v39,0x2000,0);
          v39 = -1;
          (abort(),0u);
        }
        if (sub_800155D4(v110,r_u32((0x800A55F4u+(0)*4u))))
        {
          sub_80069DF0(v39,0x2000,0);
          v39 = -1;
          sub_80011B04();
          sub_8002E814(33);
          sub_80011B34();
          sub_8006F7D8();
        }
        if (sub_800155D4(v110,r_u32((0x800A55F8u+(0)*4u))))
        {
          sub_80011B04();
          sub_8002E814(28);
          sub_80011B34();
          sub_8006F7D8();
        }
        if (sub_800155D4(v110,r_u32((0x800A55FCu+(0)*4u))))
        {
          v18 = 12;
          sub_800153D8(r_u32(0x800FF024u));
        }
        if (sub_800155D4(v110,r_u32((0x800A5600u+(0)*4u))))
        {
          v18 = 5;
          v116 = r_u16(0x800A7304u);
          v117 = r_u16(0x800A7306u);
        }
        if (sub_800155D4(v110,r_u32((0x800A55A4u+(0)*4u))))
        {
          v39 = 19;
          v18 = 3;
        }
      }
        goto LABEL_296;

      case 5:
        if ((r_u8(0x800EC188u) && (r_u16(0x800A7304u) < 32)))
      {
        (w_u16(0x800A7304u,(r_u16(0x800A7304u)+1u)),r_u16(0x800A7304u));
        (w_u16(0x800C6374u,(r_u16(0x800C6374u)+1u)),r_u16(0x800C6374u));
      }
        if ((r_u8(0x800EC178u) && (r_u16(0x800A7304u) > 0)))
      {
        (w_u16(0x800A7304u,(r_u16(0x800A7304u)-1u)),r_u16(0x800A7304u));
        (w_u16(0x800C6374u,(r_u16(0x800C6374u)-1u)),r_u16(0x800C6374u));
      }
        if ((r_u8(0x800EC198u) && (r_u16(0x800A7306u) > 0)))
      {
        (w_u16(0x800A7306u,(r_u16(0x800A7306u)-1u)),r_u16(0x800A7306u));
        (w_u16(0x800C6376u,(r_u16(0x800C6376u)-1u)),r_u16(0x800C6376u));
      }
        if ((r_u8(0x800EC1A8u) && (r_u16(0x800A7306u) < 32)))
      {
        (w_u16(0x800A7306u,(r_u16(0x800A7306u)+1u)),r_u16(0x800A7306u));
        (w_u16(0x800C6376u,(r_u16(0x800C6376u)+1u)),r_u16(0x800C6376u));
      }
        if (v101)
        v18 = 4;
        if (v102)
      {
        v118 = 2;
        v18 = 4;
      }
        goto LABEL_373;

      case 6:
        sub_80015BA4(v111);
        if (v101)
      {
        if (sub_800155D4(v111,r_u32((0x800A5648u+(0)*4u))))
        {
          v39 = 4;
          w_u32(0x800FF384u,0);
        }
        if (sub_800155D4(v111,r_u32((0x800A564Cu+(0)*4u))))
        {
          v39 = 6;
          w_u32(0x800FF384u,1);
        }
        if (sub_800155D4(v111,r_u32((0x800A5650u+(0)*4u))))
        {
          v39 = 8;
          w_u32(0x800FF384u,2);
        }
        if (sub_800155D4(v111,r_u32((0x800A55A4u+(0)*4u))))
        {
          v39 = 19;
          v18 = 3;
        }
      }
        w_u8((v111+(8)*1u),r_u32(0x800FF384u));
        LABEL_296:
      v54 = v39;

        if (v102)
        v18 = 3;
        goto LABEL_374;

      case 7:
        sub_80015BA4(v112);
        v78 = sub_8001173C();
        if ((sub_800155D4(v112,r_u32((0x800A55A8u+(0)*4u))) && (v78 || v101)))
      {
        sub_80069DF0(16,0x2000,0);
        w_u16(0x800ECC7Au,(r_u16(0x800ECC7Au)+(((uint32)(((uint16)(v78))) << (uint32)(10)))));
        if (((r_u16(0x800ECC7Au) & 0x8000) != 0))
          w_u16(0x800ECC7Au,0);
        if ((r_u16(0x800ECC7Au) >= 0x4000))
          w_u16(0x800ECC7Au,0x3FFF);
      }
        if (sub_800155D4(v112,r_u32((0x800A55ACu+(0)*4u))))
      {
        w_u16(0x800ECC78u,(r_u16(0x800ECC78u)+(((uint32)(8) * (uint32)(v78)))));
        if (((r_u16(0x800ECC78u) & 0x8000) != 0))
          w_u16(0x800ECC78u,0);
        if ((r_u16(0x800ECC78u) >= 128))
          w_u16(0x800ECC78u,127);
        ((void)(0),sub_8006A428());
        if (v101)
        {
          v79 = (((uint32)(0x800A5254u))+(((uint32)(24) * (uint32)(++v119)))*1u);
          if (!(r_u8(r_u32((((uint32)(v79))+(2)*4u)))))
            v119 = 0;
          sub_80015F48(r_u32(((((uint32)(0x800A5254u))+(((uint32)(6) * (uint32)(v119)))*4u)+(2)*4u)));
          ((void)(0),sub_8006A428());
        }
      }
      else
      {
        sub_8006A3F0();
      }
        if (sub_800155D4(v112,r_u32((0x800A55B0u+(0)*4u))))
      {
        if (((v78 || v101) || !r_u32(0x800FF020u)))
        {
          v80 = sub_80066570(2);
          sub_8002FD2C(0,v80);
          w_u16(0x800ECC7Cu,(r_u16(0x800ECC7Cu)+(((uint32)(16) * (uint32)(v78)))));
          if (((r_u16(0x800ECC7Cu) & 0x8000) != 0))
            w_u16(0x800ECC7Cu,0);
          if ((r_u16(0x800ECC7Cu) >= 256))
            w_u16(0x800ECC7Cu,255);
          w_u32(0x800FF020u,1);
        }
      }
      else
      {
        w_u32(0x800FF020u,0);
      }
        if (((!sub_800155D4(v112,r_u32((0x800A55A4u+(0)*4u))) || !v101) && !v102))
        goto LABEL_329;
        v18 = 3;
        sub_80069A94();
        w_u32(0x800FF020u,0);
        v54 = 19;
        goto LABEL_374;

      case 8:
        if (((r_u32(0x800EC264u) == 65) || (PadGetState(0) != 2)))
        sub_8001551C(v15,r_u32((0x800A5604u+(0)*4u)));
      else
        sub_800154E0(v15,r_u32(0x800A5604u));
        if ((PadGetState(0) == 6))
        sub_800154E0(v15,r_u32(0x800A5608u));
      else
        sub_8001551C(v15,r_u32((0x800A5608u+(0)*4u)));
        sub_80015BA4(v15);
        v58 = 0;
        if (v101)
      {
        if ((((sub_800155D4(v15,r_u32((0x800A5654u+(0)*4u))) || sub_800155D4(v15,r_u32((0x800A5658u+(0)*4u)))) || sub_800155D4(v15,r_u32((0x800A565Cu+(0)*4u)))) || sub_800155D4(v15,r_u32((0x800A5730u+(0)*4u)))))
        {
          v58 = 1;
        }
        if (v58)
          v39 = -1;
      }
        v59 = (r_u8(0x800EC148u) != 0);
        if (r_u8(0x800EC168u))
        v59 |= 2u;
        if (r_u8(0x800EC138u))
        v59 |= 4u;
        v60 = ((uint32)(v59) - (uint32)(1));
        if (r_u8(0x800EC158u))
      {
        v59 |= 8u;
        v60 = ((uint32)(v59) - (uint32)(1));
      }
        if ((((v60 >= 2) && (v59 != 4)) && (v59 != 8)))
        goto LABEL_253;
        if ((r_u32(0x800EC264u) == 65))
      {
        if (sub_800155D4(v15,r_u32((0x800A5654u+(0)*4u))))
          sub_80011860(v59,0x800EC47Au,0x800EC474u,0x800EC476u,((sint32)(0x800EC478u)));
        if (sub_800155D4(v15,r_u32((0x800A5658u+(0)*4u))))
          sub_80011860(v59,0x800EC474u,0x800EC47Au,0x800EC476u,((sint32)(0x800EC478u)));
        if (sub_800155D4(v15,r_u32((0x800A565Cu+(0)*4u))))
          sub_80011860(v59,0x800EC476u,0x800EC47Au,0x800EC474u,((sint32)(0x800EC478u)));
        if (sub_800155D4(v15,r_u32((0x800A5730u+(0)*4u))))
        {
          v61 = 0x800EC478u;
          LABEL_252:
          sub_80011860(v59,v61,(v61+(1)*2u),(v61-(2)*2u),((sint32)((v61-(1)*2u))));

        }
      }
      else
      {
        if (sub_800155D4(v15,r_u32((0x800A5654u+(0)*4u))))
          sub_80011860(v59,0x800EC49Au,0x800EC494u,0x800EC496u,((sint32)(0x800EC498u)));
        if (sub_800155D4(v15,r_u32((0x800A5658u+(0)*4u))))
          sub_80011860(v59,0x800EC494u,0x800EC49Au,0x800EC496u,((sint32)(0x800EC498u)));
        if (sub_800155D4(v15,r_u32((0x800A565Cu+(0)*4u))))
          sub_80011860(v59,0x800EC496u,0x800EC49Au,0x800EC494u,((sint32)(0x800EC498u)));
        if (sub_800155D4(v15,r_u32((0x800A5730u+(0)*4u))))
        {
          v61 = 0x800EC498u;
          goto LABEL_252;
        }
      }
        LABEL_253:
      if (v101)
      {
        if (sub_800155D4(v15,r_u32((0x800A55A4u+(0)*4u))))
        {
          v39 = 19;
          v18 = 3;
        }
        if (sub_800155D4(v15,r_u32((0x800A55FCu+(0)*4u))))
        {
          w_u32(0x800FF2FCu,2);
          w_u16(0x800EC47Au,((unsigned char)(r_u8(0x800A0594u))));
          w_u16(0x800EC474u,((unsigned char)(r_u8(0x800A0595u))));
          w_u16(0x800EC476u,((unsigned char)(r_u8(0x800A0596u))));
          w_u16(0x800EC478u,((unsigned char)(r_u8(0x800A0597u))));
          w_u16(0x800EC49Au,((unsigned char)(r_u8(0x800A0598u))));
          w_u16(0x800EC494u,((unsigned char)(r_u8(0x800A0599u))));
          w_u16(0x800EC496u,((unsigned char)(r_u8(0x800A059Au))));
          w_u16(0x800EC498u,((unsigned char)(r_u8(0x800A059Bu))));
        }
        if (sub_800155D4(v15,r_u32((0x800A5604u+(0)*4u))))
        {
          v120 = r_u32(0x800FF7E0u);
          v62 = r_u32(0x800FF7F0u);
          v121 = r_u32(0x800FF7E4u);
          v63 = r_u32(0x800FF7F4u);
          v122 = r_u32(0x800FF7E8u);
          v64 = r_u32(0x800FF7F8u);
          v18 = 9;
          v123 = r_u32(0x800FF7ECu);
          v65 = r_u32(0x800FF7FCu);
          w_u32(0x800FF7E0u,255);
          w_u32(0x800FF7E4u,0);
          w_u32(0x800FF7E8u,255);
          w_u32(0x800FF7ECu,0);
          w_u32(0x800FF7F0u,255);
          w_u32(0x800FF7F4u,0);
          w_u32(0x800FF7F8u,255);
          w_u32(0x800FF7FCu,0);
          v124 = v62;
          v125 = v63;
          v126 = v64;
          v127 = v65;
        }
        if (sub_800155D4(v15,r_u32((0x800A5608u+(0)*4u))))
        {
          w_u8(((uint32)(((uint32)(v17) + (uint32)(8)))),r_u32(0x800FF2FCu));
          v18 = 11;
          sub_80015374(v17,((unsigned char)(r_u32(0x800FF2FCu))));
        }
      }

        if (v102)
        v18 = 3;
        /* MIPS 80013CD4..80013CF0 passes four signed halfwords */
        sub_8006FDFC(0x800EC0F8u,(sint16)r_u16(0x800EC474u),(sint16)r_u16(0x800EC476u),(sint16)r_u16(0x800EC478u),(sint16)r_u16(0x800EC47Au));
        v94 = 0x800EC494u;
        v95 = 0x800EC496u;
        v96 = 0x800EC498u;
        v97 = ((uint32)(0x800EC49Au));
        /* MIPS 80013D18..80013D2C passes zero then four signed halfwords */
        sub_8006FE14(0x800EC0F8u,3,2,1,0,(sint16)r_u16(0x800EC494u),(sint16)r_u16(0x800EC496u),(sint16)r_u16(0x800EC498u),(sint16)r_u16(0x800EC49Au));
        v54 = v39;
        goto LABEL_374;

      case 9:
        if ((((unsigned char)(r_u8(0x800EC25Du))) < r_u32(0x800FF7E0u)))
        w_u32(0x800FF7E0u,((unsigned char)(r_u8(0x800EC25Du))));
        if ((r_u32(0x800FF7E4u) < ((unsigned char)(r_u8(0x800EC25Du)))))
        w_u32(0x800FF7E4u,((unsigned char)(r_u8(0x800EC25Du))));
        if ((((unsigned char)(r_u8(0x800EC25Cu))) < r_u32(0x800FF7E8u)))
        w_u32(0x800FF7E8u,((unsigned char)(r_u8(0x800EC25Cu))));
        if ((r_u32(0x800FF7ECu) < ((unsigned char)(r_u8(0x800EC25Cu)))))
        w_u32(0x800FF7ECu,((unsigned char)(r_u8(0x800EC25Cu))));
        if ((((unsigned char)(r_u8(0x800EC25Fu))) < r_u32(0x800FF7F0u)))
        w_u32(0x800FF7F0u,((unsigned char)(r_u8(0x800EC25Fu))));
        if ((r_u32(0x800FF7F4u) < ((unsigned char)(r_u8(0x800EC25Fu)))))
        w_u32(0x800FF7F4u,((unsigned char)(r_u8(0x800EC25Fu))));
        if ((((unsigned char)(r_u8(0x800EC25Eu))) < r_u32(0x800FF7F8u)))
        w_u32(0x800FF7F8u,((unsigned char)(r_u8(0x800EC25Eu))));
        if ((r_u32(0x800FF7FCu) < ((unsigned char)(r_u8(0x800EC25Eu)))))
        w_u32(0x800FF7FCu,((unsigned char)(r_u8(0x800EC25Eu))));
        if (v101)
        v18 = 10;
        v54 = v39;
        if (v102)
      {
        w_u32(0x800FF7E0u,v120);
        w_u32(0x800FF7E4u,v121);
        w_u32(0x800FF7E8u,v122);
        w_u32(0x800FF7ECu,v123);
        w_u32(0x800FF7F0u,v124);
        w_u32(0x800FF7F4u,v125);
        w_u32(0x800FF7F8u,v126);
        w_u32(0x800FF7FCu,v127);
        v18 = 8;
      }
        goto LABEL_374;

      case 10:
        w_u32(0x800FF800u,((unsigned char)(r_u8(0x800EC25Du))));
        w_u32(0x800FF804u,((unsigned char)(r_u8(0x800EC25Cu))));
        w_u32(0x800FF808u,((unsigned char)(r_u8(0x800EC25Fu))));
        w_u32(0x800FF80Cu,((unsigned char)(r_u8(0x800EC25Eu))));
        v39 = 23;
        if (v101)
        v18 = 8;
      else
        LABEL_329:
      v39 = -1;

        goto LABEL_373;

      case 11:
        sub_80015BA4(v17);
        if (v101)
      {
        if (sub_800155D4(v17,r_u32((0x800A560Cu+(0)*4u))))
          w_u32(0x800FF2FCu,0);
        if (sub_800155D4(v17,r_u32((0x800A5610u+(0)*4u))))
          w_u32(0x800FF2FCu,1);
        if (sub_800155D4(v17,r_u32((0x800A5614u+(0)*4u))))
          w_u32(0x800FF2FCu,2);
        if (sub_800155D4(v17,r_u32((0x800A5618u+(0)*4u))))
        {
          v18 = 8;
          v39 = 19;
        }
      }
        w_u8(((uint32)(((uint32)(v17) + (uint32)(8)))),r_u32(0x800FF2FCu));
        v54 = v39;
        if (v102)
        v18 = 8;
        goto LABEL_374;

      case 12:
        sub_80015BA4(r_u32(0x800FF024u));
        if (v101)
      {
        if (sub_800155D4(r_u32(0x800FF024u),r_u32((0x800A5594u+(0)*4u))))
        {
          v66 = 0x800A545Cu;
          v67 = 0x800A0514u;
          do
          {
            v68 = ((sint32)(r_u32((v67+(1)*4u))));
            v69 = ((sint32)(r_u32((v67+(2)*4u))));
            v70 = ((sint32)(r_u32((v67+(3)*4u))));
            w_u32(v66,((sint32)(r_u32(v67))));
            w_u32((v66+(1)*4u),v68);
            w_u32((v66+(2)*4u),v69);
            w_u32((v66+(3)*4u),v70);
            v67 += (4)*4u;
            v66 += (4)*4u;
          }
          while ((v67 != ((uint32)(0x800A0594u))));
          v71 = ((sint32)(r_u32((v67+(1)*4u))));
          w_u32(v66,((sint32)(r_u32(v67))));
          w_u32((v66+(1)*4u),v71);
          v72 = 0x800A53D4u;
          v73 = 0x800A0514u;
          do
          {
            v74 = ((sint32)(r_u32((v73+(1)*4u))));
            v75 = ((sint32)(r_u32((v73+(2)*4u))));
            v76 = ((sint32)(r_u32((v73+(3)*4u))));
            w_u32(v72,((sint32)(r_u32(v73))));
            w_u32((v72+(1)*4u),v74);
            w_u32((v72+(2)*4u),v75);
            w_u32((v72+(3)*4u),v76);
            v73 += (4)*4u;
            v72 += (4)*4u;
          }
          while ((v73 != ((uint32)(0x800A0594u))));
          v77 = ((sint32)(r_u32((v73+(1)*4u))));
          w_u32(v72,((sint32)(r_u32(v73))));
          w_u32((v72+(1)*4u),v77);
          sub_80016014(0x800A0584u);
          v18 = 0;
          sub_8006805C();
          w_u32(0x800FF7E4u,255);
          w_u32(0x800FF7ECu,255);
          w_u32(0x800FF7F4u,255);
          w_u32(0x800FF7FCu,255);
          w_u32(0x800FF7E0u,0);
          w_u32(0x800FF7E8u,0);
          w_u32(0x800FF7F0u,0);
          w_u32(0x800FF7F8u,0);
          w_u32(0x800FF800u,128);
          w_u32(0x800FF804u,128);
          w_u32(0x800FF808u,128);
          w_u32(0x800FF80Cu,128);
          sub_8001551C(v6,r_u32((0x800A554Cu+(0)*4u)));
        }
        else
        {
          v39 = 19;
          v18 = 4;
        }
      }
        v54 = v39;
        if (v102)
        v18 = 4;
        goto LABEL_374;

      case 13:
        v81 = ((uint32)(v114) - (uint32)(1));
        if (v114)
      {
        --v114;
        if (!v81)
        {
          v115 = 150;
          v18 = 14;
        }
      }
      else
      {
        v114 = 10;
      }
        goto LABEL_373;

      case 14:
        if ((r_u32(0x800FEEC4u) == -1))
      {
        v18 = 18;
        goto LABEL_373;
      }
        if ((r_u32(0x800FEEC4u) < 0))
      {
        if ((r_u32(0x800FEEC4u) == -2))
        {
          v18 = 16;
        }
        else
        {
          LABEL_346:
          if (v115)
            --v115;
          else
            LABEL_340:
          v18 = 15;


        }
        LABEL_373:
        v54 = v39;

        goto LABEL_374;
      }
        if ((r_u32(0x800FEEC4u) != 1))
        goto LABEL_346;
        if ((abort(),0u))
        goto LABEL_340;
        v82 = ((uint32)(sub_8002FED8(444)));
        v113 = v82;
        if (v82)
      {
        v94 = 192;
        v95 = 18;
        v113 = ((uint32)(sub_80015228(v82,256,55,0,0x100u,0xC0u,0x12u)));
      }
        w_u8((v113+(9)*1u),1);
        v18 = 17;
        ((void)(v113),abort(),0u);
        v54 = v39;
        LABEL_374:
      sub_80069DF0(v54,0x2000,0);

        sub_800119C4(((sint32)(v107)));
        if ((r_u32(0x800FF64Cu) == v129))
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
        break;

      case 15:

      case 16:
        if ((r_u32(0x800FEEC4u) == 2))
        v18 = 13;
        goto LABEL_371;

      case 17:
        v83 = 0;
        if ((r_u32(0x800FEEC4u) == -1))
      {
        v18 = 18;
        v83 = 1;
      }
        sub_80015BA4(v113);
        if (v101)
      {
        v84 = 0;
        if ((sub_800155D4(v113,r_u32((0x800A55CCu+(0)*4u))) || sub_800155D4(v113,r_u32((0x800A5660u+(0)*4u)))))
          v84 = 1;
        v39 = 28;
        if (!v84)
        {
          v39 = 23;
          v85 = 0x800A545Cu;
          v86 = (0x800A493Cu+(((uint32)(136) * (uint32)(r_u8((v113+(6)*1u)))))*1u);
          v87 = ((uint32)((v86+(512)*1u)));
          v88 = (v86+(640)*1u);
          do
          {
            v89 = ((sint32)(r_u32((v87+(1)*4u))));
            v90 = ((sint32)(r_u32((v87+(2)*4u))));
            v91 = ((sint32)(r_u32((v87+(3)*4u))));
            w_u32(v85,((sint32)(r_u32(v87))));
            w_u32((v85+(1)*4u),v89);
            w_u32((v85+(2)*4u),v90);
            w_u32((v85+(3)*4u),v91);
            v87 += (4)*4u;
            v85 += (4)*4u;
          }
          while ((v87 != ((uint32)(v88))));
          v92 = ((sint32)(r_u32((v87+(1)*4u))));
          w_u32(v85,((sint32)(r_u32(v87))));
          w_u32((v85+(1)*4u),v92);
          sub_80016014(0x800A54CCu);
          v83 = 1;
          v106 = 1;
        }
      }
        if (v102)
      {
        v83 = 1;
        v18 = 0;
      }
        if (v83)
      {
        if (v113)
          apocalypse_menu_destroy(v113);
        v113 = 0;
      }
        goto LABEL_373;

      case 18:
        if (((r_u32(0x800FEEC4u) == -2) || ((((sint32)r_u32(0x800FEEC4u) >= -2) && ((sint32)r_u32(0x800FEEC4u) < 3)) && ((sint32)r_u32(0x800FEEC4u) > 0))))
        v18 = 13;
        LABEL_371:
      v54 = v39;

        if (!v103)
        goto LABEL_374;
        v39 = 19;
        v18 = 0;
        goto LABEL_373;

      default:
        goto LABEL_373;

    }

  }

  sub_8002E2B8();
  sub_800664E4(1);
  DrawSync(0);
  sub_800681B8();
  sub_80015EC8();
  if (v6)
    apocalypse_menu_destroy(v6);
  if (v108)
    apocalypse_menu_destroy(v108);
  if (v109)
    apocalypse_menu_destroy(v109);
  if (v110)
    apocalypse_menu_destroy(v110);
  if (v111)
    apocalypse_menu_destroy(v111);
  if (v112)
    apocalypse_menu_destroy(v112);
  if (v15)
    apocalypse_menu_destroy(v15);
  if (v17)
    apocalypse_menu_destroy(v17);
  if (v107)
    apocalypse_menu_destroy(v107);
  sub_80011B04();
  sub_8007001C();
  sub_80015F7C(0x800A54CCu);
  return v105;
}


/* TODO Missing call adapter sub_800887E4 */
uint32 sub_80068160(void)
{
  uint32 v0;
  sint32 result;
  v0 = r_u32(0x800A72A0u);
  if ((((uint32)(r_u32(0x800FF660u))) == r_u32(0x800A72A0u)))
    v0 = r_u32(0x800C6310u);
  w_u32(0x800FF660u,((sint32)(v0)));
  ((void)((v0+(28)*4u)),(void)(4096),abort(),0u);
  result = (((uint32)(r_u32(0x800FF660u)) + (uint32)(16496)) & 0x7FFFFFFF);
  w_u32(0x800FF668u,result);
  return result;
}


/* TODO Missing call adapter sub_800888EC */
/* TODO Missing call adapter sub_8008895C */
/* TODO Missing call adapter sub_80088B28 */
uint32 sub_800681B8(void)
{
  ResetGraph(1);
  ((void)(((uint32)(r_u32(0x800FF660u)) + (uint32)(92))),abort(),0u);
  ((void)(r_u32(0x800FF660u)),abort(),0u);
  return ((void)(((uint32)(r_u32(0x800FF660u)) + (uint32)(16492))),abort(),0u);
}


/* TODO Missing call adapter sub_8008FC3C */
/* TODO Missing call adapter sub_800918C8 */
/* TODO Missing call adapter sub_80093D3C */
/* TODO Missing call adapter sub_80093E08 */
/* TODO Missing call adapter sub_800940D4 */
/* TODO Missing call adapter xport_draft_missing_gte_adapter */
/* TODO Missing host buffer adapter xport_draft_host_sub_8008F9FC_p4 */
/* TODO Missing host buffer adapter xport_draft_host_sub_80093ED8_p2 */
/* TODO Missing host buffer adapter xport_draft_host_sub_80094134_p2 */
uint32 sub_8006A4C4(uint32 a1, uint32 a2, uint32 a3, uint32 a4, uint32 a5)
{
  uint32 xport_draft_asm_carrier; /* TODO GTE carrier */
  sint32 v10;
  sint32 v11;
  uint32 v12;
  uint32 v14;
  unsigned char v15;
  sint32 v16;
  sint32 v17;
  sint32 v18;
  short v20;
  sint32 v21;
  uint32 v22;
  sint32 v23;
  sint32 v24;
  uint32 v25;
  sint32 v26;
  short v28[2];
  short v29;
  short v30;
  short v31;
  short v32;
  short v33;
  short v34[8];
  char v35[4];
  unsigned char v36;
  unsigned char v37;
  short v38;
  short v39;
  short v40;
  uint32 v41;
  short v42;
  short v43;
  sint32 v44;
  v10 = a5;
  v11 = (((a1>>16)&65535u) & 0x7F);
  v12 = a1;
  v14 = (a1 >> 31);
  v42 = a3;
  v43 = a4;
  if (a5)
    v10 = ((uint32)(sub_80066570(((uint32)(2) * (uint32)(a5)))) - (uint32)(a5));
  v15 = 1;
  if ((((unsigned short)(v12)) >= 2u))
  {
    v15 = 0;
    v16 = 0;
    v17 = ((v17 & 0xFFFFFF00u)|(((v12) & 0xFFu)<<0));
    do
    {
      if (((v17 & 1) != 0))
        ++v15;
      v17 = (((sint32)(((sint32)(((unsigned short)(v12)))))) >> ++v16);
    }
    while ((((sint32)(v16)) < 16));
  }
  sub_80094544();
  if (v14)
    v18 = 127;
  else
    v18 = 0;
  xport_draft_asm_carrier = sub_8009426C(v15,v18);
  if ((xport_draft_asm_carrier > 0))
  {
    (abort(),0u);
    if (((a1 & 0x40000000) != 0))
      v20 = r_u32(0x800FF6A0u);
    else
      v20 = r_u32(0x800FF69Cu);
    (abort(),0u);
    v21 = 0;
    v22 = 0;
    v23 = v20;
    v28[1] = v20;
    v44 = xport_draft_asm_carrier;
    v32 = v11;
    v41 = ((uint32)(32) - (uint32)(v41));
    v24 = ((uint32)(((uint32)(a2) + (uint32)(v10))) << (uint32)(16));
    v34[0] = v42;
    v25 = r_u32(0x80100738u);
    v33 = (abort(),0u);
    v34[3] = 0;
    v34[1] = v43;
    while (((v22 < v41) && ((uint16)(v12))))
    {
      v26 = v21;
      if ((((xport_draft_asm_carrier >> v22) & 1) != 0))
      {
        while ((((sint32)(v26)) < 16))
        {
          if ((((((sint32)(((sint32)(((unsigned short)(v12)))))) >> v26) & 1) != 0))
          {
            v21 = ((uint32)(v26) + (uint32)(1));
            xport_draft_host_sub_8008F9FC_p4(v23,v11,((short)(v26)),v35);
            v28[0] = v40;
            v29 = ((void)((((sint32)(v24)) >> 16)),(void)(0),(void)(v36),(void)(v37),abort(),0u);
            w_u16(((uint32)(v25)),v29);
            v31 = v26;
            v30 = v36;
            xport_draft_host_sub_80094134_p2(v22,v28);
            v34[2] = v29;
            v34[4] = (((void)(v23),(void)(v40),abort(),0u) >> 3);
            v34[5] = v38;
            v34[6] = v39;
            xport_draft_host_sub_80093ED8_p2(v22,v34);
            v12 &= ~(((uint32)(1) << (uint32)(v26)));
            break;
          }
          ++v26;
        }

      }
      v25 = ((uint32)((((uint32)(v25))+(2)*1u)));
      ++v22;
    }

    ((void)(xport_draft_asm_carrier),abort(),0u);
    ((void)(xport_draft_asm_carrier),(void)(v44),abort(),0u);
  }
  sub_80094570();
  return xport_draft_asm_carrier;
}



uint32 sub_8002E4D8(uint32 a1)
{
  sint32 v2;
  sint32 v3;
  uint32 v4;
  sint32 result;
  sint8 v6;
  v2 = 0;
  v3 = r_u32((0x800FF230u+(((unsigned char)(r_u8(0x800FF238u))))*4u));
  v4 = ((uint32)(((uint32)(v3) + (uint32)(((uint32)(((uint32)(((unsigned short)(r_u16(0x800FF23Cu)))) - (uint32)(1))) * (uint32)(((unsigned short)(r_u16(0x800FF23Au)))))))));
  if ((((unsigned short)(r_u16(0x800FF23Au))) >> 2))
  {
    do
    {
      w_u32(v4,0);
      ++v2;
      (v4+=4u);
    }
    while ((((sint32)(v2)) < (((unsigned short)(r_u16(0x800FF23Au))) >> 2)));
  }
  result = sub_80066570((((unsigned char)(r_u8(0x800FF23Eu))) >> 1));
  v6 = ((uint32)((((unsigned char)(r_u8(0x800FF23Eu))) >> 1)) + (uint32)(result));
  while (a1)
  {
    result = ((uint32)(sub_80066570((((unsigned short)(r_u16(0x800FF23Au))) >> 1))) + (uint32)((((unsigned short)(r_u16(0x800FF23Au))) >> 2)));
    w_u8(((uint32)(((sint32)(((uint32)(((uint32)(v3) + (uint32)(((uint32)(((uint32)(((unsigned short)(r_u16(0x800FF23Cu)))) - (uint32)(1))) * (uint32)(((unsigned short)(r_u16(0x800FF23Au)))))))) + (uint32)(result)))))),v6);
    w_u8(((uint32)(((uint32)(((sint32)(((uint32)(((uint32)(v3) + (uint32)(((uint32)(((uint32)(((unsigned short)(r_u16(0x800FF23Cu)))) - (uint32)(1))) * (uint32)(((unsigned short)(r_u16(0x800FF23Au)))))))) + (uint32)(result))))) + (uint32)(1)))),v6);
    --a1;
    w_u8(((uint32)(((uint32)(((sint32)(((uint32)(((uint32)(v3) + (uint32)(((uint32)(((uint32)(((unsigned short)(r_u16(0x800FF23Cu)))) - (uint32)(1))) * (uint32)(((unsigned short)(r_u16(0x800FF23Au)))))))) + (uint32)(result))))) - (uint32)(1)))),v6);
  }

  return result;
}







