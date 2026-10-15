#ifndef GAME_RANDOM_H
#define GAME_RANDOM_H

extern "C"
{
#include "psx.h"
}

uint32 SetRandomSeed(uint32 seed);
uint32 SetAuxiliaryRandomSeed(uint32 seed);
uint32 RandomRange(uint32 range);

#endif
