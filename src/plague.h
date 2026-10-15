#ifndef PLAGUE_H
#define PLAGUE_H
#include "enemy.h"

class Plague : public Enemy
{
public:
    Plague(const uint8 *record_data, uint32 index);
    virtual ~Plague() {}
    static Plague &Attach(uint32 object);
    virtual uint32 Destroy(uint32 reason);
    uint32 Configure();
    void UpdateState();
    uint32 SetJumpTarget(const uint32 *position);
    uint32 SetMoveTarget(const uint32 *position);
    uint32 TrackTarget(const uint8 *target);
    uint32 ClearFacing();
    uint32 SpawnPickup();
    uint32 UpdateOrbitingWeapons();
    uint32 ReadParameters(uint32 command);
    virtual void SetParameters(uint32 command);
    virtual uint32 ExecuteCommand(uint32 command);
    virtual uint32 Update();
    virtual uint32 Damage(uint32 damage, const sint32 *vector, uint32 kind);
};

#endif
