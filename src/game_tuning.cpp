#include <stdlib.h>

#include "game_tuning.h"
#include "game_diagnostics.h"
#include "game_settings.h"
#include "game_pad_poll.h"
#include "resource_io.h"

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
        GameDiagnostic("Truncated game tuning\n");
        abort();
    }
    tuning.player.gravity = (sint16)xport_load_le16(data + 0x3C);
    tuning.player.ground_offset = data[0x40];
    tuning.player.jump_first_frame = (sint16)xport_load_le16(data + 0x42);
    tuning.player.collision_probe_size = xport_load_le16(data + 0x44);
    tuning.player.ground_probe_offset = (sint16)xport_load_le16(data + 0x46);
    tuning.player.ground_probe_length = (sint16)xport_load_le16(data + 0x48);
    tuning.player.move_speed = xport_load_le16(data + 0x4C);
    tuning.player.jump_last_frame = (sint16)xport_load_le16(data + 0x60);
    tuning.player.airborne_last_frame = (sint16)xport_load_le16(data + 0x62);
    tuning.player.jump_hold_ticks = data[0x64];
    tuning.player.jump_impulse = (sint16)xport_load_le16(data + 0x66);
    tuning.object_explosion_strength = (sint16)xport_load_le16(data + 0xA6);
    tuning.object_explosion_radius = (sint16)xport_load_le16(data + 0xA8);
    tuning.initial_lives = (sint16)xport_load_le16(data + 0xAE);
    tuning.player.maximum_health = xport_load_le16(data + 0xB2);
    tuning.pickup_floor_offset = xport_load_le16(data + 0xBC);
    tuning.tracking_projectile_ticks = (sint16)xport_load_le16(data + 0x104);
    tuning.player.footstep_frames[0] = (sint16)xport_load_le16(data + 0x170);
    tuning.player.footstep_frames[1] = (sint16)xport_load_le16(data + 0x172);
    for (uint32 index = 0; index < 3; ++index)
        tuning.health_pickup_amount[index] = (sint16)xport_load_le16(data + 0x1E2 + index * 2);
    tuning.weapon_activity_value = (sint16)xport_load_le16(data + 0x1F2);
    tuning.shadow_height = xport_load_le16(data + 0x20C);
    tuning.shadow_radius = xport_load_le16(data + 0x20E);
    tuning.player.vertical_drag = data[0x264];
    tuning.player.airborne_drag = data[0x266];
    tuning.launched_projectile_seconds = (sint16)xport_load_le16(data + 0x38A);
    tuning.grenade_radius = (sint16)xport_load_le16(data + 0x390);
}

static void DecodeParticlePreferences(const uint8 *data, uint32 bytes)
{
    if (!data || bytes < 0x26)
    {
        GameDiagnostic("Truncated particle preferences\n");
        abort();
    }
    for (uint32 index = 0; index < 3; ++index)
        tuning.death_particle_color[index] = xport_load_le16(data + 0x20 + index * 2);
}

static uint32 LoadGameConfiguration()
{
    FUNCTION_MARKER(0x80071054, "SLUS_003.73");
    uint32 capacity = LookupResource("tweak.dat");
    uint32 bytes = GetResourceFileSize("tweak.dat");
    if (bytes < 0x392 || bytes > capacity)
    {
        GameDiagnostic("Invalid game tuning resource size\n");
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
        GameDiagnostic("Invalid game preferences resource size\n");
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

uint32 ReloadGameConfiguration()
{
    FUNCTION_MARKER(0x8007113C, "SLUS_003.73");
    return LoadGameConfiguration();
}
