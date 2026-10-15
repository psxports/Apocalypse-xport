#ifndef DEATH_H
#define DEATH_H

#include "enemy.h"

class Death : public Enemy
{
public:
    Death(const uint8 *record_data, uint32 index);
    static Death &Attach(uint32 object);
    virtual uint32 Destroy(uint32 reason);
    virtual uint32 Update();
    void UpdateState();
    virtual uint32 Damage(uint32 damage, const sint32 *vector, uint32 kind);
    virtual uint32 ExecuteCommand(uint32 command);
    virtual uint32 ReadParameters(uint32 command);
    virtual void SetParameters(uint32 command);
    uint32 Configure();
    uint32 SetDestination(const uint32 *position);
    uint32 TrackTarget(uint32 target);
    uint32 ClearFacing();

private:
    // Native helper retains original inline facing interpolation
    void InterpolateFacing(const uint16 desired[3]);
};

#endif
