#ifndef OBJECT_FORCE_H
#define OBJECT_FORCE_H

extern "C"
{
#include "psx.h"
}

uint32 ApplyObjectContactImpulse(uint32 hero, uint32 object);

class Force
{
public:
    static Force &Attach(uint32 object);
    virtual uint32 Destroy(uint32 reason);

protected:
    // Bind the native view without changing original guest state
    Force() {}
};

class ObjectForce : public Force
{
public:
    ObjectForce(uint32 mode, uint32 owner, uint32 first_list, uint32 second_list);
    static ObjectForce &Attach(uint32 object);
    virtual uint32 Destroy(uint32 reason);
    void SetPosition(uint32 position);
    void SetStrength(uint32 strength, uint32 range_shift);
    uint32 SetDirection(const uint16 *angles);
    uint32 SetRadius(uint32 radius);
    uint32 ApplyDirectional(uint32 list);
    uint32 ApplyDirectionalRadial(uint32 list);
    uint32 ApplyOmnidirectional(uint32 list);
    uint32 Update();

private:
    // Native shared implementation of the two recovered radial operations
    uint32 ApplyRadial(uint32 list, uint32 directional);
};

#endif
