#ifndef PLAGUE_H
#define PLAGUE_H

#include "object_reference.h"
#include "vector3.h"
#include "enemy.h"
#include "particle_beam.h"

class ParticleBeam;
class RingEffect;

class Plague : public Enemy
{
private:
    struct SpawnPoints
    {
        uint16 orbit_rec;
        uint16 orbit_center;
        uint16 far_points[8];
        uint16 near_points[2];
        uint16 ground_effect[2];
        uint16 early_support[2];
        uint16 early_pickups[4];
        uint16 late_pickups[4];
        uint16 late_health[4];

        SpawnPoints() : orbit_rec(0), orbit_center(0)
        {
            uint32 idx;
            for (idx = 0; idx < 2; ++idx)
            {
                near_points[idx] = 0;
                ground_effect[idx] = 0;
                early_support[idx] = 0;
            }
            for (idx = 0; idx < 8; ++idx)
                far_points[idx] = 0;
            for (idx = 0; idx < 4; ++idx)
            {
                early_pickups[idx] = 0;
                late_pickups[idx] = 0;
                late_health[idx] = 0;
            }
        }
    };

private:
    uint32 last_pickup_tick;
    uint8 jump_idx;
    uint8 attack_count;
    uint16 jump_timer;
    uint16 pickup_stage;

    SpawnPoints spawn_points;

    uint32 orbit_sound;
    ModelReference orbit_object;
    ModelReference tracked_target;
    Vector3 facing_pos;
    ObjectReference<ParticleBeam> orbit_weapons[3];
    ShortVector3 orbit_angles;
    uint16 orbit_phase;
    uint16 orbit_timer;
    ObjectReference<RingEffect> attack_ring;

public:
    Plague(const RecordCursor &rec_data, uint16 idx);

private:
    void Configure();

public:
    virtual uint32 Destroy(uint32 reason);
    virtual uint32 Update();
    virtual uint32 Damage(uint32 damage, const Vector3 *vector, uint32 reason);

private:
    void UpdateState();
    void SpawnPickup();
    void UpdateOrbitingWeapons();

public:
    virtual void SetParameters(uint16 command);
    virtual uint32 ExecuteCommand(uint16 command);

private:
    void SetJumpTarget(const Vector3 *pos);
    void SetMoveTarget(const Vector3 *pos);
    void TrackTarget(ModelInstance *target);
    void ClearFacing();

public:
    virtual ~Plague();

private:
    void StopMotion();
    void ClearVelocity();
    void SaveRecovery(uint16 state);
    void SelectFarTarget(Vector3 &pos);
    void CreateOrbitWeapon(uint32 idx);
    sint32 ReadOrbitDelta() const;
};

#endif
