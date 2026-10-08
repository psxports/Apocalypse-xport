#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
#include <stdlib.h>
/* TODO Missing call adapter xport_draft_missing_gte_adapter */
/* TODO Missing host buffer adapter xport_draft_host_sub_8006C3AC_p13 */
/* TODO Missing host buffer adapter xport_draft_host_sub_8006C564_p123 */
uint32 sub_8006AAB8(void)
{
  uint32 xport_draft_asm_carrier; /* TODO GTE carrier */
  uint32 v5;
  uint32 v6;
  sint32 result;
  uint32 v8;
  sint32 v11;
  sint32 v12;
  uint32 v13;
  short v14;
  short v15;
  sint32 v16;
  sint32 v17;
  sint32 v18;
  sint32 v19;
  sint32 v20;
  sint32 v21;
  short v22;
  sint32 v23;
  sint32 v24;
  sint32 v25;
  sint32 v26;
  sint8 v27;
  sint8 v28;
  sint8 v29;
  short v30;
  short v31;
  sint32 v32;
  int v33[4];
  int v34[4];
  sint32 v35;
  sint32 v36;
  sint32 v37;
  sint8 v38;
  sint32 v39;
  sint32 v40;
  uint32 v41;
  int *v42;
  char *v43;
  xport_draft_asm_carrier = 0x800ED590u;
  xport_draft_asm_carrier = r_u32(0x800ED594u);
  (abort(),0u);
  xport_draft_asm_carrier = r_u32(0x800ED598u);
  xport_draft_asm_carrier = r_u32(0x800ED59Cu);
  xport_draft_asm_carrier = r_u32(0x800ED5A0u);
  (abort(),0u);
  v5 = 0x800E5C08u;
  v42 = &v35;
  v43 = &v38;
  v6 = r_u32(0x800E5C18u);
  v33[0] = ((uint32)(r_u32(0x800ED520u)) << (uint32)(12));
  v33[1] = ((uint32)(r_u32(0x800ED524u)) << (uint32)(12));
  v33[2] = ((uint32)(r_u32(0x800ED528u)) << (uint32)(12));
  while (1)
  {
    result = ((sint32)(r_u32(v5)));
    if (((sint32)(r_u32(v5))))
      return result;
    v8 = ((uint32)(((sint32)(r_u32((v6-(2)*4u))))));
    if (r_u8(((uint32)(v6))))
    {
      xport_draft_host_sub_8006C3AC_p13(v43,(v5+(5)*4u),v33);
      v39 = 12;
      xport_draft_host_sub_8006C564_p123(v42,v43,&v39);
      xport_draft_asm_carrier = v34;
      v34[0] = v35;
      v34[1] = v36;
      v34[2] = v37;
      xport_draft_asm_carrier = (((unsigned short)(v35)) | ((uint32)(((unsigned short)(v36))) << (uint32)(16)));
      (abort(),0u);
      v11 = ((sint32)(r_u32((v6-(1)*4u))));
      (abort(),0u);
      if (((((sint32)(v40)) < r_u32(0x800FFAE8u)) || (r_u32(0x800FFAD8u) < ((sint32)(v40)))))
        goto LABEL_29;
      (abort(),0u);
      v12 = ((unsigned short)(v41));
      v13 = ((v41>>16)&65535u);
    }
    else
    {
      v12 = r_u16((((uint32)(v6))-(6)*2u));
      v13 = ((v13 & 0xFFFF0000u)|(((r_u16((((uint32)(v6))-(5)*2u))) & 0xFFFFu)<<0));
      v11 = ((sint32)(r_u32((v6-(1)*4u))));
    }
    v14 = ((uint32)(v13) + (uint32)(5));
    v15 = ((uint32)(v13) + (uint32)(2));
    while (1)
    {
      v16 = ((unsigned char)(r_u8(v8)));
      if (!(r_u8(v8)))
        break;
      v17 = r_u8((((uint32)(0x800E6888u))+((r_u32(((v8+=1u)-1u)) & 0x7F))*1u));
      if ((v16 == 95))
        v17 = ((r_u32((0x800E6888u+(11)*4u))>>8)&255u);
      v18 = ((uint32)(8) * (uint32)(v17));
      if ((v17 == 255))
      {
        v12 += 5;
      }
      else
      {
        v19 = r_u32(((uint32)(((uint32)(((uint32)(r_u32(0x800FF6DCu)) + (uint32)(v18))) + (uint32)(4)))));
        if ((r_u8(((uint32)(((uint32)(((uint32)(r_u32(0x800FF6DCu)) + (uint32)(v18))) + (uint32)(2))))) < 4u))
          ++v12;
        v20 = r_u32(0x800FF668u);
        result = 150994944;
        if ((r_u32(0x800FF374u) < ((uint32)(((uint32)(r_u32(0x800FF668u)) + (uint32)(40))))))
          return result;
        w_u32(0x800FF668u,(r_u32(0x800FF668u)+(40)));
        w_u32(((uint32)(v20)),150994944);
        w_u32(((uint32)(((uint32)(v20) + (uint32)(4)))),v11);
        w_u32(((uint32)(((uint32)(v20) + (uint32)(12)))),r_u32(((uint32)(v19))));
        w_u32(((uint32)(((uint32)(v20) + (uint32)(20)))),r_u32(((uint32)(((uint32)(v19) + (uint32)(4))))));
        v21 = r_u8(((uint32)(((uint32)(v20) + (uint32)(20)))));
        w_u32(((uint32)(((uint32)(v20) + (uint32)(28)))),r_u32(((uint32)(((uint32)(v19) + (uint32)(8))))));
        v22 = r_u16(((uint32)(((uint32)(v19) + (uint32)(10)))));
        v23 = r_u8(((uint32)(((uint32)(v20) + (uint32)(29)))));
        v24 = ((uint32)(v21) - (uint32)(r_u8(((uint32)(((uint32)(v20) + (uint32)(12)))))));
        v25 = r_u8(((uint32)(((uint32)(v20) + (uint32)(13)))));
        w_u16(((uint32)(((uint32)(v20) + (uint32)(36)))),v22);
        v26 = ((sint32)(((uint32)(v23) - (uint32)(v25))));
        if (r_u8((((uint32)(v6))+(1)*1u)))
        {
          v24 = (((uint32)(55) * (uint32)(v24)) >> 5);
          v26 = (((uint32)(55) * (uint32)(v26)) >> 5);
          v27 = r_u8(((uint32)(((uint32)(v20) + (uint32)(36)))));
          w_u8(((uint32)(((uint32)(v20) + (uint32)(20)))),((uint32)(v21) - (uint32)(1)));
          v28 = r_u8(((uint32)(((uint32)(v20) + (uint32)(29)))));
          w_u8(((uint32)(((uint32)(v20) + (uint32)(36)))),((uint32)(v27) - (uint32)(1)));
          v29 = r_u8(((uint32)(((uint32)(v20) + (uint32)(37)))));
          w_u8(((uint32)(((uint32)(v20) + (uint32)(29)))),((uint32)(v28) - (uint32)(1)));
          w_u8(((uint32)(((uint32)(v20) + (uint32)(37)))),((uint32)(v29) - (uint32)(1)));
        }
        v30 = ((sint32)(((uint32)(v12) + (uint32)(v24))));
        if ((v17 == 30))
        {
          if ((v16 == 95))
          {
            v30 = ((sint32)(((uint32)(v12) + (uint32)(v24))));
            v31 = ((uint32)(((uint32)(v13) + (uint32)(v26))) + (uint32)(5));
            w_u16(((uint32)(((uint32)(v20) + (uint32)(8)))),v12);
            w_u16(((uint32)(((uint32)(v20) + (uint32)(10)))),v14);
            w_u16(((uint32)(((uint32)(v20) + (uint32)(16)))),((sint32)(((uint32)(v12) + (uint32)(v24)))));
            w_u16(((uint32)(((uint32)(v20) + (uint32)(18)))),v14);
          }
          else
          {
            v31 = ((uint32)(((uint32)(v13) + (uint32)(v26))) + (uint32)(2));
            w_u16(((uint32)(((uint32)(v20) + (uint32)(8)))),v12);
            w_u16(((uint32)(((uint32)(v20) + (uint32)(10)))),v15);
            w_u16(((uint32)(((uint32)(v20) + (uint32)(16)))),v30);
            w_u16(((uint32)(((uint32)(v20) + (uint32)(18)))),v15);
          }
        }
        else
        {
          v31 = ((uint32)(v13) + (uint32)(v26));
          w_u16(((uint32)(((uint32)(v20) + (uint32)(8)))),v12);
          w_u16(((uint32)(((uint32)(v20) + (uint32)(10)))),v13);
          w_u16(((uint32)(((uint32)(v20) + (uint32)(16)))),v30);
          w_u16(((uint32)(((uint32)(v20) + (uint32)(18)))),v13);
        }
        w_u16(((uint32)(((uint32)(v20) + (uint32)(24)))),v12);
        w_u16(((uint32)(((uint32)(v20) + (uint32)(26)))),v31);
        w_u16(((uint32)(((uint32)(v20) + (uint32)(32)))),v30);
        w_u16(((uint32)(((uint32)(v20) + (uint32)(34)))),v31);
        if (r_u8(((uint32)(v6))))
          v32 = ((uint32)((v40 & 0x3FFC)) + (uint32)(r_u32(0x800FF660u)));
        else
          v32 = r_u32(0x800FF660u);
        w_u32(((uint32)(v20)),((r_u32(((uint32)(v20))) & 0xFF000000) | (r_u32(((uint32)(((uint32)(v32) + (uint32)(112))))) & 0xFFFFFF)));
        w_u32(((uint32)(((uint32)(v32) + (uint32)(112)))),((r_u32(((uint32)(((uint32)(v32) + (uint32)(112))))) & 0xFF000000) | (v20 & 0xFFFFFF)));
        v12 += ((uint32)(3) + (uint32)(v24));
        if ((((sint32)(v24)) < 4))
          ++v12;
      }
    }

    LABEL_29:
    v6 += (8)*4u;

    v5 += (8)*4u;
  }

}


