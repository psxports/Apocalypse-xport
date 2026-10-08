#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
#include <stdlib.h>
/* Unverified draft; TODO Recover omitted call arguments, host-buffer adapters and signed field widths */
/* TODO Missing call adapter sub_80077274 */
/* TODO Missing call adapter sub_800875DC */
uint32 sub_80077748(uint32 a1)
{
  sint32 v2;
  sint32 v3;
  uint32 v4;
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
  sint32 v23;
  sint32 v24;
  sint32 v25;
  sint32 v26;
  sint32 v27;
  sint32 v28;
  sint32 v29;
  sint32 v30;
  sint32 v31;
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
  sint32 v48;
  sint32 result;
  sint32 v50;
  sint32 v51;
  short v52;
  sint32 v53;
  sint32 v54;
  sint32 v55;
  short v56;
  sint32 v57;
  sint32 v58;
  sint32 v59;
  sint32 v60;
  sint32 v61;
  char v62[32];
  sint32 v63;
  sint32 v64;
  sint32 v65;
  sint32 v66;
  sint32 v67;
  sint32 v68;
  int v69[2];
  sint32 v70;
  sint32 v71;
  sint32 v72;
  sint32 v73;
  int v74[2];
  sint32 v75;
  int v76[4];
  sint32 v77;
  sint32 v78;
  sint32 v79;
  int v80[2];
  int v81[2];
  sint32 v82;
  sint32 v83;
  sint32 v84;
  sint32 v85;
  sint32 v86;
  sint32 v87;
  sint32 v88;
  sint32 v89;
  sint32 v90;
  sint32 v91;
  sint32 v92;
  sint32 v93;
  int v94[4];
  sint32 v95;
  sint32 v96;
  uint32 v97[6];
  sint32 v98;
  sint8 v99;
  sint32 v100;
  sint32 v101;
  sint32 v102;
  sint32 v103;
  uint32 v104[4];
  sint32 v105;
  sint32 v106;
  v2 = r_u32(((uint32)((a1 + 8))));
  v3 = r_u32(((uint32)((a1 + 12))));
  v76[0] = r_u32(((uint32)((a1 + 4))));
  v76[1] = v2;
  v76[2] = v3;
  v4 = ((uint32)((a1 + 460)));
  if (r_u32(0x800FF008u))
    goto LABEL_25;
  v5 = r_u32(0x800FF8FCu);
  v6 = r_u32(((uint32)((a1 + 432))));
  v7 = r_u32(((uint32)((a1 + 436))));
  v8 = r_u32(((uint32)((a1 + 440))));
  w_u32(((uint32)((a1 + 460))),r_u32(((uint32)((a1 + 428)))));
  w_u32(((uint32)((a1 + 464))),v6);
  w_u32(((uint32)((a1 + 468))),v7);
  w_u32(((uint32)((a1 + 472))),v8);
  v63 = 0;
  v64 = 0;
  v65 = (-4096 * v5);
  sub_80076310(((uint32)((a1 + 444))),((sint32)(v62)));
  ((void)(v62),(void)(&v63),(void)(&v66),abort(),0u);
  v9 = r_u32(((uint32)((a1 + 540))));
  if ((v9 == -1))
  {
    v74[1] = 0;
    v74[0] = v66;
    v75 = v68;
  }
  else
    if ((v9 > 0))
  {
    v63 = 0;
    v64 = 0;
    v65 = (-4096 * r_u32(((uint32)((a1 + 540)))));
    ((void)(v62),(void)(&v63),(void)(v74),abort(),0u);
  }
  v10 = r_u32(((uint32)((a1 + 536))));
  if ((v10 == -1))
  {
    v11 = r_u32(0x800FF8FCu);
    v65 = 0;
    v64 = 0;
    LABEL_10:
    v63 = (-4096 * v11);

    ((void)(v62),(void)(&v63),(void)(v69),abort(),0u);
    v71 = -v69[0];
    v73 = -v70;
    v72 = -v69[1];
    goto LABEL_11;
  }
  if ((v10 > 0))
  {
    v65 = 0;
    v64 = 0;
    v11 = r_u32(((uint32)((a1 + 536))));
    goto LABEL_10;
  }
  LABEL_11:
  v77 = (r_u32(((uint32)((a1 + 308)))) + v66);

  v78 = (r_u32(((uint32)((a1 + 312)))) + v67);
  v79 = (r_u32(((uint32)((a1 + 316)))) + v68);
  w_u32(0x801028C8u,v77);
  w_u32(0x801028CCu,v78);
  w_u32(0x801028D0u,v79);
  sub_8006C3AC(v80,&v77,v76);
  if (r_u32(((uint32)((a1 + 536)))))
  {
    v82 = r_u32(((uint32)((a1 + 308))));
    v83 = r_u32(((uint32)((a1 + 312))));
    v84 = r_u32(((uint32)((a1 + 316))));
    v85 = (r_u32(((uint32)((a1 + 308)))) + v69[0]);
    v86 = r_u32(((uint32)((a1 + 312))));
    v87 = (r_u32(((uint32)((a1 + 316)))) + v70);
    sub_8007BB24(&v82);
    v99 = 0;
    w_u32(0x800FF96Cu,1);
    sub_8007DD04(&v82,1);
    w_u32(0x800FF96Cu,0);
    if (v98)
    {
      if (!v96)
        v96 = 1;
      sub_800762A8(&v100,((short)(((r_u16(((uint32)((a1 + 544)))) * (((((v96 - v95) << 12) / v96) * (((v96 - v95) << 12) / v96)) >> 12)) >> 12))));
      v12 = r_u32(((uint32)((a1 + 456))));
      v13 = r_u32(((uint32)((a1 + 444))));
      v14 = (v12 * v103);
      v15 = r_u32(((uint32)((a1 + 448))));
      v16 = (v13 * v100);
      v17 = (v15 * v101);
      v18 = (v12 * v100);
      v19 = (v13 * v103);
      v20 = v102;
      v21 = r_u32(((uint32)((a1 + 452))));
      v22 = (v12 * v101);
      v106 = (v21 * v101);
      v23 = (v21 * v100);
      v24 = (v13 * v102);
      v25 = (v12 * v102);
      v26 = (v21 * v103);
      v27 = (v13 * v101);
      v28 = (v15 * v100);
      v105 = (v15 * v103);
      w_u32(((uint32)((a1 + 444))),((((v18 + v19) + (v15 * v102)) - (v21 * v101)) >> 12));
      w_u32(((uint32)((a1 + 448))),((((v22 + v105) + v23) - v24) >> 12));
      w_u32(((uint32)((a1 + 452))),((((v25 + v26) + v27) - v28) >> 12));
      w_u32(((uint32)((a1 + 456))),((((v14 - v16) - v17) - (v21 * v20)) >> 12));
    }
    v82 = r_u32(((uint32)((a1 + 308))));
    v83 = r_u32(((uint32)((a1 + 312))));
    v84 = r_u32(((uint32)((a1 + 316))));
    v85 = (r_u32(((uint32)((a1 + 308)))) + v71);
    v86 = r_u32(((uint32)((a1 + 312))));
    v87 = (r_u32(((uint32)((a1 + 316)))) + v73);
    sub_8007BB24(&v82);
    v99 = 0;
    w_u32(0x800FF96Cu,1);
    sub_8007DD04(&v82,1);
    w_u32(0x800FF96Cu,0);
    if (v98)
    {
      if (!v96)
        v96 = 1;
      sub_800762A8(&v100,-(((short)(((r_u16(((uint32)((a1 + 544)))) * (((((v96 - v95) << 12) / v96) * (((v96 - v95) << 12) / v96)) >> 12)) >> 12)))));
      v29 = r_u32(((uint32)((a1 + 456))));
      v30 = r_u32(((uint32)((a1 + 444))));
      v31 = (v29 * v103);
      v32 = r_u32(((uint32)((a1 + 448))));
      v33 = (v30 * v100);
      v34 = (v32 * v101);
      v35 = (v29 * v100);
      v36 = (v30 * v103);
      v37 = v102;
      v38 = r_u32(((uint32)((a1 + 452))));
      v39 = (v29 * v101);
      v106 = (v38 * v101);
      v40 = (v38 * v100);
      v41 = (v30 * v102);
      v42 = (v29 * v102);
      v43 = (v38 * v103);
      v44 = (v30 * v101);
      v45 = (v32 * v100);
      v105 = (v32 * v103);
      w_u32(((uint32)((a1 + 444))),((((v35 + v36) + (v32 * v102)) - (v38 * v101)) >> 12));
      w_u32(((uint32)((a1 + 448))),((((v39 + v105) + v40) - v41) >> 12));
      w_u32(((uint32)((a1 + 452))),((((v42 + v43) + v44) - v45) >> 12));
      w_u32(((uint32)((a1 + 456))),((((v31 - v33) - v34) - (v38 * v37)) >> 12));
    }
  }
  v4 = ((uint32)((a1 + 460)));
  if (r_u32(((uint32)((a1 + 540)))))
  {
    v82 = r_u32(((uint32)((a1 + 308))));
    v83 = r_u32(((uint32)((a1 + 312))));
    v84 = r_u32(((uint32)((a1 + 316))));
    v85 = (r_u32(((uint32)((a1 + 308)))) + (2 * v74[0]));
    v86 = r_u32(((uint32)((a1 + 312))));
    v87 = (r_u32(((uint32)((a1 + 316)))) + (2 * v75));
    sub_8007BB24(&v82);
    v99 = 0;
    w_u32(0x800FF96Cu,1);
    sub_8007DD04(&v82,1);
    w_u32(0x800FF96Cu,0);
    v4 = ((uint32)((a1 + 460)));
    if (v98)
    {
      if (!v96)
        v96 = 1;
      sub_80076274(v104,((short)(((r_u16(((uint32)((a1 + 546)))) * (((((v96 - v95) << 12) / v96) * (((v96 - v95) << 12) / v96)) >> 12)) >> 12))));
      sub_80075F80(&v100,((uint32)((a1 + 444))),v104);
      v46 = v101;
      v47 = v102;
      v48 = v103;
      w_u32(((uint32)((a1 + 444))),v100);
      w_u32(((uint32)((a1 + 448))),v46);
      w_u32(((uint32)((a1 + 452))),v47);
      w_u32(((uint32)((a1 + 456))),v48);
      v4 = ((uint32)((a1 + 460)));
    }
  }
  LABEL_25:
  sub_80076800(v4,((uint32)((a1 + 444))),1023,((uint32)((a1 + 428))));

  v63 = 0;
  v64 = 0;
  v65 = (-4096 * r_u32(0x800FF8FCu));
  sub_80076310(((uint32)((a1 + 428))),((sint32)(v62)));
  ((void)(v62),(void)(&v63),(void)(&v66),abort(),0u);
  w_u32(((uint32)((a1 + 4))),(r_u32(((uint32)((a1 + 308)))) + v66));
  w_u32(((uint32)((a1 + 8))),(r_u32(((uint32)((a1 + 312)))) + v67));
  w_u32(((uint32)((a1 + 12))),(r_u32(((uint32)((a1 + 316)))) + v68));
  sub_8006C3AC(v80,((uint32)((a1 + 4))),v76);
  result = r_u32(0x800FF008u);
  v50 = v80[1];
  v51 = v81[0];
  w_u32(((uint32)((a1 + 104))),v80[0]);
  w_u32(((uint32)((a1 + 108))),v50);
  w_u32(((uint32)((a1 + 112))),v51);
  if (!result)
  {
    result = r_u32(((uint32)((a1 + 336))));
    if ((r_u32(((uint32)((a1 + 296)))) != result))
    {
      sub_80066B8C(((uint32)(v80)),((uint32)((a1 + 308))),&v77);
      sub_80066B8C(((uint32)(v81)),((uint32)((r_u32(0x800FF4ECu) + 4))),&v77);
      v52 = ((void)(((sint16)((v80[0])>>16))),(void)(((sint16)((v81[0])>>16))),abort(),0u);
      v82 = 0;
      v83 = 0;
      v84 = 0;
      v85 = 4096;
      v86 = 0;
      v87 = 0;
      v88 = 0;
      v89 = 4096;
      v53 = r_u32(((uint32)((a1 + 432))));
      v54 = r_u32(((uint32)((a1 + 436))));
      v55 = r_u32(((uint32)((a1 + 440))));
      w_u32(((uint32)((a1 + 476))),r_u32(((uint32)((a1 + 428)))));
      w_u32(((uint32)((a1 + 480))),v53);
      w_u32(((uint32)((a1 + 484))),v54);
      w_u32(((uint32)((a1 + 488))),v55);
      v56 = v52;
      sub_80066B8C(((uint32)(v80)),((uint32)((a1 + 4))),((uint32)((a1 + 308))));
      sub_80066B8C(((uint32)(v81)),((uint32)((a1 + 4))),((uint32)((r_u32(0x800FF4ECu) + 4))));
      v57 = ((short)((4096 - v56)));
      v58 = ((((v81[0]>>0)&65535u) - ((v80[0]>>0)&65535u)) & 0xFFF);
      sub_800762A8(&v95,v57);
      sub_80075F80(v94,&v95,((uint32)((a1 + 428))));
      sub_80076274(v97,v58);
      sub_80075F80(&v90,v94,v97);
      v82 = v90;
      v83 = v91;
      v84 = v92;
      v85 = v93;
      v59 = r_u32(((uint32)((a1 + 432))));
      v60 = r_u32(((uint32)((a1 + 436))));
      v61 = r_u32(((uint32)((a1 + 440))));
      v90 = r_u32(((uint32)((a1 + 428))));
      v91 = v59;
      v92 = v60;
      v93 = v61;
      return sub_80076800(&v90,&v82,1023,((uint32)((a1 + 428))));
    }
  }
  return result;
}



