#ifndef PLAYER_H
#define PLAYER_H

#include "vector3.h"
#include "character.h"

void ResetPlayerRuntime();
uint32 TryPlayPlayerKillQuote(const uint32 *position);
void SavePlayerTransform();
class Player;
uint32 ResolvePlayerCollisionMotion(Player &player);

struct ModelPolygon;
class WeaponNode;
class ObjectForce;
class TraversalRail;

class BeamBarrier;
class OneShotActivator;
class ResettingActivator;
class TraversalRegion;

struct GamePadInput;

class Player : public Character
{
public:
    Player();
    uint32 ReadInput();
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
    uint32 UpdateWeapons(const ShortVector3 &rotation, uint32 enabled);
    uint32 ReplaceWeapon(uint32 kind);
    uint32 UpdateGroundContact();
    uint32 SetContactColor(const ModelPolygon &polygon, const ModelInstance &surface);
    uint32 *ApplyContactImpulse(const VisualObject &object);
    uint32 ApplyBeamContactImpulse(const BeamBarrier &barrier);

    Vector3 saved_pos;
    ShortVector3 saved_angles;
    Vector3 muzzle_pos;
    ModelReference ground;
    uint32 facing_target;
    uint32 facing_step;
    uint32 facing_ticks;
    GamePadInput *pad;
    uint32 death_camera_enabled;
    uint32 disable_timer;
    uint32 weapon_stop_ticks;
    uint32 movement_state;
    uint8 update_disabled;
    uint8 hit_reaction_ticks;
    uint8 damage_glow_ticks;
    uint8 move_input[2];
    uint8 analog_aim;
    uint8 aim_input[2];
    uint8 aim_press_ticks;
    uint16 move_heading;
    uint16 aim_heading;
    uint32 fall_start_y;
    uint32 fall_damage_threshold;
    uint32 fatal_fall_distance;
    uint32 rail_voice;
    Vector3 rail_pos;
    uint32 on_rail;
    ObjectReference<TraversalRail> rail;
    ObjectReference<OneShotActivator> one_shot_activator;
    ObjectReference<ResettingActivator> resetting_activator;
    ObjectReference<TraversalRegion> traversal_region;
    ObjectReference<ObjectForce> active_force;
    uint32 smartbombs;
    uint8 fire_ticks;
    Vector3 ledge_pos;
    Vector3 ledge_normal;
    uint32 jump_velocity;
    uint8 jump_hold_ticks;
    uint8 jump_latch;
    uint16 airborne_ticks;
    uint8 weapons_enabled;
    uint16 slowdown_ticks;
    uint16 slowdown_duration;
    uint8 ground_grace_ticks;
    uint16 idle_ticks;
    uint8 ground_offset;
    uint16 collision_probe_size;
    Vector3 saved_surface_pos;
    uint16 action_heading;
    uint32 move_ramp;
    WeaponNode *weapons[8];
    uint32 sel_weapon;
    uint32 prev_weapon;
    uint16 death_ticks;

private:
    void StopSelectedWeapon(uint32 selected);
    static uint32 Abs32(sint32 value);
};

#endif