/* TODO Missing call adapter SLOWORD */
/* TODO Postincrement memory expressions may require ordering refinement */
void sub_8006D91C(void)
{
  uint32 v0;
  uint32 v1;
  sint32 v2;
  sint8 v3;
  sint32 v4;
  sint32 v5;
  sint32 v6;
  sint32 v7;
  sint32 v8;
  uint32 v9;
  short v10;
  sint32 v11;
  sint32 v12;
  sint32 v13;
  uint32 v14;
  uint32 v15;
  uint32 v16;
  sint32 v17;
  uint32 v18;
  uint32 v19;
  sint32 v20;
  sint32 v21;
  sint32 v22;
  sint32 v23;
  sint32 v24;
  sint32 v25;
  short v26;
  short v27;
  sint32 v28;
  sint32 v29;
  sint32 v30;
  sint8 v31;
  sint32 v32;
  sint8 v33;
  sint8 v34;
  sint8 v35;
  sint32 v36;
  sint8 v37;
  sint32 v38;
  sint8 vars0;
  sint8 vars4;
  sint32 vars8;
  if (r_u32(0x800FF758u))
  {
    (w_u32(0x800FF758u,(r_u32(0x800FF758u)-1u)),r_u32(0x800FF758u));
    v0 = ((uint32)(sub_8006D028(((uint32)(r_u32(0x800FF75Cu)) + (uint32)(8)))));
    if (v0)
      sub_8006D164(40,27,v0,((uint32)(((uint32)(r_u32(0x800FF75Cu)) + (uint32)(8)))));
    if (r_u32(0x800FF758u))
    {
      if (r_u32(0x800FF320u))
      {
        v1 = ((uint32)(r_u32(0x800A558Cu)));
      }
      else
      {
        v2 = (sint32)(r_u32(0x800FF874u) - 1u) / 10;
        v1 = ((uint32)(0x800FF774u));
        if (v2)
          v3 = ((uint32)(v2) + (uint32)(48));
        else
          v3 = 32;
        w_u8(0x800FF774u,v3);
        w_u8(0x800FF775u, (uint32)((sint32)(r_u32(0x800FF874u) - 1u) % 10) + 48u);
      }
      sub_8001A7BC(256);
      sub_8001A7D4(0x80u,0x80u,0x80u,0);
      sub_8001A7B0(1);
      sub_8001AA28(65,27,v1,0,0,256);
    }
  }
  if (!r_u32(0x800FF008u))
  {
    v4 = r_u32(0x800FF5A0u);
    if (r_u32(0x800FF304u))
    {
      sub_8001A7BC(256);
      sub_8001A7D4(0x80u,0x80u,0x80u,0);
      sub_8001AA28(58,50,r_u32(0x800A556Cu),0,0,256);
    }
    if (r_u32(0x800FF324u))
    {
      v5 = r_u32(((uint32)(((uint32)(v4) + (uint32)(532)))));
      v6 = 412;
      if ((((sint32)(v5)) >= 16))
        v5 = 15;
      v7 = 0;
      if ((((sint32)(v5)) <= 0))
      {
        LABEL_23:
        v8 = (((sint32)(v5)) < 9);

      }
      else
      {
        while (1)
        {
          v8 = (((sint32)(v5)) < 9);
          if ((((sint32)(v7)) >= 8))
            break;
          v9 = ((uint32)(sub_8006D028(((uint32)(r_u32(0x800FF75Cu)) + (uint32)(80)))));
          if (v9)
            sub_8006D0D4(v6,25,v9,((uint32)(((uint32)(r_u32(0x800FF75Cu)) + (uint32)(80)))));
          ++v7;
          v6 += 8;
          if ((((sint32)(v7)) >= ((sint32)(v5))))
            goto LABEL_23;
        }

      }
      if (!v8)
      {
        v10 = 30;
        v11 = ((uint32)(v6) - (uint32)(8));
        v12 = 0;
        if ((((sint32)(((uint32)(v5) - (uint32)(8)))) > 0))
        {
          v13 = ((uint32)(v5) - (uint32)(8));
          do
          {
            v14 = ((uint32)(sub_8006D028(((uint32)(r_u32(0x800FF75Cu)) + (uint32)(80)))));
            if (v14)
              sub_8006D0D4(v11,v10,v14,((uint32)(((uint32)(r_u32(0x800FF75Cu)) + (uint32)(80)))));
            ++v12;
            v10 += 5;
          }
          while ((((sint32)(v12)) < ((sint32)(v13))));
        }
      }
      sub_8006D704(1);
      sub_8006D704(2);
      sub_8006D704(3);
      sub_8006D704(4);
      sub_8006D704(5);
      sub_8006D704(6);
      sub_8006D704(7);
      v15 = ((uint32)(sub_8006D028(r_u32(0x800FF75Cu))));
      v16 = v15;
      if (v15)
      {
        sub_8006D0D4(436,46,v15,((uint32)(r_u32(0x800FF75Cu))));
        w_u8((v16+(6)*1u),90);
        w_u8((v16+(5)*1u),90);
        w_u8((v16+(4)*1u),90);
      }
      v17 = r_u32(0x800FF5A0u);
      if (r_u32(0x800FF5A0u))
      {
        v18 = ((uint32)(r_u32(0x800FF75Cu)));
        switch (r_u32(((uint32)(((uint32)(r_u32(((uint32)(((uint32)(((uint32)(r_u32(0x800FF5A0u)) + (uint32)(((uint32)(4) * (uint32)(r_u32(((uint32)(((uint32)(r_u32(0x800FF5A0u)) + (uint32)(644)))))))))) + (uint32)(612)))))) + (uint32)(4))))))
        {
          case 1:
            v18 = ((uint32)(((uint32)(r_u32(0x800FF75Cu)) + (uint32)(72))));
            break;

          case 2:
            v18 = ((uint32)(((uint32)(r_u32(0x800FF75Cu)) + (uint32)(24))));
            break;

          case 3:
            v18 = ((uint32)(((uint32)(r_u32(0x800FF75Cu)) + (uint32)(40))));
            break;

          case 4:
            v18 = ((uint32)(((uint32)(r_u32(0x800FF75Cu)) + (uint32)(48))));
            break;

          case 5:
            v18 = ((uint32)(((uint32)(r_u32(0x800FF75Cu)) + (uint32)(32))));
            break;

          case 6:
            v18 = ((uint32)(((uint32)(r_u32(0x800FF75Cu)) + (uint32)(64))));
            break;

          case 7:
            v18 = ((uint32)(((uint32)(r_u32(0x800FF75Cu)) + (uint32)(56))));
            break;

          case 8:
            v18 = 0;
            sub_8006D1C0(425,39,22,14,40,40,40,0);
            break;

          case 9:
            v18 = 0;
            break;

          default:
            break;

        }

        if (v18)
        {
          v19 = ((uint32)(sub_8006D028(((sint32)(v18)))));
          if (v19)
            sub_8006D0D4(436,46,v19,v18);
        }
      }
      {
        sint32 denominator = r_s16(0x800EC526u);
        sint32 numerator = (sint32)(100u * ((uint32)denominator - (uint32)(sint32)r_s16((uint32)v17 + 218u)));
        v20 = denominator == 0 ? (numerator < 0 ? 1 : -1) :
            (numerator == (sint32)0x80000000u && denominator == -1 ? numerator : numerator / denominator);
      }
      v21 = (sint32)(80u * (uint32)v20) / 100;
      sub_8006D1C0(((uint32)(469) - (uint32)(v21)),16,v21,6,0,0,0,0);
      if ((((sint32)(v21)) < 41))
        sub_8006D294(429,16,40,6,0,255,0,255,255,0,0);
      if ((((sint32)(v20)) < 61))
      {
        sub_8006D294(389,16,40,6,255,255,0,255,0,0,0);
      }
      else
      {
        v22 = r_s16(0x800F863Cu + ((((uint32)((sint32)(350u * ((uint32)v20 - 60u)) / 40) + 50u) * r_u32(0x800FF2F0u)) & 0xFFFu) * 4u);
        v23 = (((sint32)(((uint32)(v22) * (uint32)(v22)))) >> 16);
        if ((((sint32)(v23)) >= 256))
          v23 = 255;
        sub_8006D294(389,16,40,6,255,((unsigned char)(v23)),0,255,v23,0,0);
      }
      v24 = r_u32(((uint32)(((uint32)(((uint32)(v17) + (uint32)(((uint32)(4) * (uint32)(r_u32(((uint32)(((uint32)(v17) + (uint32)(644)))))))))) + (uint32)(612)))));
      if (v24)
      {
        if ((r_u32(((uint32)(((uint32)(v24) + (uint32)(4))))) == 8))
        {
          v26 = 25;
          v27 = 50;
        }
        else
        {
          v25 = (((uint32)(1000) - (uint32)(r_u32(((uint32)(((uint32)(v24) + (uint32)(56))))))) / 20);
          sub_8006D1C0(474,25,10,v25,0,0,0,0);
          v26 = ((uint32)(v25) + (uint32)(25));
          v27 = ((uint32)(50) - (uint32)(v25));
        }
        sub_8006D1C0(474,v26,10,v27,0,100,200,0);
      }
      sub_8006D3C0();
    }
  }
}


