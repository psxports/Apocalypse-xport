#ifndef SCALED_OBJECT_H
#define SCALED_OBJECT_H
extern "C"
{
#include "psx.h"
}

class ScaledObject
{
public:
    virtual ~ScaledObject() {}
    virtual uint32 Destroy(uint32 reason);

protected:
    ScaledObject();
};

#endif
