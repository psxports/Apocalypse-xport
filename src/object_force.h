#ifndef OBJECT_FORCE_H
#define OBJECT_FORCE_H

#include "object.h"
#include "vector3.h"

struct ModelInstance;
class VisualObject;

class Force : public Object
{
public:
    virtual uint32 Destroy(uint32 reason);

protected:
    Force()
    {
    }
};

class ObjectForce : public Force
{
public:
    ObjectForce(uint8 mode, VisualObject *owner, ModelInstance **list0, ModelInstance **list1);
    virtual ~ObjectForce();
    virtual uint32 Destroy(uint32 reason);
    void SetPosition(VisualObject &src);
    void SetStrength(sint16 strength, uint8 range_shift);
    void SetDirection(const uint16 *angles);
    void SetRadius(uint16 radius);
    void ApplyDirectional(ModelInstance *list);
    void ApplyDirectionalRadial(ModelInstance *list);
    void ApplyOmnidirectional(ModelInstance *list);
    void Update();

private:
    friend void UpdateObjectForces();
    void Unlink();
    void ApplyRadial(ModelInstance *list, bool directional);
    ObjectForce *next;
    uint8 mode;
    uint8 range_shift;
    sint16 strength;
    uint32 radius_sq;
    ObjectReference<VisualObject> pos_owner;
    ShortVector3 dir;
    ModelInstance **lists[2];
    ObjectReference<VisualObject> owner;
};

#endif
