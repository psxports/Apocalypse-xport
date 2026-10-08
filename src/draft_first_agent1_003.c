#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
#include <stdlib.h>
/* Unverified draft; TODO Implement GTE adapters separately */

void sub_800879EC(uint32 A0)
{
    xport_draft_gte_control_write(26u, A0);
}

void sub_8008445C(uint32 A0, uint32 a2, uint32 a3)
{
    uint32 index;
    xport_draft_gte_data_write(0u, r_u32(A0));
    xport_draft_gte_data_write(1u, r_u32(A0 + 4u));
    for (index = 0u; index < 5u; ++index)
        xport_draft_gte_control_write(index, r_u32(a2 + index * 4u));
    xport_draft_gte_control_write(5u, (uint32)(sint32)(sint16)r_u16(a2 + 18u));
    xport_draft_gte_control_write(6u, (uint32)(sint32)(sint16)r_u16(a2 + 20u));
    xport_draft_gte_control_write(7u, (uint32)(sint32)(sint16)r_u16(a2 + 22u));
    xport_draft_gte_control_write(13u, r_u32(a3 + 20u));
    xport_draft_gte_control_write(14u, r_u32(a3 + 24u));
    xport_draft_gte_control_write(15u, r_u32(a3 + 28u));
    xport_draft_gte_execute(0x480012u);
    for (index = 0u; index < 5u; ++index)
        xport_draft_gte_control_write(8u + index, r_u32(a3 + index * 4u));
    xport_draft_gte_execute(0x4BA012u);
}

void sub_80085DBC(uint32 A0)
{
    (void)xport_draft_gte_data_read(9u);
    xport_draft_gte_data_write(8u, A0);
    xport_draft_gte_execute(0x198003Du);
}

void sub_80085DD8(uint32 a1, uint32 A1)
{
    uint32 x = (uint32)(sint32)(sint16)r_u16(a1);
    uint32 y = (uint32)(sint32)(sint16)r_u16(a1 + 2u);
    uint32 z = (uint32)(sint32)(sint16)r_u16(a1 + 4u);
    (void)xport_draft_gte_data_read(9u);
    xport_draft_gte_data_write(8u, A1);
    xport_draft_gte_data_write(25u, x);
    xport_draft_gte_data_write(26u, y);
    xport_draft_gte_data_write(27u, z);
    xport_draft_gte_execute(0x1A8003Eu);
}

void sub_80084504(uint32 a1)
{
    uint32 x = (uint32)((sint32)r_u32(a1) >> 12);
    uint32 y = (uint32)((sint32)r_u32(a1 + 4u) >> 12);
    uint32 z = (uint32)((sint32)r_u32(a1 + 8u) >> 12);
    xport_draft_gte_control_write(5u, x);
    xport_draft_gte_control_write(6u, y);
    xport_draft_gte_control_write(7u, z);
}

void sub_80082508(uint32 input0, uint32 input1, uint32 input2)
{
    uint32 row0 = r_u32(input0 + 8000u);
    uint32 tail0 = r_u16(input0 + 4u);
    uint32 row1 = r_u32(input1 + 8000u);
    uint32 tail1 = r_u16(input1 + 4u);
    uint32 row2 = r_u32(input2 + 8000u);
    uint32 tail2 = r_u16(input2 + 4u);
    xport_draft_gte_control_write(0u, (row0 & 0xFFFFu) | (row1 << 16));
    xport_draft_gte_control_write(1u, ((row2 ^ row0) & 0xFFFFu) ^ row0);
    xport_draft_gte_control_write(2u, (row1 >> 16) | (row2 & 0xFFFF0000u));
    xport_draft_gte_control_write(3u, tail0 | (tail1 << 16));
    xport_draft_gte_control_write(4u, tail2);
}

void sub_80082200(uint32 lookup_index, uint32 table, uint32 ir1, uint32 ir2, uint32 ir3)
{
    uint32 color = r_u32(table + (lookup_index & 0x3FCu));
    xport_draft_gte_data_write(9u, ir1);
    xport_draft_gte_data_write(10u, ir2);
    xport_draft_gte_data_write(11u, ir3);
    xport_draft_gte_data_write(6u, color);
}

uint32 sub_8001E650(uint32 a1)
{
    /* TODO Original return is inherited from the last call on paths that make no call */
    uint32 result;
    if (r_u32(0x800FF378u) != 9u)
        result = sub_80067388(a1, 1u, 512u, 32u, 256u);
    else
        result = xport_draft_unknown_result_8001E650();
    if (r_u32(0x800FF738u) == 0u)
    {
        sub_8001E4C8(a1, sub_80066570(20u), 10u, 750u, 80u);
        sub_8001E1B8(a1, 0u);
        result = sub_8001D320(a1, 70u, 240u, 200u, 0u, 5u, 0u, 100u);
    }
    return result;
}
