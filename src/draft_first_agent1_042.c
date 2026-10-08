#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
#include <stdlib.h>
/* Unverified draft; TODO Recover omitted call arguments, host-buffer adapters and signed field widths */
/* TODO Missing call adapter abs32 */
/* TODO Missing call adapter indirect */
/* TODO Missing call adapter nullsub_20 */
/* TODO Missing call adapter sub_80069FAC */
/* TODO Postincrement memory expressions may require ordering refinement */
uint32 sub_8003BBD8(uint32 a1)
{
  sint32 v2;
  sint32 v3;
  short v4;
  short v5;
  short v6;
  sint32 v7;
  short v8;
  short v9;
  short v10;
  sint32 v11;
  short v12;
  short v13;
  short v14;
  sint32 v15;
  sint32 v16;
  short v17;
  sint8 v18;
  sint32 v19;
  short v20;
  sint32 v21;
  sint32 v22;
  sint32 v23;
  sint32 v24;
  sint32 v25;
  sint32 v26;
  unsigned short v27;
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
  unsigned short v42;
  sint32 v43;
  sint32 v44;
  sint32 v45;
  sint32 v46;
  sint32 v47;
  sint32 v48;
  sint32 v49;
  short v50;
  sint32 v51;
  uint32 v52;
  sint32 v53;
  sint32 v54;
  sint32 v55;
  sint32 v56;
  sint32 result;
  sint32 v58;
  sint32 v59;
  sint32 v60;
  sint32 v61;
  sint32 v62;
  sint32 v63;
  sint32 v64;
  sint32 v65;
  sint32 v66;
  sint32 v67;
  sint32 v68;
  sint32 v69;
  sint32 v70;
  sint32 v71;
  sint32 v72;
  sint32 direction[3];
  uint32 angles[2];
  sint32 v77;
  unsigned short v78;
  char v79[16];
  v2 = r_u16(((uint32)((a1 + 496))));
  v3 = (v2 == 0);
  v4 = (v2 - 1);
  if (!v3)
  {
    v5 = r_u16(((uint32)((a1 + 36))));
    v6 = r_u16(((uint32)((a1 + 502))));
    w_u16(((uint32)((a1 + 496))),v4);
    w_u16(((uint32)((a1 + 36))),(v5 + v6));
  }
  v7 = r_u16(((uint32)((a1 + 498))));
  v3 = (v7 == 0);
  v8 = (v7 - 1);
  if (!v3)
  {
    v9 = r_u16(((uint32)((a1 + 38))));
    v10 = r_u16(((uint32)((a1 + 504))));
    w_u16(((uint32)((a1 + 498))),v8);
    w_u16(((uint32)((a1 + 38))),(v9 + v10));
  }
  v11 = r_u16(((uint32)((a1 + 500))));
  v3 = (v11 == 0);
  v12 = (v11 - 1);
  if (!v3)
  {
    v13 = r_u16(((uint32)((a1 + 40))));
    v14 = r_u16(((uint32)((a1 + 506))));
    w_u16(((uint32)((a1 + 500))),v12);
    w_u16(((uint32)((a1 + 40))),(v13 + v14));
  }
  v15 = r_u32(0x800A71D0u);
  v16 = r_u32(0x800A71D4u);
  w_u32(((uint32)((a1 + 116))),0x800A71CCu);
  w_u32(((uint32)((a1 + 120))),v15);
  w_u32(((uint32)((a1 + 124))),v16);
  if (!(r_u32(((uint32)((a1 + 512))))))
    (w_u32(((uint32)((a1 + 516))),(r_u32(((uint32)((a1 + 516))))+1u)),(r_u32(((uint32)((a1 + 516))))+1u));
  v17 = r_u16(((uint32)((a1 + 76))));
  w_u16(((uint32)((a1 + 388))),v17);
  if (((v17 & 1) != 0))
  {
    v18 = r_u8(((uint32)((a1 + 384))));
    w_u16(((uint32)((a1 + 76))),(v17 & 0xFFFE));
    w_u8(((uint32)((a1 + 384))),(v18 + 1));
  }
  if (r_u8(((uint32)((a1 + 380)))))
  {
    v19 = r_u16(((uint32)((a1 + 476))));
    v3 = (v19 == 0);
    v20 = (v19 - 1);
    if (v3)
      sub_8004BE90(a1,r_u32(((uint32)((a1 + 400)))));
    else
      w_u16(((uint32)((a1 + 476))),v20);
  }
  else
  {
    ((void)(a1),abort(),0u);
  }
  if (((r_u32(((uint32)((a1 + 396)))) & 1) == 0))
  {
    if ((r_u16(((uint32)((a1 + 440)))) != 1))
      goto LABEL_64;
    v44 = -(r_u16(((uint32)((a1 + 20)))));
    if ((-(r_u16(((uint32)((a1 + 20))))) < -2048))
      v44 = (4096 - r_u16(((uint32)((a1 + 20)))));
    v45 = (v44 << 16);
    if ((((short)(v44)) >= 2049))
      v45 = ((v44 - 4096) << 16);
    v46 = (v45 >> 16);
    v3 = (v46 != 0);
    v47 = (v46 >> 3);
    if (v3)
    {
      w_u16(((uint32)((a1 + 142))),v47);
      goto LABEL_64;
    }
    LABEL_62:
    w_u16(((uint32)((a1 + 136))),0);

    goto LABEL_64;
  }
  sub_8006C3AC(&direction[0],(a1 + 4),(a1 + 484));
  if ((r_u16(((uint32)((a1 + 436)))) < sub_8006BF04(&direction[0])))
  {
    v23 = r_u16(((uint32)((a1 + 440))));
    if (!(r_u16(((uint32)((a1 + 440))))))
    {
      xport_draft_host_sub_80066B8C_p1(angles,a1+4u,a1+484u);
      xport_draft_host_sub_800667CC_p3(a1+104u,r_u16(a1+436u),angles);
      goto LABEL_64;
    }
    if ((v23 == 1))
    {
      xport_draft_host_sub_80066B8C_p1(angles,a1+4u,a1+484u);
      sub_8006C3AC(&v77,(a1 + 484),(a1 + 4));
      v24 = (8160 / sub_8006BF04(&v77));
      if ((((v24 >= -256) && (v24 < 257)) && (((uint32)((v24 - 1))) >= 3)))
      {
        v25 = (a1 + 132);
        if ((v24 >= 0))
        {
          LABEL_30:
          v26 = (a1 + 138);

          LABEL_31:
          v27 = r_u16(((uint32)((a1 + 20))));

          v77 = r_u32(((uint32)((a1 + 16))));
          v78 = v27;
          sub_80066CF0((uint32)v77,v27,v25,v26,angles[0],angles[1]&65535u,(uint32)(v24 < -256 ? -256 : v24 > 256 ? 256 : (uint32)(v24-1)<3u ? 4 : v24<0 && v24>=-3 ? -4 : v24));
          v28 = r_u16(((uint32)((a1 + 436))));
          w_u16(((uint32)((a1 + 16))),0);
          xport_draft_host_sub_800667CC_p3(a1+116u,v28,angles);
          if (((r_u32(((uint32)((a1 + 396)))) & 0x20) != 0))
            v29 = ((((angles[0]>>16)&65535u) - r_u16(((uint32)((a1 + 18))))) & 0xFFF);
          else
            v29 = 0;
          v30 = (v29 << 16);
          if ((((short)(v29)) >= 2049))
          {
            v29 -= 4096;
            v30 = (v29 << 16);
          }
          v3 = ((v30 >> 16) < 1025);
          v31 = ((v30 >> 16) < -1024);
          if (v3)
          {
            v3 = !v31;
            v33 = (v29 << 16);
            if (v3)
              goto LABEL_41;
            v32 = -1024;
          }
          else
          {
            v32 = 1024;
          }
          v33 = (v32 << 16);
          LABEL_41:
          v34 = (((-((v33 >> 16)) >> 1) & 0xFFF) - r_u16(((uint32)((a1 + 20)))));

          v35 = v34;
          if ((((short)(v34)) < -2048))
            v35 = (v34 + 4096);
          v36 = (v35 << 16);
          if ((((short)(v35)) >= 2049))
            v36 = ((v35 - 4096) << 16);
          v37 = (v36 >> 16);
          v3 = (v37 == 0);
          v38 = (v37 >> 3);
          if (!v3)
          {
            w_u16(((uint32)((a1 + 142))),v38);
            goto LABEL_64;
          }
          goto LABEL_62;
        }
        v26 = (a1 + 138);
        if ((v24 < -3))
          goto LABEL_31;
      }
      v25 = (a1 + 132);
      goto LABEL_30;
    }
    if ((v23 != 2))
      goto LABEL_64;
    xport_draft_host_sub_80066B8C_p1(angles,a1+4u,a1+484u);
    sub_8006C3AC(v79,(a1 + 484),(a1 + 4));
    v39 = (8160 / sub_8006BF04(v79));
    if ((((v39 >= -256) && (v39 < 257)) && (((uint32)((v39 - 1))) >= 3)))
    {
      v40 = (a1 + 132);
      if ((v39 >= 0))
      {
        LABEL_54:
        v41 = (a1 + 138);

        goto LABEL_55;
      }
      v41 = (a1 + 138);
      if ((v39 < -3))
      {
        LABEL_55:
        v42 = r_u16(((uint32)((a1 + 20))));

        v77 = r_u32(((uint32)((a1 + 16))));
        v78 = v42;
        sub_80066CF0((uint32)v77,v42,v40,v41,angles[0],angles[1]&65535u,(uint32)(v39 < -256 ? -256 : v39 > 256 ? 256 : (uint32)(v39-1)<3u ? 4 : v39<0 && v39>=-3 ? -4 : v39));
        v43 = r_u16(((uint32)((a1 + 436))));
        w_u16(((uint32)((a1 + 16))),0);
        sub_800667CC((a1 + 116),v43,&angles[0]);
        goto LABEL_64;
      }
    }
    v40 = (a1 + 132);
    goto LABEL_54;
  }
  v21 = r_u32(((uint32)((a1 + 488))));
  v22 = r_u32(((uint32)((a1 + 492))));
  w_u32(((uint32)((a1 + 4))),r_u32(((uint32)((a1 + 484)))));
  w_u32(((uint32)((a1 + 8))),v21);
  w_u32(((uint32)((a1 + 12))),v22);
  if ((r_u16(((uint32)((a1 + 440)))) < 2u))
  {
    w_u32(((uint32)((a1 + 112))),0);
    w_u32(((uint32)((a1 + 108))),0);
    w_u32(((uint32)((a1 + 104))),0);
  }
  w_u32(((uint32)((a1 + 396))),(r_u32(((uint32)((a1 + 396))))&(~1u)));
  LABEL_64:
  if (r_u8(((uint32)((a1 + 382)))))
    sub_8006C0B8((a1 + 116),0x800A6844u);

  sub_8003C558(a1);
  if (((r_u16(((uint32)((a1 + 440)))) == 2) && r_u32(0x800FF5A0u)))
  {
    if ((sub_80066918((a1 + 4),(r_u32(0x800FF5A0u) + 4)) >= r_u16(((uint32)((a1 + 442))))))
    {
      v55 = r_u32(((uint32)((a1 + 520))));
      w_u32(((uint32)((a1 + 396))),(r_u32(((uint32)((a1 + 396))))&(~8u)));
      if (v55)
      {
        sub_8006A294(v55);
        w_u32(((uint32)((a1 + 520))),0);
      }
    }
    else
    {
      if (((r_u32(((uint32)((a1 + 396)))) & 8) == 0))
        sub_80064A08((r_u32(((uint32)(((4 * r_u16(((uint32)((a1 + 214))))) + r_u32(0x800FF624u))))) + 6));
      v48 = r_u32(((uint32)((a1 + 520))));
      w_u32(((uint32)((a1 + 396))),(r_u32(((uint32)((a1 + 396))))|(8u)));
      if (!v48)
        w_u32(((uint32)((a1 + 520))),sub_80069DF0(11,0x2000,0));
      v49 = 0;
      ((void)((r_u32(0x800FF5A0u) + r_u16(((uint32)((r_u32(((uint32)((r_u32(0x800FF5A0u) + 68)))) + 48)))))),(void)(2),(void)(0x800A71CCu),abort(),0u);
      while ((v49 < sub_80066570(4)))
      {
        v50 = sub_80066570(4096);
        v51 = sub_80066570((r_u16(((uint32)((a1 + 442)))) >> 1));
        v52 = (0x800F863Cu+((v50 & 0xFFF))*4u);
        v53 = (r_u16((((uint32)(v52))+(1)*2u)) * v51);
        direction[0] = (r_u16(((uint32)(v52))) * v51);
        direction[1] = 0;
        direction[2] = v53;
        sub_8006C0B8(&direction[0],(r_u32(0x800FF5A0u) + 4));
        direction[1] = r_u32(((uint32)((a1 + 8))));
        v54 = sub_80032DC0(120);
        if (v54)
          v54 = xport_draft_host_sub_80035478_p2(v54,&direction[0],0,512,1,1,0xFFFFFFFFu);
        w_u16(((uint32)((v54 + 64))),64);
        ++v49;
      }

    }
  }
  v56 = r_u32(((uint32)((a1 + 524))));
  if (((v56 != -1) && !((r_u32(0x800FF2F0u) % v56))))
    ((void)(r_u32(((uint32)((a1 + 520))))),(void)((a1 + 4)),(void)(0),abort(),0u);
  result = (r_u32(((uint32)((a1 + 396)))) & 4);
  w_u32(((uint32)((a1 + 512))),0);
  if (result)
  {
    result = r_u32(0x800FF5A0u);
    if (r_u32(0x800FF5A0u))
    {
      v58 = r_u32(((uint32)((r_u32(0x800FF5A0u) + 8))));
      v59 = r_u32(((uint32)((r_u32(0x800FF5A0u) + 12))));
      direction[0] = r_u32(((uint32)((r_u32(0x800FF5A0u) + 4))));
      direction[1] = v58;
      direction[2] = v59;
      v60 = r_u32(((uint32)((a1 + 4))));
      v61 = r_u32(((uint32)((a1 + 528))));
      result = (v60 - v61);
      if ((direction[0] < (v60 + v61)))
      {
        result = (result < direction[0]);
        if (result)
        {
          v62 = r_u32(((uint32)((a1 + 12))));
          v63 = r_u32(((uint32)((a1 + 536))));
          result = (v62 - v63);
          if ((direction[2] < (v62 + v63)))
          {
            result = (result < direction[2]);
            if (result)
            {
              v64 = r_u32(((uint32)((a1 + 8))));
              v65 = r_u32(((uint32)((a1 + 532))));
              result = (v64 - v65);
              if ((direction[1] < (v64 + v65)))
              {
                result = (result < direction[1]);
                if (result)
                {
                  v66 = r_u32(((uint32)((a1 + 104))));
                  v67 = r_u32(((uint32)((a1 + 112))));
                  result = ((void)(v67),abort(),0u);
                  if ((result >= ((sint32)(((void)(v66),abort(),0u)))))
                  {
                    if ((result > 0))
                    {
                      v70 = (v62 + v63);
                      if ((v67 <= 0))
                      {
                        v70 = (v62 - v63);
                        v71 = -262144;
                      }
                      else
                      {
                        v71 = 0x40000;
                      }
                      result = (v70 + v71);
                      w_u32(((uint32)((r_u32(0x800FF5A0u) + 12))),result);
                    }
                  }
                  else
                  {
                    v68 = (v60 + v61);
                    if ((v66 <= 0))
                    {
                      v68 = (v60 - v61);
                      v69 = -262144;
                    }
                    else
                    {
                      v69 = 0x40000;
                    }
                    result = (v68 + v69);
                    w_u32(((uint32)((r_u32(0x800FF5A0u) + 4))),result);
                  }
                }
              }
            }
          }
        }
      }
    }
  }
  return result;
}



