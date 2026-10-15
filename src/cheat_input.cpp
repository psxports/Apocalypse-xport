extern "C"
{
#include "draft_first_signatures.h"
}

static void ToggleCheatFlag(uint32 address)
{
    w_u32(address, r_u32(address) ^ 1);
}

uint32 UpdateCheatInput(void)
{
    uint32 key, cursor, entry, pattern, remaining, matched = (uint32)-1, equal, current, candidate, value, player;
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
    cursor = r_u32(0x800FF8A8);
    w_u8(0x801028A8 + cursor, key);
    for (entry = 0; entry < 12; ++entry)
    {
        remaining = r_u8(0x800ED504 + entry);
        cursor = r_u32(0x800FF8A8);
        --remaining;
        pattern = r_u32(0x800ED4D4 + entry * 4);
        current = r_u8(0x801028A8 + cursor);
        candidate = r_u8(pattern + remaining);
        if (current == candidate)
        {
            pattern = r_u32(0x800ED4D4 + entry * 4);
            equal = 1;
            for (;;)
            {
                --cursor;
                if (!remaining)
                    break;
                --remaining;
                if ((sint32)cursor < 0)
                    cursor = 15;
                current = r_u8(0x801028A8 + cursor);
                candidate = r_u8(pattern + remaining);
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
        w_u8(0x801028A8 + r_u32(0x800FF8A8), 0);
    cursor = r_u32(0x800FF8A8) + 1;
    w_u32(0x800FF8A8, cursor);
    if (cursor == 16)
        w_u32(0x800FF8A8, 0);
    switch (matched)
    {
        case 0: ToggleCheatFlag(0x800FF318); break;
        case 1:
            if (r_u32(0x800FF5A0)) ToggleCheatFlag(0x800FF31C);
            break;
        case 2: ToggleCheatFlag(0x800FF324); break;
        case 3:
            player = r_u32(0x800FF5A0);
            if (player) w_u16(player + 218, r_u16(0x800EC526));
            break;
        case 4: ToggleCheatFlag(0x800FF328); break;
        case 5: ToggleCheatFlag(0x800FF32C); break;
        case 6: ToggleCheatFlag(0x800FF320); break;
        case 7: ToggleCheatFlag(0x800FF33C); break;
        case 8: ToggleCheatFlag(0x800FF338); break;
        case 9: ToggleCheatFlag(0x800FF340); break;
        case 10: ToggleCheatFlag(0x800FF344); break;
        case 11:
            value = r_u32(0x800FF330) ^ 1;
            w_u32(0x800FF330, value);
            w_u32(0x800FF334, value);
            w_u32(0x800FF348, value);
            w_u32(0x800FF338, value);
            break;
        default: break;
    }
    if (matched == (uint32)-1)
        return 0;
    sub_80069DF0(26, 8192, 0);
    return 1;
}

extern "C" uint32 sub_80076D6C(void)
{
    FUNCTION_MARKER(0x80076D6C, "SLUS_003.73");
    return UpdateCheatInput();
}
