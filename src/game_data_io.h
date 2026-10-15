#ifndef GAME_DATA_IO_H
#define GAME_DATA_IO_H

#include "psx_spu.h"

bool InitializeDataCd();
bool FindDataCdFile(CdlFILE *result, const char *name);
const void *PrepareDataCdLocation(uint32 command, const void *parameter, CdlLOC *location);
void RebaseDataCdResponse(uint8 *response);
uint32 LoadDataResourceDirectory();
bool ReadDataResource(void *destination, uint32 offset, uint32 bytes);

#endif
