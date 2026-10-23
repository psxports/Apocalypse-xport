#include <stdlib.h>
#include <new>
#include <string.h>

#include "level_session.h"
#include "visual_object.h"
#include "player.h"
#include "angle_math.h"
#include "game_diagnostics.h"
#include "resource_script.h"
#include "global.h"
#include "record_commands.h"
#include "record_data.h"
#include "record_factory.h"
#include "game_panel.h"
#include "game_heap.h"
#include "model_bank.h"
#include "resource_io.h"

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

static RecordModelBankSelection rec_model_bank;

const char *GetRecModelBankName()
{
    return rec_model_bank.name;
}

uint8 GetRecModelBankSlot()
{
    return rec_model_bank.slot;
}

void SelectRecModelBank(const char *name)
{
    char *copy = AllocateStringCopy(name);
    free(rec_model_bank.name);
    rec_model_bank.name = copy;
    rec_model_bank.slot = ModelBank::FindByName(copy);
}

static uint16 start_rec_idx = 65535;

uint16 GetStartRecIdx()
{
    return start_rec_idx;
}

static ResourceScript *resource_scripts;
static ResourceScript *resource_script_buckets[256];

ResourceScript *RegisterResourceScript(uint32 resource, uint16 idx, const RecordCursor &script)
{
    FUNCTION_MARKER(0x80063B5C, "SLUS_003.73");
    void *storage = malloc(sizeof(ResourceScript));
    if (!storage)
    {
        GameDiagnostic(("Cannot allocate resource script\n"));
        abort();
    }
    ResourceScript *rec = new (storage) ResourceScript();
    rec->next = resource_scripts;
    uint32 bucket = resource & 255;
    rec->hash_next = resource_script_buckets[bucket];
    resource_scripts = rec;
    resource_script_buckets[bucket] = rec;
    rec->idx = idx;
    rec->script = script;
    rec->resource = resource;
    return rec;
}

void ClearResourceScripts()
{
    FUNCTION_MARKER(0x80063BF0, "SLUS_003.73");
    for (sint32 idx = 255; idx >= 0; --idx)
        resource_script_buckets[idx] = 0;
    ResourceScript *rec = resource_scripts;
    resource_scripts = 0;
    while (rec)
    {
        ResourceScript *next = rec->next;
        if (ResourceScriptBorrow::script_borrows)
        {
            rec->retired_next = ResourceScriptBorrow::retired_scripts;
            ResourceScriptBorrow::retired_scripts = rec;
        }
        else
            ResourceScriptBorrow::DestroyResourceScript(rec);
        rec = next;
    }
}

ResourceScript *FindScriptRec(uint32 idx)
{
    RecordDataBorrow rec_borrow;
    FUNCTION_MARKER(0x80063C54, "SLUS_003.73");
    if (idx == 65535)
        return 0;
    RecordCursor desc(GetLevelRec(idx));
    if (desc.Read16() != 6)
        return 0;
    ResourceScript *rec = resource_scripts;
    while (rec)
    {
        if (rec->idx == idx)
            return rec;
        rec = rec->next;
    }
    return 0;
}

void ClearScriptTouches()
{
    FUNCTION_MARKER(0x80063CC4, "SLUS_003.73");
    ResourceScript *rec = resource_scripts;
    while (rec)
    {
        rec->touched = false;
        rec = rec->next;
    }
}

void ReleaseUntouchedScripts()
{
    FUNCTION_MARKER(0x80063CF0, "SLUS_003.73");
    ResourceScript *rec = resource_scripts;
    while (rec)
    {
        if (rec->active && !rec->touched)
            rec->active = false;
        rec = rec->next;
    }
}

