#include "game_gpu_present.h"
#include "psx_gpu.h"
#include <stdlib.h>
#include <string.h>

static uint32 movie_pixels[GPU_VRAM_WIDTH * GPU_VRAM_HEIGHT];
static sint32 display_enabled = 1;

void apocalypse_set_disp_mask(sint32 enabled)
{
    SetDispMask(enabled);
    display_enabled = enabled != 0;
}

sint32 apocalypse_gpu_present(void)
{
    DISPENV display;
    sint32 x, y, width, height, offset_x, offset_y;
    const uint8 *vram_bytes = (const uint8 *)VRAM;
    if (r_u8(0x800FCF29u) != 1u) return gpu_present();
    if (!display_enabled) return 1;
    if (xport_is_headless()) {
        const char *raster = getenv("XPORT_RASTERIZE");
        const char *verify = getenv("XPORT_VERIFY_VRAM");
        if (!(raster && !strcmp(raster, "1")) && !(verify && !strcmp(verify, "1"))) return 1;
    }
    memcpy(&display, psx_addr(0x800FCF18u, sizeof(display)), sizeof(display));
    width = display.disp.w;
    height = display.disp.h;
    offset_x = display.screen.x;
    offset_y = display.screen.y;
    if (width < 1 || height < 1 || width > GPU_VRAM_WIDTH || height > GPU_VRAM_HEIGHT) return gpu_present();
    for (y = 0; y < height; ++y) {
        for (x = 0; x < width; ++x) {
            sint32 source_x = x - offset_x;
            sint32 source_y = y - offset_y;
            uint32 pixel = 0;
            if (source_x >= 0 && source_x < width && source_y >= 0 && source_y < height) {
                sint32 byte_x = display.disp.x * 2 + source_x * 3;
                sint32 row_y = display.disp.y + source_y;
                if (byte_x >= 0 && byte_x + 2 < GPU_VRAM_WIDTH * 2 && row_y >= 0 && row_y < GPU_VRAM_HEIGHT) {
                    const uint8 *rgb = vram_bytes + row_y * GPU_VRAM_WIDTH * 2 + byte_x;
                    pixel = ((uint32)rgb[0] << 16) | ((uint32)rgb[1] << 8) | rgb[2];
                }
            }
            movie_pixels[y * width + x] = pixel;
        }
    }
    return xport_present(movie_pixels, width, height, 0, 0, width, height, WND_TITLE);
}