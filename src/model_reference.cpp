#include "model_reference.h"
#include "model_instance.h"
#include "scaled_object.h"

ModelReference::ModelReference() : object(0), poly(0), prev(0), next(0)
{
}

ModelReference::ModelReference(const ModelReference &other) : object(0), poly(0), prev(0), next(0)
{
    Bind(other.object, other.poly);
}

ModelReference::~ModelReference()
{
    Reset();
}

ModelReference &ModelReference::operator=(const ModelReference &other)
{
    if (this != &other)
        Bind(other.object, other.poly);
    return *this;
}

void ModelReference::Reset()
{
    if (prev)
        prev->next = next;
    else if (object)
        object->references = next;
    if (next)
        next->prev = prev;
    object = 0;
    poly = 0;
    prev = 0;
    next = 0;
}

void ModelReference::Bind(ModelInstance *target, ModelPolygon *polygon)
{
    Reset();
    if (!target || !target->active)
        return;
    object = target;
    poly = polygon;
    next = target->references;
    if (next)
        next->prev = this;
    target->references = this;
}

void ModelReference::SetObject(ModelInstance *target)
{
    Bind(target, target == object ? poly : 0);
}

void ModelReference::SetPolygon(ModelPolygon *polygon)
{
    poly = object ? polygon : 0;
}

ModelInstance *ModelReference::GetObject() const
{
    if (!object || !object->active)
        return 0;
    return !object->owner || object->owner->IsAlive() ? object : 0;
}

ModelPolygon *ModelReference::GetPolygon() const
{
    return GetObject() ? poly : 0;
}
