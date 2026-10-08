#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
#include <stdlib.h>

/* Unverified draft; the following hidden helper and GTE adapters remain TODO */
void sub_80082638(uint32 vertices, uint32 stride, uint32 flags, const uint32 geometry[3], uint32 subdivisions, uint32 step)
{
    const uint32 clip_bias=0x00F00200u,clip_mask=0xBFFFBFFFu;
    uint32 color0,color1,color2,row_remaining,inner_remaining;
    uint32 vertical=0u,row_start,row_end=0x1F800000u,point,remaining,horizontal,packed;
    uint32 screen,flag,color;
    if(flags & 0x40000000u)
    {
        color0=r_u32(vertices+12u);
        color1=r_u32(vertices+stride+12u);
        color2=r_u32(vertices+2u*stride+12u);
        xport_draft_gte_control_write(8u,((color0&255u)<<4)|((color1&255u)<<20));
        xport_draft_gte_control_write(9u,((color2&255u)<<4)|((color0 & 0xFF00u)<<12));
        xport_draft_gte_control_write(10u,((color1 & 0xFF00u)>>4)|((color2 & 0xFF00u)<<12));
        xport_draft_gte_control_write(11u,0u);
        xport_draft_gte_control_write(12u,0u);
    }
    if((sint32)flags<0)
    {
        color0=r_u32(vertices+4u);
        color1=r_u32(vertices+stride+4u);
        color2=r_u32(vertices+2u*stride+4u);
        xport_draft_gte_control_write(16u,((color0&255u)<<4)|((color1&255u)<<20));
        xport_draft_gte_control_write(17u,((color2&255u)<<4)|((color0 & 0xFF00u)<<12));
        xport_draft_gte_control_write(18u,((color1 & 0xFF00u)>>4)|((color2 & 0xFF00u)<<12));
        xport_draft_gte_control_write(19u,((color0>>12) & 0xFF0u)|(((color1>>12) & 0xFF0u)<<16));
        xport_draft_gte_control_write(20u,(color2>>12) & 0xFF0u);
    }
    sub_80082508(geometry[0],geometry[1],geometry[2]);
    row_remaining=subdivisions;
    do
    {
        row_start=row_end;
        if((sint32)(vertical-3840u)>=0) vertical=4096u;
        remaining=4096u-vertical;
        horizontal=0u;
        xport_draft_gte_data_write(0u,remaining);
        xport_draft_gte_data_write(1u,vertical);
        point=row_start-16u;
        inner_remaining=row_remaining;
        do
        {
            xport_draft_gte_execute(0x180001u);
            point+=16u;
            remaining-=step;
            horizontal+=step;
            packed=(horizontal<<16)|remaining;
            if((sint32)(horizontal-3840u)>=0) packed=0x10000000u;
            screen=xport_draft_gte_data_read(14u);
            flag=xport_draft_gte_control_read(31u);
            xport_draft_gte_execute(0x4A6412u);
            screen=((screen&clip_mask)|~((screen-clip_bias)|clip_mask)) & 0x7FFFFFFFu;
            xport_draft_gte_execute(0x198003Du);
            w_u32(point,screen|(flag & 0x80000000u));
            color=xport_draft_gte_data_read(22u);
            w_u32(point+8u,color);
            if((sint32)flags<0)
            {
                xport_draft_gte_execute(0x4C6412u);
                xport_draft_gte_execute(0x198003Du);
                w_u32(point+12u,xport_draft_gte_data_read(22u));
            }
            xport_draft_gte_data_write(0u,packed);
        } while(inner_remaining--!=0u);
        vertical+=step;
        row_end=row_start+128u;
    } while(row_remaining--!=0u);
    /* The MIPS continuation returns to the caller with its packet cursor unchanged */
}

