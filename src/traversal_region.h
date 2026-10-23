#ifndef TRAVERSAL_REGION_H
#define TRAVERSAL_REGION_H

#include "vector3.h"
#include "visual_object.h"

class TraversalRegion : public VisualObject
{
public:
    virtual ~TraversalRegion()
    {
    }

private:
    Vector3 bounds[2];
    bool reverse;
    Vector3 target;

public:
    TraversalRegion(const RecordCursor &rec_data, uint32 idx);
    virtual uint32 Destroy(uint32 reason);
    virtual uint32 Update();
};

#endif
