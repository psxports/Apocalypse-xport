#ifndef VISUAL_REGISTRY_ENTRY_H
#define VISUAL_REGISTRY_ENTRY_H

#include "visual_object.h"

// Original visual registry owner
class VisualRegistryEntry : public VisualObject
{
public:
    virtual ~VisualRegistryEntry()
    {
    }

private:
    uint16 identifier;

public:
    static void Enable(uint32 identifier);
    static void Disable(uint32 identifier);
    VisualRegistryEntry(uint32 resource);
    virtual uint32 Destroy(uint32 reason);
    virtual uint32 Update();
};

#endif
