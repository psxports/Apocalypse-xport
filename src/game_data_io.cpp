#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game_diagnostics.h"
#include "game_data_io.h"

struct RiffFileHeader
{
    char magic[4];
    uint32 bytes;
    char format[4];
};

typedef char RiffFileHeaderSize[sizeof(RiffFileHeader) == 12 ? 1 : -1];

struct RiffChunkFile
{
    char kind[4];
    uint32 bytes;
};

typedef char RiffChunkFileSize[sizeof(RiffChunkFile) == 8 ? 1 : -1];

struct DataCdFile
{
    char path[256];
    sint32 sector;
    uint32 sectors;
};

static DataCdFile data_cd_files[64];
static uint32 data_cd_count;
static sint32 data_cd_next_sector = 1;
static DataCdFile *data_cd_active;

static void FailDataRead(const char *path)
{
    GameDiagnostic(("Cannot read game data: %s\n", path));
    abort();
}

static bool ResolveDataPath(char *path, const char *name)
{
    strcpy(path, "DATA/");
    while (*name == '/' || *name == '\\')
        ++name;
    uint32 len = 5;
    while (*name && *name != ';')
    {
        char val = *name++;
        if (len >= 254 || val == ':')
            return false;
        if (val == '\\')
            val = '/';
        path[len++] = val >= 'a' && val <= 'z' ? val - 'a' + 'A' : val;
    }
    path[len] = 0;
    return len > 5 && !strstr(path, "..");
}

bool FindDataCdFile(CdlFILE *result, const char *name)
{
    char path[256];
    if (!ResolveDataPath(path, name))
        return false;
    DataCdFile *entry = 0;
    for (uint32 idx = 0; idx < data_cd_count; ++idx)
        if (!strcmp(data_cd_files[idx].path, path))
            entry = data_cd_files + idx;
    if (!entry)
    {
        FILE *file = xport_fopen(path, "rb");
        if (!file)
            return false;
        RiffFileHeader header;
        bool valid = xport_fread(&header, 1, sizeof(header), file) == sizeof(header);
        uint32 payload = 0;
        if (valid && !memcmp(header.magic, "RIFF", 4) && !memcmp(header.format, "CDXA", 4))
        {
            uint32 end = header.bytes;
            uint32 pos = 12;
            valid = end <= 0x7FFFFFF7;
            end += 8;
            while (valid && pos <= end && end - pos >= 8)
            {
                RiffChunkFile chunk;
                valid = xport_fread(&chunk, 1, sizeof(chunk), file) == sizeof(chunk);
                if (!valid)
                    break;
                uint32 size = chunk.bytes;
                if (size > end - pos - 8)
                    break;
                if (!memcmp(chunk.kind, "data", 4))
                {
                    payload = size;
                    break;
                }
                pos += 8 + size + (size & 1);
                valid = fseek(file, (long)pos, SEEK_SET) == 0;
            }
        }
        else if (valid && !header.magic[0] && !header.format[3] && fseek(file, 0, SEEK_END) == 0)
        {
            long size = ftell(file);
            if (size > 0)
                payload = size;
        }
        fclose(file);
        if (!valid || !payload || payload % 2352 || data_cd_count == 64)
            FailDataRead(path);
        entry = data_cd_files + data_cd_count++;
        strcpy(entry->path, path);
        entry->sector = data_cd_next_sector;
        entry->sectors = payload / 2352;
        data_cd_next_sector += entry->sectors + 1;
    }
    memset(result, 0, sizeof(*result));
    CdIntToPos(entry->sector, &result->pos);
    result->size = entry->sectors * 2048;
    size_t name_len = strlen(path + 5);
    if (name_len >= sizeof(result->name))
        name_len = sizeof(result->name) - 1;
    memcpy(result->name, path + 5, name_len);
    return true;
}

static const void *PrepareDataCdLocation(uint32 command, const void *parameter, CdlLOC *location)
{
    if (!parameter || (command != 2 && command != 21 && command != 22 && command != 27))
        return parameter;
    const CdlLOC *source = static_cast<const CdlLOC *>(parameter);
    CdlLOC input;
    input.minute = source->minute;
    input.second = source->second;
    input.sector = source->sector;
    input.track = 0;
    sint32 sector = CdPosToInt(&input);
    for (uint32 idx = 0; idx < data_cd_count; ++idx)
    {
        DataCdFile *entry = data_cd_files + idx;
        if (sector >= entry->sector && (uint32)(sector - entry->sector) < entry->sectors)
        {
            if (data_cd_active != entry)
            {
                if (!cd_mount_stream_file(entry->path))
                    FailDataRead(entry->path);
                data_cd_active = entry;
            }
            CdIntToPos(sector - entry->sector, location);
            return location;
        }
    }
    FailDataRead("Unmapped stream position");
    return 0;
}

static void RebaseDataCdResponse(uint8 *response)
{
    if (response && data_cd_active)
    {
        CdlLOC location;
        location.minute = response[5];
        location.second = response[6];
        location.sector = response[7];
        location.track = 0;
        sint32 sector = CdPosToInt(&location);
        CdIntToPos(sector + data_cd_active->sector, &location);
        memcpy(response + 5, &location, 3);
    }
}

bool InitializeDataCd()
{
    CdlFILE file;
    CdlLOC local;
    if (!FindDataCdFile(&file, "APOC.XA"))
        return false;
    PrepareDataCdLocation(2, &file.pos, &local);
    return true;
}

bool ReadDataResource(void *dst, uint32 offset, uint32 bytes)
{
    FILE *file = xport_fopen("DATA/CD.WAD", "rb");
    if (!file)
        return false;
    bool success = offset <= 0x7FFFFFFF && fseek(file, (long)offset, SEEK_SET) == 0 && xport_fread(dst, 1, bytes, file) == bytes;
    fclose(file);
    return success;
}

uint32 ControlDataCd(uint32 command, const void *parameter, void *output)
{
    CdlLOC location;
    FUNCTION_MARKER(0x80097DF8, "SLUS_003.73");
    parameter = PrepareDataCdLocation(command, parameter, &location);
    return CdControlB(command, (uint8 *)parameter, (uint8 *)output);
}

uint32 FastControlDataCd(uint32 command, const void *parameter)
{
    CdlLOC location;
    FUNCTION_MARKER(0x80097F34, "SLUS_003.73");
    parameter = PrepareDataCdLocation(command, parameter, &location);
    return CdControlF(command, (uint8 *)parameter);
}

uint32 SyncDataCd(sint32 mode, uint8 *response)
{
    FUNCTION_MARKER(0x80097D90, "SLUS_003.73");
    uint32 completion = CdSync(mode, response);
    if (completion == CdlComplete && CdLastCom() == CdlGetlocP)
        RebaseDataCdResponse(response);
    return completion;
}
