#ifndef PAD_BUTTON_H
#define PAD_BUTTON_H

#include "psx.h"

struct PadButton
{
    PadButton() : down(false), pressed(false), held(0), released(0), elapsed(0)
    {
    }

    bool down;
    bool pressed;
    uint32 held;
    uint32 released;
    uint32 elapsed;

public:
    uint32 Update(bool val);
    uint32 Record(bool val, uint8 idx);
    uint32 Replay(uint32 idx);
};

#endif