void sub_80082750(uint32 count, uint32 corner0, uint32 corner1, uint32 corner2, uint32 corner3)
{
  uint32 position=0x1F800000u;
  sub_800826C4(position,count,16u,corner0,corner1);
  position+=count*16u;
  sub_800826C4(position,count,128u,corner1,corner3);
  position+=count*128u;
  sub_800826C4(position,count,0xFFFFFFF0u,corner3,corner2);
  position-=count*16u;
  sub_800826C4(position,count,0xFFFFFF80u,corner2,corner0);
}


/* TODO Missing call adapter indirect */
uint32 sub_8005CF0C(uint32 a1)
{
  uint32 v2;
  sint32 v3;
  sint32 v4;
  sint32 v5;
  signed int v6;
  sint32 v7;
  uint32 v8;
  short v9;
  sint32 v10;
  uint32 v11;
  short v12;
  short v13;
  sint32 result;
  char v15[16];
  short v16[8];
  unsigned char v17;
  unsigned char v18;
  char v19[2];
  sint32 v20;
  w_u16(0x800FFBB4u,0);
  w_u16(0x800FFBB6u,-4096);
  w_u16(0x800FFBB8u,0);
  w_u16(0x800FFBBCu,0);
  w_u16(0x800FFBBEu,4096);
  w_u16(0x800FFBC0u,0);
  w_u16(0x800FFBC4u,0);
  w_u16(0x800FFBC6u,0);
  w_u16(0x800FFBC8u,0);
  w_u16(0x800FFBD0u,4096);
  w_u16(0x800FFBCEu,4096);
  w_u16(0x800FFBCCu,4096);
  v2 = r_u32(((uint32)((a1 + 32))));
  v3 = ((unsigned char)(v2));
  v4 = ((v2>>8)&255u);
  v5 = ((v2>>16)&255u);
  v6 = ((v2>>8)&255u);
  if ((((uint32)((v6 - 1))) < 0x7F))
  {
    v3 = ((v3 * (0x80000 / v6)) >> 12);
    v4 = ((v4 * (0x80000 / v6)) >> 12);
    v5 = ((v5 * (0x80000 / v6)) >> 12);
  }
  w_u32(0x800FFBD4u,((0x800FFBD4u&0xFFFF0000u)|((((16 * v3))&0xFFFFu)<<0)));
  w_u32(0x800FFBD4u,((0x800FFBD4u&0x0000FFFFu)|((((16 * v4))&0xFFFFu)<<16)));
  w_u32(0x800FFBD8u,((r_u32(0x800FFBD8u)&0xFFFF0000u)|((((16 * v5))&0xFFFFu)<<0)));
  w_u32(0x800FFBDCu,0x800FFBD4u);
  w_u32(0x800FFBE0u,r_u32(0x800FFBD8u));
  v7 = r_u32(((uint32)(((a1 + (4 * r_u32(((uint32)((a1 + 644)))))) + 612))));
  if (((void)((v7 + r_u16(((uint32)((r_u32(((uint32)(v7))) + 32)))))),(void)(&v17),(void)(&v18),(void)(v19),abort(),0u))
  {
    sub_8006C3AC(v16,(a1 + 416),(a1 + 4));
    v20 = 12;
    sub_8006C564(v15,v16,&v20);
    sub_800666DC(v16,v15);
    w_u16(0x800FFBCCu,(16 * v17));
    w_u16(0x800FFBB4u,v16[0]);
    w_u16(0x800FFBB6u,0);
    w_u16(0x800FFBB8u,v16[4]);
    w_u16(0x800FFBCEu,(16 * v18));
    w_u16(0x800FFBD0u,(16 * ((unsigned char)(v19[0]))));
  }
  else
  {
    w_u16(0x800FFBB8u,0);
    w_u16(0x800FFBB4u,0);
    w_u16(0x800FFBB6u,-4096);
    v8 = r_u8(((uint32)((a1 + 35))));
    v9 = (16 * v8);
    if ((v8 < 0x40))
      v9 = 1024;
    w_u16(0x800FFBD0u,v9);
    w_u16(0x800FFBCEu,v9);
    w_u16(0x800FFBCCu,v9);
  }
  if (r_u32(0x800FF904u))
    v10 = (r_u16(((uint32)((r_u32(0x800FF904u) + 494)))) & 0xFFF);
  else
    v10 = 0;
  v11 = (0x800F863Cu+(v10)*4u);
  v12 = r_u16(((uint32)(v11)));
  w_u16(0x800FFBC6u,0);
  w_u16(0x800FFBC4u,v12);
  v13 = r_u16((((uint32)(v11))+(1)*2u));
  w_u16(0x800EE6F0u,r_u16(0x800FFBB4u));
  w_u16(0x800EE6F4u,r_u16(0x800FFBB8u));
  w_u16(0x800EE6F6u,r_u16(0x800FFBBCu));
  w_u16(0x800EE6F8u,r_u16(0x800FFBBEu));
  w_u16(0x800EE6FAu,r_u16(0x800FFBC0u));
  w_u16(0x800EE6FCu,v12);
  w_u16(0x800EE6FEu,0);
  w_u16(0x800EE6F2u,r_u16(0x800FFBB6u));
  w_u16(0x800EE710u,r_u16(0x800FFBCCu));
  w_u16(0x800EE714u,0x800FFBDCu);
  w_u16(0x800EE716u,r_u16(0x800FFBCEu));
  w_u16(0x800EE718u,((0x800FFBD4u>>16)&65535u));
  w_u16(0x800EE71Au,((0x800FFBDCu>>16)&65535u));
  w_u16(0x800EE712u,0x800FFBD4u);
  w_u16(0x800FFBC8u,v13);
  w_u16(0x800EE700u,v13);
  w_u16(0x800EE71Cu,r_u16(0x800FFBD0u));
  result = 64;
  w_u16(0x800FFA98u,64);
  w_u16(0x800FFA96u,64);
  w_u16(0x800FFA94u,64);
  w_u16(0x800EE71Eu,r_u32(0x800FFBD8u));
  w_u16(0x800EE720u,r_u32(0x800FFBE0u));
  return result;
}



