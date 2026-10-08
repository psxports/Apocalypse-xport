#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
#include <stdlib.h>
/* TODO Missing call adapter abs16 */
/* TODO Missing call adapter abs32 */
/* TODO Missing call adapter indirect */
/* TODO Missing call adapter sub_8004B428 */
/* TODO Missing call adapter sub_80052FCC */
/* TODO Missing call adapter sub_8005C11C */
/* TODO Missing host buffer adapter xport_draft_host_sub_80032EE4_p2 */
/* TODO Missing host buffer adapter xport_draft_host_sub_8006C3AC_p1 */
/* TODO Missing host buffer adapter xport_draft_host_sub_8007CD74_p13 */
/* TODO Missing host buffer adapter xport_draft_host_sub_800872BC_p12 */
uint32 sub_8004E71C(uint32 a1)
{
  sint32 v2;
  sint32 v3;
  sint32 v4;
  sint32 v5;
  sint32 v6;
  sint32 v7;
  sint32 v8;
  short v9;
  sint8 v10;
  sint32 v11;
  sint32 v12;
  short v13;
  sint32 v14;
  sint32 v15;
  sint32 v16;
  uint32 v17;
  uint32 v18;
  unsigned char v19;
  sint32 v20;
  sint32 v21;
  short v22;
  sint32 v23;
  sint32 v24;
  sint32 v25;
  sint32 v26;
  sint32 v27;
  sint32 v28;
  sint32 v29;
  sint32 v30;
  short v31;
  unsigned short v32;
  short v33;
  unsigned short v34;
  sint32 v35;
  sint32 v36;
  sint32 v37;
  sint32 v38;
  sint32 v39;
  sint32 v40;
  sint8 v41;
  sint32 v42;
  sint32 v43;
  sint32 v44;
  uint32 v45;
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
  unsigned short v56;
  sint32 v57;
  short v58;
  sint32 v59;
  sint32 v60;
  short v61;
  unsigned short v62;
  sint32 v63;
  sint32 v64;
  sint32 v65;
  sint32 v66;
  sint32 v67;
  sint32 v68;
  uint32 v69;
  sint32 v70;
  uint32 v71;
  sint32 v72;
  sint32 v73;
  sint32 v74;
  sint32 v75;
  sint32 v76;
  short v77;
  sint32 result;
  sint32 v79;
  sint32 v80;
  unsigned char v81;
  sint32 v82;
  short v83;
  short v84;
  sint32 v85;
  unsigned short v86;
  sint32 v87;
  unsigned short v88;
  int v89[4];
  int v90[4];
  v2 = r_u32(0x800A71D0u);
  v3 = r_u32(0x800A71D4u);
  w_u32(((uint32)(((uint32)(a1) + (uint32)(116)))),0x800A71CCu);
  w_u32(((uint32)(((uint32)(a1) + (uint32)(120)))),v2);
  w_u32(((uint32)(((uint32)(a1) + (uint32)(124)))),v3);
  v4 = r_u32(((uint32)(((uint32)(a1) + (uint32)(620)))));
  v5 = ((uint32)(a1) + (uint32)(4));
  if (v4)
    ((void)(((sint32)(((uint32)(v4) + (uint32)(((sint16)(r_u16(((uint32)(((uint32)(r_u32(((uint32)(v4)))) + (uint32)(24)))))))))))),(void)(v5),(void)(0x800FF5E4u),abort(),0u);
  if ((r_u16(((uint32)(((uint32)(a1) + (uint32)(390))))) == 128))
  {
    v6 = sub_80066570(240);
    v7 = 98;
    if (!v6 || (v8 = sub_80066570(240), v7 = 99, !v8))
      sub_80069EF4(v7,((uint32)(a1) + (uint32)(4)),0);
  }
  v9 = r_u16(((uint32)(((uint32)(a1) + (uint32)(76)))));
  w_u16(((uint32)(((uint32)(a1) + (uint32)(388)))),v9);
  if (((v9 & 1) != 0))
  {
    v10 = r_u8(((uint32)(((uint32)(a1) + (uint32)(384)))));
    w_u16(((uint32)(((uint32)(a1) + (uint32)(76)))),(v9 & 0xFFFE));
    w_u8(((uint32)(((uint32)(a1) + (uint32)(384)))),((uint32)(v10) + (uint32)(1)));
  }
  if (((r_u16(((uint32)(((uint32)(a1) + (uint32)(76))))) & 4) != 0))
    sub_8004E624(a1);
  if (r_u8(((uint32)(((uint32)(a1) + (uint32)(380))))))
  {
    v11 = r_u16(((uint32)(((uint32)(a1) + (uint32)(476)))));
    v12 = (v11 == 0);
    v13 = ((uint32)(v11) - (uint32)(1));
    if (v12)
      sub_8004BE90(a1,r_u32(((uint32)(((uint32)(a1) + (uint32)(400))))));
    else
      w_u16(((uint32)(((uint32)(a1) + (uint32)(476)))),v13);
  }
  else
  {
    sub_8004FF6C(a1);
  }
  v14 = r_u8(((uint32)(((uint32)(a1) + (uint32)(381)))));
  w_u32(((uint32)(((uint32)(a1) + (uint32)(396)))),(r_u32(((uint32)(((uint32)(a1) + (uint32)(396)))))&(~0x400u)));
  if (!v14)
  {
    v15 = r_u16(((uint32)(((uint32)(a1) + (uint32)(390)))));
    if ((v15 == 8))
    {
      v16 = a1;
      v17 = 0x800A6CD4u;
    }
    else
    {
      if ((r_u16(((uint32)(((uint32)(a1) + (uint32)(390))))) < 9u))
      {
        if ((v15 != 2))
        {
          if ((r_u16(((uint32)(((uint32)(a1) + (uint32)(390))))) >= 3u))
          {
            if ((v15 != 4))
            {
              w_u8(((uint32)(((uint32)(a1) + (uint32)(381)))),1);
              goto LABEL_40;
            }
            v16 = a1;
            v17 = 0x800A6C84u;
          }
          else
          {
            if ((v15 != 1))
            {
              w_u8(((uint32)(((uint32)(a1) + (uint32)(381)))),1);
              goto LABEL_40;
            }
            v16 = a1;
            v17 = 0x800A6C5Cu;
          }
          goto LABEL_39;
        }
        goto LABEL_38;
      }
      if ((v15 != 64))
      {
        if ((r_u16(((uint32)(((uint32)(a1) + (uint32)(390))))) < 0x41u))
        {
          if ((v15 != 16))
          {
            w_u8(((uint32)(((uint32)(a1) + (uint32)(381)))),1);
            goto LABEL_40;
          }
          v16 = a1;
          v17 = 0x800A6CC0u;
          goto LABEL_39;
        }
        if ((v15 != 128))
        {
          if ((v15 != 256))
          {
            w_u8(((uint32)(((uint32)(a1) + (uint32)(381)))),1);
            goto LABEL_40;
          }
          v16 = a1;
          v17 = 0x800A6C98u;
          goto LABEL_39;
        }
        LABEL_38:
        v16 = a1;

        v17 = 0x800A6C70u;
        goto LABEL_39;
      }
      v16 = a1;
      v17 = 0x800A6CACu;
    }
    LABEL_39:
    sub_8007CA50(v16,v17);

    w_u8(((uint32)(((uint32)(a1) + (uint32)(381)))),1);
  }
  LABEL_40:
  if (r_u8(((uint32)(((uint32)(a1) + (uint32)(382))))))
    sub_8006C0B8(((uint32)(a1) + (uint32)(116)),0x800A6844u);

  sub_8004B9A4(a1);
  if (((r_u32(((uint32)(((uint32)(a1) + (uint32)(396))))) & 2) != 0))
  {
    v18 = r_u8(((uint32)(((uint32)(a1) + (uint32)(386)))));
    v19 = ((uint32)(r_u8(((uint32)(((uint32)(a1) + (uint32)(387)))))) + (uint32)(1));
    w_u8(((uint32)(((uint32)(a1) + (uint32)(387)))),v19);
    if ((v19 >= v18))
    {
      v20 = ((sint16)(r_u16(((uint32)(((uint32)(a1) + (uint32)(456)))))));
      w_u8(((uint32)(((uint32)(a1) + (uint32)(387)))),0);
      v21 = sub_80067A18(((uint32)(a1) + (uint32)(4)),v20,((uint32)(4) * (uint32)(v20)));
      if ((v21 != -1))
      {
        v22 = (r_u16(((uint32)(((uint32)(a1) + (uint32)(216))))) | 2);
        w_u32(((uint32)(((uint32)(a1) + (uint32)(8)))),((uint32)(v21) - (uint32)(((uint32)(((sint16)(r_u16(((uint32)(((uint32)(a1) + (uint32)(456)))))))) << (uint32)(12)))));
        w_u16(((uint32)(((uint32)(a1) + (uint32)(216)))),v22);
      }
    }
  }
  sub_8007FDF0(a1);
  v23 = r_u32(((uint32)(((uint32)(a1) + (uint32)(12)))));
  v24 = r_u32(((uint32)(((uint32)(a1) + (uint32)(396)))));
  v25 = ((uint32)(((sint16)(r_u16(((uint32)(((uint32)(a1) + (uint32)(456)))))))) << (uint32)(12));
  w_u32(((uint32)(((uint32)(a1) + (uint32)(184)))),r_u32(((uint32)(((uint32)(a1) + (uint32)(4))))));
  v26 = r_u32(((uint32)(((uint32)(a1) + (uint32)(8)))));
  w_u32(((uint32)(((uint32)(a1) + (uint32)(192)))),v23);
  w_u32(((uint32)(((uint32)(a1) + (uint32)(188)))),((sint32)(((uint32)(v26) + (uint32)(v25)))));
  if (((v24 & 0x80) != 0))
  {
    v27 = r_u32(((uint32)(((uint32)(a1) + (uint32)(508)))));
    if ((r_u16(((uint32)(((uint32)(v27) + (uint32)(58))))) == 50))
    {
      v28 = r_u32(((uint32)(((uint32)(v27) + (uint32)(184)))));
      v29 = r_u32(((uint32)(((uint32)(v27) + (uint32)(188)))));
      v30 = r_u32(((uint32)(((uint32)(v27) + (uint32)(192)))));
    }
    else
    {
      v28 = r_u32(((uint32)(((uint32)(v27) + (uint32)(4)))));
      v29 = r_u32(((uint32)(((uint32)(v27) + (uint32)(8)))));
      v30 = r_u32(((uint32)(((uint32)(v27) + (uint32)(12)))));
    }
    w_u32(((uint32)(((uint32)(a1) + (uint32)(496)))),v28);
    w_u32(((uint32)(((uint32)(a1) + (uint32)(500)))),v29);
    w_u32(((uint32)(((uint32)(a1) + (uint32)(504)))),v30);
    v31 = ratan2(((uint32)(r_u32(((uint32)(((uint32)(a1) + (uint32)(504)))))) - (uint32)(r_u32(((uint32)(((uint32)(a1) + (uint32)(12))))))),((uint32)(r_u32(((uint32)(((uint32)(a1) + (uint32)(496)))))) - (uint32)(r_u32(((uint32)(((uint32)(a1) + (uint32)(4))))))));
    v82 = 0u;
    v82 = ((v82 & 0x0000FFFFu)|((((((uint32)(3072) - (uint32)(v31)) & 0xFFF)) & 0xFFFFu)<<16));
    v83 = 0;
    v32 = r_u16(((uint32)(((uint32)(a1) + (uint32)(20)))));
    v85 = r_u32(((uint32)(((uint32)(a1) + (uint32)(16)))));
    v86 = v32;
    sub_80066CF0((((unsigned short)(v85)) | ((uint32)(((v85>>16)&65535u)) << (uint32)(16))),v32,((uint32)(a1) + (uint32)(132)),((uint32)(a1) + (uint32)(138)),v82,v83,32u);
  }
  if (((r_u32(((uint32)(((uint32)(a1) + (uint32)(396))))) & 1) != 0))
  {
    v33 = ratan2(((uint32)(r_u32(((uint32)(((uint32)(a1) + (uint32)(492)))))) - (uint32)(r_u32(((uint32)(((uint32)(a1) + (uint32)(12))))))),((uint32)(r_u32(((uint32)(((uint32)(a1) + (uint32)(484)))))) - (uint32)(r_u32(((uint32)(((uint32)(a1) + (uint32)(4))))))));
    v82 = 0u;
    v82 = ((v82 & 0x0000FFFFu)|((((((uint32)(3072) - (uint32)(v33)) & 0xFFF)) & 0xFFFFu)<<16));
    v83 = 0;
    v34 = r_u16(((uint32)(((uint32)(a1) + (uint32)(20)))));
    v87 = r_u32(((uint32)(((uint32)(a1) + (uint32)(16)))));
    v88 = v34;
    sub_80066CF0((((unsigned short)(v87)) | ((uint32)(((v87>>16)&65535u)) << (uint32)(16))),v34,((uint32)(a1) + (uint32)(132)),((uint32)(a1) + (uint32)(138)),v82,v83,32u);
    v35 = r_u32(((uint32)(((uint32)(a1) + (uint32)(396)))));
    if (((v35 & 8) != 0))
    {
      if ((((sint32)(((sint8)(r_u8(((uint32)(((uint32)(a1) + (uint32)(24))))))))) >= 3))
      {
        if (r_u8(((uint32)(((uint32)(a1) + (uint32)(612))))))
          w_u8(((uint32)(((uint32)(a1) + (uint32)(382)))),1);
      }
      else
      {
        w_u32(((uint32)(((uint32)(a1) + (uint32)(304)))),0x10000);
        w_u8(((uint32)(((uint32)(a1) + (uint32)(382)))),0);
        w_u32(((uint32)(((uint32)(a1) + (uint32)(112)))),0);
        w_u32(((uint32)(((uint32)(a1) + (uint32)(108)))),0);
        w_u32(((uint32)(((uint32)(a1) + (uint32)(104)))),0);
        w_u32(((uint32)(((uint32)(a1) + (uint32)(124)))),0);
        w_u32(((uint32)(((uint32)(a1) + (uint32)(120)))),0);
        w_u32(((uint32)(((uint32)(a1) + (uint32)(116)))),0);
      }
      if (((((sint32)(((sint8)(r_u8(((uint32)(((uint32)(a1) + (uint32)(24))))))))) < 3) || ((r_u32(((uint32)(((uint32)(a1) + (uint32)(396))))) & 0x20) != 0)))
      {
        if ((((sint32)(((sint8)(r_u8(((uint32)(((uint32)(a1) + (uint32)(24))))))))) >= 22))
        {
          if (!(r_u8(((uint32)(((uint32)(a1) + (uint32)(612)))))))
          {
            LABEL_68:
            if (!(r_u8(((uint32)(((uint32)(a1) + (uint32)(303)))))))
              goto LABEL_83;

            v45 = (r_u32(((uint32)(((uint32)(a1) + (uint32)(396))))) & 0xFFFFFFD6);
            goto LABEL_82;
          }
          w_u32(((uint32)(((uint32)(a1) + (uint32)(304)))),0);
        }
      }
      else
      {
        v36 = (((uint32)(r_u32(((uint32)(((uint32)(a1) + (uint32)(484)))))) - (uint32)(r_u32(((uint32)(((uint32)(a1) + (uint32)(4))))))) / r_u8(((uint32)(((uint32)(a1) + (uint32)(612))))));
        v37 = (((uint32)(r_u32(((uint32)(((uint32)(a1) + (uint32)(492)))))) - (uint32)(r_u32(((uint32)(((uint32)(a1) + (uint32)(12))))))) / r_u8(((uint32)(((uint32)(a1) + (uint32)(612))))));
        v38 = r_u8(((uint32)(((uint32)(a1) + (uint32)(612)))));
        v39 = ((uint32)(r_u32(((uint32)(((uint32)(a1) + (uint32)(488)))))) - (uint32)(r_u32(((uint32)(((uint32)(a1) + (uint32)(8)))))));
        w_u8(((uint32)(((uint32)(a1) + (uint32)(129)))),31);
        w_u32(((uint32)(((uint32)(a1) + (uint32)(396)))),(r_u32(((uint32)(((uint32)(a1) + (uint32)(396)))))|(0x20u)));
        w_u32(((uint32)(((uint32)(a1) + (uint32)(104)))),v36);
        w_u32(((uint32)(((uint32)(a1) + (uint32)(112)))),v37);
        w_u32(((uint32)(((uint32)(a1) + (uint32)(108)))),((uint32)((((sint32)(v39)) / ((sint32)(v38)))) - (uint32)(((uint32)(v38) << (uint32)(14)))));
        w_u8(((uint32)(((uint32)(a1) + (uint32)(130)))),31);
        w_u8(((uint32)(((uint32)(a1) + (uint32)(131)))),31);
      }
      v40 = r_u8(((uint32)(((uint32)(a1) + (uint32)(612)))));
      v12 = (v40 == 0);
      v41 = ((uint32)(v40) - (uint32)(1));
      if (!v12)
      {
        w_u8(((uint32)(((uint32)(a1) + (uint32)(612)))),v41);
        v12 = (v41 == 0);
        v42 = ((uint32)(a1) + (uint32)(104));
        if (v12 || ((sint32)r_u32(a1 + 108u) > 0 && (v42 = a1 + 104u, r_u32(a1 + 488u) < r_u32(a1 + 8u))))
        {
          w_u8(((uint32)(((uint32)(a1) + (uint32)(612)))),0);
          v43 = r_u32(((uint32)(((uint32)(a1) + (uint32)(488)))));
          v44 = r_u32(((uint32)(((uint32)(a1) + (uint32)(492)))));
          w_u32(((uint32)(((uint32)(a1) + (uint32)(4)))),r_u32(((uint32)(((uint32)(a1) + (uint32)(484))))));
          w_u32(((uint32)(((uint32)(a1) + (uint32)(8)))),v43);
          w_u32(((uint32)(((uint32)(a1) + (uint32)(12)))),v44);
          w_u32(((uint32)(((uint32)(v42) + (uint32)(8)))),0);
          w_u32(((uint32)(((uint32)(v42) + (uint32)(4)))),0);
          w_u32(((uint32)(((uint32)(a1) + (uint32)(104)))),0);
          w_u32(((uint32)(((uint32)(a1) + (uint32)(124)))),0);
          w_u32(((uint32)(((uint32)(a1) + (uint32)(120)))),0);
          w_u32(((uint32)(((uint32)(a1) + (uint32)(304)))),0x10000);
          w_u32(((uint32)(((uint32)(a1) + (uint32)(116)))),0);
          w_u8(((uint32)(((uint32)(a1) + (uint32)(382)))),0);
          w_u8(((uint32)(((uint32)(a1) + (uint32)(129)))),31);
          w_u8(((uint32)(((uint32)(a1) + (uint32)(130)))),3);
          w_u8(((uint32)(((uint32)(a1) + (uint32)(131)))),31);
        }
      }
      goto LABEL_68;
    }
    if (((v35 & 0x10) != 0))
    {
      v46 = r_u32(((uint32)(((uint32)(a1) + (uint32)(488)))));
      if ((((sint32)(v46)) < r_u32(((uint32)(((uint32)(a1) + (uint32)(8)))))))
      {
        w_u32(((uint32)(((uint32)(a1) + (uint32)(8)))),v46);
        w_u8(((uint32)(((uint32)(a1) + (uint32)(382)))),0);
        w_u32(((uint32)(((uint32)(a1) + (uint32)(112)))),0);
        w_u32(((uint32)(((uint32)(a1) + (uint32)(108)))),0);
        v47 = ((sint8)(r_u8(((uint32)(((uint32)(a1) + (uint32)(24)))))));
        w_u32(((uint32)(((uint32)(a1) + (uint32)(104)))),0);
        sub_80063038(a1,4,v47,-1);
      }
      if (r_u8(((uint32)(((uint32)(a1) + (uint32)(303))))))
      {
        v45 = (r_u32(((uint32)(((uint32)(a1) + (uint32)(396))))) & 0xFFFFFFEE);
        LABEL_82:
        w_u32(((uint32)(((uint32)(a1) + (uint32)(396)))),v45);

      }
    }
    else
    {
      v89[0] = (((uint32)(r_u32(((uint32)(((uint32)(a1) + (uint32)(484)))))) - (uint32)(r_u32(((uint32)(((uint32)(a1) + (uint32)(4))))))) >> 12);
      v89[1] = (((uint32)(r_u32(((uint32)(((uint32)(a1) + (uint32)(488)))))) - (uint32)(r_u32(((uint32)(((uint32)(a1) + (uint32)(8))))))) >> 12);
      v89[2] = (((uint32)(r_u32(((uint32)(((uint32)(a1) + (uint32)(492)))))) - (uint32)(r_u32(((uint32)(((uint32)(a1) + (uint32)(12))))))) >> 12);
      xport_draft_host_sub_800872BC_p12(v89,v90);
      v48 = ((void)(r_u16(((uint32)(((uint32)(a1) + (uint32)(134)))))),abort(),0u);
      if ((((sint32)(v48)) >= 65))
        v48 = 64;
      v49 = (((uint32)(r_u16(((uint32)(((uint32)(a1) + (uint32)(436)))))) * (uint32)(((uint32)(64) - (uint32)(v48)))) / 64);
      v50 = ((sint32)(((uint32)(v90[1]) * (uint32)(v49))));
      v51 = ((sint32)(((uint32)(v90[2]) * (uint32)(v49))));
      w_u32(((uint32)(((uint32)(a1) + (uint32)(104)))),((sint32)(((uint32)(v90[0]) * (uint32)(v49)))));
      w_u32(((uint32)(((uint32)(a1) + (uint32)(108)))),v50);
      w_u32(((uint32)(((uint32)(a1) + (uint32)(112)))),v51);
      v52 = ((uint32)(a1) + (uint32)(4));
      if (((r_u32(((uint32)(((uint32)(a1) + (uint32)(396))))) & 2) != 0))
        v53 = (((uint32)(sub_80066918(v52,((uint32)(a1) + (uint32)(484))))) < 0x40);
      else
        v53 = (((uint32)(sub_8006689C(v52,((uint32)(a1) + (uint32)(484))))) < 0x40);
      if (v53)
      {
        v45 = (r_u32(((uint32)(((uint32)(a1) + (uint32)(396))))) & 0xFFFFFFFE);
        goto LABEL_82;
      }
    }
  }
  LABEL_83:
  v54 = r_u8(((uint32)(((uint32)(a1) + (uint32)(26)))));

  if ((v54 == 3))
  {
    v55 = sub_8006325C(a1,3,r_u8(((uint32)(((uint32)(a1) + (uint32)(613))))));
    if ((((sub_80062D24(a1,v55,1) && r_u32(0x800FF5A0u)) && (((uint32)(sub_80066918(((uint32)(a1) + (uint32)(4)),((uint32)(r_u32(0x800FF5A0u)) + (uint32)(4))))) < 0x100)) && (((sint32)(((sint32)(((void)(((uint32)(((uint32)(r_u32(((uint32)(((uint32)(r_u32(0x800FF5A0u)) + (uint32)(8)))))) + (uint32)(((uint32)(r_u8(((uint32)(((uint32)(r_u32(0x800FF5A0u)) + (uint32)(582)))))) << (uint32)(12))))) - (uint32)(((uint32)(r_u32(((uint32)(((uint32)(a1) + (uint32)(8)))))) + (uint32)(((uint32)(((sint16)(r_u16(((uint32)(((uint32)(a1) + (uint32)(456)))))))) << (uint32)(12))))))),abort(),0u))))) <= 0x7FFFF)))
    {
      v56 = sub_8004BDFC(a1,0,1u);
      v57 = sub_8004B914(a1,v56);
      xport_draft_host_sub_8006C3AC_p1(&v82,((uint32)(r_u32(0x800FF5A0u)) + (uint32)(4)),((uint32)(a1) + (uint32)(4)));
      ((void)(((uint32)(r_u32(0x800FF5A0u)) + (uint32)(((sint16)(r_u16(((uint32)(((uint32)(r_u32(((uint32)(((uint32)(r_u32(0x800FF5A0u)) + (uint32)(68)))))) + (uint32)(48)))))))))),(void)(v57),(void)(&v82),(void)(0),abort(),0u);
    }
    v54 = r_u8(((uint32)(((uint32)(a1) + (uint32)(26)))));
  }
  if ((v54 == 1))
  {
    v58 = r_u16(((uint32)(((uint32)(a1) + (uint32)(390)))));
    if (((v58 & 0x10) != 0))
    {
      v59 = sub_8006325C(a1,1,10);
      if ((sub_80062D24(a1,v59,2) && (sub_80066918(((uint32)(a1) + (uint32)(4)),((uint32)(r_u32(0x800FF5A0u)) + (uint32)(4))) < 4500)))
        goto LABEL_101;
    }
    else
      if (((v58 & 8) != 0))
    {
      v60 = sub_8006325C(a1,1,12);
      if (sub_80062D24(a1,v60,2))
        LABEL_101:
      ((void)(a1),abort(),0u);

    }
    else
      if (((v58 & 0x40) != 0))
    {
      v61 = ratan2(((uint32)(r_u32(((uint32)(((uint32)(r_u32(0x800FF5A0u)) + (uint32)(12)))))) - (uint32)(r_u32(((uint32)(((uint32)(a1) + (uint32)(12))))))),((uint32)(r_u32(((uint32)(((uint32)(r_u32(0x800FF5A0u)) + (uint32)(4)))))) - (uint32)(r_u32(((uint32)(((uint32)(a1) + (uint32)(4))))))));
      v82 = 0u;
      v82 = ((v82 & 0x0000FFFFu)|((((((uint32)(3072) - (uint32)(v61)) & 0xFFF)) & 0xFFFFu)<<16));
      v83 = 0;
      v62 = r_u16(((uint32)(((uint32)(a1) + (uint32)(20)))));
      v85 = r_u32(((uint32)(((uint32)(a1) + (uint32)(16)))));
      v86 = v62;
      sub_80066CF0((((unsigned short)(v85)) | ((uint32)(((v85>>16)&65535u)) << (uint32)(16))),v62,((uint32)(a1) + (uint32)(132)),((uint32)(a1) + (uint32)(138)),v82,v83,32u);
      v63 = r_u8(((uint32)(((uint32)(a1) + (uint32)(24)))));
      if (((((uint32)(((uint32)(v63) - (uint32)(21)))) < 0x13) && ((v63 & 3) == 0)))
        goto LABEL_101;
    }
  }
  if ((r_u16(((uint32)(((uint32)(a1) + (uint32)(390))))) == 128))
  {
    v64 = r_u32(0x800FF5A0u);
    w_u32(((uint32)(((uint32)(a1) + (uint32)(176)))),100687952);
    w_u16(((uint32)(((uint32)(a1) + (uint32)(180)))),10240);
    w_u16(((uint32)(((uint32)(a1) + (uint32)(182)))),10);
    if ((((((uint32)(sub_80066918(((uint32)(a1) + (uint32)(4)),((uint32)(v64) + (uint32)(4))))) < 0xA0) && (r_u16(((uint32)(((uint32)(a1) + (uint32)(472))))) != 16)) && r_u32(0x800FF5A0u)))
    {
      v65 = sub_8004B914(a1,1);
      ((void)(((uint32)(r_u32(0x800FF5A0u)) + (uint32)(((sint16)(r_u16(((uint32)(((uint32)(r_u32(((uint32)(((uint32)(r_u32(0x800FF5A0u)) + (uint32)(68)))))) + (uint32)(48)))))))))),(void)(v65),(void)(0x800A71CCu),(void)(0),abort(),0u);
    }
  }
  if ((((r_u32(((uint32)(((uint32)(a1) + (uint32)(396))))) & 0x100) != 0) && !sub_80066570(4)))
  {
    w_u32(0x800FF3ACu,0);
    v66 = sub_80066570(3);
    v67 = ((uint32)(v66) + (uint32)(2));
    if ((v66 != -2))
    {
      do
      {
        v68 = sub_80032DC0(116);
        if (v68)
          ((void)(v68),(void)(((uint32)(a1) + (uint32)(4))),abort(),0u);
        --v67;
      }
      while (v67);
    }
    w_u32(0x800FF3ACu,1);
  }
  if (((r_u32(((uint32)(((uint32)(a1) + (uint32)(396))))) & 4) != 0))
  {
    if (!(r_u32(((uint32)(((uint32)(a1) + (uint32)(608)))))))
    {
      v69 = ((uint32)(sub_80032DC0(176)));
      if (v69)
        v69 = sub_80034C0C(v69,((uint32)(((uint32)(a1) + (uint32)(4)))),64,32,64,112,96,0,48,0x25u,0);
      w_u32(((uint32)(((uint32)(a1) + (uint32)(608)))),v69);
      w_u8((((uint32)(v69))+(66)*1u),1);
      sub_80032EE4(r_u32(((uint32)(((uint32)(a1) + (uint32)(608))))),((uint32)(((uint32)(a1) + (uint32)(4)))));
      w_u32(((uint32)(((uint32)(a1) + (uint32)(396)))),(r_u32(((uint32)(((uint32)(a1) + (uint32)(396)))))|(0x800u)));
    }
    if (((r_u32(0x800FF2F0u) & 1) != 0))
    {
      v70 = r_u16(((uint32)(((uint32)(a1) + (uint32)(390)))));
      if ((v70 == 4))
      {
        v71 = 0x800FF500u;
        v84 = ((unsigned char)(r_u8(0x800FF501u)));
      }
      else
        if ((r_u16(((uint32)(((uint32)(a1) + (uint32)(390))))) >= 5u))
      {
        if ((v70 == 16))
        {
          v71 = 0x800FF518u;
          v84 = ((unsigned char)(r_u8(0x800FF519u)));
        }
        else
          if ((r_u16(((uint32)(((uint32)(a1) + (uint32)(390))))) >= 0x11u))
        {
          if ((v70 != 256))
          {
            v84 = 0;
            goto LABEL_137;
          }
          v71 = 0x800FF520u;
          v84 = ((unsigned char)(r_u8(0x800FF521u)));
        }
        else
        {
          if ((v70 != 8))
          {
            v84 = 0;
            goto LABEL_137;
          }
          v71 = 0x800FF510u;
          v84 = ((unsigned char)(r_u8(0x800FF511u)));
        }
      }
      else
        if ((v70 == 1))
      {
        v71 = 0x800FF4F8u;
        v84 = ((unsigned char)(r_u8(0x800FF4F9u)));
      }
      else
      {
        if ((v70 != 2))
        {
          v84 = 0;
          LABEL_137:
          v71 = 0;

          goto LABEL_138;
        }
        v71 = 0x800FF508u;
        v84 = ((unsigned char)(r_u8(0x800FF509u)));
      }
      LABEL_138:
      if (v71)
      {
        v72 = sub_80066570(((unsigned char)(((sint8)(r_u8(v71))))));
        w_u32(0x800FF3ACu,0);
        v73 = v72;
        v74 = sub_80032DC0(132);
        if (v74)
          ((void)(v74),(void)(a1),(void)(((unsigned char)(((sint8)(r_u8((v71+(((uint32)(v73) + (uint32)(1)))*1u))))))),abort(),0u);
        w_u32(0x800FF3ACu,1);
      }

      if (r_u32(((uint32)(((uint32)(a1) + (uint32)(608))))))
      {
        v83 = 0;
        v82 = 0;
        xport_draft_host_sub_8007CD74_p13(&v85,a1,&v82);
        xport_draft_host_sub_80032EE4_p2(r_u32(((uint32)(((uint32)(a1) + (uint32)(608))))),&v85);
      }
    }
  }
  v75 = r_u16(((uint32)(((uint32)(a1) + (uint32)(390)))));
  if ((v75 == 8))
  {
    v76 = r_u8(((uint32)(((uint32)(a1) + (uint32)(26)))));
    if ((v76 == 8))
    {
      v77 = (r_u16(((uint32)(a1))) & 0xFFFE);
    }
    else
    {
      if ((!(r_u8(((uint32)(((uint32)(a1) + (uint32)(303)))))) || (v76 != 7)))
        goto LABEL_151;
      v77 = (r_u16(((uint32)(a1))) | 1);
    }
    w_u16(((uint32)(a1)),v77);
    LABEL_151:
    v75 = r_u16(((uint32)(((uint32)(a1) + (uint32)(390)))));

  }
  result = 2;
  if (((v75 == 256) && (r_u8(((uint32)(((uint32)(a1) + (uint32)(26))))) == 2)))
  {
    v79 = sub_8006325C(a1,2,3);
    if (sub_80062D24(a1,v79,1))
      return sub_80069EF4(121,((uint32)(a1) + (uint32)(4)),3);
    v80 = sub_8006325C(a1,2,17);
    result = sub_80062D24(a1,v80,2);
    if (result)
      return sub_80069EF4(121,((uint32)(a1) + (uint32)(4)),3);
  }
  return result;
}



