#include "pad_button.h"
#include "game_pad_input.h"
#include "input_recording.h"
#include "game_tick.h"

uint32 PadButton::Update(bool val)
{
    FUNCTION_MARKER(0x800702E0, "SLUS_003.73");
    ++elapsed;
    if (!down && val)
    {
        pressed = true;
        elapsed = 0;
    }
    if (val)
        pad_polling.idle_ticks = 0;
    down = val;
    if (down)
    {
        released = 0;
        return ++held;
    }
    held = 0;
    return ++released;
}

uint32 PadButton::Record(bool val, uint8 idx)
{
    FUNCTION_MARKER(0x80070354, "SLUS_003.73");
    ++elapsed;
    if (!down && val)
    {
        pressed = true;
        elapsed = 0;
    }
    if (down != val)
        input_recording.Append(game_clock.elapsed, pad_polling.frame, idx, val);
    down = val;
    if (down)
    {
        released = 0;
        return ++held;
    }
    held = 0;
    return ++released;
}

uint32 PadButton::Replay(uint32 idx)
{
    FUNCTION_MARKER(0x80070410, "SLUS_003.73");
    if ((sint32)pad_polling.frame < input_recording.NextFrame())
        return 1;
    RecordedInputEvent event = input_recording.ReadEvent();
    if (event.button != idx)
        return event.button;
    Update(event.down);
    input_recording.AdvanceReplay(event.timestamp);
    return 1;
}