/* TODO Missing call adapter sub_8006D3B4 */
void sub_8006D3C0(void)
{
  sint32 v0;
  sint32 v1;
  sint32 v2;
  short v3;
  sint32 v4;
  short v5;
  sint32 v6;
  sint32 v7;
  uint32 v8;
  uint32 v9;
  short v10;
  sint32 v11;
  sint32 v12;
  sint32 v13;
  sint32 v14;
  sint32 v15;
  sint32 v16;
  sint32 v17;
  short v18;
  short v19;
  uint32 v20;
  sint32 v21;
  short v22;
  short v23;
  short v24;
  sint32 v25;
  sint32 v26;
  if (r_u32(0x800FF76Cu))
  {
    if (((r_u16(((uint32)(((uint32)(r_u32(0x800FF76Cu)) + (uint32)(78))))) & 0x40) != 0))
    {
      (abort(),0u);
    }
    else
    {
      v0 = ((sint16)(r_u16(((uint32)(((uint32)(r_u32(0x800FF76Cu)) + (uint32)(218)))))));
      if ((((sint32)(v0)) < 0))
        v0 = 0;
      if ((((sint32)(v0)) < r_u32(0x800FFC4Cu)))
      {
        w_u32(0x800FFC4Cu,v0);
        w_u32(0x800FF770u,8);
      }
      v1 = r_u32(0x800FF668u);
      if ((r_u32(0x800FF374u) >= ((uint32)(((uint32)(r_u32(0x800FF668u)) + (uint32)(24))))))
      {
        w_u32(0x800FF668u,(r_u32(0x800FF668u)+(24)));
        w_u8(((uint32)(((uint32)(v1) + (uint32)(3)))),5);
        w_u8(((uint32)(((uint32)(v1) + (uint32)(7)))),40);
        w_u8(((uint32)(((uint32)(v1) + (uint32)(4)))),0x80);
        w_u8(((uint32)(((uint32)(v1) + (uint32)(5)))),((uint32)(16) * (uint32)(r_u32(0x800FF770u))));
        w_u8(((uint32)(((uint32)(v1) + (uint32)(6)))),((uint32)(16) * (uint32)(r_u32(0x800FF770u))));
        v2 = (((sint32)(v0)) >> 2);
        v3 = r_u32(0x800FFC50u);
        v4 = ((uint32)(v2) * (uint32)(r_u32(0x800FFC5Cu)));
        v5 = r_u32(0x800FFC54u);
        w_u8(((uint32)(((uint32)(v1) + (uint32)(7)))),(r_u8(((uint32)(((uint32)(v1) + (uint32)(7)))))|(2u)));
        v6 = r_u32(0x800FF660u);
        w_u16(((uint32)(((uint32)(v1) + (uint32)(8)))),v3);
        w_u16(((uint32)(((uint32)(v1) + (uint32)(16)))),v3);
        w_u16(((uint32)(((uint32)(v1) + (uint32)(10)))),v5);
        w_u16(((uint32)(((uint32)(v1) + (uint32)(14)))),v5);
        v5 += 6;
        w_u16(((uint32)(((uint32)(v1) + (uint32)(18)))),v5);
        w_u16(((uint32)(((uint32)(v1) + (uint32)(22)))),v5);
        v7 = r_u32(0x800FF668u);
        v8 = (r_u32(((uint32)(v1))) & 0xFF000000);
        v9 = ((uint32)(r_u32(0x800FF668u)) + (uint32)(8));
        v10 = ((uint32)(v3) + (uint32)((((sint32)(v4)) >> 12)));
        w_u16(((uint32)(((uint32)(v1) + (uint32)(12)))),v10);
        w_u16(((uint32)(((uint32)(v1) + (uint32)(20)))),v10);
        w_u32(((uint32)(v1)),(v8 | (r_u32(((uint32)(((uint32)(v6) + (uint32)(112))))) & 0xFFFFFF)));
        v11 = (r_u32(0x800FF374u) < v9);
        w_u32(((uint32)(((uint32)(v6) + (uint32)(112)))),((r_u32(((uint32)(((uint32)(v6) + (uint32)(112))))) & 0xFF000000) | (v1 & 0xFFFFFF)));
        if (!v11)
        {
          w_u32(0x800FF668u,v9);
          w_u8(((uint32)(((uint32)(v7) + (uint32)(3)))),1);
          v12 = r_u32(0x800FF660u);
          w_u32(((uint32)(((uint32)(v7) + (uint32)(4)))),-520093152);
          v13 = r_u32(0x800FF770u);
          w_u32(((uint32)(v7)),((r_u32(((uint32)(v7))) & 0xFF000000) | (r_u32(((uint32)(((uint32)(v12) + (uint32)(112))))) & 0xFFFFFF)));
          w_u32(((uint32)(((uint32)(v12) + (uint32)(112)))),((r_u32(((uint32)(((uint32)(v12) + (uint32)(112))))) & 0xFF000000) | (v7 & 0xFFFFFF)));
          if (v13)
            w_u32(0x800FF770u,((uint32)(v13) - (uint32)(1)));
          v14 = r_u32(0x800FF668u);
          if ((r_u32(0x800FF374u) >= ((uint32)(((uint32)(r_u32(0x800FF668u)) + (uint32)(28))))))
          {
            w_u32(0x800FF668u,(r_u32(0x800FF668u)+(28)));
            w_u8(((uint32)(((uint32)(v14) + (uint32)(3)))),6);
            w_u8(((uint32)(((uint32)(v14) + (uint32)(7)))),76);
            w_u16(((uint32)(((uint32)(v14) + (uint32)(4)))),240);
            w_u8(((uint32)(((uint32)(v14) + (uint32)(6)))),0);
            v15 = r_u32(0x800FFC58u);
            w_u32(((uint32)(((uint32)(v14) + (uint32)(24)))),1431655765);
            v16 = r_u32(0x800FF668u);
            v17 = ((uint32)((((sint32)(v15)) >> 2)) * (uint32)(r_u32(0x800FFC5Cu)));
            v15 = ((v15 & 0xFFFF0000u)|(((((uint32)(r_u32(0x800FFC54u)) - (uint32)(1))) & 0xFFFFu)<<0));
            v18 = ((uint32)(r_u32(0x800FFC54u)) + (uint32)(6));
            v19 = r_u32(0x800FFC50u);
            v20 = ((uint32)(r_u32(0x800FF668u)) + (uint32)(16));
            w_u16(((uint32)(((uint32)(v14) + (uint32)(18)))),((uint32)(r_u32(0x800FFC54u)) + (uint32)(6)));
            w_u16(((uint32)(((uint32)(v14) + (uint32)(22)))),v18);
            w_u16(((uint32)(((uint32)(v14) + (uint32)(10)))),v15);
            w_u16(((uint32)(((uint32)(v14) + (uint32)(14)))),v15);
            w_u16(((uint32)(((uint32)(v14) + (uint32)(8)))),((uint32)(v19) - (uint32)(1)));
            w_u16(((uint32)(((uint32)(v14) + (uint32)(20)))),((uint32)(v19) - (uint32)(1)));
            v21 = r_u32(0x800FF660u);
            v22 = ((uint32)(((uint32)(v19) + (uint32)((((sint32)(v17)) >> 12)))) + (uint32)(1));
            w_u16(((uint32)(((uint32)(v14) + (uint32)(12)))),v22);
            w_u16(((uint32)(((uint32)(v14) + (uint32)(16)))),v22);
            w_u32(((uint32)(v14)),((r_u32(((uint32)(v14))) & 0xFF000000) | (r_u32(((uint32)(((uint32)(v21) + (uint32)(112))))) & 0xFFFFFF)));
            v11 = (r_u32(0x800FF374u) < v20);
            w_u32(((uint32)(((uint32)(v21) + (uint32)(112)))),((r_u32(((uint32)(((uint32)(v21) + (uint32)(112))))) & 0xFF000000) | (v14 & 0xFFFFFF)));
            if (!v11)
            {
              w_u32(0x800FF668u,v20);
              w_u8(((uint32)(((uint32)(v16) + (uint32)(3)))),3);
              w_u8(((uint32)(((uint32)(v16) + (uint32)(7)))),64);
              w_u16(((uint32)(((uint32)(v16) + (uint32)(4)))),240);
              w_u8(((uint32)(((uint32)(v16) + (uint32)(6)))),0);
              v23 = r_u32(0x800FFC50u);
              v24 = ((uint32)(r_u32(0x800FFC54u)) - (uint32)(1));
              w_u16(((uint32)(((uint32)(v16) + (uint32)(14)))),((uint32)(r_u32(0x800FFC54u)) + (uint32)(6)));
              v25 = r_u32(0x800FF660u);
              w_u16(((uint32)(((uint32)(v16) + (uint32)(8)))),--v23);
              w_u16(((uint32)(((uint32)(v16) + (uint32)(12)))),v23);
              v26 = r_u32(((uint32)(v16)));
              w_u16(((uint32)(((uint32)(v16) + (uint32)(10)))),v24);
              w_u32(((uint32)(v16)),((v26 & 0xFF000000) | (r_u32(((uint32)(((uint32)(v25) + (uint32)(112))))) & 0xFFFFFF)));
              w_u32(((uint32)(((uint32)(v25) + (uint32)(112)))),((r_u32(((uint32)(((uint32)(v25) + (uint32)(112))))) & 0xFF000000) | (v16 & 0xFFFFFF)));
            }
          }
        }
      }
    }
  }
}


