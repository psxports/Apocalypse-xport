#ifndef INPUT_RECORDING_H
#define INPUT_RECORDING_H

extern "C"
{
#include "psx.h"
}

struct RecordedInputEvent
{
    uint32 timestamp;
    uint16 frame;
    uint8 button;
    uint8 down;
};

class InputRecording
{
public:
    enum Mode
    {
        Live,
        Recording,
        Replay
    };

    Mode GetMode() const
    {
        return mode;
    }

    bool HasBuffer() const
    {
        return data != 0;
    }

private:
    InputRecording &operator=(const InputRecording &);
    Mode mode;
    const RecordedInputEvent *data;
    RecordedInputEvent *owned;
    uint32 bytes;
    uint32 cursor;
    uint32 last_timestamp;

public:
    InputRecording();
    ~InputRecording();
    void Stop();
    void ReleaseData();
    void ResetCursor();
    void ResetTimestamp();
    void UseTestReplay();
    void LoadReplay(const char *name);
    void BeginRecording(uint32 capacity);
    void Append(uint32 timestamp, uint16 frame, uint8 button, uint8 down);
    uint16 NextFrame() const;
    bool AtEnd() const;
    RecordedInputEvent ReadEvent() const;
    void AdvanceReplay(uint32 timestamp);

private:
    InputRecording(const InputRecording &);
    void Require(uint32 span) const;
};

extern InputRecording input_recording;
void ClearInputRecording();
const char *GetDemoRecordingName(uint32 idx);

#endif
