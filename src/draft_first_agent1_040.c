#include "game_movie_startup.h"
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
#include <stdlib.h>
/* Unverified draft; TODO Recover omitted call arguments, host-buffer adapters and signed field widths */
/* TODO Missing call adapter sub_800885A4 */
/* TODO Resolve original data label 0x800FF684u */
uint32 sub_80069320(void)
{
  uint32 palette[512];
  sint16 rectangle[4];
  uint32 row,column;
  sub_80068450(0xFFFFFFFFu,16,148,0x800FF68Cu,0x800FF690u,16,0,0);
  sub_80068450(0xFFFFFFFFu,256,82,0x800FF694u,0x800FF698u,16,0,0);
  for(row=0;row<148u;++row) w_u8(0x80100590u+row,1);
  w_u8(0x80100630u,0);
  for(row=1;row<82u;++row) w_u8(0x80100630u+row,1);
  sub_8006B04C(0x800FF684u);
  xport_draft_host_sub_8006B234_p1(palette);
  sub_8006B44C();
  for(row=0;row<16u;++row) {
    for(column=0;column<16u;++column) {
      uint32 color=palette[6u+16u*(15u-row)+column];
      uint32 red=(color&255u)>>3;
      uint32 green=(color>>11)&31u;
      uint32 blue=(color>>19)&31u;
      uint32 packed=(red==31u && green==0u && blue==31u)
          ?0u:0x8000u|red|(green<<5)|(blue<<10);
      w_u16(0x801003AEu+32u*row-2u*column,packed);
    }
  }
  rectangle[0]=(sint16)r_u16(0x800FF694u);
  rectangle[1]=(sint16)r_u16(0x800FF698u);
  rectangle[2]=256;
  rectangle[3]=1;
  return xport_draft_host_sub_800885A4_p1(rectangle,0x80100390u);
}

uint32 sub_8002E814(uint32 movie)
{
    return apocalypse_play_movie(movie);
}

void sub_80010184(void)
{
    if (r_u32(0x800FEEC0u))
    {
        sub_8006BC20(r_u32(0x800FEEC0u));
        w_u32(0x800FEEC0u, 0u);
    }
    if (r_u32(0x800FEEBCu))
    {
        sub_8006BC20(r_u32(0x800FEEBCu));
        w_u32(0x800FEEBCu, 0u);
    }
}

/* TODO Missing call adapter sub_8008F16C */
/* TODO Missing call adapter sub_8008F38C */
/* TODO Missing call adapter sub_800936AC */
uint32 sub_8006994C(uint32 a1)
{
  sint32 result;
  sub_8006A334(0);
  w_u32(0x800FF69Cu,sub_80069D2C(a1,0x800FF6D0u));
  sub_8008F16C(0x80100688u,1,1);
  w_u32(0x801044F0u,((sint32)(0x8008CDFCu)));
  w_u32(0x801044F4u,((sint32)(0x8008CEDCu)));
  w_u32(0x801044F8u,((sint32)(0x8008CB1Cu)));
  w_u32(0x80104500u,((sint32)(0x8008CBCCu)));
  w_u32(0x8010450Cu,((sint32)(0x8008C8CCu)));
  w_u32(0x80104510u,((sint32)(0x8008C99Cu)));
  w_u32(0x801044FCu,0);
  w_u32(0x80104504u,0);
  w_u32(0x80104514u,0);
  w_u32(0x80104518u,((sint32)(0x8008CA6Cu)));
  w_u32(0x8010451Cu,0);
  w_u32(0x80104520u,0);
  w_u32(0x80104524u,0);
  w_u32(0x80104528u,0);
  w_u32(0x8010452Cu,0);
  w_u32(0x80104530u,0);
  w_u32(0x80104508u,0);
  w_u32(0x80104534u,0);
  w_u32(0x80104538u,0);
  w_u32(0x8010453Cu,0);
  w_u32(0x80104540u,0);
  w_u32(0x80104544u,0);
  w_u32(0x80104548u,0);
  w_u32(0x8010454Cu,0);
  w_u32(0x80104550u,0);
  w_u32(0x80104554u,0);
  w_u32(0x80104558u,0);
  w_u32(0x8010455Cu,0);
  w_u32(0x80104560u,0);
  w_u32(0x80104564u,0);
  w_u32(0x80104568u,0);
  w_u32(0x8010456Cu,0);
  w_u32(0x80104570u,0);
  w_u32(0x80104574u,0);
  w_u32(0x80104578u,0);
  w_u32(0x8010457Cu,0);
  w_u32(0x80104580u,0);
  SsSetTickMode(4096);
  sub_800936AC(24);
  sub_8008EC4C();
  result = 1;
  w_u32(0x800FF6B4u,0);
  w_u32(0x800FF6B8u,1);
  return result;
}



uint32 sub_80071288(uint32 a1)
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
  sint32 v13;
  sint32 result;
  int v15[4];
  sub_8006A0EC();
  sub_8006A0FC();
  w_u16(0x800A67C8u,256);
  sub_8002F130();
  sub_800711D0();
  sub_80063BF0();
  if (a1)
  {
    sub_8006F004();
  }
  else
  {
    sub_8006A3F0();
    sub_80069A94();
    sub_80069B8C();
    sub_8006A3A4();
    sub_8006EE2C();
    sub_800653F4();
    sub_80063F20();
    sub_800691B8();
    sub_8002E2B8();
    w_u8(0x800A53E1u,0);
  }
  sub_8006654C(312921176);
  sub_8006FB04();
  sub_8007DCDC();
  sub_8001BE54();
  sub_8001BAC0();
  sub_8006CDA0();
  v15[0] = 0;
  v15[1] = 0x8000;
  v15[2] = 0;
  sub_8003AA20(v15);
  w_u32(0x800FF2F0u,0);
  sub_8007001C();
  w_u32(0x800FF5ECu,13000);
  result = -20;
  w_u32(0x800FF380u,0);
  w_u16(0x800FF038u,-20);
  return result;
}



uint32 sub_8006C4EC(uint32 a1, uint32 a2, uint32 a3)
{
  uint32 result;
  sint32 v4;
  sint32 v5;
  result = a1;
  v5 = (r_u32((a2+(2)*4u)) / r_u32(a3));
  v4 = (r_u32((a2+(1)*4u)) / r_u32(a3));
  w_u32(a1,(r_u32(a2) / r_u32(a3)));
  w_u32((a1+(1)*4u),v4);
  w_u32((a1+(2)*4u),v5);
  return result;
}



void sub_8001A7BC(uint32 a1)
{
  w_u16(0x800FF1B8u,a1);
}


