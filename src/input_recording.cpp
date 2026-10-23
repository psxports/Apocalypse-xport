#include <string.h>
#include <stdlib.h>

#include "input_recording.h"
#include "game_diagnostics.h"
#include "resource_io.h"

// Original800A6558 schedules three button events at frame5 then terminates
static const RecordedInputEvent test_replay[4] = {{5, 5, 10, 1}, {5, 5, 0, 1}, {5, 5, 2, 1}, {10, 65535, 0, 0}};

InputRecording input_recording;

static RecordedInputEvent *AllocateInputEvents(uint32 bytes)
{
    uint32 count = bytes / 8 + ((bytes & 7) != 0);
    RecordedInputEvent *events;
    if (count > (size_t)-1 / sizeof(*events))
        abort();
    events = (RecordedInputEvent *)malloc(count * sizeof(*events));
    if (!events)
        abort();
    return events;
}

InputRecording::InputRecording() : mode(Live), data(0), owned(0), bytes(0), cursor(0), last_timestamp(0)
{
}

InputRecording::~InputRecording()
{
    ReleaseData();
}

void InputRecording::Stop()
{
    mode = Live;
}

void InputRecording::ReleaseData()
{
    free(owned);
    owned = 0;
    data = 0;
    bytes = cursor = 0;
}

void ClearInputRecording()
{
    FUNCTION_MARKER(0x80063A38, "SLUS_003.73");
    input_recording.ReleaseData();
    input_recording.Stop();
}

void InputRecording::ResetCursor()
{
    cursor = 0;
}

void InputRecording::ResetTimestamp()
{
    last_timestamp = 0;
}

void InputRecording::Require(uint32 span) const
{
    if (!data || cursor > bytes / 8 || span > bytes - cursor * 8)
    {
        GameDiagnostic(("Input event outside recording buffer\n"));
        abort();
    }
}

void InputRecording::UseTestReplay()
{
    ReleaseData();
    data = test_replay;
    bytes = 32;
    mode = Replay;
}

void InputRecording::LoadReplay(const char *name)
{
    uint32 size = LookupResource(name);
    uint32 logical_bytes = GetResourceFileSize(name);
    if (logical_bytes < 6 || logical_bytes > size)
    {
        GameDiagnostic(("Invalid input recording resource size\n"));
        abort();
    }
    ReleaseData();
    uint8 *encoded = (uint8 *)malloc(size);
    if (!encoded)
        abort();
    BeginResourceRead(encoded);
    WaitForResourceRead();
    owned = AllocateInputEvents(logical_bytes);
    uint32 count = logical_bytes / 8 + ((logical_bytes & 7) != 0);
    for (uint32 idx = 0; idx < count; ++idx)
    {
        uint32 offset = idx * 8;
        uint32 remaining = logical_bytes - offset;
        RecordedInputEvent event = {0, 0, 0, 0};
        if (remaining >= 4)
            memcpy(&event.timestamp, encoded + offset, sizeof(event.timestamp));
        if (remaining >= 6)
            memcpy(&event.frame, encoded + offset + 4, sizeof(event.frame));
        if (remaining >= 7)
            event.button = encoded[offset + 6];
        if (remaining >= 8)
            event.down = encoded[offset + 7];
        owned[idx] = event;
    }
    free(encoded);
    data = owned;
    bytes = logical_bytes;
    mode = Replay;
}

void InputRecording::BeginRecording(uint32 capacity)
{
    if (capacity < 6)
    {
        GameDiagnostic(("Input recording capacity is too small\n"));
        abort();
    }
    ReleaseData();
    owned = AllocateInputEvents(capacity);
    data = owned;
    bytes = capacity;
    owned[0].frame = 65535;
    mode = Recording;
}

void InputRecording::Append(uint32 timestamp, uint16 frame, uint8 button, uint8 down)
{
    Require(14);
    if (mode != Recording || !owned)
    {
        GameDiagnostic(("Input recording is not writable\n"));
        abort();
    }
    if (button >= 20)
    {
        GameDiagnostic(("Input recording button index is invalid\n"));
        abort();
    }
    RecordedInputEvent *event = owned + cursor;
    event->button = button;
    event->frame = frame;
    event->down = down;
    event->timestamp = timestamp;
    ++cursor;
    owned[cursor].frame = 65535;
}

uint16 InputRecording::NextFrame() const
{
    Require(6);
    return data[cursor].frame;
}

bool InputRecording::AtEnd() const
{
    return NextFrame() == 65535;
}

RecordedInputEvent InputRecording::ReadEvent() const
{
    Require(8);
    const RecordedInputEvent &event = data[cursor];
    if (event.frame == 65535 || event.button >= 20)
    {
        GameDiagnostic(("Invalid nonterminal input event\n"));
        abort();
    }
    return event;
}

void InputRecording::AdvanceReplay(uint32 timestamp)
{
    Require(8);
    ++cursor;
    last_timestamp = timestamp;
}

const char *GetDemoRecordingName(uint32 idx)
{
    static const char *const names[4] = {"demo2.dem", "demo3.dem", "demo1.dem", ""};
    if (idx >= 4)
    {
        GameDiagnostic(("Demo recording index outside catalogue\n"));
        abort();
    }
    return names[idx];
}
