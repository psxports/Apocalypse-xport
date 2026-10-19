#ifndef DEATH_H
#define DEATH_H

#include "vector3.h"

#include "enemy.h"

class EndpointBeam;
class Debris;

class Death : public Enemy
{
public:
    // Decoded script records retain the original field order
    struct SpawnPoints
    {
        uint16 destinations[4];
        uint16 spawn[21];
        uint16 attack[3];
        uint16 special_spawn[10];
        uint16 beam_target;
        uint16 activation_record;

        SpawnPoints() : beam_target(0), activation_record(0)
        {
            uint32 index;
            for (index = 0; index < 4; ++index)
                destinations[index] = 0;
            for (index = 0; index < 21; ++index)
                spawn[index] = 0;
            for (index = 0; index < 3; ++index)
                attack[index] = 0;
            for (index = 0; index < 10; ++index)
                special_spawn[index] = 0;
        }
    } spawn_points;

    Death(const RecordCursor &record_data, uint32 index);
    virtual ~Death();
    virtual uint32 Destroy(uint32 reason);
    virtual uint32 Update();
    void UpdateState();
    virtual uint32 Damage(uint32 damage, const sint32 *vector, uint32 kind);
    virtual uint32 ExecuteCommand(uint32 command);
    virtual uint32 ReadParameters(uint32 command);
    virtual void SetParameters(uint32 command);
    uint32 Configure();
    uint32 SetDestination(const uint32 *position);
    uint32 TrackTarget(ModelInstance *target);
    uint16 *ClearFacing();

private:
    void AdvancePhase();
    void ClearVelocity();
    void HoldTarget(uint32 mask);
    void RestoreState();
    uint16 pickup_timer;
    uint32 spawn_sound;
    uint16 pulse_angle;
    ModelReference tracked_target;
    Vector3 aim_pos;
    ObjectReference<EndpointBeam> beams[10];
    ObjectReference<Debris> spawn_debris[10];
    // Native extractions of phases in the original state machine
    void ReadSpawnPoint(uint32 index, Vector3 &point);
    void SelectDestination();
    void UpdateAttack(uint32 phase);
    void UpdateSpawn(uint32 phase);
    void UpdateSummon(uint32 phase);
    void UpdateEnding(uint32 phase);
};

#endif
