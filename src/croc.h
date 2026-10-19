#ifndef CROC_H
#define CROC_H
#include "enemy.h"

class Croc : public Enemy
{
public:
    struct PhaseConfig
    {
        uint16 speed[4];
        uint16 health_threshold[3];
        uint16 opening_spawn[4];
        uint16 reinforcement_count;
        uint16 reinforcement_spawn[13];

        PhaseConfig() : reinforcement_count(0)
        {
            uint32 index;
            for (index = 0; index < 4; ++index)
            {
                speed[index] = 0;
                opening_spawn[index] = 0;
            }
            for (index = 0; index < 3; ++index)
                health_threshold[index] = 0;
            for (index = 0; index < 13; ++index)
                reinforcement_spawn[index] = 0;
        }
    } phase_config;

    Croc(const RecordCursor &record_data, uint32 index);
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