uint32 sub_8003C558(uint32 a1)
{
  short v2;
  sint32 v3;
  sint32 v4;
  unsigned short v5;
  sint32 v6;
  sint32 result;
  v2 = r_u16(0x800FF5E8u);
  w_u32(((uint32)((a1 + 164))),0x800FF5E4u);
  w_u16(((uint32)((a1 + 168))),v2);
  v3 = r_u16(((uint32)((a1 + 440))));
  v4 = (v3 != 0);
  v5 = (v3 - 1);
  if (v4)
  {
    result = (v5 < 2u);
    if (!result)
      return result;
    sub_8006C0B8((a1 + 104),(a1 + 116));
    sub_8006C270((a1 + 104),(a1 + 129));
    sub_8006C05C((a1 + 104));
    sub_8006C0B8((a1 + 4),(a1 + 104));
    v6 = (a1 + 132);
    sub_8006C730((a1 + 16),(a1 + 132));
    sub_8006C624((a1 + 16));
    sub_8006C730((a1 + 132),(a1 + 138));
    sub_8006C8E8((a1 + 132),(a1 + 144));
  }
  else
  {
    sub_8006C0B8((a1 + 104),(a1 + 116));
    sub_8006C05C((a1 + 104));
    sub_8006C0B8((a1 + 4),(a1 + 104));
    v6 = (a1 + 132);
    sub_8006C730((a1 + 16),(a1 + 132));
    sub_8006C624((a1 + 16));
    sub_8006C730((a1 + 132),(a1 + 138));
  }
  return sub_8006C64C(v6);
}


