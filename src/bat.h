#ifndef BAT_H
#define BAT_H

#include "vector3.h"
#include "enemy.h"

class Bat : public Enemy
{
private:
    uint8 swarm_slot;
    uint8 swarm_size;
    uint32 swarm_sound;

public:
    Bat(const RecordCursor &rec_data, uint16 idx);

private:
    Bat(const Vector3 *pos, uint8 slot);
    uint32 Initialize();

public:
    virtual uint32 Destroy(uint32 reason);
    virtual uint32 Damage(uint32 damage, const Vector3 *input_vector, uint32 kind);
    virtual uint32 Update();
    virtual uint32 ExecuteCommand(uint16 command);

private:
    uint32 SetTarget(const Vector3 *target);
    void UpdateState();

public:
    virtual ~Bat();
};

#endif
