#include "object_reference_base.h"
#include "object.h"

ObjectReferenceBase::ObjectReferenceBase() : object(0), prev(0), next(0)
{
}

ObjectReferenceBase::ObjectReferenceBase(const ObjectReferenceBase &other) : object(0), prev(0), next(0)
{
    Bind(other.object);
}

ObjectReferenceBase::~ObjectReferenceBase()
{
    Reset();
}

ObjectReferenceBase &ObjectReferenceBase::operator=(const ObjectReferenceBase &other)
{
    if (this != &other)
        Bind(other.object);
    return *this;
}

void ObjectReferenceBase::Reset()
{
    if (prev)
        prev->next = next;
    else if (object)
        object->references = next;
    if (next)
        next->prev = prev;
    object = 0;
    prev = 0;
    next = 0;
}

void ObjectReferenceBase::Bind(Object *target)
{
    Reset();
    if (!target || !target->IsAlive())
        return;
    object = target;
    next = target->references;
    if (next)
        next->prev = this;
    target->references = this;
}