/* TODO Missing call adapter indirect */
/* TODO Missing call adapter sub_8003A8B0 */
/* TODO Missing call adapter sub_8005C11C */
/* TODO Missing call adapter sub_80069FAC */
uint32 sub_8004D800(uint32 a1)
{
  short v2;
  sint8 v3;
  sint32 v4;
  sint32 v5;
  short v6;
  sint32 i;
  sint32 v8;
  sint32 v9;
  sint32 v10;
  sint32 v11;
  sint32 v12;
  sint32 result;
  sint32 v14;
  sint32 v15;
  sint32 v16;
  uint32 v17;
  sint32 v18;
  sint32 v19;
  sint32 v20;
  sint32 v21;
  sint32 v22;
  uint32 v23;
  sint32 v24;
  sint32 v25;
  uint32 v26;
  sint32 v27;
  sint32 v28;
  sint32 v29;
  sint32 v30;
  sint32 v31;
  sint32 v32;
  sint32 v33;
  sint32 v34;
  int v35[4];
  char v36[16];
  char v37[16];
  sint32 v38;
  uint32 v39;
  v2 = r_u16(((uint32)((a1 + 76))));
  w_u16(((uint32)((a1 + 388))),v2);
  if (((v2 & 1) != 0))
  {
    v3 = r_u8(((uint32)((a1 + 384))));
    w_u16(((uint32)((a1 + 76))),(v2 & 0xFFFE));
    w_u8(((uint32)((a1 + 384))),(v3 + 1));
  }
  if (r_u8(((uint32)((a1 + 380)))))
  {
    v4 = r_u16(((uint32)((a1 + 476))));
    v5 = (v4 == 0);
    v6 = (v4 - 1);
    if (v5)
      sub_8004BE90(a1,r_u32(((uint32)((a1 + 400)))));
    else
      w_u16(((uint32)((a1 + 476))),v6);
  }
  else
  {
    ((void)((a1 + r_u16(((uint32)((r_u32(((uint32)((a1 + 68)))) + 16)))))),abort(),0u);
  }
  if ((((r_u32(((uint32)((a1 + 396)))) & 0x100) != 0) && !sub_80066570(4)))
  {
    w_u32(0x800FF3ACu,0);
    for (i = (sub_80066570(3) + 2); i; --i)
    {
      v8 = r_u32(((uint32)((a1 + 8))));
      v9 = r_u32(((uint32)((a1 + 12))));
      v29 = r_u32(((uint32)((a1 + 4))));
      v30 = v8;
      v31 = v9;
      v29 += ((sub_80066570(128) - 64) << 12);
      v30 += ((sub_80066570(128) - 64) << 12);
      v31 += ((sub_80066570(128) - 64) << 12);
      v10 = sub_80032DC0(116);
      if (v10)
        ((void)(v10),(void)((a1 + 4)),abort(),0u);
    }

    w_u32(0x800FF3ACu,1);
  }
  v11 = r_u32(((uint32)((a1 + 516))));
  if (((v11 != -1) && !((r_u32(0x800FF2F0u) % v11))))
    ((void)(r_u32(((uint32)((a1 + 512))))),(void)((a1 + 4)),(void)(0),abort(),0u);
  v12 = r_u32(((uint32)((a1 + 496))));
  if (v12)
    ((void)(v12),abort(),0u);
  result = r_u32(((uint32)((a1 + 504))));
  v14 = 0;
  if (result)
  {
    v39 = &v32;
    v15 = r_u32(((uint32)((r_u32(0x800FF5A0u) + 188))));
    while (1)
    {
      result = (v14 < r_u16(((uint32)((a1 + 500)))));
      if ((v14 >= r_u16(((uint32)((a1 + 500))))))
        break;
      v16 = (r_u32(((uint32)((a1 + 504)))) + (16 * v14));
      v17 = r_u32(((uint32)((v16 + 12))));
      v18 = r_u32((v17+(21)*4u));
      v19 = r_u32((v17+(22)*4u));
      v29 = r_u32((v17+(20)*4u));
      v30 = v18;
      v31 = v19;
      sub_8006C0B8(&v29,v16);
      if ((v15 < v30))
      {
        v20 = ((sub_80066570(2400) - 1200) << 12);
        v22 = ((sub_80066570(400) - 1200) << 12);
        v21 = sub_80066570(2400);
        v32 = v20;
        v33 = v22;
        v34 = ((v21 - 1200) << 12);
        sub_8006C34C(v35,(r_u32(0x800FF5A0u) + 4),&v32);
        v29 = v35[0];
        v30 = v35[1];
        v31 = v35[2];
      }
      v23 = r_u32(((uint32)((v16 + 12))));
      v24 = v30;
      v25 = v31;
      w_u32((v23+(20)*4u),v29);
      w_u32((v23+(21)*4u),v24);
      w_u32((v23+(22)*4u),v25);
      v26 = r_u32(((uint32)((r_u32(((uint32)((v16 + 12)))) + 76))));
      ++v14;
      sub_8006C3AC(v36,&v29,v16);
      v38 = 1;
      sub_8006C564(v37,(r_u32(0x800FF904u) + 104),&v38);
      sub_8006C34C(v39,v36,v37);
      v27 = v33;
      v28 = v34;
      w_u32(v26,v32);
      w_u32((v26+(1)*4u),v27);
      w_u32((v26+(2)*4u),v28);
    }

  }
  return result;
}


