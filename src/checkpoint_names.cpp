#include <stdlib.h>

#include "checkpoint_names.h"
#include "global.h"
#include "game_diagnostics.h"

CheckpointNames::CheckpointNames() : names(0), count(0), capacity(0)
{
}

CheckpointNames::~CheckpointNames()
{
    Clear();
}

void CheckpointNames::Clear()
{
    for (uint32 idx = 0; idx < count; ++idx)
        free(names[idx]);
    free(names);
    names = 0;
    count = 0;
    capacity = 0;
}

void CheckpointNames::Add(const char *name)
{
    if (count == capacity)
    {
        size_t limit = static_cast<size_t>(-1) / sizeof(*names);
        if (count >= limit || count == 0xFFFFFFFF)
        {
            GameDiagnostic(("Checkpoint name count overflow\n"));
            abort();
        }
        uint32 next_capacity = capacity ? capacity : 8;
        if (next_capacity <= count)
            next_capacity = capacity > 0x7FFFFFFF || capacity > limit / 2 ? count + 1 : capacity * 2;
        if (next_capacity > limit)
            next_capacity = count + 1;
        char **next = (char **)realloc(names, next_capacity * sizeof(*names));
        if (!next)
        {
            GameDiagnostic(("Cannot allocate checkpoint names\n"));
            abort();
        }
        names = next;
        capacity = next_capacity;
    }
    names[count++] = AllocateStringCopy(name);
}

uint32 CheckpointNames::Count() const
{
    return count;
}

const char *CheckpointNames::Get(uint32 idx) const
{
    if (idx >= count)
    {
        GameDiagnostic(("Checkpoint name index out of bounds\n"));
        abort();
    }
    return names[idx];
}

CheckpointNames checkpoint_names;