ResourceScript *ActivateResourceScript(uint32 resource)
{
    ResourceScriptBorrow borrow;
    FUNCTION_MARKER(0x80063D3C, "SLUS_003.73");
    ResourceScript *rec = resource_script_buckets[resource & 255];
    while (rec)
    {
        if (rec->resource == resource)
        {
            bool active = rec->active;
            rec->touched = true;
            if (!active)
            {
                ExecuteRecCommands(rec->script, rec->idx, false);
                rec->active = true;
                return rec;
            }
        }
        rec = rec->hash_next;
    }
    return 0;
}

uint32 SelectStartRec(const char *name)
{
    RecordDataBorrow rec_borrow;
    uint32 idx = 0;
    Vector3 pos;
    RecordCursor desc;
    RecordCursor payload;
    FUNCTION_MARKER(0x80063DD4, "SLUS_003.73");
    start_rec_idx = 65535;
    while (idx < GetLevelRecCount())
    {
        desc = GetLevelRec(idx);
        if (desc.Read16() == 8)
        {
            payload = ReadRecPos(&pos, idx);
            if (NamesEqualIgnoringCase((payload + 6).CStr(), name))
            {
                start_rec_idx = idx;
                return 1;
            }
        }
        idx += 1;
    }
    return idx << 2;
}

uint32 ApplyStartRec()
{
    RecordDataBorrow rec_borrow;
    Vector3 pos;
    FUNCTION_MARKER(0x80063E7C, "SLUS_003.73");
    RecordCursor payload(ReadRecPos(&pos, start_rec_idx));
    Player &player = *static_cast<Player *>(visual_lists.players->owner);
    player.model.pos = pos;
    for (uint32 axis = 0; axis < 3; ++axis)
        player.model.angles.comp[axis] = (payload + axis * 2).Read16();
    RecordCursor cursor = SkipRecString(RecordCursor(payload + 6));
    ExecuteRecCommands(cursor, start_rec_idx, true);
    return ResetPanelDispTimer();
}

void UnloadLevelRecs()
{
    FUNCTION_MARKER(0x80063F20, "SLUS_003.73");
    ReleaseLevelRecData();
    checkpoint_names.Clear();
}

uint32 LoadLevelRecs(const char *basename)
{
    RecordDataBorrow rec_borrow;
    char filename[32];
    uint32 idx;
    Vector3 pos;
    RecordCursor desc;
    RecordCursor payload;
    FUNCTION_MARKER(0x80063F4C, "SLUS_003.73");
    game_progress.level.enemy_total = 0;
    game_progress.level.enemy_kills = 0;
    size_t len = strlen(basename);
    if (len > sizeof(filename) - sizeof(".trg"))
    {
        GameDiagnostic(("Level record filename exceeds buffer\n"));
        abort();
    }
    memcpy(filename, basename, len);
    memcpy(filename + len, ".trg", sizeof(".trg"));
    uint32 file_size = LookupResource(filename);
    uint32 src_bytes = GetResourceFileSize(filename);
    uint8 *data = (uint8 *)malloc(file_size);
    if (!data)
    {
        GameDiagnostic(("Cannot allocate level record data\n"));
        abort();
    }
    BeginResourceRead(data);
    WaitForResourceRead();
    AdoptLevelRecData(data, src_bytes);
    if (!GetLevelRecCount())
    {
        // TODO Original zero-count load reads an invalid preceding table entry
        GameDiagnostic(("TODO Empty level record table\n"));
        abort();
    }
    ModelBank::UnloadUnprotected();
    checkpoint_names.Clear();
    for (idx = 0; idx < GetLevelRecCount(); idx += 1)
    {
        desc = GetLevelRec(idx);
        if (desc.Read16() == 8)
        {
            payload = ReadRecPos(&pos, idx);
            checkpoint_names.Add((payload + 6).CStr());
        }
    }
    start_rec_idx = 65535;
    for (idx = 0; idx < GetLevelRecCount(); idx += 1)
    {
        desc = GetLevelRec(idx);
        if (desc.Read16() == 4)
            ExecuteRecCommands(RecordCursor(desc + 2), idx, true);
    }
    return InitializeLevelRecs();
}
