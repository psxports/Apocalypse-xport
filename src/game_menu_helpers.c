#include "game_menu_helpers.h"
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
uint32 sub_800151FC(void)
{ return sub_8001A7D4(0,255,0,0); }
uint32 sub_800154E0(uint32 object,uint32 label)
{ w_u8(object+28*sub_800153F8(object,label)+37,1);return 1; }
uint32 sub_80011860(uint32 button,uint32 selected,uint32 other1,uint32 other2,uint32 other3)
{
    uint32 value,result;
    sint32 first;
    w_u16(selected,button);
    first=(sint16)r_u16(other1);
    value=~((uint32)(sint16)r_u16(other3)|button|(uint32)first|(uint32)(sint16)r_u16(other2))&15;
    if(value&8)value=8;
    if(value&4)value=4;
    if(value&2)value=2;
    if(value&1)value=1;
    if((sint32)button==first)w_u16(other1,value);
    if((sint32)button==(sint16)r_u16(other2))w_u16(other2,value);
    result=(uint32)(sint16)r_u16(other3);
    if(button==result)w_u16(other3,value);
    return result;
}
uint32 sub_80067808(uint32 source,uint32 destination)
{
    uint32 value;
    do { value=r_u8(source++);w_u8(destination++,value); } while(value);
    return value;
}
uint32 xport_draft_host_sub_80067808_p1(void *source,uint32 destination)
{
    const uint8 *bytes=(const uint8 *)source;
    uint32 value;
    do { value=*bytes++;w_u8(destination++,value); } while(value);
    return value;
}
uint32 xport_draft_host_sub_80067808_p2(uint32 source,void *destination)
{
    uint8 *bytes=(uint8 *)destination;
    uint32 value;
    do { value=r_u8(source++);*bytes++=(uint8)value; } while(value);
    return value;
}
void apocalypse_menu_destroy(uint32 object)
{
    uint32 table=r_u32(object),target=r_u32(table+12);
    uint32 adjusted=object+(uint32)(sint32)(sint16)r_u16(table+8);
    if(target==0x80011990u)sub_80011990(adjusted,3);
    else if(target==0x800152C4u)sub_800152C4(adjusted,3);
    else {
        /* TODO Bind any other original menu class destructor */
        xport_draft_guest_call2(target,adjusted,3);
    }
}

