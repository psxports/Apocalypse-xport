#include "draft_first_adapters.h"
#include <stdio.h>
#include <string.h>

static void resource_rectangle_debug(const char *name, const PSX_RECT *rectangle)
{
    uint32 mode = r_u8(0x800FCEAEu);
    sint32 width = (sint16)r_u16(0x800FCEB0u);
    sint32 height = (sint16)r_u16(0x800FCEB2u);
    if (mode == 2u)
        printf("%s", name);
    else if (mode == 1u && (width < rectangle->w || width < rectangle->w + rectangle->x || height < rectangle->y || height < rectangle->y + rectangle->h || rectangle->w <= 0 || rectangle->x < 0 || rectangle->y < 0 || rectangle->h <= 0))
        printf("%s:bad RECT", name);
    else
        return;
    printf("(%d,%d)-(%d,%d)\n", rectangle->x, rectangle->y, rectangle->w, rectangle->h);
}

uint32 xport_draft_host_sub_8008847C_p1(const sint16 *rectangle, uint32 red, uint32 green, uint32 blue)
{
    PSX_RECT native_rectangle;
    memcpy(&native_rectangle, rectangle, sizeof(native_rectangle));
    resource_rectangle_debug("ClearImage", &native_rectangle);
    return (uint32)ClearImage(&native_rectangle, (uint8)red, (uint8)green, (uint8)blue);
}

uint32 xport_draft_host_sub_80088664_p1(const sint16 *rectangle, uint32 x, uint32 y)
{
    PSX_RECT native_rectangle;
    uint32 position, size;
    memcpy(&native_rectangle, rectangle, sizeof(native_rectangle));
    resource_rectangle_debug("MoveImage", &native_rectangle);
    if (!native_rectangle.w || !native_rectangle.h)
        return 0xFFFFFFFFu;
    memcpy(&position, rectangle, 4u);
    memcpy(&size, rectangle + 2, 4u);
    w_u32(0x800FCF50u, (y << 16) | (x & 65535u));
    w_u32(0x800FCF4Cu, position);
    w_u32(0x800FCF54u, size);
    return (uint32)MoveImage(&native_rectangle, (sint32)(x & 65535u), (sint32)y);
}
