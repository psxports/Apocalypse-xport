#ifndef GAME_TUNING_H
#define GAME_TUNING_H

extern "C"
{
#include "psx.h"
}

struct PlayerTuning
{
    sint16 gravity;
    uint8 ground_offset;
    sint16 jump_first_frame;
    uint16 col_probe_size;
    sint16 ground_probe_offset;
    sint16 ground_probe_len;
    uint16 move_speed;
    sint16 jump_last_frame;
    sint16 airborne_last_frame;
    uint8 jump_hold_ticks;
    sint16 jump_impulse;
    uint16 maximum_health;
    sint16 footstep_frames[2];
    uint8 vertical_drag;
    uint8 airborne_drag;
};

struct GameTuning
{
    PlayerTuning player;
    sint16 object_explosion_strength;
    sint16 object_explosion_radius;
    sint16 initial_lives;
    uint16 pickup_floor_offset;
    sint16 tracking_projectile_ticks;
    sint16 health_pickup_amount[3];
    // Published to the session after a weapon action
    sint16 weapon_activity;
    uint16 shadow_height;
    uint16 shadow_radius;
    sint16 launched_projectile_seconds;
    sint16 grenade_radius;
    uint16 death_particle_color[3];
};

const GameTuning &GetGameTuning();
void ResetGameTuning();
uint32 ReloadGameCfg();

#endif