/* TODO Missing call adapter indirect */
/* TODO Missing call adapter sub_80010D3C */
/* TODO Missing call adapter sub_800322F4 */
/* TODO Missing call adapter sub_8003243C */
/* TODO Missing call adapter sub_8003A154 */
/* TODO Missing call adapter sub_8003A214 */
/* TODO Missing call adapter sub_8003A228 */
/* TODO Missing call adapter sub_8005ABF8 */
uint32 sub_8004DC40(uint32 a1, uint32 a2)
{
  sint32 v3;
  uint32 v4;
  unsigned short v5;
  sint32 v6;
  sint32 v7;
  uint32 v8;
  unsigned short v9;
  sint32 v10;
  sint32 v11;
  sint32 v12;
  sint32 v13;
  sint32 result;
  uint32 v15;
  sint32 v16;
  uint32 v17;
  uint32 v18;
  sint32 v19;
  unsigned short v20;
  unsigned short v21;
  sint32 v22;
  sint32 v23;
  sint32 v24;
  sint32 v25;
  sint32 v26;
  sint32 v27;
  sint32 v28;
  unsigned short v29;
  sint32 v30;
  sint32 v31;
  uint32 v32;
  sint32 v33;
  sint32 v34;
  uint32 v35;
  sint32 v36;
  sint32 v37;
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
  uint32 v48;
  sint32 v49;
  sint32 v50;
  sint32 v51;
  sint32 v52;
  sint32 v53;
  sint32 v54;
  sint32 v55;
  sint32 v56;
  sint32 v57;
  sint32 v58;
  sint32 v59;
  int v60[4];
  char v61[16];
  char v62[16];
  sint32 v63;
  if ((a2 == 17667))
  {
    v29 = sub_8004CF78(a1);
    v30 = 0;
    w_u16(((uint32)((a1 + 500))),v29);
    v31 = sub_8006B864((16 * v29),0,1);
    v32 = ((uint32)(0x800FF5A0u));
    w_u32(((uint32)((a1 + 504))),v31);
    w_u32(0x800FF3ACu,0);
    v33 = r_u32((v32+(2)*4u));
    v34 = r_u32((v32+(3)*4u));
    v54 = r_u32((v32+(1)*4u));
    v55 = v33;
    v56 = v34;
    while ((v30 < r_u16(((uint32)((a1 + 500))))))
    {
      v35 = ((uint32)((r_u32(((uint32)((a1 + 504)))) + (16 * v30))));
      v36 = ((sub_80066570(2400) - 1200) << 12);
      v38 = ((sub_80066570(400) - 1200) << 12);
      v37 = sub_80066570(2400);
      v57 = v36;
      v58 = v38;
      v59 = ((v37 - 1200) << 12);
      sub_8006C0B8(&v57,&v54);
      v39 = ((sub_80066570(16) - 8) << 12);
      v41 = ((sub_80066570(48) + 48) << 12);
      v40 = sub_80066570(16);
      w_u32(v35,v39);
      w_u32((v35+(1)*4u),v41);
      w_u32((v35+(2)*4u),((v40 - 8) << 12));
      v42 = sub_80032DC0(100);
      if (v42)
        v42 = ((void)(v42),(void)(1),abort(),0u);
      w_u32((v35+(3)*4u),v42);
      w_u8(((uint32)((v42 + 66))),1);
      v43 = v58;
      v44 = v59;
      w_u32(((uint32)((v42 + 80))),v57);
      w_u32(((uint32)((v42 + 84))),v43);
      w_u32(((uint32)((v42 + 88))),v44);
      v45 = r_u32(((uint32)((v42 + 76))));
      sub_8006C3AC(v61,&v57,v35);
      ++v30;
      v63 = 1;
      sub_8006C564(v62,(r_u32(0x800FF904u) + 104),&v63);
      sub_8006C34C(v60,v61,v62);
      v46 = v60[1];
      v47 = v60[2];
      w_u32(v45,v60[0]);
      w_u32((v45+(1)*4u),v46);
      w_u32((v45+(2)*4u),v47);
      w_u8(((uint32)((v42 + 92))),64);
      w_u8(((uint32)((v42 + 93))),64);
      w_u8(((uint32)((v42 + 94))),80);
      w_u8(((uint32)((r_u32(((uint32)((v42 + 76)))) + 12))),16);
      w_u8(((uint32)((r_u32(((uint32)((v42 + 76)))) + 13))),16);
      w_u8(((uint32)((r_u32(((uint32)((v42 + 76)))) + 14))),32);
      ((void)(v42),abort(),0u);
    }

    w_u32(0x800FF3ACu,1);
    return 1;
  }
  if ((a2 >= 0x4504u))
  {
    if ((a2 == 17671))
    {
      v3 = r_u32(((uint32)((a1 + 512))));
      if (v3)
        sub_8006A294(v3);
      v4 = r_u32(((uint32)((a1 + 400))));
      v5 = r_u16(v4);
      w_u32(((uint32)((a1 + 400))),(v4+(1)*2u));
      w_u32(((uint32)((a1 + 512))),sub_80069DF0(v5,0x2000,0));
      w_u32(((uint32)((a1 + 516))),-1);
      return 1;
    }
    if ((a2 >= 0x4508u))
    {
      if ((a2 == 17673))
      {
        v11 = r_u32(((uint32)((a1 + 512))));
        if (v11)
          sub_8006A294(v11);
        w_u32(((uint32)((a1 + 512))),0);
      }
      else
        if ((a2 < 0x4509u))
      {
        v6 = r_u32(((uint32)((a1 + 512))));
        v7 = (a1 + 4);
        if (v6)
        {
          sub_8006A294(v6);
          v7 = (a1 + 4);
        }
        v8 = r_u32(((uint32)((a1 + 400))));
        v9 = r_u32(((v8+=2u)-2u));
        w_u32(((uint32)((a1 + 400))),v8);
        v10 = r_u16(v8);
        w_u32(((uint32)((a1 + 400))),(v8+(1)*2u));
        w_u32(((uint32)((a1 + 516))),v10);
        w_u32(((uint32)((a1 + 512))),sub_80069EF4(v9,v7,0));
      }
      else
      {
        if ((a2 != 18176))
          return sub_8004BF3C(a1,a2);
        v15 = r_u32(((uint32)((a1 + 400))));
        v16 = r_u16(v15);
        w_u32(((uint32)((a1 + 400))),(v15+(1)*2u));
        if (v16)
          v17 = (r_u32(((uint32)((a1 + 396)))) | 0x100);
        else
          v17 = (r_u32(((uint32)((a1 + 396)))) & 0xFFFFFEFF);
        w_u32(((uint32)((a1 + 396))),v17);
      }
      return 1;
    }
    if ((a2 != 17669))
    {
      if ((a2 >= 0x4506u))
      {
        if (0x800FF5A0u)
        {
          v12 = r_u32(((uint32)((0x800FF5A0u + 8))));
          v13 = r_u32(((uint32)((0x800FF5A0u + 12))));
          w_u32(((uint32)((a1 + 4))),r_u32(((uint32)((0x800FF5A0u + 4)))));
          w_u32(((uint32)((a1 + 8))),v12);
          w_u32(((uint32)((a1 + 12))),v13);
          return 1;
        }
      }
      else
      {
        v18 = r_u32(((uint32)((a1 + 400))));
        v19 = 0x800FF5A0u;
        v20 = r_u32(((v18+=2u)-2u));
        w_u32(((uint32)((a1 + 400))),v18);
        v21 = r_u16(v18);
        w_u32(((uint32)((a1 + 400))),(v18+(1)*2u));
        w_u32(((uint32)((a1 + 400))),(v18+(2)*2u));
        w_u32(((uint32)((a1 + 400))),(v18+(3)*2u));
        if (v19)
        {
          v22 = (((uint32)(sub_80066918((a1 + 4),(v19 + 4)))) >= 0x1000);
          result = 1;
          if (v22)
            return result;
          v23 = sub_800625AC(504);
          if (v23)
          {
            ((void)(v23),(void)((a1 + 4)),(void)(v20),(void)(v21),abort(),0u);
            return 1;
          }
        }
      }
      return 1;
    }
    v48 = ((uint32)(sub_8002FED8(40)));
    if (v48)
      v48 = ((void)(((sint32)(v48))),(void)(3),(void)(a1),(void)(((sint32)(0x800FF5A0u))),(void)(0),(void)(v49),(void)(v50),(void)(v51),(void)(v52),abort(),0u);
    LABEL_64:
    w_u32(((uint32)((a1 + 496))),v48);

    ((void)(((sint32)(v48))),(void)((a1 + 4)),abort(),0u);
    ((void)(r_u32(((uint32)((a1 + 496))))),(void)((a1 + 16)),abort(),0u);
    return 1;
  }
  if ((a2 == 17057))
  {
    v26 = sub_80032DC0(364);
    if (v26)
      v26 = ((void)(v26),(void)((a1 + 4)),(void)((a1 + 16)),(void)(r_u16(r_u32(((uint32)((a1 + 400)))))),(void)(0xFFFF),(void)(r_u16(((uint32)((r_u32(((uint32)((a1 + 400)))) + 2))))),(void)(r_u16(((uint32)((r_u32(((uint32)((a1 + 400)))) + 4))))),(void)(r_u16(((uint32)((r_u32(((uint32)((a1 + 400)))) + 6))))),(void)(r_u16(((uint32)((r_u32(((uint32)((a1 + 400)))) + 8))))),(void)(r_u8(((uint32)((r_u32(((uint32)((a1 + 400)))) + 10))))),(void)(1756868918),(void)(v53),(void)(v54),(void)(v55),(void)(v56),abort(),0u);
    v27 = r_u32(((uint32)((a1 + 400))));
    w_u32(((uint32)((a1 + 508))),v26);
    v25 = (v27 + 12);
    goto LABEL_46;
  }
  if ((a2 >= 0x42A2u))
  {
    if ((a2 == 17665))
    {
      v48 = ((uint32)(sub_8002FED8(40)));
      if (v48)
        v48 = ((void)(((sint32)(v48))),(void)(1),(void)(a1),(void)(((sint32)(0x800FF5A0u))),(void)(0),(void)(v49),(void)(v50),(void)(v51),(void)(v52),abort(),0u);
    }
    else
      if ((a2 >= 0x4502u))
    {
      v48 = ((uint32)(sub_8002FED8(40)));
      if (v48)
        v48 = ((void)(((sint32)(v48))),(void)(2),(void)(a1),(void)(((sint32)(0x800FF5A0u))),(void)(0),(void)(v49),(void)(v50),(void)(v51),(void)(v52),abort(),0u);
    }
    else
    {
      if ((a2 != 17664))
        return sub_8004BF3C(a1,a2);
      v48 = ((uint32)(sub_8002FED8(40)));
      if (v48)
        v48 = ((void)(((sint32)(v48))),(void)(0),(void)(a1),(void)(((sint32)(0x800FF5A0u))),(void)(0),(void)(v49),(void)(v50),(void)(v51),(void)(v52),abort(),0u);
    }
    goto LABEL_64;
  }
  if ((a2 == 17051))
  {
    v24 = sub_80032DC0(364);
    if (v24)
      v24 = ((void)(v24),(void)((a1 + 4)),(void)((a1 + 16)),(void)(r_u16(r_u32(((uint32)((a1 + 400)))))),(void)(0xFFFF),(void)(r_u16(((uint32)((r_u32(((uint32)((a1 + 400)))) + 2))))),(void)(r_u16(((uint32)((r_u32(((uint32)((a1 + 400)))) + 4))))),(void)(r_u16(((uint32)((r_u32(((uint32)((a1 + 400)))) + 6))))),(void)(r_u16(((uint32)((r_u32(((uint32)((a1 + 400)))) + 8))))),(void)(r_u8(((uint32)((r_u32(((uint32)((a1 + 400)))) + 10))))),(void)(-857728900),(void)(v53),(void)(v54),(void)(v55),(void)(v56),abort(),0u);
    w_u32(((uint32)((a1 + 508))),v24);
    w_u8(((uint32)((v24 + 66))),1);
    v25 = (r_u32(((uint32)((a1 + 400)))) + 12);
    LABEL_46:
    w_u32(((uint32)((a1 + 400))),v25);

    return 1;
  }
  if ((a2 != 17055))
    return sub_8004BF3C(a1,a2);
  v28 = r_u32(((uint32)((a1 + 508))));
  if (v28)
    ((void)((v28 + r_u16(((uint32)((r_u32(((uint32)((v28 + 68)))) + 8)))))),(void)(3),abort(),0u);
  w_u32(((uint32)((a1 + 508))),0);
  return 1;
}



uint32 sub_80033688(uint32 a1)
{
  sub_80032F7C(((sint32)(a1)));
  w_u32((a1+(17)*4u),0x800A1D50u);
  w_u32((a1+(36)*4u),746619008);
  sub_80032E50(a1,0x800FF444u);
  return a1;
}



