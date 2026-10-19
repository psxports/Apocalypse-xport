#include <stdlib.h>
#include <new>
#include <string.h>

#include "level_session.h"
#include "visual_object.h"
#include "player.h"
#include "angle_math.h"
#include "game_diagnostics.h"
#include "record_scripts.h"
#include "game_string.h"
#include "record_commands.h"
#include "record_data.h"
#include "record_factory.h"
#include "game_panel.h"
#include "game_heap.h"
#include "model_resources.h"
#include "resource_io.h"

static char *CopyRecordName(const char *name)
{
    size_t length = strlen(name);
    char *copy = (char *)malloc(length + 1);
    if (!copy)
    {
        GameDiagnostic("Cannot allocate record name\n");
        abort();
    }
    memcpy(copy, name, length + 1);
    return copy;
}

CheckpointNames checkpoint_names;

CheckpointNames::CheckpointNames() : names(0), count(0), capacity(0)
{
}

CheckpointNames::~CheckpointNames()
{
    Clear();
}

void CheckpointNames::Clear()
{
    for (uint32 index = 0; index < count; ++index)
        free(names[index]);
    free(names);
    names = 0;
    count = 0;
    capacity = 0;
}

void CheckpointNames::Add(const char *name)
{
    if (count == capacity)
    {
        size_t limit = static_cast<size_t>(-1) / sizeof(*names);
        if (count >= limit || count == 0xFFFFFFFF)
        {
            GameDiagnostic("Checkpoint name count overflow\n");
            abort();
        }
        uint32 next_capacity = capacity ? capacity : 8;
        if (next_capacity <= count)
            next_capacity = capacity > 0x7FFFFFFF || capacity > limit / 2 ? count + 1 : capacity * 2;
        if (next_capacity > limit)
            next_capacity = count + 1;
        char **next = (char **)realloc(names, next_capacity * sizeof(*names));
        if (!next)
        {
            GameDiagnostic("Cannot allocate checkpoint names\n");
            abort();
        }
        names = next;
        capacity = next_capacity;
    }
    names[count++] = CopyRecordName(name);
}

uint32 CheckpointNames::Count() const
{
    return count;
}

const char *CheckpointNames::Get(uint32 index) const
{
    if (index >= count)
    {
        GameDiagnostic("Checkpoint name index out of bounds\n");
        abort();
    }
    return names[index];
}

// The selected bank survives record unload until the next selection
struct RecordModelBankSelection
{
    RecordModelBankSelection() : name(0), slot(0)
    {
    }

    ~RecordModelBankSelection()
    {
        free(name);
    }

    char *name;
    uint8 slot;
};

static RecordModelBankSelection record_model_bank;

const char *GetRecordModelBankName()
{
    return record_model_bank.name;
}

uint8 GetRecordModelBankSlot()
{
    return record_model_bank.slot;
}

void SelectRecordModelBank(const char *name)
{
    char *copy = CopyRecordName(name);
    free(record_model_bank.name);
    record_model_bank.name = copy;
    record_model_bank.slot = FindModelBankByName(copy);
}

static uint32 start_record_index = 65535;

uint32 GetStartRecordIndex()
{
    return start_record_index;
}

static ResourceScript *resource_scripts;
static ResourceScript *resource_script_buckets[256];
static ResourceScript *retired_scripts;
static uint32 script_borrows;

static void DestroyResourceScript(ResourceScript *record)
{
    record->~ResourceScript();
    free(record);
}

ResourceScriptBorrow::ResourceScriptBorrow()
{
    ++script_borrows;
}

ResourceScriptBorrow::~ResourceScriptBorrow()
{
    if (--script_borrows)
        return;
    while (retired_scripts)
    {
        ResourceScript *record = retired_scripts;
        retired_scripts = record->retired_next;
        DestroyResourceScript(record);
    }
}

ResourceScript *FindScriptRecord(uint32 index)
{
    RecordDataBorrow record_borrow;
    FUNCTION_MARKER(0x80063C54, "SLUS_003.73");
    if (index == 65535)
        return 0;
    RecordCursor descriptor(GetLevelRecord(index));
    if ((sint16)(descriptor).Read16() != 6)
        return 0;
    ResourceScript *record = resource_scripts;
    while (record)
    {
        if (record->index == index)
            return record;
        record = record->next;
    }
    return 0;
}

ResourceScript *ActivateResourceScript(uint32 resource)
{
    ResourceScriptBorrow borrow;
    FUNCTION_MARKER(0x80063D3C, "SLUS_003.73");
    ResourceScript *record = resource_script_buckets[resource & 255];
    while (record)
    {
        if (record->resource == resource)
        {
            uint32 active = record->active;
            record->touched = 1;
            if (!active)
            {
                ExecuteRecordCommands(record->script, record->index, 0);
                record->active = 1;
                return record;
            }
        }
        record = record->hash_next;
    }
    return 0;
}

void ClearScriptTouches()
{
    FUNCTION_MARKER(0x80063CC4, "SLUS_003.73");
    ResourceScript *record = resource_scripts;
    while (record)
    {
        record->touched = 0;
        record = record->next;
    }
}