uint32 sub_80066CF0(uint32 start_xy, uint32 unused_start_z, uint32 zero_output, uint32 output, uint32 end_xy, uint32 unused_end_z, uint32 scale)
{
  sint32 dx = (sint16)((uint16)end_xy - (uint16)start_xy);
  sint32 dy = (sint16)((uint16)(end_xy >> 16) - (uint16)(start_xy >> 16));
  sint32 result;
  (void)unused_start_z;
  (void)unused_end_z;
  if (dx < -2048) dx += 4096;
  if (dx > 2048) dx -= 4096;
  if (dy < -2048) dy += 4096;
  if (dy > 2048) dy -= 4096;
  if (!dx && !dy)
  {
    w_u16(zero_output,0);
    w_u16(zero_output + 2u,0);
    return 0u;
  }
  result = (sint32)(scale * (uint32)dx) >> 8;
  w_u16(output,result);
  result = (sint32)(scale * (uint32)dy) >> 8;
  w_u16(output + 2u,result);
  return (uint32)result;
}


uint32 sub_8006689C(uint32 a1, uint32 a2)
{
  sint32 v2;
  sint32 v4;
  v4 = (((uint32)(r_u32((a1+(1)*4u))) - (uint32)(r_u32((a2+(1)*4u)))) >> 12);
  v2 = (((uint32)(r_u32((a1+(2)*4u))) - (uint32)(r_u32((a2+(2)*4u)))) >> 12);
  return sub_80085B54(((uint32)(((uint32)(((uint32)((((uint32)(r_u32(a1)) - (uint32)(r_u32(a2))) >> 12)) * (uint32)((((uint32)(r_u32(a1)) - (uint32)(r_u32(a2))) >> 12)))) + (uint32)(((sint32)(((uint32)(v4) * (uint32)(v4))))))) + (uint32)(((sint32)(((uint32)(v2) * (uint32)(v2)))))));
}



