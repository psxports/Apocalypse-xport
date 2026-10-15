extern "C"
{
#include "draft_first_signatures.h"
}

uint32 ClearRecordedInput(void)
{
    uint32 resource, result = 0x80600000;
    FUNCTION_MARKER(0x80063A38, "SLUS_003.73");
    resource = r_u32(0x800FF81C);
    if (resource != 0x80600000 && resource)
    {
        result = sub_8006BC20(resource);
        w_u32(0x800FF81C, 0);
    }
    w_u32(0x800FF818, 0);
    return result;
}

extern "C" uint32 sub_80063A38(void)
{
    FUNCTION_MARKER(0x80063A38, "SLUS_003.73");
    return ClearRecordedInput();
}

extern "C" void nullsub_17(void)
{
    FUNCTION_MARKER(0x8002FF88, "SLUS_003.73");
}
