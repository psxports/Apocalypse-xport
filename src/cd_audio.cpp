#include "movie_lifecycle.h"
#include "game_diagnostics.h"

uint32 RequestCdStreamRefresh(void)
{
    FUNCTION_MARKER(0x8002EF58, "SLUS_003.73");
    movie_playback.refresh_pending = 1;
    return 1;
}