uint32 sub_80032DC0(uint32 a1)
{
  sint32 v2;
  sint32 v3;
  uint32 v4;
  uint32 v5;
  uint32 i;
  if (r_u32(0x800FF3ACu))
    v2 = -1;
  else
    v2 = 0;
  v3 = sub_8006B864(a1,v2,1);
  v4 = ((uint32)(v3));
  v5 = (((uint32)(((uint32)(a1) + (uint32)(3)))) >> 2);
  for (i = 0; (i < v5); (v4+=4u))
  {
    w_u32(v4,0);
    ++i;
  }

  return v3;
}


/* TODO Missing call adapter sub_8006A890 */
/* TODO Missing call adapter sub_8006A8FC */
/* TODO Missing host buffer adapter xport_draft_host_sub_8006613C_p1 */
uint32 sub_8006CDD4(void)
{
  sint32 result;
  uint32 v1;
  sint32 v2;
  sint32 v3;
  sint32 v4;
  sint32 v5;
  sint32 v6;
  sint32 v7;
  sint32 v8;
  sint32 v9;
  sint32 v10;
  unsigned char v11;
  sint8 v12;
  result = r_u32(0x800FF33Cu);
  if (r_u32(0x800FF33Cu))
  {
    v1 = 0x800FF768u;
    if (((r_u32(0x800FF620u) != 0xFFFF) && (r_u16(r_u32(((uint32)(((uint32)(((uint32)(4) * (uint32)(r_u32(0x800FF620u)))) + (uint32)(r_u32(0x800FF624u))))))) == 8)))
      v1 = ((uint32)((((uint32)(xport_draft_host_sub_8006613C_p1(&v10,r_u32(0x800FF620u))))+(6)*1u)));
    ((void)(24),(void)(193),(void)(((sint32)(v1))),(void)(0x20u),(void)(32),(void)(114),(void)(1),(void)(v2),(void)(v10),(void)(v11),(void)(v12),abort(),0u);
    ((void)(23),(void)(205),(void)((((sint32)(((sint32)(r_u32(((uint32)(((uint32)(r_u32(0x800FF5A0u)) + (uint32)(4))))))))) >> 12)),(void)(0x20u),(void)(32),(void)(114),(void)(1),(void)(v3),(void)(v10),(void)(v11),(void)(v12),abort(),0u);
    ((void)(90),(void)(205),(void)((((sint32)(((sint32)(r_u32(((uint32)(((uint32)(r_u32(0x800FF5A0u)) + (uint32)(8))))))))) >> 12)),(void)(0x20u),(void)(32),(void)(114),(void)(1),(void)(v4),(void)(v10),(void)(v11),(void)(v12),abort(),0u);
    ((void)(152),(void)(205),(void)((((sint32)(((sint32)(r_u32(((uint32)(((uint32)(r_u32(0x800FF5A0u)) + (uint32)(12))))))))) >> 12)),(void)(0x20u),(void)(32),(void)(114),(void)(1),(void)(v5),(void)(v10),(void)(v11),(void)(v12),abort(),0u);
    ((void)(24),(void)(217),(void)((((uint32)(100) * (uint32)(r_u32(0x800FF34Cu))) / 110592)),(void)(0x20u),(void)(32),(void)(114),(void)(1),(void)(v6),(void)(v10),(void)(v11),(void)(v12),abort(),0u);
    ((void)(49),(void)(217),(void)((((uint32)(100) * (uint32)(r_u32((0x800FF748u+(0)*4u)))) / ((uint32)(((uint32)(r_u32(0x800E6C0Cu)) - (uint32)(r_u32(0x800E6C08u))))))),(void)(0x20u),(void)(32),(void)(114),(void)(1),(void)(v7),(void)(v10),(void)(v11),(void)(v12),abort(),0u);
    if (r_u32(0x800FF5A0u))
      ((void)(74),(void)(217),(void)(r_u32(((uint32)(((uint32)(r_u32(0x800FF5A0u)) + (uint32)(644)))))),(void)(0x20u),(void)(32),(void)(114),(void)(1),(void)(v8),(void)(v10),(void)(v11),(void)(v12),abort(),0u);
    ((void)(109),(void)(217),(void)(r_u32(0x800FF700u)),(void)(0x20u),(void)(32),(void)(114),(void)(1),(void)(v8),(void)(v10),(void)(v11),(void)(v12),abort(),0u);
    return ((void)(127),(void)(217),(void)(r_u32(0x800FF2B8u)),(void)(0x20u),(void)(32),(void)(114),(void)(1),(void)(v9),(void)(v10),(void)(v11),(void)(v12),abort(),0u);
  }
  return result;
}


