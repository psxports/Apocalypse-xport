#ifndef TANK_H
#define TANK_H
#include "enemy.h"

// Original callers include Chopper; implicit receiver ownership remains unresolved
uint32 SpawnTank(uint32 object, uint32 index);

// A view of the tank resource owner's existing guest storage
class Tank : public Enemy
{
public:
    Tank(const uint8 *record_data, uint32 index);
    static Tank &Attach(uint32 object);
    static uint32 Spawn(uint32 object, uint32 position, uint32 script);
    uint32 Init();
    virtual uint32 Destroy(uint32 reason);
    virtual uint32 Update();
    uint32 FireProjectile();
    void FireBeam(uint32 fire);
    uint32 FireDebris();
    virtual uint32 Damage(uint32 damage, const sint32 *vector, uint32 kind);
    void UpdateState();
    uint32 SetMode(uint32 mode);
    virtual uint32 ExecuteCommand(uint32 command);
    uint32 SetDestination(const uint32 *position);
    uint32 Aim(uint32 enabled);
    uint32 SetTargetRecord(uint32 index);
    uint32 SetTargetObject(uint32 target);
    uint32 ClearTarget();
    uint32 FragmentAndFinish();

private:
    Tank(const uint32 *position, uint32 script);
    void ClearVelocity();
    void ClearAngularVelocity();
    // Native extraction of the original adjusted slot16 call
    void FinishFromTable();
    void SetState(uint32 state);
    static void InitParticle(uint32 object, const uint32 acceleration[3]);
    static uint32 TraceDivide(uint32 numerator, uint32 denominator);
};

#endif
