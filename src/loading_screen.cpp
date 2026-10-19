#include <stdlib.h>
#include <string.h>

#include "scene_lifecycle.h"
#include "loading_screen.h"
#include "startup_resources.h"
#include "resource_io.h"
#include "game_string.h"
#include "game_diagnostics.h"

struct LevelLoadEstimate
{
    const char *name;
    uint32 bytes;
};

struct LevelLoading
{
    LevelLoadEstimate *estimates;
    char *names;
    uint32 count;
    uint32 completed;
    uint32 maximum;
    bool active;
    bool pending;
};

static LevelLoading level_loading;

static bool DecodeLevelLoadEstimates(const uint8 *data, uint32 bytes)
{
    uint32 pos = 0, count = 0, name_bytes = 0;
    while (pos < bytes && data[pos])
    {
        uint32 begin = pos;
        while (pos < bytes && data[pos])
            ++pos;
        if (pos == bytes || pos > 0x7FFFFFFC)
            return false;
        name_bytes += pos - begin + 1;
        // Original counts begin at the aligned name terminator
        pos = (pos + 3) & ~3;
        for (;;)
        {
            if (pos > bytes || bytes - pos < 4)
                return false;
            uint32 size = xport_load_le32(data + pos);
            pos += 4;
            if (size == 0xFFFFFFFF)
                break;
        }
        ++count;
    }
    if (pos == bytes || !count)
        return false;
    LevelLoadEstimate *estimates = (LevelLoadEstimate *)calloc(count, sizeof(*estimates));
    char *names = (char *)malloc(name_bytes);
    if (!estimates || !names)
    {
        free(estimates);
        free(names);
        return false;
    }
    pos = 0;
    char *name = names;
    for (uint32 idx = 0; idx < count; ++idx)
    {
        uint32 begin = pos;
        while (data[pos])
            ++pos;
        uint32 len = pos - begin + 1;
        memcpy(name, data + begin, len);
        estimates[idx].name = name;
        name += len;
        pos = (pos + 3) & ~3;
        uint32 size = xport_load_le32(data + pos);
        pos += 4;
        while (size != 0xFFFFFFFF)
        {
            estimates[idx].bytes += size;
            size = xport_load_le32(data + pos);
            pos += 4;
        }
    }
    free(level_loading.estimates);
    free(level_loading.names);
    level_loading.estimates = estimates;
    level_loading.names = names;
    level_loading.count = count;
    return true;
}

void InitializeLevelLoadEstimates()
{
    uint32 bytes = LookupResource("sizes.dat");
    uint8 *data = (uint8 *)malloc(bytes);
    if (!data)
        abort();
    BeginResourceRead((void *)data);
    WaitForResourceRead();
    bool valid = DecodeLevelLoadEstimates(data, bytes);
    free(data);
    if (!valid)
    {
        GameDiagnostic("Invalid level load estimates\n");
        abort();
    }
}

uint32 PrepareLevelLoadingScreen(const char *name, const char *image)
{
    FUNCTION_MARKER(0x80016354, "SLUS_003.73");
    PutDispEnv(&scene_render.buffers[1].disp);
    PSX_RECT rect = {0, 0, 512, 512};
    ClearImage(&rect, 0, 0, 0);
    LoadStartupImage(0, image, 240);
    rect.x = 106;
    rect.y = 200;
    rect.w = 300;
    rect.h = 12;
    ClearImage(&rect, 100, 0, 0);
    rect.x = 108;
    rect.y = 201;
    rect.w = 296;
    rect.h = 10;
    ClearImage(&rect, 0, 0, 0);
    level_loading.active = true;
    level_loading.maximum = 0;
    level_loading.completed = 0;
    for (uint32 idx = 0; idx < level_loading.count; ++idx)
    {
        const LevelLoadEstimate *estimate = level_loading.estimates + idx;
        if (NamesEqualIgnoringCase(estimate->name, name))
        {
            level_loading.maximum = estimate->bytes;
            return 0xFFFFFFFF;
        }
    }
    return 0;
}

void ResetLevelLoadDisplayState()
{
    FUNCTION_MARKER(0x80016554, "SLUS_003.73");
    level_loading.active = false;
}

uint32 DrawLevelLoadProgress()
{
    FUNCTION_MARKER(0x80016560, "SLUS_003.73");
    if (!level_loading.active)
        return 0;
    if (GetResourceReadPhase())
    {
        level_loading.pending = true;
        return 1;
    }
    if (!level_loading.pending)
        return 0x80100000;
    level_loading.completed += GetResourceReadProgress();
    PSX_RECT rect = {108, 201, 0, 10};
    uint32 result;
    if (level_loading.maximum && level_loading.maximum >= level_loading.completed)
    {
        rect.w = (sint16)((296 * level_loading.completed) / level_loading.maximum);
        result = (uint32)ClearImage(&rect, 150, 0, 0);
    }
    else
    {
        rect.w = 296;
        result = (uint32)ClearImage(&rect, 0, 0, 255);
    }
    level_loading.pending = false;
    return result;
}
