#include "game_pad_poll.h"
#include "menu_input.h"

uint32 ReadMenuInput(uint32 *confirm, uint32 *cancel, uint32 *any, uint32 *start)
{
    FUNCTION_MARKER(0x80017364, "SLUS_003.73");
    static const uint8 events[7] = {1, 2, 15, 4, 5, 6, 7};
    uint32 value = game_pads[0].buttons[14].pressed;
    *start = value;
    if (value)
        game_pads[0].buttons[14].pressed = 0;
    value = 0;
    if (game_pads[0].buttons[3].pressed || *start)
        value = 1;
    *confirm = value;
    if (value)
        game_pads[0].buttons[3].pressed = 0;
    value = game_pads[0].buttons[0].pressed;
    *cancel = value;
    if (value)
        game_pads[0].buttons[0].pressed = 0;
    uint32 cancel_value = *cancel;
    uint32 confirm_value = *confirm;
    *any = cancel_value | confirm_value;
    for (uint32 index = 0; index < 7; ++index)
    {
        if (game_pads[0].buttons[events[index]].pressed)
        {
            game_pads[0].buttons[events[index]].pressed = 0;
            *any = 1;
        }
    }
    value = *confirm;
    if (value)
        *cancel = 0;
    return value;
}
