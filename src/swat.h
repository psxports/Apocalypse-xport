#ifndef SWAT_H
#define SWAT_H
#include "enemy.h"

// The default type200 model resource is swat; variants share its original table
class Swat : public Enemy
{
public:
    Swat(const uint8 *record_data, uint32 index);
    Swat(const uint32 *position, uint32 script);
    static Swat &Attach(uint32 object);
    uint32 Init();
    uint32 SetMoveTarget(const uint32 *position);
    uint32 SetJumpTarget(const uint32 *position);
    uint32 SetSpecialTarget(const uint32 *position);
    uint32 SetTargetRecord(uint32 index);
    uint32 ClearTarget();
    uint32 ChooseBehavior();
    uint32 SpawnZombieFragment();
    void FragmentAndFinish(uint32 mode);
    uint32 EnterDeathState13();
    uint32 EnterDeathState14();
    uint32 BeginAttack();
    uint32 Fire();
    uint32 FireProjectile();
    uint32 SetMode(uint32 mode);
    uint32 SetBehaviorRange(uint32 index, uint32 first, uint32 second, uint32 third, uint32 fourth);
    virtual uint32 Destroy(uint32 reason);
    virtual uint32 Update();
    void UpdateState();
    uint32 ApplyExplosionDamage();
    virtual uint32 Damage(uint32 damage, const sint32 *vector, uint32 reason);
    virtual uint32 ExecuteCommand(uint32 command);
    void DeathReaction(const sint32 *vector, uint32 reason);
    uint32 DetachTurret();
    uint32 AttachTurret(uint32 linked);

private:
    static uint32 ReadVector(const sint32 *vector, uint32 index);
    void SetDeathState(uint32 state);
    static sint32 Divide(sint32 numerator, uint32 divisor);
    static uint32 DivideUnsigned(uint32 numerator, uint32 denominator);
    static sint32 Abs32(sint32 value);
    static void SetEffectDirection(const sint16 angles[3]);
    static void SetEffectAngles(const sint16 angles[3]);
    static void MissingBehavior(uint32 service, uint32 receiver, uint32 arg2, uint32 arg3, uint32 arg4);
    uint32 ReadCommand();
    uint32 ResolveCommandValue(uint32 value);
    static sint32 QueryGround(const uint32 position[3], uint32 below, uint32 above);
    void ApplyCommandPosition(uint32 target, const uint32 position[3]);
};

#endif