/* TODO Missing call adapter SHIWORD */
/* TODO Missing call adapter sub_800151FC */
/* TODO Missing call adapter sub_8008BFAC */
/* TODO Missing host buffer adapter xport_draft_host_sub_8001AA28_p3 */
/* TODO Postincrement memory expressions may require ordering refinement */
uint32 sub_80016884(void)
{
  sint32 result;
  sint32 v1;
  sint32 v2;
  sint32 v3;
  uint32 v4;
  sint32 v5;
  sint32 i;
  sint32 v7;
  sint32 v8;
  sint32 v9;
  sint32 v10;
  sint32 v11;
  uint32 v12;
  uint32 v13;
  sint32 v14;
  char v15[104];
  if (r_u32(0x800FF33Cu))
    /* MIPS 800168B0..800168C0 stores zero for all four stack arguments */
    sub_8006D1C0(19,190,194,42,0,0,0,0);
  if (r_u32(0x800FF008u))
  {
    nullsub_19();
    sub_8001A8E8();
  }
  sub_8001A7B0(0);
  if ((r_u32(0x800FF81Cu) && ((r_u32(0x800FF64Cu) & 0x20) != 0)))
  {
    sub_8001A7D4(128,128,128,0);
    sub_8001A7BC(256);
    sub_8001AA28(256,35,r_u32((0x800A556Cu+(0)*4u)),0,0u,256u);
  }
  if (((r_u32(0x800FF5A0u) && (((sint32)(((sint16)(r_u16(((uint32)(((uint32)(r_u32(0x800FF5A0u)) + (uint32)(218))))))))) <= 0)) && (r_u32(0x800FF874u) == 1)))
  {
    if (!sub_8002E13C())
      sub_8002E148(951706923,64,64,127);
    sub_8001A7D4(128,128,128,0);
    sub_8001A7BC(384);
    sub_8001AA28(256,r_u16(0x800FF038u),r_u32((0x800A54E8u+(0)*4u)),0,128u,256u);
    w_u16(0x800FF038u,(r_u16(0x800FF038u)+(8)));
    if ((r_u16(0x800FF038u) >= 121))
      w_u16(0x800FF038u,120);
  }
  sub_8001A7BC(256);
  result = -2146828288;
  switch (r_u32(0x800FF080u))
  {
    case 1:
      sub_8001664C();
      v1 = ((uint32)(r_u32(0x800FF00Cu)) - (uint32)(2));
      if (r_u32(0x800FF00Cu))
    {
      w_u32(0x800FF00Cu,(r_u32(0x800FF00Cu)-(2)));
      w_u32(0x800FF010u,(r_u32(0x800FF010u)+(690)));
      if ((((sint32)(v1)) < 0))
      {
        w_u32(0x800FF00Cu,0);
        w_u32(0x800FF010u,0);
      }
    }
      v2 = r_u32(0x800FF084u);
      w_u32(((uint32)(((uint32)(r_u32(0x800FF084u)) + (uint32)(12)))),((uint32)((((uint32)(r_u32(0x800FF00Cu)) * (uint32)(((void)(r_u32((0x800F863Cu+((r_u32(0x800FF010u) & 0xFFF))*4u))),abort(),0u))) / 4096)) + (uint32)(256)));
      sub_800156E0(v2);
      if (r_u32(0x800FF33Cu))
      return sub_80016744();
    else
      return sub_80016800();

    case 2:
      v3 = r_u32(0x800FF088u);
      if (r_u32(0x800FF088u))
      return sub_800156E0(v3);
      return result;

    case 3:
      sub_8001664C();
      sub_800167D8();
      sub_8001A7D4(149,20,20,0);
      sub_8001AA28(256,95,r_u32((0x800A559Cu+(0)*4u)),0,0u,256u);
      v3 = r_u32(0x800FF024u);
      w_u32(((uint32)(((uint32)(r_u32(0x800FF024u)) + (uint32)(16)))),135);
      return sub_800156E0(v3);

    case 4:
      sub_8001664C();
      sub_800151D0();
      result = ((((sint32)(((sint8)(r_u32(0x800FF2F0u))))) / 10) & 1);
      if ((((((sint32)(((sint8)(r_u32(0x800FF2F0u))))) / 10) & 1) == 0))
      return result;
      sub_8001AA28(256,171,r_u32((0x800A5564u+(0)*4u)),0,0u,256u);
      v4 = r_u32((0x800A5568u+(0)*4u));
      v5 = 197;
      goto LABEL_57;

    case 5:
      for (i = 0; (((sint32)(i)) < r_u32(0x800FF034u)); ++i)
      v15[i] = r_u32((0x800FF030u+(i)*4u));

      v15[r_u32(0x800FF034u)] = 0;
      sub_8001A7BC(192);
      sub_8001A7D4(128,128,128,0);
      sub_8001A7B0(1);
      v7 = sub_8001A97C(r_u32(0x800FF030u));
      return xport_draft_host_sub_8001AA28_p3(((uint32)(256) - (uint32)((((sint32)(v7)) >> 1))),34,v15,0);

    case 6:
      goto LABEL_31;

    case 9:
      sub_8001A7D4(149,20,20,0);
      sub_8001AA28(256,103,r_u32((0x800A5550u+(0)*4u)),0,0u,256u);
      v8 = r_u32(0x800FF024u);
      w_u32(((uint32)(((uint32)(r_u32(0x800FF024u)) + (uint32)(16)))),143);
      sub_800156E0(v8);
      sub_80016800();
      LABEL_31:
    sub_8001A7BC(192);

      sub_8001A7D4(128,128,128,0);
      sub_8001A7B0(0);
      sub_8001AA28(256,34,r_u32(0x800FF030u),0,0u,256u);
      result = r_u32(0x800FF040u);
      if (!r_u32(0x800FF040u))
      return result;
      sub_8001A7BC(256);
      sub_8001A7B0(1);
      sub_8001AA28(166,68,r_u32((0x800A5558u+(0)*4u)),0,0u,256u);
      result = 100;
      v9 = 1;
      if ((r_u32(0x800FFB58u) == 100))
    {
      if (r_u32(0x800FF044u))
        (w_u32(0x800FF044u,(r_u32(0x800FF044u)-1u)),r_u32(0x800FF044u));
      result = ((r_u32(0x800FF044u) >> 2) & 1);
      if (result)
        v9 = 0;
    }
      if (!v9)
      return result;
      ((void)(0x800FFD30u),(void)(((uint32)(0x800FF08Cu))),(void)(r_u32(0x800FFB58u)),abort(),0u);
      v10 = 270;
      v11 = 68;
      v12 = 0x800FFD30u;
      goto LABEL_48;

    case 10:

    case 11:
      (abort(),0u);
      sub_8001AA28(256,90,r_u32((0x800A56B4u+(0)*4u)),0,0u,256u);
      sub_800151D0();
      sub_8001AA28(256,140,r_u32((0x800A56ACu+(0)*4u)),0,0u,256u);
      v4 = r_u32((0x800A56B0u+(0)*4u));
      v5 = 166;
      goto LABEL_57;

    case 12:
      sub_800167D8();
      sub_800151D0();
      sub_8001AA28(256,25,r_u32((0x800A5554u+(0)*4u)),0,0u,256u);
      v3 = r_u32(0x800FF028u);
      return sub_800156E0(v3);

    case 13:
      sub_800167D8();
      (abort(),0u);
      sub_8001AA28(256,40,r_u32((0x800A56DCu+(0)*4u)),0,0u,256u);
      sub_8001AA28(256,66,r_u32((0x800A56E0u+(0)*4u)),0,0u,256u);
      sub_8001AA28(256,92,r_u32((0x800A56E4u+(0)*4u)),0,0u,256u);
      v3 = r_u32(0x800FF024u);
      return sub_800156E0(v3);

    case 14:
      (abort(),0u);
      sub_8001AA28(256,110,r_u32((0x800A56E8u+(0)*4u)),0,0u,256u);
      sub_800151D0();
      sub_8001AA28(256,140,r_u32((0x800A56ACu+(0)*4u)),0,0u,256u);
      v4 = r_u32((0x800A56B0u+(0)*4u));
      v5 = 166;
      goto LABEL_57;

    case 15:
      sub_800151D0();
      sub_8001AA28(256,110,r_u32((0x800A56ECu+(0)*4u)),0,0u,256u);
      v14 = 136;
      v13 = r_u32((0x800A56F0u+(0)*4u));
      goto LABEL_55;

    case 16:
      (abort(),0u);
      sub_8001AA28(256,110,r_u32((0x800A5704u+(0)*4u)),0,0u,256u);
      v5 = 140;
      goto LABEL_56;

    case 17:
      sub_800151D0();
      sub_8001A7BC(192);
      sub_8001AA28(256,67,r_u32((0x800A56BCu+(0)*4u)),0,0u,256u);
      sub_8001AA28(256,85,r_u32((0x800A56C0u+(0)*4u)),0,0u,256u);
      sub_8001AA28(256,103,r_u32((0x800A56C4u+(0)*4u)),0,0u,256u);
      sub_8001AA28(256,121,r_u32((0x800A56C8u+(0)*4u)),0,0u,256u);
      sub_8001AA28(256,139,r_u32((0x800A56CCu+(0)*4u)),0,0u,256u);
      sub_8001AA28(256,157,r_u32((0x800A56D0u+(0)*4u)),0,0u,256u);
      sub_8001A7BC(256);
      v5 = 188;
      goto LABEL_56;

    case 18:
      sub_800151D0();
      sub_8001AA28(256,90,r_u32((0x800A5664u+(0)*4u)),0,0u,256u);
      sub_8001AA28(256,116,r_u32((0x800A5668u+(0)*4u)),0,0u,256u);
      sub_8001AA28(256,142,r_u32((0x800A566Cu+(0)*4u)),0,0u,256u);
      v4 = r_u32((0x800A5670u+(0)*4u));
      v5 = 168;
      goto LABEL_57;

    case 19:
      sub_800167D8();
      sub_800151D0();
      sub_8001AA28(256,35,r_u32((0x800A5688u+(0)*4u)),0,0u,256u);
      sub_8001AA28(256,61,r_u32((0x800A568Cu+(0)*4u)),0,0u,256u);
      sub_8001AA28(256,87,r_u32((0x800A5690u+(0)*4u)),0,0u,256u);
      sub_8001AA28(256,113,r_u32(0x800A5694u),0,0u,256u);
      v3 = r_u32(0x800FF024u);
      w_u32(((uint32)(((uint32)(r_u32(0x800FF024u)) + (uint32)(16)))),148);
      return sub_800156E0(v3);

    case 20:
      (abort(),0u);
      if ((((((sint32)(((sint8)(r_u32(0x800FF2F0u))))) / 10) & 1) != 0))
      sub_8001AA28(256,110,r_u32((0x800A56A8u+(0)*4u)),0,0u,256u);
      sub_800151D0();
      sub_8001AA28(256,140,r_u32((0x800A56ACu+(0)*4u)),0,0u,256u);
      v4 = r_u32((0x800A56B0u+(0)*4u));
      v5 = 166;
      goto LABEL_57;

    case 21:
      sub_800151D0();
      sub_8001AA28(256,90,r_u32((0x800A56F4u+(0)*4u)),0,0u,256u);
      sub_8001AA28(256,116,r_u32((0x800A56F8u+(0)*4u)),0,0u,256u);
      v13 = r_u32((0x800A56FCu+(0)*4u));
      v14 = 142;
      LABEL_55:
    sub_8001AA28(256,v14,v13,0,0u,256u);

      v5 = 170;
      LABEL_56:
    v4 = r_u32(0x800A5590u);

      LABEL_57:
    result = sub_8001AA28(256,v5,v4,0,0u,256u);

      break;

    case 22:
      (abort(),0u);
      if ((((((sint32)(((sint8)(r_u32(0x800FF2F0u))))) / 10) & 1) != 0))
      sub_8001AA28(256,110,r_u32((0x800A5700u+(0)*4u)),0,0u,256u);
      v10 = 256;
      v11 = 140;
      v12 = ((uint32)(r_u32(0x800A5590u)));
      LABEL_48:
    result = sub_8001AA28(v10,v11,v12,0,0u,256u);

      break;

    default:
      return result;

  }

  return result;
}



uint32 sub_80030390(uint32 a1)
{
  sint32 result;
  sint32 v2;
  sint32 v3;
  sint32 v4;
  sint32 v5;
  result = r_u32(0x800FF5A0u);
  if ((a1 == ((uint32)(r_u32(0x800FF5A0u)))))
  {
    v2 = r_u32((a1+(148)*4u));
    v3 = r_u32((a1+(149)*4u));
    w_u32((a1+(46)*4u),r_u32((a1+(147)*4u)));
    w_u32((a1+(47)*4u),v2);
    w_u32((a1+(48)*4u),v3);
  }
  for (; a1; a1 = ((uint32)(r_u32((a1+(7)*4u)))))
  {
    v4 = r_u32((a1+(61)*4u));
    v5 = r_u32((a1+(62)*4u));
    w_u32((a1+(1)*4u),r_u32((a1+(60)*4u)));
    w_u32((a1+(2)*4u),v4);
    w_u32((a1+(3)*4u),v5);
  }

  return result;
}



uint32 sub_8003525C(uint32 a1, uint32 a2)
{
  uint32 result;
  sint32 v5;
  w_u32((a1+(17)*4u),0x800A1C90u);
  result = sub_80034F38(a1,0);
  if (((a2 & 1) != 0))
    return ((uint32)(sub_80032E30(((sint32)(a1)))));
  return result;
}



uint32 sub_80032E7C(uint32 a1, uint32 a2)
{
  uint32 v2;
  uint32 result;
  v2 = ((uint32)(r_u32((a1+(1)*4u))));
  if (v2)
    w_u32(v2,r_u32(a1));
  if (r_u32(a1))
    w_u32(((uint32)(((uint32)(r_u32(a1)) + (uint32)(4)))),r_u32((a1+(1)*4u)));
  result = ((uint32)(r_u32(a2)));
  if ((((uint32)(r_u32(a2))) == a1))
  {
    result = ((uint32)(r_u32((a1+(1)*4u))));
    w_u32(a2,result);
  }
  return result;
}



uint32 sub_80062FB8(uint32 a1, uint32 a2)
{
  sint32 v4;
  sint32 v5;
  sint32 v6;
  sint32 result;
  v4 = r_u32((a1+(89)*4u));
  w_u32((a1+(17)*4u),0x800A3340u);
  if (v4)
    ((void)(a2),sub_8006BC20(v4));
  v5 = r_u32((a1+(90)*4u));
  if (v5)
    ((void)(a2),sub_8006BC20(v5));
  sub_800629BC(((sint32)(a1)),0);
  result = (a2 & 1);
  if (((a2 & 1) != 0))
    return sub_80062608(a1);
  return result;
}


/* TODO Missing call adapter sub_80061D80 */
uint32 sub_8005EBD0(uint32 a1)
{
  sint32 result;
  sint32 v3;
  short v4;
  sint32 v5;
  result = 0;
  if (r_u32(((uint32)(((uint32)(a1) + (uint32)(516))))))
  {
    if ((r_u8(((uint32)(((uint32)(a1) + (uint32)(471))))) | r_u8(((uint32)(((uint32)(a1) + (uint32)(472)))))))
    {
      w_u32(((uint32)(((uint32)(a1) + (uint32)(112)))),0);
      w_u32(((uint32)(((uint32)(a1) + (uint32)(108)))),0);
      w_u32(((uint32)(((uint32)(a1) + (uint32)(104)))),0);
      w_u32(((uint32)(((uint32)(a1) + (uint32)(124)))),0);
      w_u32(((uint32)(((uint32)(a1) + (uint32)(120)))),0);
      v3 = r_u32(((uint32)(((uint32)(a1) + (uint32)(516)))));
      w_u32(((uint32)(((uint32)(a1) + (uint32)(116)))),0);
      v4 = r_u16(((uint32)(((uint32)(v3) + (uint32)(20)))));
      w_u32(((uint32)(((uint32)(a1) + (uint32)(16)))),r_u32(((uint32)(((uint32)(v3) + (uint32)(16))))));
      w_u16(((uint32)(((uint32)(a1) + (uint32)(20)))),v4);
      sub_80063038(a1,7,0,-1);
      v5 = r_u32(((uint32)(((uint32)(a1) + (uint32)(516)))));
      w_u32(((uint32)(((uint32)(a1) + (uint32)(460)))),0x10000);
      ((void)(v5),abort(),0u);
      return 1;
    }
    else
    {
      return 0;
    }
  }
  return result;
}