uint32 sub_8002349C(uint32 a1, uint32 a2, uint32 a3, uint32 a4)
{
  sint32 result;
  result = 1;
  if (!(r_u32(((uint32)((a1 + 84))))))
    return 0;
  w_u8(a2,-1);
  w_u8(a3,0);
  w_u8(a4,0);
  w_u32(((uint32)((a1 + 84))),0);
  return result;
}


/* TODO Missing call adapter sub_8008128C */
uint32 sub_8007FB34(uint32 a1)
{
  uint32 v2;
  sint32 v3;
  sint8 v4;
  sint32 result;
  uint32 v6;
  v2 = (a1+(8)*4u);
  v3 = r_u32((a1+(1)*4u));
  w_u32(0x800FFA9Cu,1);
  v4 = sub_80080EF4((a1+(8)*4u),v3);
  if (r_u32(0x800FFB3Cu))
    ((void)(v3),abort(),0u);
  result = (8 * v3);
  v6 = (v2+((2 * v3))*4u);
  if (!r_u32(0x800FFB38u))
  {
    result = (v4 & 0xBF);
    if (((v4 & 0xBF) == 0))
      return sub_800817FC((v6+((2 * r_u32((a1+(2)*4u))))*4u),v6,r_u32((a1+(3)*4u)));
  }
  return result;
}



void sub_80085C34(uint32 a1, uint32 a2)
{
  sint32 v2;
  sint32 v3;
  sint32 v4;
  sint32 v5;
  short v6;
  v2 = r_u32(a1);
  v3 = r_u32((a1+(1)*4u));
  v4 = r_u32((a1+(2)*4u));
  v5 = r_u32((a1+(3)*4u));
  v6 = r_u16((((uint32)(a1))+(8)*2u));
  w_u32(((uint32)(a2)),(((unsigned short)((r_u32(a1) ^ v3))) ^ v3));
  w_u32(((uint32)((a2 + 4))),(((unsigned short)((v5 ^ v2))) ^ v2));
  w_u32(((uint32)((a2 + 8))),(((unsigned short)((v4 ^ v5))) ^ v5));
  w_u32(((uint32)((a2 + 12))),(((unsigned short)((v3 ^ v4))) ^ v4));
  w_u16(((uint32)((a2 + 16))),v6);
}


/* TODO Missing call adapter sub_8007FA7C */
/* TODO Missing call adapter sub_8007FD3C */
/* TODO Missing call adapter sub_800878DC */
uint32 sub_8007F138(uint32 a1)
{
  sint32 v1;
  uint32 v2;
  sint32 v3;
  sint32 v4;
  sint32 result;
  char v6[32];
  char v7[32];
  v1 = a1;
  if (a1)
  {
    w_u32(0x1F800164,0x7FFFFFFF);
    do
    {
      if ((((r_u16(((uint32)(v1))) & 0x8001) == 0) && ((r_u32((0x800EAEF8u+(((16 * r_u8(((uint32)((v1 + 27))))) + 2))*4u))>>16)&255u)))
      {
        v2 = r_u32(0x800FFAA0u);
        if (((r_u16(((uint32)(v1))) & 0x400) != 0))
        {
          v2 = ((r_u32(0x800FFAA0u) & 0xFFFB0000) | 8);
          v3 = r_u32(((uint32)((v1 + 32))));
          w_u32(0x800FFB20u,(v3 & 0xFFFFFF));
          w_u32(0x1F800154,(v3 & 0xFFFFFF));
          w_u32(0x1F8001A4,(((unsigned char)(v3)) << 6));
          w_u32(0x1F8001B4,(((unsigned short)((v3 & 0xFF00))) >> 2));
          w_u32(0x1F8001C4,(((v3 & 0xFFFFFFu) >> 10) & 0x3FC0));
        }
        if (((r_u16(((uint32)(v1))) & 0x800) != 0))
          v2 = (((v2 & 0xFFFFFE7F) | (r_u32(((uint32)((v1 + 44)))) & 0x180)) | 0x40);
        w_u32(0x1F800144,v2);
        ((void)((r_u32(0x800FFB0Cu) + 116)),abort(),0u);
        if ((r_u32(((uint32)((v1 + 16)))) || r_u16(((uint32)((v1 + 20))))))
        {
          sub_800858FC((v1 + 16),v7);
          sub_800854F4(v7,v6);
          ((void)(v6),abort(),0u);
        }
        else
        {
          sub_800854D8(v7);
        }
        w_u32(0x800FFB04u,r_u32((0x800EAEF8u+(((16 * r_u8(((uint32)((v1 + 27))))) + 8))*4u)));
        v4 = r_u32(((uint32)(((4 * r_u16(((uint32)((v1 + 22))))) + r_u32((0x800EAEF8u+(((16 * r_u8(((uint32)((v1 + 27))))) + 4))*4u))))));
        if ((r_u32(((uint32)((v1 + 16)))) || r_u16(((uint32)((v1 + 20))))))
          ((void)(v4),abort(),0u);
        else
          ((void)(v4),abort(),0u);
      }
      v1 = r_u32(((uint32)((v1 + 28))));
    }
    while (v1);
    result = r_u32(0x800FFACCu);
    w_u32(0x1F800164,r_u32(0x800FFACCu));
  }
  return result;
}



uint32 sub_8001C004(void)
{
  sint32 result;
  sint32 v1;
  sint32 v2;
  uint32 v3;
  sint32 v4;
  sint8 v5;
  sint8 v6;
  sint8 v7;
  sint32 v8;
  sint32 v9;
  if ((r_u32(0x800FF1E4u) || (result = r_u32(0x800FF1E8u) != 0)))
  {
    v1 = r_u32(0x800FF668u);
    v2 = ((r_u32(0x800FF660u) + (4 * r_u32(0x800FF1E0u))) + 112);
    v3 = ((uint32)((v2 + 4)));
    result = (r_u32(0x800FF374u) < ((uint32)((r_u32(0x800FF668u) + 32))));
    v4 = (r_u32(0x800FF668u) + 24);
    if ((r_u32(0x800FF374u) >= ((uint32)((r_u32(0x800FF668u) + 32)))))
    {
      w_u32(0x800FF668u,(r_u32(0x800FF668u)+(32)));
      w_u8(((uint32)((v1 + 3))),5);
      w_u8(((uint32)((v1 + 7))),42);
      v5 = ((r_u32(0x800FFB60u)>>16)&255u);
      w_u16(((uint32)((v1 + 8))),0);
      w_u16(((uint32)((v1 + 10))),0);
      w_u16(((uint32)((v1 + 14))),0);
      w_u16(((uint32)((v1 + 16))),0);
      w_u8(((uint32)((v1 + 4))),v5);
      v6 = ((r_u32(0x800FFB64u)>>16)&255u);
      w_u16(((uint32)((v1 + 12))),512);
      w_u16(((uint32)((v1 + 20))),512);
      w_u8(((uint32)((v1 + 5))),v6);
      v7 = ((r_u32(0x800FFB68u)>>16)&255u);
      w_u16(((uint32)((v1 + 18))),240);
      w_u16(((uint32)((v1 + 22))),240);
      w_u8(((uint32)((v1 + 6))),v7);
      w_u32(((uint32)(v1)),((r_u32(((uint32)(v1))) & 0xFF000000) | (r_u32(((uint32)((v2 + 4)))) & 0xFFFFFF)));
      v8 = r_u32(0x800FF1E8u);
      w_u32(((uint32)((v2 + 4))),((r_u32(((uint32)((v2 + 4)))) & 0xFF000000) | (v1 & 0xFFFFFF)));
      w_u32(((uint32)((v1 + 24))),0x1000000);
      if (v8)
        v9 = -520092096;
      else
        v9 = -520092128;
      w_u32(((uint32)((v4 + 4))),v9);
      w_u32(((uint32)(v4)),((r_u32(((uint32)(v4))) & 0xFF000000) | (r_u32(v3) & 0xFFFFFF)));
      result = ((r_u32(v3) & 0xFF000000) | (v4 & 0xFFFFFF));
      w_u32(v3,result);
    }
  }
  return result;
}


