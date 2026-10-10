#include "game_str_vlc.h"
#include "psx_press.h"

uint32 apocalypse_str_vlc_decode(uint32 source, uint32 destination, uint32 table)
{
    return (uint32)DecDCTvlc2(source ? (uint32 *)psx_addr(source, 12u) : NULL, destination ? (uint32 *)psx_addr(destination, 4u) : NULL, (uint16 *)psx_addr(table, sizeof(DECDCTTAB)));
}
