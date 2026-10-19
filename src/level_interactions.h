#ifndef LEVEL_INTERACTIONS_H
#define LEVEL_INTERACTIONS_H

#include "vector3.h"

#include "visual_object.h"

// Player interaction objects use level-provided model and record data
class OneShotActivator : public VisualObject
{
public:
    OneShotActivator(const RecordCursor &record_data, uint32 index);

    virtual ~OneShotActivator()
    {
    }

    virtual uint32 Destroy(uint32 reason);
    virtual uint32 Update();
    uint32 Activate();
    uint32 SpawnObjects();
    Vector3 activation_pos;
    uint32 activated;
};

class TraversalRegion : public VisualObject
{
public:
    TraversalRegion(const RecordCursor &record_data, uint32 index);

    virtual ~TraversalRegion()
    {
    }

    virtual uint32 Destroy(uint32 reason);
    virtual uint32 Update();
    Vector3 bounds[2];
    uint8 reverse;
    Vector3 target;
};

class ResettingActivator : public VisualObject
{
public:
    ResettingActivator(const RecordCursor &record_data, uint32 index);

    virtual ~ResettingActivator()
    {
    }

    virtual uint32 Destroy(uint32 reason);
    virtual uint32 Update();
    uint32 BeginActivation();
    uint32 Activate();
    Vector3 rest_pos;
    Vector3 activation_pos;
    uint32 activated;
    uint32 timer;
    uint32 active_model;
    uint32 idle_model;
};

#endif
