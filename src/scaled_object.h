#ifndef SCALED_OBJECT_H
#define SCALED_OBJECT_H
extern "C"
{
#include "psx.h"
}

#include "object.h"
#include "model_instance.h"

class ScaledObject : public Object
{
public:
    ModelInstance model;

    virtual ~ScaledObject()
    {
    }

    virtual uint32 Destroy(uint32 reason);

protected:
    ScaledObject();
};

#endif
