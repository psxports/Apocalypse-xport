#include <stdlib.h>
#include <string.h>

#include "movie_lifecycle.h"
#include "cd_voice_event.h"
#include "game_diagnostics.h"
#include "resource_io.h"
#include "game_data_io.h"
#include "loading_screen.h"

struct ResourceExtentFile
{
    uint32 offset;
    uint32 bytes;
};

typedef char ResourceExtentFileSize[sizeof(ResourceExtentFile) == 8 ? 1 : -1];

struct ResourceEntry
{
    const char *name;
    uint32 offset;
    uint32 bytes;
};

struct ResourceDirectory
{
    ResourceEntry *entries;
    char *names;
    uint32 count;
};

struct ResourceRead
{
    enum Phase
    {
        Idle,
        Preparing,
        Reading,
        Completing
    };

    void *dst;
    uint32 offset;
    uint32 bytes;
    uint32 progress;
    Phase phase;
    uint32 option;
    uint8 failures;
    bool failed;
};

static ResourceDirectory resource_directory;
static ResourceRead resource_read;

static uint32 RoundResourceSize(uint32 bytes)
{
    // Directory decoding limits every resource to 0x7FFFF800 bytes
    return (bytes + 2047) & ~2047;
}

static void ReleaseResourceDirectory()
{
    free(resource_directory.entries);
    free(resource_directory.names);
    ResourceDirectory empty = {0, 0, 0};
    resource_directory = empty;
}

static bool DecodeResourceDirectory(const uint8 *data, uint32 bytes, uint32 wad_bytes)
{
    uint32 pos = 0, count = 0, name_bytes = 0;
    while (pos < bytes && data[pos] != 255)
    {
        uint32 begin = pos;
        while (pos < bytes && data[pos])
            ++pos;
        if (pos == bytes || pos == begin || pos > 0x7FFFFFFB)
            return false;
        name_bytes += pos - begin + 1;
        pos = (pos + 4) & ~3;
        if (pos > bytes || bytes - pos < 8)
            return false;
        const ResourceExtentFile &file = *(const ResourceExtentFile *)(data + pos);
        uint32 offset = file.offset;
        uint32 size = file.bytes;
        if ((offset & 2047) || size > 0x7FFFF800 || offset > wad_bytes || RoundResourceSize(size) > wad_bytes - offset)
            return false;
        pos += 8;
        ++count;
    }
    if (pos == bytes || !count)
        return false;
    ResourceEntry *entries = (ResourceEntry *)calloc(count, sizeof(*entries));
    char *names = (char *)malloc(name_bytes);
    if (!entries || !names)
    {
        free(entries);
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
        entries[idx].name = name;
        name += len;
        pos = (pos + 4) & ~3;
        const ResourceExtentFile &file = *(const ResourceExtentFile *)(data + pos);
        entries[idx].offset = file.offset;
        entries[idx].bytes = file.bytes;
        pos += 8;
    }
    ReleaseResourceDirectory();
    resource_directory.entries = entries;
    resource_directory.names = names;
    resource_directory.count = count;
    return true;
}

static void LoadResourceDirectory()
{
    size_t bytes, wad_bytes, read_bytes;
    const char *path = "DATA/CD.HED";
    if (!xport_file_size(path, &bytes) || !bytes || bytes > 0x7FFFFFFF || !xport_file_size("DATA/CD.WAD", &wad_bytes) || wad_bytes > 0x7FFFFFFF)
    {
        GameDiagnostic(("Cannot size game resource directory or archive\n"));
        abort();
    }
    uint8 *data = (uint8 *)malloc(bytes);
    bool valid = data && xport_file_read(path, data, bytes, &read_bytes) && read_bytes == bytes && DecodeResourceDirectory(data, (uint32)bytes, (uint32)wad_bytes);
    free(data);
    if (!valid)
    {
        GameDiagnostic(("Invalid game resource directory: %s\n", path));
        abort();
    }
}

static uint8 FoldResourceNameByte(uint8 val)
{
    if (val >= 'A' && val <= 'Z')
        val += 'a' - 'A';
    return val;
}

