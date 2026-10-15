#ifndef OBJECT_INITIALIZERS_H
#define OBJECT_INITIALIZERS_H

extern "C"
{
#include "psx.h"
}

uint32 InitializeObjectUpVector();
uint32 InitializeObjectMotionConstants();
uint32 InitializeObjectZeroVectors();

#endif
