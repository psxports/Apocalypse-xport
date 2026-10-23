#ifndef GAME_SETTINGS_H
#define GAME_SETTINGS_H

#include "control_bindings.h"

extern "C"
{
#include "psx.h"
}

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
    // Signed gains retain halfword wrap before the menu sign-bit and upper clamps
    sint16 music_volume;
    sint16 effects_volume;
    sint16 voice_volume;
    VibrationMode vibration;
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

uint8 SaveGameSettings(SavedGameSettings &rec);
uint32 RestoreGameSettings(const SavedGameSettings &rec);

#endif
