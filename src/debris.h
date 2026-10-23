#ifndef DEBRIS_H
#define DEBRIS_H

#include "vector3.h"
#include "visual_object.h"

class Debris : public VisualObject
{
public:
    virtual ~Debris()
    {
    }

    uint8 bounce_count;
    uint8 damage;
    uint16 lifetime;
    uint16 flags;
    sint32 floor_y;
    Vector3 target_pos;

private:
    // Native helper extracted from the original scale operations

public:
    Debris(const Vector3 *pos, const Vector3 *velocity, uint16 model_idx, sint32 floor_height, uint16 flags, uint16 lifetime, const char *name);
    virtual uint32 Destroy(uint32 reason);
    virtual uint32 Update();

private:
    void Shrink();
};

#endif
