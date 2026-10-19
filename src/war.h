#ifndef WAR_H
#define WAR_H

#include "vector3.h"

#include "enemy.h"
#include "angle_math.h"

class ParticleBeam;

class War : public Enemy
{
public:
    War(const RecordCursor &record_data, uint32 index);
    virtual ~War();
    virtual uint32 Destroy(uint32 reason);
    virtual uint32 Update();
    void UpdateState();
    virtual uint32 Damage(uint32 damage, const sint32 *vector, uint32 kind);
    virtual uint32 ExecuteCommand(uint32 command);
    virtual uint32 ReadParameters(uint32 command);
    virtual void SetParameters(uint32 command);
    uint32 Configure();
    uint32 Resize(uint32 increment);
    uint32 SetDestination(const uint32 *position);
    uint32 TrackTarget(ModelInstance *target);
    uint16 *ClearFacing();
    uint32 SpawnProjectileWave(uint32 count);

private:
    AngleMotion aim;
    ModelReference tracked_target;
    Vector3 aim_pos;
    uint16 jump_timer;
    ObjectReference<ParticleBeam> beam;
    ObjectReference<ParticleBeam> attack_beam;
    ObjectReference<ParticleBeam> death_beams[5];
    uint8 last_voice;
    uint8 group_cursor;
    uint8 threshold_index;
    // Native helpers retain inline blocks from the original state machine
    void AdvancePhase();
    void AdvanceState();
    void RestoreState();
    void ClearVelocity();
    void SelectPoint(const uint16 *table, uint32 minimum, uint32 attempts, Vector3 &point);
    void ActivateGroup(uint32 maximum);
    void FireDeathWeapons();
    void CreateDeathWeapon(uint32 index);
};

#endif
