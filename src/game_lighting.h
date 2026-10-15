#ifndef GAME_LIGHTING_H
#define GAME_LIGHTING_H

extern "C"
{
#include "psx.h"
}

sint16 *SetLightingDirections(const sint16 *first, const sint16 *second, const sint16 *third);
sint16 *SetLightingColors(const sint16 *first, const sint16 *second, const sint16 *third);

#endif
