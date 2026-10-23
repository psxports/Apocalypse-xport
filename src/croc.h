#ifndef CROC_H
#define CROC_H

#include "vector3.h"
#include "enemy.h"

class Croc : public Enemy
{
private:
    struct PhaseConfig
    {
        uint16 speed[4];
        uint16 health_threshold[3];
        uint16 opening_spawn[4];
        uint16 reinforcement_count;
        uint16 reinforcement_spawn[13];

        PhaseConfig() : reinforcement_count(0)
        {
            uint32 idx;
            for (idx = 0; idx < 4; ++idx)
            {
                speed[idx] = 0;
                opening_spawn[idx] = 0;
            }
            for (idx = 0; idx < 3; ++idx)
                health_threshold[idx] = 0;
            for (idx = 0; idx < 13; ++idx)
                reinforcement_spawn[idx] = 0;
        }
    } phase_config;

public:
    Croc(const RecordCursor &rec_data, uint16 idx);

private:
    uint32 Initialize();

public:
    virtual uint32 Destroy(uint32 reason);
    virtual uint32 Update();
    virtual void SetParameters(uint16 command);
    virtual uint32 ExecuteCommand(uint16 command);

private:
    void UpdateState();
    uint32 SetDst(const Vector3 *pos);

public:
    virtual uint32 Damage(uint32 damage, const Vector3 *vector, uint32 kind);

private:
    void ClearAngularVelocity();
};

#endif