/* TODO Postincrement memory expressions may require ordering refinement */
uint32 sub_8001B7C4(void)
{
  sint32 v0;
  sint32 i;
  uint32 v2;
  uint32 v3;
  sint32 v4;
  sint32 result;
  v0 = r_u32(0x800FF1ACu);
  if (r_u32(0x800FF1ACu))
  {
    for (i = r_u32(((uint32)(((uint32)(r_u32(0x800FF1ACu)) + (uint32)(12)))));; i = r_u32(((uint32)(((uint32)(i) + (uint32)(12))))))
    {
      v2 = r_u32(((uint32)(((uint32)(v0) + (uint32)(8)))));
      v3 = r_u16(((uint32)(((uint32)(v0) + (uint32)(4)))));
      if ((v3 < r_u16(v2)))
      {
        w_u16(((uint32)(((uint32)(v0) + (uint32)(4)))),((uint32)(v3) + (uint32)(1)));
      }
      else
      {
        v4 = ((sint8)(r_u8((((uint32)(v2))+(27)*1u))));
        if (!(r_u8((((uint32)(v2))+(27)*1u))))
        {
          sub_8001B694(v0,((sint32)(v2)));
          v0 = i;
          goto LABEL_10;
        }
        if ((v4 != 127))
        {
          w_u32(((uint32)(((uint32)(v0) + (uint32)(8)))),(v2+(((uint32)(14) * (uint32)(v4)))*2u));
          w_u16(((uint32)(((uint32)(v0) + (uint32)(4)))),0);
        }
      }
      v0 = i;
      LABEL_10:
      if (!i)
        break;

    }

  }
  result = ((uint32)(r_u32(0x800FF1C8u)) - (uint32)(1));
  if (r_u32(0x800FF1C8u))
  {
    (w_u32(0x800FF1C8u,(r_u32(0x800FF1C8u)-1u)),r_u32(0x800FF1C8u));
  }
  else
  {
    if (r_u32(0x800FF1CCu))
      result = ((uint32)(sub_80066570(16)) + (uint32)(16));
    else
      result = sub_80066570(24);
    w_u32(0x800FF1C8u,result);
    w_u32(0x800FF1CCu,0);
  }
  return result;
}



uint32 sub_8003032C(uint32 a1)
{
  sint32 result;
  sint32 v2;
  sint32 v3;
  sint32 v4;
  sint32 v5;
  result = r_u32(0x800FF5A0u);
  if ((a1 == ((uint32)(r_u32(0x800FF5A0u)))))
  {
    v2 = r_u32((a1+(47)*4u));
    v3 = r_u32((a1+(48)*4u));
    w_u32((a1+(147)*4u),r_u32((a1+(46)*4u)));
    w_u32((a1+(148)*4u),v2);
    w_u32((a1+(149)*4u),v3);
  }
  for (; a1; a1 = ((uint32)(r_u32((a1+(7)*4u)))))
  {
    v4 = r_u32((a1+(2)*4u));
    v5 = r_u32((a1+(3)*4u));
    w_u32((a1+(60)*4u),r_u32((a1+(1)*4u)));
    w_u32((a1+(61)*4u),v4);
    w_u32((a1+(62)*4u),v5);
  }

  return result;
}


