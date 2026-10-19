#ifndef GAME_SETTINGS_H
#define GAME_SETTINGS_H

extern "C"
{
#include "psx.h"
}

struct ControlBindings
{
    ControlBindings();
    sint16 digital[4];
    sint16 analog[4];
};

enum VibrationMode
{
    VibrationNone,
    VibrationSome,
    VibrationFull
};

struct GameSettings
{
    GameSettings() : music_volume(0), effects_volume(0), voice_volume(0), vibration(VibrationFull), difficulty(0)
    {
    }

    ControlBindings bindings;
    uint16 music_volume;
    uint16 effects_volume;
    uint16 voice_volume;
    uint32 vibration;
    uint32 difficulty;
};

extern GameSettings game_settings;
void ResetGameSettings();
void DecodeGamePreferences(const uint8 *data, uint32 bytes);
void RestoreDefaultControlBindings();
void DecodeControlBindings(const uint8 *data, uint32 bytes);

enum
{
    GameSettingsSaveSize = 24
};

struct SavedGameSettings
{
    uint32 difficulty;
    sint32 effects_volume;
    sint32 music_volume;
    sint32 voice_volume;
    uint8 bindings[8];
};

uint32 SaveGameSettings(SavedGameSettings &record);
uint32 RestoreGameSettings(const SavedGameSettings &record);

#endif