/* TODO Postincrement memory expressions may require ordering refinement */
uint32 sub_8001AA28(uint32 a1, uint32 a2, uint32 a3, uint32 a4, uint32 color, uint32 scale)
{
  sint32 v14;
  sint32 v15;
  sint32 v16;
  sint32 v17;
  sint32 v18;
  sint32 v19;
  sint32 v20;
  unsigned char v21;
  sint32 v22;
  sint32 v23;
  sint32 v24;
  sint32 v25;
  sint32 v26;
  uint32 v27;
  sint32 v28;
  sint32 v29;
  sint32 v30;
  sint32 v31;
  sint32 v32;
  short v33;
  sint8 v34;
  sint8 v35;
  sint8 v36;
  sint32 v37;
  sint8 v38;
  sint8 v39;
  sint32 v40;
  sint32 v41;
  sint8 v42;
  sint32 v43;
  sint32 v44;
  sint32 v45;
  sint32 v46;
  sint32 v47;
  sint32 v48;
  short v49;
  short v50;
  sint32 v51;
  sint32 v52;
  sint32 v53;
  sint32 v54;
  sint32 v55;
  sint32 v56;
  sint32 v57;
  short v58;
  sint32 v59;
  sint32 v60;
  short v61;
  sint32 v62;
  short v63;
  short v64;
  short v65;
  short v66;
  short v67;
  short v68;
  short v69;
  short v70;
  short v71;
  short v72;
  sint32 v73;
  sint32 v74;
  uint32 v75;
  sint32 v76;
  sint32 v77;
  uint32 v78;
  uint32 v79;
  sint32 v80;
  sint32 v81;
  sint32 v82;
  sint32 v83;
  sint32 v84;
  sint32 v85;
  short v86;
  short v87;
  short v88;
  short v89;
  short v90;
  short v91;
  short v92;
  sint8 v93;
  sint8 v94;
  short v95;
  sint32 v96;
  sint32 v97;
  sint32 v99;
  sint32 v100;
  uint32 v101;
  uint32 v102;
  sint32 v103;
  sint32 v104;
  sint32 v105;
  short v106;
  sint32 v107;
  uint32 v108;
  sint32 v109;
  short v110;
  sint32 v111;
  short v112;
  sint32 v114;
  if (r_u32(0x800FF1D0u))
    v114 = ((color | (color << 16)) | (color << 8));
  else
    v114 = 0;
  v14 = 0;
  v99 = 0;
  v101 = a3;
  v103 = 0;
  while (r_u8(v101))
  {
    v15 = sub_8001A784(r_u8(v101));
    v16 = (v15 < 0);
    v17 = (8 * v15);
    if (v16)
    {
      v18 = (v103 + 10);
    }
    else
    {
      ++v14;
      v18 = ((v103 + r_u8(((uint32)(((v17 + r_u32(0x800FF6E0u)) + 2))))) + (sint32)(sint8)r_u8((uint32)(v17 + r_u32(0x800FF6E0u))));
    }
    v103 = v18;
    (v101+=1u);
  }

  v104 = (((v103 * ((unsigned short)(r_u16(0x800FF1B8u)))) >> 8) + (3 * v14));
  if ((r_u8(0x800FF1B0u) == 1))
  {
    v20 = 0;
    goto LABEL_19;
  }
  if ((((unsigned char)(r_u8(0x800FF1B0u))) >= 2u))
  {
    if ((r_u8(0x800FF1B0u) != 2))
    {
      /* TODO Invalid alignment retains an undefined incoming offset */
      v19 = a1 + xport_draft_unknown_alignment_offset_8001AA28();
      goto LABEL_20;
    }
    v20 = -v104;
    LABEL_19:
    v19 = (a1 + v20);

    goto LABEL_20;
  }
  if (!r_u8(0x800FF1B0u))
  {
    v20 = (((v104 > 0) - v104) >> 1);
    goto LABEL_19;
  }
  /* TODO Invalid alignment retains an undefined incoming offset */
      v19 = a1 + xport_draft_unknown_alignment_offset_8001AA28();
  LABEL_20:
  v105 = v19;

  v106 = a2;
  v102 = a3;
  v107 = ((9 * ((unsigned short)(r_u16(0x800FF1B8u)))) >> 8);
  while (1)
  {
    v21 = r_u8(v102);
    if (!(r_u8(v102)))
      return v104;
    (v102+=1u);
    v22 = sub_8001A784(v21);
    if ((v22 < 0))
    {
      v105 += ((5 * ((uint32)(((unsigned short)(r_u16(0x800FF1B8u)))))) >> 7);
    }
    else
    {
      v23 = r_u32(0x800FF668u);
      v24 = (r_u32(0x800FF668u) + 40);
      v25 = (r_u32(0x800FF668u) + 80);
      v26 = (r_u32(0x800FF668u) + 120);
      v108 = ((uint32)((r_u32(0x800FF6E0u) + (8 * v22))));
      v27 = ((uint32)(r_u32((((uint32)(v108))+(1)*4u))));
      v28 = (r_u32(0x800FF668u) + 160);
      if ((r_u32(0x800FF374u) < ((uint32)((r_u32(0x800FF668u) + 212)))))
        return v104;
      w_u32(0x800FF668u,(r_u32(0x800FF668u)+(212)));
      v29 = r_u32(0x800FF1B4u);
      w_u32(((uint32)(v23)),150994944);
      w_u32(((uint32)((v23 + 4))),v29);
      v30 = r_u32(v27);
      v31 = r_u32((v27+(1)*4u));
      v32 = r_u32((v27+(2)*4u));
      v33 = r_u16((((uint32)(v27))+(5)*2u));
      w_u32(((uint32)((v23 + 12))),v30);
      w_u32(((uint32)((v23 + 20))),v31);
      w_u32(((uint32)((v23 + 28))),v32);
      w_u16(((uint32)((v23 + 36))),v33);
      if (((((unsigned short)(r_u16(0x800FF1B8u))) >= 0x200u) || a4))
      {
        v34 = r_u8(((uint32)((v23 + 29))));
        (w_u8(((uint32)((v23 + 20))),(r_u8(((uint32)((v23 + 20))))-1u)),(r_u8(((uint32)((v23 + 20))))-1u));
        v35 = r_u8(((uint32)((v23 + 36))));
        w_u8(((uint32)((v23 + 29))),(v34 - 1));
        v36 = (r_u8(((uint32)((v23 + 37)))) - 1);
        w_u8(((uint32)((v23 + 36))),(v35 - 1));
        w_u8(((uint32)((v23 + 37))),v36);
      }
      if (v114)
      {
        w_u32(((uint32)((v26 + 4))),(v114 | 0x2E000000));
        v37 = r_u32(0x800FF1D0u);
        w_u32(((uint32)(v26)),150994944);
        w_u32(((uint32)((v26 + 12))),r_u32(((uint32)(v37))));
        w_u32(((uint32)((v26 + 20))),r_u32(((uint32)((v37 + 4)))));
        v38 = r_u8(((uint32)(v37)));
        v39 = r_u8((uint32)v37 + 9u) - (((sint32)r_u8((uint32)v37 + 9u) - (sint32)r_u8((uint32)v37 + 1u)) >> 3);
        w_u8(((uint32)((v26 + 28))),r_u8(((uint32)(v37))));
        w_u8(((uint32)((v26 + 29))),v39);
        w_u32(((uint32)((v28 + 4))),(v114 | 0x3E000000));
        w_u32(((uint32)((v28 + 16))),v114);
        w_u32(((uint32)((v28 + 28))),0);
        w_u32(((uint32)((v28 + 40))),0);
        v40 = r_u32(0x800FF1D0u);
        w_u32(((uint32)(v28)),201326592);
        w_u16(((uint32)((v28 + 14))),r_u16(((uint32)((v40 + 2)))));
        v40 = ((v40&0xFFFF0000u)|(((r_u16(((uint32)((v40 + 6)))))&0xFFFFu)<<0));
        w_u8(((uint32)((v28 + 12))),v38);
        w_u8(((uint32)((v28 + 13))),v39);
        v41 = r_u32(0x800FF1D0u);
        w_u16(((uint32)((v28 + 26))),v40);
        v42 = r_u8(((uint32)((v41 + 4))));
        w_u8(((uint32)((v26 + 37))),v39);
        w_u8(((uint32)((v26 + 36))),v42);
        w_u8(((uint32)((v28 + 24))),v42);
        w_u8(((uint32)((v28 + 25))),v39);
        v43 = r_u8(((uint32)((r_u32(0x800FF1D0u) + 10))));
        v44 = r_u8(((uint32)((r_u32(0x800FF1D0u) + 8))));
        v45 = ((3 * (v43 - v44)) >> 4);
        v46 = ((v44 + v43) >> 1);
        w_u8(((uint32)((v28 + 36))),(v46 - v45));
        v41 = ((v41&0xFFFFFF00u)|(((r_u8(((uint32)((r_u32(0x800FF1D0u) + 9)))))&0xFFu)<<0));
        w_u8(((uint32)((v28 + 48))),(v46 + v45));
        w_u8(((uint32)((v28 + 37))),v41);
        w_u8(((uint32)((v28 + 49))),r_u8(((uint32)((r_u32(0x800FF1D0u) + 11)))));
        v46 = ((v46&0xFFFFFF00u)|((((r_u8(((uint32)((v28 + 49)))) - 1))&0xFFu)<<0));
        (w_u8(((uint32)((v28 + 37))),(r_u8(((uint32)((v28 + 37))))-1u)),(r_u8(((uint32)((v28 + 37))))-1u));
        w_u8(((uint32)((v28 + 49))),v46);
      }
      v47 = ((unsigned short)(r_u16(0x800FF1B8u)));
      w_u16(((uint32)((v23 + 8))),(v105 + (((sint32)(sint8)r_u8(v108) * ((unsigned short)(r_u16(0x800FF1B8u)))) >> 8)));
      w_u16(((uint32)((v23 + 10))),((v106 + (((sint32)(sint8)r_u8(v108 + 1u) * v47) >> 8)) - v107));
      v48 = (((unsigned char)(r_u8((v108+(2)*1u)))) * v47);
      v49 = r_u16(((uint32)((v23 + 8))));
      w_u16(((uint32)((v23 + 18))),r_u16(((uint32)((v23 + 10)))));
      w_u16(((uint32)((v23 + 24))),v49);
      w_u16(((uint32)((v23 + 16))),(v49 + (v48 >> 8)));
      w_u16(((uint32)((v23 + 26))),(r_u16(((uint32)((v23 + 10)))) + ((((unsigned char)(r_u8((v108+(3)*1u)))) * v47) >> 8)));
      v50 = r_u16(((uint32)((v23 + 26))));
      w_u16(((uint32)((v23 + 32))),r_u16(((uint32)((v23 + 16)))));
      w_u16(((uint32)((v23 + 34))),v50);
      if (v114)
      {
        v51 = v99;
        v100 = (v99 + 1);
        v52 = ((sint32)(sint16)r_u16((uint32)v23 + 10u) - r_u32((0x800A593Cu+(v51)*4u)));
        v53 = v100;
        if ((v100 >= 41))
        {
          v100 = 0;
          v53 = 0;
        }
        v99 = (v100 + 1);
        if ((v99 >= 41))
          v99 = 0;
        v54 = ((v50 - ((sint32)(sint16)r_u16((uint32)v23 + 18u) - r_u32((0x800A593Cu+(v53)*4u)))) * scale);
        v55 = ((sint32)(sint16)r_u16((uint32)v23 + 8u) - 10);
        v56 = ((sint32)(sint16)r_u16((uint32)v23 + 32u) + 10);
        w_u16(((uint32)((v26 + 8))),v55);
        w_u16(((uint32)((v26 + 16))),v56);
        w_u16(((uint32)((v26 + 24))),v55);
        w_u16(((uint32)((v26 + 32))),v56);
        v57 = v50 - ((sint32)((uint32)(v50 - v52) * scale) >> 8);
        w_u16(((uint32)((v26 + 10))),v57);
        w_u16(((uint32)((v26 + 26))),(v50 - ((v50 - v57) >> 3)));
        w_u16(((uint32)((v26 + 18))),(v50 - (v54 >> 8)));
        w_u16(((uint32)((v26 + 34))),(v50 - (v54 >> 11)));
        w_u16(((uint32)((v28 + 8))),v55);
        w_u16(((uint32)((v28 + 10))),r_u16(((uint32)((v26 + 26)))));
        w_u16(((uint32)((v28 + 20))),r_u16(((uint32)((v26 + 32)))));
        v58 = r_u16(((uint32)((v26 + 34))));
        w_u16(((uint32)((v28 + 34))),(v50 + 2));
        w_u16(((uint32)((v28 + 46))),(v50 + 2));
        v59 = ((3 * (v56 - v55)) >> 4);
        v60 = ((v55 + v56) >> 1);
        w_u16(((uint32)((v28 + 32))),(v60 - v59));
        w_u16(((uint32)((v28 + 44))),(v60 + v59));
        w_u16(((uint32)((v28 + 22))),v58);
      }
      w_u32(((uint32)(v24)),r_u32(((uint32)(v23))));
      w_u32(((uint32)((v24 + 4))),r_u32(((uint32)((v23 + 4)))));
      w_u32(((uint32)((v24 + 8))),r_u32(((uint32)((v23 + 8)))));
      v61 = r_u16(((uint32)((v24 + 10))));
      w_u32(((uint32)((v24 + 12))),r_u32(((uint32)((v23 + 12)))));
      w_u32(((uint32)((v24 + 16))),r_u32(((uint32)((v23 + 16)))));
      w_u32(((uint32)((v24 + 20))),r_u32(((uint32)((v23 + 20)))));
      w_u32(((uint32)((v24 + 24))),r_u32(((uint32)((v23 + 24)))));
      w_u32(((uint32)((v24 + 28))),r_u32(((uint32)((v23 + 28)))));
      w_u32(((uint32)((v24 + 32))),r_u32(((uint32)((v23 + 32)))));
      v62 = r_u32(((uint32)((v23 + 36))));
      v63 = r_u16(((uint32)((v24 + 8))));
      w_u16(((uint32)((v24 + 10))),(v61 + 1));
      v64 = r_u16(((uint32)((v24 + 18))));
      w_u8(((uint32)((v24 + 6))),0);
      w_u8(((uint32)((v24 + 5))),0);
      w_u8(((uint32)((v24 + 4))),0);
      w_u16(((uint32)((v24 + 8))),(v63 + 1));
      v65 = r_u16(((uint32)((v24 + 16))));
      w_u16(((uint32)((v24 + 18))),(v64 + 1));
      v66 = r_u16(((uint32)((v24 + 26))));
      w_u16(((uint32)((v24 + 16))),(v65 + 1));
      (w_u16(((uint32)((v24 + 24))),(r_u16(((uint32)((v24 + 24))))+1u)),(r_u16(((uint32)((v24 + 24))))+1u));
      w_u32(((uint32)((v24 + 36))),v62);
      w_u16(((uint32)((v24 + 26))),(v66 + 1));
      v67 = (r_u16(((uint32)((v24 + 34)))) + 1);
      (w_u16(((uint32)((v24 + 32))),(r_u16(((uint32)((v24 + 32))))+1u)),(r_u16(((uint32)((v24 + 32))))+1u));
      w_u16(((uint32)((v24 + 34))),v67);
      w_u32(((uint32)(v25)),r_u32(((uint32)(v23))));
      w_u32(((uint32)((v25 + 4))),r_u32(((uint32)((v23 + 4)))));
      w_u32(((uint32)((v25 + 8))),r_u32(((uint32)((v23 + 8)))));
      v68 = r_u16(((uint32)((v25 + 10))));
      w_u32(((uint32)((v25 + 12))),r_u32(((uint32)((v23 + 12)))));
      w_u32(((uint32)((v25 + 16))),r_u32(((uint32)((v23 + 16)))));
      w_u32(((uint32)((v25 + 20))),r_u32(((uint32)((v23 + 20)))));
      w_u32(((uint32)((v25 + 24))),r_u32(((uint32)((v23 + 24)))));
      w_u32(((uint32)((v25 + 28))),r_u32(((uint32)((v23 + 28)))));
      w_u32(((uint32)((v25 + 32))),r_u32(((uint32)((v23 + 32)))));
      w_u32(((uint32)((v25 + 36))),r_u32(((uint32)((v23 + 36)))));
      w_u8(((uint32)((v25 + 4))),r_u32(0x800FF1BCu));
      w_u8(((uint32)((v25 + 5))),r_u32(0x800FF1C0u));
      v62 = ((v62&0xFFFFFF00u)|(((r_u32(0x800FF1C4u))&0xFFu)<<0));
      w_u16(((uint32)((v25 + 8))),(r_u16(((uint32)((v25 + 8))))-(2)));
      w_u8(((uint32)((v25 + 6))),v62);
      w_u16(((uint32)((v25 + 10))),(v68 - 1));
      v69 = r_u16(((uint32)((v25 + 24))));
      w_u16(((uint32)((v25 + 16))),(r_u16(((uint32)((v25 + 16))))-(2)));
      v70 = r_u16(((uint32)((v25 + 18))));
      w_u16(((uint32)((v25 + 24))),(v69 - 2));
      v71 = r_u16(((uint32)((v25 + 32))));
      w_u16(((uint32)((v25 + 18))),(v70 - 1));
      v72 = r_u16(((uint32)((v25 + 26))));
      w_u16(((uint32)((v25 + 32))),(v71 - 2));
      w_u16(((uint32)((v25 + 26))),(v72 - 1));
      v16 = (r_u32(0x800FF1CCu) != 0);
      (w_u16(((uint32)((v25 + 34))),(r_u16(((uint32)((v25 + 34))))-1u)),(r_u16(((uint32)((v25 + 34))))-1u));
      if (v16)
        goto LABEL_39;
      if (v114)
      {
        v73 = ((unsigned short)(r_u16(0x800FF1BAu)));
        w_u16(((uint32)((v26 + 22))),((r_u16(((uint32)((v26 + 22)))) & 0xFF9F) | 0x20));
        w_u16(((uint32)((v28 + 26))),((r_u16(((uint32)((v28 + 26)))) & 0xFF9F) | 0x20));
        v74 = ((4 * v73) + r_u32(0x800FF660u));
        w_u32(((uint32)(v26)),((r_u32(((uint32)(v26))) & 0xFF000000) | (r_u32(((uint32)((v74 + 112)))) & 0xFFFFFF)));
        v75 = ((r_u32(((uint32)((v74 + 112)))) & 0xFF000000) | (v26 & 0xFFFFFF));
        w_u32(((uint32)((v74 + 112))),v75);
        w_u32(((uint32)(v28)),((r_u32(((uint32)(v28))) & 0xFF000000) | (v75 & 0xFFFFFF)));
        w_u32(((uint32)((v74 + 112))),((r_u32(((uint32)((v74 + 112)))) & 0xFF000000) | (v28 & 0xFFFFFF)));
      }
      v76 = r_u32(0x800FF1CCu);
      v77 = ((4 * ((unsigned short)(r_u16(0x800FF1BAu)))) + r_u32(0x800FF660u));
      w_u32(((uint32)(v23)),((r_u32(((uint32)(v23))) & 0xFF000000) | (r_u32(((uint32)((v77 + 112)))) & 0xFFFFFF)));
      v78 = ((r_u32(((uint32)((v77 + 112)))) & 0xFF000000) | (v23 & 0xFFFFFF));
      w_u32(((uint32)((v77 + 112))),v78);
      w_u32(((uint32)(v25)),((r_u32(((uint32)(v25))) & 0xFF000000) | (v78 & 0xFFFFFF)));
      v79 = ((r_u32(((uint32)((v77 + 112)))) & 0xFF000000) | (v25 & 0xFFFFFF));
      w_u32(((uint32)((v77 + 112))),v79);
      w_u32(((uint32)(v24)),((r_u32(((uint32)(v24))) & 0xFF000000) | (v79 & 0xFFFFFF)));
      w_u32(((uint32)((v77 + 112))),((r_u32(((uint32)((v77 + 112)))) & 0xFF000000) | (v24 & 0xFFFFFF)));
      if (v76)
      {
        LABEL_39:
        v80 = 0;

        v109 = ((sint32)(sint16)r_u16((uint32)v23 + 16u) - (sint32)(sint16)r_u16((uint32)v23 + 8u));
        v110 = (r_u8(((uint32)((v23 + 20)))) - r_u8(((uint32)((v23 + 12)))));
        v111 = ((sint32)(sint16)r_u16((uint32)v23 + 26u) - (sint32)(sint16)r_u16((uint32)v23 + 10u));
        v112 = (r_u8(((uint32)((v23 + 29)))) - r_u8(((uint32)((v23 + 13)))));
        while ((v80 < 3))
        {
          v81 = sub_80066570(32);
          v82 = sub_80066570(128);
          v83 = ((v81 + sub_80066570((192 - v81))) + 64);
          v84 = ((v82 + sub_80066570((192 - v82))) + 64);
          v85 = r_u32(0x800FF668u);
          if ((r_u32(0x800FF374u) < ((uint32)((r_u32(0x800FF668u) + 40)))))
            return v104;
          w_u32(0x800FF668u,(r_u32(0x800FF668u)+(40)));
          v87 = (sub_80066570(6) - 3);
          v86 = sub_80066570(6);
          w_u32(((uint32)(v85)),r_u32(((uint32)(v23))));
          w_u32(((uint32)((v85 + 4))),r_u32(((uint32)((v23 + 4)))));
          v86 -= 3;
          w_u16(((uint32)((v85 + 14))),r_u16(((uint32)((v23 + 14)))));
          w_u16(((uint32)((v85 + 22))),r_u16(((uint32)((v23 + 22)))));
          w_u16(((uint32)((v85 + 8))),((r_u16(((uint32)((v23 + 8)))) + ((v81 * v109) >> 8)) + v87));
          v88 = r_u16(((uint32)((v85 + 8))));
          w_u16(((uint32)((v85 + 10))),((r_u16(((uint32)((v23 + 10)))) + ((v82 * v111) >> 8)) + v86));
          v89 = r_u16(((uint32)((v23 + 8))));
          w_u16(((uint32)((v85 + 24))),v88);
          v90 = r_u16(((uint32)((v85 + 10))));
          w_u16(((uint32)((v85 + 16))),((v89 + ((v83 * v109) >> 8)) + v87));
          w_u16(((uint32)((v85 + 18))),v90);
          v91 = ((r_u16(((uint32)((v23 + 10)))) + ((v84 * v111) >> 8)) + v86);
          v92 = r_u16(((uint32)((v85 + 16))));
          ++v80;
          w_u16(((uint32)((v85 + 26))),v91);
          w_u16(((uint32)((v85 + 32))),v92);
          w_u16(((uint32)((v85 + 34))),v91);
          w_u8(((uint32)((v85 + 12))),(r_u8(((uint32)((v23 + 12)))) + (((unsigned short)((v81 * v110))) >> 8)));
          w_u8(((uint32)((v85 + 13))),(r_u8(((uint32)((v23 + 13)))) + (((unsigned short)((v82 * v112))) >> 8)));
          v93 = r_u8(((uint32)((v85 + 12))));
          v90 = ((v90&0xFFFFFF00u)|(((r_u8(((uint32)((v85 + 13)))))&0xFFu)<<0));
          w_u8(((uint32)((v85 + 20))),(r_u8(((uint32)((v23 + 12)))) + (((unsigned short)((v83 * v110))) >> 8)));
          w_u8(((uint32)((v85 + 21))),v90);
          w_u8(((uint32)((v85 + 28))),v93);
          v92 = ((v92&0xFFFFFF00u)|(((r_u8(((uint32)((v23 + 13)))))&0xFFu)<<0));
          v94 = r_u8(((uint32)((v85 + 20))));
          w_u8(((uint32)((v85 + 7))),(r_u8(((uint32)((v85 + 7))))|(2u)));
          w_u8(((uint32)((v85 + 29))),(v92 + (((unsigned short)((v84 * v112))) >> 8)));
          v95 = r_u16(((uint32)((v85 + 22))));
          v91 = ((v91&0xFFFFFF00u)|(((r_u8(((uint32)((v85 + 29)))))&0xFFu)<<0));
          w_u8(((uint32)((v85 + 36))),v94);
          w_u8(((uint32)((v85 + 37))),v91);
          v96 = ((unsigned short)(r_u16(0x800FF1BAu)));
          w_u16(((uint32)((v85 + 22))),(v95 | 0x60));
          v97 = ((4 * v96) + r_u32(0x800FF660u));
          w_u32(((uint32)(v85)),((r_u32(((uint32)(v85))) & 0xFF000000) | (r_u32(((uint32)((v97 + 112)))) & 0xFFFFFF)));
          w_u32(((uint32)((v97 + 112))),((r_u32(((uint32)((v97 + 112)))) & 0xFF000000) | (v85 & 0xFFFFFF)));
        }

      }
      v105 += (3 + (((((unsigned char)(r_u8((v108+(2)*1u)))) + (sint32)(sint8)r_u8(v108)) * ((unsigned short)(r_u16(0x800FF1B8u)))) >> 8));
    }
  }

}



