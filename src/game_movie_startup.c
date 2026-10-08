#include "game_mdec.h"
#include "game_movie_startup.h"
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
#include "psx.h"

uint32 apocalypse_play_movie(uint32 movie)
{
    DISPENV environment;
    uint8 volume[4], status[8];
    uint32 i, result = r_u32(0x800FF250u);
    sint32 timeout;
    uint16 screen_width;
    if (result) return result;
    w_u32(0x800FF248u,1);
    sub_8002EE7C(movie);
    w_u16(0x800FF28Au,(320-r_u16(0x800FF290u))/2);
    w_u16(0x800FF28Eu,r_u16(0x800FF28Au)+(r_u8(0x800FF298u)?3*r_u16(0x800FF290u)/2:r_u16(0x800FF290u)));
    w_u16(0x800FF28Cu,(240-r_u16(0x800FF292u))/2);
    w_u32(0x800FF24Cu,0);
    result=r_u32(0x800A5B8Cu+28*r_u8(0x800FF299u)+24);
    if (!result) return result;
    w_u16(0x800FF29Cu,r_u16(0x800FF28Au));
    w_u16(0x800FF29Eu,r_u16(0x800FF28Cu));
    w_u16(0x800FF2A0u,r_u8(0x800FF298u)?24:16);
    w_u16(0x800FF2A2u,r_u16(0x800FF292u));
    screen_width=r_u16(0x800A7308u);
    w_u32(0x800FF258u,0); w_u32(0x800FF25Cu,0); w_u32(0x800FF260u,0);
    w_u32(0x800FFB88u,sub_8006B864(69632,1,0));
    if (!r_u32(0x800FFB88u)) goto cleanup;
    for(i=0;i<2;++i) {
        w_u32(0x800FF2A4u+4*i,sub_8006B864(r_u32(0x800FF280u),1,0));
        if(!r_u32(0x800FF2A4u+4*i)) goto cleanup;
    }
    w_u32(0x800FFB84u,sub_8006B864(r_u32(0x800FF284u)<<11,1,0));
    if(!r_u32(0x800FFB84u)) goto cleanup;
    w_u8(0x800FF288u,2);
    for(i=0;i<r_u8(0x800FF288u);++i) {
        w_u32(0x800A5F68u+4*i,sub_8006B864((r_u8(0x800FF298u)?48:32)*r_u16(0x800FF292u),1,0));
        if(!r_u32(0x800A5F68u+4*i)) goto cleanup;
    }
    sub_8009C65C(r_u32(0x800FFB88u));
    w_u32(0x800FF250u,1); sub_80015EC8();
    SetDefDispEnv(&environment,0,256,320,240); VSync(0); PutDispEnv(&environment);
    volume[0]=volume[1]=(uint8)(movie==3?192:((sint16)r_u16(0x800ECC78u)>=64?63*((sint16)r_u16(0x800ECC78u)-64)/64+192:3*(sint16)r_u16(0x800ECC78u)));
    volume[2]=volume[3]=0;
    xport_draft_host_sub_800981B4_p1(volume);
    sub_8002F7C8(); sub_80070748();
    w_u8(0x800EC1D9u,0); w_u8(0x800EC129u,0);
    if(r_u32(0x800FF310u)) {
        w_u8(0x800EC1E9u,0);
        for(i=0;i<8;++i) if(i!=3) w_u8(0x800EC0F9u+16*i,0);
    }
    while(!sub_8002F9E4()) {}
    for(;;) {
        sub_8009BDA0(r_u32(0x800FF2A4u+4*r_u32(0x800FF25Cu)),r_u8(0x800FF298u)!=0);
        sub_8009BE1C(r_u32(0x800A5F68u+4*r_u32(0x800FF258u)),(sint16)r_u16(0x800FF2A0u)*(sint16)r_u16(0x800FF2A2u)/2);
        xport_draft_host_sub_80097DF8_p3(1,0,status);
        if(status[0]&16) { sub_8002F758(); break; }
        timeout=0x800000;
        while(!sub_8002F9E4()) if(timeout--==0) goto finish;
        if(!timeout) break;
        timeout=0x800000;
        while((sint16)r_u16(0x800FF29Cu)!=r_u16(0x800FF28Au)) if(--timeout==0) goto finish;
        SetDefDispEnv(&environment,0,(sint16)r_u16(0x800FF29Eu)>=256?(sint16)r_u16(0x800FF29Eu)-256:(sint16)r_u16(0x800FF29Eu)+256,r_u8(0x800FF298u)?480:320,240);
        if(r_u8(0x800FF298u)) { environment.isrgb24=1; environment.disp.w=2*environment.disp.w/3; }
        VSync(0); PutDispEnv(&environment);
        if(r_u32(0x800FF24Cu)) break;
        sub_80070748();
        if(r_u8(0x800EC1D9u)||r_u8(0x800EC129u)) break;
        if(r_u32(0x800FF310u)) {
            if(r_u8(0x800EC1E9u)) break;
            for(i=0;i<8;++i) if(i!=3 && r_u8(0x800EC0F9u+16*i)) break;
            if(i<8) break;
        }
    }
finish:
    sub_80070100(0x800EC0F8u); w_u32(0x800FF250u,0); sub_80015EC8();
    environment.isrgb24=0; environment.disp.w=512; environment.screen.w=screen_width;
    VSync(0); PutDispEnv(&environment);
    volume[0]=volume[1]=volume[2]=volume[3]=0; xport_draft_host_sub_800981B4_p1(volume);
    sub_8009BED8(0); apocalypse_str_stop(); xport_draft_host_sub_80097DF8_p2(9,0,0);
cleanup:
    if(r_u32(0x800FFB84u)) { sub_8006BC20(r_u32(0x800FFB84u)); w_u32(0x800FFB84u,0); }
    if(r_u32(0x800FFB88u)) { sub_8006BC20(r_u32(0x800FFB88u)); w_u32(0x800FFB88u,0); }
    for(i=0;i<2;++i) if(r_u32(0x800FF2A4u+4*i)) {sub_8006BC20(r_u32(0x800FF2A4u+4*i));w_u32(0x800FF2A4u+4*i,0);}
    result=r_u8(0x800FF288u);
    for(i=0;i<r_u8(0x800FF288u);++i) {if(r_u32(0x800A5F68u+4*i)){sub_8006BC20(r_u32(0x800A5F68u+4*i));w_u32(0x800A5F68u+4*i,0);} result=i+1<r_u8(0x800FF288u);}
    return result;
}



uint32 sub_8002F758(void)
{
    uint8 status[8];
    do { xport_draft_host_sub_80097DF8_p3(1,0,status); } while(status[0]&16);
    xport_draft_host_sub_80097DF8_p2(19,0,0);
    while(!(status[0]&2)) xport_draft_host_sub_80097DF8_p3(1,0,status);
    return status[0]&2;
}
