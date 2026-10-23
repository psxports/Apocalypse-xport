#ifndef RESOURCE_SCRIPT_H
#define RESOURCE_SCRIPT_H

#include "resource_script_borrow.h"
#include "checkpoint_names.h"
#include "record_data.h"

extern "C"
{
#include "psx.h"
}

extern CheckpointNames checkpoint_names;
const char *GetRecModelBankName();
uint8 GetRecModelBankSlot();
void SelectRecModelBank(const char *name);

struct ResourceScript
{
    ResourceScript() : script(), touched(false), active(false), spawn_cycles_initialized(false), spawn_count(0), spawn_cycles_left(0), idx(0), resource(0), hash_next(0), next(0), retired_next(0)
    {
    }

    RecordCursor script;
    bool touched;
    bool active;
    bool spawn_cycles_initialized;
    uint8 spawn_count;
    uint16 spawn_cycles_left;
    uint16 idx;
    uint32 resource;
    ResourceScript *hash_next;
    ResourceScript *next;
    ResourceScript *retired_next;
};

uint16 GetStartRecIdx();

ResourceScript *RegisterResourceScript(uint32 resource, uint16 idx, const RecordCursor &script);
void ClearResourceScripts();
ResourceScript *FindScriptRec(uint32 idx);
void ClearScriptTouches();
void ReleaseUntouchedScripts();
ResourceScript *ActivateResourceScript(uint32 resource);
uint32 SelectStartRec(const char *name);
uint32 ApplyStartRec();
void UnloadLevelRecs();
uint32 LoadLevelRecs(const char *basename);

#endif
