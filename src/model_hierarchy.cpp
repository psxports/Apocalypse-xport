#include "model_hierarchy.h"

// Original hierarchy table 800A68E4
static const ModelHierarchyLink war_hierarchy_links[] =
{
    { 10, 65535 },
    { 9, 0 },
    { 8, 1 },
    { 18, 2 },
    { 19, 3 },
    { 11, 0 },
    { 12, 5 },
    { 13, 6 },
    { 14, 0 },
    { 15, 8 },
    { 16, 9 },
    { 3, 2 },
    { 5, 11 },
    { 4, 12 },
    { 0, 2 },
    { 2, 14 },
    { 1, 15 },
    { 6, 2 },
    { 7, 17 },
    { 17, 2 },
};

const ModelHierarchyTable WarHierarchy = { 20, war_hierarchy_links };

// Original hierarchy table 800A6AAC
static const ModelHierarchyLink the_beast_hierarchy_links[] =
{
    { 0, 65535 },
    { 11, 0 },
    { 12, 1 },
    { 14, 2 },
    { 13, 3 },
    { 15, 0 },
    { 18, 5 },
    { 17, 6 },
    { 16, 7 },
    { 9, 0 },
    { 1, 9 },
    { 2, 10 },
    { 3, 11 },
    { 4, 11 },
    { 5, 9 },
    { 6, 14 },
    { 7, 15 },
    { 8, 9 },
    { 10, 17 },
};

const ModelHierarchyTable TheBeastHierarchy = { 19, the_beast_hierarchy_links };

// Original hierarchy table 800A6CF4
static const ModelHierarchyLink chopper_hierarchy_links[] =
{
    { 3, 65535 },
    { 8, 0 },
    { 9, 0 },
    { 10, 0 },
};

const ModelHierarchyTable ChopperHierarchy = { 4, chopper_hierarchy_links };

// Original hierarchy table 800A6D28
static const ModelHierarchyLink tank_hierarchy_links[] =
{
    { 0, 65535 },
    { 3, 0 },
    { 2, 1 },
    { 1, 0 },
};

const ModelHierarchyTable TankHierarchy = { 4, tank_hierarchy_links };

// Original hierarchy table 800A6DA4
static const ModelHierarchyLink turret_default_hierarchy_links[] =
{
    { 0, 65535 },
    { 1, 0 },
    { 4, 0 },
    { 3, 0 },
    { 2, 0 },
};

const ModelHierarchyTable TurretDefaultHierarchy = { 5, turret_default_hierarchy_links };

// Original hierarchy table 800A6E00
static const ModelHierarchyLink turret_raised_hierarchy_links[] =
{
    { 0, 65535 },
    { 1, 0 },
    { 3, 0 },
    { 2, 0 },
};

const ModelHierarchyTable TurretRaisedHierarchy = { 4, turret_raised_hierarchy_links };

// Original hierarchy table 800A6E50
static const ModelHierarchyLink turret_level4_hierarchy_links[] =
{
    { 0, 65535 },
    { 1, 0 },
    { 2, 1 },
};

const ModelHierarchyTable TurretLevel4Hierarchy = { 3, turret_level4_hierarchy_links };

// Original hierarchy table 800A6E94
static const ModelHierarchyLink turret_level10_hierarchy_links[] =
{
    { 0, 65535 },
    { 1, 0 },
    { 2, 1 },
};

const ModelHierarchyTable TurretLevel10Hierarchy = { 3, turret_level10_hierarchy_links };

// Original hierarchy table 800A6F20
static const ModelHierarchyLink player_hierarchy_links[] =
{
    { 0, 65535 },
    { 1, 0 },
    { 2, 1 },
    { 3, 2 },
    { 4, 0 },
    { 5, 4 },
    { 6, 5 },
    { 7, 0 },
    { 9, 7 },
    { 8, 8 },
    { 10, 9 },
    { 14, 7 },
    { 12, 7 },
    { 11, 12 },
    { 13, 13 },
};

const ModelHierarchyTable PlayerHierarchy = { 15, player_hierarchy_links };

// Original hierarchy table 800ED494
static const ModelHierarchyLink mini_tank_hierarchy_links[] =
{
    { 1, 65535 },
    { 0, 0 },
    { 3, 1 },
    { 2, 2 },
};

const ModelHierarchyTable MiniTankHierarchy = { 4, mini_tank_hierarchy_links };

