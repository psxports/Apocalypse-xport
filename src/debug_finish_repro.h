#ifndef DEBUG_FINISH_REPRO_H
#define DEBUG_FINISH_REPRO_H

#include "level_session.h"

struct PlayerSaveRecord;

#if defined(_DEBUG)
void ResetDebugFinishRepro();
void DebugLoadFinishPlayerSave(PlayerSaveRecord &rec);
void DebugRestoreFinishMenu();
#endif

#endif