uint32 sub_80032E50(uint32 a1, uint32 a2)
{
  sint32 v2;
  uint32 result;
  v2 = r_u32(a2);
  w_u32(a1,0);
  w_u32((a1+(1)*4u),v2);
  w_u32(a2,((sint32)(a1)));
  result = ((uint32)(r_u32((a1+(1)*4u))));
  if (result)
    w_u32(result,a1);
  return result;
}



uint32 sub_80033764(uint32 a1, uint32 a2, uint32 a3)
{
  sint32 result;
  result = r_u32(((uint32)((((8 * a3) + sub_8006F164(a2)) + 4))));
  w_u32(((uint32)((a1 + 148))),result);
  return result;
}



uint32 sub_80033874(uint32 a1)
{
  sint32 result;
  result = (r_u32(((uint32)((a1 + 144)))) | 0x2000000);
  w_u32(((uint32)((a1 + 144))),result);
  return result;
}



uint32 sub_8006C34C(uint32 a1, uint32 a2, uint32 a3)
{
  uint32 result;
  sint32 v4;
  sint32 v5;
  result = a1;
  v4 = (r_u32((a2+(1)*4u)) + r_u32((a3+(1)*4u)));
  v5 = (r_u32((a2+(2)*4u)) + r_u32((a3+(2)*4u)));
  w_u32(a1,(r_u32(a2) + r_u32(a3)));
  w_u32((a1+(1)*4u),v4);
  w_u32((a1+(2)*4u),v5);
  return result;
}


/* TODO Missing call adapter indirect */
void sub_8006FD18(void)
{
  uint32 i;
  uint32 v1;
  uint32 v2;
  for (i = ((uint32)(r_u32(0x800FF7DCu))); i; i = v1)
  {
    v1 = ((uint32)(r_u32((i+(5)*4u))));
    if ((((uint32)(sub_8006696C(((uint32)((r_u32(0x800FF5A0u) + 4))),(i+(2)*4u)))) < 0x2328))
    {
      v2 = sub_800641E8(r_u16((((uint32)(i))+(2)*2u)));
      if (v2)
      {
        if ((r_u16((((uint32)(i))+(3)*2u)) >= 0))
          w_u16((v2+(109)*2u),r_u16((((uint32)(i))+(3)*2u)));
        w_u16((v2+(39)*2u),(r_u16((v2+(39)*2u))|(4u)));
        ((void)((((sint32)(i)) + r_u16(((uint32)((r_u32(i) + 8)))))),(void)(3),abort(),0u);
      }
    }
  }

}



void sub_800638A4(void)
{
  sint32 v0;
  sint32 i;
  v0 = r_u32(0x800FF5E0u);
  if (r_u32(0x800FF5E0u))
  {
    for (i = r_u32(((uint32)((r_u32(0x800FF5E0u) + 28))));; i = r_u32(((uint32)((i + 28)))))
    {
      if ((((r_u16(((uint32)((v0 + 78)))) & 2) != 0) && (r_u32(0x800FF5ECu) >= ((uint32)(sub_8006696C((r_u32(0x800FF5A0u) + 4),(v0 + 4)))))))
        sub_80062BF0(v0);
      v0 = i;
      if (!i)
        break;
    }

  }
}



uint32 sub_80032C88(void)
{
  sub_80032C18(r_u32(0x800FF434u));
  sub_80032C18(r_u32(0x800FF438u));
  sub_80032C18(r_u32(0x800FF43Cu));
  sub_80032C18(r_u32(0x800FF440u));
  sub_80032C18(r_u32(0x800FF45Cu));
  sub_80032C18(r_u32(0x800FF460u));
  sub_80032C18(r_u32(0x800FF444u));
  sub_80032C18(r_u32(0x800FF448u));
  sub_80032C18(r_u32(0x800FF44Cu));
  sub_80032C18(r_u32(0x800FF450u));
  sub_80032C18(r_u32(0x800FF454u));
  sub_80032C18(r_u32(0x800FF464u));
  return sub_80032C18(r_u32(0x800FF458u));
}


