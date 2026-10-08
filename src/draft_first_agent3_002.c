#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
#include <stdlib.h>

/* Unverified draft C; integration remains TODO */
uint32 sub_8006805C(void)
{
    w_u16(0x800C6374u, 0); w_u16(0x800A7304u, 0);
    w_u16(0x800C6376u, 0); w_u16(0x800A7306u, 0);
    return 0x800A72A0u;
}
uint32 sub_800771F4(uint32 a1)
{
    w_u32(0x800FFD00u,a1); w_u32(0x800FFD04u,314159265u);
    w_u32(0x800FFD08u,178453311u); return 178453311u;
}
uint32 sub_80066458(void)
{
    uint32 result;
    w_u32(0x800FF64Cu,r_u32(0x800FF64Cu)+1u);
    if (!r_u32(0x800FF008u) && !r_u32(0x800FF300u))
        w_u32(0x800FF650u,r_u32(0x800FF650u)+1u);
    if (r_u32(0x800FF658u)) { sub_800681FC(); w_u32(0x800FF658u,0); }
    result=r_u32(0x800FF65Cu);
    if(result) return sub_800662A0();
    return result;
}
uint32 sub_8006DFA0(void)
{
    uint32 i,p;
    p=0x800EAEE8u;
    for(i=0;i<20u;++i,p-=16u) w_u8(p+13u,0);
    w_u32(0x800FF79Cu,0); w_u32(0x800FF7A0u,0);
    p=0x800EAEF8u;
    for(i=0;i<40u;++i,p+=64u) {
        w_u8(p,0); w_u8(p+11u,0); w_u8(p+10u,0);
        w_u32(p+16u,0); w_u32(p+20u,0); w_u32(p+24u,0);
        w_u32(p+40u,0); w_u32(p+36u,0); w_u32(p+28u,0);
    }
    p=0x800EC0F4u;
    for(i=0;i<512u;++i,p-=4u) w_u32(p,0);
    w_u32(0x800FF7A4u,0); w_u32(0x800FF778u,0xFFFFFFFFu);
    w_u32(0x800FF77Cu,0xFFFFFFFFu); w_u32(0x800FF780u,0);
    w_u32(0x800FF784u,0); w_u32(0x800FF794u,0); w_u32(0x800FF798u,0);
    return 0xFFFFFFFFu;
}
uint32 sub_8006BD14(uint32 a1, uint32 a2)
{
    uint32 block=a1-8u, header=r_u32(a1-4u), result=0xFFFFFFFFu, size, tail;
    sint32 heap=((sint32)(header<<28))>>28;
    if(heap!=-1) {
        if(heap==-2 && block==r_u32(0x800FF730u)) {
            result=(r_u32(a1-4u)&15u)|(16u*a2); w_u32(block+4u,result); return result;
        }
        if(heap!=-2 || block!=r_u32(0x800FF734u)) {
            size=r_u32(block+4u)>>4; result=a2<size-8u; tail=a1+a2;
            if(a2<size-8u) {
                w_u32(tail+4u,(r_u32(tail+4u)&15u)|(16u*(size-a2-8u)));
                w_u32(0x800FF748u+4u*(uint32)heap,r_u32(0x800FF748u+4u*(uint32)heap)-((r_u32(block+4u)>>4)-a2));
                w_u32(block+4u,(r_u32(block+4u)&15u)|(16u*a2));
                result=sub_8006B4B4(tail,(uint32)heap);
                if(!heap) { result=1; w_u32(0x800FF738u,r_u32(0x800FF748u)>=r_u32(0x800FF73Cu)); }
            }
        } else result=r_u32(0x800FF734u);
    }
    return result;
}
uint32 sub_80015374(uint32 a1, uint32 a2)
{
    uint32 i=0,p=a1,v;
    w_u8(a1+6u,a2);
    do {
        v=r_u16(p+(i==r_u8(a1+6u)?32u:34u)); w_u16(p+30u,v);
        v=r_u16(p+30u); ++i;
        w_u16(p+46u,0); w_u16(p+48u,0); w_u8(p+44u,0x80u); w_u16(p+28u,v); p+=28u;
    }while(i<15u);
    return i<15u;
}
uint32 sub_800682CC(uint32 a1, uint32 a2, uint32 a3, uint32 a4)
{
    uint32 p=sub_80068298(0x800FF67Cu);
    w_u16(p,a1); w_u16(p+2u,a2); w_u16(p+4u,a3); w_u16(p+6u,a4); return p;
}
uint32 sub_80068298(uint32 a1)
{
    uint32 p=sub_80068240(); w_u32(p+8u,r_u32(a1)); w_u32(a1,p); return p;
}
uint32 sub_800698FC(void)
{
    sub_8008E10C(); sub_8008E93C(0,0,1); sub_8008E93C(0,1,1); return sub_8008F05C(0,127,127);
}
uint32 sub_8008E10C(void)
{
    sub_8008655C(); sub_8009459C(); sub_800968DC(7); return sub_8008E01C();
}
uint32 sub_8009459C(void) { return sub_800945BC(0); }
uint32 sub_80096DEC(uint32 a1) { w_u32(0x800FD270u,a1!=1u); return 1u; }
uint32 sub_80068084(void)
{
    sub_80087E5C(0x800A72A0u,0,0,512,240);
    sub_80087E5C(0x800C6310u,0,256,512,240);
    sub_80087F10(0x800A72FCu,0,256,512,240);
    sub_80087F10(0x800C636Cu,0,0,512,240);
    sub_8006805C();
    w_u8(0x800A72B8u,1); w_u8(0x800A72B9u,0); w_u8(0x800A72BAu,0); w_u8(0x800A72BBu,0);
    w_u8(0x800C6328u,1); w_u8(0x800C6329u,0); w_u8(0x800C632Au,0); w_u8(0x800C632Bu,0);
    return 1;
}
