#ifndef LEVEL_INTERACTIONS_H
#define LEVEL_INTERACTIONS_H

#include "visual_object.h"

// Player interaction objects use level-provided model and record data
class OneShotActivator : public VisualObject
{
public:
    OneShotActivator(const uint8 *record_data, uint32 index);
    virtual ~OneShotActivator() {}
    static OneShotActivator &Attach(uint32 object);
    virtual uint32 Destroy(uint32 reason);
    virtual uint32 Update();
    uint32 Activate();
    uint32 SpawnObjects();

};

class TraversalRegion : public VisualObject
{
public:
    TraversalRegion(const uint8 *record_data, uint32 index);
    virtual ~TraversalRegion() {}
    static TraversalRegion &Attach(uint32 object);
    virtual uint32 Destroy(uint32 reason);
    virtual void Update();
};

class ResettingActivator : public VisualObject
{
public:
    ResettingActivator(const uint8 *record_data, uint32 index);
    virtual ~ResettingActivator() {}
    static ResettingActivator &Attach(uint32 object);
    virtual uint32 Destroy(uint32 reason);
    virtual uint32 Update();
    uint32 BeginActivation();
    uint32 Activate();
};

#endif
