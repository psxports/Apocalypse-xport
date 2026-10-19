#ifndef GEOMETRY_COMMANDS_H
#define GEOMETRY_COMMANDS_H

#include "psx.h"

uint32 CountGeometryLeadingBits(uint32 value);
void ExecuteGeometryCommand(uint32 opcode);
uint32 ReadGeometryControl(uint32 index);

#endif
