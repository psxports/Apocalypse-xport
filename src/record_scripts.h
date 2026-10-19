#ifndef RECORD_SCRIPTS_H
#define RECORD_SCRIPTS_H

#include "record_data.h"

extern "C"
{
#include "psx.h"
}

class CheckpointNames
{
public:
    CheckpointNames();
    ~CheckpointNames();
    void Clear();
    void Add(const char *name);
    uint32 Count() const;
    const char *Get(uint32 index) const;

private:
    CheckpointNames(const CheckpointNames &);
    CheckpointNames &operator=(const CheckpointNames &);
    char **names;
    uint32 count;
    uint32 capacity;
};

extern CheckpointNames checkpoint_names;
const char *GetRecordModelBankName();
uint8 GetRecordModelBankSlot();
void SelectRecordModelBank(const char *name);

struct ResourceScript
{
    ResourceScript() : script(), touched(0), active(0), spawn_cycles_initialized(0), spawn_count(0), spawn_cycles_left(0), index(0), resource(0), hash_next(0), next(0), retired_next(0)
    {
    }

    RecordCursor script;
    uint8 touched;
    uint8 active;
    uint8 spawn_cycles_initialized;
    uint8 spawn_count;
    uint16 spawn_cycles_left;
    uint16 index;
    uint32 resource;
    ResourceScript *hash_next;
    ResourceScript *next;
    ResourceScript *retired_next;
};

class ResourceScriptBorrow
{
public:
    ResourceScriptBorrow();
    ~ResourceScriptBorrow();

private:
    RecordDataBorrow data_borrow;
    ResourceScriptBorrow(const ResourceScriptBorrow &);
    ResourceScriptBorrow &operator=(const ResourceScriptBorrow &);
};

ResourceScript *RegisterResourceScript(uint32 resource, uint32 index, const RecordCursor &script);
void ClearResourceScripts();
ResourceScript *FindScriptRecord(uint32 index);
ResourceScript *ActivateResourceScript(uint32 resource);
void UnloadLevelRecords();
uint32 LoadLevelRecords(const char *basename);
uint32 SelectStartRecord(const char *name);
uint32 GetStartRecordIndex();
uint32 ApplyStartRecord();
void ClearScriptTouches();
void ReleaseUntouchedScripts();

#endif