/* TODO Missing call adapter SHIWORD */
/* TODO Missing host buffer adapter xport_draft_host_sub_80066B8C_p13 */
/* TODO Missing host buffer adapter xport_draft_host_sub_8006C34C_p13 */
/* TODO Missing host buffer adapter xport_draft_host_sub_8006C40C_p1 */
uint32 sub_80066F38(uint32 a1, uint32 a2, uint32 a3, uint32 a4, uint32 a5, uint32 a6, uint32 a7, uint32 a8, uint32 a9)
{
  sint32 v13;
  sint32 v17;
  short v18;
  sint32 v19;
  short v20;
  signed int v21;
  sint32 v22;
  sint32 v23;
  sint32 v24;
  sint32 v25;
  short v26;
  sint32 result;
  short v28;
  sint32 v29;
  short v30;
  uint32 v31[4];
  char v32[16];
  v13 = a9;
  v17 = 0;
  v18 = a8;
  v19 = 1000000;
  while (v13)
  {
    if (((r_u16(((uint32)(((uint32)(v13) + (uint32)(76))))) & ((unsigned short)(v18))) == 0))
    {
      v20 = r_u16(((uint32)(((uint32)(v13) + (uint32)(78)))));
      if (((((v20 & 0x40) == 0) && ((v20 & 0x10) != 0)) && !(r_u8(((uint32)(((uint32)(v13) + (uint32)(224))))))))
      {
        xport_draft_host_sub_8006C40C_p1(v32,((uint32)(v13) + (uint32)(104)),&a7);
        xport_draft_host_sub_8006C34C_p13(v31,((uint32)(v13) + (uint32)(4)),v32);
        v21 = xport_draft_host_sub_80066B8C_p13((short *) (&v29),a2,v31);
        if ((((sint32)(a4)) >= ((sint32)(v21))))
        {
          v22 = ((uint32)(((void)(v29),abort(),0u)) - (uint32)(((sint16)(r_u16((a3+(1)*2u))))));
          v23 = (((sint32)(v22)) < 2049);
          if ((((sint32)(v22)) < -2048))
          {
            v22 += 4096;
            v23 = (((sint32)(v22)) < 2049);
          }
          if (!v23)
            v22 -= 4096;
          if ((((sint32)(v22)) < 0))
            v22 = -v22;
          if ((((sint32)(a4)) > 0))
            v22 += (((uint32)(((uint32)(16) * (uint32)(v22))) * (uint32)(v21)) / ((sint32)(a4)));
          if ((((sint32)(a5)) >= ((sint32)(v22))))
          {
            v24 = ((sint32)(((uint32)(((short)(v29))) - (uint32)(((sint16)(r_u16(a3)))))));
            v25 = (((sint32)(v24)) < 2049);
            if ((((sint32)(v24)) < -2048))
            {
              v24 += 4096;
              v25 = (((sint32)(v24)) < 2049);
            }
            if (!v25)
              v24 -= 4096;
            if ((((sint32)(v24)) < 0))
              v24 = -v24;
            if ((((sint32)(a4)) > 0))
              v24 += (((uint32)(((uint32)(16) * (uint32)(v24))) * (uint32)(v21)) / ((sint32)(a4)));
            if (((((sint32)(a6)) >= ((sint32)(v24))) && (((sint32)(v21)) < ((sint32)(v19)))))
            {
              v17 = v13;
              v19 = v21;
              v26 = v30;
              w_u32(((uint32)(a1)),v29);
              w_u16(((uint32)(((uint32)(a1) + (uint32)(4)))),v26);
            }
          }
        }
      }
    }
    v13 = r_u32(((uint32)(((uint32)(v13) + (uint32)(28)))));
  }

  result = v17;
  if (!v17)
  {
    v28 = ((sint16)(r_u16((a3+(2)*2u))));
    w_u32(((uint32)(a1)),r_u32(((uint32)(a3))));
    w_u16(((uint32)(((uint32)(a1) + (uint32)(4)))),v28);
  }
  return result;
}



uint32 sub_80034A44(uint32 a1, uint32 a2, uint32 a3, uint32 a4)
{
  uint32 v4;
  sint32 result;
  sint32 v6;
  v4 = 0;
  result = r_u32(((uint32)(((uint32)(a1) + (uint32)(80)))));
  v6 = ((((uint32)(a4) << (uint32)(16)) | ((uint32)(a3) << (uint32)(8))) | a2);
  if (result)
  {
    do
    {
      w_u32(((uint32)(((uint32)(((uint32)(((uint32)(8) * (uint32)(v4++))) + (uint32)(r_u32(((uint32)(((uint32)(a1) + (uint32)(72)))))))) + (uint32)(4)))),v6);
      result = (v4 < r_u32(((uint32)(((uint32)(a1) + (uint32)(80))))));
    }
    while ((v4 < r_u32(((uint32)(((uint32)(a1) + (uint32)(80)))))));
  }
  return result;
}



uint32 sub_80034A9C(uint32 a1, uint32 a2, uint32 a3, uint32 a4, uint32 a5)
{
  sint32 v10;
  sint32 v11;
  uint32 v12;
  sint32 v13;
  sint32 result;
  sint32 v15;
  v10 = r_u32(((uint32)(((uint32)(a1) + (uint32)(80)))));
  v11 = ((sint32)(((uint32)(a2) * (uint32)(v10))));
  v12 = 0;
  v13 = (((((uint32)(a5) << (uint32)(16)) | ((uint32)(a4) << (uint32)(8))) | 0x3A000000) | a3);
  result = ((uint32)(8) * (uint32)(v11));
  v15 = ((uint32)(r_u32(((uint32)(((uint32)(a1) + (uint32)(76)))))) + (uint32)(((uint32)(8) * (uint32)(v11))));
  if (v10)
  {
    do
    {
      w_u32(((uint32)(((uint32)(v15) + (uint32)(4)))),v13);
      result = (++v12 < r_u32(((uint32)(((uint32)(a1) + (uint32)(80))))));
      v15 += 8;
    }
    while ((v12 < r_u32(((uint32)(((uint32)(a1) + (uint32)(80)))))));
  }
  return result;
}



uint32 sub_800349D0(uint32 a1, uint32 a2, uint32 a3)
{
  sint32 v4;
  sint32 v5;
  uint32 v6;
  sint32 result;
  uint32 v8;
  v4 = r_u32(((uint32)(((uint32)(a1) + (uint32)(80)))));
  v5 = ((sint32)(((uint32)(a2) * (uint32)(v4))));
  v6 = 0;
  result = ((uint32)(8) * (uint32)(v5));
  v8 = ((uint32)(((uint32)(r_u32(((uint32)(((uint32)(a1) + (uint32)(76)))))) + (uint32)(((uint32)(8) * (uint32)(v5))))));
  if (v4)
  {
    do
    {
      w_u32(v8,a3);
      result = (++v6 < r_u32(((uint32)(((uint32)(a1) + (uint32)(80))))));
      v8 += (2)*4u;
    }
    while ((v6 < r_u32(((uint32)(((uint32)(a1) + (uint32)(80)))))));
  }
  return result;
}



uint32 sub_80032068(uint32 a1, uint32 a2)
{
  sint32 v4;
  sint32 v5;
  sint32 v6;
  uint32 v7;
  sint32 v8;
  sint32 v9;
  sub_80032F7C(a1);
  w_u32((a1+(17)*4u),0x800A1B50u);
  v4 = sub_8006B864(((uint32)(16) * (uint32)(a2)),0,1);
  v5 = 0;
  for (w_u32((a1+(20)*4u),v4); (((sint32)(v5)) < ((sint32)(a2))); w_u32((v7+(2)*4u),v9))
  {
    v6 = ((uint32)(16) * (uint32)(v5));
    w_u32(((uint32)(((uint32)(((uint32)(((uint32)(16) * (uint32)(v5++))) + (uint32)(r_u32((a1+(20)*4u))))) + (uint32)(12)))),1090519039);
    v7 = ((uint32)(((uint32)(v6) + (uint32)(r_u32((a1+(20)*4u))))));
    v8 = r_u32(0x800A71D0u);
    v9 = r_u32(0x800A71D4u);
    w_u32(v7,r_u32(0x800A71CCu));
    w_u32((v7+(1)*4u),v8);
  }

  w_u32((a1+(19)*4u),a2);
  sub_80032E50(a1,0x800FF45Cu);
  return a1;
}



void sub_8001E740(uint32 a1, uint32 a2, uint32 a3)
{
  sint32 v5;
  uint32 v6;
  sint32 v7;
  uint32 v8;
  uint32 v9;
  signed int v10;
  sint32 v11;
  sint32 v12;
  sint32 v13;
  sint32 vars0;
  if (a1)
  {
    if (((r_u16(((uint32)(a1))) & 1) == 0))
    {
      v5 = r_u32(0x800FF794u);
      if (r_u32(0x800FF794u))
      {
        do
        {
          if ((v5 == a1))
            break;
          v5 = r_u32(((uint32)(((uint32)(v5) + (uint32)(28)))));
        }
        while (v5);
        if (v5)
        {
          v6 = ((sint32)(r_u32(((uint32)(((uint32)(((uint32)(4) * (uint32)(r_u16(((uint32)(((uint32)(a1) + (uint32)(22)))))))) + (uint32)(r_u32((0x800EAEF8u+(((uint32)(((uint32)(16) * (uint32)(r_u8(((uint32)(((uint32)(a1) + (uint32)(27)))))))) + (uint32)(4)))*4u)))))))));
          v7 = ((sint32)(r_u32(v6)));
          v8 = ((((sint32)(r_u32(v6)))>>8)&255u);
          if (((((sint32)(r_u32(v6))) & 2) != 0))
          {
            v9 = ((((sint32)(r_u32(v6)))>>8)&255u);
            v10 = ((((sint32)(r_u32(v6)))>>8)&255u);
            if ((((((sint32)(r_u32(v6)))>>8)&255u) || (a3 == 0xFFFF)))
            {
              sub_8001BCDC(a1,0xFFu,0xFFu,0xFFu,3);
              v9 = ((uint32)(v8) - (uint32)(a3));
              if ((((sint32)(a3)) >= ((sint32)(v10))))
              {
                v9 = 0;
                sub_8001CC1C(a1);
              }
              v7 = ((sint32)(r_u32(v6)));
            }
            w_u32(v6,((v7 & 0xFFFFFF) | ((uint32)(v9) << (uint32)(24))));
          }
          else
            if (a2)
          {
            sub_80021CA8(a1,a2,1,1,1,1,1);
          }
        }
      }
    }
  }
}



uint32 sub_80034D88(uint32 a1)
{
  sint32 result;
  sub_8003319C(a1);
  w_u32(((uint32)(((uint32)(a1) + (uint32)(68)))),0x800A1CD8u);
  sub_80032E50(((uint32)(a1)),0x800FF438u);
  result = a1;
  w_u8(((uint32)(((uint32)(a1) + (uint32)(109)))),32);
  w_u16(((uint32)(((uint32)(a1) + (uint32)(102)))),1);
  w_u32(((uint32)(((uint32)(a1) + (uint32)(104)))),268439552);
  return result;
}



