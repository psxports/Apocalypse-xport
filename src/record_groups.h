#ifndef RECORD_GROUPS_H
#define RECORD_GROUPS_H

#include "record_data.h"

extern "C"
{
#include "psx.h"
}

uint32 SetRecGrpEnabled(uint32 grp, bool enabled);
uint32 ApplyRecGrpAction(uint32 grp, uint32 mode);
uint32 SetObjectRecGrpMode(RecordCursor grp, uint32 mode);

#endif