/* TODO Missing call adapter sub_8009C858 */
/* TODO Missing call adapter sub_8009CB8C */
/* TODO Missing call adapter sub_8009CC0C */
uint32 sub_8007011C(uint32 a1, uint32 a2, uint32 a3, uint32 a4)
{
  uint32 result;
  sint32 v9;
  sint32 v10;
  uint32 v11;
  sint32 v12;
  uint32 v13;
  sint32 v14;
  sint32 v15;
  result = ((uint32)(r_u32(0x800FF818u)));
  if (!r_u32(0x800FF818u))
  {
    v9 = a1;
    v10 = (16 * (a1 != 0));
    result = ((uint32)(((void)(v10),abort(),0u)));
    v11 = result;
    if (result)
    {
      if ((result == ((uint32)(1))))
        w_u8((0x800EC0F8u+(((380 * v9) + 375))*1u),0);
      v12 = (380 * v9);
      v13 = (0x800EC0F8u+((380 * v9))*1u);
      if (!r_u8((v13+(375)*1u)))
      {
        ((void)(v10),(void)((0x800EC26Cu+(v12)*1u)),(void)(2),abort(),0u);
        if ((v11 != ((uint32)(2))))
        {
          v14 = a1;
          if ((v11 != ((uint32)(6))))
            goto LABEL_11;
          v15 = ((void)(v10),(void)(0x800FF824u),abort(),0u);
          v14 = a1;
          if (!v15)
            goto LABEL_11;
        }
        w_u8((v13+(375)*1u),1);
      }
      v14 = a1;
      LABEL_11:
      result = (0x800EC270u+((95 * v14))*4u);

      w_u8((0x800EC26Cu+(((380 * v14) + a3))*1u),a4);
      w_u16((((uint32)(result))+(a3)*2u),a2);
    }
  }
  return result;
}



uint32 sub_8005ED30(uint32 a1)
{
  sint32 result;
  short v3;
  sint32 v4;
  sint32 v5;
  short v6;
  short v7;
  sint32 v8;
  uint32 v9;
  sint32 v10;
  sint32 v11;
  short v12;
  sint32 v13;
  short v14;
  sint32 v15;
  sint32 v16;
  if (!(r_u8(((uint32)((r_u32((((uint32)(a1))+(111)*4u)) + 256))))))
    return 0;
  if ((r_u32((((uint32)(a1))+(115)*4u)) == 256))
  {
    result = 0;
    if (!((r_u8((((uint32)(a1))+(468)*1u)) | r_u8((((uint32)(a1))+(469)*1u)))))
      return result;
    v3 = r_u16((a1+(237)*2u));
    v4 = r_u32(0x800FF904u);
    v5 = (r_u32(0x800FF904u) == 0);
    w_u16((a1+(303)*2u),v3);
    if (v5)
      v6 = (v3 & 0xFFF);
    else
      v6 = ((r_u16(((uint32)((v4 + 494)))) + v3) & 0xFFF);
    w_u16((a1+(9)*2u),v6);
    v7 = r_u16(a1);
    w_u32((((uint32)(a1))+(115)*4u),2048);
    v8 = 0;
    if (((v7 & 8) != 0))
      v8 = 55;
    sub_80069DF0(v8,0x2000,0);
    v9 = a1;
    v10 = 12;
    v11 = 6;
    goto LABEL_21;
  }
  if (((r_u32((((uint32)(a1))+(115)*4u)) & 0x630) == 0))
  {
    v16 = (r_u16(a1) & 8);
    w_u32((((uint32)(a1))+(115)*4u),256);
    if (v16)
      sub_80069DF0(72,0x2000,0);
    v9 = a1;
    v10 = 5;
    v11 = 0;
    LABEL_21:
    sub_80063038(v9,v10,v11,-1);

    return 1;
  }
  result = 0;
  if ((r_u8((((uint32)(a1))+(468)*1u)) | r_u8((((uint32)(a1))+(469)*1u))))
  {
    v12 = r_u16((a1+(237)*2u));
    v13 = r_u32(0x800FF904u);
    v5 = (r_u32(0x800FF904u) == 0);
    w_u16((a1+(303)*2u),v12);
    if (v5)
      v14 = (v12 & 0xFFF);
    else
      v14 = ((r_u16(((uint32)((v13 + 494)))) + v12) & 0xFFF);
    w_u16((a1+(9)*2u),v14);
    w_u32((((uint32)(a1))+(115)*4u),2048);
    sub_80063038(a1,12,0,-1);
    v15 = 0;
    if (((r_u16(a1) & 8) != 0))
      v15 = 55;
    sub_80069DF0(v15,0x2000,0);
    return 1;
  }
  return result;
}



uint32 sub_8005ED28(void)
{
  return 0;
}



uint32 sub_8006C47C(uint32 a1, uint32 a2, uint32 a3)
{
  uint32 result;
  sint32 v4;
  sint32 v5;
  result = a1;
  v4 = (r_u32((a3+(1)*4u)) * r_u32(a2));
  v5 = (r_u32((a3+(2)*4u)) * r_u32(a2));
  w_u32(a1,(r_u32(a3) * r_u32(a2)));
  w_u32((a1+(1)*4u),v4);
  w_u32((a1+(2)*4u),v5);
  return result;
}



uint32 sub_80030764(void)
{
  uint32 result;
  sint32 v1;
  sint32 v2;
  sint32 v3;
  sint32 v4;
  sint32 v5;
  sint32 v6;
  sint32 v7;
  sint32 v8;
  result = ((uint32)(r_u32(0x800FF444u)));
  if (r_u32(0x800FF444u))
  {
    do
    {
      v1 = r_u32((result+(4)*4u));
      v2 = r_u32((result+(5)*4u));
      w_u32((result+(6)*4u),r_u32((result+(3)*4u)));
      w_u32((result+(7)*4u),v1);
      w_u32((result+(8)*4u),v2);
      v3 = r_u32((result+(28)*4u));
      v4 = r_u32((result+(29)*4u));
      w_u32((result+(18)*4u),r_u32((result+(27)*4u)));
      w_u32((result+(19)*4u),v3);
      w_u32((result+(20)*4u),v4);
      v5 = r_u32((result+(31)*4u));
      v6 = r_u32((result+(32)*4u));
      w_u32((result+(21)*4u),r_u32((result+(30)*4u)));
      w_u32((result+(22)*4u),v5);
      w_u32((result+(23)*4u),v6);
      v7 = r_u32((result+(34)*4u));
      v8 = r_u32((result+(35)*4u));
      w_u32((result+(24)*4u),r_u32((result+(33)*4u)));
      w_u32((result+(25)*4u),v7);
      w_u32((result+(26)*4u),v8);
      result = ((uint32)(r_u32((result+(1)*4u))));
    }
    while (result);
  }
  return result;
}



uint32 sub_80070288(uint32 a1, uint32 a2)
{
  uint32 result;
  result = ((uint32)(r_u32(0x800FF818u)));
  if (!r_u32(0x800FF818u))
  {
    result = (((uint32)((0x800EC270u+((95 * a1))*4u)))+(a2)*2u);
    w_u8((0x800EC26Cu+(((380 * a1) + a2))*1u),0);
    w_u16(result,0);
  }
  return result;
}



uint32 sub_800350E8(uint32 a1)
{
  sint32 result;
  result = ((a1 << 16) | a1);
  w_u32(0x800FF424u,result);
  return result;
}



uint32 sub_80034EF4(uint32 a1)
{
  sub_80032F7C(((sint32)(a1)));
  w_u32((a1+(17)*4u),0x800A1CA8u);
  sub_80032E50(a1,0x800FF440u);
  return a1;
}



uint32 sub_800667CC(uint32 a1, uint32 a2, uint32 a3)
{
  sint32 result;
  w_u32(a1,(0u - ((((a2 * ((sint16)((r_u32((0x800F863Cu+((r_u16(a3) & 0xFFF))*4u)))>>16))) >> 12) * ((sint16)((r_u32((0x800F863Cu+((r_u16((a3+(1)*2u)) & 0xFFF))*4u)))>>0))))));
  w_u32((a1+(1)*4u),(a2 * ((sint16)((r_u32((0x800F863Cu+((r_u16(a3) & 0xFFF))*4u)))>>0))));
  result = (0u - ((((a2 * ((sint16)((r_u32((0x800F863Cu+((r_u16(a3) & 0xFFF))*4u)))>>16))) >> 12) * ((sint16)((r_u32((0x800F863Cu+((r_u16((a3+(1)*2u)) & 0xFFF))*4u)))>>16)))));
  w_u32((a1+(2)*4u),result);
  return result;
}



uint32 sub_800352B0(uint32 a1)
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
  sint32 v14;
  sint32 v15;
  sint32 v16;
  sint32 v17;
  sint8 v18;
  sint32 result;
  sint8 v20;
  v2 = (r_u32(((uint32)((a1 + 24)))) + r_u32(((uint32)((a1 + 36)))));
  v3 = (r_u32(((uint32)((a1 + 28)))) + r_u32(((uint32)((a1 + 40)))));
  v4 = r_u32(((uint32)((a1 + 36))));
  v5 = r_u32(((uint32)((a1 + 32))));
  v6 = r_u32(((uint32)((a1 + 44))));
  v7 = r_u32(((uint32)((a1 + 48))));
  w_u32(((uint32)((a1 + 24))),v2);
  v8 = (v5 + v6);
  v9 = (v4 + v7);
  v10 = r_u32(((uint32)((a1 + 40))));
  v11 = r_u32(((uint32)((a1 + 52))));
  v12 = r_u32(((uint32)((a1 + 44))));
  v13 = r_u32(((uint32)((a1 + 56))));
  w_u32(((uint32)((a1 + 28))),v3);
  w_u32(((uint32)((a1 + 32))),v8);
  v14 = ((v10 + v11) - ((v10 + v11) >> 3));
  v15 = ((v12 + v13) - ((v12 + v13) >> 3));
  v11 = ((v11&0xFFFF0000u)|(((r_u16(((uint32)((a1 + 8)))))&0xFFFFu)<<0));
  v16 = r_u16(((uint32)((a1 + 10))));
  w_u32(((uint32)((a1 + 36))),(v9 - (v9 >> 3)));
  w_u32(((uint32)((a1 + 40))),v14);
  w_u32(((uint32)((a1 + 44))),v15);
  v11 = ((v11&0xFFFF0000u)|((((v11 + 1))&0xFFFFu)<<0));
  w_u16(((uint32)((a1 + 8))),v11);
  if ((((short)(v11)) >= v16))
    sub_80032ED8(a1);
  v17 = r_u8(((uint32)((a1 + 85))));
  v18 = r_u8(((uint32)((a1 + 86))));
  w_u8(((uint32)((a1 + 76))),(r_u8(((uint32)((a1 + 76))))-(r_u8(((uint32)((a1 + 84)))))));
  result = (r_u8(((uint32)((a1 + 77)))) - v17);
  v20 = (r_u8(((uint32)((a1 + 78)))) - v18);
  w_u8(((uint32)((a1 + 77))),result);
  w_u8(((uint32)((a1 + 78))),v20);
  return result;
}