uint32 sub_800358B4(uint32 a1, uint32 a2, uint32 a3, uint32 a4, uint32 a5, uint32 a6, uint32 a7, uint32 a8, uint32 a9, uint32 a10, uint32 a11)
{
  sint32 v19;
  sint32 v20;
  sint32 v21;
  sub_80034D88(a1);
  w_u32(((uint32)(((uint32)(a1) + (uint32)(68)))),0x800A1C30u);
  v19 = r_u32((a2+(1)*4u));
  v20 = r_u32((a2+(2)*4u));
  w_u32(((uint32)(((uint32)(a1) + (uint32)(24)))),r_u32(a2));
  w_u32(((uint32)(((uint32)(a1) + (uint32)(28)))),v19);
  w_u32(((uint32)(((uint32)(a1) + (uint32)(32)))),v20);
  sub_80033398(a1,a6);
  v21 = a1;
  if (a8)
  {
    sub_800332A4(a1);
    v21 = a1;
  }
  sub_800332FC(v21,a3,a4,a5);
  w_u16(((uint32)(((uint32)(a1) + (uint32)(94)))),a7);
  w_u32(((uint32)(((uint32)(a1) + (uint32)(36)))),((uint32)(((uint32)(sub_80066570(((uint32)(((uint32)(2) * (uint32)(a9))) + (uint32)(1)))) - (uint32)(a9))) << (uint32)(12)));
  w_u32(((uint32)(((uint32)(a1) + (uint32)(40)))),((uint32)(-4096) * (uint32)(sub_80066570(a9))));
  w_u32(((uint32)(((uint32)(a1) + (uint32)(44)))),((uint32)(((uint32)(sub_80066570(((uint32)(((uint32)(2) * (uint32)(a9))) + (uint32)(1)))) - (uint32)(a9))) << (uint32)(12)));
  w_u32(((uint32)(((uint32)(a1) + (uint32)(52)))),a10);
  w_u8(((uint32)(((uint32)(a1) + (uint32)(60)))),3);
  w_u8(((uint32)(((uint32)(a1) + (uint32)(61)))),3);
  w_u8(((uint32)(((uint32)(a1) + (uint32)(62)))),3);
  w_u16(((uint32)(((uint32)(a1) + (uint32)(10)))),sub_80066570(a11));
  return a1;
}


/* TODO Missing host buffer adapter xport_draft_host_sub_800665CC_p1 */
/* TODO Missing host buffer adapter xport_draft_host_sub_800667CC_p3 */
/* TODO Missing host buffer adapter xport_draft_host_sub_8006C1E8_p2 */
/* TODO Missing host buffer adapter xport_draft_host_sub_8006C34C_p1 */
/* TODO Missing host buffer adapter xport_draft_host_sub_8006C3AC_p1 */
uint32 sub_8002396C(uint32 a1)
{
  sint32 v2;
  uint32 v3;
  sint32 v4;
  sint32 v5;
  sint32 v6;
  sint32 v7;
  sint32 v8;
  sint32 v9;
  sint32 v10;
  sint32 v11;
  uint32 v12;
  sint32 v13;
  sint32 v14;
  sint32 result;
  sint32 v16;
  sint32 v17;
  int v18[4];
  sint32 v19;
  v2 = r_u32((a1+(28)*4u));
  v3 = (a1+(6)*4u);
  if ((((uint32)(((uint32)(r_u32(0x800FF650u)) - (uint32)(v2)))) >= 0x78))
  {
    result = (((uint32)(((uint32)(r_u32(0x800FF650u)) - (uint32)(v2)))) < 0x79);
    if ((((uint32)(((uint32)(r_u32(0x800FF650u)) - (uint32)(v2)))) >= 0x79))
      return sub_80032ED8(a1);
  }
  else
  {
    sub_8006C0B8((a1+(6)*4u),(a1+(9)*4u));
    v4 = r_u32((a1+(27)*4u));
    if ((((sint32)(v4)) < r_u32((a1+(7)*4u))))
    {
      v5 = r_u32((a1+(10)*4u));
      w_u32((a1+(7)*4u),v4);
      v19 = 1;
      w_u32((a1+(10)*4u),-v5);
      xport_draft_host_sub_8006C1E8_p2((a1+(9)*4u),&v19);
      if ((((uint32)(((uint32)(r_u32((a1+(10)*4u))) + (uint32)(0x2000)))) >= 0x4001))
      {
        v17 = ((v17 & 0xFFFF0000u)|(((0) & 0xFFFFu)<<0));
        v16 = ((v16 & 0xFFFF0000u)|(((0) & 0xFFFFu)<<0));
        v16 = ((v16 & 0x0000FFFFu)|(((sub_80066570(4096)) & 0xFFFFu)<<16));
        if (!sub_80066570(4))
        {
          v6 = sub_80066570(3);
          sub_80069EF4(((uint32)(v6) + (uint32)(12)),v3,0);
        }
        xport_draft_host_sub_800667CC_p3((a1+(24)*4u),3,&v16);
        v7 = sub_80066570(1025);
        xport_draft_host_sub_800665CC_p1(v18,(a1+(9)*4u),((uint32)(v7) - (uint32)(512)));
        v8 = v18[1];
        v9 = v18[2];
        w_u32((a1+(9)*4u),v18[0]);
        w_u32((a1+(10)*4u),v8);
        w_u32((a1+(11)*4u),v9);
      }
    }
    xport_draft_host_sub_8006C34C_p1(&v16,(a1+(6)*4u),(a1+(24)*4u));
    v10 = v17;
    v11 = v18[0];
    w_u32((a1+(21)*4u),v16);
    w_u32((a1+(22)*4u),v10);
    w_u32((a1+(23)*4u),v11);
    v12 = ((uint32)(r_u32((a1+(20)*4u))));
    xport_draft_host_sub_8006C3AC_p1(&v16,(a1+(6)*4u),(a1+(24)*4u));
    v13 = v17;
    v14 = v18[0];
    w_u32(v12,v16);
    w_u32((v12+(1)*4u),v13);
    w_u32((v12+(2)*4u),v14);
    result = ((uint32)(r_u32((a1+(10)*4u))) + (uint32)(14792));
    w_u32((a1+(10)*4u),result);
  }
  return result;
}



uint32 sub_80035A00(uint32 a1, uint32 a2)
{
  uint32 result;
  sint32 v5;
  w_u32((a1+(17)*4u),0x800A1C30u);
  result = sub_80034DE8(a1,0);
  if (((a2 & 1) != 0))
    return ((uint32)(sub_80032E30(((sint32)(a1)))));
  return result;
}



uint32 sub_80032880(uint32 a1, uint32 a2)
{
  sint32 result;
  w_u32(((uint32)(((uint32)(a1) + (uint32)(68)))),0x800A1B08u);
  sub_80032E7C(a1,0x800FF464u);
  result = sub_80032FB8(a1,0);
  if (((a2 & 1) != 0))
    return sub_80032E30(a1);
  return result;
}



uint32 sub_8003554C(uint32 a1, uint32 a2)
{
  uint32 result;
  sint32 v5;
  w_u32((a1+(17)*4u),0x800A1C78u);
  result = sub_80034DE8(a1,0);
  if (((a2 & 1) != 0))
    return ((uint32)(sub_80032E30(((sint32)(a1)))));
  return result;
}



uint32 sub_80022318(uint32 a1, uint32 a2, uint32 a3)
{
  sint32 v6;
  sint32 v7;
  uint32 v8;
  sint32 v9;
  sint32 v10;
  uint32 v11;
  sint32 v12;
  sint32 v13;
  sint32 result;
  sint32 v15;
  sint32 vars0;
  sint32 vars4;
  sint32 vars8;
  v6 = 0;
  v7 = 0;
  v8 = r_u32(((uint32)(((uint32)(((uint32)(4) * (uint32)(r_u16(((uint32)(((uint32)(a1) + (uint32)(22)))))))) + (uint32)(r_u32((0x800EAEF8u+(((uint32)(((uint32)(16) * (uint32)(r_u8(((uint32)(((uint32)(a1) + (uint32)(27)))))))) + (uint32)(4)))*4u)))))));
  v9 = 0;
  v10 = r_u32((v8+(3)*4u));
  v11 = ((uint32)((v8+(((uint32)(((uint32)(((uint32)(2) * (uint32)(r_u32((v8+(1)*4u))))) + (uint32)(8))) + (uint32)(((uint32)(2) * (uint32)(r_u32((v8+(2)*4u)))))))*4u)));
  while ((((sint32)(v9)) < ((sint32)(v10))))
  {
    v12 = sub_80066570(5);
    v13 = sub_80021CA8(a1,v11,(v12 == 0),0,a2,a3,(((sint32)(v7)) < 6));
    if (v13)
    {
      v6 = 1;
      if ((v13 == 1))
        ++v7;
    }
    ++v9;
    v11 = ((uint32)((((uint32)(v11))+((((sint16)(r_u16((v11+(1)*2u)))) & 0xFFFC))*1u)));
  }

  result = v6;
  if ((a3 && v6))
  {
    w_u16(((uint32)(a1)),(r_u16(((uint32)(a1)))|(1u)));
    return v6;
  }
  return result;
}


