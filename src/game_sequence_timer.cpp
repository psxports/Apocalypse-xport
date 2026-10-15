extern "C"
{
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
}
#include <stdlib.h>

static void FailSequenceTimerDivision(void)
{
    /* TODO MIPS BREAK7 on a zero timer divisor */
    abort();
}

extern "C" void sub_8008E9FC(uint32 a1)
{
    FUNCTION_MARKER(0x8008E9FC, "SLUS_003.73");
    uint32 source = 0xF2000002;
    uint32 rate = 0x44E8;
    uint32 setting, callback;
    w_u8(0x800FD06E, 6);
    w_u8(0x800FD06C, 0);
    w_u8(0x800FD06D, 0);
    w_u32(0x800FD068, 0);
    setting = r_u32(0x800FD05C);
    if (setting == 0)
    {
        w_u8(0x800FD06E, 127);
        return;
    }
    if (setting == 3)
        rate = 0x89D0;
    else if (setting == 5)
    {
        w_u8(0x800FD06E, 0);
        if (a1 != 0)
        {
            source = 0xF2000003;
            rate = 1;
        }
        else
            w_u8(0x800FD06C, 1);
    }
    else if (setting != 2)
    {
        if (r_u32(0x800FD060) != 0)
            return;
        setting = r_u32(0x800FD05C);
        if (setting == 0)
        {
            FailSequenceTimerDivision();
        }
        if ((sint32)setting < 70)
        {
            rate = (uint32)(0x204CC0 / (sint32)setting) & 0xFFFF;
            w_u8(0x800FD06D, r_u8(0x800FD06D) + 1);
        }
        else
            rate = (uint32)(0x409980 / (sint32)setting) & 0xFFFF;
    }
    xport_bios_enter_critical();
    if (r_u8(0x800FD06C) != 0)
        sub_800865EC(r_u32(0x800FD064));
    else
    {
        sub_8008EE44(source);
        sub_8008ED0C(source, rate, 4096);
        if (r_u8(0x800FD06E) == 0)
        {
            w_u32(0x800FD068, sub_8008658C(0, 0));
            callback = 0x8008EC6C;
        }
        else
            callback = r_u8(0x800FD06D) != 0 ? 0x8008ECB8 : r_u32(0x800FD064);
        sub_8008658C((uint32)(sint32)(sint8)r_u8(0x800FD06E), callback);
    }
    xport_bios_exit_critical();
}

extern "C" void sub_8008EC4C(void)
{
    FUNCTION_MARKER(0x8008EC4C, "SLUS_003.73");
    sub_8008E9FC(0);
}
