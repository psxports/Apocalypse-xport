#ifndef SCRIPTED_LEVEL_OBJECT_H
#define SCRIPTED_LEVEL_OBJECT_H

#include "enemy.h"

// Script-controlled level model with motion, sound and damage commands
class ScriptedLevelObject : public Enemy
{
public:
    ScriptedLevelObject(const RecordCursor &data, uint32 index);
    virtual ~ScriptedLevelObject();
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

private:
    uint16 scale_ticks[3];
    ShortVector3 scale_step;
    sint16 contact_damage;
    uint32 contacted;
    uint32 contact_age;
    uint32 sound_voices;
    uint32 sound_period;
    Vector3 push_extent;
};

#endif