/* TODO Missing call adapter SLOWORD */
/* TODO Missing call adapter sub_80078310 */
/* TODO Missing call adapter sub_80079DC8 */
/* TODO Missing call adapter sub_8007A1A8 */
/* TODO Missing host buffer adapter xport_draft_host_sub_80075F80_p12 */
/* TODO Missing host buffer adapter xport_draft_host_sub_80075F80_p123 */
/* TODO Missing host buffer adapter xport_draft_host_sub_80076274_p1 */
/* TODO Missing host buffer adapter xport_draft_host_sub_800762A8_p1 */
/* TODO Missing host buffer adapter xport_draft_host_sub_800762DC_p1 */
/* TODO Postincrement memory expressions may require ordering refinement */
uint32 sub_8007876C(uint32 a1)
{
  sint32 result;
  sint32 v3;
  sint32 v4;
  sint32 v5;
  sint32 v6;
  uint32 v7;
  sint32 v8;
  sint32 v9;
  uint32 v10;
  sint32 v11;
  sint32 v12;
  short v13;
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
  short v27;
  short v28;
  short v29;
  short v30;
  sint32 v31;
  short v32;
  sint32 v33;
  uint32 v34;
  uint32 v35;
  sint32 v36;
  sint32 v37;
  sint32 v38;
  sint32 v39;
  sint32 v40;
  sint32 v41;
  sint32 v42;
  sint32 v43;
  short v44;
  short v45;
  short v46;
  short v47;
  short v48;
  short v49;
  sint32 v50;
  sint32 v51;
  sint32 v52;
  sint32 v53;
  sint32 v54;
  sint32 v55;
  sint32 v56;
  int v57[4];
  int v58[4];
  int v59[4];
  uint32 v60[4];
  uint32 v61[4];
  result = r_u32(0x800FF008u);
  if (r_u32(0x800FF008u))
    return result;
  v3 = r_u32(((uint32)(((uint32)(a1) + (uint32)(8)))));
  v4 = r_u32(((uint32)(((uint32)(a1) + (uint32)(12)))));
  w_u32(0x801028C8u,r_u32(((uint32)(((uint32)(a1) + (uint32)(4))))));
  w_u32(0x801028CCu,v3);
  w_u32(0x801028D0u,v4);
  v5 = r_u32(((uint32)(((uint32)(a1) + (uint32)(332)))));
  (w_u32(((uint32)(((uint32)(a1) + (uint32)(576)))),(r_u32(((uint32)(((uint32)(a1) + (uint32)(576)))))+1u)),r_u32(((uint32)(((uint32)(a1) + (uint32)(576))))));
  if (v5)
  {
    sub_8006C0B8(((uint32)(((uint32)(a1) + (uint32)(308)))),((uint32)(((uint32)(a1) + (uint32)(320)))));
    (w_u32(((uint32)(((uint32)(a1) + (uint32)(332)))),(r_u32(((uint32)(((uint32)(a1) + (uint32)(332)))))-1u)),r_u32(((uint32)(((uint32)(a1) + (uint32)(332))))));
  }
  else
  {
    v6 = r_u32(((uint32)(((uint32)(a1) + (uint32)(296)))));
    if ((v6 && !sub_80062F48(v6)))
    {
      v7 = r_u32(((uint32)(((uint32)(a1) + (uint32)(296)))));
      v8 = r_u32((v7+(2)*4u));
      v9 = r_u32((v7+(3)*4u));
      w_u32(((uint32)(((uint32)(a1) + (uint32)(308)))),r_u32((v7+(1)*4u)));
      w_u32(((uint32)(((uint32)(a1) + (uint32)(312)))),v8);
      w_u32(((uint32)(((uint32)(a1) + (uint32)(316)))),v9);
    }
  }
  if (r_u32(((uint32)(((uint32)(a1) + (uint32)(368))))))
  {
    sub_8006C0B8(((uint32)(((uint32)(a1) + (uint32)(344)))),((uint32)(((uint32)(a1) + (uint32)(356)))));
    (w_u32(((uint32)(((uint32)(a1) + (uint32)(368)))),(r_u32(((uint32)(((uint32)(a1) + (uint32)(368)))))-1u)),r_u32(((uint32)(((uint32)(a1) + (uint32)(368))))));
  }
  else
  {
    v10 = r_u32(((uint32)(((uint32)(a1) + (uint32)(336)))));
    v11 = r_u32((v10+(2)*4u));
    v12 = r_u32((v10+(3)*4u));
    w_u32(((uint32)(((uint32)(a1) + (uint32)(344)))),r_u32((v10+(1)*4u)));
    w_u32(((uint32)(((uint32)(a1) + (uint32)(348)))),v11);
    w_u32(((uint32)(((uint32)(a1) + (uint32)(352)))),v12);
  }
  if (r_u16(((uint32)(((uint32)(a1) + (uint32)(376))))))
  {
    v13 = ((uint32)(r_u16(((uint32)(((uint32)(a1) + (uint32)(376)))))) - (uint32)(1));
    w_u32(((uint32)(((uint32)(a1) + (uint32)(372)))),(r_u32(((uint32)(((uint32)(a1) + (uint32)(372)))))+(r_u32(((uint32)(((uint32)(a1) + (uint32)(380))))))));
    w_u16(((uint32)(((uint32)(a1) + (uint32)(376)))),v13);
  }
  if (r_u32(((uint32)(((uint32)(a1) + (uint32)(384))))))
    ((void)(((uint32)(a1))),abort(),0u);
  if (r_u8(0x800EC1E8u))
  {
    if (r_u32(0x800FF944u))
      w_u32(0x800FF940u,1);
    w_u32(0x800FF944u,0);
  }
  else
  {
    w_u32(0x800FF944u,1);
  }
  if ((((!r_u8(0x800EC2D5u) || !r_u8(0x800EC2E5u)) || !r_u8(0x800EC2D4u)) || ((((unsigned char)(r_u8(0x800EC1E8u))) & ((unsigned char)((r_u8(0x800EC2E4u) & r_u8((0x800EC0F8u+(0)*1u)))))) == 0)))
  {
    goto LABEL_25;
  }
  w_u8(0x800EC2D5u,0);
  w_u8(0x800EC2E5u,0);
  w_u32(0x800FF934u,(r_u32(0x800FF934u)^(1u)));
  w_u32(0x800FF300u,r_u32(0x800FF934u));
  if (r_u32(0x800FF934u))
  {
    v14 = ((sint16)(r_u16(((uint32)(((uint32)(a1) + (uint32)(494)))))));
    v15 = r_u32(((uint32)(((uint32)(a1) + (uint32)(372)))));
    w_u32(0x800FFCA0u,r_u32(0x800FF918u));
    w_u32(0x800FFCA4u,r_u32(0x800FF920u));
    w_u32(0x800FFCA8u,r_u32(0x800FF928u));
    w_u32(0x800FFCACu,r_u32(0x800FF908u));
    w_u32(0x800FFCB0u,r_u32(0x800FF910u));
    w_u32(0x800FFCB4u,v14);
    w_u32(0x800FF93Cu,v15);
    LABEL_25:
    if (r_u32(0x800FF934u))
    {
      if (r_u8(0x800EC354u))
      {
        v16 = r_u32(0x800FF93Cu);
        w_u32(0x800FF918u,r_u32(0x800FFCA0u));
        w_u32(0x800FF920u,r_u32(0x800FFCA4u));
        w_u32(0x800FF928u,r_u32(0x800FFCA8u));
        w_u32(0x800FF908u,r_u32(0x800FFCACu));
        w_u32(0x800FF910u,r_u32(0x800FFCB0u));
        w_u16(((uint32)(((uint32)(a1) + (uint32)(494)))),r_u32(0x800FFCB4u));
        w_u32(((uint32)(((uint32)(a1) + (uint32)(372)))),v16);
      }
      if (r_u8(0x800EC108u))
        (w_u32(((uint32)(((uint32)(a1) + (uint32)(372)))),(r_u32(((uint32)(((uint32)(a1) + (uint32)(372)))))-1u)),r_u32(((uint32)(((uint32)(a1) + (uint32)(372))))));
      if (r_u8(0x800EC118u))
        (w_u32(((uint32)(((uint32)(a1) + (uint32)(372)))),(r_u32(((uint32)(((uint32)(a1) + (uint32)(372)))))+1u)),r_u32(((uint32)(((uint32)(a1) + (uint32)(372))))));
      if (r_u8((0x800EC0F8u+(0)*1u)))
        w_u32(((uint32)(((uint32)(a1) + (uint32)(372)))),256);
      if (r_u8(0x800EC2F4u))
      {
        if (r_u8(0x800EC364u))
          v17 = ((uint32)(r_u32(0x800FF918u)) - (uint32)(32));
        else
          v17 = ((uint32)(r_u32(0x800FF918u)) - (uint32)(8));
        w_u32(0x800FF918u,v17);
      }
      if (r_u8(0x800EC304u))
      {
        if (r_u8(0x800EC364u))
          v18 = ((uint32)(r_u32(0x800FF918u)) + (uint32)(32));
        else
          v18 = ((uint32)(r_u32(0x800FF918u)) + (uint32)(8));
        w_u32(0x800FF918u,v18);
      }
      if (r_u8(0x800EC314u))
      {
        if (r_u8(0x800EC364u))
          v19 = ((uint32)(r_u32(0x800FF928u)) + (uint32)(32));
        else
          v19 = ((uint32)(r_u32(0x800FF928u)) + (uint32)(8));
        w_u32(0x800FF928u,v19);
      }
      if (r_u8(0x800EC324u))
      {
        if (r_u8(0x800EC364u))
          v20 = ((uint32)(r_u32(0x800FF928u)) - (uint32)(32));
        else
          v20 = ((uint32)(r_u32(0x800FF928u)) - (uint32)(8));
        w_u32(0x800FF928u,v20);
      }
      if (r_u8(0x800EC2B4u))
      {
        if (r_u8(0x800EC364u))
          v21 = ((uint32)(r_u32(0x800FF920u)) - (uint32)(32));
        else
          v21 = ((uint32)(r_u32(0x800FF920u)) - (uint32)(8));
        w_u32(0x800FF920u,v21);
      }
      if (r_u8(0x800EC2C4u))
      {
        if (r_u8(0x800EC364u))
          v22 = ((uint32)(r_u32(0x800FF920u)) + (uint32)(32));
        else
          v22 = ((uint32)(r_u32(0x800FF920u)) + (uint32)(8));
        w_u32(0x800FF920u,v22);
      }
      if (r_u8(0x800EC2D4u))
      {
        if (r_u8(0x800EC364u))
          v23 = ((uint32)(r_u32(0x800FF910u)) - (uint32)(32));
        else
          v23 = ((uint32)(r_u32(0x800FF910u)) - (uint32)(8));
        w_u32(0x800FF910u,v23);
      }
      if (r_u8(0x800EC2E4u))
      {
        if (r_u8(0x800EC364u))
          v24 = ((uint32)(r_u32(0x800FF910u)) + (uint32)(32));
        else
          v24 = ((uint32)(r_u32(0x800FF910u)) + (uint32)(8));
        w_u32(0x800FF910u,v24);
      }
      if (r_u8(0x800EC274u))
      {
        if (r_u8(0x800EC364u))
          v25 = ((uint32)(r_u32(0x800FF908u)) - (uint32)(32));
        else
          v25 = ((uint32)(r_u32(0x800FF908u)) - (uint32)(8));
        w_u32(0x800FF908u,v25);
      }
      if (r_u8(0x800EC2A4u))
      {
        if (r_u8(0x800EC364u))
          v26 = ((uint32)(r_u32(0x800FF908u)) + (uint32)(32));
        else
          v26 = ((uint32)(r_u32(0x800FF908u)) + (uint32)(8));
        w_u32(0x800FF908u,v26);
      }
      if (r_u8(0x800EC284u))
      {
        v27 = r_u16(((uint32)(((uint32)(a1) + (uint32)(494)))));
        v28 = ((uint32)(v27) + (uint32)(32));
        if (!r_u8(0x800EC364u))
          v28 = ((uint32)(v27) + (uint32)(8));
        w_u16(((uint32)(((uint32)(a1) + (uint32)(494)))),(v28 & 0xFFF));
      }
      if (r_u8(0x800EC294u))
      {
        v29 = r_u16(((uint32)(((uint32)(a1) + (uint32)(494)))));
        v30 = ((uint32)(v29) - (uint32)(32));
        if (!r_u8(0x800EC364u))
          v30 = ((uint32)(v29) - (uint32)(8));
        w_u16(((uint32)(((uint32)(a1) + (uint32)(494)))),(v30 & 0xFFF));
      }
    }

  }
  if (r_u32(0x800FF90Cu))
  {
    (w_u32(0x800FF90Cu,(r_u32(0x800FF90Cu)-1u)),r_u32(0x800FF90Cu));
    w_u32(0x800FF908u,(r_u32(0x800FF908u)+(r_u32(0x800FFD0Cu))));
  }
  if (r_u32(0x800FF914u))
  {
    (w_u32(0x800FF914u,(r_u32(0x800FF914u)-1u)),r_u32(0x800FF914u));
    w_u32(0x800FF910u,(r_u32(0x800FF910u)+(r_u32(0x800FFD10u))));
  }
  v31 = ((uint32)(r_u32(0x800FFD18u)) - (uint32)(1));
  if (r_u32(0x800FFD18u))
  {
    (w_u32(0x800FFD18u,(r_u32(0x800FFD18u)-1u)),r_u32(0x800FFD18u));
    if (v31)
      v32 = (((uint32)(r_u16(((uint32)(((uint32)(a1) + (uint32)(494)))))) + (uint32)(r_u32(0x800FFD14u))) & 0xFFF);
    else
      v32 = r_u32(0x800FFD1Cu);
    w_u16(((uint32)(((uint32)(a1) + (uint32)(494)))),v32);
  }
  if (r_u32(0x800FF91Cu))
  {
    (w_u32(0x800FF91Cu,(r_u32(0x800FF91Cu)-1u)),r_u32(0x800FF91Cu));
    w_u32(0x800FF918u,(r_u32(0x800FF918u)+(r_u32(0x800FFD20u))));
  }
  if (r_u32(0x800FF924u))
  {
    (w_u32(0x800FF924u,(r_u32(0x800FF924u)-1u)),r_u32(0x800FF924u));
    w_u32(0x800FF920u,(r_u32(0x800FF920u)+(r_u32(0x800FFD24u))));
  }
  if (r_u32(0x800FF92Cu))
  {
    (w_u32(0x800FF92Cu,(r_u32(0x800FF92Cu)-1u)),r_u32(0x800FF92Cu));
    w_u32(0x800FF928u,(r_u32(0x800FF928u)+(r_u32(0x800FFD28u))));
  }
  w_u32(0x800FF8FCu,SquareRoot0(((uint32)(((uint32)(r_u32(0x800FF908u)) * (uint32)(r_u32(0x800FF908u)))) + (uint32)(((uint32)(r_u32(0x800FF910u)) * (uint32)(r_u32(0x800FF910u)))))));
  v33 = ratan2(r_u32(0x800FF910u),r_u32(0x800FF908u));
  w_u32(0x800ED514u,((uint32)(r_u32(0x800FF920u)) << (uint32)(12)));
  v34 = (0x800F863Cu+((r_u16(((uint32)(((uint32)(a1) + (uint32)(494))))) & 0xFFF))*4u);
  w_u32(0x800ED510u,0u - (((uint32)(((uint32)(r_u32(0x800FF918u)) * (uint32)(((sint16)(r_u16((((uint32)(v34))+(1)*2u))))))) + (uint32)(((uint32)(r_u32(0x800FF928u)) * (uint32)(((sint16)(r_u16(((uint32)(v34)))))))))));
  v35 = (0x800F863Cu+((r_u16(((uint32)(((uint32)(a1) + (uint32)(494))))) & 0xFFF))*4u);
  v36 = ((uint32)(r_u32(0x800FF918u)) * (uint32)(((sint16)(r_u16(((uint32)(v35)))))));
  v37 = ((uint32)(r_u32(0x800FF928u)) * (uint32)(((sint16)(r_u16((((uint32)(v35))+(1)*2u))))));
  w_u32(0x800FF8F8u,-v33);
  w_u32(0x800ED518u,((sint32)(((uint32)(v36) - (uint32)(v37)))));
  sub_80066B8C(((uint32)(((uint32)(a1) + (uint32)(16)))),((uint32)(((uint32)(a1) + (uint32)(4)))),((uint32)(((uint32)(a1) + (uint32)(344)))));
  v38 = r_u32(((uint32)(((uint32)(a1) + (uint32)(296)))));
  if (((r_u16(((uint32)(((uint32)(v38) + (uint32)(570))))) && r_u16(((uint32)(((uint32)(a1) + (uint32)(414)))))) && !(r_u32(((uint32)(((uint32)(a1) + (uint32)(332))))))))
  {
    if ((((sint32)(((sint32)(r_u32(((uint32)(((uint32)(v38) + (uint32)(108))))))))) >= 0))
      v39 = ((uint32)(r_u32(0x800FF94Cu)) - (uint32)(1));
    else
      v39 = ((uint32)(r_u32(0x800FF94Cu)) + (uint32)(1));
    w_u32(0x800FF94Cu,v39);
    v40 = ((uint32)(r_u32(0x800FF948u)) + (uint32)(((uint32)(((0u - r_u32(((uint32)(((uint32)(r_u32(((uint32)(((uint32)(a1) + (uint32)(296)))))) + (uint32)(108)))))) >> 12)) * (uint32)(((sint16)(r_u16(((uint32)(((uint32)(a1) + (uint32)(414)))))))))));
    w_u32(0x800FF948u,v40);
    if ((((sint32)(v39)) > 0))
      w_u32(((uint32)(((uint32)(a1) + (uint32)(312)))),(r_u32(((uint32)(((uint32)(a1) + (uint32)(312)))))+(v40)));
  }
  else
  {
    w_u32(0x800FF948u,0);
    w_u32(0x800FF94Cu,0);
  }
  if (r_u16(((uint32)(((uint32)(a1) + (uint32)(412))))))
  {
    if ((((sint32)(((sint32)(r_u32(((uint32)(((uint32)(r_u32(((uint32)(((uint32)(a1) + (uint32)(296)))))) + (uint32)(108))))))))) < 0))
      w_u32(0x800FF8F8u,(r_u32(0x800FF8F8u)-(((sint16)(r_u16(((uint32)(((uint32)(a1) + (uint32)(412))))))))));
    if ((((sint32)(((sint32)(r_u32(((uint32)(((uint32)(r_u32(((uint32)(((uint32)(a1) + (uint32)(296)))))) + (uint32)(108))))))))) > 0))
      w_u32(0x800FF8F8u,(r_u32(0x800FF8F8u)-((((uint32)(r_u16(((uint32)(((uint32)(a1) + (uint32)(412)))))) << (uint32)(16)) >> 17))));
    w_u32(0x800FF8F8u,(r_u32(0x800FF8F8u)&(0xFFFu)));
  }
  switch (r_u32(((uint32)(((uint32)(a1) + (uint32)(564))))))
  {
    case 2:

    case 3:

    case 7:

    case 0xF:
      w_u32(0x800FF930u,0);
      sub_800793C8(a1);
      break;

    case 0x12:

    case 0x13:
      w_u32(0x800FF930u,1);
      ((void)(a1),abort(),0u);
      break;

    case 0x14:
      w_u32(0x800FF930u,1);
      ((void)(a1),abort(),0u);
      break;

    default:
      break;

  }

  sub_80077748(a1);
  sub_80066B8C(((uint32)(((uint32)(a1) + (uint32)(16)))),((uint32)(((uint32)(a1) + (uint32)(4)))),((uint32)(((uint32)(a1) + (uint32)(344)))));
  if ((r_u32(((uint32)(((uint32)(a1) + (uint32)(548))))) || r_u16(((uint32)(((uint32)(a1) + (uint32)(552)))))))
  {
    xport_draft_host_sub_800762A8_p1(v59,((short)((((uint32)(((sint16)(r_u16(((uint32)(((uint32)(a1) + (uint32)(550)))))))) * (uint32)(((void)(r_u32((0x800F863Cu+((((uint32)(r_u16(((uint32)(((uint32)(a1) + (uint32)(560)))))) * (uint32)(((uint16)(r_u32(0x800FF2F0u))))) & 0xFFF))*4u))),abort(),0u))) / 4096))));
    xport_draft_host_sub_80075F80_p12(v58,v59,((uint32)(((uint32)(a1) + (uint32)(428)))));
    xport_draft_host_sub_80076274_p1(v60,((short)((((uint32)(((sint16)(r_u16(((uint32)(((uint32)(a1) + (uint32)(548)))))))) * (uint32)(((void)(r_u32((0x800F863Cu+((((uint32)(r_u16(((uint32)(((uint32)(a1) + (uint32)(558)))))) * (uint32)(((uint16)(r_u32(0x800FF2F0u))))) & 0xFFF))*4u))),abort(),0u))) / 4096))));
    xport_draft_host_sub_80075F80_p123(v57,v58,v60);
    xport_draft_host_sub_800762DC_p1(v61,((short)((((uint32)(((sint16)(r_u16(((uint32)(((uint32)(a1) + (uint32)(552)))))))) * (uint32)(((void)(r_u32((0x800F863Cu+((((uint32)(r_u16(((uint32)(((uint32)(a1) + (uint32)(562)))))) * (uint32)(((uint16)(r_u32(0x800FF2F0u))))) & 0xFFF))*4u))),abort(),0u))) / 4096))));
    xport_draft_host_sub_80075F80_p123(&v53,v57,v61);
    v41 = v54;
    v42 = v55;
    v43 = v56;
    w_u32(((uint32)(((uint32)(a1) + (uint32)(428)))),v53);
    w_u32(((uint32)(((uint32)(a1) + (uint32)(432)))),v41);
    w_u32(((uint32)(((uint32)(a1) + (uint32)(436)))),v42);
    w_u32(((uint32)(((uint32)(a1) + (uint32)(440)))),v43);
    v41 = ((v41 & 0xFFFF0000u)|(((r_u16(((uint32)(((uint32)(a1) + (uint32)(552)))))) & 0xFFFFu)<<0));
    v53 = r_u32(((uint32)(((uint32)(a1) + (uint32)(548)))));
    v54 = ((v54 & 0xFFFF0000u)|(((v41) & 0xFFFFu)<<0));
    v44 = r_u16(((uint32)(((uint32)(a1) + (uint32)(548)))));
    if ((((sint32)(v44)) >= 0))
      v45 = ((uint32)(v44) - (uint32)(r_u8(((uint32)(((uint32)(a1) + (uint32)(554)))))));
    else
      v45 = ((uint32)(v44) + (uint32)(r_u8(((uint32)(((uint32)(a1) + (uint32)(554)))))));
    w_u16(((uint32)(((uint32)(a1) + (uint32)(548)))),v45);
    v46 = r_u16(((uint32)(((uint32)(a1) + (uint32)(550)))));
    if ((((sint32)(v46)) >= 0))
      v47 = ((uint32)(v46) - (uint32)(r_u8(((uint32)(((uint32)(a1) + (uint32)(555)))))));
    else
      v47 = ((uint32)(v46) + (uint32)(r_u8(((uint32)(((uint32)(a1) + (uint32)(555)))))));
    w_u16(((uint32)(((uint32)(a1) + (uint32)(550)))),v47);
    v48 = r_u16(((uint32)(((uint32)(a1) + (uint32)(552)))));
    if ((((sint32)(v48)) >= 0))
      v49 = ((uint32)(v48) - (uint32)(r_u8(((uint32)(((uint32)(a1) + (uint32)(556)))))));
    else
      v49 = ((uint32)(v48) + (uint32)(r_u8(((uint32)(((uint32)(a1) + (uint32)(556)))))));
    w_u16(((uint32)(((uint32)(a1) + (uint32)(552)))),v49);
    if ((((((unsigned short)(v53)) ^ r_u16(((uint32)(((uint32)(a1) + (uint32)(548)))))) & 0x8000) != 0))
      w_u16(((uint32)(((uint32)(a1) + (uint32)(548)))),0);
    if ((((((v53>>16)&65535u) ^ r_u16(((uint32)(((uint32)(a1) + (uint32)(550)))))) & 0x8000) != 0))
      w_u16(((uint32)(((uint32)(a1) + (uint32)(550)))),0);
    if ((((((unsigned short)(v54)) ^ r_u16(((uint32)(((uint32)(a1) + (uint32)(552)))))) & 0x8000) != 0))
      w_u16(((uint32)(((uint32)(a1) + (uint32)(552)))),0);
  }
  sub_80077680(((uint32)(a1)));
  result = r_u32(0x800FF930u);
  if (r_u32(0x800FF930u))
  {
    v50 = r_u32(((uint32)(((uint32)(a1) + (uint32)(480)))));
    v51 = r_u32(((uint32)(((uint32)(a1) + (uint32)(484)))));
    v52 = r_u32(((uint32)(((uint32)(a1) + (uint32)(488)))));
    w_u32(((uint32)(((uint32)(a1) + (uint32)(428)))),r_u32(((uint32)(((uint32)(a1) + (uint32)(476))))));
    w_u32(((uint32)(((uint32)(a1) + (uint32)(432)))),v50);
    w_u32(((uint32)(((uint32)(a1) + (uint32)(436)))),v51);
    w_u32(((uint32)(((uint32)(a1) + (uint32)(440)))),v52);
  }
  return result;
}


