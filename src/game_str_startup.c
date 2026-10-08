#include "game_movie_startup.h"
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
#include "psx_spu.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
extern sint32 cd_read_sector_native(uint8 output[2352]);
static uint32 active;
static void str_todo(const char *s){fprintf(stderr,"TODO STR: %s\n",s);abort();}
static void clear_headers(uint32 first,uint32 count){uint32 i;for(i=0;i<count;++i)w_u32(r_u32(0x80105850u)+32*(first+i),0);}
uint32 sub_80097B1C(uint32 ring,uint32 count)
{
    w_u32(0x80105850u,ring);w_u32(0x80105854u,count);
    w_u32(0x8010583Cu,0);w_u32(0x80105838u,0);w_u32(0x80105834u,0);w_u32(0x8010582Cu,0);
    clear_headers(0,count);w_u32(0x8010581Cu,0);w_u16(0x80105814u,0);w_u32(0x80105810u,0);
    if(!count)str_todo("empty ring initialization return");
    return 0;
}
void sub_8009AEEC(uint32 mode,uint32 start,uint32 end,uint32 callback,uint32 argument)
{
    w_u32(0x80105848u,1);w_u32(0x80105824u,start);w_u32(0x80105844u,end);
    w_u32(0x80105840u,0);w_u32(0x80107CA0u,callback);w_u32(0x80105818u,mode&1);
    w_u32(0x80105828u,0);w_u32(0x80105820u,0);w_u16(0x80105814u,0);w_u32(0x80105810u,0);w_u32(0x80107CA4u,argument);

}
uint32 apocalypse_str_start(uint32 mode)
{
    uint8 value=(uint8)mode;
    if(!xport_draft_host_sub_80097DF8_p2(14,&value,0))return 0;
    if(mode&256)w_u32(0x80105888u,(mode&32)==0);
    active=xport_draft_host_sub_80097DF8_p2(27,0,0);return active;
}
uint32 apocalypse_str_pump(void)
{
    uint8 sector[2352];uint32 ring=r_u32(0x80105850u),index=r_u32(0x80105834u),header=ring+32*index;
    uint32 chunks,part,frame,i,target,complete;
    if(r_u32(0x8010582Cu)==1)return 1;
    if(r_u32(0x80105840u))str_todo("memory-backed stream source");
    if(!active)return 5;
    if(r_u16(header)){w_u32(0x800FDE54u,4);return 4;}
    if(!cd_read_sector_native(sector))return 5;
    for(i=0;i<32;++i)w_u8(header+i,sector[24+i]);
    w_u32(header+28,(uint32)sector[12]|((uint32)sector[13]<<8)|((uint32)sector[14]<<16)|((uint32)sector[15]<<24));
    w_u32(0x80107CA8u,header);
    frame=r_u16(header+8);part=r_u16(header+4);chunks=r_u16(header+6);
    if(r_u32(0x80105848u)==1 && r_u32(0x80105824u)) {
        if(r_u32(0x80105824u)!=frame){w_u16(header,0);return 0;}w_u32(0x80105848u,0);
    }
    if(r_u16(header)!=352 || ((r_u16(header+2)>>10)&31)!=r_u32(0x80105828u)){w_u16(header,0);w_u32(0x800FDE54u,5);return 5;}
    if((sint16)r_u16(0x80105814u)!=(sint32)part || (r_u32(0x80105810u)&&r_u32(0x80105810u)!=frame)) {
        w_u32(0x80105810u,0);w_u16(0x80105814u,0);clear_headers(r_u32(0x80105838u),index-r_u32(0x80105838u));w_u32(0x80105834u,r_u32(0x80105838u));w_u16(header,0);w_u32(0x800FDE54u,6);return 6;
    }
    if(!part) {
        w_u16(0x80105814u,0);w_u32(0x80105810u,frame);
        if(r_u32(0x80105844u)&&frame>=r_u32(0x80105844u)) {
            w_u32(0x80105810u,0);clear_headers(r_u32(0x80105838u),index-r_u32(0x80105838u));w_u32(0x80105834u,r_u32(0x80105838u));w_u16(header,0);w_u32(0x80105848u,1);if(r_u32(0x80107CA4u))str_todo("frame limit callback");w_u32(0x800FDE54u,7);return 7;
        }
        if(r_u32(0x80105854u)-index-1<chunks) {
            if(!r_u32(0x80105844u)){w_u16(header,1);w_u32(0x80105848u,1);if(r_u32(0x80107CA4u))str_todo("ring wrap callback");w_u32(0x800FDE54u,8);return 8;}
            if(r_u16(ring)){w_u16(header,0);w_u32(0x800FDE54u,9);return 9;}
            w_u16(header,1);for(i=0;i<8;++i)w_u32(ring+4*i,r_u32(header+4*i));index=0;header=ring;w_u32(0x80105834u,0);w_u32(0x80107CA8u,ring);
        }
        w_u32(0x80105838u,index);
    }
    w_u32(0x800FDE54u,10);w_u16(0x80105814u,r_u16(0x80105814u)+1);
    target=ring+32*r_u32(0x80105854u)+2016*index;w_u32(0x8010584Cu,target);
    for(i=0;i<2016;++i)w_u8(target+i,sector[56+i]);
    complete=chunks-1==part;
    if(complete){w_u32(0x8010582Cu,1);w_u16(0x80105814u,0);w_u32(0x80105810u,0);w_u32(0x80105828u,r_u32(0x80105820u));}
    w_u16(header,3);w_u32(0x80105834u,index+1);
    if(complete){target=ring+32*r_u32(0x80105838u);w_u16(target,2);w_u32(0x80107C90u,r_u32(target+28));w_u32(0x80107C94u,r_u32(target+8));w_u32(0x80105838u,index+1);if(r_u32(0x80107CA0u))str_todo("frame completion callback");w_u32(0x8010582Cu,0);}
    return index+1;
}
uint32 apocalypse_str_next(uint32 *frame,uint32 *header)
{
    uint32 node=r_u32(0x80105850u)+32*r_u32(0x8010583Cu);
    if(r_u16(node)==1){w_u32(0x8010583Cu,0);if(r_u32(0x80105844u))w_u16(node,0);node=r_u32(0x80105850u);}
    if(r_u16(node)!=2){apocalypse_str_pump();return 1;}
    w_u16(node,4);*frame=r_u32(0x80105850u)+32*r_u32(0x80105854u)+2016*r_u32(0x8010583Cu);*header=node;return 0;
}
uint32 apocalypse_str_free(uint32 frame)
{
    uint32 index=(frame-r_u32(0x80105850u)-32*r_u32(0x80105854u))/2016;
    uint32 node=r_u32(0x80105850u)+32*index,count=r_u16(node+6);
    if(r_u16(node)!=4)return 1;clear_headers(index,count);w_u32(0x8010583Cu,index+count);return 0;
}
void apocalypse_str_stop(void){active=0;w_u32(0x800FDA28u,0);}



