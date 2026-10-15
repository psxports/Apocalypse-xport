#ifndef ITEMS_H
#define ITEMS_H
#include "visual_object.h"

// Resource owner for the original items model
class Items : public VisualObject
{
public:
    virtual ~Items() {}
    static Items &Attach(uint32 object);
    Items(const uint8 *data, uint32 index);
    Items(const uint32 *position, uint32 first, uint32 second, uint32 value312, uint32 value314, uint32 value316, uint32 value318);
    virtual uint32 Destroy(uint32 reason);
    virtual uint32 Damage(uint32 damage);
    virtual uint32 Update();

};

#endif