/* TODO Missing call adapter SHIWORD */
/* TODO Missing call adapter SLOWORD */
/* TODO Missing call adapter xport_draft_missing_gte_adapter */
/* TODO Missing host buffer adapter xport_draft_host_sub_8006C34C_p12 */
/* TODO Missing host buffer adapter xport_draft_host_sub_80076420_p1 */
uint32 sub_800793C8(uint32 a1)
{
  uint32 xport_draft_asm_carrier; /* TODO GTE carrier */
  uint32 v2;
  sint32 v3;
  uint32 v4;
  uint32 v5;
  sint32 v6;
  uint32 v7;
  sint32 v8;
  uint32 v9;
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
  sint32 v28;
  sint32 v29;
  sint32 v30;
  sint32 v31;
  sint32 v32;
  sint32 v33;
  sint32 v34;
  sint32 v35;
  sint32 v36;
  short v37;
  sint32 v38;
  sint32 v39;
  sint32 v40;
  int v41[4];
  sint32 v42;
  sint32 v43;
  sint32 v44;
  sint32 v45;
  sint32 v46;
  sint32 v47;
  sint32 v48;
  sint32 v49;
  sint32 v50;
  short v51[16];
  v36 = r_u32(((uint32)(((uint32)(a1) + (uint32)(492)))));
  v37 = r_u16(((uint32)(((uint32)(a1) + (uint32)(496)))));
  v2 = (0x800F863Cu+((r_u32(0x800FF8F8u) & 0xFFF))*4u);
  v38 = 0;
  v3 = (((uint32)(r_u32(0x800FF8FCu)) << (uint32)(12)) >> 12);
  v39 = -(((sint32)(((uint32)(v3) * (uint32)(((sint16)(r_u16(((uint32)(v2))))))))));
  v40 = ((sint32)(((uint32)(v3) * (uint32)(((sint16)(r_u16((((uint32)(v2))+(1)*2u))))))));
  v4 = (0x800F863Cu+((((v36>>16)&65535u) & 0xFFF))*4u);
  v38 = ((uint32)((((sint32)(v40)) >> 12)) * (uint32)(((sint16)(r_u16(((uint32)(v4)))))));
  v40 = ((uint32)((((sint32)(v40)) >> 12)) * (uint32)(((sint16)(r_u16((((uint32)(v4))+(1)*2u))))));
  v5 = (0x800F863Cu+((v36 & 0xFFF))*4u);
  v6 = (((sint32)(v39)) >> 12);
  v39 = ((uint32)(((uint32)((((sint32)(v39)) >> 12)) * (uint32)(((sint16)(r_u16((((uint32)(v5))+(1)*2u))))))) - (uint32)(((uint32)((((sint32)(v40)) >> 12)) * (uint32)(((sint16)(r_u16(((uint32)(v5)))))))));
  v40 = ((uint32)(((uint32)((((sint32)(v40)) >> 12)) * (uint32)(((sint16)(r_u16((((uint32)(v5))+(1)*2u))))))) + (uint32)(((sint32)(((uint32)(v6) * (uint32)(((sint16)(r_u16(((uint32)(v5)))))))))));
  v7 = (0x800F863Cu+((v37 & 0xFFF))*4u);
  v8 = (((sint32)(v38)) >> 12);
  v38 = ((uint32)(((uint32)((((sint32)(v38)) >> 12)) * (uint32)(((sint16)(r_u16((((uint32)(v7))+(1)*2u))))))) - (uint32)(((uint32)((((sint32)(v39)) >> 12)) * (uint32)(((sint16)(r_u16(((uint32)(v7)))))))));
  v9 = (0x800F863Cu+((r_u32(0x800FF8F8u) & 0xFFF))*4u);
  v39 = ((uint32)(((uint32)((((sint32)(v39)) >> 12)) * (uint32)(((sint16)(r_u16((((uint32)(v7))+(1)*2u))))))) + (uint32)(((sint32)(((uint32)(v8) * (uint32)(((sint16)(r_u16(((uint32)(v7)))))))))));
  w_u32(((uint32)(((uint32)(a1) + (uint32)(416)))),0);
  w_u32(((uint32)(((uint32)(a1) + (uint32)(420)))),((uint32)(500) * (uint32)(((sint16)(r_u16((((uint32)(v9))+(1)*2u)))))));
  w_u32(((uint32)(((uint32)(a1) + (uint32)(424)))),((uint32)(500) * (uint32)(((sint16)(r_u16(((uint32)(v9))))))));
  v28 = r_u32(((uint32)(((uint32)(a1) + (uint32)(416)))));
  v30 = r_u32(((uint32)(((uint32)(a1) + (uint32)(420)))));
  v33 = r_u32(((uint32)(((uint32)(a1) + (uint32)(424)))));
  w_u32(((uint32)(((uint32)(a1) + (uint32)(416)))),((uint32)(((uint32)((((sint32)(v28)) >> 12)) * (uint32)(((void)(r_u32((0x800F863Cu+((((v36>>16)&65535u) & 0xFFF))*4u))),abort(),0u)))) + (uint32)(((uint32)((((sint32)(v33)) >> 12)) * (uint32)(((void)(r_u32((0x800F863Cu+((((v36>>16)&65535u) & 0xFFF))*4u))),abort(),0u))))));
  w_u32(((uint32)(((uint32)(a1) + (uint32)(420)))),v30);
  w_u32(((uint32)(((uint32)(a1) + (uint32)(424)))),((uint32)(((uint32)((((sint32)(v33)) >> 12)) * (uint32)(((void)(r_u32((0x800F863Cu+((((v36>>16)&65535u) & 0xFFF))*4u))),abort(),0u)))) - (uint32)(((uint32)((((sint32)(v28)) >> 12)) * (uint32)(((void)(r_u32((0x800F863Cu+((((v36>>16)&65535u) & 0xFFF))*4u))),abort(),0u))))));
  v31 = r_u32(((uint32)(((uint32)(a1) + (uint32)(420)))));
  v34 = r_u32(((uint32)(((uint32)(a1) + (uint32)(424)))));
  w_u32(((uint32)(((uint32)(a1) + (uint32)(416)))),r_u32(((uint32)(((uint32)(a1) + (uint32)(416))))));
  w_u32(((uint32)(((uint32)(a1) + (uint32)(420)))),((uint32)(((uint32)((((sint32)(v31)) >> 12)) * (uint32)(((void)(r_u32((0x800F863Cu+((v36 & 0xFFF))*4u))),abort(),0u)))) - (uint32)(((uint32)((((sint32)(v34)) >> 12)) * (uint32)(((void)(r_u32((0x800F863Cu+((v36 & 0xFFF))*4u))),abort(),0u))))));
  w_u32(((uint32)(((uint32)(a1) + (uint32)(424)))),((uint32)(((uint32)((((sint32)(v34)) >> 12)) * (uint32)(((void)(r_u32((0x800F863Cu+((v36 & 0xFFF))*4u))),abort(),0u)))) + (uint32)(((uint32)((((sint32)(v31)) >> 12)) * (uint32)(((void)(r_u32((0x800F863Cu+((v36 & 0xFFF))*4u))),abort(),0u))))));
  v29 = r_u32(((uint32)(((uint32)(a1) + (uint32)(416)))));
  v32 = r_u32(((uint32)(((uint32)(a1) + (uint32)(420)))));
  v35 = r_u32(((uint32)(((uint32)(a1) + (uint32)(424)))));
  w_u32(((uint32)(((uint32)(a1) + (uint32)(416)))),((uint32)(((uint32)((((sint32)(v29)) >> 12)) * (uint32)(((sint16)(r_u16((((uint32)(v7))+(1)*2u))))))) - (uint32)(((uint32)((((sint32)(v32)) >> 12)) * (uint32)(((sint16)(r_u16(((uint32)(v7))))))))));
  w_u32(((uint32)(((uint32)(a1) + (uint32)(420)))),((uint32)(((uint32)((((sint32)(v32)) >> 12)) * (uint32)(((sint16)(r_u16((((uint32)(v7))+(1)*2u))))))) + (uint32)(((uint32)((((sint32)(v29)) >> 12)) * (uint32)(((sint16)(r_u16(((uint32)(v7))))))))));
  w_u32(((uint32)(((uint32)(a1) + (uint32)(424)))),v35);
  xport_draft_host_sub_8006C34C_p12(v41,&v38,((uint32)(((uint32)(a1) + (uint32)(308)))));
  v10 = v41[1];
  v11 = v41[2];
  w_u32(((uint32)(((uint32)(a1) + (uint32)(512)))),v41[0]);
  w_u32(((uint32)(((uint32)(a1) + (uint32)(516)))),v10);
  w_u32(((uint32)(((uint32)(a1) + (uint32)(520)))),v11);
  v12 = r_u32(((uint32)(((uint32)(a1) + (uint32)(312)))));
  v13 = r_u32(((uint32)(((uint32)(a1) + (uint32)(316)))));
  w_u32(((uint32)(((uint32)(a1) + (uint32)(524)))),r_u32(((uint32)(((uint32)(a1) + (uint32)(308))))));
  w_u32(((uint32)(((uint32)(a1) + (uint32)(528)))),v12);
  w_u32(((uint32)(((uint32)(a1) + (uint32)(532)))),v13);
  v14 = ((uint32)((((sint32)(((sint32)(r_u32(((uint32)(((uint32)(a1) + (uint32)(524))))))))) >> 12)) - (uint32)((((sint32)(((sint32)(r_u32(((uint32)(((uint32)(a1) + (uint32)(512))))))))) >> 12)));
  v15 = ((uint32)((((sint32)(((sint32)(r_u32(((uint32)(((uint32)(a1) + (uint32)(528))))))))) >> 12)) - (uint32)((((sint32)(((sint32)(r_u32(((uint32)(((uint32)(a1) + (uint32)(516))))))))) >> 12)));
  v16 = ((uint32)((((sint32)(((sint32)(r_u32(((uint32)(((uint32)(a1) + (uint32)(532))))))))) >> 12)) - (uint32)((((sint32)(((sint32)(r_u32(((uint32)(((uint32)(a1) + (uint32)(520))))))))) >> 12)));
  v17 = SquareRoot0(((uint32)(((uint32)(((sint32)(((uint32)(v14) * (uint32)(v14))))) + (uint32)(((sint32)(((uint32)(v15) * (uint32)(v15))))))) + (uint32)(((sint32)(((uint32)(v16) * (uint32)(v16)))))));
  if (!v17)
    v17 = 1;
  v48 = (((uint32)(v14) << (uint32)(12)) / ((sint32)(v17)));
  v49 = (((uint32)(v15) << (uint32)(12)) / ((sint32)(v17)));
  v50 = (((uint32)(v16) << (uint32)(12)) / ((sint32)(v17)));
  v18 = (((sint32)(((sint32)(r_u32(((uint32)(((uint32)(a1) + (uint32)(416))))))))) >> 12);
  v19 = (((sint32)(((sint32)(r_u32(((uint32)(((uint32)(a1) + (uint32)(420))))))))) >> 12);
  v20 = (((sint32)(((sint32)(r_u32(((uint32)(((uint32)(a1) + (uint32)(424))))))))) >> 12);
  v21 = SquareRoot0(((uint32)(((uint32)(((sint32)(((uint32)(v18) * (uint32)(v18))))) + (uint32)(((sint32)(((uint32)(v19) * (uint32)(v19))))))) + (uint32)(((sint32)(((uint32)(v20) * (uint32)(v20)))))));
  v22 = ((uint32)(v18) << (uint32)(12));
  if (!v21)
    v21 = 1;
  v47 = (((uint32)(v20) << (uint32)(12)) / ((sint32)(v21)));
  v45 = (((sint32)(v22)) / ((sint32)(v21)));
  v46 = (((uint32)(v19) << (uint32)(12)) / ((sint32)(v21)));
  xport_draft_asm_carrier = (((sint32)(v22)) / ((sint32)(v21)));
  xport_draft_asm_carrier = v46;
  (abort(),0u);
  xport_draft_asm_carrier = v47;
  (abort(),0u);
  xport_draft_asm_carrier = &v48;
  (abort(),0u);
  v42 >>= 12;
  v44 >>= 12;
  v43 >>= 12;
  v51[7] = v47;
  v51[2] = v48;
  v51[5] = v49;
  v51[8] = v50;
  v51[4] = v46;
  v51[0] = v42;
  v51[6] = v44;
  v51[1] = (((sint32)(v22)) / ((sint32)(v21)));
  v51[3] = v43;
  return xport_draft_host_sub_80076420_p1(v51,((uint32)(((uint32)(a1) + (uint32)(444)))));
}