uint32 sub_80032ED8(uint32 a1)
{
  sint32 result;
  result = 1;
  w_u8(((uint32)((a1 + 63))),1);
  return result;
}



uint32 sub_80034F38(uint32 a1, uint32 a2)
{
  uint32 result;
  sint32 v5;
  w_u32((a1+(17)*4u),0x800A1CA8u);
  sub_80032E7C(a1,0x800FF440u);
  result = sub_80032FB8(((sint32)(a1)),0);
  if (((a2 & 1) != 0))
    return ((uint32)(sub_80032E30(((sint32)(a1)))));
  return result;
}



uint32 sub_80032FB8(uint32 a1, uint32 a2)
{
  uint32 result;
  sint32 v3;
  sint32 v4;
  result = 0x800A1DB8u;
  v3 = r_u32(0x800FF3A8u);
  v4 = (a2 & 1);
  w_u32(((uint32)((a1 + 68))),0x800A1DB8u);
  w_u32(0x800FF3A8u,(v3 - 1));
  if (v4)
    return ((uint32)(sub_80032E30(a1)));
  return result;
}



uint32 sub_80032E30(uint32 a1)
{
  return sub_8006BC20(a1);
}



uint32 sub_8004B948(uint32 a1)
{
  sint32 result;
  sint32 v2;
  result = 0xFFFF;
  if (!(r_u8(((uint32)((a1 + 385))))))
  {
    v2 = r_u16(((uint32)((a1 + 214))));
    result = 1;
    if ((v2 != 0xFFFF))
    {
      w_u8(((uint32)((a1 + 385))),1);
      return sub_80064A08((r_u32(((uint32)(((4 * v2) + r_u32(0x800FF624u))))) + 6));
    }
  }
  return result;
}



uint32 sub_80062A64(uint32 a1, uint32 a2)
{
  sint32 v4;
  sint32 v5;
  sint32 result;
  if ((((r_u16(((uint32)((a1 + 78)))) & 1) != 0) && (a2 != 0x800FF5E0u)))
    sub_80062BF0(a1);
  v4 = r_u32(((uint32)((a1 + 28))));
  if (v4)
    w_u32(((uint32)((v4 + 48))),r_u32(((uint32)((a1 + 48)))));
  v5 = r_u32(((uint32)((a1 + 48))));
  if (v5)
    w_u32(((uint32)((v5 + 28))),r_u32(((uint32)((a1 + 28)))));
  result = r_u32(a2);
  if ((r_u32(a2) == a1))
  {
    result = r_u32(((uint32)((a1 + 28))));
    w_u32(a2,result);
  }
  return result;
}


/* TODO Missing call adapter indirect */
uint32 sub_800629BC(uint32 a1, uint32 a2)
{
  sint32 v4;
  sint32 v5;
  sint32 result;
  v4 = r_u32(((uint32)((a1 + 196))));
  w_u32(((uint32)((a1 + 68))),0x800A3380u);
  if (v4)
    ((void)((v4 + r_u16(((uint32)((r_u32(((uint32)((v4 + 68)))) + 8)))))),(void)(3),abort(),0u);
  sub_80062650(a1,0);
  result = (a2 & 1);
  if (((a2 & 1) != 0))
    return sub_80062608(a1);
  return result;
}


/* TODO Missing call adapter indirect */
uint32 sub_80022C90(uint32 a1, uint32 a2, uint32 a3)
{
  sint32 v5;
  sint32 v6;
  uint32 v7;
  sint32 v8;
  sint32 v9;
  sint32 v10;
  sint32 v11;
  short v12;
  sint32 v13;
  sint32 v14;
  sint32 v15;
  sint32 v16;
  uint32 v17;
  sint32 v18;
  sint32 v19;
  sint32 v20;
  sint32 v21;
  sint32 v22;
  sint32 v23;
  sint32 v24;
  sint32 v25;
  sint32 v26;
  sint32 result;
  sint32 v28;
  sint32 v29;
  short v30;
  sint32 endpoint[3];
  char v34[16];
  int v35[4];
  int v36[4];
  int v37[3];
  char v39[16];
  char v40[16];
  char v41[16];
  sint32 v42;
  sint32 v43;
  v5 = r_u32((a2+(1)*4u));
  v6 = r_u32((a2+(2)*4u));
  w_u32(((uint32)((a1 + 8))),r_u32(a2));
  w_u32(((uint32)((a1 + 12))),v5);
  w_u32(((uint32)((a1 + 16))),v6);
  v7 = ((uint32)((a1 + 8)));
  if (!(r_u32(((uint32)((a1 + 20))))))
  {
    sub_80022ABC(a1);
    v8 = r_u32(((uint32)((a1 + 76))));
    w_u32(((uint32)((a1 + 28))),r_u32(0x800FF2F0u));
    w_u32(((uint32)((a1 + 52))),sub_80069DF0(v8,0x2000,0));
  }
  if (r_u32(((uint32)((a1 + 80)))))
  {
    v9 = sub_80032DC0(124);
    if (v9)
      sub_8001CF9C(v9,v7,5,255,235,0,2,90,10,0,20,(sint32)(20u*r_u32(a1+80))/256,0,0,(sint32)(14u*r_u32(a1+80))/256,0,(sint32)(10u*r_u32(a1+80))/256,(sint32)(20u*r_u32(a1+80))/256,0,1);
  }
  if ((r_u32(((uint32)((a1 + 20)))) && ((r_u32(0x800FF2F0u) - r_u32(((uint32)((a1 + 24))))) < (30 / r_u8(((uint32)((a1 + 70))))))))
  {
    w_u32(((uint32)((a1 + 20))),1);
    return 0;
  }
  v10 = r_u8(((uint32)((a1 + 68))));
  w_u32(((uint32)((a1 + 84))),1);
  if (v10)
  {
    w_u32(0x800FF3ACu,0);
    v11 = sub_80032DC0(116);
    if (v11)
      sub_8002374C(v11,(a1 + 8),a3);
    w_u32(0x800FF3ACu,1);
  }
  v12 = r_u16(((uint32)((a3 + 4))));
  v29 = r_u32(((uint32)(a3)));
  v30 = v12;
  v29 = ((v29&0xFFFF0000u)|(((((v29 + sub_80066570(((2 * r_u32(((uint32)((a1 + 60))))) | 1))) - r_u16(((uint32)((a1 + 60))))))&0xFFFFu)<<0));
  v29 = ((v29&0x0000FFFFu)|((((sub_80066570(((2 * r_u32(((uint32)((a1 + 64))))) | 1)) - r_u16(((uint32)((a1 + 64))))))&0xFFFFu)<<16));
  sub_8006C624(&v29);
  xport_draft_host_sub_800667CC_p13(endpoint,r_u32(a1+72u),&v29);
  sub_8006C4EC(v34,&endpoint[0],(a1 + 72));
  sub_8006C0B8(&endpoint[0],(a1 + 8));
  v30 = 0;
  v29 = ((v29&0xFFFF0000u)|(((0)&0xFFFFu)<<0));
  xport_draft_host_sub_800667CC_p13(v36,(uint32)-150,&v29);
  sub_8006C0B8(v36,(a1 + 8));
  w_u32(0x800ED638u,v36[0]);
  w_u32(0x800ED640u,v36[2]);
  w_u32(0x800ED63Cu,v36[1]);
  v13 = 0;
  w_u32(0x800ED644u,r_u32(((uint32)((a1 + 8)))));
  v14 = 0;
  w_u32(0x800ED648u,r_u32(((uint32)((a1 + 12)))));
  w_u32(0x800ED64Cu,r_u32(((uint32)((a1 + 16)))));
  sub_8007BB24(0x800ED638u);
  w_u32(0x800FF974u,1);
  sub_8007DD04(0x800ED638u,1);
  w_u32(0x800FF974u,0);
  v15 = 0;
  if (r_u32(0x800ED6A0u))
    goto LABEL_13;
  w_u32(0x800ED638u,r_u32(((uint32)((a1 + 8)))));
  w_u32(0x800ED63Cu,r_u32(((uint32)((a1 + 12)))));
  v16 = r_u32(((uint32)((a1 + 16))));
  w_u32(0x800ED644u,endpoint[0]);
  w_u32(0x800ED648u,endpoint[1]);
  w_u32(0x800ED64Cu,endpoint[2]);
  w_u32(0x800ED640u,v16);
  sub_8007BB24(0x800ED638u);
  w_u32(0x800FF974u,1);
  sub_8007DD04(0x800ED638u,1);
  v17 = r_u32(((uint32)((a1 + 44))));
  w_u32(0x800FF974u,0);
  v18 = xport_draft_host_sub_8007C398_p23(a1+8u,endpoint,v35,r_u32(v17),0);
  v15 = v18;
  if (!r_u32(0x800ED6A0u))
  {
    if (!v18)
      goto LABEL_20;
    goto LABEL_19;
  }
  if (v18)
  {
    if ((sub_8006689C((v18 + 4),(a1 + 8)) >= ((uint32)(r_u32(0x800ED678u)))))
    {
      v13 = 1;
      goto LABEL_20;
    }
    LABEL_19:
    v14 = 1;

    goto LABEL_20;
  }
  LABEL_13:
  v13 = 1;

  LABEL_20:
  v19 = 0;

  if (!v13)
    goto LABEL_37;
  sub_8001E740(r_u32(0x800ED6A0u),r_u32(0x800ED6B8u),r_u32(((uint32)((a1 + 40)))));
  if (((r_u16(((uint32)(r_u32(0x800ED6A0u)))) & 0x10) != 0))
    ((void)((r_u32(0x800ED6A0u) + r_u16(((uint32)((r_u32(((uint32)((r_u32(0x800ED6A0u) + 68)))) + 48)))))),(void)(r_u32(((uint32)((a1 + 40))))),(void)(v34),(void)(28),abort(),0u);
  v19 = 12;
  endpoint[0] = r_u32(0x800ED6A4u);
  endpoint[1] = r_u32(0x800ED6A8u);
  endpoint[2] = r_u32(0x800ED6ACu);
  if (!sub_80066570(4))
  {
    sub_8006C3AC(v37,&endpoint[0],(a1 + 8));
    v20 = ((((v37[0] >> 6) * r_u16(0x800ED6B0u)) + ((v37[1] >> 6) * r_u16(0x800ED6B2u))) + ((v37[2] >> 6) * r_u16(0x800ED6B4u)));
    v21 = (v20 > 0);
    v22 = (v20 >> 12);
    if (!v21)
    {
      v37[0] -= ((v22 * r_u16(0x800ED6B0u)) >> 6);
      v37[2] -= ((v22 * r_u16(0x800ED6B4u)) >> 6);
      v43 = sub_8006BF04(v37);
      if ((v43 >= 201))
      {
        v42 = 400;
        sub_8006C40C(v41,v37,&v42);
        sub_8006C4EC(v40,v41,&v43);
        sub_8006C34C(v39,&endpoint[0],v40);
        v23 = sub_80032DC0(108);
        if (v23)
          sub_800234CC(v23,&endpoint[0],v39);
      }
    }
  }
  if (sub_80066570(4))
    goto LABEL_37;
  v24 = sub_80066570(3);
  if ((v24 == 1))
  {
    v25 = 28;
  }
  else
    if ((v24 >= 2))
  {
    v25 = 29;
    if ((v24 != 2))
      goto LABEL_37;
  }
  else
  {
    v25 = 27;
    if (v24)
      goto LABEL_37;
  }
  sub_80069EF4(v25,&endpoint[0],0);
  LABEL_37:
  if (v14)
  {
    v19 = 4;
    ((void)((v15 + r_u16(((uint32)((r_u32(((uint32)((v15 + 68)))) + 48)))))),(void)(r_u32(((uint32)((a1 + 40))))),(void)(v34),(void)(28),abort(),0u);
    if (sub_80066570(2))
      v19 = 20;
    endpoint[0] = v35[0];
    endpoint[1] = v35[1];
    endpoint[2] = v35[2];
  }

  if (v19)
    sub_8002289C(v19,((sint32)(&endpoint[0])));
  v26 = sub_80032DC0(108);
  if (v26)
    sub_800234CC(v26,(a1 + 8),&endpoint[0]);
  result = 1;
  v28 = r_u32(0x800FF2F0u);
  w_u32(((uint32)((a1 + 20))),1);
  w_u32(((uint32)((a1 + 24))),v28);
  return result;
}