void sub_800849FC(uint32 a1,uint32 a2,uint32 a3,uint32 a4)
{
    uint32 geometry,entry,object_flags,matrix_word,scale;
    sint32 lower[3],upper[3],first[3],second[3];
    uint32 i,delta,flip_bits;
    /* TODO Hidden kernel inputs and outputs are carried by declared logical adapters */
    do
    {
        object_flags=r_u32(a1);
        if(object_flags & 0x21u) w_u16(a1+2u,a4);
        else if((object_flags>>16)!=a4)
        {
            entry=0x800EAEF8u+64u*r_u8(a1+27u);
            geometry=r_u32(r_u32(entry+16u)+4u*r_u16(a1+22u));
            sub_80084778(a2,lower,upper,&flip_bits);
            for(i=0u;i<3u;++i)
            {
                delta=(uint32)((sint32)r_u32(a1+4u+4u*i)>>12);
                lower[i]=(sint32)((uint32)lower[i]-delta);
                upper[i]=(sint32)((uint32)upper[i]-delta);
            }
            if(r_u32(a1+16u)|r_u16(a1+20u))
            {
                sub_800858FC(a1+16u,0x1F800028u);
                sub_80085ACC(0x1F800028u);
                for(i=0u;i<5u;++i)
                {
                    matrix_word=r_u32(0x1F800028u+4u*i);
                    xport_draft_gte_control_write(i,matrix_word);
                }
                for(i=0u;i<3u;++i) xport_draft_gte_data_write(9u+i,(uint32)lower[i]);
                xport_draft_gte_execute(0x49E012u);
                for(i=0u;i<3u;++i) lower[i]=(sint32)xport_draft_gte_data_read(9u+i);
                for(i=0u;i<3u;++i) xport_draft_gte_data_write(9u+i,(uint32)upper[i]);
                xport_draft_gte_execute(0x49E012u);
                for(i=0u;i<3u;++i) upper[i]=(sint32)xport_draft_gte_data_read(9u+i);
                sub_800847AC(lower,upper,&flip_bits);
            }
            xport_draft_call_80084814(a1,geometry,lower,upper,first,second);
            if(r_u32(a1) & 0x200u)
            {
                for(i=0u;i<3u;++i)
                {
                    scale=(uint32)(sint32)(short)r_u16(a1+36u+2u*i);
                    first[i]=(sint32)((uint32)first[i]*scale)>>12;
                    second[i]=(sint32)((uint32)second[i]*scale)>>12;
                }
            }
            if(!xport_draft_call_800848D0(a1,geometry,first,second)) w_u16(a1+2u,a4);
        }
        a1=r_u32(a1+28u);
    } while(a1);
}

uint32 sub_8007C06C(uint32 a1,uint32 a2)
{
    uint32 result=r_u16(a1) & 0x21u,entry,geometry,count,output,index,target,i;
    sint32 translation[3];
    if(result) return result;
    sub_8008793C(a2+72u);
    entry=r_u32(0x800EAEF8u+64u*r_u8(a1+27u)+16u)+4u*r_u16(a1+22u);
    geometry=r_u32(entry);
    count=r_u32(geometry+4u);
    w_u32(0x800FF9E8u,geometry);
    output=count>=129u?r_u32(0x800FFAC0u):0x1F800000u;
    if(!r_u32(a1+16u) && !r_u16(a1+20u) && !(r_u16(a1) & 0x200u))
    {
        for(i=0u;i<3u;++i) translation[i]=(sint32)(r_u32(a1+4u+4u*i)-r_u32(a2+4u*i))>>12;
        xport_draft_gte_control_write(5u,0u);
        xport_draft_gte_control_write(6u,0u);
        xport_draft_gte_control_write(7u,0u);
    }
    else
    {
        w_u32(0x800ED734u,(uint32)((sint32)(r_u32(a1+4u)-r_u32(a2))>>12));
        if(r_u8(a2+137u))
        {
            w_u32(0x800ED738u,(uint32)((sint32)(r_u32(a2+8u)-r_u32(a1+12u))>>12));
            w_u32(0x800ED73Cu,(uint32)((sint32)(r_u32(a1+8u)-r_u32(a2+4u))>>12));
        }
        else
        {
            w_u32(0x800ED738u,(uint32)((sint32)(r_u32(a1+8u)-r_u32(a2+4u))>>12));
            w_u32(0x800ED73Cu,(uint32)((sint32)(r_u32(a1+12u)-r_u32(a2+8u))>>12));
            xport_draft_gte_data_write(0u,(r_u32(0x800ED734u)&65535u)|((r_u32(0x800ED738u)&65535u)<<16));
            xport_draft_gte_data_write(1u,r_u32(0x800ED73Cu));
            xport_draft_gte_execute(0x4C6012u);
            w_u32(0x800ED734u,xport_draft_gte_data_read(25u));
            w_u32(0x800ED738u,xport_draft_gte_data_read(26u));
            w_u32(0x800ED73Cu,xport_draft_gte_data_read(27u));
        }
        sub_8008796C(0x800ED720u);
        translation[0]=translation[1]=translation[2]=0;
        if(!r_u32(a1+16u) && !r_u16(a1+20u))
        {
            sub_800854D8(0x800ED720u);
            sub_80085A08(a1,0x800ED720u);
        }
        else
        {
            sub_800858FC(a1+16u,0x800ED720u);
            if(r_u16(a1) & 0x200u) sub_80085A08(a1,0x800ED720u);
        }
        sub_800855B4(0x800ED720u,0x800ED720u);
        sub_8008793C(0x800ED720u);
    }
    /* TODO Implement the native local-vector adapter */
    w_u16(0x800FFA32u,xport_draft_host_sub_80084D4C_p4(r_u32(0x800FF9E8u),output,r_u32(a2+68u),translation));
    result=r_u16(0x800FFA32u) & 0x60Fu;
    if(!result)
    {
        w_u32(0x800FFA88u,0xFFFFFFFFu);
        sub_80084E24(r_u32(0x800FF9E8u),output,a2,a1);
        result=r_u32(0x800FFA88u);
        if(result!=0xFFFFFFFFu)
        {
            index=sub_8006F4D0(r_u8(a1+27u));
            target=r_u32(r_u32(0x800FF7B4u+4u*index)+4u*r_u32(0x800FFA88u));
            w_u32(0x800FF798u,index^1u);
            w_u32(0x800FF978u,0u);
            return sub_80063D3C(target);
        }
    }
    return result;
}

