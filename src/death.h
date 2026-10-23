#ifndef DEATH_H
#define DEATH_H

#include "object_reference.h"
#include "vector3.h"
#include "enemy.h"

class EndpointBeam;
class Debris;

class Death : public Enemy
{
private:
    // Decoded script records retain the original field order
    struct SpawnPoints
    {
        uint16 dsts[4];
        uint16 spawn[21];
        uint16 attack[3];
        uint16 special_spawn[10];
        uint16 beam_target;
        uint16 activation_rec;

        SpawnPoints() : beam_target(0), activation_rec(0)
        {
            uint32 idx;
            for (idx = 0; idx < 4; ++idx)
                dsts[idx] = 0;
            for (idx = 0; idx < 21; ++idx)
                spawn[idx] = 0;
            for (idx = 0; idx < 3; ++idx)
                attack[idx] = 0;
            for (idx = 0; idx < 10; ++idx)
                special_spawn[idx] = 0;
        }
    } spawn_points;

private:
    uint16 pickup_timer;
    uint32 spawn_sound;
    uint16 pulse_angle;
    ModelReference tracked_target;
    Vector3 aim_pos;
    ObjectReference<EndpointBeam> beams[10];
    ObjectReference<Debris> spawn_debris[10];
    // Native extractions of phases in the original state machine

public:
    Death(const RecordCursor &rec_data, uint16 idx);

private:
    void Configure();

public:
    virtual uint32 Destroy(uint32 reason);
    virtual uint32 Update();
    virtual uint32 Damage(uint32 damage, const Vector3 *vector, uint32 kind);

private:
    void UpdateState();

public:
    virtual void SetParameters(uint16 command);
    virtual uint32 ExecuteCommand(uint16 command);

private:
    void SetDst(const Vector3 *pos);
    void TrackTarget(ModelInstance *target);
    void ClearFacing();
    static EndpointBeam *CreateBeam(uint8 r, uint8 g, uint8 b);

public:
    virtual ~Death();

private:
    void AdvancePhase();
    void ClearVelocity();
    void HoldTarget(uint32 mask);
    void RestoreState();
    void ReadSpawnPoint(uint32 idx, Vector3 &point);
    void SelectDst();
    void UpdateAttack(uint16 phase);
    void UpdateSpawn(uint16 phase);
    void UpdateSummon(uint16 phase);
    void UpdateEnding(uint16 phase);
};

#endif