/* TODO Missing call adapter indirect */
/* TODO Missing call adapter sub_800103E4 */
/* TODO Missing call adapter sub_80010530 */
/* TODO Missing call adapter sub_80010A38 */
/* TODO Missing call adapter sub_80010CBC */
/* TODO Missing call adapter sub_800154E0 */
/* TODO Missing call adapter sub_80017760 */
/* TODO Missing call adapter sub_8001779C */
/* TODO Missing call adapter sub_8001BE6C */
/* TODO Missing call adapter sub_80076D6C */
/* TODO Postincrement memory expressions may require ordering refinement */
uint32 sub_80017914(void)
{
  sint32 v0;
  sint32 v1;
  sint32 v2;
  sint32 v3;
  sint32 v4;
  sint32 v5;
  sint32 v6;
  sint32 v7;
  sint32 v8;
  uint32 v9;
  sint8 v10;
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
  uint32 v21;
  uint32 v22;
  sint32 v23;
  sint32 v24;
  sint32 v25;
  sint32 v26;
  uint32 v27;
  uint32 v28;
  sint32 v29;
  sint32 v30;
  sint32 v31;
  sint32 v32;
  sint32 v33;
  sint32 v34;
  sint32 result;
  sint32 v36;
  sint32 v37;
  sint32 v38;
  short v39;
  sint8 v40;
  sint8 v41;
  short v42;
  sint32 v43;
  sint32 v44;
  sint32 v45;
  sint32 v46;
  v0 = -1;
  v1 = 0;
  if ((!r_u8(0x800EC138u) || (r_u32(0x800FF080u) != 1)))
  {
    sub_80017364(&v43,&v44,&v45,&v46);
    sub_80076D44();
  }
  v2 = 0;
  if (r_u32(0x800FF080u))
    sub_80010AC8(0,0);
  if ((r_u8(0x800EC1E8u) && r_u8(0x800EC1D8u)))
  {
    if (!r_u32(0x800FF098u))
    {
      w_u32(0x800FF098u,1);
      w_u32(0x800FF09Cu,r_u32(0x800FF64Cu));
    }
    if ((((uint32)((r_u32(0x800FF64Cu) - r_u32(0x800FF09Cu)))) >= 0x79))
    {
      w_u32(0x800FF098u,0);
      w_u32(0x800FF2ECu,7);
      v1 = 1;
    }
  }
  else
  {
    w_u32(0x800FF098u,0);
  }
  switch (r_u32(0x800FF080u))
  {
    case 0:
      if ((v46 || !r_u32(0x800EC264u)))
    {
      sub_8001B708();
      sub_80070100(0x800EC0F8u);
      w_u32(0x800FF00Cu,0);
      sub_8006A114();
      sub_80069DF0(23,0x2000,0);
      sub_80070288(0,0);
      sub_80070288(0,1);
      w_u32(0x800FF008u,1);
      v3 = sub_8002FED8(444);
      if (v3)
        v3 = sub_80015228(v3,256u,0u,0u,320u,256u,26u);
      w_u32(0x800FF084u,v3);
      sub_80015474(v3,((sint32)(r_u32((0x800A554Cu+(0)*4u)))));
      sub_80015474(r_u32(0x800FF084u),((sint32)(r_u32((0x800A5580u+(0)*4u)))));
      if (!r_u32(0x800FF338u))
        sub_8001551C(r_u32(0x800FF084u),((sint32)(r_u32((0x800A5580u+(0)*4u)))));
      sub_80015474(r_u32(0x800FF084u),((sint32)(r_u32((0x800A555Cu+(0)*4u)))));
      sub_800152F8(r_u32(0x800FF084u));
      w_u32(((uint32)((r_u32(0x800FF084u) + 16))),(r_u32(((uint32)((r_u32(0x800FF084u) + 16))))+(5)));
      if (!r_u32(0x800EC264u))
        goto LABEL_62;
      w_u32(0x800FF080u,1);
    }
      goto LABEL_190;

    case 1:
      sub_8006CDC4();
      if (!r_u32(0x800FF008u))
      v1 = 1;
      if (!r_u32(0x800EC264u))
      w_u32(0x800FF080u,4);
      if (r_u8(0x800EC138u))
    {
      if ((abort(),0u))
      {
        w_u32(0x800FF00Cu,32);
        w_u32(0x800FF010u,0);
      }
      sub_80070100(0x800EC0F8u);
      goto LABEL_190;
    }
      if (r_u32(0x800FF338u))
      ((void)(r_u32(0x800FF084u)),(void)(((sint32)(r_u32((0x800A5580u+(0)*4u))))),abort(),0u);
    else
      sub_8001551C(r_u32(0x800FF084u),((sint32)(r_u32((0x800A5580u+(0)*4u)))));
      sub_80015BA4(r_u32(0x800FF084u));
      v4 = -1;
      if (!v43)
      goto LABEL_43;
      if (v46)
    {
      v5 = 19;
      v1 = 1;
    }
    else
    {
      if (sub_800155D4(r_u32(0x800FF084u),((sint32)(r_u32((0x800A554Cu+(0)*4u))))))
      {
        v0 = 19;
        v1 = 1;
      }
      if (sub_800155D4(r_u32(0x800FF084u),((sint32)(r_u32((0x800A5580u+(0)*4u))))))
      {
        v0 = 23;
        w_u32(0x800FF080u,2);
      }
      v6 = sub_800155D4(r_u32(0x800FF084u),((sint32)(r_u32((0x800A555Cu+(0)*4u)))));
      v4 = v0;
      if (!v6)
        goto LABEL_43;
      v5 = 23;
      sub_800153D8(r_u32(0x800FF024u));
      w_u32(0x800FF080u,3);
    }
      v4 = v5;
      LABEL_43:
    sub_80069DF0(v4,0x2000,0);

      goto LABEL_190;

    case 2:
      if (!r_u32(0x800FF088u))
    {
      v7 = sub_8002FED8(444);
      if (v7)
        v7 = sub_80015228(v7,256u,0u,0u,256u,192u,26u);
      v8 = 0;
      v9 = r_u32(0x800A71FCu);
      w_u32(0x800FF088u,v7);
      w_u32(((uint32)((v7 + 20))),18);
      while ((v8 < r_u32(0x800FF618u)))
      {
        v39 = r_u16(((uint32)(r_u32(v9))));
        v10 = r_u8(((uint32)((r_u32(v9) + 2))));
        v41 = 0;
        v40 = v10;
        if (!sub_80067724(&v39,0x800FF0A8u))
          sub_80015474(r_u32(0x800FF088u),r_u32(v9));
        (v9+=4u);
        ++v8;
      }

      sub_800152F8(r_u32(0x800FF088u));
    }
      sub_80015BA4(r_u32(0x800FF088u));
      if (v43)
    {
      sub_80063DD4(r_u32(((uint32)(((r_u32(0x800FF088u) + (28 * r_u8(((uint32)((r_u32(0x800FF088u) + 6)))))) + 24)))));
      if (r_u32(0x800FF088u))
        ((void)((r_u32(0x800FF088u) + r_u16(((uint32)((r_u32(((uint32)(r_u32(0x800FF088u)))) + 8)))))),(void)(3),abort(),0u);
      v1 = 1;
      w_u32(0x800FF088u,0);
      w_u32(0x800FF2ECu,6);
      sub_80069DF0(23,0x2000,0);
    }
      if (v44)
    {
      if (r_u32(0x800FF088u))
        ((void)((r_u32(0x800FF088u) + r_u16(((uint32)((r_u32(((uint32)(r_u32(0x800FF088u)))) + 8)))))),(void)(3),abort(),0u);
      w_u32(0x800FF088u,0);
      w_u32(0x800FF080u,1);
      sub_80069DF0(19,0x2000,0);
    }
      goto LABEL_190;

    case 3:
      if (!r_u32(0x800EC264u))
    {
      LABEL_62:
      w_u32(0x800FF080u,4);

      goto LABEL_190;
    }
      sub_80015BA4(r_u32(0x800FF024u));
      if (!v43)
      goto LABEL_71;
      if (v46)
    {
      v1 = 1;
    }
    else
    {
      if (sub_800155D4(r_u32(0x800FF024u),r_u32((0x800A5594u+(0)*4u))))
      {
        v1 = 1;
        w_u32(0x800FF2ECu,7);
        v11 = 23;
        goto LABEL_70;
      }
      w_u32(0x800FF080u,1);
    }
      v11 = 19;
      LABEL_70:
    sub_80069DF0(v11,0x2000,0);

      LABEL_71:
    v2 = 19;

      if (v44)
    {
      sub_80069DF0(19,0x2000,0);
      goto LABEL_74;
    }
      goto LABEL_190;

    case 4:
      if (r_u32(0x800EC264u))
      LABEL_74:
    w_u32(0x800FF080u,1);

      goto LABEL_190;

    case 5:
      if (r_u32((0x800FF030u+(r_u32(0x800FF034u))*4u)))
    {
      v2 = 16;
      if ((((uint32)((r_u32(0x800FF64Cu) - r_u32(0x800FFB50u)))) >= 4))
      {
        (w_u32(0x800FF034u,(r_u32(0x800FF034u)+1u)),(r_u32(0x800FF034u)+1u));
        w_u32(0x800FFB50u,r_u32(0x800FF64Cu));
        sub_80069DF0(16,0x2000,0);
      }
    }
    else
    {
      if (!r_u32(0x800FF040u))
        goto LABEL_86;
      w_u32(0x800FFB5Cu,sub_80069DF0(11,0x2000,0));
      w_u32(0x800FF080u,6);
    }
      goto LABEL_190;

    case 6:
      v12 = (r_u32(0x800FFB54u) < 101);
      if ((r_u32(0x800FFB54u) < 0))
    {
      w_u32(0x800FFB54u,0);
      v12 = 1;
    }
      if (!v12)
      w_u32(0x800FFB54u,100);
      if ((r_u32(0x800FFB54u) < (w_u32(0x800FFB58u,(r_u32(0x800FFB58u)+1u)),(r_u32(0x800FFB58u)+1u))))
    {
      w_u32(0x800FFB58u,r_u32(0x800FFB54u));
      sub_8006A294(r_u32(0x800FFB5Cu));
      goto LABEL_86;
    }
      goto LABEL_190;

    case 7:
      sub_8001BEF8(32,1);
      w_u32(0x800FF080u,8);
      goto LABEL_190;

    case 8:
      sub_8001BF34();
      if ((abort(),0u))
    {
      w_u32(0x800FF2ECu,3);
      v1 = 1;
    }
      goto LABEL_190;

    case 9:
      sub_80015BA4(r_u32(0x800FF024u));
      v2 = 23;
      if (v43)
    {
      sub_80069DF0(23,0x2000,0);
      if (!sub_800155D4(r_u32(0x800FF024u),r_u32((0x800A5594u+(0)*4u))))
        goto LABEL_176;
      w_u32(0x800FF044u,0);
      w_u32(0x800FF080u,10);
    }
      goto LABEL_190;

    case 10:
      v13 = (r_u32(0x800FF02Cu) - 1);
      if (r_u32(0x800FF02Cu))
    {
      (w_u32(0x800FF02Cu,(r_u32(0x800FF02Cu)-1u)),(r_u32(0x800FF02Cu)-1u));
      if (!v13)
        w_u32(0x800FF080u,11);
    }
    else
    {
      w_u32(0x800FF02Cu,10);
    }
      goto LABEL_190;

    case 11:
      if ((r_u32(0x800FEEC4u) == -1))
    {
      w_u32(0x800FF080u,18);
    }
    else
      if ((r_u32(0x800FEEC4u) >= 0))
    {
      if ((r_u32(0x800FEEC4u) == 1))
      {
        v14 = ((abort(),0u) == 0);
        v15 = 12;
        if (!v14)
        {
          sub_800664E4(1);
          v14 = (((void)(0),(void)(0),abort(),0u) > 0);
          v15 = 12;
          if (!v14)
            v15 = 17;
        }
        w_u32(0x800FF080u,v15);
        if ((v15 == 12))
        {
          v16 = sub_8002FED8(444);
          if (v16)
            v16 = sub_80015228(v16,256u,55u,0u,256u,192u,18u);
          w_u32(0x800FF028u,v16);
          w_u8(((uint32)((v16 + 9))),1);
          ((void)(r_u32(0x800FF028u)),abort(),0u);
        }
      }
    }
    else
      if ((r_u32(0x800FEEC4u) == -2))
    {
      v17 = r_u32(0x800FF024u);
      v18 = 19;
      goto LABEL_127;
    }
      goto LABEL_190;

    case 12:
      sub_80015BA4(r_u32(0x800FF028u));
      if ((r_u32(0x800FEEC4u) == -1))
    {
      v20 = r_u32(0x800FF028u);
      w_u32(0x800FF080u,18);
      LABEL_121:
      if (v20)
        ((void)((v20 + r_u16(((uint32)((r_u32(((uint32)(v20))) + 8)))))),(void)(3),abort(),0u);

      w_u32(0x800FF028u,0);
      goto LABEL_190;
    }
      if (v44)
    {
      sub_80069DF0(19,0x2000,0);
      (abort(),0u);
      v20 = r_u32(0x800FF028u);
      goto LABEL_121;
    }
      v2 = 23;
      if (v43)
    {
      sub_80069DF0(23,0x2000,0);
      if (r_u32(((uint32)((0x800A493Cu+(((136 * r_u8(((uint32)((r_u32(0x800FF028u) + 6))))) + 512))*1u)))))
      {
        v17 = r_u32(0x800FF024u);
        v18 = 13;
        LABEL_127:
        w_u32(0x800FF080u,v18);

        sub_800153D8(v17);
      }
      else
      {
        v21 = 0x800A53D4u;
        w_u32(0x800A53D4u,sub_80018B44(0x800A53D4u));
        v2 = ((sint32)(r_u32(0x800A5454u)));
        v22 = (0x800A493Cu+(((136 * r_u8(((uint32)((r_u32(0x800FF028u) + 6))))) + 512))*1u);
        do
        {
          v23 = r_u32((v21+(1)*4u));
          v24 = r_u32((v21+(2)*4u));
          v25 = r_u32((v21+(3)*4u));
          w_u32(((uint32)(v22)),r_u32(v21));
          w_u32((((uint32)(v22))+(1)*4u),v23);
          w_u32((((uint32)(v22))+(2)*4u),v24);
          w_u32((((uint32)(v22))+(3)*4u),v25);
          v21 += (4)*4u;
          v22 += (16)*1u;
        }
        while ((v21 != r_u32(0x800A5454u)));
        v26 = r_u32((v21+(1)*4u));
        w_u32(((uint32)(v22)),r_u32(v21));
        w_u32((((uint32)(v22))+(1)*4u),v26);
        w_u32(0x800FF080u,14);
        w_u32(0x800FF0A0u,4);
        w_u32(0x800FF0A4u,4);
      }
    }
      goto LABEL_190;

    case 13:
      sub_80015BA4(r_u32(0x800FF024u));
      if (v43)
    {
      if (sub_800155D4(r_u32(0x800FF024u),r_u32((0x800A5594u+(0)*4u))))
      {
        sub_80069DF0(23,0x2000,0);
        v27 = 0x800A53D4u;
        w_u32(0x800A53D4u,sub_80018B44(0x800A53D4u));
        v28 = (0x800A493Cu+(((136 * r_u8(((uint32)((r_u32(0x800FF028u) + 6))))) + 512))*1u);
        do
        {
          v29 = r_u32((v27+(1)*4u));
          v30 = r_u32((v27+(2)*4u));
          v31 = r_u32((v27+(3)*4u));
          w_u32(((uint32)(v28)),r_u32(v27));
          w_u32((((uint32)(v28))+(1)*4u),v29);
          w_u32((((uint32)(v28))+(2)*4u),v30);
          w_u32((((uint32)(v28))+(3)*4u),v31);
          v27 += (4)*4u;
          v28 += (16)*1u;
        }
        while ((v27 != r_u32(0x800A5454u)));
        v32 = r_u32((v27+(1)*4u));
        w_u32(((uint32)(v28)),r_u32(v27));
        w_u32((((uint32)(v28))+(1)*4u),v32);
        w_u32(0x800FF080u,14);
        w_u32(0x800FF0A0u,4);
        w_u32(0x800FF0A4u,4);
      }
      else
      {
        sub_80069DF0(19,0x2000,0);
        w_u32(0x800FF080u,12);
      }
    }
      v2 = 19;
      if (v44)
      goto LABEL_171;
      goto LABEL_190;

    case 14:
      if ((r_u32(0x800FEEC4u) == -1))
    {
      if (r_u32(0x800FF028u))
        ((void)((r_u32(0x800FF028u) + r_u16(((uint32)((r_u32(((uint32)(r_u32(0x800FF028u)))) + 8)))))),(void)(3),abort(),0u);
      w_u32(0x800FF028u,0);
      w_u32(0x800FF080u,18);
    }
      if ((((w_u32(0x800FF0A0u,(r_u32(0x800FF0A0u)-1u)),(r_u32(0x800FF0A0u)-1u)) < 0) && r_u32(0x800FEEC4u)))
    {
      if ((abort(),0u))
      {
        v34 = (r_u32(0x800FF0A4u) - 1);
        w_u32(0x800FF0A4u,v34);
        if ((v34 >= 0))
          goto LABEL_189;
        w_u32(0x800FF080u,15);
      }
      else
      {
        if (r_u32(0x800FF028u))
          ((void)((r_u32(0x800FF028u) + r_u16(((uint32)((r_u32(((uint32)(r_u32(0x800FF028u)))) + 8)))))),(void)(3),abort(),0u);
        w_u32(0x800FF028u,0);
        w_u32(0x800FF080u,16);
      }
    }
      goto LABEL_190;

    case 15:
      v2 = 23;
      if (v45)
    {
      LABEL_171:
      sub_80069DF0(v2,0x2000,0);

      w_u32(0x800FF080u,12);
    }
      goto LABEL_190;

    case 16:
      v2 = 23;
      if (v45)
    {
      sub_80069DF0(23,0x2000,0);
      LABEL_176:
      w_u32(0x800FF080u,7);

    }
      goto LABEL_190;

    case 17:
      v19 = 18;
      if ((r_u32(0x800FEEC4u) == -1))
      goto LABEL_144;
      if ((r_u32(0x800FEEC4u) >= 0))
    {
      v19 = 10;
      if ((r_u32(0x800FEEC4u) == 2))
        goto LABEL_144;
    }
    else
    {
      v19 = 10;
      if ((r_u32(0x800FEEC4u) == -2))
        goto LABEL_144;
    }
      goto LABEL_145;

    case 18:
      if (((r_u32(0x800FEEC4u) == -2) || (((r_u32(0x800FEEC4u) >= -2) && (r_u32(0x800FEEC4u) < 3)) && (r_u32(0x800FEEC4u) > 0))))
    {
      v19 = 10;
      LABEL_144:
      w_u32(0x800FF080u,v19);

    }
      LABEL_145:
    if (v45)
      goto LABEL_86;

      goto LABEL_190;

    case 19:
      sub_80015BA4(r_u32(0x800FF024u));
      if ((r_u32(0x800FEEC4u) == -1))
    {
      v33 = 18;
      LABEL_153:
      w_u32(0x800FF080u,v33);

      goto LABEL_154;
    }
      if (((r_u32(0x800FEEC4u) >= -1) && (r_u32(0x800FEEC4u) < 3)))
    {
      v33 = 10;
      if ((r_u32(0x800FEEC4u) > 0))
        goto LABEL_153;
    }
      LABEL_154:
    if (v43)
    {
      if (sub_800155D4(r_u32(0x800FF024u),r_u32((0x800A5594u+(0)*4u))))
      {
        sub_80069DF0(23,0x2000,0);
        w_u32(0x800FF080u,20);
        w_u32(0x800FF0A0u,4);
        w_u32(0x800FF0A4u,4);
      }
      else
      {
        v44 = 1;
      }
    }

      v2 = 19;
      if (v44)
    {
      LABEL_169:
      sub_80069DF0(v2,0x2000,0);

      LABEL_86:
      (abort(),0u);

    }
      LABEL_190:
    if (r_u32(0x800FF008u))
    {
      result = r_u32(0x800FF0ACu);
      v36 = 951648256;
      if (!r_u32(0x800FF0ACu))
        result = sub_8002E148(951706923,64,64,127);
    }
    else
    {
      result = sub_8002E13C();
      w_u32(0x800FF0ACu,result);
    }

      if (v1)
    {
      w_u32(0x800FF008u,0);
      w_u32(0x800FF080u,0);
      v37 = 3;
      if (r_u32(0x800FF084u))
        ((void)((r_u32(0x800FF084u) + r_u16(((uint32)((r_u32(((uint32)(r_u32(0x800FF084u)))) + 8)))))),(void)(3),abort(),0u);
      w_u32(0x800FF084u,0);
      if (((r_u32(0x800FF2ECu) != 7) && (r_u32(0x800FF2ECu) != 3)))
        sub_8006A1DC();
      result = r_u32(0x800FF0ACu);
      if (!r_u32(0x800FF0ACu))
        { sub_8002E2B8(); return xport_draft_unknown_cleanup_result_80017914(); }
    }
      return result;

    case 20:
      if (((w_u32(0x800FF0A0u,(r_u32(0x800FF0A0u)-1u)),(r_u32(0x800FF0A0u)-1u)) < 0))
    {
      v2 = 0;
      if (r_u32(0x800FEEC4u))
      {
        if ((((void)(0),(void)(0),abort(),0u) == 1))
        {
          w_u32(0x800FF080u,22);
        }
        else
        {
          v34 = (r_u32(0x800FF0A4u) - 1);
          w_u32(0x800FF0A4u,v34);
          if ((v34 >= 0))
            LABEL_189:
          w_u32(0x800FF0A0u,(60 * (4 - v34)));

          else
            w_u32(0x800FF080u,21);
        }
      }
    }
      goto LABEL_190;

    case 21:
      if ((r_u32(0x800FEEC4u) == -1))
      w_u32(0x800FF080u,18);
      v2 = 23;
      if (v45)
      goto LABEL_169;
      goto LABEL_190;

    case 22:
      v2 = 23;
      if (v45)
    {
      sub_80069DF0(23,0x2000,0);
      w_u32(0x800FF080u,10);
    }
      goto LABEL_190;

    default:
      goto LABEL_190;

  }

}