/* Signed division uses ordinary local arithmetic, without guest register state */
static uint32 a_draft_agent2_divide(uint32 numerator,uint32 denominator)
{
    if(denominator==0u) return (sint32)numerator<0?1u:0xFFFFFFFFu;
    return (uint32)((long long)(sint32)numerator/(long long)(sint32)denominator);
}

static uint32 a_draft_agent2_remainder(uint32 numerator,uint32 denominator)
{
    if(denominator==0u) return numerator;
    return (uint32)((long long)(sint32)numerator%(long long)(sint32)denominator);
}

uint32 sub_8007CF38(uint32 a1,uint32 a2,uint32 a3,uint32 a4,uint32 a5,uint32 a6)
{
    uint32 frame=r_u8(a4+24u),lower,upper,entry,limit,data,factor=0u,numerator,denominator;
    uint32 source0,source1,output,index,result,i;
    uint16 difference[4];
    lower=frame-a_draft_agent2_remainder(frame,a2);
    upper=lower+a2;
    entry=a3+8u*r_u8(a4+26u);
    limit=r_u16(entry+8u);
    data=a3+r_u32(entry+4u);
    if((sint32)upper<(sint32)limit)
    {
        numerator=(frame-lower)<<12;
        denominator=upper-lower;
        factor=a_draft_agent2_divide(numerator,denominator);
    }
    else if(r_u8(a4+296u)==1u)
    {
        upper=0u;
        factor=a_draft_agent2_divide((frame-lower)<<12,limit-lower);
    }
    else
    {
        lower-=a2;
        if((sint32)lower<0) lower=0u;
        upper-=a2;
        if(upper) factor=a_draft_agent2_divide((frame-lower)<<12,upper-lower);
    }
    output=0x800ED760u+24u*a5;
    source0=data+24u*a_draft_agent2_divide(a6*lower,a2)+24u*a5;
    result=data+24u*a_draft_agent2_divide(a6*upper,a2);
    source1=result+24u*a5;
    for(index=0u;(sint32)index<(sint32)a1;++index)
    {
        /* TODO Implement the native local-vector adapter */
        xport_draft_host_sub_8006CB28_p1(difference,source1,source0);
        for(i=0u;i<3u;++i) xport_draft_gte_data_write(25u+i,(uint32)(sint32)(short)r_u16(source0+2u*i));
        for(i=0u;i<3u;++i) xport_draft_gte_data_write(9u+i,(uint32)(sint32)(short)difference[i]);
        xport_draft_gte_data_write(8u,factor);
        xport_draft_gte_execute(0x1A8003Eu);
        for(i=0u;i<3u;++i) w_u16(output+2u*i,xport_draft_gte_data_read(25u+i));
        source0+=6u;
        source1+=6u;
        output+=6u;
        result=(sint32)(index+1u)<(sint32)a1;
    }
    return result;
}

uint32 sub_8007CC10(uint32 a1,uint32 a2,uint32 a3)
{
    uint32 entry=0x800EAEF8u+64u*r_u8(a2+27u),descriptor,index,data,frames,table,interval,vertices,result,x,y,z;
    descriptor=r_u32(entry+28u)+8u*a3;
    index=r_u16(descriptor+6u);
    data=r_u32(entry+24u);
    if(r_u16(a2)&4u) vertices=r_u32(a2+356u)+24u*index;
    else
    {
        table=data+8u*r_u8(a2+26u);
        interval=r_u16(table+10u);
        if(!interval)
        {
            frames=r_u32(r_u32(entry+16u)-4u);
            vertices=data+r_u32(table+4u)+24u*(r_u8(a2+24u)*frames+index);
        }
        else
        {
            frames=r_u32(r_u32(entry+16u)-4u);
            sub_8007CF38(4u,interval+1u,data,a2,index,frames);
            vertices=0x800ED760u+24u*index;
        }
    }
    sub_8008445C(descriptor,vertices,a2+324u);
    x=xport_draft_gte_data_read(25u);
    w_u32(a1,x);
    y=xport_draft_gte_data_read(26u);
    w_u32(a1+4u,y);
    z=xport_draft_gte_data_read(27u);
    w_u32(a1+8u,z);
    x=r_u32(a1);
    z=r_u32(a1+8u);
    w_u32(a1,x<<12);
    y=r_u32(a1+4u);
    w_u32(a1+8u,z<<12);
    result=y<<12;
    w_u32(a1+4u,result);
    return result;
}
