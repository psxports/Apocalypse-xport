#ifndef OBJECT_PARAMETERS_H
#define OBJECT_PARAMETERS_H

extern "C"
{
#include "psx.h"
}

// Native adapter preserves the original getter target and adjusted receiver
uint32 DispatchObjectParameterGetter(uint32 target, uint32 receiver, uint32 command);
void DispatchObjectParameterWriter(uint32 target, uint32 receiver, uint32 command);
void DispatchBufferedObjectParameterWriter(uint32 target, uint32 buffer_owner, uint32 receiver, uint32 command, const uint16 *words, uint32 count);

#endif
