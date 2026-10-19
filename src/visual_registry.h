#ifndef VISUAL_REGISTRY_H
#define VISUAL_REGISTRY_H

#include "visual_object.h"

void EnableVisualRegistryEntry(uint32 identifier);
void DisableVisualRegistryEntry(uint32 identifier);

// Original visual registry owner
class VisualRegistryEntry : public VisualObject
{
public:
    virtual ~VisualRegistryEntry()
    {
    }

    VisualRegistryEntry(uint32 resource);
    virtual uint32 Destroy(uint32 reason);
    virtual uint32 Update();

private:
    friend void EnableVisualRegistryEntry(uint32 identifier);
    friend void DisableVisualRegistryEntry(uint32 identifier);
    uint16 identifier;
};

#endif
