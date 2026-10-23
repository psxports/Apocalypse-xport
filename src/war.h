#ifndef WAR_H
#define WAR_H

#include "object_reference.h"

class Camera;

#include "vector3.h"
#include "enemy.h"
#include "angle_math.h"
#include "particle_beam.h"

class ParticleBeam;

class War : public Enemy
{
private:
    AngleMotion aim;
    ModelReference tracked_target;
    Vector3 aim_pos;
    uint16 jump_timer;
    ObjectReference<ParticleBeam> beam;
    ObjectReference<ParticleBeam> attack_beam;
    ObjectReference<ParticleBeam> death_beams[5];
    uint8 last_voice;
    uint8 grp_cursor;
    uint8 threshold_idx;
    // Native helpers retain inline blocks from the original state machine

public:
    War(const RecordCursor &rec_data, uint16 idx);

private:
    void Configure();

public:
    virtual uint32 Destroy(uint32 reason);
    virtual uint32 Update();
    virtual uint32 Damage(uint32 damage, const Vector3 *vector, uint32 kind);

private:
    void Resize(sint32 inc);
    void UpdateState();

public:
    virtual void SetParameters(uint16 command);
    virtual uint32 ExecuteCommand(uint16 command);

private:
    void SetDst(const Vector3 *pos);
    void TrackTarget(ModelInstance *target);
    void ClearFacing();
    void SpawnProjectileWave(sint32 count);
    static void SetCamera(Camera &camera, uint16 distance, sint16 height, sint16 lateral_offset, sint16 height_offset, sint16 forward_offset, uint16 frames, uint16 yaw, bool set_yaw);

public:
    virtual ~War();

private:
    void AdvancePhase();
    void AdvanceState();
    void RestoreState();
    void ClearVelocity();
    void SelectPoint(const uint16 *table, uint32 minimum, uint32 attempts, Vector3 &point);
    void ActivateGrp(uint32 maximum);
    void FireDeathWeapons();
    void CreateDeathWeapon(uint32 idx);
};

#endif
