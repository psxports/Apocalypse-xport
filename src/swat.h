#ifndef SWAT_H
#define SWAT_H
#include "enemy.h"
#include "vector3.h"

struct SwatPopulation
{
    uint32 count;
    uint32 zombies;
};

extern SwatPopulation swat_population;
void ResetSwatPopulation();

class OscillatingRing;
class WeaponNode;

// The default type200 model resource is swat; variants share its original table
class Swat : public Enemy
{
public:
    Swat(const RecordCursor &record_data, uint32 index);
    Swat(const uint32 *position, const RecordCursor &script);
    uint32 Init();
    uint32 ChooseBehavior();
    uint32 SpawnZombieFragment();
    void FragmentAndFinish(uint32 mode);
    uint32 EnterDeathState13();
    void EnterDeathState14();
    uint32 BeginAttack();
    uint32 Fire();
    uint32 FireProjectile();
    uint32 SetMode(uint32 mode);
    virtual uint32 Destroy(uint32 reason);
    virtual uint32 Update();
    void UpdateState();
    uint32 ApplyExplosionDamage();
    virtual uint32 Damage(uint32 damage, const sint32 *vector, uint32 reason);
    virtual uint32 ExecuteCommand(uint32 command);
    void DeathReaction(const sint32 *vector, uint32 reason);
    uint32 DetachTurret();
    uint32 AttachTurret(ModelInstance *linked);

private:
    friend class Turret;

    struct BehaviorRange
    {
        uint32 minimum_distance;
        uint32 start_weight;
        uint32 maximum_distance;
        uint32 end_weight;

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
    void SetBehaviorRange(uint32 index, uint32 minimum_distance, uint32 maximum_distance, uint32 start_weight, uint32 end_weight);
    uint32 SetMoveTarget(const uint32 *position);
    uint32 SetJumpTarget(const uint32 *position);
    uint32 SetSpecialTarget(const uint32 *position);
    uint32 SetTargetRecord(uint32 index);
    uint32 ClearTarget();
    void SetDeathState(uint32 state);
    static sint32 Divide(sint32 numerator, uint32 divisor);
    static uint32 DivideUnsigned(uint32 numerator, uint32 denominator);
    static sint32 Abs32(sint32 value);
    uint32 ResolveCommandValue(uint32 value);
    static sint32 QueryGround(const uint32 position[3], uint32 below, uint32 above);
};

#endif
