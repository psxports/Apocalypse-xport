extern "C"
{
#include "draft_first_adapters.h"
}
#include <string.h>

extern "C" uint32 xport_draft_host_sub_800885A4_p1(const sint16 *rectangle, uint32 pixels)
{
    FUNCTION_MARKER(0x800885A4, "SLUS_003.73");
    PSX_RECT native_rectangle;
    uint32 bytes;
    memcpy(&native_rectangle, rectangle, sizeof(native_rectangle));
    bytes = (uint32)(uint16)native_rectangle.w * (uint32)(uint16)native_rectangle.h * 2;
    return (uint32)LoadImagePSX(&native_rectangle, (uint32 *)psx_addr(pixels, bytes));
}
