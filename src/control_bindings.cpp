#include "control_bindings.h"

ControlBindings::ControlBindings()
{
    for (uint32 idx = 0; idx < 4; ++idx)
        digital[idx] = analog[idx] = 0;
}
