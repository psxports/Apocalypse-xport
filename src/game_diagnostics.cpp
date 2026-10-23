#if !defined(NDEBUG)
    #include <stdio.h>
    #include <stdarg.h>
#endif

#include "game_diagnostics.h"

#if !defined(NDEBUG)
extern "C" sint32 PrintGameDiagnostic(const char *format, ...)
{
    va_list arguments;
    va_start(arguments, format);
    sint32 result = vfprintf(stderr, format, arguments);
    va_end(arguments);
    return result;
}
#endif
