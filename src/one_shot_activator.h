#ifndef ONE_SHOT_ACTIVATOR_H
#define ONE_SHOT_ACTIVATOR_H

#include "vector3.h"
#include "visual_object.h"

// Player interaction objects use level-provided model and record data
class OneShotActivator : public VisualObject
{
public:
    virtual ~OneShotActivator()
    {
    }

    Vector3 activation_pos;

private:
    bool activated;

public:
    OneShotActivator(const RecordCursor &rec_data, uint16 idx);
    virtual uint32 Destroy(uint32 reason);
    virtual uint32 Update();
    uint32 Activate();
    uint32 SpawnObjects();
};

#endif
