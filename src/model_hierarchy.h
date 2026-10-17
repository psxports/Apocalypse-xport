#ifndef MODEL_HIERARCHY_H
#define MODEL_HIERARCHY_H

extern "C"
{
#include "psx.h"
}

struct ModelHierarchyLink
{
    uint32 bone;
    uint32 parent;
};

struct ModelHierarchyTable
{
    uint32 count;
    const ModelHierarchyLink *links;
};

extern const ModelHierarchyTable WarHierarchy;
extern const ModelHierarchyTable TheBeastHierarchy;
extern const ModelHierarchyTable ChopperHierarchy;
extern const ModelHierarchyTable TankHierarchy;
extern const ModelHierarchyTable TurretDefaultHierarchy;
extern const ModelHierarchyTable TurretRaisedHierarchy;
extern const ModelHierarchyTable TurretLevel4Hierarchy;
extern const ModelHierarchyTable TurretLevel10Hierarchy;
extern const ModelHierarchyTable PlayerHierarchy;
extern const ModelHierarchyTable MiniTankHierarchy;

#endif
