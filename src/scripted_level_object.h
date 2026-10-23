#ifndef SCRIPTED_LEVEL_OBJECT_H
#define SCRIPTED_LEVEL_OBJECT_H

#include "vector3.h"
#include "enemy.h"

// Script-controlled level model with motion, sound and damage commands
class ScriptedLevelObject : public Enemy
{
private:
    uint16 scale_ticks[3];
    ShortVector3 scale_step;
    sint16 contact_damage;
    bool contacted;
    uint32 contact_age;
    uint32 sound_voices;
    sint32 sound_period;
    Vector3 push_extent;

public:
    ScriptedLevelObject(const RecordCursor &data, uint16 idx);
    virtual uint32 Destroy(uint32 reason);
    void ApplyPlayerDamage();
    virtual uint32 Update();
    virtual uint32 Damage(uint32 damage, const Vector3 *vector, uint32 kind);
    uint32 IntegrateMotion();

private:
    uint32 SetDst(const Vector3 &pos);

public:
    uint32 ActivateContact();
    virtual uint32 ExecuteCommand(uint16 command);
    virtual void SetParameters(uint16 command);
    virtual uint32 ResolveParameter(uint16 command);
    virtual ~ScriptedLevelObject();
};

#endif
