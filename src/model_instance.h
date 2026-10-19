#ifndef MODEL_INSTANCE_H
#define MODEL_INSTANCE_H

#include "vector3.h"

extern "C"
{
#include "psx.h"
}

#include "object.h"

void BeginModelBorrow();
void EndModelBorrow();
void RetireModelMemory(void *memory);

class ModelBorrow
{
public:
    ModelBorrow()
    {
        BeginObjectBorrow();
        BeginModelBorrow();
    }

    ~ModelBorrow()
    {
        EndObjectBorrow();
        EndModelBorrow();
    }

private:
    ModelBorrow(const ModelBorrow &);
    ModelBorrow &operator=(const ModelBorrow &);
};

class ScaledObject;
struct ModelInstance;
struct ModelPolygon;

class ModelReference
{
public:
    ModelReference();
    ModelReference(const ModelReference &other);
    ~ModelReference();
    ModelReference &operator=(const ModelReference &other);
    void Bind(ModelInstance *object, ModelPolygon *poly);
    void Reset();
    void SetObject(ModelInstance *object);
    void SetPolygon(ModelPolygon *poly);
    ModelInstance *GetObject() const;
    ModelPolygon *GetPolygon() const;

private:
    ModelInstance *object;
    ModelPolygon *poly;
    ModelReference *prev;
    ModelReference *next;
};

// Common model values follow original offsets; list identity has a separate lifetime
struct ModelInstance
{
    uint16 flags;
    uint16 generation;
    Vector3 pos;
    ShortVector3 angles;
    uint16 model_idx;
    uint8 frame;
    uint8 flash;
    uint8 animation;
    uint8 bank;
    uint32 color;
    ModelInstance *next;
    ModelInstance *prev;
    ShortVector3 scale;
    uint32 render_mode;
    ScaledObject *owner;
    bool active;

    ModelInstance() : flags(0), generation(0), model_idx(0), frame(0), flash(0), animation(0), bank(0), color(0), next(0), prev(0), render_mode(0), owner(0), active(true), references(0)
    {
        for (uint32 axis = 0; axis < 3; ++axis)
        {
            pos.words[axis] = 0;
            angles.words[axis] = 0;
            scale.words[axis] = ONE;
        }
    }

    ~ModelInstance();
    void InvalidateReferences();

    void SetAnglesXY(uint32 packed)
    {
        angles.words[0] = packed;
        angles.words[1] = packed >> 16;
    }

private:
    friend class ModelReference;
    ModelReference *references;
    ModelInstance(const ModelInstance &);
    ModelInstance &operator=(const ModelInstance &);
};

inline ModelReference::ModelReference() : object(0), poly(0), prev(0), next(0)
{
}

inline ModelReference::ModelReference(const ModelReference &other) : object(0), poly(0), prev(0), next(0)
{
    Bind(other.object, other.poly);
}

inline ModelReference::~ModelReference()
{
    Reset();
}

inline ModelReference &ModelReference::operator=(const ModelReference &other)
{
    if (this != &other)
        Bind(other.object, other.poly);
    return *this;
}

inline void ModelReference::Reset()
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

inline void ModelReference::Bind(ModelInstance *target, ModelPolygon *polygon)
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

inline void ModelReference::SetObject(ModelInstance *target)
{
    Bind(target, target == object ? poly : 0);
}

inline void ModelReference::SetPolygon(ModelPolygon *polygon)
{
    poly = object ? polygon : 0;
}

inline void ModelInstance::InvalidateReferences()
{
    active = false;
    while (references)
        references->Reset();
}

inline ModelInstance::~ModelInstance()
{
    InvalidateReferences();
}

#endif
