#ifndef OBJECT_REFERENCE_H
#define OBJECT_REFERENCE_H

#include "object.h"
#include "object_reference_base.h"

template <class T>
class ObjectReference : private ObjectReferenceBase
{
public:
    ObjectReference()
    {
    }

    explicit ObjectReference(T *target)
    {
        Bind(target);
    }

    ObjectReference(const ObjectReference &other) : ObjectReferenceBase(other)
    {
    }

    ~ObjectReference()
    {
    }

    ObjectReference &operator=(const ObjectReference &other)
    {
        ObjectReferenceBase::operator=(other);
        return *this;
    }

    ObjectReference &operator=(T *target)
    {
        Bind(target);
        return *this;
    }

    T *Get() const
    {
        return static_cast<T *>(GetObject());
    }

    T *operator->() const
    {
        return Get();
    }

    operator T *() const
    {
        return Get();
    }
};

#endif