/* TODO Missing host buffer adapter xport_draft_host_sub_80020FB4_p2345 */
/* TODO Missing host buffer adapter xport_draft_host_sub_80021358_p3 */
/* TODO Missing host buffer adapter xport_draft_host_sub_8006BF04_p1 */
/* TODO Missing host buffer adapter xport_draft_host_sub_8006C22C_p12 */
/* TODO Missing host buffer adapter xport_draft_host_sub_8006C34C_p13 */
/* TODO Missing host buffer adapter xport_draft_host_sub_8006C3AC_p1 */
/* TODO Missing host buffer adapter xport_draft_host_sub_8006C40C_p123 */
/* TODO Missing host buffer adapter xport_draft_host_sub_8006C564_p123 */
/* TODO Missing host buffer adapter xport_draft_host_sub_8006C564_p13 */
/* TODO Missing host buffer adapter xport_draft_host_sub_8006C730_p12 */
/* TODO Missing host buffer adapter xport_draft_host_sub_8006CBF8_p12 */
/* TODO Missing host buffer adapter xport_draft_host_sub_8006CCE0_p123 */
uint32 sub_80021358(uint32 a1, uint32 a2, uint32 a3, uint32 a4, uint32 a5, uint32 a6, uint32 a7, uint32 a8, uint32 a9, uint32 a10, uint32 a11)
{
    sint32 vector[3], difference[3], scaled[3], midpoint[3];
    sint32 points[3][3], center[3];
    sint16 vertices[3][4], jitter[4], shifted_jitter[4];
    sint32 length, candidate, factor, shift;
    uint32 edge = 0u, object, i, component;
    uint32 u, v, result = r_u32(0x800FF738u);
    if (result != 0u)
        return result;
    if (a11 != 0u)
    {
        xport_draft_host_sub_8006C3AC_p1(vector, a2, a3);
        length = (sint32)xport_draft_host_sub_8006BF04_p1(vector);
        xport_draft_host_sub_8006C3AC_p1(vector, a1, a3);
        candidate = (sint32)xport_draft_host_sub_8006BF04_p1(vector);
        if (length < candidate)
        {
            length = candidate;
            edge = 1u;
        }
        xport_draft_host_sub_8006C3AC_p1(vector, a1, a2);
        if (length < (sint32)xport_draft_host_sub_8006BF04_p1(vector))
            edge = 2u;
        factor = (sint32)sub_80066570(9u);
        if (edge == 1u)
            xport_draft_host_sub_8006C3AC_p1(difference, a3, a1);
        else if (edge == 2u)
            xport_draft_host_sub_8006C3AC_p1(difference, a2, a1);
        else
            xport_draft_host_sub_8006C3AC_p1(difference, a3, a2);
        xport_draft_host_sub_8006C40C_p123(scaled, difference, &factor);
        shift = 3;
        xport_draft_host_sub_8006C564_p123(midpoint, scaled, &shift);
        xport_draft_host_sub_8006C34C_p13(vector, edge == 0u ? a2 : a1, midpoint);
        if (edge == 1u)
        {
            u = a4 + (uint32)((sint32)((a8 - a4) * (uint32)factor) >> 3);
            v = a5 + (uint32)((sint32)((a9 - a5) * (uint32)factor) >> 3);
            xport_draft_host_sub_80021358_p3(a2, a1, vector, a6, a7, a4, a5, u, v, a10, a11 - 1u);
            return xport_draft_host_sub_80021358_p3(a2, a3, vector, a6, a7, u, v, a8, a9, a10, a11 - 1u);
        }
        if (edge == 2u)
        {
            u = a4 + (uint32)((sint32)((a6 - a4) * (uint32)factor) >> 3);
            v = a5 + (uint32)((sint32)((a7 - a5) * (uint32)factor) >> 3);
            xport_draft_host_sub_80021358_p3(a3, a1, vector, a8, a9, a4, a5, u, v, a10, a11 - 1u);
            return xport_draft_host_sub_80021358_p3(a3, a2, vector, a8, a9, u, v, a6, a7, a10, a11 - 1u);
        }
        u = a6 + (uint32)((sint32)((a8 - a6) * (uint32)factor) >> 3);
        v = a7 + (uint32)((sint32)((a9 - a7) * (uint32)factor) >> 3);
        xport_draft_host_sub_80021358_p3(a1, a2, vector, a4, a5, a6, a7, u, v, a10, a11 - 1u);
        return xport_draft_host_sub_80021358_p3(a1, a3, vector, a4, a5, u, v, a8, a9, a10, a11 - 1u);
    }
    shift = 12;
    xport_draft_host_sub_8006C564_p13(points[0], a1, &shift);
    xport_draft_host_sub_8006C564_p13(points[1], a2, &shift);
    xport_draft_host_sub_8006C564_p13(points[2], a3, &shift);
    for (component = 0u; component < 3u; ++component)
    {
        center[component] = (sint32)((uint32)points[0][component] +
            (uint32)points[1][component] + (uint32)points[2][component]) / 3;
        for (i = 0u; i < 3u; ++i)
            vertices[i][component] = (sint16)((uint32)points[i][component] - (uint32)center[component]);
    }
    for (i = 0u; i < 3u; ++i)
    {
        factor = (sint32)sub_80066570(100u);
        xport_draft_host_sub_8006CBF8_p12(jitter, &factor, r_u32(0x800FF210u));
        xport_draft_host_sub_8006CCE0_p123(shifted_jitter, jitter, &shift);
        xport_draft_host_sub_8006C730_p12(vertices[i], shifted_jitter);
    }
    xport_draft_host_sub_8006C22C_p12(center, &shift);
    w_u32(0x800FF3ACu, 0u);
    object = sub_80032DC0(204u);
    if (object != 0u)
        object = xport_draft_host_sub_80020FB4_p2345(object, vertices[0], vertices[1], vertices[2], center);
    w_u32(0x800FF3ACu, 1u);
    sub_80034314(object, r_u8(0x800FFB80u), r_u8(0x800FFB81u), r_u8(0x800FFB82u));
    result = sub_8003445C(object, r_u16(0x800FFB78u), r_u16(0x800FFB7Cu),
        (uint8)a4, (uint8)a5, (uint8)a6, (uint8)a7, (uint8)a8, (uint8)a9);
    w_u32(object + 188u, a10);
    return result;
}


uint32 sub_80020FB4(uint32 a1, uint32 a2, uint32 a3, uint32 a4, uint32 a5)
{
  short v10;
  sint32 v11;
  sint32 result;
  sint32 v13;
  sub_800340A4(a1,a2,a3,a4);
  w_u32(((uint32)(((uint32)(a1) + (uint32)(68)))),0x800A12C8u);
  if (sub_80066570(2))
  {
    w_u16(((uint32)(((uint32)(a1) + (uint32)(192)))),((uint32)(sub_80066570(160)) + (uint32)(80)));
    w_u16(((uint32)(((uint32)(a1) + (uint32)(194)))),0);
    w_u16(((uint32)(((uint32)(a1) + (uint32)(196)))),0);
  }
  else
  {
    v10 = sub_80066570(160);
    w_u16(((uint32)(((uint32)(a1) + (uint32)(192)))),0);
    w_u32(((uint32)(((uint32)(a1) + (uint32)(194)))),((unsigned short)(((uint32)(v10) + (uint32)(80)))));
  }
  w_u16(((uint32)(((uint32)(a1) + (uint32)(10)))),((uint32)(sub_80066570(30)) + (uint32)(45)));
  sub_80021154(a1,a5);
  sub_800344BC(a1);
  v11 = (sub_80066570(2) == 0);
  result = a1;
  if (!v11)
  {
    result = a1;
    if (!r_u32(0x800FF738u))
    {
      v13 = sub_80032DC0(100);
      if (v13)
        v13 = sub_80036B00(v13,a1+24u,4,128,128,128);
      w_u32(((uint32)(((uint32)(a1) + (uint32)(200)))),v13);
      w_u8(((uint32)(((uint32)(v13) + (uint32)(66)))),1);
      sub_80036988(r_u32(((uint32)(((uint32)(a1) + (uint32)(200))))),1200);
      sub_800368F8(r_u32(((uint32)(((uint32)(a1) + (uint32)(200))))),32,32,32);
      return a1;
    }
  }
  return result;
}


/* TODO Missing host buffer adapter xport_draft_host_sub_800666DC_p12 */
uint32 sub_800340A4(uint32 a1, uint32 a2, uint32 a3, uint32 a4)
{
  sint32 v8;
  sint32 v10;
  sint32 v11;
  sint32 v12;
  sint32 v13;
  sint32 v14;
  sint32 v15;
  int v16[4];
  int v17[4];
  sub_80032F7C(a1);
  w_u32(((uint32)(((uint32)(a1) + (uint32)(68)))),0x800A1D20u);
  w_u8(((uint32)(((uint32)(a1) + (uint32)(67)))),6);
  w_u16(((uint32)(((uint32)(a1) + (uint32)(72)))),((sint16)(r_u16(a2))));
  w_u16(((uint32)(((uint32)(a1) + (uint32)(74)))),((sint16)(r_u16((a2+(1)*2u)))));
  w_u16(((uint32)(((uint32)(a1) + (uint32)(76)))),((sint16)(r_u16((a2+(2)*2u)))));
  w_u16(((uint32)(((uint32)(a1) + (uint32)(80)))),((sint16)(r_u16(a3))));
  w_u16(((uint32)(((uint32)(a1) + (uint32)(82)))),((sint16)(r_u16((a3+(1)*2u)))));
  w_u16(((uint32)(((uint32)(a1) + (uint32)(84)))),((sint16)(r_u16((a3+(2)*2u)))));
  w_u16(((uint32)(((uint32)(a1) + (uint32)(88)))),((sint16)(r_u16(a4))));
  w_u16(((uint32)(((uint32)(a1) + (uint32)(90)))),((sint16)(r_u16((a4+(1)*2u)))));
  w_u16(((uint32)(((uint32)(a1) + (uint32)(92)))),((sint16)(r_u16((a4+(2)*2u)))));
  v10 = ((sint32)(((uint32)(((sint16)(r_u16(a4)))) - (uint32)(((sint16)(r_u16(a2)))))));
  v11 = ((sint32)(((uint32)(((sint16)(r_u16((a4+(1)*2u))))) - (uint32)(((sint16)(r_u16((a2+(1)*2u))))))));
  v12 = ((sint32)(((uint32)(((sint16)(r_u16((a4+(2)*2u))))) - (uint32)(((sint16)(r_u16((a2+(2)*2u))))))));
  v13 = ((sint32)(((uint32)(((sint16)(r_u16(a3)))) - (uint32)(((sint16)(r_u16(a2)))))));
  v14 = ((sint32)(((uint32)(((sint16)(r_u16((a3+(1)*2u))))) - (uint32)(((sint16)(r_u16((a2+(1)*2u))))))));
  v15 = ((sint32)(((uint32)(((sint16)(r_u16((a3+(2)*2u))))) - (uint32)(((sint16)(r_u16((a2+(2)*2u))))))));
  v16[0] = ((uint32)(((sint32)(((uint32)(v11) * (uint32)(v15))))) - (uint32)(((sint32)(((uint32)(v12) * (uint32)(v14))))));
  v16[1] = ((uint32)(((sint32)(((uint32)(v12) * (uint32)(v13))))) - (uint32)(((sint32)(((uint32)(v10) * (uint32)(v15))))));
  v16[2] = ((uint32)(((sint32)(((uint32)(v10) * (uint32)(v14))))) - (uint32)(((sint32)(((uint32)(v11) * (uint32)(v13))))));
  xport_draft_host_sub_800666DC_p12(v17,v16);
  v8 = ((uint32)(sub_80066570(96)) + (uint32)(128));
  w_u16(((uint32)(((uint32)(a1) + (uint32)(96)))),(((sint32)(((uint32)(v17[0]) * (uint32)(v8)))) >> 12));
  w_u16(((uint32)(((uint32)(a1) + (uint32)(98)))),(((sint32)(((uint32)(v17[1]) * (uint32)(v8)))) >> 12));
  w_u16(((uint32)(((uint32)(a1) + (uint32)(100)))),(((sint32)(((uint32)(v17[2]) * (uint32)(v8)))) >> 12));
  sub_80032E50(((uint32)(a1)),0x800FF44Cu);
  return a1;
}



