#ifndef CROC_H
#define CROC_H
#include "enemy.h"

// A view of the Croc resource owner's existing guest storage
class Croc : public Enemy
{
public:
    Croc(const uint8 *record_data, uint32 index);
    static Croc &Attach(uint32 object);
    virtual uint32 Destroy(uint32 reason);
    virtual uint32 Damage(uint32 damage, const sint32 *vector, uint32 kind);
    virtual uint32 Update();
    void UpdateState();
    virtual void SetParameters(uint32 command);
    virtual uint32 ExecuteCommand(uint32 command);
    uint32 SetDestination(const uint32 *position);

private:
    uint32 Initialize();
    void ClearAngularVelocity();
};

#endif
