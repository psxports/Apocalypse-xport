#ifndef PLAYER_H
#define PLAYER_H
#include "character.h"

uint32 TryPlayPlayerKillQuote(const uint32 *position);
uint32 SavePlayerTransform();
uint32 ResolvePlayerCollisionMotion(uint32 object);

struct ModelPolygon;
class WeaponNode;

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
    virtual uint32 UpdateLighting();
    virtual uint32 Damage(uint32 damage, const sint32 *vector, uint32 reason);
    virtual uint32 ReleaseResources();
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
    void SetFacingTransition(uint32 target_angle, uint32 heading, uint32 immediate);
    void StopWeapon();
    void SetWeaponStop(uint32 duration);
    uint32 UpdateWeapons(const sint16 *rotation, uint32 enabled);
    uint32 ReplaceWeapon(uint32 kind);
    uint32 UpdateGroundContact();
    uint32 SetContactColor(const ModelPolygon &polygon, uint32 surface);
    uint32 *ApplyContactImpulse(uint32 object);
    uint32 ApplyBeamContactImpulse(uint32 object);

    WeaponNode *weapons[8];
    uint32 sel_weapon;
    uint32 prev_weapon;
    ModelReference ground;

private:
    void StopSelectedWeapon(uint32 selected);
    static uint32 Abs32(sint32 value);
};

#endif
