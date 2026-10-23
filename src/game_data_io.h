#ifndef GAME_DATA_IO_H
#define GAME_DATA_IO_H

#include "psx_spu.h"

uint32 ControlDataCd(uint32 command, const void *parameter, void *output);
uint32 FastControlDataCd(uint32 command, const void *parameter);
uint32 SyncDataCd(sint32 mode, uint8 *response);

bool InitializeDataCd();
bool FindDataCdFile(CdlFILE *result, const char *name);
bool ReadDataResource(void *dst, uint32 offset, uint32 bytes);

#endif
