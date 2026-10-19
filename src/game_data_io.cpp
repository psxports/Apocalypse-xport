#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game_diagnostics.h"
#include "game_data_io.h"

struct DataCdFile
{
    char path[256];
    uint32 sector;
    uint32 sectors;
};

static DataCdFile data_cd_files[64];
static uint32 data_cd_count;
static uint32 data_cd_next_sector = 1;
static DataCdFile *data_cd_active;

static void FailDataRead(const char *path)
{
    GameDiagnostic("Cannot read game data: %s\n", path);
    abort();
}

static bool ResolveDataPath(char *path, const char *name)
{
    strcpy(path, "DATA/");
    while (*name == '/' || *name == '\\')
        ++name;
    uint32 length = 5;
    while (*name && *name != ';')
    {
        uint8 value = (uint8)*name++;
        if (length >= 254 || value == ':')
            return false;
        if (value == '\\')
            value = '/';
        path[length++] = value >= 'a' && value <= 'z' ? value - 'a' + 'A' : value;
    }
    path[length] = 0;
    return length > 5 && !strstr(path, "..");
}

bool FindDataCdFile(CdlFILE *result, const char *name)
{
    char path[256];
    if (!ResolveDataPath(path, name))
        return false;
    DataCdFile *entry = 0;
    for (uint32 index = 0; index < data_cd_count; ++index)
        if (!strcmp(data_cd_files[index].path, path))
            entry = data_cd_files + index;
    if (!entry)
    {
        FILE *file = xport_fopen(path, "rb");
        if (!file)
            return false;
        uint8 header[12];
        bool valid = xport_fread(header, 1, sizeof(header), file) == sizeof(header);
        uint32 payload = 0;
        if (valid && !memcmp(header, "RIFF", 4) && !memcmp(header + 8, "CDXA", 4))
        {
            uint32 end = xport_load_le32(header + 4);
            uint32 position = 12;
            valid = end <= 0x7FFFFFF7;
            end += 8;
            while (valid && position <= end && end - position >= 8)
            {
                uint8 chunk[8];
                valid = xport_fread(chunk, 1, 8, file) == 8;
                if (!valid)
                    break;
                uint32 size = xport_load_le32(chunk + 4);
                if (size > end - position - 8)
                    break;
                if (!memcmp(chunk, "data", 4))
                {
                    payload = size;
                    break;
                }
                position += 8 + size + (size & 1);
                valid = fseek(file, (long)position, SEEK_SET) == 0;
            }
        }
        else if (valid && !header[0] && !header[11] && fseek(file, 0, SEEK_END) == 0)
        {
            long size = ftell(file);
            if (size > 0)
                payload = (uint32)size;
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
    CdIntToPos((sint32)entry->sector, &result->pos);
    result->size = entry->sectors * 2048;
    size_t name_length = strlen(path + 5);
    if (name_length >= sizeof(result->name))
        name_length = sizeof(result->name) - 1;
    memcpy(result->name, path + 5, name_length);
    return true;
}

static const void *PrepareDataCdLocation(uint32 command, const void *parameter, CdlLOC *location)
{
    if (!parameter || (command != 2 && command != 21 && command != 22 && command != 27))
        return parameter;
    sint32 sector = CdPosToInt((CdlLOC *)parameter);
    for (uint32 index = 0; index < data_cd_count; ++index)
    {
        DataCdFile *entry = data_cd_files + index;
        if (sector >= (sint32)entry->sector && (uint32)sector - entry->sector < entry->sectors)
        {
            if (data_cd_active != entry)
            {
                if (!cd_mount_stream_file(entry->path))
                    FailDataRead(entry->path);
                data_cd_active = entry;
            }
            CdIntToPos(sector - (sint32)entry->sector, location);
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
        sint32 sector = CdPosToInt((CdlLOC *)(response + 5));
        CdIntToPos(sector + (sint32)data_cd_active->sector, &location);
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

bool ReadDataResource(void *destination, uint32 offset, uint32 bytes)
{
    FILE *file = xport_fopen("DATA/CD.WAD", "rb");
    if (!file)
        return false;
    bool success = offset <= 0x7FFFFFFF && fseek(file, (long)offset, SEEK_SET) == 0 && xport_fread(destination, 1, bytes, file) == bytes;
    fclose(file);
    return success;
}

uint32 ControlDataCd(uint32 command, const void *parameter, void *output)
{
    CdlLOC location;
    FUNCTION_MARKER(0x80097DF8, "SLUS_003.73");
    parameter = PrepareDataCdLocation(command, parameter, &location);
    return CdControlB((uint8)command, (uint8 *)parameter, (uint8 *)output);
}

uint32 FastControlDataCd(uint32 command, const void *parameter)
{
    CdlLOC location;
    FUNCTION_MARKER(0x80097F34, "SLUS_003.73");
    parameter = PrepareDataCdLocation(command, parameter, &location);
    return CdControlF((uint8)command, (uint8 *)parameter);
}

uint32 SyncDataCd(sint32 mode, uint8 *response)
{
    FUNCTION_MARKER(0x80097D90, "SLUS_003.73");
    uint32 completion = CdSync(mode, response);
    if (completion == CdlComplete && CdLastCom() == CdlGetlocP)
        RebaseDataCdResponse(response);
    return completion;
}
