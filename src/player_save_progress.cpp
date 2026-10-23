#include <stdlib.h>

#include "player_save_progress.h"
#include "global.h"
#include "game_diagnostics.h"

static void CopyProgressName(const char *src, char *dst, uint32 capacity)
{
    if (!src)
    {
        GameDiagnostic(("Missing player save name\n"));
        abort();
    }
    uint32 len = 0;
    while (len < capacity && src[len])
        ++len;
    if (len == capacity)
    {
        GameDiagnostic(("Player save name exceeds its field capacity\n"));
        abort();
    }
    CopyGameString(src, dst);
}

void PlayerSaveProgress::SetLevel(const char *name)
{
    CopyProgressName(name, level, sizeof(level));
}

void PlayerSaveProgress::SetCheckpoint(const char *name)
{
    CopyProgressName(name, checkpoint, sizeof(checkpoint));
}
