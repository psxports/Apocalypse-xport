#ifndef THE_BEAST_H
#define THE_BEAST_H

#include "enemy.h"

class TheBeast : public Enemy
{
public:
    TheBeast(const uint8 *record_data, uint32 index);
    static TheBeast &Attach(uint32 object);
    virtual uint32 Destroy(uint32 reason);
    virtual uint32 Update();
    void UpdateBehavior();
    virtual uint32 Damage(uint32 damage, const sint32 *vector, uint32 kind);
    uint32 Configure();
    uint32 SpawnPickup();
    uint32 Resize(uint32 increment);
    virtual uint32 UpdateLighting();
    virtual uint32 ExecuteCommand(uint32 command);
    virtual uint32 SetScriptParameters(uint32 command);
    uint32 SetDestination(const uint32 *position);
    uint32 SetJumpDestination(const uint32 *position);
    uint32 TrackTarget(uint32 target);
    uint32 SetFacingPoint(const uint32 *position);
    uint32 ClearFacing();
    uint32 SetBoneFacingPoint(const uint32 *position);
    uint32 ClearBoneFacing();
    // Native ABI routes slot72 through SetScriptParameters to preserve its result
};

#endif
