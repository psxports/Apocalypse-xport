extern "C"
{
#include "xport.h"
#include "game_sdk_bindings.h"
}
#include <stdio.h>
#include <stdarg.h>

extern "C" sint32 GameDiagnostic(const char *format, ...)
{
#if !defined(XPORT_XCRT)
    va_list arguments;
    va_start(arguments, format);
    sint32 result = vfprintf(stderr, format, arguments);
    va_end(arguments);
    return result;
#else
    (void)format;
    return 0;
#endif
}

extern "C" uint32 sub_8008BF9C(uint32 format, uint32 value)
{
    FUNCTION_MARKER(0x8008BF9C, "SLUS_003.73");
    const char *text = (const char *)psx_addr(format, 1);
    return (uint32)GameDiagnostic(text, (long)(sint32)value);
}

