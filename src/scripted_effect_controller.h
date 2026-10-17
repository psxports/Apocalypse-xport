#ifndef SCRIPTED_EFFECT_CONTROLLER_H
#define SCRIPTED_EFFECT_CONTROLLER_H

#include "enemy.h"

// Script-only owner of environment effects, object forces and timed sound
class ScriptedEffectController : public Enemy
{
public:
    ScriptedEffectController(const uint8 *record_data, uint32 index);
    virtual ~ScriptedEffectController() {}
    static ScriptedEffectController &Attach(uint32 object);
    virtual uint32 Destroy(uint32 reason);
    virtual uint32 Update();
    virtual uint32 ExecuteCommand(uint32 command);
    virtual void SetParameters(uint32 command);
};

#endif
