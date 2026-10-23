#include "audio_session.h"
#include "game_tuning.h"
#include "game_pad_input.h"
#include "visual_object.h"
#include "cheat_input.h"

static CheatOptions cheat_options = {0, 0, 0, 1};
static uint32 cheat_cursor;
static uint8 cheat_input[16];

struct CheatPattern
{
    const char *code;
    uint8 len;
};

static const CheatPattern cheat_patterns[12] = {{"SCUDXS", 6}, {"DULLTURD", 8}, {"SUX", 3}, {"XTC", 3}, {"SLUT", 4}, {"XXXXX", 5}, {"TCXS", 4}, {"DDT", 3}, {"SCX", 3}, {"LUXTST", 6}, {"STRUDL", 6}, {"TUXD", 4}};

CheatOptions &GetCheatOptions()
{
    return cheat_options;
}

void ClearCheatInputBuffer()
{
    FUNCTION_MARKER(0x80076D44, "SLUS_003.73");
    for (sint32 idx = 15; idx >= 0; --idx)
        cheat_input[idx] = 0;
}

bool UpdateCheatInput()
{
    uint8 key;
    sint32 cursor, entry, remaining, matched = -1;
    bool equal;
    uint32 val;
    const char *pattern;
    FUNCTION_MARKER(0x80076D6C, "SLUS_003.73");
    if (!game_pads[0].buttons[4].down)
        return false;
    if (game_pads[0].buttons[10].pressed)
        key = 'U';
    else if (game_pads[0].buttons[9].pressed)
        key = 'R';
    else if (game_pads[0].buttons[11].pressed)
        key = 'D';
    else if (game_pads[0].buttons[8].pressed)
        key = 'L';
    else if (game_pads[0].buttons[0].pressed)
        key = 'T';
    else if (game_pads[0].buttons[2].pressed)
        key = 'C';
    else if (game_pads[0].buttons[3].pressed)
        key = 'X';
    else if (game_pads[0].buttons[1].pressed)
        key = 'S';
    else
        return false;
    cursor = cheat_cursor;
    cheat_input[cursor] = key;
    for (entry = 0; entry < 12; ++entry)
    {
        remaining = cheat_patterns[entry].len;
        cursor = cheat_cursor;
        --remaining;
        pattern = cheat_patterns[entry].code;
        if (cheat_input[cursor] == pattern[remaining])
        {
            equal = true;
            for (;;)
            {
                --cursor;
                if (!remaining)
                    break;
                --remaining;
                if (cursor < 0)
                    cursor = 15;
                if (cheat_input[cursor] != pattern[remaining])
                {
                    equal = false;
                    break;
                }
            }
            if (equal)
            {
                matched = entry;
                break;
            }
        }
    }
    if (matched != -1)
        cheat_input[cheat_cursor] = 0;
    cursor = cheat_cursor + 1;
    cheat_cursor = cursor;
    if (cursor == 16)
        cheat_cursor = 0;
    switch (matched)
    {
        case 0:
            GetCheatOptions().weapon_cheat ^= 1;
            break;
        case 1:
            if (visual_lists.players)
                GetCheatOptions().invulnerable ^= 1;
            break;
        case 2:
            GetCheatOptions().status_hud ^= 1;
            break;
        case 3:
            if (visual_lists.players)
                static_cast<VisualObject *>(visual_lists.players->owner)->health = (sint16)GetGameTuning().player.maximum_health;
            break;
        case 4:
            GetCheatOptions().enemy_finish_cheat ^= 1;
            break;
        case 5:
            GetCheatOptions().free_movement ^= 1;
            break;
        case 6:
            GetCheatOptions().unlimited_lives ^= 1;
            break;
        case 7:
            GetCheatOptions().debug_panel ^= 1;
            break;
        case 8:
            GetCheatOptions().level_skip ^= 1;
            break;
        case 9:
            GetCheatOptions().suspend_resource_loading ^= 1;
            break;
        case 10:
            GetCheatOptions().war_rec_spawn ^= 1;
            break;
        case 11:
            val = GetCheatOptions().level_select ^ 1;
            GetCheatOptions().level_select = val;
            GetCheatOptions().autotest = val;
            GetCheatOptions().build_date = val;
            GetCheatOptions().level_skip = val;
            break;
        default:
            break;
    }
    if (matched == -1)
        return false;
    PlaySoundEffect(26, 8192, 0);
    return true;
}
