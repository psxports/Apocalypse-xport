#ifndef OBJECT_INITIALIZERS_H
#define OBJECT_INITIALIZERS_H

#include "vector3.h"

extern "C"
{
#include "psx.h"
}

void ResetGameConstructors();
void InitializeGameConstructors();
extern const Vector3 zero_vector;
extern const ShortVector3 zero_angles;

#endif
