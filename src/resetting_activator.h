#ifndef RESETTING_ACTIVATOR_H
#define RESETTING_ACTIVATOR_H

#include "vector3.h"
#include "visual_object.h"

class ResettingActivator : public VisualObject
{
public:
    virtual ~ResettingActivator()
    {
    }

    Vector3 activation_pos;

private:
    Vector3 rest_pos;
    bool activated;
    uint8 timer;
    uint32 active_model_key;
    uint32 idle_model_key;

public:
    ResettingActivator(const RecordCursor &rec_data, uint16 idx);
    virtual uint32 Destroy(uint32 reason);
    virtual uint32 Update();
    uint32 BeginActivation();
    uint32 Activate();
};

#endif
