#include "movie_lifecycle.h"

uint32 RequestCdStreamRefresh()
{
    FUNCTION_MARKER(0x8002EF58, "SLUS_003.73");
    movie_playback.refresh_pending = true;
    return 1;
}
