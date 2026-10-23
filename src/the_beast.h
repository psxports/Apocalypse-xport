#ifndef THE_BEAST_H
#define THE_BEAST_H

#include "object_reference.h"
#include "vector3.h"
#include "enemy.h"
#include "angle_math.h"
#include "particle_beam.h"

class ParticleBeam;
class Flamethrower;
class RingEffect;
class GuidedProjectile;

class TheBeast : public Enemy
{
private:
    Vector3 bone_aim_pos;
    AngleMotion aim;
    ModelReference tracked_target;
    Vector3 aim_pos;
    uint16 jump_timer;
    uint8 used_points[5];
    ObjectReference<ParticleBeam> beams[2];
    ObjectReference<Flamethrower> flamethrower;
    ObjectReference<RingEffect> charge_rings[2];
    ObjectReference<GuidedProjectile> held_projectile;

public:
    TheBeast(const RecordCursor &rec_data, uint16 idx);

private:
    void Configure();

public:
    virtual uint32 Destroy(uint32 reason);

private:
    uint32 SpawnPickup();

public:
    virtual uint32 Update();

private:
    void Resize(sint32 inc);

public:
    virtual uint32 UpdateLighting();
    virtual uint32 Damage(uint32 damage, const Vector3 *vector, uint32 reason);

private:
    void UpdateBehavior();

public:
    virtual void SetParameters(uint16 command);
    virtual uint32 ExecuteCommand(uint16 command);

private:
    void SetDst(const Vector3 *pos);
    void SetJumpDst(const Vector3 *pos);
    void TrackTarget(ModelInstance *target);
    void SetFacingPoint(const Vector3 *pos);
    void ClearFacing();
    void SetBoneFacingPoint(const Vector3 *pos);
    void ClearBoneFacing();
    static void ReadLightDir(Character &object, uint32 bone, bool horizontal, ShortVector3 &dir);

public:
    virtual ~TheBeast();

private:
    void AdvancePhase();
    void SelectState(uint16 state);
    void RestoreState();
    void ClearVelocity();
    void BeginAttack();
    void PlayAnimation(uint8 animation);
    void JumpToPoint(Vector3 &pos);
    uint16 ReadBeamPoint();
    void PosBeamPoint(Vector3 &pos, ShortVector3 &angles, uint8 turn);
};

#endif
