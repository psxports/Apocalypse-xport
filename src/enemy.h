#ifndef ENEMY_H
#define ENEMY_H
#include "character.h"

uint32 ScaleDamageForDifficulty(uint32 damage);
uint32 DispatchObjectScriptCommand(uint32 target, uint32 receiver, uint32 command);

// Original enemy methods retain their guest storage contract
class Enemy : public Character
{
public:
    virtual ~Enemy() {}
    static Enemy &Attach(uint32 object);
    uint32 ReadScriptValue();
    uint32 SkipConditionalBlock();
    uint32 ExecuteScript(const uint16 *script);
    uint32 ReadParameters(uint32 command);
    uint32 IntegrateMotion();
    uint32 AllocateBehaviorTable(uint32 rows, uint32 columns);
    uint32 FreeBehaviorTable();
    uint32 ActivateLinkedRecords();
    uint32 SetBehaviorValue(uint32 row, uint32 column, uint32 value);
    uint32 GetBehaviorValue(uint32 row, uint32 column);
    virtual uint32 Destroy(uint32 reason);
    virtual uint32 Finish();
    virtual uint32 Update() = 0;
    virtual void Callback639A4();
    virtual void Callback6399C();
    virtual uint32 Damage(uint32 damage, const sint32 *vector, uint32 kind) = 0;
    virtual void ResetWeapon();
    virtual uint32 ExecuteCommand(uint32 command);
    virtual void SetParameters(uint32 command);
    virtual uint32 ResolveParameter(uint32 command);

protected:
    Enemy();
};

#endif
