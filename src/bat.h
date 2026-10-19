#ifndef BAT_H
#define BAT_H
#include "enemy.h"

class Bat : public Enemy
{
public:
    Bat(const RecordCursor &record_data, uint32 index);
    Bat(const uint32 *position, uint32 slot);
    virtual ~Bat();
    virtual uint32 Destroy(uint32 reason);
    virtual uint32 Damage(uint32 damage, const sint32 *vector, uint32 kind);
    virtual uint32 Update();
    virtual uint32 ExecuteCommand(uint32 command);
    uint32 SetTarget(const uint32 *target);
    void UpdateState();

private:
    uint8 swarm_slot;
    uint8 swarm_size;
    uint32 swarm_sound;
    uint32 Initialize();
};

#endif
