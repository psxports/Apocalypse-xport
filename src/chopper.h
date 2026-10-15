#ifndef CHOPPER_H
#define CHOPPER_H
#include "enemy.h"

// A view of the Chopper resource owner's existing guest storage
class Chopper : public Enemy
{
public:
    static Chopper &Attach(uint32 object);
    Chopper(const uint8 *record_data, uint32 index);
    uint32 Init();
    virtual uint32 Destroy(uint32 reason);
    virtual void SetParameters(uint32 command);
    virtual uint32 Update();
    virtual uint32 Damage(uint32 damage, const sint32 *vector, uint32 kind);
    void UpdateState();
    virtual uint32 ExecuteCommand(uint32 command);
    uint32 SetDestination(const uint32 *position);
    uint32 SetDestinationRecord(uint32 index);
    uint32 Hover();
    uint32 Aim();
    uint32 SetTargetRecord(uint32 index);
    uint32 SetRelativeTarget(uint32 x, uint32 y, uint32 z);
    uint32 SetTargetObject(uint32 target);
    uint32 ClearTarget();
    uint32 FireProjectile();
    uint32 SetFireDuration(uint32 seconds);
    uint32 SpawnDebris();
    uint32 SpawnEnemy(uint32 index);

private:
    uint32 ResolveCommandValue(uint32 value);
    // Native extraction of the original adjusted slot16 call
    void FinishFromTable();
    void SpawnRandomEffect(uint32 kind, uint32 debris);
    void SetDestinationRelative(uint32 field);
    void TrackHero();
    static uint32 SquaredDistance(const uint32 vector[3]);
    static void WeaponAction(uint32 target, uint32 receiver, uint32 position, uint32 angles);
    void UpdateRoll(uint32 difference);
    static uint32 TraceDivide(uint32 numerator, uint32 denominator);
};

#endif
