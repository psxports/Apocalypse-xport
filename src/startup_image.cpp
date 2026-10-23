#include <stdlib.h>

#include "startup_resources.h"
#include "resource_io.h"
#include "game_diagnostics.h"

struct StartupImageFile
{
    uint32 reserved[2];
    uint32 bytes;
};

typedef char StartupImageFileSize[sizeof(StartupImageFile) == 12 ? 1 : -1];

struct StartupImage
{
    uint8 *compressed;
    uint32 compressed_bytes;
    uint16 *pix;
    uint32 bytes;
};

static StartupImage startup_image;

static void FailStartupImage()
{
    GameDiagnostic(("Invalid startup image or allocation failure\n"));
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
    startup_image.pix = (uint16 *)malloc(startup_image.bytes);
    if (!startup_image.pix)
        FailStartupImage();
    BeginResourceRead(startup_image.pix);
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
    BeginResourceRead(startup_image.compressed);
    WaitForResourceRead();
}

static bool DecodeStartupPixels(const uint8 *src, uint32 src_bytes, uint16 *dst, uint32 dst_bytes)
{
    if (src_bytes < 12 || (dst_bytes & 1))
        return false;
    const uint16 *encoded = (const uint16 *)src;
    uint32 pos = 12, written = 0;
    while (src_bytes - pos >= 2)
    {
        uint16 token = encoded[pos / 2];
        pos += 2;
        if (!token)
            return written == dst_bytes / 2;
        uint16 count = token & 0x7FFF;
        uint32 input_bytes = token & 0x8000 ? 2 : count * 2;
        if (input_bytes > src_bytes - pos || count > dst_bytes / 2 - written)
            return false;
        if (token & 0x8000)
        {
            uint16 val = encoded[pos / 2];
            for (uint32 idx = 0; idx < count; ++idx)
                dst[written++] = val;
        }
        else
            for (uint32 idx = 0; idx < count; ++idx)
                dst[written++] = encoded[pos / 2 + idx];
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
    startup_image.bytes = ((const StartupImageFile *)startup_image.compressed)->bytes;
    if (!startup_image.bytes || startup_image.bytes > 1024 * 512 * 2)
        FailStartupImage();
    startup_image.pix = (uint16 *)malloc(startup_image.bytes);
    if (!startup_image.pix || !DecodeStartupPixels(startup_image.compressed, startup_image.compressed_bytes, startup_image.pix, startup_image.bytes))
        FailStartupImage();
}

uint32 ClearStartupImageArea()
{
    FUNCTION_MARKER(0x80015EC8, "SLUS_003.73");
    PSX_RECT rect = {0, 0, 512, 511};
    ClearImage(&rect, 0, 0, 0);
    rect.y = 511;
    rect.h = 1;
    return ClearImage(&rect, 0, 0, 0);
}

static void DrawStartupImage(uint32 height)
{
    FUNCTION_MARKER(0x800101CC, "SLUS_003.73");
    if (!startup_image.pix)
        return;
    if (height > 512 || height * 512 * 2 > startup_image.bytes)
        FailStartupImage();
    ClearStartupImageArea();
    sint16 rect_height = height;
    sint16 top = (240 - rect_height) / 2;
    PSX_RECT rect = {0, top, 512, rect_height};
    // PsyQ uploads pixel halfwords through its legacy word-pointer interface
    LoadImagePSX(&rect, (uint32 *)startup_image.pix);
    MoveImage(&rect, 0, top + 256);
    DrawSync(0);
}

void LoadStartupImage(bool compressed, const char *name, uint32 height)
{
    FUNCTION_MARKER(0x8001024C, "SLUS_003.73");
    ReleaseStartupResources();
    if (compressed)
    {
        LoadCompressedStartupResource(name);
        DecodeStartupResource();
    }
    else
        LoadStartupResource(name);
    DrawStartupImage(height);
    ReleaseStartupResources();
}
