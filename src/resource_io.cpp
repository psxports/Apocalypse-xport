#include "game_sdk_bindings.h"
extern "C"
{
#include "draft_first_signatures.h"
}
#include "resource_io.h"
#include "game_data_io.h"
#include "loading_screen.h"
#include <stdlib.h>
#include <string.h>

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
    uint8 *dst;
    uint32 offset;
    uint32 bytes;
    uint32 progress;
    uint32 phase;
    uint32 option;
    uint32 failures;
    sint32 error;
};

static ResourceDirectory resource_directory;
static ResourceRead resource_read;

static uint32 RoundResourceSize(uint32 bytes)
{
    uint32 rounded = bytes + 2047;
    if ((sint32)rounded < 0)
        rounded = bytes + 4094;
    return (uint32)((sint32)rounded >> 11) << 11;
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
        uint32 offset = xport_load_le32(data + pos);
        uint32 size = xport_load_le32(data + pos + 4);
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
        entries[idx].offset = xport_load_le32(data + pos);
        entries[idx].bytes = xport_load_le32(data + pos + 4);
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
        GameDiagnostic("Cannot size game resource directory or archive\n");
        abort();
    }
    uint8 *data = (uint8 *)malloc(bytes);
    bool valid = data && xport_file_read(path, data, bytes, &read_bytes) && read_bytes == bytes && DecodeResourceDirectory(data, (uint32)bytes, (uint32)wad_bytes);
    free(data);
    if (!valid)
    {
        GameDiagnostic("Invalid game resource directory: %s\n", path);
        abort();
    }
}

static uint8 FoldResourceNameByte(uint8 value)
{
    if (value >= 'A' && value <= 'Z')
        value += 'a' - 'A';
    return value;
}

static const ResourceEntry *FindResource(const char *name)
{
    for (uint32 idx = 0; idx < resource_directory.count; ++idx)
    {
        const ResourceEntry *entry = resource_directory.entries + idx;
        const char *left = name, *right = entry->name;
        while (*left && FoldResourceNameByte((uint8)*left) == FoldResourceNameByte((uint8)*right))
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
    resource_read.phase = 0;
    resource_read.failures = 0;
    resource_read.dst = 0;
    resource_read.error = 0;
    sub_80097D14(0);
    if (!sub_80097B4C())
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
        GameDiagnostic("Missing game resource: %s\n", name);
        abort();
    }
    return entry->bytes;
}

uint32 LookupResource(const char *name)
{
    FUNCTION_MARKER(0x8006B04C, "SLUS_003.73");
    resource_read.progress = 0;
    GameDiagnostic("\n%s", name);
    const ResourceEntry *entry = FindResource(name);
    if (!entry)
    {
        GameDiagnostic("\nMissing game resource: %s\n", name);
        abort();
    }
    resource_read.offset = entry->offset;
    resource_read.bytes = RoundResourceSize(entry->bytes);
    return resource_read.bytes;
}

uint32 BeginResourceRead(void *dst)
{
    FUNCTION_MARKER(0x8006B234, "SLUS_003.73");
    sub_8002F130();
    resource_read.dst = (uint8 *)dst;
    resource_read.error = 0;
    resource_read.phase = 1;
    resource_read.progress = 0;
    // Media ownership still lives in the CD voice subsystem until its migration
    w_u32(0x800FF2B8, 0);
    w_u32(0x800FF2BC, 1);
    w_u32(0x800FF2C0, 0);
    return 1;
}

uint32 BeginResourceRead(uint32 destination)
{
    // Temporary incoming boundary for resource consumers not yet migrated
    return BeginResourceRead(psx_addr(destination, resource_read.bytes));
}

uint32 ProcessResourceRead()
{
    FUNCTION_MARKER(0x8006B2A8, "SLUS_003.73");
    bool error = false;
    if (resource_read.phase == 1)
    {
        resource_read.progress = 0;
        if (CdControl(9, 0, 0))
            resource_read.phase = 2;
        else
            error = true;
    }
    else if (resource_read.phase == 2)
    {
        resource_read.error = resource_read.bytes && ReadDataResource(resource_read.dst, resource_read.offset, resource_read.bytes) ? 0 : -1;
        if (!resource_read.error)
            resource_read.phase = 3;
        else
        {
            error = true;
            resource_read.phase = 1;
        }
    }
    else if (resource_read.phase == 3)
    {
        if (resource_read.progress == resource_read.bytes)
        {
            resource_read.phase = 0;
            resource_read.dst = 0;
        }
        else if (resource_read.error >= 0)
            resource_read.progress = resource_read.bytes;
        else
        {
            error = true;
            if ((sint32)++resource_read.failures >= 5)
            {
                resource_read.failures = 0;
                resource_read.phase = 1;
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

extern "C" void sub_8006AED8(uint32 option)
{
    FUNCTION_MARKER(0x8006AED8, "SLUS_003.73");
    SetResourceReadOption(option);
}



extern "C" uint32 apocalypse_resource_lookup(const char *name)
{
    return LookupResource(name);
}

extern "C" uint32 sub_8006B04C(uint32 name)
{
    FUNCTION_MARKER(0x8006B04C, "SLUS_003.73");
    return LookupResource((const char *)psx_addr(name, 1));
}