uint32 sub_8001CF9C(uint32 a1, uint32 a2, uint32 a3, uint32 a4, uint32 a5, uint32 a6, uint32 a7, uint32 a8, uint32 a9, uint32 a10, uint32 a11, uint32 a12, uint32 a13, uint32 a14, uint32 a15, uint32 a16, uint32 a17, uint32 a18, uint32 a19, uint32 a20)
{
  sint32 v27;
  sint32 v28;
  sub_80034598(a1,a3,1);
  w_u32(((uint32)((a1 + 68))),0x800A1154u);
  v27 = r_u32((a2+(1)*4u));
  v28 = r_u32((a2+(2)*4u));
  w_u32(((uint32)((a1 + 24))),r_u32(a2));
  w_u32(((uint32)((a1 + 28))),v27);
  w_u32(((uint32)((a1 + 32))),v28);
  sub_80034A18(a1,a4,a5,a6);
  w_u16(((uint32)((a1 + 104))),a7);
  sub_80034A44(a1,a8,a9,a10);
  w_u16(((uint32)((a1 + 106))),a11);
  sub_80034A9C(a1,0,0,0,0);
  sub_800349D0(a1,0,a12);
  w_u16(((uint32)((a1 + 108))),a13);
  w_u8(((uint32)((a1 + 120))),a14);
  w_u16(((uint32)((a1 + 110))),a15);
  w_u16(((uint32)((a1 + 112))),a16);
  w_u16(((uint32)((a1 + 114))),a17);
  w_u16(((uint32)((a1 + 116))),a18);
  w_u16(((uint32)((a1 + 118))),a19);
  w_u16(((uint32)((a1 + 10))),a20);
  sub_8001D114(a1);
  return a1;
}



uint32 sub_80034598(uint32 a1, uint32 a2, uint32 a3)
{
  sint32 v6;
  sint32 v7;
  uint32 v8;
  sub_80032F7C(a1);
  a2 *= 2;
  w_u32(((uint32)((a1 + 80))),a2);
  w_u32(((uint32)((a1 + 68))),0x800A1D08u);
  w_u32(((uint32)((a1 + 84))),a3);
  w_u16(((uint32)((a1 + 92))),(0x1000 / a2));
  v6 = sub_8006B864((8 * (a2 + (a2 * a3))),0,1);
  v7 = (r_u32(((uint32)((a1 + 80)))) * r_u32(((uint32)((a1 + 84)))));
  w_u32(((uint32)((a1 + 72))),v6);
  v8 = 0;
  w_u32(((uint32)((a1 + 76))),(v6 + (8 * r_u32(((uint32)((a1 + 80)))))));
  if (v7)
  {
    do
      w_u32(((uint32)((((8 * v8++) + r_u32(((uint32)((a1 + 76))))) + 4))),973078528);
    while ((v8 < (r_u32(((uint32)((a1 + 80)))) * r_u32(((uint32)((a1 + 84)))))));
  }
  w_u32(((uint32)((a1 + 88))),838860800);
  w_u32(((uint32)((a1 + 100))),-1);
  sub_80032E50(((uint32)(a1)),0x800FF450u);
  return a1;
}



uint32 sub_8001BCDC(uint32 a1, uint32 a2, uint32 a3, uint32 a4, uint32 duration)
{
  uint32 packet = sub_8001BC58(a1);
  if (packet) {
    uint32 red = a2 & 255u, green = a3 & 255u, blue = a4 & 255u;
    w_u32(packet + 16u, a1);
    w_u32(packet, (blue << 16) | (green << 8) | red);
    w_u8(packet + 12u, 0u);
    w_u8(packet + 9u, 0u);
    w_u8(packet + 10u, 0u);
    w_u8(packet + 13u, duration);
    w_u16(packet + 4u, (sint32)(0x8000u - (red << 8)) / (sint32)duration);
    w_u16(packet + 6u, (sint32)(0x8000u - (green << 8)) / (sint32)duration);
    w_u16(packet + 8u, (sint32)(0x8000u - (blue << 8)) / (sint32)duration);
    w_u16(a1, r_u16(a1) | 0x400u);
    w_u32(a1 + 32u, r_u32(packet));
    return r_u32(packet);
  }
  return packet;
}



uint32 sub_8001BC58(uint32 a1)
{
  uint32 v1;
  uint32 v3;
  v1 = 0;
  if (!r_u32(0x800FF1D8u))
    return 0;
  if (((r_u16(a1) & 0x400) != 0))
  {
    v1 = ((uint32)(r_u32(0x800FF1D4u)));
    if (!r_u32(0x800FF1D4u))
    {
      LABEL_8:
      v1 = r_u32(0x800FF1D8u);

      v3 = r_u32(((uint32)(((((uint32)(0x800F2624u))+(((uint32)(r_u32(0x800FF1D8u))))*1u)+(2146490864)*1u))));
      w_u32(((uint32)(((((uint32)(0x800F2624u))+(((uint32)(r_u32(0x800FF1D8u))))*1u)+(2146490864)*1u))),r_u32(0x800FF1D4u));
      w_u32(0x800FF1D4u,((sint32)(v1)));
      w_u32(0x800FF1D8u,v3);
      return v1;
    }
    do
    {
      if ((((uint32)(r_u32((r_u32(v1)+(4)*4u)))) == a1))
        break;
      v1 = ((uint32)(r_u32((r_u32(v1)+(5)*4u))));
    }
    while (v1);
  }
  if (!v1)
    goto LABEL_8;
  return v1;
}



