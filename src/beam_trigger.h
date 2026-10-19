#ifndef BEAM_TRIGGER_H
#define BEAM_TRIGGER_H

#include "vector3.h"

#include "visual_object.h"

class PointSegments;

class BeamTrigger : public VisualObject
{
public:
    BeamTrigger(const uint8 *unused_data, uint32 index);
    virtual ~BeamTrigger();
    virtual uint32 Destroy(uint32 reason);
    virtual uint32 Update();
    uint32 InitializeEffect();

private:
    uint8 rgb[3];
    uint8 triggered;
    uint16 timer;
    uint16 spawn_rec;
    Vector3 target;
    ObjectReference<PointSegments> effect;
};

#endif
