#include "level_catalogue.h"
#include "game_string.h"

static const LevelEntry levels[] =
{
    {"Prison", "prsn_t", "prison", "level1", "load01.bmr", 15},
    {"Sewers", "sewr_t", "sewer", "level2", "load02.bmr", 16},
    {"City", "city_t", "plague", "level3", "load03.bmr", 17},
    {"Death", "death_t", "death", "level3a", "load04.bmr", 18},
    {"Rooftops", "roof_t", "rooftops", "level4", "load05.bmr", 19},
    {"Graveyard", "grav_t", "cemetary", "level5", "load06.bmr", 20},
    {"Plague", "pest_t", "plague", "level5a", "load07.bmr", 21},
    {"War Factory", "tube_t", "warfact", "level6", "load08.bmr", 22},
    {"War", "war_t", "warboss", "level6a", "load09.bmr", 23},
    {"White House", "wh_t", "backwall", "level7", "load10.bmr", 24},
    {"The Beast", "bst_t", "beast", "level7a", "load11.bmr", 25}
};

uint32 GetLevelCount()
{
    return sizeof(levels) / sizeof(levels[0]);
}

const LevelEntry *GetLevelEntry(uint32 idx)
{
    return idx < GetLevelCount() ? levels + idx : 0;
}

const LevelEntry *NextLevelEntry(const LevelEntry *entry)
{
    for (uint32 idx = 0; idx < GetLevelCount(); ++idx)
        if (entry == levels + idx)
            return GetLevelEntry(idx + 1);
    return 0;
}

const LevelEntry *FindLevelEntry(const char *name)
{
    FUNCTION_MARKER(0x80018948, "SLUS_003.73");
    for (uint32 idx = 0; idx < GetLevelCount(); ++idx)
        if (NamesEqualIgnoringCase(levels[idx].name, name))
            return levels + idx;
    return 0;
}
