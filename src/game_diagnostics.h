#ifndef GAME_DIAGNOSTICS_H
#define GAME_DIAGNOSTICS_H

#include "psx.h"

#ifdef __cplusplus
extern "C"
{
#endif
#if defined(NDEBUG)
    #define GameDiagnostic(args) ((void)0)
#else
sint32 PrintGameDiagnostic(const char *format, ...);
    // Parenthesized arguments preserve C++98 preprocessing
    #define GameDiagnostic(args) PrintGameDiagnostic args
#endif
#ifdef __cplusplus
}
#endif

#endif
