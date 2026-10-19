#include <stdlib.h>
#include <string.h>

#include "game_pad_poll.h"
#include "game_diagnostics.h"
#include "game_settings.h"

GameSettings game_settings;

void ResetGameSettings()
{
    game_settings = GameSettings();
}

void DecodeGamePreferences(const uint8 *data, uint32 bytes)
{
    if (!data || bytes < 10)
    {
        GameDiagnostic("Truncated game preferences\n");
        abort();
    }
    game_settings.music_volume = xport_load_le16(data + 4);
    game_settings.effects_volume = xport_load_le16(data + 6);
    game_settings.voice_volume = xport_load_le16(data + 8);
}

ControlBindings::ControlBindings()
{
    for (uint32 index = 0; index < 4; ++index)
        digital[index] = analog[index] = 0;
}

void RestoreDefaultControlBindings()
{
    static const uint8 defaults[4] = {1, 4, 8, 2};
    for (uint32 index = 0; index < 4; ++index)
        game_settings.bindings.digital[(index + 3) & 3] = defaults[index];
    for (uint32 index = 0; index < 4; ++index)
        game_settings.bindings.analog[(index + 3) & 3] = defaults[index];
}

void DecodeControlBindings(const uint8 *data, uint32 bytes)
{
    if (!data || bytes < 40)
    {
        GameDiagnostic("Truncated controller bindings\n");
        abort();
    }
    for (uint32 index = 0; index < 4; ++index)
    {
        game_settings.bindings.digital[index] = (sint16)xport_load_le16(data + index * 2);
        game_settings.bindings.analog[index] = (sint16)xport_load_le16(data + 32 + index * 2);
    }
}

uint32 SaveGameSettings(SavedGameSettings &record)
{
    uint32 value, index;
    FUNCTION_MARKER(0x80015F7C, "SLUS_003.73");
    value = game_settings.difficulty;
    record.difficulty = value;
    value = (uint32)(sint32)(sint16)game_settings.effects_volume;
    record.effects_volume = value;
    value = (uint32)(sint32)(sint16)game_settings.music_volume;
    record.music_volume = value;
    value = (uint32)(sint32)(sint16)game_settings.voice_volume;
    record.voice_volume = value;
    for (index = 0; index < 8; ++index)
    {
        value = (uint8)(index < 4 ? game_settings.bindings.digital[(index + 3) & 3] : game_settings.bindings.analog[(index + 3) & 3]);
        record.bindings[index] = value;
    }
    return value;
}

uint32 RestoreGameSettings(const SavedGameSettings &record)
{
    uint32 difficulty, value, index;
    FUNCTION_MARKER(0x80016014, "SLUS_003.73");
    difficulty = record.difficulty;
    value = (uint16)record.effects_volume;
    game_settings.effects_volume = value;
    value = (uint16)record.music_volume;
    game_settings.music_volume = value;
    value = (uint16)record.voice_volume;
    game_settings.difficulty = difficulty;
    game_settings.voice_volume = value;
    for (index = 0; index < 8; ++index)
    {
        value = record.bindings[index];
        if (index < 4)
            game_settings.bindings.digital[(index + 3) & 3] = value;
        else
            game_settings.bindings.analog[(index + 3) & 3] = value;
    }
    game_pads[0].SetButtonMasks(game_settings.bindings.digital[0], game_settings.bindings.digital[1], game_settings.bindings.digital[2], game_settings.bindings.digital[3]);
    return game_pads[0].SetAxes(3, 2, 1, 0, game_settings.bindings.analog[0], game_settings.bindings.analog[1], game_settings.bindings.analog[2], game_settings.bindings.analog[3]);
}
