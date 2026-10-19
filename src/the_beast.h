#ifndef THE_BEAST_H
#define THE_BEAST_H

#include "vector3.h"

#include "enemy.h"
#include "angle_math.h"

class ParticleBeam;
class Flamethrower;
class RingEffect;
class GuidedProjectile;

class TheBeast : public Enemy
{
public:
    TheBeast(const RecordCursor &record_data, uint32 index);
    virtual ~TheBeast();
    virtual uint32 Destroy(uint32 reason);
    virtual uint32 Update();
    void UpdateBehavior();
    virtual uint32 Damage(uint32 damage, const sint32 *vector, uint32 kind);
    void Configure();
    uint32 SpawnPickup();
    uint32 Resize(uint32 increment);
    virtual uint32 UpdateLighting();
    virtual uint32 ExecuteCommand(uint32 command);
    virtual void SetParameters(uint32 command);
    uint32 SetDestination(const uint32 *position);
    uint32 SetJumpDestination(const uint32 *position);
    uint32 TrackTarget(ModelInstance *target);
    uint32 SetFacingPoint(const uint32 *position);
    uint16 *ClearFacing();
    uint32 SetBoneFacingPoint(const uint32 *position);
    uint32 ClearBoneFacing();

private:
    void AdvancePhase();
    void SelectState(uint32 state);
    void RestoreState();
    void ClearVelocity();
    void BeginAttack();
    void PlayAnimation(uint32 animation);
    void JumpToPoint(uint32 *position);
    uint32 ReadBeamPoint();
    void PositionBeamPoint(uint32 *position, uint16 *angles, uint32 turn);
    uint8 used_points[5];
    Vector3 bone_aim_pos;
    AngleMotion aim;
    ModelReference tracked_target;
    Vector3 aim_pos;
    uint16 jump_timer;
    ObjectReference<ParticleBeam> beams[2];
    ObjectReference<Flamethrower> flamethrower;
    ObjectReference<RingEffect> charge_rings[2];
    ObjectReference<GuidedProjectile> held_projectile;
};

#endif
