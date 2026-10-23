#include "force.h"

uint32 Force::Destroy(uint32 reason)
{
    FUNCTION_MARKER(0x8003A9EC, "SLUS_003.73");
    MarkDestroyed();
    return reason & 1 ? ReleaseStorage() : 0;
}
