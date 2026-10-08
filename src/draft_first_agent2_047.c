#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
#include <stdlib.h>

/* Unverified draft from full MIPS; no pseudocode was available */
uint32 sub_8005E260(uint32 owner, uint32 kind)
{
    kind &= 65535u;
    uint32 old,table,object,slot,lifetime,previous;
    if(kind<1u || kind>7u) return 0u;
    if(kind==1u) sub_8002FC64(0u,3u,owner+4u,1u);
    else sub_8002FC64(0u,sub_80066570(2u),owner+4u,1u);
    slot=owner+0x264u+4u*kind;
    old=r_u32(slot);
    if(old)
    {
        table=r_u32(old);
        /* TODO Implement the guest virtual-call adapter */
        xport_draft_guest_call2(r_u32(table+12u),old+(uint32)(sint32)(short)r_u16(table+8u),3u);
    }
    switch(kind)
    {
    case 1u:
        object=sub_8002FED8(0x48u);
        if(object) object=xport_draft_missing_sub_80024428(object,0x800FF4E8u,0u);
        lifetime=r_u32(0x800FF384u)?10u:15u;
        break;
    case 2u:
        object=sub_8002FED8(0x44u);
        if(object) object=xport_draft_missing_sub_800252CC(object,0x800FF4E8u);
        lifetime=r_u32(0x800FF384u)?10u:15u;
        break;
    case 3u:
        object=sub_8002FED8(0x68u);
        /* TODO Callee stack-argument registry refinement */
        if(object) object=sub_80027720(object,0x800FF4E8u,3000u,0u,256u,9u);
        lifetime=r_u32(0x800FF384u)?10u:15u;
        break;
    case 4u:
        object=sub_8002FED8(0x64u);
        if(object) object=xport_draft_missing_sub_80028578(object,0x800FF4E8u);
        lifetime=r_u32(0x800FF384u)?2u:3u;
        break;
    case 5u:
        object=sub_8002FED8(0x44u);
        if(object) object=xport_draft_missing_sub_8002A5C0(object,0x800FF4E8u);
        lifetime=r_u32(0x800FF384u)?120u:180u;
        break;
    case 6u:
        object=sub_8002FED8(0x44u);
        if(object) object=xport_draft_missing_sub_80025070(object,0x800FF4E8u);
        lifetime=r_u32(0x800FF384u)?15u:22u;
        break;
    default:
        object=sub_8002FED8(0x44u);
        if(object) object=xport_draft_missing_sub_8002B4EC(object,0x800FF4E8u);
        lifetime=r_u32(0x800FF384u)?40u:60u;
        break;
    }
    w_u32(slot,object);
    w_u32(object+40u,lifetime);
    previous=r_u32(owner+0x284u);
    w_u32(owner+0x284u,kind);
    w_u32(owner+0x288u,previous);
    return 1u;
}
