#ifndef MENU_LIFECYCLE_H
#define MENU_LIFECYCLE_H

extern "C"
{
#include "psx.h"
}

class Object;

void DestroyMenuObject(Object *object);

#endif
