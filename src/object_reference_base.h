#ifndef OBJECT_REFERENCE_BASE_H
#define OBJECT_REFERENCE_BASE_H

#include "psx.h"

// Intrusive native references expire at logical destruction without an address registry
class Object;

class ObjectReferenceBase
{
public:
    Object *GetObject() const
    {
        return object;
    }

private:
    friend class Object;
    Object *object;
    ObjectReferenceBase *prev;
    ObjectReferenceBase *next;

public:
    void Reset();
    ObjectReferenceBase();
    ObjectReferenceBase(const ObjectReferenceBase &other);
    ~ObjectReferenceBase();
    ObjectReferenceBase &operator=(const ObjectReferenceBase &other);
    void Bind(Object *target);
};

#endif
