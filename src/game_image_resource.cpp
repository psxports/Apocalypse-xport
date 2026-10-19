#include "game_image_resource.h"
#include "game_diagnostics.h"
#include "psx_gpu.h"

static void PrintImageRectangleDiagnostic(const char *name, const PSX_RECT *rectangle)
{
    uint32 mode = graph_debug_level;
    sint32 width = GPU_VRAM_WIDTH;
    sint32 height = GPU_VRAM_HEIGHT;
    if (mode == 2)
        GameDiagnostic("%s", name);
    else if (mode == 1 && (width < rectangle->w || width < rectangle->w + rectangle->x || height < rectangle->y || height < rectangle->y + rectangle->h || rectangle->w <= 0 || rectangle->x < 0 || rectangle->y < 0 || rectangle->h <= 0))
        GameDiagnostic("%s:bad RECT", name);
    else
        return;
    GameDiagnostic("(%d,%d)-(%d,%d)\n", rectangle->x, rectangle->y, rectangle->w, rectangle->h);
}

uint32 ClearImageRectangle(const PSX_RECT &rectangle, uint8 red, uint8 green, uint8 blue)
{
    FUNCTION_MARKER(0x8008847C, "SLUS_003.73");
    PSX_RECT native_rectangle = rectangle;
    PrintImageRectangleDiagnostic("ClearImage", &native_rectangle);
    return (uint32)ClearImage(&native_rectangle, red, green, blue);
}
