#ifndef HOUND_RAT_H
#define HOUND_RAT_H
#include "enemy.h"

// Hound and rat resources use the same guest object methods
class HoundRat : public Enemy
{
public:
    HoundRat(const uint8 *record_data, uint32 index, uint32 mode);
    static HoundRat &Attach(uint32 object);
    virtual uint32 Destroy(uint32 reason);
    virtual uint32 Update();
    virtual uint32 ExecuteCommand(uint32 command);
    void UpdateState();
    uint32 SetTarget(const uint32 *target);
    virtual uint32 Damage(uint32 damage, const sint32 *vector, uint32 kind);

private:
    uint32 Init();
    // Native extraction of the original adjusted slot16 call
    void FinishFromTable();
    void Orbit(uint32 point[3], uint32 angle, uint32 reload_angle);
};

#endif