uint32 sub_80011B34(void)
{
  sub_8006F29C(r_u32((((uint32)(0x800A5374u))+((8 * r_u32(0x800FEFF0u)))*4u)),0);
  sub_8006F29C(r_u32((0x800A5378u+((8 * r_u32(0x800FEFF0u)))*4u)),1);
  return sub_800653B8(250000);
}



void sub_8006B70C(uint32 a1, uint32 a2)
{
  w_u32(0x800FF728u,a1);
  w_u32(0x800FF72Cu,a2);
}



uint32 sub_800664E4(uint32 a1)
{
  uint32 v1;
  sint32 result;
  v1 = (r_u32(0x800FF64Cu) + a1);
  for (result = (r_u32(0x800FF64Cu) < ((uint32)((r_u32(0x800FF64Cu) + a1)))); (r_u32(0x800FF64Cu) < v1); result = (r_u32(0x800FF64Cu) < v1))
    ;

  return result;
}



uint32 sub_8006F4D0(uint32 a1)
{
  sint32 result;
  result = 0;
  if ((a1 != r_u32((0x800FF778u+(0)*4u))))
  {
    result = 1;
    if ((a1 != r_u32(0x800FF77Cu)))
      return -1;
  }
  return result;
}


/* TODO Missing call adapter sub_800959DC */
uint32 sub_8009426C(uint32 a1, uint32 a2)
{
  sint32 v4;
  sint32 v5;
  unsigned short v6;
  unsigned short v7;
  sint32 v8;
  sint8 v9;
  sint8 v10;
  unsigned char v11;
  sint32 v12;
  uint32 v13;
  sint32 v14;
  uint32 v15;
  sint32 v17;
  sint32 v18;
  sint32 v19;
  sint32 v20;
  sint32 v21;
  uint32 v22;
  sint32 v23;
  sint32 v24;
  sint32 v25;
  v4 = 0;
  v5 = 0;
  if (!(((uint8)(a1))))
    goto LABEL_25;
  v6 = -1;
  do
  {
    v7 = a2;
    v8 = 0;
    v9 = 99;
    v10 = 99;
    v11 = 0;
    if (!r_u8(0x8010561Cu))
      goto LABEL_18;
    v12 = 0;
    while (1)
    {
      if ((((1 << v12) & v5) != 0))
        goto LABEL_17;
      v13 = (0x80104E28u+((27 * v12))*2u);
      if ((!(r_u8((((uint32)(v13))+(29)*1u))) && !r_u16((v13+(3)*2u))))
        break;
      v14 = r_u16((v13+(13)*2u));
      if ((v14 >= v7))
      {
        if ((v14 == v7))
        {
          v15 = ((unsigned short)(r_u16((v13+(3)*2u))));
          if ((v15 >= v6))
          {
            if (((v15 != v6) || (v8 >= r_u16((v13+(1)*2u)))))
              goto LABEL_17;
            v8 = ((unsigned short)(r_u16((v13+(1)*2u))));
          }
          else
          {
            v8 = ((unsigned short)(r_u16((v13+(1)*2u))));
            v6 = r_u16((v13+(3)*2u));
          }
          v9 = v11;
        }
      }
      else
      {
        v7 = r_u16((v13+(13)*2u));
        v6 = r_u16((v13+(3)*2u));
        v8 = ((unsigned short)(r_u16((v13+(1)*2u))));
        v9 = v11;
      }
      LABEL_17:
      v12 = ++v11;

      if ((v11 >= ((uint32)(((unsigned char)(r_u8(0x8010561Cu)))))))
        goto LABEL_18;
    }

    v10 = v11;
    LABEL_18:
    if ((v10 == 99))
    {
      if ((v9 == 99))
        return -1;
      v17 = (1 << v9);
    }
    else
    {
      v17 = (1 << v10);
    }

    v5 |= v17;
    ++v4;
    v6 = -1;
  }
  while ((((unsigned char)(v4)) < ((uint32)(((unsigned char)(a1))))));
  LABEL_25:
  v18 = a1;

  v19 = 0;
  if (r_u8(0x8010561Cu))
  {
    v20 = 0;
    do
    {
      v21 = (1 << v20);
      if ((((1 << v20) & v5) != 0))
      {
        --v18;
        v22 = (0x80104E28u+((27 * v20))*2u);
        v23 = r_u8((((uint32)(v22))+(29)*1u));
        w_u16((v22+(1)*2u),((unsigned char)(v18)));
        w_u16((v22+(13)*2u),a2);
        w_u16((v22+(15)*2u),0);
        w_u16((v22+(21)*2u),0);
        if ((v23 == 2))
          ((void)(0),(void)(v21),abort(),0u);
        w_u8((((uint32)(v22))+(29)*1u),1);
      }
      else
      {
        w_u16((0x80104E28u+(((27 * v20) + 1))*2u),(r_u16((0x80104E28u+(((27 * v20) + 1))*2u))+(((unsigned char)(a1)))));
      }
      v20 = ((unsigned char)(++v19));
    }
    while ((((unsigned char)(v19)) < ((uint32)(((unsigned char)(r_u8(0x8010561Cu)))))));
  }
  v24 = 0;
  v25 = 0;
  do
  {
    ++v24;
    w_u32((0x801054E0u+(v25)*4u),(r_u32((0x801054E0u+(v25)*4u))&(~v5)));
    v25 = ((unsigned char)(v24));
  }
  while ((((unsigned char)(v24)) < 0x10u));
  return v5;
}



