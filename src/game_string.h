#ifndef GAME_STRING_H
#define GAME_STRING_H

extern "C"
{
#include "psx.h"
}

uint32 CopyGameString(const char *source, char *destination);
uint32 NamesEqualIgnoringCase(const char *left, const char *right);

#endif
