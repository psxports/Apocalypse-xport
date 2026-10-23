#ifndef CHECKPOINT_NAMES_H
#define CHECKPOINT_NAMES_H

#include "psx.h"

class CheckpointNames
{
private:
    CheckpointNames &operator=(const CheckpointNames &);
    char **names;
    uint32 count;
    uint32 capacity;

public:
    CheckpointNames();
    ~CheckpointNames();
    void Clear();
    void Add(const char *name);
    uint32 Count() const;
    const char *Get(uint32 idx) const;

private:
    CheckpointNames(const CheckpointNames &);
};

#endif