uint32 sub_80094570(void)
{
  sint32 result;
  result = 1;
  if (!r_u32(0x80104584u))
    return 255;
  w_u32(0x80104584u,0);
  return result;
}


/* TODO Missing call adapter sub_8008702C */
uint32 sub_800119F0(uint32 a1)
{
  sint32 v2;
  sint32 v3;
  sint32 v4;
  sint32 result;
  sint32 v6;
  v6 = (r_u32((a1+(2)*4u)) - (r_u32((a1+(5)*4u)) << 12));
  v2 = (r_u32((a1+(4)*4u)) * ((sint16)((r_u32((0x800F863Cu+((r_u32((a1+(6)*4u)) & 0xFFF))*4u)))>>16)));
  v3 = r_u32((a1+(3)*4u));
  w_u32(0x800ED520u,((r_u32((a1+(1)*4u)) + (r_u32((a1+(4)*4u)) * ((sint16)((r_u32((0x800F863Cu+((r_u32((a1+(6)*4u)) & 0xFFF))*4u)))>>0)))) >> 12));
  w_u32(0x800ED524u,(v6 >> 12));
  w_u32(0x800ED528u,((v3 + v2) >> 12));
  w_u16(0x800ED548u,-(((short)(((void)(((r_u32((a1+(5)*4u)) << 12) / r_u32((a1+(4)*4u)))),abort(),0u)))));
  v4 = r_u16((((uint32)(a1))+(12)*2u));
  w_u16(0x800ED54Cu,0);
  result = (v4 + 2048);
  w_u16(0x800ED54Au,result);
  return result;
}