uint32 sub_800234CC(uint32 a1, uint32 a2, uint32 a3)
{
  sint32 v6;
  sint32 v7;
  sint32 v8;
  sint32 v9;
  sint32 v10;
  sint32 v11;
  uint32 result;
  sint32 v13;
  sint32 v14;
  sint32 vector15[3];
  char v18[16];
  sint32 vector19[3];
  sint32 vector22[3];
  char v25[16];
  sint32 v26;
  sint32 v27;
  sint32 v28;
  sint32 v29;
  sub_80032824(a1);
  w_u32((a1+(17)*4u),0x800A1530u);
  v6 = r_u32((a3+(1)*4u));
  v7 = r_u32((a3+(2)*4u));
  vector15[0] = r_u32(a3);
  vector15[1] = v6;
  vector15[2] = v7;
  sub_8006C3AC(v18,&vector15[0],a2);
  v26 = sub_8006BF04(v18);
  if (v26)
    sub_8006C190(v18,&v26);
  else
    sub_80032ED8(a1);
  if ((v26 >= 3001))
  {
    v26 = 3000;
    sub_8006C47C(&vector22[0],&v26,v18);
    sub_8006C34C(&vector19[0],a2,&vector22[0]);
    vector15[0] = vector19[0];
    vector15[1] = vector19[1];
    vector15[2] = vector19[2];
  }
  v27 = sub_80066570((v26 / 3));
  sub_8006C40C(v25,v18,&v27);
  sub_8006C34C(&vector19[0],a2,v25);
  v8 = vector19[1];
  v9 = vector19[2];
  w_u32((a1+(20)*4u),vector19[0]);
  w_u32((a1+(21)*4u),v8);
  w_u32((a1+(22)*4u),v9);
  sub_8006C3AC(&vector22[0],&vector15[0],(a1+(20)*4u));
  v28 = 5;
  sub_8006C4EC(&vector19[0],&vector22[0],&v28);
  sub_8006C34C(&vector22[0],(a1+(20)*4u),&vector19[0]);
  v10 = vector22[1];
  v11 = vector22[2];
  w_u32((a1+(23)*4u),vector22[0]);
  w_u32((a1+(24)*4u),v10);
  w_u32((a1+(25)*4u),v11);
  v29 = 1;
  sub_8006C5C4(&vector22[0],&vector19[0],&v29);
  result = a1;
  v13 = vector22[1];
  v14 = vector22[2];
  w_u32((a1+(9)*4u),vector22[0]);
  w_u32((a1+(10)*4u),v13);
  w_u32((a1+(11)*4u),v14);
  w_u32((a1+(18)*4u),((r_u32((a1+(18)*4u)) & 0xFD000000) | 0x2000000));
  return result;
}



uint32 sub_80032824(uint32 a1)
{
  sub_80032F7C(a1);
  w_u32((a1+(18)*4u),1350598784);
  w_u32((a1+(17)*4u),0x800A1B08u);
  w_u32((a1+(19)*4u),1434484864);
  sub_80032E50(a1,0x800FF464u);
  return a1;
}



uint32 sub_8002289C(uint32 a1, uint32 a2)
{
  sint32 v4;
  sint32 v5;
  sint32 v6;
  sint32 v7;
  sint32 result;
  sint32 v9;
  sint32 v10;
  if (((a1 & 4) != 0))
  {
    v4 = sub_80032DC0(120);
    v5 = v4;
    if (v4)
      v5 = sub_80035478(v4,a2,0,744,1,1,0xFFFFFFFFu);
    w_u8(((uint32)((v5 + 90))),sub_80066570((r_u8(((uint32)((v5 + 89)))) - 1)));
    w_u16(((uint32)((v5 + 96))),sub_80066570(4096));
    w_u16(((uint32)((v5 + 64))),100);
  }
  v6 = 0;
  if (((a1 & 8) != 0))
  {
    do
    {
      v7 = sub_80032DC0(112);
      if (v7)
        sub_800358B4(v7,a2,128,128,128,6,1000,1,40,0x3000,20);
      ++v6;
    }
    while ((v6 < 4));
  }
  result = (a1 & 0x10);
  if (((a1 & 0x10) != 0))
  {
    result = sub_80032DC0(96);
    v9 = result;
    if (result)
    {
      v10 = sub_80066570(3);
      return sub_800325B0(v9,a2,1,v10+1,128,128,128,8,5,700,700);
    }
  }
  return result;
}



uint32 sub_800332A4(uint32 a1)
{
  sint32 result;
  result = (r_u32(((uint32)((a1 + 76)))) | 0x2000000);
  w_u32(((uint32)((a1 + 76))),result);
  return result;
}



uint32 sub_800355A0(uint32 a1)
{
  sint32 v2;
  sint32 result;
  sint32 v4;
  sint8 v5;
  sub_800333F0(a1);
  if (r_u32(((uint32)((a1 + 112)))))
  {
    v2 = r_u32(((uint32)((a1 + 116))));
    result = (r_u8(((uint32)((a1 + 90)))) < v2);
    if ((r_u8(((uint32)((a1 + 90)))) >= v2))
      return sub_80032ED8(a1);
  }
  else
  {
    v4 = r_u8(((uint32)((a1 + 89))));
    result = -2;
    if ((r_u8(((uint32)((a1 + 90)))) >= v4))
    {
      if ((r_u32(((uint32)((a1 + 116)))) == -2))
        v5 = (v4 - 1);
      else
        v5 = 0;
      return sub_800333D0(a1,v5);
    }
  }
  return result;
}



uint32 sub_8001D240(uint32 a1)
{
  uint32 v2;
  sint32 v3;
  sint32 v4;
  sint32 result;
  v2 = 0;
  if (r_u32(((uint32)((a1 + 80)))))
  {
    v3 = 0;
    do
    {
      sub_80032D3C(((r_u32(((uint32)((a1 + 72)))) + v3) + 4),r_u16(((uint32)((a1 + 106)))));
      sub_80032D3C((a1 + 88),r_u16(((uint32)((a1 + 104)))));
      ++v2;
      v3 = (8 * v2);
    }
    while ((v2 < r_u32(((uint32)((a1 + 80))))));
  }
  if (r_u8(((uint32)((a1 + 120)))))
    sub_8001D114(a1);
  if (r_u16(((uint32)((a1 + 10)))))
  {
    v4 = r_u16(((uint32)((a1 + 10))));
    result = ((result&0xFFFF0000u)|((((r_u16(((uint32)((a1 + 8)))) + 1))&0xFFFFu)<<0));
    w_u16(((uint32)((a1 + 8))),result);
    result = ((short)(result));
    if ((v4 >= ((short)(result))))
      return result;
  }
  else
  {
    result = (r_u32(((uint32)((a1 + 88)))) & 0xFFFFFF);
    if (result)
      return result;
  }
  return sub_80032ED8(a1);
}



uint32 sub_800236D4(uint32 a1)
{
  sint32 v2;
  sint32 v3;
  sint32 v4;
  v2 = r_u32(((uint32)((a1 + 104))));
  v3 = (a1 + 80);
  if (v2)
  {
    sub_8006C0B8(v3,(a1 + 36));
    sub_8006C0B8((a1 + 92),(a1 + 36));
    v2 = r_u32(((uint32)((a1 + 104))));
  }
  v4 = (v2 + 1);
  w_u32(((uint32)((a1 + 104))),v4);
  if ((v4 == 4))
    sub_80032ED8(a1);
  return sub_80032D3C((a1 + 76),16);
}



uint32 sub_8001FCD4(uint32 a1, uint32 a2)
{
  sint32 result;
  w_u32(((uint32)((a1 + 68))),0x800A1154u);
  result = sub_800348A8(a1,0);
  if (((a2 & 1) != 0))
    return sub_80032E30(a1);
  return result;
}



uint32 sub_80034DE8(uint32 a1, uint32 a2)
{
  uint32 result;
  sint32 v5;
  w_u32((a1+(17)*4u),0x800A1CD8u);
  sub_80032E7C(a1,0x800FF438u);
  result = sub_800331EC(((sint32)(a1)),0);
  if (((a2 & 1) != 0))
    return ((uint32)(sub_80032E30(((sint32)(a1)))));
  return result;
}



uint32 sub_8002BC44(uint32 a1, uint32 a2)
{
  sint32 result;
  w_u32(((uint32)((a1 + 68))),0x800A1530u);
  result = sub_80032880(a1,0);
  if (((a2 & 1) != 0))
    return sub_80032E30(a1);
  return result;
}


