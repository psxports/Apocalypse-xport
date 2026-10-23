#ifndef SWAT_H
#define SWAT_H

#include "object_reference.h"
#include "enemy.h"
#include "vector3.h"
#include "weapon_node.h"

struct SwatPopulation
{
    uint32 count;
    uint32 zombies;
};

extern SwatPopulation swat_population;

class OscillatingRing;
class WeaponNode;

// The default type200 model resource is swat; variants share its original table
class Swat : public Enemy
{
private:
    friend class Turret;

    struct BehaviorRange
    {
        uint32 minimum_distance;
        sint32 start_weight;
        uint32 maximum_distance;
        sint32 end_weight;

        BehaviorRange() : minimum_distance(0), start_weight(0), maximum_distance(0), end_weight(0)
        {
        }
    };

    Vector3 aim_pos;
    ModelReference aim_subject;
    BehaviorRange behavior_ranges[6];
    ObjectReference<OscillatingRing> burn_ring;
    uint8 jump_ticks;
    uint8 attack_frame;
    uint8 fragment_first;
    uint8 fragment_count;
    ModelReference mounted_object;
    ObjectReference<WeaponNode> weapon;

public:
    Swat(const RecordCursor &rec_data, uint16 idx);
    Swat(const Vector3 *pos_vector, const RecordCursor &script);

private:
    void Init();

public:
    virtual uint32 Destroy(uint32 reason);

private:
    void ApplyExplosionDamage();

public:
    virtual uint32 Update();
    virtual uint32 Damage(uint32 damage, const Vector3 *input_vector, uint32 reason);

private:
    void SpawnZombieFragment();
    void DeathReaction(const Vector3 *vector, uint32 reason);
    void FragmentAndFinish(uint32 mode);
    void EnterMovingParticleDeath();
    void EnterAttachmentParticleDeath();
    void UpdateState();
    void SetBehaviorRange(uint32 idx, uint32 minimum_distance, uint32 maximum_distance, sint32 start_weight, sint32 end_weight);

public:
    void SetMode(uint16 mode);
    virtual uint32 ExecuteCommand(uint16 command);

private:
    void DetachTurret();
    void AttachTurret(ModelInstance *linked);
    void BeginAttack();
    void Fire();
    void FireProjectile();
    void SetMoveTarget(const Vector3 *pos);
    void SetJumpTarget(const Vector3 *pos);
    void SetSpecialTarget(const Vector3 *pos);
    void SetTargetRec(uint16 idx);
    void ClearTarget();
    uint8 ChooseBehavior();

public:
    static void ResetPopulation();

private:
    void SetDeathState(uint16 state);
    uint16 ResolveCommandVal(uint16 val);
};

#endif
