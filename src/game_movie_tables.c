#include "draft_first_signatures.h"
#include "psx_press.h"

uint32 sub_8009C65C(uint32 destination)
{
    DecDCTvlcBuild((uint16 *)psx_addr(destination, sizeof(DECDCTTAB)));
    return 1u;
}
