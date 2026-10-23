#ifndef CONTROL_BINDINGS_H
#define CONTROL_BINDINGS_H

#include "psx.h"

struct ControlBindings
{
    sint16 digital[4];
    sint16 analog[4];

public:
    ControlBindings();
};

#endif