uint32 sub_80085BA4(uint32 a1, uint32 a2, uint32 a3)
{
  /* Thirty-two shifts discard every incoming accumulator bit */
  uint32 result = 0u;
  sint32 v4;
  uint32 v5;
  uint32 v6;
  v4 = 32;
  v5 = (uint32)(((unsigned long long)a1 * (unsigned long long)a2) >> 32);
  v6 = ((uint32)(a1) * (uint32)(a2));
  do
  {
    v5 = (((uint32)(2) * (uint32)(v5)) | (v6 >> 31));
    result *= 2;
    if ((v5 >= a3))
    {
      v5 -= a3;
      result |= 1u;
    }
    --v4;
    v6 *= 2;
  }
  while (v4);
  return result;
}



uint32 sub_800762A8(uint32 a1, uint32 a2)
{
  uint32 result;
  uint32 v3;
  sint32 v4;
  sint32 v5;
  result = a1;
  v3 = ((uint32)((((uint32)(0x800F863Cu))+((((uint32)(2) * (uint32)(a2)) & 0x3FFC))*1u)));
  v4 = ((sint16)(r_u16((v3+(1)*2u))));
  v5 = ((sint16)(r_u16(v3)));
  w_u32(result,0);
  w_u32((result+(2)*4u),0);
  w_u32((result+(1)*4u),v5);
  w_u32((result+(3)*4u),v4);
  return result;
}



uint32 sub_80075F80(uint32 a1, uint32 a2, uint32 a3)
{
  sint32 v3;
  sint32 v4;
  sint32 v5;
  sint32 v6;
  uint32 result;
  sint32 v8;
  sint32 v9;
  sint32 v10;
  sint32 v11;
  sint32 v12;
  sint32 v13;
  sint32 v14;
  v3 = r_u32((a3+(1)*4u));
  v4 = r_u32((a3+(3)*4u));
  v5 = ((sint32)(r_u32((a2+(3)*4u))));
  v6 = ((sint32)(r_u32(a2)));
  result = a1;
  v8 = ((uint32)(r_u32(a3)) * (uint32)(((sint32)(r_u32(a2)))));
  v9 = ((sint32)(r_u32((a2+(1)*4u))));
  v10 = ((sint32)(((uint32)(v4) * (uint32)(((sint32)(r_u32(a2)))))));
  v11 = ((sint32)(r_u32((a2+(2)*4u))));
  v12 = r_u32((a3+(2)*4u));
  v13 = (((uint32)(((uint32)(((uint32)(((sint32)(((uint32)(v4) * (uint32)(v9))))) + (uint32)(((sint32)(((uint32)(v3) * (uint32)(v5))))))) + (uint32)(((sint32)(((uint32)(v12) * (uint32)(v6))))))) - (uint32)(((uint32)(r_u32(a3)) * (uint32)(v11)))) >> 12);
  v14 = (((uint32)(((uint32)(((uint32)(((sint32)(((uint32)(v4) * (uint32)(v11))))) + (uint32)(((sint32)(((uint32)(v12) * (uint32)(v5))))))) + (uint32)(((uint32)(r_u32(a3)) * (uint32)(v9))))) - (uint32)(((sint32)(((uint32)(v3) * (uint32)(v6)))))) >> 12);
  w_u32(a1,(((sint32)(((uint32)(((uint32)(((uint32)(v10) + (uint32)(((uint32)(r_u32(a3)) * (uint32)(v5))))) + (uint32)(((sint32)(((uint32)(v3) * (uint32)(v11))))))) - (uint32)(((sint32)(((uint32)(v12) * (uint32)(v9)))))))) >> 12));
  w_u32((a1+(1)*4u),v13);
  w_u32((a1+(2)*4u),v14);
  w_u32((a1+(3)*4u),(((uint32)(((uint32)(((uint32)(((sint32)(((uint32)(v4) * (uint32)(v5))))) - (uint32)(v8))) - (uint32)(((sint32)(((uint32)(v3) * (uint32)(v9))))))) - (uint32)(((sint32)(((uint32)(v12) * (uint32)(v11)))))) >> 12));
  return result;
}


/* TODO Missing call adapter indirect */
/* TODO Missing call adapter v31 */
/* TODO Missing call adapter xport_draft_missing_gte_adapter */
void sub_8008267C(uint32 attributes, uint32 stride, uint32 attribute_flags, const uint32 geometry[3], uint32 subdivisions, uint32 step, uint32 destination)
{
  uint32 color0,color1,color2,row_count=subdivisions-1u;
  uint32 row_end=destination+16u*(subdivisions-8u);
  uint32 row_start,point,vertical=0u,horizontal,remaining,inner_count;
  uint32 packed,screen,flag;
  /* Generate the opposite triangle in the shared interpolation grid */
  if (attribute_flags&0x40000000u)
  {
    color0=r_u32(attributes+12u);
    color1=r_u32(attributes+stride+12u);
    color2=r_u32(attributes+2u*stride+12u);
    xport_draft_gte_control_write(8u,((color0&255u)<<4)|((color1&255u)<<20));
    xport_draft_gte_control_write(9u,((color2&255u)<<4)|((color0&0xFF00u)<<12));
    xport_draft_gte_control_write(10u,((color1&0xFF00u)>>4)|((color2&0xFF00u)<<12));
    xport_draft_gte_control_write(11u,0u);
    xport_draft_gte_control_write(12u,0u);
  }
  if ((sint32)attribute_flags<0)
  {
    color0=r_u32(attributes+4u);
    color1=r_u32(attributes+stride+4u);
    color2=r_u32(attributes+2u*stride+4u);
    xport_draft_gte_control_write(16u,((color0&255u)<<4)|((color1&255u)<<20));
    xport_draft_gte_control_write(17u,((color2&255u)<<4)|((color0&0xFF00u)<<12));
    xport_draft_gte_control_write(18u,((color1&0xFF00u)>>4)|((color2&0xFF00u)<<12));
    xport_draft_gte_control_write(19u,((color0>>12)&0xFF0u)|(((color1>>12)&0xFF0u)<<16));
    xport_draft_gte_control_write(20u,(color2>>12)&0xFF0u);
  }
  sub_80082508(geometry[0],geometry[1],geometry[2]);
  do
  {
    row_start=row_end;
    if ((sint32)(vertical-3840u)>=0) vertical=4096u;
    remaining=4096u-vertical;
    horizontal=0u;
    xport_draft_gte_data_write(0u,remaining);
    xport_draft_gte_data_write(1u,vertical);
    point=row_start+16u;
    inner_count=row_count;
    do
    {
      xport_draft_gte_execute(0x180001u);
      point-=16u;
      remaining-=step;
      horizontal+=step;
      packed=(horizontal<<16)|remaining;
      if ((sint32)(horizontal-3840u)>=0) packed=0x10000000u;
      screen=xport_draft_gte_data_read(14u);
      flag=xport_draft_gte_control_read(31u);
      xport_draft_gte_execute(0x4A6412u);
      screen=((screen&0xBFFFBFFFu)|~((screen-0x00F00200u)|0xBFFFBFFFu))&0x7FFFFFFFu;
      xport_draft_gte_execute(0x198003Du);
      w_u32(point,screen|(flag&0x80000000u));
      w_u32(point+8u,xport_draft_gte_data_read(22u));
      if ((sint32)attribute_flags<0)
      {
        xport_draft_gte_execute(0x4C6412u);
        xport_draft_gte_execute(0x198003Du);
        w_u32(point+12u,xport_draft_gte_data_read(22u));
      }
      xport_draft_gte_data_write(0u,packed);
    } while (inner_count--!=0u);
    vertical+=step;
    row_end=row_start-128u;
  } while (row_count--!=0u);
  /* The packet cursor is unchanged by this interpolation helper */
}