uint32 sub_800307F0(void)
{
  sint32 v0;
  short v1;
  short v2;
  uint32 result;
  sint32 vars0;
  sint32 vars4;
  sint32 vars8;
  sint32 varsC;
  if (r_u32(0x800FF2E0u))
  {
    sub_8001A7D4(0xFFu,0,0,0);
    sub_8001A7BC(256);
    sub_8001A7B0(0);
    sub_8001AA28(256u,60u,r_u32(0x800FF2E0u),0u,0u,256u);
  }
  v0 = 0;
  w_u16(0x800A67C8u,sub_80078760(0x800FF904u));
  sub_8002E2E8();
  if (!r_u32(0x800FF360u))
  {
    w_u32(0x800FF360u,1);
    w_u32(0x800FFB8Cu,r_u32(0x800FF2F0u));
  }
  if ((((r_u32(0x800FF2F0u) - r_u32(0x800FFB8Cu)) >= 2) && r_u32(0x800FF35Cu)))
  {
    w_u32(0x800FF35Cu,0);
    v0 = 1;
    sub_80030168(((uint32)(r_u32(0x800FF5A0u))));
    sub_80030168(((uint32)(r_u32(0x800FF4E8u))));
    sub_80030168(((uint32)(r_u32(0x800FF5DCu))));
    sub_800303F4();
  }
  else
  {
    w_u32(0x800FF35Cu,1);
    sub_8003032C(((uint32)(r_u32(0x800FF5A0u))));
    sub_8003032C(((uint32)(r_u32(0x800FF4E8u))));
    sub_8003032C(((uint32)(r_u32(0x800FF5DCu))));
    sub_800306D8();
  }
  w_u32(0x800FFB8Cu,r_u32(0x800FF2F0u));
  sub_80063770(0x800FF904u);
  sub_8007E63C(0x800ED51Cu,0x800A67B8u,(r_u32(0x800FF660u) + 112));
  sub_8007EBDC(r_u32(0x800FF794u));
  sub_8007EBDC(r_u32(0x800FF5DCu));
  v1 = r_u16(0x800FFAACu);
  v2 = r_u16(0x800FFAAEu);
  w_u16(0x800FFAACu,8);
  w_u16(0x800FFAAEu,-8);
  sub_8007EBDC(r_u32(0x800FF5A0u));
  w_u16(0x800FFAACu,v1);
  w_u16(0x800FFAAEu,v2);
  sub_8007EBDC(r_u32(0x800FF4E8u));
  sub_8007EBDC(r_u32(0x800FF204u));
  sub_8007EBDC(r_u32(0x800FF220u));
  sub_8007F138(r_u32(0x800FF8A0u));
  sub_8007EBC4();
  sub_8003736C();
  sub_8006AAB8();
  sub_8006D91C();
  sub_80016884();
  sub_8001B8BC();
  sub_8001C004();
  result = ((uint32)(((r_u32(0x800FF660u) + 16496) & 0x7FFFFFFF)));
  w_u32(0x800FF34Cu,((r_u32(0x800FF668u) & 0x7FFFFFFF) - ((uint32)(result))));
  if (v0)
  {
    sub_80030390(((uint32)(r_u32(0x800FF5A0u))));
    sub_80030390(((uint32)(r_u32(0x800FF4E8u))));
    sub_80030390(((uint32)(r_u32(0x800FF5DCu))));
    return sub_80030764();
  }
  return result;
}


