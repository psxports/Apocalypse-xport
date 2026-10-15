#ifndef VISUAL_REGISTRY_H
#define VISUAL_REGISTRY_H

#include "visual_object.h"

// Original visual registry owner
class VisualRegistryEntry : public VisualObject
{
public:
    virtual ~VisualRegistryEntry() {}
    VisualRegistryEntry(uint32 resource);
    static VisualRegistryEntry &Attach(uint32 object);
    virtual uint32 Destroy(uint32 reason);
    virtual uint32 Update();

};

#endif
