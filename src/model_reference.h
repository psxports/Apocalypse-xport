#ifndef MODEL_REFERENCE_H
#define MODEL_REFERENCE_H

#include "psx.h"

struct ModelInstance;
struct ModelPolygon;

class ModelReference
{
private:
    ModelInstance *object;
    ModelPolygon *poly;
    ModelReference *prev;
    ModelReference *next;

public:
    void Reset();
    ModelReference();
    ModelReference(const ModelReference &other);
    ~ModelReference();
    ModelReference &operator=(const ModelReference &other);
    void Bind(ModelInstance *object, ModelPolygon *poly);
    void SetObject(ModelInstance *object);
    void SetPolygon(ModelPolygon *poly);
    ModelInstance *GetObject() const;
    ModelPolygon *GetPolygon() const;
};

#endif
