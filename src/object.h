#ifndef OBJECT_H
#define OBJECT_H

extern "C"
{
#include "psx.h"
}

// Native lifetime interface shared by the game's allocation owners
class Object
{
public:
    virtual ~Object();
    virtual uint32 Destroy(uint32 reason) = 0;

protected:
    Object();
};

#endif
