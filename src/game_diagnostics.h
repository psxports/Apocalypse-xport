#ifndef GAME_DIAGNOSTICS_H
#define GAME_DIAGNOSTICS_H

#include "psx.h"

#ifdef __cplusplus
extern "C"
{
#endif
#if defined(NDEBUG)
    #define GameDiagnostic(...) ((void)0)
#else
sint32 GameDiagnostic(const char *format, ...);
#endif
#ifdef __cplusplus
}
#endif

#endif
