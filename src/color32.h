#ifndef COLOR32_H
#define COLOR32_H

extern "C"
{
#include "psx.h"
}

// Signed channels retain fade values before GPU byte conversion
struct Color32
{
    sint32 r, g, b;
};

#endif
