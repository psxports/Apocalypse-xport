#ifndef PLAGUE_H
#define PLAGUE_H

#include "vector3.h"
#include "enemy.h"

class ParticleBeam;
class RingEffect;

class Plague : public Enemy
{
public:
    struct SpawnPoints
    {
        uint16 orbit_record;
        uint16 orbit_center;
        uint16 far_points[8];
        uint16 near_points[2];
        uint16 ground_effect[2];
        uint16 early_support[2];
        uint16 early_pickups[4];
        uint16 late_pickups[4];
        uint16 late_health[4];

        SpawnPoints() : orbit_record(0), orbit_center(0)
        {
            uint32 index;
            for (index = 0; index < 2; ++index)
            {
                near_points[index] = 0;
                ground_effect[index] = 0;
                early_support[index] = 0;
            }
            for (index = 0; index < 8; ++index)
                far_points[index] = 0;
            for (index = 0; index < 4; ++index)
            {
                early_pickups[index] = 0;
                late_pickups[index] = 0;
                late_health[index] = 0;
            }
        }
    };

    Plague(const RecordCursor &record_data, uint32 index);
    virtual ~Plague();
    virtual uint32 Destroy(uint32 reason);
    void Configure();
    void UpdateState();
    uint32 SetJumpTarget(const uint32 *position);
    uint32 SetMoveTarget(const uint32 *position);
    uint32 TrackTarget(ModelInstance *target);
    uint16 *ClearFacing();
    uint32 SpawnPickup();
    uint32 UpdateOrbitingWeapons();
    uint32 ReadParameters(uint32 command);
    virtual void SetParameters(uint32 command);
    virtual uint32 ExecuteCommand(uint32 command);
    virtual uint32 Update();
    virtual uint32 Damage(uint32 damage, const sint32 *vector, uint32 kind);

private:
    void StopMotion();
    void ClearVelocity();
    void SaveRecovery(uint32 state);
    uint32 last_pickup_tick;
    uint8 jump_index;
    uint8 attack_count;
    uint16 jump_timer;
    uint16 pickup_stage;

public:
    // Both original script and native buffered decoders own these values
    SpawnPoints spawn_points;

private:
    uint32 orbit_sound;
    ModelReference orbit_object;
    ModelReference tracked_target;
    Vector3 facing_pos;
    ObjectReference<ParticleBeam> orbit_weapons[3];
    ShortVector3 orbit_angles;
    uint16 orbit_phase;
    uint16 orbit_timer;
    ObjectReference<RingEffect> attack_ring;
    void SelectFarTarget(Vector3 &position);
    void CreateOrbitWeapon(uint32 index);
    uint32 ReadOrbitDelta() const;
};

#endif
