#ifndef HOUND_RAT_H
#define HOUND_RAT_H
#include "enemy.h"

// Hound and rat resources share movement and behavior methods
class HoundRat : public Enemy
{
public:
    HoundRat(const RecordCursor &record_data, uint32 index, uint32 mode);
    virtual uint32 Destroy(uint32 reason);
    virtual uint32 Update();
    virtual uint32 ExecuteCommand(uint32 command);
    void UpdateState();
    uint32 SetTarget(const uint32 *target);
    virtual uint32 Damage(uint32 damage, const sint32 *vector, uint32 kind);

private:
    uint32 Init();
    // Native extraction of the original adjusted slot16 call
    void Orbit(Vector3 &point, uint32 angle, uint32 reload_angle);
};

#endif
