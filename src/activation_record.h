#ifndef ACTIVATION_RECORD_H
#define ACTIVATION_RECORD_H

#include "vector3.h"

extern "C"
{
#include "psx.h"
}

// TODO Recover the original class names
#include "object.h"
#include "activation_record_base.h"

class ActivationRecord : public ActivationRecordBase
{
public:
    virtual ~ActivationRecord()
    {
    }

private:
    uint16 rec_idx;
    sint16 saved_health;
    Vector3 pos;
    ActivationRecord *next;

public:
    static uint32 ClearAll();
    static void DeleteAll();
    ActivationRecord(uint16 idx, sint16 health);
    virtual uint32 Destroy(uint32 reason);
    static void RestoreNearby();
};

#endif
