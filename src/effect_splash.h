#ifndef EFFECT_SPLASH_H
#define EFFECT_SPLASH_H

#include "vector3.h"

extern "C"
{
#include "psx.h"
}

#include "effect_object.h"
#include "mesh_ring_effect.h"

uint32 CreateSplash(const Vector3 *pos);

#endif