/* TODO Missing call adapter sub_8001E598 */
/* TODO Postincrement memory expressions may require ordering refinement */
uint32 sub_8001CC1C(uint32 a1)
{
  sint32 v1;
  sint32 result;
  sint32 v4;
  sint32 v5;
  sint32 v6;
  uint32 v7;
  sint32 v8;
  sint32 v9;
  sint32 v10;
  sint32 v11;
  sint32 v12;
  sint32 i;
  uint32 v14;
  sint32 v15;
  sint32 v16;
  int v17[4];
  v1 = (w_u32(0x800FF1F0u,(r_u32(0x800FF1F0u)+1u)),r_u32(0x800FF1F0u));
  result = (r_u32(0x800FF1F0u) < 10);
  if ((r_u32(0x800FF1F0u) >= 10))
  {
    w_u32(0x800FF1F0u,v1);
    return result;
  }
  if (sub_80022318(a1,1,1))
  {
    v5 = 0;
    v4 = sub_8006F4D0(r_u8(((uint32)((a1 + 27)))));
    v6 = sub_80063D3C(r_u32(((uint32)(((4 * r_u16(((uint32)((a1 + 22))))) + r_u32((0x800FF7B4u+(v4)*4u)))))));
    if (v6)
      v5 = (r_u16(r_u32(((uint32)(((4 * r_u16(((uint32)((v6 + 10))))) + r_u32(0x800FF624u)))))) == 9);
    v7 = 512;
    if (r_u32(0x800FF738u))
      goto LABEL_18;
    v8 = r_u32(((uint32)((a1 + 8))));
    v9 = r_u32(((uint32)((a1 + 12))));
    v17[0] = r_u32(((uint32)((a1 + 4))));
    v17[1] = v8;
    v17[2] = v9;
    if (v5)
    {
      xport_draft_host_sub_80067388_p1(v17,0u,(sint16)r_u16(0x800EC51Cu),(sint16)r_u16(0x800EC51Au),1u);
      v10 = 2;
      if (((r_u32(0x800FF2F0u) & 1) != 0))
        v10 = 1;
      xport_draft_host_sub_80069EF4_p2(v10,v17,0u);
      v11 = sub_80032DC0(124);
      if (v11)
        xport_draft_host_sub_8001EB64_p2(v11,v17,64u,128u,0u,512u,10u);
      xport_draft_host_sub_8001E4C8_p1(v17,20u,10u,750u,80u);
      v12 = sub_80032DC0(124);
      if (v12)
      {
        xport_draft_host_sub_8001CF9C_p2(v12,v17,5u,255u,255u,255u,1u,16u,64u,0u,1u,60u,0u,1u,100u,120u,80u,100u,10u,4u);
        v7 = 512;
        LABEL_18:
        if (v5)
          v7 = 1024;

        for (i = r_u32(0x800FF794u); i; i = r_u32(((uint32)((i + 28)))))
        {
          if ((i != a1))
          {
            v14 = r_u32(r_u32(((uint32)(((4 * r_u16(((uint32)((i + 22))))) + r_u32((0x800EAEF8u+(((16 * r_u8(((uint32)((i + 27))))) + 4))*4u)))))));
            if (((v14 & 2) != 0))
            {
              v15 = 0;
              if (((v14>>8)&255u))
              {
                if (v5)
                {
                  v15 = (sub_8006696C((a1 + 4),(i + 4)) < v7);
                }
                else
                  if ((sub_8006696C((a1 + 4),(i + 4)) < v7))
                {
                  v16 = (r_u32(((uint32)((a1 + 8)))) - r_u32(((uint32)((i + 8)))));
                  if (((v16 > 409600) && (v16 <= 1638399)))
                    v15 = 1;
                }
                if (v15)
                  sub_8001CC1C(i);
              }
            }
          }
        }

        return (w_u32(0x800FF1F0u,(r_u32(0x800FF1F0u)-1u)),(r_u32(0x800FF1F0u)-1u));
      }
    }
    else
    {
      if (!sub_80066570(5))
      {
        ((void)(v17),abort(),0u);
        v7 = 512;
        goto LABEL_18;
      }
      sub_8001E650(v17);
    }
    v7 = 512;
    goto LABEL_18;
  }
  return (w_u32(0x800FF1F0u,(r_u32(0x800FF1F0u)-1u)),(r_u32(0x800FF1F0u)-1u));
}



uint32 sub_80021B3C(uint32 a1, uint32 a2, uint32 a3, uint32 a4)
{
  sint32 v7;
  sint32 v8;
  sint32 v9;
  sint32 v10;
  sint32 v11;
  sint32 v12;
  sint32 v13;
  sint32 result;
  if (a1)
  {
    v7 = r_u32(((uint32)(((4 * ((unsigned char)(a2))) + a4))));
    v8 = r_u32(((uint32)((((a2 >> 6) & 0x3FC) + a4))));
    v9 = r_u32(((uint32)((((a2 >> 14) & 0x3FC) + a4))));
    v10 = ((((unsigned char)(r_u32(((uint32)(((4 * ((unsigned char)(a2))) + a4)))))) + ((unsigned char)(r_u32(((uint32)((((a2 >> 6) & 0x3FC) + a4))))))) + ((unsigned char)(r_u32(((uint32)((((a2 >> 14) & 0x3FC) + a4)))))));
    v11 = ((((v7>>8)&255u) + ((v8>>8)&255u)) + ((v9>>8)&255u));
    v12 = ((((v7>>16)&255u) + ((v8>>16)&255u)) + ((v9>>16)&255u));
    if ((a3 == 4))
    {
      v13 = r_u32(((uint32)(((4 * ((a2>>8)&255u)) + a4))));
      w_u16(0x800FFB80u,((r_u16(0x800FFB80u)&0xFFFFFF00u)|(((((v10 + ((unsigned char)(v13))) / 4))&0xFFu)<<0)));
      w_u16(0x800FFB80u,((r_u16(0x800FFB80u)&0xFFFF00FFu)|(((((v11 + ((v13>>8)&255u)) / 4))&0xFFu)<<8)));
      result = ((v12 + ((v13>>16)&255u)) / 4);
    }
    else
    {
      w_u16(0x800FFB80u,((r_u16(0x800FFB80u)&0xFFFFFF00u)|((((((unsigned long long)((1431655766LL * v10))) >> 32))&0xFFu)<<0)));
      w_u16(0x800FFB80u,((r_u16(0x800FFB80u)&0xFFFF00FFu)|((((((unsigned long long)((1431655766LL * v11))) >> 32))&0xFFu)<<8)));
      result = (((unsigned long long)((1431655766LL * v12))) >> 32);
    }
  }
  else
  {
    result = ((a2>>16)&65535u);
    w_u16(0x800FFB80u,a2);
  }
  w_u8(0x800FFB82u,result);
  return result;
}



uint32 sub_8006CBF8(uint32 a1, uint32 a2, uint32 a3)
{
  sint32 result;
  short v4;
  sint32 v5;
  result = a1;
  v5 = ((v5&0xFFFF0000u)|((((r_u16(a3) * r_u32(a2)))&0xFFFFu)<<0));
  v5 = ((v5&0x0000FFFFu)|((((r_u16((a3+(1)*2u)) * r_u32(a2)))&0xFFFFu)<<16));
  v4 = (r_u16((a3+(2)*2u)) * r_u16(((uint32)(a2))));
  w_u32(((uint32)(a1)),v5);
  w_u16(((uint32)((a1 + 4))),v4);
  return result;
}



uint32 sub_8006CCE0(uint32 a1, uint32 a2, uint32 a3)
{
  sint32 result;
  short v4;
  sint32 v5;
  result = a1;
  v5 = ((v5&0xFFFF0000u)|((((r_u16(a2) >> r_u32(a3)))&0xFFFFu)<<0));
  v5 = ((v5&0x0000FFFFu)|((((r_u16((a2+(1)*2u)) >> r_u32(a3)))&0xFFFFu)<<16));
  v4 = (r_u16((a2+(2)*2u)) >> r_u32(a3));
  w_u32(((uint32)(a1)),v5);
  w_u16(((uint32)((a1 + 4))),v4);
  return result;
}



uint32 sub_80021154(uint32 a1, uint32 a2)
{
  sint32 v4;
  sint32 v5;
  sint32 v6;
  sint32 v7;
  sint32 result;
  sint32 v9;
  char v10[16];
  int v11[4];
  char v12[16];
  sint32 v13;
  sint32 v14;
  sint32 v15;
  sub_80032EE4(a1,a2);
  sub_8006C3AC(v10,a2,0x800FFD58u);
  v14 = sub_8006BF04(v10);
  if (v14)
  {
    v13 = sub_80066570(48);
    sub_8006C47C(v12,&v13,v10);
    sub_8006C4EC(v11,v12,&v14);
    v4 = v11[1];
    v5 = v11[2];
    w_u32((a1+(9)*4u),v11[0]);
    w_u32((a1+(10)*4u),v4);
    w_u32((a1+(11)*4u),v5);
  }
  else
  {
    sub_80032ED8(a1);
  }
  v6 = sub_80066570(45);
  v7 = r_u32(0x800FF210u);
  w_u32((a1+(9)*4u),(r_u32((a1+(9)*4u))+((v6 * r_u16(((uint32)(r_u32(0x800FF210u))))))));
  w_u32((a1+(10)*4u),(r_u32((a1+(10)*4u))+((v6 * r_u16(((uint32)((v7 + 2))))))));
  w_u32((a1+(11)*4u),(r_u32((a1+(11)*4u))+((v6 * r_u16(((uint32)((v7 + 4))))))));
  w_u32((a1+(10)*4u),(r_u32((a1+(10)*4u))-((sub_80066570(175) << 12))));
  if (!sub_80066570(20))
  {
    v15 = 1;
    sub_8006C22C((a1+(9)*4u),&v15);
  }
  result = r_u32((a1+(10)*4u));
  if ((result > 0))
    w_u32((a1+(10)*4u),0);
  v9 = r_u32((a1+(50)*4u));
  if (v9)
    return sub_800369F8(v9,(a1+(6)*4u));
  return result;
}



uint32 sub_80032EE4(uint32 a1, uint32 a2)
{
  sint32 result;
  sint32 v3;
  sint32 v4;
  result = r_u32(a2);
  v3 = r_u32((a2+(1)*4u));
  v4 = r_u32((a2+(2)*4u));
  w_u32((a1+(6)*4u),r_u32(a2));
  w_u32((a1+(7)*4u),v3);
  w_u32((a1+(8)*4u),v4);
  return result;
}



uint32 sub_8003445C(uint32 a1, uint32 a2, uint32 a3, uint32 a4, uint32 a5, uint32 a6, uint32 a7, uint32 a8, uint32 a9)
{
  sint32 result;
  w_u32(((uint32)((a1 + 160))),(((a2 << 16) | (a5 << 8)) | a4));
  w_u32(((uint32)((a1 + 164))),(((a3 << 16) | (a7 << 8)) | a6));
  result = (a8 | (a9 << 8));
  w_u16(((uint32)((a1 + 168))),result);
  return result;
}