uint32 sub_800827A4(uint32 geometry, xport_draft_polygon_strip_context *context)
{
  uint32 packet=context->packet_cursor;
  uint32 previous=context->clipping_mask;
  uint32 remaining=context->remaining_segments;
  uint32 first,second,combined;
  do
  {
    w_u32(packet+4u,context->packet_words[1]);
    if (packet>=context->packet_limit)
    {
      /* TODO Bind the packet-limit continuation at 80082F60 */
      abort();
    }
    w_u32(packet+8u,context->packet_words[2]);
    w_u32(packet+12u,context->packet_words[3]);
    first=r_u32(geometry+16u);
    second=r_u32(geometry+144u);
    geometry+=16u;
    combined=first&second;
    if (!(previous&combined))
    {
      packet+=24u;
      w_u32(packet-24u,packet+0x05000000u);
      w_u32(packet-8u,first);
      w_u32(packet-4u,second);
    }
    previous=combined&context->frustum_mask;
    context->packet_words[2]=first;
    context->packet_words[3]=second;
  } while(remaining--!=0u);
  context->packet_cursor=packet;
  context->clipping_mask=previous;
  context->remaining_segments=remaining;
  return packet;
}


uint32 sub_8007F904(uint32 a1)
{
  uint32 v2;
  sint32 v3;
  sint8 v4;
  uint32 v5;
  sint32 result;
  sint32 v7;
  uint32 v8;
  v2 = (a1+(8)*4u);
  v3 = r_u32((a1+(1)*4u));
  w_u32(0x800FFA9Cu,1);
  v4 = sub_80080D84((a1+(8)*4u),v3);
  if (r_u32(0x800FFB3Cu))
    ((void)(v3),abort(),0u);
  v5 = (v2+(((uint32)(2) * (uint32)(v3)))*4u);
  result = (v4 & 0xBF);
  if (((v4 & 0xBF) == 0))
  {
    v7 = r_u32((a1+(3)*4u));
    v8 = (v5+(((uint32)(2) * (uint32)(r_u32((a1+(2)*4u)))))*4u);
    ((void)(0x800F3E10u),abort(),0u);
    ((void)(0x800EE710u),abort(),0u);
    SetFarColor(r_u16(0x800FFA8Cu),r_u16(0x800FFA8Eu),r_u16(0x800FFA90u));
    SetBackColor(r_u16(0x800FFA94u),r_u16(0x800FFA96u),r_u16(0x800FFA98u));
    return sub_800817FC(v8,v5,v7);
  }
  return result;
}


/* TODO Missing call adapter xport_draft_missing_gte_adapter */
uint32 sub_80080D84(uint32 a1, uint32 a2)
{
  uint32 xport_draft_asm_carrier; /* TODO GTE carrier */
  sint32 result;
  sint32 v3;
  sint32 v4;
  sint32 v5;
  sint32 v6;
  sint32 v7;
  sint32 v8;
  sint32 v9;
  sint32 v10;
  uint32 v11;
  sint32 v18;
  sint32 v19;
  result = 255;
  if (a2)
  {
    v3 = ((r_u32(((uint32)(r_u32(0x800FFB08u))))>>16)&65535u);
    v4 = ((unsigned short)(r_u32(((uint32)(r_u32(0x800FFB08u))))));
    v5 = ((r_u32(((uint32)(((uint32)(r_u32(0x800FFB08u)) + (uint32)(4)))))>>16)&65535u);
    v6 = ((unsigned short)(r_u32(((uint32)(((uint32)(r_u32(0x800FFB08u)) + (uint32)(4)))))));
    v7 = ((r_u32(((uint32)(((uint32)(r_u32(0x800FFB08u)) + (uint32)(8)))))>>16)&65535u);
    v8 = ((unsigned short)(r_u32(((uint32)(((uint32)(r_u32(0x800FFB08u)) + (uint32)(8)))))));
    v9 = 0xFFFF;
    v10 = r_u32(0x800FFAC0u);
    v11 = ((uint32)(r_u32(0x800FFAC4u)));
    xport_draft_asm_carrier = ((sint32)(r_u32(a1)));
    xport_draft_asm_carrier = ((sint32)(r_u32((a1+(1)*4u))));
    do
    {
      (abort(),0u);
      a1 += (2)*4u;
      --a2;
      (abort(),0u);
      xport_draft_asm_carrier = ((sint32)(r_u32(a1)));
      xport_draft_asm_carrier = ((sint32)(r_u32((a1+(1)*4u))));
      (abort(),0u);
      w_u32(v11,(((unsigned short)(xport_draft_asm_carrier)) | ((uint32)(xport_draft_asm_carrier) << (uint32)(16))));
      (abort(),0u);
      w_u16(((uint32)(((uint32)(v10) + (uint32)(4)))),xport_draft_asm_carrier);
      w_u32(((uint32)(v10)),xport_draft_asm_carrier);
      v18 = ((((((((sint32)(v4)) < ((sint32)(((short)(xport_draft_asm_carrier))))) | ((uint32)(2) * (uint32)((((sint32)(((short)(xport_draft_asm_carrier)))) < ((sint32)(v6)))))) | ((uint32)(4) * (uint32)((((sint32)(v3)) < ((sint32)(((sint32)((((unsigned long long)(xport_draft_asm_carrier)) >> 16))))))))) | ((uint32)(8) * (uint32)((((sint32)(((sint32)((((unsigned long long)(xport_draft_asm_carrier)) >> 16))))) < ((sint32)(v5)))))) | ((uint32)(16) * (uint32)((xport_draft_asm_carrier < ((sint32)(v8)))))) | ((uint32)(32) * (uint32)((((sint32)(v7)) < xport_draft_asm_carrier))));
      if ((xport_draft_asm_carrier < 0))
        v18 ^= 0xFu;
      v19 = (v18 | (((uint32)(v18) << (uint32)(8)) ^ 0xFF00));
      w_u16(((uint32)(((uint32)(v10) + (uint32)(6)))),v19);
      v9 &= v19;
      v10 += 8;
      v11 += (2)*4u;
    }
    while (a2);
    w_u32(0x1F8001D4,v10);
    return v9;
  }
  return result;
}


/* TODO Missing call adapter xport_draft_missing_gte_adapter */
void sub_80084578(uint32 a1, uint32 a2, uint32 a3)
{
  uint32 xport_draft_asm_carrier; /* TODO GTE carrier */
  sint32 v16;
  sint32 v17;
  xport_draft_asm_carrier = ((sint32)(r_u32(a1)));
  xport_draft_asm_carrier = ((sint32)(r_u32((a1+(1)*4u))));
  xport_draft_asm_carrier = ((sint32)(r_u32((a1+(2)*4u))));
  xport_draft_asm_carrier = ((sint32)(r_u32((a1+(3)*4u))));
  xport_draft_asm_carrier = ((sint32)(r_u32((a1+(4)*4u))));
  (abort(),0u);
  xport_draft_asm_carrier = ((sint32)(r_u32((a1+(5)*4u))));
  xport_draft_asm_carrier = ((sint32)(r_u32((a1+(6)*4u))));
  xport_draft_asm_carrier = ((sint32)(r_u32((a1+(7)*4u))));
  (abort(),0u);
  xport_draft_asm_carrier = ((sint32)(r_u32(a2)));
  xport_draft_asm_carrier = ((sint32)(r_u32((a2+(1)*4u))));
  xport_draft_asm_carrier = ((sint32)(r_u32((a2+(2)*4u))));
  xport_draft_asm_carrier = ((sint32)(r_u32((a2+(3)*4u))));
  xport_draft_asm_carrier = ((sint32)(r_u32((a2+(4)*4u))));
  (abort(),0u);
  v16 = r_u32(((uint32)(((uint32)(a3) + (uint32)(4)))));
  v17 = r_u32(((uint32)(((uint32)(a3) + (uint32)(8)))));
  xport_draft_asm_carrier = r_u32(((uint32)(((uint32)(a3) + (uint32)(12)))));
  xport_draft_asm_carrier = r_u32(((uint32)(((uint32)(a3) + (uint32)(16)))));
  xport_draft_asm_carrier = (((unsigned short)((r_u32(((uint32)(a3))) ^ v16))) ^ v16);
  (abort(),0u);
  xport_draft_asm_carrier = (((r_u32(((uint32)(a3)))>>16)&65535u) | ((uint32)(v17) << (uint32)(16)));
  xport_draft_asm_carrier >>= 16;
  (abort(),0u);
  xport_draft_asm_carrier = (((unsigned short)((v16 ^ v17))) ^ v17);
  (abort(),0u);
  xport_draft_asm_carrier = (((unsigned short)(xport_draft_asm_carrier)) | ((uint32)(xport_draft_asm_carrier) << (uint32)(16)));
  (abort(),0u);
  xport_draft_asm_carrier = (((unsigned short)(xport_draft_asm_carrier)) | ((uint32)(xport_draft_asm_carrier) << (uint32)(16)));
  (abort(),0u);
  xport_draft_asm_carrier = (((uint32)(xport_draft_asm_carrier) << (uint32)(16)) | ((unsigned short)(xport_draft_asm_carrier)));
  (abort(),0u);
  xport_draft_asm_carrier = (((unsigned short)(xport_draft_asm_carrier)) | ((uint32)(xport_draft_asm_carrier) << (uint32)(16)));
  (abort(),0u);
  xport_draft_asm_carrier = (((unsigned short)(xport_draft_asm_carrier)) | ((uint32)(xport_draft_asm_carrier) << (uint32)(16)));
  xport_draft_asm_carrier = (((unsigned short)(xport_draft_asm_carrier)) | ((uint32)(xport_draft_asm_carrier) << (uint32)(16)));
  (abort(),0u);
  xport_draft_asm_carrier = (((uint32)(xport_draft_asm_carrier) << (uint32)(16)) | ((unsigned short)(xport_draft_asm_carrier)));
  xport_draft_asm_carrier = (((unsigned short)(xport_draft_asm_carrier)) | ((uint32)(xport_draft_asm_carrier) << (uint32)(16)));
  xport_draft_asm_carrier = ((sint16)(r_u16(((uint32)(((uint32)(a3) + (uint32)(22)))))));
  xport_draft_asm_carrier = (r_u16(((uint32)(((uint32)(a3) + (uint32)(18))))) | ((uint32)(((sint16)(r_u16(((uint32)(((uint32)(a3) + (uint32)(20)))))))) << (uint32)(16)));
  (abort(),0u);
}


/* TODO Missing call adapter xport_draft_missing_gte_adapter */
void sub_80085C94(uint32 a1)
{
  uint32 xport_draft_asm_carrier; /* TODO GTE carrier */
  xport_draft_asm_carrier = ((sint32)(r_u32(a1)));
  xport_draft_asm_carrier = ((sint32)(r_u32((a1+(1)*4u))));
  xport_draft_asm_carrier = ((sint32)(r_u32((a1+(2)*4u))));
  xport_draft_asm_carrier = ((sint32)(r_u32((a1+(3)*4u))));
  xport_draft_asm_carrier = ((sint32)(r_u32((a1+(4)*4u))));
  (abort(),0u);
}



/* MIPS 854F4 multiplies three packed matrix columns through the current GTE rotation */
void sub_800854F4(uint32 input, uint32 output)
{
  uint32 first0=r_u32(input), first1=r_u32(input+4u), first3=r_u32(input+12u);
  uint32 first2,first4,column0[3],column1[3],column2[3];
  xport_draft_gte_data_write(9u,first0);
  xport_draft_gte_data_write(10u,first1>>16);
  xport_draft_gte_data_write(11u,first3);
  xport_draft_gte_execute(0x49E012u);
  first2=r_u32(input+8u);
  column0[0]=xport_draft_gte_data_read(25u);
  column0[1]=xport_draft_gte_data_read(26u);
  column0[2]=xport_draft_gte_data_read(27u);
  xport_draft_gte_data_write(9u,first0>>16);
  xport_draft_gte_data_write(10u,first2);
  xport_draft_gte_data_write(11u,first3>>16);
  xport_draft_gte_execute(0x49E012u);
  first4=r_u32(input+16u);
  column1[0]=xport_draft_gte_data_read(25u);
  column1[1]=xport_draft_gte_data_read(26u);
  column1[2]=xport_draft_gte_data_read(27u);
  xport_draft_gte_data_write(9u,first1);
  xport_draft_gte_data_write(10u,first2>>16);
  xport_draft_gte_data_write(11u,first4);
  xport_draft_gte_execute(0x49E012u);
  w_u32(output,(column0[0]&0xFFFFu)|(column1[0]<<16));
  w_u32(output+12u,(column0[2]&0xFFFFu)|(column1[2]<<16));
  column2[0]=xport_draft_gte_data_read(25u);
  column2[1]=xport_draft_gte_data_read(26u);
  column2[2]=xport_draft_gte_data_read(27u);
  w_u32(output+4u,(column2[0]&0xFFFFu)|(column0[1]<<16));
  w_u32(output+8u,(column1[1]&0xFFFFu)|(column2[1]<<16));
  w_u32(output+16u,column2[2]);
}
