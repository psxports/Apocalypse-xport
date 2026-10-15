#ifndef PARTICLE_DEBRIS_H
#define PARTICLE_DEBRIS_H

#include "visual_object.h"

class Debris : public VisualObject
{
public:
    Debris(const uint32 *position, const uint32 *velocity, uint32 kind, uint32 floor_height, uint32 flags, uint32 lifetime, const void *model_data);
    virtual ~Debris() {}
    static Debris &Attach(uint32 object);
    virtual uint32 Destroy(uint32 reason);
    virtual uint32 Update();

private:
    // Native helpers extracted from original inline update logic
    uint32 FinishFromTable();
    void Shrink();
};

#endif