uint32 sub_8001551C(uint32 a1, uint32 a2)
{
  sint32 result;
  result = sub_800153F8(a1,a2);
  w_u8(((uint32)(((a1 + (28 * result)) + 37))),0);
  return result;
}



uint32 sub_8001A8E8(void)
{
  sint32 v0;
  uint32 v1;
  uint32 v2;
  uint32 v3;
  sint32 result;
  v0 = 0;
  v1 = r_u32(0x800A593Cu);
  v2 = r_u32(0x800A57FCu);
  v3 = r_u32(0x800A589Cu);
  do
  {
    w_u32(((v1+=4u)-4u),(((r_u32(((v2+=4u)-4u)) * ((sint16)((r_u32((0x800F863Cu+(((((unsigned short)(r_u32(v3))) * ((uint16)(r_u32(0x800FF64Cu)))) & 0xFFF))*4u)))>>0))) / 4096) + 40));
    result = (++v0 < 40);
    (v3+=4u);
  }
  while ((v0 < 40));
  return result;
}


/* TODO Missing call adapter sub_8008128C */
/* TODO Missing call adapter sub_8008790C */
/* TODO Missing call adapter sub_8008793C */
/* TODO Missing call adapter sub_8008798C */
/* TODO Missing call adapter sub_800879AC */
uint32 sub_8007FC60(uint32 a1, uint32 a2)
{
  uint32 v3;
  sint32 v4;
  sint8 v5;
  uint32 v6;
  sint32 result;
  sint32 v8;
  uint32 v9;
  v3 = (a1+(8)*4u);
  v4 = r_u32((a1+(1)*4u));
  w_u32(0x800FFA9Cu,0);
  v5 = sub_800809B0((a1+(8)*4u),v4,a2);
  if (r_u32(0x800FFB3Cu))
    ((void)(v4),abort(),0u);
  v6 = (v3+((2 * v4))*4u);
  result = (v5 & 0xBF);
  if (((v5 & 0xBF) == 0))
  {
    v8 = r_u32((a1+(3)*4u));
    v9 = (v6+((2 * r_u32((a1+(2)*4u))))*4u);
    ((void)(0x800EE6F0u),abort(),0u);
    ((void)(0x800EE710u),abort(),0u);
    ((void)(r_u16(0x800FFA8Cu)),(void)(r_u16(0x800FFA8Eu)),(void)(r_u16(0x800FFA90u)),abort(),0u);
    ((void)(r_u16(0x800FFA94u)),(void)(r_u16(0x800FFA96u)),(void)(r_u16(0x800FFA98u)),abort(),0u);
    return sub_800817FC(v9,v6,v8);
  }
  return result;
}



void sub_800821A4(void)
{
  ;
}



uint32 sub_80082188(uint32 packet, uint32 command, uint32 texture, uint32 flags)
{
  /* TODO Command packing follows the full MIPS arguments */
  w_u32(packet + 4u, command | texture | ((flags & 0x40u) << 19));
  return packet;
}



void sub_800826C4(uint32 position, uint32 count, uint32 stride, uint32 previous, uint32 next)
{
  uint32 difference=next-previous;
  uint32 delta=16u*((difference&0x2000u)-0x1000u)
      - (((uint32)((sint32)difference>>28)&2u)-1u);
  uint32 cursor=position+stride;
  uint32 remaining=count-1u;
  do {
    --remaining;
    w_u32(cursor,r_u32(cursor)+delta);
    cursor+=stride;
  } while (remaining!=0u);
}


