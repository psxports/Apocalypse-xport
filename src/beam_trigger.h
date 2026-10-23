#ifndef BEAM_TRIGGER_H
#define BEAM_TRIGGER_H

#include "object_reference.h"
#include "vector3.h"
#include "visual_object.h"

class PointSegments;

class BeamTrigger : public VisualObject
{
private:
    uint8 rgb[3];
    bool triggered;
    uint16 timer;
    uint16 spawn_rec;
    Vector3 target;
    ObjectReference<PointSegments> effect;

public:
    BeamTrigger(uint16 idx);
    virtual uint32 Destroy(uint32 reason);

private:
    uint8 InitializeEffect();

public:
    virtual uint32 Update();
    virtual ~BeamTrigger();
};

#endif