/* TODO Missing call adapter sub_8008702C */
uint32 sub_80066B8C(uint32 a1, uint32 a2, uint32 a3)
{
  sint32 v4;
  sint32 v5;
  sint32 v6;
  short v7;
  uint32 v8;
  uint32 v9;
  sint32 v10;
  short v11;
  short v12;
  uint32 result;
  v4 = ((r_u32(a3) - r_u32(a2)) >> 12);
  v5 = ((r_u32((a3+(2)*4u)) - r_u32((a2+(2)*4u))) >> 12);
  v6 = ((r_u32((a3+(1)*4u)) - r_u32((a2+(1)*4u))) >> 12);
  if (v5)
  {
    if ((v5 <= 0))
      w_u16((a1+(1)*2u),((void)(((v4 << 12) / v5)),abort(),0u));
    else
      w_u16((a1+(1)*2u),(2048 - ((void)(((v4 << 12) / -v5)),abort(),0u)));
  }
  else
  {
    v7 = -1024;
    if ((v4 <= 0))
      v7 = 1024;
    w_u16((a1+(1)*2u),v7);
  }
  v8 = sub_80085B54(((v4 * v4) + (v5 * v5)));
  v9 = v8;
  if (!v8)
  {
    v10 = ((v10&0xFFFF0000u)|(((1024)&0xFFFFu)<<0));
    if ((v6 <= 0))
      v10 = ((v10&0xFFFF0000u)|(((-1024)&0xFFFFu)<<0));
    goto LABEL_14;
  }
  if ((v6 <= 0))
  {
    v10 = (0u - ((void)(((-4096 * v6) / v8)),abort(),0u));
    LABEL_14:
    w_u16(a1,v10);

    goto LABEL_15;
  }
  w_u16(a1,((void)(((v6 << 12) / v8)),abort(),0u));
  LABEL_15:
  v11 = r_u16(a1);

  v12 = r_u16((a1+(1)*2u));
  result = v9;
  w_u16((a1+(2)*2u),0);
  w_u16(a1,(v11 & 0xFFF));
  w_u16((a1+(1)*2u),(v12 & 0xFFF));
  return result;
}


/* TODO Missing call adapter sub_8008718C */
uint32 sub_80076420(uint32 a1, uint32 a2)
{
  sint32 v4;
  sint32 v5;
  sint32 v6;
  sint32 v7;
  sint32 v8;
  sint32 v9;
  sint32 result;
  sint32 v11;
  uint32 v12;
  sint32 v13;
  sint32 v14;
  sint32 v15;
  v4 = r_u16(a1);
  v5 = r_u16((a1+(4)*2u));
  v6 = r_u16((a1+(8)*2u));
  v7 = ((v4 + v5) + v6);
  v8 = (v4 < v5);
  if ((v7 > 0))
  {
    v9 = ((void)(((v7 + 4096) << 12)),abort(),0u);
    w_u32((a2+(3)*4u),(v9 >> 1));
    w_u32(a2,(((r_u16((a1+(5)*2u)) - r_u16((a1+(7)*2u))) * (0x800000 / v9)) >> 12));
    w_u32((a2+(1)*4u),(((r_u16((a1+(6)*2u)) - r_u16((a1+(2)*2u))) * (0x800000 / v9)) >> 12));
    result = (((r_u16((a1+(1)*2u)) - r_u16((a1+(3)*2u))) * (0x800000 / v9)) >> 12);
    LABEL_30:
    w_u32((a2+(2)*4u),result);

    return result;
  }
  if ((r_u16((a1+((4 * v8))*2u)) < v6))
    v8 = 2;
  v11 = r_u32((0x800ED4C8u+(v8)*4u));
  v12 = (0x800ED4C8u+(v11)*4u);
  v13 = r_u32(v12);
  v14 = ((void)((((r_u16((a1+((4 * v8))*2u)) - (r_u16((a1+((4 * v11))*2u)) + r_u16((a1+((4 * r_u32(v12)))*2u)))) + 4096) << 12)),abort(),0u);
  if ((v8 == 1))
  {
    w_u32((a2+(1)*4u),(v14 >> 1));
  }
  else
    if ((v8 >= 2))
  {
    if ((v8 == 2))
      w_u32((a2+(2)*4u),(v14 >> 1));
  }
  else
  {
    w_u32(a2,(v14 >> 1));
  }
  v15 = (0x800000 / v14);
  w_u32((a2+(3)*4u),(((r_u16((a1+(((3 * v11) + v13))*2u)) - r_u16((a1+(((3 * v13) + v11))*2u))) * v15) >> 12));
  if ((v11 == 1))
  {
    w_u32((a2+(1)*4u),(((r_u16((a1+(((3 * v8) + 1))*2u)) + r_u16((a1+((v8 + 3))*2u))) * v15) >> 12));
  }
  else
    if ((v11 >= 2))
  {
    if ((v11 == 2))
      w_u32((a2+(2)*4u),(((r_u16((a1+(((3 * v8) + 2))*2u)) + r_u16((a1+((v8 + 6))*2u))) * v15) >> 12));
  }
  else
    if (!v11)
  {
    w_u32(a2,(((r_u16((a1+((3 * v8))*2u)) + r_u16((a1+(v8)*2u))) * v15) >> 12));
  }
  result = (v13 < 2);
  if ((v13 == 1))
  {
    result = (((r_u16((a1+(((3 * v8) + 1))*2u)) + r_u16((a1+((v8 + 3))*2u))) * v15) >> 12);
    w_u32((a2+(1)*4u),result);
    return result;
  }
  if ((v13 >= 2))
  {
    result = 2;
    if ((v13 == 2))
    {
      result = (((r_u16((a1+(((3 * v8) + 2))*2u)) + r_u16((a1+((v8 + 6))*2u))) * v15) >> 12);
      goto LABEL_30;
    }
  }
  else
    if (!v13)
  {
    result = (((r_u16((a1+((3 * v8))*2u)) + r_u16((a1+(v8)*2u))) * v15) >> 12);
    w_u32(a2,result);
  }
  return result;
}


