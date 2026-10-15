#ifndef PLAYER_H
#define PLAYER_H
#include "character.h"

uint32 TryPlayPlayerKillQuote(const uint32 *position);
uint32 SavePlayerTransform();
uint32 UpdatePlayerLighting(uint32 object);
uint32 ResolvePlayerCollisionMotion(uint32 object);

class Player : public Character
{
public:
    Player();
    uint32 ReadInput();
    static Player &Attach(uint32 object);
    virtual uint32 Destroy(uint32 reason);

    virtual ~Player()
    {
    }

    virtual uint32 Finish();
    virtual uint32 Update();
    virtual uint32 Damage(uint32 damage, const sint32 *vector, uint32 reason);
    virtual uint32 ClearWeapons();
    uint32 RestoreHealth(uint32 amount);
    uint32 EnterDeathState();
    void PrepareDeathCamera();
    uint32 InitializeAnimation();
    uint32 TryEnterAnimation10State();
    uint32 TryEnterRecordMovementState();
    uint32 TryEnterLandingState();
    uint32 TryEnterJumpState();
    uint32 TryEnterAirborneState();
    uint32 TryEnterActionState();
    uint32 TryActivateOneShotActivator();
    uint32 TryActivateResettingActivator();
    uint32 TryEnterMovementState16();
    uint32 TryEnterMovementState32();
    uint32 TryEnterMovementState512();
    uint32 TryEnterMovementState1024();
    uint32 SetFacingTransition(uint32 target_angle, uint32 heading, uint32 immediate);
    void StopWeapon();
    void SetWeaponStop(uint32 duration);
    uint32 UpdateWeapons(const sint16 *rotation, uint32 enabled);
    uint32 ReplaceWeapon(uint32 kind);
    uint32 UpdateGroundContact();
    uint32 SetContactColor(uint32 polygon, uint32 surface);
    uint32 ApplyContactImpulse(uint32 object);
    uint32 ApplyBeamContactImpulse(uint32 object);

private:
    void StopSelectedWeapon(uint32 selected);
    static uint32 Abs32(sint32 value);
};

#endif