uint32 sub_80036634(uint32 a1, uint32 a2, uint32 a3, uint32 a4, uint32 a5, uint32 a6, uint32 a7, uint32 a8, uint32 a9, uint32 a10, uint32 a11, uint32 a12)
{
  sint32 v16;
  sint32 v17;
  sint32 v18;
  sint32 v19;
  sint32 v20;
  uint32 v21;
  sint32 v22;
  sint32 v23;
  sint32 v24;
  sint32 v25;
  sint32 v26;
  uint32 v27;
  uint32 v28;
  sub_800330F4(((uint32)(a1)));
  w_u32(((uint32)(((uint32)(a1) + (uint32)(72)))),a3);
  --a4;
  w_u32(((uint32)(((uint32)(a1) + (uint32)(68)))),0x800A1BC8u);
  w_u32(((uint32)(((uint32)(a1) + (uint32)(76)))),a4);
  v16 = ((uint32)(((sint32)(((uint32)(a3) * (uint32)(a4))))) + (uint32)(1));
  w_u32(((uint32)(((uint32)(a1) + (uint32)(80)))),v16);
  v17 = sub_8006B864(((uint32)(12) * (uint32)(v16)),0,1);
  v18 = r_u32(((uint32)(((uint32)(a1) + (uint32)(80)))));
  v19 = 0;
  w_u32(((uint32)(((uint32)(a1) + (uint32)(88)))),v17);
  if ((((sint32)(v18)) > 0))
  {
    v20 = 0;
    do
    {
      ++v19;
      v21 = ((uint32)(((uint32)(v20) + (uint32)(r_u32(((uint32)(((uint32)(a1) + (uint32)(88)))))))));
      v22 = r_u32((a2+(1)*4u));
      v23 = r_u32((a2+(2)*4u));
      w_u32(v21,r_u32(a2));
      w_u32((v21+(1)*4u),v22);
      w_u32((v21+(2)*4u),v23);
      v20 += 12;
    }
    while ((((sint32)(v19)) < r_u32(((uint32)(((uint32)(a1) + (uint32)(80)))))));
  }
  v24 = sub_8006B864(((uint32)(4) * (uint32)(a3)),0,1);
  v25 = ((uint32)(r_u32(((uint32)(((uint32)(a1) + (uint32)(72)))))) - (uint32)(1));
  for (w_u32(((uint32)(((uint32)(a1) + (uint32)(92)))),v24); (((sint32)(v25)) >= 0); --v25)
  {
    v26 = ((uint32)(4) * (uint32)(v25));
    v28 = ((uint32)(((uint32)(((uint32)(4) * (uint32)(v25))) + (uint32)(r_u32(((uint32)(((uint32)(a1) + (uint32)(92)))))))));
    v27 = ((uint32)(sub_80032DC0(120)));
    if (v27)
      v27 = sub_8003658C(v27);
    w_u32(v28,v27);
    w_u8(((uint32)(((uint32)(r_u32(((uint32)(((uint32)(v26) + (uint32)(r_u32(((uint32)(((uint32)(a1) + (uint32)(92))))))))))) + (uint32)(66)))),1);
    sub_80033354(r_u32(((uint32)(((uint32)(v26) + (uint32)(r_u32(((uint32)(((uint32)(a1) + (uint32)(92)))))))))),a9);
    sub_80033290(r_u32(((uint32)(((uint32)(v26) + (uint32)(r_u32(((uint32)(((uint32)(a1) + (uint32)(92)))))))))),a11);
    if (a12)
      sub_800332A4(r_u32(((uint32)(((uint32)(v26) + (uint32)(r_u32(((uint32)(((uint32)(a1) + (uint32)(92)))))))))));
  }

  w_u8(((uint32)(((uint32)(r_u32(r_u32(((uint32)(((uint32)(a1) + (uint32)(92))))))) + (uint32)(88)))),(r_u8(((uint32)(((uint32)(r_u32(r_u32(((uint32)(((uint32)(a1) + (uint32)(92))))))) + (uint32)(88)))))|(0x10u)));
  sub_80033354(r_u32(r_u32(((uint32)(((uint32)(a1) + (uint32)(92)))))),a10);
  return a1;
}



uint32 sub_800330F4(uint32 a1)
{
  sub_80032F7C(((sint32)(a1)));
  w_u32((a1+(17)*4u),0x800A1D80u);
  sub_80032E50(a1,0x800FF434u);
  return a1;
}



uint32 sub_8001D320(uint32 a1, uint32 a2, uint32 a3, uint32 a4, uint32 a5, uint32 a6, uint32 a7, uint32 a8)
{
  sint32 result;
  result = r_u32(0x800FF738u);
  if (!r_u32(0x800FF738u))
  {
    if (a7)
    {
      result = sub_80032DC0(124);
      if (result)
        return sub_8001CF9C(result,a1,a6,a3,a4,a5,2,a3,a4,a5,20,a8,0,0,a2,a2,(((sint32)(a2)) / 2),(((sint32)(a2)) / 2),0,0);
    }
    else
    {
      result = sub_80032DC0(124);
      if (result)
        return sub_8001CF9C(result,a1,a6,a3,a4,a5,2,a3,a4,a5,20,a8,0,0,a2,0,a2,0,0,0);
    }
  }
  return result;
}



uint32 sub_800369F8(uint32 a1, uint32 a2)
{
  sint32 v3;
  sint32 v4;
  uint32 v5;
  sint32 v6;
  sint32 v7;
  uint32 result;
  sint32 v9;
  sint32 v10;
  sint32 i;
  uint32 v12;
  uint32 v13;
  sint32 v14;
  sint32 v15;
  uint32 v16;
  sint32 v17;
  sint32 v18;
  v3 = r_u32((a1+(20)*4u));
  v4 = ((uint32)(r_u32((a1+(21)*4u))) + (uint32)(1));
  w_u32((a1+(21)*4u),v4);
  if ((v4 == v3))
    w_u32((a1+(21)*4u),0);
  v5 = ((uint32)(((uint32)(((uint32)(12) * (uint32)(r_u32((a1+(21)*4u))))) + (uint32)(r_u32((a1+(22)*4u))))));
  v6 = r_u32((a2+(1)*4u));
  v7 = r_u32((a2+(2)*4u));
  w_u32(v5,r_u32(a2));
  w_u32((v5+(1)*4u),v6);
  w_u32((v5+(2)*4u),v7);
  result = ((uint32)(r_u32((a1+(18)*4u))));
  v9 = r_u32((a1+(21)*4u));
  v10 = ((uint32)(((sint32)(result))) - (uint32)(1));
  for (i = ((uint32)(4) * (uint32)(((uint32)(((uint32)(result))) - (uint32)(1)))); (((sint32)(v10)) >= 0); i = ((uint32)(4) * (uint32)(v10)))
  {
    v12 = r_u32(((uint32)(((uint32)(i) + (uint32)(r_u32((a1+(23)*4u)))))));
    v13 = ((uint32)(((uint32)(((uint32)(12) * (uint32)(v9))) + (uint32)(r_u32((a1+(22)*4u))))));
    v14 = r_u32((v13+(1)*4u));
    v15 = r_u32((v13+(2)*4u));
    w_u32((v12+(27)*4u),r_u32(v13));
    w_u32((v12+(28)*4u),v14);
    w_u32((v12+(29)*4u),v15);
    v9 -= r_u32((a1+(19)*4u));
    if ((((sint32)(v9)) < 0))
      v9 += r_u32((a1+(20)*4u));
    --v10;
    v16 = r_u32(((uint32)(((uint32)(i) + (uint32)(r_u32((a1+(23)*4u)))))));
    result = ((uint32)(((uint32)(((uint32)(12) * (uint32)(v9))) + (uint32)(r_u32((a1+(22)*4u))))));
    v17 = r_u32((result+(1)*4u));
    v18 = r_u32((result+(2)*4u));
    w_u32((v16+(24)*4u),r_u32(result));
    w_u32((v16+(25)*4u),v17);
    w_u32((v16+(26)*4u),v18);
  }

  return result;
}


/* TODO Missing call adapter indirect */
uint32 sub_8001D9D0(uint32 a1)
{
  sint32 result;
  sint32 v3;
  sint32 v4;
  sint32 v5;
  sint32 v6;
  sint32 v7;
  result = ((result & 0xFFFF0000u)|(((((uint32)(r_u16(((uint32)(((uint32)(a1) + (uint32)(8)))))) + (uint32)(1))) & 0xFFFFu)<<0));
  w_u16(((uint32)(((uint32)(a1) + (uint32)(8)))),result);
  result = ((short)(result));
  v3 = 1;
  if ((((short)(result)) != 1))
  {
    result = r_u32(((uint32)(((uint32)(a1) + (uint32)(152)))));
    v4 = 0;
    if ((((sint32)(result)) > 0))
    {
      v5 = a1;
      do
      {
        v6 = r_u32(((uint32)(((uint32)(v5) + (uint32)(72)))));
        if (v6)
        {
          sub_8006C0B8(((uint32)(v6) + (uint32)(24)),((uint32)(v6) + (uint32)(36)));
          sub_8006C0B8(((uint32)(r_u32(((uint32)(((uint32)(v5) + (uint32)(72)))))) + (uint32)(36)),((uint32)(r_u32(((uint32)(((uint32)(v5) + (uint32)(72)))))) + (uint32)(48)));
          sub_8006C270(((uint32)(r_u32(((uint32)(((uint32)(v5) + (uint32)(72)))))) + (uint32)(36)),((uint32)(r_u32(((uint32)(((uint32)(v5) + (uint32)(72)))))) + (uint32)(60)));
          w_u16(((uint32)(((uint32)(r_u32(((uint32)(((uint32)(v5) + (uint32)(72)))))) + (uint32)(96)))),(r_u16(((uint32)(((uint32)(r_u32(((uint32)(((uint32)(v5) + (uint32)(72)))))) + (uint32)(96)))))+(r_u16(((uint32)(((uint32)(r_u32(((uint32)(((uint32)(v5) + (uint32)(72)))))) + (uint32)(98))))))));
          w_u16(((uint32)(((uint32)(r_u32(((uint32)(((uint32)(v5) + (uint32)(72)))))) + (uint32)(98)))),(r_u16(((uint32)(((uint32)(r_u32(((uint32)(((uint32)(v5) + (uint32)(72)))))) + (uint32)(98)))))+(r_u16(((uint32)(((uint32)(r_u32(((uint32)(((uint32)(v5) + (uint32)(72)))))) + (uint32)(100))))))));
          w_u16(((uint32)(((uint32)(r_u32(((uint32)(((uint32)(v5) + (uint32)(72)))))) + (uint32)(94)))),(r_u16(((uint32)(((uint32)(r_u32(((uint32)(((uint32)(v5) + (uint32)(72)))))) + (uint32)(94)))))+(r_u16(((uint32)(((uint32)(r_u32(((uint32)(((uint32)(v5) + (uint32)(72)))))) + (uint32)(74))))))));
          v3 = 0;
          if ((r_u32(((uint32)(((uint32)(a1) + (uint32)(156))))) < ((sint32)(((sint16)(r_u16(((uint32)(((uint32)(a1) + (uint32)(8)))))))))))
          {
            sub_80032D3C(((uint32)(r_u32(((uint32)(((uint32)(v5) + (uint32)(72)))))) + (uint32)(76)),((sint16)(r_u16(((uint32)(((uint32)(r_u32(((uint32)(((uint32)(v5) + (uint32)(72)))))) + (uint32)(72))))))));
            v7 = r_u32(((uint32)(((uint32)(v5) + (uint32)(72)))));
            if (((r_u32(((uint32)(((uint32)(v7) + (uint32)(76))))) & 0xFFFFFF) == 0))
            {
              if (v7)
                ((void)(((sint32)(((uint32)(v7) + (uint32)(((sint16)(r_u16(((uint32)(((uint32)(r_u32(((uint32)(((uint32)(v7) + (uint32)(68)))))) + (uint32)(8)))))))))))),(void)(3),abort(),0u);
              w_u32(((uint32)(((uint32)(v5) + (uint32)(72)))),0);
            }
          }
        }
        result = (++v4 < r_u32(((uint32)(((uint32)(a1) + (uint32)(152))))));
        v5 += 4;
      }
      while ((((sint32)(v4)) < r_u32(((uint32)(((uint32)(a1) + (uint32)(152)))))));
    }
    if (v3)
      return sub_80032ED8(a1);
  }
  return result;
}



uint32 sub_800762DC(uint32 a1, uint32 a2)
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
  w_u32((result+(1)*4u),0);
  w_u32((result+(2)*4u),v5);
  w_u32((result+(3)*4u),v4);
  return result;
}