/* TODO Missing call adapter sub_80067808 */
uint32 sub_800156E0(uint32 a1)
{
  sint32 v2;
  sint32 v3;
  sint32 v4;
  sint32 result;
  short v6;
  short v7;
  unsigned char v8;
  uint32 v9;
  uint32 v10;
  sint32 v11;
  sint32 v13;
  uint32 v14;
  sint32 v15;
  sint32 v16;
  sint32 v17;
  sint32 v18;
  sint32 v19;
  short v20;
  short v21;
  short v22;
  short v23;
  sint32 v24;
  uint32 v25;
  char v26[104];
  v2 = 0;
  sub_8001A7B0(r_u8(((uint32)((a1 + 4)))));
  v3 = a1;
  v4 = r_u32(((uint32)((a1 + 16))));
  while (1)
  {
    result = (v2 < r_u8(((uint32)((a1 + 10)))));
    if ((v2 >= r_u8(((uint32)((a1 + 10))))))
      return result;
    v4 += r_u8(((uint32)((v3 + 36))));
    if (!(r_u8(((uint32)((v3 + 37))))))
      goto LABEL_37;
    if (((sint16)r_u16(((uint32)((v3 + 28)))) > 0))
    {
      if ((v2 == r_u8(((uint32)((a1 + 6))))))
      {
        sub_8001A7D4(r_u8(((uint32)((v3 + 38)))),r_u8(((uint32)((v3 + 39)))),r_u8(((uint32)((v3 + 40)))),0);
        v6 = ((sint16)r_u16(((uint32)((v3 + 46)))) - 16);
        if (((sint16)r_u16(((uint32)((v3 + 46)))) < 129))
        {
          LABEL_9:
          if (((sint16)r_u16(((uint32)((v3 + 46)))) < 128))
            w_u16(((uint32)((v3 + 46))),255);

        }
        else
        {
          w_u16(((uint32)((v3 + 46))),v6);
          if ((v6 < 128))
          {
            w_u16(((uint32)((v3 + 46))),128);
            goto LABEL_9;
          }
        }
        v7 = ((sint16)r_u16(((uint32)((v3 + 48)))) + 32);
        if (((sint16)r_u16(((uint32)((v3 + 48)))) < 256))
        {
          w_u16(((uint32)((v3 + 48))),v7);
          if ((v7 >= 257))
            w_u16(((uint32)((v3 + 48))),256);
        }
        w_u8(((uint32)((v3 + 44))),0);
      }
      else
      {
        sub_8001A7D4(((unsigned char)(((r_u8(((uint32)((v3 + 41)))) * r_u8(((uint32)((v3 + 44))))) >> 7))),((unsigned char)(((r_u8(((uint32)((v3 + 42)))) * r_u8(((uint32)((v3 + 44))))) >> 7))),((unsigned char)(((r_u8(((uint32)((v3 + 43)))) * r_u8(((uint32)((v3 + 44))))) >> 7))),0);
        v8 = (r_u8(((uint32)((v3 + 44)))) + 6);
        w_u8(((uint32)((v3 + 44))),v8);
        if ((v8 >= 0x81u))
          w_u8(((uint32)((v3 + 44))),0x80);
        if (((sint16)r_u16(((uint32)((v3 + 46)))) < 16))
          w_u16(((uint32)((v3 + 46))),0);
        else
          w_u16(((uint32)((v3 + 46))),((sint16)r_u16(((uint32)((v3 + 46))))-(16)));
        if ((sint16)r_u16(((uint32)((v3 + 46)))))
          w_u16(((uint32)((v3 + 48))),256);
        else
          w_u16(((uint32)((v3 + 48))),0);
      }
      sub_8001A7C8(((sint16)r_u16(((uint32)((v3 + 34)))) >= (sint16)r_u16(((uint32)((v3 + 28))))));
      v9 = r_u32(((uint32)((v3 + 24))));
      if ((r_u8(((uint32)((a1 + 9)))) && !sub_80067724(r_u32(((uint32)((v3 + 24)))),r_u32((0x800A5660u+(0)*4u)))))
      {
        v10 = r_u32(((uint32)((v3 + 24))));
        v11 = 1;
        if (r_u8(v10))
        {
          while (r_u8((v10+(v11++)*1u)))
            ;

        }
        v13 = (v11 - 1);
        v9 = v26;
        ((void)(r_u32(((uint32)((v3 + 24))))),(void)(v26),abort(),0u);
        v26[v13] = 32;
        v14 = &v26[(v13 + 1)];
        w_u8(v14,(v2 + 49));
        w_u8((v14+(1)*1u),0);
      }
      sub_8001A7BC((sint16)r_u16(((uint32)((v3 + 32)))));
      if ((sub_8001A97C(v9) < 491))
        v15 = (sint16)r_u16(((uint32)((v3 + 28))));
      else
        v15 = (sint16)r_u16(((uint32)((v3 + 34))));
      sub_8001A7BC(v15);
      v16 = sub_8001AA28(r_u32(a1+12u),v4,v9,0u,(sint16)r_u16(v3+46u),(sint16)r_u16(v3+48u));
      if (((r_u8(((uint32)((a1 + 7)))) && (v2 == r_u8(((uint32)((a1 + 8)))))) && !(r_u8(((uint32)((a1 + 4)))))))
      {
        v17 = (((v16 * (sint16)r_u16(((uint32)((v3 + 28))))) / 512) + ((14 * (sint16)r_u16(((uint32)((v3 + 28))))) / 256));
        v18 = r_u32(0x800FF668u);
        v19 = (r_u32(0x800FF668u) + 20);
        if ((r_u32(0x800FF374u) >= ((uint32)((r_u32(0x800FF668u) + 40)))))
        {
          w_u32(0x800FF668u,(r_u32(0x800FF668u)+(40)));
          w_u8(((uint32)((v18 + 3))),4);
          w_u8(((uint32)((v18 + 7))),32);
          w_u8(((uint32)((v19 + 3))),4);
          w_u8(((uint32)((v19 + 7))),32);
          w_u16(((uint32)((v18 + 4))),255);
          w_u8(((uint32)((v18 + 6))),0);
          v20 = r_u16(((uint32)((a1 + 12))));
          w_u16(((uint32)((v18 + 10))),v4);
          w_u16(((uint32)((v18 + 8))),(v20 - v17));
          w_u16(((uint32)((v18 + 14))),(v4 - 6));
          v21 = r_u16(((uint32)((v18 + 8))));
          w_u16(((uint32)((v18 + 18))),(v4 + 6));
          v21 -= 20;
          w_u16(((uint32)((v18 + 12))),v21);
          w_u16(((uint32)((v18 + 16))),v21);
          w_u16(((uint32)((v19 + 4))),255);
          w_u8(((uint32)((v19 + 6))),0);
          v22 = r_u16(((uint32)((a1 + 12))));
          w_u16(((uint32)((v19 + 10))),v4);
          w_u16(((uint32)((v19 + 8))),(v22 + v17));
          w_u16(((uint32)((v19 + 14))),(v4 - 6));
          v23 = r_u16(((uint32)((v19 + 8))));
          v24 = r_u32(0x800FF660u);
          w_u16(((uint32)((v19 + 18))),(v4 + 6));
          v23 += 20;
          w_u16(((uint32)((v19 + 12))),v23);
          w_u16(((uint32)((v19 + 16))),v23);
          w_u32(((uint32)(v18)),((r_u32(((uint32)(v18))) & 0xFF000000) | (r_u32(((uint32)((v24 + 112)))) & 0xFFFFFF)));
          v25 = ((r_u32(((uint32)((v24 + 112)))) & 0xFF000000) | (v18 & 0xFFFFFF));
          w_u32(((uint32)((v24 + 112))),v25);
          w_u32(((uint32)((v18 + 20))),((r_u32(((uint32)((v18 + 20)))) & 0xFF000000) | (v25 & 0xFFFFFF)));
          w_u32(((uint32)((v24 + 112))),((r_u32(((uint32)((v24 + 112)))) & 0xFF000000) | (v19 & 0xFFFFFF)));
        }
      }
    }
    v4 += r_u32(((uint32)((a1 + 20))));
    LABEL_37:
    v3 += 28;

    ++v2;
  }

}