void ReleaseUntouchedScripts()
{
    FUNCTION_MARKER(0x80063CF0, "SLUS_003.73");
    ResourceScript *record = resource_scripts;
    while (record)
    {
        if (record->active && !record->touched)
            record->active = 0;
        record = record->next;
    }
}

uint32 SelectStartRecord(const char *name)
{
    RecordDataBorrow record_borrow;
    uint32 index = 0, matched;
    Vector3 position;
    RecordCursor descriptor;
    RecordCursor payload;
    FUNCTION_MARKER(0x80063DD4, "SLUS_003.73");
    start_record_index = 65535;
    while ((sint32)index < (sint32)GetLevelRecordCount())
    {
        descriptor = GetLevelRecord(index);
        if ((sint16)(descriptor).Read16() == 8)
        {
            payload = ReadRecordPosition(position.words, index);
            matched = NamesEqualIgnoringCase((payload + 6).CStr(), name);
            if (matched)
            {
                start_record_index = index;
                return matched;
            }
        }
        index += 1;
    }
    return index << 2;
}

uint32 ApplyStartRecord()
{
    RecordDataBorrow record_borrow;
    Vector3 position;
    FUNCTION_MARKER(0x80063E7C, "SLUS_003.73");
    RecordCursor payload(ReadRecordPosition(position.words, start_record_index));
    Player &player = *static_cast<Player *>(visual_lists.players->owner);
    for (uint32 axis = 0; axis < 3; ++axis)
        player.model.pos.words[axis] = position.words[axis];
    RecordCursor angles(payload);
    for (uint32 axis = 0; axis < 3; ++axis)
        player.model.angles.words[axis] = (angles + axis * 2).Read16();
    RecordCursor cursor = SkipRecordString(RecordCursor(angles + 6));
    ExecuteRecordCommands(cursor, start_record_index, 1);
    return ResetPanelDisplayTimer();
}

void UnloadLevelRecords()
{
    FUNCTION_MARKER(0x80063F20, "SLUS_003.73");
    ReleaseLevelRecordData();
    checkpoint_names.Clear();
}

uint32 LoadLevelRecords(const char *basename)
{
    RecordDataBorrow record_borrow;
    char filename[32];
    uint32 index;
    Vector3 position;
    RecordCursor descriptor;
    RecordCursor payload;
    FUNCTION_MARKER(0x80063F4C, "SLUS_003.73");
    game_progress.level.enemy_total = 0;
    game_progress.level.enemy_kills = 0;
    size_t length = strlen(basename);
    if (length > sizeof(filename) - sizeof(".trg"))
    {
        GameDiagnostic("Level record filename exceeds buffer\n");
        abort();
    }
    memcpy(filename, basename, length);
    memcpy(filename + length, ".trg", sizeof(".trg"));
    uint32 file_size = LookupResource(filename);
    uint32 source_bytes = GetResourceFileSize(filename);
    uint8 *data = (uint8 *)malloc(file_size);
    if (!data)
    {
        GameDiagnostic("Cannot allocate level record data\n");
        abort();
    }
    BeginResourceRead(data);
    WaitForResourceRead();
    AdoptLevelRecordData(data, source_bytes);
    if (!GetLevelRecordCount())
    {
        // TODO Original zero-count load reads an invalid preceding table entry
        GameDiagnostic("TODO Empty level record table\n");
        abort();
    }
    UnloadUnprotectedModelBanks();
    checkpoint_names.Clear();
    for (index = 0; (sint32)index < (sint32)GetLevelRecordCount(); index += 1)
    {
        descriptor = GetLevelRecord(index);
        if ((sint16)(descriptor).Read16() == 8)
        {
            payload = ReadRecordPosition(position.words, index);
            checkpoint_names.Add((payload + 6).CStr());
        }
    }
    start_record_index = 65535;
    for (index = 0; (sint32)index < (sint32)GetLevelRecordCount(); index += 1)
    {
        descriptor = GetLevelRecord(index);
        if ((sint16)(descriptor).Read16() == 4)
            ExecuteRecordCommands(RecordCursor(descriptor + 2), index, 1);
    }
    return InitializeLevelRecords();
}

ResourceScript *RegisterResourceScript(uint32 resource, uint32 index, const RecordCursor &script)
{
    FUNCTION_MARKER(0x80063B5C, "SLUS_003.73");
    void *storage = malloc(sizeof(ResourceScript));
    if (!storage)
    {
        GameDiagnostic("Cannot allocate resource script\n");
        abort();
    }
    ResourceScript *record = new (storage) ResourceScript();
    record->next = resource_scripts;
    uint32 bucket = resource & 255;
    record->hash_next = resource_script_buckets[bucket];
    resource_scripts = record;
    resource_script_buckets[bucket] = record;
    record->index = index;
    record->script = script;
    record->resource = resource;
    return record;
}

void ClearResourceScripts()
{
    FUNCTION_MARKER(0x80063BF0, "SLUS_003.73");
    for (sint32 index = 255; index >= 0; --index)
        resource_script_buckets[index] = 0;
    ResourceScript *record = resource_scripts;
    resource_scripts = 0;
    while (record)
    {
        ResourceScript *next = record->next;
        if (script_borrows)
        {
            record->retired_next = retired_scripts;
            retired_scripts = record;
        }
        else
            DestroyResourceScript(record);
        record = next;
    }
}
