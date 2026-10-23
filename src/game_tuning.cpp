#include <stdlib.h>

#include "game_tuning.h"
#include "game_diagnostics.h"
#include "game_settings.h"
#include "game_pad_input.h"
#include "resource_io.h"

struct ParticlePreferencesFile
{
    uint8 reserved[32];
    uint16 color[3];
};

typedef char ParticlePreferencesFileSize[sizeof(ParticlePreferencesFile) == 38 ? 1 : -1];

struct GameTuningFile
{
    uint8 reserved0[60];
    sint16 gravity;
    uint8 reserved1[2];
    uint8 ground_offset;
    uint8 reserved2[1];
    sint16 jump_first_frame;
    uint16 col_probe_size;
    sint16 ground_probe_offset;
    sint16 ground_probe_len;
    uint8 reserved3[2];
    uint16 move_speed;
    uint8 reserved4[18];
    sint16 jump_last_frame;
    sint16 airborne_last_frame;
    uint8 jump_hold_ticks;
    uint8 reserved5[1];
    sint16 jump_impulse;
    uint8 reserved6[62];
    sint16 object_explosion_strength;
    sint16 object_explosion_radius;
    uint8 reserved7[4];
    sint16 initial_lives;
    uint8 reserved8[2];
    uint16 maximum_health;
    uint8 reserved9[8];
    uint16 pickup_floor_offset;
    uint8 reserved10[70];
    sint16 tracking_projectile_ticks;
    uint8 reserved11[106];
    sint16 footstep_frames[2];
    uint8 reserved12[110];
    sint16 health_pickup_amount[3];
    uint8 reserved13[10];
    sint16 weapon_activity;
    uint8 reserved14[24];
    uint16 shadow_height;
    uint16 shadow_radius;
    uint8 reserved15[84];
    uint8 vertical_drag;
    uint8 reserved16[1];
    uint8 airborne_drag;
    uint8 reserved17[291];
    sint16 launched_projectile_seconds;
    uint8 reserved18[4];
    sint16 grenade_radius;
};

typedef char GameTuningFileSize[sizeof(GameTuningFile) == 914 ? 1 : -1];

static GameTuning tuning;

const GameTuning &GetGameTuning()
{
    return tuning;
}

void ResetGameTuning()
{
    tuning = GameTuning();
}

static void DecodeGameTuning(const uint8 *data, uint32 bytes)
{
    if (!data || bytes < 0x392)
    {
        GameDiagnostic(("Truncated game tuning\n"));
        abort();
    }
    const GameTuningFile &file = *(const GameTuningFile *)data;
    tuning.player.gravity = file.gravity;
    tuning.player.ground_offset = file.ground_offset;
    tuning.player.jump_first_frame = file.jump_first_frame;
    tuning.player.col_probe_size = file.col_probe_size;
    tuning.player.ground_probe_offset = file.ground_probe_offset;
    tuning.player.ground_probe_len = file.ground_probe_len;
    tuning.player.move_speed = file.move_speed;
    tuning.player.jump_last_frame = file.jump_last_frame;
    tuning.player.airborne_last_frame = file.airborne_last_frame;
    tuning.player.jump_hold_ticks = file.jump_hold_ticks;
    tuning.player.jump_impulse = file.jump_impulse;
    tuning.object_explosion_strength = file.object_explosion_strength;
    tuning.object_explosion_radius = file.object_explosion_radius;
    tuning.initial_lives = file.initial_lives;
    tuning.player.maximum_health = file.maximum_health;
    tuning.pickup_floor_offset = file.pickup_floor_offset;
    tuning.tracking_projectile_ticks = file.tracking_projectile_ticks;
    tuning.player.footstep_frames[0] = file.footstep_frames[0];
    tuning.player.footstep_frames[1] = file.footstep_frames[1];
    for (uint32 idx = 0; idx < 3; ++idx)
        tuning.health_pickup_amount[idx] = file.health_pickup_amount[idx];
    tuning.weapon_activity = file.weapon_activity;
    tuning.shadow_height = file.shadow_height;
    tuning.shadow_radius = file.shadow_radius;
    tuning.player.vertical_drag = file.vertical_drag;
    tuning.player.airborne_drag = file.airborne_drag;
    tuning.launched_projectile_seconds = file.launched_projectile_seconds;
    tuning.grenade_radius = file.grenade_radius;
}

static void DecodeParticlePreferences(const uint8 *data, uint32 bytes)
{
    if (!data || bytes < 0x26)
    {
        GameDiagnostic(("Truncated particle preferences\n"));
        abort();
    }
    const ParticlePreferencesFile &file = *(const ParticlePreferencesFile *)data;
    for (uint32 idx = 0; idx < 3; ++idx)
        tuning.death_particle_color[idx] = file.color[idx];
}

static uint32 LoadGameCfg()
{
    FUNCTION_MARKER(0x80071054, "SLUS_003.73");
    uint32 capacity = LookupResource("tweak.dat");
    uint32 bytes = GetResourceFileSize("tweak.dat");
    if (bytes < 0x392 || bytes > capacity)
    {
        GameDiagnostic(("Invalid game tuning resource size\n"));
        abort();
    }
    uint8 *data = (uint8 *)malloc(capacity);
    if (!data)
        abort();
    BeginResourceRead(data);
    WaitForResourceRead();
    DecodeControlBindings(data, bytes);
    DecodeGameTuning(data, bytes);
    free(data);
    game_pads[0].SetButtonMasks(game_settings.bindings.digital[0], game_settings.bindings.digital[1], game_settings.bindings.digital[2], game_settings.bindings.digital[3]);
    game_pads[0].SetAxes(3, 2, 1, 0, game_settings.bindings.analog[0], game_settings.bindings.analog[1], game_settings.bindings.analog[2], game_settings.bindings.analog[3]);
    capacity = LookupResource("prefs.dat");
    bytes = GetResourceFileSize("prefs.dat");
    if (bytes < 0x26 || bytes > capacity)
    {
        GameDiagnostic(("Invalid game preferences resource size\n"));
        abort();
    }
    data = (uint8 *)malloc(capacity);
    if (!data)
        abort();
    BeginResourceRead(data);
    uint32 result = WaitForResourceRead();
    DecodeGamePreferences(data, bytes);
    DecodeParticlePreferences(data, bytes);
    free(data);
    return result;
}

uint32 ReloadGameCfg()
{
    FUNCTION_MARKER(0x8007113C, "SLUS_003.73");
    return LoadGameCfg();
}