/* TODO Postincrement memory expressions may require ordering refinement */
uint32 sub_80015BA4(uint32 a1)
{
  sint32 v2;
  uint32 v3;
  sint32 v4;
  sint32 v5;
  sint32 v6;
  sint32 v7;
  sint32 v8;
  sint32 v9;
  sint8 v10;
  sint32 v11;
  sint32 v12;
  uint32 v13;
  unsigned char v14;
  sint32 v15;
  sint32 result;
  sint32 v17;
  sint32 i;
  short v19;
  short v20;
  while (1)
  {
    v6 = r_u8(((uint32)((a1 + 6))));
    if (r_u8(((uint32)(((a1 + (28 * v6)) + 37)))))
      break;
    w_u16(((uint32)(((a1 + (28 * v6)) + 30))),r_u16(((uint32)(((a1 + (28 * v6)) + 34)))));
    v2 = (a1 + (28 * r_u8(((uint32)((a1 + 6))))));
    w_u16(((uint32)((v2 + 28))),r_u16(((uint32)((v2 + 34)))));
    v3 = r_u8(((uint32)((a1 + 10))));
    v2 = ((v2&0xFFFFFF00u)|((((r_u8(((uint32)((a1 + 6)))) + 1))&0xFFu)<<0));
    w_u8(((uint32)((a1 + 6))),v2);
    if ((((unsigned char)(v2)) >= v3))
      w_u8(((uint32)((a1 + 6))),0);
    v4 = (a1 + (28 * r_u8(((uint32)((a1 + 6))))));
    w_u16(((uint32)((v4 + 30))),r_u16(((uint32)((v4 + 32)))));
    v5 = (a1 + (28 * r_u8(((uint32)((a1 + 6))))));
    w_u16(((uint32)((v5 + 28))),r_u16(((uint32)((v5 + 34)))));
  }

  if ((r_u8(0x800EC198u) || r_u8(0x800EC1A8u)))
  {
    if ((!r_u32(0x800FF014u) || (((sint32)r_u32(0x800FF014u) >= 11) && ((r_u32(0x800FF014u) & 1) == 0))))
    {
      if (r_u8(0x800EC198u))
      {
        v7 = (a1 + (28 * r_u8(((uint32)((a1 + 6))))));
        w_u16(((uint32)((v7 + 30))),r_u16(((uint32)((v7 + 34)))));
        do
        {
          v8 = r_u8(((uint32)((a1 + 6))));
          v9 = (v8 != 0);
          v10 = (v8 - 1);
          if (!v9)
            v10 = (r_u8(((uint32)((a1 + 10)))) - 1);
          w_u8(((uint32)((a1 + 6))),v10);
          v11 = (a1 + (28 * r_u8(((uint32)((a1 + 6))))));
        }
        while (!(r_u8(((uint32)((v11 + 37))))));
        w_u16(((uint32)((v11 + 30))),r_u16(((uint32)((v11 + 32)))));
        sub_80069DF0(26,0x2000,0);
      }
      if (r_u8(0x800EC1A8u))
      {
        v12 = (a1 + (28 * r_u8(((uint32)((a1 + 6))))));
        w_u16(((uint32)((v12 + 30))),r_u16(((uint32)((v12 + 34)))));
        v13 = r_u8(((uint32)((a1 + 10))));
        do
        {
          v14 = (r_u8(((uint32)((a1 + 6)))) + 1);
          w_u8(((uint32)((a1 + 6))),v14);
          if ((v14 >= v13))
            w_u8(((uint32)((a1 + 6))),0);
          v15 = (a1 + (28 * r_u8(((uint32)((a1 + 6))))));
        }
        while (!(r_u8(((uint32)((v15 + 37))))));
        w_u16(((uint32)((v15 + 30))),r_u16(((uint32)((v15 + 32)))));
        sub_80069DF0(26,0x2000,0);
      }
    }
    (w_u32(0x800FF014u,(r_u32(0x800FF014u)+1u)),(r_u32(0x800FF014u)+1u));
  }
  else
  {
    w_u32(0x800FF014u,0);
  }
  result = r_u8(((uint32)((a1 + 10))));
  v17 = 0;
  if (r_u8(((uint32)((a1 + 10)))))
  {
    for (i = a1;; i += 28)
    {
      v19 = ((sint16)r_u16(((uint32)((i + 28)))) + 40);
      if (((sint16)r_u16(((uint32)((i + 28)))) >= (sint16)r_u16(((uint32)((i + 30))))))
        goto LABEL_30;
      w_u16(((uint32)((i + 28))),v19);
      if (((sint16)r_u16(((uint32)((i + 30)))) < v19))
        break;
      LABEL_33:
      result = (++v17 < r_u8(((uint32)((a1 + 10)))));

      if ((v17 >= r_u8(((uint32)((a1 + 10))))))
        return result;
    }

    w_u16(((uint32)((i + 28))),(sint16)r_u16(((uint32)((i + 30)))));
    LABEL_30:
    v20 = ((sint16)r_u16(((uint32)((i + 28)))) - 40);

    if (((sint16)r_u16(((uint32)((i + 30)))) < (sint16)r_u16(((uint32)((i + 28))))))
    {
      w_u16(((uint32)((i + 28))),v20);
      if ((v20 < (sint16)r_u16(((uint32)((i + 30))))))
        w_u16(((uint32)((i + 28))),(sint16)r_u16(((uint32)((i + 30)))));
    }
    goto LABEL_33;
  }
  return result;
}



uint32 sub_800119C4(uint32 a1)
{
  w_u32(((uint32)((a1 + 24))),(r_u32(((uint32)((a1 + 24))))+(4)));
  return sub_800119F0(a1);
}



uint32 sub_80016744(void)
{
  uint32 result;
  sub_8001A7B0(1u);
  sub_8001A7BC(192u);
  sub_8001A7D4(149u,20u,20u,0u);
  sub_8001AA28(348u,210u,r_u32(0x800A5578u),0u,0u,256u);
  sub_8001A7B0(0u);
  result = sub_8006D028(r_u32(0x800FF01Cu));
  if (result) result = sub_8006D0D4(320u,211u,result,r_u32(0x800FF01Cu));
  sub_8001A7BC(256u);
  return result;
}



uint32 sub_8006D1C0(uint32 a1, uint32 a2, uint32 a3, uint32 a4, uint32 red, uint32 green, uint32 blue, uint32 depth)
{
  sint32 v12;
  sint32 v13;
  sint32 v14;
  sint32 result;
  v12 = r_u32(0x800FF668u);
  if ((r_u32(0x800FF374u) < ((uint32)((r_u32(0x800FF668u) + 24)))))
    return 0;
  w_u32(0x800FF668u,(r_u32(0x800FF668u)+(24)));
  w_u8(((uint32)((v12 + 3))),5);
  w_u8(((uint32)((v12 + 7))),40);
  w_u8(((uint32)((v12 + 4))),red);
  w_u8(((uint32)((v12 + 5))),green);
  w_u8(((uint32)((v12 + 6))),blue);
  w_u16(((uint32)((v12 + 18))),(a2 + a4));
  w_u16(((uint32)((v12 + 22))),(a2 + a4));
  v13 = r_u32(0x800FF660u);
  w_u16(((uint32)((v12 + 8))),a1);
  w_u16(((uint32)((v12 + 16))),a1);
  w_u16(((uint32)((v12 + 10))),a2);
  w_u16(((uint32)((v12 + 12))),(a1 + a3));
  w_u16(((uint32)((v12 + 14))),a2);
  w_u16(((uint32)((v12 + 20))),(a1 + a3));
  v14 = ((4 * depth) + v13);
  w_u32(((uint32)(v12)),((r_u32(((uint32)(v12))) & 0xFF000000) | (r_u32(((uint32)((v14 + 112)))) & 0xFFFFFF)));
  result = v12;
  w_u32(((uint32)((v14 + 112))),((r_u32(((uint32)((v14 + 112)))) & 0xFF000000) | (v12 & 0xFFFFFF)));
  return result;
}



uint32 sub_8007FF20(uint32 a1)
{
  uint32 v1;
  uint32 v2;
  sint32 v3;
  sint8 v4;
  v1 = r_u8(((uint32)((a1 + 3))));
  v2 = (v1 >> 4);
  v3 = ((v1 >> 4) != 0);
  v4 = (v1 & 0xF);
  if (v3)
    return r_u16((((uint32)((0x800F5130u+((32 * v2))*4u)))+(((((uint8)(r_u32(0x800FFB48u))) + (4 * v4)) & 0x3F))*2u));
  else
    return 0;
}


/* TODO Missing call adapter sub_80097DF8 */
uint32 sub_8002FAD0(void)
{
  sint32 result;
  char v1[8];
  v1[0] = -56;
  ((void)(14),(void)(v1),(void)(0x800FF2D8u),abort(),0u);
  result = 1;
  if (0x800FF2D8u)
    w_u32(0x800FF2C0u,1);
  return result;
}



uint32 sub_80011594(void)
{
  return sub_8001A7D4(149,20,20,0);
}



uint32 sub_80015614(uint32 a1, uint32 a2, uint32 a3, uint32 a4)
{
  sint32 result;
  sint32 v8;
  sint32 v9;
  w_u32(a3,r_u32(((uint32)((a1 + 12)))));
  w_u32(a4,r_u32(((uint32)((a1 + 16)))));
  result = r_u8(((uint32)((a1 + 10))));
  v8 = 0;
  if (r_u8(((uint32)((a1 + 10)))))
  {
    v9 = a1;
    do
    {
      if (r_u8(((uint32)((v9 + 37)))))
      {
        w_u32(a4,(r_u32(a4)+(r_u8(((uint32)((v9 + 36)))))));
        result = sub_80067724(a2,r_u32(((uint32)((v9 + 24)))));
        if (result)
          return result;
        w_u32(a4,(r_u32(a4)+(r_u32(((uint32)((a1 + 20)))))));
      }
      result = (++v8 < r_u8(((uint32)((a1 + 10)))));
      v9 += 28;
    }
    while ((v8 < r_u8(((uint32)((a1 + 10))))));
  }
  return result;
}



