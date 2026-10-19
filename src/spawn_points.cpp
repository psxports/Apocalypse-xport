#include "game_diagnostics.h"
#include "spawn_points.h"
#include "game_random.h"

void ShuffleSpawnPoints(uint16 *points, uint32 count)
{
    uint32 index = 0, value, selected, replacement;
    FUNCTION_MARKER(0x80067E18, "SLUS_003.73");
    while ((sint32)index < (sint32)count)
    {
        value = points[index];
        selected = RandomRange(count);
        replacement = points[selected];
        points[index] = replacement;
        ++index;
        points[selected] = value;
    }
}
