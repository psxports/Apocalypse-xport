#include "visual_object.h"
#include "cheat_input.h"
extern "C"
{
#include "draft_first_signatures.h"
}

static CheatOptions cheat_options = {0, 0, 0, 1};
static uint32 cheat_cursor;
static uint8 cheat_input[16];

struct CheatPattern
{
    const char *code;
    uint32 len;
};

static const CheatPattern cheat_patterns[12] = {
    {"SCUDXS", 6},
    {"DULLTURD", 8},
    {"SUX", 3},
    {"XTC", 3},
    {"SLUT", 4},
    {"XXXXX", 5},
    {"TCXS", 4},
    {"DDT", 3},
    {"SCX", 3},
    {"LUXTST", 6},
    {"STRUDL", 6},
    {"TUXD", 4}
};

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

static void ToggleCheatFlag(uint32 &value)
{
    value ^= 1;
}

uint32 UpdateCheatInput(void)
{
    uint32 key, cursor, entry, remaining, matched = (uint32)-1, equal, current, candidate, value, player;
    const char *pattern;
    FUNCTION_MARKER(0x80076D6C, "SLUS_003.73");
    if (!r_u8(0x800EC138))
        return 0;
    if (r_u8(0x800EC199)) key = 85;
    else if (r_u8(0x800EC189)) key = 82;
    else if (r_u8(0x800EC1A9)) key = 68;
    else if (r_u8(0x800EC179)) key = 76;
    else if (r_u8(0x800EC0F9)) key = 84;
    else if (r_u8(0x800EC119)) key = 67;
    else if (r_u8(0x800EC129)) key = 88;
    else if (r_u8(0x800EC109)) key = 83;
    else return 0;
    cursor = cheat_cursor;
    cheat_input[cursor] = (uint8)key;
    for (entry = 0; entry < 12; ++entry)
    {
        remaining = cheat_patterns[entry].len;
        cursor = cheat_cursor;
        --remaining;
        pattern = cheat_patterns[entry].code;
        current = cheat_input[cursor];
        candidate = ((uint32)(uint8)pattern[remaining]);
        if (current == candidate)
        {
            pattern = cheat_patterns[entry].code;
            equal = 1;
            for (;;)
            {
                --cursor;
                if (!remaining)
                    break;
                --remaining;
                if ((sint32)cursor < 0)
                    cursor = 15;
                current = cheat_input[cursor];
                candidate = ((uint32)(uint8)pattern[remaining]);
                if (current != candidate)
                {
                    equal = 0;
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
    if (matched != (uint32)-1)
        cheat_input[cheat_cursor] = 0;
    cursor = cheat_cursor + 1;
    cheat_cursor = cursor;
    if (cursor == 16)
        cheat_cursor = 0;
    switch (matched)
    {
        case 0: ToggleCheatFlag(GetCheatOptions().weapon_cheat); break;
        case 1:
            if (visual_lists.players) ToggleCheatFlag(GetCheatOptions().invulnerable);
            break;
        case 2: ToggleCheatFlag(GetCheatOptions().status_hud); break;
        case 3:
            player = visual_lists.players;
            if (player) w_u16(player + 218, r_u16(0x800EC526));
            break;
        case 4: ToggleCheatFlag(GetCheatOptions().enemy_finish_cheat); break;
        case 5: ToggleCheatFlag(GetCheatOptions().free_movement); break;
        case 6: ToggleCheatFlag(GetCheatOptions().unlimited_lives); break;
        case 7: ToggleCheatFlag(GetCheatOptions().debug_panel); break;
        case 8: ToggleCheatFlag(GetCheatOptions().level_skip); break;
        case 9: ToggleCheatFlag(GetCheatOptions().suspend_resource_loading); break;
        case 10: ToggleCheatFlag(GetCheatOptions().war_record_spawn); break;
        case 11:
            value = GetCheatOptions().level_select ^ 1;
            GetCheatOptions().level_select = value;
            GetCheatOptions().autotest = value;
            GetCheatOptions().build_date = value;
            GetCheatOptions().level_skip = value;
            break;
        default: break;
    }
    if (matched == (uint32)-1)
        return 0;
    sub_80069DF0(26, 8192, 0);
    return 1;
}


