#ifndef FORCE_H
#define FORCE_H

#include "object.h"

class Force : public Object
{
protected:
    Force()
    {
    }

public:
    virtual uint32 Destroy(uint32 reason);
};

#endif
