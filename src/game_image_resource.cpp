#include "game_sdk_bindings.h"
extern "C"
{
#include "draft_first_adapters.h"
}
#include <stdio.h>
#include <string.h>

static void PrintImageRectangleDiagnostic(const char *name, const PSX_RECT *rectangle)
{
    uint32 mode = r_u8(0x800FCEAE);
    sint32 width = (sint16)r_u16(0x800FCEB0);
    sint32 height = (sint16)r_u16(0x800FCEB2);
    if (mode == 2)
        GameDiagnostic("%s", name);
    else if (mode == 1 && (width < rectangle->w || width < rectangle->w + rectangle->x || height < rectangle->y || height < rectangle->y + rectangle->h || rectangle->w <= 0 || rectangle->x < 0 || rectangle->y < 0 || rectangle->h <= 0))
        GameDiagnostic("%s:bad RECT", name);
    else
        return;
    GameDiagnostic("(%d,%d)-(%d,%d)\n", rectangle->x, rectangle->y, rectangle->w, rectangle->h);
}

extern "C" uint32 xport_draft_host_sub_8008847C_p1(const sint16 *rectangle, uint32 red, uint32 green, uint32 blue)
{
    FUNCTION_MARKER(0x8008847C, "SLUS_003.73");
    PSX_RECT native_rectangle;
    memcpy(&native_rectangle, rectangle, sizeof(native_rectangle));
    PrintImageRectangleDiagnostic("ClearImage", &native_rectangle);
    return (uint32)ClearImage(&native_rectangle, (uint8)red, (uint8)green, (uint8)blue);
}

extern "C" uint32 xport_draft_host_sub_80088664_p1(const sint16 *rectangle, uint32 x, uint32 y)
{
    FUNCTION_MARKER(0x80088664, "SLUS_003.73");
    PSX_RECT native_rectangle;
    uint32 position, size;
    memcpy(&native_rectangle, rectangle, sizeof(native_rectangle));
    PrintImageRectangleDiagnostic("MoveImage", &native_rectangle);
    if (!native_rectangle.w || !native_rectangle.h)
        return 0xFFFFFFFF;
    memcpy(&position, rectangle, 4);
    memcpy(&size, rectangle + 2, 4);
    w_u32(0x800FCF50, (y << 16) | (x & 65535));
    w_u32(0x800FCF4C, position);
    w_u32(0x800FCF54, size);
    return (uint32)MoveImage(&native_rectangle, (sint32)(x & 65535), (sint32)y);
}
