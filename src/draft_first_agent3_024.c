#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
#include <stdlib.h>
/* TODO Missing call adapter sub_8003BB60 */
/* TODO Missing call adapter sub_8003C79C */
uint32 sub_8003ACB8(uint32 a1)
{
  union { sint32 words[36]; sint16 halves[72]; } collision;
  sint32 vector0[3], vector1[3], vector2[3], vector3[3];
  sint8 v2;
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
  short v36;
  sint32 v37;
  sint32 v38;
  sint32 v39;
  short v40;
  sint32 v41;
  sint32 v42;
  sint32 v43;
  sint32 v44;
  sint32 v45;
  sint32 v46;
  sint32 v47;
  sint32 v48;
  sint32 v49;
  sint32 v50;
  sint32 v51;
  sint32 v52;
  sint32 v53;
  sint32 v54;
  sint32 v55;
  int v57[4];
  int v58[4];
  sint32 v85[3];
  char v86[16];
  char v87[16];
  sint32 v88;
  sint32 v89;
  sint32 v90;
  sint32 v91;
  sint32 v92;
  sint32 v93;
  sint32 v94;
  sint32 *v95;
  sint32 *v96;
  sint32 *v97;
  v2 = 15;
  v3 = r_u32(0x800FF378u);
  v4 = (r_u32(0x800FF378u) == 12);
  w_u16(((uint32)((a1 + 216))),0);
  if (v4)
    goto LABEL_5;
  if ((v3 == 3))
  {
    v2 = 11;
    LABEL_6:
    v5 = (a1 + 104);

    goto LABEL_7;
  }
  v5 = (a1 + 104);
  if ((v3 == 6))
  {
    LABEL_5:
    v2 = 1;

    goto LABEL_6;
  }
  LABEL_7:
  v6 = r_u32(((uint32)((a1 + 108))));

  v7 = r_u32(((uint32)((a1 + 112))));
  v57[0] = r_u32(((uint32)((a1 + 104))));
  v57[1] = v6;
  v57[2] = v7;
  sub_8006C0B8(v5,(a1 + 116));
  sub_8006C270(v5,(a1 + 129));
  sub_8006C05C(v5);
  v8 = -1;
  if ((((sint32)(r_u32(((uint32)((a1 + 108)))))) >= 0))
  {
    v8 = r_u32(((uint32)((a1 + 108))));
    w_u32(((uint32)((a1 + 108))),0);
  }
  v9 = r_u32(((uint32)((a1 + 108))));
  v10 = r_u32(((uint32)((a1 + 112))));
  v58[0] = r_u32(((uint32)((a1 + 104))));
  v58[1] = v9;
  v58[2] = v10;
  v11 = ((((sint32)(r_u32(((uint32)((a1 + 104)))))) >> 9) * (((sint32)(r_u32(((uint32)((a1 + 104)))))) >> 9));
  v12 = ((((sint32)(r_u32(((uint32)((a1 + 108)))))) >> 9) * (((sint32)(r_u32(((uint32)((a1 + 108)))))) >> 9));
  v13 = (((sint32)(r_u32(((uint32)((a1 + 112)))))) >> 9);
  v14 = 0;
  v95 = v85;
  v96 = &vector1[0];
  v97 = &vector0[0];
  v93 = sub_80085B54(((v11 + v12) + (v13 * v13)));
  do
  {
    v15 = ((((sint32)(r_u32(((uint32)((a1 + 104)))))) >> 9) * (((sint32)(r_u32(((uint32)((a1 + 104)))))) >> 9));
    v16 = ((((sint32)(r_u32(((uint32)((a1 + 112)))))) >> 9) * (((sint32)(r_u32(((uint32)((a1 + 112)))))) >> 9));
    v90 = sub_80085B54(((v15 + ((((sint32)(r_u32(((uint32)((a1 + 108)))))) >> 9) * (((sint32)(r_u32(((uint32)((a1 + 108)))))) >> 9))) + v16));
    if (!v90)
      break;
    v17 = sub_80085B54((v15 + v16));
    if (!v17)
      break;
    v18 = r_u16(((uint32)((a1 + 584))));
    v19 = (4 * r_u16(0x800EC4C0u));
    if ((v19 < v17))
      v18 += ((v18 * (v17 - v19)) / v19);
    v20 = r_u16(((uint32)((a1 + 584))));
    v21 = (((v20 * 4) * r_u32(((uint32)((a1 + 112))))) / v90);
    v22 = (((-v20 * 4) * r_u32(((uint32)((a1 + 104))))) / v90);
    vector0[1] = 0;
    vector0[0] = v21;
    vector0[2] = v22;
    v23 = r_u32(((uint32)((a1 + 12))));
    v24 = (r_u32(((uint32)((a1 + 8)))) + 0x10000);
    vector1[0] = r_u32(((uint32)((a1 + 4))));
    vector1[1] = v24;
    vector1[2] = v23;
    vector2[0] = (((v18 * 8) * r_u32(((uint32)((a1 + 104))))) / v17);
    vector2[1] = (((r_u16(((uint32)((a1 + 584)))) * 8) * r_u32(((uint32)((a1 + 108))))) / v90);
    v25 = (8 * r_u32(((uint32)((a1 + 112)))));
    collision.words[0] = (vector1[0] + v21);
    collision.words[1] = v24;
    collision.words[2] = (v23 + v22);
    collision.words[3] = ((vector1[0] + v21) + vector2[0]);
    collision.words[4] = (v24 + vector2[1]);
    vector2[2] = ((v18 * v25) / v17);
    collision.words[5] = ((v23 + v22) + vector2[2]);
    xport_draft_host_sub_8007BB24_p1(&collision);
    xport_draft_host_sub_8007DD04_p1(&collision,1);
    ++v14;
    if (collision.words[26])
      goto LABEL_21;
    collision.words[0] = (vector1[0] - vector0[0]);
    collision.words[3] = ((vector1[0] - vector0[0]) + vector2[0]);
    collision.words[1] = vector1[1];
    collision.words[2] = (vector1[2] - vector0[2]);
    collision.words[5] = ((vector1[2] - vector0[2]) + vector2[2]);
    collision.words[4] = (vector1[1] + vector2[1]);
    xport_draft_host_sub_8007BB24_p1(&collision);
    xport_draft_host_sub_8007DD04_p1(&collision,1);
    if (collision.words[26])
      goto LABEL_21;
    v26 = (v2 & 4);
    if (((v2 & 2) == 0))
      goto LABEL_23;
    vector1[0] = r_u32(((uint32)((a1 + 4))));
    vector1[2] = r_u32(((uint32)((a1 + 12))));
    v27 = ((r_u8(((uint32)((a1 + 26)))) == 12)) ? ((r_u32(((uint32)((a1 + 8)))) - 0x10000)) : ((r_u32(((uint32)((a1 + 8)))) - 0x40000));
    vector1[1] = v27;
    v88 = 3;
    xport_draft_host_sub_8006C5C4_p13(v87,a1+104,&v88);
    v89 = (r_u16(((uint32)((a1 + 584)))) - 16);
    xport_draft_host_sub_8006C47C_p123(v86,&v89,v87);
    xport_draft_host_sub_8006C4EC_p123(v95,v86,&v90);
    xport_draft_host_sub_8006C34C_p123(vector3,vector1,v95);
    vector2[0] = vector3[0];
    vector2[1] = vector3[1];
    vector2[2] = vector3[2];
    collision.words[0] = vector1[0];
    collision.words[1] = vector1[1];
    collision.words[2] = vector1[2];
    collision.words[3] = vector3[0];
    collision.words[4] = vector3[1];
    collision.words[5] = vector3[2];
    xport_draft_host_sub_8007BB24_p1(&collision);
    xport_draft_host_sub_8007DD04_p1(&collision,1);
    v26 = (v2 & 4);
    if (collision.words[26])
    {
      LABEL_21:
      v28 = r_u32(((uint32)((a1 + 104))));

      w_u16(((uint32)((a1 + 216))),(r_u16(((uint32)((a1 + 216))))|(1u)));
      v29 = (((v28 >> 6) * collision.halves[60]) + ((((sint32)(r_u32(((uint32)((a1 + 112)))))) >> 6) * collision.halves[62]));
      v4 = (v29 > 0);
      v30 = (v29 >> 12);
      if (!v4)
      {
        w_u32(((uint32)((a1 + 104))),(v28 - ((v30 * collision.halves[60]) >> 6)));
        w_u32(((uint32)((a1 + 112))),(r_u32(((uint32)((a1 + 112))))-(((v30 * collision.halves[62]) >> 6))));
        v31 = (((sint32)(r_u32(((uint32)((a1 + 112)))))) >> 2);
        w_u32(((uint32)((a1 + 104))),(r_u32(((uint32)((a1 + 104))))>>(2)));
        w_u32(((uint32)((a1 + 112))),v31);
      }
    }
    else
    {
      LABEL_23:
      if (v26)
      {
        vector1[0] = r_u32(((uint32)((a1 + 4))));
        vector1[1] = (r_u32(((uint32)((a1 + 8)))) + 0x10000);
        vector1[2] = r_u32(((uint32)((a1 + 12))));
        xport_draft_host_sub_8006C34C_p123(vector3,v96,v97);
        vector2[0] = vector3[0];
        vector2[1] = vector3[1];
        vector2[2] = vector3[2];
        collision.words[0] = vector1[0];
        collision.words[1] = vector1[1];
        collision.words[2] = vector1[2];
        collision.words[3] = vector3[0];
        collision.words[4] = vector3[1];
        collision.words[5] = vector3[2];
        xport_draft_host_sub_8007BB24_p1(&collision);
        xport_draft_host_sub_8007DD04_p1(&collision,1);
        if (!collision.words[26])
        {
          vector1[0] = r_u32(((uint32)((a1 + 4))));
          vector1[1] = (r_u32(((uint32)((a1 + 8)))) + 0x10000);
          vector1[2] = r_u32(((uint32)((a1 + 12))));
          xport_draft_host_sub_8006C3AC_p123(vector3,v96,v97);
          vector2[0] = vector3[0];
          vector2[1] = vector3[1];
          vector2[2] = vector3[2];
          collision.words[0] = vector1[0];
          collision.words[1] = vector1[1];
          collision.words[2] = vector1[2];
          collision.words[3] = vector3[0];
          collision.words[4] = vector3[1];
          collision.words[5] = vector3[2];
          xport_draft_host_sub_8007BB24_p1(&collision);
          xport_draft_host_sub_8007DD04_p1(&collision,1);
          v32 = (v14 < 3);
          if (!collision.words[26])
            goto LABEL_30;
        }
        w_u32(((uint32)((a1 + 104))),(r_u32(((uint32)((a1 + 104))))+((16 * collision.halves[60]))));
        w_u32(((uint32)((a1 + 112))),(r_u32(((uint32)((a1 + 112))))+((16 * collision.halves[62]))));
        collision.words[26] = 0;
      }

    }
  }
  while ((collision.words[26] && (v14 < 3)));
  v32 = (v14 < 3);
  LABEL_30:
  if (v32)
    sub_8006C0B8((a1 + 4),(a1 + 104));
  else
    w_u32(((uint32)((a1 + 8))),(r_u32(((uint32)((a1 + 8))))+(r_u32(((uint32)((a1 + 108)))))));

  v33 = 0;
  if (((((sint32)(r_u32(((uint32)((a1 + 108)))))) < 0) || ((v34 = r_u32(((uint32)((a1 + 428)))) != 0) && (((sint32)(r_u32(((uint32)((v34 + 108)))))) < 0))))
    v33 = 1;
  if (v33)
  {
    collision.words[0] = r_u32(((uint32)((a1 + 4))));
    collision.words[1] = r_u32(((uint32)((a1 + 8))));
    v35 = r_u32(((uint32)((a1 + 12))));
    collision.words[3] = collision.words[0];
    collision.words[4] = (collision.words[1] - 753664);
    collision.words[2] = v35;
    collision.words[5] = v35;
    xport_draft_host_sub_8007BB24_p1(&collision);
    xport_draft_host_sub_8007DD04_p1(&collision,1);
    if (collision.words[26])
    {
      if (((r_u32(((uint32)((a1 + 8)))) - 491520) < collision.words[28]))
      {
        v36 = r_u16(((uint32)((a1 + 216))));
        w_u32(((uint32)((a1 + 8))),(collision.words[28] + 491520));
        w_u32(((uint32)((a1 + 108))),0);
        w_u16(((uint32)((a1 + 216))),(v36 | 0x100));
      }
    }
  }
  if ((v8 >= 0))
  {
    collision.words[0] = r_u32(((uint32)((a1 + 4))));
    collision.words[1] = (r_u32(((uint32)((a1 + 8)))) - ((r_u16(0x800EC4BAu) - 64) << 12));
    v37 = r_u32(((uint32)((a1 + 12))));
    collision.words[3] = collision.words[0];
    collision.words[4] = ((collision.words[1] + v8) + (r_u16(0x800EC4BCu) << 12));
    collision.words[2] = v37;
    collision.words[5] = v37;
    xport_draft_host_sub_8007BB24_p1(&collision);
    xport_draft_host_sub_8007DD04_p1(&collision,1);
    if (collision.words[26])
    {
      sub_8005D20C(a1,collision.words[32],collision.words[26]);
      if ((collision.halves[61] >= -3072))
      {
        v41 = (v8 - (v8 >> r_u8(((uint32)((a1 + 129))))));
        w_u32(((uint32)((a1 + 8))),(collision.words[28] - (r_u8(((uint32)((a1 + 582)))) << 12)));
        v42 = ((v41 >> 12) * collision.halves[60]);
        v43 = (v8 - (v8 >> r_u8(((uint32)((a1 + 131))))));
        w_u32(((uint32)((a1 + 4))),(r_u32(((uint32)((a1 + 4))))+(v42)));
        v44 = ((v43 >> 12) * collision.halves[62]);
        v45 = r_u32(((uint32)((a1 + 12))));
        w_u32(((uint32)((a1 + 108))),v8);
        w_u32(((uint32)((a1 + 8))),(r_u32(((uint32)((a1 + 8))))+((v8 - (v8 >> r_u8(((uint32)((a1 + 130)))))))));
        w_u32(((uint32)((a1 + 12))),(v45 + v44));
      }
      else
      {
        v38 = r_u8(((uint32)((a1 + 582))));
        v39 = collision.words[28];
        v40 = r_u16(((uint32)((a1 + 216))));
        w_u32(((uint32)((a1 + 108))),0);
        w_u32(((uint32)((a1 + 8))),(v39 - (v38 << 12)));
        w_u16(((uint32)((a1 + 216))),(v40 | 2));
      }
      w_u16(((uint32)((a1 + 164))),collision.halves[60]);
      w_u16(((uint32)((a1 + 166))),collision.halves[61]);
      w_u16(((uint32)((a1 + 168))),collision.halves[62]);
      w_u32(((uint32)((a1 + 184))),collision.words[27]);
      w_u32(((uint32)((a1 + 188))),collision.words[28]);
      w_u32(((uint32)((a1 + 192))),collision.words[29]);
      if (((r_u16(collision.words[26]) & 0x100) != 0))
      {
        ((void)(collision.words[26]),(void)(a1),(void)((a1 + 4)),(void)((a1 + 164)),abort(),0u);
        ((void)(collision.words[26]),abort(),0u);
        w_u32(((uint32)((a1 + 428))),collision.words[26]);
      }
      else
      {
        w_u32(((uint32)((a1 + 428))),0);
      }
    }
    else
    {
      if (!v93)
        goto LABEL_61;
      v91 = 3;
      xport_draft_host_sub_8006C5C4_p123(vector2,v58,&v91);
      v92 = (r_u16(((uint32)((a1 + 584)))) + 8);
      xport_draft_host_sub_8006C47C_p123(vector1,&v92,vector2);
      xport_draft_host_sub_8006C4EC_p123(vector0,vector1,&v93);
      collision.words[0] = (r_u32(((uint32)((a1 + 4)))) + vector0[0]);
      collision.words[1] = (r_u32(((uint32)((a1 + 8)))) - (r_u16(0x800EC4BAu) << 12));
      v46 = r_u32(((uint32)((a1 + 12))));
      collision.words[3] = collision.words[0];
      collision.words[2] = (v46 + vector0[2]);
      v47 = r_u32(((uint32)((a1 + 8))));
      collision.words[5] = (v46 + vector0[2]);
      collision.words[4] = (v47 + v8);
      xport_draft_host_sub_8007BB24_p1(&collision);
      xport_draft_host_sub_8007DD04_p1(&collision,1);
      if (!collision.words[26])
      {
        v94 = 1;
        xport_draft_host_sub_8006C1E8_p12(vector0,&v94);
        collision.words[0] = (r_u32(((uint32)((a1 + 4)))) + vector0[0]);
        collision.words[1] = (r_u32(((uint32)((a1 + 8)))) - (r_u16(0x800EC4BAu) << 12));
        v48 = r_u32(((uint32)((a1 + 12))));
        collision.words[3] = collision.words[0];
        collision.words[2] = (v48 + vector0[2]);
        v49 = r_u32(((uint32)((a1 + 8))));
        collision.words[5] = (v48 + vector0[2]);
        collision.words[4] = (v49 + v8);
        xport_draft_host_sub_8007BB24_p1(&collision);
        xport_draft_host_sub_8007DD04_p1(&collision,1);
        if (!collision.words[26])
          goto LABEL_61;
      }
      if (((r_u16(((uint32)((collision.words[32] + 18)))) & 4) != 0))
      {
        if (((r_u16(collision.words[26]) & 0x100) != 0))
          w_u32(((uint32)((a1 + 428))),collision.words[26]);
        else
          w_u32(((uint32)((a1 + 428))),0);
        v50 = 1;
        w_u16(((uint32)((a1 + 216))),(r_u16(((uint32)((a1 + 216))))|(0x40u)));
        w_u32(((uint32)((a1 + 544))),collision.words[28]);
        while (1)
        {
          collision.words[0] = r_u32(((uint32)((a1 + 4))));
          collision.words[1] = (r_u32(((uint32)((a1 + 544)))) + (v50 << 16));
          collision.words[2] = r_u32(((uint32)((a1 + 12))));
          v51 = r_u32(((uint32)((a1 + 4))));
          collision.words[4] = collision.words[1];
          collision.words[3] = (v51 + vector0[0]);
          collision.words[5] = (r_u32(((uint32)((a1 + 12)))) + vector0[2]);
          xport_draft_host_sub_8007BB24_p1(&collision);
          xport_draft_host_sub_8007DD04_p1(&collision,1);
          ++v50;
          if (collision.words[26])
            break;
          if ((v50 >= 7))
          {
            v52 = (a1 + 104);
            goto LABEL_63;
          }
        }

        w_u16(((uint32)((a1 + 216))),(r_u16(((uint32)((a1 + 216))))|(0x80u)));
        w_u32(((uint32)((a1 + 540))),collision.words[27]);
        w_u32(((uint32)((a1 + 548))),collision.words[29]);
        w_u32(((uint32)((a1 + 552))),collision.halves[60]);
        w_u32(((uint32)((a1 + 556))),collision.halves[61]);
        w_u32(((uint32)((a1 + 560))),collision.halves[62]);
      }
      else
      {
        LABEL_61:
        v53 = r_u32(((uint32)((a1 + 8))));

        w_u32(((uint32)((a1 + 108))),v8);
        w_u32(((uint32)((a1 + 8))),(v53 + v8));
      }
    }
  }
  v52 = (a1 + 104);
  LABEL_63:
  xport_draft_host_sub_8006C3AC_p13(vector0,v52,v57);

  v54 = vector0[1];
  v55 = vector0[2];
  w_u32(((uint32)((a1 + 152))),vector0[0]);
  w_u32(((uint32)((a1 + 156))),v54);
  w_u32(((uint32)((a1 + 160))),v55);
  sub_8006C730((a1 + 16),(a1 + 132));
  sub_8006C624((a1 + 16));
  sub_8006C730((a1 + 132),(a1 + 138));
  sub_8006C8E8((a1 + 132),(a1 + 144));
  return sub_8006C64C((a1 + 132));
}
uint32 sub_8006C3AC(uint32 a1, uint32 a2, uint32 a3)
{
  uint32 result;
  sint32 v4;
  sint32 v5;
  result = a1;
  v4 = (r_u32((a2+(1)*4u)) - r_u32((a3+(1)*4u)));
  v5 = (r_u32((a2+(2)*4u)) - r_u32((a3+(2)*4u)));
  w_u32(a1,(r_u32(a2) - r_u32(a3)));
  w_u32((a1+(1)*4u),v4);
  w_u32((a1+(2)*4u),v5);
  return result;
}
uint32 sub_8006C64C(uint32 a1)
{
  sint32 result;
  if ((((unsigned short)((r_u16(a1) + 1))) < 3u))
    w_u16(a1,0);
  if ((((unsigned short)((r_u16((a1+(1)*2u)) + 1))) < 3u))
    w_u16((a1+(1)*2u),0);
  result = (((unsigned short)((r_u16((a1+(2)*2u)) + 1))) < 3u);
  if ((((unsigned short)((r_u16((a1+(2)*2u)) + 1))) < 3u))
    w_u16((a1+(2)*2u),0);
  return result;
}
uint32 sub_8005EA80(uint32 a1)
{
  sint32 result;
  sint32 v2;
  sint32 v3;
  result = 0;
  if ((((sint8)(r_u8(((uint32)((a1 + 468)))))) > 0))
  {
    v2 = ((sint16)(r_u16(((uint32)((a1 + 474))))));
    result = 0;
    if ((v2 >= 1281))
    {
      if ((v2 >= 2816))
      {
        return 0;
      }
      else
      {
        v3 = r_u8(((uint32)((a1 + 26))));
        w_u32(((uint32)((a1 + 460))),32);
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
void sub_80085B04(uint32 a1, uint32 a2)
{
  short v2;
  short v3;
  short v4;
  short v5;
  short v6;
  short v7;
  short v8;
  short v9;
  v2 = r_u16((a1+(1)*2u));
  v3 = r_u16((a1+(2)*2u));
  v4 = r_u16((a1+(3)*2u));
  v5 = r_u16((a1+(4)*2u));
  v6 = r_u16((a1+(5)*2u));
  v7 = r_u16((a1+(6)*2u));
  v8 = r_u16((a1+(7)*2u));
  v9 = r_u16((a1+(8)*2u));
  w_u16(a2,r_u16(a1));
  w_u16((a2+(3)*2u),v2);
  w_u16((a2+(6)*2u),v3);
  w_u16((a2+(1)*2u),v4);
  w_u16((a2+(4)*2u),v5);
  w_u16((a2+(7)*2u),v6);
  w_u16((a2+(2)*2u),v7);
  w_u16((a2+(5)*2u),v8);
  w_u16((a2+(8)*2u),v9);
}
uint32 sub_8006C22C(uint32 a1, uint32 a2)
{
  uint32 result;
  result = a1;
  w_u32(a1,(r_u32(a1)<<(r_u32(a2))));
  w_u32((a1+(1)*4u),(r_u32((a1+(1)*4u))<<(r_u32(a2))));
  w_u32((a1+(2)*4u),(r_u32((a1+(2)*4u))<<(r_u32(a2))));
  return result;
}
uint32 sub_8006C304(uint32 a1, uint32 a2)
{
  sint32 v2;
  v2 = 0;
  if ((((r_u32(a1) != r_u32(a2)) || (r_u32((a1+(1)*4u)) != r_u32((a2+(1)*4u)))) || (r_u32((a1+(2)*4u)) != r_u32((a2+(2)*4u)))))
    return 1;
  return v2;
}
uint32 sub_80067508(uint32 a1, uint32 a2, uint32 a3)
{
  sint32 result;
  char v6[16];
  result = xport_draft_host_sub_8007C398_p3(a1,a2,v6,a3,0u);
  if (!result)
  {
    w_u32(0x800ED638u,((sint32)(r_u32(a1))));
    w_u32(0x800ED63Cu,((sint32)(r_u32((a1+(1)*4u)))));
    w_u32(0x800ED640u,((sint32)(r_u32((a1+(2)*4u)))));
    w_u32(0x800ED644u,((sint32)(r_u32(a2))));
    w_u32(0x800ED648u,((sint32)(r_u32((a2+(1)*4u)))));
    w_u32(0x800ED64Cu,((sint32)(r_u32((a2+(2)*4u)))));
    sub_8007BB24(0x800ED638u);
    w_u32(0x800FF974u,1);
    sub_8007BEA0(r_u32(0x800FF5DCu),0x800ED638u);
    result = r_u32(0x800ED6A0u);
    w_u32(0x800FF974u,0);
  }
  return result;
}
uint32 sub_8006C40C(uint32 a1, uint32 a2, uint32 a3)
{
  uint32 result;
  sint32 v4;
  sint32 v5;
  result = a1;
  v4 = (r_u32((a2+(1)*4u)) * r_u32(a3));
  v5 = (r_u32((a2+(2)*4u)) * r_u32(a3));
  w_u32(a1,(r_u32(a2) * r_u32(a3)));
  w_u32((a1+(1)*4u),v4);
  w_u32((a1+(2)*4u),v5);
  return result;
}
/* TODO Missing call adapter sub_80085A08 */
uint32 sub_8007FDF0(uint32 a1)
{
  sint32 v2;
  sint32 v3;
  sint32 result;
  sub_800858FC((a1 + 16),(a1 + 324));
  if (((r_u16(((uint32)(a1))) & 0x200) != 0))
    ((void)(a1),(void)((a1 + 324)),abort(),0u);
  v2 = r_u32(((uint32)((a1 + 12))));
  w_u32(((uint32)((a1 + 344))),(((sint32)(r_u32(((uint32)((a1 + 4)))))) >> 12));
  v3 = r_u32(((uint32)((a1 + 8))));
  w_u32(((uint32)((a1 + 352))),(v2 >> 12));
  result = (v3 >> 12);
  w_u32(((uint32)((a1 + 348))),result);
  return result;
}
uint32 sub_8007D148(uint32 a1)
{
  sint32 v1;
  sint32 result;
  uint32 v3;
  uint32 v4;
  uint32 v5;
  sint32 v6;
  uint32 v7;
  sint32 v8;
  sint32 v9;
  uint32 v10;
  uint32 v11;
  uint32 v12;
  uint32 v13;
  sint32 v14;
  unsigned short i;
  sint32 v16;
  sint32 v17;
  sint32 v18;
  sint32 v19;
  uint32 v20;
  uint32 v21;
  sint32 v22;
  sint32 v23;
  uint32 v24;
  sint32 v25;
  sint32 v26;
  sint32 v27;
  sint32 v28;
  uint32 v29;
  uint32 v30;
  sint32 v31;
  sint32 v32;
  uint32 v33;
  sint32 v34;
  uint32 v35;
  uint32 v36;
  sint32 v37;
  sint32 v38;
  sint32 v39;
  sint32 v40;
  sint8 v41;
  char v42[144];
  char v43[18];
  short v44;
  short v45;
  short v46;
  uint32 v47;
  sint32 v48;
  sint32 v49;
  sint32 v50;
  sint32 v51;
  v50 = a1;
  v1 = r_u32(((uint32)((a1 + 356))));
  result = (r_u16(((uint32)(a1))) & 4);
  v47 = ((uint32)(v1));
  v3 = r_u32(((uint32)((a1 + 364))));
  v4 = r_u32(((uint32)((a1 + 360))));
  if (result)
  {
    if (v1)
    {
      result = -2146500608;
      if (v4)
      {
        v5 = (0x800EAEF8u+((16 * r_u8(((uint32)((a1 + 27))))))*4u);
        v49 = r_u32(((uint32)((((sint32)(r_u32((v5+(4)*4u)))) - 4))));
        v6 = 0;
        v51 = ((sint32)(r_u32((v5+(6)*4u))));
        if ((v49 > 0))
        {
          v7 = ((uint32)(v1));
          do
          {
            w_u16(v7,0x8000);
            ++v6;
            v7 += (12)*2u;
          }
          while ((v6 < v49));
        }
        v8 = ((8 * r_u8(((uint32)((v50 + 26))))) + v51);
        v9 = r_u16(((uint32)((v8 + 10))));
        v48 = r_u16((v3-(1)*2u));
        if (v9)
          v10 = r_u32(0x800ED760u);
        else
          v10 = ((uint32)(((v51 + r_u32(((uint32)((v8 + 4))))) + ((24 * r_u8(((uint32)((v50 + 24))))) * v49))));
        v11 = &v41;
        v12 = v3;
        v13 = v4;
        v14 = 0;
        if (v48)
        {
          do
          {
            for (i = r_u16(v12); (v11 >= v42); v11 -= (28)*1u)
            {
              if ((((uint32)(r_u16((v12+(5)*2u)))) >= r_u32((((uint32)(v11))+(6)*4u))))
                break;
            }

            if (((r_u16(v13) || r_u16((v13+(1)*2u))) || r_u16((v13+(2)*2u))))
            {
              sub_800858FC(v13,v43);
              v16 = (6 * i);
              v46 = 0;
              v45 = 0;
              v44 = 0;
              sub_80085674(v43,(v10+(v16)*4u),v43);
              if ((v11 >= v42))
                sub_80085674(v43,v11,v43);
              v11 += (28)*1u;
              sub_800857A8(v11,v43,(v10+(v16)*4u));
              v17 = 0;
              v18 = 0;
              w_u32((((uint32)(v11))+(6)*4u),v14);
              v19 = 0;
              v20 = (v47+(v16)*4u);
              v21 = (v47+(v16)*4u);
              do
              {
                v22 = 0;
                v23 = v19;
                do
                {
                  ++v22;
                  w_u16(((uint32)((((uint32)(v21))+(v23)*1u))),r_u16(((uint32)(&v43[v23]))));
                  v23 += 2;
                }
                while ((v22 < 3));
                v24 = &v43[v18];
                v18 += 2;
                v19 += 6;
                ++v17;
                w_u16((((uint32)(v20))+(9)*2u),r_u16((((uint32)(v24))+(9)*2u)));
                v20 = ((uint32)((((uint32)(v20))+(2)*1u)));
              }
              while ((v17 < 3));
              ++v14;
            }
            else
            {
              v25 = 0;
              if ((v11 < v42))
              {
                v26 = (6 * r_u16(v12));
                v27 = 0;
                v28 = 0;
                v29 = (v47+(v26)*4u);
                v30 = (v47+(v26)*4u);
                do
                {
                  v31 = 0;
                  v32 = v28;
                  do
                  {
                    ++v31;
                    w_u16(((uint32)((((uint32)(v30))+(v32)*1u))),r_u16(((uint32)((((uint32)((v10+(v26)*4u)))+(v32)*1u)))));
                    v32 += 2;
                  }
                  while ((v31 < 3));
                  v33 = (((uint32)((v10+(v26)*4u)))+(v27)*1u);
                  v27 += 2;
                  v28 += 6;
                  ++v25;
                  w_u16((((uint32)(v29))+(9)*2u),r_u16((((uint32)(v33))+(9)*2u)));
                  v29 = ((uint32)((((uint32)(v29))+(2)*1u)));
                }
                while ((v25 < 3));
                ++v14;
              }
              else
              {
                sub_80085674((v47+((6 * r_u16(v12)))*4u),v11,(v10+((6 * r_u16(v12)))*4u));
                ++v14;
              }
            }
            v12 += (6)*2u;
            v13 += (6)*2u;
          }
          while ((v14 < v48));
        }
        result = r_u16(((uint32)((((8 * r_u8(((uint32)((v50 + 26))))) + v51) + 10))));
        if (!(r_u16(((uint32)((((8 * r_u8(((uint32)((v50 + 26))))) + v51) + 10))))))
        {
          v34 = 0;
          if ((v49 > 0))
          {
            v35 = v47;
            v36 = v10;
            do
            {
              if ((((sint16)(r_u16(((uint32)(v35))))) == -32768))
              {
                v37 = ((sint32)(r_u32((v36+(1)*4u))));
                v38 = ((sint32)(r_u32((v36+(2)*4u))));
                v39 = ((sint32)(r_u32((v36+(3)*4u))));
                w_u32(v35,((sint32)(r_u32(v36))));
                w_u32((v35+(1)*4u),v37);
                w_u32((v35+(2)*4u),v38);
                w_u32((v35+(3)*4u),v39);
                v40 = ((sint32)(r_u32((v36+(5)*4u))));
                w_u32((v35+(4)*4u),((sint32)(r_u32((v36+(4)*4u)))));
                w_u32((v35+(5)*4u),v40);
              }
              v35 += (6)*4u;
              result = (++v34 < v49);
              v36 += (6)*4u;
            }
            while ((v34 < v49));
          }
        }
      }
    }
  }
  return result;
}
uint32 sub_80023380(uint32 a1, uint32 a2)
{
  sint32 v3;
  sint32 result;
  sint32 v6;
  sint32 v7;
  sint32 v8;
  v3 = r_u32((a1+(8)*4u));
  if (v3)
  {
    sub_80032EE4(v3,a2);
    if ((((uint32)((r_u32(0x800FF2F0u) - r_u32((a1+(9)*4u))))) >= 0x1F))
      sub_80022ABC(((sint32)(a1)));
  }
  result = r_u32((a1+(5)*4u));
  if (result)
  {
    v6 = r_u32((a1+(13)*4u));
    w_u32((a1+(5)*4u),0);
    if (v6)
      sub_8006A294(v6);
    v7 = r_u32((a1+(7)*4u));
    result = (((uint32)((r_u32(0x800FF2F0u) - v7))) < 0x2E);
    if ((((uint32)((r_u32(0x800FF2F0u) - v7))) >= 0x2E))
    {
      v8 = sub_80032DC0(96);
      if (v8)
        v8 = sub_800325B0(v8,a2,-1,1,100,100,100,3,8,188,188);
      w_u32((a1+(8)*4u),v8);
      w_u8(((uint32)((v8 + 66))),1);
      result = r_u32(0x800FF2F0u);
      w_u32((a1+(9)*4u),r_u32(0x800FF2F0u));
    }
  }
  return result;
}
/* TODO Missing call adapter indirect */
uint32 sub_80062D84(uint32 a1)
{
  sint32 v2;
  sint32 result;
  v2 = r_u32(((uint32)((a1 + 196))));
  result = (r_u16(((uint32)((a1 + 78)))) & 0xFFF7);
  w_u16(((uint32)((a1 + 78))),result);
  if (v2)
  {
    result = ((void)((v2 + ((sint16)(r_u16(((uint32)((r_u32(((uint32)((v2 + 68)))) + 8)))))))),(void)(3),abort(),0u);
    w_u32(((uint32)((a1 + 196))),0);
  }
  return result;
}
/* TODO Missing call adapter indirect */
uint32 sub_80037258(uint32 a1)
{
  sint32 i;
  sint32 result;
  for (i = a1; i; i = r_u32(((uint32)((i + 4)))))
  {
    result = r_u8(((uint32)((i + 63))));
    if (!(r_u8(((uint32)((i + 63))))))
      result = ((void)((i + ((sint16)(r_u16(((uint32)((r_u32(((uint32)((i + 68)))) + 16)))))))),abort(),0u);
  }

  return result;
}
/* TODO Missing call adapter indirect */
uint32 sub_8003C7B4(uint32 a1, uint32 a2)
{
  uint32 v3;
  sint32 v4;
  short v5;
  sint32 v6;
  uint32 v7;
  unsigned short v8;
  sint32 v9;
  sint32 v10;
  uint32 v11;
  unsigned short v12;
  sint32 v13;
  sint32 v14;
  uint32 v15;
  sint32 v16;
  short v17;
  sint32 v18;
  short v19;
  short v20;
  short v21;
  uint32 v22;
  sint32 v23;
  short v24;
  uint32 v25;
  sint32 v26;
  short v27;
  uint32 v28;
  sint32 v29;
  short v30;
  uint32 v31;
  short v32;
  uint32 v33;
  unsigned short v34;
  unsigned short v35;
  unsigned short v36;
  sint32 v37;
  sint32 result;
  uint32 v39;
  sint32 v40;
  sint32 v41;
  sint32 v42;
  uint32 v43;
  uint32 v44;
  uint32 v45;
  unsigned short v46;
  uint32 v47;
  sint32 v48;
  sint32 v49;
  sint32 v50;
  sint32 v51;
  sint32 v52;
  sint32 v53;
  sint32 v54;
  sint32 v55;
  sint32 v56;
  if ((a2 == 17155))
  {
    v31 = ((sint32)(r_u32(((uint32)((a1 + 400))))));
    v32 = ((sint16)(r_u16(v31)));
    w_u32(((uint32)((a1 + 400))),(v31+(1)*2u));
    w_u16(((uint32)((a1 + 508))),v32);
    return 1;
  }
  if ((a2 >= 0x4304u))
  {
    if ((a2 == 17159))
    {
      if (((r_u16(((uint32)(a1))) & 0x200) == 0))
      {
        w_u16(((uint32)(a1)),(r_u16(((uint32)(a1)))|(0x200u)));
        w_u16(((uint32)((a1 + 40))),4096);
        w_u16(((uint32)((a1 + 38))),4096);
        w_u16(((uint32)((a1 + 36))),4096);
      }
      v25 = r_u32(((uint32)((a1 + 400))));
      v26 = r_u32((v25+=2u,v25-2u));
      w_u32(((uint32)((a1 + 400))),v25);
      v27 = r_u16(v25);
      w_u32(((uint32)((a1 + 400))),(v25+(1)*2u));
      w_u16(((uint32)((a1 + 498))),v27);
      if (v27)
        w_u16(((uint32)((a1 + 504))),(((v26 << 16) >> 4) / (100 * r_u16(((uint32)((a1 + 498)))))));
      else
        w_u16(((uint32)((a1 + 504))),(((v26 << 16) >> 4) / 100));
      w_u16(((uint32)((a1 + 38))),(r_u16(((uint32)((a1 + 38))))+(r_u16(((uint32)((a1 + 504)))))));
    }
    else
      if ((a2 >= 0x4308u))
    {
      if ((a2 == 17671))
      {
        v6 = r_u32(((uint32)((a1 + 520))));
        if (v6)
          sub_8006A294(v6);
        v7 = r_u32(((uint32)((a1 + 400))));
        v8 = r_u16(v7);
        w_u32(((uint32)((a1 + 400))),(v7+(1)*2u));
        w_u32(((uint32)((a1 + 520))),sub_80069DF0(v8,0x2000,0));
        w_u32(((uint32)((a1 + 524))),-1);
      }
      else
        if ((a2 >= 0x4508u))
      {
        if ((a2 == 17672))
        {
          v9 = r_u32(((uint32)((a1 + 520))));
          v10 = (a1 + 4);
          if (v9)
          {
            sub_8006A294(v9);
            v10 = (a1 + 4);
          }
          v11 = r_u32(((uint32)((a1 + 400))));
          v12 = r_u32((v11+=2u,v11-2u));
          w_u32(((uint32)((a1 + 400))),v11);
          v13 = r_u16(v11);
          w_u32(((uint32)((a1 + 400))),(v11+(1)*2u));
          w_u32(((uint32)((a1 + 524))),v13);
          w_u32(((uint32)((a1 + 520))),sub_80069EF4(v12,v10,0));
        }
        else
        {
          if ((a2 != 17673))
            return sub_8004BF3C(a1,a2);
          v14 = r_u32(((uint32)((a1 + 520))));
          if (v14)
            sub_8006A294(v14);
          w_u32(((uint32)((a1 + 520))),0);
        }
      }
      else
      {
        if ((a2 != 17160))
          return sub_8004BF3C(a1,a2);
        if (((r_u16(((uint32)(a1))) & 0x200) == 0))
        {
          w_u16(((uint32)(a1)),(r_u16(((uint32)(a1)))|(0x200u)));
          w_u16(((uint32)((a1 + 40))),4096);
          w_u16(((uint32)((a1 + 38))),4096);
          w_u16(((uint32)((a1 + 36))),4096);
        }
        v28 = r_u32(((uint32)((a1 + 400))));
        v29 = r_u32((v28+=2u,v28-2u));
        w_u32(((uint32)((a1 + 400))),v28);
        v30 = r_u16(v28);
        w_u32(((uint32)((a1 + 400))),(v28+(1)*2u));
        w_u16(((uint32)((a1 + 500))),v30);
        if (v30)
          w_u16(((uint32)((a1 + 506))),(((v29 << 16) >> 4) / (100 * r_u16(((uint32)((a1 + 500)))))));
        else
          w_u16(((uint32)((a1 + 506))),(((v29 << 16) >> 4) / 100));
        w_u16(((uint32)((a1 + 40))),(r_u16(((uint32)((a1 + 40))))+(r_u16(((uint32)((a1 + 506)))))));
      }
    }
    else
      if ((a2 == 17157))
    {
      v3 = r_u32(((uint32)((a1 + 400))));
      v4 = r_u16(v3);
      w_u32(((uint32)((a1 + 400))),(v3+(1)*2u));
      if (v4)
        v5 = (r_u16(((uint32)(a1))) | 8);
      else
        v5 = (r_u16(((uint32)(a1))) & 0xFFF7);
      w_u16(((uint32)(a1)),v5);
    }
    else
      if ((a2 >= 0x4306u))
    {
      if (((r_u16(((uint32)(a1))) & 0x200) == 0))
      {
        w_u16(((uint32)(a1)),(r_u16(((uint32)(a1)))|(0x200u)));
        w_u16(((uint32)((a1 + 40))),4096);
        w_u16(((uint32)((a1 + 38))),4096);
        w_u16(((uint32)((a1 + 36))),4096);
      }
      v22 = r_u32(((uint32)((a1 + 400))));
      v23 = r_u32((v22+=2u,v22-2u));
      w_u32(((uint32)((a1 + 400))),v22);
      v24 = r_u16(v22);
      w_u32(((uint32)((a1 + 400))),(v22+(1)*2u));
      w_u16(((uint32)((a1 + 496))),v24);
      if (v24)
        w_u16(((uint32)((a1 + 502))),(((v23 << 16) >> 4) / (100 * r_u16(((uint32)((a1 + 496)))))));
      else
        w_u16(((uint32)((a1 + 502))),(((v23 << 16) >> 4) / 100));
      w_u16(((uint32)((a1 + 36))),(r_u16(((uint32)((a1 + 36))))+(r_u16(((uint32)((a1 + 502)))))));
    }
    else
    {
      if (((r_u16(((uint32)(a1))) & 0x200) == 0))
      {
        w_u16(((uint32)(a1)),(r_u16(((uint32)(a1)))|(0x200u)));
        w_u16(((uint32)((a1 + 40))),4096);
        w_u16(((uint32)((a1 + 38))),4096);
        w_u16(((uint32)((a1 + 36))),4096);
      }
      v15 = r_u32(((uint32)((a1 + 400))));
      v16 = r_u32((v15+=2u,v15-2u));
      w_u32(((uint32)((a1 + 400))),v15);
      v17 = r_u16(v15);
      w_u32(((uint32)((a1 + 400))),(v15+(1)*2u));
      w_u16(((uint32)((a1 + 496))),v17);
      w_u16(((uint32)((a1 + 500))),v17);
      w_u16(((uint32)((a1 + 498))),v17);
      if (v17)
        v18 = (((v16 << 16) >> 4) / (100 * r_u16(((uint32)((a1 + 496))))));
      else
        v18 = (((v16 << 16) >> 4) / 100);
      w_u16(((uint32)((a1 + 502))),v18);
      w_u16(((uint32)((a1 + 506))),v18);
      w_u16(((uint32)((a1 + 504))),v18);
      v19 = r_u16(((uint32)((a1 + 504))));
      v20 = r_u16(((uint32)((a1 + 506))));
      w_u16(((uint32)((a1 + 36))),(r_u16(((uint32)((a1 + 36))))+(r_u16(((uint32)((a1 + 502)))))));
      v21 = (r_u16(((uint32)((a1 + 40)))) + v20);
      w_u16(((uint32)((a1 + 38))),(r_u16(((uint32)((a1 + 38))))+(v19)));
      w_u16(((uint32)((a1 + 40))),v21);
    }
    return 1;
  }
  if ((a2 == 16930))
  {
    v47 = ((uint32)(((r_u32(((uint32)((a1 + 400)))) + 3) & 0xFFFFFFFC)));
    v48 = r_u32((v47+=4u,v47-4u));
    v52 = v48;
    v49 = r_u32((v47+=4u,v47-4u));
    v53 = v49;
    v50 = ((sint32)(r_u32(v47)));
    v43 = (v47+(1)*4u);
    v56 = 12;
    v54 = v50;
    sub_8006C22C(&v52,&v56);
    sub_8006C0B8(&v52,(a1 + 4));
    v44 = ((uint32)(a1));
    goto LABEL_71;
  }
  if ((a2 >= 0x4223u))
  {
    if ((a2 == 17153))
    {
      if ((r_u32(((uint32)((r_u32(0x800FF5A0u) + 428)))) == a1))
        w_u32(((uint32)((r_u32(0x800FF5A0u) + 428))),0);
      v51 = r_u32(((uint32)((a1 + 68))));
      w_u16(((uint32)(a1)),(r_u16(((uint32)(a1)))|(1u)));
      ((void)((a1 + ((sint16)(r_u16(((uint32)((v51 + 16)))))))),abort(),0u);
      sub_80022318(a1,0,0);
      return 0;
    }
    if ((a2 < 0x4302u))
    {
      if ((a2 == 17152))
      {
        result = 1;
        if (!(r_u32(((uint32)((a1 + 512))))))
        {
          result = 0;
          w_u32(((uint32)((a1 + 400))),(r_u32(((uint32)((a1 + 400))))-(2)));
        }
        return result;
      }
      return sub_8004BF3C(a1,a2);
    }
    v33 = r_u32(((uint32)((a1 + 400))));
    v34 = r_u32((v33+=2u,v33-2u));
    w_u32(((uint32)((a1 + 400))),v33);
    v35 = r_u16(v33);
    w_u32(((uint32)((a1 + 400))),(v33+(1)*2u));
    v36 = r_u16((v33+(1)*2u));
    v37 = r_u32(((uint32)((a1 + 396))));
    w_u32(((uint32)((a1 + 400))),(v33+(2)*2u));
    w_u32(((uint32)((a1 + 396))),(v37 | 4));
    w_u32(((uint32)((a1 + 528))),(v34 << 11));
    w_u32(((uint32)((a1 + 532))),(v35 << 11));
    w_u32(((uint32)((a1 + 536))),(v36 << 11));
    return 1;
  }
  if ((a2 == 16928))
  {
    v39 = ((uint32)(((r_u32(((uint32)((a1 + 400)))) + 3) & 0xFFFFFFFC)));
    v40 = r_u32((v39+=4u,v39-4u));
    v52 = v40;
    v41 = r_u32((v39+=4u,v39-4u));
    v53 = v41;
    v42 = ((sint32)(r_u32(v39)));
    v43 = (v39+(1)*4u);
    v55 = 12;
    v54 = v42;
    sub_8006C22C(&v52,&v55);
    v44 = ((uint32)(a1));
    LABEL_71:
    w_u32((v44+(100)*4u),v43);

    sub_8003C684(v44,&v52);
    return 1;
  }
  if ((a2 < 0x4221u))
  {
    if ((a2 == 16901))
    {
      ((void)((a1 + ((sint16)(r_u16(((uint32)((r_u32(((uint32)((a1 + 68)))) + 16)))))))),abort(),0u);
      return 0;
    }
    return sub_8004BF3C(a1,a2);
  }
  v45 = r_u32(((uint32)((a1 + 400))));
  v46 = r_u16(v45);
  w_u32(((uint32)((a1 + 400))),(v45+(1)*2u));
  if (((v46 & 0x2000) != 0))
    v46 = ((void)((a1 + ((sint16)(r_u16(((uint32)((r_u32(((uint32)((a1 + 68)))) + 80)))))))),(void)(v46),abort(),0u);
  sub_8006613C(&v52,v46);
  sub_8003C684(((uint32)(a1)),&v52);
  return 1;
}
uint32 sub_8003CF3C(uint32 a1, uint32 a2)
{
  uint32 v3;
  sint8 v4;
  sint32 result;
  uint32 v6;
  short v7;
  uint32 v8;
  short v9;
  sint32 v10;
  sint32 v11;
  short v12;
  short v13;
  uint32 v14;
  short v15;
  short v16;
  switch (a2)
  {
    case 0x2123u:
      v3 = ((uint32)(r_u32((((uint32)(a1))+(100)*4u))));
      v4 = ((sint8)(r_u8(v3)));
      result = ((sint32)((v3+(2)*1u)));
      w_u32((((uint32)(a1))+(100)*4u),result);
      w_u8((((uint32)(a1))+(382)*1u),v4);
      return result;

    case 0x2124u:
      v6 = ((uint32)(r_u32((((uint32)(a1))+(100)*4u))));
      v7 = ((sint16)(r_u16(v6)));
      result = ((sint32)((v6+(1)*2u)));
      w_u32((((uint32)(a1))+(100)*4u),result);
      w_u16((a1+(11)*2u),v7);
      return result;

    case 0x2127u:
      v15 = sub_8004CF78(a1);
      v13 = sub_8004CF78(a1);
      result = sub_8004CF78(a1);
      v14 = (a1+(66)*2u);
      w_u16((a1+(66)*2u),v15);
      goto LABEL_9;

    case 0x2128u:
      v16 = sub_8004CF78(a1);
      v13 = sub_8004CF78(a1);
      result = sub_8004CF78(a1);
      v14 = (a1+(69)*2u);
      w_u16((a1+(69)*2u),v16);
      goto LABEL_9;

    case 0x212Fu:
      v8 = ((uint32)(((r_u32((((uint32)(a1))+(100)*4u)) + 3) & 0xFFFFFFFC)));
      w_u16((a1+(11)*2u),sub_8006E080(r_u32(v8),r_u8((((uint32)(a1))+(27)*1u))));
      v9 = ((sint16)(r_u16(a1)));
      w_u32((((uint32)(a1))+(100)*4u),(v8+(1)*4u));
      result = (v9 & 0xFFFE);
      w_u16(a1,result);
      return result;

    case 0x2134u:
      v10 = sub_8004CF78(a1);
      v11 = sub_8004CF78(a1);
      result = ((sub_8004CF78(a1) << 16) >> 4);
      w_u32((((uint32)(a1))+(26)*4u),((v10 << 16) >> 4));
      w_u32((((uint32)(a1))+(27)*4u),((v11 << 16) >> 4));
      w_u32((((uint32)(a1))+(28)*4u),result);
      return result;

    case 0x2137u:
      v12 = sub_8004CF78(a1);
      v13 = sub_8004CF78(a1);
      result = sub_8004CF78(a1);
      v14 = (a1+(8)*2u);
      w_u16((a1+(8)*2u),v12);
      LABEL_9:
    w_u16((v14+(1)*2u),v13);

      w_u16((v14+(2)*2u),result);
      break;

    default:
      result = sub_8004CFDC(a1,a2);
      break;

  }

  return result;
}
uint32 sub_8003D0F0(uint32 a1, uint32 a2)
{
  a2 &= 0xFFFFu;
  if ((a2 == 8704))
    return r_u32(((uint32)((a1 + 516))));
  else
    return ((short)(sub_8004D2B4(a1,a2)));
}
/* TODO Missing call adapter SLOWORD */
/* TODO Missing call adapter abs32 */
/* TODO Missing call adapter sub_80087A3C */
uint32 sub_80067E9C(uint32 a1, uint32 a2, uint32 a3)
{
  uint32 v6;
  sint32 v7;
  sint32 v8;
  sint32 v9;
  sint32 v11;
  sint32 v12;
  sint32 v13;
  sint32 v14;
  sint32 v15;
  int v16[2];
  sint32 v17;
  v6 = 0;
  if (!r_u32(0x800FF904u))
    return 0;
  v7 = r_u32(((uint32)((r_u32(0x800FF904u) + 8))));
  v8 = r_u32(((uint32)((r_u32(0x800FF904u) + 12))));
  v16[0] = r_u32(((uint32)((r_u32(0x800FF904u) + 4))));
  v16[1] = v7;
  v17 = v8;
  v9 = sub_8006696C(a1,v16);
  if ((((sint32)(a2)) >= v9))
    return 268374015;
  if ((v9 >= ((sint32)(a3))))
    return 0;
  v11 = ((4095 * (a3 - v9)) / a3);
  if ((((sint32)(((void)((((sint16)(r_u16(((uint32)((r_u32(0x800FF904u) + 16)))))) - 1024)),abort(),0u))) < 64))
    return (v11 | (v11 << 16));
  v12 = (((1024 - ((unsigned short)(((void)((r_u32((a1+(2)*4u)) - v17)),(void)((r_u32(a1) - v16[0])),abort(),0u)))) - (r_u16(((uint32)((r_u32(0x800FF904u) + 494)))) - 2048)) & 0xFFF);
  if ((((uint32)((v12 - 1025))) < 0x7FF))
  {
    v6 = 0x80000000;
    v11 -= (v11 >> 4);
  }
  v13 = v12;
  if ((v12 >= 2048))
  {
    v15 = v11;
    v14 = (v11 + ((v11 * ((void)(r_u32((0x800F863Cu+(v13)*4u))),abort(),0u)) >> 12));
  }
  else
  {
    v14 = v11;
    v15 = (v11 - ((v11 * ((void)(r_u32((0x800F863Cu+(v13)*4u))),abort(),0u)) >> 12));
  }
  return ((v15 | (v14 << 16)) | v6);
}
/* TODO Missing call adapter sub_8003A21C */
/* TODO Missing call adapter sub_8003A274 */
void sub_8004DB90(uint32 a1, uint32 a2)
{
  sint32 v3;
  unsigned short v4;
  uint32 v5;
  sint32 v6;
  short v7;
  sint8 v8;
  if ((a2 == 8960))
  {
    if (r_u32(((uint32)((a1 + 496)))))
    {
      v3 = r_u32(0x24F0);
      v4 = r_u32(r_u32(0x2490));
      w_u32(((uint32)((a1 + 400))),(r_u32(0x2490) + 2));
      ((void)(v3),(void)(v4),abort(),0u);
    }
  }
  else
    if ((a2 == 8961))
  {
    v5 = ((sint32)(r_u32(((uint32)((a1 + 400))))));
    v6 = r_u32(((uint32)((a1 + 496))));
    v7 = r_u32((v5+=2u,v5-2u));
    w_u32(((uint32)((a1 + 400))),v5);
    v8 = r_u8(((uint32)(v5)));
    w_u32(((uint32)((a1 + 400))),(v5+(1)*2u));
    if (v6)
      ((void)(v6),(void)(v7),(void)(v8),abort(),0u);
  }
  else
  {
    sub_8004CFDC(a1,a2);
  }
}
uint32 sub_8004B9A4(uint32 a1)
{
  sint32 v2;
  short v3;
  v2 = (a1 + 104);
  v3 = r_u16(0x800FF5E8u);
  w_u32(((uint32)((a1 + 164))),0x800FF5E4u);
  w_u16(((uint32)((a1 + 168))),v3);
  w_u16(((uint32)((a1 + 216))),0);
  sub_8006C0B8((a1 + 104),(a1 + 116));
  sub_8006C270(v2,(a1 + 129));
  sub_8006C05C(v2);
  sub_8006C0B8((a1 + 4),v2);
  sub_8006C730((a1 + 16),(a1 + 132));
  sub_8006C624((a1 + 16));
  sub_8006C730((a1 + 132),(a1 + 138));
  sub_8006C8E8((a1 + 132),(a1 + 144));
  return sub_8006C64C((a1 + 132));
}
uint32 sub_8003384C(uint32 a1, uint32 a2)
{
  sint32 result;
  result = (r_u8(((uint32)((a1 + 152)))) | a2);
  w_u8(((uint32)((a1 + 152))),result);
  return result;
}
/* TODO Missing call adapter indirect */
uint32 sub_80032C18(uint32 a1)
{
  sint32 v1;
  sint32 i;
  sint32 result;
  v1 = a1;
  if (a1)
  {
    for (i = r_u32(((uint32)((a1 + 4))));; i = r_u32(((uint32)((i + 4)))))
    {
      result = r_u8(((uint32)((v1 + 63))));
      if (r_u8(((uint32)((v1 + 63)))))
      {
        if (v1)
          result = ((void)((v1 + ((sint16)(r_u16(((uint32)((r_u32(((uint32)((v1 + 68)))) + 8)))))))),(void)(3),abort(),0u);
      }
      v1 = i;
      if (!i)
        break;
    }

  }
  return result;
}
uint32 sub_80076D44(void)
{
  sint32 v0;
  uint32 result;
  v0 = 15;
  result = (((uint32)((0x801028ACu+(2)*4u)))+(3)*1u);
  do
  {
    w_u8(result,0);
    --v0;
    (result-=1u);
  }
  while ((v0 >= 0));
  return result;
}
uint32 sub_80078760(uint32 a1)
{
  return r_u32(((uint32)((a1 + 372))));
}
uint32 sub_800306D8(void)
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
      v1 = r_u32((result+(7)*4u));
      v2 = r_u32((result+(8)*4u));
      w_u32((result+(3)*4u),r_u32((result+(6)*4u)));
      w_u32((result+(4)*4u),v1);
      w_u32((result+(5)*4u),v2);
      v3 = r_u32((result+(19)*4u));
      v4 = r_u32((result+(20)*4u));
      w_u32((result+(27)*4u),r_u32((result+(18)*4u)));
      w_u32((result+(28)*4u),v3);
      w_u32((result+(29)*4u),v4);
      v5 = r_u32((result+(22)*4u));
      v6 = r_u32((result+(23)*4u));
      w_u32((result+(30)*4u),r_u32((result+(21)*4u)));
      w_u32((result+(31)*4u),v5);
      w_u32((result+(32)*4u),v6);
      v7 = r_u32((result+(25)*4u));
      v8 = r_u32((result+(26)*4u));
      w_u32((result+(33)*4u),r_u32((result+(24)*4u)));
      w_u32((result+(34)*4u),v7);
      w_u32((result+(35)*4u),v8);
      result = ((uint32)(r_u32((result+(1)*4u))));
    }
    while (result);
  }
  return result;
}
void sub_80085ACC(uint32 a1)
{
  short v1;
  short v2;
  short v3;
  short v4;
  short v5;
  v1 = r_u16((a1+(2)*2u));
  v2 = r_u16((a1+(3)*2u));
  v3 = r_u16((a1+(5)*2u));
  v4 = r_u16((a1+(6)*2u));
  v5 = r_u16((a1+(7)*2u));
  w_u16((a1+(3)*2u),r_u16((a1+(1)*2u)));
  w_u16((a1+(6)*2u),v1);
  w_u16((a1+(1)*2u),v2);
  w_u16((a1+(7)*2u),v3);
  w_u16((a1+(2)*2u),v4);
  w_u16((a1+(5)*2u),v5);
}
uint32 sub_80076274(uint32 a1, uint32 a2)
{
  uint32 result;
  uint32 v3;
  sint32 v4;
  sint32 v5;
  result = a1;
  v3 = ((uint32)((((uint32)(0x800F863Cu))+(((2 * a2) & 0x3FFC))*1u)));
  v4 = ((sint16)(r_u16((v3+(1)*2u))));
  v5 = ((sint16)(r_u16(v3)));
  w_u32((result+(1)*4u),0);
  w_u32((result+(2)*4u),0);
  w_u32(result,v5);
  w_u32((result+(3)*4u),v4);
  return result;
}
/* TODO Missing call adapter SLOWORD */
uint32 sub_80076800(uint32 a1, uint32 a2, uint32 a3, uint32 a4)
{
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
  sint32 result;
  v7 = ((sint32)(r_u32(a1)));
  v8 = a3;
  v9 = (((((((sint32)(r_u32(a1))) * r_u32(a2)) + (((sint32)(r_u32((a1+(1)*4u)))) * r_u32((a2+(1)*4u)))) + (((sint32)(r_u32((a1+(2)*4u)))) * r_u32((a2+(2)*4u)))) + (((sint32)(r_u32((a1+(3)*4u)))) * r_u32((a2+(3)*4u)))) >> 12);
  v10 = (v9 + 4096);
  if ((v9 < 0))
  {
    v9 = -v9;
    v11 = ((sint32)(r_u32((a1+(1)*4u))));
    w_u32(a1,-v7);
    v12 = ((sint32)(r_u32((a1+(3)*4u))));
    w_u32((a1+(1)*4u),-v11);
    v13 = ((sint32)(r_u32((a1+(2)*4u))));
    w_u32((a1+(3)*4u),-v12);
    w_u32((a1+(2)*4u),-v13);
    v10 = (v9 + 4096);
  }
  if ((v10 < 129))
  {
    w_u32(a4,-((sint32)(r_u32((a1+(1)*4u)))));
    w_u32((a4+(1)*4u),-(((sint32)(r_u32(a1)))));
    w_u32((a4+(2)*4u),-((sint32)(r_u32((a1+(3)*4u)))));
    w_u32((a4+(3)*4u),((sint32)(r_u32((a1+(2)*4u)))));
    v19 = ((sint16)(r_u16(((uint32)((((uint32)(0x800F863Cu))+(((((uint32)((3217 * (4096 - a3)))) >> 9) & 0x3FFC))*1u))))));
    v20 = ((sint16)(r_u16(((uint32)((((uint32)(0x800F863Cu))+(((((uint32)((3217 * a3))) >> 9) & 0x3FFC))*1u))))));
    w_u32(a4,(((v19 * ((sint32)(r_u32(a1)))) + (v20 * ((sint32)(r_u32(a4))))) >> 12));
    w_u32((a4+(1)*4u),(((v19 * ((sint32)(r_u32((a1+(1)*4u))))) + (v20 * ((sint32)(r_u32((a4+(1)*4u)))))) >> 12));
    w_u32((a4+(2)*4u),(((v19 * ((sint32)(r_u32((a1+(2)*4u))))) + (v20 * ((sint32)(r_u32((a4+(2)*4u)))))) >> 12));
    v17 = (v19 * ((sint32)(r_u32((a1+(3)*4u)))));
    v18 = (v20 * ((sint32)(r_u32((a4+(3)*4u)))));
  }
  else
  {
    v14 = (4096 - a3);
    if (((4096 - v9) >= 129))
    {
      v15 = sub_80067930(v9);
      v16 = ((void)(r_u32((0x800F863Cu+((v15 & 0xFFF))*4u))),abort(),0u);
      v14 = ((((sint16)(r_u16(((uint32)((((uint32)(0x800F863Cu))+(((((4096 - v8) * v15) >> 10) & 0x3FFC))*1u)))))) << 12) / v16);
      a3 = ((((sint16)(r_u16(((uint32)((((uint32)(0x800F863Cu))+((((v8 * v15) >> 10) & 0x3FFC))*1u)))))) << 12) / v16);
    }
    w_u32(a4,(((v14 * ((sint32)(r_u32(a1)))) + (a3 * r_u32(a2))) >> 12));
    w_u32((a4+(1)*4u),(((v14 * ((sint32)(r_u32((a1+(1)*4u))))) + (a3 * r_u32((a2+(1)*4u)))) >> 12));
    w_u32((a4+(2)*4u),(((v14 * ((sint32)(r_u32((a1+(2)*4u))))) + (a3 * r_u32((a2+(2)*4u)))) >> 12));
    v17 = (v14 * ((sint32)(r_u32((a1+(3)*4u)))));
    v18 = (a3 * r_u32((a2+(3)*4u)));
  }
  result = ((v17 + v18) >> 12);
  w_u32((a4+(3)*4u),result);
  return result;
}
/* TODO Missing call adapter JUMPOUT */
uint32 sub_80082B14(uint32 geometry, xport_draft_polygon_strip_context *context)
{
  uint32 packet=context->packet_cursor, clipping=context->clipping_mask;
  uint32 remaining=context->remaining_segments;
  uint32 word0=context->packet_words[0], word1=context->packet_words[1];
  uint32 word2=context->packet_words[2], word3=context->packet_words[3];
  uint32 word4=context->packet_words[4];
  uint32 repeat;
  do {
    uint32 next0,next1,next2,next3,mask;
    if (packet>=context->packet_limit) {
      context->packet_cursor=packet; context->clipping_mask=clipping;
      context->remaining_segments=remaining;
      /* TODO Resume assembly packet continuation at 80082F60 */
      abort();
    }
    w_u32(packet+4,word0); w_u32(packet+8,word1);
    w_u32(packet+12,word2|context->texture_high0);
    w_u32(packet+16,word3); w_u32(packet+20,word4|context->texture_high1);
    next0=r_u32(geometry+16); next1=r_u16(geometry+24);
    next2=r_u32(geometry+144); next3=r_u16(geometry+152);
    geometry+=16; mask=next0&next2;
    repeat=clipping&mask; clipping=mask&context->frustum_mask;
    if (!repeat) {
      packet+=40; w_u32(packet-40,packet+0x09000000u);
      w_u32(packet-16,next0); w_u32(packet-12,next1);
      w_u32(packet-8,next2); w_u32(packet-4,next3);
    }
    word1=next0; word2=next1; word3=next2; word4=next3;
    repeat=remaining!=0; --remaining;
  } while (repeat);
  context->packet_cursor=packet; context->clipping_mask=clipping;
  context->remaining_segments=remaining;
  context->packet_words[0]=word0; context->packet_words[1]=word1;
  context->packet_words[2]=word2; context->packet_words[3]=word3;
  context->packet_words[4]=word4;
  return packet;
}
/* TODO Missing call adapter sub_8007B2E4 */
/* TODO Missing call adapter sub_8007FBD4 */
/* TODO Missing call adapter sub_80081458 */
/* TODO Missing call adapter sub_800874CC */
/* TODO Missing call adapter sub_800878DC */
/* TODO Missing call adapter sub_8008793C */
/* TODO Missing call adapter sub_8008798C */
/* TODO Missing call adapter sub_800879AC */
/* TODO Missing call adapter xport_draft_missing_gte_adapter */
uint32 sub_8007F340(uint32 a1)
{
  /* TODO Recover GTE register carriers and missing host GTE adapters */
  uint32 gte_T4 = 0u;
  uint32 gte_T5 = 0u;
  uint32 gte_T6 = 0u;
  uint32 gte_V0 = 0u;
  
  uint32 v3;
  sint32 v4;
  sint32 v5;
  sint8 v6;
  uint32 v7;
  sint32 v8;
  sint32 v9;
  sint32 v10;
  uint32 v11;
  sint32 result;
  uint32 v13;
  sint32 v14;
  sint32 v15;
  sint32 v19;
  uint32 v20;
  sint32 v21;
  sint32 v22;
  uint32 v23;
  sint32 v24;
  char v25[20];
  char v26[12];
  uint16 v27[10];
  sint32 v28;
  sint32 v29;
  sint32 v30;
  char v31[32];
  uint32 v32;
  sint32 v33;
  sint32 v34;
  sint32 v35;
  unsigned short v36;
  unsigned short v37;
  sint32 v38;
  v38 = r_u16(a1);
  w_u32(0x800FFB44u,0);
  ((void)((r_u32(0x800FFB0Cu) + 116)),abort(),0u);
  sub_800854F4((a1+(162)*2u),v25);
  v21 = ((((sint32)(r_u32((((uint32)(a1))+(1)*4u)))) >> 12) - r_u32(((uint32)((r_u32(0x800FFB0Cu) + 4)))));
  v22 = ((((sint32)(r_u32((((uint32)(a1))+(2)*4u)))) >> 12) - r_u32(((uint32)((r_u32(0x800FFB0Cu) + 8)))));
  v23 = ((uint32)(((((sint32)(r_u32((((uint32)(a1))+(3)*4u)))) >> 12) - r_u32(((uint32)((r_u32(0x800FFB0Cu) + 12)))))));
  gte_V0 = &v21;
  (abort(),0u);
  v34 = r_u32((0x800EAEF8u+(((16 * r_u8((((uint32)(a1))+(27)*1u))) + 4))*4u));
  v32 = r_u32(((uint32)((v34 - 4))));
  (abort(),0u);
  if (((v38 & 4) != 0))
  {
    v3 = ((uint32)(r_u32((((uint32)(a1))+(89)*4u))));
  }
  else
  {
    v4 = r_u32((0x800EAEF8u+(((16 * r_u8((((uint32)(a1))+(27)*1u))) + 6))*4u));
    v5 = (8 * r_u8((((uint32)(a1))+(26)*1u)));
    if (r_u16(((uint32)(((v5 + v4) + 10)))))
    {
      sub_8007CECC(((sint32)(a1)));
      v3 = ((uint32)(r_u32(0x800ED760u)));
    }
    else
    {
      v3 = ((uint32)(((v4 + r_u32(((uint32)(((v5 + v4) + 4))))) + ((24 * r_u8((((uint32)(a1))+(24)*1u))) * v32))));
    }
  }
  if (((v38 & 0x80) != 0))
  {
    ((void)(0x800EE710u),abort(),0u);
    ((void)(r_u16(0x800FFA8Cu)),(void)(r_u16(0x800FFA8Eu)),(void)(r_u16(0x800FFA90u)),abort(),0u);
    ((void)(r_u16(0x800FFA94u)),(void)(r_u16(0x800FFA96u)),(void)(r_u16(0x800FFA98u)),abort(),0u);
    ((void)(0x800EE6F0u),(void)((a1+(162)*2u)),(void)(v31),abort(),0u);
  }
  v6 = r_u8((((uint32)(a1))+(179)*1u));
  if (((v6 & 0x7F) != 0))
  {
    v7 = ((v6 - 1) & 0x7F);
    if ((v7 >= 0xF))
      v7 = 14;
    v35 = r_u32((0x800A6808u+(v7)*4u));
    if ((r_u32(0x800FFAB8u) != r_u32(0x800FF650u)))
    {
      w_u32(0x800FFABCu,r_u32(0x800FFAA8u));
      w_u32(0x800FFAB8u,r_u32(0x800FF650u));
    }
    v8 = r_u32(((uint32)((v35 - 4))));
    v36 = r_u16((a1+(90)*2u));
    v9 = r_u32((((uint32)(a1))+(44)*4u));
    v37 = r_u16((a1+(91)*2u));
    v33 = (v9 & 0xFFFFFF);
    v35 += (8 * (r_u32(0x800FFABCu) % v8));
  }
  if (r_u32(0x800FFB44u))
    v10 = 528483320;
  else
    v10 = (r_u32(0x800FFAC0u) + 7992);
  v11 = 0;
  w_u32(0x800FFB18u,v10);
  w_u32(0x800FFB04u,r_u32((0x800EAEF8u+(((16 * r_u8((((uint32)(a1))+(27)*1u))) + 8))*4u)));
  while (1)
  {
    result = (v11 < v32);
    if ((v11 >= v32))
      break;
    w_u32(0x800FFB38u,((r_u32((((uint32)(a1))+(93)*4u)) & (1 << v11)) != 0));
    v13 = r_u32(((uint32)(((4 * v11) + v34))));
    sub_80084578(v25,v31,v3);
    if (((r_u32(v13) & 1) != 0))
    {
      ((void)((a1+(162)*2u)),(void)(v3),(void)(v27),abort(),0u);
      ((void)(v27),(void)(v26),(void)(0x800F3E70u),(void)(v13),abort(),0u);
      v14 = (v6 & 0x80);
    }
    else
    {
      v14 = (v6 & 0x80);
      if (((v38 & 0x80) != 0))
      {
        sub_80081794(v13 + 32u + r_u32(v13 + 4u) * 8u, r_u32(v13 + 8u) == r_u32(v13 + 12u) ? r_u32(v13 + 8u) : r_u32(v13 + 4u));
        v14 = (v6 & 0x80);
      }
    }
    if (!v14)
    {
      if (r_u32(0x800FFB44u))
        ((void)(v13),abort(),0u);
      else
        sub_8007FB34(v13);
    }
    if (!r_u32(0x800FFB38u))
    {
      v15 = (r_u8((((uint32)(a1))+(314)*1u)) != 0);
      if ((((v6 & 0x7F) != 0) || r_u8((((uint32)(a1))+(314)*1u))))
      {
        ((void)((a1+(162)*2u)),(void)(v3),(void)(v27),abort(),0u);
        gte_T4 = r_u16((v3+(9)*2u));
        gte_T5 = r_u16((v3+(10)*2u));
        gte_T6 = r_u16((v3+(11)*2u));
        (abort(),0u);
        sub_80085C94((a1+(162)*2u));
        (abort(),0u);
        v28 += (((sint32)(r_u32((((uint32)(a1))+(1)*4u)))) >> 12);
        v29 += (((sint32)(r_u32((((uint32)(a1))+(2)*4u)))) >> 12);
        v30 += (((sint32)(r_u32((((uint32)(a1))+(3)*4u)))) >> 12);
      }
      if ((((v6 & 0x7F) != 0) && (!r_u32(0x800FFB3Cu) || (v29 < ((r_u32(0x800FF37Cu) >> 12) - 40)))))
        ((void)(v13),(void)(v27),(void)(v33),(void)(v36),(void)(((short)(v37))),(void)(r_u32(((uint32)((v35 + 4))))),(void)(v21),(void)(v22),(void)(((sint32)(v23))),(void)(v24),abort(),0u);
      if (v15)
      {
        v19 = 0;
        if (r_u8((((uint32)(a1))+(314)*1u)))
        {
          v20 = v13;
          do
          {
            sub_8007A8F8(v20,v27,((sint32)((a1+(82)*2u))),(((uint32)(a1))+(46)*4u),(r_u32((((uint32)(a1))+(79)*4u)) + (8 * v19)),r_u32(((uint32)(((4 * v19) + r_u32((((uint32)(a1))+(80)*4u)))))),v21,v22,v23,v24);
            ++v19;
            v20 = v13;
          }
          while ((v19 < r_u8((((uint32)(a1))+(314)*1u))));
        }
      }
    }
    ++v11;
    v3 += (12)*2u;
  }

  return result;
}
/* TODO Missing call adapter xport_draft_missing_gte_adapter */
void sub_80081794(uint32 a1, uint32 a2)
{
  /* TODO Recover GTE register carriers and missing host GTE adapters */
  uint32 gte_T0 = 0u;
  uint32 gte_T1 = 0u;
  uint32 gte_T7 = 0u;
  uint32 gte_T8 = 0u;
  uint32 gte_T9 = 0u;
  
  uint32 v2;
  v2 = 0x800F3E70u;
  gte_T0 = 0xFFFFFF;
  (abort(),0u);
  gte_T0 = ((sint32)(r_u32(a1)));
  gte_T1 = ((sint32)(r_u32((a1+(1)*4u))));
  do
  {
    (abort(),0u);
    a1 += (2)*4u;
    --a2;
    (abort(),0u);
    gte_T0 = ((sint32)(r_u32(a1)));
    gte_T1 = ((sint32)(r_u32((a1+(1)*4u))));
    (abort(),0u);
    w_u32(v2,(((unsigned short)(gte_T7)) | (gte_T8 << 16)));
    w_u32((v2+(1)*4u),gte_T9);
    v2 += (2)*4u;
  }
  while ((((sint32)(a2)) > 0));
}




