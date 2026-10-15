#ifndef BAT_H
#define BAT_H
#include "enemy.h"

// Parent and child swarm objects share the existing guest storage contract
class Bat : public Enemy
{
public:
    Bat(const uint8 *record_data, uint32 index);
    Bat(const uint32 *position, uint32 slot);
    static Bat &Attach(uint32 object);
    virtual uint32 Destroy(uint32 reason);
    virtual uint32 Damage(uint32 damage, const sint32 *vector, uint32 kind);
    virtual uint32 Update();
    virtual uint32 ExecuteCommand(uint32 command);
    uint32 SetTarget(const uint32 *target);
    void UpdateState();

private:
    uint32 Initialize();
    // Native extraction of the original adjusted slot16 call
    void FinishFromTable();
};

#endif
