#ifndef PARTICLE_DEBRIS_H
#define PARTICLE_DEBRIS_H

#include "vector3.h"

#include "visual_object.h"

class Debris : public VisualObject
{
public:
    Debris(const uint32 *position, const uint32 *velocity, uint32 kind, uint32 floor_height, uint32 flags, uint32 lifetime, const char *name);

    virtual ~Debris()
    {
    }

    virtual uint32 Destroy(uint32 reason);
    virtual uint32 Update();

    uint8 bounce_count;
    uint8 damage;
    uint16 lifetime;
    uint16 flags;
    uint32 floor_y;
    Vector3 target_pos;

private:
    // Native helper extracted from the original scale operations
    void Shrink();
};

#endif
