#ifndef LEVEL_CATALOGUE_H
#define LEVEL_CATALOGUE_H

#include "psx.h"

struct LevelEntry
{
    const char *title;
    const char *name;
    const char *music;
    const char *sound_bank;
    const char *loading_image;
    uint8 completion_movie;
};

uint32 GetLevelCount();
const LevelEntry *GetLevelEntry(uint32 idx);
const LevelEntry *NextLevelEntry(const LevelEntry *entry);
const LevelEntry *FindLevelEntry(const char *name);

#endif
