#ifndef ACTIVATION_RECORD_H
#define ACTIVATION_RECORD_H

extern "C"
{
#include "psx.h"
}

// TODO Recover the original class names
class ActivationRecordBase
{
public:
    virtual ~ActivationRecordBase() {}
    static ActivationRecordBase &Attach(uint32 object);
    virtual uint32 Destroy(uint32 reason);

protected:
    // Bind the native base without adding original memory writes
    ActivationRecordBase() {}
};

class ActivationRecord : public ActivationRecordBase
{
public:
    ActivationRecord(uint32 index, uint32 state);
    virtual ~ActivationRecord() {}
    static ActivationRecord &Attach(uint32 object);
    virtual uint32 Destroy(uint32 reason);
};

uint32 ClearActivationRecords();
void DeleteActivationRecords();
void RestoreNearbyObjects();
uint32 DeactivateDistantObject(uint32 object, uint32 distance);

#endif
