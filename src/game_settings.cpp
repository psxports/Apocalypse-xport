#include <stdlib.h>

#include "game_pad_input.h"
#include "game_diagnostics.h"
#include "game_settings.h"

struct GamePreferencesFile
{
    uint8 reserved[4];
    sint16 music_volume;
    sint16 effects_volume;
    sint16 voice_volume;
};

typedef char GamePreferencesFileSize[sizeof(GamePreferencesFile) == 10 ? 1 : -1];

struct ControlBindingsFile
{
    sint16 digital[4];
    uint8 reserved[24];
    sint16 analog[4];
};

typedef char ControlBindingsFileSize[sizeof(ControlBindingsFile) == 40 ? 1 : -1];

GameSettings game_settings;

void ResetGameSettings()
{
    game_settings = GameSettings();
}

void DecodeGamePreferences(const uint8 *data, uint32 bytes)
{
    if (!data || bytes < 10)
    {
        GameDiagnostic(("Truncated game preferences\n"));
        abort();
    }
    const GamePreferencesFile &file = *(const GamePreferencesFile *)data;
    game_settings.music_volume = file.music_volume;
    game_settings.effects_volume = file.effects_volume;
    game_settings.voice_volume = file.voice_volume;
}

void RestoreDefaultControlBindings()
{
    static const uint8 defaults[4] = {1, 4, 8, 2};
    for (uint32 idx = 0; idx < 4; ++idx)
        game_settings.bindings.digital[(idx + 3) & 3] = defaults[idx];
    for (uint32 idx = 0; idx < 4; ++idx)
        game_settings.bindings.analog[(idx + 3) & 3] = defaults[idx];
}

void DecodeControlBindings(const uint8 *data, uint32 bytes)
{
    if (!data || bytes < 40)
    {
        GameDiagnostic(("Truncated controller bindings\n"));
        abort();
    }
    const ControlBindingsFile &file = *(const ControlBindingsFile *)data;
    for (uint32 idx = 0; idx < 4; ++idx)
    {
        game_settings.bindings.digital[idx] = file.digital[idx];
        game_settings.bindings.analog[idx] = file.analog[idx];
    }
}

uint8 SaveGameSettings(SavedGameSettings &rec)
{
    uint8 val;
    uint32 idx;
    FUNCTION_MARKER(0x80015F7C, "SLUS_003.73");
    rec.difficulty = game_settings.difficulty;
    rec.effects_volume = game_settings.effects_volume;
    rec.music_volume = game_settings.music_volume;
    rec.voice_volume = game_settings.voice_volume;
    for (idx = 0; idx < 8; ++idx)
    {
        val = idx < 4 ? game_settings.bindings.digital[(idx + 3) & 3] : game_settings.bindings.analog[(idx + 3) & 3];
        rec.bindings[idx] = val;
    }
    return val;
}

uint32 RestoreGameSettings(const SavedGameSettings &rec)
{
    uint32 difficulty, idx;
    sint16 voice_volume;
    uint8 val;
    FUNCTION_MARKER(0x80016014, "SLUS_003.73");
    difficulty = rec.difficulty;
    game_settings.effects_volume = rec.effects_volume;
    game_settings.music_volume = rec.music_volume;
    voice_volume = rec.voice_volume;
    game_settings.difficulty = difficulty;
    game_settings.voice_volume = voice_volume;
    for (idx = 0; idx < 8; ++idx)
    {
        val = rec.bindings[idx];
        if (idx < 4)
            game_settings.bindings.digital[(idx + 3) & 3] = val;
        else
            game_settings.bindings.analog[(idx + 3) & 3] = val;
    }
    game_pads[0].SetButtonMasks(game_settings.bindings.digital[0], game_settings.bindings.digital[1], game_settings.bindings.digital[2], game_settings.bindings.digital[3]);
    return game_pads[0].SetAxes(3, 2, 1, 0, game_settings.bindings.analog[0], game_settings.bindings.analog[1], game_settings.bindings.analog[2], game_settings.bindings.analog[3]);
}
