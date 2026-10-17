#ifndef SCRIPTED_LEVEL_OBJECT_H
#define SCRIPTED_LEVEL_OBJECT_H

#include "enemy.h"

// Script-controlled level model with motion, sound and damage commands
class ScriptedLevelObject : public Enemy
{
public:
    ScriptedLevelObject(const uint8 *data, uint32 index);
    virtual ~ScriptedLevelObject() {}
    static ScriptedLevelObject &Attach(uint32 object);
    virtual uint32 Destroy(uint32 reason);
    virtual uint32 Update();
    uint32 IntegrateMotion();
    void ApplyPlayerDamage();
    uint32 ActivateContact();
    virtual uint32 Damage(uint32 damage, const sint32 *vector, uint32 kind);
    uint32 SetDestination(const uint32 *position);
    virtual uint32 ExecuteCommand(uint32 command);
    virtual void SetParameters(uint32 command);
    uint32 SetScriptParameters(uint32 command);
    virtual uint32 ResolveParameter(uint32 command);
};

#endif