static const ResourceEntry *FindResource(const char *name)
{
    for (uint32 idx = 0; idx < resource_directory.count; ++idx)
    {
        const ResourceEntry *entry = resource_directory.entries + idx;
        const char *left = name, *right = entry->name;
        while (*left && FoldResourceNameByte(*left) == FoldResourceNameByte(*right))
        {
            ++left;
            ++right;
        }
        if (!*left && !*right)
            return entry;
    }
    return 0;
}

static void DrawResourceReadError()
{
    // Original diagnostic rectangle is immutable data beside the resource log format
    PSX_RECT rect = {485, 18, 10, 10};
    ClearImage(&rect, 255, 0, 0);
    rect.y += 256;
    ClearImage(&rect, 255, 0, 0);
}

void SetResourceReadOption(uint32 option)
{
    FUNCTION_MARKER(0x8006AED8, "SLUS_003.73");
    // Retained request has no consumer in the synchronous native file reader
    resource_read.option = option;
}

uint32 InitializeResourceDirectory()
{
    FUNCTION_MARKER(0x8006AEE4, "SLUS_003.73");
    resource_read.phase = ResourceRead::Idle;
    resource_read.failures = 0;
    resource_read.dst = 0;
    resource_read.failed = false;
    CdSetDebug(0);
    if (!CdInit())
        abort();
    LoadResourceDirectory();
    return 0;
}

uint32 GetResourceReadPhase()
{
    return resource_read.phase;
}

uint32 GetResourceReadProgress()
{
    return resource_read.progress;
}

uint32 GetResourceFileSize(const char *name)
{
    const ResourceEntry *entry = FindResource(name);
    if (!entry)
    {
        GameDiagnostic(("Missing game resource: %s\n", name));
        abort();
    }
    return entry->bytes;
}

uint32 LookupResource(const char *name)
{
    FUNCTION_MARKER(0x8006B04C, "SLUS_003.73");
    resource_read.progress = 0;
    GameDiagnostic(("\n%s", name));
    const ResourceEntry *entry = FindResource(name);
    if (!entry)
    {
        GameDiagnostic(("\nMissing game resource: %s\n", name));
        abort();
    }
    resource_read.offset = entry->offset;
    resource_read.bytes = RoundResourceSize(entry->bytes);
    return resource_read.bytes;
}

uint32 BeginResourceRead(void *dst)
{
    FUNCTION_MARKER(0x8006B234, "SLUS_003.73");
    ResetMoviePlayback();
    resource_read.dst = dst;
    resource_read.failed = false;
    resource_read.phase = ResourceRead::Preparing;
    resource_read.progress = 0;
    cd_voice.active = 0;
    cd_voice.available = 1;
    cd_voice.prepared = 0;
    return 1;
}

uint32 ProcessResourceRead()
{
    FUNCTION_MARKER(0x8006B2A8, "SLUS_003.73");
    bool error = false;
    if (resource_read.phase == ResourceRead::Preparing)
    {
        resource_read.progress = 0;
        if (CdControl(CdlPause, 0, 0))
            resource_read.phase = ResourceRead::Reading;
        else
            error = true;
    }
    else if (resource_read.phase == ResourceRead::Reading)
    {
        resource_read.failed = !resource_read.bytes || !ReadDataResource(resource_read.dst, resource_read.offset, resource_read.bytes);
        if (!resource_read.failed)
            resource_read.phase = ResourceRead::Completing;
        else
        {
            error = true;
            resource_read.phase = ResourceRead::Preparing;
        }
    }
    else if (resource_read.phase == ResourceRead::Completing)
    {
        if (resource_read.progress == resource_read.bytes)
        {
            resource_read.phase = ResourceRead::Idle;
            resource_read.dst = 0;
        }
        else if (!resource_read.failed)
            resource_read.progress = resource_read.bytes;
        else
        {
            error = true;
            if (++resource_read.failures >= 5)
            {
                resource_read.failures = 0;
                resource_read.phase = ResourceRead::Preparing;
            }
        }
    }
    if (error)
        DrawResourceReadError();
    return DrawLevelLoadProgress();
}

uint32 WaitForResourceRead()
{
    FUNCTION_MARKER(0x8006B44C, "SLUS_003.73");
    while (resource_read.phase)
        ProcessResourceRead();
    return 0;
}
