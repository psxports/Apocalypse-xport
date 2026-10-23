#ifndef ACTIVATION_RECORD_BASE_H
#define ACTIVATION_RECORD_BASE_H

#include "vector3.h"
#include "object.h"
#include "activation_record.h"

class ActivationRecordBase : public Object
{
public:
    virtual ~ActivationRecordBase()
    {
    }

protected:
    ActivationRecordBase()
    {
    }

public:
    virtual uint32 Destroy(uint32 reason);
};

#endif
