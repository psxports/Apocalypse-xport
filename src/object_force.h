#ifndef OBJECT_FORCE_H
#define OBJECT_FORCE_H

#include "force.h"
#include "object_reference.h"
#include "object.h"
#include "vector3.h"

struct ModelInstance;
class VisualObject;

class ObjectForce : public Force
{
private:
    friend class ScriptedEffectController;

    ObjectForce *next;
    uint8 mode;
    uint8 range_shift;
    sint16 strength;
    uint32 radius_sq;
    ObjectReference<VisualObject> pos_owner;
    ShortVector3 dir;
    ModelInstance **lists[2];
    ObjectReference<VisualObject> owner;

public:
    ObjectForce(uint8 mode, VisualObject *owner, ModelInstance **list0, ModelInstance **list1);
    virtual uint32 Destroy(uint32 reason);

private:
    void SetPos(VisualObject &src);
    void SetStrength(sint16 strength, uint8 range_shift);
    void SetDir(const ShortVector3 &angles);
    void SetRadius(uint16 radius);
    void ApplyDirectional(ModelInstance *const *head);
    void ApplyDirectionalRadial(ModelInstance *const *head);
    void ApplyOmnidirectional(ModelInstance *const *head);
    void Update();

public:
    static void UpdateAll();
    virtual ~ObjectForce();

private:
    void Unlink();
    void ApplyRadial(ModelInstance *const *head, bool directional);
};

#endif