void sub_80011B04(void)
{
  sub_8006EE2C();
  sub_800691B8();
  sub_800653F4();
}



uint32 sub_80015F7C(uint32 a1)
{
  sint32 result;
  w_u32(((uint32)(a1)),r_u32(0x800FF384u));
  w_u32(((uint32)((a1 + 4))),r_u16(0x800ECC7Au));
  w_u32(((uint32)((a1 + 8))),r_u16(0x800ECC78u));
  w_u32(((uint32)((a1 + 12))),r_u16(0x800ECC7Cu));
  w_u8(((uint32)((a1 + 16))),r_u16(0x800EC47Au));
  w_u8(((uint32)((a1 + 17))),r_u16(0x800EC474u));
  w_u8(((uint32)((a1 + 18))),r_u16(0x800EC476u));
  w_u8(((uint32)((a1 + 19))),r_u16(0x800EC478u));
  w_u8(((uint32)((a1 + 20))),r_u16(0x800EC49Au));
  w_u8(((uint32)((a1 + 21))),r_u16(0x800EC494u));
  w_u8(((uint32)((a1 + 22))),r_u16(0x800EC496u));
  result = ((unsigned char)(r_u16(0x800EC498u)));
  w_u8(((uint32)((a1 + 23))),r_u16(0x800EC498u));
  return result;
}



uint32 sub_80018C30(uint32 a1)
{
  uint32 v2;
  sint32 v3;
  sint32 v4;
  sint32 v5;
  uint32 result;
  w_u32(0x800FF83Cu,((uint32)((a1 + 4))));
  sub_8006654C(312921176);
  v2 = sub_80018948(((sint32)(r_u32(0x800FF83Cu))));
  sub_80016354(((sint32)(r_u32(0x800FF83Cu))),((sint32)(r_u32((v2+(4)*4u)))));
  if (v2)
  {
    sub_8006A3F0();
    sub_80069A94();
    sub_80069B8C();
    sub_80069B0C(r_u32((v2+(3)*4u)));
    sub_80069BC4(r_u32((v2+(2)*4u)));
  }
  sub_8006AED8(2000);
  sub_80063F4C(r_u32(0x800FF83Cu));
  sub_80016554();
  sub_8006AED8(4);
  v3 = sub_800625AC(656);
  v4 = v3;
  if (v3)
    v4 = sub_8005C5C0(v3);
  w_u16(((uint32)((v4 + 218))),r_u16(((uint32)((a1 + 100)))));
  sub_80018B70(a1);
  if (r_u8(((uint32)((a1 + 13)))))
    sub_80063DD4((a1 + 13));
  w_u32(0x800FF874u,r_u32(((uint32)((a1 + 108)))));
  if ((r_u32(0x800FF874u) < r_u16(0x800EC522u)))
    w_u32(0x800FF874u,r_u16(0x800EC522u));
  v5 = sub_800625AC(580);
  if (v5)
    sub_800772B8(v5,v4);
  sub_80063E7C();
  sub_800189EC();
  sub_8006CDC4();
  w_u32(((uint32)((r_u32(0x800FF904u) + 564))),3);
  w_u8(0x800A72B8u,1);
  w_u8(0x800A72B9u,0);
  w_u8(0x800A72BAu,0);
  w_u8(0x800A72BBu,0);
  w_u8(0x800C6328u,1);
  result = 0x800C6310u;
  w_u8(0x800C6329u,0);
  w_u8(0x800C632Au,0);
  w_u8(0x800C632Bu,0);
  return result;
}



uint32 sub_80018948(uint32 a1)
{
  sint32 v2;
  uint32 v3;
  uint32 result;
  v2 = 0;
  if (r_u32(r_u32(0x800A5258u)))
  {
    v3 = ((uint32)(0x800A5254u));
    while (!sub_80067724(r_u32((v3+(1)*4u)),a1))
    {
      v3 += (6)*4u;
      result = 0;
      if ((((sint32)(v3)) >= ((sint32)(r_u32(0x800A562Cu)))))
        return result;
      if (!(r_u8(r_u32((v3+(1)*4u)))))
        return ((uint32)(v2));
    }

    return v3;
  }
  return ((uint32)(v2));
}



uint32 sub_80069B0C(uint32 a1)
{
  uint32 i;
  sint32 v2;
  uint32 v3;
  sint32 result;
  char v5[24];
  for (i = v5;; (i+=1u))
  {
    v2 = r_u32(((a1+=1u)-1u));
    if (!v2)
      break;
    w_u8(i,v2);
  }

  w_u8(i,46);
  v3 = (i+(1)*1u);
  w_u32(((v3+=1u)-1u),86);
  w_u8(v3,65);
  w_u16(((uint32)((v3+(1)*1u))),66);
  result = sub_80069D2C(v5,0x800FF6D4u);
  w_u32(0x800FF6A0u,result);
  return result;
}



uint32 sub_80063C54(uint32 a1)
{
  sint32 v1;
  sint32 result;
  if ((a1 == 0xFFFF))
    return 0;
  if ((r_u16(r_u32(((uint32)(((4 * a1) + r_u32(0x800FF624u)))))) != 6))
    return 0;
  v1 = r_u32(0x800FF630u);
  if (!r_u32(0x800FF630u))
    return 0;
  while (1)
  {
    result = v1;
    if ((r_u16(((uint32)((v1 + 10)))) == a1))
      break;
    v1 = r_u32(((uint32)((v1 + 20))));
    if (!v1)
      return 0;
  }

  return result;
}



uint32 sub_80063DD4(uint32 a1)
{
  sint32 v2;
  sint32 result;
  sint32 v4;
  char v5[16];
  v2 = 0;
  w_u32(0x800FF620u,0xFFFF);
  while (1)
  {
    result = (4 * v2);
    if ((v2 >= r_u32(0x800FF628u)))
      break;
    if ((r_u16(r_u32(((uint32)((result + r_u32(0x800FF624u)))))) == 8))
    {
      v4 = sub_8006613C(v5,v2);
      result = sub_80067724((v4 + 6),a1);
      if (result)
      {
        w_u32(0x800FF620u,v2);
        return result;
      }
    }
    ++v2;
  }

  return result;
}



uint32 sub_80069540(uint32 a1, uint32 a2)
{
  sint32 v2;
  sint32 v3;
  v2 = r_u32(((uint32)((a1 + 16))));
  if (v2)
    w_u32(((uint32)((v2 + 20))),r_u32(((uint32)((a1 + 20)))));
  v3 = r_u32(((uint32)((a1 + 20))));
  if (v3)
    w_u32(((uint32)((v3 + 16))),r_u32(((uint32)((a1 + 16)))));
  if ((a1 == r_u32(0x800FF680u)))
    w_u32(0x800FF680u,r_u32(((uint32)((a1 + 16)))));
  return sub_8006BC20(a1);
}



void sub_80016554(void)
{
  w_u32(0x800FF050u,0);
}



uint32 sub_80062924(uint32 a1)
{
  sint32 result;
  short v3;
  sub_80062628(a1);
  w_u32(((uint32)((a1 + 68))),0x800A3380u);
  w_u8(((uint32)((a1 + 129))),1);
  w_u8(((uint32)((a1 + 130))),1);
  w_u8(((uint32)((a1 + 131))),1);
  w_u8(((uint32)((a1 + 144))),1);
  w_u8(((uint32)((a1 + 145))),1);
  w_u8(((uint32)((a1 + 146))),1);
  w_u32(((uint32)((a1 + 208))),10);
  w_u16(((uint32)((a1 + 148))),0);
  w_u16(((uint32)((a1 + 214))),-1);
  result = a1;
  w_u16(((uint32)((a1 + 200))),r_u16(0x800EC682u));
  v3 = r_u16(0x800EC680u);
  w_u16(((uint32)((a1 + 78))),(r_u16(((uint32)((a1 + 78))))|(0x12u)));
  w_u16(((uint32)((a1 + 212))),50);
  w_u16(((uint32)((a1 + 204))),v3);
  return result;
}



uint32 sub_80062628(uint32 a1)
{
  sint32 result;
  uint32 v2;
  result = a1;
  w_u32(((uint32)((a1 + 68))),0x800A33C0u);
  v2 = ((uint32)((a1 + 36)));
  w_u16(v2,4096);
  w_u16((v2+(1)*2u),4096);
  w_u16((v2+(2)*2u),4096);
  return result;
}





