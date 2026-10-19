#ifndef ACTIVATION_RECORD_H
#define ACTIVATION_RECORD_H

#include "vector3.h"

extern "C"
{
#include "psx.h"
}

// TODO Recover the original class names
#include "object.h"

class ActivationRecordBase : public Object
{
public:
    virtual ~ActivationRecordBase()
    {
    }

    virtual uint32 Destroy(uint32 reason);

protected:
    ActivationRecordBase()
    {
    }
};

class ActivationRecord : public ActivationRecordBase
{
public:
    ActivationRecord(uint32 index, uint32 state);
    uint16 record_idx;
    sint16 saved_health;
    Vector3 pos;
    ActivationRecord *next;

    virtual ~ActivationRecord()
    {
    }

    virtual uint32 Destroy(uint32 reason);
};

uint32 ClearActivationRecords();
void DeleteActivationRecords();
void RestoreNearbyObjects();

#endif
