#ifndef BEAST_FINAL_VISUAL_H
#define BEAST_FINAL_VISUAL_H

#include "vector3.h"
#include "visual_object.h"

// Uses model key 4E5B6ED6 from bst_obj.psx
class BeastFinalVisual : public VisualObject
{
public:
    virtual ~BeastFinalVisual()
    {
    }

public:
    virtual uint32 Update();
    virtual uint32 Destroy(uint32 reason);
    BeastFinalVisual(const Vector3 *pos);
};

#endif
