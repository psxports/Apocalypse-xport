#include "startup_resources.h"
#include "resource_io.h"
#include "game_sdk_bindings.h"
#include <stdlib.h>

struct StartupImage
{
    uint8 *compressed;
    uint32 compressed_bytes;
    uint32 *pix;
    uint32 bytes;
};

static StartupImage startup_image;

static void FailStartupImage()
{
    GameDiagnostic("Invalid startup image or allocation failure\n");
    abort();
}

static void ReleaseStartupResources()
{
    FUNCTION_MARKER(0x80010184, "SLUS_003.73");
    free(startup_image.pix);
    free(startup_image.compressed);
    StartupImage empty = {0, 0, 0, 0};
    startup_image = empty;
}

static void LoadStartupResource(const char *name)
{
    FUNCTION_MARKER(0x80010138, "SLUS_003.73");
    startup_image.bytes = LookupResource(name);
    startup_image.pix = (uint32 *)malloc(startup_image.bytes);
    if (!startup_image.pix)
        FailStartupImage();
    BeginResourceRead((void *)startup_image.pix);
    WaitForResourceRead();
}

static void LoadCompressedStartupResource(const char *name)
{
    FUNCTION_MARKER(0x80010028, "SLUS_003.73");
    if (startup_image.compressed)
        return;
    startup_image.compressed_bytes = LookupResource(name);
    startup_image.compressed = (uint8 *)malloc(startup_image.compressed_bytes);
    if (!startup_image.compressed)
        FailStartupImage();
    BeginResourceRead((void *)startup_image.compressed);
    WaitForResourceRead();
}

static bool DecodeStartupPixels(const uint8 *src, uint32 src_bytes, uint16 *dst, uint32 dst_bytes)
{
    if (src_bytes < 12 || (dst_bytes & 1))
        return false;
    uint32 pos = 12, written = 0;
    while (src_bytes - pos >= 2)
    {
        uint32 token = xport_load_le16(src + pos);
        pos += 2;
        if (!token)
            return written == dst_bytes / 2;
        uint32 count = token & 0x7FFF;
        uint32 input_bytes = token & 0x8000 ? 2 : count * 2;
        if (input_bytes > src_bytes - pos || count > dst_bytes / 2 - written)
            return false;
        if (token & 0x8000)
        {
            uint16 value = xport_load_le16(src + pos);
            for (uint32 idx = 0; idx < count; ++idx)
                dst[written++] = value;
        }
        else
            for (uint32 idx = 0; idx < count; ++idx)
                dst[written++] = xport_load_le16(src + pos + idx * 2);
        pos += input_bytes;
    }
    return false;
}

static void DecodeStartupResource()
{
    FUNCTION_MARKER(0x80010074, "SLUS_003.73");
    if (startup_image.pix)
        return;
    if (startup_image.compressed_bytes < 12)
        FailStartupImage();
    startup_image.bytes = xport_load_le32(startup_image.compressed + 8);
    if (!startup_image.bytes || startup_image.bytes > 1024 * 512 * 2)
        FailStartupImage();
    startup_image.pix = (uint32 *)malloc(startup_image.bytes);
    if (!startup_image.pix || !DecodeStartupPixels(startup_image.compressed, startup_image.compressed_bytes, (uint16 *)startup_image.pix, startup_image.bytes))
        FailStartupImage();
}

uint32 ClearStartupImageArea()
{
    FUNCTION_MARKER(0x80015EC8, "SLUS_003.73");
    PSX_RECT rect = {0, 0, 512, 511};
    ClearImage(&rect, 0, 0, 0);
    rect.y = 511;
    rect.h = 1;
    return (uint32)ClearImage(&rect, 0, 0, 0);
}

static void DrawStartupImage(uint32 height)
{
    FUNCTION_MARKER(0x800101CC, "SLUS_003.73");
    if (!startup_image.pix)
        return;
    if (height > 512 || height * 512 * 2 > startup_image.bytes)
        FailStartupImage();
    ClearStartupImageArea();
    sint32 top = (sint32)(240 - height) / 2;
    PSX_RECT rect = {0, (sint16)top, 512, (sint16)height};
    LoadImagePSX(&rect, startup_image.pix);
    MoveImage(&rect, 0, top + 256);
    DrawSync(0);
}

void LoadStartupImage(uint32 compressed, const char *name, uint32 height)
{
    FUNCTION_MARKER(0x8001024C, "SLUS_003.73");
    ReleaseStartupResources();
    const char *resource = name;
    if (compressed)
    {
        LoadCompressedStartupResource(resource);
        DecodeStartupResource();
    }
    else
        LoadStartupResource(resource);
    DrawStartupImage(height);
    ReleaseStartupResources();
}

extern "C" void sub_8001024C(uint32 compressed, uint32 name, uint32 height)
{
    FUNCTION_MARKER(0x8001024C, "SLUS_003.73");
    LoadStartupImage(compressed, (const char *)psx_addr(name, 1), height);
}


