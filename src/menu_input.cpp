#include "game_pad_input.h"
#include "menu_input.h"

uint32 ReadMenuInput(uint32 *confirm, uint32 *cancel, uint32 *any, uint32 *start)
{
    FUNCTION_MARKER(0x80017364, "SLUS_003.73");
    static const uint8 events[7] = {1, 2, 15, 4, 5, 6, 7};
    uint32 val = game_pads[0].buttons[14].pressed;
    *start = val;
    if (val)
        game_pads[0].buttons[14].pressed = 0;
    val = 0;
    if (game_pads[0].buttons[3].pressed || *start)
        val = 1;
    *confirm = val;
    if (val)
        game_pads[0].buttons[3].pressed = 0;
    val = game_pads[0].buttons[0].pressed;
    *cancel = val;
    if (val)
        game_pads[0].buttons[0].pressed = 0;
    *any = *cancel | *confirm;
    for (uint32 idx = 0; idx < 7; ++idx)
    {
        if (game_pads[0].buttons[events[idx]].pressed)
        {
            game_pads[0].buttons[events[idx]].pressed = 0;
            *any = 1;
        }
    }
    val = *confirm;
    if (val)
        *cancel = 0;
    return val;
}
