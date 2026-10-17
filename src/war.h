#ifndef WAR_H
#define WAR_H

#include "enemy.h"

class War : public Enemy
{
public:
    War(const uint8 *record_data, uint32 index);
    static War &Attach(uint32 object);
    virtual uint32 Destroy(uint32 reason);
    virtual uint32 Update();
    void UpdateState();
    virtual uint32 Damage(uint32 damage, const sint32 *vector, uint32 kind);
    virtual uint32 ExecuteCommand(uint32 command);
    virtual uint32 ReadParameters(uint32 command);
    virtual void SetParameters(uint32 command);
    uint32 Configure();
    uint32 Resize(uint32 increment);
    uint32 SetDestination(const uint32 *position);
    uint32 TrackTarget(uint32 target);
    uint16 *ClearFacing();
    uint32 SpawnProjectileWave(uint32 count);

private:
    // Native helpers retain inline blocks from the original state machine
    void AdvancePhase();
    void AdvanceState();
    void RestoreState();
    void ClearVelocity();
    void SelectPoint(uint32 table, uint32 minimum, uint32 attempts, uint32 point[3]);
    void ActivateGroup(uint32 maximum);
    void FireDeathWeapons();
    void CreateDeathWeapon(uint32 offset);
};

#endif
