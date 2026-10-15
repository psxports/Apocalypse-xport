#ifndef BEAM_TRIGGER_H
#define BEAM_TRIGGER_H

#include "visual_object.h"

class BeamTrigger : public VisualObject
{
public:
    BeamTrigger(const uint8 *unused_data, uint32 index);
    virtual ~BeamTrigger() {}
    static BeamTrigger &Attach(uint32 object);
    virtual uint32 Destroy(uint32 reason);
    virtual uint32 Update();
    uint32 InitializeEffect();

};

#endif
